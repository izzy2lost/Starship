#pragma once

#include <factories/BaseFactory.h>
#include "types/RawBuffer.h"

class SequenceData : public IParsedData {
public:
    uint32_t mId;
    uint32_t mSize;
    std::vector<uint8_t> mBuffer;
    std::vector<std::string> mBanks;

    SequenceData(uint32_t id, uint32_t size, uint8_t* data, std::vector<std::string>& banks) : mId(id), mSize(size), mBanks(banks) {
        mBuffer = std::vector<uint8_t>(data, data + size);
    }
};

class SequenceBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class SequenceModdingExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class SequenceFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    std::optional<std::shared_ptr<IParsedData>> parse_modding(std::vector<uint8_t>& buffer, YAML::Node& data) override;

    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Modding, SequenceModdingExporter)
            REGISTER(Binary, SequenceBinaryExporter)
        };
    }

    bool SupportModdedAssets() override { return true; }
};