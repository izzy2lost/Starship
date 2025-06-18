#pragma once

#include <vector>
#include <cstdint>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include <optional>
#include <cstdint>
#include "lib/binarytools/BinaryReader.h"

enum class CompressionType {
    None,
    MIO0,
    YAY0,
    YAZ0,
};


struct DataChunk {
    uint8_t* data;
    size_t size;
};

struct DecompressedData {
    DataChunk* root;
    DataChunk segment;

    LUS::BinaryReader GetReader() {
        return LUS::BinaryReader(reinterpret_cast<char*>(segment.data), segment.size);
    }
};

class Decompressor {
public:
    static DataChunk* Decode(const std::vector<uint8_t>& buffer, uint32_t offset, CompressionType type, bool ignoreCache = false);
    static DataChunk* DecodeTKMK00(const std::vector<uint8_t>& buffer, const uint32_t offset, const uint32_t size, const uint32_t alpha);
    static DecompressedData AutoDecode(YAML::Node& node, std::vector<uint8_t>& buffer, std::optional<size_t> size = std::nullopt);
    static DecompressedData AutoDecode(uint32_t offset, std::optional<size_t> size, std::vector<uint8_t>& buffer);
    static CompressionType GetCompressionType(std::vector<uint8_t>& buffer, const uint32_t offset);
    static uint32_t TranslateAddr(uint32_t addr, bool baseAddress = false);
    static bool IsSegmented(uint32_t addr);

    static void ClearCache();
};