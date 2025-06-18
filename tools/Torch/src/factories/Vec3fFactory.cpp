#include "Vec3fFactory.h"
#include "spdlog/spdlog.h"

#include "Companion.h"
#include "utils/Decompressor.h"
#include "utils/TorchUtils.h"

#define FORMAT_FLOAT(x, w, p) std::dec << std::setfill(' ') << std::fixed << std::setprecision(p) << std::setw(w) << x

Vec3fData::Vec3fData(std::vector<Vec3f> vecs): mVecs(vecs) {
    mMaxPrec = 1;
    mMaxWidth = 3;
    for(Vec3f v : vecs) {
        mMaxPrec = std::max(mMaxPrec, v.precision());
        mMaxWidth = std::max(mMaxWidth, v.width());
    }
}

ExportResult Vec3fHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern Vec3f " << symbol << "[];\n";
    return std::nullopt;
}


int GetPrecision(Vec3f v) {
    return std::max(std::max(GetPrecision(v.x), GetPrecision(v.y)), GetPrecision(v.z));
}

ExportResult Vec3fCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto vecData = std::static_pointer_cast<Vec3fData>(raw);
    int i = 0;

    write << "Vec3f " << symbol << "[] = {";

    int cols = 120 / (3 * vecData->mMaxWidth + 8);
    for(Vec3f v : vecData->mVecs) {
        if((i++ % cols) == 0) {
            write << "\n" << fourSpaceTab;
        }
        write << FORMAT_FLOAT(v, vecData->mMaxWidth, vecData->mMaxPrec) << ", ";
    }

    write << "\n};\n";

    if (Companion::Instance->IsDebug()) {
        write << "// Count: " << vecData->mVecs.size() << " Vec3fs\n";
    }

    return offset + vecData->mVecs.size() * sizeof(Vec3f);
}

ExportResult Vec3fBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto vecData = std::static_pointer_cast<Vec3fData>(raw);

    WriteHeader(writer, Torch::ResourceType::Vec3f, 0);
    writer.Write((uint32_t) vecData->mVecs.size());

    for(Vec3f v : vecData->mVecs) {
        auto [x, y, z] = v;
        writer.Write(x);
        writer.Write(y);
        writer.Write(z);
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> Vec3fFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    std::vector<Vec3f> vecs;
    const auto count = GetSafeNode<int>(node, "count");
    auto [root, segment] = Decompressor::AutoDecode(node, buffer, count * sizeof(Vec3f));
    LUS::BinaryReader reader(segment.data, segment.size);
    reader.SetEndianness(Torch::Endianness::Big);

    for(int i = 0; i < count; i++) {
        auto vx = reader.ReadFloat();
        auto vy = reader.ReadFloat();
        auto vz = reader.ReadFloat();

        vecs.push_back(Vec3f(vx, vy, vz));
    }

    return std::make_shared<Vec3fData>(vecs);
}
