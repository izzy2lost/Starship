#pragma once

#include "factories/BaseFactory.h"
#include "types/Vec3D.h"
#include "macro/MacroPresets.h"

namespace SM64 {

struct MacroObject {
    MacroPresets preset;
    int16_t yaw;
    int16_t posX;
    int16_t posY;
    int16_t posZ;
    int16_t behParam;
    MacroObject(MacroPresets preset, int16_t yaw, int16_t posX, int16_t posY, int16_t posZ, int16_t behParam) : preset(preset), yaw(yaw), posX(posX), posY(posY), posZ(posZ), behParam(behParam) {}
};

class MacroData : public IParsedData {
public:
    std::vector<MacroObject> mMacroData;

    MacroData(std::vector<MacroObject> macroData) : mMacroData(std::move(macroData)) {}
};

class MacroDataAlt : public IParsedData {
public:
    std::vector<int16_t> mMacroData;

    MacroDataAlt(std::vector<int16_t> macroData) : mMacroData(std::move(macroData)) {}
};

class MacroHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class MacroBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class MacroCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class MacroFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
//          REGISTER(Code, MacroCodeExporter)
            REGISTER(Header, MacroHeaderExporter)
            REGISTER(Binary, MacroBinaryExporter)
        };
    }
};
}
