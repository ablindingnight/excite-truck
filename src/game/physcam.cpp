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
    u8* cam;         // offset 0x0, null when unused
    f32 lo1;         // offset 0x4, first heading window
    f32 hi1;         // offset 0x8
    f32 lo2;         // offset 0xC, second heading window
    f32 hi2;         // offset 0x10
    u8 dual;         // offset 0x14, second window in use
    u8 pad15[0x7];
};

extern CamRegion lbl_8049F9D8[];
extern f32 lbl_8055F540;
extern f32 lbl_8055F544;
extern s32 lbl_8055F54C;
extern s32 lbl_8055F554;
extern u8 lbl_8055F558;

// A camera cycle: how many views it has and which one is current.
struct CamCycle {
    u8 pad0[0x4];   // offset 0x0
    s32 first;      // offset 0x4, index of the first view in lbl_80346BA0
    s32 count;      // offset 0x8
    s32 cur;        // offset 0xC
    u8 pad10[0x4];  // offset 0x10
};

extern "C" int fn_80273494(int x);

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
        lbl_8049F9D8[i].cam = 0;
    }
    lbl_8055F554 = 0;
}

extern char lbl_80346B00[];  // this unit's string pool ("TV Set 1", ...)
extern s32 lbl_8055F550;
extern u8 lbl_8055F564;
extern u8 lbl_8049EDC8[];  // camera records, 0x60 each
void fn_80272738(CamRegion* r, u8* car);

// Collects the camera regions usable for this event; falls back to region 0.
void fn_80270C04(u8* car, u32 flags) {
    char* pool = lbl_80346B00;
    fn_80270B74();
    if (lbl_8055F550) {
        u8* cam;
        u8 check = (flags >> 8) & 1;
        lbl_8055F564 = check;
        fn_80270B00(pool + 0x1E0, lbl_8055F550);  // "Preparing regions (%d)"
        cam = lbl_8049EDC8;
        for (int i = 0; i < lbl_8055F550; i++) {
            fn_80270B00(pool + 0x1F8, i);  // "  Region %d:"
            fn_80270B00(pool + 0x208);     // "  ...is ok for event type"
            CamRegion* r = &lbl_8049F9D8[lbl_8055F554];
            r->cam = cam;
            if (check) {
                fn_80272738(r, car);
            }
            lbl_8055F554++;
            cam += 0x60;
        }
        if (!lbl_8055F554) {
            fn_80270B00(pool + 0x224);  // "Found no regions!  Using only region 0"
            lbl_8049F9D8[0].cam = lbl_8049EDC8;
            if (check) {
                fn_80272738(&lbl_8049F9D8[0], car);
            }
            lbl_8055F554 = 1;
        }
    }
}

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

s32 fn_80270D4C(u8* a, const u8* b) {
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

// A view into a camera track's angle window; `alt` selects the second window.
struct AngleRef {
    u8* track;  // offset 0x0
    u8 alt;     // offset 0x4
};

static inline f32* AngleLo(AngleRef* r) { return (f32*)(r->alt ? r->track + 0xC : r->track + 0x4); }
static inline f32* AngleHi(AngleRef* r) { return (f32*)(r->alt ? r->track + 0x10 : r->track + 0x8); }

extern const f32 lbl_80563F40;  // 0.5

// Meets two adjacent windows halfway: a's end and b's start become their midpoint (mod wrap).
void fn_80270DB0(AngleRef* a, AngleRef* b, f32 wrap) {
    f32 hi = *AngleHi(a);
    f32 lo = *AngleLo(b);
    if (hi < *AngleLo(a)) {
        hi += wrap;
    }
    if (lo < *AngleLo(a)) {
        lo += wrap;
    }
    f32 v = *AngleLo(a);
    f32 mid = lbl_80563F40 * (hi + lo);
    v = v > mid ? v : mid;
    if (v >= wrap) {
        v -= wrap;
    }
    *AngleHi(a) = v;
    *AngleLo(b) = v;
}

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

void fn_80272738(CamRegion* r, u8* car) {}  // stub (315)

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

class CarCtl {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual u8* Track(void);  // 0x18
};

f32 PSVECDistance(const u8* a, const u8* b);
extern f32 lbl_8055F568;        // car heading
extern const f32 lbl_80563F48;  // 0.0
extern const f32 lbl_80563F50;  // 1.0

// How strongly `r` applies to `car` (0..1 into *weight); returns whether the region is usable.
int fn_80272C74(CamRegion* r, u8* car, f32* weight, int type) {
    *weight = lbl_80563F48;
    if (!S32(r->cam, 0x58)) {
        return 0;
    }
    if (type != -1 && type != S32(r->cam, 0x5C)) {
        return 0;
    }
    int ok = 1;
    if (lbl_8055F564) {
        f32 wrap = F32(PTR(((CarCtl*)PTR(car, 0x270))->Track(), 0x2C), 0x8);
        if (fn_80272C24(lbl_8055F568, r->lo1, r->hi1, wrap) ||
            (r->dual && fn_80272C24(lbl_8055F568, r->lo2, r->hi2, wrap))) {
            *weight = lbl_80563F50;
            return 1;
        }
        ok = 0;
    }
    f32 d = PSVECDistance(car + 0x60, r->cam) - F32(r->cam, 0xC);
    if (d <= lbl_80563F48) {
        *weight = lbl_80563F50;
    } else {
        *weight = lbl_80563F50 / (lbl_80563F50 + d);
    }
    return ok;
}

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

void fn_802733F4(u8* p) {
    if (!U8(p, 0x138) && lbl_8055F558) {
        int t = fn_80273494(S32(p, 0x10));
        if (t != 6) {
            CamCycle* c = &((CamCycle*)lbl_8049A258)[t];
            if (++c->cur >= c->count) {
                c->cur = 0;
            }
            if (c->count > 1) {
                U8(p, 0x138) = 1;
            }
        }
    }
    lbl_8055F558 = 0;
}

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

// One selectable camera view.
struct CamView {
    s32 type;       // offset 0x0
    u8 pad4[0x20];  // offset 0x4
    u8 b24;         // offset 0x24
    u8 b25;         // offset 0x25
    u8 pad26[0x2];
};

extern CamView lbl_80346BA0[];
extern u8 lbl_8055F480;

// HUD/feature flags for the camera used in `mode`.
u32 fn_802734F0(int mode) {
    int t = fn_80273494(mode);
    u32 f = 0;
    if (lbl_8055F480) {
        f |= 8;
    } else if (t != 6 && t != 0) {
        CamCycle* c = &((CamCycle*)lbl_8049A258)[t];
        CamView* v = &lbl_80346BA0[c->cur + c->first];
        if (!v->b24) {
            f |= 1;
        } else if (v->type == 2) {
            f |= 2;
        } else if (v->type == 0) {
            f |= 8;
        }
        if (!v->b25) {
            f |= 4;
        }
    } else {
        switch (mode) {
        case 1:
            break;
        case 2:
            f |= 6;
            break;
        case 0:
            f |= 8;
            break;
        default:
            f |= 5;
            break;
        }
    }
    return f;
}

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
