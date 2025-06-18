#include "AudioTableFactory.h"
#include "utils/Decompressor.h"
#include "spdlog/spdlog.h"
#include "AudioContext.h"
#include "Companion.h"
#include <iomanip>

#define INT2(c) std::dec << std::setfill(' ') << c
#define HEX(c) "0x" << std::hex << std::setw(4) << std::setfill('0') << c
#define ADDR(c) "0x" << std::hex << std::setw(5) << std::setfill('0') << c

static const std::unordered_map <std::string, AudioTableType> gTableTypes = {
    { "SOUNDFONT", AudioTableType::FONT_TABLE },
    { "SAMPLE", AudioTableType::SAMPLE_TABLE },
    { "SEQUENCE", AudioTableType::SEQ_TABLE }
};

ExportResult AudioTableHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern AudioTable " << symbol << ";\n";
    
    return std::nullopt;
}

ExportResult AudioTableCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto table = std::static_pointer_cast<AudioTableData>(raw);

    if(table->type == AudioTableType::FONT_TABLE){
        write << "#define SOUNDFONT_ENTRY(offset, size, medium, cachePolicy, bank1, bank2, numInst, numDrums) { \\\n";
        write << fourSpaceTab << "offset, size, medium, cachePolicy, (((bank1) &0xFF) << 8) | ((bank2) &0xFF),              \\\n";
        write << fourSpaceTab << fourSpaceTab << "(((numInst) &0xFF) << 8) | ((numDrums) &0xFF)                                         \\\n}\n\n";
    }

    write << "AudioTable " << symbol << " = {\n";
    write << fourSpaceTab << "{ " << table->entries.size() << ", " << table->medium << ", " << table->addr << " },\n";
    write << fourSpaceTab << "{ \n";

    for(size_t i = 0; i < table->entries.size(); i++){
        auto &entry = table->entries[i];

        if(table->type == AudioTableType::FONT_TABLE) {
            uint32_t numInstruments = (entry.shortData2 >> 8) & 0xFFu;
            uint32_t numDrums = entry.shortData2 & 0xFFu;
            uint32_t sampleBankId1 = (entry.shortData1 >> 8) & 0xFFu;
            uint32_t sampleBankId2 = entry.shortData1 & 0xFFu;

            write << fourSpaceTab << fourSpaceTab << "SOUNDFONT_ENTRY(" << ADDR(entry.addr) << ", " << HEX(entry.size) << ", " << INT2((uint32_t) entry.medium) << ", " << INT2((uint32_t) entry.cachePolicy) << ", " << INT2(numInstruments) << ", " << INT2(numDrums) << ", " << INT2(sampleBankId1) << ", " << INT2(sampleBankId2) << "),\n" ;
        } else {
            write << fourSpaceTab << fourSpaceTab << "{ " << ADDR(entry.addr) << ", " << HEX(entry.size) << ", " << (uint32_t) entry.medium << ", " << (uint32_t) entry.cachePolicy << " },\n";
        }
    }

    write << fourSpaceTab << "},\n";
    write << "};\n";

    if (Companion::Instance->IsDebug()) {
        write << "// Count: " << table->entries.size() << "\n";
    }

    return offset + 0x10 + (table->entries.size() * 0x10);
}

ExportResult AudioTableBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto data = std::static_pointer_cast<AudioTableData>(raw);

    WriteHeader(writer, Torch::ResourceType::AudioTable, 0);
    writer.Write(data->medium);
    writer.Write(data->addr);
    writer.Write((uint32_t) data->entries.size());

    for(auto& entry : data->entries){
        if(data->type == AudioTableType::FONT_TABLE && entry.crc == 0){
            throw std::runtime_error("Fuck this thing");
        }
        if(entry.size == 0){
            auto item = AudioContext::tables[AudioTableType::SEQ_TABLE].info->entries[entry.addr];
            writer.Write(item.crc);
        } else {
            writer.Write(entry.crc);
        }
        writer.Write(entry.size);
        writer.Write(entry.medium);
        writer.Write(entry.cachePolicy);
        writer.Write(entry.shortData1);
        writer.Write(entry.shortData2);
        writer.Write(entry.shortData3);
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> AudioTableFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    // Parse table entry
    auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    LUS::BinaryReader reader(segment.data, segment.size);
    reader.SetEndianness(Torch::Endianness::Big);

    auto format = GetSafeNode<std::string>(node, "format");
    std::transform(format.begin(), format.end(), format.begin(), ::toupper);

    if(!gTableTypes.contains(format)) {
        return std::nullopt;
    }

    auto type = gTableTypes.at(format);
    auto count = reader.ReadInt16();
    auto bmedium = reader.ReadInt16();
    auto baddr = reader.ReadUInt32();
    reader.Read(0x8); // PAD

    SPDLOG_INFO("count: {}", count);
    SPDLOG_INFO("medium: {}", bmedium);
    SPDLOG_INFO("addr: {}", baddr);

    std::vector<AudioTableEntry> entries;

    for(size_t i = 0; i < count; i++){
        auto addr = reader.ReadUInt32();
        auto size = reader.ReadUInt32();
        auto medium = reader.ReadInt8();
        auto policy = reader.ReadInt8();
        auto sd1 = reader.ReadInt16();
        auto sd2 = reader.ReadInt16();
        auto sd3 = reader.ReadInt16();
        uint64_t crc = 0;

        auto entry = AudioTableEntry { addr, size, medium, policy, sd1, sd2, sd3, crc };
        AudioContext::tables[type].entries[addr] = entry;

        switch (type) {
            case AudioTableType::FONT_TABLE: {
                YAML::Node font;
                font["type"] = "NAUDIO:V1:SOUND_FONT";
                font["offset"] = addr;
                font["id"] = (uint32_t) i;
                font["medium"] = (int32_t) medium;
                font["policy"] = (int32_t) policy;
                font["sd1"] = (int32_t) sd1;
                font["sd2"] = (int32_t) sd2;
                font["sd3"] = (int32_t) sd3;

                Companion::Instance->AddAsset(font);
                std::string path = font["vpath"].as<std::string>();
                crc = CRC64(path.c_str());
                break;
            }
            case AudioTableType::SEQ_TABLE: {
                auto parent = AudioContext::tables[AudioTableType::SEQ_TABLE].offset;
                if(size != 0){
                    YAML::Node seq;
                    seq["type"] = "NAUDIO:V1:SEQUENCE";
                    seq["offset"] = parent + addr;
                    seq["size"] = size;
                    Companion::Instance->AddAsset(seq);
                    auto path = seq["vpath"].as<std::string>();
                    crc = CRC64(path.c_str());
                    break;
                }
            }
        }

        entry.crc = crc;
        AudioContext::tables[type].entries[addr].crc = crc;
        entries.push_back(entry);
    }

    auto table = std::make_shared<AudioTableData>(bmedium, offset, type, entries);
    AudioContext::tables[type].info = table;
    return table;
}
