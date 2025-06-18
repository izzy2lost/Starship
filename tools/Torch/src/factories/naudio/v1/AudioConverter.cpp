#include "AudioConverter.h"

#include <factories/naudio/v1/BookFactory.h>
#include <factories/naudio/v1/LoopFactory.h>
#include <factories/naudio/v1/AudioContext.h>
#include <factories/naudio/v1/SampleFactory.h>
#include <factories/BaseFactory.h>
#include <Companion.h>
#include <cassert>
#include <cstring>
#include "hj/pyutils.h"

void AIFCWriter::End(std::string chunk, LUS::BinaryWriter& writer) {
    auto buffer = writer.ToVector();
    this->Chunks.push_back({
        chunk, buffer
    });

    this->totalSize += ALIGN(buffer.size(), 2) + 8;
}

void AIFCWriter::Close(LUS::BinaryWriter& out){
    out.SetEndianness(Torch::Endianness::Big);

    out.Write(AIFCMagicValues::FORM);
    out.Write((uint32_t) (this->totalSize + 4));
    out.Write(AIFCMagicValues::AIFC);

    for(auto& chunk : this->Chunks){
        out.Write((char*) chunk.id.data(), chunk.id.size());
        out.Write((uint32_t) chunk.data.size());
        out.Write((char*) chunk.data.data(), chunk.data.size());

        if(chunk.data.size() % 2 == 1) {
            out.Write((uint8_t) 0);
        }
    }
}

// Function to serialize double to 80-bit extended-precision
void SerializeF80(double num, LUS::BinaryWriter &writer) {
    // Convert the input double to a uint64_t representation
    uint64_t f64;
    memcpy((void*) &f64, (void*) &num, sizeof(double));

    // Extract the sign bit
    uint64_t f64_sign_bit = f64 & (1ULL << 63);

    // Handle the special case: zero
    if (num == 0.0) {
        if (f64_sign_bit) {
            writer.Write(static_cast<uint16_t>(0x8000)); // Sign bit set
        } else {
            writer.Write(static_cast<uint16_t>(0x0000)); // No sign bit
        }
        writer.Write(static_cast<uint64_t>(0x0000000000000000)); // Zero mantissa
        return;
    }

    // Extract the exponent and mantissa
    uint64_t exponent = (f64 >> 52) & 0x7FF; // Exponent bits
    assert(exponent != 0);                        // Ensure not denormal
    assert(exponent != 0x7FF);                    // Ensure not infinity/NaN

    exponent -= 1023; // Adjust bias for 64-bit

    uint64_t f64_mantissa_bits = f64 & ((1ULL << 52) - 1); // Mantissa bits

    // Construct the 80-bit extended-precision fields
    uint64_t f80_sign_bit = f64_sign_bit << (80 - 64);           // Shift sign
    uint64_t f80_exponent = (exponent + 0x3FFF) & 0x7FFF;        // Adjust bias
    uint64_t f80_mantissa_bits = (1ULL << 63) | (f64_mantissa_bits << (63 - 52)); // Add implicit bit

    // Combine components into the 80-bit representation
    uint16_t high = static_cast<uint16_t>(f80_sign_bit >> 48) | static_cast<uint16_t>(f80_exponent);
    uint64_t low = f80_mantissa_bits;

    // Write the result in big-endian order
    writer.Write(high);
    writer.Write(low);
}

