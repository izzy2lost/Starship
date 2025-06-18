#pragma once

#include <factories/BaseFactory.h>

class SoundFontData : public IParsedData {
public:
    uint8_t numInstruments;
    uint8_t numDrums;
    uint8_t sampleBankId1;
    uint8_t sampleBankId2;
    std::vector<uint32_t> instruments;
    std::vector<uint32_t> drums;
};

class SoundFontHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class SoundFontBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class SoundFontCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class SoundFontXMLExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class SoundFontFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Header, SoundFontHeaderExporter)
            REGISTER(Binary, SoundFontBinaryExporter)
            REGISTER(Code, SoundFontCodeExporter)
            REGISTER(XML, SoundFontXMLExporter)
        };
    }

    bool HasModdedDependencies() override { return true; }
};
