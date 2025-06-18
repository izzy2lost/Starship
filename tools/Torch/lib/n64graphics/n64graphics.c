#include <stdio.h>
#include <stdlib.h>
#if defined(_WIN32) || defined(_WIN64)
#include <string.h>
#else
#include <strings.h>
#endif

#define STBI_NO_LINEAR
#define STBI_NO_HDR
#define STBI_NO_TGA
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "n64graphics.h"
#include "libmio0/utils.h"

// SCALE_M_N: upscale/downscale M-bit integer to N-bit
#define SCALE_5_8(VAL_) (((VAL_) * 0xFF) / 0x1F)
#define SCALE_8_5(VAL_) ((((VAL_) + 4) * 0x1F) / 0xFF)
#define SCALE_4_8(VAL_) ((VAL_) * 0x11)
#define SCALE_8_4(VAL_) ((VAL_) / 0x11)
#define SCALE_3_8(VAL_) ((VAL_) * 0x24)
#define SCALE_8_3(VAL_) ((VAL_) / 0x24)

unsigned short magicFiller = 0x07FE;

typedef struct {
    enum {
        IMG_FORMAT_RGBA,
        IMG_FORMAT_IA,
        IMG_FORMAT_I,
        IMG_FORMAT_CI,
    } format;
    int depth;
} img_format;

//---------------------------------------------------------
// N64 RGBA/IA/I/CI -> internal RGBA/IA
//---------------------------------------------------------

rgba* raw2rgba(const uint8_t* raw, int width, int height, int depth) {
    rgba* img;
    int img_size;

    img_size = width * height * sizeof(*img);
    img = malloc(img_size);
    if (!img) {
        ERROR("Error allocating %d bytes\n", img_size);
        return NULL;
    }

    if (depth == 16) {
        for (int i = 0; i < width * height; i++) {
            img[i].red = SCALE_5_8((raw[i * 2] & 0xF8) >> 3);
            img[i].green = SCALE_5_8(((raw[i * 2] & 0x07) << 2) | ((raw[i * 2 + 1] & 0xC0) >> 6));
            img[i].blue = SCALE_5_8((raw[i * 2 + 1] & 0x3E) >> 1);
            img[i].alpha = (raw[i * 2 + 1] & 0x01) ? 0xFF : 0x00;
        }
    } else if (depth == 32) {
        for (int i = 0; i < width * height; i++) {
            img[i].red = raw[i * 4];
            img[i].green = raw[i * 4 + 1];
            img[i].blue = raw[i * 4 + 2];
            img[i].alpha = raw[i * 4 + 3];
        }
    }

    return img;
}

ia* raw2ia(const uint8_t* raw, int width, int height, int depth) {
    ia* img;
    int img_size;

    img_size = width * height * sizeof(*img);
    img = malloc(img_size);
    if (!img) {
        ERROR("Error allocating %u bytes\n", img_size);
        return NULL;
    }

    switch (depth) {
        case 16:
            for (int i = 0; i < width * height; i++) {
                img[i].intensity = raw[i * 2];
                img[i].alpha = raw[i * 2 + 1];
            }
            break;
        case 8:
            for (int i = 0; i < width * height; i++) {
                img[i].intensity = SCALE_4_8((raw[i] & 0xF0) >> 4);
                img[i].alpha = SCALE_4_8(raw[i] & 0x0F);
            }
            break;
        case 4:
            for (int i = 0; i < width * height; i++) {
                uint8_t bits;
                bits = raw[i / 2];
                if (i % 2) {
                    bits &= 0xF;
                } else {
                    bits >>= 4;
                }
                img[i].intensity = SCALE_3_8((bits >> 1) & 0x07);
                img[i].alpha = (bits & 0x01) ? 0xFF : 0x00;
            }
            break;
        case 1:
            for (int i = 0; i < width * height; i++) {
                uint8_t bits;
                uint8_t mask;
                bits = raw[i / 8];
                mask = 1 << (7 - (i % 8)); // MSb->LSb
                bits = (bits & mask) ? 0xFF : 0x00;
                img[i].intensity = bits;
                img[i].alpha = bits;
            }
            break;
        default:
            ERROR("Error invalid depth %d\n", depth);
            break;
    }

    return img;
}

