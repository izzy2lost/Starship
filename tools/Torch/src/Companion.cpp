#include "Companion.h"

#include "utils/Decompressor.h"
#include "utils/TorchUtils.h"
#include "archive/SWrapper.h"
#include "archive/ZWrapper.h"
#include "spdlog/spdlog.h"
#include "hj/sha1.h"

#include <regex>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "factories/GenericArrayFactory.h"
#include "factories/VtxFactory.h"
#include "factories/MtxFactory.h"
#include "factories/FloatFactory.h"
#include "factories/IncludeFactory.h"
#include "factories/TextureFactory.h"
#include "factories/DisplayListFactory.h"
#include "factories/DisplayListOverrides.h"
#include "factories/BlobFactory.h"
#include "factories/LightsFactory.h"
#include "factories/Vec3fFactory.h"
#include "factories/Vec3sFactory.h"
#include "factories/AssetArrayFactory.h"
#include "factories/ViewportFactory.h"
#include "factories/CompressedTextureFactory.h"

#ifdef SM64_SUPPORT
#include "factories/sm64/AnimationFactory.h"
#include "factories/sm64/BehaviorScriptFactory.h"
#include "factories/sm64/CollisionFactory.h"
#include "factories/sm64/DialogFactory.h"
#include "factories/sm64/DictionaryFactory.h"
#include "factories/sm64/TextFactory.h"
#include "factories/sm64/GeoLayoutFactory.h"
#include "factories/sm64/LevelScriptFactory.h"
#include "factories/sm64/MacroFactory.h"
#include "factories/sm64/MovtexFactory.h"
#include "factories/sm64/MovtexQuadFactory.h"
#include "factories/sm64/PaintingFactory.h"
#include "factories/sm64/PaintingMapFactory.h"
#include "factories/sm64/TrajectoryFactory.h"
#include "factories/sm64/WaterDropletFactory.h"
#endif

#ifdef MK64_SUPPORT
#include "factories/mk64/CourseVtx.h"
#include "factories/mk64/Waypoints.h"
#include "factories/mk64/TrackSections.h"
#include "factories/mk64/SpawnData.h"
#include "factories/mk64/UnkSpawnData.h"
#include "factories/mk64/DrivingBehaviour.h"
#include "factories/mk64/ItemCurve.h"
#include "factories/mk64/CourseMetadata.h"
#endif

#ifdef SF64_SUPPORT
#include "factories/sf64/ColPolyFactory.h"
#include "factories/sf64/MessageFactory.h"
#include "factories/sf64/MessageLookupFactory.h"
#include "factories/sf64/SkeletonFactory.h"
#include "factories/sf64/AnimFactory.h"
#include "factories/sf64/ScriptFactory.h"
#include "factories/sf64/HitboxFactory.h"
#include "factories/sf64/EnvironmentFactory.h"
#include "factories/sf64/ObjInitFactory.h"
#include "factories/sf64/TriangleFactory.h"
#endif

#ifdef FZERO_SUPPORT
#include "factories/fzerox/CourseFactory.h"
#include "factories/fzerox/GhostRecordFactory.h"
#endif

#ifdef NAUDIO_SUPPORT
#include "factories/naudio/v0/AudioHeaderFactory.h"
#include "factories/naudio/v0/BankFactory.h"
#include "factories/naudio/v0/SampleFactory.h"
#include "factories/naudio/v0/SequenceFactory.h"

#include "factories/naudio/v1/AudioContext.h"
#include "factories/naudio/v1/SoundFontFactory.h"
#include "factories/naudio/v1/AudioTableFactory.h"
#include "factories/naudio/v1/InstrumentFactory.h"
#include "factories/naudio/v1/SampleFactory.h"
#include "factories/naudio/v1/DrumFactory.h"
#include "factories/naudio/v1/EnvelopeFactory.h"
#include "factories/naudio/v1/LoopFactory.h"
#include "factories/naudio/v1/BookFactory.h"
#include "factories/naudio/v1/SequenceFactory.h"
#endif

#include "preprocess/CompTool.h"

using namespace std::chrono;
namespace fs = std::filesystem;

static const std::string regular = "[%Y-%m-%d %H:%M:%S.%e] [%l] %v";
static const std::string line    = "[%Y-%m-%d %H:%M:%S.%e] [%l] > %v";

