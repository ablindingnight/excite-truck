#ifndef GAME_VALUEBINDING_H
#define GAME_VALUEBINDING_H
// A tunable read from a .val table: its type, key name and the variable it fills (crushmaster, ringchal). One
// declaration for both units (port ODR work, 2026-11-16); named apart from inputbind.cpp's input Binding. Names
// unproven.
#include "game/common.h"

struct ValueBinding {
    s32 type;          // offset 0x0
    const char* name;  // offset 0x4
    void* ptr;         // offset 0x8
};

#endif
