#include "PaintingFactory.h"
#include "spdlog/spdlog.h"

#include "Companion.h"
#include "utils/Decompressor.h"
#include "utils/TorchUtils.h"
#include <regex>

ExportResult SM64::PaintingHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern struct Painting " << symbol << "[];\n";
    return std::nullopt;
}

ExportResult SM64::PaintingCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");

    auto painting = std::static_pointer_cast<SM64::Painting>(raw);

    write << "struct Painting " << symbol << " = {\n";

    std::stringstream textureType;

    if (painting->textureType == PAINTING_IMAGE) {
        textureType << "PAINTING_IMAGE";
    } else if (painting->textureType == PAINTING_ENV_MAP) {
        textureType << "PAINTING_ENV_MAP";
    } else {
        textureType << painting->textureType; // should not reach this case, throw error?
    }

    std::stringstream nDLSymbol;
    {
        auto dec = Companion::Instance->GetNodeByAddr(painting->normalDisplayList);
        if (dec.has_value()) {
            auto nDLNode = std::get<1>(dec.value());
            nDLSymbol << GetSafeNode<std::string>(nDLNode, "symbol");
        } else {
            SPDLOG_WARN("Cannot find node for ptr 0x{:X}", painting->normalDisplayList);
            nDLSymbol << std::hex << "0x" << painting->normalDisplayList << std::dec;
        }
    }

    std::stringstream tMapSymbol;
    {
        auto dec = Companion::Instance->GetNodeByAddr(painting->textureMaps);
        if (dec.has_value()) {
            auto tMapNode = std::get<1>(dec.value());
            tMapSymbol << GetSafeNode<std::string>(tMapNode, "symbol");
        } else {
            SPDLOG_WARN("Cannot find node for ptr 0x{:X}", painting->textureMaps);
            tMapSymbol << std::hex << "0x" << painting->textureMaps << std::dec;
        }
    }

    std::stringstream tArrSymbol;
    {
        auto dec = Companion::Instance->GetNodeByAddr(painting->textureArray);
        if (dec.has_value()) {
            auto tMapNode = std::get<1>(dec.value());
            tArrSymbol << GetSafeNode<std::string>(tMapNode, "symbol");
        } else {
            SPDLOG_WARN("Cannot find node for ptr 0x{:X}", painting->textureArray);
            tArrSymbol << std::hex << "0x" << painting->textureArray << std::dec;
        }
    }

    std::stringstream rDLSymbol;
    {
        auto dec = Companion::Instance->GetNodeByAddr(painting->rippleDisplayList);
        if (dec.has_value()) {
            auto rDLNode = std::get<1>(dec.value());
            rDLSymbol << GetSafeNode<std::string>(rDLNode, "symbol");
        } else {
            SPDLOG_WARN("Cannot find node for ptr 0x{:X}", painting->rippleDisplayList);
            rDLSymbol << std::hex << "0x" << painting->rippleDisplayList << std::dec;
        }
    }

    std::stringstream rippleTrigger;
    if (painting->rippleTrigger == RIPPLE_TRIGGER_PROXIMITY) {
        rippleTrigger << "RIPPLE_TRIGGER_PROXIMITY";
    } else if (painting->rippleTrigger == RIPPLE_TRIGGER_CONTINUOUS) {
        rippleTrigger << "RIPPLE_TRIGGER_CONTINUOUS";
    } else {
        rippleTrigger << painting->rippleTrigger; // should not reach this case, throw error?
    }

    write << fourSpaceTab << "/* id */ " << std::hex << "0x" << painting->id << ",\n";
    write << fourSpaceTab << "/* Image Count */ " << std::hex << "0x" << (uint32_t)painting->imageCount << ",\n";
    write << fourSpaceTab << "/* Texture Type */ " << textureType.str() << ",\n";
    write << fourSpaceTab << "/* Floor Status */ " << std::hex << "0x" << (uint32_t)painting->lastFloor << ", " << std::hex << "0x" << (uint32_t)painting->currFloor << ", " << std::hex << "0x" << (uint32_t)painting->floorEntered << ",\n";
    write << fourSpaceTab << "/* Ripple Status */ " << (uint32_t)painting->state << ",\n";
    write << fourSpaceTab << "/* Rotation */ " << painting->pitch << ", " << painting->yaw << ",\n";
    write << fourSpaceTab << "/* Position */ " << painting->posX << ", " << painting->posY << ", " << painting->posZ << ",\n";
    write << fourSpaceTab << "/* Ripple Magnitude */ " << painting->currRippleMag << ", " << painting->passiveRippleMag << ", " << painting->entryRippleMag << ",\n";
    write << fourSpaceTab << "/* Ripple Decay */ " << painting->rippleDecay << ", " << painting->passiveRippleDecay << ", " << painting->entryRippleDecay << ",\n";
    write << fourSpaceTab << "/* Ripple Rate */ " << painting->currRippleRate << ", " << painting->passiveRippleRate << ", " << painting->entryRippleRate << ",\n";
    write << fourSpaceTab << "/* Ripple Dispersion */ " << painting->dispersionFactor << ", " << painting->passiveDispersionFactor << ", " << painting->entryDispersionFactor << ",\n";
    write << fourSpaceTab << "/* Curr Ripple Timer */ " << painting->rippleTimer << ",\n";
    write << fourSpaceTab << "/* Curr Ripple x, y */ " << painting->rippleX << ", " << painting->rippleY << ",\n";
    write << fourSpaceTab << "/* Normal DList */ " << nDLSymbol.str() << ",\n";
    write << fourSpaceTab << "/* Texture Maps */ " << tMapSymbol.str() << ",\n";
    write << fourSpaceTab << "/* Textures */ " << tArrSymbol.str() << ",\n";
    write << fourSpaceTab << "/* Texture w, h */ " << std::dec << painting->textureWidth << ", " << painting->textureHeight << ",\n";
    write << fourSpaceTab << "/* Ripple DList */ " << rDLSymbol.str() << ",\n";
    write << fourSpaceTab << "/* Ripple Trigger */ " << rippleTrigger.str() << ",\n";
    write << fourSpaceTab << "/* Alpha */ " << std::dec << (uint32_t)painting->alpha << ",\n";
    write << fourSpaceTab << "/* Mario Below */ " << std::hex << "0x" << (uint32_t)painting->marioWasUnder << ", " << std::hex << "0x" << (uint32_t)painting->marioIsUnder << ", " << std::hex << "0x" << (uint32_t)painting->marioWentUnder << ",\n";
    write << fourSpaceTab << "/* Size */ " << painting->size << ",\n";

    write << "};\n";

    return offset + 120;
}

