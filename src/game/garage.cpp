// Garage screen (0x8024DAD8-0x802526E0): the car locker ("#locker", "ChassisList", "c_list"), colour
// labels ("UICar:COLOR%d"), trophy and car reveal cues ("trophy_snd", "a_trophy1.sfx", "reveal_car")
// and the chassis list entries. Written against raw field offsets; the large screen routines are still
// stubs. Names are guesses apart from the strings.
#include "game/common.h"
#include "game/menu.h"
#include "game/menustate.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

struct VehicleInfo;
VehicleInfo* FindVehicleNamed(const char* name, int i);
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

class VDel {
public:
    virtual ~VDel(void);
};

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
    u32 unk0;              // offset 0x0, profile slot
    u8 alt;                // offset 0x4
    u8 pad5[0x3];
    GaragePair pairs[4];   // offset 0x8
    char points[0x70];     // offset 0x48, "#unlock_pts" text
};

extern "C" {
void fn_8024739C(u8* p);
u32 fn_802E4A10(void);
int fn_80129800(u32 ch, int feature);
void fn_80216D28(void);
void fn_80214D50(int s);
void __ct__9LocStringFPCc(u8* p, const char* key);
void fn_80235380(u8* p);
void fn_802CAC54(u8* p);
void fn_802354A0(u8* o, int kind, s32 v, f32 x, f32 y, f32 z, f32 w, f32 h);
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
static GarageFlags lbl_80498C48;
extern u8* lbl_8055F2EC;

class VList {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual int v1C(u32 id);
};
extern void* lbl_8034453C[];
extern void* lbl_803444D8[];

// One car icon: its chassis name ("_e" suffix for the second, "e" texture set) and texture rectangle.
// Fixed-capacity name buffer (original class name unknown).
// Append is a bounded concatenation that always terminates.
template<int N> struct IconName {
    char text[N];
    operator char*() { return text; }
    operator const char*() const { return text; }
    static void CopyTail(const char* s, int n, char* d) {
        while ((*d++ = *s++) != 0) {
            if (--n <= 0) { d[-1] = 0; break; }
        }
    }
    void Append(const char* s) {
        char* d = text;
        int n = N;
        while (*d) { n--; d++; }
        if (n > 0) { CopyTail(s, n, d); }
    }
};

struct IconEntry {
    IconName<32> name;  // offset 0x0
    u8* tex;            // offset 0x20
    u8* texE;           // offset 0x24
    f32 u1;             // offset 0x28
    f32 v1;             // offset 0x2C
    f32 u2;             // offset 0x30
    f32 v2;             // offset 0x34
};

static inline void CopyIconName(char* d, int n, const char* s) {
    char* p = d;
    while ((*p++ = *s++) != 0) {
        if (--n <= 0) {
            p[-1] = 0;
            break;
        }
    }
}

ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* t);
extern "C" u8* fn_802CAC28(const char* name, int flags);
extern "C" int strcmp(const char* a, const char* b);
extern "C" int strcasecmp(const char* a, const char* b);

// The car icon table (caricons.val): two icons per Car entry, plain and "_e"; shared by the car lists and
// released with the last one.
struct IconPool {
    IconPool(void) {
        ValueTable* t = fn_80131808("caricons.val");
        count = 0;
        entries = new ("cicon") IconEntry[50];
        ValueNode* n;
        for (int i = 0; (n = t->GetChild(i)) != 0; i++) {
            if (strcmp(n->GetName(), "Car")) {
                continue;
            }
            for (int k = 0; k < 2; k++) {
                IconEntry* c = &entries[count++];
                CopyIconName(c->name, 32, n->GetValue("chassis"));
                if (k) {
                    c->name.Append("_e");
                }
                c->tex = fn_802CAC28(k ? "caricons0e.tex" : "caricons0.tex", 0);
                c->texE = fn_802CAC28(k ? "caricons1e.tex" : "caricons1.tex", 0);
                c->u1 = n->GetFloat("u1");
                c->v1 = n->GetFloat("v1");
                c->u2 = n->GetFloat("u2");
                c->v2 = n->GetFloat("v2");
            }
        }
        fn_80131818(t);
    }
    IconEntry* entries;  // offset 0x0
    s32 count;           // offset 0x4
};

