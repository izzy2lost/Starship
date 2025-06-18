#pragma once

#include <factories/BaseFactory.h>
#include "types/Vec3D.h"

namespace SF64 {

class EnvironmentData : public IParsedData {
public:
    int32_t  mType;
    int32_t  mGround;
    uint16_t mBgColor;
    uint16_t mSeqId;
    int32_t  mFogR;
    int32_t  mFogG;
    int32_t  mFogB;
    int32_t  mFogN;
    int32_t  mFogF;
    Vec3f    mLightDir;
    int32_t  mLightR;
    int32_t  mLightG;
    int32_t  mLightB;
    int32_t  mAmbR;
    int32_t  mAmbG;
    int32_t  mAmbB;

    EnvironmentData(int32_t type, int32_t ground, uint16_t bgColor, uint16_t seqId, int32_t fogR, int32_t fogG, int32_t fogB, int32_t fogN, int32_t fogF, Vec3f lightDir, int32_t lightR, int32_t lightG, int32_t lightB, int32_t ambR, int32_t ambG, int32_t ambB);
};

class EnvironmentHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class EnvironmentBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class EnvironmentCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class EnvironmentXMLExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class EnvironmentFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(XML, EnvironmentXMLExporter)
            REGISTER(Code, EnvironmentCodeExporter)
            REGISTER(Header, EnvironmentHeaderExporter)
            REGISTER(Binary, EnvironmentBinaryExporter)
        };
    }
    bool SupportModdedAssets() override { return true; }
};
}
