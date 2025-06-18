#pragma once

#include <string>
#include <memory>
#include <vector>
#include "lib/binarytools/endianness.h"
#include "lib/binarytools/Stream.h"

class BinaryReader;

namespace LUS {

class BinaryReader {
  public:
    BinaryReader(char* nBuffer, size_t nBufferSize);
    BinaryReader(uint8_t* nBuffer, size_t nBufferSize);
    BinaryReader(Stream* nStream);
    BinaryReader(std::shared_ptr<Stream> nStream);

    void Close();

    void SetEndianness(Torch::Endianness endianness);
    Torch::Endianness GetEndianness() const;

    void Seek(int32_t offset, SeekOffsetType seekType);
    uint32_t GetBaseAddress();
    size_t GetLength();

    void Read(int32_t length);
    void Read(char* buffer, int32_t length);
    char ReadChar();
    int8_t ReadInt8();
    int16_t ReadInt16();
    int32_t ReadInt32();
    uint8_t ReadUByte();
    uint16_t ReadUInt16();
    uint32_t ReadUInt32();
    uint64_t ReadUInt64();
    unsigned short ReadUShort();
    short ReadShort();
    float ReadFloat();
    double ReadDouble();
    std::string ReadString();
    std::string ReadCString();

    std::vector<char> ToVector();

  protected:
    std::shared_ptr<Stream> mStream;
    Torch::Endianness mEndianness = Torch::Endianness::Native;
};
} // namespace LUS
