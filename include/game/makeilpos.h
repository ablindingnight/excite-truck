#ifndef GAME_MAKEILPOS_H
#define GAME_MAKEILPOS_H
// The packed (segment << 16 | fraction) word that many units store in the two statics after their ModuleTag
// (MakeILPos(0x7F00, 0), MakeILPos(0xFFFF, 0)). Seventeen units carried identical copies; one definition for the port
// (aids audit 3, 2026-11-17: each unit converted only where code-identical).
#include "types.h"

/* Helper name unproven */
static inline u32 MakeILPos(u32 seg, u32 frac) { return (seg << 16) | frac; }

#endif
