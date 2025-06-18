#include "GeoLayoutFactory.h"

#include "spdlog/spdlog.h"
#include "geo/GeoCommand.h"
#include "Companion.h"
#include "geo/GeoUtils.h"
#include "utils/TorchUtils.h"

#include <regex>

std::unordered_map<uint32_t, std::string> gFunctionMap;
static std::regex pattern(R"(0x([0-9a-fA-F]+)\s{16}([^\s]+))");

uint64_t RegisterAutoGen(uint32_t ptr, std::string type) {

    if (ptr != 0) {
        YAML::Node node;
        node["type"] = type;
        node["offset"] = ptr;
        Companion::Instance->AddAsset(node);
    } else {
        SPDLOG_WARN("RegisterAutoGen: ptr is 0 type: {}", type);
    }

    return ptr;
}

void StoreFunc(uint32_t vram) {
    return;

    if(!gFunctionMap.contains(vram)) {
        return;
    }

    auto name = gFunctionMap[vram];
    SPDLOG_INFO("Found Function: 0x{:X} Name: {}", vram, name);

    std::ofstream outfile;
    outfile.open("map.txt", std::ios_base::app);
    outfile << "{ 0x" << std::hex << vram << ", " << name << " },\n";

    gFunctionMap.erase(vram);
}

SM64::GeoLayoutFactory::GeoLayoutFactory() {
    // std::ifstream file("/Users/lywx/Downloads/sm64.jp.map");
    // std::string str;
    // while (std::getline(file, str)) {
    //     if(str.find('=') != std::string::npos) {
    //         continue;
    //     }

    //     std::smatch match;
    //     if (std::regex_search(str, match, pattern)) {
    //         gFunctionMap[std::stoul(match[1].str(), nullptr, 16)] = match[2].str();
    //     }
    // }
}

ExportResult SM64::GeoCodeExporter::Export(std::ostream&write, std::shared_ptr<IParsedData> data, std::string&entryName, YAML::Node&node, std::string* replacement) {
    const auto cmds = std::static_pointer_cast<GeoLayout>(data)->commands;
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    uint32_t indentCount = 1;
    uint32_t cmdCount = 0;

    write << "GeoLayout " << symbol << "[] = {\n";

    for(auto& [opcode, arguments, skip] : cmds) {
        bool commaFlag = false;

        if (opcode == GeoOpcode::OpenNode) {
            ++indentCount;
        }

        for (uint32_t i = 0; i < indentCount; ++i) {
            write << fourSpaceTab;
        }

        if (opcode == GeoOpcode::CloseNode) {
            --indentCount;
        }

        write << opcode << "(";
        for(auto& args : arguments) {
            if (commaFlag) {
                write << ", ";
            } else {
                commaFlag = true;
            }

            switch(static_cast<GeoArgumentType>(args.index())) {
                case GeoArgumentType::U8: {
                    write << std::hex << "0x" << static_cast<uint32_t>(std::get<uint8_t>(args));
                    break;
                }
                case GeoArgumentType::S8: {
                    write << std::hex << "0x" << static_cast<uint32_t>(std::get<int8_t>(args));
                    break;
                }
                case GeoArgumentType::U16: {
                    write << std::hex << "0x" << std::get<uint16_t>(args);
                    break;
                }
                case GeoArgumentType::S16: {
                    write << std::dec << std::get<int16_t>(args);
                    break;
                }
                case GeoArgumentType::U32: {
                    write << std::hex << "0x" << std::get<uint32_t>(args);
                    break;
                }
                case GeoArgumentType::S32: {
                    write << std::dec << std::get<int32_t>(args);
                    break;
                }
                case GeoArgumentType::U64: {
                    // write << std::hex << "0x" << std::get<uint64_t>(args);
                    uint32_t ptr = std::get<uint64_t>(args);
                    auto dec = Companion::Instance->GetNodeByAddr(ptr);
                    std::string symbol = "NULL";

                    if (dec.has_value()) {
                        auto node = std::get<1>(dec.value());
                        symbol = GetSafeNode<std::string>(node, "symbol");
                        write << symbol;
                    } else if (ptr == 0) {
                        write << symbol;
                    } else {
                        SPDLOG_WARN("Cannot find node for ptr 0x{:X}", ptr);
                        write << std::hex << "0x" << ptr;
                    }
                    break;
                }
                case GeoArgumentType::VEC2F: {
                    const auto [x, y] = std::get<Vec2f>(args);
                    write << std::dec << x << ", " << y;
                    break;
                }
                case GeoArgumentType::VEC3F: {
                    const auto [x, y, z] = std::get<Vec3f>(args);
                    write << std::dec << x << ", " << y << ", " << z;
                    break;
                }
                case GeoArgumentType::VEC3S: {
                    const auto [x, y, z] = std::get<Vec3s>(args);
                    write << std::dec <<  x << ", " << y << ", " << z;
                    break;
                }
                case GeoArgumentType::VEC3I: {
                    const auto [x, y, z] = std::get<Vec3i>(args);
                    write << std::dec <<  x << ", " << y << ", " << z;
                    break;
                }
                case GeoArgumentType::VEC4F: {
                    const auto [x, y, z, w] = std::get<Vec4f>(args);
                    write << std::dec << x << ", " << y << ", " << z << ", " << w;
                    break;
                }
                case GeoArgumentType::VEC4S: {
                    const auto [x, y, z, w] = std::get<Vec4s>(args);
                    write << std::dec <<  x << ", " << y << ", " << z << ", " << w;
                    break;
                }
                case GeoArgumentType::STRING: {
                    write << std::get<std::string>(args);
                    break;
                }
                default: {
                    break;
                }
            }
        }
        if(skip){
            write << "), //! more close than open nodes\n";
        } else {
            write << "),\n";
        }
        ++cmdCount;
    }

    write << "};\n";

    if (Companion::Instance->IsDebug()) {
        write << "// count: " << std::to_string(cmdCount) << " GeoLayout\n";
    } else {
        write << "\n";
    }

    return std::nullopt;
}

