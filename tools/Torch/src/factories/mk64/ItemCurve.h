#pragma once

#include <factories/BaseFactory.h>

namespace MK64 {

    class ItemCurveData : public IParsedData {
    public:
        std::vector<uint8_t> mItems;

        explicit ItemCurveData(std::vector<uint8_t> items) : mItems(items) {}
    };

    class ItemCurveHeaderExporter : public BaseExporter {
        ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
    };

    class ItemCurveBinaryExporter : public BaseExporter {
        ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
    };

    class ItemCurveCodeExporter : public BaseExporter {
        ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
    };

    class ItemCurveFactory : public BaseFactory {
    public:
        std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
        std::optional<std::shared_ptr<IParsedData>> parse_modding(std::vector<uint8_t>& buffer, YAML::Node& data) override {
            return std::nullopt;
        }
        inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
            return {
                REGISTER(Code, ItemCurveCodeExporter)
                REGISTER(Header, ItemCurveHeaderExporter)
                REGISTER(Binary, ItemCurveBinaryExporter)
            };
        }
        bool SupportModdedAssets() override { return false; }
    };

}