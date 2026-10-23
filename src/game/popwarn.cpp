// Front-end popups (0x80246938-0x802494F8): the wireless and caution warnings ("pop_warn.lyt",
// "WARN:CAUTION_TITLE"), the mode/difficulty choice popups, the menu music cues and the attract-mode
// timeouts. Written against raw field offsets; the large screen routines are still stubs. Names are
// guesses apart from the strings.
#include "game/common.h"
#include "game/menu.h"
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

// A popup that waits for the card, then counts down before closing.
struct WaitPopup {
    u8 pad0[0x18];  // offset 0x0
    u8* card;       // offset 0x18
    u8 done;        // offset 0x1C
    f32 timer;      // offset 0x20
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
void fn_80246CB8(const char* lyt);
void fn_80214AF0(int s);
void fn_80214A9C(int s);
void fn_80214CB4(s32 a);
void fn_80214B44(int i);
void fn_80235380(u8* p);
void fn_802C519C(u8* p);
void fn_802328D4(u8* p);
void fn_80127464(void);
void fn_802E2D74(int ms);
void fn_8020A634(void);
int fn_8027B9EC(int i);
int fn_8027B8C0(int a);
int fn_8027B94C(int i);
void fn_80214B94(int i);
s32 fn_80214D04(void);
int fn_802411D4(void);
int fn_802E499C(u16* keys);
void fn_8020C918(int a);
s32 fn_80221EA8(u8* p);
int fn_802E8690(u8* p);
f32 fn_80214D48(void);
void fn_80214D50(int s);
int fn_802C9A58(void);
void fn_802168BC(int a);
void fn_802ECEC0(void);
void fn_802ECEC4(void);
void fn_802483E0(void);
u8* fn_80232578(const char* name);
u8* fn_802C5130(const char* name);
u8* fn_8023512C(const char* art);
}

void operator delete(void* p);

extern void* lbl_80343780[];
extern u8 lbl_803432EC[];
extern u8 lbl_803432FC[];
extern u8 lbl_8034330C[];
extern const ModeEntry lbl_802FA3B4[5];
extern s32 lbl_8055BEF0;
extern f32 lbl_80558DC4[];  // FLT_EPSILON
extern const f32 lbl_80563898;  // 1.0f
extern f32 lbl_8056389C;  // 0.7f

s32 lbl_8055BEAC = -1;
s32 lbl_8055BEB0 = -1;
s32 lbl_8055BEB4 = -1;
extern s32 lbl_8055BEA8;
extern s32 lbl_8055BEBC;
extern s32 lbl_8055BEB8;
extern void* lbl_80343600[];
extern UnkStatic123 lbl_804988F0;  // this unit's end-of-menu tag
extern char lbl_8055B860[8];
extern char lbl_8055BAC0[4];  // ""
extern char lbl_8055BABC[4];  // ""

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

}  // extern "C"

// Waits on a controller channel while the warning is shown (vtable lbl_80343720).
struct PadWait {
    void* vt;        // offset 0x0
    u8 pad4[0x10];   // offset 0x4
    s32 f14;         // offset 0x14
    s32 chan;        // offset 0x18
    u8 pad1C[0x8];
};

extern void* lbl_80343720[];