ci *raw2ci_torch(const uint8_t* raw, int width, int height, int depth) {
    ci *img = NULL;
    int img_size;

    img_size = width * height * sizeof(*img);
    img = malloc(img_size);
    if (!img) {
        ERROR("Error allocating %u bytes\n", img_size);
        return NULL;
    }

    switch (depth) {
        case 8:
            for (int i = 0; i < width * height; i++) {
                img[i].index = raw[i];
            }
        break;
        case 4:
            for (int i = 0; i < width * height; i++) {
                int pos = i / 2;
                img[i].index = i % 2 ? raw[pos] & 0xF : raw[pos] >> 4;
            }
        break;
        default:
            ERROR("Error invalid depth %d\n", depth);
        break;
    }

    return img;
}

ia* raw2i(const uint8_t* raw, int width, int height, int depth) {
    ia* img = NULL;
    int img_size;

    img_size = width * height * sizeof(*img);
    img = malloc(img_size);
    if (!img) {
        ERROR("Error allocating %u bytes\n", img_size);
        return NULL;
    }

    switch (depth) {
        case 8:
            for (int i = 0; i < width * height; i++) {
                img[i].intensity = raw[i];
                img[i].alpha = 0xFF;
            }
            break;
        case 4:
            for (int i = 0; i < width * height; i++) {
                uint8_t bits;
                bits = raw[i / 2];
                if (i % 2) {
                    bits &= 0xF;
                } else {
                    bits >>= 4;
                }
                img[i].intensity = SCALE_4_8(bits);
                img[i].alpha = 0xFF;
            }
            break;
        default:
            ERROR("Error invalid depth %d\n", depth);
            break;
    }

    return img;
}

// convert CI raw data and palette to raw data (either RGBA16 or IA16)
uint8_t* ci2raw(const uint8_t* rawci, const uint8_t* palette, int width, int height, int ci_depth) {
    uint8_t* raw;
    int raw_size;

    // first convert to raw RGBA
    raw_size = sizeof(uint16_t) * width * height;
    raw = malloc(raw_size);
    if (!raw) {
        ERROR("Error allocating %u bytes\n", raw_size);
        return NULL;
    }

    for (int i = 0; i < width * height; i++) {
        int pal_idx = rawci[i];
        if (ci_depth == 4) {
            int byte_idx = i / 2;
            int nibble = 1 - (i % 2);
            int shift = 4 * nibble;
            pal_idx = (rawci[byte_idx] >> shift) & 0xF;
        }
        raw[2 * i] = palette[2 * pal_idx];
        raw[2 * i + 1] = palette[2 * pal_idx + 1];
    }

    return raw;
}

//---------------------------------------------------------
// internal RGBA/IA -> N64 RGBA/IA/I/CI
// returns length written to 'raw' used or -1 on error
//---------------------------------------------------------

int rgba2raw(uint8_t* raw, const rgba* img, int width, int height, int depth) {
    int size = width * height * depth / 8;
    INFO("Converting RGBA%d %dx%d to raw\n", depth, width, height);

    if (depth == 16) {
        for (int i = 0; i < width * height; i++) {
            uint8_t r, g, b, a;
            r = SCALE_8_5(img[i].red);
            g = SCALE_8_5(img[i].green);
            b = SCALE_8_5(img[i].blue);
            a = img[i].alpha ? 0x1 : 0x0;
            raw[i * 2] = (r << 3) | (g >> 2);
            raw[i * 2 + 1] = ((g & 0x3) << 6) | (b << 1) | a;
        }
    } else if (depth == 32) {
        for (int i = 0; i < width * height; i++) {
            raw[i * 4] = img[i].red;
            raw[i * 4 + 1] = img[i].green;
            raw[i * 4 + 2] = img[i].blue;
            raw[i * 4 + 3] = img[i].alpha;
        }
    } else {
        ERROR("Error invalid depth %d\n", depth);
        size = -1;
    }

    return size;
}

