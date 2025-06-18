#include "DisplayListOverrides.h"

#include "utils/Decompressor.h"
#include "DisplayListFactory.h"
#include "spdlog/spdlog.h"
#include "Companion.h"
#include <string>

#ifdef STANDALONE
#include <gfxd.h>
#endif

namespace GFXDOverride {

std::unordered_map<uint32_t, std::tuple<std::string, YAML::Node>> mVtxOverlaps;

#ifdef STANDALONE
void Triangle2(const N64Gfx* gfx) {
    auto w0 = gfx->words.w0;
    auto w1 = gfx->words.w1;

    auto v1 = std::to_string( ((w0 >> 16) & 0xFF) / 2 );
    auto v2 = std::to_string( ((w0 >> 8) & 0xFF) / 2 );
    auto v3 = std::to_string( (w0 & 0xFF) / 2 );

    auto v4 = std::to_string( ((w1 >> 16) & 0xFF) / 2 );
    auto v5 = std::to_string( ((w1 >> 8) & 0xFF) / 2 );
    auto v6 = std::to_string( (w1 & 0xFF) / 2 );
    auto flag = "0";

    const auto str = v1 + ", " + v2 + ", " + v3 + ", " + flag + ", " + v4 + ", " + v5 + ", " + v6 + ", " + flag;

    gfxd_puts("gsSP2Triangles(");
    gfxd_puts(str.c_str());
    gfxd_puts(")");
}

void Quadrangle(const N64Gfx* gfx) {
    auto w1 = gfx->words.w1;

    auto v1 = std::to_string( ((w1 >> 16) & 0xFF) / 2 );
    auto v2 = std::to_string( ((w1 >> 8) & 0xFF) / 2 );
    auto v3 = std::to_string( (w1 & 0xFF) / 2 );
    auto v4 = std::to_string( ((w1 >> 24) & 0xFF) / 2 );
    auto flag = "0";

    const auto str = v1 + ", " + v2 + ", " + v3 + ", " + v4 + ", " + flag;

    gfxd_puts("gsSP1Quadrangle(");
    gfxd_puts(str.c_str());
    gfxd_puts(")");
}

int Vtx(uint32_t ptr, int32_t num) {
    auto vtx = GetVtxOverlap(ptr);

    if(vtx.has_value()){
        auto symbol = std::get<0>(vtx.value());
        auto node = std::get<1>(vtx.value());

        auto offset = GetSafeNode<uint32_t>(node, "offset");
        auto count = GetSafeNode<uint32_t>(node, "count");
        auto idx = (ptr - offset) / sizeof(N64Vtx_t);

        SPDLOG_INFO("Replaced Vtx Overlapped: 0x{:X} Symbol: {}", ptr, symbol);
        gfxd_puts("&");
        gfxd_puts(symbol.c_str());
        gfxd_puts("[");
        gfxd_puts(std::to_string(idx).c_str());
        gfxd_puts("]");
        return 1;
    }

    auto dec = Companion::Instance->GetSafeNodeByAddr(ptr, "VTX");

    if(dec.has_value()){
        auto node = std::get<1>(dec.value());
        auto symbol = GetSafeNode<std::string>(node, "symbol");
        SPDLOG_INFO("Found Vtx: 0x{:X} Symbol: {}", ptr, symbol);
        gfxd_puts(symbol.c_str());
        return 1;
    }

    SPDLOG_WARN("Could not find vtx at 0x{:X}", ptr);
    return 0;
}

int Texture(uint32_t ptr, int32_t fmt, int32_t siz, int32_t width, int32_t height, int32_t pal) {
    auto dec = Companion::Instance->GetSafeNodeByAddr(ptr, "TEXTURE");

    if(dec.has_value()){
        auto node = std::get<1>(dec.value());
        auto symbol = GetSafeNode<std::string>(node, "symbol");
        SPDLOG_INFO("Found Texture: 0x{:X} Symbol: {}", ptr, symbol);
        gfxd_puts(symbol.c_str());
        return 1;
    }

    SPDLOG_WARN("Could not find texture at 0x{:X}", ptr);
    return 0;
}

int Palette(uint32_t ptr, int32_t idx, int32_t count) {
    auto dec = Companion::Instance->GetSafeNodeByAddr(ptr, "TEXTURE");

    if(dec.has_value()){
        auto node = std::get<1>(dec.value());
        auto symbol = GetSafeNode<std::string>(node, "symbol");
        SPDLOG_INFO("Found TLUT: 0x{:X} Symbol: {}", ptr, symbol);
        gfxd_puts(symbol.c_str());
        return 1;
    }

    SPDLOG_WARN("Could not find tlut at 0x{:X}", ptr);
    return 0;
}

int Lights(uint32_t ptr, int32_t count) {
    auto dec = Companion::Instance->GetSafeNodeByAddr(ptr, "LIGHTS");

    if(dec.has_value()){
        auto node = std::get<1>(dec.value());
        auto symbol = GetSafeNode<std::string>(node, "symbol");
        SPDLOG_INFO("Found Lightsn: 0x{:X} Symbol: {}", ptr, symbol);
        gfxd_puts(symbol.c_str());
        return 1;
    }

    SPDLOG_WARN("Could not find lights at 0x{:X}", ptr);
    return 0;
}

int Light(uint32_t ptr) {
    auto res = Companion::Instance->GetSafeNodeByAddr(ptr, "LIGHTS");

    if(res.has_value()){
        auto node = std::get<1>(res.value());
        auto symbol = GetSafeNode<std::string>(node, "symbol");
        SPDLOG_INFO("Found Light A Ptr: 0x{:X} Symbol: {}", ptr, symbol);
        gfxd_puts(("&" + symbol + ".a").c_str());
        return 1;
    }

    res = Companion::Instance->GetSafeNodeByAddr(ptr - 0x8, "LIGHTS");

    if(res.has_value()){
        auto node = std::get<1>(res.value());
        auto symbol = GetSafeNode<std::string>(node, "symbol");
        SPDLOG_INFO("Found Light L Ptr: 0x{:X} Symbol: {}", ptr, symbol);
        gfxd_puts(("&" + symbol + ".l").c_str());
        return 1;
    }

    SPDLOG_WARN("Could not find light at 0x{:X}", ptr);
    return 0;
}

int DisplayList(uint32_t ptr) {
    auto dec = Companion::Instance->GetSafeNodeByAddr(ptr, "GFX");

    if(dec.has_value()){
        auto node = std::get<1>(dec.value());
        auto symbol = GetSafeNode<std::string>(node, "symbol");
        SPDLOG_INFO("Found Display List: 0x{:X} Symbol: {}", ptr, symbol);
        gfxd_puts(symbol.c_str());
        return 1;
    }

    SPDLOG_WARN("Could not find display list to override at 0x{:X}", ptr);
    return 0;
}

int Viewport(uint32_t ptr) {
    auto dec = Companion::Instance->GetSafeNodeByAddr(ptr, "VP");

    if(dec.has_value()){
        auto node = std::get<1>(dec.value());
        auto symbol = GetSafeNode<std::string>(node, "symbol");
        SPDLOG_INFO("Found Viewport: 0x{:X} Symbol: {}", ptr, symbol);
        gfxd_puts(("&" + symbol).c_str());
        return 1;
    }

    SPDLOG_TRACE("Could not find viewport to override at 0x{:X}", ptr);
    return 0;
}

int Matrix(uint32_t ptr) {
    auto dec = Companion::Instance->GetSafeNodeByAddr(ptr, "MTX");

    if(dec.has_value()){
        auto node = std::get<1>(dec.value());
        auto symbol = GetSafeNode<std::string>(node, "symbol");
        SPDLOG_INFO("Found Matrix: 0x{:X} Symbol: {}", ptr, symbol);
        gfxd_puts(("&" + symbol).c_str());
        return 1;
    }

    SPDLOG_TRACE("Could not find viewport to override at 0x{:X}", ptr);
    return 0;
}
#endif

std::optional<std::tuple<std::string, YAML::Node>> GetVtxOverlap(uint32_t ptr){
    if(mVtxOverlaps.contains(ptr)){
        SPDLOG_INFO("Found overlap for ptr 0x{:X}", ptr);
        return mVtxOverlaps[ptr];
    }

    SPDLOG_TRACE("Failed to find overlap for ptr 0x{:X}", ptr);

    return std::nullopt;
}

void RegisterVTXOverlap(uint32_t ptr, std::tuple<std::string, YAML::Node>& vtx){
    mVtxOverlaps[ptr] = vtx;
    SPDLOG_INFO("Register overlap for ptr 0x{:X}", ptr);
}

void ClearVtx(){
    mVtxOverlaps.clear();
}
}
