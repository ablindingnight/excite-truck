#ifndef GAME_SETTINGS_H
#define GAME_SETTINGS_H
// Settings: one declaration for the units that each carried an identical copy (betarace, trackobj). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"

struct Settings {
    u8 pad0[0x20];
    ValueNode* vals;  // offset 0x20
};

#endif
