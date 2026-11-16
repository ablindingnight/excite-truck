#ifndef GAME_LOGOMOVER_H
#define GAME_LOGOMOVER_H

#include "game/menustate.h"

extern "C" {
void fn_80214A9C(int i);
void fn_80214AF0(int i);
void fn_80214BE4(int i, int dx, int dy);
f32 fn_80214D48(void);
s32 fn_80221EA8(u8* item);  // the menu item index of a widget
}

// Odd smoothstep: -1 below -1, 1 above 1.
static inline f32 SmoothSign(f32 v) {
    if (v < -1.0f) {
        return -1.0f;
    }
    if (v > 1.0f) {
        return 1.0f;
    }
    if (v > 0.0f) {
        return v * (-2.0f * v * v) + 3.0f * v * v;
    }
    return v * (-2.0f * v * v) - 3.0f * v * v;
}

static inline f32 LerpF(f32 a, f32 b, f32 k) { return a + k * (b - a); }

// A LogoMover's motion state (from 0x18): eases from (fromX, fromY) to (toX, toY).
struct LogoMotion {
    LogoMotion(f32 time, s32 easing = 2) {
        rate = 1.0f / time;
        ease = easing;
        t = 1.0f;
        Reset(2);
    }
    bool Done(void) const { return t >= 1.0f; }
    void Reset(s32 n) {
        active = 0;
        jumpOn = 0;
        moveOn = 0;
        lastVis = n;
        vis = 0;
        f44 = 0;
    }

    u8 active;     // offset 0x18
    s32 idx;       // offset 0x1C: this item's index in the menu
    s32 vis;       // offset 0x20
    s32 lastVis;   // offset 0x24
    s32 fromX;     // offset 0x28
    s32 fromY;     // offset 0x2C
    s32 toX;       // offset 0x30
    s32 toY;       // offset 0x34
    f32 rate;      // offset 0x38
    f32 t;         // offset 0x3C
    s32 ease;      // offset 0x40: 1 = quadratic, 2 = smooth, else linear
    u8 f44;        // offset 0x44: show when the move ends
    s32 jumpX;     // offset 0x48
    s32 jumpY;     // offset 0x4C
    u8 jumpOn;     // offset 0x50
    s32 moveX;     // offset 0x54
    s32 moveY;     // offset 0x58
    u8 moveOn;     // offset 0x5C
};

// A menu widget that eases into place over `time` seconds (0x60 bytes): the thank-you screen's logo
// (popwarn.cpp), the event path's cup markers (evpath.cpp).
class LogoMover : public MenuState {
public:
    LogoMover(f32 time, s32 easing = 2) : m(time, easing) {}
    // Leaves the item hidden (written out: a call here would make the dtor store the vptr first).
    virtual ~LogoMover(void) {
        m.active = 0;
        m.jumpOn = 0;
        m.moveOn = 0;
        m.lastVis = 1;
        m.vis = 0;
        m.f44 = 0;
    }
    // Enter/Exit/Update are defined in logomover_inl.h (out of line in popwarn.cpp, as in retail).
    virtual void Enter(void);
    virtual void Exit(void);
    virtual void Update(void);

    LogoMotion m;  // offset 0x18
};

#endif
