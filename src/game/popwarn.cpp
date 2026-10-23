// Front-end popups (0x80246938-0x802494F8): the wireless and caution warnings ("pop_warn.lyt",
// "WARN:CAUTION_TITLE"), the mode/difficulty choice popups, the menu music cues and the attract-mode
// timeouts. Written against raw field offsets; the large screen routines are still stubs. Names are
// guesses apart from the strings.
#include "game/common.h"
#include "macros.h"
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
    s32 a;            // offset 0x0
    const char* lyt;  // offset 0x4
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
extern s32 lbl_8055BEC0;
extern void* lbl_803435A0[];
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

}  // extern "C"

// A track record as GetTrack returns it; only its value node is used here.
struct KioskTrack {
    u8 pad0[0x80];      // offset 0x0
    ValueNode* vals;    // offset 0x80
};

int GetNumTracks(void);
KioskTrack* GetTrack(int track);

extern "C" {
f32 fn_8020A5F0(void);
void fn_8023D704(int a);
void fn_80129AFC(void);
void fn_802E47E8(void);
void fn_8023D7CC(void);
int fn_80248668(void);
int fn_80278A1C(const char* movie);
void fn_802E317C(const char* fmt, ...);
void fn_8020C91C(int song);
void fn_8023BFBC(int track, int eve);

}  // extern "C"

inline void KioskLoadLog(void) { fn_802E317C("MenuShowKioskLoadIntoGame!"); }

extern "C" {

// Kiosk attract loop: logos, idle menu (or the attract movie), then the E3 track picker; loads the
// chosen demo track with its song.
void fn_80247210(void) {
    if (!lbl_8055F268) {
        lbl_8055F264 = fn_8020A5F0();
        lbl_8055F268 = 1;
    }
    lbl_8055F260 = 1;
    fn_8023D704(0);
    fn_80129AFC();
    fn_802E47E8();
    fn_8023D7CC();
    int skip = 0;
    for (;;) {
        if (skip) {
            skip = 0;
        } else {
            fn_80246DE8();
            fn_80246DF4();
            fn_80246E00();
        }
        if (fn_80248668() == -234) {
            if (!fn_80278A1C("attract.avi")) {
                skip = 1;
            }
            if (!skip) {
                fn_80247174();
            }
            continue;
        }
        skip = fn_80246E4C();
        if (skip >= 0) {
            break;
        }
        skip = 1;
    }
    KioskLoadLog();
    int track = -1;
    int song = -1;
    for (int i = 0; i < GetNumTracks(); i++) {
        if (skip == GetTrack(i)->vals->GetInt("demo_idx", -1)) {
            track = i;
            song = GetTrack(i)->vals->GetInt("song_id", -1);
            break;
        }
    }
    fn_8020C91C(song);
    fn_8020A634();
    fn_8023BFBC(track, 0);
    fn_802483E0();
    fn_8020C91C(-1);
    lbl_8055F260 = 0;
}

u32 fn_8024739C(u8* p) { return 0; }

}  // extern "C"

class LocString;
extern "C" {
extern u32 lbl_8055E044;  // localization version
void fn_80131318(LocString* s);
}

// A localized string looked up by key and refreshed when the language changes.
class LocString {
public:
    LocString(const char* key);
    void Update(void) {
        if (version != lbl_8055E044) {
            fn_80131318(this);
        }
    }

    const char* key;   // offset 0x0
    const char* text;  // offset 0x4
    u32 version;       // offset 0x8
};

extern u32 lbl_8055D430;

// Sets debug flag bit 0 for its scope.
class FlagGuard {
public:
    FlagGuard(void) {
        saved = lbl_8055D430 & 1;
        lbl_8055D430 |= 1;
    }
    ~FlagGuard(void) { lbl_8055D430 = (lbl_8055D430 & ~1) | saved; }
    u32 saved;
};

void ClearUnlockAll(void);
void UnlockAll(void);
int IdentGetCurrentSlot(void);
BOOL IdentIsSlotEmpty(int slot);
char* IdentGetDefaultName(int slot);
void SetPlayerName(const char* name, int slot);

