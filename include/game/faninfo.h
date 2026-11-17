#ifndef GAME_FANINFO_H
#define GAME_FANINFO_H
// One cell of the ground "fan" grids (faninfo.cpp; fn_802BE844 looks one up by position and size): the objects
// standing in it (xlobj's item list), the trees drawn in it (tree.cpp's TCell list) and its ground bits
// (0-5 materials present, 6-8 dominant material, 10-13 ?, 14-22 height, 23/24 xlobj flags). One declaration for
// every unit (port ODR work, 2026-11-16; tree's FanCell, groundvis/xlobj's GroundCell were views). Names unproven.
#include "game/common.h"

struct Item;   // xlobj.cpp: a placed object
struct TCell;  // tree.cpp: a tree draw cell

struct FanData {
    ~FanData(void) {
        items = 0;
        head = 0;
        bits = 0;
    }

    Item** items;  // offset 0x0 (the default cell points it at lbl_8055FAC8)
    TCell* head;   // offset 0x4
    u32 bits;      // offset 0x8
};

struct FanCell : FanData {
    void Clear(void);
};

#endif
