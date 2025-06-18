#include "UnkSpawnData.h"

#include "Companion.h"
#include "utils/Decompressor.h"

#define NUM(x) std::dec << std::setfill(' ') << std::setw(6) << x
#define COL(c) "0x" << std::hex << std::setw(2) << std::setfill('0') << c

ExportResult MK64::UnkSpawnDataHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern UnkActorSpawnData " << symbol << "[];\n";
    return std::nullopt;
}

ExportResult MK64::UnkSpawnDataCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto spawns = std::static_pointer_cast<UnkSpawnDataData>(raw)->mSpawns;
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");

    write << "UnkActorSpawnData " << symbol << "[] = {\n";

    for (int i = 0; i < spawns.size(); i++) {
        auto x = spawns[i].x;
        auto y = spawns[i].y;
        auto z = spawns[i].z;

        auto someId = spawns[i].someId;
        auto unk8 = spawns[i].unk8;

        if(i <= spawns.size() - 1) {
            write << fourSpaceTab;
        }

        // { x, y, z, id },
        write << "{" << NUM(x) << ", " << NUM(y) << ", " << NUM(z) << ", " << NUM(someId) << ", " << NUM(unk8) << " },\n";
    }
    write << "};\n";

    return offset + spawns.size() * sizeof(MK64::UnkActorSpawnData);
}

ExportResult MK64::UnkSpawnDataBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto spawns = std::static_pointer_cast<UnkSpawnDataData>(raw)->mSpawns;
    auto writer = LUS::BinaryWriter();

    WriteHeader(writer, Torch::ResourceType::UnkSpawnData, 0);
    writer.Write((uint32_t) spawns.size());
    for(auto s : spawns) {
        writer.Write(s.x);
        writer.Write(s.y);
        writer.Write(s.z);
        writer.Write(s.someId);
        writer.Write(s.unk8);
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> MK64::UnkSpawnDataFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto count = GetSafeNode<size_t>(node, "count");

    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    LUS::BinaryReader reader(segment.data, count * sizeof(MK64::UnkActorSpawnData));

    reader.SetEndianness(Torch::Endianness::Big);
    std::vector<MK64::UnkActorSpawnData> spawns;

    for(size_t i = 0; i < count; i++) {
        auto x = reader.ReadInt16();
        auto y = reader.ReadInt16();
        auto z = reader.ReadInt16();
        auto someId = reader.ReadInt16();
        auto unk8 = reader.ReadInt16();

        spawns.push_back(MK64::UnkActorSpawnData({
           x, y, z, someId, unk8 
       }));
    }

    return std::make_shared<UnkSpawnDataData>(spawns);
}