#include "CourseFactory.h"
#include "course/Course.h"

#include "utils/Decompressor.h"
#include "spdlog/spdlog.h"
#include "Companion.h"
#include <types/Vec3D.h>

#define CP_ENUMS_PER_LINE 4

#define ARRAY_COUNT(arr) (int32_t)(sizeof(arr) / sizeof(arr[0]))

#define FORMAT_HEX(x, w) std::hex << std::setfill('0') << std::setw(w) << x
#define FORMAT_FLOAT(x, w, p) std::dec << std::setfill(' ') << std::fixed << std::setprecision(p) << std::setw(w) << x

uint32_t FZX::CourseData::CalculateChecksum(void) {
    uint32_t checksum = mControlPointInfos.size();
    int32_t counter = 0;

    for (const auto& controlPointInfo : mControlPointInfos) {
        int32_t trackSegmentInfo = controlPointInfo.controlPoint.trackSegmentInfo;
        trackSegmentInfo &= ~TRACK_JOIN_MASK;
        trackSegmentInfo &= ~TRACK_FORM_MASK;
        trackSegmentInfo &= ~TRACK_FLAG_CONTINUOUS;

        checksum += (int32_t) ((controlPointInfo.controlPoint.pos.x + ((1.1f + (0.7f * counter)) * controlPointInfo.controlPoint.pos.y)) +
                   ((2.2f + (1.2f * counter)) * controlPointInfo.controlPoint.pos.z * (4.4f + (0.9f * counter))) +
                   controlPointInfo.controlPoint.radiusLeft + ((5.5f + (0.8f * counter)) * controlPointInfo.controlPoint.radiusRight * 4.8f)) +
            trackSegmentInfo * (0xFE - counter) + controlPointInfo.bankAngle * (0x93DE - counter * 2);
        checksum += (controlPointInfo.pit * counter);
        checksum += (controlPointInfo.dash * (counter + 0x10));
        checksum += (controlPointInfo.dirt * (counter + 0x80));
        checksum += (controlPointInfo.ice * (counter + 0x100));
        checksum += (controlPointInfo.jump * (counter + 0x800));
        checksum += (controlPointInfo.landmine * (counter + 0x1000));
        checksum += (controlPointInfo.gate * (counter + 0x8000));
        checksum += (controlPointInfo.building * (counter + 0x10000));
        checksum += (controlPointInfo.sign * (counter + 0x80000));
        counter++;
    }


    return checksum;
}

ExportResult FZX::CourseHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern CourseData " << symbol << ";\n";

    return std::nullopt;
}

