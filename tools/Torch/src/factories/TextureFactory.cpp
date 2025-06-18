#include "TextureFactory.h"
#include "utils/Decompressor.h"
#include "spdlog/spdlog.h"
#include "Companion.h"
#include <iomanip>
#include <regex>

extern "C" {
#include "n64graphics/n64graphics.h"
#include "BaseFactory.h"
}

static bool isTable = false;
static std::vector<std::string> tableEntries;

static const std::unordered_map <std::string, TextureFormat> sTextureFormats = {
    { "RGBA16", { TextureType::RGBA16bpp, 16 } },
    { "RGBA32", { TextureType::RGBA32bpp, 32 } },
    { "CI4",    { TextureType::Palette4bpp, 4 } },
    { "CI8",    { TextureType::Palette8bpp, 8 } },
    { "I4",     { TextureType::Grayscale4bpp, 4 } },
    { "I8",     { TextureType::Grayscale8bpp, 8 } },
    { "IA1",    { TextureType::GrayscaleAlpha1bpp, 1 } },
    { "IA4",    { TextureType::GrayscaleAlpha4bpp, 4 } },
    { "IA8",    { TextureType::GrayscaleAlpha8bpp, 8 } },
    { "IA16",   { TextureType::GrayscaleAlpha16bpp, 16 } },
    { "TLUT",   { TextureType::TLUT, 16 } },
};

ExportResult TextureHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto format = GetSafeNode<std::string>(node, "format");
    auto texture = std::static_pointer_cast<TextureData>(raw);
    auto data = texture->mBuffer;
    auto isOTR = Companion::Instance->IsOTRMode();
    size_t byteSize = std::max(1, (int) (texture->mFormat.depth / 8));

    const auto searchTable = Companion::Instance->SearchTable(offset);

    if(searchTable.has_value()){
        const auto [name, start, end, mode, index_size] = searchTable.value();
        unsigned int isize = index_size > -1 ? index_size : data.size() / byteSize;

        if(isOTR){
            write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";

            tableEntries.push_back(symbol);

            if(end == offset){
                write << "static const char* " << name << "[] = {\n";
                for(auto& entry : tableEntries){
                    write << tab_t << entry << ",\n";
                }
                write << "};\n\n";
                tableEntries.clear();
            }
        } else {
            write << "extern " << GetSafeNode<std::string>(node, "ctype", "u8") << " " << name << "[][" << isize << "];\n";
        }
    } else {
        if(isOTR){
            write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
            if (Companion::Instance->AddTextureDefines()) {
                write << "#define _" << symbol << "_WIDTH 0x" << std::hex << texture->mWidth << std::dec << "\n";
                write << "#define _" << symbol << "_HEIGHT 0x" << std::hex << texture->mHeight << std::dec << "\n";
            }
        } else {
            write << "extern " << GetSafeNode<std::string>(node, "ctype", "u8") << " " << symbol << "[];\n";
            if (Companion::Instance->AddTextureDefines()) {
                write << "#define _" << symbol << "_WIDTH 0x" << std::hex << texture->mWidth << std::dec << "\n";
                write << "#define _" << symbol << "_HEIGHT 0x" << std::hex << texture->mHeight << std::dec << "\n";
            }
        }
    }

    return std::nullopt;
}

