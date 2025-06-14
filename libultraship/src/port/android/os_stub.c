#include "os.h"

int GetNumAudioChannels(void) {
    // Stub: Always return 2 (stereo) or 1 as default
    return 2;
}

s32 osAiSetFrequency(u32 frequency) {
    // Stub: pretend to set frequency, return a fake previous value
    (void)frequency;
    return 32000; // e.g. N64 default
}

void osWritebackDCacheAll(void) {
    // No cache to write back
}

int osSendMesg(OSMesgQueue* mq, OSMesg msg, int flag) {
    (void)mq; (void)msg; (void)flag;
    return 0;
}

void osInvalDCache(void* addr, s32 size) {
    (void)addr; (void)size;
}

int osRecvMesg(OSMesgQueue* mq, OSMesg* msg, int flag) {
    (void)mq; (void)msg; (void)flag;
    return 0;
}

void osCreateMesgQueue(OSMesgQueue* mq, OSMesg* msg, int count) {
    (void)mq; (void)msg; (void)count;
}
