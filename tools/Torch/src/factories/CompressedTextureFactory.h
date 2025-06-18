#pragma once

#include "BaseFactory.h"
#include "utils/Decompressor.h"
#include "utils/TextureUtils.h"

class CompressedTextureData : public IParsedData {
public:
    TextureFormat mFormat;
    uint32_t mWidth;
    uint32_t mHeight;
    std::vector<uint8_t> mBuffer;
    CompressionType mCompressionType;

    CompressedTextureData(TextureFormat format, uint32_t width, uint32_t height, std::vector<uint8_t>& buffer, CompressionType compressionType) : mFormat(format), mWidth(width), mHeight(height), mBuffer(std::move(buffer)), mCompressionType(compressionType) {}
};

class CompressedTextureHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class CompressedTextureCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class CompressedTextureBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class CompressedTextureModdingExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class CompressedTextureFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    std::optional<std::shared_ptr<IParsedData>> parse_modding(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Header, CompressedTextureHeaderExporter)
            REGISTER(Binary, CompressedTextureBinaryExporter)
            REGISTER(Code, CompressedTextureCodeExporter)
            REGISTER(Modding, CompressedTextureModdingExporter)
        };
    }
    bool SupportModdedAssets() override { return true; }
};
