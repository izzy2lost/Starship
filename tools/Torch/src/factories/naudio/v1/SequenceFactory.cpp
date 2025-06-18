#include "SequenceFactory.h"
#include "Companion.h"
#include "utils/Decompressor.h"
#include "AudioContext.h"

ExportResult NSequenceHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern u8 " << symbol << "[];\n";

    return std::nullopt;
}

ExportResult NSequenceCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto symbol = GetSafeNode(node, "symbol", entryName);
    auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto data = std::static_pointer_cast<RawBuffer>(raw)->mBuffer;

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << GetSafeNode<std::string>(node, "ctype", "u8") << " " << symbol << "[] = {\n" << tab_t;

    for (int i = 0; i < data.size(); i++) {
        if ((i % 15 == 0) && i != 0) {
            write << "\n" << tab_t;
        }

        write << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int) data[i] << ", ";
    }
    write << "\n};\n";

    if (Companion::Instance->IsDebug()) {
        write << "// size: 0x" << std::hex << std::uppercase << data.size() << "\n";
    }

    return offset + data.size();
}

ExportResult NSequenceModdingExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto data = std::static_pointer_cast<RawBuffer>(raw)->mBuffer;
    *replacement += ".m64";
    writer.Write((char*) data.data(), data.size());
    writer.Finish(write);
    return std::nullopt;
}

ExportResult NSequenceBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto data = std::static_pointer_cast<RawBuffer>(raw)->mBuffer;

    // Its the same shit as a blob
    WriteHeader(writer, Torch::ResourceType::Blob, 0);
    writer.Write((uint32_t) data.size());
    writer.Write((char*) data.data(), data.size());
    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> NSequenceFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto size = GetSafeNode<size_t>(node, "size");
    auto [_, segment] = Decompressor::AutoDecode(node, buffer, size);
    return std::make_shared<RawBuffer>(segment.data, segment.size);
}

std::optional<std::shared_ptr<IParsedData>> NSequenceFactory::parse_modding(std::vector<uint8_t>& buffer, YAML::Node& node) {
    return std::make_shared<RawBuffer>(buffer.data(), buffer.size());
}
