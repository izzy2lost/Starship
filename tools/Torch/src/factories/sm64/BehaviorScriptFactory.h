#pragma once

#include "factories/BaseFactory.h"
#include "behavior/BehaviorCommand.h"

typedef std::variant<uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, float, uint64_t> BehaviorArgument;

enum class BehaviorArgumentType {
    U8, S8, U16, S16, U32, S32, F32, PTR
};

namespace SM64 {

struct BehaviorCommand {
    BehaviorOpcode opcode;
    std::vector<BehaviorArgument> arguments;

    BehaviorCommand(BehaviorOpcode opcode, std::vector<BehaviorArgument> arguments) : opcode(opcode), arguments(arguments) {}
};

class BehaviorScriptData : public IParsedData {
public:
    std::vector<BehaviorCommand> mCommands;

    BehaviorScriptData(std::vector<BehaviorCommand> commands) : mCommands(std::move(commands)) {}
};

class BehaviorScriptHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class BehaviorScriptBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class BehaviorScriptCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class BehaviorScriptFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Code, BehaviorScriptCodeExporter)
            REGISTER(Header, BehaviorScriptHeaderExporter)
            REGISTER(Binary, BehaviorScriptBinaryExporter)
        };
    }
};
}