ExportResult SM64::PaintingBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto painting = std::static_pointer_cast<SM64::Painting>(raw);
    uint32_t ptr;

    WriteHeader(writer, Torch::ResourceType::Painting, 0);

    writer.Write(painting->id);
    writer.Write(painting->imageCount);
    writer.Write(painting->textureType);
    writer.Write(painting->lastFloor);
    writer.Write(painting->currFloor);
    writer.Write(painting->floorEntered);
    writer.Write(painting->state);
    writer.Write(painting->pitch);
    writer.Write(painting->yaw);
    writer.Write(painting->posX);
    writer.Write(painting->posY);
    writer.Write(painting->posZ);
    writer.Write(painting->currRippleMag);
    writer.Write(painting->passiveRippleMag);
    writer.Write(painting->entryRippleMag);
    writer.Write(painting->rippleDecay);
    writer.Write(painting->passiveRippleDecay);
    writer.Write(painting->entryRippleDecay);
    writer.Write(painting->currRippleRate);
    writer.Write(painting->passiveRippleRate);
    writer.Write(painting->entryRippleRate);
    writer.Write(painting->dispersionFactor);
    writer.Write(painting->passiveDispersionFactor);
    writer.Write(painting->entryDispersionFactor);
    writer.Write(painting->rippleTimer);
    writer.Write(painting->rippleX);
    writer.Write(painting->rippleY);

    ptr = painting->normalDisplayList;
    {
        auto dec = Companion::Instance->GetNodeByAddr(ptr);
        if (dec.has_value()) {
            uint64_t hash = CRC64(std::get<0>(dec.value()).c_str());
            SPDLOG_INFO("Found DisplayList: 0x{:X} Hash: 0x{:X} Path: {}", ptr, hash, std::get<0>(dec.value()));
            writer.Write(hash);
        } else {
            SPDLOG_WARN("Could not find DisplayList at 0x{:X}", ptr);
            writer.Write((uint64_t) 0);
        }
    }

    ptr = painting->textureMaps;
    {
        auto dec = Companion::Instance->GetNodeByAddr(ptr);
        if (dec.has_value()) {
            uint64_t hash = CRC64(std::get<0>(dec.value()).c_str());
            SPDLOG_INFO("Found Texture Maps: 0x{:X} Hash: 0x{:X} Path: {}", ptr, hash, std::get<0>(dec.value()));
            writer.Write(hash);
        } else {
            SPDLOG_WARN("Could not find Texture Maps at 0x{:X}", ptr);
            writer.Write((uint64_t) 0);
        }
    }

    ptr = painting->textureArray;
    {
        auto dec = Companion::Instance->GetNodeByAddr(ptr);
        if (dec.has_value()) {
            uint64_t hash = CRC64(std::get<0>(dec.value()).c_str());
            SPDLOG_INFO("Found Texture Arrays: 0x{:X} Hash: 0x{:X} Path: {}", ptr, hash, std::get<0>(dec.value()));
            writer.Write(hash);
        } else {
            SPDLOG_WARN("Could not find Texture Arrays at 0x{:X}", ptr);
            writer.Write((uint64_t) 0);
        }
    }

    writer.Write(painting->textureWidth);
    writer.Write(painting->textureHeight);

    ptr = painting->rippleDisplayList;
    {
        auto dec = Companion::Instance->GetNodeByAddr(ptr);
        if (dec.has_value()) {
            uint64_t hash = CRC64(std::get<0>(dec.value()).c_str());
            SPDLOG_INFO("Found DisplayList: 0x{:X} Hash: 0x{:X} Path: {}", ptr, hash, std::get<0>(dec.value()));
            writer.Write(hash);
        } else {
            SPDLOG_WARN("Could not find DisplayList at 0x{:X}", ptr);
            writer.Write((uint64_t) 0);
        }
    }

    writer.Write(painting->rippleTrigger);
    writer.Write(painting->alpha);
    writer.Write(painting->marioWasUnder);
    writer.Write(painting->marioIsUnder);
    writer.Write(painting->marioWentUnder);
    writer.Write(painting->size);

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> SM64::PaintingFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    LUS::BinaryReader reader(segment.data, segment.size);
    reader.SetEndianness(Torch::Endianness::Big);

    auto id = reader.ReadInt16();
    auto imageCount = reader.ReadInt8();
    auto textureType = reader.ReadInt8();
    auto lastFloor = reader.ReadInt8();
    auto currFloor = reader.ReadInt8();
    auto floorEntered = reader.ReadInt8();
    auto state = reader.ReadInt8();
    auto pitch = reader.ReadFloat();
    auto yaw = reader.ReadFloat();
    auto posX = reader.ReadFloat();
    auto posY = reader.ReadFloat();
    auto posZ = reader.ReadFloat();
    auto currRippleMag = reader.ReadFloat();
    auto passiveRippleMag = reader.ReadFloat();
    auto entryRippleMag = reader.ReadFloat();
    auto rippleDecay = reader.ReadFloat();
    auto passiveRippleDecay = reader.ReadFloat();
    auto entryRippleDecay = reader.ReadFloat();
    auto currRippleRate = reader.ReadFloat();
    auto passiveRippleRate = reader.ReadFloat();
    auto entryRippleRate = reader.ReadFloat();
    auto dispersionFactor = reader.ReadFloat();
    auto passiveDispersionFactor = reader.ReadFloat();
    auto entryDispersionFactor = reader.ReadFloat();
    auto rippleTimer = reader.ReadFloat();
    auto rippleX = reader.ReadFloat();
    auto rippleY = reader.ReadFloat();
    auto normalDisplayList = reader.ReadUInt32();
    auto textureMaps = reader.ReadUInt32();
    auto textureArray = reader.ReadUInt32();
    auto textureWidth = reader.ReadInt16();
    auto textureHeight = reader.ReadInt16();
    auto rippleDisplayList = reader.ReadUInt32();
    auto rippleTrigger = reader.ReadInt8();
    auto alpha = reader.ReadUByte();
    auto marioWasUnder = reader.ReadInt8();
    auto marioIsUnder = reader.ReadInt8();
    auto marioWentUnder = reader.ReadInt8();
    reader.ReadInt8(); // pad
    reader.ReadInt8(); // pad
    reader.ReadInt8(); // pad
    auto size = reader.ReadFloat();

    YAML::Node nDLNode;
    nDLNode["type"] = "GFX";
    nDLNode["offset"] = normalDisplayList;
    Companion::Instance->AddAsset(nDLNode);

    YAML::Node rDLNode;
    rDLNode["type"] = "GFX";
    rDLNode["offset"] = rippleDisplayList;
    Companion::Instance->AddAsset(rDLNode);

    return std::make_shared<SM64::Painting>(id, imageCount, textureType, lastFloor, currFloor, floorEntered, state, pitch, yaw, posX, posY, posZ, currRippleMag, passiveRippleMag, entryRippleMag, rippleDecay, passiveRippleDecay, entryRippleDecay, currRippleRate, passiveRippleRate, entryRippleRate, dispersionFactor, passiveDispersionFactor, entryDispersionFactor, rippleTimer, rippleX, rippleY, normalDisplayList, textureMaps, textureArray, textureWidth, textureHeight, rippleDisplayList, rippleTrigger, alpha, marioWasUnder, marioIsUnder, marioWentUnder, size);
}