ExportResult TextureCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    auto texture = std::static_pointer_cast<TextureData>(raw);
    auto data = texture->mBuffer;
    auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto symbol = GetSafeNode(node, "symbol", entryName);
    auto format = GetSafeNode<std::string>(node, "format");

    std::transform(format.begin(), format.end(), format.begin(), tolower);
    (*replacement) += "." + format;

    std::string dpath = Companion::Instance->GetOutputPath() + "/" + (*replacement);
    if(!exists(fs::path(dpath).parent_path())){
        create_directories(fs::path(dpath).parent_path());
    }

    std::ostringstream imgstream;

    size_t byteSize = std::max(1, (int) (texture->mFormat.depth / 8));
    size_t isize = texture->mBuffer.size() / byteSize;

    for (int i = 0; i < data.size(); i+=byteSize) {
        if (i % 16 == 0 && i != 0) {
            imgstream << std::endl;
        }

        imgstream << "0x";

        for (int j = 0; j < byteSize; j++) {
            imgstream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i + j]);
        }

        imgstream << ", ";
    }
    imgstream << std::endl;

    if (!Companion::Instance->IsUsingIndividualIncludes()){
        std::ofstream file(dpath + ".inc.c", std::ios::binary);
        file << imgstream.str();
        file.close();
    }

    const auto searchTable = Companion::Instance->SearchTable(offset);

    if(searchTable.has_value()){
        const auto [name, start, end, mode, index_size] = searchTable.value();

        if(mode != TableMode::Append){
            throw std::runtime_error("Reference mode is not supported for now");
        }

        if (index_size > -1) {
            isize = index_size;
        }

        if(start == offset){
            write << GetSafeNode<std::string>(node, "ctype", "u8") << " " << name << "[][" << isize << "] = {\n";
        }

        write << tab_t << "{\n";
        if (!Companion::Instance->IsUsingIndividualIncludes()){
            write << tab_t << tab_t << "#include \"" << Companion::Instance->GetOutputPath() + "/" << *replacement << ".inc.c\"\n";
        } else {
            write << imgstream.str();
        }
        write << tab_t << "},\n";

        if(end == offset){
            write << "};\n";
            if (Companion::Instance->IsDebug()) {
                write << "// size: 0x" << std::hex << std::uppercase << ASSET_PTR((end - start) + isize * byteSize) << "\n";
            }
        }
    } else {
        write << GetSafeNode<std::string>(node, "ctype", "u8") << " " << symbol  << "[] = {\n";

        if (!Companion::Instance->IsUsingIndividualIncludes()){
            write << tab_t << "#include \"" << Companion::Instance->GetOutputPath() + "/" << *replacement << ".inc.c\"\n";
        } else {
            write << imgstream.str();
        }
        write << "};\n";

        const auto sz = data.size();
        if (Companion::Instance->IsDebug()) {
            write << "// size: 0x" << std::hex << std::uppercase << sz;
        }

        write << "\n";
    }
    return offset + isize * byteSize;
}

ExportResult TextureBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    auto writer = LUS::BinaryWriter();
    auto texture = std::static_pointer_cast<TextureData>(raw);
    auto data = texture->mBuffer;

    WriteHeader(writer, Torch::ResourceType::Texture, 0);

    if(texture->mFormat.type == TextureType::TLUT) {
        texture->mFormat.type = TextureType::RGBA16bpp;
    }

    writer.Write((uint32_t) texture->mFormat.type);
    writer.Write(texture->mWidth);
    writer.Write(texture->mHeight);

    writer.Write((uint32_t) data.size());
    writer.Write((char*) data.data(), data.size());
    writer.Finish(write);
    return std::nullopt;
}

ExportResult TextureModdingExporter::Export(std::ostream&write, std::shared_ptr<IParsedData> data, std::string&entryName, YAML::Node&node, std::string* replacement) {
    auto texture = std::static_pointer_cast<TextureData>(data);
    auto format = texture->mFormat;
    uint8_t* raw = new uint8_t[TextureUtils::CalculateTextureSize(format.type, texture->mWidth, texture->mHeight) * 2];
    int size = 0;

    auto ext = GetSafeNode<std::string>(node, "format");

    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
    *replacement += "." + ext + ".png";

    switch (format.type) {
        case TextureType::TLUT:
        case TextureType::RGBA16bpp:
        case TextureType::RGBA32bpp: {
            rgba* imgr = raw2rgba(texture->mBuffer.data(), texture->mWidth, texture->mHeight, format.depth);
            if(rgba2png(&raw, &size, imgr, texture->mWidth, texture->mHeight)) {
                throw std::runtime_error("Failed to convert texture to PNG");
            }
            break;
        }
        case TextureType::GrayscaleAlpha16bpp:
        case TextureType::GrayscaleAlpha8bpp:
        case TextureType::GrayscaleAlpha4bpp:
        case TextureType::GrayscaleAlpha1bpp: {
            ia* imgia = raw2ia(texture->mBuffer.data(), texture->mWidth, texture->mHeight, format.depth);
            if(ia2png(&raw, &size, imgia, texture->mWidth, texture->mHeight)) {
                throw std::runtime_error("Failed to convert texture to PNG");
            }
            break;
        }
        case TextureType::Palette8bpp:
        case TextureType::Palette4bpp: {
            if (node["tlut_symbol"]) {
                auto tlut = GetSafeNode<std::string>(node,"tlut_symbol");
                auto palette = Companion::Instance->GetParseDataBySymbol(tlut);

                if (palette.has_value()) {
                    auto palTexture = std::static_pointer_cast<TextureData>(palette.value().data.value());
                    convert_raw_to_ci8(&raw, &size, texture->mBuffer.data(), (uint8_t *)palTexture->mBuffer.data(), 0, texture->mWidth, texture->mHeight, texture->mFormat.depth, palTexture->mFormat.depth);
                } else {
                    auto symbol = GetSafeNode<std::string>(node, "symbol");
                    throw std::runtime_error("Could not convert ci8 '"+symbol+"' the tlut symbol name is probably wrong for tlut_symbol node");
                }
                break;
            }

            if (node["tlut"]) {
                auto tlut = GetSafeNode<uint32_t>(node,"tlut");
                auto palette = Companion::Instance->GetParseDataByAddr(tlut);

                if (palette.has_value()) {
                    auto palTexture = std::static_pointer_cast<TextureData>(palette.value().data.value());
                    convert_raw_to_ci8(&raw, &size, texture->mBuffer.data(), (uint8_t *)palTexture->mBuffer.data(), 0, texture->mWidth, texture->mHeight, texture->mFormat.depth, palTexture->mFormat.depth);
                } else {
                    auto symbol = GetSafeNode<std::string>(node, "symbol");
                    throw std::runtime_error("Could not convert ci8 '"+symbol+"' the address is probably wrong for tlut address node");
                }
                break;
            }
        }
        case TextureType::Grayscale8bpp:
        case TextureType::Grayscale4bpp: {
            ia* imgi = raw2i(texture->mBuffer.data(), texture->mWidth, texture->mHeight, format.depth);
            if(ia2png(&raw, &size, imgi, texture->mWidth, texture->mHeight)) {
                throw std::runtime_error("Failed to convert texture to PNG");
            }
            break;
        }
        default: {
            SPDLOG_ERROR("Unsupported texture format for modding: {}", ext);
        }
    }

    write.write(reinterpret_cast<char*>(raw), size);
    return std::nullopt;
}