extern "C" {
int fn_802E4A10(void);
const char* fn_80131234(const char* key);

// "Wrong controller type" warning, repeated while the bad controller stays connected.
int fn_80246938(void) {
    int chan = fn_802E4A10();
    if (chan >= 0 && chan < 4) {
        fn_80214A9C(0x1F);
        while (fn_802E8690((u8*)chan)) {
            PadWait w;
            w.f14 = 0;
            w.vt = lbl_80343720;
            w.chan = chan;
            MenuItem items[] = {
                MenuItem(0, 5, &w, 0, 0),
                MenuItem("title", fn_80131234("WARN:CAUTION_TITLE")),
                MenuItem("message", fn_80131234("WARN:BAD_CONTROLLER_TYPE_MSG")),
                MenuItem(lbl_804988F0),
            };
            MenuDesc desc = {"pop_warn.lyt", 0, 0, 0, 0};
            desc.items = items;
            fn_802146BC(&desc, -999, -999, 1);
        }
        fn_80214AF0(0x1F);
    }
    return 1;
}

u8* fn_80246AA8(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

}  // extern "C"


// A menu art widget that owns its image ("extruck.art"); dtor is fn_80246C48.
struct MenuArt {
    MenuArt(const char* art) {
        vt = lbl_80343780;
        f14 = 0;
        image = fn_8023512C(art);
    }
    ~MenuArt(void) {
        vt = lbl_80343780;
        fn_80235380(image);
    }
    void* vt;        // offset 0x0
    u8 pad4[0x10];   // offset 0x4
    s32 f14;         // offset 0x14
    u8* image;       // offset 0x18
    u8 pad1C[0x4];
};

extern char lbl_80498908[0x40];  // version text buffer

extern "C" {
int fn_8024762C(void);

// Kiosk title screen: logo art and version text.
int fn_80246AE8(void) {
    lbl_8055F25C = fn_802E2C20();
    MenuArt logo("extruck.art");
    lbl_80498908[0] = 0;
    MenuItem items[] = {
        MenuItem("excite_truck", 5, &logo, 0, 0),
        MenuItem("version", lbl_80498908, CaptionContext(0x40)),
        MenuItem(lbl_8055B860, 6, (const void*)fn_80246938, -2, 13),
        MenuItem(lbl_804988F0),
    };
    MenuDesc desc = {"kiosk.lyt", 0, (int (*)(int*))fn_8024762C, 0, 0};
    desc.items = items;
    fn_802146BC(&desc, -999, -999, 1);
    return 1;
}

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

extern void* lbl_803436C0[];

// Timed menu widget (vtable lbl_803436C0): closes the menu after dur ms.
struct MenuTimer {
    MenuTimer(s32 ms, s32 m) {
        f14 = 0;
        vt = lbl_803436C0;
        dur = ms;
        mode = m;
        f28 = 0;
        start = fn_802E2C20();
        f18 = 1;
    }
    void* vt;       // offset 0x0
    u8 pad4[0x10];  // offset 0x4
    s32 f14;        // offset 0x14
    s32 f18;        // offset 0x18
    s32 start;      // offset 0x1C
    s32 dur;        // offset 0x20
    s32 mode;       // offset 0x24
    s32 f28;        // offset 0x28
};

void fn_80216CA8(void);

// Shows one boot logo layout for 2.5 s.
void fn_80246CB8(const char* lyt) {
    MenuTimer t(2500, 1);
    MenuItem items[] = {
        MenuItem(0, 5, &t, 0, 0),
        MenuItem(lbl_804988F0),
    };
    MenuDesc desc = {lyt, 0, 0, 0, 0};
    desc.items = items;
    fn_80216CA8();
    fn_802146BC(&desc, -999, -999, 1);
}

u8* fn_80246DA8(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_80246DE8(void) { fn_80246CB8((const char*)lbl_803432EC); }

void fn_80246DF4(void) { fn_80246CB8((const char*)lbl_803432FC); }

void fn_80246E00(void) { fn_80246CB8((const char*)lbl_8034330C); }

u8* fn_80246E0C(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

}  // extern "C"

void* operator new(size_t size, const char* tag);

// Track-name text style (0x14, tagged "font").
struct CupText {
    CupText(const char* name) {
        font = 0;
        color = -1;
        flags = 9;
        scale = lbl_80563898;
        f10 = lbl_80563898;
        font = fn_80232578(name);
    }
    ~CupText(void) {
        fn_802328D4(font);
        font = 0;
    }
    u8* font;    // offset 0x0
    s32 color;   // offset 0x4
    u32 flags;   // offset 0x8
    f32 scale;   // offset 0xC
    f32 f10;     // offset 0x10
};

// The E3 track picker: cup tiles, track images and their names (vtable lbl_80343600, dtor fn_802470D4).
struct CupSelect {
    CupSelect(int s) {
        f14 = 0;
        vt = lbl_80343600;
        tileSel = fn_8023512C("tile_cup_s.art");
        tile = fn_8023512C("tile_cup.art");
        tracks = fn_802C5130("e3_trks.img");
        text = new ("font") CupText("uifnt_o.gtx");
        text->scale = lbl_8056389C;
        text->flags = (text->flags & ~7) | 4;
        text->flags = (text->flags & ~0x58) | 0x10;
        count = 2;
        sel = s;
        pos = (f32)sel;
    }
    ~CupSelect(void) {
        vt = lbl_80343600;
        fn_80235380(tileSel);
        fn_80235380(tile);
        fn_802C519C(tracks);
        delete text;
    }
    void* vt;        // offset 0x0
    u8 pad4[0x10];   // offset 0x4
    s32 f14;         // offset 0x14
    u8* tileSel;     // offset 0x18
    u8* tile;        // offset 0x1C
    u8* tracks;      // offset 0x20
    s32 count;       // offset 0x24
    s32 sel;         // offset 0x28
    f32 pos;         // offset 0x2C
    CupText* text;   // offset 0x30
};

extern "C" {

// E3 demo track select; returns the chosen track or -1.
int fn_80246E4C(void) {
    int r;
    fn_802ECEC4();
    {
        MenuTimer t(60000, 3);
        t.f28 = 3;
        CupSelect w(0);
        MenuItem items[] = {
            MenuItem(0, 5, &t, 0, 0),
            MenuItem(0, 5, &w, 0, 0),
            MenuItem(lbl_804988F0),
        };
        MenuDesc desc = {"e3_track.lyt", 0, 0, 0, 0};
        desc.items = items;
        int ret = fn_802146BC(&desc, -999, -999, 1);
        if (ret == -2) {
            r = w.sel;
        } else if (ret == -234) {
            fn_802483E0();
            r = -1;
        } else {
            r = -1;
        }
        fn_802ECEC0();
    }
    return r;
}

u8* fn_802470D4(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80343600;
        fn_80235380(PTR(p, 0x18));
        fn_80235380(PTR(p, 0x1C));
        fn_802C519C(PTR(p, 0x20));
        u8* h = PTR(p, 0x30);
        if (h) {
            fn_802328D4(PTR(h, 0x0));
            PTR(h, 0x0) = 0;
            operator delete(h);
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_80247174(void) {
    for (int i = 0; i < 50; i++) {
        fn_80246CAC(1.0f - (f32)i / 49.0f);
        fn_80127464();
        fn_802E2D74(16);
    }
    fn_8020A634();
    fn_80246CAC(1.0f);
}

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

void fn_8024778C(void) {
    if (lbl_8055BEB8 != lbl_8055F26C) {
        lbl_8055BEB8 = lbl_8055F26C;
        switch (lbl_8055F26C) {
        case 2:
            lbl_8055BEB0 = 1;
            break;
        case 3:
            lbl_8055BEB0 = 2;
            break;
        default:
            lbl_8055BEB0 = 0;
            break;
        }
    }
    fn_80214CB4(lbl_8055BEB0);
    switch (lbl_8055F26C) {
    case 1:
        fn_80214B44(2);
        fn_80214A9C(3);
        break;
    case 2:
        if (fn_8027B8C0(1)) {
            fn_80214B44(3);
        } else {
            fn_80214A9C(3);
        }
        break;
    }
    for (int i = 0; i < 3; i++) {
        if (!fn_8027B94C(i)) {
            fn_80214A9C(i + 11);
        }
    }
}

void fn_80247880(void) { lbl_8055BEB0 = fn_80214D04(); }

// Career difficulty menu.
void fn_802478A4(void) {
    MenuItem items[] = {
        MenuItem(lbl_8055BAC0, 8, (const void*)fn_8024778C, 0, 0),
        MenuItem(lbl_8055BABC, 8, (const void*)fn_80247880, 1, 0),
        // Literal-pool offsets differ (0x10.. vs 0x2A0..) until the unit's earlier strings are literals too.
        MenuItem("career_0", 0, 0, 3, 0),
        MenuItem("career_1", 0, 0, 4, 0),
        MenuItem("career_2", 0, 0, 5, 0),
        MenuItem(lbl_8055B860, 6, 0, -1, 27),
        MenuItem(lbl_804988F0),
    };
    MenuDesc desc = {"excite_diff.lyt", 0, 0, 0, 0};
    desc.items = items;
    fn_802146BC(&desc, -999, -999, 1);
}

void fn_80247A0C(void) {
    if (lbl_8055BEBC != lbl_8055BEA8) {
        lbl_8055BEBC = lbl_8055BEA8;
        switch (lbl_8055BEA8) {
        case 5:
            lbl_8055BEB4 = 1;
            break;
        default:
            lbl_8055BEB4 = 0;
            break;
        }
    }
    fn_80214CB4(lbl_8055BEB4);
    switch (lbl_8055BEBC) {
    case 4:
        fn_80214B44(2);
        break;
    }
    for (int i = 0; i < 2; i++) {
        if (!fn_8027B9EC(i)) {
            fn_80214A9C(i + 11);
        }
    }
}

void fn_80247AAC(void) { lbl_8055BEB4 = fn_80214D04(); }

// Challenge difficulty menu.
void fn_80247AD0(void) {
    MenuItem items[] = {
        MenuItem(lbl_8055BAC0, 8, (const void*)fn_80247A0C, 0, 0),
        MenuItem(lbl_8055BABC, 8, (const void*)fn_80247AAC, 1, 0),
        MenuItem("challenge_0", 0, 0, 6, 0),
        MenuItem("challenge_1", 0, 0, 7, 0),
        MenuItem(lbl_8055B860, 6, 0, -1, 27),
        MenuItem(lbl_804988F0),
    };
    MenuDesc desc = {"chaldiff.lyt", 0, 0, 0, 0};
    desc.items = items;
    fn_802146BC(&desc, -999, -999, 1);
}

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

void fn_802483E0(void) {}  // stub (111)

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

}  // extern "C"

extern void* lbl_803436C0[];

// Kiosk idle timer driven by the kiosk menu (30 s timeout).
struct KioskTimer {
    KioskTimer(void) {
        f14 = 0;
        vt = lbl_803436C0;
        timeout = 30000;
        f24 = 1;
        f28 = 0;
        start = fn_802E2C20();
        armed = 1;
    }
    void* vt;        // offset 0x0
    u8 pad4[0x10];   // offset 0x4
    s32 f14;         // offset 0x14
    s32 armed;       // offset 0x18
    s32 start;       // offset 0x1C
    s32 timeout;     // offset 0x20
    s32 f24;         // offset 0x24
    s32 f28;         // offset 0x28
    u8 pad2C[0x10];  // offset 0x2C
};

extern "C" {
int fn_8020A640(void);
void fn_8020A600(void);

void fn_80248668(void) {
    if (!fn_8020A640()) {
        fn_8020A600();
    }
    KioskTimer t;
    MenuItem items[] = {
        MenuItem(0, 5, &t, 0, 0),
        MenuItem(lbl_804988F0),
    };
    MenuDesc desc = {"kiosk.lyt", 0, (int (*)(int*))fn_802485FC, 0, 0};
    desc.items = items;
    fn_802146BC(&desc, -999, -999, 1);
}

void fn_80248770(u8* p) {
    U8(p, 0x18) = 1;
    S32(p, 0x1C) = fn_80221EA8(p);
}

void fn_802487A8(u8* p) { U8(p, 0x18) = 0; }

void fn_802487B4(u8* p) {}  // stub (173)

void fn_80248A68(u8* p) { F32(p, 0x1C) = 0.0f; }

void fn_80248A74(u8* p) {
    s32 rate;
    if (F32(p, 0x1C) > 370.0f) {
        rate = 200;
    } else {
        rate = 50;
    }
    f32 dt = fn_80214D48();
    F32(p, 0x1C) = F32(p, 0x1C) + (f32)rate * dt;
}

void fn_80230C30(f32* x, f32* y, s32 a, s32 b);
f32 fn_80235F64(u8* text, int flag, f32 v);
void fn_802353F4(u8* text, int flag, f32 x, f32 y, f32 z);
extern const f32 lbl_805638C8;  // 370.0
extern const f32 lbl_805638B0;  // 0.0

// Draws the scrolling warning text, wrapping the scroll offset at the box width.
void fn_80248AF0(u8* p) {
    f32 x, y;
    fn_80230C30(&x, &y, S32(p, 0x4), S32(p, 0x8));
    f32 w = S32(p, 0xC);
    f32 h = S32(p, 0x10);
    if (F32(p, 0x1C) > w) {
        F32(p, 0x1C) -= w;
    }
    int wide = F32(p, 0x1C) > lbl_805638C8;
    fn_802353F4(PTR(p, 0x18), wide, x + F32(p, 0x1C), (y - h) + fn_80235F64(PTR(p, 0x18), wide, F32(p, 0x1C)),
                lbl_805638B0);
}

void fn_80248BD0(u8* p) {
    F32(p, 0x20) = 2.0f;
    U8(p, 0x1C) = fn_802E8690(PTR(p, 0x18));
}

void fn_80248C0C(u8* p) {
    WaitPopup* w = (WaitPopup*)p;
    w->done = fn_802E8690(w->card);
    if (w->done) {
        w->timer = 2.0f;
        fn_80214A9C(0x1E);
    } else {
        w->timer -= fn_80214D48();
        fn_80214AF0(0x1E);
    }
    if (w->timer < lbl_80558DC4[0]) {
        ((MenuState*)p)->Finish(0);
    }
}

int fn_80248C94(u8* p, int key) {
    if (!U8(p, 0x1C) && key == 0xD) {
        ((MenuState*)p)->Finish(0);
        return 1;
    }
    return 0;
}

void fn_80248CD8(u8* p) {}  // stub (86)

void fn_80248E30(u8* p) {}  // stub (96)

int fn_80248FB0(u8* p, int key) {
    s32 now = fn_802E2C20();
    s32 t0 = S32(p, 0x1C);
    if (now < t0 + 750) {
        return 0;
    }
    s32 d = S32(p, 0x20);
    if (d && now - t0 > d - 500) {
        return 0;
    }
    if (S32(p, 0x18) != 1) {
        return 0;
    }
    switch (key) {
    case 27:
        if (U32(p, 0x24) & 2) {
            S32(p, 0x18) = 4;
            break;
        }
        return 0;
    case 13:
        if (U32(p, 0x24) & 1) {
            S32(p, 0x18) = 2;
            break;
        }
        return 0;
    default:
        return 0;
    }
    S32(p, 0x1C) = now - S32(p, 0x20) + 500;
    return 1;
}

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

int fn_80249334(u8* p, int key) {
    switch (key) {
    case 0x125:
        if (S32(p, 0x28) > 0) {
            fn_80214D50(0);
            S32(p, 0x28)--;
        } else {
            fn_80214D50(1);
        }
        break;
    case 0x127:
        if (S32(p, 0x28) + 1 < S32(p, 0x24)) {
            fn_80214D50(0);
            S32(p, 0x28)++;
        } else {
            fn_80214D50(1);
        }
        break;
    }
    return 0;
}

void fn_802493E8(u8* p) {
    for (int i = 0; i < S32(p, 0x1C); i++) {
        u8* e = PTR(p, 0x18) + i * 8;
        if (U8(e, 0x4)) {
            fn_80214AF0(S32(e, 0x0));
        } else {
            fn_80214A9C(S32(e, 0x0));
        }
    }
}


}  // extern "C"
