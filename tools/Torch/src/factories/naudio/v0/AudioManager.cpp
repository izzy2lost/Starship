#include "AudioManager.h"

#include <vector>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <cassert>
#include <cstring>
#include <factories/BaseFactory.h>

#include "hj/zip.h"
#include "hj/pyutils.h"
#include "spdlog/spdlog.h"
#include "lib/binarytools/BinaryReader.h"
#include "spdlog/spdlog.h"

std::unordered_map<std::string, uint32_t> name_table;
std::unordered_map<uint32_t, std::string> sample_table;
AudioManager* AudioManager::Instance;

std::vector<uint32_t> PyUtils::range(uint32_t start, uint32_t end) {
    std::vector<uint32_t> result;
    for (uint32_t i = start; i < end; ++i) {
        result.push_back(i);
    }
    return result;
}

std::string gen_name(const std::string& prefix){
    if(!name_table.contains(prefix)){
        name_table[prefix] = 0;
    }
    return prefix + std::to_string(name_table[prefix]++);
}

AudioBankSample* SampleBank::AddSample(uint32_t addr, size_t sampleSize, const AdpcmBook& book, const AdpcmLoop& loop){
    assert(sampleSize % 2 == 0);
    if(sampleSize % 9 != 0){
        assert(sampleSize % 9 == 1);
        sampleSize -= 1;
    }

    AudioBankSample* entry;

    if(this->entries.contains(addr)){
        entry = this->entries[addr];
        assert(entry->book == book);
        assert(entry->loop == loop);
        assert(entry->data.size() == sampleSize);
    } else {
        entry = new AudioBankSample{ gen_name("aifc"), PyUtils::slice(this->data, addr, addr + sampleSize), book, loop };
        this->entries[addr] = entry;
    }

    return entry;
}

void Bank::print() const {
    SPDLOG_DEBUG("Bank: {}", name);
    SPDLOG_DEBUG("Instruments: {}", std::to_string(insts.size()));
    SPDLOG_DEBUG("Drums: {}", std::to_string(drums.size()));
    SPDLOG_DEBUG("Samples: {}", std::to_string(samples.size()));
    SPDLOG_DEBUG("Envelopes: {}", std::to_string(envelopes.size()));
    SPDLOG_DEBUG("All Instruments: {}", std::to_string(allInsts.size()));
    SPDLOG_DEBUG("Inst Offsets: {}", std::to_string(instOffsets.size()));
    SPDLOG_DEBUG("Sample Bank: {}", sampleBank->name);
    SPDLOG_DEBUG("Sample Bank Offset: {}", std::to_string(sampleBank->offset));
}

std::vector<Entry> AudioManager::parse_seq_file(std::vector<uint8_t>& buffer, uint32_t offset, bool isCTL){
    std::vector<Entry> entries;
    LUS::BinaryReader reader((char*) buffer.data(), buffer.size());
    reader.SetEndianness(Torch::Endianness::Big);
    reader.Seek(offset, LUS::SeekOffsetType::Start);

    uint16_t magic = reader.ReadUInt16();
    uint16_t num_entries = reader.ReadUInt16();
    uint32_t prev = ALIGN(4 + num_entries * 8, 16);

    assert(magic == (isCTL ? 1 : 2));

    for (int i = 0; i < num_entries; ++i) {
        reader.Seek((offset + 4) + (i * 8), LUS::SeekOffsetType::Start);
        uint32_t addr = reader.ReadUInt32();
        uint32_t length = reader.ReadUInt32();

        if(isCTL){
            assert(addr == prev);
        } else {
            assert(addr <= prev);
        }

        prev = std::max(prev, addr + length);
        entries.push_back({addr, length});
    }

    reader.Close();
    return entries;
}

CTLHeader AudioManager::parse_ctl_header(std::vector<uint8_t>& data){
    LUS::BinaryReader reader((char*) data.data(), data.size());
    reader.SetEndianness(Torch::Endianness::Big);

    CTLHeader header = { reader.ReadUInt32(), reader.ReadUInt32(), reader.ReadUInt32() };

    reader.Close();
    return header;
}

