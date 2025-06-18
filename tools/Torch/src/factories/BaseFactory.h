#pragma once

#include "ResourceType.h"
#include "n64/Cartridge.h"
#include <cstdint>
#include <iostream>
#include <any>
#include <memory>
#include <vector>
#include <string>
#include <variant>
#include <optional>
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <strhash64/StrHash64.h>
#include "lib/binarytools/BinaryWriter.h"
#include "lib/binarytools/BinaryReader.h"

namespace fs = std::filesystem;

#define REGISTER(type, c) { ExportType::type, std::make_shared<c>() },

#define SEGMENT_OFFSET(a) ((uint32_t)(a) & 0x00FFFFFF)
#define SEGMENT_NUMBER(x) (((uint32_t)(x) >> 24) & 0xFF)
// I would love to use 0x01000000, but the stupid compiler takes it as 0x01
#define IS_SEGMENTED(x) ((SEGMENT_NUMBER(x) > 0) && (SEGMENT_NUMBER(x) < 0x20))
#define ASSET_PTR(x) (IS_SEGMENTED(x) ? SEGMENT_OFFSET(x) : (x))

#define tab_t "\t"
#define fourSpaceTab "    "

struct OffsetEntry {
    uint32_t start;
    uint32_t end;
};

typedef std::optional<std::variant<size_t, OffsetEntry>> ExportResult;

enum class ExportType {
    Header,
    Code,
    Binary,
    Modding,
    XML
};

template<typename T>
std::optional<T> GetNode(YAML::Node& node, const std::string& key) {
    if(!node[key]) {
        return std::nullopt;
    }

    return std::optional<T>(node[key].as<T>());
}

template<typename T>
T GetSafeNode(YAML::Node& node, const std::string& key) {
    if(!node[key]) {
        auto dump = YAML::Dump(node);

        if (node["symbol"]) {
            throw std::runtime_error("Yaml asset missing the '" + key + "' node for '" + node["symbol"].as<std::string>() + "'\nProblematic YAML:\n" + dump);
        } else {
            throw std::runtime_error("Yaml asset missing the '" + key + "' node\nProblematic YAML:\n" + dump);
        }
    }

    return node[key].as<T>();
}

template<typename T>
T GetSafeNode(YAML::Node& node, const std::string& key, const T& def) {
    if(!node[key]) {
        return def;
    }

    return node[key].as<T>();
}

class IParsedData{};

template<typename T>
class GenericData : public IParsedData {
public:
    T mData;
};

class BaseExporter {
public:
    virtual ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) = 0;
    static void WriteHeader(LUS::BinaryWriter& write, Torch::ResourceType resType, int32_t version);
};

class BaseFactory {
public:
    virtual std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) = 0;
    virtual std::optional<std::shared_ptr<IParsedData>> parse_modding(std::vector<uint8_t>& buffer, YAML::Node& data) {
        return std::nullopt;
    }
    std::optional<std::shared_ptr<BaseExporter>> GetExporter(ExportType type) {
        auto exporters = this->GetExporters();
        if (exporters.find(type) != exporters.end()) {
            return exporters[type];
        }
        return std::nullopt;
    }
    virtual bool SupportModdedAssets() {
        return false;
    }
    virtual bool HasModdedDependencies() {
        return false;
    }
    virtual uint32_t GetAlignment() {
        return 4;
    }
    virtual std::optional<std::shared_ptr<IParsedData>> CreateDataPointer() {
        return std::nullopt;
    }
private:
    virtual std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() {
        return {};
    }
};