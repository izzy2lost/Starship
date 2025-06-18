#include "DrumFactory.h"
#include "utils/Decompressor.h"
#include "Companion.h"

ExportResult DrumHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern Drum " << symbol << ";\n";

    return std::nullopt;
}

ExportResult DrumCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    return std::nullopt;
}

ExportResult DrumBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto data = std::static_pointer_cast<DrumData>(raw);

    WriteHeader(writer, Torch::ResourceType::Drum, 0);
    writer.Write(data->adsrDecayIndex);
    writer.Write(data->pan);
    writer.Write(data->isRelocated);
    writer.Write(AudioContext::GetPathByAddr(data->tunedSample.sample));
    writer.Write(data->tunedSample.tuning);
    writer.Write(AudioContext::GetPathByAddr(data->envelope));

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> DrumFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto parent = GetSafeNode<uint32_t>(node, "parent");
    auto sampleBankId = GetSafeNode<uint32_t>(node, "sampleBankId");

    auto entry = AudioContext::tables[AudioTableType::FONT_TABLE].entries[parent];

    auto reader = AudioContext::MakeReader(AudioTableType::FONT_TABLE, offset);

    auto drum = std::make_shared<DrumData>();
    drum->adsrDecayIndex = reader.ReadInt8();
    drum->pan = reader.ReadInt8();
    drum->isRelocated = reader.ReadUByte();
    reader.ReadUByte();;

    drum->tunedSample = AudioContext::LoadTunedSample(reader, parent, sampleBankId);

    auto envAddr = entry.addr + reader.ReadUInt32();
    YAML::Node envelope;
    envelope["type"] = "NAUDIO:V1:ENVELOPE";
    envelope["offset"] = envAddr;
    drum->envelope = envAddr;
    Companion::Instance->AddAsset(envelope);

    return drum;
}
