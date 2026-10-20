// Tutorial popups (0x801DE7E8-0x801DF6FC): the one-time hint screens ("Nice Landing!", morphs, excite
// turbo/jump/overheat...) read from popups.val, remembered per save slot in "popups_seen", shown with a
// box, text and "Press A to continue" (or a caller-supplied draw callback for queued popups), with the
// music ducked while one is up and an instruction sound per press. Names are guesses apart from the
// strings.
#include "game/common.h"

class Sound2D {
public:
    virtual ~Sound2D(void);
    void SetVolume(f32 v) {
        if (v != volume) {
            volumeDirty = 1;
            volume = v;
        }
    }
    void Play(void) {
        start = 1;
        loop = 0;
        stop = 0;
    }
    f32 pitch;   // offset 0x4
    f32 volume;  // offset 0x8
    u8 padC[0xD];
    u8 active;   // offset 0x19
    u8 pad1A[0x2F];
    u8 start;        // offset 0x49
    u8 stop;         // offset 0x4A
    u8 loop;         // offset 0x4B
    u8 restart;      // offset 0x4C
    u8 volumeDirty;  // offset 0x4D
};

class ILayout {
public:
    virtual ~ILayout(void);
    virtual void* Find(const char* name) = 0;
    virtual void* FindArt(const char* name) = 0;
};

struct V2 {
    f32 x;
    f32 y;
};

struct ArtElem {
    const char* name;  // offset 0x0
    V2 pos;            // offset 0x4
    s32 just;          // offset 0xC
    f32 w;             // offset 0x10
    f32 h;             // offset 0x14
};

class PopupDrawer {
public:
    virtual void vf08(void);
    virtual void Draw(void);  // 0x0C
};

typedef void (*PopupDrawFn)(const char* arg, const char* text);

namespace {
struct PopupDef {
    PopupDef(void) {
        text = "";
        arg = "";
        draw = 0;
        repeat = 0;
        layout = 0;
        drawer = 0;
        tex = -1;
    }
    u8 repeat;            // offset 0x0
    const char* text;     // offset 0x4
    const char* arg;      // offset 0x8
    ILayout* layout;      // offset 0xC
    PopupDrawer* drawer;  // offset 0x10
    s32 tex;              // offset 0x14
    PopupDrawFn draw;     // offset 0x18
};
}  // namespace

