#pragma once

#include <factories/BaseFactory.h>
#include "AudioContext.h"

class ADPCMLoopData : public IParsedData {
public:
    uint32_t start;
    uint32_t end;
    uint32_t count;
    int16_t predictorState[16];
};

class ADPCMLoopHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class ADPCMLoopBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class ADPCMLoopCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class ADPCMLoopFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Header, ADPCMLoopHeaderExporter)
            REGISTER(Binary, ADPCMLoopBinaryExporter)
            REGISTER(Code, ADPCMLoopCodeExporter)
        };
    }

    bool HasModdedDependencies() override { return true; }
};
