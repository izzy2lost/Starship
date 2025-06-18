#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>
#include "lib/binarytools/endianness.h"
#include "lib/binarytools/Stream.h"

namespace LUS {
class BinaryWriter {
  public:
    BinaryWriter();
    BinaryWriter(Stream* nStream);
    BinaryWriter(std::shared_ptr<Stream> nStream);

    void SetEndianness(Torch::Endianness endianness);

    std::shared_ptr<Stream> GetStream();
    uint64_t GetBaseAddress();
    uint64_t GetLength();
    void Seek(int32_t offset, SeekOffsetType seekType);
    void Close();

    void Write(int8_t value);
    void Write(uint8_t value);
    void Write(int16_t value);
    void Write(uint16_t value);
    void Write(int32_t value);
    void Write(int32_t valueA, int32_t valueB);
    void Write(uint32_t value);
    void Write(int64_t value);
    void Write(uint64_t value);
    void Write(float value);
    void Write(double value);
    void Write(const std::string& str, bool writeLength = true);
    void Write(char* srcBuffer, size_t length);
    void WriteByte(char value);

    std::vector<char> ToVector();

    void Finish(std::ostream &output);

protected:
    std::shared_ptr<Stream> mStream;
    Torch::Endianness mEndianness = Torch::Endianness::Native;

};
} // namespace LUS
