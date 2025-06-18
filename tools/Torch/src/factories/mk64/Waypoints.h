#pragma once

#include <factories/BaseFactory.h>

namespace MK64 {

    struct TrackWaypoint {
        int16_t posX;
        int16_t posY;
        int16_t posZ;
        uint16_t trackSegment;
    };

    class WaypointData : public IParsedData {
    public:
        std::vector<TrackWaypoint> mWaypoints;

        explicit WaypointData(std::vector<TrackWaypoint> waypoints) : mWaypoints(waypoints) {}
    };

    class WaypointHeaderExporter : public BaseExporter {
        ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
    };

    class WaypointBinaryExporter : public BaseExporter {
        ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
    };

    class WaypointCodeExporter : public BaseExporter {
        ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
    };

    class WaypointsFactory : public BaseFactory {
    public:
        std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
        std::optional<std::shared_ptr<IParsedData>> parse_modding(std::vector<uint8_t>& buffer, YAML::Node& data) override {
            return std::nullopt;
        }
        inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
            return {
                REGISTER(Code, WaypointCodeExporter)
                REGISTER(Header, WaypointHeaderExporter)
                REGISTER(Binary, WaypointBinaryExporter)
            };
        }
        bool SupportModdedAssets() override { return false; }
    };
}