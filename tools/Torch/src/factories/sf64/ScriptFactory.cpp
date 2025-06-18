#include "ScriptFactory.h"
#include "spdlog/spdlog.h"

#include "Companion.h"
#include "utils/Decompressor.h"
#include "utils/TorchUtils.h"
#include "factories/sf64/MessageFactory.h"
#include <tinyxml2.h>
#include <regex>

#include "archive/SWrapper.h"

#define CLAMP_MAX(val, max) (((val) < (max)) ? (val) : (max))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define VALUE_TO_ENUM(val, enumname, fallback) (Companion::Instance->GetEnumFromValue(enumname, val).value_or("/*" + std::string(fallback) + " */ " + std::to_string(val)));

SF64::ScriptData::ScriptData(std::vector<uint32_t> ptrs, std::vector<uint16_t> cmds, std::map<uint32_t, int> sizeMap, uint32_t ptrsStart, uint32_t cmdsStart): mPtrs(ptrs), mCmds(cmds), mSizeMap(sizeMap), mPtrsStart(ptrsStart), mCmdsStart(cmdsStart) {

}

ExportResult SF64::ScriptHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern u16* " << symbol << "[];\n";
    return std::nullopt;
}

std::string GetMsg(uint16_t msgId) {
    std::string msg = "";
    auto rawmsg = Companion::Instance->GetParseDataBySymbol("gMsg_ID_" + std::to_string(msgId));

    if(rawmsg.has_value() && rawmsg.value().data.has_value()) {
        auto msgData = std::static_pointer_cast<SF64::MessageData>(rawmsg.value().data.value());
        auto msg = std::regex_replace(msgData->mMesgStr, std::regex(R"(\n)"), " ");
    }
    return msg;
}

