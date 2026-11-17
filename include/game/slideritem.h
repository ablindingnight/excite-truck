#ifndef GAME_SLIDERITEM_H
#define GAME_SLIDERITEM_H
// MenuRange + SliderItem: one declaration for the units that each carried an identical copy (opsound, uioptions).
// Port ODR work, 2026-11-17. Included where MenuRange was (MenuItem's inline range constructor uses it before
// SliderItem). Names unproven.
#include "game/common.h"

struct MenuRange {
    MenuRange(void) {}
    MenuRange(f32 a, f32 b) {
        hi = b;
        lo = a;
    }
    f32 lo, hi;
};

// A slider: the range is stored in the item's argument words.
struct SliderItem {
    SliderItem(const char* n, f32* v, MenuRange r) {
        name = n;
        type = 9;
        data = v;
        range = r;
    }
    const char* name;
    int type;
    const void* data;
    MenuRange range;
};

#endif
