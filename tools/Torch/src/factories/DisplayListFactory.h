#pragma once

#include "BaseFactory.h"
#include "n64/CommandMacros.h"
#include <tuple>

class DListData : public IParsedData {
public:
    std::vector<uint32_t> mGfxs;

    DListData() {}
    DListData(std::vector<uint32_t> gfxs) : mGfxs(gfxs) {}
};

class DListHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class DListBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

#ifdef STANDALONE
class DListCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};
#endif

class DListFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Header, DListHeaderExporter)
            REGISTER(Binary, DListBinaryExporter)
        #ifdef STANDALONE
            REGISTER(Code, DListCodeExporter)
        #endif
        };
    }
    uint32_t GetAlignment() override {
        return 8;
    }
};
