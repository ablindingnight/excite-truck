// Car damage (0x8018BFA0-0x8018FB18): the detachable body parts ("right hood hinge", ...), wheel loss
// ("cardmg: wheel %d is already missing from car %d") and the part break-away timers. Written against
// raw field offsets; the large routines and the initialiser are still stubs. Names are guesses apart
// from the strings.
#include "game/common.h"

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

// One entry of the damageable part table.
struct DamagePart {
    s32 kind;          // offset 0x0
    u8 data[0x3C];     // offset 0x4
};

extern DamagePart lbl_80332840[];

extern "C" {

void fn_8018BFA0(u8* p) {}  // stub (347)

void fn_8018C50C(u8* p) {}

void fn_8018C510(u8* p) {}

f32 PSVECDistance(const f32* a, const f32* b);
extern const f32 lbl_80561F70;  // large distance

// Index of the damage point nearest to pos (entry 0 is skipped).
int fn_8018C514(u8* p, const f32* pos) {
    f32 best = lbl_80561F70;
    int idx = 0;
    for (int i = 1; i < *(s32*)PTR(p, 0x26C); i++) {
        f32 d = PSVECDistance(pos, (f32*)(PTR(p, 0x26C) + i * 0x34 + 0x20));
        if (d < best) {
            best = d;
            idx = i;
        }
    }
    return idx;
}

void fn_8018C5A0(u8* p) {}  // stub (359)

void fn_8018CB3C(u8* p) {}  // stub (968)

void fn_8018DA5C(u8* p) {}  // stub (270)

void fn_8018DE94(u8* p) {}  // stub (295)

void fn_8018E330(u8* p) {}  // stub (47)

void fn_8018E3EC(u8* p) {}  // stub (949)

void fn_8018F2C0(u8* p) {}  // stub (158)

void fn_8018F538(u8* p) {}  // stub (112)

void fn_8018F6F8(u8* p) {
    DamagePart* t = lbl_80332840;
    u8* w = p + 0x152C;
    for (int i = 0; i < 17; i++) {
        if (U32(w, 0x0) && !S32(w, 0x7C) && (u32)(t->kind - 14) <= 2) {
            S32(w, 0x7C) = 4;
            U8(w, 0x78) = 0;
            F32(w, 0xC) = 0.0f;
            F32(w, 0x10) = F32(w, 0x14) / 0.75f;
        }
        t++;
        w += 0x80;
    }
}

void fn_8018F76C(u8* p) {}  // stub (146)

void fn_8018F9B4(u8* p) {}  // stub (70)

void fn_8018FACC(u8* p) {}  // stub (19)
}  // extern "C"
