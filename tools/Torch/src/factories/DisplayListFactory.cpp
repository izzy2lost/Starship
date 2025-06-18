#include "DisplayListFactory.h"
#include "DisplayListOverrides.h"
#include "utils/Decompressor.h"
#include "spdlog/spdlog.h"
#include "Companion.h"
#include <fstream>
#include "n64/gbi-otr.h"

#ifdef STANDALONE
#include <gfxd.h>
#endif

#define C0(pos, width) ((w0 >> (pos)) & ((1U << width) - 1))
#define ALIGN16(val) (((val) + 0xF) & ~0xF)

std::unordered_map<std::string, uint8_t> gF3DTable = {
    { "G_VTX", 0x04 },
    { "G_DL", 0x06 },
    { "G_MTX", 0x1 },
    { "G_ENDDL", 0xB8 },
    { "G_SETTIMG", 0xFD },
    { "G_MOVEMEM", 0x03 },
    { "G_MV_L0", 0x86 },
    { "G_MV_L1", 0x88 },
    { "G_MV_LIGHT", 0xA },
    { "G_TRI2", 0xB1 },
    { "G_QUAD", -1 }
};

std::unordered_map<std::string, uint8_t> gF3DExTable = {
    { "G_VTX", 0x04 },
    { "G_DL", 0x06 },
    { "G_MTX", 0x1 },
    { "G_ENDDL", 0xB8 },
    { "G_SETTIMG", 0xFD },
    { "G_MOVEMEM", 0x03 },
    { "G_MV_L0", 0x86 },
    { "G_MV_L1", 0x88 },
    { "G_MV_LIGHT", 0xA },
    { "G_TRI2", 0xB1 },
    { "G_QUAD", 0xB5 }
};

std::unordered_map<std::string, uint8_t> gF3DEx2Table = {
    { "G_VTX", 0x01 },
    { "G_DL", 0xDE },
    { "G_MTX", 0xDA },
    { "G_ENDDL", 0xDF },
    { "G_SETTIMG", 0xFD },
    { "G_MOVEMEM", 0xDC },
    { "G_MV_L0", 0x86 },
    { "G_MV_L1", 0x88 },
    { "G_MV_LIGHT", 0xA },
    { "G_TRI2", 0x06 },
    { "G_QUAD", 0x07 }
};

std::unordered_map<GBIVersion, std::unordered_map<std::string, uint8_t>> gGBITable = {
    { GBIVersion::f3d, gF3DTable },
    { GBIVersion::f3dex, gF3DExTable },
    { GBIVersion::f3dex2, gF3DEx2Table },
};

#define GBI(cmd) gGBITable[Companion::Instance->GetGBIVersion()][#cmd]

#ifdef STANDALONE
void GFXDSetGBIVersion(){
    switch (Companion::Instance->GetGBIVersion()) {
        case GBIVersion::f3d:
            gfxd_target(gfxd_f3d);
            break;
        case GBIVersion::f3dex:
            gfxd_target(gfxd_f3dex);
            break;
        case GBIVersion::f3db:
            gfxd_target(gfxd_f3db);
            break;
        case GBIVersion::f3dex2:
            gfxd_target(gfxd_f3dex2);
            break;
        case GBIVersion::f3dexb:
            gfxd_target(gfxd_f3dexb);
            break;
    }
}
#endif

ExportResult DListHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern Gfx " << symbol << "[];\n";
    return std::nullopt;
}

