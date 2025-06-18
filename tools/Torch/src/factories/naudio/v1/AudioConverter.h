#pragma once

#include <factories/BaseFactory.h>
#include <factories/naudio/v1/SampleFactory.h>
#include <factories/naudio/v0/AudioManager.h>

enum AIFCMagicValues {
    FORM = (uint32_t) 0x464f524d,
    AIFC = (uint32_t) 0x41494643,
    VAPC = (uint32_t) 0x56415043,
    AAPL = (uint32_t) 0x4150504c
};

#define NONE 0xFFFF
#define ALIGN(val, al) (size_t) ((val + (al - 1)) & -al)

class AudioConverter {
public:
    static void SampleV0ToAIFC(AudioBankSample* entry, LUS::BinaryWriter &out);
    static void SampleV1ToAIFC(NSampleData* tSample, LUS::BinaryWriter &out);
};

struct AIFCChunk {
    std::string id;
    std::vector<char> data;
};

class AIFCWriter {
public:
    std::vector<AIFCChunk> Chunks;
    size_t totalSize = 0;

    LUS::BinaryWriter Start(){
        auto writer = LUS::BinaryWriter();
        writer.SetEndianness(Torch::Endianness::Big);
        return writer;
    }
    void End(std::string chunk, LUS::BinaryWriter& writer);
    void Close(LUS::BinaryWriter& out);
};