std::string MakeScriptCmd(uint16_t s1, uint16_t s2) {
    auto opcode = (s1 & 0xFE00) >> 9;
    auto arg1 = s1 & 0x1FF;
    int waitframes = 0;
    std::ostringstream cmd;
    std::ostringstream comment;

    switch (opcode) {
        case 0:
        case 1: {
            auto f3 = arg1 & 0x7F;
            auto zmode = VALUE_TO_ENUM((arg1 >> 7) & 3, "EventModeZ", "EVOP_UNK");
            
            if(opcode == 0 && s2 == 1) {
                cmd << "EVENT_UPDATE_SPEED(" << std::dec << f3 << ", " << zmode;
            } else {
                cmd << "EVENT_SET_" << (opcode ? "ACCEL" : "SPEED") << "(" << std::dec << f3 << ", " << zmode << ", " << s2;
                waitframes = s2;
            }
        } break;
        case 2:
            cmd << "EVENT_SET_BASE_ZVEL(" << std::dec << s2;
            break;
        case 3:
            cmd << "EVENT_SET_AS_LEADER(";
            break;
        case 4:
            cmd << "EVENT_START_FORMATION(" << std::dec << s2;
            break;
        case 8:
            cmd << "EVENT_STOP_FORMATION(";
            break;
        case 9:
        case 10:
        case 11:
        case 12:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21: {
            auto rotcmd = VALUE_TO_ENUM(opcode, "EventOpcode", "EVOP_UNK");
            if(opcode < 16 && s2 != 0) {
                waitframes = std::ceil(10.0f * arg1 / s2);
            }
            if(opcode < 16 && (arg1 == s2 / 10) && arg1 != 0) {
                cmd << rotcmd.replace(0, 4, "EVENT_UPDATE") << "(" << std::dec << arg1;
                waitframes = 0;
            } else {
                cmd << rotcmd.replace(0, 4, "EVENT") << "(" << std::dec << arg1 << ", " << std::fixed << std::setprecision(1) <<  s2 / 10.0f;
                if(arg1 == 0) {
                    waitframes = 1;
                }
            }
        } break;
        case 24:
            cmd << "EVENT_LOCAL_ROTATION(";
            break;
        case 25:
            cmd << "EVENT_FREE_ROTATION(";
            break;
        case 40:
        case 41:
        case 42:
        case 43:
        case 44:
        case 46:
        case 47: {
            auto chaseCmd = VALUE_TO_ENUM(opcode, "EventOpcode", "EVOP_UNK");
            cmd << chaseCmd.replace(0, 4, "EVENT") << "(" << std::dec << s2 << ", " << arg1;
        } break;
        case 45: {
            auto teamId = VALUE_TO_ENUM(arg1, "TeamId", "TEAMID_UNK");
            cmd << "EVENT_SET_TARGET(" << teamId << ", " << std::dec << s2;
        } break;
        case 48:
            if(s2 == 1) {
                cmd << "EVENT_UPDATE_ACTOR(";
            } else {
                cmd << "EVENT_SET_WAIT(" << std::dec << s2;
                waitframes = s2;
            }
            break;
        case 56:
            cmd << "EVENT_SET_CALL(" << std::dec << s2 << ", " << arg1;
            break;
        case 57: {
            auto teamId = VALUE_TO_ENUM(s2, "TeamId", "TEAMID_UNK");
            cmd << "EVENT_RESTORE_TEAM(" << teamId;
         } break;
        case 58:
        case 59: {
            auto sfxIndex = VALUE_TO_ENUM(s2, "EventSfx", "EVSFX_UNK");
            cmd << "EVENT_" << ((opcode == 58) ? "PLAY" : "STOP") << "_SFX(" << sfxIndex;
        } break;
        case 96:
            if(s2 == 0) {
                cmd << "EVENT_CLEAR_TRIGGER(" << std::dec << arg1;
            } else {
                if (s2 >= 100) {
                    cmd << "EVENT_SET_Z_TRIGGER(" << std::dec << (s2 - 100) * 100 << ", ";
                } else {
                    auto condition = VALUE_TO_ENUM(s2, "EventCondition", "EVC_UNK");
                    cmd << "EVENT_SET_TRIGGER(" << condition << ", ";
                }
                cmd << ((arg1 < 200) ? "" : "EVENT_AI_CHANGE + ") << std::dec << ((arg1 < 200) ? arg1 : arg1 - 200);
            }
            break;
        case 104: {
            auto actorInfo = VALUE_TO_ENUM(s2, "EventActorId", "EVID_UNK");
            cmd << "EVENT_INIT_ACTOR(" << actorInfo << ", " << std::dec << arg1;
        } break;
        case 105: {
            auto teamId = VALUE_TO_ENUM(s2, "TeamId", "TEAMID_UNK");
            cmd << "EVENT_SET_TEAM_ID(" << teamId;
        } break;
        case 112:{
            auto actiontype = VALUE_TO_ENUM(s2, "EventAction", "EVACT_UNK");
            cmd << "EVENT_SET_ACTION(" << actiontype;
            if((s2 == 14 || s2 == 15)) {
                waitframes = 1;
            }
        } break;
        case 113:
            cmd << "EVENT_ADD_TO_GROUP(" << std::dec << s2 << ", " << arg1;
            break;
        case 116: {
            auto itemType = VALUE_TO_ENUM(s2, "ItemDrop", "DROP_UNK");
            cmd << "EVENT_DROP_ITEM(" << itemType;
        } break;
        case 118:
            cmd << "EVENT_SET_REVERB(" << std::dec << s2;
            break;
        case 119: {
            auto groundtype = VALUE_TO_ENUM(s2, "GroundSurface", "SURFACE_UNK");
            cmd << "EVENT_SET_SURFACE(" << groundtype;
        } break;
        case 120: {
            auto rcidName = VALUE_TO_ENUM(arg1, "RadioCharacterId", "RCID_UNK");
            cmd << "EVENT_PLAY_MSG(" << rcidName << ", " << std::dec << s2;
            auto msg = GetMsg(s2);
            if(!msg.empty()) {
                comment << " // " << msg;
            }
        } break;
        case 121: {
            auto teamId = VALUE_TO_ENUM(s2, "TeamId", "TEAMID_UNK");
            cmd << "EVENT_DAMAGE_TEAM(" << teamId << ", " << std::dec << arg1;
        } break;
        case 122:
            cmd << "EVENT_STOP_BGM(";
            break;
        case 124: {
            auto color = VALUE_TO_ENUM(s2, "TexLineColor", "TXLC_UNK");
            cmd << "EVENT_MAKE_TEXLINE(" << color;
        } break;
        case 125:
            cmd << "EVENT_STOP_TEXLINE(";
            break;
        case 126:
            cmd << ((s2 == 0) ? "EVENT_GOTO(" : "EVENT_LOOP(");
            if(s2 != 0) {
                cmd << std::dec << s2 << ", ";
            }
            cmd << ((arg1 < 200) ? "" : "EV_CHANGE_SCRIPT + ") << std::dec << ((arg1 < 200) ? arg1 : arg1 - 200);
            break;
        case 127:
            cmd << "EVENT_STOP_SCRIPT(";
            break;
        default: {
            auto opcodeName = VALUE_TO_ENUM(opcode, "EventOpcode", "EVOP_UNK");
            cmd << "EVENT_CMD(" << opcodeName << ", " << std::dec << arg1 << ", " << s2;
            if(opcode >= 40 && opcode <= 48) {
                waitframes = s2;
            }
        } break;
    }
    cmd << ")," << comment.str();
    if(waitframes > 1) {
        cmd << "\n          // wait " << waitframes << " frames";
    } else if (waitframes == 1) {
        cmd << "\n          // update actor";
    }
    return cmd.str();
}

