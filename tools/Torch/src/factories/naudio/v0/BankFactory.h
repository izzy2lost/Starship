#pragma once

#include <utility>

#include <factories/BaseFactory.h>
#include "AudioManager.h"

class BankData : public IParsedData {
public:
    Bank mBank;
    uint32_t mBankId;

    BankData(Bank bank, const uint32_t bankId) : mBank(std::move(bank)), mBankId(bankId) {}
};

class BankBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class BankFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Binary, BankBinaryExporter)
        };
    }
};