std::optional<std::shared_ptr<IParsedData>> TextureFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto format = GetSafeNode<std::string>(node, "format");
    auto symbol = GetSafeNode<std::string>(node, "symbol");
    uint32_t width;
    uint32_t height;
    uint32_t size;

    std::transform(format.begin(), format.end(), format.begin(), ::toupper);

    if (format.empty()) {
        SPDLOG_ERROR("Texture entry at {:X} in yaml missing format node\n\
                      Please add one of the following formats\n\
                      rgba16, rgba32, ia16, ia8, ia4, i8, i4, ci8, ci4, 1bpp, tlut", offset);
        return std::nullopt;
    }

    if(!sTextureFormats.contains(format)) {
        return std::nullopt;
    }

    TextureFormat fmt = sTextureFormats.at(format);

    if(fmt.type == TextureType::TLUT){
        width = GetSafeNode<uint32_t>(node, "colors");
        height = 1;
    } else {
        width = GetSafeNode<uint32_t>(node, "width");
        height = GetSafeNode<uint32_t>(node, "height");
    }

    if((format == "CI4" || format == "CI8") && node["tlut"] && node["colors"]) {
        YAML::Node tlutNode;
        const auto tlutOffset = GetSafeNode<uint32_t>(node, "tlut");
        const auto tlutSymbol = GetSafeNode(node, "tlut_symbol", symbol + "_tlut");
        std::ostringstream offsetSeg;
        offsetSeg << std::uppercase << std::hex << tlutOffset;
        tlutNode["symbol"] = std::regex_replace(tlutSymbol, std::regex(R"(OFFSET)"), offsetSeg.str());
        tlutNode["type"] = "TEXTURE";
        tlutNode["format"] = "TLUT";
        tlutNode["offset"] = tlutOffset;
        tlutNode["colors"] = GetSafeNode<uint32_t>(node, "colors");
        node["tlut"] = tlutOffset;
        if(node["tlut_ctype"]) {
            tlutNode["ctype"] = GetSafeNode<std::string>(node, "tlut_ctype");
        }
        Companion::Instance->AddAsset(tlutNode);
    }
    size = GetSafeNode<uint32_t>(node, "size", TextureUtils::CalculateTextureSize(sTextureFormats.at(format).type, width, height));
    auto [_, segment] = Decompressor::AutoDecode(node, buffer, size);
    std::vector<uint8_t> result;

    if(fmt.type == TextureType::GrayscaleAlpha1bpp){
        result = TextureUtils::alloc_ia8_text_from_i1(reinterpret_cast<uint16_t*>(segment.data), 8, 16);
    } else {
        result = std::vector(segment.data, segment.data + segment.size);
    }

    SPDLOG_INFO("Texture: {}", format);
    if(fmt.type == TextureType::TLUT){
        SPDLOG_INFO("Colors: {}", width);
    } else {
        SPDLOG_INFO("Width: {}", width);
        SPDLOG_INFO("Height: {}", height);
    }
    SPDLOG_INFO("Size: {}", size);
    SPDLOG_INFO("Offset: 0x{:X}", offset);

    if(result.size() == 0){
        return std::nullopt;
    }

        if(result.size() == 0){
        return std::nullopt;
    }

    return std::make_shared<TextureData>(fmt, width, height, result);
}

