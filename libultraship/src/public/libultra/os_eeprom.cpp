#include "libultraship/libultraship.h"
#include <string>

#ifdef __ANDROID__
#include <SDL2/SDL.h>
#endif

extern "C" {

// Get the proper save file path for the current platform
static std::string GetSaveFilePath() {
#ifdef __ANDROID__
    // On Android, use the internal storage path
    const char* internalPath = SDL_AndroidGetInternalStoragePath();
    if (internalPath) {
        return std::string(internalPath) + "/default.sav";
    }
    // Fallback to current directory if SDL path fails
    return "default.sav";
#else
    // On other platforms, use current directory
    return "default.sav";
#endif
}

int32_t osEepromProbe(OSMesgQueue* mq) {
    return EEPROM_TYPE_4K;
}

int32_t osEepromLongRead(OSMesgQueue* mq, uint8_t address, uint8_t* buffer, int32_t length) {
    u8 content[512];
    s32 ret = -1;

    std::string savePath = GetSaveFilePath();
    FILE* fp = fopen(savePath.c_str(), "rb");
    if (fp == NULL) {
        return -1;
    }
    if (fread(content, 1, 512, fp) == 512) {
        memcpy(buffer, content + address * 8, length);
        ret = 0;
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
    FILE* fp = fopen(savePath.c_str(), "wb");
    if (fp == NULL) {
        return -1;
    }
    s32 ret = fwrite(content, 1, 512, fp) == 512 ? 0 : -1;
    fclose(fp);
    return ret;
}

int32_t osEepromWrite(OSMesgQueue* mq, uint8_t address, uint8_t* buffer) {
    return osEepromLongWrite(mq, address, buffer, 8);
}
}