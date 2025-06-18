#include <iostream>
#include "CLI11.hpp"
#include "Companion.h"

Companion* Companion::Instance;

#ifdef STANDALONE

int main(int argc, char *argv[]) {
    CLI::App app{"Torch - [T]orch is [O]ur [R]esource [C]onversion [H]elper\n\
        * It extracts from a baserom and generates code or an otr.\n\
        * It can also generate an otr from a folder of assets.\n"
    };
    std::string mode;
    std::string filename;
    std::string target;
    std::string folder;
    std::string archive;
    ArchiveType otrMode = ArchiveType::None;
    bool otrModeSelected = false;
    bool xmlMode = false;
    bool debug = false;

    app.require_subcommand();

    /* Generate an OTR */
    const auto otr = app.add_subcommand("otr", "OTR - Generates an otr\n");

    otr->add_option("<baserom.z64>", filename, "")->required()->check(CLI::ExistingFile);
    otr->add_flag("-v,--verbose", debug, "Verbose Debug Mode");

    otr->parse_complete_callback([&] {
        const auto instance = Companion::Instance = new Companion(filename, ArchiveType::OTR, debug);
        instance->Init(ExportType::Binary);
    });

    /* Generate an O2R */
    const auto o2r = app.add_subcommand("o2r", "O2R - Generates an o2r\n");

    o2r->add_option("<baserom.z64>", filename, "")->required()->check(CLI::ExistingFile);
    o2r->add_flag("-v,--verbose", debug, "Verbose Debug Mode");

    o2r->parse_complete_callback([&] {
        const auto instance = Companion::Instance = new Companion(filename, ArchiveType::O2R, debug);
        instance->Init(ExportType::Binary);
    });

    /* Generate C code */
    const auto code = app.add_subcommand("code", "Code - Generates C code\n");

    code->add_option("<baserom.z64>", filename, "")->required()->check(CLI::ExistingFile);
    code->add_flag("-v,--verbose", debug, "Verbose Debug Mode; adds offsets to C code");

    code->parse_complete_callback([&]() {
        const auto instance = Companion::Instance = new Companion(filename, ArchiveType::None, debug);
        instance->Init(ExportType::Code);
    });

    /* Generate a binary */
    const auto binary = app.add_subcommand("binary", "Binary - Generates a binary\n");

    binary->add_option("<baserom.z64>", filename, "")->required()->check(CLI::ExistingFile);

    binary->parse_complete_callback([&] {
        const auto instance = Companion::Instance = new Companion(filename, ArchiveType::None, debug);
        instance->Init(ExportType::Binary);
    });

    /* Generate headers */
    const auto header = app.add_subcommand("header", "Header - Generates headers only\n");

    header->add_option("<baserom.z64>", filename, "")->required()->check(CLI::ExistingFile);
    header->add_flag("-o,--otr", otrModeSelected, "OTR/O2R Mode");


    header->parse_complete_callback([&] {
        if (otrModeSelected) {
            otrMode = ArchiveType::OTR;
        } else {
            otrMode = ArchiveType::None;
        }

        const auto instance = Companion::Instance = new Companion(filename, otrMode, debug);
        instance->Init(ExportType::Header);
    });

    /* Pack an archive from a folder */
    const auto pack = app.add_subcommand("pack", "Pack - Packs an archive from a folder\n");

    pack->add_option("<folder>", folder, "Generate OTR from a directory of assets")->required()->check(CLI::ExistingDirectory);
    pack->add_option("<target>", target, "Archive output destination")->required();
    pack->add_option("<archive-type>", archive, "Archive type: otr or o2r")->required();

    
    pack->parse_complete_callback([&] {
        if (archive == "otr") {
            otrMode = ArchiveType::OTR;
        } else if (archive == "o2r") {
            otrMode = ArchiveType::O2R;
        } else {
            std::cout << "Invalid archive type" << std::endl;
        }

        if (!folder.empty()) {
            Companion::Pack(folder, target, otrMode);
        } else {
            std::cout << "The folder is empty" << std::endl;
        }
    });

    /* Generate modding files */
    const auto modding_root = app.add_subcommand("modding", "Modding - Generates modding files like png\n");
    const auto modding_import = modding_root->add_subcommand("import", "Import - Import modified files to generate C code\n");
    const auto modding_export = modding_root->add_subcommand("export", "Export - Export modified files to a folder\n");

    modding_import->add_option("mode", mode, "code, otr, o2r or header")->required();
    modding_import->add_option("<baserom.z64>", filename, "")->required()->check(CLI::ExistingFile);
    modding_import->add_flag("-v,--verbose", debug, "Verbose Debug Mode");

    modding_import->parse_complete_callback([&] {
        ArchiveType otrMode;

        if (mode == "otr") {
            otrMode = ArchiveType::OTR;
        } else if (mode == "o2r") {
            otrMode = ArchiveType::O2R;
        } else {
            otrMode = ArchiveType::None;
        }

        const auto instance = Companion::Instance = new Companion(filename, otrMode, debug, true);
        if (mode == "code") {
            instance->Init(ExportType::Code);
        } else if (mode == "otr" || mode == "o2r") {
            instance->Init(ExportType::Binary);
        } else if (mode == "header") {
            instance->Init(ExportType::Header);
        } else {
            std::cout << "Invalid mode" << std::endl;
        }
    });

    modding_export->add_flag("-x,--xml", xmlMode, "XML Mode");
    modding_export->add_option("<baserom.z64>", filename, "")->required()->check(CLI::ExistingFile);

    modding_export->parse_complete_callback([&] {
        const auto instance = Companion::Instance = new Companion(filename, ArchiveType::None, debug);
        if (xmlMode) {
            instance->Init(ExportType::XML);
        } else {
            instance->Init(ExportType::Modding);
        }
    });

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        std::cout << app.help() << std::endl;
        return app.exit(e);
    }

    // No arguments --> display help.
    if (argc == 1) {
        std::cout << app.help() << std::endl;
    }

    return 0;
}
#endif