int ia2raw(uint8_t* raw, const ia* img, int width, int height, int depth) {
    int size = width * height * depth / 8;
    INFO("Converting IA%d %dx%d to raw\n", depth, width, height);

    switch (depth) {
        case 16:
            for (int i = 0; i < width * height; i++) {
                raw[i * 2] = img[i].intensity;
                raw[i * 2 + 1] = img[i].alpha;
            }
            break;
        case 8:
            for (int i = 0; i < width * height; i++) {
                uint8_t val = SCALE_8_4(img[i].intensity);
                uint8_t alpha = SCALE_8_4(img[i].alpha);
                raw[i] = (val << 4) | alpha;
            }
            break;
        case 4:
            for (int i = 0; i < width * height; i++) {
                uint8_t val = SCALE_8_3(img[i].intensity);
                uint8_t alpha = img[i].alpha ? 0x01 : 0x00;
                uint8_t old = raw[i / 2];
                if (i % 2) {
                    raw[i / 2] = (old & 0xF0) | (val << 1) | alpha;
                } else {
                    raw[i / 2] = (old & 0x0F) | (((val << 1) | alpha) << 4);
                }
            }
            break;
        case 1:
            for (int i = 0; i < width * height; i++) {
                uint8_t val = img[i].intensity;
                uint8_t old = raw[i / 8];
                uint8_t bit = 1 << (7 - (i % 8));
                if (val) {
                    raw[i / 8] = old | bit;
                } else {
                    raw[i / 8] = old & (~bit);
                }
            }
            break;
        default:
            ERROR("Error invalid depth %d\n", depth);
            size = -1;
            break;
    }

    return size;
}


/**
 * Check 2 rgba structs for equality. 0 if unequal, 1 if equal
**/
int comp_rgba(const rgba left, const rgba right) {
   if ((left.red != right.red) || (left.green != right.green) || (left.blue != right.blue) || (left.alpha != right.alpha)) {
      return 0;
   } else {
      return 1;
   }
}

/**
 * Check if a given rgba (comp) is in a given palette (pal, represented as an array of rgba structs)
 * If found, return the index in pal it was found out
 * Otherwise, return -1
**/
int get_color_index(const rgba comp, const rgba *pal, int mask_value, int pal_size) {
   int pal_idx;
   // The starting values used here are super specific to MK64, they're not really portable to anything else
   if (mask_value == 0) {
      pal_idx = 0;
   } else {
      pal_idx = 0xC0;
   }
   for (; pal_idx < pal_size; pal_idx++) {
      if (comp_rgba(comp, pal[pal_idx]) == 1) return pal_idx;
   }
   ERROR("Could not find a color in the palette\n");
   ERROR("comp: %x%x%x%x\n", comp.red, comp.green, comp.blue, comp.alpha);
   return -1;
}

/**
 * Takes an image (img, an array of rgba structs) and a palette (pal, also an array of rgba structs)
 * Sets the values of rawci (8 bit color index array) to the appropriate index in pal that each entry in img can be found at
 * If a value in img is not found in pal, return 0, indicating an error
 * Returns 1 if all values in img are found somewhere in pal
**/
int imgpal2rawci(uint8_t *rawci, const rgba *img, const rgba *pal, const uint8_t *wheel_mask, int raw_size, int ci_depth, int img_size, int pal_size) {
   int img_idx;
   int pal_idx;
   int mask_value;
   memset(rawci, 0, raw_size);

   for (img_idx = 0; img_idx < img_size; img_idx++) {
      if (wheel_mask != NULL) {
         mask_value = wheel_mask[img_idx];
      } else {
         mask_value = 0;
      }
      pal_idx = get_color_index(img[img_idx], pal, mask_value, pal_size);
      if (pal_idx != -1) {
         switch (ci_depth) {
            case 8:
                  rawci[img_idx] = pal_idx;
               break;
            case 4:
            {
               int byte_idx = img_idx / 2;
               int nibble = 1 - (img_idx % 2);
               uint8_t mask = 0xF << (4 * (1 - nibble));
               rawci[byte_idx] = (rawci[byte_idx] & mask) | (pal_idx << (4 * nibble));
               break;
            }
         }
      } else {
         return 0;
      }
   }
   return 1;
}

int i2raw(uint8_t* raw, const ia* img, int width, int height, int depth) {
    int size = width * height * depth / 8;
    INFO("Converting I%d %dx%d to raw\n", depth, width, height);

    switch (depth) {
        case 8:
            for (int i = 0; i < width * height; i++) {
                raw[i] = img[i].intensity;
            }
            break;
        case 4:
            for (int i = 0; i < width * height; i++) {
                uint8_t val = SCALE_8_4(img[i].intensity);
                uint8_t old = raw[i / 2];
                if (i % 2) {
                    raw[i / 2] = (old & 0xF0) | val;
                } else {
                    raw[i / 2] = (old & 0x0F) | (val << 4);
                }
            }
            break;
        default:
            ERROR("Error invalid depth %d\n", depth);
            size = -1;
            break;
    }

    return size;
}

