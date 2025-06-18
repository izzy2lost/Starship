#pragma once

#include "BaseFactory.h"

class IncludeData : public IParsedData {
public:
    uint32_t blanks;

    explicit IncludeData(uint32_t blank) : blanks(blank) {}
};

class IncludeHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class IncludeBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class IncludeCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class IncludeFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Code, IncludeCodeExporter)
            REGISTER(Header, IncludeHeaderExporter)
        };
    }
    uint32_t GetAlignment() override {
        return 8;
    };
};
