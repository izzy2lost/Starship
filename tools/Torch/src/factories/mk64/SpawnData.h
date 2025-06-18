#pragma once

#include <factories/BaseFactory.h>

namespace MK64 {

    struct ActorSpawnData {
        int16_t x;
        int16_t y;
        int16_t z;
        uint16_t id;
    };

    class SpawnDataData : public IParsedData {
    public:
        std::vector<ActorSpawnData> mSpawns;

        explicit SpawnDataData(std::vector<ActorSpawnData> spawns) : mSpawns(spawns) {}
    };

    class SpawnDataHeaderExporter : public BaseExporter {
        ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
    };

    class SpawnDataBinaryExporter : public BaseExporter {
        ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
    };

    class SpawnDataCodeExporter : public BaseExporter {
        ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
    };

    class SpawnDataFactory : public BaseFactory {
    public:
        std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
        std::optional<std::shared_ptr<IParsedData>> parse_modding(std::vector<uint8_t>& buffer, YAML::Node& data) override {
            return std::nullopt;
        }
        inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
            return {
                REGISTER(Code, SpawnDataCodeExporter)
                REGISTER(Header, SpawnDataHeaderExporter)
                REGISTER(Binary, SpawnDataBinaryExporter)
            };
        }
        bool SupportModdedAssets() override { return false; }
    };
}