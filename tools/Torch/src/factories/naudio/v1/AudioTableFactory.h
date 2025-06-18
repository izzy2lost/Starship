#pragma once

#include <factories/BaseFactory.h>

struct AudioTableEntry {
    uint32_t addr;
    uint32_t size;
    int8_t medium;
    int8_t cachePolicy;
    int16_t shortData1;
    int16_t shortData2;
    int16_t shortData3;
    uint64_t crc;
};

enum class AudioTableType {
    SAMPLE_TABLE,
    SEQ_TABLE,
    FONT_TABLE
};

class AudioTableData : public IParsedData {
public:
    int16_t medium;
    uint32_t addr;
    AudioTableType type;
    std::vector<AudioTableEntry> entries;
    
    AudioTableData(int16_t medium, uint32_t addr, AudioTableType type, std::vector<AudioTableEntry> entries) : medium(medium), addr(addr), type(type), entries(entries) {}
};

class AudioTableHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class AudioTableBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class AudioTableCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class AudioTableFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Header, AudioTableHeaderExporter)
            REGISTER(Binary, AudioTableBinaryExporter)
            REGISTER(Code, AudioTableCodeExporter)
        };
    }

    bool HasModdedDependencies() override { return true; }
};
