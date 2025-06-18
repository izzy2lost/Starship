#include "ViewportFactory.h"

#include "utils/Decompressor.h"
#include "spdlog/spdlog.h"
#include "Companion.h"

ExportResult ViewportHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern Vp " << symbol << ";\n";

    return std::nullopt;
}

ExportResult ViewportCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto viewport = std::static_pointer_cast<VpData>(raw);

    write << "Vp " << symbol << " = {\n";

    write << fourSpaceTab;
    for (size_t i = 0; i < 4; i++) {
        write << viewport->mViewport.vscale[i];
        if (i != 3) {
            write << ", ";
        }
    }
    write << ",\n";

    write << fourSpaceTab;
    for (size_t i = 0; i < 4; i++) {
        write << viewport->mViewport.vtrans[i];
        if (i != 3) {
            write << ", ";
        }
    }
    write << ",\n";

    write << "};\n\n";

    return offset + sizeof(VpRaw);
}

ExportResult ViewportBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto viewport = std::static_pointer_cast<VpData>(raw);

    WriteHeader(writer, Torch::ResourceType::Viewport, 0);

    for (size_t i = 0; i < 4; i++) {
        writer.Write(viewport->mViewport.vscale[i]);
    }

    for (size_t i = 0; i < 4; i++) {
        writer.Write(viewport->mViewport.vtrans[i]);
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> ViewportFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    LUS::BinaryReader reader(segment.data, segment.size);
    VpRaw viewport;

    reader.SetEndianness(Torch::Endianness::Big);

    for (size_t i = 0; i < 4; i++) {
        viewport.vscale[i] = reader.ReadInt16();
    }

    for (size_t i = 0; i < 4; i++) {
        viewport.vtrans[i] = reader.ReadInt16();
    }

    return std::make_shared<VpData>(viewport);
}
