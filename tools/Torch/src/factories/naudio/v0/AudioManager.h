#pragma once

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include "lib/binarytools/BinaryWriter.h"
#include <variant>
#include <optional>

#define NONE 0xFFFF
#define ALIGN(val, al) (size_t) ((val + (al - 1)) & -al)

struct Entry {
    uint32_t offset;
    uint32_t length;
};

struct AudioBankSound {
    uint32_t offset;
    float tuning;
    bool operator==(const AudioBankSound& other) const {
        return offset == other.offset && tuning == other.tuning;
    }
};

struct Instrument {
    bool valid;
    std::string name;
    uint32_t offset;
    uint8_t releaseRate;
    uint8_t normalRangeLo;
    uint8_t normalRangeHi;
    uint32_t envelope;
    std::optional<AudioBankSound> soundLo;
    std::optional<AudioBankSound> soundMed;
    std::optional<AudioBankSound> soundHi;
};

struct AdpcmBook {
    int32_t order{};
    int32_t npredictors{};
    std::vector<int16_t> table;
    bool operator==(const AdpcmBook& other) const {
        return order == other.order && npredictors == other.npredictors && table == other.table;
    }
};

struct Drum {
    std::string name;
    uint32_t offset;
    uint8_t releaseRate;
    uint8_t pan;
    uint32_t envelope;
    AudioBankSound sound;
    bool operator==(const Drum& other) const {
        return name == other.name && offset == other.offset && releaseRate == other.releaseRate && pan == other.pan && envelope == other.envelope && sound == other.sound;
    }
};

struct AdpcmLoop {
    uint32_t start{};
    uint32_t end{};
    int32_t count{};
    uint32_t pad{};
    std::optional<std::vector<int16_t>> state;
    bool operator==(const AdpcmLoop& other) const {
        return start == other.start && end == other.end && count == other.count && pad == other.pad && state == other.state;
    }
};

struct AudioBankSample {
    std::string name;
    std::vector<uint8_t> data;
    AdpcmBook book;
    AdpcmLoop loop;
    std::vector<float> tunings;
};

struct SampleBank {
    std::string name;
    uint32_t offset;
    std::vector<uint8_t> data;
    std::map<uint32_t, AudioBankSample*> entries;

    AudioBankSample* AddSample(uint32_t addr, size_t sampleSize, const AdpcmBook& book, const AdpcmLoop& loop);
};

struct TBLFile {
    std::vector<SampleBank*> banks;
    std::vector<std::string> tbls;
    std::map<std::string, SampleBank*> map;
};

struct CTLHeader {
    uint32_t instruments;
    uint32_t numDrums;
    uint32_t shared;
};

struct AdsrEnvelope {
    int16_t delay;
    int16_t arg;
};

struct Envelope {
    std::string name;
    std::vector<AdsrEnvelope> entries;
};

struct Bank {
    std::string name;
    SampleBank* sampleBank;
    std::vector<Instrument> insts;
    std::vector<Drum> drums;
    std::vector<std::variant<Instrument, std::vector<Drum>>> allInsts;
    std::vector<uint32_t> instOffsets;
    std::map<uint32_t, Envelope> envelopes;
    std::map<uint32_t, AudioBankSample*> samples;
    void print() const;
};

class AudioManager {
public:
    static AudioManager* Instance;
    void initialize(std::vector<uint8_t>& buffer, YAML::Node& data);
    void bind_sample(YAML::Node& node, const std::string& path);
    std::string& get_sample(uint32_t id);
    AudioBankSample get_aifc(int32_t index);
    std::map<uint32_t, Bank> get_banks();
    std::vector<SampleBank*> get_loaded_banks();
    std::vector<AudioBankSample*> get_samples();
    uint32_t get_index(AudioBankSample* bank);
private:
    std::map<uint32_t, Bank> banks;
    std::map<AudioBankSample*, uint32_t> sampleMap;
    TBLFile loaded_tbl;

    static std::vector<Entry> parse_seq_file(std::vector<uint8_t>& buffer, uint32_t offset, bool isCTL);
    static CTLHeader parse_ctl_header(std::vector<uint8_t>& data);
    static std::optional<AudioBankSound> parse_sound(std::vector<uint8_t> data);
    static Drum parse_drum(std::vector<uint8_t>& data, uint32_t addr);
    static Instrument parse_inst(std::vector<uint8_t>& data, uint32_t addr);
    static AdpcmLoop parse_loop(uint32_t addr, std::vector<uint8_t>& bankData);
    static AdpcmBook parse_book(uint32_t addr, std::vector<uint8_t>& bankData);
    static AudioBankSample* parse_sample(std::vector<uint8_t>& data, std::vector<uint8_t>& bankData, SampleBank* sampleBank);
    static std::vector<AdsrEnvelope> parse_envelope(uint32_t addr, std::vector<uint8_t>& dataBank);
    static Bank parse_ctl(CTLHeader header, std::vector<uint8_t> data, SampleBank* bank, uint32_t index);
    static TBLFile parse_tbl(std::vector<uint8_t>& data, std::vector<Entry>& entries);
};