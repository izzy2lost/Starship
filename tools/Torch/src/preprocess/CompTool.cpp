#include "CompTool.h"
#include "utils/Decompressor.h"
#include "lib/binarytools/BinaryWriter.h"
#include "lib/binarytools/BinaryReader.h"
#include <fstream>
#include <cstring>

uint32_t CompTool::FindFileTable(std::vector<uint8_t>& rom) {
    uint8_t query_one[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x50, 0x00, 0x00, 0x00, 0x00 };
    uint8_t query_two[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x60, 0x00, 0x00, 0x00, 0x00 };

    for(size_t i = 0; i < rom.size() - sizeof(query_one); i++) {
        if(memcmp(rom.data() + i, query_one, sizeof(query_one)) == 0){
            return i;
        }

        if(memcmp(rom.data() + i, query_two, sizeof(query_two)) == 0){
            return i;
        }
    }

    throw std::runtime_error("Failed to find file table");
}

#define ROL(i, b) ((i << (b)) | (i >> (32 - (b))))

std::pair<uint32_t, uint32_t> CompTool::CalculateCRCs(LUS::BinaryWriter& decompFile)
{
    uint32_t start = 0x1000;
    uint32_t end = 0x101000;
    LUS::BinaryReader readFile(decompFile.GetStream());
    uint32_t t1, t2, t3, t4, t5, t6;

    // Todo: Implement other bootcodes
    t1 = t2 = t3 = t4 = t5 = t6 = sCrcSeed;

    readFile.SetEndianness(Torch::Endianness::Big);
    readFile.Seek(start, LUS::SeekOffsetType::Start);

    for (size_t i = start; i < end; i += sizeof(uint32_t)) {
        uint32_t d = readFile.ReadUInt32();
        uint32_t r = ROL(d, d & 0x1F);

        if ((t6 + d) < t6) {
            t4++;
        }

        t3 ^= d;
        t6 += d;
        t2 ^= ((t2 > d) ? r : (t6 ^ d));
        t5 += r;
        t1 += (t5 ^ d);
    }

    return std::make_pair(t6 ^ t4 ^ t3, t5 ^ t2 ^ t1);
}

std::vector<uint8_t> CompTool::Decompress(std::vector<uint8_t> rom){
    LUS::BinaryReader basefile((char*) rom.data(), rom.size());
    basefile.SetEndianness(Torch::Endianness::Big);

    LUS::BinaryWriter decompfile;
    decompfile.SetEndianness(Torch::Endianness::Big);
    decompfile.Write((uint8_t)0x80);

    uint32_t table = CompTool::FindFileTable(rom);
    uint32_t count = 0;
    while (true){
        auto entry = table + 0x10 * count;
        basefile.Seek(entry, LUS::SeekOffsetType::Start);

        auto v_begin = basefile.ReadInt32();
        auto p_begin = basefile.ReadInt32();
        auto p_end = basefile.ReadInt32();
        auto comp_flag = basefile.ReadInt32();

        auto p_size = p_end - p_begin;
        auto v_size = (int32_t) 0;
        DataChunk* decoded = nullptr;

        if(v_begin == 0 && p_end == 0){
            break;
        }

        basefile.Seek(p_begin, LUS::SeekOffsetType::Start);

        auto bytes = new uint8_t[p_size];
        basefile.Read((char*) bytes, p_size);

        switch ((CompType) comp_flag) {
            case CompType::UNCOMPRESSED:
                v_size = p_size;
                break;
            case CompType::COMPRESSED:
                decoded = Decompressor::Decode(std::vector(bytes, bytes + p_size), 0, CompressionType::MIO0, true);
                bytes = decoded->data;
                v_size = decoded->size;
                break;
            default:
                throw std::runtime_error("Invalid compression flag. There may be a problem with your ROM.");
        }

        decompfile.Seek(v_begin, LUS::SeekOffsetType::Start);
        decompfile.Write((char*) bytes, v_size);
        auto v_end = v_begin + v_size;

        decompfile.Seek(entry + 4, LUS::SeekOffsetType::Start);
        decompfile.Write(v_begin);
        decompfile.Write(v_end);
        decompfile.Write((uint32_t) CompType::UNCOMPRESSED);
        count++;
    }

    auto crcs = CompTool::CalculateCRCs(decompfile);

    decompfile.Seek(0x10, LUS::SeekOffsetType::Start);

    decompfile.Write(crcs.first); // CRC1
    decompfile.Write(crcs.second); // CRC2

    auto result = decompfile.ToVector();
    return { (uint8_t*) result.data(), (uint8_t*) result.data() + result.size() };
}