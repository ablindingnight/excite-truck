#ifndef GAME_DROPSOURCE_H
#define GAME_DROPSOURCE_H

#include "game/common.h"

// One rectangular droplet emitter of a source (0x30).
struct Droplet {
    ValVec3 n;       // offset 0x0
    ValVec3 origin;  // offset 0xC
    ValVec3 sv;      // offset 0x18
    ValVec3 tv;      // offset 0x24
};

struct DropSource {
    DropSource(void);
    Droplet drops[4];  // offset 0x0
    s32 count;         // offset 0xC0
};

#endif
