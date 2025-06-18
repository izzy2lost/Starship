#pragma once

#include <factories/BaseFactory.h>

/*
class AudioAIFCExporter : public BaseExporter {
public:
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement);
};
*/

class AudioDummyExporter : public BaseExporter {
public:
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override {
        return std::nullopt;
    }
};

class AudioHeaderFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    std::optional<std::shared_ptr<IParsedData>> parse_modding(std::vector<uint8_t>& buffer, YAML::Node& data) override {
        return std::nullopt;
    }
    std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
        //  REGISTER(Modding, AudioGenericAIFCExporter)
            REGISTER(Header, AudioDummyExporter)
            REGISTER(Binary, AudioDummyExporter)
            REGISTER(Code, AudioDummyExporter)
        };
    }
    bool HasModdedDependencies() override { return true; }
};