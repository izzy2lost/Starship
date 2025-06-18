#include "ColPolyFactory.h"
#include "spdlog/spdlog.h"

#include "Companion.h"
#include "utils/Decompressor.h"
#include "utils/TorchUtils.h"
#include <regex>


#define NUM(x, w) std::dec << std::setfill(' ') << std::setw(w) << x
#define FORMAT_FLOAT(x, w, p) std::dec << std::setfill(' ') << std::fixed << std::setprecision(p) << std::setw(w) << x

SF64::ColPolyData::ColPolyData(std::vector<SF64::CollisionPoly> polys, std::vector<YAML::Node> meshNodes): mPolys(polys), mMeshNodes(meshNodes) {

}

ExportResult SF64::ColPolyHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    auto colpolys = std::static_pointer_cast<SF64::ColPolyData>(raw);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern CollisionPoly " << symbol << "[" << std::dec << colpolys->mPolys.size() << "];\n";
    return std::nullopt;
}

ExportResult SF64::ColPolyCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto colpolys = std::static_pointer_cast<SF64::ColPolyData>(raw);
    const auto meshSize = GetSafeNode(colpolys->mMeshNodes[0], "count", 0);

    write << "CollisionPoly " << symbol << "[] = {";
    int width = std::log10(meshSize) + 1;

    for(SF64::CollisionPoly poly : colpolys->mPolys) {
        write << "\n" << fourSpaceTab;
        write << "{ " << NUM(poly.tri, width) << ",  ";
        if (poly.unk_06 != 0) {
            SPDLOG_ERROR("SF64:COLPOLY error: Nonzero value found in padding");
            write << "/* ALERT: NONZERO PAD */ ";
        }
        write << "{" << NUM(poly.norm, 6) << ",  ";
        if(poly.unk_0E != 0) {
            SPDLOG_ERROR("SF64:COLPOLY error: Nonzero value found in padding");
            write << "/* ALERT: NONZERO PAD */ ";
        }
        write << NUM(poly.dist, 9) << "} }, ";
    }

    write << "\n};\n";
    uint32_t endOffset = ASSET_PTR(offset) + colpolys->mPolys.size() * sizeof(SF64::CollisionPoly);
    if (Companion::Instance->IsDebug()) {
        write << "// CollisionPoly count: " << colpolys->mPolys.size() << "\n";
        write << "// 0x" << std::uppercase << std::hex << endOffset << "\n";
    }

    write << "\n";

    return endOffset;
}

ExportResult SF64::ColPolyBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto colpolys = std::static_pointer_cast<SF64::ColPolyData>(raw);

    WriteHeader(writer, Torch::ResourceType::ColPoly, 0);

    writer.Write((uint32_t) colpolys->mPolys.size());

    for(auto &poly : colpolys->mPolys) {
        writer.Write(poly.tri.x);
        writer.Write(poly.tri.y);
        writer.Write(poly.tri.z);
        if (poly.unk_06 != 0) {
            SPDLOG_ERROR("SF64:COLPOLY error: Nonzero value found in padding");
        }
        writer.Write(poly.norm.x);
        writer.Write(poly.norm.y);
        writer.Write(poly.norm.z);
        if (poly.unk_0E != 0) {
            SPDLOG_ERROR("SF64:COLPOLY error: Nonzero value found in padding");
        }
        writer.Write(poly.dist);
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> SF64::ColPolyFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    const auto offset = GetSafeNode<uint32_t>(node, "offset");
    const auto count = GetSafeNode<uint32_t>(node, "count");
    const auto meshCount = GetSafeNode<uint32_t>(node, "mesh_count", 1);
    std::vector<SF64::CollisionPoly> polys;
    std::vector<YAML::Node> meshNodes;
    int meshSize = 0;
    auto [_, segment] = Decompressor::AutoDecode(node, buffer, count * sizeof(SF64::CollisionPoly));
    LUS::BinaryReader reader(segment.data, segment.size);
    reader.SetEndianness(Torch::Endianness::Big);

    for(int i = 0; i < count; i++) {
        int16_t v0 = reader.ReadInt16();
        meshSize = std::max(meshSize, (int)v0);
        int16_t v1 = reader.ReadInt16();
        meshSize = std::max(meshSize, (int)v1);
        int16_t v2 = reader.ReadInt16();
        meshSize = std::max(meshSize, (int)v2);
        int16_t pad1 = reader.ReadInt16();
        int16_t nx = reader.ReadInt16();
        int16_t ny = reader.ReadInt16();
        int16_t nz = reader.ReadInt16();
        int16_t pad2 = reader.ReadInt16();
        int32_t dist = reader.ReadInt32();

        polys.push_back(CollisionPoly({{v0, v1, v2}, pad1, {nx, ny, nz}, pad2, dist}));
    }
    meshSize++;
    auto meshOffset = GetSafeNode<uint32_t>(node, "mesh_offset", offset + count * sizeof(SF64::CollisionPoly));
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
        meshNode["type"] = "VEC3S";
        meshNode["count"] = meshSize;
        meshNode["offset"] = meshOffset;

        meshNode = Companion::Instance->AddAsset(meshNode).value();

        meshNodes.push_back(meshNode);
        meshOffset += meshSize * sizeof(Vec3s);
    }

    return std::make_shared<SF64::ColPolyData>(polys, meshNodes);
}
