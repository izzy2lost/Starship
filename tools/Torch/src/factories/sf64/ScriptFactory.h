#pragma once

#include <factories/BaseFactory.h>

namespace SF64 {

class ScriptData : public IParsedData {
public:
    std::vector<uint32_t> mPtrs;
    std::vector<uint16_t> mCmds;
    std::map<uint32_t, int> mSizeMap;
    uint32_t mCmdsStart;
    uint32_t mPtrsStart;

    ScriptData(std::vector<uint32_t> ptrs, std::vector<uint16_t> cmds, std::map<uint32_t, int> sizeMap, uint32_t ptrsStart, uint32_t cmdsStart);
};

class ScriptHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class ScriptBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class ScriptCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class ScriptXMLExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class ScriptFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(XML, ScriptXMLExporter)
            REGISTER(Code, ScriptCodeExporter)
            REGISTER(Header, ScriptHeaderExporter)
            REGISTER(Binary, ScriptBinaryExporter)
        };
    }
    bool SupportModdedAssets() override { return true; }
};
}
