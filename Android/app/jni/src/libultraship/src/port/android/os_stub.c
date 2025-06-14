// os_stub.c -- Platform stub implementations for N64 OS functions

#include "os.h"

void osInvalDCache(void* addr, int32_t size) {
    // No-op for non-N64 targets
    (void)addr; (void)size;
}

void osInvalICache(void* addr, int32_t size) {
    (void)addr; (void)size;
}

void osWritebackDCache(void* addr, int32_t size) {
    (void)addr; (void)size;
}

void osWritebackDCacheAll(void) {
    // Nothing to do
}

void osCreateMesgQueue(OSMesgQueue* mq, OSMesg* msg, int count) {
    // No actual message queues on PC/Android; stub
    (void)mq; (void)msg; (void)count;
}

int osRecvMesg(OSMesgQueue* mq, OSMesg* msg, int flag) {
    // Always succeed or return "no message" (customize as needed)
    (void)mq; (void)msg; (void)flag;
    return 0;
}

int osSendMesg(OSMesgQueue* mq, OSMesg msg, int flag) {
    (void)mq; (void)msg; (void)flag;
    return 0;
}

// Add any other os* functions your build may complain about here, as empty stubs.