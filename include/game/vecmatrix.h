#ifndef GAME_VECMATRIX_H
#define GAME_VECMATRIX_H

#include <revolution/MTX/mtxtypes.h>

// The 0x30 pose spelled with the SDK's Vec rows: three rotation rows, then the origin. It is common.h's
// PlainValMatrix with the SDK vector type, for units that hand the rows straight to the PSVEC/PSMTX functions (no
// casts). Same member names as ValMatrix, so a later merge is a type swap. No constructor (copies stay plain).
// One declaration for every unit (vector unification, 2026-11-16). /* Helper name unproven */
struct VecMatrix {
    Vec basis[3];  // offset 0x0
    Vec origin;    // offset 0x24
};

#endif
