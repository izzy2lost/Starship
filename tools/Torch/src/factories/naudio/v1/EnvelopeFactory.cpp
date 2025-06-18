#include "EnvelopeFactory.h"
#include "utils/Decompressor.h"
#include "Companion.h"

ExportResult EnvelopeHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern Envelope " << symbol << ";\n";

    return std::nullopt;
}

ExportResult EnvelopeCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    return std::nullopt;
}

ExportResult EnvelopeBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto data = std::static_pointer_cast<EnvelopeData>(raw);

    WriteHeader(writer, Torch::ResourceType::Envelope, 0);
    writer.Write((uint32_t) data->points.size());
    for(auto& point : data->points){
        writer.Write(point.delay);
        writer.Write(point.arg);
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> EnvelopeFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto reader = AudioContext::MakeReader(AudioTableType::FONT_TABLE, offset);

    std::vector<EnvelopePoint> temp;
    while(true) {
        int16_t delay = BSWAP16(reader.ReadInt16());
        int16_t arg   = BSWAP16(reader.ReadInt16());

        temp.push_back({delay, arg});

        if (1 <= (-delay) % (1 << 16) && (-delay) % (1 << 16) <= 3){
            break;
        }
    }

    return std::make_shared<EnvelopeData>(temp);
}
