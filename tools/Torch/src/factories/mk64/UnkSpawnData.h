#pragma once

#include <factories/BaseFactory.h>

namespace MK64 {

    struct UnkActorSpawnData {
        int16_t x;
        int16_t y;
        int16_t z;
        int16_t someId;
        int16_t unk8;
    };

    class UnkSpawnDataData : public IParsedData {
    public:
        std::vector<UnkActorSpawnData> mSpawns;

        explicit UnkSpawnDataData(std::vector<UnkActorSpawnData> spawns) : mSpawns(spawns) {}
    };

    class UnkSpawnDataHeaderExporter : public BaseExporter {
        ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
    };

    class UnkSpawnDataBinaryExporter : public BaseExporter {
        ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
    };

    class UnkSpawnDataCodeExporter : public BaseExporter {
        ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
    };

    class UnkSpawnDataFactory : public BaseFactory {
    public:
        std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
        std::optional<std::shared_ptr<IParsedData>> parse_modding(std::vector<uint8_t>& buffer, YAML::Node& data) override {
            return std::nullopt;
        }
        inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
            return {
                REGISTER(Code, UnkSpawnDataCodeExporter)
                REGISTER(Header, UnkSpawnDataHeaderExporter)
                REGISTER(Binary, UnkSpawnDataBinaryExporter)
            };
        }
        bool SupportModdedAssets() override { return false; }
    };
}