std::optional<std::shared_ptr<IParsedData>> TextureFactory::parse_modding(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto format = GetSafeNode<std::string>(node, "format");
    int width;
    int height;
    uint32_t size;
    auto offset = GetSafeNode<uint32_t>(node, "offset");

    if (format.empty()) {
        SPDLOG_ERROR("Texture entry at {:X} in yaml missing format node\n\
                      Please add one of the following formats\n\
                      rgba16, rgba32, ia16, ia8, ia4, i8, i4, ci8, ci4, 1bpp, tlut", offset);
        return std::nullopt;
    }

    if(!sTextureFormats.contains(format)) {
        return std::nullopt;
    }

    TextureFormat fmt = sTextureFormats.at(format);
    if(fmt.type == TextureType::TLUT){
        width = GetSafeNode<uint32_t>(node, "colors");
        height = 1;
    } else {
        width = GetSafeNode<uint32_t>(node, "width");
        height = GetSafeNode<uint32_t>(node, "height");
    }

    uint8_t* raw;
    switch (fmt.type) {
        case TextureType::TLUT:
        case TextureType::RGBA16bpp:
        case TextureType::RGBA32bpp: {
            const auto imgr = png2rgba(buffer.data(), buffer.size(), &width, &height);
            size = width * height * fmt.depth / 8;
            raw = new uint8_t[size];
            if(rgba2raw(raw, imgr, width, height, fmt.depth) <= 0){
                throw std::runtime_error("Failed to convert PNG to texture");
            }
            break;
        }
        case TextureType::GrayscaleAlpha16bpp:
        case TextureType::GrayscaleAlpha8bpp:
        case TextureType::GrayscaleAlpha4bpp:
        case TextureType::GrayscaleAlpha1bpp: {
            const auto imgia = png2ia(buffer.data(), buffer.size(), &width, &height);
            size = width * height * fmt.depth / 8;
            raw = new uint8_t[size];
            if(ia2raw(raw, imgia, width, height, fmt.depth) <= 0){
                throw std::runtime_error("Failed to convert PNG to texture");
            }
            break;
        }
        case TextureType::Palette8bpp:
        case TextureType::Palette4bpp: {
            // This implementation is not correct.
            // The process should be:
            // png2rgba --> imgpal2rawci

            // todo: Add wheel palette input
            // Implement so that it works.

            // auto tlut = GetSafeNode<std::string>(node,"tlut_symbol");
            // auto tlutTextureMap = Companion::Instance->GetTlutTextureMap();
            // auto palettePtr = tlutTextureMap[tlut];

            // if (palettePtr) {

            //     auto imgi = png2rgba(buffer.data(), buffer.size(), &width, &height);
            //     auto pal = png2rgba(palettePtr->mBuffer.data(), (palettePtr->mWidth * palettePtr->mWidth * palettePtr->mFormat.depth * 2), &width, &height);

            //     size = width * height * fmt.depth / 8;
            //     raw = new uint8_t[size];

            //     if(imgpal2rawci(raw, imgi, pal, 0, 0, width, height, fmt.depth) <= 0){
            //         throw std::runtime_error("Failed to convert PNG to texture");
            //     }
            // } else {

            // }
            SPDLOG_ERROR("Unsupported texture format for modding: {}", format);
            return std::nullopt;
        }
        case TextureType::Grayscale8bpp:
        case TextureType::Grayscale4bpp: {
            const auto imgi = png2ia(buffer.data(), buffer.size(), &width, &height);
            size = width * height * fmt.depth / 8;
            raw = new uint8_t[size];
            if(i2raw(raw, imgi, width, height, fmt.depth) <= 0){
                throw std::runtime_error("Failed to convert PNG to texture");
            }
            break;
        }
        default: {
            SPDLOG_ERROR("Unsupported texture format for modding: {}", format);
            return std::nullopt;
        }
    }

    auto result = std::vector(raw, raw + size);

    SPDLOG_INFO("Texture: {}", format);
    if(fmt.type == TextureType::TLUT){
        SPDLOG_INFO("Colors: {}", width);
    } else {
        SPDLOG_INFO("Width: {}", width);
        SPDLOG_INFO("Height: {}", height);
    }
    SPDLOG_INFO("Size: {}", size);
    SPDLOG_INFO("Offset: 0x{:X}", offset);

    if(result.size() == 0){
        return std::nullopt;
    }

    return std::make_shared<TextureData>(fmt, width, height, result);
}
