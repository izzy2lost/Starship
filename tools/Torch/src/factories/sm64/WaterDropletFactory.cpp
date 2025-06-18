#include "WaterDropletFactory.h"
#include "spdlog/spdlog.h"

#include "Companion.h"
#include "utils/Decompressor.h"
#include "utils/TorchUtils.h"
#include <regex>

#define FORMAT_FLOAT(x) std::fixed << std::setprecision(1) << x << "f"

ExportResult SM64::WaterDropletHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern struct WaterDropletParams " << symbol << "[];\n";
    return std::nullopt;
}

ExportResult SM64::WaterDropletCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");

    auto waterDropletData = std::static_pointer_cast<SM64::WaterDropletData>(raw);

    write << "struct WaterDropletParams " << symbol << " = {\n";

    std::stringstream flagData;
    auto flags = waterDropletData->flags;
    bool prevFlag = false;
    if (waterDropletData->flags & WATER_DROPLET_FLAG_RAND_ANGLE) {
        flags -= WATER_DROPLET_FLAG_RAND_ANGLE;
        flagData << "WATER_DROPLET_FLAG_RAND_ANGLE";
        prevFlag = true;
    }
    if (waterDropletData->flags & WATER_DROPLET_FLAG_RAND_OFFSET_XZ) {
        flags -= WATER_DROPLET_FLAG_RAND_OFFSET_XZ;
        if (prevFlag) {
            flagData << " | ";
        }
        flagData << "WATER_DROPLET_FLAG_RAND_OFFSET_XZ";
        prevFlag = true;
    }
    if (waterDropletData->flags & WATER_DROPLET_FLAG_RAND_OFFSET_XYZ) {
        flags -= WATER_DROPLET_FLAG_RAND_OFFSET_XYZ;
        if (prevFlag) {
            flagData << " | ";
        }
        flagData << "WATER_DROPLET_FLAG_RAND_OFFSET_XYZ";
        prevFlag = true;
    }
    if (waterDropletData->flags & WATER_DROPLET_FLAG_SET_Y_TO_WATER_LEVEL) {
        flags -= WATER_DROPLET_FLAG_SET_Y_TO_WATER_LEVEL;
        if (prevFlag) {
            flagData << " | ";
        }
        flagData << "WATER_DROPLET_FLAG_SET_Y_TO_WATER_LEVEL";
        prevFlag = true;
    }
    if (waterDropletData->flags & WATER_DROPLET_FLAG_RAND_ANGLE_INCR_PLUS_8000) {
        flags -= WATER_DROPLET_FLAG_RAND_ANGLE_INCR_PLUS_8000;
        if (prevFlag) {
            flagData << " | ";
        }
        flagData << "WATER_DROPLET_FLAG_RAND_ANGLE_INCR_PLUS_8000";
        prevFlag = true;
    }
    if (waterDropletData->flags & WATER_DROPLET_FLAG_RAND_ANGLE_INCR) {
        flags -= WATER_DROPLET_FLAG_RAND_ANGLE_INCR;
        if (prevFlag) {
            flagData << " | ";
        }
        flagData << "WATER_DROPLET_FLAG_RAND_ANGLE_INCR";
    }

    if (flags != 0) {
        throw std::runtime_error("WaterDropletCodeExporter: Invalid flags field");
    }

    std::stringstream model;
    if (waterDropletData->model == MODEL_FISH) {
        model << "MODEL_FISH";
    } else if (waterDropletData->model == MODEL_WHITE_PARTICLE_SMALL) {
        model << "MODEL_WHITE_PARTICLE_SMALL";
    } else {
        model << waterDropletData->model;
    }
    std::stringstream bhvSymbol;
    auto dec = Companion::Instance->GetNodeByAddr(waterDropletData->behavior);
    if (dec.has_value()) {
        auto bhvNode = std::get<1>(dec.value());
        bhvSymbol << GetSafeNode<std::string>(bhvNode, "symbol");
    } else {
        SPDLOG_WARN("Cannot find node for ptr 0x{:X}", waterDropletData->behavior);
        bhvSymbol << std::hex << "0x" << waterDropletData->behavior << std::dec;
    }

    write << fourSpaceTab << "/* Flags */ " << flagData.str() << ",\n";
    write << fourSpaceTab << "/* Model */ " << model.str() << ",\n";
    write << fourSpaceTab << "/* Behavior */ " << bhvSymbol.str() << ",\n";
    write << fourSpaceTab << "/* Move angle range */ " << std::hex << "0x" << waterDropletData->moveAngleRange << std::dec << ",\n";
    write << fourSpaceTab << "/* Unused (flag-specific) */ " << waterDropletData->moveRange << ",\n";
    write << fourSpaceTab << "/* Random fvel offset, scale */ " << FORMAT_FLOAT(waterDropletData->randForwardVelOffset) << ", " << FORMAT_FLOAT(waterDropletData->randForwardVelScale) << ",\n";
    write << fourSpaceTab << "/* Random yvel offset, scale */ " << FORMAT_FLOAT(waterDropletData->randYVelOffset) << ", " << FORMAT_FLOAT(waterDropletData->randYVelScale) << ",\n";
    write << fourSpaceTab << "/* Random size offset, scale */ " << FORMAT_FLOAT(waterDropletData->randSizeOffset) << ", " << FORMAT_FLOAT(waterDropletData->randSizeScale) << ",\n";

    write << "};\n";

    return offset + 36;
}

