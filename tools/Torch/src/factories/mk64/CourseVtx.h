#pragma once

#include <factories/BaseFactory.h>

namespace MK64 {

    struct CourseVtx {
        int16_t		ob[3];	/* x, y, z */
        int16_t		tc[2];	/* texture coord */
        uint8_t	    cn[4];	/* color & alpha */
    };

    class CourseVtxData : public IParsedData {
    public:
        std::vector<CourseVtx> mVtxs;

        explicit CourseVtxData(std::vector<CourseVtx> vtxs) : mVtxs(vtxs) {}
    };

    class CourseVtxHeaderExporter : public BaseExporter {
        ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
    };

    class CourseVtxBinaryExporter : public BaseExporter {
        ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
    };

    class CourseVtxCodeExporter : public BaseExporter {
        ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
    };

    class CourseVtxFactory : public BaseFactory {
    public:
        std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
        std::optional<std::shared_ptr<IParsedData>> parse_modding(std::vector<uint8_t>& buffer, YAML::Node& data) override {
            return std::nullopt;
        }
        inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
            return {
                REGISTER(Code, CourseVtxCodeExporter)
                REGISTER(Header, CourseVtxHeaderExporter)
                REGISTER(Binary, CourseVtxBinaryExporter)
            };
        }
        bool SupportModdedAssets() override { return false; }
    };

}
