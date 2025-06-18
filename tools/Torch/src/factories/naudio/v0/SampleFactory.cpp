#include "SampleFactory.h"

#include <vector>
#include "Companion.h"
#include "AIFCDecode.h"
#include "spdlog/spdlog.h"
#include <factories/naudio/v1/AudioConverter.h>

ExportResult SampleModdingExporter::Export(std::ostream& writer, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node& node, std::string* replacement) {
   auto sample = std::static_pointer_cast<SampleData>(raw);
   *replacement += ".aiff";

    LUS::BinaryWriter aifc = LUS::BinaryWriter();
    AudioConverter::SampleV0ToAIFC(&sample->mSample, aifc);
    
    LUS::BinaryWriter aiff = LUS::BinaryWriter();
    write_aiff(aifc.ToVector(), aiff);
    aifc.Close();
    aiff.Finish(writer);
    return std::nullopt;
}

ExportResult SampleBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto sample = std::static_pointer_cast<SampleData>(raw)->mSample;

    WriteHeader(writer, Torch::ResourceType::Sample, 0);
    writer.Write(sample.loop.start);
    writer.Write(sample.loop.end);
    writer.Write(sample.loop.count);
    writer.Write(sample.loop.pad);

    if(sample.loop.state.has_value()){
        auto state = sample.loop.state.value();
        writer.Write(static_cast<uint32_t>(state.size()));
        writer.Write(reinterpret_cast<char*>(state.data()), state.size() * sizeof(int16_t));
    } else {
        writer.Write(static_cast<uint32_t>(0));
    }

    writer.Write(sample.book.order);
    writer.Write(sample.book.npredictors);

    writer.Write(static_cast<uint32_t>(sample.book.table.size()));
    writer.Write(reinterpret_cast<char*>(sample.book.table.data()), sample.book.table.size() * sizeof(int16_t));

    writer.Write(static_cast<int32_t>(sample.data.size()));
    writer.Write(reinterpret_cast<char*>(sample.data.data()), sample.data.size());

    writer.Write(sample.name);

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> SampleFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& data) {
    const auto id = data["id"].as<int32_t>();
    if(AudioManager::Instance == nullptr){
        throw std::runtime_error("AudioManager not initialized");
    }
    AudioBankSample entry = AudioManager::Instance->get_aifc(id);
    return std::make_shared<SampleData>(entry);
}