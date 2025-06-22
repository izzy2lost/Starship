/**
 * Used to reproduce recordings made from real N64 hardware
 * to accurately reproduce Cutscenes at the correct speed.
 * These recordings adjust gVisPerFrame during runtime to produce
 * the same behaviour as the original game.
 */
#ifndef N64_RECORD_H
#define N64_RECORD_H

#include "global.h"

typedef struct Record {
    u8 vis;
    u16 frame;
} Record;

extern Record gCarrierCutsceneRecord[13];

void UpdateVisPerFrameFromRecording(Record* record, s32 maxFrames);

#endif
