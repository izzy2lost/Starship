#include "TriangleFactory.h"
#include "spdlog/spdlog.h"

#include "Companion.h"
#include "utils/Decompressor.h"
#include "utils/TorchUtils.h"
#include <regex>


#define NUM(x, w) std::dec << std::setfill(' ') << std::setw(w) << x
#define FORMAT_FLOAT(x, w, p) std::dec << std::setfill(' ') << std::fixed << std::setprecision(p) << std::setw(w) << x

SF64::TriangleData::TriangleData(std::vector<Vec3s> tris, std::vector<YAML::Node> meshNodes): mTris(tris), mMeshNodes(meshNodes) {
}

ExportResult SF64::TriangleHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    auto triData = std::static_pointer_cast<SF64::TriangleData>(raw);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern Triangle " << symbol << "[" << std::dec << triData->mTris.size() << "];\n";
    return std::nullopt;
}

ExportResult SF64::TriangleCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto triData = std::static_pointer_cast<SF64::TriangleData>(raw);
    const auto meshSize = GetSafeNode(triData->mMeshNodes[0], "count", 0);
    auto segment = 0;
    int width = std::log10(meshSize) + 1;
    int i = 0;

    write << "Triangle " << symbol << "[] = {";
    for(Vec3s tri : triData->mTris) {
        if((i++ % 6) == 0) {
            write << "\n" << fourSpaceTab;
        }
        write << NUM(tri, width) << ", ";
    }

    write << "\n};\n";

    if (Companion::Instance->IsDebug()) {
        write << "// Triangle count: " << triData->mTris.size() << "\n";
    }

    return offset + triData->mTris.size() * sizeof(Vec3s);
}

ExportResult SF64::TriangleBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto triData = std::static_pointer_cast<TriangleData>(raw);

    WriteHeader(writer, Torch::ResourceType::Vec3s, 0);
    writer.Write((uint32_t) triData->mTris.size());

    for(Vec3s tri : triData->mTris) {
        auto [x, y, z] = tri;
        writer.Write(x);
        writer.Write(y);
        writer.Write(z);
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> SF64::TriangleFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    const auto offset = GetSafeNode<uint32_t>(node, "offset");
    const auto count = GetSafeNode<uint32_t>(node, "count");
    const auto meshCount = GetSafeNode<uint32_t>(node, "mesh_count", 1);
    std::vector<Vec3s> tris;
    std::vector<YAML::Node> meshNodes;
    int meshSize = 0;
    auto [_, segment] = Decompressor::AutoDecode(node, buffer, count * sizeof(Vec3s));
    LUS::BinaryReader reader(segment.data, segment.size);
    reader.SetEndianness(Torch::Endianness::Big);

    for(int i = 0; i < count; i++) {
        Vec3s tri;

        tri.x = reader.ReadInt16();
        meshSize = std::max(meshSize, (int)tri.x);
        tri.y = reader.ReadInt16();
        meshSize = std::max(meshSize, (int)tri.y);
        tri.z = reader.ReadInt16();
        meshSize = std::max(meshSize, (int)tri.z);

        tris.push_back(tri);
    }
    meshSize++;
    auto meshOffset = GetSafeNode<uint32_t>(node, "mesh_offset", offset + count * sizeof(Vec3s));
    for(int j = 0; j < meshCount; j++) {
        YAML::Node meshNode;

        if(node["mesh_symbol"]) {
            auto meshSymbol = GetSafeNode<std::string>(node, "mesh_symbol");
            if (meshSymbol.find("OFFSET") == std::string::npos) {
                if(meshCount > 1) {
                    meshSymbol += "_" + std::to_string(j);
                }
            } else {
                std::ostringstream offsetSeg;

                offsetSeg << std::uppercase << std::hex << meshOffset;
                meshSymbol = std::regex_replace(meshSymbol, std::regex(R"(OFFSET)"), offsetSeg.str());
            }
            meshNode["symbol"] = meshSymbol;
        }
        meshNode["type"] = "VEC3F";
        meshNode["count"] = meshSize;
        meshNode["offset"] = meshOffset;

        meshNode = Companion::Instance->AddAsset(meshNode).value();
        meshNodes.push_back(meshNode);
        meshOffset += meshSize * sizeof(Vec3f);
    }

    return std::make_shared<SF64::TriangleData>(tris, meshNodes);
}