ExportResult SM64::WaterDropletBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto waterDropletData = std::static_pointer_cast<SM64::WaterDropletData>(raw);

    WriteHeader(writer, Torch::ResourceType::WaterDroplet, 0);

    writer.Write(waterDropletData->flags);
    writer.Write(waterDropletData->model);
    auto ptr = waterDropletData->behavior;
    auto dec = Companion::Instance->GetNodeByAddr(ptr);
    if (dec.has_value()) {
        uint64_t hash = CRC64(std::get<0>(dec.value()).c_str());
        SPDLOG_INFO("Found Behavior Script: 0x{:X} Hash: 0x{:X} Path: {}", ptr, hash, std::get<0>(dec.value()));
        writer.Write(hash);
    } else {
        SPDLOG_WARN("Could not find Behavior Script at 0x{:X}", ptr);
    }
    writer.Write(waterDropletData->moveAngleRange);
    writer.Write(waterDropletData->moveRange);
    writer.Write(waterDropletData->randForwardVelOffset);
    writer.Write(waterDropletData->randForwardVelScale);
    writer.Write(waterDropletData->randYVelOffset);
    writer.Write(waterDropletData->randYVelScale);
    writer.Write(waterDropletData->randSizeOffset);
    writer.Write(waterDropletData->randSizeScale);

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> SM64::WaterDropletFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    LUS::BinaryReader reader(segment.data, segment.size);
    reader.SetEndianness(Torch::Endianness::Big);

    auto flags = reader.ReadInt16();
    auto model = reader.ReadInt16();
    auto behavior = reader.ReadUInt32();
    auto moveAngleRange = reader.ReadInt16();
    auto moveRange = reader.ReadInt16();
    auto randForwardVelOffset = reader.ReadFloat();
    auto randForwardVelScale = reader.ReadFloat();
    auto randYVelOffset = reader.ReadFloat();
    auto randYVelScale = reader.ReadFloat();
    auto randSizeOffset = reader.ReadFloat();
    auto randSizeScale = reader.ReadFloat();

    // For autogen if desired
    // YAML::Node bhvNode;
    // bhvNode["type"] = "SM64:BEHAVIOR_SCRIPT";
    // bhvNode["offset"] = behavior;
    // Companion::Instance->AddAsset(bhvNode);

    return std::make_shared<SM64::WaterDropletData>(flags, model, behavior, moveAngleRange, moveRange, randForwardVelOffset, randForwardVelScale, randYVelOffset, randYVelScale, randSizeOffset, randSizeScale);
}