extern "C" {
void fn_80129AEC(int a);
void fn_8020A600(void);
int fn_80247C18(void);
int fn_802478A4(void);
int fn_80247AD0(void);
void fn_8024CF60(void);
void fn_8024CFB4(void);
void fn_8027DDCC(int a);
void fn_80260954(int a);
void fn_8023BEA8(void);
void fn_80214FD8(const char* title, const char* text);
void fn_80239F74(void);
void fn_802E6FBC(void);
void fn_80137C78(int n);
void fn_80137C84(int pad, int slot);
void fn_8023BCEC(void);
void fn_8025812C(void);
void fn_8025C054(void);
void fn_80161DE4(void);
void fn_8027DE6C(void);
void fn_80263F30(int a, int b, int c);
int fn_8024CF54(void);

// Demo main menu loop: dispatches the demo menu's choices until the title screen is left.
void fn_802473A4(void) {
    lbl_8055F260 = 1;
    fn_8020A600();
    fn_802ECEC4();
    while (fn_80246AE8()) {
        while (fn_8024CF54()) {
            ClearUnlockAll();
            int stay = 1;
            do {
                fn_80129AEC(0);
                switch (fn_80247C18()) {
                case 1:
                    fn_8024CF60();
                    break;
                case 2:
                    fn_8024CFB4();
                    break;
                case 3:
                    fn_8027DDCC(0);
                    break;
                case 4:
                    fn_8027DDCC(1);
                    break;
                case 5:
                    fn_8027DDCC(2);
                    break;
                case 6:
                    fn_80260954(0);
                    break;
                case 7:
                    fn_80260954(1);
                    break;
                case 8: {
                    FlagGuard guard;
                    fn_8023BEA8();
                    if (!fn_802411D4()) {
                        static LocString sTitle("UIMain:VERSUS_MISSING_CONTROLLER_TITLE");
                        static LocString sText("UIMain:VERSUS_MISSING_CONTROLLER_TEXT");
                        sTitle.Update();
                        sText.Update();
                        fn_80214FD8(sTitle.text, sText.text);
                    }
                    break;
                }
                case 9:
                    fn_80239F74();
                    if (IdentIsSlotEmpty(IdentGetCurrentSlot())) {
                        fn_802E6FBC();
                        stay = 0;
                    }
                    break;
                case 10:
                    fn_802E6FBC();
                    break;
                case 11: {
                    FlagGuard guard;
                    SetPlayerName(IdentGetDefaultName(0), 0);
                    fn_80137C78(1);
                    int slot = IdentGetCurrentSlot();
                    fn_80137C84(fn_802E4A10(), slot);
                    fn_80129AEC(fn_802E4A10());
                    fn_8023BCEC();
                    break;
                }
                case 12:
                    fn_8025812C();
                    break;
                case 16:
                    fn_8025C054();
                    break;
                case 13:
                    UnlockAll();
                    break;
                case 14:
                    fn_80161DE4();
                    fn_8027DE6C();
                    break;
                case 17:
                    fn_80263F30(1, 1, 1);
                    break;
                case 15:
                    break;
                case -1:
                    stay = 0;
                    break;
                default:
                    fn_802E317C("MenuDoDemo: bad return value %s from demo menu!");
                    break;
                }
            } while (stay);
        }
    }
    lbl_8055F260 = 0;
}

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
int fn_802478A4(void) {
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
    return fn_802146BC(&desc, -999, -999, 1);
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
int fn_80247AD0(void) {
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
    return fn_802146BC(&desc, -999, -999, 1);
}

}  // extern "C"

// The demo menu's mode buttons list (vtable lbl_803435A0): up to 10 {id, locked} entries.
struct ModeList {
    struct Entry {
        s32 id;  // offset 0x0
        u8 on;   // offset 0x4
    };
    ModeList(void) {
        f14 = 0;
        count = 0;
        vt = lbl_803435A0;
        items = buf;
        cap = 10;
    }
    ~ModeList(void) {
        items = 0;
        count = 0;
        cap = 0;
    }
    void Add(s32 id, u8 on) {
        Entry& e = items[count++];
        e.id = id;
        e.on = on;
    }
    void* vt;        // offset 0x0
    u8 pad4[0x10];   // offset 0x4
    s32 f14;         // offset 0x14
    Entry* items;    // offset 0x18
    s32 count;       // offset 0x1C
    s32 cap;         // offset 0x20
    Entry buf[10];   // offset 0x24
};

int IsUnlockAll(void);

extern "C" {
int fn_80161D50(void);
int fn_80161D14(void);
int fn_8027DE68(int a);

// The demo main menu: works out which modes are open, then runs the menu (career and challenge go
// through their difficulty popups). Returns the chosen entry.
int fn_80247C18(void) {
    lbl_8055F26C = 0;
    int n = fn_80161D50();
    bool ok = fn_80161D14() > n;
    if (ok) {
        lbl_8055F26C = 1;
        if (fn_8027DE68(0)) {
            lbl_8055F26C = 2;
            if (fn_8027DE68(1)) {
                lbl_8055F26C = 3;
            }
        }
    }
    lbl_8055BEA8 = 4;
    if (fn_8027B9EC(0)) {
        lbl_8055BEA8 = 5;
    }
    if (IsUnlockAll()) {
        lbl_8055F26C = 3;
        lbl_8055BEA8 = 5;
    }
    const ModeEntry* mode = &lbl_802FA3B4[lbl_8055F26C];
    ModeList list;
    int x = fn_8027DE68(2);
    list.Add(11, !x && lbl_8055F26C >= 2);
    list.Add(12, !x && lbl_8055F26C >= 3);
    list.Add(13, x);
    int y = fn_8027B9EC(1);
    list.Add(14, !y && lbl_8055BEA8 >= 5);
    list.Add(15, y);
    list.Add(16, 0);
    int slot = IdentGetCurrentSlot();
    if (lbl_8055BEC0 < 0 || lbl_8055BEC0 != slot) {
        lbl_8055BEAC = mode->a;
    }
    lbl_8055BEC0 = slot;
    MenuItem items[] = {
        MenuItem(0, 5, &list, 0, 0),
        MenuItem(lbl_8055BAC0, 8, (const void*)fn_8024768C, 0, 0),
        MenuItem(lbl_8055BABC, 8, (const void*)fn_80247768, 1, 0),
        MenuItem(lbl_8055B860, 6, 0, -1, 27),
        MenuItem("new_game", 0, 0, 1, 0),
        MenuItem("load", 0, 0, 2, 0),
        MenuItem("career_0", 0, 0, 3, 0),
        MenuItem("challenge", 0, 0, 6, 0),
        MenuItem("multi", 0, 0, 8, 0),
        MenuItem("tutorial", 0, 0, 12, 0),
        MenuItem("options", 0, 0, 9, 0),
        MenuItem("save", 0, 0, 10, 0),
        MenuItem("action", 0, 0, 11, 0),
        MenuItem("records", 0, 0, 16, 0),
        MenuItem(lbl_804988F0),
    };
    for (;;) {
        int r;
        if (lbl_8055F270) {
            r = 3;
        } else if (lbl_8055F271) {
            r = 6;
        } else {
            MenuDesc desc = {0, 0, (int (*)(int*))fn_802476FC, 0, 0};
            desc.layout = mode->lyt;
            desc.items = items;
            r = fn_802146BC(&desc, -999, -999, 1);
        }
        if (r == 3) {
            int d = fn_802478A4();
            if (d != -1) {
                lbl_8055F270 = 1;
                return d;
            }
            lbl_8055F270 = 0;
        } else if (r == 6) {
            int d = fn_80247AD0();
            if (d != -1) {
                lbl_8055F271 = 1;
                return d;
            }
            lbl_8055F271 = 0;
        } else {
            return r;
        }
    }
}

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

double sin(double x);

}  // extern "C"

struct BobPoint {
    f32 x;  // offset 0x0
    f32 y;  // offset 0x4
};

extern "C" {

}  // extern "C"

// y of the drift curve at x, scaled by the remaining ease u.
static inline f32 BobCurve(f32 x, f32 y, f32 u) {
    BobPoint pts[9] = {
        {-200.0f, -66.0f}, {-170.0f, -46.0f}, {-26.0f, -57.0f}, {27.0f, -32.0f}, {67.0f, -35.0f},
        {96.0f, -30.0f},   {116.0f, -20.0f},  {158.0f, -18.0f}, {200.0f, -6.0f},
    };
    int i;
    for (i = 1; i < 8; i++) {
        if (x < pts[i].x) {
            break;
        }
    }
    f32 y1 = pts[i].y;
    f32 y0 = pts[i - 1].y;
    f32 x1 = pts[i].x;
    f32 x0 = pts[i - 1].x;
    f32 f = (x - x0) / (x1 - x0);
    f = MIN(1.0f, f);
    f = MAX(f, 0.0f);
    return y + u * (y0 + f * (y1 - y0));
}

extern "C" {

// Attract-mode cursor drift: x eases in with a sway 1.5 s after lbl_8055F274; y follows a fixed curve over x.
void fn_802481A0(f32* outX, f32* outY, f32 x, f32 y) {
    int ms = fn_802E2C20() - lbl_8055F274 - 1500;
    f32 t = MIN(1.0f, (f32)ms / 1000.0f);
    t = MAX(t, 0.0f);
    f32 u = 1.0f - t;
    *outX = x + 3.0f * (f32)sin(956.0f * u);
    *outY = BobCurve(x, y, u);
}

}  // extern "C"

// Starts a {start, dur, on} ramp.
static inline void SetRamp(u8* r, s32 start, s32 dur) {
    U8(r, 0x8) = 1;
    S32(r, 0x0) = start;
    S32(r, 0x4) = dur;
}

extern "C" {

}  // extern "C"

static inline void RestartRamps(u8* o) {
    s32 b;
    s32 a;
    a = S32(o, 0x4);
    b = S32(o, 0x8);
    SetRamp(o + 0x48, lbl_8055BEF0 + a, b);
    u8* q = lbl_8055F278;
    SetRamp(q + 0x54, a, b);
    U8(q, 0x44) = 0;
}

extern "C" {

void fn_80248398(void) {
    if (lbl_8055F278) {
        RestartRamps(lbl_8055F278);
    }
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

int fn_80248668(void) {
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
    return fn_802146BC(&desc, -999, -999, 1);
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
