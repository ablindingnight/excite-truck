#ifndef GAME_ILPOS_H
#define GAME_ILPOS_H
// A position on a centre line: the segment's start point, the fraction along it and a mode (idealline.cpp).
// One declaration for every unit (port ODR work, 2026-11-16). Names unproven.
#include "game/centerline.h"

static inline f32 ILerpT(f32 t, f32 a, f32 b) { return a + t * (b - a); }

struct ILPos {
    ILPos(void) {
        seg = 0;
        mode = 10000;
        t = 0.0f;
    }
    ILPos(IdealPoint* s, f32 f, s32 m) {
        seg = s;
        t = f;
        mode = m;
    }
    // Point and heading on the line at this position (method names unproven).
    void Pos(V2* o) const {
        o->x = ILerpT(t, seg->pos.x, seg->next->pos.x);
        o->y = ILerpT(t, seg->pos.y, seg->next->pos.y);
    }
    void Dir(V2* o) const {
        o->x = ILerpT(t, seg->dir.x, seg->next->dir.x);
        o->y = ILerpT(t, seg->dir.y, seg->next->dir.y);
    }
    // Distance along the line at this position (zone).
    f32 Dist(void) const { return seg->dist + seg->len * t; }
    IdealPoint* seg;  // offset 0x0
    f32 t;            // offset 0x4
    s32 mode;         // offset 0x8
};

#endif
