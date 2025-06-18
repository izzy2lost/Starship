#include "LevelScriptFactory.h"
#include "spdlog/spdlog.h"

#include "Companion.h"
#include "utils/Decompressor.h"
#include "utils/TorchUtils.h"
#include <regex>

uint64_t RegisterPtr(uint32_t ptr, std::string type) {

    // Disable autogen for now since most assets will be from external files

    // if (ptr != 0) {
    //     YAML::Node node;
    //     node["type"] = type;
    //     node["offset"] = ptr;
    //     Companion::Instance->AddAsset(node);
    // }

    return ptr;
}

ExportResult SM64::LevelScriptHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern LevelScript " << symbol << "[];\n";
    return std::nullopt;
}

ExportResult SM64::LevelScriptCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");
    const auto commands = std::static_pointer_cast<LevelScriptData>(raw)->mCommands;
    uint32_t indentCount = 1;

    write << "static const LevelScript " << symbol << "[] = {\n";

    for(auto& [opcode, arguments] : commands) {
        bool commaFlag = false;

        if (opcode == LevelOpcode::END_AREA) {
            --indentCount;
        }

        for (uint32_t i = 0; i < indentCount; ++i) {
            write << fourSpaceTab;
        }

        if (opcode == LevelOpcode::AREA) {
            ++indentCount;
        }

        write << opcode << "(";
        for(auto& args : arguments) {
            if (commaFlag) {
                write << ", ";
            } else {
                commaFlag = true;
            }

            switch(static_cast<LevelArgumentType>(args.index())) {
                case LevelArgumentType::U8: {
                    write << std::hex << "0x" << static_cast<uint32_t>(std::get<uint8_t>(args));
                    break;
                }
                case LevelArgumentType::S8: {
                    write << std::hex << "0x" << static_cast<uint32_t>(std::get<int8_t>(args));
                    break;
                }
                case LevelArgumentType::U16: {
                    write << std::hex << "0x" << std::get<uint16_t>(args);
                    break;
                }
                case LevelArgumentType::S16: {
                    write << std::dec << std::get<int16_t>(args);
                    break;
                }
                case LevelArgumentType::U32: {
                    write << std::hex << "0x" << std::get<uint32_t>(args);
                    break;
                }
                case LevelArgumentType::S32: {
                    write << std::dec << std::get<int32_t>(args);
                    break;
                }
                case LevelArgumentType::F32: {
                    write << std::dec << std::get<float>(args);
                    break;
                }
                case LevelArgumentType::PTR: {
                    auto ptr = std::get<uint64_t>(args);
                    auto dec = Companion::Instance->GetNodeByAddr(ptr);
                    std::string symbol = "NULL";

                    if (ptr == 0) {
                        write << symbol;
                    } else if (dec.has_value()) {
                        auto node = std::get<1>(dec.value());
                        symbol = GetSafeNode<std::string>(node, "symbol");
                        write << symbol;
                    } else {
                        SPDLOG_WARN("Cannot find node for ptr 0x{:X}", ptr);
                        write << std::hex << "0x" << ptr;
                    }
                    break;
                }
                default: {
                    throw std::runtime_error("Unknown Level Argument Type");
                }
            }
        }
        write << "),\n";
    }

    write << "\n};\n";

    size_t size = 0;

    return offset + size;
}

