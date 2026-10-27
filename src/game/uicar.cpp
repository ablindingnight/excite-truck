// Car select widgets (0x8023FE20-0x80243460): the per-player join/leave widget ("JoinLeaveWidget"),
// the car icons ("caricona.art"), colour and stat labels ("UICar:COLOR%d", "UICar:STAT_%i_TEXT_%i")
// and the list selection state. Written against raw field offsets; the large routines are still
// stubs. Names are guesses apart from the strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

class VDel {
public:
    virtual ~VDel(void);
};

class VU {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual int v14(void);
    virtual const char* v18(int i);
    virtual u8* v1C(int i);
};

extern "C" {
void fn_80235380(u8* p);
void fn_802E317C(const char* fmt, ...);
u8* fn_8023512C(const char* name);
u32 fn_802E4A10(void);
void fn_802328D4(u8* p);
u8* fn_80232578(const char* name);
int fn_802E8504(int ch);
void __construct_array(void* p, void* ctor, void* dtor, u32 size, u32 n);
u8* fn_80241098(u8* p);
}

void operator delete(void* p);

// One player's joined slot ("get_player_info").
struct JoinSlot {
    s32 id;      // offset 0x0
    u32 unk4;    // offset 0x4
    u32 car;     // offset 0x8
    u32 color;   // offset 0xC
};

extern JoinSlot lbl_80498828[2];
extern void* lbl_803426B0[];
extern void* lbl_80342734[];
extern void* lbl_80342618[];
extern s32 lbl_8055F1F4;
extern u8 lbl_8055F1FC;
extern u32 lbl_8055F200;

ModuleTag lbl_8055F1E8;
UnkStatic123s lbl_80498810;
UnkStatic7F lbl_8055F1F0;

