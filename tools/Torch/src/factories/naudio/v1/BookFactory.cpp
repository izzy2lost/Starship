#include "BookFactory.h"
#include "utils/Decompressor.h"
#include "Companion.h"

ExportResult ADPCMBookHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern AdpcmBook " << symbol << ";\n";

    return std::nullopt;
}

ExportResult ADPCMBookCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    return std::nullopt;
}

ExportResult ADPCMBookBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto data = std::static_pointer_cast<ADPCMBookData>(raw);

    WriteHeader(writer, Torch::ResourceType::AdpcmBook, 0);
    writer.Write(data->order);
    writer.Write(data->numPredictors);
    writer.Write((uint32_t) data->book.size());

    for(auto& page : data->book){
        writer.Write(page);
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> ADPCMBookFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto reader = AudioContext::MakeReader(AudioTableType::FONT_TABLE, offset);

    auto book = std::make_shared<ADPCMBookData>();

    book->order = reader.ReadInt32();
    book->numPredictors = reader.ReadInt32();
    size_t length = 8 * book->order * book->numPredictors;

    for(size_t i = 0; i < length; i++){
        book->book.push_back(reader.ReadInt16());
    }

    return book;
}