Bank AudioManager::parse_ctl(CTLHeader header, std::vector<uint8_t> data, SampleBank* bank, uint32_t index) {
    name_table.clear();
    std::ostringstream ss;
    ss << std::hex << std::setw(2) << std::setfill('0') << index;
    std::string name = ss.str();
    uint32_t numInstruments = header.instruments;
    uint32_t numDrums = header.numDrums;
    char* rawData = (char*) data.data();

    uint32_t drumBaseAddr;
    memcpy(&drumBaseAddr, rawData, 4);
    drumBaseAddr = BSWAP32(drumBaseAddr);

    std::vector<uint32_t> drumOffsets;

    if(numDrums != 0){
        assert(drumBaseAddr != 0);
        for (size_t i = 0; i < numDrums; ++i) {
            uint32_t drumOffset;
            memcpy(&drumOffset, rawData + drumBaseAddr + i * 4, 4);
            if(drumOffset == 0){
                continue;
            }

            drumOffsets.push_back(BSWAP32(drumOffset));
        }
    } else {
        assert(drumBaseAddr == 0);
    }

    uint32_t instrumentBaseAddr = 4;
    std::vector<uint32_t> instrumentOffsets;
    std::vector<uint32_t> instrumentList;

    for (size_t i = 0; i < numInstruments; ++i) {
        uint32_t instOffset;
        memcpy(&instOffset, rawData + (instrumentBaseAddr + i * 4), 4);
        instOffset = BSWAP32(instOffset);
        if(instOffset == 0){
            instrumentList.push_back(NONE);
            instrumentOffsets.push_back(NONE);
        } else {
            instrumentOffsets.push_back(instOffset);
            instrumentList.push_back(instOffset);
        }
    }

//    std::sort(instrumentOffsets.begin(), instrumentOffsets.end());

    std::vector<Instrument> insts;
    for(auto &offset : instrumentOffsets){
        if(offset == NONE){
            Instrument invalid = { .valid = false };
            insts.push_back(invalid);
            continue;
        }
        auto rInst = PyUtils::slice(data, offset, offset + 32);
        Instrument inst = parse_inst(rInst, offset);
        insts.push_back(inst);
    }

    std::vector<Drum> drums;
    for(auto &offset : drumOffsets){
        auto rDrum = PyUtils::slice(data, offset, offset + 16);
        Drum drum = parse_drum(rDrum, offset);
        drums.push_back(drum);
    }

    auto envOffsets = std::vector<uint32_t>();
    auto sampleOffsets = std::vector<uint32_t>();
    auto tunings = std::unordered_map<uint32_t, float>();

    for(auto &inst : insts){
        for(auto &sound : {inst.soundLo, inst.soundMed, inst.soundHi}){
            if(sound.has_value()){
                sampleOffsets.push_back(sound.value().offset);
                tunings[sound.value().offset] = sound.value().tuning;
            }
        }
        envOffsets.push_back(inst.envelope);
    }
    for(auto &drum : drums){
        sampleOffsets.push_back(drum.sound.offset);
        tunings[drum.sound.offset] = drum.sound.tuning;
        envOffsets.push_back(drum.envelope);
    }
    // Put drums somewhere in the middle of the instruments to make sample
    // addresses come in increasing order. (This logic isn't totally right,
    // but it works for our purposes.)
    std::vector<std::variant<Instrument, std::vector<Drum>>> allInsts;
    bool needDrums = !drums.empty();
    for(auto &inst : insts){
        std::vector<std::optional<AudioBankSound>> sounds = {inst.soundLo, inst.soundMed, inst.soundHi};

        if(needDrums && std::any_of(sounds.cbegin(), sounds.cend(), [&drums](std::optional<AudioBankSound> sound){ return sound.has_value() && sound.value().offset > drums[0].sound.offset; })){
            allInsts.emplace_back(drums);
            needDrums = false;
        }

        allInsts.emplace_back(inst);
    }


    if(needDrums){
        allInsts.emplace_back(drums);
    }

    std::map<uint32_t, AudioBankSample*> samples;
    std::sort(sampleOffsets.begin(), sampleOffsets.end());
    for(auto &offset : sampleOffsets){
        auto rSample = PyUtils::slice(data, offset, offset + 20);
        AudioBankSample* sample = parse_sample(rSample, data, bank);
        for(auto &tuning : tunings){
            sample->tunings.push_back(tuning.second);
        }
        samples[offset] = sample;
    }

    std::unordered_map<uint32_t, std::vector<AdsrEnvelope>> envData;
    std::vector<uint32_t> usedEnvOffsets;
    std::sort(envOffsets.begin(), envOffsets.end());
    for(auto &offset : envOffsets){
        auto env = parse_envelope(offset, data);
        envData[offset] = env;
        for(int i = 0; i < ALIGN(env.size(), 4); i++){
            usedEnvOffsets.push_back(offset + (i * 4));
        }
    }

    std::vector<uint32_t> unusedEnvOffsets;
    if(!usedEnvOffsets.empty()){
        size_t min = std::min_element(usedEnvOffsets.begin(), usedEnvOffsets.end()) - usedEnvOffsets.begin();
        size_t max = std::max_element(usedEnvOffsets.begin(), usedEnvOffsets.end()) - usedEnvOffsets.begin();
        for(size_t idx = min + 4; idx < max; idx += 4){
            uint32_t addr = usedEnvOffsets[idx];
            if(std::find(usedEnvOffsets.begin(), usedEnvOffsets.end(), addr) == usedEnvOffsets.end()){
                unusedEnvOffsets.push_back(addr);
                uint32_t stubMarker;
                memcpy(&stubMarker, rawData + addr, 4);
                stubMarker = BSWAP32(stubMarker);
                assert(stubMarker == 0);
                auto env = parse_envelope(addr, data);
                envData[addr] = env;
                for(int i = 0; i < ALIGN(env.size(), 4); i++){
                    usedEnvOffsets.push_back(addr + (i * 4));
                }
            }
        }
    }

    std::map<uint32_t, Envelope> envelopes;
    for(auto &entry : envData){
        Envelope env = { gen_name("envelope"), entry.second };
        envelopes[entry.first] = env;
    }

    Bank bankData = { name, bank, insts, drums, allInsts, instrumentList, envelopes, samples };
    return bankData;
}

