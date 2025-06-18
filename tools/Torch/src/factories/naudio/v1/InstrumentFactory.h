#pragma once

#include <factories/BaseFactory.h>
#include "AudioContext.h"

class InstrumentData : public IParsedData {
public:
    uint8_t isRelocated;
    uint8_t normalRangeLo;
    uint8_t normalRangeHi;
    uint8_t adsrDecayIndex;
    uint32_t envelope;
    TunedSample lowPitchTunedSample;
    TunedSample normalPitchTunedSample;
    TunedSample highPitchTunedSample;
};

class InstrumentHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class InstrumentBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class InstrumentCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class InstrumentFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Header, InstrumentHeaderExporter)
            REGISTER(Binary, InstrumentBinaryExporter)
            REGISTER(Code, InstrumentCodeExporter)
        };
    }

    bool HasModdedDependencies() override { return true; }
};
