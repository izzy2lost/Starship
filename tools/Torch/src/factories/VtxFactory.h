#pragma once

#include "BaseFactory.h"

struct VtxRaw {
    int16_t		ob[3];	/* x, y, z */
    uint16_t	flag;
    int16_t		tc[2];	/* texture coord */
    uint8_t	    cn[4];	/* color & alpha */
};

class VtxData : public IParsedData {
public:
    std::vector<VtxRaw> mVtxs;

    explicit VtxData(std::vector<VtxRaw> vtxs) : mVtxs(vtxs) {}
};

class VtxHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class VtxBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class VtxCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class VtxFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Code, VtxCodeExporter)
            REGISTER(Header, VtxHeaderExporter)
            REGISTER(Binary, VtxBinaryExporter)
        };
    }
    uint32_t GetAlignment() override {
        return 8;
    };
};