std::optional<AudioBankSound> AudioManager::parse_sound(std::vector<uint8_t> data) {
    LUS::BinaryReader reader((char*) data.data(), data.size());
    reader.SetEndianness(Torch::Endianness::Big);

    uint32_t addr = reader.ReadUInt32();
    float tuning = reader.ReadFloat();

    if(addr == 0){
        assert(tuning == 0.0f);
        return std::nullopt;
    }

    AudioBankSound sound = { addr, tuning };

    reader.Close();
    return sound;
}

Drum AudioManager::parse_drum(std::vector<uint8_t>& data, uint32_t addr) {
    LUS::BinaryReader reader((char*) data.data(), data.size());
    reader.SetEndianness(Torch::Endianness::Big);
    std::string name = gen_name("drum");

    uint8_t releaseRate = reader.ReadInt8();
    uint8_t pan = reader.ReadInt8();

    reader.Seek(12, LUS::SeekOffsetType::Start);
    AudioBankSound sound = parse_sound(PyUtils::slice(data, 4, 12)).value();
    uint32_t envOffset = reader.ReadInt32();
    assert(envOffset != 0);

    Drum drum = { name, addr, releaseRate, pan, envOffset, sound };

    return drum;
}

Instrument AudioManager::parse_inst(std::vector<uint8_t>& data, uint32_t addr) {
    std::string name = gen_name("inst");
    uint8_t normalRangeLo = data[1];
    uint8_t normalRangeHi = data[2];
    uint8_t releaseRate = data[3];

    uint32_t envAddr;
    memcpy(&envAddr, (char*) data.data() + 4, 4);
    envAddr = BSWAP32(envAddr);

    assert(envAddr != 0);

    auto soundLo  = parse_sound(PyUtils::slice(data, 8, 16));
    auto soundMed = parse_sound(PyUtils::slice(data, 16, 24));
    auto soundHi  = parse_sound(PyUtils::slice(data, 24));

    if (soundLo == std::nullopt) {
        assert(normalRangeLo == 0);
    }
    if (soundHi == std::nullopt) {
        assert(normalRangeHi == 127);
    }

    Instrument inst = { true, name, addr, releaseRate, normalRangeLo, normalRangeHi, envAddr, soundLo, soundMed, soundHi };
    return inst;
}

