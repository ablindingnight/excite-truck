#ifndef GAME_NEARZERO_H
#define GAME_NEARZERO_H
// |x| <= FLT_EPSILON through the float fabs. Several units carried identical copies (betarace's out-of-line weak copy
// shows it is a plain, non-static inline); one definition for the port (aids audit 3, 2026-11-17: each unit converted
// only where code-identical).
#include "types.h"

extern f32 __float_epsilon[];  // FLT_EPSILON (MSL)

/* Helper name unproven */
inline int NearZero(f32 x) { return (f32)__fabs(x) <= __float_epsilon[0]; }

#endif
