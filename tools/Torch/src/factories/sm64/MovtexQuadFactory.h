#pragma once

#include "factories/BaseFactory.h"
#include "types/Vec3D.h"

namespace SM64 {

class MovtexQuadData : public IParsedData {
public:
    std::vector<std::pair<int16_t, uint32_t>> mMovtexQuads;

    MovtexQuadData(std::vector<std::pair<int16_t, uint32_t>> movtexQuads) : mMovtexQuads(std::move(movtexQuads)) {}
};

class MovtexQuadHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class MovtexQuadBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class MovtexQuadCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class MovtexQuadFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Code, MovtexQuadCodeExporter)
            REGISTER(Header, MovtexQuadHeaderExporter)
            REGISTER(Binary, MovtexQuadBinaryExporter)
        };
    }
};
}
