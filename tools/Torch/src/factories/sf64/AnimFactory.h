#pragma once

#include <factories/BaseFactory.h>

namespace SF64 {

struct JointKey {
    uint16_t keys[6];
};

class AnimData : public IParsedData {
public:
    int16_t mFrameCount;
    int16_t mLimbCount;
    uint32_t mDataOffset;
    uint32_t mKeyOffset;
    std::vector<uint16_t> mFrameData;
    std::vector<JointKey> mJointKeys;

    AnimData(int16_t frameCount, int16_t limbCount, uint32_t dataOffset, std::vector<uint16_t> frameData, uint32_t keyOffset, std::vector<JointKey> jointKeys);
};

class AnimHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class AnimBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class AnimCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class AnimFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Code, AnimCodeExporter)
            REGISTER(Header, AnimHeaderExporter)
            REGISTER(Binary, AnimBinaryExporter)
        };
    }
};
}
