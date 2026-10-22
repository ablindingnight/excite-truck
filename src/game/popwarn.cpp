// Front-end popups (0x80246938-0x802494F8): the wireless and caution warnings ("pop_warn.lyt",
// "WARN:CAUTION_TITLE"), the mode/difficulty choice popups, the menu music cues and the attract-mode
// timeouts. Written against raw field offsets; the large screen routines are still stubs. Names are
// guesses apart from the strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

class MenuState {
public:
    void Finish(int r);
};

struct ModeEntry {
    s32 a;      // offset 0x0
    s32 b;      // offset 0x4
    s32 count;  // offset 0x8
};

extern "C" {
int fn_802E2C20(void);
void fn_80235380(u8* p);
void fn_8020A5F8(f32 v);
void fn_80246CB8(const void* desc);
void fn_80214AF0(int s);
void fn_80214A9C(int s);
void fn_80214CB4(s32 a);
void fn_80214B44(int i);
void fn_80214B94(int i);
s32 fn_80214D04(void);
int fn_802411D4(void);
int fn_802E499C(u16* keys);
void fn_8020C918(int a);
s32 fn_80221EA8(u8* p);
u8 fn_802E8690(u8* p);
int fn_802C9A58(void);
void fn_802168BC(int a);
}

void operator delete(void* p);

extern void* lbl_80343780[];
extern u8 lbl_803432EC[];
extern u8 lbl_803432FC[];
extern u8 lbl_8034330C[];
extern const ModeEntry lbl_802FA3B4[5];
extern s32 lbl_8055BEF0;

s32 lbl_8055BEAC = -1;
s32 lbl_8055BEB0 = -1;
s32 lbl_8055BEB4 = -1;

ModuleTag lbl_8055F250;
s32 lbl_8055F258;
s32 lbl_8055F25C;
u8 lbl_8055F260;  // career mode active
f32 lbl_8055F264;
u8 lbl_8055F268;
u8 lbl_8055F269;
u8 lbl_8055F26A;
s32 lbl_8055F26C;
u8 lbl_8055F270;
u8 lbl_8055F271;
s32 lbl_8055F274;
u8* lbl_8055F278;

