#include "SampleFactory.h"
#include "AudioConverter.h"
#include "Companion.h"
#include <tinyxml2.h>
#include "LoopFactory.h"
#include "BookFactory.h"
#include <factories/sf64/audio/AudioDecompressor.h>
#include <factories/naudio/v0/AIFCDecode.h>

ExportResult NSampleHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern Sample " << symbol << ";\n";

    return std::nullopt;
}

ExportResult NSampleCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    return std::nullopt;
}

ExportResult NSampleBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto data = std::static_pointer_cast<NSampleData>(raw);

    WriteHeader(writer, Torch::ResourceType::Sample, 1);
    writer.Write((uint8_t) data->codec);
    writer.Write((uint8_t) data->medium);
    writer.Write((uint8_t) data->unk);
    writer.Write((uint32_t) data->size);

    writer.Write(AudioContext::GetPathByAddr(data->loop));
    writer.Write(AudioContext::GetPathByAddr(data->book));

    auto table = AudioContext::tables[AudioTableType::SAMPLE_TABLE];
    writer.Write((char*) table.buffer.data() + table.info->entries[data->sampleBankId].addr + data->sampleAddr, data->size);

    writer.Finish(write);
    return std::nullopt;
}

ExportResult NSampleModdingExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto aiff = LUS::BinaryWriter();
    auto data = std::static_pointer_cast<NSampleData>(raw);

#ifdef SF64_SUPPORT
    if(AudioContext::driver == NAudioDrivers::SF64 && data->codec == 2) {
        *replacement += ".pcm";
        auto table = AudioContext::tables[AudioTableType::SAMPLE_TABLE];
        auto ptr = table.buffer.data() + table.info->entries[data->sampleBankId].addr + data->sampleAddr;
        auto vec = std::vector<uint8_t>(ptr, ptr + data->size);
        auto output = new int16_t[data->size * 2];
        SF64::DecompressAudio(vec, output);
        auto writer = LUS::BinaryWriter();
        writer.Write((char*) output, data->size);
        writer.Finish(write);
    } else {
#endif
        *replacement += ".aiff";
        auto aifc = LUS::BinaryWriter();
        AudioConverter::SampleV1ToAIFC(data.get(), aifc);
        auto cnv = aifc.ToVector();

        if(!cnv.empty()){
            write_aiff(cnv, aiff);
            aiff.Finish(write);
        }
#ifdef SF64_SUPPORT
    }
#endif


    return std::nullopt;
}

ExportResult NSampleXMLExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto entry = std::static_pointer_cast<NSampleData>(raw);

    auto path = fs::path(*replacement);
    tinyxml2::XMLDocument sample;
    tinyxml2::XMLElement* root = sample.NewElement("Sample");
    root->SetAttribute("Version", 0);
    root->SetAttribute("Codec", AudioContext::GetCodecStr(entry->codec));
    root->SetAttribute("Medium", AudioContext::GetMediumStr(entry->medium));
    root->SetAttribute("bit26", entry->unk);
    root->SetAttribute("Tuning", entry->tuning);
    root->SetAttribute("Size", entry->size);
    root->SetAttribute("Relocated", 0);
    root->SetAttribute("Path", (path.string() + "_data").c_str());

    if(entry->loop != 0) {
        auto loop = std::static_pointer_cast<ADPCMLoopData>(Companion::Instance->GetParseDataByAddr(entry->loop)->data.value());
        tinyxml2::XMLElement* adpcmLoop = sample.NewElement("ADPCMLoop");
        adpcmLoop->SetAttribute("Start", loop->start);
        adpcmLoop->SetAttribute("End", loop->end);
        adpcmLoop->SetAttribute("Count", loop->count);
        if (loop->count != 0) {
            for (auto& state : loop->predictorState) {
                tinyxml2::XMLElement* loopEntry = adpcmLoop->InsertNewChildElement("Predictor");
                loopEntry->SetAttribute("State", state);
                adpcmLoop->InsertEndChild(loopEntry);
            }
        }
        root->InsertEndChild(adpcmLoop);
    }
    
    if(entry->book != 0) {
        auto book = std::static_pointer_cast<ADPCMBookData>(Companion::Instance->GetParseDataByAddr(entry->book)->data.value());
        tinyxml2::XMLElement* adpcmBook = sample.NewElement("ADPCMBook");
        adpcmBook->SetAttribute("Order", book->order);
        adpcmBook->SetAttribute("Npredictors", book->numPredictors);

        for (auto& page : book->book) {
            tinyxml2::XMLElement* bookEntry = adpcmBook->InsertNewChildElement("Book");
            bookEntry->SetAttribute("Page", page);
            adpcmBook->InsertEndChild(bookEntry);
        }
        root->InsertEndChild(adpcmBook);
    }
    sample.InsertEndChild(root);

    tinyxml2::XMLPrinter printer;
    sample.Accept(&printer);
    write.write(printer.CStr(), printer.CStrSize() - 1);

    auto table = AudioContext::tables[AudioTableType::SAMPLE_TABLE];
    auto sampleData = table.buffer.data() + table.info->entries[entry->sampleBankId].addr + entry->sampleAddr;
    std::vector<char> data(sampleData, sampleData + entry->size);
    Companion::Instance->RegisterCompanionFile(path.filename().string() + "_data", data);

    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> NSampleFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto parent = GetSafeNode<uint32_t>(node, "parent");
    auto tuning = GetSafeNode<float>(node, "tuning", 0.0f);
    auto sampleRate = GetSafeNode<uint32_t>(node, "sampleRate", 0);
    auto sampleBankId = GetSafeNode<uint32_t>(node, "sampleBankId");

    auto table = AudioContext::tables[AudioTableType::FONT_TABLE].entries[parent];
    auto reader = AudioContext::MakeReader(AudioTableType::FONT_TABLE, offset);

    auto sample = std::make_shared<NSampleData>();

    uint32_t flags = reader.ReadUInt32();
    uint32_t addr = reader.ReadUInt32();

    sample->codec = (flags >> 28) & 0x0F;
    sample->medium = (flags >> 24) & 0x03;
    sample->unk = (flags >> 22) & 0x01;
    sample->size = flags;

    auto loopAddr = reader.ReadUInt32();
    auto bookAddr = reader.ReadUInt32();

    if(loopAddr != 0){
        loopAddr += table.addr;
        YAML::Node loop;
        loop["type"] = "NAUDIO:V1:ADPCM_LOOP";
        loop["offset"] = loopAddr;
        Companion::Instance->AddAsset(loop);
    }

    if(bookAddr != 0){
        bookAddr += table.addr;
        YAML::Node book;
        book["type"] = "NAUDIO:V1:ADPCM_BOOK";
        book["offset"] = bookAddr;
        Companion::Instance->AddAsset(book);
    }

    sample->loop = loopAddr;
    sample->book = bookAddr;

    sample->sampleAddr = addr;
    sample->tuning = tuning;
    sample->sampleBankId = sampleBankId;
    sample->sampleRate = sampleRate;

    return sample;
}
