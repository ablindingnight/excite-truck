// Achievements viewer (0x80259148-0x8025C028): the trophy cabinet ("trophy", "cnt1".."cnt3",
// "MenuDoAchievementsViewer: bad return value %d ...") with its camera orbit and trophy spin helpers.
// Written against raw field offsets; the large routines and the initialiser are still stubs. Names
// are guesses apart from the strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

// Interface of UIObjectsImpl (uiobjs.cpp).
class UIObjects {
public:
    virtual ~UIObjects(void);
    virtual void AddImage(const char* name, const char* file, int frame);
    virtual void AddFont(const char* name, const char* file);
    virtual void AddSound(const char* name, const char* file, u8 loop);
};

class VA {
public:
    virtual void v08(void);
    virtual void v0C(u8* q);
};

extern "C" {
void fn_80236914(void);
void fn_802C6CF8(s32 h);
void fn_802CAC54(u8* p);
void fn_80214AF0(int s);
void fn_80214A9C(int s);
f32 PSVECDotProduct(const f32* a, const f32* b);
double sqrt(double x);
double sin(double x);
}

const char* AchievementsGetLevelModel(int cat, int level);

extern "C" int fn_801345D8(char* buf, int n, const char* fmt, ...);

// Copies at most n characters, always terminating.
static inline void CopyName(const char* s, int n, char* d) {
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

// Shows one trophy: its model name and its level caption.
static inline void SetTrophy(u8* self, s32 cat, s32 level) {
    u8* m = PTR(self, 0x18);
    S32(m, 0x4C) = level;
    const char* name = AchievementsGetLevelModel(cat, level);
    if (name) {
        CopyName(name, 50, (char*)(m + 0x4));
    } else {
        U8(m, 0x4) = 0;
    }
    if (level > 0) {
        fn_801345D8((char*)(m + 0x36), 0x14, "Level %d", level);
    } else {
        U8(m, 0x36) = 0;
    }
}

struct Words9 {
    u32 w[9];
};

extern UIObjects* lbl_8055F394;
extern "C" UIObjects* fn_80258354(void);
extern void* lbl_80345158[];
extern bool lbl_8055F3A0;
extern f32 lbl_8055F3A4;

extern "C" {

void fn_80259148(void) {
    lbl_8055F394 = fn_80258354();
    lbl_8055F394->AddSound("achieve_enter", "a_achieve.sfx", 0);
    lbl_8055F394->AddSound("achieve_trophy", "a_trophy1.sfx", 0);
    lbl_8055F394->AddSound("achieve_tally", "a_tally.sfx", 1);
}

void fn_802591D8(void) {
    delete lbl_8055F394;
    lbl_8055F394 = 0;
}

u32 fn_8025921C(u8* p) { return 0; }

void fn_80259224(u8* p) {}  // stub (279)

u8* fn_80259680(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80345158;
        if (S32(p, 0x50) != -1) {
            fn_802C6CF8(S32(p, 0x50));
            S32(p, 0x50) = -1;
        }
        fn_802CAC54(PTR(p, 0xA8));
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_80259700(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_80259740(int x, f32 v) {
    fn_80236914();
    lbl_8055F3A4 = v;
    lbl_8055F3A0 = x == 2;
}

void fn_8025978C(u8* p) {}  // stub (654)

u8* fn_8025A1C4(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_8025A204(u8* p) {}  // stub (178)

u8* fn_8025A4CC(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_8025A50C(u8* p) {}  // stub (204)

void fn_8025A83C(u8* p) {
    F32(p, 0x88) = 0.0f;
    F32(p, 0x84) = 1.0f;
    F32(p, 0x8C) = 0.0f;
    F32(p, 0x90) = 0.0f;
    F32(p, 0x94) = 1.0f;
    F32(p, 0x98) = 0.0f;
    F32(p, 0x9C) = 0.0f;
    F32(p, 0xA0) = 0.0f;
    F32(p, 0xA4) = 1.0f;
    ((VA*)p)->v0C(p + 4);
}

void fn_8025A87C(u8* p) {
    F32(p, 0x88) = 0.0f;
    F32(p, 0x84) = 1.0f;
    F32(p, 0x8C) = 0.0f;
    F32(p, 0x90) = 0.0f;
    F32(p, 0x94) = 1.0f;
    F32(p, 0x98) = 0.0f;
    F32(p, 0x9C) = 0.0f;
    F32(p, 0xA0) = 0.0f;
    F32(p, 0xA4) = 1.0f;
    U8(p, 0x4) = 0;
    U8(p, 0x36) = 0;
}

void fn_8025A8B8(u8* p) {}  // stub (118)

void fn_8025AA90(u8* p) {}  // stub (174)

void fn_8025AD3C(f32* out, const f32* a, const f32* b, f32 t) {
    out[2] = a[2] + t * (b[2] - a[2]);
    out[1] = a[1] + t * (b[1] - a[1]);
    out[0] = a[0] + t * (b[0] - a[0]);
}

f32 fn_8025AD88(const f32* a, const f32* b) { return PSVECDotProduct(a, b); }

void fn_8025AD8C(u8* self, f32* out, const f32* a, const f32* b, f32 t) {
    f32 s = 4.1f * t;
    f32 k = 1.0f >= s ? s : 1.0f;
    out[0] = a[0] + k * (b[0] - a[0]);
    out[1] = a[1] + k * (b[1] - a[1]);
    out[2] = a[2] + k * (b[2] - a[2]);
}

f32 fn_8025ADF4(f32 x) { return (f32)sqrt(x); }

void fn_8025AE18(u8* p) {}  // stub (165)

f32 fn_8025B0A8(f32* r) {
    f32 hi = r[1];
    f32 lo = r[0];
    return lo + (hi - lo) * 0.5f;
}

f32 fn_8025B0C4(f32 x) { return (f32)sin(x); }

void fn_8025B0E8(Words9* d, const Words9* s) { *d = *s; }

void fn_8025B134(u8* p) {}

void fn_8025B138(u8* p) {
    s32 level = S32(p, 0x1C);
    S32(p, 0x74) = level;
    SetTrophy(p, S32(p, 0x70), level);
    fn_801345D8((char*)(p + 0x20), 0x50, "%s", AchievementsGetLevelModel(S32(p, 0x70), S32(p, 0x74)));
}

void fn_8025B22C(u8* p) {
    for (int i = 1; i <= 5; i++) {
        if (S32(p, 0x74) == i) {
            fn_80214AF0(i + 5);
        } else {
            fn_80214A9C(i + 5);
        }
        if (i <= S32(p, 0x1C)) {
            fn_80214AF0(i);
            fn_80214A9C(i + 10);
        } else {
            fn_80214AF0(i + 10);
            fn_80214A9C(i);
        }
    }
}

void fn_8025B2BC(u8* p) {}  // stub (153)

void fn_8025B520(u8* p) {}  // stub (153)

void fn_8025B784(u8* p) {}

void fn_8025B788(u8* p) {}  // stub (285)

void fn_8025BBFC(u8* p) {}  // stub (132)

void fn_8025BE0C(u8* p) {
    s32 level = S32(p, 0x24);
    SetTrophy(p, S32(p, 0x20), level);
}

void fn_8025BED0(u8* p) {
    u8* o = PTR(p, 0x1C);
    if (U8(o, 0xC5) && !U8(o, 0x58)) {
        s32 level = S32(o, 0xC8);
        S32(p, 0x24) = level;
        s32 cat = S32(p, 0x20) = S32(o, 0x18);
        SetTrophy(p, cat, level);
        U8(PTR(p, 0x18), 0x4A) = 1;
    }
}

}  // extern "C"

ModuleTag lbl_8055F388;
UnkStatic123s lbl_80498E98;
UnkStatic7F lbl_8055F390;
extern const f32 lbl_80563B38;
f32 lbl_8055F39C = lbl_80563B38;