ExportResult FZX::CourseCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");
    const auto course = std::static_pointer_cast<CourseData>(raw);
    size_t controlPointCount = course->mControlPointInfos.size();

    write << "CourseData " << symbol << " = {\n";

    write << std::dec;
    if (course->mCreatorId == CREATOR_NINTENDO) {
        write << fourSpaceTab << "CREATOR_NINTENDO" << ", /* Creator Id */\n";
    } else {
        write << fourSpaceTab << (int32_t)course->mCreatorId << ", /* Creator Id */\n";
    }
    write << fourSpaceTab << (int32_t)controlPointCount << ", /* Control Point Count */\n";
    write << fourSpaceTab << static_cast<Venue>(course->mVenue) << ", /* Venue */\n";
    write << fourSpaceTab << static_cast<Skybox>(course->mSkybox) << ", /* Skybox */\n";
    write << fourSpaceTab << "0x" << std::hex << std::uppercase << course->CalculateChecksum() << std::dec << ", /* Checksum */\n";
    write << fourSpaceTab << (int32_t)course->mFlag << ", /* Flag */\n";

    write << fourSpaceTab << "\"" << course->mFileName << std::hex;
    for (int32_t i = 0; i < 22 - course->mFileName.length(); i++) {
        if (i < (int32_t)course->mFileNameExtra.size() - 1) {
            write << "\\x" << FORMAT_HEX((uint32_t)(uint8_t)course->mFileNameExtra.at(i), 2);
        } else {
            write << "\\x00";
        }
    }
    write << std::dec << "\", /* File Name */\n";

    write << fourSpaceTab << "{ /* Control Points */\n";

    for (size_t i = 0; i < 64; i++) {
        write << fourSpaceTab << fourSpaceTab;
        if (i < controlPointCount) {
            const ControlPointInfo& controlPointInfo = course->mControlPointInfos.at(i);
            Vec3f pos(controlPointInfo.controlPoint.pos.x, controlPointInfo.controlPoint.pos.y, controlPointInfo.controlPoint.pos.z);
            write << "{ { " << FORMAT_FLOAT(pos, 6, 4) << "}, ";
            write << controlPointInfo.controlPoint.radiusLeft << ", ";
            write << controlPointInfo.controlPoint.radiusRight << ",\n";
            write << fourSpaceTab << fourSpaceTab << "  ";
            int32_t trackSegmentInfo = controlPointInfo.controlPoint.trackSegmentInfo;
            bool hasFlags = false;
            if (trackSegmentInfo & TRACK_FLAG_20000000) {
                if (hasFlags) {
                    write << " | ";
                }
                write << "TRACK_FLAG_20000000";
                hasFlags = true;
                trackSegmentInfo &= ~TRACK_FLAG_20000000;
            }

            if (trackSegmentInfo & TRACK_FLAG_JOINABLE) {
                if (hasFlags) {
                    write << " | ";
                }
                write << "TRACK_FLAG_JOINABLE";
                hasFlags = true;
                trackSegmentInfo &= ~TRACK_FLAG_JOINABLE;
            }

            if (trackSegmentInfo & TRACK_FLAG_8000000) {
                if (hasFlags) {
                    write << " | ";
                }
                write << "TRACK_FLAG_8000000";
                hasFlags = true;
                trackSegmentInfo &= ~TRACK_FLAG_8000000;
            }

            switch (trackSegmentInfo & TRACK_FORM_MASK) {
                case TRACK_FORM_STRAIGHT:
                    if (hasFlags) {
                        write << " | ";
                    }
                    write << "TRACK_FORM_STRAIGHT";
                    break;
                case TRACK_FORM_LEFT:
                    if (hasFlags) {
                        write << " | ";
                    }
                    write << "TRACK_FORM_LEFT";
                    break;
                case TRACK_FORM_RIGHT:
                    if (hasFlags) {
                        write << " | ";
                    }
                    write << "TRACK_FORM_RIGHT";
                    break;
                case TRACK_FORM_S:
                    if (hasFlags) {
                        write << " | ";
                    }
                    write << "TRACK_FORM_S";
                    break;
                case TRACK_FORM_S_FLIPPED:
                    if (hasFlags) {
                        write << " | ";
                    }
                    write << "TRACK_FORM_S_FLIPPED";
                    break;
            }
            trackSegmentInfo &= ~TRACK_FORM_MASK;

            switch (trackSegmentInfo & TRACK_SHAPE_MASK) {
                case TRACK_SHAPE_ROAD:
                    if (hasFlags) {
                        write << " | ";
                    }
                    write << "TRACK_SHAPE_ROAD";
                    trackSegmentInfo &= ~TRACK_SHAPE_MASK;
                    if ((trackSegmentInfo & TRACK_TYPE_MASK) == TRACK_TYPE_NONE) {
                        write << " | TRACK_TYPE_NONE";
                    } else {
                        write << " | " << static_cast<Road>(trackSegmentInfo & TRACK_TYPE_MASK);
                    }
                    break;
                case TRACK_SHAPE_WALLED_ROAD:
                    if (hasFlags) {
                        write << " | ";
                    }
                    write << "TRACK_SHAPE_WALLED_ROAD";
                    trackSegmentInfo &= ~TRACK_SHAPE_MASK;
                    if ((trackSegmentInfo & TRACK_TYPE_MASK) == TRACK_TYPE_NONE) {
                        write << " | TRACK_TYPE_NONE";
                    } else {
                        write << " | " << static_cast<WalledRoad>(trackSegmentInfo & TRACK_TYPE_MASK);
                    }
                    break;
                case TRACK_SHAPE_PIPE:
                    if (hasFlags) {
                        write << " | ";
                    }
                    write << "TRACK_SHAPE_PIPE";
                    trackSegmentInfo &= ~TRACK_SHAPE_MASK;
                    if ((trackSegmentInfo & TRACK_TYPE_MASK) == TRACK_TYPE_NONE) {
                        write << " | TRACK_TYPE_NONE";
                    } else {
                        write << " | " << static_cast<Pipe>(trackSegmentInfo & TRACK_TYPE_MASK);
                    }
                    break;
                case TRACK_SHAPE_CYLINDER:
                    if (hasFlags) {
                        write << " | ";
                    }
                    write << "TRACK_SHAPE_CYLINDER";
                    trackSegmentInfo &= ~TRACK_SHAPE_MASK;
                    if ((trackSegmentInfo & TRACK_TYPE_MASK) == TRACK_TYPE_NONE) {
                        write << " | TRACK_TYPE_NONE";
                    } else {
                        write << " | " << static_cast<Cylinder>(trackSegmentInfo & TRACK_TYPE_MASK);
                    }
                    break;
                case TRACK_SHAPE_HALF_PIPE:
                    if (hasFlags) {
                        write << " | ";
                    }
                    write << "TRACK_SHAPE_HALF_PIPE";
                    trackSegmentInfo &= ~TRACK_SHAPE_MASK;
                    if ((trackSegmentInfo & TRACK_TYPE_MASK) == TRACK_TYPE_NONE) {
                        write << " | TRACK_TYPE_NONE";
                    } else {
                        write << " | " << static_cast<HalfPipe>(trackSegmentInfo & TRACK_TYPE_MASK);
                    }
                    break;
                case TRACK_SHAPE_TUNNEL:
                    if (hasFlags) {
                        write << " | ";
                    }
                    write << "TRACK_SHAPE_TUNNEL";
                    trackSegmentInfo &= ~TRACK_SHAPE_MASK;
                    if ((trackSegmentInfo & TRACK_TYPE_MASK) == TRACK_TYPE_NONE) {
                        write << " | TRACK_TYPE_NONE";
                    } else {
                        write << " | " << static_cast<Tunnel>(trackSegmentInfo & TRACK_TYPE_MASK);
                    }
                    break;
                case TRACK_SHAPE_AIR:
                    if (hasFlags) {
                        write << " | ";
                    }
                    write << "TRACK_SHAPE_AIR";
                    trackSegmentInfo &= ~TRACK_SHAPE_MASK;
                    if ((trackSegmentInfo & TRACK_TYPE_MASK) == TRACK_TYPE_NONE) {
                        write << " | TRACK_TYPE_NONE";
                    }
                    break;
                case TRACK_SHAPE_BORDERLESS_ROAD:
                    if (hasFlags) {
                        write << " | ";
                    }
                    write << "TRACK_SHAPE_BORDERLESS_ROAD";
                    trackSegmentInfo &= ~TRACK_SHAPE_MASK;
                    if ((trackSegmentInfo & TRACK_TYPE_MASK) == TRACK_TYPE_NONE) {
                        write << " | TRACK_TYPE_NONE";
                    } else {
                        write << " | " << static_cast<BorderlessRoad>(trackSegmentInfo & TRACK_TYPE_MASK);
                    }
                    break;
            }
            trackSegmentInfo &= ~TRACK_TYPE_MASK;

            if (trackSegmentInfo != 0) {
                SPDLOG_WARN("Invalid Track Segment Information Found: 0x{:X}", trackSegmentInfo);
            }

            write << " },\n";

        } else {
            write << "{ 0 },\n";
        }
    }
    write << fourSpaceTab << "},\n";

    write << fourSpaceTab << "{ /* Bank Angle */";
    for (size_t i = 0; i < 64; i++) {
        if ((i % 16) == 0) {
            write << "\n" << fourSpaceTab << fourSpaceTab;
        }

        if (i < controlPointCount) {
            const ControlPointInfo& controlPointInfo = course->mControlPointInfos.at(i);
            write << controlPointInfo.bankAngle << ", ";
        } else {
            write << "0, ";
        }
    }
    write << "\n" << fourSpaceTab << "},\n";

    write << fourSpaceTab << "{ /* Pit */";
    for (size_t i = 0; i < 64; i++) {
        if ((i % CP_ENUMS_PER_LINE) == 0) {
            write << "\n" << fourSpaceTab << fourSpaceTab;
        }

        if (i < controlPointCount) {
            const ControlPointInfo& controlPointInfo = course->mControlPointInfos.at(i);
            write << static_cast<PitZone>(controlPointInfo.pit) << ", ";
        } else {
            write << "0, ";
        }
    }
    write << "\n" << fourSpaceTab << "},\n";

    write << fourSpaceTab << "{ /* Dash */";
    for (size_t i = 0; i < 64; i++) {
        if ((i % CP_ENUMS_PER_LINE) == 0) {
            write << "\n" << fourSpaceTab << fourSpaceTab;
        }

        if (i < controlPointCount) {
            const ControlPointInfo& controlPointInfo = course->mControlPointInfos.at(i);
            write << static_cast<DashZone>(controlPointInfo.dash) << ", ";
        } else {
            write << "0, ";
        }
    }
    write << "\n" << fourSpaceTab << "},\n";

    write << fourSpaceTab << "{ /* Dirt */";
    for (size_t i = 0; i < 64; i++) {
        if ((i % CP_ENUMS_PER_LINE) == 0) {
            write << "\n" << fourSpaceTab << fourSpaceTab;
        }

        if (i < controlPointCount) {
            const ControlPointInfo& controlPointInfo = course->mControlPointInfos.at(i);
            write << static_cast<Dirt>(controlPointInfo.dirt) << ", ";
        } else {
            write << "0, ";
        }
    }
    write << "\n" << fourSpaceTab << "},\n";

    write << fourSpaceTab << "{ /* Ice */";
    for (size_t i = 0; i < 64; i++) {
        if ((i % CP_ENUMS_PER_LINE) == 0) {
            write << "\n" << fourSpaceTab << fourSpaceTab;
        }

        if (i < controlPointCount) {
            const ControlPointInfo& controlPointInfo = course->mControlPointInfos.at(i);
            write << static_cast<Ice>(controlPointInfo.ice) << ", ";
        } else {
            write << "0, ";
        }
    }
    write << "\n" << fourSpaceTab << "},\n";

    write << fourSpaceTab << "{ /* Jump */";
    for (size_t i = 0; i < 64; i++) {
        if ((i % CP_ENUMS_PER_LINE) == 0) {
            write << "\n" << fourSpaceTab << fourSpaceTab;
        }

        if (i < controlPointCount) {
            const ControlPointInfo& controlPointInfo = course->mControlPointInfos.at(i);
            write << static_cast<Jump>(controlPointInfo.jump) << ", ";
        } else {
            write << "0, ";
        }
    }
    write << "\n" << fourSpaceTab << "},\n";

    write << fourSpaceTab << "{ /* Landmine */";
    for (size_t i = 0; i < 64; i++) {
        if ((i % CP_ENUMS_PER_LINE) == 0) {
            write << "\n" << fourSpaceTab << fourSpaceTab;
        }

        if (i < controlPointCount) {
            const ControlPointInfo& controlPointInfo = course->mControlPointInfos.at(i);
            write << static_cast<Landmine>(controlPointInfo.landmine) << ", ";
        } else {
            write << "0, ";
        }
    }
    write << "\n" << fourSpaceTab << "},\n";

    write << fourSpaceTab << "{ /* Gate */";
    for (size_t i = 0; i < 64; i++) {
        if ((i % CP_ENUMS_PER_LINE) == 0) {
            write << "\n" << fourSpaceTab << fourSpaceTab;
        }

        if (i < controlPointCount) {
            const ControlPointInfo& controlPointInfo = course->mControlPointInfos.at(i);
            write << static_cast<Gate>(controlPointInfo.gate) << ", ";
        } else {
            write << "0, ";
        }
    }
    write << "\n" << fourSpaceTab << "},\n";

    write << fourSpaceTab << "{ /* Building */";
    for (size_t i = 0; i < 64; i++) {
        if ((i % CP_ENUMS_PER_LINE) == 0) {
            write << "\n" << fourSpaceTab << fourSpaceTab;
        }

        if (i < controlPointCount) {
            const ControlPointInfo& controlPointInfo = course->mControlPointInfos.at(i);
            write << static_cast<Building>(controlPointInfo.building) << ", ";
        } else {
            write << "0, ";
        }
    }
    write << "\n" << fourSpaceTab << "},\n";

    write << fourSpaceTab << "{ /* Sign */";
    for (size_t i = 0; i < 64; i++) {
        if ((i % CP_ENUMS_PER_LINE) == 0) {
            write << "\n" << fourSpaceTab << fourSpaceTab;
        }

        if (i < controlPointCount) {
            const ControlPointInfo& controlPointInfo = course->mControlPointInfos.at(i);
            write << static_cast<Sign>(controlPointInfo.sign) << ", ";
        } else {
            write << "0, ";
        }
    }
    write << "\n" << fourSpaceTab << "}\n";
    write << "};\n\n";

    return offset + sizeof(CourseRawData);
}