int ci2raw_torch(uint8_t *raw, const ci *img, int width, int height, int depth) {
    int size = width * height * depth / 8;
    INFO("Converting I%d %dx%d to raw\n", depth, width, height);

    switch (depth) {
        case 8:
            for (int i = 0; i < width * height; i++) {
                raw[i] = img[i].index;
            }
        break;
        case 4:
            for(int y = 0; y < height; y++) {
                for(int x = 0; x < width; x += 2) {
                    const size_t pos = (y * width + x) / 2;

                    const uint8_t cR1 = img[y * width + x].index;
                    const uint8_t cR2 = img[y * width + x + 1].index;

                    raw[pos] = cR1 << 4 | cR2;
                }
            }
        break;
        default:
            ERROR("Error invalid depth %d\n", depth);
        size = -1;
        break;
    }

    return size;
}

//---------------------------------------------------------
// internal RGBA/IA -> PNG
//---------------------------------------------------------

int rgba2png(unsigned char** png_output, int* size_output, const rgba* img, int width, int height) {
    int ret = 0;

    // convert to format stb_image_write expects
    uint8_t* data = malloc(4 * width * height);
    if (data) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                int idx = j * width + i;
                data[4 * idx] = img[idx].red;
                data[4 * idx + 1] = img[idx].green;
                data[4 * idx + 2] = img[idx].blue;
                data[4 * idx + 3] = img[idx].alpha;
            }
        }


        *png_output = stbi_write_png_to_mem(data, 0, width, height, 4, size_output);

        free(data);
    }

    return ret;
}

int ia2png(unsigned char** png_output, int* size_output, const ia* img, int width, int height) {
    int ret = 0;

    // convert to format stb_image_write expects
    uint8_t* data = malloc(2 * width * height);
    if (data) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                int idx = j * width + i;
                data[2 * idx] = img[idx].intensity;
                data[2 * idx + 1] = img[idx].alpha;
            }
        }

        (*png_output) = stbi_write_png_to_mem(data, 0, width, height, 2, size_output);

        free(data);
    }

    return ret;
}

int ci2png(unsigned char **png_output, int *size_output, const ci *img, int width, int height) {
    int ret = 0;

    // convert to format stb_image_write expects
    uint8_t* data = malloc(width * height);
    if (data) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                int idx = j * width + i;
                data[idx] = img[idx].index;
            }
        }

        (*png_output) = stbi_write_plte_png_to_mem(data, 0, width, height, 1, NULL, 0, size_output);

        free(data);
    }

    return ret;
}

//---------------------------------------------------------
// PNG -> internal RGBA/IA
//---------------------------------------------------------

rgba* png2rgba(unsigned char* png_input, int size_input, int* width, int* height) {
    rgba* img = NULL;
    int w = 0;
    int h = 0;
    int channels = 0;
    int img_size;

    stbi_uc* data = stbi_load_from_memory(png_input, size_input, &w, &h, &channels, STBI_default);
    if (!data || w <= 0 || h <= 0) {
        ERROR("Error loading file\n");
        return NULL;
    }
    INFO("Read %dx%d channels: %d\n", w, h, channels);

    img_size = w * h * sizeof(rgba);
    img = malloc(img_size);
    if (!img) {
        ERROR("Error allocating %u bytes\n", img_size);
        return NULL;
    }

    switch (channels) {
        case 3: // red, green, blue
        case 4: // red, green, blue, alpha
            for (int j = 0; j < h; j++) {
                for (int i = 0; i < w; i++) {
                    int idx = j * w + i;
                    img[idx].red = data[channels * idx];
                    img[idx].green = data[channels * idx + 1];
                    img[idx].blue = data[channels * idx + 2];
                    if (channels == 4) {
                        img[idx].alpha = data[channels * idx + 3];
                    } else {
                        img[idx].alpha = 0xFF;
                    }
                }
            }
            break;
        case 2: // grey, alpha
            for (int j = 0; j < h; j++) {
                for (int i = 0; i < w; i++) {
                    int idx = j * w + i;
                    img[idx].red = data[2 * idx];
                    img[idx].green = data[2 * idx];
                    img[idx].blue = data[2 * idx];
                    img[idx].alpha = data[2 * idx + 1];
                }
            }
            break;
        default:
            ERROR("Don't know how to read channels: %d\n", channels);
            free(img);
            img = NULL;
    }

    // cleanup
    stbi_image_free(data);

    *width = w;
    *height = h;
    return img;
}