static inline int FindIcon(IconPool* pool, const char* name) {
    for (int i = 0; i < pool->count; i++) {
        if (strcasecmp(pool->entries[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

extern IconPool* lbl_8055F2E4;

static inline void FreeIconPool(IconPool* pool) {
    if (pool) {
        for (int i = 0; i < pool->count; i++) {
            IconEntry* e = &pool->entries[i];
            fn_802CAC54(e->tex);
            fn_802CAC54(e->texE);
        }
        delete[] pool->entries;
        operator delete(pool);
    }
}
extern s32 lbl_8055F2E8;
extern void* lbl_803443B0[];
extern void* lbl_80344384[];
extern f32 lbl_80558DC4[];  // FLT_EPSILON

extern char lbl_8055C0E8[4];  // ""
extern u32 lbl_8055E044;
extern "C" void fn_80131318(u8* p);

static inline void CopyName(const char* s, int n, char* d) {
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

static inline void CopyNameTo(const char* s, int n, char* d) {
    char* dd;
    int nn;
    const char* ss;
    dd = d;
    ss = s;
    nn = n;
    while ((*dd++ = *ss++) != 0) {
        if (--nn <= 0) {
            dd[-1] = 0;
            break;
        }
    }
}

static inline void CopyNameA(const char* s, int n, char* d) {
    char* dd;
    int nn;
    const char* ss;
    nn = n;
    ss = s;
    dd = d;
    while ((*dd++ = *ss++) != 0) {
        if (--nn <= 0) {
            dd[-1] = 0;
            break;
        }
    }
}

extern const f32 lbl_80563960;  // 0.0
extern "C" u8* fn_8023512C(const char* name);
extern "C" void fn_802E317C(const char* fmt, ...);

// Animated slot: shown value chases the selected one.
struct SlotAnim {
    s32 shown;  // offset 0x0
    s32 sel;    // offset 0x4
    f32 t;      // offset 0x8
};

struct VehicleInfo {
    s32 index;                // offset 0x0
    s32 id;                   // offset 0x4
    s32 base;                 // offset 0x8
    char display_name[0x20];  // offset 0xC
    char chassis[0x20];       // offset 0x2C
    u8 pad4C[0x18];
    u32 colors[12];           // offset 0x64
    s32 default_color;        // offset 0x94
    s32 num_colors;           // offset 0x98
    s32 unk9C;                // offset 0x9C
    u32 flags;                // offset 0xA0, 2: extreme, 4: shiny
};

VehicleInfo* FindVehicle(s32 id);

// One list entry: its two icons (plain, extreme), the car, and the colour value shown for it.
struct CarEntry {
    s32 icon[2];           // offset 0x0
    VehicleInfo* vehicle;  // offset 0x8
    u8 extreme;            // offset 0xC
    u32 value;             // offset 0x10
};

struct CarStat {
    const char* name;  // offset 0x0
    s32 value;         // offset 0x4
};

struct CarStats {
    const char* chassis;  // offset 0x0
    CarStat stats[5];     // offset 0x4
};

CarStats* CarStatsLookup(const char* chassis);
BOOL GetUnlockReq(const char* name, s32* races_run, s32* aaas);
s32 AchievementsGetModelStat(int category, const char* model);

// One stat bar (vtable lbl_80342734, from the car select screen).
class StatBar {
public:
    StatBar(void);
    virtual ~StatBar(void);
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

class GarageList {
public:
    virtual void v08(void);
    virtual void Add(const char* name, u8 unlocked, u8 extreme);  // 0x0C
    virtual void v10(void);
    virtual int Count(void);                 // 0x14
    virtual const char* Name(int i);      // 0x18
    virtual int Unlocked(int i);          // 0x1C
    virtual const char* ExtremeName(int i);  // 0x20
    virtual int IsExtreme(int i);         // 0x24
    virtual const char* Title(int i);     // 0x28
};

extern "C" {
int fn_80129D18(int seed, int n);
void fn_80130FCC(char* buf, s32 size, s32 value);
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

extern char lbl_8034287C[];
extern "C" int fn_801345D8(char* buf, int n, const char* fmt, ...);
extern "C" const char* fn_80131234(u8* s);

static inline int NearZero(f32 x) { return (f32)__fabs(x) <= lbl_80558DC4[0]; }

extern "C" {

void fn_8024DAD8(u8* p) { fn_8024739C(p); }

void fn_8024DADC(u8* p) { fn_8024739C(p); }

}  // extern "C"

extern void* lbl_80344410[];
extern void* lbl_80344474[];

extern "C" {
void fn_8013C804(void);
void fn_8013C8B8(void);
void fn_80257C10(const char* chassis);
int fn_80128314(int heap);
int fn_80265D58(void);
void fn_8024EA1C(u8* p);
u32 fn_8024EB94(u8* p);
u8* fn_8024EB9C(u8* p, int i);
void fn_8024EBAC(u8* p, s32 v, int set);
u32 fn_8024EBE4(u8* p);
int fn_8024EC3C(void);
int fn_8024ECD4(void);
u8* fn_8024F17C(u8* p, GarageList* list, s32* color, s32* car, StatPanel* panel, CarUIScreen* ui);
u8* fn_8025007C(int n);
int fn_8024DAE0(s32* vehicle, u32* color, int slot, GarageList* screen, int bar);
}
int IdentGetCurrentSlot(void);
void CarStatsInit(void);
void CarStatsShutdown(void);
extern u8 lbl_8055FC8C;            // widescreen
extern const char lbl_8055B860[];  // ""

// Heap use at the start of a screen, for leak checks.
class HeapMark {
public:
    HeapMark(const char* name) {
        this->name = name;
        heap0 = fn_80128314(0);
        heap1 = fn_80128314(1);
        other = fn_80265D58();
    }
    ~HeapMark(void) {}

    const char* name;
    int heap0;
    int heap1;
    int other;
};

// The car slot list's teardown (vtables lbl_803444D8, then lbl_8034453C): the slot entries, the shared icon
// pool with the last list, then the strip's slot animations, widget and texture.
static inline void DestroyCarSlots(u8* p) {
    *(void**)p = lbl_803444D8;
    delete[] PTR(p, 0xCC);
    if (--lbl_8055F2E8 == 0) {
        FreeIconPool(lbl_8055F2E4);
        lbl_8055F2E4 = 0;
    }
    *(void**)p = lbl_8034453C;
    delete[] PTR(p, 0x18);
    fn_80235380(PTR(p, 0x24));
    fn_802CAC54(PTR(p, 0x20));
}

// The car list (vtable lbl_80344474), the garage's current list while it runs.
struct CarPick {
    CarPick(GarageList* list, s32* color, s32* car, StatPanel* panel, CarUIScreen* ui) {
        fn_8024F17C(data, list, color, car, panel, ui);
        *(void**)data = lbl_80344474;
        lbl_8055F2EC = data;
    }
    ~CarPick(void) {
        lbl_8055F2EC = 0;
        DestroyCarSlots(data);
    }
    char* Chassis(void) { return (char*)data + 0x60; }
    char* Name(void) { return (char*)data + 0x80; }
    u8 data[0xD4];
};

// The paint list (vtable lbl_80344410): one entry per colour of the chosen car.
struct ColorPick {
    ColorPick(GarageList* list, s32* color, s32* car, StatPanel* panel, CarUIScreen* ui, u32* out) {
        fn_8024F17C(data, list, color, car, panel, ui);
        *(void**)data = lbl_80344410;
        PTR(data, 0xD8) = (u8*)ui;
        PTR(data, 0xD4) = (u8*)list;
        PTR(data, 0xDC) = (u8*)out;
    }
    ~ColorPick(void) {
        *(void**)data = lbl_80344410;
        delete (VDel*)PTR(data, 0xD4);
        DestroyCarSlots(data);
    }
    u8 data[0x1E0];
};

// The garage: pick a car (unless the list has only one), then its paint; "3" opens the car's tuning screen.
// Returns 0 when backed out.
int fn_8024DAE0(s32* vehicle, u32* color, int slot, GarageList* screen, int bar) {
    int cur = IdentGetCurrentSlot();
    if (lbl_80498C48.unk0 != cur) {
        lbl_80498C48.unk0 = cur;
        for (int i = 0; i < 4; i++) {
            lbl_80498C48.pairs[i].Clear();
        }
    }
    lbl_80498C48.alt = screen->Count() == 1;
    fn_8013C804();
    CarStatsInit();
    int r = -1;
    {
        CarUIScreen ui(0);
        ui.channel = fn_802E4A10();
        StatPanel panel;
        panel.Clear();
        GaragePair* pair = &lbl_80498C48.pairs[slot];
        CarPick cars(screen, lbl_80498C48.alt ? &pair->d : &pair->b, lbl_80498C48.alt ? &pair->c : &pair->a, &panel,
                     &ui);
        for (;;) {
            if (!lbl_80498C48.alt) {
                const char* bars[3] = {"car_bar1_e.art", "car_bar1_s.art", "car_bar1_m.art"};
                MenuItem items[] = {
                    MenuItem("#car_model", 0x13, &ui, 0, 0),
                    MenuItem("#car_model", cars.Chassis()),
                    MenuItem("#car_name", cars.Name()),
                    MenuItem("#car_position", 5, ui.state, 0, 0),
                    MenuItem("#stat_text_1", panel.text[0]),
                    MenuItem("#stat_art_1", 2, &panel.stats.stats[0].value, 0, 0),
                    MenuItem("#stat_text_2", panel.text[1]),
                    MenuItem("#stat_art_2", 2, &panel.stats.stats[1].value, 0, 0),
                    MenuItem("#stat_text_3", panel.text[2]),
                    MenuItem("#stat_art_3", 2, &panel.stats.stats[2].value, 0, 0),
                    MenuItem("#stat_text_4", panel.text[3]),
                    MenuItem("#stat_custom_4", 5, &panel.bars[3], 0, 0),
                    MenuItem("#stat_text_5", panel.text[4]),
                    MenuItem("#stat_art_5", 2, &panel.stats.stats[4].value, 0, 0),
                    MenuItem("#unlock_pts", lbl_80498C48.points),
                    MenuItem("#locker", 2, cars.data + 0xC0, 0, 0),
                    MenuItem("#car_bar_color", bars[bar]),
                    MenuItem("#races_run", (char*)cars.data + 0x3C),
                    MenuItem("#aaa", (char*)cars.data + 0x50),
                    MenuItem(0, 5, cars.data, 0, 0),
                    MenuItem(lbl_8055B860, 6, (const void*)fn_8024ECD4, 0, 0x72),
                    MenuItem(lbl_8055B860, 6, 0, -1, 0x1B),
                    MenuItem(lbl_8055B860, 6, (const void*)fn_8024EC3C, -2, 0xD),
                    MenuItem(lbl_80498C30),
                };
                MenuDesc desc = {0, 0, (int (*)(int*))fn_8024DAD8, 0, 0};
                desc.layout = lbl_8055FC8C ? "carchoose2a_w.lyt" : "carchoose2a.lyt";
                desc.items = items;
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
                ColorPick paint(colors, lbl_80498C48.alt ? &pair->d : &pair->b,
                                lbl_80498C48.alt ? &pair->c : &pair->a, &panel, &ui, color);
                fn_8024EBAC(paint.data, fn_8024EBE4(paint.data), 1);
                u8* e = fn_8024EB9C(paint.data, 0);
                for (int i = 0; i < fn_8024EB94(paint.data); i++) {
                    e = fn_8024EB9C(paint.data, i);
                    S32(e, 0x10) = ((VehicleInfo*)PTR(e, 0x8))->colors[i];
                }
                fn_8024EA1C(paint.data);
                const char* bars[3] = {"car_bar1_e.art", "car_bar1_s.art", "car_bar1_m.art"};
                MenuItem items[] = {
                    MenuItem(0, 5, paint.data, 0, 0),
                    MenuItem("#car_model", 0x13, &ui, 0, 0),
                    MenuItem("#car_model", cars.Chassis()),
                    MenuItem("#car_name", cars.Name()),
                    MenuItem("#color_name", (char*)paint.data + 0xE0),
                    MenuItem("#car_position", 5, ui.state, 0, 0),
                    MenuItem("#locker", 2, cars.data + 0xC0, 0, 0),
                    MenuItem("#car_bar_color", bars[bar]),
                    MenuItem("#races_run", (char*)cars.data + 0x3C),
                    MenuItem("#aaa", (char*)cars.data + 0x50),
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
    GaragePair* pair = &lbl_80498C48.pairs[slot];
    *vehicle = lbl_80498C48.alt ? pair->c : pair->a;
    return r != -1;
}

extern "C" {

u8* fn_8024E6E4(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80344410;
        delete (VDel*)PTR(p, 0xD4);
        if (p) {
            *(void**)p = lbl_803444D8;
            delete[] PTR(p, 0xCC);
            if (--lbl_8055F2E8 == 0) {
                FreeIconPool(lbl_8055F2E4);
                lbl_8055F2E4 = 0;
            }
            if (p) {
                *(void**)p = lbl_8034453C;
                delete[] PTR(p, 0x18);
                fn_80235380(PTR(p, 0x24));
                fn_802CAC54(PTR(p, 0x20));
            }
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_8024E818(u8* p, int del) {
    if (p) {
        lbl_8055F2EC = 0;
        if (p) {
            *(void**)p = lbl_803444D8;
            delete[] PTR(p, 0xCC);
            if (--lbl_8055F2E8 == 0) {
                FreeIconPool(lbl_8055F2E4);
                lbl_8055F2E4 = 0;
            }
            if (p) {
                *(void**)p = lbl_8034453C;
                delete[] PTR(p, 0x18);
                fn_80235380(PTR(p, 0x24));
                fn_802CAC54(PTR(p, 0x20));
            }
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_8024E920(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_803444D8;
        delete[] PTR(p, 0xCC);
        if (--lbl_8055F2E8 == 0) {
            FreeIconPool(lbl_8055F2E4);
            lbl_8055F2E4 = 0;
        }
        if (p) {
            *(void**)p = lbl_8034453C;
            delete[] PTR(p, 0x18);
            fn_80235380(PTR(p, 0x24));
            fn_802CAC54(PTR(p, 0x20));
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

}  // extern "C"

// Tints the car with the selected slot's animation, or its saved paint.
static inline void ShowCarColor(u8* p) {
    VehicleInfo* v2 = FindVehicleNamed((const char*)(p + 0x60), 0);
    u32 col = GetSelColor(p);
    u32 anim = *(u32*)(PTR(p, 0xCC) + ((GarageSel*)p)->sel * 0x14 + 0x10);
    ((CarUIScreen*)PTR(p, 0x4C))->SetAnim(anim ? anim : ((u32*)((u8*)v2 + 0x64))[col]);
}

// Remembers colour c as the car's body (alt) or trim colour.
static inline void SaveColor(u8* p, s32 c) {
    VehicleInfo* v = FindVehicleNamed((const char*)(p + 0x60), 0);
    if (lbl_80498C48.alt) {
        GetVehicleColor(v, 0)[0] = c;
    } else {
        GetVehicleColor(v, 0)[1] = c;
    }
}

// Saves the selected colour for the car and shows it (tint and "UICar:COLOR%d" label).
static inline void ShowSelColor(u8* p) {
    VehicleInfo* v = FindVehicleNamed((const char*)(p + 0x60), 0);
    if (lbl_80498C48.alt) {
        GetVehicleColor(v, 0)[0] = S32(p, 0x2C);
    } else {
        GetVehicleColor(v, 0)[1] = S32(p, 0x2C);
    }
    *(u32*)PTR(p, 0xDC) = ((u32*)((u8*)v + 0x64))[((GarageSel*)p)->sel];
    ShowCarColor(p);
    fn_801345D8((char*)(p + 0xE0), 0x100, lbl_8034287C, ((GarageSel*)p)->sel + 1);
    const char* str = fn_80131234(p + 0xE0);
    CopyName(str, 0x100, (char*)(p + 0xE0));
}

extern "C" {

void fn_8024EA1C(u8* p) { ShowSelColor(p); }

u32 fn_8024EB94(u8* p) { return U32(p, 0x1c); }

u8* fn_8024EB9C(u8* p, int i) { return PTR(p, 0xCC) + i * 0x14; }

void fn_8024EBAC(u8* p, s32 v, int set) {
    S32(p, 0x2C) = v;
    if (set) {
        F32(p, 0x28) = (f32)v;
    }
}

u32 fn_8024EBE4(u8* p) { return GetSelColor(p); }

int fn_8024EC3C(void) {
    int ok = 0;
    if (fn_80129800(fn_802E4A10(), 5) && fn_80129800(fn_802E4A10(), 6)) {
        ok = 1;
    }
    if (ok || ((VList*)PTR(lbl_8055F2EC, 0xD0))->v1C(U32(lbl_8055F2EC, 0x2C))) {
        return 1;
    }
    fn_80216D28();
    fn_80214D50(1);
    return 0;
}

// The same bounded copy written with a destination cursor.
static inline void CopyNameD(const char* s, int n, char* d) {
    char* p = d;
    while ((*p++ = *s++) != 0) {
        if (--n <= 0) {
            p[-1] = 0;
            break;
        }
    }
}

// Shows car `sel` of the list: its name (the extreme version when owned), stats, unlock progress and paint.
static inline void ShowCar(u8* g, s32 sel) {
    const char* name = ((GarageList*)PTR(g, 0xD0))->Name(sel);
    VehicleInfo* info = FindVehicleNamed(name, 0);
    const char* base = name;
    if (((u8*)GetVehicleColor(info, 0))[8] && ((GarageList*)PTR(g, 0xD0))->IsExtreme(sel)) {
        name = ((GarageList*)PTR(g, 0xD0))->ExtremeName(sel);
    }
    VehicleInfo* v = FindVehicleNamed(name, 0);
    CopyNameD(v->chassis, 0x20, (char*)(g + 0x60));
    if (((GarageList*)PTR(g, 0xD0))->IsExtreme(sel)) {
        fn_801345D8((char*)(g + 0x80), 0x40, "\\extreme_star %s", v->display_name + 1);
    } else {
        CopyNameD(v->display_name, 0x40, (char*)(g + 0x80));
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
        fn_80130FCC((char*)(g + 0x3C), 0x10, AchievementsGetModelStat(0, (char*)(g + 0x60)));
        fn_80130FCC((char*)(g + 0x50), 0x10, AchievementsGetModelStat(1, (char*)(g + 0x60)));
    }
    ShowCarColor(g);
}

// Picks a random car for the garage and shows it.
int fn_8024ECD4(void) {
    s32 sel = fn_80129D18(1, S32(lbl_8055F2EC, 0x1C) - 1);
    u8* g = lbl_8055F2EC;
    S32(g, 0x2C) = sel;
    ShowCar(g, sel);
    return 0;
}

// The slot strip (vtable lbl_8034453C): one animated slot per entry of `list`, plus the icon frame art.
static inline void InitSlotStrip(u8* p, GarageList* list) {
    S32(p, 0x1C) = list->Count();
    *(void**)p = lbl_8034453C;
    S32(p, 0x14) = 0;
    PTR(p, 0x18) = (u8*)new ("icon") SlotAnim[S32(p, 0x1C)];
    for (int i = 0; i < S32(p, 0x1C); i++) {
        SlotAnim* s = &((SlotAnim*)PTR(p, 0x18))[i];
        s->sel = 0;
        s->shown = 0;
        s->t = lbl_80563960;
    }
    PTR(p, 0x24) = fn_8023512C("caricona.art");
    PTR(p, 0x20) = fn_802CAC28("cariconb.tex", 0);
}

// The car slot list (vtable lbl_803444D8, over the slot strip lbl_8034453C): one animated slot and one entry
// per car of `list`, starting on the car `car` points at.
u8* fn_8024F17C(u8* p, GarageList* list, s32* color, s32* car, StatPanel* panel, CarUIScreen* ui) {
    InitSlotStrip(p, list);
    S32(p, 0x2C) = 0;
    *(void**)p = lbl_803444D8;
    F32(p, 0x28) = S32(p, 0x2C);
    if (lbl_8055F2E8 == 0) {
        lbl_8055F2E4 = new ("cimgr") IconPool;
    }
    lbl_8055F2E8++;
    PTR(p, 0xC8) = (u8*)lbl_8055F2E4;
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
            e->icon[k] = FindIcon((IconPool*)PTR(p, 0xC8), name);
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
        s->t = lbl_80563960;
        e->value = 0;
    }
    fn_80131818(t);
    S32(p, 0x2C) = sel;
    F32(p, 0x28) = sel;
    ShowCar(p, sel);
    S32(p, 0xC0) = 1;
    return p;
}

u8* fn_8024FB64(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_8034453C;
        delete[] PTR(p, 0x18);
        fn_80235380(PTR(p, 0x24));
        fn_802CAC54(PTR(p, 0x20));
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

}  // extern "C"

extern void* lbl_803443B0[];

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
extern const f32 lbl_80563964;  // 1.0

// The car unlock screen's state (vtable lbl_803443B0): walks `list`, revealing each unlocked car with the
// trophy cue.
extern "C" {
void fn_80236954(const char* event);
void fn_80214AF0(int s);
f32 fn_80214D48(void);
}
extern const f32 lbl_80563970;  // 2.0

static inline f32 AbsF(f32 v) { return (f32)__fabs(v); }

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
            objects->SetVolume("trophy_snd", lbl_80563964);
        }
    }
    virtual ~UnlockScreen(void) {
        if (objects) {
            delete objects;
        }
    }
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
                CopyNameD(v->display_name, 0x40, name);
                if (!(v->flags & 2)) {
                    CopyNameD("EventPath:CAR_UNLOCKED", 0x80, message);
                    ui->SetAnim(v->colors[v->default_color]);
                } else {
                    CopyNameD("EventPath:EXTREME_UNLOCKED", 0x80, message);
                    if (color) {
                        ui->SetAnim(*color);
                    } else {
                        ui->SetAnim(v->colors[v->default_color]);
                    }
                }
                const char* text = fn_80131234((u8*)message);
                if (text != message) {
                    CopyNameD(text, 0x80, message);
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
                wait = lbl_80563970;
            }
        }
        panel->SetCar(model);
        f32 t = wait - fn_80214D48();
        wait = lbl_80563960 > t ? lbl_80563960 : t;
        if (AbsF(wait) <= lbl_80558DC4[0]) {
            fn_80214AF0(0x1F);
        }
        t = hold - fn_80214D48();
        hold = lbl_80563960 > t ? lbl_80563960 : t;
        if (AbsF(hold) <= lbl_80558DC4[0]) {
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
void fn_8024FBD8(GarageList* list, u32* color) {
    fn_8013C804();
    CarStatsInit();
    CarUIScreen ui(1);
    ui.channel = fn_802E4A10();
    StatPanel panel;
    panel.Clear();
    UnlockScreen screen(list, &ui, &panel, color);
    MenuItem items[] = {
        MenuItem(0, 5, &screen, 0, 0),
        MenuItem("#title", screen.title),
        MenuItem("#car_model", 0x13, &ui, 0, 0),
        MenuItem("#car_model", screen.model),
        MenuItem("#car_position", 5, ui.state, 0, 0),
        MenuItem("#unlock_msg", screen.message),
        MenuItem("#chassis", screen.name),
        MenuItem("#stat_text_1", panel.text[0]),
        MenuItem("#stat_art_1", 2, &panel.stats.stats[0].value, 0, 0),
        MenuItem("#stat_text_2", panel.text[1]),
        MenuItem("#stat_art_2", 2, &panel.stats.stats[1].value, 0, 0),
        MenuItem("#stat_text_3", panel.text[2]),
        MenuItem("#stat_art_3", 2, &panel.stats.stats[2].value, 0, 0),
        MenuItem("#stat_text_4", panel.text[3]),
        MenuItem("#stat_custom_4", 5, &panel.bars[3], 0, 0),
        MenuItem("#stat_text_5", panel.text[4]),
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

u8* fn_8024FFF8(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_803443B0;
        VDel* c = (VDel*)PTR(p, 0x188);
        if (c) {
            delete c;
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_8025007C(int n) {
    u8* o = (u8*)operator new(0x20, "ChassisList");
    if (o) {
        *(void**)o = lbl_80344384;
        u8* buf = (u8*)operator new[](n * 0x48, "dsa_buf");
        PTR(o, 0x4) = buf;
        S32(o, 0xC) = n;
        U32(o, 0x8) = 0;
        PTR(o, 0x10) = buf;
        __ct__9LocStringFPCc(o + 0x14, "EventPath:CONGRATULATIONS");
    }
    return o;
}

f32 fn_80214D48(void);
extern const f32 lbl_80563960;
extern const f32 lbl_80563964;
extern const f32 lbl_80563968;
extern const f32 lbl_8056396C;
extern const f32 lbl_80563970;

static inline void UpdateSlotAnims(u8* p) {
    int i;
    SlotAnim* e;
    f32 dt;
    dt = fn_80214D48();
    F32(p, 0x28) = F32(p, 0x28) + lbl_80563968 * ((f32)S32(p, 0x2C) - F32(p, 0x28));
    for (i = 0; i < S32(p, 0x1C); i++) {
        e = &((SlotAnim*)PTR(p, 0x18))[i];
        if (e->sel != e->shown) {
            e->t = e->t + lbl_8056396C * dt;
            if (e->t >= lbl_80563964) {
                e->shown = e->sel;
                e->t = e->t - lbl_80563970;
            }
        } else {
            f32 t = e->t + lbl_8056396C * dt;
            if (lbl_80563960 < t) {
                t = lbl_80563960;
            }
            e->t = t;
        }
    }
}

// Same body as UpdateSlotAnims; the declaration order here (found by decomp-permuter) gives the target's registers.
void fn_80250108(u8* p) {
    SlotAnim* e;
    f32 dt = fn_80214D48();
    int i;
    SlotAnim* slot;
    F32(p, 0x28) = F32(p, 0x28) + lbl_80563968 * ((f32)S32(p, 0x2C) - F32(p, 0x28));
    for (i = 0; i < S32(p, 0x1C); i++) {
        slot = &((SlotAnim*)PTR(p, 0x18))[i];
        e = slot;
        if (e->sel != e->shown) {
            e->t = e->t + lbl_8056396C * dt;
            if (e->t >= lbl_80563964) {
                e->shown = e->sel;
                e->t = e->t - lbl_80563970;
            }
        } else {
            f32 t = e->t + lbl_8056396C * dt;
            if (lbl_80563960 < t) {
                t = lbl_80563960;
            }
            e->t = t;
        }
    }
}

double fmod(double x, double y);

// Car list input: left/right step through the cars (bumping an end plays the error cue); "2" swaps an
// extreme car between its plain and extreme paint once its slot has settled (either of two keys).
int fn_80250204(u8* p, int key) {
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
                ShowCar(p, S32(p, 0x2C));
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

}  // extern "C"

extern const f32 lbl_80563980;  // 0.5
extern const f32 lbl_80563984;  // 255.0
extern const f32 lbl_80563988;  // 0.8
extern "C" {
f32 fn_80235EF0(u8* art, int part);
f32 fn_80235F64(u8* art, int part);
void fn_80231220(f32 x, f32 y, f32 w, f32 h, u8* tex, u32 color, f32 u0, f32 v0, f32 u1, f32 v1);
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
    f32 t = lbl_80563964 < v ? lbl_80563964 : v;
    return t > lbl_80563960 ? t : lbl_80563960;
}

static inline int RoundF(f32 v) { return (int)(v < lbl_80563960 ? v - lbl_80563980 : v + lbl_80563980); }

// Scales `c` by the tint `t`, per channel.
static inline u32 TintColor(u32 c, u32 t) {
    int a = t >> 24;
    int r = (t >> 16) & 0xFF;
    int g = (t >> 8) & 0xFF;
    int b = t & 0xFF;
    b = b * b * (int)(c & 0xFF) / 256 / 256;
    g = g * g * (int)((c >> 8) & 0xFF) / 256 / 256;
    r = a * r * (int)((c >> 16) & 0xFF) / 256 / 256;
    return 0xFF000000 | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}

extern "C" {

// Draws car slot `i`: its frame (greyed when locked), the two pulsing extreme stars, and the car icon painted
// with its colour.
void fn_80251078(u8* p, int i, s32 shown, f32 x, f32 y, f32 w, f32 h, f32 alpha) {
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
    fn_802354A0(PTR(p, 0x24), shown && extreme, locked ? 0xFF808080 : -1, x, y, lbl_80563960, w, h);
    f32 fade = fn_80216CB4();
    if (extreme) {
        for (int k = 0; k < 2; k++) {
            int part = 5;
            if (k == 0) {
                part = 4;
            }
            f32 t = (alpha - lbl_80563960) / lbl_80563964;
            f32 pw = fn_80235EF0(PTR(p, 0x24), part) * (lbl_80563980 + Clamp01(t) * lbl_80563980);
            f32 ph = fn_80235F64(PTR(p, 0x24), part) * (lbl_80563980 + Clamp01(t) * lbl_80563980);
            int a = RoundF(lbl_80563984 * fade);
            a = a > 0x13F ? 0x13F : a;
            fn_802354A0(PTR(p, 0x24), part, (a << 24) | 0xFFFFFF, (x + lbl_80563980 * w) - lbl_80563980 * pw,
                        k == 0 ? y + ph : y - h, lbl_80563960, pw, ph);
        }
    }
    int k = shown > 0 ? shown : 0;
    k = k >= 1 ? 1 : k;
    IconEntry* ic = &((IconPool*)PTR(p, 0xC8))->entries[e->icon[k]];
    f32 u1 = ic->u1;
    f32 v1 = ic->v1;
    f32 u2 = ic->u2;
    f32 v2 = ic->v2;
    fn_80231220(x + lbl_80563980 * (w - lbl_80563988 * w), y - lbl_80563980 * (h - lbl_80563988 * h),
                lbl_80563988 * w, lbl_80563988 * h, ic->tex, locked ? 0xFF000000 : -1, u1, v1, u2, v2);
    fn_80231220(x + lbl_80563980 * (w - lbl_80563988 * w), y - lbl_80563980 * (h - lbl_80563988 * h),
                lbl_80563988 * w, lbl_80563988 * h, ic->texE, TintColor(color, locked ? 0xFF000000 : -1), u1, v1,
                u2, v2);
}

void fn_802514B4(u8* p, int i, f32 x, f32 y, f32 w, f32 h) {
    s32 v = S32(PTR(p, 0xCC) + i * 0x14, 0x10);
    fn_802354A0(PTR(p, 0x24), 0, -1, x, y, 0.0f, w, h);
    fn_80216CB4();
    fn_802354A0(PTR(p, 0x24), 3, v, x, y, 0.0f, w, h);
    fn_802354A0(PTR(p, 0x24), 6, -1, x, y, 0.0f, w, h);
}

// Colour list input: left/right pick the previous/next colour; bumping an end plays the error cue.
int fn_8025157C(u8* p, int key) {
    switch (key) {
    case 0x125:
        if (S32(p, 0x2C) > 0) {
            s32 c = --S32(p, 0x2C);
            SaveColor(p, c);
            ShowSelColor(p);
        } else {
            fn_80216D28();
            fn_80214D50(1);
        }
        return 1;
    case 0x127:
        if (S32(p, 0x2C) < S32(p, 0x1C) - 1) {
            s32 c = ++S32(p, 0x2C);
            SaveColor(p, c);
            ShowSelColor(p);
        } else {
            fn_80216D28();
            fn_80214D50(1);
        }
        return 1;
    }
    return 0;
}

void fn_80214AF0(int s);

void fn_80251934(u8* p) {
    int on;
    SlotAnim* e;
    int i;
    u8* c;
    S32(p, 0xC0) = U8(PTR(p, 0x4C), 0xDC) == 0;
    for (i = 0; i < S32(p, 0x1C); i++) {
        c = PTR(p, 0xCC) + i * 0x14;
        on = 0;
        if (((u8*)GetVehicleColor(*(VehicleInfo**)(c + 8), 0))[8] && c[0xC]) {
            on = 1;
        }
        SlotAnim* slot = &((SlotAnim*)PTR(p, 0x18))[i];
        e = slot;
        if (on != e->sel) {
            e->sel = on;
            e->t = (f32)__fabs(e->t);
        }
    }
    UpdateSlotAnims(p);
    if (((VList*)PTR(p, 0xD0))->v1C(S32(p, 0x2C)) == 0) {
        fn_80214A9C(2);
    } else {
        fn_80214AF0(2);
    }
}

}  // extern "C"

// The garage screen's per-slot draw hook (vtable slot 0x60).
class GarageSlotView {
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
    virtual void DrawSlot(int i, s32 value, f32 x, f32 y, f32 w, f32 h, f32 alpha);  // 0x60
};

extern s32 lbl_8055CFF0;  // screen width
extern s32 lbl_8055CFF4;  // screen height

extern "C" {
void fn_80231220(f32 x, f32 y, f32 w, f32 h, u8* tex, u32 color, f32 u0, f32 v0, f32 u1, f32 v1);

static inline double AbsD(double v) { return __fabs(v); }

// Draws the slot strip: the backdrop, each slot sliding/fading by its distance from the cursor, and the frame.
void fn_80251B04(u8* p) {
    f32 top = lbl_8055CFF4 / 2 - 24.0f;
    f32 w = 1.7777778f * lbl_8055CFF0 / 1.3333334f;
    fn_80231220(-0.5f * w, 225.0f, w, 66.0f, PTR(p, 0x20), -1, 0.0f, 0.0f, 10.0f, 1.0f);
    f32 fade = fn_80216CB4();
    for (int i = 0; i < S32(p, 0x1C); i++) {
        SlotAnim* e = &((SlotAnim*)PTR(p, 0x18))[i];
        f32 s = 1.0f - (f32)__fabs(e->t);
        s = s < fade ? s : fade;
        f32 h = 48.0f * s;
        f32 d = (f32)AbsD(F32(p, 0x28) - i);
        f32 y = top - 0.5f * (48.0f * (1.0f - s));
        f32 x = 82.5f * (i - F32(p, 0x28)) - 41.25f;
        if (1.0f < d) {
            d = 1.0f;
        }
        d = d > 0.0f ? d : 0.0f;
        f32 alpha = 1.0f - d;
        ((GarageSlotView*)p)->DrawSlot(i, e->shown, x, y + 5.0f * s, 82.5f, h + 10.0f * s, alpha);
    }
    fn_802354A0(PTR(p, 0x24), 2, -1, -41.25f, 5.0f + top, 0.0f, 82.5f, 58.0f);
}

void fn_80251D0C(u8* p) {
    ((CarUIScreen*)PTR(p, 0x20))->UnloadModel();
    S32(p, 0x190) = -1;
    U8(p, 0x28) = 0;
    U8(p, 0x48) = 0;
    U8(p, 0x88) = 0;
    U8(p, 0x108) = 0;
    F32(p, 0x194) = 0.0f;
    F32(p, 0x198) = 20.0f;
    fn_80214A9C(0x1F);
    U8(p, 0x19C) = 0;
}

int fn_80252238(u8* p, int key) {
    if (key == 0xD) {
        if (AbsF(F32(p, 0x194)) <= lbl_80558DC4[0] || AbsF(F32(p, 0x198)) <= lbl_80558DC4[0]) {
            ((MenuState*)p)->Finish(0);
            return 1;
        }
    }
    return 0;
}

u8* fn_802522A8(u8* p, int del) {
    if (p) {
        if (p + 4) {
            delete[] PTR(p, 0x10);
            if (p + 4) {
                U32(p, 0x4) = 0;
                U32(p, 0x8) = 0;
                U32(p, 0xC) = 0;
            }
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_80252320(u8* p, const char* name, u8 a, u8 b) {
    u8* e = PTR(p, 4) + S32(p, 8)++ * 0x48;
    CopyNameA(name, 0x20, (char*)e);
    U8(e, 0x20) = a;
    U8(e, 0x41) = b;
    if (b) {
        const char* sp = GetSpecialChassis(name);
        CopyName(sp, 0x20, (char*)e + 0x21);
    } else {
        CopyNameTo(lbl_8055C0E8, 0x20, (char*)e + 0x21);
    }
    if (U32(p, 0x1C) != lbl_8055E044) {
        fn_80131318(p + 0x14);
    }
    S32(e, 0x44) = S32(p, 0x18);
}

void fn_80252454(u8* p, int i, s32 v) { S32(PTR(p, 0x4) + i * 0x48, 0x44) = v; }

u32 fn_80252468(u8* p) { return U32(p, 0x8); }

u8* fn_80252470(u8* p, int i) { return PTR(p, 0x4) + i * 0x48; }

u8 fn_80252480(u8* p, int i) { return U8(PTR(p, 0x4) + i * 0x48, 0x20); }

u8* fn_80252494(u8* p, int i) { return PTR(p, 0x4) + i * 0x48 + 0x21; }

u8 fn_802524A8(u8* p, int i) { return U8(PTR(p, 0x4) + i * 0x48, 0x41); }

s32 fn_802524BC(u8* p, int i) { return S32(PTR(p, 0x4) + i * 0x48, 0x44); }

}  // extern "C"