ExportResult FZX::CourseBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    auto writer = LUS::BinaryWriter();
    const auto course = std::static_pointer_cast<CourseData>(raw);
    int8_t controlPointCount = (int8_t)course->mControlPointInfos.size();

    WriteHeader(writer, Torch::ResourceType::CourseData, 0);

    writer.Write(course->mCreatorId);
    writer.Write(controlPointCount);
    writer.Write(course->mVenue);
    writer.Write(course->mSkybox);
    writer.Write(course->CalculateChecksum());
    writer.Write(course->mFlag);
    writer.Write(course->mFileName);
    writer.Write((uint32_t)course->mFileNameExtra.size());
    for (const auto extra : course->mFileNameExtra) {
        writer.Write(extra);
    }
    for (const auto& controlPointInfo : course->mControlPointInfos) {
        writer.Write(controlPointInfo.controlPoint.pos.x);
        writer.Write(controlPointInfo.controlPoint.pos.y);
        writer.Write(controlPointInfo.controlPoint.pos.z);
        writer.Write(controlPointInfo.controlPoint.radiusLeft);
        writer.Write(controlPointInfo.controlPoint.radiusRight);
        writer.Write(controlPointInfo.controlPoint.trackSegmentInfo);
        writer.Write(controlPointInfo.bankAngle);
        writer.Write(controlPointInfo.pit);
        writer.Write(controlPointInfo.dash);
        writer.Write(controlPointInfo.dirt);
        writer.Write(controlPointInfo.ice);
        writer.Write(controlPointInfo.jump);
        writer.Write(controlPointInfo.landmine);
        writer.Write(controlPointInfo.gate);
        writer.Write(controlPointInfo.building);
        writer.Write(controlPointInfo.sign);
    }

    writer.Finish(write);
    return std::nullopt;
}

