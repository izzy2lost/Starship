#include "TextureUtils.h"
#include <vector>
#include <binarytools/endianness.h>

size_t TextureUtils::CalculateTextureSize(TextureType type, uint32_t width, uint32_t height) {
    switch (type) {
        // 4 bytes per pixel
        case TextureType::RGBA32bpp:
            return width * height * 4;
        // 2 bytes per pixel
        case TextureType::TLUT:
        case TextureType::RGBA16bpp:
        case TextureType::GrayscaleAlpha16bpp:
            return width * height * 2;
        // 1 byte per pixel
        case TextureType::Grayscale8bpp:
        case TextureType::Palette8bpp:
        case TextureType::GrayscaleAlpha8bpp:
        // TODO: We need to validate this MegaMech
        case TextureType::GrayscaleAlpha1bpp:
            return width * height;
        // 1/2 byte per pixel
        case TextureType::Palette4bpp:
        case TextureType::Grayscale4bpp:
        case TextureType::GrayscaleAlpha4bpp:
            return (width * height) / 2;
        default:
            return 0;
    }
}

std::vector<uint8_t> TextureUtils::alloc_ia8_text_from_i1(uint16_t *in, int16_t width, int16_t height) {
    int32_t inPos;
    uint16_t bitMask;
    int16_t outPos = 0;
    const auto out = new uint8_t[width * height];

    for (int32_t inPos = 0; inPos < (width * height) / 16; inPos++) {
        uint16_t bitMask = 0x8000;

        while (bitMask != 0) {
            if (BSWAP16(in[inPos]) & bitMask) {
                out[outPos] = 0xFF;
            } else {
                out[outPos] = 0x00;
            }

            bitMask /= 2;
            outPos++;
        }
    }

    auto result = std::vector(out, out + width * height);
    delete[] out;

    return result;
}