#ifndef GAME_SOLGROUP_H
#define GAME_SOLGROUP_H

#include "game/common.h"
#include "game/shapes.h"

// A quadtree node over a group's item indices.
struct QuadNode {
    u16 first;  // offset 0x0
    u16 end;    // offset 0x2
    u16 child;  // offset 0x4
    u16 pad6;
};

// A group of static world objects (track.sol, loaded by fn_801A4548, or a game-supplied one): the objects
// (shapes.h StaticCollObject; b40 = active), a quadtree index over them and the function that frees the group.
// physcol collides against it, aiobst steers around it. Names are guesses.
struct SolGroup {
    s32 count;                     // offset 0x0
    StaticCollObject** items;      // offset 0x4
    u32 unk8;                      // offset 0x8
    u16* index;                    // offset 0xC
    QuadNode* nodes;               // offset 0x10
    void (*release)(SolGroup* g);  // offset 0x14
};

#endif
