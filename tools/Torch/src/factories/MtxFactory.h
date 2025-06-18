#pragma once

#include "BaseFactory.h"

union MtxS {
    struct {
        uint16_t intPart[4][4];
        uint16_t fracPart[4][4];
    };
    int32_t mint[4][4];
    long long int forc_structure_alignment;
};

struct MtxRaw {
    float mtx[16];
    MtxS mt;
};

class MtxData : public IParsedData {
public:
    std::vector<MtxRaw> mMtxs;

    explicit MtxData(std::vector<MtxRaw> mtxs) : mMtxs(mtxs) {}
};

class MtxHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class MtxBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class MtxCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class MtxFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    std::optional<std::shared_ptr<IParsedData>> parse_modding(std::vector<uint8_t>& buffer, YAML::Node& data) override {
        return std::nullopt;
    }
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Code, MtxCodeExporter)
            REGISTER(Header, MtxHeaderExporter)
            REGISTER(Binary, MtxBinaryExporter)
        };
    }
};