#ifdef STANDALONE
bool hasTable = false;
ExportResult DListCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto cmds = std::static_pointer_cast<DListData>(raw)->mGfxs;
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    auto offset = GetSafeNode<uint32_t>(node, "offset");
    const auto searchTable = Companion::Instance->SearchTable(offset);
    const auto sz = (sizeof(uint32_t) * cmds.size());
    hasTable = searchTable.has_value();

    size_t isize = cmds.size();

    char out[0xFFFF] = {0};

    gfxd_input_buffer(cmds.data(), sizeof(uint32_t) * cmds.size());
    gfxd_output_buffer(out, sizeof(out));

    gfxd_endian(gfxd_endian_host, sizeof(uint32_t));
    gfxd_macro_fn([] {
        auto gfx = static_cast<const N64Gfx*>(gfxd_macro_data());
        const uint8_t opcode = (gfx->words.w0 >> 24) & 0xFF;

        if(hasTable) {
            gfxd_puts(fourSpaceTab fourSpaceTab);
        } else {
            gfxd_puts(fourSpaceTab);
        }

        // For mk64 only
        if(opcode == GBI(G_QUAD) && Companion::Instance->GetGBIMinorVersion() == GBIMinorVersion::Mk64) {
            GFXDOverride::Quadrangle(gfx);
        // Prevents mix and matching of quadrangle commands. Forces 2TRI only.
        } else if(opcode == GBI(G_TRI2)) {
            GFXDOverride::Triangle2(gfx);
        } else {
            gfxd_macro_dflt();
        }

        gfxd_puts(",\n");
        return 0;
    });

    gfxd_vtx_callback(GFXDOverride::Vtx);
    gfxd_timg_callback(GFXDOverride::Texture);
    gfxd_dl_callback(GFXDOverride::DisplayList);
    gfxd_tlut_callback(GFXDOverride::Palette);
    gfxd_lightsn_callback(GFXDOverride::Lights);
    gfxd_light_callback(GFXDOverride::Light);
    gfxd_vp_callback(GFXDOverride::Viewport);
    gfxd_mtx_callback(GFXDOverride::Matrix);
    GFXDSetGBIVersion();

    if(searchTable.has_value()){
        const auto [name, start, end, mode, index_size] = searchTable.value();

        if(mode != TableMode::Append){
            throw std::runtime_error("Reference mode is not supported for now");
        }

        if (index_size > -1) {
            isize = index_size;
        }

        if(start == offset){
            gfxd_puts(("Gfx " + name + "[][" + std::to_string(isize / 2) + "] = {\n").c_str());
            gfxd_puts("\t{\n");
        } else {
            gfxd_puts("\t{\n");
        }
        gfxd_execute();
        write << std::string(out);
        if(end == offset){
            write << fourSpaceTab << "}\n";
            write << "};\n";
            if (Companion::Instance->IsDebug()) {
                write << "// count: " << std::to_string(sz / 8) << " Gfx\n";
            }else {
                write << "\n";
            }
        } else {
            write << fourSpaceTab << "},\n";
        }
    } else {
        gfxd_puts(("Gfx " + symbol + "[] = {\n").c_str());
        gfxd_execute();
        write << std::string(out);
        write << "};\n";

        if (Companion::Instance->IsDebug()) {
            write << "// count: " << std::to_string(sz / 8) << " Gfx\n";
        } else {
            write << "\n";
        }
    }

    return offset + sz;
}

void DebugDisplayList(uint32_t w0, uint32_t w1){
    uint32_t dlist[] = {w0, w1};
    gfxd_input_buffer(dlist, sizeof(dlist));
    gfxd_output_fd(fileno(stdout));
    gfxd_endian(gfxd_endian_host, sizeof(uint32_t));
    gfxd_macro_fn([](){
        gfxd_puts("> ");
        gfxd_macro_dflt();
        gfxd_puts("\n");
        return 0;
    });
    gfxd_vtx_callback(GFXDOverride::Vtx);
    gfxd_timg_callback(GFXDOverride::Texture);
    gfxd_dl_callback(GFXDOverride::DisplayList);
    gfxd_tlut_callback(GFXDOverride::Palette);
    //gfxd_light_callback(GFXDOverride::Light);
    GFXDSetGBIVersion();
    gfxd_execute();
}
#endif

std::optional<std::tuple<std::string, YAML::Node>> SearchVtx(uint32_t ptr){
    auto decs = Companion::Instance->GetNodesByType("VTX");

    if(!decs.has_value()){
        return std::nullopt;
    }

    for(auto& dec : decs.value()){
        auto [name, node] = dec;

        auto offset = GetSafeNode<uint32_t>(node, "offset");
        auto count = GetSafeNode<uint32_t>(node, "count");
        auto end = ALIGN16((count * sizeof(N64Vtx_t)));

        if(ptr > offset && ptr <= offset + end){
            return std::make_tuple(GetSafeNode<std::string>(node, "symbol", name), node);
        }
    }

    return std::nullopt;
}

