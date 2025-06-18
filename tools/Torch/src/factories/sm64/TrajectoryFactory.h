#pragma once

#include "factories/BaseFactory.h"
#include "types/Vec3D.h"

namespace SM64 {

struct Trajectory {
    int16_t trajId;
    int16_t posX;
    int16_t posY;
    int16_t posZ;
    Trajectory(int16_t trajId, int16_t posX, int16_t posY, int16_t posZ) : trajId(trajId), posX(posX), posY(posY), posZ(posZ) {}
};

class TrajectoryData : public IParsedData {
public:
    std::vector<Trajectory> mTrajectoryData;

    TrajectoryData(std::vector<Trajectory> trajectoryData) : mTrajectoryData(std::move(trajectoryData)) {}
};

class TrajectoryHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class TrajectoryBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class TrajectoryCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class TrajectoryFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Code, TrajectoryCodeExporter)
            REGISTER(Header, TrajectoryHeaderExporter)
            REGISTER(Binary, TrajectoryBinaryExporter)
        };
    }
};
}
