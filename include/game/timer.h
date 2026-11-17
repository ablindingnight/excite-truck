#ifndef GAME_TIMER_H
#define GAME_TIMER_H

#include "game/common.h"

// A one-shot timer on the game clock (stdoverlay.cpp defines the constructor, Now and Progress; retail keeps them as
// weak copies): start it, ask whether it is still running or how far along it is. crushmaster, stdoverlay and zone
// use it. Names are guesses.
class Timer {
public:
    Timer(f32 len);
    f32 Now(void) const;
    f32 Progress(void) const;
    int Running(void) const {
        f32 t = Now() - start;
        return t >= 0.0f && t < len;
    }
    void Start(void) { start = Now(); }
    // zone's name for Running (retail zone emits it out of line as Active__5TimerCFv).
    int Active(void) const {
        f32 t = Now() - start;
        return t >= 0.0f && t < len;
    }
    void Clear(void) { start = -999.0f; }

    f32 len;    // offset 0x0
    f32 start;  // offset 0x4
};

#endif
