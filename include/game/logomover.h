#ifndef GAME_LOGOMOVER_H
#define GAME_LOGOMOVER_H

#include "game/menustate.h"

extern "C" {
void fn_80214A9C(int i);
void fn_80214AF0(int i);
void fn_80214BE4(int i, int dx, int dy);
f32 fn_80214D48(void);
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
    LogoMotion(f32 time) {
        rate = 1.0f / time;
        ease = 2;
        t = 1.0f;
        Reset(2);
    }
    ~LogoMotion(void) { Reset(1); }
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
    LogoMover(f32 time) : m(time) {}
    virtual ~LogoMover(void) {}
    virtual void Enter(void);
    virtual void Exit(void);
    // One frame of motion: shows/hides the menu item, applies a pending jump or move, eases toward the target.
    virtual void Update(void) {
        if (m.vis != m.lastVis) {
            if (m.vis == 0) {
                fn_80214AF0(m.idx);
            } else {
                fn_80214A9C(m.idx);
            }
            m.lastVis = m.vis;
        }
        if (m.jumpOn) {
            m.fromX = m.toX = m.jumpX;
            m.fromY = m.toY = m.jumpY;
            fn_80214BE4(m.idx, m.jumpX - x, m.jumpY - y);
            m.jumpOn = 0;
            m.t = 1.1f;
        }
        if (m.moveOn) {
            m.fromX = x;
            m.fromY = y;
            m.toX = m.moveX;
            m.toY = m.moveY;
            m.t = 0.0f;
            m.moveOn = 0;
        }
        m.t += m.rate * fn_80214D48();
        f32 k;
        switch (m.ease) {
        case 1: {
            f32 v = m.t < 1.0f ? m.t : 1.0f;
            k = v * v;
            break;
        }
        case 2: {
            f32 v = m.t < 1.0f ? m.t : 1.0f;
            k = SmoothSign(v);
            break;
        }
        default:
            k = m.t < 1.0f ? m.t : 1.0f;
            break;
        }
        int nx = (int)LerpF((f32)m.fromX, (f32)m.toX, k);
        int ny = (int)LerpF((f32)m.fromY, (f32)m.toY, k);
        fn_80214BE4(m.idx, nx - x, ny - y);
        if (m.f44 && m.Done()) {
            m.f44 = 0;
            m.vis = 1;
        }
    }

    LogoMotion m;  // offset 0x18
};

#endif
