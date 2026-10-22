// Named cameras (0x80276068-0x8027820C): the named camera sets ("namedcam", "rddefault", "raceday",
// "house") and the timed camera transitions ("physcam: Clock looped during camera transition!!").
// Written against raw field offsets; the large routines and the initialiser are still stubs. Names
// are guesses apart from the strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

extern u8 lbl_8049FD58[];
extern f32 lbl_8055E3C8;
extern s32 lbl_8055F598;
extern s32 lbl_8055F59C;
extern s32 lbl_8055F5A0;

extern "C" {

int fn_80276068(int mode) {
    switch (mode) {
    case 4:
    case 5:
    case 6:
    case 9:
    case 23:
        return 1;
    }
    return 0;
}

void fn_80276098(u8* p) {}  // stub (309)

void fn_8027656C(u8* p) {}  // stub (310)

void fn_80276A44(u8* p) {}  // stub (34)

void fn_80276ACC(u8* p) {}  // stub (48)

void fn_80276B8C(void) {
    u8* e = lbl_8049FD58;
    for (int i = 0; i < 4; i++) {
        memset(e, 0, 0x50);
        e += 0x50;
    }
}

void fn_80276BE4(u8* p) {}  // stub (51)

void fn_80276CB0(u8* p) {}  // stub (35)

void fn_80276D3C(u8* p) {}  // stub (35)

void fn_80276DC8(u8* p) {}  // stub (591)

f32 fn_80277704(int i) {
    u8* e = lbl_8049FD58 + i * 0x50;
    if (e[0]) {
        return (lbl_8055E3C8 - F32(e, 0x48)) / F32(e, 0x44);
    }
    return -1.0f;
}

void fn_80277740(void) {
    lbl_8055F5A0 = 0;
    lbl_8055F598 = 0;
    lbl_8055F59C = 0;
}

void fn_80277754(u8* p) {}  // stub (53)

void fn_80277828(u8* p) {}  // stub (78)

void fn_80277960(u8* p) {}  // stub (45)

void fn_80277A14(u8* p) {}  // stub (348)

void fn_80277F84(u8* p) {}  // stub (121)

void fn_80278168(u8* p) {}  // stub (41)
}  // extern "C"
