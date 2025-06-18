#pragma once

#include <vector>
#include <string>
#include "BinaryWrapper.h"
#ifdef USE_STORMLIB
#include <StormLib/src/StormLib.h>
#endif

class SWrapper : public BinaryWrapper {
public:
    explicit SWrapper(const std::string& path);

    int32_t CreateArchive(void) override;
    bool AddFile(const std::string& path, std::vector<char> data) override;
    int32_t Close(void) override;
#ifdef USE_STORMLIB
private:
    HANDLE hMpq{};
#endif
};
