#pragma once

#include <factories/BaseFactory.h>
#include "types/Vec3D.h"

namespace SF64 {

class LimbData : public IParsedData {
public:
    uint32_t mAddr;
    uint32_t mDList;
    Vec3f mTrans;
    Vec3s mRot;
    uint32_t mSibling;
    uint32_t mChild;
    int mIndex;

    LimbData(uint32_t addr, uint32_t dList, Vec3f trans, Vec3s rot, uint32_t sibling, uint32_t child, int index);
};

class SkeletonData : public IParsedData {
public:
    std::vector<LimbData> mSkeleton;

    explicit SkeletonData(std::vector<LimbData> skeleton) : mSkeleton(skeleton) {}
};

class SkeletonHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class SkeletonBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class SkeletonCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class SkeletonFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Code, SkeletonCodeExporter)
            REGISTER(Header, SkeletonHeaderExporter)
            REGISTER(Binary, SkeletonBinaryExporter)
        };
    }
};
}
