// Car select widgets (0x8023FE20-0x80243460): the per-player join/leave widget ("JoinLeaveWidget"),
// the car icons ("caricona.art"), colour and stat labels ("UICar:COLOR%d", "UICar:STAT_%i_TEXT_%i")
// and the list selection state. Written against raw field offsets; the large routines are still
// stubs. Names are guesses apart from the strings.
#include "game/common.h"
#include "game/gart.h"
#include "game/gamedb.h"

// Stat level thresholds (defined first: its .rodata precedes the unit's initialiser templates).
extern const s32 lbl_802FA300[3] = {0, 4, 7};  // stat level thresholds
#include "game/menu.h"
#include "game/menuwidget.h"
#include "game/menuwidget_inl.h"
#include "game/menustate.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

#include "game/vdel.h"

class VU {
public:
    virtual ~VU(void);  // 0x08 (the VehicleList (lbl_80344384): slot 0 fn_802522A8 is its destructor)
    virtual void v0C(void);
    virtual void v10(void);
    virtual int v14(void);
    virtual const char* v18(int i);
    virtual u8* v1C(int i);
};

extern "C" {
void fn_802E317C(const char* fmt, ...);
int fn_802E4A10(void);
void fn_802328D4(u8* p);
u8* fn_80232578(const char* name);
int fn_802E8504(int ch);
void __construct_array(void* p, void* ctor, void* dtor, u32 size, u32 n);
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
extern s32 lbl_8055F1F4;

ModuleTag lbl_8055F1E8;
UnkStatic123s lbl_80498810;
JoinSlot lbl_80498828[2];
UnkStatic7F lbl_8055F1F0;
s32 lbl_8055F1F4;
s32 lbl_8055F1F8;
u8 lbl_8055F1FC;
u32 lbl_8055F200;

extern "C" {

// Clears both join slots.
void fn_8023FE20(void) {
    JoinSlot* j = lbl_80498828;
    j++->id = -1;
    j->id = -1;
}

// Number of joined players.
int fn_8023FE34(void) {
    int n = 0;
    for (int i = 0; i < 2; i++) {
        if (lbl_80498828[i].id >= 0) {
            n++;
        }
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

}  // extern "C"

// One row of a player's car choice list: a menu value widget (sel = flag0, value = id).
typedef MenuWidgetBase CarSelItem;

static inline void SetItem(MenuWidgetBase* w, u8 s, u32 v) {  /* Helper name unproven */
    w->flag0 = s;
    w->id = v;
}


// Copies at most n - 1 characters and always terminates.


extern "C" {
int fn_801345D8(char* buf, int n, const char* fmt, ...);
const char* fn_80131234(const char* key);
VU* fn_8027DB40(int kind);
const char* fn_802E85B8(s32 slot);
}


#include "game/roll_strings.h"

// One stat bar: a menu state that draws one stat's caption (class name from garage.cpp).
class StatBar : public MenuState {
public:
    StatBar(void);
    virtual ~StatBar(void);
    virtual void Exit(void);
    virtual void Update(void);
    virtual void Draw(void);
    virtual void Set(int i, s32* value, u8* flag);

    u8* font;    // offset 0x18
    s32 index;   // offset 0x1C: stat number + 1 (-1 = none)
    u8* flag;    // offset 0x20
    s32* value;  // offset 0x24
};

#include "game/vehcolor.h"

#include "game/carstat.h"
#include "game/strcopy.h"

struct CarStats {
    const char* chassis;  // offset 0x0
    CarStat stats[5];     // offset 0x4
};

class CarUIScreen {
public:
    CarUIScreen(int mode);
    virtual ~CarUIScreen(void);
    void SetAnim(u32 a);
    u8 pad4[0xD8];
    u8 tint;          // offset 0xDC
    bool nextTint;    // offset 0xDD
    u8 padDE[0x1E];
    u8 state[0x1C];   // offset 0xFC, the view's menu widget
    u8 pad118[0xC];
};

VehicleColor* GetVehicleColor(VehicleInfo* v, int slot);
CarStats* CarStatsLookup(const char* chassis);


static inline s32 Min(s32 a, s32 b) { return b < a ? b : a; }

// The body colour chooser of the car picker.
extern "C" const f32 lbl_80563758;  // 0.0
extern "C" const f32 lbl_80563764;  // 1.0
extern "C" const f32 lbl_80563768;  // 1000.0
extern "C" const f32 lbl_8056376C;  // 250.0
extern "C" const f32 lbl_80563770;  // -1.0
extern "C" const f32 lbl_8056377C;  // 5.0
extern "C" {
f32 fn_8023659C(void);
void fn_80216D28(void);
void fn_80214D50(int s);
}

static inline f32 Clamp01(f32 v) {
    f32 t = lbl_80563764 < v ? lbl_80563764 : v;
    return t > lbl_80563758 ? t : lbl_80563758;
}

extern "C" const f32 lbl_80563774;
extern "C" const f32 lbl_80563778;

// Odd smoothstep over [-1, 1]: 3t^2 - 2t^3 mirrored for negative t.
static inline f32 Ease(f32 t) {
    if (t < lbl_80563770) {
        return lbl_80563770;
    }
    if (t > lbl_80563764) {
        return lbl_80563764;
    }
    if (t > lbl_80563758) {
        const f32 cubic = lbl_80563774 * t;
        const f32 quadratic = lbl_80563778 * t;
        return cubic * t * t + quadratic * t;
    }
    const f32 cubic = lbl_80563774 * t;
    const f32 quadratic = lbl_80563778 * t;
    return cubic * t * t - quadratic * t;
}

// The body colour chooser of the car picker: a menu value widget (MenuWidgetBase at +0) that is also a menu
// state (MenuState at +0x10); the compiler emits its this-0x10 thunks at the end of the unit (0x80243460).
class ColorPick : public MenuWidgetBase, public MenuState {  /* Class name unproven */
public:
    ColorPick(u32* rgb, s32* sel, CarUIScreen* scr);
    virtual ~ColorPick(void) {
        lbl_8055F1F4--;
        fn_80235380(art);
    }
    virtual int Key(int key);
    virtual void Draw(void);

    VehicleInfo* vehicle;  // offset 0x28
    s32 color;             // offset 0x2C
    s32* colorOut;         // offset 0x30
    u32* rgbOut;           // offset 0x34
    CarUIScreen* screen;   // offset 0x38
    s32 slot;              // offset 0x3C
    u8 pad40[0x4];
    gArtSet* art;            // offset 0x44
    s32 time;              // offset 0x48, ms when the slide started
    f32 pos;               // offset 0x4C
    f32 target;            // offset 0x50
    f32 leftFlash;         // offset 0x54
    f32 rightFlash;        // offset 0x58
    char label[0x40];      // offset 0x5C

    // Remembers the chosen colour for the current car.
    void SaveColor(void) {
        if (vehicle) {
            GetVehicleColor(vehicle, slot)->trim = color;
        }
    }

    // Shows colour `color`: tints the car and names it ("UICar:COLOR%d").
    void ShowColor(void) {
        label[0] = 0;
        if (color >= 0 && color < vehicle->num_colors) {
            u32 rgb = vehicle->colors[color];
            *rgbOut = rgb;
            screen->SetAnim(rgb);
            fn_801345D8(label, 0x40, RollStrings::ColorKey(), color + 1);
            const char* text = fn_80131234(label);
            SafeStrCopy(label, text, 0x40);
        }
        *colorOut = color;
    }

    // Starts sliding the swatch strip towards the new colour.
    void Slide(void) {
        s32 now = fn_8023659C() * lbl_80563768;
        f32 t = Clamp01((f32)(now - time) / lbl_8056376C);
        f32 e = Ease(t);
        time = now;
        pos = pos + e * (target - pos);
        target = color;
    }

    // Saves the colour chosen for the old car, then shows the saved colour of v.
    void SetVehicle(VehicleInfo* v) {
        SaveColor();
        vehicle = v;
        color = Min(color, v->num_colors - 1);
        color = GetVehicleColor(vehicle, slot)->trim;
        ShowColor();
        time = -250;
        pos = color;
        target = color;
    }
};


// A player's car choice list: a menu state over the rows (class name unproven).
class CarSel : public MenuState {
public:
    CarSel(s32* slot, u8* car);
    virtual ~CarSel(void) {}
    virtual void Enter(void);
    virtual int Key(int key);

    CarSelItem* items[4];   // offset 0x18
    u8* car;                // offset 0x28
    s32 count;              // offset 0x2C
    s32 cur;                // offset 0x30
    s32* slot;              // offset 0x34
    u8 active;              // offset 0x38
    char name[0x20];        // offset 0x39

    // Marks the current row (when active) and gives every row the slot.
    void Refresh(void) {
        for (int i = 0; i < count; i++) {
            SetItem(items[i], active && i == cur, *slot);
        }
    }
    void SetSlot(s32 s);
    void SetCur(s32 c) {
        cur = c;
        Refresh();
    }
    // True when the list is active and every row has been set.
    u8 Confirmable(void) {
        if (active) {
            for (int i = 0; i < count; i++) {
                if (!items[i]->vf14()) {
                    return 0;
                }
            }
        }
        return active;
    }
    void SetActive(u8 a) {
        active = a;
        Refresh();
    }
};

// Shows controller slot s (no name when negative).
inline void CarSel::SetSlot(s32 s) {
    *slot = s;
    S32(car, 0xD8) = *slot;
    if (*slot < 0) {
        SafeStrCopy(name, "", 0x20);
    } else {
        SafeStrCopy(name, fn_802E85B8(*slot), 0x20);
    }
    Refresh();
}

// The stat panel of the car picker: the car's stats, one bar and caption per stat.
struct StatPanel {
    CarStats stats;        // offset 0x0
    StatBar bars[5];       // offset 0x2C
    char text[5][0x64];    // offset 0xF4
    u8 flag;               // offset 0x2E8

    StatPanel(void);

    void SetCar(const char* chassis) {
        CarStats* cs = CarStatsLookup(chassis);
        if (cs) {
            stats = *cs;
            for (int i = 0; i < 5; i++) {
                if (stats.stats[i].name) {
                    SafeStrCopy(text[i], stats.stats[i].name, 0x64);
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
class CarPicker : public MenuWidget {  /* Class name unproven */
public:
    CarPicker(s32* unk9COut, s32* idOut, ColorPick* colors, StatPanel* stats, CarUIScreen* screen);
    virtual ~CarPicker(void) { delete (VDel*)list; }
    virtual const char* Current(void);
    virtual void Prev(void);
    virtual void Next(void);
    virtual u8* Item(void);  /* Helper name unproven */

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
            SafeStrCopy(chassis, c, 0x20);
            SafeStrCopy(name, v->display_name, 0x20);
            item = list->v1C(index);
            screen->nextTint = item == 0;
            colors->SetVehicle(v);
            stats->SetCar(chassis);
        }
    }
};


// One player's focus group in the join/leave widget.
struct FocusGroup {
    CarSel* sel;  // offset 0x0
    int join;     // offset 0x4
    int car;      // offset 0x8
    int leave;    // offset 0xC
    int start;    // offset 0x10
    int ready;    // offset 0x14
    s32 prev;     // offset 0x18
    s32 state;    // offset 0x1C
};

extern s32 lbl_8055F1F8;

extern "C" {
void fn_80214AF0(int s);
void fn_80214A9C(int s);
}

class JoinLeaveWidget : public MenuState {
public:
    JoinLeaveWidget(void) { memset(groups, 0, sizeof(groups)); }
    virtual ~JoinLeaveWidget(void) {}
    virtual void Enter(void);
    virtual void Update(void);
    virtual int Key(int key);

    FocusGroup groups[2];  // offset 0x18
    u8 allReady;           // offset 0x58

    // True when every player has joined and confirmed (state 4); none may be in state 2 or 3.
    u32 AllReady(void) {
        int n = 0;
        for (int i = 0; i < 2; i++) {
            s32 st = groups[i].state;
            if (st > 1 && st != 4) {
                return 0;
            }
            if (st == 4) {
                n++;
            }
        }
        return n >= 2;
    }

    // Counts the groups past the join prompt that have not confirmed yet.
    int Unconfirmed(int k) {
        int n = 0;
        for (int i = 0; i < 2; i++) {
            if (groups[k].state > 1 && groups[k].state != 4) {
                n++;
            }
        }
        return n;
    }

    // Shows the widgets of each focus group whose state changed.
    void UpdateFocus(void) {
        u32 ready = AllReady();
        if (allReady != ready) {
            groups[0].prev = 0;
            groups[1].prev = 0;
        }
        for (u32 i = 0; i < 2; i++) {
            FocusGroup* g = &groups[i];
            if (g->state != g->prev) {
                switch (g->state) {
                case 0:
                    break;
                case 1:
                    fn_80214AF0(g->join);
                    fn_80214A9C(g->car);
                    fn_80214A9C(g->leave);
                    break;
                case 2:
                case 3:
                    fn_80214A9C(g->join);
                    fn_80214AF0(g->car);
                    fn_80214A9C(g->leave);
                    fn_80214AF0(g->ready);
                    if (i != 0 && g->state == 2) {
                        fn_80214A9C(g->start);
                    } else {
                        fn_80214AF0(g->start);
                    }
                    g->sel->SetActive(1);
                    break;
                case 4:
                    fn_80214A9C(g->join);
                    fn_80214AF0(g->car);
                    fn_80214AF0(g->leave);
                    fn_80214A9C(g->ready);
                    fn_80214AF0(g->start);
                    g->sel->SetActive(0);
                    break;
                default:
                    fn_802E317C(RollStrings::InvalidState(), g->state, i);
                    break;
                }
                g->prev = g->state;
            }
        }
        (ready ? fn_80214AF0 : fn_80214A9C)(0x1E);
        if (ready) {
            for (int j = 7; j <= 10; j++) {
                fn_80214A9C(j);
            }
        }
        allReady = ready;
    }
};

// Gives the next free focus group choice list s and its widget ids.
static inline void AddFocus(JoinLeaveWidget* w, CarSel* s, int join, int car, int leave, int start, int ready) {
    for (int i = 0; i < 2; i++) {
        FocusGroup* g = &w->groups[i];
        if (!g->sel) {
            g->sel = s;
            g->car = car;
            g->join = join;
            g->leave = leave;
            g->start = start;
            g->ready = ready;
            g->prev = 0;
            if (*s->slot < 0) {
                g->state = 1;
            } else {
                g->state = 2;
            }
            return;
        }
    }
    fn_802E317C("JoinLeaveWidget: tried to add too many focii! (%d is the max)", 2);
}

extern u8 lbl_8055FC8C;  // wide-screen layout
void CarStatsInit(void);
void CarStatsShutdown(void);
extern "C" {
void fn_8013C804(void);
void fn_8013C8B8(void);
void fn_80129AEC(int a);
int fn_802E4A10(void);
void fn_8023D4B8(s32 ch);
void __destroy_arr(void* p, void* dtor, u32 size, u32 n);
void fn_802409AC(CarSel* p, CarSelItem* item);
}  // extern "C"


extern "C" {
void fn_802409A4(u8* p, u8 v);
int fn_802411A4(int* out);
}

// One player's half of the roll-call screen: the car view, colour chooser, stat panel, car picker and the
// choice list over them. The chooser and stat panel are built by their (out-of-line) constructors.
struct RollPanel {
    CarUIScreen screen;  // offset 0x0
    ColorPick color;     // offset 0x124
    StatPanel stats;     // offset 0x1C0
    CarPicker picker;    // offset 0x4AC
    CarSel sel;          // offset 0x51C
    s32 colorIndex;      // offset 0x578

    RollPanel(JoinSlot* s, u8 flag) : screen(0), color(&s->color, &colorIndex, &screen), stats(),
        picker((s32*)&s->unk4, (s32*)&s->car, &color, &stats, &screen), sel(&s->id, (u8*)&screen) {
        fn_802409AC(&sel, &picker);
        fn_802409AC(&sel, &color);
        fn_802409A4((u8*)&stats, flag);
    }
    ~RollPanel(void) {
    }
};

// Both players' panels.
struct RollPanels {
    RollPanel p1;  // offset 0x0
    RollPanel p2;  // offset 0x57C
    RollPanels(u8 flag) : p1(&lbl_80498828[0], flag), p2(&lbl_80498828[1], flag) {}
};

extern "C" {

// MenuDoRollCall: both players pick a car and colour ("rollcall2.lyt"); returns 1 to go on, 0 to go back.
int fn_8023FF1C(u8 flag) {
    CarStatsInit();
    fn_8013C804();
    RollPanels pp(flag);
    JoinLeaveWidget w;
    AddFocus(&w, &pp.p1.sel, 3, 1, 5, 7, 9);
    AddFocus(&w, &pp.p2.sel, 4, 2, 6, 8, 10);
    MenuItem items[] = {
        MenuItem(NULL, 5, &w, 0, 0),
        MenuItem(NULL, 5, &pp.p1.sel, 0, 0),
        MenuItem("#carmodel_1", 0x13, &pp.p1.screen, 0, 0),
        MenuItem("#carmodel_1", 3, pp.p1.picker.chassis, 0, 0),
        MenuItem("#model_1", 0x10, NULL, (MenuArg)&pp.p1.picker, 0),
        MenuItem("#color_1", 5, static_cast<MenuState*>(&pp.p1.color), 0, 0),
        MenuItem("#locker_1", 2, &pp.p1.picker.item, 0, 0),
        MenuItem("#controller_1", pp.p1.sel.name),
        MenuItem("#car_position_1", 5, pp.p1.screen.state, 0, 0),
        MenuItem("#stat_text_1_p1", CaptionContext(), pp.p1.stats.text[0]),
        MenuItem("#stat_art_1_p1", 2, &pp.p1.stats.stats.stats[0].value, 0, 0),
        MenuItem("#stat_text_2_p1", CaptionContext(), pp.p1.stats.text[1]),
        MenuItem("#stat_art_2_p1", 2, &pp.p1.stats.stats.stats[1].value, 0, 0),
        MenuItem("#stat_text_3_p1", pp.p1.stats.text[2]),
        MenuItem("#stat_art_3_p1", 2, &pp.p1.stats.stats.stats[2].value, 0, 0),
        MenuItem("#stat_text_4_p1", 3, pp.p1.stats.text[3], 0, 0),
        MenuItem("#stat_custom_4_p1", 5, &pp.p1.stats.bars[3], 0, 0),
        MenuItem("#stat_text_5_p1", 3, pp.p1.stats.text[4], 0, 0),
        MenuItem("#stat_art_5_p1", 2, &pp.p1.stats.stats.stats[4].value, 0, 0),
        MenuItem(NULL, 5, &pp.p2.sel, 0, 0),
        MenuItem("#carmodel_2", 0x13, &pp.p2.screen, 0, 0),
        MenuItem("#carmodel_2", CaptionContext(), pp.p2.picker.chassis),
        MenuItem("#model_2", 0x10, NULL, (MenuArg)&pp.p2.picker, 0),
        MenuItem("#color_2", 5, static_cast<MenuState*>(&pp.p2.color), 0, 0),
        MenuItem("#locker_2", 2, &pp.p2.picker.item, 0, 0),
        MenuItem("#controller_2", CaptionContext(), pp.p2.sel.name),
        MenuItem("#car_position_2", 5, pp.p2.screen.state, 0, 0),
        MenuItem("#stat_text_1_p2", 3, pp.p2.stats.text[0], 0, 0),
        MenuItem("#stat_art_1_p2", 2, &pp.p2.stats.stats.stats[0].value, 0, 0),
        MenuItem("#stat_text_2_p2", 3, pp.p2.stats.text[1], 0, 0),
        MenuItem("#stat_art_2_p2", 2, &pp.p2.stats.stats.stats[1].value, 0, 0),
        MenuItem("#stat_text_3_p2", CaptionContext(), pp.p2.stats.text[2]),
        MenuItem("#stat_art_3_p2", 2, &pp.p2.stats.stats.stats[2].value, 0, 0),
        MenuItem("#stat_text_4_p2", CaptionContext(), pp.p2.stats.text[3]),
        MenuItem("#stat_custom_4_p2", 5, &pp.p2.stats.bars[3], 0, 0),
        MenuItem("#stat_text_5_p2", CaptionContext(), pp.p2.stats.text[4]),
        MenuItem("#stat_art_5_p2", 2, &pp.p2.stats.stats.stats[4].value, 0, 0),
        MenuItem(lbl_80498810),
    };
    const char* layout = lbl_8055FC8C ? "rollcall2_w.lyt" : "rollcall2.lyt";
    MenuDesc desc = {layout, items, fn_802411A4, 0, 0};
    fn_80129AEC(-1);
    int result = 1;
    int run = 1;
    while (run) {
        int r = fn_802146BC(&desc, -999, -999, 1);
        switch (r) {
        case 0:
            result = 1;
            run = 0;
            break;
        case 1:
            fn_8023D4B8(fn_802E4A10());
            break;
        case 2:
            result = 0;
            run = 0;
            break;
        default:
            fn_802E317C("MenuDoRollCall: received an invalid return value (%d) from the dialog!", r);
            break;
        }
    }
    fn_80129AEC(0);
    fn_8013C8B8();
    CarStatsShutdown();
    return result;
}

}  // extern "C"

// Out of line: garage.cpp destroys stat bars through this destructor (retail's copy is a global function here).
StatBar::~StatBar(void) {
    value = 0;
    flag = 0;
    fn_802328D4(font);
    font = 0;
}

extern "C" {

void fn_802409A4(u8* p, u8 v) { U8(p, 0x2E8) = v; }

}  // extern "C"

extern "C" {
const char* fn_802E85B8(s32 slot);

void fn_802409AC(CarSel* p, CarSelItem* item) {
    int n = p->count;
    p->items[n] = item;
    p->count = n + 1;
    p->Refresh();
}


}  // extern "C"

CarSel::CarSel(s32* slot, u8* car) {
    this->car = car;
    this->count = 0;
    this->cur = 0;
    this->slot = slot;
    this->active = 1;
    SafeStrCopy(this->name, "", 0x20);
    S32(this->car, 0xD8) = *this->slot;
    if (*this->slot < 0) {
        SafeStrCopy(this->name, "", 0x20);
    } else {
        const char* nm = fn_802E85B8(*this->slot);
        SafeStrCopy(this->name, nm, 0x20);
    }
    this->Refresh();
}

extern "C" {
}  // extern "C"

CarPicker::CarPicker(s32* unk9COut, s32* idOut, ColorPick* colors, StatPanel* stats, CarUIScreen* screen) {
    this->unk9COut = unk9COut;
    this->idOut = idOut;
    this->colors = colors;
    this->stats = stats;
    this->screen = screen;
    this->list = fn_8027DB40(6);
    flag8 = 1;
    this->index = 0;
    int n = this->list->v14();
    for (int i = 0; i < n; i++) {
        VehicleInfo* v = FindVehicleNamed(this->list->v18(i), 0);
        if (v->id == *idOut) {
            this->index = i;
            break;
        }
    }
    this->Select();
}

extern "C" {

// Car picker constructor: starts on the car whose id is *idOut (the first car if none matches).




}  // extern "C"

ColorPick::ColorPick(u32* rgb, s32* sel, CarUIScreen* scr) : MenuWidgetBase(1) {
    vehicle = 0;
    color = *sel;
    colorOut = sel;
    rgbOut = rgb;
    screen = scr;
    slot = lbl_8055F1F4++;
    art = fn_8023512C("caricona.art");
    rightFlash = lbl_80563758;
    leftFlash = lbl_80563758;
    label[0] = 0;
}

extern "C" {

int fn_802411A4(int* out) {
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

}  // extern "C"

void StatBar::Set(int i, s32* v, u8* f) {
    value = v;
    index = i + 1;
    flag = f;
}

StatPanel::StatPanel(void) {
    memset(this, 0, sizeof(CarStats));
    text[0][0] = 0;
    text[1][0] = 0;
    text[2][0] = 0;
    text[3][0] = 0;
    text[4][0] = 0;
}

StatBar::StatBar(void) {
    index = -1;
    value = 0;
    flag = 0;
    font = fn_80232578(RollStrings::StatFont());
}

void StatBar::Exit(void) {}

void StatBar::Update(void) {}

extern "C" {

void fn_80230C30(f32* x, f32* y, s32 a, s32 b);
int fn_801345D8(char* buf, int n, const char* fmt, ...);
const char* fn_80131234(const char* key);
void fn_80232BA4(const char* text, u8* font, f32 x, f32 y, s32 color, int align, f32 size, f32 a, f32 b);
extern const f32 lbl_80563758;     // 0.0
extern const f32 lbl_8056375C;     // 0.43
extern const f32 lbl_80563760;     // 0.37
extern const f32 lbl_80563764;     // 1.0

// Draws the text for one car stat at its level ("UICar:STAT_%i_TEXT_%i").
// The stat value the bars compare against (field 0x24 points at it).
static inline s32 StatOf(const u8* p) { return *(s32*)PTR(p, 0x24); }

}  // extern "C"

void StatBar::Draw(void) {
    StatBar* p = this;
    f32 x, y;
    fn_80230C30(&x, &y, p->x, p->y);
    if (p->index >= 0) {
        int lvl = 0;
        for (int i = 0; i < 3; i++) {
            s32 v = StatOf((u8*)p);
            if (v && v >= lbl_802FA300[i]) {
                lvl = i + 1;
            }
        }
        char buf[0x1E];
        fn_801345D8(buf, 0x1E, RollStrings::StatKey(), p->index, lvl);
        int align = 10;
        f32 size = lbl_8056375C;
        if (*p->flag) {
            size = lbl_80563760;
            align = 9;
        }
        fn_80232BA4(fn_80131234(buf), p->font, x, y, -0x100, align, size, lbl_80563758, lbl_80563764);
    }
}

extern "C" {

// Colour chooser input: right/left step through the car's colours; bumping an end leaves the menu.
int ColorPick::Key(int key) {
    int handled = 0;
    if (this->vf10()) {
        handled = 1;
        switch (key) {
        case 0x127: {
            bool end = false;
            if (++this->color >= this->vehicle->num_colors) {
                this->color = this->vehicle->num_colors - 1;
                end = true;
            }
            if (end) {
                fn_80216D28();
                fn_80214D50(1);
                break;
            }
            this->SaveColor();
            this->ShowColor();
            this->Slide();
            this->rightFlash = lbl_8056377C;
            break;
        }
        case 0x125: {
            bool end = false;
            if (--this->color < 0) {
                this->color = 0;
                end = true;
            }
            if (end) {
                fn_80216D28();
                fn_80214D50(1);
                break;
            }
            this->SaveColor();
            this->ShowColor();
            this->Slide();
            this->leftFlash = lbl_8056377C;
            break;
        }
        default:
            handled = 0;
            break;
        }
    }
    return handled;
}

}  // extern "C"
gArtSet* fn_80211660(long i);  // uiapi.cpp: the lite-mode blinker art
extern "C" {
void fn_8022BD40(int font, f32 x, f32 y, f32 z, f32 w, f32 h, f32 scale, const char* text, int a, int b);
extern const f32 lbl_80563780;  // 3.5 swatches across
extern const f32 lbl_80563784;  // 0.5
extern const f32 lbl_80563788;  // 0.3 edge alpha
extern const f32 lbl_8056378C;  // 0.7
extern const f32 lbl_80563790;  // 255.0
extern const f32 lbl_80563794;  // 4.0 arrow gap

// Colour chooser: a sliding strip of colour swatches (faded and clipped at the edges) with arrows that
// flash when stepped; without a car (or hidden) it shows its label instead.
// Helper name unproven: half of a size, the value stays the left operand.
static inline f32 HalfF(f32 v) { return v * lbl_80563784; }

void ColorPick::Draw(void) {
    if (this->vehicle && this->vf0C()) {
        f32 x;
        f32 y;
        fn_80230C30(&x, &y, this->x, this->y);
        f32 w = this->w;
        f32 h;
        h = this->h;
        f32 cell = w / lbl_80563780;
        s32 now = fn_8023659C() * lbl_80563768;
        f32 t = Clamp01((f32)(now - this->time) / lbl_8056376C);
        f32 e = Ease(t);
        f32 cur = this->pos + e * (this->target - this->pos);
        f32 start = x + w * lbl_80563784 - cell * lbl_80563784 - cell * cur;
        // The swatch colour and its frame.
        struct Swatch {  /* Helper name unproven */
            u32 tint;
            u32 frame;
            Swatch() : tint(0), frame(0) {}
        } sw;
        int i;
        for (i = 0; i < this->vehicle->num_colors; i++) {
            f32 sx = start + (f32)i * cell;
            if (sx + cell > x && sx < x + w) {
                f32 d = sx + cell * lbl_80563784 - x;
                if (d > w * lbl_80563784) {
                    d = w - d;
                }
                f32 a = Clamp01((d - lbl_80563758) / (w * lbl_80563784 - lbl_80563758));
                u8 alpha = (s32)(lbl_80563790 * (lbl_80563788 + a * lbl_8056378C));
                sw.tint = (this->vehicle->colors[i] & 0xFFFFFF) | (alpha << 24);
                sw.frame = (alpha << 24) | 0xFFFFFF;
                if (sx > x && sx + cell < x + w) {
                    fn_802354A0(this->art, 3, sx, y, lbl_80563758, cell, h, sw.tint);
                    fn_802354A0(this->art, 6, sx, y, lbl_80563758, cell, h, sw.frame);
                } else if (sx > x) {
                    f32 vis = cell - (sx + cell - (x + w));
                    f32 u = vis / cell;
                    fn_80235758(this->art, 3, sx, y, vis, h, lbl_80563758, lbl_80563758, u, lbl_80563764, sw.tint);
                    fn_80235758(this->art, 6, sx, y, vis, h, lbl_80563758, lbl_80563758, u, lbl_80563764, sw.frame);
                } else {
                    f32 vis = cell - (x - sx);
                    f32 u = (cell - vis) / cell;
                    fn_80235758(this->art, 3, x, y, vis, h, u, lbl_80563758, lbl_80563764, lbl_80563764, sw.tint);
                    fn_80235758(this->art, 6, x, y, vis, h, u, lbl_80563758, lbl_80563764, lbl_80563764, sw.frame);
                }
            }
        }
        if (this->vf0C()) {
            if (this->color > 0) {
                gArtSet* art = fn_80211660(0);
                f32 ah = fn_80235F64(art, 1);
                f32 ay = y - HalfF(h - ah);
                f32 gap = lbl_80563794 + this->leftFlash;
                fn_802353F4(art, 1, x - fn_80235EF0(art, 1) - gap, ay, lbl_80563758);
            }
            if (this->color < this->vehicle->num_colors - 1) {
                gArtSet* art = fn_80211660(1);
                f32 ay = y - HalfF(h - fn_80235F64(art, 1));
                fn_802353F4(art, 1, lbl_80563794 + this->rightFlash + (x + w), ay,
                            lbl_80563758);
            }
            f32 l = this->leftFlash - lbl_80563764;
            this->leftFlash = lbl_80563758 > l ? lbl_80563758 : l;
            f32 r = this->rightFlash - lbl_80563764;
            this->rightFlash = lbl_80563758 > r ? lbl_80563758 : r;
        }
    } else {
        f32 x = this->x;
        f32 y = this->y;
        f32 w = this->w;
        f32 h = this->h;
        fn_8022BD40(0xCC, x + lbl_80563784 * w, y + lbl_80563784 * h, lbl_80563758, w, h, lbl_80563764, this->label, 0, 0);
    }
}

}  // extern "C"

void CarSel::Enter(void) { Refresh(); }

extern "C" {

}  // extern "C"

int CarSel::Key(int key) {
    if (*this->slot != (s32)fn_802E4A10() || !this->active) {
        return 0;
    }
    this->Refresh();
    return 1;
}

extern "C" {

}  // extern "C"

const char* CarPicker::Current(void) { return name; }

extern "C" {

}  // extern "C"

void CarPicker::Next(void) {
    this->index++;
    if (this->index >= this->list->v14()) {
        this->index = 0;
    }
    this->Select();
}

extern "C" {

}  // extern "C"

void CarPicker::Prev(void) {
    if (--this->index < 0) {
        this->index = this->list->v14() - 1;
    }
    this->Select();
}

extern "C" {

}  // extern "C"

u8* CarPicker::Item(void) {
    CarPicker* p = this;
    s32 i = p->index;
    if (i >= 0 && i < p->list->v14()) {
        return p->list->v1C(i);
    }
    return 0;
}

extern "C" {

}  // extern "C"

extern "C" {

}  // extern "C"

void JoinLeaveWidget::Enter(void) {
    lbl_8055F1FC = 0;
    for (int i = 0; i < 2; i++) {
        this->groups[i].sel->SetSlot(i);
        this->groups[i].state = 2;
    }
    this->allReady = 0;
    this->groups[0].prev = 0;
    this->groups[1].prev = 0;
    this->UpdateFocus();
}

extern "C" {

// Every tenth frame, drops players whose controller is gone back to the join prompt.
}  // extern "C"

void JoinLeaveWidget::Update(void) {
    if (++lbl_8055F1F8 % 10 == 0) {
        for (u32 i = 0; i < 2; i++) {
            if (this->groups[i].state > 1 && !fn_802E8504(*this->groups[i].sel->slot)) {
                FocusGroup* g = &this->groups[i];
                g->sel->SetSlot(-1);
                g->state = 1;
            }
        }
    }
    this->UpdateFocus();
    if (!fn_802411D4()) {
        lbl_8055F1FC = 1;
        lbl_8055F200 = 0;
    }
}

extern "C" {

void fn_80216D28(void);
void fn_80214D50(int s);

// Join/leave input: Back steps a player back (or leaves), Enter confirms, joins or starts the race.
}  // extern "C"

int JoinLeaveWidget::Key(int key) {
    int ctrl = fn_802E4A10();
    if (ctrl >= 2) {
        return 0;
    }
    FocusGroup* c = &this->groups[ctrl];
    int same = ctrl == *c->sel->slot;
    u8 owns = 0;
    if (c->state > 1 && same) {
        owns = 1;
    }
    int mine = owns ? ctrl : -1;
    int free = c->state == 1 ? ctrl : -1;
    switch (key) {
    case 0x1B:
        if (mine >= 0) {
            FocusGroup* g = &this->groups[mine];
            if (--g->state == 1) {
                if (mine == 0) {
                    lbl_8055F1FC = 1;
                    lbl_8055F200 = 0;
                } else {
                    fn_80216D28();
                    fn_80214D50(1);
                }
                g->state = 2;
            }
            g->sel->SetCur(g->state != 2);
        } else {
            lbl_8055F1FC = 1;
            lbl_8055F200 = 0;
        }
        break;
    case 0x0D:
        if (this->AllReady()) {
            if (ctrl == 0) {
                lbl_8055F1FC = 1;
                lbl_8055F200 = 2;
            } else {
                fn_80216D28();
                fn_80214D50(1);
            }
        } else if (mine >= 0) {
            FocusGroup* g = &this->groups[mine];
            if (this->Unconfirmed(mine) > 0) {
                if (g->state > 1 && g->sel->Confirmable()) {
                    g->state++;
                    g->sel->SetCur(g->state != 2);
                }
            }
        } else if (free >= 0) {
            this->groups[free].sel->SetSlot(ctrl);
            this->groups[free].state = 2;
        }
        break;
    case 0x6F:
        break;
    default:
        return 0;
    }
    return 1;
}

extern "C" {

// .sdata2 constants owned by this unit (0x80563750-0x80563798), in address order.
extern const float lbl_80563758 = 0.0f;
extern const float lbl_8056375C = 0.43f;
extern const float lbl_80563760 = 0.37f;
extern const float lbl_80563764 = 1.0f;
extern const float lbl_80563768 = 1000.0f;
extern const float lbl_8056376C = 250.0f;
extern const float lbl_80563770 = -1.0f;
extern const float lbl_80563774 = -2.0f;
extern const float lbl_80563778 = 3.0f;
extern const float lbl_8056377C = 5.0f;
extern const float lbl_80563780 = 3.5f;
extern const float lbl_80563784 = 0.5f;
extern const float lbl_80563788 = 0.3f;
extern const float lbl_8056378C = 0.7f;
extern const float lbl_80563790 = 255.0f;
extern const float lbl_80563794 = 4.0f;

}  // extern "C"
