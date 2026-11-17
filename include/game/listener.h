#ifndef GAME_LISTENER_H
#define GAME_LISTENER_H
// Listener: one declaration for the units that each carried an identical copy (sound, soundmgr). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"
#include "game/valvec3.h"

struct Listener {
    s32 id;          // offset 0x0
    ValMatrix* mtx;  // offset 0x4
    ValVec3* vel;    // offset 0x8
};

#endif
