#pragma once

#include "BaseFactory.h"
#include "utils/TextureUtils.h"

class TextureData : public IParsedData {
public:
    TextureFormat mFormat;
    uint32_t mWidth;
    uint32_t mHeight;
    std::vector<uint8_t> mBuffer;

    TextureData(TextureFormat format, uint32_t width, uint32_t height, std::vector<uint8_t>& buffer) : mFormat(format), mWidth(width), mHeight(height), mBuffer(std::move(buffer)) {}
};

class TextureHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class TextureCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class TextureBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class TextureModdingExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class TextureFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    std::optional<std::shared_ptr<IParsedData>> parse_modding(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Header, TextureHeaderExporter)
            REGISTER(Binary, TextureBinaryExporter)
            REGISTER(Code, TextureCodeExporter)
            REGISTER(Modding, TextureModdingExporter)
        };
    }
    bool SupportModdedAssets() override { return true; }
};