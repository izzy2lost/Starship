#include "MessageLookupFactory.h"

#include "utils/Decompressor.h"
#include "spdlog/spdlog.h"
#include "Companion.h"
#include <tinyxml2.h>

ExportResult SF64::MessageLookupHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern MsgLookup " << symbol << "[];\n";
    return std::nullopt;
}

ExportResult SF64::MessageLookupCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto table = std::static_pointer_cast<MessageTable>(raw)->mTable;
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    auto offset = GetSafeNode<uint32_t>(node, "offset");

    write << "// clang-format on\n";
    write << "MsgLookup " << symbol << "[] = {\n" << fourSpaceTab;
    for (int i = 0; i < table.size(); ++i) {
        auto m = table[i];
        if(i % 4 == 0 && i != 0){
            write << "\n" << fourSpaceTab;
        }

        auto dec = Companion::Instance->GetNodeByAddr(m.ptr);
        std::string msgSymbol = "NULL";

        if(dec.has_value()){
            auto node = std::get<1>(dec.value());
            msgSymbol = GetSafeNode<std::string>(node, "symbol");
        }

        write << "{ " << m.id << std::dec << ", " << msgSymbol << " }, ";
    }

    write << "\n};\n";
    return offset + table.size() * sizeof(uint16_t);
}

ExportResult SF64::MessageLookupXMLExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto table = std::static_pointer_cast<MessageTable>(raw)->mTable;
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    tinyxml2::XMLPrinter printer;
    tinyxml2::XMLDocument lookup;
    tinyxml2::XMLElement* root = lookup.NewElement("MessageTable");
    root->SetAttribute("Size", (int) table.size());

    *replacement += ".meta";

    for (auto m : table) {
        auto item = lookup.NewElement("Entry");
        auto dec = Companion::Instance->GetNodeByAddr(m.ptr);
        std::string ref = "None";

        if(dec.has_value()){
            ref = std::get<0>(dec.value());
        }

        item->SetAttribute("Ref", ref.c_str());
        root->InsertEndChild(item);
    }

    lookup.InsertEndChild(root);
    lookup.Accept(&printer);
    write.write(printer.CStr(), printer.CStrSize() - 1);
    return std::nullopt;
}

ExportResult SF64::MessageLookupBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    const auto data = std::static_pointer_cast<MessageTable>(raw);

    WriteHeader(writer, Torch::ResourceType::MessageTable, 0);

    const auto count = data->mTable.size();
    SPDLOG_INFO("Message Count: {}", count);
    writer.Write(static_cast<uint32_t>(count));
    for(auto m : data->mTable) {
        writer.Write(m.id);
        auto dec = Companion::Instance->GetNodeByAddr(m.ptr);
        if(dec.has_value()){
            std::string path = std::get<0>(dec.value());
            SPDLOG_INFO("Message ID: {} Ptr: {:X} Path: {}", m.id, m.ptr, path);
            writer.Write(CRC64(path.c_str()));
        } else {
            writer.Write((uint64_t) 0);
            SPDLOG_WARN("Failed to find message ID: {} Ptr: {:X}", m.id, m.ptr);
        }
    }
    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> SF64::MessageLookupFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    const auto vram = GetSafeNode<uint32_t>(node, "vram");
    const auto offset = GetSafeNode<uint32_t>(node, "offset");
    std::vector<MessageEntry> message;

    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    LUS::BinaryReader reader(segment.data, segment.size);
    reader.SetEndianness(Torch::Endianness::Big);

    int32_t id = reader.ReadInt32();
    uint32_t ptr = reader.ReadInt32();

    while(id != -1) {

        YAML::Node entry;

        ptr = (SEGMENT_NUMBER(offset) << 24) | (ptr - vram);
        std::string output = "gMsg_ID_" + std::to_string(id);
        entry["type"] = "SF64:MESSAGE";
        entry["offset"] = ptr;
        entry["symbol"] = output;
        entry["autogen"] = true;
        SPDLOG_INFO("Message ID: {} Ptr: {:X} Offset: {:X}", id, ptr, (SEGMENT_NUMBER(offset) << 24) | (ptr - vram));

        Companion::Instance->AddAsset(entry);
        message.push_back({id, ptr});
        id = reader.ReadInt32();
        ptr = reader.ReadInt32();
    }

    message.push_back({-1, 0});

    return std::make_shared<MessageTable>(message);
}