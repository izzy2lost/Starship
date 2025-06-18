#pragma once

#include <cstdint>
#include <vector>

namespace SF64 {
    void DecompressAudio(std::vector<uint8_t> data, int16_t* output);
}