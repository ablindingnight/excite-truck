#ifndef GAME_SOUNDSAMPLE_H
#define GAME_SOUNDSAMPLE_H
// SoundSample: one declaration for the units that each carried an identical copy (sounddash, unk_8020A6AC). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"

struct SoundSample {
    char name[0x20];  // offset 0x0
    f32 volume;       // offset 0x20
};

#endif