ExportResult SF64::ScriptCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto script = std::static_pointer_cast<SF64::ScriptData>(raw);
    auto sortedPtrs = script->mPtrs;
    std::sort(sortedPtrs.begin(), sortedPtrs.end());
    auto cmdOff = ASSET_PTR(script->mCmdsStart);
    auto cmdIndex = 0;
    std::map<uint32_t, std::string> scriptNames;

    for(int i = 0; i < sortedPtrs.size(); i++) {
        std::ostringstream scriptDefaultName;
        auto scriptIndex = std::find(script->mPtrs.begin(), script->mPtrs.end(), sortedPtrs[i]) - script->mPtrs.begin();

        scriptDefaultName << symbol << "_script_" << std::dec << scriptIndex << "_" << std::uppercase << std::hex << cmdOff;
        auto scriptName = GetSafeNode(node, "script_symbol", scriptDefaultName.str());
        scriptNames[sortedPtrs[i]] = scriptName;
        if (Companion::Instance->IsDebug()) {
            write << "// 0x" << std::hex << std::uppercase << cmdOff << "\n";
        }
        write << "u16 " << scriptName << "[] = {";

        auto cmdCount = script->mSizeMap[sortedPtrs[i]] / 2;
        for(int j = 0; j < cmdCount; j++, cmdIndex+=2) {
            // if((j % 3) == 0) {
            write  << "\n" << fourSpaceTab << "/* " << std::setfill(' ') << std::setw(2) << std::dec << j << " */ ";
            // }
            write << MakeScriptCmd(script->mCmds[cmdIndex], script->mCmds[cmdIndex + 1]);
        }

        write << "\n};\n";

        cmdOff += 4 * cmdCount;
        if (Companion::Instance->IsDebug()) {
            write << "// count: " << cmdCount << " commands\n";
            write << "// 0x" << std::hex << std::uppercase << cmdOff << "\n";
        }
        write << "\n";
    }

    write << "// 0x" << std::hex << std::uppercase << ASSET_PTR(offset) << "\n";
    write << "u16* " << symbol << "[] = {";
    for(int i = 0; i < script->mPtrs.size(); i++) {
        if((i % 4) == 0) {
            write << "\n" << fourSpaceTab;
        }
        write << scriptNames[script->mPtrs[i]] << ", ";
    }
    write << "\n};\n";

    if (Companion::Instance->IsDebug()) {
        write << "// count: " << std::dec << script->mPtrs.size() << " events\n";
    }

    return OffsetEntry {
        script->mCmdsStart,
        static_cast<uint32_t>(offset + script->mPtrs.size() * sizeof(uint32_t))
    };
}

