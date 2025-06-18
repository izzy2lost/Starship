#include "Cartridge.h"

#include "lib/binarytools/BinaryReader.h"
#include <Companion.h>

void N64::Cartridge::Initialize() {
    LUS::BinaryReader reader((char*) this->gRomData.data(), this->gRomData.size());
    reader.SetEndianness(Torch::Endianness::Big);
    reader.Seek(0x10, LUS::SeekOffsetType::Start);
    this->gRomCRC = BSWAP32(reader.ReadUInt32());
    reader.Seek(0x20, LUS::SeekOffsetType::Start);
    this->gGameTitle = std::string(reader.ReadCString());
    this->gGameTitle.pop_back(); // Remove null terminator
    reader.Seek(0x3E, LUS::SeekOffsetType::Start);
    uint8_t country = reader.ReadUByte();
    this->gVersion = reader.ReadUByte();
    this->gHash = Companion::CalculateHash(this->gRomData);
    switch (country) {
        case 'J':
            this->gCountryCode = CountryCode::Japan;
            break;
        case 'E':
            this->gCountryCode = CountryCode::NorthAmerica;
            break;
        case 'P':
            this->gCountryCode = CountryCode::Europe;
            break;
        default:
            this->gCountryCode = CountryCode::Unknown;
            break;
    }
    reader.Close();
}

const std::string &N64::Cartridge::GetGameTitle() {
    return this->gGameTitle;
}

N64::CountryCode N64::Cartridge::GetCountry() {
    return this->gCountryCode;
}

uint8_t N64::Cartridge::GetVersion() const {
    return this->gVersion;
}

std::string N64::Cartridge::GetHash() {
    return this->gHash;
}

std::string N64::Cartridge::GetCountryCode() {
    switch (this->gCountryCode) {
        case CountryCode::Japan:
            return "jp";
        case CountryCode::NorthAmerica:
            return "us";
        case CountryCode::Europe:
            return "eu";
        default:
            return "unk";
    }
}

uint32_t N64::Cartridge::GetCRC() {
    return this->gRomCRC;
}
