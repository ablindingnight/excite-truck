// Physics camera (0x80270AE8-0x80273A88): the camera definitions file ("camera3.val") with its
// cam/radius/group/event/fov/shakey/rumble fields, camera region lookup ("find_best_region"), pit-mode
// checks and the angle/range helpers. Written against raw field offsets; the large routines and the
// static-array initialiser are still stubs. Names are guesses apart from the strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

// A position on a camera path: the segment and how far along it.
struct PathPos {
    u8* seg;  // offset 0x0
    f32 t;    // offset 0x4
};

extern "C" {
u8* fn_801E87A8(u8* p);
int strcasecmp(const char* a, const char* b);
u8* fn_80178C9C(void);
void fn_80136468(PathPos* out, u8* src);
void fn_80270EC4(u8* c, u32 id);
void fn_802712F8(const char* path);
void fn_80271624(u8* p);
}

void operator delete(void* p);

extern u8 lbl_8049A258[];
extern const char* lbl_80346B54[];

// One camera region record.
struct CamRegion {
    s32 active;      // offset 0x0
    u8 data[0x18];   // offset 0x4
};

extern CamRegion lbl_8049F9D8[];
extern f32 lbl_8055F540;
extern f32 lbl_8055F544;
extern s32 lbl_8055F54C;
extern s32 lbl_8055F554;
extern u8 lbl_8055F558;

extern "C" {

void fn_80270AE8(f32* p) {
    f32 z = 0.0f;
    p[0] = z;
    p[1] = z;
    p[3] = z;
    p[2] = z;
}

void fn_80270B00(const char* fmt, ...) {}

u32 fn_80270B50(u8* p) { return U32(fn_801E87A8(p), 0x4); }

void fn_80270B74(void) {
    for (int i = 0; i < 32; i++) {
        lbl_8049F9D8[i].active = 0;
    }
    lbl_8055F554 = 0;
}

void fn_80270C04(u8* p) {}  // stub (70)

int fn_80270D1C(int x) {
    switch (x) {
    case 0:
        return 1;
    case 1:
        return 2;
    default:
        return 0;
    }
}

s32 fn_80270D4C(u8* a, u8* b) {
    f32* pb;
    f32* pa;
    if (U8(b, 0x4)) {
        pb = (f32*)(PTR(b, 0x0) + 0xC);
    } else {
        pb = (f32*)(PTR(b, 0x0) + 0x4);
    }
    if (U8(a, 0x4)) {
        pa = (f32*)(PTR(a, 0x0) + 0xC);
    } else {
        pa = (f32*)(PTR(a, 0x0) + 0x4);
    }
    return (s32)(*pa - *pb);
}

void fn_80270DB0(u8* p) {}  // stub (69)

void fn_80270EC4(u8* c, u32 id) {}  // stub (119)

void fn_802710A0(u8* p) {}  // stub (150)

void fn_802712F8(const char* path) {}  // stub (157)

void fn_8027156C(void) { fn_802712F8("camera3.val"); }

void fn_80271578(void) {
    u32 keep;
    u8* c = fn_80178C9C();
    keep = U32(c, 0xC);
    fn_80270EC4(c, fn_80270B50(c));
    U32(c, 0xC) = keep;
}

void fn_802715C4(f32* out, u8* p) {
    f32 k = 0.017453292f;
    out[2] = k * F32(p, 0x14);
    out[0] = k * F32(p, 0xC);
    out[1] = k * F32(p, 0x10);
}

f32 fn_802715F0(u8* p) {
    f32 v = 0.017453292f * F32(p, 0x18);
    f32 x;
    if (lbl_8055F544 < v) {
        x = lbl_8055F544;
    } else {
        x = v;
    }
    if (x > lbl_8055F540) {
        return x;
    }
    return lbl_8055F540;
}

void fn_80271624(u8* p) {}  // stub (610)

int fn_80271FAC(const char* name) {
    for (int i = 0; i < 18; i++) {
        if (i != 11 && !strcasecmp(name, lbl_80346B54[i])) {
            return i;
        }
    }
    return 11;
}

void fn_80272028(u8* p) {}  // stub (203)

void fn_80272354(u8* p) {
    F32(p, 0x0) = 0.0f;
    F32(p, 0x4) = 0.0f;
    F32(p, 0x8) = 0.0f;
    F32(p, 0xC) = 10.0f;
    memset(p + 0x10, 0, 8);
    S32(p, 0x58) = 0;
    S32(p, 0x5C) = 2;
}

void fn_802723B4(u8* src, f32* out) {
    PathPos pos;
    fn_80136468(&pos, src);
    f32 t = pos.t;
    u8* q = pos.seg;
    out[0] = F32(q, 0x0) + t * (F32(PTR(q, 0x14), 0x0) - F32(q, 0x0));
    out[1] = F32(q, 0x4) + t * (F32(PTR(q, 0x14), 0x4) - F32(q, 0x4));
}

void fn_80272428(u8* p) {}  // stub (137)

void fn_8027264C(const char* name, u8* src, f32 x) {
    f32 v[2];
    fn_802723B4(src, v);
    fn_80270B00("  %s: %.3f  (%.3f, %.3f)", name, x, v[0], v[1]);
}

int fn_802726AC(f32 x, f32 a, f32 b) {
    if (a < b) {
        return a < x && x < b;
    }
    return a > x && x > b;
}

int fn_802726EC(f32 x, f32 a, f32 b) {
    int r = 0;
    if (x != a && x != b && fn_802726AC(x, a, b)) {
        r = 1;
    }
    return r;
}

void fn_80272738(u8* p) {}  // stub (315)

int fn_80272C24(f32 x, f32 a, f32 b, f32 m) {
    if (a == b) {
        return 1;
    }
    if (a > b) {
        if (a > x) {
            a -= m;
        } else {
            b += m;
        }
    }
    return x >= a && x <= b;
}

void fn_80272C74(u8* p) {}  // stub (86)

void fn_80272DCC(u8* p) {}  // stub (354)

void fn_80273354(void) {
    memset(lbl_8049A258, 0, 100);
    S32(lbl_8049A258, 0x1C) = 1;
    S32(lbl_8049A258, 0x18) = 0;
    S32(lbl_8049A258, 0x2C) = 1;
    S32(lbl_8049A258, 0x30) = 1;
    S32(lbl_8049A258, 0x40) = 2;
    S32(lbl_8049A258, 0x44) = 3;
    S32(lbl_8049A258, 0x54) = 5;
    S32(lbl_8049A258, 0x58) = 3;
    fn_8027156C();
}

void fn_802733C8(void) {
    fn_80271578();
    lbl_8055F54C = 0x12;
}

void fn_802733F0(u8* p) {}

void fn_802733F4(u8* p) {}  // stub (39)

void fn_80273490(u8* p) { fn_80271624(p); }

int fn_80273494(int x) {
    switch (x) {
    case 0xB:
        return 0;
    case 0xC:
        return 1;
    case 0xD:
        return 2;
    case 0xE:
        return 3;
    case 0xF:
        return 4;
    default:
        return 6;
    }
}

void fn_802734F0(u8* p) {}  // stub (65)

void fn_802735F4(int a, u8 b) {
    if (a == 0) {
        lbl_8055F558 = b;
    }
}

void fn_80273604(u8* p) {}  // stub (221)

u8* fn_80273978(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_802739B8(u8* p) {}  // stub (52)
}  // extern "C"
