#pragma once

#include "factories/BaseFactory.h"

#define WATER_DROPLET_FLAG_RAND_ANGLE                0x02
#define WATER_DROPLET_FLAG_RAND_OFFSET_XZ            0x04 // Unused
#define WATER_DROPLET_FLAG_RAND_OFFSET_XYZ           0x08 // Unused
#define WATER_DROPLET_FLAG_SET_Y_TO_WATER_LEVEL      0x20
#define WATER_DROPLET_FLAG_RAND_ANGLE_INCR_PLUS_8000 0x40
#define WATER_DROPLET_FLAG_RAND_ANGLE_INCR           0x80 // Unused

#define MODEL_WHITE_PARTICLE_SMALL 0xA4
#define MODEL_FISH                 0xB9

namespace SM64 {

class WaterDropletData : public IParsedData {
public:
    int16_t flags;
    int16_t model;
    uint32_t behavior;
    int16_t moveAngleRange;
    int16_t moveRange;
    float randForwardVelOffset;
    float randForwardVelScale;
    float randYVelOffset;
    float randYVelScale;
    float randSizeOffset;
    float randSizeScale;

    WaterDropletData(
        int16_t flags, int16_t model, uint32_t behavior, int16_t moveAngleRange, int16_t moveRange, float randForwardVelOffset, float randForwardVelScale, float randYVelOffset, float randYVelScale, float randSizeOffset, float randSizeScale
    ) : flags(flags), model(model), behavior(behavior), moveAngleRange(moveAngleRange), moveRange(moveRange), randForwardVelOffset(randForwardVelOffset), randForwardVelScale(randForwardVelScale), randYVelOffset(randYVelOffset), randYVelScale(randYVelScale), randSizeOffset(randSizeOffset), randSizeScale(randSizeScale) {}
};

class WaterDropletHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class WaterDropletBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class WaterDropletCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class WaterDropletFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Code, WaterDropletCodeExporter)
            REGISTER(Header, WaterDropletHeaderExporter)
            REGISTER(Binary, WaterDropletBinaryExporter)
        };
    }
};
}
