#pragma once

#include "factories/BaseFactory.h"
#include "types/Vec3D.h"

namespace SM64 {

struct PaintingMapping {
    int16_t vtxId;
    int16_t texX;
    int16_t texY;
    PaintingMapping(int16_t vtxId, int16_t texX, int16_t texY) : vtxId(vtxId), texX(texX), texY(texY) {}
};

class PaintingData : public IParsedData {
public:
    std::vector<PaintingMapping> mPaintingMappings;
    std::vector<Vec3s> mPaintingGroups;

    PaintingData(std::vector<PaintingMapping> paintingMappings, std::vector<Vec3s> paintingGroups) : mPaintingMappings(std::move(paintingMappings)), mPaintingGroups(std::move(paintingGroups)) {}
};

class PaintingMapHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class PaintingMapBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class PaintingMapCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class PaintingMapFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Code, PaintingMapCodeExporter)
            REGISTER(Header, PaintingMapHeaderExporter)
            REGISTER(Binary, PaintingMapBinaryExporter)
        };
    }
};
}
