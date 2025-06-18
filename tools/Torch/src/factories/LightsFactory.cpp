#include "LightsFactory.h"

#include "utils/Decompressor.h"
#include "spdlog/spdlog.h"
#include "Companion.h"

ExportResult LightsHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    const auto offset = GetSafeNode<uint32_t>(node, "offset");
    const auto searchTable = Companion::Instance->SearchTable(offset);

    if(searchTable.has_value()){
        const auto [name, start, end, mode, index_size] = searchTable.value();

        if(start != offset){
            return std::nullopt;
        }

        write << "extern Lights1 " << name << "[];\n";
    } else {
        write << "extern Lights1 " << symbol << ";\n";
    }
    return std::nullopt;
}

ExportResult LightsCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto light = std::static_pointer_cast<LightsData>(raw)->mLights;
    auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");
    const auto searchTable = Companion::Instance->SearchTable(offset);

    if(searchTable.has_value()){
        const auto [name, start, end, mode, index_size] = searchTable.value();


        if(start == offset){
            write << "Lights1 " << name << "[] = {\n";
        }

        // Ambient
        auto r = (int16_t) light.a.l.col[0];
        auto g = (int16_t) light.a.l.col[1];
        auto b = (int16_t) light.a.l.col[2];

        // Diffuse
        auto r2 = (int16_t) light.l[0].l.col[0];
        auto g2 = (int16_t) light.l[0].l.col[1];
        auto b2 = (int16_t) light.l[0].l.col[2];

        // Direction
        auto x = (int16_t) light.l[0].l.dir[0];
        auto y = (int16_t) light.l[0].l.dir[1];
        auto z = (int16_t) light.l[0].l.dir[2];

        SPDLOG_INFO("Read light: {:X} {:X} {:X} {:X} {:X}", r, g, b, r2, g2);

        write << fourSpaceTab << "gdSPDefLights1(\n";

        write << fourSpaceTab << fourSpaceTab;
        write << r << ", " << g << ", " << b << ",\n";
        write << fourSpaceTab << fourSpaceTab;
        write << r2 << ", " << g2 << ", " << b2 << ", " << x << ", " << y << ", " << z << "\n";

        write << fourSpaceTab << "),\n";

        if (end == offset) {
            write << "};\n\n";
        }

    } else {
        write << "Lights1 " << symbol << " = gdSPDefLights1(\n";

        // Ambient
        auto r = (int16_t) light.a.l.col[0];
        auto g = (int16_t) light.a.l.col[1];
        auto b = (int16_t) light.a.l.col[2];

        // Diffuse
        auto r2 = (int16_t) light.l[0].l.col[0];
        auto g2 = (int16_t) light.l[0].l.col[1];
        auto b2 = (int16_t) light.l[0].l.col[2];

        // Direction
        auto x = (int16_t) light.l[0].l.dir[0];
        auto y = (int16_t) light.l[0].l.dir[1];
        auto z = (int16_t) light.l[0].l.dir[2];

        SPDLOG_INFO("Read light: {:X} {:X} {:X} {:X} {:X}", r, g, b, r2, g2);

        write << fourSpaceTab;
        write << r << ", " << g << ", " << b << ",\n";
        write << fourSpaceTab;
        write << r2 << ", " << g2 << ", " << b2 << ", " << x << ", " << y << ", " << z << "\n";

        write << ");\n\n";
    }
    return std::nullopt;
}

ExportResult LightsBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto light = std::static_pointer_cast<LightsData>(raw)->mLights;
    auto writer = LUS::BinaryWriter();
    WriteHeader(writer, Torch::ResourceType::Lights, 0);
    writer.Write(reinterpret_cast<char*>(&light), sizeof(Lights1Raw));
    writer.Finish(write);
    writer.Close();
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> LightsFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto decoded = Decompressor::AutoDecode(node, buffer);
    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    LUS::BinaryReader reader(segment.data, sizeof(Lights1Raw));
    Lights1Raw lights;
    reader.Read((char*) &lights, sizeof(Lights1Raw));
    return std::make_shared<LightsData>(lights);
}
