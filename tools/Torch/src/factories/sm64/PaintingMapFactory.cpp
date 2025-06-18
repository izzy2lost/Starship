#include "PaintingMapFactory.h"
#include "spdlog/spdlog.h"

#include "Companion.h"
#include "utils/Decompressor.h"
#include "utils/TorchUtils.h"
#include <regex>

// #define FORMAT_INT(x, w) std::dec << std::setfill(' ') << std::setw(w) << x

ExportResult SM64::PaintingMapHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern PaintingData " << symbol << "[];\n";
    return std::nullopt;
}

ExportResult SM64::PaintingMapCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");

    auto paintingData = std::static_pointer_cast<SM64::PaintingData>(raw);

    write << "static const PaintingData " << symbol << "[] = {\n";

    write << fourSpaceTab << paintingData->mPaintingMappings.size() << ",\n";

    for (auto &mapping : paintingData->mPaintingMappings) {
        write << fourSpaceTab;
        write << mapping.vtxId << ", " << mapping.texX << ", " << mapping.texY << ",\n";
    }

    write << fourSpaceTab << paintingData->mPaintingGroups.size() << ",\n";

    for (auto &group : paintingData->mPaintingGroups) {
        write << fourSpaceTab;
        write << group.x << ", " << group.y << ", " << group.z << ",\n";
    }

    write << "};\n";

    size_t size = (paintingData->mPaintingMappings.size() + paintingData->mPaintingGroups.size()) * 3 * sizeof(int16_t) + 2;

    return offset + size;
}

ExportResult SM64::PaintingMapBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto paintingData = std::static_pointer_cast<SM64::PaintingData>(raw);

    WriteHeader(writer, Torch::ResourceType::PaintingData, 0);

    writer.Write((uint32_t) (paintingData->mPaintingMappings.size() + paintingData->mPaintingGroups.size()) + 2 );

    writer.Write((int16_t) paintingData->mPaintingMappings.size());

    for (auto &mapping : paintingData->mPaintingMappings) {
        writer.Write(mapping.vtxId);
        writer.Write(mapping.texX);
        writer.Write(mapping.texY);
    }

    writer.Write((int16_t) paintingData->mPaintingGroups.size());

    for (auto &group : paintingData->mPaintingGroups) {
        writer.Write(group.x);
        writer.Write(group.y);
        writer.Write(group.z);
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> SM64::PaintingMapFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    std::vector<PaintingMapping> paintingMappings;
    std::vector<Vec3s> paintingGroups;
    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    LUS::BinaryReader reader(segment.data, segment.size);
    reader.SetEndianness(Torch::Endianness::Big);

    auto mappingsSize = reader.ReadInt16();

    for (int16_t i = 0; i < mappingsSize; ++i) {
        auto vtxId = reader.ReadInt16();
        auto texX = reader.ReadInt16();
        auto texY = reader.ReadInt16();
        paintingMappings.emplace_back(vtxId, texX, texY);
    }

    auto groupSize = reader.ReadInt16();

    for (int16_t i = 0; i < groupSize; ++i) {
        auto x = reader.ReadInt16();
        auto y = reader.ReadInt16();
        auto z = reader.ReadInt16();
        paintingGroups.emplace_back(x, y, z);
    }

    return std::make_shared<SM64::PaintingData>(paintingMappings, paintingGroups);
}