rgb* png2rgb(unsigned char* png_input, int size_input, int* width, int* height) {
    rgb* img = NULL;
    int w = 0;
    int h = 0;
    int channels = 0;
    int img_size;

    stbi_uc* data = stbi_load_from_memory(png_input, size_input, &w, &h, &channels, STBI_rgb);
    if (!data || w <= 0 || h <= 0) {
        ERROR("Error loading file\n");
        return NULL;
    }
    INFO("Read %dx%d channels: %d\n", w, h, channels);

    img_size = w * h * sizeof(*img);
    img = malloc(img_size);
    if (!img) {
        ERROR("Error allocating %u bytes\n", img_size);
        return NULL;
    }

    switch (channels) {
        case 3: // red, green, blue
        case 4: // red, green, blue
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                int idx = j * w + i;
                img[idx].red = data[channels * idx];
                img[idx].green = data[channels * idx + 1];
                img[idx].blue = data[channels * idx + 2];
            }
        }
        break;
        case 2: // grey, alpha
            for (int j = 0; j < h; j++) {
                for (int i = 0; i < w; i++) {
                    int idx = j * w + i;
                    img[idx].red = data[2 * idx];
                    img[idx].green = data[2 * idx];
                    img[idx].blue = data[2 * idx];
                }
            }
        break;
        default:
            ERROR("Don't know how to read channels: %d\n", channels);
        free(img);
        img = NULL;
    }

    // cleanup
    stbi_image_free(data);

    *width = w;
    *height = h;
    return img;
}

ia* png2ia(unsigned char* png_input, int size_input, int* width, int* height) {
    ia* img = NULL;
    int w = 0, h = 0;
    int channels = 0;
    int img_size;

    stbi_uc* data = stbi_load_from_memory(png_input, size_input, &w, &h, &channels, STBI_default);
    if (!data || w <= 0 || h <= 0) {
        ERROR("Error loading file\n");
        return NULL;
    }
    INFO("Read %dx%d channels: %d\n", w, h, channels);

    img_size = w * h * sizeof(*img);
    img = malloc(img_size);
    if (!img) {
        ERROR("Error allocating %d bytes\n", img_size);
        return NULL;
    }

    switch (channels) {
        case 3: // red, green, blue
        case 4: // red, green, blue, alpha
            ERROR("Warning: averaging RGB PNG to create IA\n");
            for (int j = 0; j < h; j++) {
                for (int i = 0; i < w; i++) {
                    int idx = j * w + i;
                    int sum = data[channels * idx] + data[channels * idx + 1] + data[channels * idx + 2];
                    img[idx].intensity = (sum + 1) / 3; // add 1 to round up where appropriate
                    if (channels == 4) {
                        img[idx].alpha = data[channels * idx + 3];
                    } else {
                        img[idx].alpha = 0xFF;
                    }
                }
            }
            break;
        case 2: // grey, alpha
            for (int j = 0; j < h; j++) {
                for (int i = 0; i < w; i++) {
                    int idx = j * w + i;
                    img[idx].intensity = data[2 * idx];
                    img[idx].alpha = data[2 * idx + 1];
                }
            }
            break;
        default:
            ERROR("Don't know how to read channels: %d\n", channels);
            free(img);
            img = NULL;
    }

    // cleanup
    stbi_image_free(data);

    *width = w;
    *height = h;
    return img;
}

