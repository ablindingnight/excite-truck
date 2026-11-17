#ifndef GAME_ROUND_H
#define GAME_ROUND_H
// Round half away from zero to int. Many units carried their own copy (with or without the int cast, 0.5f + x or
// x + 0.5f: the literal is generated first either way, so every spelling gives the same code); one definition for the
// port (aids audit 3, 2026-11-17: each unit converted only where code-identical).
#include "types.h"

/* Helper name unproven */
static inline int Round(f32 x) { return (int)(x < 0.0f ? x - 0.5f : 0.5f + x); }

#endif
