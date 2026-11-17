#ifndef GAME_GXTEX_H
#define GAME_GXTEX_H

#include "game/common.h"
#include <revolution/GX/GXTypes.h>
#include <revolution/GX/GXTexture.h>

// A loaded GX texture slot (drtex.cpp: texture object(s), palettes, data and heap bookkeeping). gndtex binds them
// for the ground layers and reads alphaRef for its alpha compare. Names are guesses unless noted.
struct GXTex {
    char name[32];      // offset 0x0
    u8 used;            // offset 0x20
    GXTexObj obj;       // offset 0x24
    u8 pad44[0x20];
    GXTlutObj tlut;     // offset 0x64
    u16* lut;           // offset 0x70
    u32 lutSize;        // offset 0x74
    GXTexObj obj2;      // offset 0x78
    u8 pad98[0x20];
    GXTlutObj tlut2;    // offset 0xB8
    u16* lut2;          // offset 0xC4
    u32 lut2Size;       // offset 0xC8
    u8 hasAlpha;        // offset 0xCC
    void* data;         // offset 0xD0
    u32 dataSize;       // offset 0xD4
    u32* srcPal;        // offset 0xD8
    u32 zhint;          // offset 0xDC
    u32 flags;          // offset 0xE0
    u8 alphaRef;        // offset 0xE4: alpha-compare reference (gndtex)
    void* heap;         // offset 0xE8
    u32 lastFrame;      // offset 0xEC
    u32 uses;           // offset 0xF0
};

#endif