void Companion::Init(const ExportType type) {

    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

    this->gConfig.exporterType = type;
    this->RegisterFactory("BLOB", std::make_shared<BlobFactory>());
    this->RegisterFactory("TEXTURE", std::make_shared<TextureFactory>());
    this->RegisterFactory("VTX", std::make_shared<VtxFactory>());
    this->RegisterFactory("MTX", std::make_shared<MtxFactory>());
    this->RegisterFactory("F32", std::make_shared<FloatFactory>());
    this->RegisterFactory("INC", std::make_shared<IncludeFactory>());
    this->RegisterFactory("LIGHTS", std::make_shared<LightsFactory>());
    this->RegisterFactory("GFX", std::make_shared<DListFactory>());
    this->RegisterFactory("VEC3F", std::make_shared<Vec3fFactory>());
    this->RegisterFactory("VEC3S", std::make_shared<Vec3sFactory>());
    this->RegisterFactory("ARRAY", std::make_shared<GenericArrayFactory>());
    this->RegisterFactory("ASSET_ARRAY", std::make_shared<AssetArrayFactory>());
    this->RegisterFactory("VP", std::make_shared<ViewportFactory>());
    this->RegisterFactory("COMPRESSED_TEXTURE", std::make_shared<CompressedTextureFactory>());

#ifdef SM64_SUPPORT
    this->RegisterFactory("SM64:DIALOG", std::make_shared<SM64::DialogFactory>());
    this->RegisterFactory("SM64:TEXT", std::make_shared<SM64::TextFactory>());
    this->RegisterFactory("SM64:DICTIONARY", std::make_shared<SM64::DictionaryFactory>());
    this->RegisterFactory("SM64:ANIM", std::make_shared<SM64::AnimationFactory>());
    this->RegisterFactory("SM64:BEHAVIOR_SCRIPT", std::make_shared<SM64::BehaviorScriptFactory>());
    this->RegisterFactory("SM64:COLLISION", std::make_shared<SM64::CollisionFactory>());
    this->RegisterFactory("SM64:GEO_LAYOUT", std::make_shared<SM64::GeoLayoutFactory>());
    this->RegisterFactory("SM64:LEVEL_SCRIPT", std::make_shared<SM64::LevelScriptFactory>());
    this->RegisterFactory("SM64:MACRO", std::make_shared<SM64::MacroFactory>());
    this->RegisterFactory("SM64:MOVTEX_QUAD", std::make_shared<SM64::MovtexQuadFactory>());
    this->RegisterFactory("SM64:MOVTEX", std::make_shared<SM64::MovtexFactory>());
    this->RegisterFactory("SM64:PAINTING", std::make_shared<SM64::PaintingFactory>());
    this->RegisterFactory("SM64:PAINTING_MAP", std::make_shared<SM64::PaintingMapFactory>());
    this->RegisterFactory("SM64:TRAJECTORY", std::make_shared<SM64::TrajectoryFactory>());
    this->RegisterFactory("SM64:WATER_DROPLET", std::make_shared<SM64::WaterDropletFactory>());
#endif

#ifdef MK64_SUPPORT
    this->RegisterFactory("MK64:COURSE_VTX", std::make_shared<MK64::CourseVtxFactory>());
    this->RegisterFactory("MK64:TRACK_WAYPOINTS", std::make_shared<MK64::WaypointsFactory>());
    this->RegisterFactory("MK64:TRACK_SECTIONS", std::make_shared<MK64::TrackSectionsFactory>());
    this->RegisterFactory("MK64:SPAWN_DATA", std::make_shared<MK64::SpawnDataFactory>());
    this->RegisterFactory("MK64:UNK_SPAWN_DATA", std::make_shared<MK64::UnkSpawnDataFactory>());
    this->RegisterFactory("MK64:DRIVING_BEHAVIOUR", std::make_shared<MK64::DrivingBehaviourFactory>());
    this->RegisterFactory("MK64:ITEM_CURVE", std::make_shared<MK64::ItemCurveFactory>()); // Item curve for decomp only
    this->RegisterFactory("MK64:METADATA", std::make_shared<MK64::CourseMetadataFactory>());
#endif

#ifdef SF64_SUPPORT
    this->RegisterFactory("SF64:ANIM", std::make_shared<SF64::AnimFactory>());
    this->RegisterFactory("SF64:SKELETON", std::make_shared<SF64::SkeletonFactory>());
    this->RegisterFactory("SF64:MESSAGE", std::make_shared<SF64::MessageFactory>());
    this->RegisterFactory("SF64:MSG_TABLE", std::make_shared<SF64::MessageLookupFactory>());
    this->RegisterFactory("SF64:SCRIPT", std::make_shared<SF64::ScriptFactory>());
    this->RegisterFactory("SF64:HITBOX", std::make_shared<SF64::HitboxFactory>());
    this->RegisterFactory("SF64:ENVIRONMENT", std::make_shared<SF64::EnvironmentFactory>());
    this->RegisterFactory("SF64:OBJECT_INIT", std::make_shared<SF64::ObjInitFactory>());
    this->RegisterFactory("SF64:COLPOLY", std::make_shared<SF64::ColPolyFactory>());
    this->RegisterFactory("SF64:TRIANGLE", std::make_shared<SF64::TriangleFactory>());
#endif

#ifdef FZERO_SUPPORT
    this->RegisterFactory("FZX:COURSE", std::make_shared<FZX::CourseFactory>());
    this->RegisterFactory("FZX:GHOST", std::make_shared<FZX::GhostRecordFactory>());
#endif

#ifdef NAUDIO_SUPPORT
    this->RegisterFactory("NAUDIO:V0:AUDIO_HEADER", std::make_shared<AudioHeaderFactory>());
    this->RegisterFactory("NAUDIO:V0:SEQUENCE", std::make_shared<SequenceFactory>());
    this->RegisterFactory("NAUDIO:V0:SAMPLE", std::make_shared<SampleFactory>());
    this->RegisterFactory("NAUDIO:V0:BANK", std::make_shared<BankFactory>());

    this->RegisterFactory("NAUDIO:V1:AUDIO_SETUP", std::make_shared<AudioContextFactory>());
    this->RegisterFactory("NAUDIO:V1:AUDIO_TABLE", std::make_shared<AudioTableFactory>());
    this->RegisterFactory("NAUDIO:V1:SOUND_FONT", std::make_shared<SoundFontFactory>());
    this->RegisterFactory("NAUDIO:V1:INSTRUMENT", std::make_shared<InstrumentFactory>());
    this->RegisterFactory("NAUDIO:V1:DRUM", std::make_shared<DrumFactory>());
    this->RegisterFactory("NAUDIO:V1:SAMPLE", std::make_shared<NSampleFactory>());
    this->RegisterFactory("NAUDIO:V1:ENVELOPE", std::make_shared<EnvelopeFactory>());
    this->RegisterFactory("NAUDIO:V1:ADPCM_LOOP", std::make_shared<ADPCMLoopFactory>());
    this->RegisterFactory("NAUDIO:V1:ADPCM_BOOK", std::make_shared<ADPCMBookFactory>());
    this->RegisterFactory("NAUDIO:V1:SEQUENCE", std::make_shared<NSequenceFactory>());
#endif

    this->Process();
}

void Companion::ParseEnums(std::string& header) {
    std::ifstream file(header);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open header files for enums node in config");
    }

    std::regex enumRegex(R"(enum\s+(\w+)\s*(?:\s*:\s*(\w+))?[\s\n\r]*\{)");

    std::string line;
    std::smatch match;
    std::string enumName;

    bool inEnum = false;
    int enumIndex;
    while (std::getline(file, line)) {
        if (!inEnum && std::regex_search(line, match, enumRegex) && match.size() > 1) {
            enumName = match.str(1);
            inEnum = true;
            enumIndex = -1;
            continue;
        }

        if(!inEnum) {
            continue;
        }

        if(line.find('}') != std::string::npos) {
            inEnum = false;
            continue;
        }

        // Remove any comments and non-alphanumeric characters
        line = std::regex_replace(line, std::regex(R"((/\*.*?\*/)|(//.*$)|([^a-zA-Z0-9=_\-\.]))"), "");

        if(line.find('=') != std::string::npos) {
            auto value = line.substr(line.find('=') + 1);
            auto name = line.substr(0, line.find('='));
            enumIndex = static_cast<int32_t>(std::stoll(value, nullptr, 0));
            this->gEnums[enumName][enumIndex] = name;
        } else {
            enumIndex++;
            this->gEnums[enumName][enumIndex] = line;
        }

    }
}

std::optional<ParseResultData> Companion::ParseNode(YAML::Node& node, std::string& name) {
    auto type = GetSafeNode<std::string>(node, "type");
    std::transform(type.begin(), type.end(), type.begin(), ::toupper);

    spdlog::set_pattern(regular);
    if(node["offset"]) {
        auto offset = node["offset"].as<uint32_t>();
        SPDLOG_INFO("- [{}] Processing {} at 0x{:X}", type, name, offset);
    } else {
        SPDLOG_INFO("- [{}] Processing {}", type, name);
    }
    spdlog::set_pattern(line);
    node["vpath"] = name;

    auto factory = this->GetFactory(type);
    if(!factory.has_value()){
        throw std::runtime_error("No factory by the name '"+type+"' found for '"+name+"'");
    }

    auto impl = factory->get();

    auto exporter = impl->GetExporter(this->gConfig.exporterType);
    if(!exporter.has_value() && !impl->HasModdedDependencies()){
        SPDLOG_WARN("No exporter found for {}", name);
        return std::nullopt;
    }

    bool executeDef = true;
    std::optional<std::shared_ptr<IParsedData>> result;
    if(this->gConfig.modding && impl->SupportModdedAssets() && this->gModdedAssetPaths.contains(name)) {
        auto path = fs::path(this->gConfig.moddingPath) / this->gModdedAssetPaths[name];
        if(!exists(path)) {
            SPDLOG_ERROR("Modded asset {} not found", this->gModdedAssetPaths[name]);
        } else {
            std::ifstream input(path, std::ios::binary);
            std::vector<uint8_t> data = std::vector<uint8_t>( std::istreambuf_iterator( input ), {});
            input.close();

            result = impl->parse_modding(data, node);
            executeDef = !result.has_value();
        }
    }

    if(executeDef && this->gConfig.parseMode == ParseMode::Default) {
        result = impl->parse(this->gRomData, node);
    }

    if(executeDef && this->gConfig.parseMode == ParseMode::Directory) {
        auto path = GetSafeNode<std::string>(node, "path");
        std::ifstream input( path, std::ios::binary );
        auto data = std::vector<uint8_t>( std::istreambuf_iterator( input ), {} );
        result = impl->parse(data, node);
        input.close();
    }

    if(!result.has_value()){
        SPDLOG_ERROR("Failed to process {}", name);
        return std::nullopt;
    }

    SPDLOG_INFO("Processed {}", name);

    return ParseResultData {
        name, type, node, result
    };
}

