#include "libultraship/libultraship.h"
#include "Context.h"
#include <string>
#include <filesystem>

extern "C" {

// Get the proper save file path using the Context system
static std::string GetSaveFilePath() {
    // Use the Context system to get the proper app directory path
    // This ensures consistency with where other files are stored
    std::string savePath = Ship::Context::GetPathRelativeToAppDirectory("default.sav");
    
    // Debug logging to help track down save issues
    SPDLOG_INFO("Save file path: {}", savePath);
    
    return savePath;
}

int32_t osEepromProbe(OSMesgQueue* mq) {
    return EEPROM_TYPE_4K;
}

int32_t osEepromLongRead(OSMesgQueue* mq, uint8_t address, uint8_t* buffer, int32_t length) {
    u8 content[512];
    s32 ret = -1;

    std::string savePath = GetSaveFilePath();
    SPDLOG_INFO("Attempting to read save file from: {}", savePath);
    
    FILE* fp = fopen(savePath.c_str(), "rb");
    if (fp == NULL) {
        SPDLOG_INFO("Save file not found (this is normal for first run): {}", savePath);
        return -1;
    }
    if (fread(content, 1, 512, fp) == 512) {
        memcpy(buffer, content + address * 8, length);
        ret = 0;
        SPDLOG_INFO("Successfully read save file");
    } else {
        SPDLOG_ERROR("Failed to read save file content");
    }
    fclose(fp);

    return ret;
}

int32_t osEepromRead(OSMesgQueue* mq, u8 address, u8* buffer) {
    return osEepromLongRead(mq, address, buffer, 8);
}

int32_t osEepromLongWrite(OSMesgQueue* mq, uint8_t address, uint8_t* buffer, int32_t length) {
    u8 content[512] = { 0 };
    if (address != 0 || length != 512) {
        osEepromLongRead(mq, 0, content, 512);
    }
    memcpy(content + address * 8, buffer, length);

    std::string savePath = GetSaveFilePath();
    SPDLOG_INFO("Attempting to write save file to: {}", savePath);
    
    FILE* fp = fopen(savePath.c_str(), "wb");
    if (fp == NULL) {
        SPDLOG_ERROR("Failed to open save file for writing: {}", savePath);
        return -1;
    }
    s32 ret = fwrite(content, 1, 512, fp) == 512 ? 0 : -1;
    fclose(fp);
    
    if (ret == 0) {
        SPDLOG_INFO("Successfully wrote save file");
    } else {
        SPDLOG_ERROR("Failed to write save file");
    }
    
    return ret;
}

int32_t osEepromWrite(OSMesgQueue* mq, uint8_t address, uint8_t* buffer) {
    return osEepromLongWrite(mq, address, buffer, 8);
}
}