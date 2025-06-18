#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace N64 {
enum class CountryCode {
    Japan,
    NorthAmerica,
    Europe,
    Unknown
};

class Cartridge {
public:
    explicit Cartridge(const std::vector<uint8_t>& romData)
      : gRomData(romData), gCountryCode(CountryCode::Unknown), gVersion(0), gGameTitle("Unknown"), gRomCRC(0) {
  }
  void Initialize();
    const std::string& GetGameTitle();
    std::string GetCountryCode();
    CountryCode GetCountry();
    uint8_t GetVersion() const;
    std::string GetHash();
    uint32_t GetCRC();
private:
    std::vector<uint8_t> gRomData;
    CountryCode gCountryCode;
    uint8_t gVersion;
    std::string gGameTitle;
    std::string gHash;
    uint32_t gRomCRC;
};
}