void Companion::ParseModdingConfig() {
    auto path = fs::path(this->gConfig.moddingPath) / "modding.yml";
    if(!fs::exists(path)) {
        throw std::runtime_error("No modding config found, please run in export mode first");
    }
    auto modding = YAML::LoadFile(path.string());
    for(auto assets = modding["assets"].begin(); assets != modding["assets"].end(); ++assets) {
        auto name = assets->first.as<std::string>();
        auto asset = assets->second.as<std::string>();
        this->gModdedAssetPaths[name] = asset;
    }
}


void Companion::ParseCurrentFileConfig(YAML::Node node) {
    if (node["external_files"]) {
        auto externalFiles = node["external_files"];
        if (externalFiles.IsSequence() && externalFiles.size()) {
            for(size_t i = 0; i < externalFiles.size(); i++) {
                auto externalFile = externalFiles[i];
                if (externalFile.size() == 0) {
                    this->gCurrentExternalFiles.push_back(externalFile.as<std::string>());
                } else {
                    SPDLOG_INFO("External File size {}", externalFile.size());
                    throw std::runtime_error("Incorrect yaml syntax for external files.\n\nThe yaml expects:\n:config:\n  external_files:\n  - <external_files>\n\ne.g.:\nexternal_files:\n  - actors/actor1.yaml");
                }

                auto externalFileName = externalFile.as<std::string>();
                if (std::filesystem::relative(externalFileName, this->gAssetPath).string().starts_with("../")) {
                    throw std::runtime_error("External File " + externalFileName + " Not In Asset Directory " + this->gAssetPath);
                } else if (std::filesystem::relative(externalFileName, this->gAssetPath).string() == "") {
                    throw std::runtime_error("External File " + externalFileName + " Not In Asset Directory " + this->gAssetPath);
                }

                if (!this->gAddrMap.contains(externalFileName)) {
                    SPDLOG_INFO("Dependency on external file {}. Now processing {}", externalFileName, externalFileName);
                    auto currentFile = this->gCurrentFile;
                    auto currentDirectory = this->gCurrentDirectory;
                    auto currentExternalFiles = this->gCurrentExternalFiles;

                    this->gCurrentFile = externalFileName;
                    this->gCurrentDirectory = std::filesystem::relative(externalFileName, this->gAssetPath).replace_extension("");

                    YAML::Node root = YAML::LoadFile(externalFileName);

                    if (!this->gProcessedFiles.contains(this->gCurrentFile)) {
                        ProcessFile(root);
                        this->gProcessedFiles.insert(this->gCurrentFile);
                    }

                    SPDLOG_INFO("Finishing processing of file: {}", currentFile);

                    this->gCurrentFile = currentFile;
                    this->gCurrentDirectory = currentDirectory;
                    this->gCurrentExternalFiles = currentExternalFiles;
                    this->gFileHeader.clear();
                }
            }
        }
    }

    if(node["segments"]) {
        auto segments = node["segments"];

        // Set global variables for segmented data
        if (segments.IsSequence() && segments.size()) {
            if (segments[0].IsSequence() && segments[0].size() == 2) {
                gCurrentSegmentNumber = segments[0][0].as<uint32_t>();
                gCurrentFileOffset = segments[0][1].as<uint32_t>();
                gCurrentCompressionType = Decompressor::GetCompressionType(this->gRomData, gCurrentFileOffset);
                if(node["no_compression"]) {
                    gCurrentCompressionType = CompressionType::None;
                }
            } else {
                throw std::runtime_error("Incorrect yaml syntax for segments.\n\nThe yaml expects:\n:config:\n  segments:\n  - [<segment>, <file_offset>]\n\nLike so:\nsegments:\n  - [0x06, 0x821D10]");
            }
        }

        // Set file offset for later use.
        for(size_t i = 0; i < segments.size(); i++) {
            auto segment = segments[i];
            if (segment.IsSequence() && segment.size() == 2) {
                const auto id = segment[0].as<uint32_t>();
                const auto replacement = segment[1].as<uint32_t>();
                this->gConfig.segment.local[id] = replacement;
                SPDLOG_DEBUG("Segment {} replaced with 0x{:X}", id, replacement);
            } else {
                throw std::runtime_error("Incorrect yaml syntax for segments.\n\nThe yaml expects:\n:config:\n  segments:\n  - [<segment>, <file_offset>]\n\nLike so:\nsegments:\n  - [0x06, 0x821D10]");
            }
        }
    }

    if(node["header"]) {
        auto header = node["header"];
        switch (this->gConfig.exporterType) {
            case ExportType::Header: {
                if(header["header"].IsSequence()) {
                    for(auto line = header["header"].begin(); line != header["header"].end(); ++line) {
                        this->gFileHeader += line->as<std::string>() + "\n";
                    }
                }
                break;
            }
            case ExportType::Code: {
                if(header["code"].IsSequence()) {
                    for(auto line = header["code"].begin(); line != header["code"].end(); ++line) {
                        this->gFileHeader += line->as<std::string>() + "\n";
                    }
                }
                break;
            }
            default: break;
        }
    }

    if(node["tables"]){
        for(auto table = node["tables"].begin(); table != node["tables"].end(); ++table){
            auto name = table->first.as<std::string>();
            auto range = table->second["range"].as<std::vector<uint32_t>>();
            auto start = gCurrentSegmentNumber ? gCurrentSegmentNumber << 24 | range[0] : range[0];
            auto end = gCurrentSegmentNumber ? gCurrentSegmentNumber << 24 | range[1] : range[1];
            auto mode = GetSafeNode<std::string>(table->second, "mode", "APPEND");
            TableMode tMode = mode == "REFERENCE" ? TableMode::Reference : TableMode::Append;
            auto index_size = GetSafeNode<int32_t>(table->second, "index_size", -1);
            this->gTables.push_back({name, start, end, tMode, index_size});
        }
    }

    if(node["vram"]){
        auto vram = node["vram"];
        const auto addr = GetSafeNode<uint32_t>(vram, "addr");
        const auto offset = GetSafeNode<uint32_t>(vram, "offset");
        this->gCurrentVram = { addr, offset };
    }

    this->gEnablePadGen = GetSafeNode<bool>(node, "autopads", true);
    this->gNodeForceProcessing = GetSafeNode<bool>(node, "force", false);
    this->gIndividualIncludes = GetSafeNode<bool>(node, "individual_data_incs", false);
    this->gCurrentVirtualPath = GetSafeNode<std::string>(node, "path", "");
}

void Companion::ParseHash() {
    const std::string out = "torch.hash.yml";

    if(fs::exists(out)) {
        this->gHashNode = YAML::LoadFile(out);
    } else {
        this->gHashNode = YAML::Node();
    }
}

std::string ExportTypeToString(ExportType type) {
    switch (type) {
        case ExportType::Binary: return "Binary";
        case ExportType::Header: return "Header";
        case ExportType::Code: return "Code";
        case ExportType::Modding: return "Modding";
        case ExportType::XML: return "XML";
        default:
            throw std::runtime_error("Invalid ExportType");
    }
}

