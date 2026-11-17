#ifndef GAME_EVENT_H
#define GAME_EVENT_H

#include "game/common.h"

// An event-table entry (eve.cpp; ring reaches the current one through fn_8000AC38 for its .val node). Names are
// guesses.
struct Event {
    s32 index;        // offset 0x0
    s32 track;        // offset 0x4
    u32 type;         // offset 0x8
    s32 laps;         // offset 0xC
    s32 drivers;      // offset 0x10
    u32 terrain;      // offset 0x14
    s32 rounds;       // offset 0x18
    s32 purse;        // offset 0x1C
    ValueNode* node;  // offset 0x20
    u8 enabled;       // offset 0x24
};

#endif
