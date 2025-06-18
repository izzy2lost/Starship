#include "MovtexQuadFactory.h"
#include "spdlog/spdlog.h"

#include "Companion.h"
#include "utils/Decompressor.h"
#include "utils/TorchUtils.h"
#include <regex>

ExportResult SM64::MovtexQuadHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern MovtexQuadCollection " << symbol << "[];\n";
    return std::nullopt;
}

ExportResult SM64::MovtexQuadCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");

    auto quadData = std::static_pointer_cast<SM64::MovtexQuadData>(raw);

    write << "const struct MovtexQuadCollection " << symbol << "[] = {\n";

    for (auto &quad: quadData->mMovtexQuads) {
        write << fourSpaceTab << "{" << quad.first << ", ";
        if (quad.second == 0) {
            write << "NULL";
        } else {
            auto dec = Companion::Instance->GetNodeByAddr(quad.second);
            if (dec.has_value()) {
                auto movtexNode = std::get<1>(dec.value());
                auto movtexSymbol = GetSafeNode<std::string>(movtexNode, "symbol");
                write << movtexSymbol;
            } else {
                SPDLOG_WARN("Could not find movtex at 0x{:X}", quad.second);
                write << "0x" << std::hex << std::uppercase << quad.second << std::nouppercase << std::dec;
            }
        }
        write << "},\n";
    }

    write << "};\n";

    if (Companion::Instance->IsDebug()) {
        write << "// MovtexQuad count: " << quadData->mMovtexQuads.size() << "\n";
    }

    return offset + quadData->mMovtexQuads.size() * 4;
}

ExportResult SM64::MovtexQuadBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto quadData = std::static_pointer_cast<SM64::MovtexQuadData>(raw);

    WriteHeader(writer, Torch::ResourceType::MovtexQuad, 0);
    writer.Write((uint32_t) quadData->mMovtexQuads.size());

    for (auto &quad: quadData->mMovtexQuads) {
        writer.Write(quad.first);
        if (quad.second == 0) {
            writer.Write((uint64_t) quad.second);
        } else {
            auto dec = Companion::Instance->GetNodeByAddr(quad.second);
            if (dec.has_value()) {
                uint64_t hash = CRC64(std::get<0>(dec.value()).c_str());
                SPDLOG_INFO("Found movtex: 0x{:X} Hash: 0x{:X} Path: {}", quad.second, hash, std::get<0>(dec.value()));
                writer.Write(hash);
            } else {
                SPDLOG_WARN("Could not find movtex at 0x{:X}", quad.second);
                writer.Write(0);
                throw std::runtime_error("PANIC");
            }
        }
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> SM64::MovtexQuadFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    const auto offset = GetSafeNode<uint32_t>(node, "offset");
    const auto symbol = GetSafeNode<std::string>(node, "symbol");
    const auto count = GetSafeNode<size_t>(node, "count");
    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    LUS::BinaryReader reader(segment.data, segment.size);
    reader.SetEndianness(Torch::Endianness::Big);
    std::vector<std::pair<int16_t, uint32_t>> movtexQuads;

    for (size_t i = 0; i < count; ++i) {
        auto id = reader.ReadInt16();
        auto pad = reader.ReadInt16();
        auto movtexPtr = reader.ReadUInt32();
        movtexQuads.emplace_back(std::make_pair(id, movtexPtr));

        if (movtexPtr != 0) {
            YAML::Node movtexNode;
            movtexNode["type"] = "SM64:MOVTEX";
            movtexNode["offset"] = movtexPtr;
            movtexNode["quad"] = true;
            Companion::Instance->AddAsset(movtexNode);
        }
    }

    return std::make_shared<SM64::MovtexQuadData>(movtexQuads);
}
