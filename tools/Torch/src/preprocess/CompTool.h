#pragma once

#include "lib/binarytools/BinaryWriter.h"
#include <cstdint>
#include <string>
#include <vector>

enum class CompType {
    UNCOMPRESSED,
    COMPRESSED,
    UNKNOWN
};

class CompTool {
public:
    static std::vector<uint8_t> Decompress(std::vector<uint8_t> rom);
private:
    static uint32_t FindFileTable(std::vector<uint8_t>& rom);
    static std::pair<uint32_t, uint32_t> CalculateCRCs(LUS::BinaryWriter& decompFile);
    static inline const uint32_t sCrcSeed = 0xF8CA4DDC;
};