AdpcmLoop AudioManager::parse_loop(uint32_t addr, std::vector<uint8_t>& bankData){
    LUS::BinaryReader reader((char*) bankData.data(), bankData.size());
    reader.SetEndianness(Torch::Endianness::Big);
    reader.Seek(addr, LUS::SeekOffsetType::Start);

    std::optional<std::vector<int16_t>> state = std::nullopt;
    uint32_t start = reader.ReadUInt32();
    uint32_t end = reader.ReadUInt32();
    int32_t count = reader.ReadInt32();
    uint32_t pad = reader.ReadUInt32();

    if(count != 0){
        state = std::vector<int16_t>();
        for (size_t i = 0; i < 16; ++i) {
            state.value().push_back(reader.ReadInt16());
        }
    }
    AdpcmLoop loop = { start, end, count, pad, state };
    return loop;
}

AdpcmBook AudioManager::parse_book(uint32_t addr, std::vector<uint8_t>& bankData){
    LUS::BinaryReader reader((char*) bankData.data(), bankData.size());
    reader.SetEndianness(Torch::Endianness::Big);
    reader.Seek(addr, LUS::SeekOffsetType::Start);

    int32_t order = reader.ReadInt32();
    int32_t npredictors = reader.ReadInt32();

    assert(order == 2);
    assert(npredictors == 2);

    std::vector<int16_t> table;
    std::vector<uint8_t> tableData = PyUtils::slice(bankData, addr + 8, addr + 8 + 16 * order * npredictors);
    for (size_t i = 0; i < ( 16 * order * npredictors ); i += 2) {
        int16_t dtable;
        memcpy(&dtable, tableData.data() + i, 2);
        table.push_back(BSWAP16(dtable));
    }

    AdpcmBook book = { order, npredictors, table };
    return book;
}

AudioBankSample* AudioManager::parse_sample(std::vector<uint8_t>& data, std::vector<uint8_t>& bankData, SampleBank* sampleBank){
    LUS::BinaryReader reader((char*) data.data(), data.size());
    reader.SetEndianness(Torch::Endianness::Big);

    uint32_t zero = reader.ReadUInt32();
    uint32_t addr = reader.ReadUInt32();
    uint32_t loop = reader.ReadUInt32();
    uint32_t book = reader.ReadUInt32();
    uint32_t sampleSize = reader.ReadUInt32();

    SPDLOG_INFO("Zero: 0x{:X}", zero);
    SPDLOG_INFO("Addr: 0x{:X}", addr);
    SPDLOG_INFO("Loop: 0x{:X}", loop);
    SPDLOG_INFO("Book: 0x{:X}", book);
    SPDLOG_INFO("Sample Size: {}", sampleSize);

    // assert(zero == 0);
    assert(loop != 0);
    assert(book != 0);

    AdpcmLoop loopData = parse_loop(loop, bankData);
    AdpcmBook bookData = parse_book(book, bankData);

    reader.Close();
    return sampleBank->AddSample(addr, sampleSize, bookData, loopData);
}


std::vector<AdsrEnvelope> AudioManager::parse_envelope(uint32_t addr, std::vector<uint8_t>& dataBank){
    std::vector<AdsrEnvelope> entries;
    LUS::BinaryReader reader((char*) dataBank.data(), dataBank.size());
    reader.SetEndianness(Torch::Endianness::Big);

    while(true){
        reader.Seek(addr, LUS::SeekOffsetType::Start);
        int16_t delay = reader.ReadInt16();
        int16_t arg = reader.ReadInt16();
        AdsrEnvelope entry = { delay, arg };
        entries.push_back(entry);
        addr += 4;
        if (1 <= (-delay) % (1 << 16) && (-delay) % (1 << 16) <= 3){
            break;
        }
    }

    reader.Close();
    return entries;
}

TBLFile AudioManager::parse_tbl(std::vector<uint8_t>& data, std::vector<Entry>& entries) {
    TBLFile tbl;
    std::unordered_map<uint32_t, std::string> cache;
    for(auto &entry : entries){
        if(!cache.contains(entry.offset)){
            std::string name = gen_name("sample_bank");
            auto* sampleBank = new SampleBank{
                name, entry.offset, PyUtils::slice(data, entry.offset, entry.offset + entry.length)
            };
            tbl.banks.push_back(sampleBank);
            tbl.map[name] = sampleBank;
            cache[entry.offset] = name;
        }
        tbl.tbls.push_back(cache[entry.offset]);
    }
    cache.clear();
    return tbl;
}