ExportResult DListBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto gbi = Companion::Instance->GetGBIVersion();
    auto cmds = std::static_pointer_cast<DListData>(raw)->mGfxs;
    auto writer = LUS::BinaryWriter();

    WriteHeader(writer, Torch::ResourceType::DisplayList, 0);

    writer.Write((int8_t) gbi);
    
    while (writer.GetBaseAddress() % 8 != 0)
        writer.Write(static_cast<int8_t>(0xFF));

    auto bhash = CRC64((*replacement).c_str());
    writer.Write(static_cast<uint32_t>((G_MARKER << 24)));
    writer.Write(0xBEEFBEEF);
    writer.Write(static_cast<uint32_t>(bhash >> 32));
    writer.Write(static_cast<uint32_t>(bhash & 0xFFFFFFFF));

    for(size_t i = 0; i < cmds.size(); i+=2){
        auto w0 = cmds[i];
        auto w1 = cmds[i + 1];
        uint8_t opcode = w0 >> 24;

        if(opcode == GBI(G_VTX)) {
            size_t nvtx;
            size_t didx;

            switch (gbi) {
                case GBIVersion::f3dex2:
                    nvtx = C0(12, 8);
                    didx = C0(1, 7) - C0(12, 8);
                    break;
                case GBIVersion::f3dex:
                case GBIVersion::f3dexb:
                    nvtx = C0(10, 6);
                    didx = C0(17, 7);
                    break;
                default:
                    nvtx = (C0(0, 16)) / sizeof(N64Vtx_t);
                    didx = C0(16, 4);
                    break;
            }

            auto ptr = w1;
            auto overlap = GFXDOverride::GetVtxOverlap(ptr);

            if(overlap.has_value()){
                auto ovnode = std::get<1>(overlap.value());
                auto path = Companion::Instance->RelativePath(std::get<0>(overlap.value()));
                uint64_t hash = CRC64(path.c_str());

                if(hash == 0) {
                    throw std::runtime_error("Vtx hash is 0 for " + std::get<0>(overlap.value()));
                }

                SPDLOG_INFO("Found vtx: 0x{:X} Hash: 0x{:X} Path: {}", ptr, hash, path);

                auto offset = GetSafeNode<uint32_t>(ovnode, "offset");
                auto count = GetSafeNode<uint32_t>(ovnode, "count");
                auto diff = ASSET_PTR(ptr) - ASSET_PTR(offset);

                N64Gfx value = gsSPVertexOTR(diff, nvtx, didx);

                SPDLOG_INFO("gsSPVertexOTR({}, {}, {})", diff, nvtx, didx);

                w0 = value.words.w0;
                w1 = value.words.w1;

                writer.Write(w0);
                writer.Write(w1);

                w0 = hash >> 32;
                w1 = hash & 0xFFFFFFFF;
            } else {
                SPDLOG_WARN("Could not find vtx at 0x{:X}", ptr);
            }

            auto dec = Companion::Instance->GetNodeByAddr(ptr);

            if(dec.has_value()){
                uint64_t hash = CRC64(std::get<0>(dec.value()).c_str());
                if(hash == 0) {
                    throw std::runtime_error("Vtx hash is 0 for " + std::get<0>(dec.value()));
                }

                SPDLOG_INFO("Found vtx: 0x{:X} Hash: 0x{:X} Path: {}", ptr, hash, std::get<0>(dec.value()));

                N64Gfx value = gsSPVertexOTR(0, nvtx, didx);

                SPDLOG_INFO("gsSPVertex({}, {}, 0x{:X})", nvtx, didx, ptr);

                w0 = value.words.w0;
                w1 = value.words.w1;

                writer.Write(w0);
                writer.Write(w1);

                w0 = hash >> 32;
                w1 = hash & 0xFFFFFFFF;
            } else {
                SPDLOG_WARN("Could not find vtx at 0x{:X}", ptr);
            }
        }

        if(opcode == GBI(G_DL)) {
            N64Gfx value;
            auto ptr = w1;
            auto dec = Companion::Instance->GetNodeByAddr(ptr);
            auto branch = (w0 >> 16) & G_DL_NO_PUSH;

            // Export displaylist segment addresses as an index into a buffer of gfx
            if ((Companion::Instance->GetGBIMinorVersion() == GBIMinorVersion::Mk64) && (SEGMENT_NUMBER(w1) == 0x07)) {
                value = gsSPDisplayListOTRIndex(w1);
                w0 = value.words.w0;
                w1 = value.words.w1;
            } else {
                value = gsSPDisplayListOTRHash(ptr);
                w0 = value.words.w0;
                w1 = value.words.w1;
            }

            writer.Write(w0);
            writer.Write(w1);

            if(dec.has_value()){
                uint64_t hash = CRC64(std::get<0>(dec.value()).c_str());
                SPDLOG_INFO("Found display list: 0x{:X} Hash: 0x{:X} Path: {}", ptr, hash, std::get<0>(dec.value()));
                w0 = hash >> 32;
                w1 = hash & 0xFFFFFFFF;
            } else {
                SPDLOG_WARN("Could not find display list at 0x{:X}", ptr);
            }

            if(branch){
                writer.Write(w0);
                writer.Write(w1);

                value = gsSPRawOpcode(GBI(G_ENDDL));
                w0 = value.words.w0;
                w1 = value.words.w1;
            }
        }

        // TODO: Fix this opcode
        if(opcode == GBI(G_MOVEMEM)) {
            auto ptr = w1;

            uint8_t index = 0;
            uint8_t offset = 0;
            bool hasOffset = false;

            switch (gbi) {
                case GBIVersion::f3d:
                    index = C0(16, 8);
                    offset = 0;
                    break;
                case GBIVersion::f3dex:
                    index = (w0 >> 16) & 0xFF;
                    offset = C0(8, 8) * 8;
                    break;
                default:
                    index = C0(0, 8);
                    offset = C0(8, 8) * 8;
                    break;
            }
                        
            auto res = Companion::Instance->GetNodeByAddr(ptr);

            if(!res.has_value()){
                res = Companion::Instance->GetNodeByAddr(ptr - 0x8);
                hasOffset = res.has_value();

                if(!hasOffset){
                    SPDLOG_INFO("Could not find light {:X}", ptr);
                    // throw std::runtime_error("Could not find light");                    
                }
            }

            w0 &= 0x00FFFFFF;
            w0 += G_MOVEMEM_OTR_HASH << 24;
            w1 = _SHIFTL(index, 24, 8) | _SHIFTL(offset, 16, 8) | _SHIFTL((uint8_t)(hasOffset ? 1 : 0), 8, 8);

            writer.Write(w0);
            writer.Write(w1);

            if(res.has_value()){
                uint64_t hash = CRC64(std::get<0>(res.value()).c_str());
                SPDLOG_INFO("Found movemem: 0x{:X} Hash: 0x{:X} Path: {}", ptr, hash, std::get<0>(res.value()));
                w0 = hash >> 32;
                w1 = hash & 0xFFFFFFFF;
            } else {
                SPDLOG_WARN("Could not find light at 0x{:X}", ptr);
            }
        }

        if(opcode == GBI(G_SETTIMG)) {
            auto ptr = w1;
            auto dec = Companion::Instance->GetNodeByAddr(ptr);

            // Export texture segment addresses as segmented addresses
            if ((Companion::Instance->GetGBIMinorVersion() == GBIMinorVersion::Mk64) && ((SEGMENT_NUMBER(w1) == 0x03) || (SEGMENT_NUMBER(w1) == 0x05))) {
                w1 |= 1;
                writer.Write(w0);
                writer.Write(w1);
            } else {
                N64Gfx value = gsDPSetTextureOTRImage(C0(21, 3), C0(19, 2), C0(0, 10), ptr);
                w0 = value.words.w0;
                w1 = value.words.w1;

                writer.Write(w0);
                writer.Write(w1);

                if(dec.has_value()){
                    uint64_t hash = CRC64(std::get<0>(dec.value()).c_str());

                    if(hash == 0){
                        throw std::runtime_error("Texture hash is 0 for " + std::get<0>(dec.value()));
                    }

                    SPDLOG_INFO("Found texture: 0x{:X} Hash: 0x{:X} Path: {}", ptr, hash, std::get<0>(dec.value()));
                    w0 = hash >> 32;
                    w1 = hash & 0xFFFFFFFF;
                } else {
                    SPDLOG_WARN("Could not find texture at 0x{:X}", ptr);
                }
            }
        }

        if(opcode == GBI(G_MTX)) {
            auto ptr = w1;
            auto dec = Companion::Instance->GetNodeByAddr(ptr);

            w0 &= 0x00FFFFFF;
            w0 += G_MTX_OTR << 24;
            w1 = 0;

            writer.Write(w0);
            writer.Write(w1);

            if(dec.has_value()){
                uint64_t hash = CRC64(std::get<0>(dec.value()).c_str());

                if(hash == 0){
                    throw std::runtime_error("Matrix hash is 0 for " + std::get<0>(dec.value()));
                }

                SPDLOG_INFO("Found matrix: 0x{:X} Hash: 0x{:X} Path: {}", ptr, hash, std::get<0>(dec.value()));
                w0 = hash >> 32;
                w1 = hash & 0xFFFFFFFF;
            } else {
                SPDLOG_WARN("Could not find matrix at 0x{:X}", ptr);
            }
        }

        writer.Write(w0);
        writer.Write(w1);
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> DListFactory::parse(std::vector<uint8_t>& raw_buffer, YAML::Node& node) {
    const auto gbi = Companion::Instance->GetGBIVersion();

    auto count = GetSafeNode<int32_t>(node, "count", -1);
    auto [_, segment] = Decompressor::AutoDecode(node, raw_buffer);
    LUS::BinaryReader reader(segment.data, segment.size);
    reader.SetEndianness(Torch::Endianness::Big);

    std::vector<uint32_t> gfxs;
    auto processing = true;
    size_t length = 0;

    while (processing){
        auto w0 = reader.ReadUInt32();
        auto w1 = reader.ReadUInt32();

        uint8_t opcode = w0 >> 24;

        if(opcode == GBI(G_ENDDL)) {
            processing = false;
        }

        if(opcode == GBI(G_DL)) {
            if (SEGMENT_NUMBER(node["offset"].as<uint32_t>()) == SEGMENT_NUMBER(w1)) {

                std::optional<uint32_t> segment;

                if ((w0 >> 16) & G_DL_NO_PUSH) {
                    SPDLOG_INFO("Branch List Command Found");
                    processing = false;
                }

                YAML::Node gfx;
                gfx["type"] = "GFX";
                gfx["offset"] = w1;

                Companion::Instance->AddAsset(gfx);
            }
        }

        // This opcode is generally used as part of multiple macros such as gsSPSetLights1.
        // We need to process gsSPLight which is a subcommand inside G_MOVEMEM (0x03).
        if(opcode == GBI(G_MOVEMEM)) {
            // 0x03860000 or 0x03880000 subcommand will contain 0x86/0x88 for G_MV_L0 and G_MV_L1. Other subcommands also exist.
            uint8_t subcommand = (w0 >> 16) & 0xFF;
            uint8_t index = 0;
            uint8_t offset = 0;
            bool light = false;

            switch (Companion::Instance->GetGBIVersion()) {
               // If needing light generation on G_MV_L0 then we'll need to walk the DL ptr forward/backward to check for 0xBC
               // Otherwise mk64 will break.
               // PD: Mega, this works for sm64 too, why you didn't implement it? >:(
               // PD: Im jk, <3
               case GBIVersion::f3d:
               case GBIVersion::f3dex:
                    /*
                     * Only generate lights on the second gsSPLight.
                     * gsSPSetLights1(name) outputs three macros:
                     *
                     * gsSPNumLights(NUMLIGHTS_1)
                     * gsSPLight(&name.l[0], G_MV_L0)
                     * gsSPLight(&name.a, G_MV_L1) <-- This ptr is used to generate the lights
                    */
                    if (subcommand == GBI(G_MV_L1)) {
                        light = true;
                    }
                case GBIVersion::f3dex2:
                    index = C0(0, 8);
                    offset = C0(8, 8) * 8;

                    // same thing as above; see macro gSPLight at gbi.h
                    if(index == GBI(G_MV_LIGHT) && offset == (2 * 24 + 24)) {
                        light = true;
                    }
                    break;
                default:
                    throw std::runtime_error("Unsupported GBI version");
            }

            if(light){
                YAML::Node lnode;
                lnode["type"] = "LIGHTS";
                lnode["offset"] = w1;
                Companion::Instance->AddAsset(lnode);
            }
        }

        if(opcode == GBI(G_VTX)) {
            uint32_t nvtx;

            switch (gbi) {
                case GBIVersion::f3dex2:
                    nvtx = C0(12, 8);
                break;
                case GBIVersion::f3dex:
                case GBIVersion::f3dexb:
                    nvtx = C0(10, 6);
                break;
                default:
                    nvtx = (C0(0, 16)) / sizeof(N64Vtx_t);
                break;
            }
            const auto decl = Companion::Instance->GetNodeByAddr(w1);

            if(!decl.has_value()){
                auto search = SearchVtx(w1);

                if(search.has_value()){
                    auto [path, vtx] = search.value();

                    SPDLOG_INFO("Path: {}", path);

                    auto lOffset = GetSafeNode<uint32_t>(vtx, "offset");
                    auto lCount = GetSafeNode<uint32_t>(vtx, "count");
                    auto lSize = ALIGN16(lCount * sizeof(N64Vtx_t));

                    if(w1 > lOffset && w1 <= lOffset + lSize){
                        SPDLOG_INFO("Found vtx at 0x{:X} matching last vtx at 0x{:X}", w1, lOffset);
                        GFXDOverride::RegisterVTXOverlap(w1, search.value());
                    }
                } else {
                    YAML::Node vtx;
                    vtx["type"] = "VTX";
                    vtx["offset"] = w1;
                    vtx["count"] = nvtx;
                    Companion::Instance->AddAsset(vtx);
                }
            } else {
                SPDLOG_WARN("Found vtx at 0x{:X}", w1);
            }
        }

        if(count != -1 && length++ >= count){
            break;
        }

        gfxs.push_back(w0);
        gfxs.push_back(w1);
    }



    return std::make_shared<DListData>(gfxs);
}