void AudioConverter::SampleV0ToAIFC(AudioBankSample* sample, LUS::BinaryWriter &out) {
    auto aifc = AIFCWriter();
    auto data = sample->data;

    uint32_t num_frames = data.size() * 16 / 9;
    uint32_t sample_rate = -1;

    if(sample->tunings.size() == 1){
        sample_rate = 32000 * sample->tunings[0];
    } else {
        float tmin = PyUtils::min(sample->tunings);
        float tmax = PyUtils::max(sample->tunings);

        if(tmin <= 0.5f <= tmax){
            sample_rate = 16000;
        } else if(tmin <= 1.0f <= tmax){
            sample_rate = 32000;
        } else if(tmin <= 1.5f <= tmax){
            sample_rate = 48000;
        } else if(tmin <= 2.5f <= tmax){
            sample_rate = 80000;
        } else {
            sample_rate = 16000 * (tmin + tmax);
        }
    }

    int16_t num_channels = 1;
    int16_t sample_size = 16;

    // COMM Chunk
    auto comm = aifc.Start();
    comm.Write(num_channels);
    comm.Write(num_frames);
    comm.Write(sample_size);
    SerializeF80(sample_rate, comm);
    comm.Write(AIFCMagicValues::VAPC);
    comm.Write((char*) "\x0bVADPCM ~4-1", 12);
    aifc.End("COMM", comm);

    // INST Chunk
    auto inst = aifc.Start();
    for(size_t i = 0; i < 5; i++){
        inst.Write((int32_t) 0);
    }
    aifc.End("INST", inst);

    // VADPCMCODES Chunk
    auto vcodes = aifc.Start();
    vcodes.Write((char*) "stoc\x0bVADPCMCODES", 16);
    vcodes.Write((int16_t) 1);
    vcodes.Write((int16_t) sample->book.order);
    vcodes.Write((int16_t) sample->book.npredictors);

    for(auto page : sample->book.table){
        vcodes.Write(page);
    }
    aifc.End("APPL", vcodes);

    // SSND Chunk
    auto ssnd = aifc.Start();
    ssnd.Write((uint64_t) 0);
    ssnd.Write((char*) data.data(), data.size());
    aifc.End("SSND", ssnd);

    // VADPCMLOOPS
    if(sample->loop.count != 0){
        auto vloops = aifc.Start();
        vloops.Write((char*) "stoc\x0bVADPCMLOOPS", 16);
        vloops.Write((uint16_t) 1);
        vloops.Write((uint16_t) 1);
        vloops.Write(sample->loop.start);
        vloops.Write(sample->loop.end);
        vloops.Write(sample->loop.count);

        if(sample->loop.state.has_value()){
            for(auto state : sample->loop.state.value()){
                vcodes.Write(state);
            }
        }
        aifc.End("APPL", vloops);
    }

    aifc.Close(out);
}

void AudioConverter::SampleV1ToAIFC(NSampleData* sample, LUS::BinaryWriter &out) {
    auto loop = std::static_pointer_cast<ADPCMLoopData>(Companion::Instance->GetParseDataByAddr(sample->loop)->data.value());
    auto book = std::static_pointer_cast<ADPCMBookData>(Companion::Instance->GetParseDataByAddr(sample->book)->data.value());
    auto entry = AudioContext::tables[AudioTableType::SAMPLE_TABLE];
    auto sampleData = entry.buffer.data() + entry.info->entries[sample->sampleBankId].addr + sample->sampleAddr;
    auto aifc = AIFCWriter();
    std::vector<uint8_t> data(sampleData, sampleData + sample->size);

    uint32_t num_frames = data.size() * 16 / 9;
    uint32_t sample_rate = sample->sampleRate;

    if(sample_rate == 0){
        sample_rate = 32000 * sample->tuning;
    }

    int16_t num_channels = 1;
    int16_t sample_size = 16;

    // COMM Chunk
    auto comm = aifc.Start();
    comm.Write(num_channels);
    comm.Write(num_frames);
    comm.Write(sample_size);
    SerializeF80(sample_rate, comm);
    comm.Write(AIFCMagicValues::VAPC);
    comm.Write((char*) "\x0bVADPCM ~4-1", 12);
    aifc.End("COMM", comm);

    // INST Chunk
    auto inst = aifc.Start();
    for(size_t i = 0; i < 5; i++){
        inst.Write((int32_t) 0);
    }
    aifc.End("INST", inst);

    // VADPCMCODES Chunk
    auto vcodes = aifc.Start();
    vcodes.Write((char*) "stoc\x0bVADPCMCODES", 16);
    vcodes.Write((int16_t) 1);
    vcodes.Write((int16_t) book->order);
    vcodes.Write((int16_t) book->numPredictors);

    for(auto page : book->book){
        vcodes.Write(page);
    }
    aifc.End("APPL", vcodes);

    // SSND Chunk
    auto ssnd = aifc.Start();
    ssnd.Write((uint64_t) 0);
    ssnd.Write((char*) data.data(), data.size());
    aifc.End("SSND", ssnd);

    // VADPCMLOOPS
    if(loop->count != 0){
        auto vloops = aifc.Start();
        vloops.Write((char*) "stoc\x0bVADPCMLOOPS", 16);
        vloops.Write((uint16_t) 1);
        vloops.Write((uint16_t) 1);
        vloops.Write(loop->start);
        vloops.Write(loop->end);
        vloops.Write(loop->count);

        for(size_t i = 0; i < 16; i++){
            vloops.Write(loop->predictorState[i]);
        }
        aifc.End("APPL", vloops);
    }

    aifc.Close(out);
}