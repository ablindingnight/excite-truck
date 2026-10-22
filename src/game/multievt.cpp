// Multiplayer event menu (0x802494F8-0x8024BDDC, "multi_evt.lyt" / "multi_country.lyt" /
// "multi_course.lyt"): choosing the location, course, event and difficulty for a multiplayer race,
// remembering the last choices ("last_mp_event_count", ...), and the stats table beside the course list.
// Written against raw field offsets; the large screen routines are still stubs. Names are guesses
// apart from the strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

// A list widget: the base data comes first, the vptr after it.
class VC {
public:
    u8 b0;
    s32 f4;
    u8 b8;
    virtual void s08(void);
    virtual void s0C(void);
    virtual void s10(void);
    virtual void s14(void);
    virtual void s18(void);
    virtual void s1C(void);
    virtual void s20(void);
    virtual void s24(void);
    virtual void s28(void);
    virtual void s2C(void);
    virtual void s30(void);
    virtual void s34(void);
    virtual void s38(s32* a, s32* b);
};

class StatsTable {
public:
    void Show(s32 event);
};

struct NameEntry {
    s32 id;
    const char* name;
};

void* FindTrackEvent(int track, int eve);

extern "C" {
int strcasecmp(const char* a, const char* b);
void fn_8023E52C(u32 a);
s32 fn_80214D04(void);
void fn_80214CB4(s32 a);
int fn_802411D4(void);
void __dt__7UITableFv(void* p, int del);
void __dt__7DynoTexFv(void* p, int del);
void fn_8024974C(u32 a, u32 b, u32 c, int d);
void fn_8024B260(u8* p);
void fn_8024B6AC(u8* p);
}

void operator delete(void* p);

extern const NameEntry lbl_802FA4B0[3];

ModuleTag lbl_8055F280;
UnkStatic123s lbl_80498998;
UnkStatic7F lbl_8055F288;
s32 lbl_8055BF34 = -1;

