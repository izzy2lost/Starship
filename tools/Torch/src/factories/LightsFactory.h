#pragma once

#include "BaseFactory.h"

struct Light_tRaw {
  uint8_t	col[3];		/* diffuse light value (rgba) */
  int8_t 		pad1;
  uint8_t	colc[3];	/* copy of diffuse light value (rgba) */
  int8_t 		pad2;
  int8_t	dir[3];		/* direction of light (normalized) */
  int8_t 		pad3;
};

union LightRaw {
    Light_tRaw	l;
    long long int	force_structure_alignment[2];
};

struct Ambient_tRaw {
  uint8_t	col[3];		/* ambient light value (rgba) */
  int8_t 		pad1;
  uint8_t	colc[3];	/* copy of ambient light value (rgba) */
  int8_t 		pad2;
};


union AmbientRaw {
    Ambient_tRaw	l;
    long long int	force_structure_alignment[1];
};

struct Lights1Raw {
    AmbientRaw	a;
    LightRaw	l[1];
};

class LightsData : public IParsedData {
public:
    Lights1Raw mLights;

    LightsData(Lights1Raw lights) : mLights(lights) {}
};

class LightsHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class LightsBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class LightsCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class LightsFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Code, LightsCodeExporter)
            REGISTER(Header, LightsHeaderExporter)
            REGISTER(Binary, LightsBinaryExporter)
        };
    }
};