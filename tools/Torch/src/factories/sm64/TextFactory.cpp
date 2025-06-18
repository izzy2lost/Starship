#include "TextFactory.h"
#include "utils/Decompressor.h"

ExportResult SM64::TextBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto data = std::static_pointer_cast<RawBuffer>(raw)->mBuffer;

    WriteHeader(writer, Torch::ResourceType::Blob, 0);
    writer.Write((uint32_t) data.size());
    writer.Write((char*) data.data(), data.size());
    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> SM64::TextFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {

    std::vector<uint8_t> text;
    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    size_t idx = 0;

    while(segment.data[idx] != 0xFF){
        auto c = segment.data[idx++];
        text.push_back(c);
    }
    text.push_back(0xFF);

    return std::make_shared<RawBuffer>(text);
}