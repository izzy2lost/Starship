#pragma once

#include "factories/BaseFactory.h"

class AssetArrayData : public IParsedData {
public:
    std::vector<uint32_t> mPtrs;
    std::string mType;

    AssetArrayData(std::vector<uint32_t> ptrs, std::string type) : mPtrs(std::move(ptrs)), mType(type) {}
};

class AssetArrayHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class AssetArrayBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class AssetArrayCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class AssetArrayFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Code, AssetArrayCodeExporter)
            REGISTER(Header, AssetArrayHeaderExporter)
            REGISTER(Binary, AssetArrayBinaryExporter)
        };
    }
};
