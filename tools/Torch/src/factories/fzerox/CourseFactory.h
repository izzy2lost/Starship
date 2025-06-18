#pragma once

#include <factories/BaseFactory.h>

namespace FZX {

// In-Game Structs
typedef struct Vec3fStruct {
    float x, y, z;
} Vec3fStruct;

typedef struct ControlPoint {
    Vec3fStruct pos;
    int16_t radiusLeft;
    int16_t radiusRight;
    int32_t trackSegmentInfo;
} ControlPoint; // size = 0x14

typedef struct CourseRawData {
    int8_t creatorId;
    int8_t controlPointCount;
    int8_t venue;
    int8_t skybox;
    uint32_t checksum;
    int8_t flag;
    char fileName[23];
    ControlPoint controlPoint[64];
    int16_t bankAngle[64];
    int8_t pit[64];
    int8_t dash[64];
    int8_t dirt[64];
    int8_t ice[64];
    int8_t jump[64];
    int8_t landmine[64];
    int8_t gate[64];
    int8_t building[64];
    int8_t sign[64];
} CourseRawData; // size = 0x7E0

// Factory Structs
typedef struct ControlPointInfo {
    ControlPoint controlPoint;
    int16_t bankAngle;
    int8_t pit;
    int8_t dash;
    int8_t dirt;
    int8_t ice;
    int8_t jump;
    int8_t landmine;
    int8_t gate;
    int8_t building;
    int8_t sign;
} ControlPointInfo;

class CourseData : public IParsedData {
public:
    int8_t mCreatorId;
    int8_t mVenue;
    int8_t mSkybox;
    int8_t mFlag;
    std::string mFileName;
    std::vector<int8_t> mFileNameExtra;
    std::vector<ControlPointInfo> mControlPointInfos;

    CourseData(int8_t creatorId, int8_t venue, int8_t skybox, int8_t flag, std::string fileName, std::vector<int8_t> fileNameExtra, std::vector<ControlPointInfo> controlPointInfos) :
        mCreatorId(creatorId),
        mVenue(venue),
        mSkybox(skybox),
        mFlag(flag),
        mFileName(std::move(fileName)),
        mFileNameExtra(std::move(fileNameExtra)),
        mControlPointInfos(std::move(controlPointInfos)) {}

    uint32_t CalculateChecksum(void);
};

class CourseHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class CourseBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class CourseCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class CourseModdingExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class CourseFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    std::optional<std::shared_ptr<IParsedData>> parse_modding(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Code, CourseCodeExporter)
            REGISTER(Header, CourseHeaderExporter)
            REGISTER(Binary, CourseBinaryExporter)
            REGISTER(Modding, CourseModdingExporter)
        };
    }
    bool SupportModdedAssets() override { return true; }
};
} // namespace FZX
