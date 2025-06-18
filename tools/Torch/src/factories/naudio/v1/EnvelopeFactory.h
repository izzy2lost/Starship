#pragma once

#include <factories/BaseFactory.h>
#include <vector>
#include "AudioContext.h"

struct EnvelopePoint {
    int16_t delay;
    int16_t arg;
};

class EnvelopeData : public IParsedData {
public:
    std::vector<EnvelopePoint> points;

    EnvelopeData(std::vector<EnvelopePoint> points) : points(points) {};
};

class EnvelopeHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class EnvelopeBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class EnvelopeCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class EnvelopeFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Header, EnvelopeHeaderExporter)
            REGISTER(Binary, EnvelopeBinaryExporter)
            REGISTER(Code, EnvelopeCodeExporter)
        };
    }

    bool HasModdedDependencies() override { return true; }
};
