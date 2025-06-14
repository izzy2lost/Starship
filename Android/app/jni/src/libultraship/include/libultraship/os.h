// os.h -- N64/ultra64 OS function stubs for PC/Android ports

#ifndef OS_H
#define OS_H

#include <stdint.h>

// Message Queue and Messaging
typedef struct {
    int dummy; // Replace with actual structure if needed
} OSMesgQueue;

typedef void* OSMesg;

void osCreateMesgQueue(OSMesgQueue* mq, OSMesg* msg, int count);
int osRecvMesg(OSMesgQueue* mq, OSMesg* msg, int flag);
int osSendMesg(OSMesgQueue* mq, OSMesg msg, int flag);

// Cache operations
void osInvalDCache(void* addr, int32_t size);
void osInvalICache(void* addr, int32_t size);
void osWritebackDCache(void* addr, int32_t size);
void osWritebackDCacheAll(void);

// Types used by audio (add more as needed)
typedef int32_t s32;
typedef uint32_t u32;
typedef uint8_t u8;
typedef int8_t s8;

// Macros (you might have these already elsewhere)
#define OS_MESG_NOBLOCK 0
#define OS_MESG_BLOCK   1

#endif // OS_H
