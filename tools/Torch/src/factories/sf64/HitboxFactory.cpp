#include "HitboxFactory.h"
#include "spdlog/spdlog.h"

#include "Companion.h"
#include "utils/Decompressor.h"
#include "utils/TorchUtils.h"

static int GetPrecision(float f) {
    int p = 0;
    int shift = 1;
    float approx = std::round(f);

    while(f != approx && p < 12 ){
        shift *= 10;
        p++;
        approx = std::round(f * shift) / shift;
    }
    return p;
}

static void FormatFloat(std::ostream& out, const float x, int w) {
    if(x == (int) x) {
        out << std::dec << std::fixed << std::setprecision(0) << std::setfill(' ') << std::setw(w - 2) << x << ".0f";
    } else {
        out << std::dec << std::fixed << std::setprecision(GetPrecision(x)) << std::setfill(' ') << std::setw(w) << x << "f";
    }
}

SF64::HitboxData::HitboxData(std::vector<float> data, std::vector<int> types): mData(data), mTypes(types) {

}

ExportResult SF64::HitboxHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern f32 " << symbol << "[];\n";
    return std::nullopt;
}

ExportResult SF64::HitboxCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto hitbox = std::static_pointer_cast<SF64::HitboxData>(raw);
    auto index = 0;
    auto count = hitbox->mData[index++];
    auto hasRot = false;

    for(int type : hitbox->mTypes) {
        if(type == 2) {
            hasRot = true;
            break;
        }
    }

    write << "f32 " << symbol << "[] = {\n";
    write << fourSpaceTab << count << ",\n";

    for(int i = 0; i < (int)count; i++) {
        if(hitbox->mTypes[i] == 4) {
            write << fourSpaceTab << "HITBOX_WHOOSH,     ";
            index++;
        } else if (hitbox->mTypes[i] == 3) {
            write << fourSpaceTab << "HITBOX_SHADOW,     ";
            index++;
        } else if (hitbox->mTypes[i] == 2) {
            write << fourSpaceTab << "HITBOX_ROTATED,    ";
            index++;
            for(int j = 0; j < 3; j++) {
                auto tempf = hitbox->mData[index++];
                FormatFloat(write, tempf, 6);
                write << ", ";
            }
        } else {
            write << " /* HITBOX_STANDARD */ ";
        }
        if(hasRot && hitbox->mTypes[i] != 2) {
            for(int j = 0; j < 7; j++) {
                write << fourSpaceTab;
            }
        }
        for(int j = 0; j < 6; j++) {
            auto tempf = hitbox->mData[index++];
            FormatFloat(write, tempf, 7);
            write << ", ";
        }
        write << "\n";
    }
    write << "};\n";

    return offset + sizeof(float) * index;
}

ExportResult SF64::HitboxBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto hitbox = std::static_pointer_cast<SF64::HitboxData>(raw);
    auto writer = LUS::BinaryWriter();

    WriteHeader(writer, Torch::ResourceType::Hitbox, 0);

    auto count = hitbox->mData.size();
    writer.Write((uint32_t) count);
    for(size_t i = 0; i < hitbox->mData.size(); i++) {
        writer.Write(hitbox->mData[i]);
    }
    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> SF64::HitboxFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    std::vector<float> data;
    int count;
    std::vector<int> types;
    auto [_, segment] = Decompressor::AutoDecode(node, buffer, 0x10000);
    LUS::BinaryReader reader(segment.data, segment.size);
    reader.SetEndianness(Torch::Endianness::Big);

    data.push_back(reader.ReadFloat());
    count = data[0];

    while(count > 0) {
        auto typecode = reader.ReadFloat();
        auto readCount = 0;
        if(typecode == 300000.0f || typecode == 400000.0f) {
            readCount = 6;
            types.push_back((typecode == 300000.0f) ? 3 : 4);
        } else if (typecode == 200000.0f) {
            readCount = 9;
            types.push_back(2);
        } else {
            readCount = 5;
            types.push_back(1);
        }
        data.push_back(typecode);
        for(int i = 0; i < readCount; i++) {
            data.push_back(reader.ReadFloat());
        }
        count--;
    }

    return std::make_shared<SF64::HitboxData>(data, types);
}
