#include "GhostRecordFactory.h"
#include "ghost/Ghost.h"

#include "utils/Decompressor.h"
#include "spdlog/spdlog.h"
#include "Companion.h"

#define ARRAY_COUNT(arr) (int32_t)(sizeof(arr) / sizeof(arr[0]))
#define ALIGN4(val) (((val) + 0x3) & ~0x3)

uint16_t FZX::GhostRecordData::Save_CalculateChecksum(void* data, int32_t size) {
    uint8_t* dataPtr = (uint8_t*)data;
    uint16_t checksum = 0;
    int32_t i;

    for (i = 0; i < size; i++) {
        checksum += *dataPtr++;
    }

    return checksum;
}

uint16_t FZX::GhostRecordData::CalculateRecordChecksum(void) {
    uint16_t checksum = 0;

    checksum += Save_CalculateChecksum(&mGhostType, sizeof(mGhostType));
    checksum += Save_CalculateChecksum(&mReplayChecksum, sizeof(mReplayChecksum));
    checksum += Save_CalculateChecksum(&mCourseEncoding, sizeof(mCourseEncoding));
    checksum += Save_CalculateChecksum(&mRaceTime, sizeof(mRaceTime));
    checksum += Save_CalculateChecksum(&mUnk10, sizeof(mUnk10));
    checksum += Save_CalculateChecksum((void*)mTrackName.c_str(), mTrackName.length());
    checksum += Save_CalculateChecksum(&mGhostMachineInfo, sizeof(mGhostMachineInfo));
    
    return checksum;
}

uint16_t FZX::GhostRecordData::CalculateDataChecksum(void) {
    uint16_t checksum = 0;
    
    checksum += Save_CalculateChecksum(&mReplayEnd, sizeof(mReplayEnd));
    checksum += Save_CalculateChecksum(&mReplaySize, sizeof(mReplaySize));

    for (auto lapTime : mLapTimes) {
        checksum += Save_CalculateChecksum(&lapTime, sizeof(lapTime));
    }

    checksum += Save_CalculateChecksum(mReplayData.data(), mReplayData.size());

    return checksum;
}

int32_t FZX::GhostRecordData::CalculateReplayChecksum(void) {
    int32_t checksum = 0;
    int32_t i = 0;
    int32_t sum = 0;

    for (auto dataByte : mReplayData) {
        sum += ((uint32_t)(uint8_t)dataByte) << ((3 - i) * 8);

        i++;
        i %= 4;
        if (i == 0) {
            checksum += sum;
            sum = 0;
        }
    }

    return checksum;
}

ExportResult FZX::GhostRecordHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "Record[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern GhostRecord " << symbol << "Record;\n";
    write << "extern GhostReplayInfo " << symbol << "ReplayInfo;\n";
    write << "extern s8 " << symbol << "Data[];\n";

    return std::nullopt;
}