extern "C" {

u8* fn_802494F8(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_80249538(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_80249578(u8* p) { fn_8023E52C(*(u32*)PTR(p, 0x10)); }

void fn_80249584(u8* p) {}

void fn_80249588(u8* p) {}

void fn_8024958C(u8* p) {}  // stub (53)

int fn_80249660(u8* p) { return 1; }

void fn_80249668(u8* p) {}  // stub (46)

void fn_80249720(void) { lbl_8055BF34 = fn_80214D04(); }

void fn_80249744(void) { fn_80214CB4(lbl_8055BF34); }

void fn_8024974C(u32 a, u32 b, u32 c, int d) {}

u8* fn_8024A110(u8* p, int del) {
    if (p) {
        if (p) {
            __dt__7UITableFv(p, 0);
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_8024A16C(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_8024A1AC(u8* p, int del) {
    if (p) {
        __dt__7DynoTexFv(p, 0);
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_8024A204(u8* p, int del) {
    if (p) {
        U32(p, 0x11C) = 0;
        u8* m = p + 0x10;
        if (m) {
            if (m) {
                U32(p, 0x10) = 0;
                U32(p, 0x14) = 0;
                U32(p, 0x18) = 0;
            }
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_8024A268(u8* p, int del) {
    if (p) {
        U32(p, 0x10) = 0;
        S32(p, 0x18) = -1;
        U32(p, 0x1C) = 0;
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_8024A2BC(u8* p, s32 a, s32 b) {
    u8* q = p;
    for (int i = 0; i < S32(p, 0x14); i++) {
        if (a == S32(q, 0x20) && b == S32(q, 0x24)) {
            S32(p, 0x10) = i;
            return;
        }
        q += 0xC;
    }
}

void fn_8024A304(u8* p) {}  // stub (109)

int fn_8024A4B8(const char** a, const char** b) { return strcasecmp(*a, *b); }

void fn_8024A4C4(u32 a, u32 b, u32 c) { fn_8024974C(a, b, c, 0); }

void fn_8024A4CC(u8* p) {}  // stub (118)

int fn_8024A6A4(s32* p) {
    if (fn_802411D4() == 0) {
        *p = -1;
        return 1;
    }
    return 0;
}

void fn_8024A6EC(u8* p) {}  // stub (346)

u8* fn_8024AC54(u8* p, int del) {
    if (p) {
        u8* m = p + 0x18;
        if (m) {
            if (m) {
                U32(p, 0x18) = 0;
                U32(p, 0x1C) = 0;
                U32(p, 0x20) = 0;
            }
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_8024ACB4(u8* p) {}  // stub (209)

u8* fn_8024AFF8(u8* p, int del) {
    if (p) {
        __dt__7DynoTexFv(p + 0x4C, -1);
        u8* m = p + 0x18;
        if (m) {
            if (m) {
                U32(p, 0x18) = 0;
                U32(p, 0x1C) = 0;
                U32(p, 0x20) = 0;
            }
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_8024B070(u8* p) {}  // stub (66)

void fn_8024B178(u8* p) {}  // stub (39)

int fn_8024B214(u8* a, u8* b) {
    u8 fa = U8(a, 0x4);
    if (fa && !U8(b, 0x4)) {
        return 1;
    }
    if (U8(b, 0x4) && !fa) {
        return -1;
    }
    return strcasecmp(*(const char**)a, *(const char**)b);
}

void fn_8024B260(u8* p) {}  // stub (98)

void fn_8024B3E8(u8* p) {}  // stub (87)

void fn_8024B544(u8* p) {}  // stub (36)

const char* fn_8024B5D4(u8* p, int i) {
    if (i < S32(p, 0x1C)) {
        return ((const char**)PTR(p, 0x18))[i * 2];
    }
    return "";
}

u8* fn_8024B5F8(u8* p, int i) { return p + i * 0x68 + 0xAA; }

u8* fn_8024B608(u8* p, int i) { return p + i * 0x68 + 0x78; }

u8* fn_8024B618(u8* p, int i) { return p + i * 0x68 + 0xDC; }

void fn_8024B628(u8* p) {}  // stub (32)

void fn_8024B6A8(u8* p) { fn_8024B260(p); }

void fn_8024B6AC(u8* p) {}  // stub (36)

void fn_8024B73C(u8* p) {}  // stub (61)

void fn_8024B830(u8* p, s32 id) {
    S32(p, 0x9C) = -1;
    for (int i = 0; i < S32(p, 0x1C); i++) {
        if (id == S32(((u8**)PTR(p, 0x18))[i], 0x0)) {
            S32(p, 0x9C) = i;
        }
    }
}

u8* fn_8024B874(u8* p) { return p + 0x64; }

const char* fn_8024B87C(u8* p, int i) {
    if (i < S32(p, 0x1C)) {
        return *(const char**)(((u8**)PTR(p, 0x18))[i] + 0x4);
    }
    return "";
}

s32 fn_8024B8A4(u8* p, int i) {
    if (i < S32(p, 0x1C)) {
        return S32(((u8**)PTR(p, 0x18))[i], 0x0);
    }
    return -1;
}

u8* fn_8024B8CC(u8* p) { return p + 0x4c; }

u8* fn_8024B8D4(u8* p) { return p + 0x98; }

void fn_8024B8DC(u8* p) {}  // stub (37)

void fn_8024B970(u8* p) { fn_8024B6AC(p); }

u32 fn_8024B974(u8* p) { return U32(p + S32(p, 0x10) * 0xC, 0x28); }

void fn_8024B988(u8* p) {
    s32 n = S32(p, 0x14);
    S32(p, 0x10) = (n + S32(p, 0x10) - 1) % n;
}

void fn_8024B9AC(u8* p) { S32(p, 0x10) = (S32(p, 0x10) + 1) % S32(p, 0x14); }

void fn_8024B9CC(u8* p, s32* a, s32* b) {
    u8* q = p + S32(p, 0x10) * 0xC;
    *a = S32(q, 0x20);
    *b = S32(q, 0x24);
}

u32 fn_8024B9EC(u8* p) { return ((u32*)PTR(p, 0x10))[S32(p, 0x11C) * 2]; }

void fn_8024BA00(u8* p) {}  // stub (40)

void fn_8024BAA0(u8* p) {}  // stub (39)

void fn_8024BB3C(u8* p) {
    s32 v = S32(p, 0x10) - 1;
    S32(p, 0x10) = v > 0 ? v : 0;
}

void fn_8024BB5C(u8* p) {
    s32 v = S32(p, 0x10) + 1;
    S32(p, 0x10) = v > 2 ? 2 : v;
}

const char* fn_8024BB7C(u8* p) { return lbl_802FA4B0[S32(p, 0x10)].name; }

void fn_8024BB98(u8* p) {
    s32 a;
    s32 b;
    ((VC*)PTR(p, 0x4C))->s38(&a, &b);
    ((StatsTable*)p)->Show(*(s32*)FindTrackEvent(a, b));
}

void fn_8024BBF4(u8* p) {}  // stub (51)

void fn_8024BCC0(u8* p) {}  // stub (51)



}  // extern "C"
