#pragma once

#include <factories/BaseFactory.h>
#include "AudioContext.h"

class ADPCMBookData : public IParsedData {
public:
    int32_t order;
    int32_t numPredictors;
    std::vector<int16_t> book;
};

class ADPCMBookHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class ADPCMBookBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class ADPCMBookCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class ADPCMBookFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Header, ADPCMBookHeaderExporter)
            REGISTER(Binary, ADPCMBookBinaryExporter)
            REGISTER(Code, ADPCMBookCodeExporter)
        };
    }

    bool HasModdedDependencies() override { return true; }
};