extern "C" {

void fn_80246938(u8* p) {}  // stub (92)

u8* fn_80246AA8(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_80246AE8(u8* p) {}  // stub (88)

u8* fn_80246C48(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80343780;
        fn_80235380(PTR(p, 0x18));
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_80246CAC(f32 v) { fn_8020A5F8(v * lbl_8055F264); }

void fn_80246CB8(const void* desc) {}

u8* fn_80246DA8(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_80246DE8(void) { fn_80246CB8(lbl_803432EC); }

void fn_80246DF4(void) { fn_80246CB8(lbl_803432FC); }

void fn_80246E00(void) { fn_80246CB8(lbl_8034330C); }

u8* fn_80246E0C(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_80246E4C(u8* p) {}  // stub (162)

void fn_802470D4(u8* p) {}  // stub (40)

void fn_80247174(u8* p) {}  // stub (39)

void fn_80247210(u8* p) {}  // stub (99)

u32 fn_8024739C(u8* p) { return 0; }

void fn_802473A4(u8* p) {}  // stub (160)

u8 fn_80247624(void) { return lbl_8055F260; }

int fn_8024762C(void) {
    if (fn_802E2C20() % 1500 > 500) {
        fn_80214AF0(1);
    } else {
        fn_80214A9C(1);
    }
    return 0;
}

void fn_8024768C(void) {
    fn_80214CB4(lbl_8055BEAC);
    for (int i = 1; i < lbl_802FA3B4[lbl_8055F26C].count; i++) {
        fn_80214B44(i);
    }
    fn_80214A9C(0x14);
}

int fn_802476FC(void) {
    int ok = 0;
    if (fn_802411D4() && lbl_8055F26C > 0) {
        ok = 1;
    }
    (ok ? fn_80214B94 : fn_80214B44)(4);
    return 0;
}

void fn_80247768(void) { lbl_8055BEAC = fn_80214D04(); }

void fn_8024778C(u8* p) {}  // stub (61)

void fn_80247880(void) { lbl_8055BEB0 = fn_80214D04(); }

void fn_802478A4(u8* p) {}  // stub (90)

void fn_80247A0C(u8* p) {}  // stub (40)

void fn_80247AAC(void) { lbl_8055BEB4 = fn_80214D04(); }

void fn_80247AD0(u8* p) {}  // stub (82)

void fn_80247C18(u8* p) {}  // stub (330)

u8* fn_80248140(u8* p, int del) {
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

void fn_802481A0(u8* p) {}  // stub (126)

void fn_80248398(void) {
    u8* o = lbl_8055F278;
    if (!o) {
        return;
    }
    s32 a = S32(o, 0x4);
    s32 b = S32(o, 0x8);
    U8(o, 0x50) = 1;
    S32(o, 0x48) = lbl_8055BEF0 + a;
    S32(o, 0x4C) = b;
    U8(lbl_8055F278, 0x5C) = 1;
    S32(lbl_8055F278, 0x54) = a;
    S32(lbl_8055F278, 0x58) = b;
    U8(lbl_8055F278, 0x44) = 0;
}

void fn_802483E0(u8* p) {}  // stub (111)

u8* fn_8024859C(u8* p, int del) {
    if (p) {
        U8(p, 0x18) = 0;
        U8(p, 0x50) = 0;
        U8(p, 0x5C) = 0;
        S32(p, 0x24) = 1;
        S32(p, 0x20) = 0;
        U8(p, 0x44) = 0;
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

int fn_802485FC(void) {
    u16 keys[5] = {0x126, 0x128, 0x125, 0x127, 0};
    if (fn_802E499C(keys)) {
        fn_80246CAC(0.0f);
        fn_8020C918(1);
    }
    return 0;
}

void fn_80248668(u8* p) {}  // stub (66)

void fn_80248770(u8* p) {
    U8(p, 0x18) = 1;
    S32(p, 0x1C) = fn_80221EA8(p);
}

void fn_802487A8(u8* p) { U8(p, 0x18) = 0; }

void fn_802487B4(u8* p) {}  // stub (173)

void fn_80248A68(u8* p) { F32(p, 0x1C) = 0.0f; }

void fn_80248A74(u8* p) {}  // stub (31)

void fn_80248AF0(u8* p) {}  // stub (56)

void fn_80248BD0(u8* p) {
    F32(p, 0x20) = 2.0f;
    U8(p, 0x1C) = fn_802E8690(PTR(p, 0x18));
}

void fn_80248C0C(u8* p) {}  // stub (34)

int fn_80248C94(u8* p, int key) {
    if (!U8(p, 0x1C) && key == 0xD) {
        ((MenuState*)p)->Finish(0);
        return 1;
    }
    return 0;
}

void fn_80248CD8(u8* p) {}  // stub (86)

void fn_80248E30(u8* p) {}  // stub (96)

void fn_80248FB0(u8* p) {}  // stub (63)

void fn_802490AC(u8* p) {
    U8(p, 0x1C) = 0;
    S32(p, 0x18) = fn_802E2C20();
    if (fn_802C9A58()) {
        fn_80214A9C(1);
    } else {
        fn_80214A9C(2);
    }
}

void fn_80249104(u8* p) {
    if (!U8(p, 0x1C) && fn_802E2C20() - S32(p, 0x18) >= 15000) {
        fn_802168BC(-2);
    }
}

int fn_80249154(u8* p) {
    if (!U8(p, 0x1C) && fn_802E2C20() - S32(p, 0x18) >= 1500) {
        fn_802168BC(-2);
    }
    return 1;
}

void fn_802491A8(u8* p) {}  // stub (99)

void fn_80249334(u8* p) {}  // stub (45)

void fn_802493E8(u8* p) {}  // stub (32)


}  // extern "C"