bool Companion::NodeHasChanges(const std::string& path) {

    if(this->gConfig.modding) {
        return true;
    }

    std::ifstream yaml(path);
    const std::vector<uint8_t> data = std::vector<uint8_t>(std::istreambuf_iterator( yaml ), {});
    this->gCurrentHash = CalculateHash(data);
    bool needsInit = true;

    if(this->gHashNode[path]) {
        auto entry = GetSafeNode<YAML::Node>(this->gHashNode, path);
        const auto hash = GetSafeNode<std::string>(entry, "hash", "no-hash");
        auto modes = GetSafeNode<YAML::Node>(entry, "extracted");
        auto extracted = GetSafeNode<bool>(modes, ExportTypeToString(this->gConfig.exporterType));

        if(hash == this->gCurrentHash) {
            needsInit = false;
            if(extracted) {
                SPDLOG_INFO("Skipping {} as it has not changed", path);
                return false;
            }
        }
    }

    if(needsInit) {
        this->gHashNode[path] = YAML::Node();
        this->gHashNode[path]["hash"] = this->gCurrentHash;
        this->gHashNode[path]["extracted"] = YAML::Node();
        for(size_t m = 0; m <= static_cast<size_t>(ExportType::Modding); m++) {
            this->gHashNode[path]["extracted"][ExportTypeToString(static_cast<ExportType>(m))] = false;
        }
    }

    return true;
}

void Companion::LoadYAMLRecursively(const std::string &dirPath, std::vector<YAML::Node> &result, bool skipRoot) {
    for (const auto &entry : std::filesystem::directory_iterator(dirPath)) {
        if (entry.is_directory()) {
            // Skip the root directory if specified
            if (skipRoot && entry.path() == dirPath) {
                continue;
            }

            // Recursive call for subdirectories
            LoadYAMLRecursively(entry.path().generic_string(), result, false);
        } else if (entry.path().extension() == ".yaml" || entry.path().extension() == ".yml") {
            // Load YAML file and add it to the result vector
            result.push_back(YAML::LoadFile(entry.path().generic_string()));
        }
    }
}

/**
 * Config yaml requires tables: [assets/courses]
 * Activate the factory using a normal asset yaml with type, input_directory, and output_directory nodes.
 */
void Companion::ProcessTables(YAML::Node& rom) {
    auto dirs = rom["metadata"].as<std::vector<std::string>>();

    for (const auto &dir : dirs) {
        std::vector<YAML::Node> configNodes;
        LoadYAMLRecursively(dir, configNodes, true);
        gCourseMetadata[dir] = configNodes;
    }

    // Write yaml data to console
    if (this->IsDebug()) {
        SPDLOG_INFO("------ Metadata ouptut ------");
        for (auto &node : gCourseMetadata[dirs[0]]) {
            std::cout << node << std::endl;
            SPDLOG_INFO("------------");
        }
        SPDLOG_INFO("------ Metadata end ------");
    }
}