ExportResult SF64::ScriptBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    auto scriptWriter = LUS::BinaryWriter();
    auto script = std::static_pointer_cast<SF64::ScriptData>(raw);
    auto sortedPtrs = script->mPtrs;
    std::sort(sortedPtrs.begin(), sortedPtrs.end());
    uint32_t ptrCount = 0;
    uint32_t cmdIndex = 0;
    std::unordered_map<uint32_t, uint64_t> ptrMap;

    // Export Commands and Populate Hashes Map
    for (auto ptr : sortedPtrs) {
        auto wrapper = Companion::Instance->GetCurrentWrapper();
        std::ostringstream stream;

        auto cmdWriter = LUS::BinaryWriter();
        WriteHeader(cmdWriter, Torch::ResourceType::ScriptCmd, 0);

        auto cmdCount = script->mSizeMap.at(ptr) / 2;
        cmdWriter.Write((uint32_t) cmdCount);

        // Writing in pairs for readability
        for (uint32_t i = 0; i < cmdCount; ++i) {
            cmdWriter.Write(script->mCmds.at(cmdIndex++));
            cmdWriter.Write(script->mCmds.at(cmdIndex++));
        }

        cmdWriter.Finish(stream);

        auto data = stream.str();
        wrapper->AddFile(entryName + "_cmd_" + std::to_string(ptrCount), std::vector(data.begin(), data.end()));

        std::ostringstream cmdName;
        cmdName << entryName << "_cmd_" << std::dec << ptrCount;
        ptrMap[ptr] = CRC64(cmdName.str().c_str());
        ptrCount++;
    }

    // Export Script
    WriteHeader(scriptWriter, Torch::ResourceType::Script, 0);
    auto count = script->mPtrs.size();
    scriptWriter.Write((uint32_t) count);
    for (size_t i = 0; i < script->mPtrs.size(); i++) {
        scriptWriter.Write(ptrMap.at(script->mPtrs.at(i)));
    }

    scriptWriter.Finish(write);

    return std::nullopt;
}