ExportResult SM64::LevelScriptBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    const auto commands = std::static_pointer_cast<LevelScriptData>(raw)->mCommands;

    WriteHeader(writer, Torch::ResourceType::LevelScript, 0);

    writer.Write((uint32_t)commands.size());

    for(auto& [opcode, arguments] : commands) {
        writer.Write(static_cast<uint8_t>(opcode));

        for(auto& args : arguments) {
            switch(static_cast<LevelArgumentType>(args.index())) {
                case LevelArgumentType::U8: {
                    writer.Write(std::get<uint8_t>(args));
                    break;
                }
                case LevelArgumentType::S8: {
                    writer.Write(std::get<int8_t>(args));
                    break;
                }
                case LevelArgumentType::U16: {
                    writer.Write(std::get<uint16_t>(args));
                    break;
                }
                case LevelArgumentType::S16: {
                    writer.Write(std::get<int16_t>(args));
                    break;
                }
                case LevelArgumentType::U32: {
                    writer.Write(std::get<uint32_t>(args));
                    break;
                }
                case LevelArgumentType::S32: {
                    writer.Write(std::get<int32_t>(args));
                    break;
                }
                case LevelArgumentType::F32: {
                    writer.Write(std::get<float>(args));
                    break;
                }
                case LevelArgumentType::PTR: {
                    auto ptr = static_cast<uint32_t>(std::get<uint64_t>(args));
                    auto dec = Companion::Instance->GetNodeByAddr(ptr);
                    if (ptr == 0) {
                        writer.Write(ptr);
                    } else if (dec.has_value()) {
                        uint64_t hash = CRC64(std::get<0>(dec.value()).c_str());
                        SPDLOG_INFO("Found Asset: 0x{:X} Hash: 0x{:X} Path: {}", ptr, hash, std::get<0>(dec.value()));
                        writer.Write(hash);
                    } else {
                        SPDLOG_WARN("Could not find Asset at 0x{:X}", ptr);
                    }
                    break;
                }
                default: {
                    throw std::runtime_error("Unknown Level Argument Type");
                }
            }
        }
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> SM64::LevelScriptFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    auto cmd = segment.data;
    bool processing = true;
    std::vector<LevelCommand> commands;
    size_t count;

    if (node["count"]) {
        count = GetSafeNode<size_t>(node, "count");
    } else {
        count = 0;
    }

    while(processing) {
        auto opcode = static_cast<LevelOpcode>(cmd[0x00]);

        SPDLOG_INFO("Processing Command {}", opcode);

        std::vector<LevelArgument> arguments;

        switch (opcode) {
            case LevelOpcode::EXECUTE: {
                auto seg = cur_level_cmd_s16(0x02);
                auto scriptStart = cur_level_cmd_u32(0x04);
                auto scriptEnd = cur_level_cmd_u32(0x08);
                auto ptr = cur_level_cmd_u32(0x0C);
                arguments.emplace_back(seg);
                arguments.emplace_back(scriptStart);
                arguments.emplace_back(scriptEnd);
                arguments.emplace_back(RegisterPtr(ptr, "SM64:LEVEL_SCRIPT"));

                cmd += 0x10 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::EXIT_AND_EXECUTE: {
                auto seg = cur_level_cmd_s16(0x02);
                auto scriptStart = cur_level_cmd_u32(0x04);
                auto scriptEnd = cur_level_cmd_u32(0x08);
                auto ptr = cur_level_cmd_u32(0x0C);
                arguments.emplace_back(seg);
                arguments.emplace_back(scriptStart);
                arguments.emplace_back(scriptEnd);
                arguments.emplace_back(RegisterPtr(ptr, "SM64:LEVEL_SCRIPT"));
                processing = false;
                cmd += 0x10 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::EXIT: {
                processing = false;
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::SLEEP:
            case LevelOpcode::SLEEP_BEFORE_EXIT: {
                auto frames = cur_level_cmd_s16(0x02);
                arguments.emplace_back(frames);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::JUMP: {
                auto targetPtr = cur_level_cmd_u32(0x04);
                arguments.emplace_back(RegisterPtr(targetPtr, "SM64:LEVEL_SCRIPT"));
                processing = false;
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::JUMP_LINK: {
                // unused, not sure if processing = false
                auto targetPtr = cur_level_cmd_u32(0x04);
                arguments.emplace_back(RegisterPtr(targetPtr, "SM64:LEVEL_SCRIPT"));
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::RETURN: {
                processing = false;
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::JUMP_LINK_PUSH_ARG: {
                // unused, not sure if processing = false
                auto arg = cur_level_cmd_s16(0x02);
                arguments.emplace_back(arg);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::JUMP_N_TIMES: {
                // unused
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::LOOP_BEGIN: {
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::LOOP_UNTIL: {
                auto op = cur_level_cmd_u8(0x02);
                auto arg = cur_level_cmd_s32(0x04);
                arguments.emplace_back(op);
                arguments.emplace_back(arg);
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::JUMP_IF:
            case LevelOpcode::JUMP_LINK_IF: {
                auto op = cur_level_cmd_u8(0x02);
                auto arg = cur_level_cmd_s32(0x04);
                auto targetPtr = cur_level_cmd_u32(0x08);
                arguments.emplace_back(op);
                arguments.emplace_back(arg);
                arguments.emplace_back(RegisterPtr(targetPtr, "SM64:LEVEL_SCRIPT"));
                cmd += 0xC << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::SKIP_IF: {
                auto op = cur_level_cmd_u8(0x02);
                auto arg = cur_level_cmd_s32(0x04);
                arguments.emplace_back(op);
                arguments.emplace_back(arg);
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::SKIP:
            case LevelOpcode::SKIP_NOP: {
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::CALL:
            case LevelOpcode::CALL_LOOP: {
                auto arg = cur_level_cmd_s16(0x02);
                auto func = cur_level_cmd_u32(0x04);
                arguments.emplace_back(arg);
                arguments.emplace_back(func);
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::SET_REG: {
                auto value = cur_level_cmd_s16(0x02);
                arguments.emplace_back(value);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::PUSH_POOL:
            case LevelOpcode::POP_POOL: {
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::FIXED_LOAD: {
                auto loadAddr = cur_level_cmd_u32(0x04);
                auto romStart = cur_level_cmd_u32(0x08);
                auto romEnd = cur_level_cmd_u32(0x0C);
                arguments.emplace_back(loadAddr);
                arguments.emplace_back(romStart);
                arguments.emplace_back(romEnd);
                cmd += 0x10 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::LOAD_RAW:
            case LevelOpcode::LOAD_MIO0: {
                auto seg = cur_level_cmd_s16(0x02);
                auto romStart = cur_level_cmd_u32(0x04);
                auto romEnd = cur_level_cmd_u32(0x08);
                arguments.emplace_back(seg);
                arguments.emplace_back(romStart);
                arguments.emplace_back(romEnd);
                cmd += 0xC << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::LOAD_MARIO_HEAD: {
                auto setHead = cur_level_cmd_s16(0x02);
                arguments.emplace_back(setHead);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::LOAD_MIO0_TEXTURE: {
                auto seg = cur_level_cmd_s16(0x02);
                auto romStart = cur_level_cmd_u32(0x04);
                auto romEnd = cur_level_cmd_u32(0x08);
                arguments.emplace_back(seg);
                arguments.emplace_back(romStart);
                arguments.emplace_back(romEnd);
                cmd += 0xC << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::INIT_LEVEL:
            case LevelOpcode::CLEAR_LEVEL:
            case LevelOpcode::ALLOC_LEVEL_POOL:
            case LevelOpcode::FREE_LEVEL_POOL: {
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::AREA: {
                auto index = cur_level_cmd_u8(0x02);
                auto geo = cur_level_cmd_u32(0x04);
                arguments.emplace_back(index);
                arguments.emplace_back(RegisterPtr(geo, "SM64:GEO_LAYOUT"));
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::END_AREA: {
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::LOAD_MODEL_FROM_DL: {
                auto modelLayer = cur_level_cmd_s16(0x02);
                int16_t layer = (modelLayer >> 12);
                int16_t model = modelLayer - (layer << 12);
                auto dl = cur_level_cmd_u32(0x04);

                arguments.emplace_back(model);
                arguments.emplace_back(RegisterPtr(dl, "GFX"));
                arguments.emplace_back(layer);
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::LOAD_MODEL_FROM_GEO: {
                auto model = cur_level_cmd_s16(0x02);
                auto geo = cur_level_cmd_u32(0x04);
                arguments.emplace_back(model);
                arguments.emplace_back(RegisterPtr(geo, "SM64:GEO_LAYOUT"));
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::CMD23: {
                // unused
                auto model = cur_level_cmd_s16(0x02);
                auto unk4 = cur_level_cmd_u32(0x04);
                auto unk8 = cur_level_cmd_f32(0x08);
                arguments.emplace_back(model);
                arguments.emplace_back(unk4);
                arguments.emplace_back(unk8);
                cmd += 0xC << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::OBJECT_WITH_ACTS: {
                auto acts = cur_level_cmd_u8(0x02);
                auto model = cur_level_cmd_u8(0x03);
                auto posX = cur_level_cmd_s16(0x04);
                auto posY = cur_level_cmd_s16(0x06);
                auto posZ = cur_level_cmd_s16(0x08);
                auto angleX = cur_level_cmd_s16(0x0A);
                auto angleY = cur_level_cmd_s16(0x0C);
                auto angleZ = cur_level_cmd_s16(0x0E);
                auto bhvParam = cur_level_cmd_s32(0x10);
                auto bhv = cur_level_cmd_u32(0x14);
                arguments.emplace_back(model);
                arguments.emplace_back(posX);
                arguments.emplace_back(posY);
                arguments.emplace_back(posZ);
                arguments.emplace_back(angleX);
                arguments.emplace_back(angleY);
                arguments.emplace_back(angleZ);
                arguments.emplace_back(bhvParam);
                arguments.emplace_back(RegisterPtr(bhv, "SM64:BEHAVIOR_SCRIPT"));
                arguments.emplace_back(acts);
                cmd += 0x18 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::MARIO: {
                auto model = cur_level_cmd_u8(0x03);
                auto bhvArg = cur_level_cmd_s32(0x04);
                auto bhv = cur_level_cmd_u32(0x08);
                arguments.emplace_back(model);
                arguments.emplace_back(bhvArg);
                arguments.emplace_back(RegisterPtr(bhv, "SM64:BEHAVIOR_SCRIPT"));
                cmd += 0xC << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::WARP_NODE:
            case LevelOpcode::PAINTING_WARP_NODE: {
                auto id = cur_level_cmd_u8(0x01);
                auto destLevel = cur_level_cmd_u8(0x02);
                auto destArea = cur_level_cmd_u8(0x03);
                auto destNode = cur_level_cmd_u8(0x04);
                auto flags = cur_level_cmd_u8(0x05);
                arguments.emplace_back(id);
                arguments.emplace_back(destLevel);
                arguments.emplace_back(destArea);
                arguments.emplace_back(destNode);
                arguments.emplace_back(flags);
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::INSTANT_WARP: {
                auto id = cur_level_cmd_u8(0x02);
                auto destArea = cur_level_cmd_u8(0x03);
                auto displayX = cur_level_cmd_s16(0x04);
                auto displayY = cur_level_cmd_s16(0x06);
                auto displayZ = cur_level_cmd_s16(0x08);
                arguments.emplace_back(id);
                arguments.emplace_back(destArea);
                arguments.emplace_back(displayX);
                arguments.emplace_back(displayY);
                arguments.emplace_back(displayZ);
                cmd += 0xC << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::LOAD_AREA: {
                auto area = cur_level_cmd_u8(0x02);
                arguments.emplace_back(area);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::CMD2A: {
                auto unk2 = cur_level_cmd_u8(0x02);
                arguments.emplace_back(unk2);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::MARIO_POS: {
                auto area = cur_level_cmd_u8(0x02);
                auto yaw = cur_level_cmd_s16(0x04);
                auto posX = cur_level_cmd_s16(0x06);
                auto posY = cur_level_cmd_s16(0x08);
                auto posZ = cur_level_cmd_s16(0x0A);
                arguments.emplace_back(area);
                arguments.emplace_back(yaw);
                arguments.emplace_back(posX);
                arguments.emplace_back(posY);
                arguments.emplace_back(posZ);
                cmd += 0xC << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::CMD2C:
            case LevelOpcode::CMD2D: {
                // unused
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::TERRAIN: {
                auto terrainPtr = cur_level_cmd_u32(0x04);
                arguments.emplace_back(RegisterPtr(terrainPtr, "SM64:COLLISION"));
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::ROOMS: {
                auto roomPtr = cur_level_cmd_u32(0x04);
                arguments.emplace_back(RegisterPtr(roomPtr, "BLOB"));
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::SHOW_DIALOG: {
                auto index = cur_level_cmd_u8(0x02);
                auto dialogId = cur_level_cmd_u8(0x03);
                arguments.emplace_back(index);
                arguments.emplace_back(dialogId);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::TERRAIN_TYPE: {
                auto terrainType = cur_level_cmd_s16(0x02);
                arguments.emplace_back(terrainType);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::NOP: {
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::TRANSITION: {
                auto transType = cur_level_cmd_u8(0x02);
                auto time = cur_level_cmd_u8(0x03);
                auto colorR = cur_level_cmd_u8(0x04);
                auto colorG = cur_level_cmd_u8(0x05);
                auto colorB = cur_level_cmd_u8(0x06);
                arguments.emplace_back(transType);
                arguments.emplace_back(time);
                arguments.emplace_back(colorR);
                arguments.emplace_back(colorG);
                arguments.emplace_back(colorB);
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::BLACKOUT: {
                auto active = cur_level_cmd_u8(0x02);
                arguments.emplace_back(active);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::GAMMA: {
                auto enabled = cur_level_cmd_u8(0x02);
                arguments.emplace_back(enabled);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::SET_BACKGROUND_MUSIC: {
                auto settingsPreset = cur_level_cmd_s16(0x02);
                auto seq = cur_level_cmd_s16(0x04);
                arguments.emplace_back(settingsPreset);
                arguments.emplace_back(seq);
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::SET_MENU_MUSIC: {
                auto seq = cur_level_cmd_s16(0x02);
                arguments.emplace_back(seq);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::STOP_MUSIC: {
                auto fadeOutTime = cur_level_cmd_s16(0x02);
                arguments.emplace_back(fadeOutTime);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::MACRO_OBJECTS: {
                auto objPtr = cur_level_cmd_u32(0x04);
                arguments.emplace_back(RegisterPtr(objPtr, "SM64:MACRO"));
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::CMD3A: {
                // unused
                auto unk2 = cur_level_cmd_s16(0x02);
                auto unk4 = cur_level_cmd_s16(0x04);
                auto unk6 = cur_level_cmd_s16(0x06);
                auto unk8 = cur_level_cmd_s16(0x08);
                auto unkA = cur_level_cmd_s16(0x0A);
                arguments.emplace_back(unk2);
                arguments.emplace_back(unk4);
                arguments.emplace_back(unk6);
                arguments.emplace_back(unk8);
                arguments.emplace_back(unkA);
                cmd += 0xC << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::WHIRLPOOL: {
                auto index = cur_level_cmd_u8(0x2);
                auto condition = cur_level_cmd_u8(0x3);
                auto posX = cur_level_cmd_s16(0x4);
                auto posY = cur_level_cmd_s16(0x6);
                auto posZ = cur_level_cmd_s16(0x8);
                auto strength = cur_level_cmd_s16(0xA);
                arguments.emplace_back(index);
                arguments.emplace_back(condition);
                arguments.emplace_back(posX);
                arguments.emplace_back(posY);
                arguments.emplace_back(posZ);
                arguments.emplace_back(strength);
                cmd += 0xC << CMD_SIZE_SHIFT;
                break;
            }
            case LevelOpcode::GET_OR_SET: {
                auto op = cur_level_cmd_u8(0x2);
                auto var = cur_level_cmd_u8(0x3);
                arguments.emplace_back(op);
                arguments.emplace_back(var);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            default:
                throw std::runtime_error("Unknown Level Opcode");
        }

        commands.emplace_back(opcode, arguments);

        if (commands.size() == count) {
            // This condition is needed for malformed commands which have hardcoded jumps
            // By default, commands sie will always be greater than count
            break;
        }
    }

    return std::make_shared<SM64::LevelScriptData>(commands);
}
