#ifndef GAME_EASE_H
#define GAME_EASE_H
// Odd smoothstep: -2|x|^3 + 3x^2 with the sign of x, clamped to -1..1. Many units carried identical copies (Ease,
// Smooth; logomover.h's SmoothSign is the same body); one definition for the port (aids audit 3, 2026-11-17: each unit
// converted only where code-identical).
#include "types.h"

/* Helper name unproven */
static inline f32 Ease(f32 x) {
    if (x < -1.0f) {
        return -1.0f;
    } else if (x > 1.0f) {
        return 1.0f;
    } else if (x > 0.0f) {
        return x * (-2.0f * x * x) + 3.0f * x * x;
    } else {
        return x * (-2.0f * x * x) - 3.0f * x * x;
    }
}

#endif
