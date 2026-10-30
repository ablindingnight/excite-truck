#ifndef GAME_LOGOMOVER_H
#define GAME_LOGOMOVER_H

#include "game/menustate.h"

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
    virtual void Update(void);

    LogoMotion m;  // offset 0x18
};

#endif
