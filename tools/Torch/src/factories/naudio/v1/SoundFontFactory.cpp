#include "SoundFontFactory.h"
#include "AudioContext.h"
#include "Companion.h"
#include "spdlog/spdlog.h"
#include "utils/Decompressor.h"
#include <tinyxml2.h>
#include "DrumFactory.h"
#include "EnvelopeFactory.h"
#include "InstrumentFactory.h"

ExportResult SoundFontHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern SoundFont " << symbol << ";\n";

    return std::nullopt;
}

ExportResult SoundFontCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    return std::nullopt;
}

ExportResult SoundFontBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto data = std::static_pointer_cast<SoundFontData>(raw);

    WriteHeader(writer, Torch::ResourceType::SoundFont, 0);
    writer.Write(data->numInstruments);
    writer.Write(data->numDrums);
    writer.Write(data->sampleBankId1);
    writer.Write(data->sampleBankId2);

    for(auto& instrument : data->instruments){
        auto crc = AudioContext::GetPathByAddr(instrument);
        writer.Write(crc);
    }

    for(auto& drum : data->drums){
        auto crc = AudioContext::GetPathByAddr(drum);
        writer.Write(crc);
    }

    writer.Finish(write);
    return std::nullopt;
}

void WriteInstrument(tinyxml2::XMLElement* parent, uint32_t offset){
    auto instrument = std::static_pointer_cast<InstrumentData>(Companion::Instance->GetParseDataByAddr(offset)->data.value());
    auto envelopeData = std::static_pointer_cast<EnvelopeData>(Companion::Instance->GetParseDataByAddr(instrument->envelope)->data.value());

    tinyxml2::XMLElement* root = parent->InsertNewChildElement("Instrument");
    root->SetAttribute("NormalRangeLo", instrument->normalRangeLo);
    root->SetAttribute("NormalRangeHi", instrument->normalRangeHi);
    root->SetAttribute("ReleaseRate", instrument->adsrDecayIndex);

    tinyxml2::XMLElement* envelopes = root->InsertNewChildElement("Envelopes");
    for(size_t i = 0; i < envelopeData->points.size(); i++){
        auto point = envelopeData->points[i];
        tinyxml2::XMLElement* pointEntry = envelopes->InsertNewChildElement("Envelope");
        pointEntry->SetAttribute("Delay", point.delay);
        pointEntry->SetAttribute("Arg", point.arg);
        envelopes->InsertEndChild(pointEntry);
    }

    auto lowSample = instrument->lowPitchTunedSample;
    auto normSample = instrument->normalPitchTunedSample;
    auto highSample = instrument->highPitchTunedSample;

    if(lowSample.sample != 0 && lowSample.tuning != 0.0f){
        tinyxml2::XMLElement* low = root->InsertNewChildElement("LowNotesSound");
        low->SetAttribute("Tuning", lowSample.tuning);
        low->SetAttribute("SampleRef", (std::get<std::string>(Companion::Instance->GetNodeByAddr(lowSample.sample).value())).c_str());

        root->InsertEndChild(low);
    }

    if(normSample.sample != 0 && normSample.tuning != 0.0f) {
        tinyxml2::XMLElement* normal = root->InsertNewChildElement("NormalNotesSound");
        normal->SetAttribute("Tuning", normSample.tuning);
        normal->SetAttribute("SampleRef", (std::get<std::string>(Companion::Instance->GetNodeByAddr(normSample.sample).value())).c_str());

        root->InsertEndChild(normal);
    }

    if(highSample.sample != 0 && highSample.tuning != 0.0f) {
        tinyxml2::XMLElement* high = root->InsertNewChildElement("HighNotesSound");
        high->SetAttribute("Tuning", highSample.tuning);
        high->SetAttribute("SampleRef", (std::get<std::string>(Companion::Instance->GetNodeByAddr(highSample.sample).value())).c_str());

        root->InsertEndChild(high);
    }

    parent->InsertEndChild(root);
}

void WriteDrum(tinyxml2::XMLElement* parent, uint32_t offset){
    auto drum = std::static_pointer_cast<DrumData>(Companion::Instance->GetParseDataByAddr(offset)->data.value());
    auto envelopeData = std::static_pointer_cast<EnvelopeData>(Companion::Instance->GetParseDataByAddr(drum->envelope)->data.value());
    auto sample = drum->tunedSample;

    tinyxml2::XMLElement* root = parent->InsertNewChildElement("Drum");
    root->SetAttribute("ReleaseRate", drum->adsrDecayIndex);
    root->SetAttribute("Pan", drum->pan);
    root->SetAttribute("Loaded", 0);
    root->SetAttribute("SampleRef", (std::get<std::string>(Companion::Instance->GetNodeByAddr(sample.sample).value())).c_str());
    root->SetAttribute("Tuning", sample.tuning);

    tinyxml2::XMLElement* envelopes = root->InsertNewChildElement("Envelopes");
    envelopes->SetAttribute("Count", (uint32_t) envelopeData->points.size());
    for(size_t i = 0; i < envelopeData->points.size(); i++){
        auto point = envelopeData->points[i];
        tinyxml2::XMLElement* pointEntry = envelopes->InsertNewChildElement("Envelope");
        pointEntry->SetAttribute("Delay", point.delay);
        pointEntry->SetAttribute("Arg", point.arg);
        envelopes->InsertEndChild(pointEntry);
    }

    parent->InsertEndChild(root);
}

