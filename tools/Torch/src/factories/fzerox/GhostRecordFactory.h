#pragma once

#include <factories/BaseFactory.h>

namespace FZX {

typedef struct GhostMachineInfo {
    /* 0x00 */ uint8_t character;
    /* 0x01 */ uint8_t customType;
    /* 0x02 */ uint8_t frontType;
    /* 0x03 */ uint8_t rearType;
    /* 0x04 */ uint8_t wingType;
    /* 0x05 */ uint8_t logo;
    /* 0x06 */ uint8_t number;
    /* 0x07 */ uint8_t decal;
    /* 0x08 */ uint8_t bodyR;
    /* 0x09 */ uint8_t bodyG;
    /* 0x0A */ uint8_t bodyB;
    /* 0x0B */ uint8_t numberR;
    /* 0x0C */ uint8_t numberG;
    /* 0x0D */ uint8_t numberB;
    /* 0x0E */ uint8_t decalR;
    /* 0x0F */ uint8_t decalG;
    /* 0x10 */ uint8_t decalB;
    /* 0x11 */ uint8_t cockpitR;
    /* 0x12 */ uint8_t cockpitG;
    /* 0x13 */ uint8_t cockpitB;
} GhostMachineInfo;

class GhostRecordData : public IParsedData {
public:
    // Records
    uint16_t mRecordChecksum;
    uint16_t mGhostType;
    int32_t mReplayChecksum;
    int32_t mCourseEncoding;
    int32_t mRaceTime;
    uint16_t mUnk10;
    std::string mTrackName; // empty for normal tracks
    GhostMachineInfo mGhostMachineInfo;
    
    // Data
    uint16_t mDataChecksum;
    std::vector<int32_t> mLapTimes;
    int32_t mReplayEnd;
    uint32_t mReplaySize;
    std::vector<int8_t> mReplayData;

    GhostRecordData(uint16_t recordChecksum, uint16_t ghostType, int32_t replayChecksum, int32_t courseEncoding, int32_t raceTime, uint16_t unk_10, std::string trackName, GhostMachineInfo& ghostMachineInfo, uint16_t dataChecksum, std::vector<int32_t> lapTimes, int32_t replayEnd, uint32_t replaySize, std::vector<int8_t> replayData) :
        mRecordChecksum(recordChecksum),
        mGhostType(ghostType),
        mReplayChecksum(replayChecksum),
        mCourseEncoding(courseEncoding),
        mRaceTime(raceTime),
        mUnk10(unk_10),
        mTrackName(trackName),
        mGhostMachineInfo(ghostMachineInfo),
        mDataChecksum(dataChecksum),
        mLapTimes(std::move(lapTimes)),
        mReplayEnd(replayEnd),
        mReplaySize(replaySize),
        mReplayData(std::move(replayData)) {}

    uint16_t Save_CalculateChecksum(void* data, int32_t size);
    uint16_t CalculateRecordChecksum(void);
    uint16_t CalculateDataChecksum(void);
    int32_t CalculateReplayChecksum(void);
};

class GhostRecordHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class GhostRecordBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class GhostRecordCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class GhostRecordModdingExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class GhostRecordFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    std::optional<std::shared_ptr<IParsedData>> parse_modding(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Code, GhostRecordCodeExporter)
            REGISTER(Header, GhostRecordHeaderExporter)
            REGISTER(Binary, GhostRecordBinaryExporter)
            REGISTER(Modding, GhostRecordModdingExporter)
        };
    }
    bool SupportModdedAssets() override { return true; }
};
} // namespace FZX
