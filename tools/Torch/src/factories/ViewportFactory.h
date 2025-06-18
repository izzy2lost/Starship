#pragma once

#include "BaseFactory.h"

typedef struct {
	int16_t	vscale[4];  /* scale */
	int16_t	vtrans[4];  /* translate */
} VpRaw;

class VpData : public IParsedData {
public:
    VpRaw mViewport;

    VpData(VpRaw viewport) : mViewport(std::move(viewport)) {}
};

class ViewportHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class ViewportBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class ViewportCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class ViewportFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Code, ViewportCodeExporter)
            REGISTER(Header, ViewportHeaderExporter)
            REGISTER(Binary, ViewportBinaryExporter)
        };
    }
};
