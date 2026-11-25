// Garage screen (0x8024DAD8-0x802526E0): the car locker ("#locker", "ChassisList", "c_list"), colour
// labels ("UICar:COLOR%d"), trophy and car reveal cues ("trophy_snd", "a_trophy1.sfx", "reveal_car")
// and the chassis list entries. Written against raw field offsets; the large screen routines are still
// stubs. Names are guesses apart from the strings.
#include "game/common.h"
#include "game/gart.h"
// Included before caricontable.h: weak strings come out in REVERSE definition order of their inline owners, and
// retail has IconPool's five before RollStrings' three.
#include "game/roll_strings.h"
#include "game/caricontable.h"
#include "game/locstring.h"
#include "game/gamedb.h"
#include "game/menu.h"
#include "game/menustate.h"
// The car list interface the garage is handed (gamedo.cpp calls it SessionScreen). Only its destructor has a
// body; the list classes implement the rest.
#include "game/sessionscreen.h"
typedef SessionScreen GarageList;

#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

u32* GetVehicleColor(VehicleInfo* v, int i);
const char* GetSpecialChassis(const char* name);

// The 3D car view (caruimod.cpp).
class CarUIScreen {
public:
    CarUIScreen(int mode);
    virtual ~CarUIScreen(void);
    void UnloadModel(void);
    int ModelLoaded(void);
    void SetAnim(u32 anim);
    void SetShine(u8 a);

    u8 pad4[0xD4];
    u32 channel;      // offset 0xD8
    u8 tint;          // offset 0xDC
    bool nextTint;    // offset 0xDD
    u8 padDE[0x1E];
    u8 state[0x1C];   // offset 0xFC, the view's menu widget
    u8 shine;         // offset 0x118
    f32 unk11C;       // offset 0x11C
    s32 mode;         // offset 0x120
};