std::string MakeXMLScriptCmd(uint16_t s1, uint16_t s2) {
    auto opcode = (s1 & 0xFE00) >> 9;
    auto arg1 = s1 & 0x1FF;
    std::ostringstream cmd;

    switch (opcode) {
        case 0:
        case 1: {
            auto f3 = arg1 & 0x7F;
            auto zmode = VALUE_TO_ENUM((arg1 >> 7) & 3, "EventModeZ", "EVOP_UNK");
            cmd << "SET_" << (opcode ? "ACCEL" : "SPEED") << "(" << std::dec << f3 << ", " << zmode << ", " << s2;
        } break;
        case 2:
            cmd << "SET_BASE_ZVEL(" << std::dec << s2;
            break;
        case 3:
            cmd << "SET_AS_LEADER(";
            break;
        case 4:
            cmd << "START_FORMATION(" << std::dec << s2;
            break;
        case 8:
            cmd << "STOP_FORMATION(";
            break;
        case 9:
        case 10:
        case 11:
        case 12:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21: {
            auto rotcmd = VALUE_TO_ENUM(opcode, "EventOpcode", "EVOP_UNK");
            cmd << rotcmd.replace(0, 4, "EVENT") << "(" << std::dec << s2 << ", " << std::fixed << std::setprecision(1) <<  arg1 / 10.0f;
        } break;
        case 24:
            cmd << "SET_ROTATE(";
            break;
        case 25:
            cmd << "STOP_ROTATE(";
            break;
        case 40:
        case 41:
        case 42:
        case 43:
        case 44:
        case 46:
        case 47: {
            auto chaseCmd = VALUE_TO_ENUM(opcode, "EventOpcode", "EVOP_UNK");
            cmd << chaseCmd.replace(0, 4, "EVENT") << "(" << std::dec << s2 << ", " << arg1;
        } break;
        case 45: {
            auto teamId = VALUE_TO_ENUM(arg1, "TeamId", "TEAMID_UNK");
            cmd << "SET_TARGET(" << teamId << ", " << std::dec << s2;
        } break;
        case 48:
            cmd << "SET_WAIT(" << std::dec << s2;
            break;
        case 56:
            cmd << "SET_CALL(" << std::dec << s2 << ", " << arg1;
            break;
        case 57: {
            auto teamId = VALUE_TO_ENUM(s2, "TeamId", "TEAMID_UNK");
            cmd << "RESTORE_TEAM(" << teamId;
         } break;
        case 58:
        case 59: {
            auto sfxIndex = VALUE_TO_ENUM(s2, "EventSfx", "EVSFX_UNK");
            cmd << "" << ((opcode == 58) ? "PLAY" : "STOP") << "_SFX(" << sfxIndex;
        } break;
        case 96:
            if(s2 == 0) {
                cmd << "CLEAR_TRIGGER(" << std::dec << arg1;
            } else {
                if (s2 >= 100) {
                    cmd << "SET_Z_TRIGGER(" << std::dec << (s2 - 100) * 100 << ", ";
                } else {
                    auto condition = VALUE_TO_ENUM(s2, "EventCondition", "EVC_UNK");
                    cmd << "SET_TRIGGER(" << condition << ", ";
                }
                cmd << ((arg1 < 200) ? "" : "AI_CHANGE + ") << std::dec << ((arg1 < 200) ? arg1 : arg1 - 200);
            }
            break;
        case 104: {
            auto actorInfo = VALUE_TO_ENUM(s2, "EventActorId", "EVID_UNK");
            cmd << "INIT_ACTOR(" << actorInfo << ", " << std::dec << arg1;
        } break;
        case 105: {
            auto teamId = VALUE_TO_ENUM(s2, "TeamId", "TEAMID_UNK");
            cmd << "SET_TEAM_ID(" << teamId;
        } break;
        case 112:{
            auto actiontype = VALUE_TO_ENUM(s2, "EventAction", "EVACT_UNK");
            cmd << "SET_ACTION(" << actiontype;
        } break;
        case 113:
            cmd << "ADD_TO_GROUP(" << std::dec << s2 << ", " << arg1;
            break;
        case 116: {
            auto itemType = VALUE_TO_ENUM(s2, "ItemDrop", "DROP_UNK");
            cmd << "DROP_ITEM(" << itemType;
        } break;
        case 118:
            cmd << "SET_REVERB(" << std::dec << s2;
            break;
        case 119: {
            auto groundtype = VALUE_TO_ENUM(s2, "GroundSurface", "SURFACE_UNK");
            cmd << "SET_SURFACE(" << groundtype;
        } break;
        case 120: {
            auto rcidName = VALUE_TO_ENUM(arg1, "RadioCharacterId", "RCID_UNK");
            cmd << "PLAY_MSG(" << rcidName << ", " << std::dec << s2;
        } break;
        case 121: {
            auto teamId = VALUE_TO_ENUM(s2, "TeamId", "TEAMID_UNK");
            cmd << "DAMAGE_TEAM(" << teamId << ", " << std::dec << arg1;
        } break;
        case 122:
            cmd << "STOP_BGM(";
            break;
        case 124: {
            auto color = VALUE_TO_ENUM(s2, "TexLineColor", "TXLC_UNK");
            cmd << "MAKE_TEXLINE(" << color;
        } break;
        case 125:
            cmd << "STOP_TEXLINE(";
            break;
        case 126:
            cmd << ((s2 == 0) ? "GOTO(" : "LOOP(");
            if(s2 != 0) {
                cmd << std::dec << s2 << ", ";
            }
            cmd << ((arg1 < 200) ? "" : "AI_CHANGE + ") << std::dec << ((arg1 < 200) ? arg1 : arg1 - 200);
            break;
        case 127:
            cmd << "STOP_SCRIPT(";
            break;
        default: {
            auto opcodeName = VALUE_TO_ENUM(opcode, "EventOpcode", "EVOP_UNK");
            cmd << "CMD(" << opcodeName << ", " << std::dec << arg1 << ", " << s2;
        } break;
    }
    cmd << ")";
    return cmd.str();
}

