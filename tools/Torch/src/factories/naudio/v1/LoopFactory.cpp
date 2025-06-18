#include "LoopFactory.h"
#include "utils/Decompressor.h"
#include "Companion.h"

ExportResult ADPCMLoopHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern AdpcmLoop " << symbol << ";\n";

    return std::nullopt;
}

ExportResult ADPCMLoopCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    return std::nullopt;
}

ExportResult ADPCMLoopBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto data = std::static_pointer_cast<ADPCMLoopData>(raw);

    WriteHeader(writer, Torch::ResourceType::AdpcmLoop, 0);
    writer.Write(data->start);
    writer.Write(data->end);
    writer.Write(data->count);
    if(data->count != 0){
        for(size_t i = 0; i < 16; i++){
            writer.Write(data->predictorState[i]);
        }
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> ADPCMLoopFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto reader = AudioContext::MakeReader(AudioTableType::FONT_TABLE, offset);
    auto loop = std::make_shared<ADPCMLoopData>();

    loop->start = reader.ReadInt32();
    loop->end = reader.ReadUInt32();
    loop->count = reader.ReadUInt32();

    if(loop->count != 0){
        for(size_t i = 0; i < 16; i++){
            loop->predictorState[i] = reader.ReadInt16();
        }
    }

    return loop;
}
