#pragma once

#include "AudioTableFactory.h"
#include "factories/naudio/v0/AudioHeaderFactory.h"
#include "factories/BaseFactory.h"

struct TunedSample {
    uint32_t sample;
    uint32_t sampleBankId;
    float tuning;
};

struct TableEntry {
    std::shared_ptr<AudioTableData> info;
    std::unordered_map<uint32_t, AudioTableEntry> entries;
    std::vector<uint8_t> buffer;
    uint32_t offset;
};

enum class NAudioDrivers {
    SF64, FZEROX, UNKNOWN
};

class AudioContext {
public:
    static std::unordered_map<AudioTableType, TableEntry> tables;
    static NAudioDrivers driver;

    static LUS::BinaryReader MakeReader(AudioTableType type, uint32_t offset);
    static TunedSample LoadTunedSample(LUS::BinaryReader& reader, uint32_t parent, uint32_t sampleBankId);
    static uint64_t GetPathByAddr(uint32_t addr);

    static const char* GetMediumStr(uint8_t medium);
    static const char* GetCachePolicyStr(uint8_t policy);
    static const char* GetCodecStr(uint8_t codec);
};

class AudioContextFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;

    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Header, AudioDummyExporter)
            REGISTER(Binary, AudioDummyExporter)
            REGISTER(Code, AudioDummyExporter)
        };
    }

    bool HasModdedDependencies() override { return true; }
};