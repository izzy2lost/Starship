#include "TrackSections.h"

#include "Companion.h"
#include "utils/Decompressor.h"

#define NUM(x) std::dec << std::setfill(' ') << std::setw(6) << x
#define COL(c) "0x" << std::hex << std::setw(2) << std::setfill('0') << c

ExportResult MK64::TrackSectionsHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern TrackSections " << symbol << "[];\n";
    return std::nullopt;
}

ExportResult MK64::TrackSectionsCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto sections = std::static_pointer_cast<TrackSectionsData>(raw)->mSecs;
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");

    if (Companion::Instance->IsDebug()) {
        write << "// 0x" << std::hex << std::uppercase << offset << "\n";
    }

    write << "TrackSections " << symbol << "[] = {\n";

    for (int i = 0; i < sections.size(); i++) {
        auto entry = sections[i];

        auto addr = entry.addr;
        auto surf = entry.surfaceType;
        auto sect = entry.sectionId;
        auto flags = entry.flags;

        if(i <= sections.size() - 1) {
            write << fourSpaceTab;
        }

        // { addr, surface, section, flags },
        write << "{" << NUM(addr) << ", " << NUM(surf) << ", " << NUM(sect) << ", " << NUM(flags) << "},\n";
    }
    write << "};\n";

    return offset + sections.size() * sizeof(MK64::TrackSections);
}

ExportResult MK64::TrackSectionsBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto sections = std::static_pointer_cast<TrackSectionsData>(raw);
    auto writer = LUS::BinaryWriter();

    WriteHeader(writer, Torch::ResourceType::TrackSection, 0);
    writer.Write((uint32_t) sections->mSecs.size());
    for(auto entry : sections->mSecs) {
        writer.Write(entry.addr);
        writer.Write(entry.surfaceType);
        writer.Write(entry.sectionId);
        writer.Write(entry.flags);
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> MK64::TrackSectionsFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto count = GetSafeNode<size_t>(node, "count");

    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    LUS::BinaryReader reader(segment.data, count * sizeof(MK64::TrackSections));

    reader.SetEndianness(Torch::Endianness::Big);
    std::vector<MK64::TrackSections> sections;

    for(size_t i = 0; i < count; i++) {
        auto addr = reader.ReadUInt32();
        auto surf = reader.ReadInt8();
        auto sect = reader.ReadInt8();
        auto flags = reader.ReadUInt16();

        sections.push_back(MK64::TrackSections({
           addr, surf, sect, flags,
       }));
    }

    return std::make_shared<TrackSectionsData>(sections);
}