void Companion::ProcessFile(YAML::Node root) {
    // Set compressed file offsets and compression type
    if (auto segments = root[":config"]["segments"]) {
        if (segments.IsSequence() && segments.size() > 0) {
            if (segments[0].IsSequence() && segments[0].size() == 2) {
                gCurrentSegmentNumber = segments[0][0].as<uint32_t>();
                gCurrentFileOffset = segments[0][1].as<uint32_t>();
                gCurrentCompressionType = Decompressor::GetCompressionType(this->gRomData, gCurrentFileOffset);
                if(root[":config"]["no_compression"]) {
                    gCurrentCompressionType = CompressionType::None;
                }
            } else {
                throw std::runtime_error("Incorrect yaml syntax for segments.\n\nThe yaml expects:\n:config:\n  segments:\n  - [<segment>, <file_offset>]\n\nLike so:\nsegments:\n  - [0x06, 0x821D10]");
            }
        }
    }

    for(auto asset = root.begin(); asset != root.end(); ++asset){
        auto node = asset->second;
        auto entryName = asset->first.as<std::string>();
        auto output = (this->gCurrentDirectory / entryName).string();
        std::replace(output.begin(), output.end(), '\\', '/');

        if(node["type"]){
            const auto type = GetSafeNode<std::string>(node, "type");
            if(type == "NAUDIO:V0:SAMPLE"){
                AudioManager::Instance->bind_sample(node, output);
            }
        }

        if(!node["offset"])  {
            continue;
        }

        if(gCurrentSegmentNumber) {
            if (IS_SEGMENTED(node["offset"].as<uint32_t>()) == false) {
                node["offset"] = (gCurrentSegmentNumber << 24) | node["offset"].as<uint32_t>();
            }
        }

        if(!gCurrentVirtualPath.empty()) {
            node["path"] = gCurrentVirtualPath;
        }

        this->gAddrMap[this->gCurrentFile][node["offset"].as<uint32_t>()] = std::make_tuple(output, node);
    }

    // Stupid hack because the iteration broke the assets
    root = YAML::LoadFile(this->gCurrentFile);
    this->gConfig.segment.local.clear();
    this->gFileHeader.clear();
    this->gCurrentPad = 0;
    this->gCurrentVram = std::nullopt;
    this->gCurrentVirtualPath = "";
    this->gCurrentSegmentNumber = 0;
    this->gCurrentCompressionType = CompressionType::None;
    this->gCurrentFileOffset = 0;
    this->gTables.clear();
    this->gCurrentExternalFiles.clear();
    GFXDOverride::ClearVtx();

    if(root[":config"]) {
        this->ParseCurrentFileConfig(root[":config"]);
    }

    if(!this->NodeHasChanges(this->gCurrentFile) && !this->gNodeForceProcessing) {
        return;
    }

    spdlog::set_pattern(regular);
    SPDLOG_INFO("------------------------------------------------");
    spdlog::set_pattern(line);

    for(auto asset = root.begin(); asset != root.end(); ++asset){

        auto entryName = asset->first.as<std::string>();
        auto assetNode = asset->second;

        if(entryName.find(":config") != std::string::npos) {
            continue;
        }

        if(gCurrentFileOffset && assetNode["offset"]) {
            const auto offset = assetNode["offset"].as<uint32_t>();
            if (!IS_SEGMENTED(offset)) {
                assetNode["offset"] = (gCurrentSegmentNumber << 24) | offset;
            }
        }

        if(!gCurrentVirtualPath.empty()) {
            assetNode["path"] = gCurrentVirtualPath;
        }

        std::string output = (this->gCurrentDirectory / entryName).string();
        std::replace(output.begin(), output.end(), '\\', '/');
        this->gConfig.segment.temporal.clear();
        auto result = this->ParseNode(assetNode, output);
        if(result.has_value()) {
            this->gParseResults[this->gCurrentFile].push_back(result.value());
        }

        spdlog::set_pattern(regular);
        SPDLOG_INFO("------------------------------------------------");
        spdlog::set_pattern(line);
    }

    for(auto& result : this->gParseResults[this->gCurrentFile]){
        std::ostringstream stream;
        ExportResult endptr = std::nullopt;
        WriteEntry wEntry;

        auto data = result.data.value();
        const auto impl = this->GetFactory(result.type)->get();
        const auto exporter = impl->GetExporter(this->gConfig.exporterType);

        if(!exporter.has_value()) {
            continue;
        }

        switch (this->gConfig.exporterType) {
            case ExportType::Binary: {
                stream.str("");
                stream.clear();
                exporter->get()->Export(stream, data, result.name, result.node, &result.name);
                auto data = stream.str();
                this->gCurrentWrapper->AddFile(result.name, std::vector(data.begin(), data.end()));

                for(auto& entry : this->gCompanionFiles){
                    auto output = (this->gCurrentDirectory / entry.first).string();
                    std::replace(output.begin(), output.end(), '\\', '/');
                    this->gCurrentWrapper->AddFile(output, entry.second);
                }

                break;
            }
            case ExportType::XML:
            case ExportType::Modding: {
                stream.str("");
                stream.clear();
                std::string ogname = result.name;
                exporter->get()->Export(stream, data, result.name, result.node, &result.name);

                auto data = stream.str();
                if(data.empty()) {
                    break;
                }

                std::string dpath = Instance->GetOutputPath() + "/" + result.name;
                if(!exists(fs::path(dpath).parent_path())){
                    create_directories(fs::path(dpath).parent_path());
                }

                this->gModdedAssetPaths[ogname] = result.name;

                std::ofstream file(dpath, std::ios::binary);
                file.write(data.c_str(), data.size());
                file.close();

                for(auto& entry : this->gCompanionFiles){
                    auto cpath = (Instance->GetOutputPath() / this->gCurrentDirectory / entry.first).string();
                    std::replace(cpath.begin(), cpath.end(), '\\', '/');
                    if(!exists(fs::path(cpath).parent_path())){
                        create_directories(fs::path(cpath).parent_path());
                    }

                    std::ofstream cfile(cpath, std::ios::binary);
                    cfile.write(entry.second.data(), entry.second.size());
                    cfile.close();
                }

                break;
            }
            default: {
                endptr = exporter->get()->Export(stream, data, result.name, result.node, &result.name);
                break;
            }
        }

        this->gCompanionFiles.clear();

        if(result.node["offset"]) {
            auto alignment = GetSafeNode<uint32_t>(result.node, "alignment", impl->GetAlignment());
            if(!endptr.has_value()) {
                wEntry = {
                    result.name,
                    result.node["offset"].as<uint32_t>(),
                    alignment,
                    stream.str(),
                    GetNode<std::string>(result.node, "comment"),
                    std::nullopt
                };
            } else {
                switch (endptr->index()) {
                    case 0:
                        wEntry = {
                            result.name,
                            result.node["offset"].as<uint32_t>(),
                            alignment,
                            stream.str(),
                            GetNode<std::string>(result.node, "comment"),
                            std::get<size_t>(endptr.value())
                        };
                        break;
                    case 1: {
                        const auto oentry = std::get<OffsetEntry>(endptr.value());
                        wEntry = {
                            result.name,
                            oentry.start,
                            alignment,
                            stream.str(),
                            GetNode<std::string>(result.node, "comment"),
                            oentry.end
                        };
                        break;
                    }
                    default:
                        SPDLOG_ERROR("Invalid endptr index {}", endptr->index());
                        SPDLOG_ERROR("Type of endptr: {}", typeid(endptr).name());
                        throw std::runtime_error("We should never reach this point");
                }
            }
        }

        this->gWriteMap[this->gCurrentFile][result.type].push_back(wEntry);
    }

    auto fsout = fs::path(this->gConfig.outputPath);

    if(this->gConfig.exporterType == ExportType::Modding || this->gConfig.exporterType == ExportType::XML) {
        fsout /= "modding.yml";
        YAML::Node modding;

        for (const auto& [key, value] : this->gModdedAssetPaths) {
            modding["assets"][key] = value;
        }

        std::ofstream file(fsout.string(), std::ios::binary);
        file << modding;
        file.close();
    } else if(this->gConfig.exporterType != ExportType::Binary){
        std::string filename = this->gCurrentDirectory.filename().string();

        switch (this->gConfig.exporterType) {
            case ExportType::Header: {
                fsout /= this->gCurrentDirectory.parent_path() / (filename + ".h");
                break;
            }
            case ExportType::Code: {
                fsout /= this->gCurrentDirectory / (filename + ".c");
                break;
            }
            default: break;
        }

        std::ostringstream stream;

        std::vector<WriteEntry> entries;

        if(std::holds_alternative<std::string>(this->gWriteOrder)) {
            auto sort = std::get<std::string>(this->gWriteOrder);
            for (const auto& [type, raw] : this->gWriteMap[this->gCurrentFile]) {
                entries.insert(entries.end(), raw.begin(), raw.end());
            }

            if(sort == "OFFSET") {
                std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
                    return a.addr < b.addr;
                });
            } else if(sort == "ROFFSET") {
                std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
                    return a.addr > b.addr;
                });
            } else if(sort != "LINEAR") {
                throw std::runtime_error("Invalid write order");
            }
        } else {
            for (const auto& type : std::get<std::vector<std::string>>(this->gWriteOrder)) {
                entries = this->gWriteMap[this->gCurrentFile][type];

                std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
                    return a.addr > b.addr;
                });
            }
        }

        for (size_t i = 0; i < entries.size(); i++) {
            const auto result = entries[i];
            const auto hasSize = result.endptr.has_value();

            if(result.comment.has_value()){
                stream << "// " << result.comment.value() << "\n";
            }

            if (hasSize && this->IsDebug()) {
                stream << "// 0x" << std::hex << std::uppercase << ASSET_PTR(result.addr) << "\n";
            }

            stream << result.buffer;

            if (hasSize && this->IsDebug()) {
                stream << "// 0x" << std::hex << std::uppercase << ASSET_PTR(result.endptr.value()) << "\n\n";
            }

            if(hasSize && i < entries.size() - 1 && this->gConfig.exporterType == ExportType::Code && !this->gIndividualIncludes){
                int32_t startptr = ASSET_PTR(result.endptr.value());
                int32_t end = ASSET_PTR(entries[i + 1].addr);

                uint32_t alignment = entries[i + 1].alignment;
                int32_t gap = end - startptr;

                if(gap < 0) {
                    stream << "// WARNING: Overlap detected between 0x" << std::hex << startptr << " and 0x" << end << " with size 0x" << std::abs(gap) << "\n";
                    SPDLOG_WARN("Overlap detected between 0x{:X} and 0x{:X} with size 0x{:X} on file {}", startptr, end, gap, this->gCurrentFile);
                } else if(gap < 0x10 && gap >= alignment && end % alignment == 0 && this->gEnablePadGen) {
                    SPDLOG_WARN("Gap detected between 0x{:X} and 0x{:X} with size 0x{:X} on file {}", startptr, end, gap, this->gCurrentFile);
                    SPDLOG_WARN("Creating pad of 0x{:X} bytes", gap);
                    const auto padfile = this->gCurrentDirectory.filename().string();
                    if(this->IsDebug()){
                        stream << "// 0x" << std::hex << std::uppercase << startptr << "\n";
                    }
                    stream << "char pad_" << padfile << "_" << std::to_string(gCurrentPad++) << "[] = {\n" << tab_t;
                    auto gapSize = gap & ~3;
                    for(size_t j = 0; j < gapSize; j++){
                        stream << "0x00, ";
                    }
                    stream << "\n};\n";
                    if(this->IsDebug()){
                        stream << "// 0x" << std::hex << std::uppercase << end << "\n\n";
                    } else {
                        stream << "\n";
                    }
                } else if(gap >= 0x10) {
                    stream << "// WARNING: Gap detected between 0x" << std::hex << startptr << " and 0x" << end << " with size 0x" << gap << "\n";
                }
            }

            if (this->gConfig.exporterType == ExportType::Code && this->gIndividualIncludes) {
                fs::path outinc = fs::path(this->gConfig.outputPath) / this->gCurrentDirectory.parent_path() /
                    fs::relative(fs::path(result.name + ".inc.c"), this->gCurrentDirectory.parent_path());

                if(!exists(outinc.parent_path())){
                    create_directories(outinc.parent_path());
                }

                std::ofstream file(outinc, std::ios::binary);

                if(!this->gFileHeader.empty()) {
                    file << this->gFileHeader << std::endl;
                }
                file << stream.str();
                stream.str("");
                stream.seekp(0);
                file.close();
            }
        }

        this->gWriteMap.clear();

        if (this->gConfig.exporterType != ExportType::Code || !this->gIndividualIncludes) {
            std::string buffer = stream.str();

            if(buffer.empty()) {
                return;
            }

            std::string output = fsout.string();
            std::replace(output.begin(), output.end(), '\\', '/');
            if(!exists(fs::path(output).parent_path())){
                create_directories(fs::path(output).parent_path());
            }

            std::ofstream file(output, std::ios::binary);

            if(this->gConfig.exporterType == ExportType::Header) {
                fs::path entryPath = this->gCurrentFile;
                std::string symbol = entryPath.stem().string();
                std::transform(symbol.begin(), symbol.end(), symbol.begin(), toupper);
                if(this->IsOTRMode()){
                    file << "#pragma once\n\n";
                } else {
                    file << "#ifndef " << symbol << "_H" << std::endl;
                    file << "#define " << symbol << "_H" << std::endl << std::endl;
                }
                if(!this->gFileHeader.empty()) {
                    file << this->gFileHeader << std::endl;
                }
                file << buffer;
                if(!this->IsOTRMode()){
                    file << std::endl << "#endif" << std::endl;
                }
            } else {
                if(!this->gFileHeader.empty()) {
                    file << this->gFileHeader << std::endl;
                }
                file << buffer;
            }

            file.close();
        }
    }

    if(this->gConfig.exporterType != ExportType::Binary) {
        this->gHashNode[this->gCurrentFile]["extracted"][ExportTypeToString(this->gConfig.exporterType)] = true;
    }
}

