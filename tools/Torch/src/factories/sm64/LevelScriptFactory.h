#pragma once

#include "factories/BaseFactory.h"
#include "level/LevelCommand.h"

typedef std::variant<uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, float, uint64_t> LevelArgument;

enum class LevelArgumentType {
    U8, S8, U16, S16, U32, S32, F32, PTR
};

namespace SM64 {

struct LevelCommand {
    LevelOpcode opcode;
    std::vector<LevelArgument> arguments;

    LevelCommand(LevelOpcode opcode, std::vector<LevelArgument> arguments) : opcode(opcode), arguments(arguments) {}
};

class LevelScriptData : public IParsedData {
public:
    std::vector<LevelCommand> mCommands;

    LevelScriptData(std::vector<LevelCommand> commands) : mCommands(std::move(commands)) {}
};

class LevelScriptHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class LevelScriptBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class LevelScriptCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class LevelScriptFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Code, LevelScriptCodeExporter)
            REGISTER(Header, LevelScriptHeaderExporter)
            REGISTER(Binary, LevelScriptBinaryExporter)
        };
    }
};
}
