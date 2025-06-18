#pragma once

#include "../factories/BaseFactory.h"

class RawBuffer : public IParsedData {
public:
    std::vector<uint8_t> mBuffer;

    RawBuffer(uint8_t* data, size_t size) : mBuffer(data, data + size) {}
    explicit RawBuffer(std::vector<uint8_t>& buffer) : mBuffer(std::move(buffer)) {}
    RawBuffer(std::vector<uint8_t>::iterator begin, std::vector<uint8_t>::iterator end) : mBuffer(begin, end) {}
};