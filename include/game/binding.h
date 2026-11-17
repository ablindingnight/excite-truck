#ifndef GAME_BINDING_H
#define GAME_BINDING_H
// An input binding (inputbind.cpp; widgets.cpp edits them): the bound control's type and its button/key code, and a
// latch. One declaration for both units (port ODR work, 2026-11-16). Names unproven.
#include "game/common.h"

// One control: kind (1 key, 2 stick axis, 3 button, 4 direction bit, 5 mouse axis, 6 mouse button), the
// pad it reads and the control index.
struct Binding {
    s32 type;  // offset 0x0
    s32 pad;   // offset 0x4
    union {
        s32 code;
        u8 key;
    };           // offset 0x8
    u8 latched;  // offset 0xC
};

#endif