ExportResult FZX::GhostRecordCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");
    const auto record = std::static_pointer_cast<GhostRecordData>(raw);

    // HACK!!
    // These checksums when read from disk do not line up to the data since the save of the data calculates the checksum with unsaved and unused buffer data, and therefore is uncalculable.
    // The record checksum can occasionally not align with the value for some other unknown reason.
    // When modding these values will be read in as 0 and calculated here
    if (record->mReplayChecksum == 0) {
        record->mReplayChecksum = record->CalculateReplayChecksum();
    }
    if (record->mRecordChecksum == 0) {
        record->mRecordChecksum = record->CalculateRecordChecksum();
    }
    if (record->mDataChecksum == 0) {
        record->mDataChecksum = record->CalculateDataChecksum();
    }

    write << "GhostRecord " << symbol << "Record = {\n";

    write << fourSpaceTab << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << record->mRecordChecksum << std::dec << ", /* Checksum */\n";

    write << fourSpaceTab << static_cast<GhostType>(record->mGhostType) << ",\n";

    write << fourSpaceTab << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << record->mReplayChecksum << std::dec << ", /* Replay Checksum */\n";

    write << fourSpaceTab << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << record->mCourseEncoding << std::dec << ", /* Course Encoding */\n";

    write << fourSpaceTab << record->mRaceTime << ", /* Race Time */ \n";

    write << fourSpaceTab << record->mUnk10 << ", { 0 },\n";

    write << fourSpaceTab << "\"" << record->mTrackName << "\",\n";

    write << fourSpaceTab << " {\n";
    write << fourSpaceTab << fourSpaceTab << static_cast<Character>(record->mGhostMachineInfo.character) << ", ";
    write << static_cast<CustomType>(record->mGhostMachineInfo.customType) << ",\n";

    write << fourSpaceTab << fourSpaceTab << static_cast<FrontType>(record->mGhostMachineInfo.frontType) << ", ";
    write << static_cast<RearType>(record->mGhostMachineInfo.rearType) << ", ";
    write << static_cast<WingType>(record->mGhostMachineInfo.wingType) << ",\n";

    write << fourSpaceTab << fourSpaceTab << static_cast<Logo>(record->mGhostMachineInfo.logo) << ", ";
    write << "MACHINE_NUMBER(" << (uint32_t)record->mGhostMachineInfo.number + 1 << "), ";
    write << static_cast<Decal>(record->mGhostMachineInfo.decal) << ",\n";

    write << fourSpaceTab << fourSpaceTab << (uint32_t)record->mGhostMachineInfo.bodyR << ", ";
    write << (uint32_t)record->mGhostMachineInfo.bodyG << ", ";
    write << (uint32_t)record->mGhostMachineInfo.bodyB << ", /* Body Color */\n";
    write << fourSpaceTab << fourSpaceTab << (uint32_t)record->mGhostMachineInfo.numberR << ", ";
    write << (uint32_t)record->mGhostMachineInfo.numberG << ", ";
    write << (uint32_t)record->mGhostMachineInfo.numberB << ", /* Number Color */\n";
    write << fourSpaceTab << fourSpaceTab << (uint32_t)record->mGhostMachineInfo.decalR << ", ";
    write << (uint32_t)record->mGhostMachineInfo.decalG << ", ";
    write << (uint32_t)record->mGhostMachineInfo.decalB << ", /* Decal Color */\n";
    write << fourSpaceTab << fourSpaceTab << (uint32_t)record->mGhostMachineInfo.cockpitR << ", ";
    write << (uint32_t)record->mGhostMachineInfo.cockpitG << ", ";
    write << (uint32_t)record->mGhostMachineInfo.cockpitB << "  /* Cockpit Color */\n";
    write << fourSpaceTab << "}\n";

    write << "};\n\n";


    write << "GhostReplayInfo " << symbol << "ReplayInfo = {\n";

    write << fourSpaceTab << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << record->mDataChecksum << std::dec << ", /* Checksum */\n";

    write << fourSpaceTab << "0,\n";

    write << fourSpaceTab << "{ ";

    for (int32_t i = 0; i < 3; i++) {
        write << record->mLapTimes.at(i);
        if (i != 2) {
            write << ", ";
        }
    }

    write << " },\n";

    write << fourSpaceTab << record->mReplayEnd << ", ";

    write << record->mReplaySize << ",\n";

    write << fourSpaceTab << "0, 0\n";

    write << "};\n\n";


    write << "s8 " << symbol << "Data[] = {\n";

    write << fourSpaceTab;

    int32_t counter = 0;
    for (uint32_t i = 0; i < record->mReplaySize; i++) {
        int32_t replayData = (int8_t)record->mReplayData.at(i);

        if (replayData == -0x80) {
            write << "";

            replayData = (int16_t)(((uint32_t)(uint8_t)record->mReplayData.at(i + 1) << 8) | (uint8_t)record->mReplayData.at(i + 2));
            write << "REPLAY_DATA_LARGE(" << replayData << ")";

            i += 2;
        } else {
            write << replayData;
        }

        if (i != record->mReplaySize - 1) {
            write << ", ";
        } else {
            write << "\n";
            break;
        }

        if ((++counter % 3) == 0) {
            write << "\n" << fourSpaceTab;
        }
    }

    write << "};\n\n";

    return offset + 0x20 + 0x40 + ALIGN4(record->mReplaySize);
}