void Companion::Process() {

    if(!fs::exists("config.yml")) {
        SPDLOG_ERROR("No config file found");
        return;
    }

    auto start = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    YAML::Node config = YAML::LoadFile("config.yml");

    bool isDirectoryMode = config["mode"] && config["mode"].as<std::string>() == "directory";

    if(!isDirectoryMode) {
        if(this->gRomPath.has_value()){
            std::ifstream input( this->gRomPath.value(), std::ios::binary );
            this->gRomData = std::vector<uint8_t>( std::istreambuf_iterator( input ), {} );
            input.close();
        }

        this->gCartridge = std::make_shared<N64::Cartridge>(this->gRomData);
        this->gCartridge->Initialize();

        if(!config[this->gCartridge->GetHash()]){
            SPDLOG_ERROR("No config found for {}", this->gCartridge->GetHash());
            return;
        }

        this->gConfig.parseMode = ParseMode::Default;
    } else {
        this->gConfig.parseMode = ParseMode::Directory;
    }

    auto rom = !isDirectoryMode ? config[this->gCartridge->GetHash()] : config;

    if(rom["preprocess"]) {
        auto preprocess = rom["preprocess"];
        for(auto job = preprocess.begin(); job != preprocess.end(); job++) {
            auto name = job->first.as<std::string>();
            auto item = job->second;
            auto method = GetSafeNode<std::string>(item, "method");
            if (method == "mio0-comptool") {
                auto type = GetSafeNode<std::string>(item, "type");
                auto target = GetSafeNode<std::string>(item, "target");
                auto restart = GetSafeNode<bool>(item, "restart");

                if (type == "decompress") {
                    this->gRomData = CompTool::Decompress(this->gRomData);
                    this->gCartridge = std::make_shared<N64::Cartridge>(this->gRomData);
                    this->gCartridge->Initialize();

                    auto hash = this->gCartridge->GetHash();

                    SPDLOG_INFO("ROM decompressed to {}", hash);

                    if (hash != target) {
                        throw std::runtime_error("Hash mismatch");
                    }

                    if(restart){
                        rom = config[this->gCartridge->GetHash()];
                    }
                } else {
                    throw std::runtime_error("Only decompression is supported");
                }
            } else {
                throw std::runtime_error("Invalid preprocess method");
            }
        }
    }

    auto cfg = rom["config"];
    if(!cfg) {
        SPDLOG_ERROR("No config found for {}", !isDirectoryMode ? this->gCartridge->GetHash() : GetSafeNode<std::string>(config, "folder"));
        return;
    }

    if (rom["metadata"]) {
        ProcessTables(rom);
    }

    if(rom["segments"]) {
        auto segments = rom["segments"].as<std::vector<uint32_t>>();
        for (int i = 0; i < segments.size(); i++) {
            this->gConfig.segment.global[i + 1] = segments[i];
        }
    }
    this->gAssetPath = rom["path"].as<std::string>();
    auto opath = cfg["output"];
    auto gbi = cfg["gbi"];
    auto gbi_floats = cfg["gbi_floats"];
    auto modding_path = opath && opath["modding"] ? opath["modding"].as<std::string>() : "modding";

    this->gConfig.moddingPath = modding_path;
    switch (this->gConfig.exporterType) {
        case ExportType::Binary: {
            std::string extension = "";
            switch (this->gConfig.otrMode) {
                case ArchiveType::OTR:
                    extension = ".otr";
                    break;
                case ArchiveType::O2R:
                    extension = ".o2r";
                    break;
                default:
                    throw std::runtime_error("Invalid archive type for export type Binary");
            }
            this->gConfig.outputPath = opath && opath["binary"] ? opath["binary"].as<std::string>() : ("generic" + extension);
            break;
        }
        case ExportType::Header: {
            this->gConfig.outputPath = opath && opath["headers"] ? opath["headers"].as<std::string>() : "headers";
            break;
        }
        case ExportType::Code: {
            this->gConfig.outputPath = opath && opath["code"] ? opath["code"].as<std::string>() : "code";
            break;
        }
        case ExportType::XML:
        case ExportType::Modding: {
            this->gConfig.outputPath = modding_path;
            break;
        }
    }

    if(gbi) {
        auto key = gbi.as<std::string>();

        if(key == "F3D") {
            this->gConfig.gbi.version = GBIVersion::f3d;
        } else if(key == "F3DEX") {
            this->gConfig.gbi.version = GBIVersion::f3dex;
        } else if(key == "F3DB") {
            this->gConfig.gbi.version = GBIVersion::f3db;
        } else if(key == "F3DEX2") {
            this->gConfig.gbi.version = GBIVersion::f3dex2;
        } else if(key == "F3DEXB") {
            this->gConfig.gbi.version = GBIVersion::f3dexb;
        } else if (key == "F3DEX_MK64") {
            this->gConfig.gbi.version = GBIVersion::f3dex;
            this->gConfig.gbi.subversion = GBIMinorVersion::Mk64;
        } else {
            SPDLOG_ERROR("Invalid GBI version");
            return;
        }

        if(gbi_floats) {
            this->gConfig.gbi.useFloats = gbi_floats.as<bool>();
        }
    }

    if(auto sort = cfg["sort"]) {
        if(sort.IsSequence()) {
            this->gWriteOrder = sort.as<std::vector<std::string>>();
        } else {
            this->gWriteOrder = sort.as<std::string>();
        }
    } else {
        this->gWriteOrder = std::vector<std::string> {
            "LIGHTS", "TEXTURE", "VTX", "GFX"
        };
    }

    if((this->gConfig.exporterType == ExportType::Code || this->gConfig.exporterType == ExportType::Binary) && this->gConfig.modding) {
        this->ParseModdingConfig();
    }

    if(std::holds_alternative<std::vector<std::string>>(this->gWriteOrder)) {
        for (auto& [key, _] : this->gFactories) {
            auto entries = std::get<std::vector<std::string>>(this->gWriteOrder);

            if(std::find(entries.begin(), entries.end(), key) != entries.end()) {
                continue;
            }

            entries.push_back(key);
        }
    }
#ifdef STANDALONE
    if(cfg["enums"]) {
        auto enums = GetSafeNode<std::vector<std::string>>(cfg, "enums");
        for (auto& file : enums) {
            this->ParseEnums(file);
        }
    }
#endif

    if(cfg["logging"]){
        auto level = cfg["logging"].as<std::string>();
        if(level == "TRACE") {
            spdlog::set_level(spdlog::level::trace);
        } else if(level == "DEBUG") {
            spdlog::set_level(spdlog::level::debug);
        } else if(level == "INFO") {
            spdlog::set_level(spdlog::level::info);
        } else if(level == "WARN") {
            spdlog::set_level(spdlog::level::warn);
        } else if(level == "ERROR") {
            spdlog::set_level(spdlog::level::err);
        } else if(level == "CRITICAL") {
            spdlog::set_level(spdlog::level::critical);
        } else if(level == "OFF") {
            spdlog::set_level(spdlog::level::off);
        } else {
            throw std::runtime_error("Invalid logging level, please use TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL or OFF");
        }
    }

    this->gConfig.textureDefines = cfg["textures"] && (cfg["textures"].as<std::string>() == "ADDITIONAL_DEFINES");

    this->ParseHash();

    SPDLOG_INFO("------------------------------------------------");
    spdlog::set_pattern(line);

    SPDLOG_INFO("Starting Torch...");

    if(this->gConfig.parseMode == ParseMode::Default) {
        SPDLOG_INFO("Game: {}", this->gCartridge->GetGameTitle());
        SPDLOG_INFO("CRC: {}", this->gCartridge->GetCRC());
        SPDLOG_INFO("Version: {}", this->gCartridge->GetVersion());
        SPDLOG_INFO("Country: [{}]", this->gCartridge->GetCountryCode());
        SPDLOG_INFO("Hash: {}", this->gCartridge->GetHash());
        SPDLOG_INFO("Assets: {}", this->gAssetPath);
    } else {
        SPDLOG_INFO("Mode: Directory");
        SPDLOG_INFO("Directory: {}", rom["folder"].as<std::string>());
    }

    AudioManager::Instance = new AudioManager();
    BinaryWrapper* wrapper = nullptr;

    if (this->gConfig.exporterType == ExportType::Binary) {
        switch (this->gConfig.otrMode) {
            case ArchiveType::OTR:
                wrapper = new SWrapper(this->gConfig.outputPath);
                break;
            case ArchiveType::O2R:
                wrapper = new ZWrapper(this->gConfig.outputPath);
                break;
            default:
                throw std::runtime_error("Invalid archive type for export type Binary");
        }
    }

    if (wrapper) {
        wrapper->CreateArchive();
    }
    this->gCurrentWrapper = wrapper;

    auto vWriter = LUS::BinaryWriter();
    vWriter.SetEndianness(Torch::Endianness::Big);
    vWriter.Write(static_cast<uint8_t>(Torch::Endianness::Big));

    if(this->gConfig.parseMode == ParseMode::Default) {
        vWriter.Write(this->gCartridge->GetCRC());
    } else {
        vWriter.Write((uint32_t) 0);
    }

    for (const auto & entry : Torch::getRecursiveEntries(this->gAssetPath)){
        if(entry.is_directory())  {
            continue;
        }

        const auto yamlPath = entry.path().generic_string();

        if(yamlPath.find(".yaml") == std::string::npos && yamlPath.find(".yml") == std::string::npos) {
            continue;
        }

        if(yamlPath.find("config.yml") != std::string::npos) {
            continue;
        }

        YAML::Node root = YAML::LoadFile(yamlPath);
        this->gCurrentDirectory = relative(entry.path(), this->gAssetPath).replace_extension("");
        this->gCurrentFile = yamlPath;

        if (!this->gProcessedFiles.contains(this->gCurrentFile)) {
            ProcessFile(root);
            this->gProcessedFiles.insert(this->gCurrentFile);
        }
    }

    if(wrapper != nullptr) {
        SPDLOG_INFO("Writing version file");
        wrapper->AddFile("version", vWriter.ToVector());
        vWriter.Close();
        wrapper->Close();
    }

    // Write entries hash
    std::ofstream file("torch.hash.yml", std::ios::binary);
    file << this->gHashNode;
    file.close();

    auto end = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    auto level = spdlog::get_level();
    spdlog::set_level(spdlog::level::info);
    SPDLOG_INFO("Done! Took {}ms", end.count() - start.count());
    spdlog::set_level(level);
    spdlog::set_pattern(regular);
    SPDLOG_INFO("------------------------------------------------");

    Decompressor::ClearCache();
    this->gCartridge = nullptr;
    Instance = nullptr;
}

