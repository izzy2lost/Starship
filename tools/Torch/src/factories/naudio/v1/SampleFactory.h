#pragma once

#include <factories/BaseFactory.h>
#include "AudioContext.h"

class NSampleData : public IParsedData {
public:
    uint32_t codec : 4;
    uint32_t medium : 2;
    uint32_t unk : 1;
    uint32_t isRelocated : 1;
    uint32_t size : 24;
    uint32_t sampleAddr;
    uint32_t loop;
    uint32_t book;

    // Custom fields
    uint32_t sampleBankId;
    uint32_t sampleRate;
    float tuning;
};

class NSampleHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class NSampleBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class NSampleCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class NSampleModdingExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class NSampleXMLExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class NSampleFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Header, NSampleHeaderExporter)
            REGISTER(Binary, NSampleBinaryExporter)
            REGISTER(Code, NSampleCodeExporter)
            REGISTER(Modding, NSampleModdingExporter)
            REGISTER(XML, NSampleXMLExporter)
        };
    }
    bool SupportModdedAssets() override { return true; }
};
