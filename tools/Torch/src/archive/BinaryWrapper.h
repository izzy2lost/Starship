#pragma once

#include <vector>
#include <string>
#include <mutex>

class BinaryWrapper {
public:
    BinaryWrapper() {}
    BinaryWrapper(const std::string& path);
    virtual ~BinaryWrapper() = default;

    virtual int32_t CreateArchive(void) = 0;
    virtual bool AddFile(const std::string& path, std::vector<char> data) = 0;
    virtual int32_t Close(void) = 0;
protected:
    std::mutex mMutex;
    std::string mPath;
};
