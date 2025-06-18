#include "CourseMetadata.h"

#include "Companion.h"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <filesystem>

#define NUM(x) std::dec << std::setfill(' ') << std::setw(6) << x
#define COL(c) "0x" << std::hex << std::setw(2) << std::setfill('0') << c

ExportResult MK64::CourseMetadataCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto metadata = std::static_pointer_cast<MetadataData>(raw)->mMetadata;

    if (metadata.empty()) {
        throw std::runtime_error("Course metadata null");
    }

    // Sort the data by id, 0 to 20 and beyond.
    std::sort(metadata.begin(), metadata.end(),
        [this](const CourseMetadata& a, const CourseMetadata& b) {
            return a.id < b.id;
    });

    std::ofstream file;
    auto outDir = GetSafeNode<std::string>(node, "out_directory") + "/";

    // Check if the output directory exists, and create it if it doesn't.
    if (!std::filesystem::exists(outDir)) {
        std::filesystem::create_directory(outDir);
    }
    
    file.open(outDir+"gCourseNames.inc.c", std::ios_base::binary | std::ios_base::out);
    if (file.is_open()) {
       // file << "char *gCourseNames[] = {\n" << fourSpaceTab;
        for (const auto& m : metadata) {
            if (m.name == "null") { continue; }
            // Remove debug line once proven that sort worked right (start at id 0 and go up)
            SPDLOG_INFO("Processing Course Id: "+std::to_string(m.id));
            file << '"' << m.name << "\", ";
        }
        file << "\n";
       // file << "\n};\n\n";
        file.close();
    } else if (file.fail()) {
        throw std::runtime_error("Course metadata output folder is likely bad or the file is in-use");
    }

    file.open(outDir+"gCourseDebugNames.inc.c", std::ios_base::binary | std::ios_base::out);
    if (file.is_open()) {
       // file << "char *gDebugCourseNames[] = {\n" << fourSpaceTab;
        for (const auto& m : metadata) {
            if (m.name == "null") { continue; }
            file << '"' << m.debugName << "\", ";
        }
        file << "\n";
        //file << "\n};\n\n";
        file.close();
    }

    file.open(outDir+"gCupSelectionByCourseId.inc.c", std::ios_base::binary | std::ios_base::out);
    if (file.is_open()) {
        //file << "char *gCupSelectionByCourseId[] = {\n" << fourSpaceTab;
        for (const auto& m : metadata) {
            if (m.cup == "null") { continue; }
            file << m.cup << ", ";
        }
        file << "\n";
       // file << "\n};\n\n";
        file.close();
    }

    file.open(outDir+"gPerCupIndexByCourseId.inc.c", std::ios_base::binary | std::ios_base::out);
    if (file.is_open()) {
        //file << "const u8 gPerCupIndexByCourseId[] = {\n" << fourSpaceTab;
        for (const auto& m : metadata) {
            if (m.cupIndex == -1) { continue; }
            file << m.cupIndex << ", ";
        }
        file << "\n";
        //file << "\n};\n\n";
        file.close();
    }

    file.open(outDir+"sCourseLengths.inc.c", std::ios_base::binary | std::ios_base::out);
    if (file.is_open()) {
        for (const auto& m : metadata) {
            if (m.courseLength == "null") { continue; }
            file << '"' << m.courseLength << "\", ";
        }
        file << "\n";
        file.close();
    }

    file.open(outDir+"gKartAIBehaviourLUT.inc.c", std::ios_base::binary | std::ios_base::out);
    if (file.is_open()) {
        for (const auto& m : metadata) {
            file << m.kartAIBehaviourLUT << ", ";
        }
        file << 0; // @WARNING TRAILING ZERO IN ARRAY
        file << "\n";
        file.close();
    }

    file.open(outDir+"gKartAICourseMaximumSeparation.inc.c", std::ios_base::binary | std::ios_base::out);
    if (file.is_open()) {
        // file << "f32 gWaypointWidth[] = {\n" << fourSpaceTab;
        for (const auto& m : metadata) {
            file << m.kartAIMaximumSeparation << ", ";
        }
        file << "\n";
        // file << "\n};\n\n";
        file.close();
    }

    file.open(outDir+"gKartAICourseMinimumSeparation.inc.c", std::ios_base::binary | std::ios_base::out);
    if (file.is_open()) {
        // file << "f32 gWaypointWidth2[] = {\n" << fourSpaceTab;
        for (const auto& m : metadata) {
            file << m.kartAIMinimumSeparation << ", ";
        }
        file << "\n";
        // file << "\n};\n\n";
        file.close();
    }

    file.open(outDir+"D_800DCBB4.inc.c", std::ios_base::binary | std::ios_base::out);
    if (file.is_open()) {
        //file << "uintptr_t *D_800DCBB4[] = {\n" << fourSpaceTab;
        for (const auto& m : metadata) {
            file << m.D_800DCBB4 << ", ";
        }
        file << "\n";
        //file << "\n};\n\n";
        file.close();
    }

    file.open(outDir+"gCPUSteeringSensitivity.inc.c", std::ios_base::binary | std::ios_base::out);

    // @WARNING THIS FILE HAS A TRAILING ZERO
    if (file.is_open()) {
        //file << "u16 gCPUSteeringSensitivity[] = {\n" << fourSpaceTab;
        for (const auto& m : metadata) {
            file << m.steeringSensitivity << ", ";
        }
        file << 0;
        file << "\n";
        //file << "\n};\n\n";
        file.close();
    }

    file.open(outDir+"gBombKartSpawns.inc.c", std::ios_base::binary | std::ios_base::out);
    if (file.is_open()) {
        //file << "u16 gCPUSteeringSensitivity[] = {\n" << fourSpaceTab;
        for (const auto& m : metadata) {
            file << "{ // " << m.name << "\n";
            for (const auto& bombKart : m.bombKartSpawns) {
                file << "{ ";
                file << bombKart.waypointIndex << ", ";
                file << bombKart.startingState << ", ";
                file << bombKart.unk_04 << ", ";
                file << bombKart.x << ", ";
                file << bombKart.z << ", ";
                file << bombKart.unk10 << ", ";
                file << bombKart.unk14;
                file << " },\n";
            }
            file << "},\n";
        }
        file.close();
    }

    file.open(outDir+"gCoursePathSizes.inc.c", std::ios_base::binary | std::ios_base::out);
    if (file.is_open()) {
        for (const auto& m : metadata) {
            file << "// " << m.name << "\n";
            file << "{ ";
            for (size_t i = 0; i < m.pathSizes.size(); i++) {
                if (i == 5) {
                    file << "{";
                }
                if (i == 7) {
                    file << m.pathSizes[i];
                    file << "}";
                } else {
                    file << m.pathSizes[i] << ", ";
                }
            }
            file << "},\n";
        }
        file.close();
    }

    file.open(outDir+"D_0D009418.inc.c", std::ios_base::binary | std::ios_base::out);
    if (file.is_open()) {
        for (const auto& m : metadata) {
            file << "// " << m.name << "\n";
            file << "{ ";
            for (const auto size : m.D_0D009418) {
                file << size << ", ";
            }
            file << "},\n";
        }
        file.close();
    }

    file.open(outDir+"D_0D009568.inc.c", std::ios_base::binary | std::ios_base::out);
    if (file.is_open()) {
        for (const auto& m : metadata) {
            file << "// " << m.name << "\n";
            file << "{ ";
            for (const auto& size : m.D_0D009568) {
                file << size << ", ";
            }
            file << "},\n";
        }
        file.close();
    }

    file.open(outDir+"D_0D0096B8.inc.c", std::ios_base::binary | std::ios_base::out);
    if (file.is_open()) {
        for (const auto& m : metadata) {
            file << "// " << m.name << "\n";
            file << "{ ";
            for (const auto& size : m.D_0D0096B8) {
                file << size << ", ";
            }
            file << "},\n";
        }
        file.close();
    }

    file.open(outDir+"D_0D009808.inc.c", std::ios_base::binary | std::ios_base::out);
    if (file.is_open()) {
        for (const auto& m : metadata) {
            file << "// " << m.name << "\n";
            file << "{ ";
            for (const auto& size : m.D_0D009808) {
                file << size << ", ";
            }
            file << "},\n";
        }
        file.close();
    }

    file.open(outDir+"gCoursePathTable.inc.c", std::ios_base::binary | std::ios_base::out);
    if (file.is_open()) {
        for (const auto& m : metadata) {
            file << "// " << m.name << "\n";
            file << "{ ";
            for (const auto& size : m.pathTable) {
                file << size << ", ";
            }
            file << "},\n";
        }
        file.close();
    }

    file.open(outDir+"gCoursePathTableUnknown.inc.c", std::ios_base::binary | std::ios_base::out);
    if (file.is_open()) {
        for (const auto& m : metadata) {
            file << "// " << m.name << "\n";
            file << "{ ";
            for (const auto& size : m.pathTableUnknown) {
                file << size << ", ";
            }
            file << "},\n";
        }
        file.close();
    }

    file.open(outDir+"sSkyColors.inc.c", std::ios_base::binary | std::ios_base::out);
    if (file.is_open()) {
        for (const auto& m : metadata) {
            file << "// " << m.name << "\n";
            file << "{ ";
            for (const auto& size : m.skyColors) {
                file << size << ", ";
            }
            file << "},\n";
        }
        file.close();
    }

    file.open(outDir+"sSkyColors2.inc.c", std::ios_base::binary | std::ios_base::out);
    if (file.is_open()) {
        for (const auto& m : metadata) {
            file << "// " << m.name << "\n";
            file << "{ ";
            for (const auto& size : m.skyColors2) {
                file << size << ", ";
            }
            file << "},\n";
        }
        file.close();
    }
    return std::nullopt;
}

