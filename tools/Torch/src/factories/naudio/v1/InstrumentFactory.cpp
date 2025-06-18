#include "InstrumentFactory.h"
#include "utils/Decompressor.h"
#include "Companion.h"
#include "EnvelopeFactory.h"
#include <tinyxml2.h>

ExportResult InstrumentHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern Instrument " << symbol << ";\n";

    return std::nullopt;
}

ExportResult InstrumentCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    return std::nullopt;
}

ExportResult InstrumentBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto data = std::static_pointer_cast<InstrumentData>(raw);

    WriteHeader(writer, Torch::ResourceType::Instrument, 0);
    writer.Write(data->isRelocated);
    writer.Write(data->normalRangeLo);
    writer.Write(data->normalRangeHi);
    writer.Write(data->adsrDecayIndex);
    writer.Write(AudioContext::GetPathByAddr(data->envelope));
    writer.Write(AudioContext::GetPathByAddr(data->lowPitchTunedSample.sample));
    writer.Write(data->lowPitchTunedSample.tuning);
    writer.Write(AudioContext::GetPathByAddr(data->normalPitchTunedSample.sample));
    writer.Write(data->normalPitchTunedSample.tuning);
    writer.Write(AudioContext::GetPathByAddr(data->highPitchTunedSample.sample));
    writer.Write(data->highPitchTunedSample.tuning);

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> InstrumentFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto parent = GetSafeNode<uint32_t>(node, "parent");
    auto sampleBankId = GetSafeNode<uint32_t>(node, "sampleBankId");

    auto entry = AudioContext::tables[AudioTableType::FONT_TABLE].entries[parent];

    auto reader = AudioContext::MakeReader(AudioTableType::FONT_TABLE, offset);

    auto instrument = std::make_shared<InstrumentData>();
    instrument->isRelocated = reader.ReadUByte();
    instrument->normalRangeLo = reader.ReadUByte();
    instrument->normalRangeHi = reader.ReadUByte();
    instrument->adsrDecayIndex = reader.ReadUByte();

    auto envAddr = entry.addr + reader.ReadUInt32();
    YAML::Node envelope;
    envelope["type"] = "NAUDIO:V1:ENVELOPE";
    envelope["offset"] = envAddr;
    instrument->envelope = envAddr;
    Companion::Instance->AddAsset(envelope);
    
    instrument->lowPitchTunedSample = AudioContext::LoadTunedSample(reader, parent, sampleBankId);
    instrument->normalPitchTunedSample = AudioContext::LoadTunedSample(reader, parent, sampleBankId);
    instrument->highPitchTunedSample = AudioContext::LoadTunedSample(reader, parent, sampleBankId);

    return instrument;
}