ExportResult SoundFontXMLExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto font = std::static_pointer_cast<SoundFontData>(raw);
    auto id = GetSafeNode<uint32_t>(node, "id");
    auto medium = (int8_t) GetSafeNode<int32_t>(node, "medium");
    auto policy = (int8_t) GetSafeNode<int32_t>(node, "policy");
    auto sd1 = (int16_t) GetSafeNode<int32_t>(node, "sd1");
    auto sd2 = (int16_t) GetSafeNode<int32_t>(node, "sd2");
    auto sd3 = (int16_t) GetSafeNode<int32_t>(node, "sd3");

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SoundFont");
    root->SetAttribute("Version", 0);
    root->SetAttribute("Num", id);
    root->SetAttribute("Medium", AudioContext::GetMediumStr(medium));
    root->SetAttribute("CachePolicy", AudioContext::GetCachePolicyStr(policy));
    root->SetAttribute("Data1", sd1);
    root->SetAttribute("Data2", sd2);
    root->SetAttribute("Data3", sd3);

    tinyxml2::XMLElement* drums = doc.NewElement("Drums");
    drums->SetAttribute("Count", font->numDrums);

    for(auto& drum : font->drums){
        if(drum == 0){
            continue;
        }
        WriteDrum(drums, drum);
    }
    root->InsertEndChild(drums);

    tinyxml2::XMLElement* insts = doc.NewElement("Instruments");
    insts->SetAttribute("Count", font->numInstruments);

    for(auto& inst : font->instruments){
        if(inst == 0){
            continue;
        }
        WriteInstrument(insts, inst);
    }
    root->InsertEndChild(insts);

    // This is ignored since it's an exclusive feature of OoT and MM
    tinyxml2::XMLElement* sfx = doc.NewElement("SfxTable");
    sfx->SetAttribute("Count", 0);
    root->InsertEndChild(sfx);

    doc.InsertEndChild(root);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);
    write.write(printer.CStr(), printer.CStrSize() - 1);

    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> SoundFontFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto entry = AudioContext::tables[AudioTableType::FONT_TABLE].entries[offset];
    auto reader = AudioContext::MakeReader(AudioTableType::FONT_TABLE, entry.addr);
    auto font = std::make_shared<SoundFontData>();

    font->numInstruments = (entry.shortData2 >> 8) & 0xFFu;
    font->numDrums = entry.shortData2 & 0xFFu;
    font->sampleBankId1 = (entry.shortData1 >> 8) & 0xFFu;
    font->sampleBankId2 = entry.shortData1 & 0xFFu;

    uint32_t drumBaseAddr = reader.ReadUInt32();
    uint32_t instBaseAddr = 4;

    if(drumBaseAddr != 0){
        reader.Seek(entry.addr + drumBaseAddr, LUS::SeekOffsetType::Start);
        for(size_t i = 0; i < font->numDrums; i++){
            uint32_t addr = reader.ReadUInt32();

            if(addr == 0){
                font->drums.push_back(0);
                continue;
            }

            // SPDLOG_INFO("Found Drum[{}] 0x{:X}", i, addr);

            YAML::Node drum;
            drum["type"] = "NAUDIO:V1:DRUM";
            drum["parent"] = entry.addr;
            drum["offset"] = entry.addr + addr;
            drum["sampleBankId"] = (uint32_t) font->sampleBankId1;

            Companion::Instance->AddAsset(drum);
            font->drums.push_back(entry.addr + addr);
        }

//        YAML::Node table;
//        table["type"] = "ASSET_ARRAY";
//        table["assetType"] = "Drum";
//        table["factoryType"] = "NAUDIO:V1:DRUM";
//        table["offset"] = entry.addr + drumBaseAddr;
//        table["count"] = (uint32_t) font->numDrums;
//        Companion::Instance->AddAsset(table);
    }

    reader.Seek(entry.addr + instBaseAddr, LUS::SeekOffsetType::Start);
    for(size_t i = 0; i < font->numInstruments; i++){
        uint32_t addr = reader.ReadUInt32();

        if(addr == 0){
            font->instruments.push_back(0);
            continue;
        }

        // SPDLOG_INFO("Found Instrument[{}] 0x{:X}", i, addr);

        YAML::Node instrument;
        instrument["type"] = "NAUDIO:V1:INSTRUMENT";
        instrument["parent"] = entry.addr;
        instrument["offset"] = entry.addr + addr;
        instrument["sampleBankId"] = (uint32_t) font->sampleBankId1;
        font->instruments.push_back(entry.addr + addr);

        Companion::Instance->AddAsset(instrument);
    }

//    YAML::Node table;
//    table["type"] = "ASSET_ARRAY";
//    table["assetType"] = "Instrument";
//    table["factoryType"] = "NAUDIO:V1:INSTRUMENT";
//    table["offset"] = entry.addr + instBaseAddr;
//    table["count"] = (uint32_t) font->numInstruments;
//    Companion::Instance->AddAsset(table);

    return font;
}