void Companion::Pack(const std::string& folder, const std::string& output, const ArchiveType otrMode) {

    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

    SPDLOG_INFO("------------------------------------------------");

    SPDLOG_INFO("Starting Torch...");
    SPDLOG_INFO("Scanning {}", folder);

    auto start = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    std::unordered_map<std::string, std::vector<char>> files;

    for (const auto & entry : Torch::getRecursiveEntries(folder)){
        if(entry.is_directory())  {
            continue;
        }

        std::ifstream input( entry.path(), std::ios::binary );
        auto data = std::vector( std::istreambuf_iterator( input ), {} );
        input.close();
        files[entry.path().generic_string()] = data;
    }

    std::unique_ptr<BinaryWrapper> wrapper;
    switch (otrMode) {
        case ArchiveType::OTR:
            wrapper.reset(new SWrapper(output));
            break;
        case ArchiveType::O2R:
            wrapper.reset(new ZWrapper(output));
            break;
        default:
            throw std::runtime_error("Invalid archive type for export type Binary");
    }
    wrapper->CreateArchive();

    for(auto& [path, data] : files){
        std::string normalized = path;
        std::replace(normalized.begin(), normalized.end(), '\\', '/');
        // Remove parent folder
        normalized = normalized.substr(folder.length() + 1);
        wrapper->AddFile(normalized, data);
        SPDLOG_INFO("> Added {}", normalized);
    }

    auto end = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    SPDLOG_INFO("Done! Took {}ms", end.count() - start.count());
    SPDLOG_INFO("Exported to {}", output);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    SPDLOG_INFO("------------------------------------------------");

    wrapper->Close();
}

std::optional<std::tuple<std::string, YAML::Node>> Companion::RegisterAsset(const std::string& name, YAML::Node& node) {
    if(!node["offset"]) {
        return std::nullopt;
    }

    auto output = (this->gCurrentDirectory / name).string();
    std::replace(output.begin(), output.end(), '\\', '/');

    auto entry = std::make_tuple(output, node);
    this->gAddrMap[this->gCurrentFile][node["offset"].as<uint32_t>()] = entry;
    auto dResult = this->ParseNode(node, output);
    if(dResult.has_value()) {
        this->gParseResults[this->gCurrentFile].push_back(dResult.value());
    }
    spdlog::set_pattern(regular);
    SPDLOG_INFO("------------------------------------------------");
    spdlog::set_pattern(line);
    
    return entry;
}

void Companion::RegisterFactory(const std::string& type, const std::shared_ptr<BaseFactory>& factory) {
    this->gFactories[type] = factory;
    SPDLOG_DEBUG("Registered factory for {}", type);
}

std::optional<std::shared_ptr<BaseFactory>> Companion::GetFactory(const std::string &type) {
    if(!this->gFactories.contains(type)){
        return std::nullopt;
    }

    return this->gFactories[type];
}