// The header of a list over separately held storage; its destructor clears it.
template <class T>
struct VecHead {
    ~VecHead(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

#include "game/vdel.h"

// Two (0, 2) value pairs built by the constructor.
struct GaragePair {
    GaragePair(void) { Clear(); }
    void Clear(void) {
        c = 0;
        a = 0;
        d = 2;
        b = 2;
    }
    s32 a;  // offset 0x0
    s32 b;  // offset 0x4
    s32 c;  // offset 0x8
    s32 d;  // offset 0xC
};

// Garage-wide state.
struct GarageFlags {
    GarageFlags(void) {
        unk0 = -1;
        alt = 0;
    }
    s32 unk0;              // offset 0x0, profile slot
    bool alt;              // offset 0x4
    u8 pad5[0x3];
    GaragePair pairs[4];   // offset 0x8
};

extern "C" {
void fn_8024739C(u8* p);
u32 fn_802E4A10(void);
int GetButtonState(u32 ch, int feature);
void fn_80216D28(void);
void fn_80214D50(int s);
void gxForgetTexture(u8* p);
f32 fn_80216CB4(void);  // menu fade-in (0..1)
void fn_80214A9C(int s);
}

void operator delete(void* p);
void operator delete[](void* p);
void* operator new(unsigned long n, const char* tag);
void* operator new[](unsigned long n, const char* tag);

ModuleTag lbl_8055F2D8;
static UnkStatic123 lbl_80498C30;
UnkStatic7F lbl_8055F2E0;
extern s32 lbl_8055F2E8;
static GarageFlags lbl_80498C48;
static char lbl_80498C90[0x70];  // "#unlock_pts" text; its own object (third in .bss, so the menu shares one base)
extern u8* lbl_8055F2EC;


extern "C" ValueTable* fn_80131808(const char* name);
extern "C" void fn_80131818(ValueTable* t);
extern "C" u8* fn_802CAC28(const char* name, int flags);
extern "C" int strcmp(const char* a, const char* b);
extern "C" int strcasecmp(const char* a, const char* b);

// The car icon table (caricons.val): two icons per Car entry, plain and "_e"; shared by the car lists and
// released with the last one.
struct CarIconTable;
static inline int FindIcon(CarIconTable* pool, const char* name) {
    for (int i = 0; i < pool->count; i++) {
        if (strcasecmp(pool->icons[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

CarIconTable* lbl_8055F2E4;
s32 lbl_8055F2E8;
u8* lbl_8055F2EC;

static inline void FreeIconPool(CarIconTable* pool) {
    if (pool) {
        for (int i = 0; i < pool->count; i++) {
            CarIcon* e = &pool->icons[i];
            gxForgetTexture(e->tex);
            gxForgetTexture(e->overlay);
        }
        delete[] pool->icons;
        operator delete(pool);
    }
}
extern s32 lbl_8055F2E8;
extern f32 __float_epsilon[];  // FLT_EPSILON


static inline void CopyName(const char* s, int n, char* d) {
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}



extern "C" void fn_802E317C(const char* fmt, ...);

// Animated slot: shown value chases the selected one.
struct SlotAnim {
    s32 shown;  // offset 0x0
    s32 sel;    // offset 0x4
    f32 t;      // offset 0x8
};


// One list entry: its two icons (plain, extreme), the car, and the colour value shown for it.
struct CarEntry {
    s32 icon[2];           // offset 0x0
    VehicleInfo* vehicle;  // offset 0x8
    u8 extreme;            // offset 0xC
    u32 value;             // offset 0x10
};

#include "game/carstat.h"

CarStats* CarStatsLookup(const char* chassis);
BOOL GetUnlockReq(const char* name, s32* races_run, s32* aaas);
s32 AchievementsGetModelStat(int category, const char* model);

// One stat bar (shared with the car select screen, mroll.cpp).
#include "game/statbar.h"

// The stat panel: the car's stats, one bar and caption per stat.
struct StatPanel {
    CarStats stats;        // offset 0x0
    StatBar bars[5];       // offset 0x2C
    char text[5][0x64];    // offset 0xF4
    u8 flag;               // offset 0x2E8

    void Clear(void) {
        memset(&stats, 0, sizeof(CarStats));
        text[0][0] = 0;
        text[1][0] = 0;
        text[2][0] = 0;
        text[3][0] = 0;
        text[4][0] = 0;
    }

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

extern char lbl_8055C0C0[];  // "%d/%d"


extern "C" {
int fn_80129D18(int seed, int n);
void FormatIntWithPrefix(char* buf, s32 size, s32 value);
}

// Garage car-select screen (partial).
struct GarageSel {
    u8 pad0[0x2C];
    s32 sel;  // offset 0x2C
};

// Selected paint index of the current vehicle.
static inline u32 GetSelColor(u8* p) {
    VehicleInfo* v = FindVehicleNamed((const char*)(p + 0x60), 0);
    if (lbl_80498C48.alt) {
        return GetVehicleColor(v, 0)[0];
    }
    return GetVehicleColor(v, 0)[1];
}

extern "C" int fn_801345D8(char* buf, int n, const char* fmt, ...);
extern "C" const char* fn_80131234(const char* key);

static inline int NearZero(f32 x) { return (f32)__fabs(x) <= __float_epsilon[0]; }

extern "C" {

void fn_8024DAD8(u8* p) { fn_8024739C(p); }

void fn_8024DADC(u8* p) { fn_8024739C(p); }

}  // extern "C"


extern "C" {
void LoadCarClassFile(void);
void fn_8013C8B8(void);
void fn_80257C10(const char* chassis);
int fn_80128314(int heap);
int fn_80265D58(void);
void ShowSelectionColor(u8* p);
s32 fn_8024EB94(u8* p);
u8* fn_8024EB9C(u8* p, int i);
void SetCarSlotSelection(u8* p, s32 v, int set);
u32 fn_8024EBE4(u8* p);
int fn_8024EC3C(void);
int PickRandomGarageCar(void);
u8* fn_8025007C(int n);
int fn_8024DAE0(s32* vehicle, u32* color, int slot, GarageList* screen, int bar);
}
int IdentGetCurrentSlot(void);
void CarStatsInit(void);
void CarStatsShutdown(void);
extern u8 lbl_8055FC8C;            // widescreen
extern const char lbl_8055B860[8];  // ""

#include "game/heapmark.h"
#include "game/strcopy.h"
#include "game/mathutil.h"

namespace {

/* Class name unproven */
// The slot strip: one animated slot per entry of a garage list, plus the icon frame art.
class SlotStrip : public MenuState {
public:
    SlotStrip(int n);
    virtual ~SlotStrip(void) {
        delete[] slots;
        fn_80235380(art);
        gxForgetTexture(tex);
    }
    virtual void Update(void);
    virtual void Draw(void);
    virtual void DrawSlot(int i, s32 shown, f32 x, f32 y, f32 w, f32 h, f32 alpha) = 0;  // 0x60

    SlotAnim* slots;  // offset 0x18
    s32 count;        // offset 0x1C
    u8* tex;          // offset 0x20
    gArtSet* art;          // offset 0x24
    f32 pos;          // offset 0x28: eased toward sel
    s32 sel;          // offset 0x2C
};

/* Class name unproven */
// The car slot list: one animated slot and one entry per car of `list`, starting on the car `car` points at.
class CarSlotList : public SlotStrip {
public:
    CarSlotList(GarageList* list, s32* color, s32* car, StatPanel* panel, CarUIScreen* ui);
    virtual ~CarSlotList(void) {
        delete[] entries;
        if (--lbl_8055F2E8 == 0) {
            FreeIconPool(lbl_8055F2E4);
            lbl_8055F2E4 = 0;
        }
    }
    virtual void Update(void);
    virtual void DrawSlot(int i, s32 shown, f32 x, f32 y, f32 w, f32 h, f32 alpha);
    char* Chassis(void) { return chassis; }
    char* Name(void) { return name; }

    s32* color;         // offset 0x30
    s32* car;           // offset 0x34
    StatPanel* panel;   // offset 0x38
    u8 pad3C[0x10];
    CarUIScreen* ui;    // offset 0x4C
    u8 pad50[0x10];
    char chassis[0x20]; // offset 0x60
    char name[0x40];    // offset 0x80
    s32 fC0;            // offset 0xC0
    u8 padC4[0x4];
    CarIconTable* icons;    // offset 0xC8
    CarEntry* entries;  // offset 0xCC
    GarageList* list;   // offset 0xD0
};

/* Class name unproven */
// The car list, the garage's current list while it runs.
class CarPick : public CarSlotList {
public:
    CarPick(GarageList* list, s32* color, s32* car, StatPanel* panel, CarUIScreen* ui)
        : CarSlotList(list, color, car, panel, ui) {
        lbl_8055F2EC = (u8*)this;
    }
    virtual ~CarPick(void) { lbl_8055F2EC = 0; }
    virtual int Key(int key);
};

/* Class name unproven */
// The paint list: one entry per colour of the chosen car.
class ColorPick : public CarSlotList {
public:
    ColorPick(GarageList* list, s32* color, s32* car, StatPanel* panel, CarUIScreen* ui, u32* out)
        : CarSlotList(list, color, car, panel, ui) {
        ui2 = ui;
        colors = (VDel*)list;
        this->out = out;
    }
    virtual ~ColorPick(void) { delete colors; }
    virtual int Key(int key);
    virtual void DrawSlot(int i, s32 shown, f32 x, f32 y, f32 w, f32 h, f32 alpha);

    VDel* colors;       // offset 0xD4
    CarUIScreen* ui2;   // offset 0xD8
    u32* out;           // offset 0xDC
    char colorText[0x100];  // offset 0xE0: the "UICar:COLOR%d" label
};

}  // namespace

// The garage: pick a car (unless the list has only one), then its paint; "3" opens the car's tuning screen.
// Returns 0 when backed out.
int fn_8024DAE0(s32* vehicle, u32* color, int slot, GarageList* screen, int bar) {
    const int& b = bar;  // form unproven: retail re-reads bar per use instead of hoisting bar*4
    int cur = IdentGetCurrentSlot();
    if (lbl_80498C48.unk0 != cur) {
        lbl_80498C48.unk0 = cur;
        for (int i = 0; i < 4; i++) {
            lbl_80498C48.pairs[i].Clear();
        }
    }
    lbl_80498C48.alt = screen->Count() == 1;
    LoadCarClassFile();
    CarStatsInit();
    int r = -1;
    {
        CarUIScreen ui(0);
        ui.channel = fn_802E4A10();
        StatPanel panel;
        panel.Clear();
        s32* sel = lbl_80498C48.alt ? &lbl_80498C48.pairs[slot].d : &lbl_80498C48.pairs[slot].b;
        CarPick cars(screen, sel, lbl_80498C48.alt ? &lbl_80498C48.pairs[slot].c : &lbl_80498C48.pairs[slot].a, &panel, &ui);
        for (;;) {
            if (!lbl_80498C48.alt) {
                const char* bars[3] = {"car_bar1_e.art", "car_bar1_s.art", "car_bar1_m.art"};
                MenuItem items[] = {
                    MenuItem("#car_model", 0x13, &ui, 0, 0),
                    MenuItem("#car_model", 3, cars.Chassis(), 0, 0),
                    MenuItem("#car_name", cars.Name(), (int)NoCaption()),
                    MenuItem("#car_position", 5, ui.state, 0, 0),
                    MenuItem("#stat_text_1", panel.text[0], (int)NoCaption()),
                    MenuItem("#stat_art_1", 2, &panel.stats.stats[0].value, 0, 0),
                    MenuItem("#stat_text_2", panel.text[1], (int)NoCaption()),
                    MenuItem("#stat_art_2", 2, &panel.stats.stats[1].value, 0, 0),
                    MenuItem("#stat_text_3", panel.text[2], NoCaption()),
                    MenuItem("#stat_art_3", 2, &panel.stats.stats[2].value, 0, 0),
                    MenuItem("#stat_text_4", panel.text[3], (int)NoCaption()),
                    MenuItem("#stat_custom_4", 5, &panel.bars[3], 0, 0),
                    MenuItem("#stat_text_5", panel.text[4], (int)NoCaption()),
                    MenuItem("#stat_art_5", 2, &panel.stats.stats[4].value, 0, 0),
                    MenuItem("#unlock_pts", lbl_80498C90, (int)NoCaption()),
                    MenuItem("#locker", 2, (u8*)&cars + 0xC0, 0, 0),
                    MenuItem("#car_bar_color", bars[b], (int)NoCaption()),
                    MenuItem("#races_run", (char*)(u8*)&cars + 0x3C, (int)NoCaption()),
                    MenuItem("#aaa", (char*)(u8*)&cars + 0x50, (int)NoCaption()),
                    MenuItem(0, 5, (u8*)&cars, 0, 0),
                    MenuItem(lbl_8055B860, 6, (const void*)PickRandomGarageCar, 0, 0x72),
                    MenuItem(lbl_8055B860, 6, 0, -1, 0x1B),
                    MenuItem(lbl_8055B860, 6, (const void*)fn_8024EC3C, -2, 0xD),
                    MenuItem(lbl_80498C30),
                };
                const char* lyt = lbl_8055FC8C ? "carchoose2a_w.lyt" : "carchoose2a.lyt";
                MenuDesc desc = {lyt, items, (int (*)(int*))fn_8024DAD8, 0, 0};
                HeapMark mark("choosevehicle1");
                r = fn_802146BC(&desc, -999, -999, 1);
            }
            if (lbl_80498C48.alt || r == -2) {
                HeapMark mark("choosevehicle2");
                VehicleInfo* v = FindVehicleNamed(cars.Chassis(), 0);
                GarageList* colors = (GarageList*)fn_8025007C(v->num_colors);
                for (int i = 0; i < v->num_colors; i++) {
                    colors->Add(cars.Chassis(), 1, 0);
                }
                s32* csel = lbl_80498C48.alt ? &lbl_80498C48.pairs[slot].d : &lbl_80498C48.pairs[slot].b;
                ColorPick paint(colors, csel, lbl_80498C48.alt ? &lbl_80498C48.pairs[slot].c : &lbl_80498C48.pairs[slot].a, &panel, &ui, color);
                u8* pp = (u8*)&paint;
                SetCarSlotSelection(pp, fn_8024EBE4(pp), 1);
                u8* e = fn_8024EB9C(pp, 0);
                for (int i = 0; i < fn_8024EB94(pp); i++) {
                    e = fn_8024EB9C(pp, i);
                    S32(e, 0x10) = ((VehicleInfo*)PTR(e, 0x8))->colors[i];
                }
                ShowSelectionColor(pp);
                const char* bars[3] = {"car_bar1_e.art", "car_bar1_s.art", "car_bar1_m.art"};
                MenuItem items[] = {
                    MenuItem(0, 5, pp, 0, 0),
                    MenuItem("#car_model", 0x13, &ui, 0, 0),
                    MenuItem("#car_model", cars.Chassis(), (int)NoCaption()),
                    MenuItem("#car_name", cars.Name(), (int)NoCaption()),
                    MenuItem("#color_name", (char*)pp + 0xE0, (int)NoCaption()),
                    MenuItem("#car_position", 5, ui.state, 0, 0),
                    MenuItem("#locker", 2, (u8*)&cars + 0xC0, 0, 0),
                    MenuItem("#car_bar_color", bars[b], (int)NoCaption()),
                    MenuItem("#races_run", (char*)(u8*)&cars + 0x3C, (int)NoCaption()),
                    MenuItem("#aaa", (char*)(u8*)&cars + 0x50, (int)NoCaption()),
                    MenuItem(lbl_8055B860, 6, 0, -1, 0x1B),
                    MenuItem(lbl_8055B860, 6, 0, -2, 0xD),
                    MenuItem(lbl_80498C30),
                };
                MenuDesc desc = {"carchoose2b.lyt", 0, (int (*)(int*))fn_8024DADC, 0, 0};
                desc.items = items;
                int r2 = fn_802146BC(&desc, -999, -999, 1);
                if (r2 == -2 && lbl_80498C48.alt) {
                    r = r2;
                } else if (r2 == -1 && !lbl_80498C48.alt) {
                    continue;
                }
            }
            if (r != 3) {
                break;
            }
            fn_80257C10(cars.Chassis());
        }
    }
    CarStatsShutdown();
    fn_8013C8B8();
    s32* pv = lbl_80498C48.alt ? &lbl_80498C48.pairs[slot].c : &lbl_80498C48.pairs[slot].a;
    *vehicle = *pv;
    return r != -1;
}

// Tints the car with the selected slot's animation, or its saved paint.
static inline void ShowCarColor(u8* p) {
    ColorPick* cp = (ColorPick*)p;
    VehicleInfo* v2 = FindVehicleNamed(cp->chassis, 0);
    u32 col = GetSelColor(p);
    u32 anim = *(u32*)(((u8*)cp->entries) + cp->sel * 0x14 + 0x10);
    cp->ui->SetAnim(anim ? anim : ((u32*)((u8*)v2 + 0x64))[col]);
}

// Remembers colour c as the car's body (alt) or trim colour.
static inline void SaveColor(u8* p, s32 c) {
    ColorPick* cp = (ColorPick*)p;
    VehicleInfo* v = FindVehicleNamed(cp->chassis, 0);
    if (lbl_80498C48.alt) {
        GetVehicleColor(v, 0)[0] = c;
    } else {
        GetVehicleColor(v, 0)[1] = c;
    }
}

// Saves the selected colour for the car and shows it (tint and "UICar:COLOR%d" label).
static inline void DisplaySelectedColor(u8* p, VehicleInfo* v) {
    ColorPick* cp = (ColorPick*)p;
    *cp->out = ((u32*)((u8*)v + 0x64))[cp->sel];
    ShowCarColor(p);
    fn_801345D8(cp->colorText, 0x100, RollStrings::ColorKey(), cp->sel + 1);
    const char* str = fn_80131234(cp->colorText);
    CopyName(str, 0x100, cp->colorText);
}

static inline void ShowSelColor(u8* p) {
    ColorPick* cp = (ColorPick*)p;
    VehicleInfo* v = FindVehicleNamed(cp->chassis, 0);
    if (lbl_80498C48.alt) {
        s32 c = cp->sel;
        GetVehicleColor(v, 0)[0] = c;
    } else {
        s32 c = cp->sel;
        GetVehicleColor(v, 0)[1] = c;
    }
    DisplaySelectedColor(p, v);
}

extern "C" {

void ShowSelectionColor(u8* p) { ShowSelColor(p); }

s32 fn_8024EB94(u8* p) {
    CarSlotList* cp = (CarSlotList*)p;
    return cp->count;
}

u8* fn_8024EB9C(u8* p, int i) {
    CarSlotList* cp = (CarSlotList*)p;
    return ((u8*)cp->entries) + i * 0x14;
}

void SetCarSlotSelection(u8* p, s32 v, int set) {
    CarSlotList* cp = (CarSlotList*)p;
    cp->sel = v;
    if (set) {
        cp->pos = (f32)v;
    }
}

u32 fn_8024EBE4(u8* p) { return GetSelColor(p); }

int fn_8024EC3C(void) {
    int ok = 0;
    if (GetButtonState(fn_802E4A10(), 5) && GetButtonState(fn_802E4A10(), 6)) {
        ok = 1;
    }
    if (ok || ((GarageList*)PTR(lbl_8055F2EC, 0xD0))->Unlocked(U32(lbl_8055F2EC, 0x2C))) {
        return 1;
    }
    fn_80216D28();
    fn_80214D50(1);
    return 0;
}

// The same bounded copy written with a destination cursor.

// Shows car `sel` of the list: its name (the extreme version when owned), stats, unlock progress and paint.
// Index of the car currently shown.
static inline s32 SelectedCar(u8* g) { return S32(g, 0x2C); }

static inline void ShowCar(u8* g, s32 sel) {
    const char* name = ((GarageList*)PTR(g, 0xD0))->Name(sel);
    VehicleInfo* info = FindVehicleNamed(name, 0);
    const char* base = name;
    if (((u8*)GetVehicleColor(info, 0))[8] && ((GarageList*)PTR(g, 0xD0))->IsExtreme(sel)) {
        name = ((GarageList*)PTR(g, 0xD0))->ExtremeName(sel);
    }
    VehicleInfo* v = FindVehicleNamed(name, 0);
    SafeStrCopy((char*)(g + 0x60), v->chassis, 0x20);
    if (((GarageList*)PTR(g, 0xD0))->IsExtreme(sel)) {
        fn_801345D8((char*)(g + 0x80), 0x40, "\\extreme_star %s", v->display_name + 1);
    } else {
        SafeStrCopy((char*)(g + 0x80), v->display_name, 0x40);
    }
    S32(g, 0xC4) = ((GarageList*)PTR(g, 0xD0))->IsExtreme(sel) ? 5 : -1;
    *(s32*)PTR(g, 0x34) = v->id;
    *(s32*)PTR(g, 0x30) = v->unk9C;
    ((StatPanel*)PTR(g, 0x38))->SetCar(v->chassis);
    ((CarUIScreen*)PTR(g, 0x4C))->nextTint = ((GarageList*)PTR(g, 0xD0))->Unlocked(sel) == 0;
    s32 races = -1;
    s32 aaas = -1;
    if (!GetUnlockReq(base, &races, &aaas)) {
        aaas = -1;
        races = -1;
    }
    if (!((GarageList*)PTR(g, 0xD0))->IsExtreme(sel) && races >= 0 && aaas >= 0) {
        fn_801345D8((char*)(g + 0x3C), 0x10, "%d/%d", AchievementsGetModelStat(0, (char*)(g + 0x60)), races);
        fn_801345D8((char*)(g + 0x50), 0x10, "%d/%d", AchievementsGetModelStat(1, (char*)(g + 0x60)), aaas);
    } else {
        FormatIntWithPrefix((char*)(g + 0x3C), 0x10, AchievementsGetModelStat(0, (char*)(g + 0x60)));
        FormatIntWithPrefix((char*)(g + 0x50), 0x10, AchievementsGetModelStat(1, (char*)(g + 0x60)));
    }
    ShowCarColor(g);
}

// Picks a random car for the garage and shows it.
int PickRandomGarageCar(void) {
    s32 sel = fn_80129D18(1, S32(lbl_8055F2EC, 0x1C) - 1);
    u8* g = lbl_8055F2EC;
    S32(g, 0x2C) = sel;
    ShowCar(g, SelectedCar(g));
    return 0;
}

inline SlotStrip::SlotStrip(int n) {
    count = n;
    slots = new ("icon") SlotAnim[count];
    for (int i = 0; i < count; i++) {
        SlotAnim* s = &slots[i];
        s->sel = 0;
        s->shown = 0;
        s->t = 0.0f;
    }
    art = fn_8023512C("caricona.art");
    tex = fn_802CAC28("cariconb.tex", 0);
    sel = 0;
}

// Reference-counted car icon manager shared by the car menus.

CarSlotList::CarSlotList(GarageList* list, s32* color, s32* car, StatPanel* panel, CarUIScreen* ui) : SlotStrip(list->Count()) {
    u8* p = (u8*)this;
    F32(p, 0x28) = S32(p, 0x2C);
    PTR(p, 0xC8) = (u8*)CarIconTable::Acquire();
    PTR(p, 0xD0) = (u8*)list;
    PTR(p, 0x30) = (u8*)color;
    PTR(p, 0x34) = (u8*)car;
    PTR(p, 0x38) = (u8*)panel;
    PTR(p, 0x4C) = (u8*)ui;
    U8(p, 0x80) = 0;
    PTR(p, 0xCC) = (u8*)new ("c_list") CarEntry[S32(p, 0x1C)];
    ValueTable* t = fn_80131808("caricons.val");
    s32 sel = 0;
    for (int i = 0; i < S32(p, 0x1C); i++) {
        CarEntry* e = &((CarEntry*)PTR(p, 0xCC))[i];
        e->vehicle = FindVehicleNamed(((GarageList*)PTR(p, 0xD0))->Name(i), 0);
        if (e->vehicle->id == *(s32*)PTR(p, 0x34)) {
            sel = i;
        }
        e->extreme = ((GarageList*)PTR(p, 0xD0))->IsExtreme(i);
        for (int k = 0; k < 2; k++) {
            const char* name = ((GarageList*)PTR(p, 0xD0))->Name(i);
            if (k == 1 && e->extreme) {
                VehicleInfo* x = FindVehicle(e->vehicle->base);
                if (x->id == *(s32*)PTR(p, 0x34)) {
                    sel = i;
                }
                name = x->chassis;
            }
            e->icon[k] = FindIcon((CarIconTable*)PTR(p, 0xC8), name);
            if (e->icon[k] < 0) {
                fn_802E317C("mvehicle: can't find %s in caricons.val", name);
            }
        }
        int on = 0;
        if (((u8*)GetVehicleColor(e->vehicle, 0))[8] && e->extreme) {
            on = 1;
        }
        SlotAnim* s = &((SlotAnim*)PTR(p, 0x18))[i];
        if (on != s->sel) {
            s->sel = on;
            s->t = (f32)__fabs(s->t);
        }
        s->shown = on;
        s->t = 0.0f;
        e->value = 0;
    }
    fn_80131818(t);
    S32(p, 0x2C) = sel;
    F32(p, 0x28) = sel;
    ShowCar(p, sel);
    S32(p, 0xC0) = 1;
}

}  // extern "C"


// The menu screens' shared UI objects (uiobjs.cpp).
class UIObjects {
public:
    virtual ~UIObjects(void);
    virtual void AddImage(const char* name, const char* file, int frame);
    virtual void AddFont(const char* name, const char* file);
    virtual void AddSound(const char* name, const char* file, u8 loop);  // 0x14
    virtual void DrawImage(const char* name, f32 x, f32 y, int z, int frame);
    virtual void DrawImageFade(const char* name, f32 x, f32 y, f32 a, int z, int frame);
    virtual void s20(void);
    virtual void s24(void);
    virtual const char* GetString(u32 id);
    virtual s32 GetInt(u32 id);
    virtual void s30(void);
    virtual void s34(void);
    virtual void PlaySound(const char* name);  // 0x38
    virtual void s3C(void);
    virtual void SetVolume(const char* name, f32 v);  // 0x40
};

extern "C" UIObjects* fn_80258354(void);

// The car unlock screen's state (vtable __vt__12UnlockScreen): walks `list`, revealing each unlocked car with the
// trophy cue.
extern "C" {
void fn_80236954(const char* event);
void fn_80214AF0(int s);
f32 fn_80214D48(void);
}

static inline f32 AbsF(f32 v) { double a = __fabs(v); return a; }

class UnlockScreen : public MenuState {
public:
    UnlockScreen(GarageList* list, CarUIScreen* ui, StatPanel* panel, u32* color) {
        this->list = list;
        this->ui = ui;
        this->panel = panel;
        this->color = 0;
        if (color) {
            this->color = color;
        }
        model[0] = 0;
        name[0] = 0;
        message[0] = 0;
        title[0] = 0;
        objects = fn_80258354();
        if (objects) {
            objects->AddSound("trophy_snd", "a_trophy1.sfx", 0);
            objects->SetVolume("trophy_snd", 1.0f);
        }
    }
    virtual ~UnlockScreen(void) {
        if (objects) {
            delete objects;
        }
    }
    virtual void Enter(void);
    virtual int Key(int key);
    virtual void Update(void) {
        s32 i = index;
        if (i >= 0 && i < list->Count()) {
            if (i != shown) {
                shown = i;
                if (objects) {
                    objects->PlaySound("trophy_snd");
                }
                CopyIconName(model, 0x20, list->Name(index));
                VehicleInfo* v = FindVehicleNamed(model, 0);
                SafeStrCopy(name, v->display_name, 0x40);
                if (!(v->flags & 2)) {
                    SafeStrCopy(message, "EventPath:CAR_UNLOCKED", 0x80);
                    ui->SetAnim(v->colors[v->default_color]);
                } else {
                    SafeStrCopy(message, "EventPath:EXTREME_UNLOCKED", 0x80);
                    if (color) {
                        ui->SetAnim(*color);
                    } else {
                        ui->SetAnim(v->colors[v->default_color]);
                    }
                }
                const char* text = fn_80131234((const char*)message);
                if (text != message) {
                    SafeStrCopy(message, text, 0x80);
                }
                CopyIconName(title, 0x80, list->Title(index));
                ui->SetShine((v->flags >> 2) & 1);
            }
            if (ui->ModelLoaded()) {
                if (!revealed) {
                    fn_80236954("reveal_car");
                }
                revealed = 1;
            } else {
                wait = 2.0f;
            }
        }
        panel->SetCar(model);
        f32 t = wait - fn_80214D48();
        wait = 0.0f > t ? 0.0f : t;
        if (AbsF(wait) <= __float_epsilon[0]) {
            fn_80214AF0(0x1F);
        }
        t = hold - fn_80214D48();
        hold = 0.0f > t ? 0.0f : t;
        if (AbsF(hold) <= __float_epsilon[0]) {
            fn_80214AF0(0x1F);
        }
    }

    GarageList* list;      // offset 0x18
    u32* color;            // offset 0x1C
    CarUIScreen* ui;       // offset 0x20
    StatPanel* panel;      // offset 0x24
    char model[0x20];      // offset 0x28
    char name[0x40];       // offset 0x48
    char message[0x80];    // offset 0x88
    char title[0x80];      // offset 0x108
    UIObjects* objects;    // offset 0x188
    s32 index;             // offset 0x18C
    s32 shown;             // offset 0x190
    f32 wait;              // offset 0x194
    f32 hold;              // offset 0x198
    u8 revealed;           // offset 0x19C
};

extern "C" {

// The car unlock screen: shows every car of `list` in turn ("carunlock.lyt").
void DisplayCarUnlockScreen(GarageList* list, u32* color) {
    LoadCarClassFile();
    CarStatsInit();
    CarUIScreen ui(1);
    ui.channel = fn_802E4A10();
    StatPanel panel;
    panel.Clear();
    UnlockScreen screen(list, &ui, &panel, color);
    MenuItem items[] = {
        MenuItem(0, 5, &screen, 0, 0),
        MenuItem("#title", screen.title, NoCaption()),
        MenuItem("#car_model", 0x13, &ui, 0, 0),
        MenuItem("#car_model", screen.model, NoCaption()),
        MenuItem("#car_position", 5, ui.state, 0, 0),
        MenuItem("#unlock_msg", screen.message, NoCaption()),
        MenuItem("#chassis", screen.name, NoCaption()),
        MenuItem("#stat_text_1", panel.text[0], NoCaption()),
        MenuItem("#stat_art_1", 2, &panel.stats.stats[0].value, 0, 0),
        MenuItem("#stat_text_2", panel.text[1], NoCaption()),
        MenuItem("#stat_art_2", 2, &panel.stats.stats[1].value, 0, 0),
        MenuItem("#stat_text_3", panel.text[2], NoCaption()),
        MenuItem("#stat_art_3", 2, &panel.stats.stats[2].value, 0, 0),
        MenuItem("#stat_text_4", panel.text[3], NoCaption()),
        MenuItem("#stat_custom_4", 5, &panel.bars[3], 0, 0),
        MenuItem("#stat_text_5", panel.text[4], NoCaption()),
        MenuItem("#stat_art_5", 2, &panel.stats.stats[4].value, 0, 0),
        MenuItem(lbl_80498C30),
    };
    MenuDesc desc = {"carunlock.lyt", 0, 0, 0, 0};
    desc.items = items;
    for (int i = 0; i < list->Count(); i++) {
        screen.index = i;
        fn_802146BC(&desc, -999, -999, 1);
    }
    CarStatsShutdown();
    fn_8013C8B8();
}

namespace {

// A car-list entry (0x48 bytes).
struct ChassisEntry {
    char name[0x20];      // offset 0x0
    u8 unlocked;          // offset 0x20
    char extreme[0x20];   // offset 0x21
    u8 isExtreme;         // offset 0x41
    s32 value;            // offset 0x44
};

/* Class name unproven */
// An array that owns its heap storage.
template <class T>
struct OwnedVec : VecHead<T> {
    OwnedVec(int n) {
        T* b = (T*)operator new[](n * sizeof(T), "dsa_buf");
        this->data = b;
        this->cap = n;
        this->count = 0;
        alloc = b;
    }
    ~OwnedVec(void) { delete[] alloc; }
    T* alloc;  // offset 0xC
};

}  // namespace

/* Class name unproven */
// The garage's list of cars ("ChassisList"), filled by the callers.
class ChassisList : public GarageList {
public:
    ChassisList(int n) : list(n), title("EventPath:CONGRATULATIONS") {}
    virtual ~ChassisList(void) {}
    virtual void Add(const char* name, u8 a, u8 b) {
        u8* p = (u8*)this;
        u8* e = PTR(p, 4) + S32(p, 8)++ * 0x48;
        SafeStrCopy((char*)e, name, 0x20);
        U8(e, 0x20) = a;
        U8(e, 0x41) = b;
        if (b) {
            const char* sp = GetSpecialChassis(name);
            CopyName(sp, 0x20, (char*)e + 0x21);
        } else {
            SafeStrCopy((char*)e + 0x21, "", 0x20);
        }
        if (U32(p, 0x1C) != lbl_8055E044) {
            fn_80131318((LocString*)(p + 0x14));  // a LocString at +0x14
        }
        S32(e, 0x44) = S32(p, 0x18);
    }
    virtual void SetValue(int i, s32 v) { list.data[i].value = v; }
    virtual int Count(void) { return list.count; }
    virtual const char* Name(int i) { return list.data[i].name; }
    virtual int Unlocked(int i) { return list.data[i].unlocked; }
    virtual const char* ExtremeName(int i) { return list.data[i].extreme; }
    virtual int IsExtreme(int i) { return list.data[i].isExtreme; }
    virtual const char* Title(int i) { return (const char*)list.data[i].value; }

    OwnedVec<ChassisEntry> list;  // offset 0x4
    LocString title;              // offset 0x14
};

u8* fn_8025007C(int n) { return (u8*)new ("ChassisList") ChassisList(n); }

f32 fn_80214D48(void);

static inline void UpdateSlotAnims(u8* p) {
    SlotStrip* cp = (SlotStrip*)p;
    int i;
    SlotAnim* e;
    f32 dt;
    dt = fn_80214D48();
    cp->pos = cp->pos + 0.2f * ((f32)cp->sel - cp->pos);
    for (i = 0; i < cp->count; i++) {
        e = &((SlotAnim*)((u8*)cp->slots))[i];
        if (e->sel != e->shown) {
            e->t = e->t + 6.0f * dt;
            if (e->t >= 1.0f) {
                e->shown = e->sel;
                e->t = e->t - 2.0f;
            }
        } else {
            f32 t = e->t + 6.0f * dt;
            if (0.0f < t) {
                t = 0.0f;
            }
            e->t = t;
        }
    }
}

// Same body as UpdateSlotAnims; the declaration order here (found by decomp-permuter) gives the target's registers.
}  // extern "C"

void SlotStrip::Update(void) {
    SlotStrip* p = this;
    SlotAnim* e;
    f32 dt = fn_80214D48();
    int i;
    SlotAnim* slot;
    p->pos = p->pos + 0.2f * ((f32)p->sel - p->pos);
    for (i = 0; i < p->count; i++) {
        slot = &p->slots[i];
        e = slot;
        if (e->sel != e->shown) {
            e->t = e->t + 6.0f * dt;
            if (e->t >= 1.0f) {
                e->shown = e->sel;
                e->t = e->t - 2.0f;
            }
        } else {
            f32 t = e->t + 6.0f * dt;
            if (0.0f < t) {
                t = 0.0f;
            }
            e->t = t;
        }
    }
}

extern "C" {

double fmod(double x, double y);

// Car list input: left/right step through the cars (bumping an end plays the error cue); "2" swaps an
// extreme car between its plain and extreme paint once its slot has settled (either of two keys).
}  // extern "C"

int CarPick::Key(int key) {
    u8* p = (u8*)this;
    s32 sel = S32(p, 0x2C);
    CarEntry* e = &((CarEntry*)PTR(p, 0xCC))[sel];
    u8* paint = (u8*)GetVehicleColor(e->vehicle, 0);
    switch (key) {
    case 0x125:
        if (sel > 0) {
            s32 n = sel - 1;
            S32(p, 0x2C) = n;
            ShowCar(p, n);
        } else {
            fn_80216D28();
            fn_80214D50(1);
        }
        return 1;
    case 0x127: {
        s32 n = sel + 1;
        if (n < S32(p, 0x1C)) {
            S32(p, 0x2C) = n;
            ShowCar(p, n);
        } else {
            fn_80216D28();
            fn_80214D50(1);
        }
        return 1;
    }
    case 0x126:
    case 0x128: {
        u8 extreme = e->extreme;
        if (extreme) {
            if (NearZero(fmod(AbsF(((SlotAnim*)PTR(p, 0x18))[sel].t), 1.0))) {
                int on = 0;
                if (!paint[8] && extreme) {
                    on = 1;
                }
                paint[8] = on;
                ShowCar(p, SelectedCar(p));
            } else {
                fn_80216D28();
            }
        } else {
            fn_80216D28();
            fn_80214D50(1);
        }
        return 1;
    }
    }
    return 0;
}


extern "C" {
void DrawTexturedQuad(f32 x, f32 y, f32 w, f32 h, u8* tex, f32 u0, f32 v0, f32 u1, f32 v1, u32 color);
}

// A car's paint `i`, or white when out of range.
static inline u32 PaintColor(CarEntry* e, s32 i) {
    int ok = 0;
    if (i >= 0 && i < e->vehicle->num_colors) {
        ok = 1;
    }
    return ok ? e->vehicle->colors[i] : (u32)-1;
}

static inline f32 Clamp01(f32 v) {
    f32 t = 1.0f < v ? 1.0f : v;
    return t > 0.0f ? t : 0.0f;
}

/* Helper name unproven */
static inline int RoundF(f32 v) { return (int)(v < 0.0f ? v - 0.5f : v + 0.5f); }

// Scales `c` by the tint `t`, per channel.
static inline u32 TintColor(u32 c, u32 t) {
    int a = t >> 24;
    int r = (t >> 16) & 0xFF;
    int g = (t >> 8) & 0xFF;
    s32 b = t & 0xFF;
    b = b * b * (int)(c & 0xFF) / 256 / 256;
    g = g * g * (int)((c >> 8) & 0xFF) / 256 / 256;
    r = a * r * (int)((c >> 16) & 0xFF) / 256 / 256;
    return 0xFF000000 | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}

extern "C" {

// Draws car slot `i`: its frame (greyed when locked), the two pulsing extreme stars, and the car icon painted
// with its colour.
}  // extern "C"

// Name unproven.
static inline f32 GarageFadeAlpha(const f32& fade) { return 255.0f * fade; }

void CarSlotList::DrawSlot(int i, s32 shown, f32 x, f32 y, f32 w, f32 h, f32 alpha) {
    u8* p = (u8*)this;
    CarEntry* e = &((CarEntry*)PTR(p, 0xCC))[i];
    u32* sel = GetVehicleColor(e->vehicle, 0);
    u32 color = PaintColor(e, lbl_80498C48.alt ? sel[0] : sel[1]);
    if (e->value) {
        color = e->value;
    }
    int locked = ((GarageList*)PTR(p, 0xD0))->Unlocked(i) == 0;
    int extreme = 0;
    if (!locked && ((GarageList*)PTR(p, 0xD0))->IsExtreme(i)) {
        extreme = 1;
    }
    int lit = shown && extreme;
    fn_802354A0(art, lit, x, y, 0.0f, w, h, locked ? 0xFF808080 : -1);
    f32 fade = fn_80216CB4();
    if (extreme) {
        for (int k = 0; k < 2; k++) {
            int part = 5;
            if (k == 0) {
                part = 4;
            }
            f32 t = Unlerp(alpha, 0.0f, 1.0f);
            f32 s = 0.5f + Clamp01(t) / 2.0f;
            f32 pw = fn_80235EF0(art, part) * s;
            f32 s2 = 0.5f + Clamp01(t) / 2.0f;
            f32 ph = GetArtHeight(art, part) * s2;
            int a = RoundF(GarageFadeAlpha(fade));
            a = a > 0x13F ? 0x13F : a;
            fn_802354A0(art, part, (x + 0.5f * w) - 0.5f * pw, k == 0 ? y + ph : y - h, 0.0f, pw, ph, (a << 24) | 0xFFFFFF);
        }
    }
    int k = shown > 0 ? shown : 0;
    k = k >= 1 ? 1 : k;
    CarIcon* ic = &((CarIconTable*)PTR(p, 0xC8))->icons[e->icon[k]];
    f32 u1 = ic->u1;
    f32 v1 = ic->v1;
    f32 u2 = ic->u2;
    f32 v2 = ic->v2;
    u32 tint = TintColor(color, locked ? 0xFF000000 : -1);
    DrawTexturedQuad(x + 0.5f * (w - 0.8f * w), y - 0.5f * (h - 0.8f * h), 0.8f * w, 0.8f * h, ic->tex, u1, v1, u2, v2, locked ? 0xFF000000 : -1);
    DrawTexturedQuad(x + 0.5f * (w - 0.8f * w), y - 0.5f * (h - 0.8f * h), 0.8f * w, 0.8f * h, ic->overlay, u1, v1, u2, v2, tint);
}


void ColorPick::DrawSlot(int i, s32 shown, f32 x, f32 y, f32 w, f32 h, f32 alpha) {
    ColorPick* p = this;
    s32 v = S32(((u8*)p->entries) + i * 0x14, 0x10);
    fn_802354A0(p->art, 0, x, y, 0.0f, w, h, -1);
    fn_80216CB4();
    fn_802354A0(p->art, 3, x, y, 0.0f, w, h, v);
    fn_802354A0(p->art, 6, x, y, 0.0f, w, h, -1);
}

extern "C" {

// Colour list input: left/right pick the previous/next colour; bumping an end plays the error cue.
static inline void ShowChangedColor(u8* p) {
    ColorPick* cp = (ColorPick*)p;
    VehicleInfo* v = FindVehicleNamed(cp->chassis, 0);
    if (lbl_80498C48.alt) {
        const struct { s32 selected; } paint = {cp->sel};
        GetVehicleColor(v, 0)[0] = paint.selected;
    } else {
        const struct { s32 selected; } paint = {cp->sel};
        GetVehicleColor(v, 0)[1] = paint.selected;
    }
    DisplaySelectedColor(p, v);
}

}  // extern "C"

int ColorPick::Key(int key) {
    u8* p = (u8*)this;
    switch (key) {
    case 0x125:
        if (S32(p, 0x2C) > 0) {
            s32 c = --S32(p, 0x2C);
            SaveColor(p, c);
            ShowChangedColor(p);
        } else {
            fn_80216D28();
            fn_80214D50(1);
        }
        return 1;
    case 0x127:
        if (S32(p, 0x2C) < S32(p, 0x1C) - 1) {
            s32 c = ++S32(p, 0x2C);
            SaveColor(p, c);
            ShowChangedColor(p);
        } else {
            fn_80216D28();
            fn_80214D50(1);
        }
        return 1;
    }
    return 0;
}

extern "C" {

void fn_80214AF0(int s);

}  // extern "C"

void CarSlotList::Update(void) {
    CarSlotList* p = this;
    int on;
    SlotAnim* e;
    int i;
    u8* c;
    p->fC0 = U8(((u8*)p->ui), 0xDC) == 0;
    for (i = 0; i < p->count; i++) {
        c = ((u8*)p->entries) + i * 0x14;
        on = 0;
        if (((u8*)GetVehicleColor(*(VehicleInfo**)(c + 8), 0))[8] && c[0xC]) {
            on = 1;
        }
        SlotAnim* slot = &p->slots[i];
        e = slot;
        if (on != e->sel) {
            e->sel = on;
            e->t = (f32)__fabs(e->t);
        }
    }
    UpdateSlotAnims((u8*)p);
    if (p->list->Unlocked(p->sel) == 0) {
        fn_80214A9C(2);
    } else {
        fn_80214AF0(2);
    }
}



extern s32 lbl_8055CFF0;  // screen width
extern s32 lbl_8055CFF4;  // screen height

extern "C" {
void DrawTexturedQuad(f32 x, f32 y, f32 w, f32 h, u8* tex, f32 u0, f32 v0, f32 u1, f32 v1, u32 color);


// Draws the slot strip: the backdrop, each slot sliding/fading by its distance from the cursor, and the frame.
}  // extern "C"

void SlotStrip::Draw(void) {
    SlotStrip* p = this;
    f32 top = lbl_8055CFF4 / 2 - 24.0f;
    f32 w = 1.7777778f * lbl_8055CFF0 / 1.3333334f;
    DrawTexturedQuad(-0.5f * w, 225.0f, w, 66.0f, p->tex, 0.0f, 0.0f, 10.0f, 1.0f, -1);
    f32 fade = fn_80216CB4();
    for (int i = 0; i < p->count; i++) {
        SlotAnim* e = &p->slots[i];
        f32 s = 1.0f - (f32)__fabs(e->t);
        s = s < fade ? s : fade;
        f32 h = 48.0f * s;
        f32 x = 82.5f * (i - p->pos) - 41.25f;
        f32 y = top - 0.5f * (48.0f * (1.0f - s));
        f32 d = (f32)AbsD(p->pos - i);
        if (1.0f < d) {
            d = 1.0f;
        }
        d = d > 0.0f ? d : 0.0f;
        f32 alpha = 1.0f - d;
        DrawSlot(i, e->shown, x, y + 5.0f * s, 82.5f, h + 10.0f * s, alpha);
    }
    fn_802354A0(p->art, 2, -41.25f, 5.0f + top, 0.0f, 82.5f, 58.0f, -1);
}

void UnlockScreen::Enter(void) {
    ui->UnloadModel();
    shown = -1;
    model[0] = 0;
    name[0] = 0;
    message[0] = 0;
    title[0] = 0;
    wait = 0.0f;
    hold = 20.0f;
    fn_80214A9C(0x1F);
    revealed = 0;
}

int UnlockScreen::Key(int key) {
    if (key == 0xD) {
        if (AbsF(wait) <= __float_epsilon[0] || AbsF(hold) <= __float_epsilon[0]) {
            Finish(0);
            return 1;
        }
    }
    return 0;
}
