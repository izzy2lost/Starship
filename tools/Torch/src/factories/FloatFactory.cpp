#include "FloatFactory.h"
#include "spdlog/spdlog.h"

#include "Companion.h"
#include "utils/Decompressor.h"

#define NUM(x) std::dec << std::setfill(' ') << std::setw(6) << x
#define COL(c) std::dec << std::setfill(' ') << std::setw(3) << c

ExportResult FloatHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern f32 " << symbol << "[];\n";
    return std::nullopt;
}

ExportResult FloatCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto f = std::static_pointer_cast<FloatData>(raw)->mFloats;
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

    write << "f32 " << symbol << "[] = {";

    /**
     * f32 sym[] = {
     *    0.1, 0.2, 0.3,
     * };
     *
    */
    for (int i = 0; i < f.size(); ++i) {

        // Make a new line every fourth iteration
        if ((i % 4) == 0) {
            write << "\n" << fourSpaceTab;
        }

        write << f[i] << ", ";

        // if(i <= f.size() - 1) {
        //     write << fourSpaceTab;
        // }
    }

    write << "\n};\n";

    if (Companion::Instance->IsDebug()) {
        write << "// count: " << std::to_string(f.size()) << " f32s\n";
        write << "// 0x" << std::hex << std::uppercase << (offset + (sizeof(float) * f.size())) << "\n";
    }

    write << "\n";

    return offset + f.size() * sizeof(float);
}

ExportResult FloatBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto f = std::static_pointer_cast<FloatData>(raw);
    auto writer = LUS::BinaryWriter();

    WriteHeader(writer, Torch::ResourceType::Float, 0);
    writer.Write((uint32_t) f->mFloats.size());
    for(auto fl : f->mFloats) {
        writer.Write(fl);
    }
    throw std::runtime_error("Float factory untested for otr/o2r exporter");
    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> FloatFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto count = GetSafeNode<size_t>(node, "count");

    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    LUS::BinaryReader reader(segment.data, count * sizeof(float));

    reader.SetEndianness(Torch::Endianness::Big);
    std::vector<float> floats;

    for(size_t i = 0; i < count; i++) {
        auto f = reader.ReadFloat();

        floats.push_back(f);
    }

    return std::make_shared<FloatData>(floats);
}