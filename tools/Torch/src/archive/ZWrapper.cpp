#include "ZWrapper.h"
#include <filesystem>
#include <iostream>
#include <fstream>

#include "spdlog/spdlog.h"
#include <Companion.h>
#include <miniz/zip_file.hpp>

namespace fs = std::filesystem;

ZWrapper::ZWrapper(const std::string& path) {
    this->mPath = path;
    this->mZip = new miniz_cpp::zip_file;
}

int32_t ZWrapper::CreateArchive() {
    SPDLOG_INFO("Loaded ZIP (O2R) archive: {}", mPath.c_str());
    return 0;
}

bool ZWrapper::AddFile(const std::string& path, std::vector<char> data) {
    char* fileData = data.data();
    size_t fileSize = data.size();

    if(Companion::Instance != nullptr && Companion::Instance->IsDebug()){
        SPDLOG_INFO("Creating debug file: debug/{}", path);
        std::string dpath = "debug/" + path;
        if(!fs::exists(fs::path(dpath).parent_path())){
            fs::create_directories(fs::path(dpath).parent_path());
        }
        std::ofstream stream(dpath, std::ios::binary);
        stream.write(fileData, fileSize);
        stream.close();
    }

    this->mZip->writebytes(path, data);
    return true;
}

int32_t ZWrapper::Close(void) {
    this->mZip->save(this->mPath);
    return 0;
}
