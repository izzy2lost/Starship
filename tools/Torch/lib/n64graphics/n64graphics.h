#ifndef N64GRAPHICS_H_
#define N64GRAPHICS_H_

#include <stdint.h>

// intermediate formats
typedef struct _rgba
{
   uint8_t red;
   uint8_t green;
   uint8_t blue;
   uint8_t alpha;
} rgba;

typedef struct _rgb
{
   uint8_t red;
   uint8_t green;
   uint8_t blue;
} rgb;

typedef struct _ia
{
   uint8_t intensity;
   uint8_t alpha;
} ia;

typedef struct _ci {
    uint8_t index;
} ci;

// CI palette
typedef struct
{
   uint16_t data[256];
   int max; // max number of entries
   int used; // number of entries used
} palette_t;

//---------------------------------------------------------
// N64 RGBA/IA/I/CI -> intermediate RGBA/IA
//---------------------------------------------------------

// N64 raw RGBA16/RGBA32 -> intermediate RGBA
rgba *raw2rgba(const uint8_t *raw, int width, int height, int depth);

// N64 raw IA1/IA4/IA8/IA16 -> intermediate IA
ia *raw2ia(const uint8_t *raw, int width, int height, int depth);

// N64 raw I4/I8 -> intermediate IA
ia *raw2i(const uint8_t *raw, int width, int height, int depth);

// N64 raw CI4/CI8 -> intermediate CI
ci *raw2ci_torch(const uint8_t* raw, int width, int height, int depth);

int convert_raw_to_ci8(unsigned char **png_output, int *size_output, uint8_t *texture, uint8_t *palette, int format, int width, int height, int depth, int pal_depth);

int imgpal2rawci(uint8_t *rawci, const rgba *img, const rgba *pal, const uint8_t *wheel_mask, int raw_size, int ci_depth, int img_size, int pal_size);

//---------------------------------------------------------
// intermediate RGBA/IA -> N64 RGBA/IA/I/CI
// returns length written to 'raw' used or -1 on error
//---------------------------------------------------------

// intermediate RGBA -> N64 raw RGBA16/RGBA32
int rgba2raw(uint8_t *raw, const rgba *img, int width, int height, int depth);

// intermediate IA -> N64 raw IA1/IA4/IA8/IA16
int ia2raw(uint8_t *raw, const ia *img, int width, int height, int depth);

// intermediate IA -> N64 raw I4/I8
int i2raw(uint8_t *raw, const ia *img, int width, int height, int depth);

// intermediate CI -> N64 raw CI4/CI8
int ci2raw_torch(uint8_t* raw, const ci* img, int width, int height, int depth);

//---------------------------------------------------------
// N64 CI <-> N64 RGBA16/IA16
//---------------------------------------------------------

// N64 CI raw data and palette to raw data (either RGBA16 or IA16)
uint8_t *ci2raw(const uint8_t *rawci, const uint8_t *palette, int width, int height, int ci_depth);

// convert from raw (RGBA16 or IA16) format to CI + palette
int raw2ci(uint8_t *rawci, palette_t *pal, const uint8_t *raw, int raw_len, int ci_depth);

//---------------------------------------------------------
// intermediate RGBA/IA -> PNG
//---------------------------------------------------------

// intermediate RGBA write to PNG file
int rgba2png(unsigned char** png_output, int* size_output, const rgba* img, int width, int height);

// intermediate IA write to grayscale PNG file
int ia2png(unsigned char** png_output, int* size_output, const ia* img, int width, int height);

int ci2png(unsigned char** png_output, int* size_output, const ci* img, int width, int height);

//---------------------------------------------------------
// PNG -> intermediate RGBA/IA
//---------------------------------------------------------

// PNG file -> intermediate RGBA
rgba *png2rgba(unsigned char* png_input, int size_input, int *width, int *height);

// PNG file -> intermediate RGB
rgb* png2rgb(unsigned char* png_input, int size_input, int* width, int* height);

// PNG file -> intermediate IA
ia *png2ia(unsigned char* png_input, int size_input, int *width, int *height);

// PNG file -> intermediate CI
ci* png2ci(unsigned char* png_input, int size_input, int* width, int* height);

// Adds colours to palette data
static int pal_add_color(palette_t* pal, uint16_t val);

//---------------------------------------------------------
// version
//---------------------------------------------------------

// get version of underlying graphics reading library
const char *n64graphics_get_read_version(void);

// get version of underlying graphics writing library
const char *n64graphics_get_write_version(void);

#endif // N64GRAPHICS_H_