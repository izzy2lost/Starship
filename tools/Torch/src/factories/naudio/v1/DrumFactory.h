#pragma once

#include <factories/BaseFactory.h>
#include "AudioContext.h"

class DrumData : public IParsedData {
public:
    uint8_t adsrDecayIndex;
    uint8_t pan;
    uint8_t isRelocated;
    TunedSample tunedSample;
    uint32_t envelope;
};

class DrumHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class DrumBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class DrumCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class DrumFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Header, DrumHeaderExporter)
            REGISTER(Binary, DrumBinaryExporter)
            REGISTER(Code, DrumCodeExporter)
        };
    }

    bool HasModdedDependencies() override { return true; }
};
