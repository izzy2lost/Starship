#pragma once

#include "factories/BaseFactory.h"
#include "types/Vec3D.h"

namespace SF64 {

class TriangleData : public IParsedData {
public:
    std::vector<Vec3s> mTris;
    std::vector<YAML::Node> mMeshNodes;

    TriangleData(std::vector<Vec3s> tris, std::vector<YAML::Node> meshNodes);
};

class TriangleHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class TriangleBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class TriangleCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class TriangleFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Code, TriangleCodeExporter)
            REGISTER(Header, TriangleHeaderExporter)
            REGISTER(Binary, TriangleBinaryExporter)
        };
    }
};
}
