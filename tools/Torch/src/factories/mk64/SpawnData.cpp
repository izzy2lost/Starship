#include "SpawnData.h"

#include "Companion.h"
#include "utils/Decompressor.h"

#define NUM(x) std::dec << std::setfill(' ') << std::setw(6) << x
#define COL(c) "0x" << std::hex << std::setw(2) << std::setfill('0') << c

ExportResult MK64::SpawnDataHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern ActorSpawnData " << symbol << "[];\n";
    return std::nullopt;
}

ExportResult MK64::SpawnDataCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto spawns = std::static_pointer_cast<SpawnDataData>(raw)->mSpawns;
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");

    write << "ActorSpawnData " << symbol << "[] = {\n";

    for (int i = 0; i < spawns.size(); i++) {
        auto x = spawns[i].x;
        auto y = spawns[i].y;
        auto z = spawns[i].z;

        auto id = spawns[i].id;

        if(i <= spawns.size() - 1) {
            write << fourSpaceTab;
        }

        // { x, y, z, id },
        write << "{" << NUM(x) << ", " << NUM(y) << ", " << NUM(z) << ", " << NUM(id) << " },\n";
    }
    write << "};\n";

    return offset + spawns.size() * sizeof(MK64::ActorSpawnData);
}

ExportResult MK64::SpawnDataBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto spawns = std::static_pointer_cast<SpawnDataData>(raw)->mSpawns;
    auto writer = LUS::BinaryWriter();

    WriteHeader(writer, Torch::ResourceType::SpawnData, 0);
    writer.Write((uint32_t) spawns.size());
    for(auto s : spawns) {
        writer.Write(s.x);
        writer.Write(s.y);
        writer.Write(s.z);
        writer.Write(s.id);
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> MK64::SpawnDataFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto count = GetSafeNode<size_t>(node, "count");

    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    LUS::BinaryReader reader(segment.data, count * sizeof(MK64::ActorSpawnData));

    reader.SetEndianness(Torch::Endianness::Big);
    std::vector<MK64::ActorSpawnData> spawns;

    for(size_t i = 0; i < count; i++) {
        auto x = reader.ReadInt16();
        auto y = reader.ReadInt16();
        auto z = reader.ReadInt16();
        auto id = reader.ReadUInt16();

        spawns.push_back(MK64::ActorSpawnData({
           x, y, z, id
       }));
    }

    return std::make_shared<SpawnDataData>(spawns);
}