ci* png2ci(unsigned char* png_input, int size_input, int* width, int* height) {
    ci* img = NULL;
    int w = 0, h = 0;
    int channels = 0;
    int img_size;

    stbi_uc* data = stbi_load_from_memory(png_input, size_input, &w, &h, &channels, STBI_default);
    if (!data || w <= 0 || h <= 0) {
        ERROR("Error loading file\n");
        return NULL;
    }
    INFO("Read %dx%d channels: %d\n", w, h, channels);

    img_size = w * h * sizeof(*img);
    img = malloc(img_size);
    if (!img) {
        ERROR("Error allocating %d bytes\n", img_size);
        return NULL;
    }

    switch (channels) {
        case 3: // red, green, blue
        case 4: // red, green, blue, alpha
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                int idx = j * w + i;
                img[idx].index = data[channels * idx];
            }
        }
        break;
        case 2: // grey, alpha
            for (int j = 0; j < h; j++) {
                for (int i = 0; i < w; i++) {
                    int idx = j * w + i;
                    img[idx].index = data[2 * idx];
                }
            }
        break;
        default:
            ERROR("Don't know how to read channels: %d\n", channels);
        free(img);
        img = NULL;
    }

    // cleanup
    stbi_image_free(data);

    *width = w;
    *height = h;
    return img;
}

// find index of palette color
// return -1 if not found
static int pal_find_color(const palette_t* pal, uint16_t val) {
    for (int i = 0; i < pal->used; i++) {
        if (pal->data[i] == val) {
            return i;
        }
    }
    return -1;
}

// find value in palette, or add if not there
// returns palette index entered or -1 if palette full
static int pal_add_color(palette_t* pal, uint16_t val) {
    int idx;
    idx = pal_find_color(pal, val);
    if (idx < 0) {
        if (pal->used == pal->max) {
            ERROR("Error: trying to use more than %d\n", pal->max);
        } else {
            idx = pal->used;
            pal->data[pal->used] = val;
            pal->used++;
        }
    }
    return idx;
}

// convert from raw (RGBA16 or IA16) format to CI + palette
// returns 1 on success
int raw2ci(uint8_t* rawci, palette_t* pal, const uint8_t* raw, int raw_len, int ci_depth) {
    // assign colors to palette
    pal->used = 0;
    memset(pal->data, 0, sizeof(pal->data));
    int ci_idx = 0;
    for (int i = 0; i < raw_len; i += sizeof(uint16_t)) {
        uint16_t val = read_u16_be(&raw[i]);
        int pal_idx = pal_add_color(pal, val);
        if (pal_idx < 0) {
            ERROR("Error adding color @ (%d): %d (used: %d/%d)\n", i, pal_idx, pal->used, pal->max);
            return 0;
        } else {
            switch (ci_depth) {
                case 8:
                    rawci[ci_idx] = (uint8_t) pal_idx;
                    break;
                case 4: {
                    int byte_idx = ci_idx / 2;
                    int nibble = 1 - (ci_idx % 2);
                    uint8_t mask = 0xF << (4 * (1 - nibble));
                    rawci[byte_idx] = (rawci[byte_idx] & mask) | (pal_idx << (4 * nibble));
                    break;
                }
            }
            ci_idx++;
        }
    }
    return 1;
}

const char* n64graphics_get_read_version(void) {
    return "stb_image 2.19";
}

const char* n64graphics_get_write_version(void) {
    return "stb_image_write 1.09";
}

/**
 * Converts binary ci8 + palette to a single .png
 */
int convert_raw_to_ci8(unsigned char **png_output, int *size_output, uint8_t *texture, uint8_t *palette, int format, int width, int height, int depth, int pal_depth) {
    uint8_t *raw_fmt;
    rgba *imgr;
    ia   *imgi;
    int res;

    raw_fmt = ci2raw(texture, palette, width, height, depth);
    switch (format) {
        case IMG_FORMAT_RGBA:
            INFO("Converting raw to RGBA%d\n", pal_depth);
            imgr = raw2rgba(raw_fmt, width, height, pal_depth);
            res = rgba2png(png_output, size_output, imgr, width, height);
            free(imgr);
            break;
        case IMG_FORMAT_IA:
            INFO("Converting raw to IA%d\n", pal_depth);
            imgi = raw2ia(raw_fmt, width, height, pal_depth);
            //res = ia2png(name, imgi, width, height);
            free(imgi);
            break;
        default:
            //ERROR("Unsupported palette format: %s\n", format2str(&config.pal_format));
            return EXIT_FAILURE;
    }
    free(raw_fmt);
}