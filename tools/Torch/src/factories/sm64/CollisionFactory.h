#pragma once

#include <factories/BaseFactory.h>
#include "collision/SpecialPresetNames.h"
#include "collision/SurfaceTerrains.h"

namespace SM64 {

struct CollisionVertex {
    int16_t x;
    int16_t y;
    int16_t z;
    CollisionVertex(int16_t x, int16_t y, int16_t z) : x(x), y(y), z(z) {};
};

struct CollisionTri {
    int16_t x;
    int16_t y;
    int16_t z;
    int16_t force;
    CollisionTri(int16_t x, int16_t y, int16_t z, int16_t force) : x(x), y(y), z(z), force(force) {};
};

struct CollisionSurface {
    SurfaceType surfaceType;
    std::vector<CollisionTri> tris;
    CollisionSurface(SurfaceType surfaceType, std::vector<CollisionTri> tris) : surfaceType(surfaceType), tris(std::move(tris)) {};
};

struct SpecialObject {
    SpecialPresets presetId;
    int16_t x;
    int16_t y;
    int16_t z;
    std::vector<int16_t> extraParams; // e.g. y-rot
    SpecialObject(SpecialPresets presetId, int16_t x, int16_t y, int16_t z, std::vector<int16_t> extraParams) : presetId(presetId), x(x), y(y), z(z), extraParams(std::move(extraParams)) {};
};

struct EnvRegionBox {
    int16_t id;
    int16_t x1;
    int16_t z1;
    int16_t x2;
    int16_t z2;
    int16_t height;
    EnvRegionBox(int16_t id, int16_t x1, int16_t z1, int16_t x2, int16_t z2, int16_t height) : id(id), x1(x1), z1(z1), x2(x2), z2(z2), height(height) {};
}; // e.g. water-box

class Collision : public IParsedData {
public:
    std::vector<CollisionVertex> mVertices;
    std::vector<CollisionSurface> mSurfaces;
    std::vector<SpecialObject> mSpecialObjects;
    std::vector<EnvRegionBox> mEnvRegionBoxes;

    explicit Collision(std::vector<CollisionVertex> vertices, std::vector<CollisionSurface> surfaces, std::vector<SpecialObject> specialObjects, std::vector<EnvRegionBox> envRegionBoxes) : mVertices(std::move(vertices)), mSurfaces(std::move(surfaces)), mSpecialObjects(std::move(specialObjects)), mEnvRegionBoxes(std::move(envRegionBoxes)) {}
};

class CollisionCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class CollisionHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class CollisionBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class CollisionFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Header, CollisionHeaderExporter)
            REGISTER(Binary, CollisionBinaryExporter)
            REGISTER(Code, CollisionCodeExporter)
        };
    }
};
}
