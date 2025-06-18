#pragma once

#include <cstdint>
#include <vector>
#include <cstddef>

enum class TextureType {
    Error,
    RGBA32bpp,
    RGBA16bpp,
    Palette4bpp,
    Palette8bpp,
    Grayscale4bpp,
    Grayscale8bpp,
    GrayscaleAlpha4bpp,
    GrayscaleAlpha8bpp,
    GrayscaleAlpha16bpp,
    GrayscaleAlpha1bpp,
    TLUT
};

struct TextureFormat {
    TextureType type;
    uint32_t depth;
};

class TextureUtils {
  public:
    static size_t CalculateTextureSize(TextureType type, uint32_t width, uint32_t height);
    static std::vector<uint8_t> alloc_ia8_text_from_i1(uint16_t *in, int16_t width, int16_t height);
}; 
