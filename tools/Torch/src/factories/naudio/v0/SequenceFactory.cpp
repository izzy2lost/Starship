#include "SequenceFactory.h"

ExportResult SequenceBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    const auto sequence = std::static_pointer_cast<SequenceData>(raw);

    WriteHeader(writer, Torch::ResourceType::Sequence, 0);
    writer.Write(sequence->mId);
    writer.Write(static_cast<uint32_t>(sequence->mBanks.size()));
    for (auto& bank : sequence->mBanks) {
        writer.Write(bank);
    }
    writer.Write( sequence->mSize);
    writer.Write(reinterpret_cast<char*>(sequence->mBuffer.data()), sequence->mSize);
    writer.Finish(write);
    return std::nullopt;
}

ExportResult SequenceModdingExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    const auto sequence = std::static_pointer_cast<SequenceData>(raw);
    *replacement += ".m64";
    
    writer.Write(reinterpret_cast<char*>(sequence->mBuffer.data()), sequence->mSize);
    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> SequenceFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& data) {
    auto id = data["id"].as<uint32_t>();
    auto size = data["size"].as<size_t>();
    const auto offset = data["offset"].as<size_t>();
    auto banks = data["banks"].as<std::vector<std::string>>();
    return std::make_shared<SequenceData>(id, size, buffer.data() + offset, banks);
}

std::optional<std::shared_ptr<IParsedData>> SequenceFactory::parse_modding(std::vector<uint8_t>& buffer, YAML::Node& node) {
    return std::make_shared<RawBuffer>(buffer.data(), buffer.size());
}