ExportResult SM64::GeoBinaryExporter::Export(std::ostream&write, std::shared_ptr<IParsedData> data, std::string&entryName, YAML::Node&node, std::string* replacement) {
    const auto layout = std::static_pointer_cast<GeoLayout>(data).get();

    auto writer = LUS::BinaryWriter();

    for(auto& [opcode, arguments, skip] : layout->commands) {
        if(skip){
            continue;
        }
        writer.Write(static_cast<uint8_t>(opcode));

        for(auto& args : arguments) {
            switch(static_cast<GeoArgumentType>(args.index())) {
                case GeoArgumentType::U8: {
                    writer.Write(std::get<uint8_t>(args));
                    break;
                }
                case GeoArgumentType::S8: {
                    writer.Write(std::get<int8_t>(args));
                    break;
                }
                case GeoArgumentType::U16: {
                    writer.Write(std::get<uint16_t>(args));
                    break;
                }
                case GeoArgumentType::S16: {
                    writer.Write(std::get<int16_t>(args));
                    break;
                }
                case GeoArgumentType::U32: {
                    writer.Write(std::get<uint32_t>(args));
                    break;
                }
                case GeoArgumentType::S32: {
                    writer.Write(std::get<int32_t>(args));
                    break;
                }
                case GeoArgumentType::U64: {
                    auto ptr = std::get<uint64_t>(args);
                    auto dec = Companion::Instance->GetNodeByAddr(ptr);
                    if (ptr == 0) {
                        writer.Write((uint64_t)0);
                    } else if (dec.has_value()) {
                        uint64_t hash = CRC64(std::get<0>(dec.value()).c_str());
                        SPDLOG_INFO("Found Asset: 0x{:X} Hash: 0x{:X} Path: {}", ptr, hash, std::get<0>(dec.value()));
                        writer.Write(hash);
                    } else {
                        SPDLOG_WARN("Could not find Asset at 0x{:X}", ptr);
                    }
                    break;
                }
                case GeoArgumentType::VEC2F: {
                    const auto [x, y] = std::get<Vec2f>(args);
                    writer.Write(x);
                    writer.Write(y);
                    break;
                }
                case GeoArgumentType::VEC3F: {
                    const auto [x, y, z] = std::get<Vec3f>(args);
                    writer.Write(x);
                    writer.Write(y);
                    writer.Write(z);
                    break;
                }
                case GeoArgumentType::VEC3S: {
                    const auto [x, y, z] = std::get<Vec3s>(args);
                    writer.Write(x);
                    writer.Write(y);
                    writer.Write(z);
                    break;
                }
                case GeoArgumentType::VEC3I: {
                    const auto [x, y, z] = std::get<Vec3i>(args);
                    writer.Write(x);
                    writer.Write(y);
                    writer.Write(z);
                    break;
                }
                case GeoArgumentType::VEC4F: {
                    const auto [x, y, z, w] = std::get<Vec4f>(args);
                    writer.Write(x);
                    writer.Write(y);
                    writer.Write(z);
                    writer.Write(w);
                    break;
                }
                case GeoArgumentType::VEC4S: {
                    const auto [x, y, z, w] = std::get<Vec4s>(args);
                    writer.Write(x);
                    writer.Write(y);
                    writer.Write(z);
                    writer.Write(w);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }

    std::vector<char> buffer = writer.ToVector();
    writer.Close();

    LUS::BinaryWriter output = LUS::BinaryWriter();
    WriteHeader(output, Torch::ResourceType::Blob, 0);

    output.Write(static_cast<uint32_t>(buffer.size()));
    output.Write(buffer.data(), buffer.size());
    output.Finish(write);
    output.Close();
    return std::nullopt;
}

ExportResult SM64::GeoHeaderExporter::Export(std::ostream&write, std::shared_ptr<IParsedData> data, std::string&entryName, YAML::Node&node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern GeoLayout " << symbol << "[];\n";

    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> SM64::GeoLayoutFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    auto cmd = segment.data;

    bool processing = true;
    int32_t openCount = 0;
    std::vector<GeoCommand> commands;

    while(processing) {
        auto opcode = static_cast<GeoOpcode>(cmd[0x00]);
        auto skip = false;

        SPDLOG_INFO("Processing Command {}", opcode);

        std::vector<GeoArgument> arguments;

        switch(opcode){
            case GeoOpcode::BranchAndLink: {
                auto ptr = cur_geo_cmd_u32(0x04);
                if (ptr == 0) {
                    processing = false;
                }
                arguments.emplace_back(RegisterAutoGen(ptr, "SM64:GEO_LAYOUT"));

                cmd += 0x08 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::End: {
                processing = false;
                break;
            }
            case GeoOpcode::Branch: {
                auto jmp = cur_geo_cmd_u8(0x01);
                auto ptr = cur_geo_cmd_u32(0x04);

                arguments.emplace_back(jmp);
                arguments.emplace_back(RegisterAutoGen(ptr, "SM64:GEO_LAYOUT"));

                cmd += 0x08 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::Return: {
                processing = false;
                break;
            }
            case GeoOpcode::OpenNode: {
                openCount++;
                cmd += 0x04 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::CloseNode: {
                cmd += 0x04 << CMD_SIZE_SHIFT;
                if (openCount - 1 < 0) {
                    skip = true;
                } else {
                    openCount--;
                }
                break;
            }
            case GeoOpcode::AssignAsView: {
                auto idx = cur_geo_cmd_s16(0x02);
                arguments.emplace_back(idx);

                cmd += 0x04 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::UpdateNodeFlags: {
                auto operation = cur_geo_cmd_u8(0x01);
                auto flags = cur_geo_cmd_s16(0x02);
                arguments.emplace_back(operation);
                arguments.emplace_back(flags);

                cmd += 0x04 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::NodeRoot: {
                auto views = cur_geo_cmd_s16(0x02);
                auto x = cur_geo_cmd_s16(0x04);
                auto y = cur_geo_cmd_s16(0x06);
                auto width = cur_geo_cmd_s16(0x08);
                auto height = cur_geo_cmd_s16(0x0A);

                arguments.emplace_back(views);
                arguments.emplace_back(x);
                arguments.emplace_back(y);
                arguments.emplace_back(width);
                arguments.emplace_back(height);

                cmd += 0x0C << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::NodeOrthoProjection: {
                auto scale = cur_geo_cmd_s16(0x02);
                arguments.emplace_back(scale);

                cmd += 0x04 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::NodePerspective: {
                auto opt = cur_geo_cmd_u8(0x01);
                auto fov = cur_geo_cmd_s16(0x02);
                auto near = cur_geo_cmd_s16(0x04);
                auto far = cur_geo_cmd_s16(0x06);

                arguments.emplace_back(opt);
                arguments.emplace_back(fov);
                arguments.emplace_back(near);
                arguments.emplace_back(far);

                if (opt != 0) {
                    // optional asm function
                    auto ptr = cur_geo_cmd_u32(0x08);
                    arguments.emplace_back(ptr);

                    StoreFunc(ptr);

                    cmd += 0x04 << CMD_SIZE_SHIFT;
                }

                cmd += 0x08 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::NodeStart: {
                cmd += 0x04 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::NodeMasterList: {
                auto list = cur_geo_cmd_u8(0x01);
                arguments.emplace_back(list);

                cmd += 0x04 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::NodeLevelOfDetail: {
                auto min = cur_geo_cmd_s16(0x04);
                auto max = cur_geo_cmd_s16(0x06);

                arguments.emplace_back(min);
                arguments.emplace_back(max);

                cmd += 0x08 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::NodeSwitchCase: {
                auto cs = cur_geo_cmd_s16(0x02);
                auto ptr = cur_geo_cmd_u32(0x04);

                arguments.emplace_back(cs);
                arguments.emplace_back(ptr);

                StoreFunc(ptr);

                cmd += 0x08 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::NodeCamera: {
                auto cmd_pos = reinterpret_cast<int16_t*>(&cmd[4]);

                Vec3f pos = {};
                Vec3f focus = {};
                cmd_pos = read_vec3s_to_vec3f(pos, cmd_pos);
                read_vec3s_to_vec3f(focus, cmd_pos);

                auto ptr = cur_geo_cmd_u32(0x10);
                auto type = cur_geo_cmd_s16(0x02);

                arguments.emplace_back(type);
                arguments.emplace_back(pos);
                arguments.emplace_back(focus);
                arguments.emplace_back(ptr);

                StoreFunc(ptr);

                cmd += 0x14 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::NodeTranslationRotation: {

                Vec3s translation = {};
                Vec3s rotation = {};
                auto params = cur_geo_cmd_u8(0x01);
                auto cmd_pos = reinterpret_cast<int16_t *>(cmd);

                arguments.emplace_back(params);

                switch ((params & 0x70) >> 4) {
                    case 0:
                        cmd_pos = read_vec3s(translation, &cmd_pos[2]);
                        cmd_pos = read_vec3s_angle(rotation, cmd_pos);
                        arguments.emplace_back(translation);
                        arguments.emplace_back(rotation);
                        break;
                    case 1:
                        cmd_pos = read_vec3s(translation, &cmd_pos[1]);
                        arguments.emplace_back(translation);
                        break;
                    case 2:
                        cmd_pos = read_vec3s_angle(rotation, &cmd_pos[1]);
                        arguments.emplace_back(rotation);
                        break;
                    case 3:
                        arguments.emplace_back(cmd_pos[1]);
                        cmd_pos += 0x04 << CMD_SIZE_SHIFT;
                        break;
                    default: {
                        break;
                    }
                }

                if (params & 0x80) {
                    auto ptr = BSWAP32(*reinterpret_cast<uint32_t*>(&cmd_pos[0]));
                    arguments.emplace_back(RegisterAutoGen(ptr, "GFX"));
                    cmd_pos += 2 << CMD_SIZE_SHIFT;
                }

                cmd = reinterpret_cast<uint8_t *>(cmd_pos);
                break;
            }
            case GeoOpcode::NodeTranslation:
            case GeoOpcode::NodeRotation: {
                Vec3s vector = {};
                auto params = cur_geo_cmd_u8(0x01);
                auto cmd_pos = reinterpret_cast<int16_t *>(cmd);

                arguments.emplace_back(params);

                cmd_pos = read_vec3s_angle(vector, &cmd_pos[1]);

                arguments.emplace_back(vector);

                if (params & 0x80) {
                    auto ptr = BSWAP32(*reinterpret_cast<uint32_t*>(&cmd_pos[0]));
                    arguments.emplace_back(RegisterAutoGen(ptr, "GFX"));
                    cmd_pos += 2 << CMD_SIZE_SHIFT;
                }

                cmd = reinterpret_cast<uint8_t *>(cmd_pos);
                break;
            }
            case GeoOpcode::NodeAnimatedPart: {

                Vec3s translation = {};
                auto layer = cur_geo_cmd_u8(0x01);
                auto  ptr = cur_geo_cmd_u32(0x08);
                auto cmd_pos = reinterpret_cast<int16_t*>(cmd);

                arguments.emplace_back(layer);

                read_vec3s(translation, &cmd_pos[1]);
                arguments.emplace_back(translation);

                arguments.emplace_back(RegisterAutoGen(ptr, "GFX"));

                cmd += 0x0C << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::NodeBillboard: {

                Vec3s translation = {};
                auto params = cur_geo_cmd_u8(0x01);
                auto cmd_pos = reinterpret_cast<int16_t*>(cmd);

                cmd_pos = read_vec3s(translation, &cmd_pos[1]);

                arguments.emplace_back(params);
                arguments.emplace_back(translation);

                if (params & 0x80) {
                    auto ptr = BSWAP32(*reinterpret_cast<uint32_t*>(&cmd_pos[0]));
                    arguments.emplace_back(RegisterAutoGen(ptr, "GFX"));
                    cmd_pos += 0x02 << CMD_SIZE_SHIFT;
                }

                cmd = reinterpret_cast<uint8_t*>(cmd_pos);
                break;
            }
            case GeoOpcode::NodeDisplayList: {
                auto layer = cur_geo_cmd_u8(0x01);
                auto ptr = cur_geo_cmd_u32(0x04);

                arguments.emplace_back(layer);
                arguments.emplace_back(RegisterAutoGen(ptr, "GFX"));

                cmd += 0x08 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::NodeShadow: {
                auto type = cur_geo_cmd_s16(0x02);
                auto solidity = cur_geo_cmd_s16(0x04);
                auto scale = cur_geo_cmd_s16(0x06);

                arguments.emplace_back(type);
                arguments.emplace_back(solidity);
                arguments.emplace_back(scale);

                cmd += 0x08 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::NodeObjectParent: {
                cmd += 0x04 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::NodeAsm: {
                auto param = cur_geo_cmd_s16(0x02);
                auto ptr = cur_geo_cmd_u32(0x04);

                arguments.emplace_back(param);
                arguments.emplace_back(ptr);
                StoreFunc(ptr);

                cmd += 0x08 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::NodeBackground: {
                auto bg = cur_geo_cmd_s16(0x02);
                auto ptr = cur_geo_cmd_u32(0x04);

                arguments.emplace_back(bg);
                arguments.emplace_back(ptr);

                StoreFunc(ptr);

                cmd += 0x08 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::NOP: {
                cmd += 0x08 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::CopyView: {
                auto idx = cur_geo_cmd_s16(0x02);

                arguments.emplace_back(idx);

                cmd += 0x04 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::NodeHeldObj: {
                auto ptr = cur_geo_cmd_u32(0x08);
                auto player = cur_geo_cmd_u8(0x01);

                StoreFunc(ptr);

                arguments.emplace_back(ptr);
                arguments.emplace_back(player);

                Vec3s vec = {};
                read_vec3s(vec, reinterpret_cast<int16_t*>(&cmd[0x02]));

                arguments.emplace_back(vec);

                cmd += 0x0C << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::NodeScale: {
                auto params = cur_geo_cmd_u8(0x01);
                auto scale  = cur_geo_cmd_u32(0x04);

                arguments.emplace_back(params);
                arguments.emplace_back(scale);

                if (params & 0x80) {
                    auto ptr = cur_geo_cmd_u32(0x08);
                    arguments.emplace_back(RegisterAutoGen(ptr, "GFX"));
                    cmd += 0x04 << CMD_SIZE_SHIFT;
                }

                cmd += 0x08 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::NOP2: {
                cmd += 0x08 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::NOP3: {
                cmd += 0x10 << CMD_SIZE_SHIFT;
                break;
            }
            case GeoOpcode::NodeCullingRadius: {
                auto radius = cur_geo_cmd_s16(0x02);

                arguments.emplace_back(radius);

                cmd += 0x04 << CMD_SIZE_SHIFT;
                break;
            }
            default: {
                SPDLOG_ERROR("Unknown geo command '{}'", opcode);
                throw std::runtime_error("Unknown geo command " + std::to_string(static_cast<uint8_t>(opcode)));
            }
        }

        commands.push_back({ opcode, arguments, skip });
    }

    return std::make_shared<GeoLayout>(commands);
}
