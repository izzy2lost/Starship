#include "BankFactory.h"
#include "Companion.h"

ExportResult BankBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto bank = std::static_pointer_cast<BankData>(raw);

    WriteHeader(writer, Torch::ResourceType::Bank, 0);
    writer.Write(bank->mBankId);
    writer.Write(static_cast<uint32_t>(bank->mBank.insts.size()));
    for (auto& instrument : bank->mBank.insts) {
        writer.Write(static_cast<uint8_t>(instrument.valid));
        if(!instrument.valid) continue;

        writer.Write(instrument.releaseRate);
        writer.Write(instrument.normalRangeLo);
        writer.Write(instrument.normalRangeHi);

        auto [name, entries] = bank->mBank.envelopes[instrument.envelope];
        if(!entries.empty()){
            writer.Write(static_cast<uint32_t>(entries.size()));
            for (auto& [delay, arg] : entries) {
                writer.Write(static_cast<uint16_t>(delay));
                writer.Write(static_cast<uint16_t>(arg));
            }
        } else {
            writer.Write(static_cast<uint32_t>(0));
        }

        uint32_t soundFlags = 0;
        const bool hasLo = instrument.soundLo.has_value();
        const bool hasMed = instrument.soundMed.has_value();
        const bool hasHi = instrument.soundHi.has_value();
        if(hasLo){
            soundFlags |= 1 << 0;
        }
        if(hasMed){
            soundFlags |= 1 << 1;
        }
        if(hasHi){
            soundFlags |= 1 << 2;
        }

        writer.Write(soundFlags);
        if(hasLo){
            auto [offset, tuning] = instrument.soundLo.value();
            const uint32_t idx = AudioManager::Instance->get_index(bank->mBank.samples[offset]);
            writer.Write(AudioManager::Instance->get_sample(idx));
            writer.Write(tuning);
        }
        if(hasMed){
            auto [offset, tuning] = instrument.soundMed.value();
            const uint32_t idx = AudioManager::Instance->get_index(bank->mBank.samples[offset]);
            writer.Write(AudioManager::Instance->get_sample(idx));
            writer.Write(tuning);
        }
        if(hasHi){
            auto [offset, tuning] = instrument.soundHi.value();
            const uint32_t idx = AudioManager::Instance->get_index(bank->mBank.samples[offset]);
            writer.Write(AudioManager::Instance->get_sample(idx));
            writer.Write(tuning);
        }
    }

    writer.Write(static_cast<uint32_t>(bank->mBank.drums.size()));
    for(auto &drum : bank->mBank.drums){
        writer.Write(drum.releaseRate);
        writer.Write(drum.pan);
        auto envelope = bank->mBank.envelopes[drum.envelope];
        if(!envelope.entries.empty()){
            writer.Write(static_cast<uint32_t>(envelope.entries.size()));
            for(auto &entry : envelope.entries){
                writer.Write(entry.delay);
                writer.Write(entry.arg);
            }
        } else {
            writer.Write(static_cast<uint32_t>(0));
        }

        const uint32_t idx = AudioManager::Instance->get_index(bank->mBank.samples[drum.sound.offset]);
        const auto out = AudioManager::Instance->get_sample(idx);
        writer.Write(out);
        writer.Write(drum.sound.tuning);
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> BankFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& data) {
    auto banks = AudioManager::Instance->get_banks();
    auto bankId = data["id"].as<uint32_t>();

    if(AudioManager::Instance == nullptr){
        throw std::runtime_error("AudioManager not initialized");
    }

    return std::make_shared<BankData>(banks[bankId], bankId);
}