ExportResult FZX::CourseModdingExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto course = std::static_pointer_cast<CourseData>(raw);
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    *replacement += ".yaml";

    std::stringstream stream;
    YAML::Emitter out;

    out << YAML::BeginMap;
    out << YAML::Key << symbol;
    out << YAML::Value;
    out.SetIndent(2);
    out << YAML::BeginMap;
    out << YAML::Key << "CreatorId";
    out << YAML::Value << course->mCreatorId;
    out << YAML::Key << "Venue";
    out << YAML::Value << course->mVenue;
    out << YAML::Key << "Skybox";
    out << YAML::Value << course->mSkybox;
    out << YAML::Key << "Flag";
    out << YAML::Value << course->mFlag;
    out << YAML::Key << "Name";
    out << YAML::Value << course->mFileName;
    if (course->mFileNameExtra.size() > 1) {
        out << YAML::Key << "NameExtra";
        out << YAML::Value << YAML::Flow << YAML::BeginSeq;
        for (size_t i = 0; i < course->mFileNameExtra.size() - 1; i++) {
            out << YAML::Hex << (uint32_t)(uint8_t)course->mFileNameExtra.at(i);
        }
        out << YAML::EndSeq << YAML::Block << YAML::Dec;
    }

    out << YAML::Key << "ControlPoints";
    out << YAML::Value << YAML::BeginSeq;
    for (const auto& controlPointInfo : course->mControlPointInfos) {
        out << YAML::BeginMap;
        out << YAML::Key << "PosX";
        out << YAML::Value << controlPointInfo.controlPoint.pos.x;
        out << YAML::Key << "PosY";
        out << YAML::Value << controlPointInfo.controlPoint.pos.y;
        out << YAML::Key << "PosZ";
        out << YAML::Value << controlPointInfo.controlPoint.pos.z;
        out << YAML::Key << "RadiusLeft";
        out << YAML::Value << controlPointInfo.controlPoint.radiusLeft;
        out << YAML::Key << "RadiusRight";
        out << YAML::Value << controlPointInfo.controlPoint.radiusRight;
        out << YAML::Key << "TrackSegmentInfo";
        out << YAML::Value << YAML::Hex << controlPointInfo.controlPoint.trackSegmentInfo << YAML::Dec;

        // Default to 0
        if (controlPointInfo.bankAngle != 0) {
            out << YAML::Key << "BankAngle";
            out << YAML::Value << controlPointInfo.bankAngle;
        }

        // Remaining Default to -1

        if (controlPointInfo.pit != -1) {
            out << YAML::Key << "Pit";
            out << YAML::Value << controlPointInfo.pit;
        }

        if (controlPointInfo.dash != -1) {
            out << YAML::Key << "Dash";
            out << YAML::Value << controlPointInfo.dash;
        }

        if (controlPointInfo.dirt != -1) {
            out << YAML::Key << "Dirt";
            out << YAML::Value << controlPointInfo.dirt;
        }

        if (controlPointInfo.ice != -1) {
            out << YAML::Key << "Ice";
            out << YAML::Value << controlPointInfo.ice;
        }

        if (controlPointInfo.jump != -1) {
            out << YAML::Key << "Jump";
            out << YAML::Value << controlPointInfo.jump;
        }

        if (controlPointInfo.landmine != -1) {
            out << YAML::Key << "Landmine";
            out << YAML::Value << controlPointInfo.landmine;
        }

        if (controlPointInfo.gate != -1) {
            out << YAML::Key << "Gate";
            out << YAML::Value << controlPointInfo.gate;
        }

        if (controlPointInfo.building != -1) {
            out << YAML::Key << "Building";
            out << YAML::Value << controlPointInfo.building;
        }

        if (controlPointInfo.sign != -1) {
            out << YAML::Key << "Sign";
            out << YAML::Value << controlPointInfo.sign;
        }
        out << YAML::EndMap;
    }

    out << YAML::EndMap;
    out << YAML::EndMap;

    write.write(out.c_str(), out.size());

    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> FZX::CourseFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    LUS::BinaryReader reader(segment.data, segment.size);

    reader.SetEndianness(Torch::Endianness::Big);

    int8_t creatorId = reader.ReadInt8();
    int8_t controlPointCount = reader.ReadInt8();
    int8_t venue = reader.ReadInt8();
    int8_t skybox = reader.ReadInt8();
    uint32_t checksum = reader.ReadUInt32();
    int8_t flag = reader.ReadInt8();
    char nameBuffer[23];

    for (int32_t i = 0; i < ARRAY_COUNT(nameBuffer); i++) {
        nameBuffer[i] = reader.ReadInt8();
    }

    std::string fileName(nameBuffer);
    std::vector<int8_t> fileNameExtra;
    for (int32_t i = fileName.length(); i < ARRAY_COUNT(nameBuffer); i++) {
        fileNameExtra.push_back(nameBuffer[i]);
    }

    std::vector<ControlPointInfo> controlPointInfos;

    for (int8_t i = 0; i < controlPointCount; i++) {
        ControlPointInfo controlPointInfo;

        reader.Seek(0x020 + i * sizeof(ControlPoint), LUS::SeekOffsetType::Start);
        controlPointInfo.controlPoint.pos.x = reader.ReadFloat();
        controlPointInfo.controlPoint.pos.y = reader.ReadFloat();
        controlPointInfo.controlPoint.pos.z = reader.ReadFloat();
        controlPointInfo.controlPoint.radiusLeft = reader.ReadInt16();
        controlPointInfo.controlPoint.radiusRight = reader.ReadInt16();
        controlPointInfo.controlPoint.trackSegmentInfo = reader.ReadInt32();
        reader.Seek(0x520 + i * sizeof(int16_t), LUS::SeekOffsetType::Start);
        controlPointInfo.bankAngle = reader.ReadInt16();
        reader.Seek(0x5A0 + i * sizeof(int8_t), LUS::SeekOffsetType::Start);
        controlPointInfo.pit = reader.ReadInt8();
        reader.Seek(0x5E0 + i * sizeof(int8_t), LUS::SeekOffsetType::Start);
        controlPointInfo.dash = reader.ReadInt8();
        reader.Seek(0x620 + i * sizeof(int8_t), LUS::SeekOffsetType::Start);
        controlPointInfo.dirt = reader.ReadInt8();
        reader.Seek(0x660 + i * sizeof(int8_t), LUS::SeekOffsetType::Start);
        controlPointInfo.ice = reader.ReadInt8();
        reader.Seek(0x6A0 + i * sizeof(int8_t), LUS::SeekOffsetType::Start);
        controlPointInfo.jump = reader.ReadInt8();
        reader.Seek(0x6E0 + i * sizeof(int8_t), LUS::SeekOffsetType::Start);
        controlPointInfo.landmine = reader.ReadInt8();
        reader.Seek(0x720 + i * sizeof(int8_t), LUS::SeekOffsetType::Start);
        controlPointInfo.gate = reader.ReadInt8();
        reader.Seek(0x760 + i * sizeof(int8_t), LUS::SeekOffsetType::Start);
        controlPointInfo.building = reader.ReadInt8();
        reader.Seek(0x7A0 + i * sizeof(int8_t), LUS::SeekOffsetType::Start);
        controlPointInfo.sign = reader.ReadInt8();
        controlPointInfos.push_back(controlPointInfo);
    }

    return std::make_shared<CourseData>(creatorId, venue, skybox, flag, fileName, fileNameExtra, controlPointInfos);
}

