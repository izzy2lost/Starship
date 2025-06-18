#pragma once

#include <factories/BaseFactory.h>

namespace MK64 {

    struct BhvRaw {
        int16_t		waypoint1;
        int16_t	waypoint2;
        int32_t		bhv;
    };

    class DrivingData : public IParsedData {
    public:
        std::vector<BhvRaw> mBhvs;

        explicit DrivingData(std::vector<BhvRaw> bhvs) : mBhvs(bhvs) {}
    };

    class DrivingBehaviourHeaderExporter : public BaseExporter {
        ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
    };

    class DrivingBehaviourBinaryExporter : public BaseExporter {
        ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
    };

    class DrivingBehaviourCodeExporter : public BaseExporter {
        ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
    };

    class DrivingBehaviourFactory : public BaseFactory {
    public:
        std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
        std::optional<std::shared_ptr<IParsedData>> parse_modding(std::vector<uint8_t>& buffer, YAML::Node& data) override {
            return std::nullopt;
        }
        inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
            return {
                REGISTER(Code, DrivingBehaviourCodeExporter)
                REGISTER(Header, DrivingBehaviourHeaderExporter)
                REGISTER(Binary, DrivingBehaviourBinaryExporter)
            };
        }
    };
}