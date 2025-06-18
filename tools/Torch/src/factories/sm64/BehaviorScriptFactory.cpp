#include "BehaviorScriptFactory.h"
#include "spdlog/spdlog.h"

#include "Companion.h"
#include "utils/Decompressor.h"
#include "utils/TorchUtils.h"
#include <regex>

ExportResult SM64::BehaviorScriptHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern BehaviorScript " << symbol << "[];\n";
    return std::nullopt;
}

ExportResult SM64::BehaviorScriptCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");
    const auto commands = std::static_pointer_cast<BehaviorScriptData>(raw)->mCommands;
    uint32_t indentCount = 1;

    write << "static const BehaviorScript " << symbol << "[] = {\n";

    for(auto& [opcode, arguments] : commands) {
        bool commaFlag = false;

        if (opcode == BehaviorOpcode::END_LOOP || opcode == BehaviorOpcode::END_REPEAT || opcode == BehaviorOpcode::END_REPEAT_CONTINUE) {
            --indentCount;
        }

        for (uint32_t i = 0; i < indentCount; ++i) {
            write << fourSpaceTab;
        }

        if (opcode == BehaviorOpcode::BEGIN_LOOP || opcode == BehaviorOpcode::BEGIN_REPEAT) {
            ++indentCount;
        }

        write << opcode << "(";
        for(auto& args : arguments) {
            if (commaFlag) {
                write << ", ";
            } else {
                commaFlag = true;
            }

            switch(static_cast<BehaviorArgumentType>(args.index())) {
                case BehaviorArgumentType::U8: {
                    write << std::hex << "0x" << static_cast<uint32_t>(std::get<uint8_t>(args));
                    break;
                }
                case BehaviorArgumentType::S8: {
                    write << std::hex << "0x" << static_cast<uint32_t>(std::get<int8_t>(args));
                    break;
                }
                case BehaviorArgumentType::U16: {
                    write << std::hex << "0x" << std::get<uint16_t>(args);
                    break;
                }
                case BehaviorArgumentType::S16: {
                    write << std::dec << std::get<int16_t>(args);
                    break;
                }
                case BehaviorArgumentType::U32: {
                    write << std::hex << "0x" << std::get<uint32_t>(args);
                    break;
                }
                case BehaviorArgumentType::S32: {
                    write << std::dec << std::get<int32_t>(args);
                    break;
                }
                case BehaviorArgumentType::F32: {
                    write << std::dec << std::get<float>(args);
                    break;
                }
                case BehaviorArgumentType::PTR: {
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
                    throw std::runtime_error("Unknown Behavior Argument Type");
                }
            }
        }
        write << "),\n";
    }

    write << "\n};\n";

    size_t size = 0;

    return offset + size;
}

