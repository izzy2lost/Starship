#include "DictionaryFactory.h"

#include "spdlog/spdlog.h"

ExportResult SM64::DictionaryBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    const auto data = std::static_pointer_cast<DictionaryData>(raw);

    WriteHeader(writer, Torch::ResourceType::Dictionary, 0);

    writer.Write(static_cast<uint32_t>(data->mDictionary.size()));
    for(auto& [key, value] : data->mDictionary){
        writer.Write(key);
        writer.Write(static_cast<uint32_t>(value.size()));
        writer.Write(reinterpret_cast<char*>(value.data()), value.size());
    }
    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> SM64::DictionaryFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& data) {
    std::unordered_map<std::string, std::vector<uint8_t>> dictionary;

    for (auto it = data["keys"].begin(); it != data["keys"].end(); ++it) {
        auto key = it->first.as<std::string>();
        auto offset = it->second.as<size_t>();

        std::vector<uint8_t> text;
        const auto bytes = buffer.data();

        while(bytes[offset] != 0xFF){
            auto c = bytes[offset++];
            text.push_back(c);
        }

        text.push_back(0xFF);
        dictionary[key] = text;

        SPDLOG_INFO("Found key: {} at offset {}", key, offset);
    }

    return std::make_shared<DictionaryData>(dictionary);
}