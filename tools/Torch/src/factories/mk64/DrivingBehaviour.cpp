#include "DrivingBehaviour.h"
#include "spdlog/spdlog.h"

#include "Companion.h"
#include "utils/Decompressor.h"

#define NUM(x) std::dec << std::setfill(' ') << std::setw(6) << x
#define COL(c) std::dec << std::setfill(' ') << std::setw(3) << c

ExportResult MK64::DrivingBehaviourHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern KartAIBehaviour " << symbol << "[];\n";
    return std::nullopt;
}

ExportResult MK64::DrivingBehaviourCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto bhv = std::static_pointer_cast<DrivingData>(raw);
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    auto offset = GetSafeNode<uint32_t>(node, "offset");

    if (IS_SEGMENTED(offset)) {
        offset = SEGMENT_OFFSET(offset);
    }

    if (Companion::Instance->IsDebug()) {
        if (IS_SEGMENTED(offset)) {
            offset = SEGMENT_OFFSET(offset);
        }
        write << "// 0x" << std::hex << std::uppercase << offset << "\n";
    }

    write << "KartAIBehaviour " << symbol << "[] = {\n";


    for(auto b : bhv->mBhvs) {
        auto w1 = b.waypoint1;
        auto w2 = b.waypoint2;
        auto id = b.bhv;

        // { w1, w2, bhvId}
        write << fourSpaceTab << "{" << NUM(w1) << ", " << NUM(w2) << ", " << NUM(id) << "},\n";
    }

    write << "};\n";

    if (Companion::Instance->IsDebug()) {
        write << "// count: " << std::to_string(bhv->mBhvs.size()) << " Entries\n";
        write << "// 0x" << std::hex << std::uppercase << (offset + (sizeof(BhvRaw) * bhv->mBhvs.size())) << "\n";
    }

    write << "\n";
    return offset + bhv->mBhvs.size() * sizeof(BhvRaw);
}

ExportResult MK64::DrivingBehaviourBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto bhv = std::static_pointer_cast<DrivingData>(raw);
    auto writer = LUS::BinaryWriter();

    WriteHeader(writer, Torch::ResourceType::DrivingBehaviour, 0);
    writer.Write((uint32_t) bhv->mBhvs.size());
    for(auto b : bhv->mBhvs) {
        writer.Write(b.waypoint1);
        writer.Write(b.waypoint2);
        writer.Write(b.bhv);
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> MK64::DrivingBehaviourFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    LUS::BinaryReader reader(segment.data, segment.size);

    reader.SetEndianness(Torch::Endianness::Big);
    std::vector<BhvRaw> behaviours;

    while(1) {
        auto w1 = reader.ReadInt16();
        auto w2 = reader.ReadInt16();
        auto id = reader.ReadInt32();

        behaviours.push_back( BhvRaw( {w1, w2, id} ) );

        // Magic number for ending of array
        if ((w1 == -1) && (w2 == -1)) {
            break;
        }
    }

    return std::make_shared<DrivingData>(behaviours);
}