extern "C" {

void fn_8023FE20(void) {
    JoinSlot* j = lbl_80498828;
    j[0].id = -1;
    j[1].id = -1;
}

int fn_8023FE34(void) {
    JoinSlot* j = lbl_80498828;
    int n = 0;
    if (j[0].id >= 0) {
        n = 1;
    }
    if (j[1].id >= 0) {
        n++;
    }
    return n;
}

// Car, colour and slot of the id-th joined player (slots with id < 0 are empty).
void fn_8023FE60(int id, u32* car, u32* color, s32* slot) {
    JoinSlot* s = lbl_80498828;
    int n = 0;
    int i;
    for (i = 0; i < 2; i++) {
        if (s->id >= 0) {
            if (n == id) {
                if (car) {
                    *car = lbl_80498828[i].car;
                }
                if (color) {
                    *color = lbl_80498828[i].color;
                }
                if (slot) {
                    *slot = lbl_80498828[i].id;
                }
                return;
            }
            n++;
        }
        s++;
    }
    fn_802E317C("mroll.cpp: get_player_info got bad index %d (there are only %d players set up)", id, n);
}

void fn_8023FF1C(u8* p) {}  // stub (549)

u8* fn_802407B0(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_802407F0(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_80240830(u8* p, int del) {
    if (p) {
        *(void**)(p + 0xC) = lbl_80342618;
        delete (VDel*)PTR(p, 0x24);
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_802408B0(u8* p, int del) {
    if (p) {
        *(void**)(p + 0xC) = lbl_803426B0;
        *(void**)(p + 0x10) = (u8*)lbl_803426B0 + 0x18;
        lbl_8055F1F4--;
        fn_80235380(PTR(p, 0x44));
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_80240928(u8* p, int del) {
    if (p) {
        U32(p, 0x24) = 0;
        *(void**)p = lbl_80342734;
        U32(p, 0x20) = 0;
        fn_802328D4(PTR(p, 0x18));
        U32(p, 0x18) = 0;
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_802409A4(u8* p, u8 v) { U8(p, 0x2E8) = v; }

}  // extern "C"

// One row of a player's car choice list.
struct CarSelItem {
    u8 sel;        // offset 0x0
    u8 pad1[0x3];
    u32 value;     // offset 0x4
};

// A player's car choice list (vtable lbl_80342650).
struct CarSel {
    void* vt;               // offset 0x0
    u8 pad4[0x10];          // offset 0x4
    s32 f14;                // offset 0x14
    CarSelItem* items[4];   // offset 0x18
    u8* car;                // offset 0x28
    s32 count;              // offset 0x2C
    s32 cur;                // offset 0x30
    s32* slot;              // offset 0x34
    u8 active;              // offset 0x38
    char name[0x20];        // offset 0x39

    void Refresh(void) {
        u8 sel;
        int i;
        CarSelItem* e;
        u32 v;
        for (i = 0; i < count; i++) {
            sel = 0;
            v = *slot;
            if (active && i == cur) {
                sel = 1;
            }
            e = items[i];
            e->sel = sel;
            e->value = v;
        }
    }
};

// Copies at most n - 1 characters and always terminates.
static inline void CopyName(const char* src, int n, char* dst) {
    char* d = dst;
    int left = n;
    while ((*d++ = *src++) != 0) {
        if (--left <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

extern void* lbl_80342650[];
extern char lbl_8055BD60[8];  // ""

extern "C" {
const char* fn_802E85B8(s32 slot);

void fn_802409AC(CarSel* p, CarSelItem* item) {
    int n = p->count;
    p->items[n] = item;
    p->count = n + 1;
    p->Refresh();
}

CarSel* fn_80240A1C(CarSel* p, s32* slot, u8* car) {
    p->car = car;
    p->vt = lbl_80342650;
    p->f14 = 0;
    p->count = 0;
    p->cur = 0;
    p->slot = slot;
    p->active = 1;
    CopyName(lbl_8055BD60, 0x20, p->name);
    S32(p->car, 0xD8) = *p->slot;
    if (*p->slot < 0) {
        CopyName(lbl_8055BD60, 0x20, p->name);
    } else {
        const char* nm = fn_802E85B8(*p->slot);
        CopyName(nm, 0x20, p->name);
    }
    p->Refresh();
    return p;
}

void fn_80240BA4(u8* p) {}  // stub (283)

void fn_80241010(u8* p, s32 a, s32 b, s32 c) {
    S32(p, 0x24) = b;
    S32(p, 0x1C) = a + 1;
    S32(p, 0x20) = c;
}

u8* fn_80241024(u8* p) {
    __construct_array(p + 0x2C, (void*)fn_80241098, (void*)fn_80240928, 0x28, 5);
    memset(p, 0, 0x2C);
    U8(p, 0xF4) = 0;
    U8(p, 0x158) = 0;
    U8(p, 0x1BC) = 0;
    U8(p, 0x220) = 0;
    U8(p, 0x284) = 0;
    return p;
}

u8* fn_80241098(u8* p) {
    *(void**)p = lbl_80342734;
    U32(p, 0x14) = 0;
    S32(p, 0x1C) = -1;
    U32(p, 0x24) = 0;
    U32(p, 0x20) = 0;
    PTR(p, 0x18) = fn_80232578("uifnt_o.gtx");
    return p;
}

u8* fn_802410F8(u8* p, u32 owner, u32* sel, u32 extra) {
    *(void**)(p + 0xC) = lbl_803426B0;
    S32(p, 0x4) = -1;
    U8(p, 0x0) = 0;
    U8(p, 0x8) = 1;
    U32(p, 0x24) = 0;
    *(void**)(p + 0x10) = (u8*)lbl_803426B0 + 0x18;
    U32(p, 0x28) = 0;
    U32(p, 0x2C) = *sel;
    PTR(p, 0x30) = (u8*)sel;
    U32(p, 0x34) = owner;
    U32(p, 0x38) = extra;
    S32(p, 0x3C) = lbl_8055F1F4++;
    PTR(p, 0x44) = fn_8023512C("caricona.art");
    F32(p, 0x58) = 0.0f;
    F32(p, 0x54) = 0.0f;
    U8(p, 0x5C) = 0;
    return p;
}

int fn_802411A4(u32* out) {
    if (lbl_8055F1FC) {
        *out = lbl_8055F200;
        return 1;
    }
    return 0;
}

void fn_802411C8(void) { fn_8023FE20(); }

int fn_802411CC(void) { return fn_8023FE34(); }

void fn_802411D0(int id, u32* car, u32* color, s32* slot) { fn_8023FE60(id, car, color, slot); }

int fn_802411D4(void) {
    int r = 0;
    if (fn_802E8504(0) && fn_802E8504(1)) {
        r = 1;
    }
    return r;
}

void fn_80241224(u8* p) {}

void fn_80241228(u8* p) {}

void fn_80230C30(f32* x, f32* y, s32 a, s32 b);
int fn_801345D8(char* buf, int n, const char* fmt, ...);
const char* fn_80131234(const char* key);
void fn_80232BA4(const char* text, u8* font, f32 x, f32 y, s32 color, int align, f32 size, f32 a, f32 b);
extern const s32 lbl_802FA300[3];  // stat level thresholds: 0, 4, 7
extern const f32 lbl_80563758;     // 0.0
extern const f32 lbl_8056375C;     // 0.43
extern const f32 lbl_80563760;     // 0.37
extern const f32 lbl_80563764;     // 1.0

// Draws the text for one car stat at its level ("UICar:STAT_%i_TEXT_%i").
void fn_8024122C(u8* p) {
    f32 x, y;
    fn_80230C30(&x, &y, S32(p, 0x4), S32(p, 0x8));
    if (S32(p, 0x1C) >= 0) {
        int lvl = 0;
        for (int i = 0; i < 3; i++) {
            if (*(s32*)PTR(p, 0x24) && *(s32*)PTR(p, 0x24) >= lbl_802FA300[i]) {
                lvl = i + 1;
            }
        }
        char buf[0x1E];
        fn_801345D8(buf, 0x1E, "UICar:STAT_%i_TEXT_%i", S32(p, 0x1C), lvl);
        int align = 10;
        f32 size = lbl_8056375C;
        if (*PTR(p, 0x20)) {
            size = lbl_80563760;
            align = 9;
        }
        fn_80232BA4(fn_80131234(buf), PTR(p, 0x18), x, y, -0x100, align, size, lbl_80563758, lbl_80563764);
    }
}

void fn_80241354(u8* p) {}  // stub (307)

void fn_80241820(u8* p) {}  // stub (370)

void fn_80241DE8(u8* p) {
    for (int i = 0; i < S32(p, 0x2C); i++) {
        u8 sel = 0;
        u32 v = *(u32*)PTR(p, 0x34);
        if (U8(p, 0x38) && i == S32(p, 0x30)) {
            sel = 1;
        }
        u8* e = *(u8**)(p + 0x18 + i * 4);
        U8(e, 0x0) = sel;
        U32(e, 0x4) = v;
    }
}

int fn_80241E40(u8* p) {
    if (*(s32*)PTR(p, 0x34) != (s32)fn_802E4A10() || !U8(p, 0x38)) {
        return 0;
    }
    u8* q = p;
    for (int i = 0; i < S32(p, 0x2C); i++) {
        u8 sel = 0;
        s32 v = *(s32*)PTR(p, 0x34);
        if (U8(p, 0x38) && i == S32(p, 0x30)) {
            sel = 1;
        }
        u8* e = PTR(q, 0x18);
        U8(e, 0x0) = sel;
        S32(e, 0x4) = v;
        q += 4;
    }
    return 1;
}

u8* fn_80241EE8(u8* p) { return p + 0x4c; }

}  // extern "C"

struct VehicleInfo {
    s32 index;                // offset 0x0
    s32 id;                   // offset 0x4
    s32 base;                 // offset 0x8
    char display_name[0x20];  // offset 0xC
    u8 pad2C[0x38];           // offset 0x2C
    u32 colors[12];           // offset 0x64
    s32 default_color;        // offset 0x94
    s32 num_colors;           // offset 0x98
    s32 unk9C;                // offset 0x9C
};

struct VehicleColor {
    s32 body;  // offset 0x0
    s32 trim;  // offset 0x4
};

struct CarStat {
    const char* name;  // offset 0x0
    s32 value;         // offset 0x4
};

struct CarStats {
    const char* chassis;  // offset 0x0
    CarStat stats[5];     // offset 0x4
};

class CarUIScreen {
public:
    void SetAnim(u32 a);
    u8 pad0[0xDC];
    u8 tint;      // offset 0xDC
    bool nextTint;  // offset 0xDD
};

VehicleInfo* FindVehicleNamed(const char* chassis, int n);
VehicleColor* GetVehicleColor(VehicleInfo* v, int slot);
CarStats* CarStatsLookup(const char* chassis);

extern char lbl_8034287C[];  // "UICar:COLOR%d"

static inline s32 Min(s32 a, s32 b) { return b < a ? b : a; }

// The body colour chooser of the car picker.
struct ColorPick {
    u8 pad0[0x28];
    VehicleInfo* vehicle;  // offset 0x28
    s32 color;             // offset 0x2C
    s32* colorOut;         // offset 0x30
    u32* rgbOut;           // offset 0x34
    CarUIScreen* screen;   // offset 0x38
    s32 slot;              // offset 0x3C
    u8 pad40[0x8];
    s32 f48;               // offset 0x48
    f32 f4C;               // offset 0x4C
    f32 f50;               // offset 0x50
    u8 pad54[0x8];
    char label[0x40];      // offset 0x5C

    // Saves the colour chosen for the old car, then shows the saved colour of v.
    void SetVehicle(VehicleInfo* v) {
        if (vehicle) {
            GetVehicleColor(vehicle, slot)->trim = color;
        }
        vehicle = v;
        color = Min(color, v->num_colors - 1);
        color = GetVehicleColor(vehicle, slot)->trim;
        label[0] = 0;
        if (color >= 0 && color < vehicle->num_colors) {
            u32 rgb = vehicle->colors[color];
            *rgbOut = rgb;
            screen->SetAnim(rgb);
            fn_801345D8(label, 0x40, lbl_8034287C, color + 1);
            const char* text = fn_80131234(label);
            CopyName(text, 0x40, label);
        }
        *colorOut = color;
        f48 = -250;
        f4C = color;
        f50 = color;
    }
};

// One stat bar (vtable lbl_80342734).
class StatBar {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual void v28(void);
    virtual void v2C(void);
    virtual void v30(void);
    virtual void v34(void);
    virtual void v38(void);
    virtual void v3C(void);
    virtual void v40(void);
    virtual void v44(void);
    virtual void v48(void);
    virtual void v4C(void);
    virtual void v50(void);
    virtual void v54(void);
    virtual void v58(void);
    virtual void v5C(void);
    virtual void Set(int i, s32* value, u8* flag);
    u8 pad4[0x24];
};

// The stat panel of the car picker: the car's stats, one bar and caption per stat.
struct StatPanel {
    CarStats stats;        // offset 0x0
    StatBar bars[5];       // offset 0x2C
    char text[5][0x64];    // offset 0xF4
    u8 flag;               // offset 0x2E8

    void SetCar(const char* chassis) {
        CarStats* cs = CarStatsLookup(chassis);
        if (cs) {
            stats = *cs;
            for (int i = 0; i < 5; i++) {
                if (stats.stats[i].name) {
                    CopyName(stats.stats[i].name, 0x64, text[i]);
                } else {
                    text[i][0] = 0;
                }
                bars[i].Set(i, &stats.stats[i].value, &flag);
            }
        } else {
            memset(this, 0, sizeof(CarStats));
            text[0][0] = 0;
            text[1][0] = 0;
            text[2][0] = 0;
            text[3][0] = 0;
            text[4][0] = 0;
        }
    }
};

// The car picker: steps through the car list and updates the colour chooser and stat panel.
struct CarPicker {
    u8 pad0[0x10];
    s32* unk9COut;         // offset 0x10
    s32* idOut;            // offset 0x14
    ColorPick* colors;     // offset 0x18
    StatPanel* stats;      // offset 0x1C
    CarUIScreen* screen;   // offset 0x20
    VU* list;              // offset 0x24
    s32 index;             // offset 0x28
    char chassis[0x20];    // offset 0x2C
    char name[0x20];       // offset 0x4C
    u8* item;              // offset 0x6C

    void Select(void) {
        s32 i = index;
        if (i >= 0 && i < list->v14()) {
            const char* c = list->v18(i);
            VehicleInfo* v = FindVehicleNamed(c, 0);
            *idOut = v->id;
            *unk9COut = v->unk9C;
            CopyName(c, 0x20, chassis);
            CopyName(v->display_name, 0x20, name);
            item = list->v1C(index);
            screen->nextTint = item == 0;
            colors->SetVehicle(v);
            stats->SetCar(chassis);
        }
    }
};

extern "C" {

void fn_80241EF0(CarPicker* p) {
    p->index++;
    if (p->index >= p->list->v14()) {
        p->index = 0;
    }
    p->Select();
}

void fn_802422DC(CarPicker* p) {
    if (--p->index < 0) {
        p->index = p->list->v14() - 1;
    }
    p->Select();
}

u8* fn_802426C0(u8* p) {
    s32 i = S32(p, 0x28);
    if (i >= 0 && i < ((VU*)PTR(p, 0x24))->v14()) {
        return ((VU*)PTR(p, 0x24))->v1C(i);
    }
    return 0;
}

void fn_80242738(u8* p) {}  // stub (249)

void fn_80242B1C(u8* p) {}  // stub (275)

void fn_80242F68(u8* p) {}  // stub (298)

}  // extern "C"