std::optional<std::shared_ptr<IParsedData>> FZX::CourseFactory::parse_modding(std::vector<uint8_t>& buffer, YAML::Node& node) {
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

    int8_t creatorId = info["CreatorId"].as<int8_t>();
    int8_t venue = info["Venue"].as<int8_t>();
    int8_t skybox = info["Skybox"].as<int8_t>();
    int8_t flag = info["Flag"].as<int8_t>();
    std::string fileName = info["Name"].as<std::string>();
    std::vector<int8_t> fileNameExtra;
    if (info["NameExtra"]) {
        auto nameExtra = info["NameExtra"];
        for (YAML::iterator it = nameExtra.begin(); it != nameExtra.end(); ++it) {
            fileNameExtra.push_back((int8_t)(*it).as<uint8_t>());
        }
    } 
    std::vector<ControlPointInfo> controlPointInfos;

    auto controlPoints = info["ControlPoints"];
    for (YAML::iterator it = controlPoints.begin(); it != controlPoints.end(); ++it) {
        const YAML::Node& controlPointNode = *it;
        ControlPointInfo controlPointInfo;
        controlPointInfo.controlPoint.pos.x = controlPointNode["PosX"].as<float>();
        controlPointInfo.controlPoint.pos.y = controlPointNode["PosY"].as<float>();
        controlPointInfo.controlPoint.pos.z = controlPointNode["PosZ"].as<float>();
        controlPointInfo.controlPoint.radiusLeft = controlPointNode["RadiusLeft"].as<int16_t>();
        controlPointInfo.controlPoint.radiusRight = controlPointNode["RadiusRight"].as<int16_t>();
        controlPointInfo.controlPoint.trackSegmentInfo = controlPointNode["TrackSegmentInfo"].as<int32_t>();
        if (controlPointNode["BankAngle"]) {
            controlPointInfo.bankAngle = controlPointNode["BankAngle"].as<int16_t>();
        } else {
            controlPointInfo.bankAngle = 0;
        }
        if (controlPointNode["Pit"]) {
            controlPointInfo.pit = controlPointNode["Pit"].as<int8_t>();
        } else {
            controlPointInfo.pit = -1;
        }

        if (controlPointNode["Dash"]) {
            controlPointInfo.dash = controlPointNode["Dash"].as<int8_t>();
        } else {
            controlPointInfo.dash = -1;
        }

        if (controlPointNode["Dirt"]) {
            controlPointInfo.dirt = controlPointNode["Dirt"].as<int8_t>();
        } else {
            controlPointInfo.dirt = -1;
        }

        if (controlPointNode["Ice"]) {
            controlPointInfo.ice = controlPointNode["Ice"].as<int8_t>();
        } else {
            controlPointInfo.ice = -1;
        }

        if (controlPointNode["Jump"]) {
            controlPointInfo.jump = controlPointNode["Jump"].as<int8_t>();
        } else {
            controlPointInfo.jump = -1;
        }

        if (controlPointNode["Landmine"]) {
            controlPointInfo.landmine = controlPointNode["Landmine"].as<int8_t>();
        } else {
            controlPointInfo.landmine = -1;
        }

        if (controlPointNode["Gate"]) {
            controlPointInfo.gate = controlPointNode["Gate"].as<int8_t>();
        } else {
            controlPointInfo.gate = -1;
        }

        if (controlPointNode["Building"]) {
            controlPointInfo.building = controlPointNode["Building"].as<int8_t>();
        } else {
            controlPointInfo.building = -1;
        }

        if (controlPointNode["Sign"]) {
            controlPointInfo.sign = controlPointNode["Sign"].as<int8_t>();
        } else {
            controlPointInfo.sign = -1;
        }
        controlPointInfos.push_back(controlPointInfo);
    }

    return std::make_shared<CourseData>(creatorId, venue, skybox, flag, fileName, fileNameExtra, controlPointInfos);
}
