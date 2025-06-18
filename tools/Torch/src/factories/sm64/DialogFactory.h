#pragma once

#include <factories/BaseFactory.h>

namespace SM64 {
class DialogData : public IParsedData {
public:
    uint32_t mUnused;
    uint8_t  mLinesPerBox;
    uint16_t mLeftOffset;
    uint16_t mWidth;
    std::vector<uint8_t> mText;

    DialogData(uint32_t unused, uint8_t linesPerBox, uint16_t leftOffset, uint16_t width, std::vector<uint8_t>& text) : mUnused(unused), mLinesPerBox(linesPerBox), mLeftOffset(leftOffset), mWidth(width), mText(text) {}
};

class DialogBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class DialogFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return { REGISTER(Binary, DialogBinaryExporter) };
    }
};
}