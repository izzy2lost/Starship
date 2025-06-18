#pragma once

#include <factories/BaseFactory.h>
#include "types/Vec3D.h"

namespace SF64 {

struct CollisionPoly {
    Vec3s tri;
    int16_t unk_06;
    Vec3s norm;
    int16_t unk_0E;
    int32_t dist;
};

class ColPolyData : public IParsedData {
public:
    std::vector<CollisionPoly> mPolys;
    std::vector<YAML::Node> mMeshNodes;

    ColPolyData(std::vector<CollisionPoly> polys, std::vector<YAML::Node> meshNodes);
};

class ColPolyHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class ColPolyBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class ColPolyCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class ColPolyFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Code, ColPolyCodeExporter)
            REGISTER(Header, ColPolyHeaderExporter)
            REGISTER(Binary, ColPolyBinaryExporter)
        };
    }
};
}