ExportResult FZX::GhostRecordBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    auto writer = LUS::BinaryWriter();
    const auto record = std::static_pointer_cast<GhostRecordData>(raw);

    WriteHeader(writer, Torch::ResourceType::GhostRecord, 0);

    writer.Write(record->mGhostType);
    writer.Write(record->mCourseEncoding);
    writer.Write(record->mRaceTime);
    writer.Write(record->mUnk10);
    writer.Write((uint32_t)record->mTrackName.length());
    writer.Write(record->mTrackName);
    writer.Write(record->mGhostMachineInfo.character);
    writer.Write(record->mGhostMachineInfo.customType);
    writer.Write(record->mGhostMachineInfo.frontType);
    writer.Write(record->mGhostMachineInfo.rearType);
    writer.Write(record->mGhostMachineInfo.wingType);
    writer.Write(record->mGhostMachineInfo.logo);
    writer.Write(record->mGhostMachineInfo.number);
    writer.Write(record->mGhostMachineInfo.decal);
    writer.Write(record->mGhostMachineInfo.bodyR);
    writer.Write(record->mGhostMachineInfo.bodyG);
    writer.Write(record->mGhostMachineInfo.bodyB);
    writer.Write(record->mGhostMachineInfo.numberR);
    writer.Write(record->mGhostMachineInfo.numberG);
    writer.Write(record->mGhostMachineInfo.numberB);
    writer.Write(record->mGhostMachineInfo.decalR);
    writer.Write(record->mGhostMachineInfo.decalG);
    writer.Write(record->mGhostMachineInfo.decalB);
    writer.Write(record->mGhostMachineInfo.cockpitR);
    writer.Write(record->mGhostMachineInfo.cockpitG);
    writer.Write(record->mGhostMachineInfo.cockpitB);

    for (uint32_t i = 0; i < 3; i++) {
        writer.Write(record->mLapTimes.at(i));
    }

    writer.Write(record->mReplayEnd);
    writer.Write(record->mReplaySize);

    for (uint32_t i = 0; i < record->mReplaySize; i++) {
        writer.Write(record->mReplayData.at(i));
    }

    writer.Finish(write);
    return std::nullopt;
}