ExportResult SM64::BehaviorScriptBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    const auto commands = std::static_pointer_cast<BehaviorScriptData>(raw)->mCommands;

    WriteHeader(writer, Torch::ResourceType::BehaviorScript, 0);

    writer.Write((uint32_t)commands.size());

    for(auto& [opcode, arguments] : commands) {
        writer.Write(static_cast<uint8_t>(opcode));

        for(auto& args : arguments) {
            switch(static_cast<BehaviorArgumentType>(args.index())) {
                case BehaviorArgumentType::U8: {
                    writer.Write(std::get<uint8_t>(args));
                    break;
                }
                case BehaviorArgumentType::S8: {
                    writer.Write(std::get<int8_t>(args));
                    break;
                }
                case BehaviorArgumentType::U16: {
                    writer.Write(std::get<uint16_t>(args));
                    break;
                }
                case BehaviorArgumentType::S16: {
                    writer.Write(std::get<int16_t>(args));
                    break;
                }
                case BehaviorArgumentType::U32: {
                    writer.Write(std::get<uint32_t>(args));
                    break;
                }
                case BehaviorArgumentType::S32: {
                    writer.Write(std::get<int32_t>(args));
                    break;
                }
                case BehaviorArgumentType::F32: {
                    writer.Write(std::get<float>(args));
                    break;
                }
                case BehaviorArgumentType::PTR: {
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
                    throw std::runtime_error("Unknown Behavior Argument Type");
                }
            }
        }
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> SM64::BehaviorScriptFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    auto cmd = segment.data;
    bool processing = true;
    std::vector<BehaviorCommand> commands;

    while(processing) {
        auto opcode = static_cast<BehaviorOpcode>(cmd[0x00]);

        SPDLOG_INFO("Processing Command {}", opcode);

        std::vector<BehaviorArgument> arguments;

        switch (opcode) {
            case BehaviorOpcode::BEGIN: {
                auto objList = cur_behavior_cmd_u8(0x01);
                arguments.emplace_back(objList);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::DELAY: {
                auto num = cur_behavior_cmd_u8(0x01);
                arguments.emplace_back(num);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::CALL: {
                uint64_t addr = cur_behavior_cmd_u32(0x04);
                arguments.emplace_back(addr);
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::RETURN: {
                processing = false;
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::GOTO: {
                uint64_t addr = cur_behavior_cmd_u32(0x04);
                arguments.emplace_back(addr);
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::BEGIN_REPEAT: {
                auto count = cur_behavior_cmd_s16(0x02);
                arguments.emplace_back(count);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::END_REPEAT:
            case BehaviorOpcode::END_REPEAT_CONTINUE:
            case BehaviorOpcode::BEGIN_LOOP: {
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::END_LOOP:
            case BehaviorOpcode::BREAK:
            case BehaviorOpcode::BREAK_UNUSED: {
                processing = false;
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::CALL_NATIVE: {
                auto func = cur_behavior_cmd_u32(0x04);
                arguments.emplace_back(func);
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::ADD_FLOAT:
            case BehaviorOpcode::SET_FLOAT:
            case BehaviorOpcode::ADD_INT:
            case BehaviorOpcode::SET_INT:
            case BehaviorOpcode::OR_INT:
            case BehaviorOpcode::BIT_CLEAR: {
                auto field = cur_behavior_cmd_u8(0x01);
                auto value = cur_behavior_cmd_s16(0x02);
                arguments.emplace_back(field);
                arguments.emplace_back(value);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::SET_INT_RAND_RSHIFT:
            case BehaviorOpcode::SET_RANDOM_FLOAT:
            case BehaviorOpcode::SET_RANDOM_INT:
            case BehaviorOpcode::ADD_RANDOM_FLOAT:
            case BehaviorOpcode::ADD_INT_RAND_RSHIFT: {
                auto field = cur_behavior_cmd_u8(0x01);
                auto min = cur_behavior_cmd_s16(0x02);
                auto rangeRShift = cur_behavior_cmd_s16(0x04);
                arguments.emplace_back(field);
                arguments.emplace_back(min);
                arguments.emplace_back(rangeRShift);
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::CMD_NOP_1:
            case BehaviorOpcode::CMD_NOP_2:
            case BehaviorOpcode::CMD_NOP_3: {
                auto field = cur_behavior_cmd_u8(0x01);
                arguments.emplace_back(field);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::SET_MODEL: {
                auto model = cur_behavior_cmd_s16(0x02);
                arguments.emplace_back(model);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::SPAWN_CHILD: {
                auto modelId = cur_behavior_cmd_s32(0x04);
                uint64_t behavior = cur_behavior_cmd_u32(0x08);
                arguments.emplace_back(modelId);
                arguments.emplace_back(behavior);
                cmd += 0xC << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::DEACTIVATE: {
                processing = false;
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::DROP_TO_FLOOR: {
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::SUM_FLOAT:
            case BehaviorOpcode::SUM_INT: {
                auto fieldDst = cur_behavior_cmd_u8(0x01);
                auto fieldSrc1 = cur_behavior_cmd_u8(0x02);
                auto fieldSrc2 = cur_behavior_cmd_u8(0x03);
                arguments.emplace_back(fieldDst);
                arguments.emplace_back(fieldSrc1);
                arguments.emplace_back(fieldSrc2);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::BILLBOARD:
            case BehaviorOpcode::HIDE: {
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::SET_HITBOX: {
                auto radius = cur_behavior_cmd_s16(0x04);
                auto height = cur_behavior_cmd_s16(0x06);
                arguments.emplace_back(radius);
                arguments.emplace_back(height);
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::CMD_NOP_4: {
                auto field = cur_behavior_cmd_u8(0x01);
                auto value = cur_behavior_cmd_s16(0x02);
                arguments.emplace_back(field);
                arguments.emplace_back(value);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::DELAY_VAR: {
                auto field = cur_behavior_cmd_u8(0x01);
                arguments.emplace_back(field);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::BEGIN_REPEAT_UNUSED: {
                auto count = cur_behavior_cmd_u8(0x01);
                arguments.emplace_back(count);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::LOAD_ANIMATIONS: {
                auto field = cur_behavior_cmd_u8(0x01);
                uint64_t anims = cur_behavior_cmd_u32(0x04);
                arguments.emplace_back(field);
                arguments.emplace_back(anims);
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::ANIMATE: {
                auto animIndex = cur_behavior_cmd_u8(0x01);
                arguments.emplace_back(animIndex);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::SPAWN_CHILD_WITH_PARAM: {
                auto bhvParam = cur_behavior_cmd_u8(0x01);
                auto modelId = cur_behavior_cmd_s32(0x04);
                uint64_t behavior = cur_behavior_cmd_u32(0x08);
                arguments.emplace_back(bhvParam);
                arguments.emplace_back(modelId);
                arguments.emplace_back(behavior);
                cmd += 0xC << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::LOAD_COLLISION_DATA: {
                uint64_t collisionData = cur_behavior_cmd_u32(0x04);
                arguments.emplace_back(collisionData);
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::SET_HITBOX_WITH_OFFSET: {
                auto radius = cur_behavior_cmd_s16(0x04);
                auto height = cur_behavior_cmd_s16(0x06);
                auto downOffset = cur_behavior_cmd_s16(0x08);
                arguments.emplace_back(radius);
                arguments.emplace_back(height);
                arguments.emplace_back(downOffset);
                cmd += 0xC << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::SPAWN_OBJ: {
                auto modelId = cur_behavior_cmd_s32(0x04);
                uint64_t behavior = cur_behavior_cmd_u32(0x08);
                arguments.emplace_back(modelId);
                arguments.emplace_back(behavior);
                cmd += 0xC << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::SET_HOME: {
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::SET_HURTBOX: {
                auto radius = cur_behavior_cmd_s16(0x04);
                auto height = cur_behavior_cmd_s16(0x06);
                arguments.emplace_back(radius);
                arguments.emplace_back(height);
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::SET_INTERACT_TYPE: {
                auto type = cur_behavior_cmd_s32(0x04);
                arguments.emplace_back(type);
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::SET_OBJ_PHYSICS: {
                auto wallHitboxRadius = cur_behavior_cmd_s16(0x04);
                auto gravity = cur_behavior_cmd_s16(0x06);
                auto bounciness = cur_behavior_cmd_s16(0x08);
                auto dragStrength = cur_behavior_cmd_s16(0x0A);
                auto friction = cur_behavior_cmd_s16(0x0C);
                auto buoyancy = cur_behavior_cmd_s16(0x0E);
                auto unused1 = cur_behavior_cmd_s16(0x10);
                auto unused2 = cur_behavior_cmd_s16(0x12);
                arguments.emplace_back(wallHitboxRadius);
                arguments.emplace_back(gravity);
                arguments.emplace_back(bounciness);
                arguments.emplace_back(dragStrength);
                arguments.emplace_back(friction);
                arguments.emplace_back(buoyancy);
                arguments.emplace_back(unused1);
                arguments.emplace_back(unused2);
                cmd += 0x14 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::SET_INTERACT_SUBTYPE: {
                auto subType = cur_behavior_cmd_s32(0x04);
                arguments.emplace_back(subType);
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::SCALE: {
                auto unusedField = cur_behavior_cmd_u8(0x01);
                auto percent = cur_behavior_cmd_s16(0x02);
                arguments.emplace_back(unusedField);
                arguments.emplace_back(percent);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::PARENT_BIT_CLEAR: {
                auto field = cur_behavior_cmd_u8(0x01);
                auto flags = cur_behavior_cmd_s32(0x04);
                arguments.emplace_back(field);
                arguments.emplace_back(flags);
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::ANIMATE_TEXTURE: {
                auto field = cur_behavior_cmd_u8(0x01);
                auto rate = cur_behavior_cmd_s16(0x02);
                arguments.emplace_back(field);
                arguments.emplace_back(rate);
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::DISABLE_RENDERING: {
                cmd += 0x4 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::SET_INT_UNUSED: {
                auto field = cur_behavior_cmd_u8(0x01);
                auto value = cur_behavior_cmd_s16(0x06);
                arguments.emplace_back(field);
                arguments.emplace_back(value);
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            case BehaviorOpcode::SPAWN_WATER_DROPLET: {
                uint64_t dropletParams = cur_behavior_cmd_s32(0x04);
                arguments.emplace_back(dropletParams);
                cmd += 0x8 << CMD_SIZE_SHIFT;
                break;
            }
            default:
                throw std::runtime_error("Unknown Behavior Opcode");
        }

        commands.emplace_back(opcode, arguments);
    }

    return std::make_shared<SM64::BehaviorScriptData>(commands);
}
