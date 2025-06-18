#pragma once

#include "factories/BaseFactory.h"
#include "types/Vec3D.h"

namespace SM64 {

class MovtexData : public IParsedData {
public:
    std::vector<int16_t> mMovtexData;
    bool mIsQuad;
    uint32_t mVertexCount;
    bool mHasColor;
    MovtexData(std::vector<int16_t> movtexData, bool isQuad, uint32_t count, bool hasColor) : mMovtexData(std::move(movtexData)), mIsQuad(isQuad), mVertexCount(count), mHasColor(hasColor) {}
};

class MovtexHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class MovtexBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class MovtexCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class MovtexFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Code, MovtexCodeExporter)
            REGISTER(Header, MovtexHeaderExporter)
            REGISTER(Binary, MovtexBinaryExporter)
        };
    }
};
}