template <typename T>
struct ArrayRef {
    ArrayRef(T* d, int n) {
        data = d;
        cap = n;
        count = 0;
    }
    ~ArrayRef(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

template <typename T, int N>
struct FixedArr : ArrayRef<T> {
    FixedArr(void) : ArrayRef<T>(storage, N) {}
    ~FixedArr(void) {}
    T storage[N];
};

extern "C" {
void* memcpy(void* d, const void* s, u32 n);
int strcasecmp(const char* a, const char* b);
void fn_802E30C8(const char* fmt, ...);  // debug print
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* t);
ILayout* fn_80145E70(const char* name);  // load layout
s32 fn_802CAC28(const char* name, int flags);
void fn_802CAC54(s32 tex);
int fn_802E2C20(void);  // milliseconds
s64 fn_802E2C9C(void);  // time
void fn_801473F8(ILayout* l, const char* name, const char* fmt, ...);
void fn_801476D8(ArtElem* a, u32 color, s32 tex);
void fn_80147F30(u32 color, int segs, int tex, f32 x, f32 y, f32 r, f32 a0, f32 a1, f32 da, f32 u0, f32 v0, f32 su, f32 sv);
void fn_8014E7D8(int x, int y, int size);
void fn_801331C4(int section, const char* name, f32* v);
void fn_801332D0(void* prefs, const char* name, u32* v);
void fn_80133348(void* prefs, const char* name, u32 v);
Sound2D* fn_80207848(const char* name, int flags);
s32 fn_802090D4(const char* name);
void fn_80209110(s32* h);
void fn_80209190(s32 h, f32 v);
int fn_801E85A4(void);
void fn_801769E0(void);
void fn_801769EC(void);
int fn_801769F8(void);
void fn_80011278(u32 color, f32 x, f32 y, f32 w, f32 h);
void fn_800103B8(u32 color, u32 flags, const char* fmt, f32 scale, f32 x, f32 y, f32 w, ...);
void fn_8000FF88(u32 color, int flags, const char* fmt, f32 scale, f32 x, f32 y, ...);
void fn_800121A4(void);
void fn_80011A2C(void);
extern s32 lbl_8055CFF0;  // screen width
void fn_801DE88C(int id);
void fn_801DE890(int id);
void fn_801DEB10(void);
}

int IdentGetCurrentSlot(void);

ModuleTag lbl_8055E9D0;
UnkStatic7F lbl_8055E9D8;
PopupDef lbl_804872B0[9];
FixedArr<PopupDef, 8> lbl_804873B8;  // queued popups
u32 lbl_8055E9DC;          // seen bits
s32 lbl_8055E9E0;          // next queued popup
ValueTable* lbl_8055E9E4;  // popups.val
s32 lbl_8055E9E8;          // current popup
s32 lbl_8055E9EC;          // music volume handle
Sound2D* lbl_8055E9F0;     // levelup.sfx
s32 lbl_8055E9F4;          // next instruction sound
Sound2D* lbl_804874A8[4];  // tut_instr.sfx
s64 lbl_8055ADF0 = -1;     // when the current popup came up

static inline void* Prefs(void) { return (u8*)IdentGetCurrentSlot() + 0xC; }

static inline void DrawRing(f32 x, f32 y, f32 r) {
    fn_80147F30(0xFFC8C800, 1000, -1, x, y, r, 0.0f, 1.5707964f, -0.006283185f, 0.0f, 0.0f, 1.0f, 1.0f);
}

extern "C" {

void fn_801DE7E8(int id) {
    switch (id) {
    case 1:
        lbl_804872B0[id].layout = fn_80145E70("betalyt.val");
        break;
    case 2:
        lbl_804872B0[id].layout = fn_80145E70("betalyt.val");
        lbl_804872B0[id].tex = fn_802CAC28("zonetex.tex", 0);
        break;
    }
}

void fn_801DE88C(int id) {}

void fn_801DE890(int id) {
    PopupDef* d = &lbl_804872B0[id];
    switch (id) {
    case 1:
        fn_801473F8(d->layout, "morphs_enabled", "Nice Landing!");
        break;
    case 2:
        if (fn_802E2C20() / 500 % 2) {
            ArtElem* a = (ArtElem*)d->layout->FindArt("stuntbar_bg_circle");
            if (a) {
                fn_801476D8(a, -1, d->tex);
                f32 half = 0.5f;
                DrawRing(a->pos.x + a->w * half, a->pos.y + a->h * half, 0.78f * (a->w * half));
            }
        }
        break;
    case 3:
        fn_8014E7D8(lbl_8055CFF0 / 2, 0x6E, 0x32);
        break;
    }
}

void fn_801DE9F0(int id, int sound) {
    if (id != 8 || lbl_8055E9E0 < lbl_804873B8.count) {
        if (id == 8) {
            memcpy(&lbl_804872B0[8], &lbl_804873B8.data[lbl_8055E9E0], sizeof(PopupDef));
            lbl_8055E9E0++;
        }
        lbl_8055E9E8 = id;
        lbl_8055E9DC |= 1 << id;
        lbl_8055ADF0 = fn_802E2C9C();
        if (lbl_8055E9F0 && sound) {
            f32 vol = 1.0f;
            fn_801331C4(4, "effects_volume", &vol);
            lbl_8055E9F0->SetVolume(vol);
            lbl_8055E9F0->Play();
        }
    }
}

void fn_801DEB10(void) {
    if (lbl_8055E9E8 == 8) {
        lbl_8055E9E0 = 0;
        lbl_804873B8.count = 0;
    }
    lbl_8055E9E8 = 0;
    lbl_8055E9DC |= 1;
    lbl_8055ADF0 = fn_802E2C9C();
}

int fn_801DEB6C(const char* name) {
    const char* names[7] = {"none", "nice_landing", "morph_enabled", "morph_activated", "excite_turbo", "excite_jump",
                            "excite_overheat"};
    for (u32 i = 0; i < 7; i++) {
        if (strcasecmp(name, names[i]) == 0) {
            return i;
        }
    }
    return 0;
}

void fn_801DEC18(void) {
    PopupDef* d;
    const char* file;
    int i;
    ValueNode* n;
    fn_801332D0(Prefs(), "popups_seen", &lbl_8055E9DC);
    file = "popups.val";
    lbl_8055E9E4 = fn_80131810(file);
    if (lbl_8055E9E4) {
        i = 0;
        n = lbl_8055E9E4->GetChild(0);
        while (n) {
            if (strcasecmp(n->GetName(), "popup") == 0) {
                d = &lbl_804872B0[fn_801DEB6C(n->GetValue("name"))];
                d->text = n->GetValue("text");
                d->repeat = n->GetBool("repeat", 0);
            }
            n = lbl_8055E9E4->GetChild(++i);
        }
    } else {
        fn_802E30C8("Popup: Couldn't find file %s", file);
    }
    lbl_8055E9EC = fn_802090D4("Popup");
    lbl_8055E9F0 = fn_80207848("levelup.sfx", 0);
    f32 vol = 1.0f;
    fn_801331C4(4, "effects_volume", &vol);
    if (lbl_8055E9F0) {
        lbl_8055E9F0->SetVolume(vol);
    }
    for (u32 j = 0; j < 4; j++) {
        lbl_804874A8[j] = fn_80207848("tut_instr.sfx", 0);
        if (lbl_804874A8[j]) {
            lbl_804874A8[j]->SetVolume(vol);
        }
    }
    lbl_8055E9F4 = 0;
    lbl_8055E9E8 = 0;
    lbl_8055ADF0 = -1;
    for (u32 k = 0; k < 9; k++) {
        fn_801DE7E8(k);
    }
}

void fn_801DEE3C(void) {
    for (u32 i = 0; i < 9; i++) {
        PopupDef* d = &lbl_804872B0[i];
        if (d->layout) {
            delete d->layout;
            d->layout = 0;
        }
        if (d->tex != -1) {
            fn_802CAC54(d->tex);
            d->tex = -1;
        }
        fn_801DE88C(i);
    }
    fn_80209110(&lbl_8055E9EC);
    if (lbl_8055E9F0) {
        delete lbl_8055E9F0;
        lbl_8055E9F0 = 0;
    }
    for (u32 j = 0; j < 4; j++) {
        if (lbl_804874A8[j]) {
            delete lbl_804874A8[j];
            lbl_804874A8[j] = 0;
        }
    }
    if (lbl_8055E9E4) {
        fn_80131818(lbl_8055E9E4);
        lbl_8055E9E4 = 0;
    }
    lbl_8055E9E8 = 0;
    fn_80133348(Prefs(), "popups_seen", lbl_8055E9DC);
    lbl_8055E9DC = 0;
}

void fn_801DEF98(void) {
    fn_801332D0(Prefs(), "popups_seen", &lbl_8055E9DC);
    lbl_8055E9E0 = 0;
    lbl_804873B8.count = 0;
}

void fn_801DEFE0(void) {
    if (lbl_8055E9E8) {
        fn_801769E0();
        fn_80209190(lbl_8055E9EC, 0.0f);
    } else if (lbl_8055ADF0 >= 0) {
        fn_801769EC();
        fn_80209190(lbl_8055E9EC, 1.0f);
        lbl_8055ADF0 = -1;
    }
}

int fn_801DF060(void) { return lbl_8055E9E8 != 0; }

int fn_801DF074(int id, int sound) {
    if ((u32)(id - 7) <= 1 && fn_801E85A4() == 1 && lbl_8055E9E8 == 0) {
        if (lbl_804872B0[id].repeat || !(lbl_8055E9DC & (1 << id))) {
            fn_801DE9F0(id, sound);
            return 1;
        }
    }
    return 0;
}

int fn_801DF110(void) {
    if (lbl_8055E9E8) {
        if (fn_801769F8()) {
            PopupDef* d = &lbl_804872B0[lbl_8055E9E8];
            if (d->draw) {
                d->draw(d->arg, d->text);
            } else {
                fn_80011278(-1, -170.0f, 70.0f, 340.0f, 190.0f);
                fn_800103B8(-1, 0xC, "%s", 1.0f, 0.0f, 50.0f, 300.0f, lbl_804872B0[lbl_8055E9E8].text);
                fn_8000FF88(-1, 0xC, "Press A to continue", 1.0f, 0.0f, -200.0f);
                fn_801DE890(lbl_8055E9E8);
            }
            PopupDrawer* p = lbl_804872B0[lbl_8055E9E8].drawer;
            if (p) {
                p->Draw();
            }
            fn_80011A2C();
        }
        return 1;
    }
    return 0;
}

int fn_801DF238(int key) {
    if (lbl_8055E9E8) {
        if (fn_801769F8()) {
            switch (key) {
            case 0x58:
            case 0xD:
                if (lbl_8055E9E8 == 8 && lbl_8055E9E0 < lbl_804873B8.count) {
                    fn_801DE9F0(8, 0);
                } else {
                    fn_801DEB10();
                }
                if (lbl_804874A8[lbl_8055E9F4] && !lbl_804874A8[lbl_8055E9F4]->active) {
                    f32 vol = 1.0f;
                    fn_801331C4(4, "effects_volume", &vol);
                    lbl_804874A8[lbl_8055E9F4]->SetVolume(vol);
                    lbl_804874A8[lbl_8055E9F4]->Play();
                } else {
                    fn_800121A4();
                }
                lbl_8055E9F4 = (lbl_8055E9F4 + 1) % 4u;
                break;
            }
        }
        return 1;
    }
    return 0;
}

void fn_801DF380(const char* arg, const char* text, PopupDrawFn draw, PopupDrawer* drawer) {
    PopupDef* d = &lbl_804873B8.data[lbl_804873B8.count++];
    d->draw = draw;
    d->drawer = drawer;
    d->arg = arg;
    d->text = text;
    d->repeat = 0;
}

}  // extern "C"