std::optional<Table> Companion::SearchTable(uint32_t addr){
    for(auto& table : this->gTables){
        if(addr >= table.start && addr <= table.end){
            return table;
        }
    }

    return std::nullopt;
}

std::optional<std::string> Companion::GetEnumFromValue(const std::string& key, int32_t id) {
    if(!this->gEnums.contains(key)){
        return std::nullopt;
    }

    if(!this->gEnums[key].contains(id)){
        return std::nullopt;
    }

    return this->gEnums[key][id];
}

std::optional<std::uint32_t> Companion::GetFileOffsetFromSegmentedAddr(const uint8_t segment) const {

    auto segments = this->gConfig.segment;

    if(segments.temporal.contains(segment)) {
        return segments.temporal[segment];
    }

    if(segments.local.contains(segment)) {
        return segments.local[segment];
    }

    if(segments.global.contains(segment)) {
        return segments.global[segment];
    }

    return std::nullopt;
}

std::optional<std::tuple<std::string, YAML::Node>> Companion::GetNodeByAddr(const uint32_t addr){
    if(!this->gAddrMap.contains(this->gCurrentFile)){
        return std::nullopt;
    }

    if(!this->gAddrMap[this->gCurrentFile].contains(addr)){
        for (auto &file : this->gCurrentExternalFiles) {
            if (!this->gAddrMap.contains(file)) {
                SPDLOG_WARN("GetNodeByAddr: External File {} Not Found.", file);
                continue;
            }

            if (!this->gAddrMap[file].contains(addr)) {
                continue;
            }
            return this->gAddrMap[file][addr];
        }
        return std::nullopt;
    }

    return this->gAddrMap[this->gCurrentFile][addr];
}

std::optional<std::tuple<std::string, YAML::Node>> Companion::GetSafeNodeByAddr(const uint32_t addr, std::string type) {
    auto node = this->GetNodeByAddr(addr);

    if(!node.has_value()) {
        return std::nullopt;
    }

    auto [name, n] = node.value();
    auto n_type = GetSafeNode<std::string>(n, "type");

    if(n_type != type) {
        throw std::runtime_error("Requested node type does not match with the target node type at " + Torch::to_hex(addr, false));
    }

    return node;

}

std::string Companion::GetSymbolFromAddr(uint32_t address, bool validZero) {
    auto dec = Companion::Instance->GetNodeByAddr(address);
    std::ostringstream outSymbol;

    if(address == 0 && !validZero) {
        outSymbol << "NULL";
    } else if (dec.has_value()) {
        auto node = std::get<1>(dec.value());
        auto symbol = GetSafeNode<std::string>(node, "symbol");
        outSymbol << "&" << symbol;
    } else {
        outSymbol << "0x" << std::uppercase << std::hex << address;
    }

    return outSymbol.str();
}

std::optional<ParseResultData> Companion::GetParseDataByAddr(uint32_t addr) {
    if(!this->gParseResults.contains(this->gCurrentFile)){
        for (auto &file : this->gCurrentExternalFiles) {
            if (!this->gParseResults.contains(file)) {
                SPDLOG_INFO("GetParseDataByAddr: External File {} Not Found.", file);
                continue;
            }

            for (auto& result : this->gParseResults[file]){
                if (result.data.has_value() && result.GetOffset() == addr){
                    return result;
                }
            }
        }
        return std::nullopt;
    }

    for(auto& result : this->gParseResults[this->gCurrentFile]){
        if(result.data.has_value() && result.GetOffset() == addr){
            return result;
        }
    }

    return std::nullopt;
}

std::optional<ParseResultData> Companion::GetParseDataBySymbol(const std::string& symbol) {
    if(!this->gParseResults.contains(this->gCurrentFile)){
        return std::nullopt;
    }

    for(auto& result : this->gParseResults[this->gCurrentFile]){
        auto sym = GetNode<std::string>(result.node, "symbol");

        if(result.data.has_value() && sym.has_value() && sym.value() == symbol){
            return result;
        }
    }

    return std::nullopt;

}

std::optional<std::vector<std::tuple<std::string, YAML::Node>>> Companion::GetNodesByType(const std::string& type){
    std::vector<std::tuple<std::string, YAML::Node>> nodes;

    if(!this->gAddrMap.contains(this->gCurrentFile)){
        return nodes;
    }

    for(auto& [addr, tpl] : this->gAddrMap[this->gCurrentFile]){
        auto [name, node] = tpl;
        const auto n_type = GetSafeNode<std::string>(node, "type");
        if(node["autogen"]){
            SPDLOG_DEBUG("Skipping autogenerated asset {}", name);
            continue;
        }
        if(n_type == type){
            nodes.push_back(tpl);
        }
    }

    return nodes;

}

void Companion::RegisterCompanionFile(const std::string path, std::vector<char> data) {
    this->gCompanionFiles[path] = data;
    SPDLOG_TRACE("Registered companion file {}", path);
}

std::string Companion::NormalizeAsset(const std::string& name) const {
    auto path = fs::path(this->gCurrentFile).stem().string() + "_" + name;
    return path;
}

std::string Companion::RelativePath(const std::string& path) const {
    std::string doutput = (this->gCurrentDirectory / path).string();
    std::replace(doutput.begin(), doutput.end(), '\\', '/');
    return doutput;
}

std::string Companion::CalculateHash(const std::vector<uint8_t>& data) {
    return Chocobo1::SHA1().addData(data).finalize().toString();
}

static std::string ConvertType(std::string type) {
    int index = type.find(':');

    if(index != std::string::npos) {
        type = type.substr(index + 1);
    }
    std::transform(type.begin(), type.end(), type.begin(), tolower);
    return type;
}

std::optional<YAML::Node> Companion::AddAsset(YAML::Node asset) {
    if(!asset["offset"] || !asset["type"]) {
        return std::nullopt;
    }
    const auto type = GetSafeNode<std::string>(asset, "type");
    const auto offset = GetSafeNode<uint32_t>(asset, "offset");
    const auto symbol = GetSafeNode<std::string>(asset, "symbol", "");
    const auto decl = this->GetNodeByAddr(offset);

    if(decl.has_value()) {
        auto found = std::get<1>(decl.value());
        if(GetSafeNode<std::string>(found, "type") != type) {
            SPDLOG_ERROR("Asset clash detected {} vs {} at 0x{:X}", type, GetSafeNode<std::string>(found, "type"), offset);
        } else {
            return found;
        }
    }

    auto rom = this->GetRomData();
    auto factory = this->GetFactory(type);

    if(!factory.has_value()) {
        return std::nullopt;
    }

    std::string output;
    std::string typeId = ConvertType(type);

    if(!symbol.empty()) {
        output = symbol;
    } else if(Decompressor::IsSegmented(offset)){
        output = this->NormalizeAsset("seg" + std::to_string(SEGMENT_NUMBER(offset)) +"_" + typeId + "_" + Torch::to_hex(SEGMENT_OFFSET(offset), false));
    } else {
        output = this->NormalizeAsset(typeId + "_" + Torch::to_hex(offset, false));
    }

    std::replace(output.begin(), output.end(), ':', '_');

    asset["autogen"] = true;
    asset["symbol"] = output;

    auto result = this->RegisterAsset(output, asset);

    if(!gCurrentVirtualPath.empty()) {
        asset["path"] = gCurrentVirtualPath;
    }

    if(result.has_value()){
        asset["vpath"] = std::get<0>(result.value());

        return std::get<1>(result.value());
    }

    return std::nullopt;
}
