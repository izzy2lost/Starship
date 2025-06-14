#ifndef OS_H
#define OS_H

#include <stdint.h>

typedef int32_t s32;
typedef uint32_t u32;
typedef uint8_t u8;
typedef int8_t s8;

typedef struct {
    int dummy; // stub
} OSMesgQueue;

typedef void* OSMesg;

// Function prototypes
int GetNumAudioChannels(void);                   // Add this if the game expects it
s32 osAiSetFrequency(u32 frequency);
void osWritebackDCacheAll(void);
int osSendMesg(OSMesgQueue* mq, OSMesg msg, int flag);
void osInvalDCache(void* addr, s32 size);
int osRecvMesg(OSMesgQueue* mq, OSMesg* msg, int flag);
void osCreateMesgQueue(OSMesgQueue* mq, OSMesg* msg, int count);

// Macros
#define OS_MESG_NOBLOCK 0
#define OS_MESG_BLOCK   1

#endif // OS_H
