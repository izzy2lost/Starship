#include "SWrapper.h"
#include <filesystem>
#include <iostream>
#include <fstream>

#include "spdlog/spdlog.h"
#include <Companion.h>

namespace fs = std::filesystem;

SWrapper::SWrapper(const std::string& path) {
    mPath = path;
}

int32_t SWrapper::CreateArchive() {
#ifndef USE_STORMLIB
    throw std::runtime_error("StormLib is not enabled. Cannot create archive");
#else
    if(fs::exists(mPath)) {
        fs::remove(mPath);
    }

    if(!SFileCreateArchive(mPath.c_str(), MPQ_CREATE_LISTFILE | MPQ_CREATE_ATTRIBUTES | MPQ_CREATE_ARCHIVE_V2, 16 * 1024, &this->hMpq)){
        SPDLOG_ERROR("Failed to create archive {} with error code {}", mPath, GetLastError());
        return -1;
    }

    return 0;
#endif
}

bool SWrapper::AddFile(const std::string& path, std::vector<char> data) {
#ifndef USE_STORMLIB
    throw std::runtime_error("StormLib is not enabled. Cannot create file");
#else
    if(Companion::Instance != nullptr && Companion::Instance->IsDebug()){
        SPDLOG_INFO("Creating debug file: debug/{}", path);
        std::string dpath = "debug/" + path;
        if(!fs::exists(fs::path(dpath).parent_path())){
            fs::create_directories(fs::path(dpath).parent_path());
        }
        std::ofstream stream(dpath, std::ios::binary);
        stream.write((char*)data.data(), data.size());
        stream.close();
    }

    HANDLE hFile;
#ifdef _WIN32
    SYSTEMTIME sysTime;
    GetSystemTime(&sysTime);
    FILETIME t;
    SystemTimeToFileTime(&sysTime, &t);
    ULONGLONG theTime = static_cast<uint64_t>(t.dwHighDateTime) << (sizeof(t.dwHighDateTime) * 8) | t.dwLowDateTime;
#else
    time_t theTime;
    time(&theTime);
#endif

    char* raw = data.data();
    size_t size = data.size();

    if(size == 0){
        SPDLOG_ERROR("File at path {} is empty", path);
        return false;
    }

    if(size >> 32){
        throw std::runtime_error("File at path " + path + " is too large with size " + std::to_string(size));
    }

    if(!SFileCreateFile(this->hMpq, path.c_str(), theTime, size, 0, MPQ_FILE_COMPRESS, &hFile)){
        return false;
    }

    if(!SFileWriteFile(hFile, (void*) raw, size, MPQ_COMPRESSION_ZLIB)){
        throw std::runtime_error("Failed to write file at path " + path + " with error " + std::to_string(GetLastError()));
    }

    if(!SFileCloseFile(hFile)){
        throw std::runtime_error("Failed to close file at path " + path + " with error " + std::to_string(GetLastError()));
    }

    return true;
#endif
}

int32_t SWrapper::Close(void) {
#ifndef USE_STORMLIB
    throw std::runtime_error("StormLib is not enabled. Cannot close archive");
#else
    if(this->hMpq == nullptr) {
        SPDLOG_ERROR("Archive already closed");
        return -1;
    }
    if (SFileCloseArchive(this->hMpq)) {
        SPDLOG_ERROR("Error closing archive");
        return -1;
    }
    return 0;
#endif
}