ExportResult FZX::GhostRecordModdingExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto record = std::static_pointer_cast<GhostRecordData>(raw);
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    *replacement += ".yaml";

    std::stringstream stream;
    YAML::Emitter out;

    out << YAML::BeginMap;
    out << YAML::Key << symbol;
    out << YAML::Value;
    out.SetIndent(2);
    out << YAML::BeginMap;
    out << YAML::Key << "GhostType";
    out << YAML::Value << record->mGhostType;
    out << YAML::Key << "CourseEncoding";
    out << YAML::Value << YAML::Hex << record->mCourseEncoding << YAML::Dec;
    out << YAML::Key << "RaceTime";
    out << YAML::Value << record->mRaceTime;
    out << YAML::Key << "Unk10";
    out << YAML::Value << record->mUnk10;
    out << YAML::Key << "TrackName";
    out << YAML::Value << record->mTrackName;
    out << YAML::Key << "GhostMachineInfo";
    out << YAML::Value;
    out << YAML::BeginMap;
    out << YAML::Key << "Character";
    out << YAML::Value << (uint32_t)record->mGhostMachineInfo.character;
    out << YAML::Key << "CustomType";
    out << YAML::Value << (uint32_t)record->mGhostMachineInfo.customType;
    out << YAML::Key << "FrontType";
    out << YAML::Value << (uint32_t)record->mGhostMachineInfo.frontType;
    out << YAML::Key << "RearType";
    out << YAML::Value << (uint32_t)record->mGhostMachineInfo.rearType;
    out << YAML::Key << "WingType";
    out << YAML::Value << (uint32_t)record->mGhostMachineInfo.wingType;
    out << YAML::Key << "Logo";
    out << YAML::Value << (uint32_t)record->mGhostMachineInfo.logo;
    out << YAML::Key << "Number";
    out << YAML::Value << (uint32_t)record->mGhostMachineInfo.number;
    out << YAML::Key << "Decal";
    out << YAML::Value << (uint32_t)record->mGhostMachineInfo.decal;
    out << YAML::Key << "BodyR";
    out << YAML::Value << (uint32_t)record->mGhostMachineInfo.bodyR;
    out << YAML::Key << "BodyG";
    out << YAML::Value << (uint32_t)record->mGhostMachineInfo.bodyG;
    out << YAML::Key << "BodyB";
    out << YAML::Value << (uint32_t)record->mGhostMachineInfo.bodyB;
    out << YAML::Key << "NumberR";
    out << YAML::Value << (uint32_t)record->mGhostMachineInfo.numberR;
    out << YAML::Key << "NumberG";
    out << YAML::Value << (uint32_t)record->mGhostMachineInfo.numberG;
    out << YAML::Key << "NumberB";
    out << YAML::Value << (uint32_t)record->mGhostMachineInfo.numberB;
    out << YAML::Key << "DecalR";
    out << YAML::Value << (uint32_t)record->mGhostMachineInfo.decalR;
    out << YAML::Key << "DecalG";
    out << YAML::Value << (uint32_t)record->mGhostMachineInfo.decalG;
    out << YAML::Key << "DecalB";
    out << YAML::Value << (uint32_t)record->mGhostMachineInfo.decalB;
    out << YAML::Key << "CockpitR";
    out << YAML::Value << (uint32_t)record->mGhostMachineInfo.cockpitR;
    out << YAML::Key << "CockpitG";
    out << YAML::Value << (uint32_t)record->mGhostMachineInfo.cockpitG;
    out << YAML::Key << "CockpitB";
    out << YAML::Value << (uint32_t)record->mGhostMachineInfo.cockpitB;
    out << YAML::EndMap;
    out << YAML::Key << "LapTimes";
    out << YAML::Value << YAML::Flow << YAML::BeginSeq;
    for (int32_t i = 0; i < 3; i++) {
        out << YAML::Value << record->mLapTimes.at(i);
    }
    out << YAML::EndSeq << YAML::Block;

    out << YAML::Key << "ReplayEnd";
    out << YAML::Value << record->mReplayEnd;
    out << YAML::Key << "ReplaySize";
    out << YAML::Value << record->mReplaySize;

    out << YAML::Key << "ReplayData";
    out << YAML::Value << YAML::Hex << YAML::Flow << YAML::BeginSeq;
    for (uint32_t i = 0; i < record->mReplaySize; i++) {
        out << YAML::Value << (uint32_t)(uint8_t)record->mReplayData.at(i);
    }
    out << YAML::EndSeq << YAML::Block << YAML::Dec;

    out << YAML::EndMap;
    out << YAML::EndMap;

    write.write(out.c_str(), out.size());

    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> FZX::GhostRecordFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    LUS::BinaryReader reader(segment.data, segment.size);

    reader.SetEndianness(Torch::Endianness::Big);

    // Record
    uint16_t recordChecksum = reader.ReadInt16();
    uint16_t ghostType = reader.ReadUInt16();
    int32_t replayChecksum = reader.ReadInt32();
    int32_t courseEncoding = reader.ReadInt32();
    int32_t raceTime = reader.ReadInt32();
    
    uint16_t unk_10 = reader.ReadInt16();
    for (int32_t i = 0; i < 5; i++) {
        reader.ReadInt8();
    }
    
    char trackNameBuffer[9];
    for (int32_t i = 0; i < ARRAY_COUNT(trackNameBuffer); i++) {
        trackNameBuffer[i] = reader.ReadChar();
    }
    std::string trackName(trackNameBuffer);

    GhostMachineInfo ghostMachineInfo;

    ghostMachineInfo.character = reader.ReadUByte();
    ghostMachineInfo.customType = reader.ReadUByte();
    ghostMachineInfo.frontType = reader.ReadUByte();
    ghostMachineInfo.rearType = reader.ReadUByte();
    ghostMachineInfo.wingType = reader.ReadUByte();
    ghostMachineInfo.logo = reader.ReadUByte();
    ghostMachineInfo.number = reader.ReadUByte();
    ghostMachineInfo.decal = reader.ReadUByte();
    ghostMachineInfo.bodyR = reader.ReadUByte();
    ghostMachineInfo.bodyG = reader.ReadUByte();
    ghostMachineInfo.bodyB = reader.ReadUByte();
    ghostMachineInfo.numberR = reader.ReadUByte();
    ghostMachineInfo.numberG = reader.ReadUByte();
    ghostMachineInfo.numberB = reader.ReadUByte();
    ghostMachineInfo.decalR = reader.ReadUByte();
    ghostMachineInfo.decalG = reader.ReadUByte();
    ghostMachineInfo.decalB = reader.ReadUByte();
    ghostMachineInfo.cockpitR = reader.ReadUByte();
    ghostMachineInfo.cockpitG = reader.ReadUByte();
    ghostMachineInfo.cockpitB = reader.ReadUByte();

    // Data
    reader.Seek(0x40, LUS::SeekOffsetType::Start);

    uint16_t dataChecksum = reader.ReadInt16();

    reader.ReadInt16();

    std::vector<int32_t> lapTimes;

    for (int32_t i = 0; i < 3; i++) {
        lapTimes.push_back(reader.ReadInt32());
    }

    int32_t replayEnd = reader.ReadInt32();
    uint32_t replaySize = reader.ReadUInt32();

    reader.ReadInt32();
    reader.ReadInt32();

    std::vector<int8_t> replayData;
    for (uint32_t i = 0; i < replaySize; i++) {
        replayData.push_back(reader.ReadInt8());
    }

    return std::make_shared<GhostRecordData>(recordChecksum, ghostType, replayChecksum, courseEncoding, raceTime, unk_10, trackName, ghostMachineInfo, dataChecksum, lapTimes, replayEnd, replaySize, replayData);
}

