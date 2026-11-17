#ifndef GAME_CARLIGHTS_H
#define GAME_CARLIGHTS_H
// The car light sets (carlights.cpp; the constructor sits in stub12.cpp). Names are guesses
// (no retail symbols).
#include "game/common.h"
#include "game/vecmatrix.h"

// One light glow of a car: its frame, offset, size, colour and type.
struct CarLight {
    const VecMatrix* frame;  // offset 0x0
    Vec off;                  // offset 0x4
    f32 h;                    // offset 0x10
    f32 w;                    // offset 0x14
    u8 r;                     // offset 0x18
    u8 g;                     // offset 0x19
    u8 b;                     // offset 0x1A
    s32 type;                 // offset 0x1C
};

class LightSet {
public:
    LightSet(void);
    CarLight lights[12];  // offset 0x0
    s32 count;            // offset 0x180
};

#endif