ExportResult SF64::ScriptXMLExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    auto script = std::static_pointer_cast<SF64::ScriptData>(raw);
    auto sortedPtrs = script->mPtrs;
    std::sort(sortedPtrs.begin(), sortedPtrs.end());
    auto cmdOff = ASSET_PTR(script->mCmdsStart);
    auto cmdIndex = 0;
    std::map<uint32_t, std::string> scriptNames;

    *replacement += ".meta";

    tinyxml2::XMLPrinter printer;
    tinyxml2::XMLDocument root;
    tinyxml2::XMLElement* event = root.NewElement("EventScript");

    tinyxml2::XMLElement* routine = event->InsertNewChildElement("Routine");
    for(unsigned int sortedPtr : sortedPtrs) {
        tinyxml2::XMLElement* src = routine->InsertNewChildElement("Script");
        std::ostringstream scriptDefaultName;
        auto scriptIndex = std::find(script->mPtrs.begin(), script->mPtrs.end(), sortedPtr) - script->mPtrs.begin();

        scriptDefaultName << symbol << "_script_" << std::dec << scriptIndex << "_" << std::uppercase << std::hex << cmdOff;
        auto scriptName = GetSafeNode(node, "script_symbol", scriptDefaultName.str());
        scriptNames[sortedPtr] = scriptName;
        src->SetAttribute("ID", scriptName.c_str());
        auto cmdCount = script->mSizeMap[sortedPtr] / 2;
        
        for(int j = 0; j < cmdCount; j++, cmdIndex+=2) {
            tinyxml2::XMLElement* obj = src->InsertNewChildElement("Run");
            obj->SetText(MakeXMLScriptCmd(script->mCmds[cmdIndex], script->mCmds[cmdIndex + 1]).c_str());
            src->InsertEndChild(obj);
        }
        routine->InsertEndChild(src);
    }

    tinyxml2::XMLElement* program = event->InsertNewChildElement("Program");

    for(unsigned int mPtr : script->mPtrs) {
        tinyxml2::XMLElement* obj = program->InsertNewChildElement("Run");
        obj->SetAttribute("Script", scriptNames[mPtr].c_str());
        program->InsertEndChild(obj);
    }

    root.InsertEndChild(event);
    root.Accept(&printer);
    write.write(printer.CStr(), printer.CStrSize() - 1);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> SF64::ScriptFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    const auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto ptrsStart = offset;
    YAML::Node scriptNode;
    std::vector<uint32_t> scriptPtrs;
    std::vector<uint16_t> scriptCmds;
    std::map<uint32_t, int> sizeMap;
    auto [_, segment] = Decompressor::AutoDecode(node, buffer, 0x1000);
    LUS::BinaryReader reader(segment.data, segment.size);
    reader.SetEndianness(Torch::Endianness::Big);
    auto ptr = reader.ReadUInt32();

    while(SEGMENT_NUMBER(ptr) == SEGMENT_NUMBER(offset)) {
        scriptPtrs.push_back(ptr);
        ptr = reader.ReadUInt32();
    }

    auto sortedPtrs = scriptPtrs;
    std::sort(sortedPtrs.begin(), sortedPtrs.end());

    auto cmdsStart = sortedPtrs[0];

    for(int i = 0; i < sortedPtrs.size() - 1; i++) {
        sizeMap[sortedPtrs[i]] = (sortedPtrs[i+1] - sortedPtrs[i]) / sizeof(uint16_t);
    }
    sizeMap[sortedPtrs[sortedPtrs.size() - 1]] = (ptrsStart - sortedPtrs[sortedPtrs.size() - 1]) / sizeof(uint16_t);

    auto scriptLen = (ptrsStart - cmdsStart) / sizeof(uint16_t);

    scriptNode["offset"] = cmdsStart;
    auto [__, scriptSegment] = Decompressor::AutoDecode(scriptNode, buffer, scriptLen * sizeof(uint16_t));
    LUS::BinaryReader scriptReader(scriptSegment.data, scriptSegment.size);
    scriptReader.SetEndianness(Torch::Endianness::Big);
    for(int i = 0; i < scriptLen; i++) {
        scriptCmds.push_back(scriptReader.ReadUInt16());
    }
    return std::make_shared<SF64::ScriptData>(scriptPtrs, scriptCmds, sizeMap, ptrsStart, cmdsStart);
}