std::optional<std::shared_ptr<IParsedData>> FZX::GhostRecordFactory::parse_modding(std::vector<uint8_t>& buffer, YAML::Node& node) {
    YAML::Node assetNode;
    
    try {
        std::string text((char*) buffer.data(), buffer.size());
        assetNode = YAML::Load(text.c_str());
    } catch (YAML::ParserException& e) {
        SPDLOG_ERROR("Failed to parse message data: {}", e.what());
        SPDLOG_ERROR("{}", (char*) buffer.data());
        return std::nullopt;
    }

    const auto info = assetNode.begin()->second;

    uint16_t ghostType = info["GhostType"].as<uint16_t>();
    int32_t courseEncoding = info["CourseEncoding"].as<uint32_t>();
    int32_t raceTime = info["RaceTime"].as<int32_t>();
    uint16_t unk_10 = info["Unk10"].as<uint16_t>();
    std::string trackName = info["TrackName"].as<std::string>();

    GhostMachineInfo ghostMachineInfo;

    ghostMachineInfo.character = info["GhostMachineInfo"]["Character"].as<uint32_t>();
    ghostMachineInfo.customType = info["GhostMachineInfo"]["CustomType"].as<uint32_t>();
    ghostMachineInfo.frontType = info["GhostMachineInfo"]["FrontType"].as<uint32_t>();
    ghostMachineInfo.rearType = info["GhostMachineInfo"]["RearType"].as<uint32_t>();
    ghostMachineInfo.wingType = info["GhostMachineInfo"]["WingType"].as<uint32_t>();
    ghostMachineInfo.logo = info["GhostMachineInfo"]["Logo"].as<uint32_t>();
    ghostMachineInfo.number = info["GhostMachineInfo"]["Number"].as<uint32_t>();
    ghostMachineInfo.decal = info["GhostMachineInfo"]["Decal"].as<uint32_t>();
    ghostMachineInfo.bodyR = info["GhostMachineInfo"]["BodyR"].as<uint32_t>();
    ghostMachineInfo.bodyG = info["GhostMachineInfo"]["BodyG"].as<uint32_t>();
    ghostMachineInfo.bodyB = info["GhostMachineInfo"]["BodyB"].as<uint32_t>();
    ghostMachineInfo.numberR = info["GhostMachineInfo"]["NumberR"].as<uint32_t>();
    ghostMachineInfo.numberG = info["GhostMachineInfo"]["NumberG"].as<uint32_t>();
    ghostMachineInfo.numberB = info["GhostMachineInfo"]["NumberB"].as<uint32_t>();
    ghostMachineInfo.decalR = info["GhostMachineInfo"]["DecalR"].as<uint32_t>();
    ghostMachineInfo.decalG = info["GhostMachineInfo"]["DecalG"].as<uint32_t>();
    ghostMachineInfo.decalB = info["GhostMachineInfo"]["DecalB"].as<uint32_t>();
    ghostMachineInfo.cockpitR = info["GhostMachineInfo"]["CockpitR"].as<uint32_t>();
    ghostMachineInfo.cockpitG = info["GhostMachineInfo"]["CockpitG"].as<uint32_t>();
    ghostMachineInfo.cockpitB = info["GhostMachineInfo"]["CockpitB"].as<uint32_t>();
    
    auto lapTimesInfo = info["LapTimes"];
    std::vector<int32_t> lapTimes;

    for (YAML::iterator it = lapTimesInfo.begin(); it != lapTimesInfo.end(); ++it) {
        lapTimes.push_back((*it).as<int32_t>());
    }


    if (lapTimes.size() < 3) {
        throw std::runtime_error("Invalid number of lap times in Ghost " + node["symbol"].as<std::string>());
    }

    int32_t replayEnd = info["ReplayEnd"].as<int32_t>();
    uint32_t replaySize = info["ReplaySize"].as<uint32_t>();

    auto replayDataInfo = info["ReplayData"];
    std::vector<int8_t> replayData;

    for (YAML::iterator it = replayDataInfo.begin(); it != replayDataInfo.end(); ++it) {
        replayData.push_back((int8_t)(*it).as<uint32_t>());
    }

    if (replayData.size() != replaySize) {
        throw std::runtime_error("Invalid replay size in Ghost " + node["symbol"].as<std::string>());
    }
    
    return std::make_shared<GhostRecordData>(0, ghostType, 0, courseEncoding, raceTime, unk_10, trackName, ghostMachineInfo, 0, lapTimes, replayEnd, replaySize, replayData);
}
