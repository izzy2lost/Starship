#pragma once

#include <factories/BaseFactory.h>

namespace SF64 {

struct MessageEntry {
    int32_t id;
    uint32_t ptr;
};

class MessageTable : public IParsedData {
public:
    std::vector<MessageEntry> mTable;

    MessageTable(const std::vector<MessageEntry>&table) : mTable(table) {}
};

class MessageLookupCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class MessageLookupHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class MessageLookupBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class MessageLookupXMLExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class MessageLookupFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(XML, MessageLookupXMLExporter)
            REGISTER(Code, MessageLookupCodeExporter)
            REGISTER(Header, MessageLookupHeaderExporter)
            REGISTER(Binary, MessageLookupBinaryExporter)
        };
    }
    bool HasModdedDependencies() override { return true; }
};
}