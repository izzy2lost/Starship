#pragma once

#include "BaseFactory.h"
#include "types/Vec3D.h"

class Vec3sData : public IParsedData {
public:
    std::vector<Vec3s> mVecs;
    int mMaxWidth;

    explicit Vec3sData(std::vector<Vec3s> vecs);
};

class Vec3sHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class Vec3sBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class Vec3sCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class Vec3sFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Code, Vec3sCodeExporter)
            REGISTER(Header, Vec3sHeaderExporter)
            REGISTER(Binary, Vec3sBinaryExporter)
        };
    }
};