ExportResult MK64::CourseMetadataBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    auto properties = std::static_pointer_cast<MetadataData>(raw);
    auto writer = LUS::BinaryWriter();

    WriteHeader(writer, Torch::ResourceType::CourseProperties, 0);
    writer.Write((uint32_t) properties->mMetadata.size());

    for (auto m : properties->mMetadata) {
        writer.Write(m.id);
        writer.Write(m.name);
        writer.Write(m.debugName);
        writer.Write(m.cup);
        writer.Write(m.cupIndex);
        writer.Write(m.courseLength);
        writer.Write(m.kartAIBehaviourLUT);
        writer.Write(m.kartAIMaximumSeparation);
        writer.Write(m.kartAIMinimumSeparation);
        writer.Write(m.D_800DCBB4);
        writer.Write(m.steeringSensitivity);

        for (auto b : m.bombKartSpawns) {
            writer.Write(b.waypointIndex);
            writer.Write(b.startingState);
            writer.Write(b.unk_04);
            writer.Write(b.x);
            writer.Write(b.z);
            writer.Write(b.unk10);
            writer.Write(b.unk14);
        }

        for (auto p : m.pathSizes) {
            writer.Write(p);
        }

        for (auto s : m.D_0D009418) {
            writer.Write(s);
        }

        for (auto s : m.D_0D009568) {
            writer.Write(s);
        }

        for (auto s : m.D_0D0096B8) {
            writer.Write(s);
        }

        for (auto s : m.D_0D009808) {
            writer.Write(s);
        }

        for (auto s : m.pathTable) {
            writer.Write(s);
        }

        for (auto s : m.pathTableUnknown) {
            writer.Write(s);
        }

        for (auto s : m.skyColors) {
            writer.Write(s);
        }

        for (auto s : m.skyColors2) {
            writer.Write(s);
        }
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> MK64::CourseMetadataFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto dir = GetSafeNode<std::string>(node, "input_directory");
 
    auto m = Companion::Instance->GetCourseMetadata();
    SPDLOG_INFO("RUNNING");
    std::vector<CourseMetadata> yamlData;
    for (const auto &yamls : m[dir]) {
        
        if (!yamls["course"]) {
            for (auto &node : yamls) {
                std::cout << node << std::endl;
            }
            throw std::runtime_error("Course yaml missing root label of course\nEx. course:");
        }

        auto metadata = yamls["course"];

        CourseMetadata data;

        data.id =                   GetSafeNode<uint32_t>(metadata, "id");
        data.name =                 GetSafeNode<std::string>(metadata, "name");
        data.debugName =            GetSafeNode<std::string>(metadata, "debug_name");
        data.cup =                  GetSafeNode<std::string>(metadata, "cup");
        data.cupIndex =             GetSafeNode<int32_t>(metadata, "cup_index");
        data.courseLength =         GetSafeNode<std::string>(metadata, "course_length");

        data.kartAIBehaviourLUT =        GetSafeNode<std::string>(metadata, "kart_ai_behaviour_ptr");
        data.kartAIMaximumSeparation =        GetSafeNode<std::string>(metadata, "kart_ai_maximum_separation");
        data.kartAIMinimumSeparation =       GetSafeNode<std::string>(metadata, "kart_ai_minimum_separation");

        data.D_800DCBB4 =           GetSafeNode<std::string>(metadata, "D_800DCBB4");
        data.steeringSensitivity =  GetSafeNode<uint32_t>(metadata, "cpu_steering_sensitivity");
        SPDLOG_INFO("BEFORE");
        for (const auto& bombKart : GetSafeNode<YAML::Node>(metadata, "bomb_kart_spawns")) {
            data.bombKartSpawns.push_back(BombKartSpawns({
                bombKart[0].as<uint16_t>(),
                bombKart[1].as<uint16_t>(),
                bombKart[2].as<std::string>(), // Parse as string because floating-point outputs incorrect values.
                bombKart[3].as<float>(),
                bombKart[4].as<float>(),
                bombKart[5].as<float>(),
                bombKart[6].as<float>(),
            }));
        }

        for (const auto& size : GetSafeNode<YAML::Node>(metadata, "path_sizes")) {
            data.pathSizes.push_back(size.as<uint16_t>());
        }

        for (const auto& value : GetSafeNode<YAML::Node>(metadata, "D_0D009418")) {
            data.D_0D009418.push_back(value.as<std::string>());
        }

        for (const auto& value : GetSafeNode<YAML::Node>(metadata, "D_0D009568")) {
            data.D_0D009568.push_back(value.as<std::string>());
        }

        for (const auto& value : GetSafeNode<YAML::Node>(metadata, "D_0D0096B8")) {
            data.D_0D0096B8.push_back(value.as<std::string>());
        }
        SPDLOG_INFO("MIDDLE");
        for (const auto& value : GetSafeNode<YAML::Node>(metadata, "D_0D009808")) {
            data.D_0D009808.push_back(value.as<std::string>());
        }

        for (const auto& str : GetSafeNode<YAML::Node>(metadata, "path_table")) {
            data.pathTable.push_back(str.as<std::string>());
        }

        for (const auto& str : GetSafeNode<YAML::Node>(metadata, "path_table_unknown")) {
            data.pathTableUnknown.push_back(str.as<std::string>());
        }
        SPDLOG_INFO("BEFORE COLOUR");
        for (const auto& value : GetSafeNode<YAML::Node>(metadata, "sky_colors")) {
            data.skyColors.push_back(value.as<uint16_t>());
        }
        SPDLOG_INFO("BEFORE COLOUR2");
        for (const auto& value : GetSafeNode<YAML::Node>(metadata, "sky_colors2")) {
            data.skyColors2.push_back(value.as<uint16_t>());
        }

        yamlData.push_back(CourseMetadata(
            {data}
        ));
    }
    SPDLOG_INFO("END RUNNING");

    return std::make_shared<MetadataData>(yamlData);
}