void AudioManager::initialize(std::vector<uint8_t>& buffer, YAML::Node& data) {

    auto ctlOffset = data["ctl"]["offset"].as<size_t>();
    auto ctlSize = data["ctl"]["size"].as<size_t>();

    auto tblOffset = data["tbl"]["offset"].as<size_t>();
    auto tblSize = data["tbl"]["size"].as<size_t>();

    std::vector<Entry> tbl = parse_seq_file(buffer, tblOffset, false);
    std::vector<Entry> ctl = parse_seq_file(buffer, ctlOffset, true);

    SPDLOG_INFO("Raw TBL Entries: {}", tbl.size());
    SPDLOG_INFO("Raw CTL Entries: {}", ctl.size());

    std::vector<uint8_t> tbl_data = PyUtils::slice(buffer, tblOffset, tblOffset + tblSize);
    std::vector<uint8_t> ctl_data = PyUtils::slice(buffer, ctlOffset, ctlOffset + ctlSize);
    this->loaded_tbl = parse_tbl(tbl_data, tbl);

    SPDLOG_INFO("Processed TBL Entries: {}", this->loaded_tbl.tbls.size());
    SPDLOG_INFO("Processed TBL Banks: {}", this->loaded_tbl.banks.size());

    auto zipped = zip(PyUtils::range(0, ctl.size()), ctl, this->loaded_tbl.tbls);

    for (const auto& item : zipped) {
        auto [index, ctrl, sample_bank_name] = item;
        auto sample_bank = this->loaded_tbl.map[sample_bank_name];
        auto entry = PyUtils::slice(ctl_data, ctrl.offset, ctrl.offset + ctrl.length);
        auto headerRaw = PyUtils::slice(entry, 0, 16);
        auto header = parse_ctl_header(headerRaw);
        auto bank = parse_ctl(header, PyUtils::slice(entry, 16), sample_bank, index);
        banks[index] = bank;
        SPDLOG_INFO("Processed Bank {}", index);
    }

    int32_t idx = -1;
    for(auto &sample_bank : this->loaded_tbl.banks){
        auto offsets = PyUtils::keys(sample_bank->entries);
        std::sort(offsets.begin(), offsets.end());

        for(auto &offset : offsets){
            this->sampleMap[sample_bank->entries[offset]] = ++idx;
        }
    }
}

void AudioManager::bind_sample(YAML::Node& node, const std::string& path){
    auto id = GetSafeNode<uint32_t>(node, "id");
    sample_table[id] = path;
}

std::string& AudioManager::get_sample(uint32_t id) {
    if(!sample_table.contains(id)) {
        throw std::runtime_error("Failed to find sample with id " + std::to_string(id));
    }
    return sample_table[id];
}

/*
void AudioManager::create_aifc(int32_t index, LUS::BinaryWriter &out) {
    int32_t idx = -1;
    for(auto &sample_bank : this->loaded_tbl.banks){
        auto offsets = PyUtils::keys(sample_bank->entries);
        std::sort(offsets.begin(), offsets.end());

        for(auto &offset : offsets){
            if(++idx == index){
                write_aifc(sample_bank->entries[offset], out);
                return;
            }
        }
    }
}
*/

AudioBankSample AudioManager::get_aifc(int32_t index) {
    int32_t idx = 0;
    for(auto &sample_bank : this->loaded_tbl.banks){
        auto offsets = PyUtils::keys(sample_bank->entries);
        std::sort(offsets.begin(), offsets.end());

        for(auto &offset : offsets){
            if(idx++ == index){
                return *sample_bank->entries[offset];
            }
        }
    }

    SPDLOG_ERROR("Invalid Index {}", index);
    throw std::runtime_error("Invalid index");
}

uint32_t AudioManager::get_index(AudioBankSample* entry) {
    if(!this->sampleMap.contains(entry)){
        return -1;
    }
    return this->sampleMap[entry];
}

std::map<uint32_t, Bank> AudioManager::get_banks() {
    return this->banks;
}

std::vector<SampleBank*> AudioManager::get_loaded_banks() {
    return this->loaded_tbl.banks;
}

std::vector<AudioBankSample*> AudioManager::get_samples() {
    std::vector<AudioBankSample*> samples;
    for(auto &bank : this->loaded_tbl.banks){
        for(auto &entry : bank->entries){
            // Avoid duplicates
            if(std::find(samples.begin(), samples.end(), entry.second) == samples.end()){
                samples.push_back(entry.second);
            }
        }
    }
    return samples;
}