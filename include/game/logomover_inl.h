#ifndef GAME_LOGOMOVER_INL_H
#define GAME_LOGOMOVER_INL_H

#include "game/logomover.h"

// LogoMover's Enter/Exit/Update bodies. Retail defines them out of line in popwarn.cpp, right after
// fn_80248668 (popwarn defines LOGOMOVER_DEFINE before including this there; retail evpath only refers to them).
// Other units see inline copies (evpath calls LogoMover::Update directly).
#ifdef LOGOMOVER_DEFINE
#define LOGOMOVER_INLINE
#else
#define LOGOMOVER_INLINE inline
#endif
LOGOMOVER_INLINE void LogoMover::Enter(void) {
    m.active = 1;
    m.idx = fn_80221EA8((u8*)this);
}
LOGOMOVER_INLINE void LogoMover::Exit(void) { m.active = 0; }
// One frame of motion: shows/hides the menu item, applies a pending jump or move, eases toward the target.
LOGOMOVER_INLINE void LogoMover::Update(void) {
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

#undef LOGOMOVER_INLINE

#endif
