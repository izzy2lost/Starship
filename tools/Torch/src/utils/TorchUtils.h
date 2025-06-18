#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <algorithm>
#include <filesystem>

namespace Torch {
template< typename T >
std::string to_hex(T number, const bool append0x = true) {
    std::stringstream stream;
    if(append0x) {
        stream << "0x";
    }
    stream << std::setfill ('0')
           << std::hex << number;

    auto format = stream.str();
    std::transform(format.begin(), format.end(), format.begin(), ::toupper);
    return format;
}

uint32_t translate(uint32_t offset);
std::vector<std::filesystem::directory_entry> getRecursiveEntries(const std::filesystem::path baseDir);

};