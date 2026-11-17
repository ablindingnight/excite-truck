#ifndef GAME_LOADPOOL_H
#define GAME_LOADPOOL_H

#include "game/common.h"

struct LoadSlot;  // loadpool.cpp

// A pool of asynchronous load slots over one buffer (loadpool.cpp); modelpool owns one. Names are guesses.
class LoadPool {
public:
    ~LoadPool(void);

    u8* buf;          // offset 0x0
    s32 count;        // offset 0x4
    s32 slotSize;     // offset 0x8
    LoadSlot* slots;  // offset 0xC
};

#endif
