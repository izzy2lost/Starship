#pragma once

#include <factories/BaseFactory.h>

namespace SM64 {
class AnimationData : public IParsedData {
public:
    int16_t mFlags;
    int16_t mAnimYTransDivisor;
    int16_t mStartFrame;
    int16_t mLoopStart;
    int16_t mLoopEnd;
    int16_t mUnusedBoneCount;
    int16_t mLength;
    std::vector<uint16_t> mIndices;
    std::vector<int16_t> mEntries;

    AnimationData(int16_t flags, int16_t animYTransDivisor, int16_t startFrame, int16_t loopStart, int16_t loopEnd, int16_t unusedBoneCount, int16_t length, std::vector<uint16_t> indices, std::vector<int16_t> entries) : mFlags(flags), mAnimYTransDivisor(animYTransDivisor), mStartFrame(startFrame), mLoopStart(loopStart), mLoopEnd(loopEnd), mUnusedBoneCount(unusedBoneCount), mLength(length), mIndices(std::move(indices)), mEntries(std::move(entries)) {}
};

class AnimationBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class AnimationFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return { REGISTER(Binary, AnimationBinaryExporter) };
    }
};
}