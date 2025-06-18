#include "Vec3sFactory.h"
#include "spdlog/spdlog.h"

#include "Companion.h"
#include "utils/Decompressor.h"
#include "utils/TorchUtils.h"

#define FORMAT_INT(x, w) std::dec << std::setfill(' ') << std::setw(w) << x

Vec3sData::Vec3sData(std::vector<Vec3s> vecs): mVecs(vecs) {
    mMaxWidth = 3;
    for(Vec3s v : vecs) {
        mMaxWidth = std::max(mMaxWidth, v.width());
    }
}

ExportResult Vec3sHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern Vec3s " << symbol << "[];\n";
    return std::nullopt;
}


int GetPrecision(Vec3s v) {
    return std::max(std::max(GetPrecision(v.x), GetPrecision(v.y)), GetPrecision(v.z));
}

ExportResult Vec3sCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto vecData = std::static_pointer_cast<Vec3sData>(raw);
    int i = 0;

    write << "Vec3s " << symbol << "[] = {";

    int cols = 120 / (3 * vecData->mMaxWidth + 8);
    for(Vec3s v : vecData->mVecs) {
        if((i++ % cols) == 0) {
            write << "\n" << fourSpaceTab;
        }
        write << FORMAT_INT(v, vecData->mMaxWidth) << ", ";
    }

    write << "\n};\n";

    if (Companion::Instance->IsDebug()) {
        write << "// Count: " << vecData->mVecs.size() << " Vec3s\n";
    }

    return offset + vecData->mVecs.size() * sizeof(Vec3s);
}

ExportResult Vec3sBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto vecData = std::static_pointer_cast<Vec3sData>(raw);

    WriteHeader(writer, Torch::ResourceType::Vec3s, 0);
    writer.Write((uint32_t) vecData->mVecs.size());

    for(Vec3s v : vecData->mVecs) {
        auto [x, y, z] = v;
        writer.Write(x);
        writer.Write(y);
        writer.Write(z);
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> Vec3sFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    std::vector<Vec3s> vecs;
    const auto count = GetSafeNode<int>(node, "count");
    auto [root, segment] = Decompressor::AutoDecode(node, buffer, count * sizeof(Vec3s));
    LUS::BinaryReader reader(segment.data, segment.size);
    reader.SetEndianness(Torch::Endianness::Big);

    for(int i = 0; i < count; i++) {
        auto vx = reader.ReadInt16();
        auto vy = reader.ReadInt16();
        auto vz = reader.ReadInt16();

        vecs.push_back(Vec3s(vx, vy, vz));
    }

    return std::make_shared<Vec3sData>(vecs);
}
