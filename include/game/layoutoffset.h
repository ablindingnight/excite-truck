#ifndef GAME_LAYOUTOFFSET_H
#define GAME_LAYOUTOFFSET_H
// LayoutOffset: one declaration for the units that each carried an identical copy (ovlayout, replay). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"

// Pushes an offset onto the layout origin for the lifetime of the object.
struct LayoutOffset {
    LayoutOffset(f32 dx, f32 dy);
    ~LayoutOffset(void);
    f32 ox;
    f32 oy;
};

#endif
