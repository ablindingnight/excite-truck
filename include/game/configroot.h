#ifndef GAME_CONFIGROOT_H
#define GAME_CONFIGROOT_H
// ConfigRoot: one declaration for the units that each carried an identical copy (crushmaster, ringchal). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"

struct ConfigRoot {
    u8 pad0[0x20];
    ValueNode* vals;  // offset 0x20
};

#endif
