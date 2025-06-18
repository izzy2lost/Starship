#include "IncludeFactory.h"
#include "spdlog/spdlog.h"

#include "Companion.h"
#include "utils/Decompressor.h"

#define NUM(x) std::dec << std::setfill(' ') << std::setw(6) << x
#define COL(c) std::dec << std::setfill(' ') << std::setw(3) << c

ExportResult IncludeHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    auto ctype = GetSafeNode<std::string>(node, "ctype");

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern " << ctype << " " << symbol << "[];\n";
    return std::nullopt;
}

ExportResult IncludeCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto file = GetSafeNode<std::string>(node, "file_path");
    const auto ctype = GetSafeNode<std::string>(node, "ctype");
        SPDLOG_INFO("writing INC");
    write << ctype << " " << symbol << "[] = {\n";

    write << fourSpaceTab << "#include \"" << file << "\"\n";

    write << "};\n\n";

    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> IncludeFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {

    SPDLOG_INFO("parsing INC");
    const uint32_t blank = 1;

    return std::make_shared<IncludeData>(blank);
}
