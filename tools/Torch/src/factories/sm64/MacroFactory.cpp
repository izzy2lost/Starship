#include "MacroFactory.h"
#include "spdlog/spdlog.h"

#include "Companion.h"
#include "utils/Decompressor.h"
#include "utils/TorchUtils.h"
#include <regex>

ExportResult SM64::MacroHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern MacroObject " << symbol << "[];\n";
    return std::nullopt;
}

ExportResult SM64::MacroCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");

    auto macro = std::static_pointer_cast<SM64::MacroData>(raw);

    write << "const MacroObject " << symbol << "[] = {\n";

    for (auto &object : macro->mMacroData) {
        write << fourSpaceTab;

        if (object.behParam == 0) {
            write << "MACRO_OBJECT(";
        } else {
            write << "MACRO_OBJECT_WITH_BEH_PARAM(";
        }

        write << object.preset << ", ";
        write << (object.yaw / 1.41) << ", ";
        write << object.posX << ", ";
        write << object.posY << ", ";
        write << object.posZ;

        if (object.behParam != 0) {
            write << ", " << object.behParam;
        }
        write << "),\n";
    }

    write << fourSpaceTab << "MACRO_OBJECT_END(),";

    write << "\n};\n";

    size_t size = (macro->mMacroData.size()) * 5 * sizeof(int16_t) + 1;

    return offset + size;
}

ExportResult SM64::MacroBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto macro = std::static_pointer_cast<SM64::MacroDataAlt>(raw);

    WriteHeader(writer, Torch::ResourceType::MacroObject, 0);

    writer.Write((uint32_t) macro->mMacroData.size());

    for(auto &entry : macro->mMacroData){
        writer.Write(entry);
    }
    /*
    for (auto &object : macro->mMacroData) {
        writer.Write(static_cast<int16_t>(object.preset));
        writer.Write(object.yaw);
        writer.Write(object.posX);
        writer.Write(object.posY);
        writer.Write(object.posZ);
        writer.Write(object.behParam);
    }
    */
    
    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> SM64::MacroFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    LUS::BinaryReader reader(segment.data, segment.size);
    reader.SetEndianness(Torch::Endianness::Big);

    std::vector<int16_t> entries;

    while(true) {
        int16_t raw = reader.ReadInt16();
        if(raw == 0x1E){
            break;
        }

        entries.push_back(raw);
    }

    entries.push_back(0x1E);

    /*
       std::vector<MacroObject> macroData;
       while (true) {
            int16_t presetID = reader.ReadInt16();

            if(presetID < 0) {
                break;
            }

            int16_t yRot = reader.ReadInt16();
            int16_t xObj = reader.ReadInt16();
            int16_t yObj = reader.ReadInt16();
            int16_t zObj = reader.ReadInt16();
            int16_t params = reader.ReadInt16();

            macroData.emplace_back(static_cast<MacroPresets>(presetID), yRot, xObj, yObj, zObj, params);
        }

        macroData.emplace_back(MacroPresets::macro_invalid, -1, -1, -1, -1, -1);

        return std::make_shared<SM64::MacroData>(macroData);
    */

    return std::make_shared<SM64::MacroDataAlt>(entries);
}
