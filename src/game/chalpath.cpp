// Challenge event path screen (0x8025FE8C-0x80262FCC): the challenge map
// ("EventPath:CHALLENGE_DIFFICULTY_%d_UNLOCKED", "EventPath:CONGRATULATIONS"), its title/message
// popup, the "%s/%s" progress counters and the list descriptors behind it. Written against raw field
// offsets; the large screen routines are still stubs. Names are guesses apart from the strings.
#include "game/common.h"
#include "game/gart.h"
#include "game/sound.h"
#include "game/gamedb.h"
#include "game/menu.h"
#include "game/menustate.h"
#include <string.h>
#include "game/mathutil.h"

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

class VPage {
public:
    virtual ~VPage(void);  // 0x08 (pathclass Contest: slot 0 is __dt__7Contest)
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual u8* v20(void);
    virtual s32 v24(void);
};

struct Words3 {
    u32 w[3];
};

struct Words5 {
    u32 w[5];
};

extern "C" {
void fn_80236914(int, f32*, f32);
void fn_80214A9C(int s);
void fn_80230E90(int a);
void fn_8025C204(u8* p, s32 a);
void fn_802328D4(u8* p);
void fn_80214AF0(int s);
}

void operator delete(void* p);

extern u8 lbl_8055B860[8];
extern u8 lbl_8055BAB8[4];
extern u8 lbl_8055BAC0[8];
extern bool lbl_8055F3E9;
extern u8 lbl_8055F3EC;
extern u8 lbl_8055F3EA;
extern u8 lbl_8055F3EB;
extern u8* lbl_8055F420;

// Per-difficulty values (10, 6, 8), indexed by fn_802625D4.
int lbl_80345B40[3] = {10, 6, 8};

// The bottom and top bar art for each difficulty (easy, standard, master).
struct BarArt {
    const char* art[3];
};
// MATCHING-AID: never used, keeps these strings ahead of the others in the pool; the tables in .rodata come from the initialisers in fn_80261948 and fn_802619A0.
static const BarArt sBarBot = {{"bar4_bot_e.art", "bar4_bot_s.art", "bar4_bot_m.art"}};
static const BarArt sBarTop = {{"bar4_top_e.art", "bar4_top_s.art", "bar4_top_m.art"}};
extern const MenuDesc lbl_802FA6C0 = {"challengemode.lyt", 0, 0, 0, 0};
extern const MenuDesc lbl_802FA6D4 = {"newmode1.lyt", 0, 0, 0, 0};

extern "C" {

int fn_8025FE8C(void) {
    if (lbl_8055F3E9 && !lbl_8055F3EC) {
        u8* o = PTR(lbl_8055F420, 0x14);
        U8(o, 0x49) = 1;
        U8(o, 0x4B) = 0;
        U8(o, 0x4A) = 0;
        lbl_8055F3EC = 1;
    }
    return 0;
}

}  // extern "C"

void SetNumPlayers(int n);

class TrackTable {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual u32 GetString(const char* key, int def);  // 0x18
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
    virtual TrackTable* GetTable(const char* key);   // 0x5C
    bool HasTable(const char* key) { return GetTable(key) != 0; }
};

class ChassisList {
public:
    virtual ~ChassisList(void);                               // 0x08
    virtual void Add(u32 chassis, int a, int b);              // 0x0C
};

// A challenge event as the map shows it.
class ChalEvent {
public:
    virtual ~ChalEvent(void);
    virtual const char* Name(void);  // 0x0C
    virtual void s10(void);
    virtual void s14(void);
    virtual void s18(void);
    virtual void s1C(void);
    virtual s32 Id(void);            // 0x20
    virtual void s24(void);
    virtual s32 Score(void);         // 0x28
    virtual s32 Grade(void);         // 0x2C
    virtual void s30(void);
    virtual void s34(void);
    virtual void s38(void);
    virtual s32 Target(void);        // 0x3C
    virtual void s40(void);
    virtual void s44(void);
    virtual int Complete(void);      // 0x48
};

class ChalLevel;
extern "C" u8* fn_80232578(const char* name);

namespace {

/* Class name unproven */
// A node on the challenge map (event or difficulty marker). The slot order is this base's; each derived class
// declares its overrides in its own order, which is the order retail emits them in.
// MATCHING-AID (unproven): novtable - retail's unit has no table for this abstract base (only the two nodes'
// and IMenuState's), and without it the compiler emits one through the nodes' inline constructors.
class __declspec(novtable) PathNode : public MenuState {
public:
    virtual void Dim(void) = 0;      // 0x60
    virtual void Open(void) = 0;     // 0x64
    virtual void Select(void) = 0;   // 0x68
    virtual void Refresh(void) = 0;  // 0x6C
};


/* Class name unproven */
// An event marker ("eventcontrol").
class EventNode : public PathNode {
public:
    EventNode(void* e) {
        ev = e;
        f1E = 0;
        sel = 0;
        open = 1;
        art = fn_8023512C("bar7.art");
        font = fn_80232578("uifnt_o.gtx");
    }
    virtual ~EventNode(void) {
        fn_80235380(art);
        art = 0;
        fn_802328D4(font);
        font = 0;
    }
    virtual void Enter(void);
    virtual void Exit(void);
    virtual void Update(void);
    virtual void Draw(void);
    virtual int Key(int key);
    virtual void Open(void) { open = 0; }
    virtual void Select(void) {
        if (!sel) {
            f1E = 1;
            sel = 1;
        }
    }
    virtual void Refresh(void) { sel = 0; }
    virtual void Dim(void) { open = 1; }

    void* ev;   // offset 0x18
    u8 sel;     // offset 0x1C
    u8 open;    // offset 0x1D
    u8 f1E;     // offset 0x1E
    gArtSet* art;    // offset 0x20
    u8* font;   // offset 0x24
};

/* Class name unproven */
// A difficulty marker ("pathcontrol").
class LevelNode : public PathNode {
public:
    LevelNode(ChalLevel* l, int i) {
        index = i;
        lv = l;
        sel = 0;
        open = 1;
        f1E = 0;
        f1F = 0;
        art = fn_8023512C("bar7.art");
        font = fn_80232578("uifnt_o.gtx");
    }
    virtual ~LevelNode(void) {
        fn_80235380(art);
        art = 0;
        fn_802328D4(font);
        font = 0;
    }
    virtual void Enter(void);
    virtual void Exit(void);
    virtual void Update(void);
    virtual void Draw(void);
    virtual int Key(int key);
    virtual void Dim(void) { open = 1; }
    virtual void Focus(void) { f1E = 1; }  // 0x70
    virtual void Refresh(void) {
        f1E = 0;
        sel = 0;
    }
    virtual void Open(void) { open = 0; }
    virtual void Select(void) {
        if (!sel) {
            sel = 1;
            f1F = 1;
        }
    }

    ChalLevel* lv;  // offset 0x18
    u8 sel;         // offset 0x1C
    u8 open;        // offset 0x1D
    u8 f1E;         // offset 0x1E
    u8 f1F;         // offset 0x1F
    s32 index;      // offset 0x20
    gArtSet* art;        // offset 0x24
    u8* font;       // offset 0x28
};

}  // namespace

// The challenge map screen (lbl_8055F420).
struct PathScreen {
    s32 event;           // offset 0x0: selected event node
    s32 level;           // offset 0x4: selected difficulty
    s32 numEvents;       // offset 0x8
    s32 numLevels;       // offset 0xC
    u8 pad10[0x8];       // offset 0x10
    EventNode** events;  // offset 0x18
    u8 pad1C[0x20];      // offset 0x1C
    LevelNode** levels;  // offset 0x3C
};

#define SCREEN ((PathScreen*)lbl_8055F420)

extern UnkStatic123s lbl_80498F00;  // end-of-menu tag

int GetEveMode(void);
void UnloadEves(void);
void LoadModeEves(int mode);

// A node pointer list with inline storage (initialised by fn_8026213C / fn_80262120).
struct NodeVec {
    PathNode** data;  // offset 0x0
    s32 count;        // offset 0x4
    s32 cap;          // offset 0x8
};

// A challenge difficulty: its events.
class ChalLevel {
public:
    virtual ~ChalLevel(void);
    virtual void s0C(void);
    virtual void s10(void);
    virtual void s14(void);
    virtual int NumEvents(void);       // 0x18
    virtual void* GetEvent(int i);     // 0x1C
};

// The challenge path (lbl_8055F3E4): up to three difficulties.
class ChalPath {
public:
    virtual ~ChalPath(void);
    virtual int NumLevels(void);       // 0x0C
    virtual ChalLevel* GetLevel(int i);  // 0x10
};

struct PathScreenBuf;
extern "C" void fn_80261C68(PathScreenBuf* s);

// Node lists with inline storage for 6 events / 3 difficulties.
struct NodeVec6 {
    inline NodeVec6(void);
    void Push(EventNode* p) {
        EventNode** d = data;
        int k = count;
        count = k + 1;
        d[k] = p;
    }
    EventNode** data;  // offset 0x0
    s32 count;         // offset 0x4
    s32 cap;           // offset 0x8
    EventNode* buf[6]; // offset 0xC
};

struct NodeVec3 {
    inline NodeVec3(void);
    void Push(LevelNode* p) {
        LevelNode** d = data;
        int k = count;
        count = k + 1;
        d[k] = p;
    }
    LevelNode** data;  // offset 0x0
    s32 count;         // offset 0x4
    s32 cap;           // offset 0x8
    LevelNode* buf[3]; // offset 0xC
};

// The whole screen as it sits on MenuDoChallengeMode's stack (PathScreen is its head).
struct PathScreenBuf {
    PathScreenBuf(int a) {
        lbl_8055F420 = (u8*)this;
        arg = a;
        level = 0;
        event = 0;
        sound = 0;
    }
    ~PathScreenBuf(void) {
        lbl_8055F420 = 0;
        Free();
        levels.data = 0;
        levels.count = 0;
        levels.cap = 0;
        events.data = 0;
        events.count = 0;
        events.cap = 0;
    }
    // Deletes the sound and the nodes.
    void Free(void) {
        if (sound) {
            delete sound;
            sound = 0;
        }
        lbl_8055F3EC = 0;
        for (int i = 0; i < numEvents; i++) {
            delete events.data[i];
        }
        for (int i = 0; i < numLevels; i++) {
            delete levels.data[i];
        }
    }
    // Whether every event of the path is complete.
    bool AllComplete(void) {
        for (int i = 0; i < numEvents; i++) {
            if (!((ChalEvent*)evObjs[i])->Complete()) {
                return false;
            }
        }
        return true;
    }
    s32 event;           // offset 0x0
    s32 level;           // offset 0x4
    s32 numEvents;       // offset 0x8
    s32 numLevels;       // offset 0xC
    s32 arg;             // offset 0x10
    GameSound* sound;        // offset 0x14: "a_levelup.sfx"
    NodeVec6 events;     // offset 0x18
    NodeVec3 levels;     // offset 0x3C
    ChalLevel* lvObjs[3];  // offset 0x54
    void* evObjs[6];       // offset 0x60
};

// A raw menu record, filled by the out-of-line item initialisers below.
struct PathItem {
    u32 w[5];
};

// What fn_8023BF4C races: the track, the event's index on it, and where the result goes.
struct CareerRun {
    s32 track;  // offset 0x0
    s32 index;  // offset 0x4
    s32 which;  // offset 0x8
    void* out;  // offset 0xC
};

int IsSpecialChassisUnlocked(const char* name);
void SetPlayer(int player, long driver, long vehicle, unsigned long color);
int IdentGetCurrentSlot(void);
const char* IdentGetSlotName(int slot);
void SetPlayerName(const char* name, int player);
void ClearPlayer(int i);
void AchievementsClearSession(void);

void* operator new(size_t size, const char* tag);

#define SCREEN_BUF ((PathScreenBuf*)lbl_8055F420)

extern "C" {
ChalPath* fn_8027B878(int a);
void fn_802E317C(const char* fmt, ...);
int fn_802E4A10(void);
void fn_80137C84(int pad, int slot);
void fn_8023BF4C(CareerRun* r);
int fn_802E8D84(void);
void fn_802E7128(void);
extern u8 lbl_8055F3E8;
extern ChalPath* lbl_8055F3E4;
}

static inline int MinIGe(int a, int b) { return a >= b ? b : a; }

extern "C" {
void* memset(void* d, int c, u32 n);
u8* fn_80232578(const char* name);
GameSound* fn_80207848(const char* name, int flags);
}

// Shows the event nodes of the selected difficulty (or all dimmed while picking a difficulty).
static inline void RedrawNodes(void) {
    for (int i = 0; i < SCREEN->numEvents; i++) {
        if (!lbl_8055F3EB) {
            SCREEN->events[i]->Dim();
            SCREEN->events[i]->Refresh();
        } else if (i == SCREEN->event) {
            SCREEN->events[i]->Select();
        } else if (i == SCREEN->level * 2 || i == SCREEN->level * 2 + 1) {
            SCREEN->events[i]->Open();
            SCREEN->events[i]->Refresh();
        } else {
            SCREEN->events[i]->Refresh();
        }
    }
    for (int i = 0; i < SCREEN->numLevels; i++) {
        if (!lbl_8055F3EB) {
            if (i == SCREEN->level) {
                SCREEN->levels[i]->Select();
            } else {
                SCREEN->levels[i]->Open();
                SCREEN->levels[i]->Refresh();
            }
        } else if (i == SCREEN->level) {
            SCREEN->levels[i]->Select();
            SCREEN->levels[SCREEN->level]->Focus();
        } else {
            SCREEN->levels[i]->Dim();
            SCREEN->levels[i]->Refresh();
        }
    }
}

// Builds the map: a "pathcontrol" node per difficulty (at most 3) and an "eventcontrol" node per event
// (at most 6 each), and the level-up sound.
static inline void BuildMap(PathScreenBuf* s) {
    int j;
    int i;
    s->numEvents = 0;
    s->numLevels = lbl_8055F3E4->NumLevels();
    memset(s->evObjs, 0, sizeof(s->evObjs));
    memset(s->lvObjs, 0, sizeof(s->lvObjs));
    s->events.count = 0;
    s->levels.count = 0;
    for (i = 0; i < MinIGe(lbl_8055F3E4->NumLevels(), 3); i++) {
        s->lvObjs[i] = lbl_8055F3E4->GetLevel(i);
        s->levels.Push(new ("pathcontrol") LevelNode(lbl_8055F3E4->GetLevel(i), i));
        for (j = 0; j < MinIGe(s->lvObjs[i]->NumEvents(), 6); j++) {
            s->evObjs[s->numEvents] = s->lvObjs[i]->GetEvent(j);
            s->events.Push(new ("eventcontrol") EventNode(lbl_8055F3E4->GetLevel(i)->GetEvent(j)));
            s->numEvents++;
        }
    }
    s->sound = fn_80207848("a_levelup.sfx", 0);
    if (s->sound) {
        s->sound->SetVolume(1.0f);
    } else {
        fn_802E317C("Error creating unlock sound in MCHALLENGEMODE!");
    }
}

s32 lbl_8055C400 = -1;
// The difficulty node's highlight frame (used by fn_802625D4).
static f32 sStart = -100.0f;
static f32 kSmall = 0.05f;
static f32 kBig = 0.1f;
static f32 kGrow = 0.5f;
static f32 kShrink = 0.5f;

extern "C" {
void fn_802E317C(const char* fmt, ...);
ChassisList* fn_8025007C(int n);
int fn_8024DAE0(u8* a, u8* b, int c, ChassisList* list, u32 d);

// Challenge mode: picks the vehicle through the garage, limited to the track's default vehicle.
int fn_8025FECC(u8* a, u8* b, int track, u32 d) {
    if (lbl_8055C400 >= 0) {
        return 1;
    }
    SetNumPlayers(1);
    TrackInfo* t = GetTrack(track);
    u32 chassis = 0;
    if (t && t->node && ((TrackTable*)t->node)->HasTable("vehicle_map")) {
        chassis = ((TrackTable*)t->node)->GetTable("vehicle_map")->GetString("default_vehicle", 0);
    } else {
        fn_802E317C("MenuDoChallengeMode::ChooseVehicle - Expecting vehicle_map table and found none!");
    }
    ChassisList* list = 0;
    if (chassis) {
        list = fn_8025007C(1);
        list->Add(chassis, 1, 0);
    } else {
        fn_802E317C("MenuDoChallengeMode::ChooseVehicle - Default chassis is NULL!");
    }
    int r = fn_8024DAE0(a, b, 0, list, d);
    delete list;
    return r;
}

/* Helper name unproven */
static inline void RebuildMapAfterRace(s32 arg) {
    ClearPlayer(0);
    AchievementsClearSession();
    lbl_8055F3E4 = fn_8027B878(arg);
    BuildMap(SCREEN_BUF);
}

// Races the selected event (vehicle picked through the garage), then rebuilds the map; flags the
// "difficulty unlocked" popup when that race completed the path.
void fn_8026003C(void) {
    s32 event;
    s32 arg;
    s32 track;
    s32 driver;
    driver = GetDriver(0)->id;
    s32 vehicle = GetVehicle(GetDriver(0)->vehicle)->id;
    u32 color = GetDriver(0)->color;
    event = ((ChalEvent*)SCREEN_BUF->evObjs[SCREEN_BUF->event])->Id();
    track = GetEvent(event)->track;
    arg = SCREEN_BUF->arg;
    if (fn_8025FECC((u8*)&vehicle, (u8*)&color, track, arg)) {
        VehicleInfo* v = FindVehicle(vehicle);
        int special = !(v->flags & 2);
        int hadSpecial = special && IsSpecialChassisUnlocked(v->chassis);
        bool wasComplete = SCREEN_BUF->AllComplete();
        SCREEN_BUF->Free();
        SetPlayer(0, driver, vehicle, color);
        SetPlayerName(IdentGetSlotName(IdentGetCurrentSlot()), 0);
        fn_80137C84(fn_802E4A10(), 0);
        u8 out[0xB0];
        memset(out, 0, sizeof(out));
        CareerRun run;
        run.track = track;
        run.index = -1;
        for (int i = GetTrack(track)->events; i-- > 0;) {
            if (event == FindTrackEvent(track, i)->id) {
                run.index = i;
            }
        }
        run.which = arg;
        run.out = out;
        fn_8023BF4C(&run);
        RebuildMapAfterRace(arg);
        RedrawNodes();
        bool nowComplete = SCREEN_BUF->AllComplete();
        if (fn_802E8D84()) {
            fn_802E7128();
        }
        if (!wasComplete && nowComplete) {
            lbl_8055F3E8 = 1;
        }
    }
}

}  // extern "C"


extern "C" {
void fn_8026003C(void);
void fn_80261AF0(u8* p, int difficulty);
void fn_80261AE8(u8* p, u32 v);
void fn_80261A7C(MenuItem* p, s32 a);
void fn_80261A18(MenuItem* p, s32 a);
u8* fn_80261A08(u8* p, int i);
u8* fn_802619D8(u8* p, int i);
void fn_802619E8(MenuItem* p, s32 a, s32 b);
void fn_80261980(MenuItem* p, s32 a, s32 b, s32 c);
const char* fn_80261948(s32* p);
const char* fn_802619A0(s32* p);
void fn_80261688(MenuItem* p, s32 a, s32 b, s32 c);
void fn_80261AA0(void);
void fn_80261A40(int, f32*, f32);
int fn_802610C8(void);
int fn_80260E30(void);
int fn_80261178(void);
int fn_802613E0(void);
int fn_802616A8(void);
u8* fn_80260DC4(u8* p, int del);
void fn_80260E04(Words5* d, const Words5* s);

// The challenge map screen: unlock popups, then the map menu until backed out.
void MenuDoChallengeMode(int arg) {
    PathItem items[19];
    int mode = GetEveMode();
    if (mode != 0x130) {
        if (mode != 0x134) {
            UnloadEves();
        }
        LoadModeEves(0x130);
    }
    lbl_8055F3E4 = fn_8027B878(arg);
    {
    PathScreenBuf scr(arg);
    s32 diff;
    fn_80261C68(&scr);
    int r;
    goto start;
    do {
        fn_8026003C();
    start:
        if (lbl_8055F3E8) {
            fn_80261AF0((u8*)&scr, scr.arg + 1);
            lbl_8055F3E8 = 0;
            if (scr.arg == 0) {
                lbl_8055F3EB = 0;
                r = -1;
                continue;
            }
        }
        fn_80261AE8((u8*)&diff, scr.arg);
        fn_80261A7C((MenuItem*)&items[0], (s32)fn_80261AA0);
        fn_80261A18((MenuItem*)&items[1], (s32)fn_80261A40);
        fn_802619E8((MenuItem*)&items[2], (s32)"path_custom_0", *(s32*)fn_80261A08((u8*)&scr.levels, 0));
        fn_802619E8((MenuItem*)&items[3], (s32)"path_custom_1", *(s32*)fn_80261A08((u8*)&scr.levels, 1));
        fn_802619E8((MenuItem*)&items[4], (s32)"path_custom_2", *(s32*)fn_80261A08((u8*)&scr.levels, 2));
        fn_802619E8((MenuItem*)&items[5], (s32)"button_custom_0", *(s32*)fn_802619D8((u8*)&scr.events, 0));
        fn_802619E8((MenuItem*)&items[6], (s32)"button_custom_1", *(s32*)fn_802619D8((u8*)&scr.events, 1));
        fn_802619E8((MenuItem*)&items[7], (s32)"button_custom_2", *(s32*)fn_802619D8((u8*)&scr.events, 2));
        fn_802619E8((MenuItem*)&items[8], (s32)"button_custom_3", *(s32*)fn_802619D8((u8*)&scr.events, 3));
        fn_802619E8((MenuItem*)&items[9], (s32)"button_custom_4", *(s32*)fn_802619D8((u8*)&scr.events, 4));
        fn_802619E8((MenuItem*)&items[10], (s32)"button_custom_5", *(s32*)fn_802619D8((u8*)&scr.events, 5));
        fn_80261980((MenuItem*)&items[11], (s32)"#bar4_top_color", (s32)fn_802619A0(&diff), 0);
        fn_80261980((MenuItem*)&items[12], (s32)"#bar4_bot_color", (s32)fn_80261948(&diff), 0);
        fn_80261688((MenuItem*)&items[13], 0x126, 0, (s32)fn_802616A8);
        fn_80261688((MenuItem*)&items[14], 0x128, 0, (s32)fn_802613E0);
        fn_80261688((MenuItem*)&items[15], 0xD, -2, (s32)fn_80261178);
        fn_80261688((MenuItem*)&items[16], 0x121, 0, (s32)fn_802610C8);
        fn_80261688((MenuItem*)&items[17], 0x1B, -1, (s32)fn_80260E30);
        fn_80260E04((Words5*)&items[18], (const Words5*)&lbl_80498F00);
        MenuDesc desc = *(const MenuDesc*)&lbl_802FA6C0;
        desc.items = (MenuItem*)items;
        for (;;) {
            switch (fn_802146BC(&desc, -999, -999, 1)) {
            case -2:
                fn_80260DC4((u8*)&diff, -1);
                r = -2;
                goto done;
            case -1:
                fn_80260DC4((u8*)&diff, -1);
                r = -1;
                goto done;
            }
            fn_802E317C("MenuDoChallengeMode: invalid return value from dialog!");
        }
    done:;
    } while (r != -1);
    }
    int now = GetEveMode();
    if (mode != now) {
        if (now != 0x134) {
            UnloadEves();
        }
        if (mode != 0x134) {
            LoadModeEves(mode);
        }
    }
}

u8* fn_80260DC4(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_80260E04(Words5* d, const Words5* s) { *d = *s; }

}  // extern "C"




extern "C" {

// Back from event picking to difficulty picking (lbl_8055F3EB set); returns 1 when nothing changed.
int fn_80260E30(void) {
    if (lbl_8055F3EB) {
        lbl_8055F3EB = 0;
        for (int i = 0; i < SCREEN->numLevels; i++) {
            SCREEN->levels[i]->Refresh();
        }
        RedrawNodes();
    } else {
        return 1;
    }
    return 0;
}

int fn_802610C8(void) {
    if (lbl_8055F3EB) {
        lbl_8055F3EA = 1;
        fn_80214A9C(0x1E);
        s32 a = (*(VPage**)(lbl_8055F420 + S32(lbl_8055F420, 0x0) * 4 + 0x60))->v24();
        fn_8025C204((*(VPage**)(lbl_8055F420 + S32(lbl_8055F420, 0x0) * 4 + 0x60))->v20(), a);
        fn_80214AF0(0x1E);
        fn_80230E90(0);
        lbl_8055F3EA = 0;
    }
    return 0;
}

// Difficulty chosen: moves to its first event node.
int fn_80261178(void) {
    if (lbl_8055F3EB) {
        return 1;
    }
    lbl_8055F3EB = 1;
    SCREEN->event = SCREEN->level * 2;
    RedrawNodes();
    return 0;
}

// Next difficulty, or next event of the chosen difficulty.
int fn_802613E0(void) {
    if (!lbl_8055F3EB) {
        SCREEN->level++;
        if (SCREEN->level >= SCREEN->numLevels) {
            SCREEN->level = SCREEN->numLevels - 1;
        }
    } else {
        SCREEN->event++;
        if (SCREEN->event > SCREEN->level * 2 + 1) {
            SCREEN->event = SCREEN->level * 2 + 1;
        }
    }
    RedrawNodes();
    return 0;
}

void fn_80261688(MenuItem* p, s32 a, s32 b, s32 c) {
    p->data = (const void*)c;
    p->type = 6;
    p->name = (const char*)lbl_8055B860;
    p->arg0 = b;
    p->arg1 = a;
}

// Previous difficulty, or previous event of the chosen difficulty.
int fn_802616A8(void) {
    if (!lbl_8055F3EB) {
        SCREEN->level--;
        if (SCREEN->level < 0) {
            SCREEN->level = 0;
        }
    } else {
        SCREEN->event--;
        if (SCREEN->event < SCREEN->level * 2) {
            SCREEN->event = SCREEN->level * 2;
        }
    }
    RedrawNodes();
    return 0;
}

const char* fn_80261948(s32* p) {
    const char* t[3] = {"bar4_bot_e.art", "bar4_bot_s.art", "bar4_bot_m.art"};
    return t[*p];
}

void fn_80261980(MenuItem* p, s32 a, s32 b, s32 c) {
    p->name = (const char*)a;
    p->type = 3;
    p->data = (const void*)b;
    p->arg0 = c;
    p->arg1 = 0;
}

const char* fn_802619A0(s32* p) {
    const char* t[3] = {"bar4_top_e.art", "bar4_top_s.art", "bar4_top_m.art"};
    return t[*p];
}

u8* fn_802619D8(u8* p, int i) { return PTR(p, 0x0) + i * 4; }

void fn_802619E8(MenuItem* p, s32 a, s32 b) {
    p->name = (const char*)a;
    p->type = 5;
    p->data = (const void*)b;
    p->arg0 = 0;
    p->arg1 = 0;
}

u8* fn_80261A08(u8* p, int i) { return PTR(p, 0x0) + i * 4; }

void fn_80261A18(MenuItem* p, s32 a) {
    p->data = (const void*)a;
    p->type = 8;
    p->name = (const char*)lbl_8055BAB8;
    p->arg0 = 2;
    p->arg1 = 0;
}

void fn_80261A40(int x, f32* out, f32 t) {
    fn_80236914(x, out, t);
    lbl_8055F3E9 = x == 2;
}

void fn_80261A7C(MenuItem* p, s32 a) {
    p->data = (const void*)a;
    p->type = 8;
    p->name = (const char*)lbl_8055BAC0;
    p->arg0 = 0;
    p->arg1 = 0;
}

void fn_80261AA0(void) {
    for (int i = 0; i < S32(lbl_8055F420, 0xC); i++) {
        fn_80214A9C(i + 10);
    }
}

void fn_80261AE8(u8* p, u32 v) { U32(p, 0x0) = v; }

}  // extern "C"


extern "C" f32 fn_8023659C(void);  // seconds

/* Class name unproven */
// The unlock animation shown in the challenge unlock box: it flashes briefly, then lets the box continue. Its
// members (and its table) come out after the unit's __sinit, which builds the static instance.
class UnlockAnim : public MenuState {
public:
    UnlockAnim(void) { start = 0.0f; }
    virtual ~UnlockAnim(void) {}
    virtual void Enter(void) {
        start = fn_8023659C();
        done = 0;
        fn_80214A9C(0x1D);
    }
    virtual void Update(void) {
        f32 now = fn_8023659C();
        f32 t0 = start;
        if (now < t0 || now - t0 > 1.5f) {
            done = 1;
        }
        if (done) {
            fn_80214AF0(0x1D);
        }
    }
    virtual int Key(int key) { return done == 0; }

    f32 start;  // offset 0x18
    u8 done;    // offset 0x1C
};

ModuleTag lbl_8055F3D8;
UnkStatic123s lbl_80498F00;
UnkStatic7F lbl_8055F3E0;
ChalPath* lbl_8055F3E4;  // the challenge path
u8 lbl_8055F3E8;
bool lbl_8055F3E9;
u8 lbl_8055F3EA;
u8 lbl_8055F3EB;  // event picking (not difficulty picking)
u8 lbl_8055F3EC;
static UnlockAnim sUnlockAnim;  // lbl_80498F20
extern "C" const char* fn_80131234(const char* key);
extern "C" int fn_801345D8(char* buf, int n, const char* fmt, ...);

extern "C" {

// "Challenge difficulty N unlocked" message box.
void fn_80261AF0(u8* p, int difficulty) {
    char buf[100];
    fn_801345D8(buf, 100, "EventPath:CHALLENGE_DIFFICULTY_%d_UNLOCKED", difficulty);
    MenuItem items[] = {
        MenuItem(0, 5, &sUnlockAnim, 0, 0),
        MenuItem("title", fn_80131234("EventPath:CONGRATULATIONS")),
        MenuItem("message", fn_80131234(buf)),
        MenuItem((const char*)lbl_8055B860, 6, 0, -2, 13),
        MenuItem((const char*)lbl_8055BAB8, 8, (const void*)fn_80261A40, 2, 0),
        MenuItem(lbl_80498F00),
    };
    MenuDesc desc = lbl_802FA6D4;
    desc.items = items;
    fn_802146BC(&desc, -999, -999, 1);
}

}  // extern "C"

void* operator new(size_t size, const char* tag);


extern "C" {

void fn_80261C68(PathScreenBuf* s) {
    int i;
    int j;
    s->numEvents = 0;
    s->numLevels = lbl_8055F3E4->NumLevels();
    memset(s->evObjs, 0, sizeof(s->evObjs));
    memset(s->lvObjs, 0, sizeof(s->lvObjs));
    s->events.count = 0;
    s->levels.count = 0;
    for (i = 0; i < MinIGe(lbl_8055F3E4->NumLevels(), 3); i++) {
        s->lvObjs[i] = lbl_8055F3E4->GetLevel(i);
        s->levels.Push(new ("pathcontrol") LevelNode(lbl_8055F3E4->GetLevel(i), i));
        for (j = 0; j < MinIGe(s->lvObjs[i]->NumEvents(), 6); j++) {
            s->evObjs[s->numEvents] = s->lvObjs[i]->GetEvent(j);
            s->events.Push(new ("eventcontrol") EventNode(lbl_8055F3E4->GetLevel(i)->GetEvent(j)));
            s->numEvents++;
        }
    }
    s->sound = fn_80207848("a_levelup.sfx", 0);
    if (s->sound) {
        s->sound->SetVolume(1.0f);
    } else {
        fn_802E317C("Error creating unlock sound in MCHALLENGEMODE!");
    }
    RedrawNodes();
}

}  // extern "C"

inline NodeVec3::NodeVec3(void) {
    data = buf;
    cap = 3;
    count = 0;
}

inline NodeVec6::NodeVec6(void) {
    data = buf;
    cap = 6;
    count = 0;
}


void EventNode::Enter(void) {}


void EventNode::Exit(void) {}


int EventNode::Key(int key) { return 0; }


void EventNode::Update(void) {
    if (f1E && lbl_8055F3E9 && !lbl_8055F3EA) {
        fn_80230E90(0);
        f1E = 0;
    }
    if (lbl_8055F3EB && !lbl_8055F3EA) {
        fn_80214AF0(0x1E);
    } else {
        fn_80214A9C(0x1E);
    }
}



const char* GetGradeDisplayName(int grade);

extern "C" {
void fn_80230C30(f32* x, f32* y, s32 lx, s32 ly);
void fn_80230F78(u8* alpha, f32* grow, u32* color, int a);
void fn_80232BA4(const char* text, u8* font, f32 x, f32 y, u32 color, int align, f32 scale, f32 z, f32 xs);
void fn_801308F4(char* buf, s32 size, s32 value);

#define EV ((ChalEvent*)p->ev)

// Event node: its tile (pulsing while selected), name, grade and "score/target".
#pragma push
#pragma opt_propagation off  /* Pragma unproven: helps but not exact (PRAGMA_AUDIT_2026-11-15) */
}  // extern "C"

void EventNode::Draw(void) {
    EventNode* p = this;
    static f32 sTextY = -(f32)p->h / 2.0f;
    char line[30];
    char target[30];
    int part;
    u32 tile;
    f32 x;
    f32 y;
    u32 color;
    f32 grow;
    u8 alpha;
    u32 gradeColor;
    fn_80230C30(&x, &y, p->x, p->y);
    color = 0xFFFFFFFF;
    part = 0;
    gradeColor = 0xFFFFFFFF;
    tile = 0xFFFFFFFF;
    if (p->sel) {
        color = 0xFFFFFFFF;
        part = 1;
        gradeColor = 0xFFFFFFFF;
        tile = 0xFFFFFFFF;
    } else if (p->open) {
        color = 0xFFFFFFFF;
        gradeColor = 0xFFFFFFFF;
        tile = 0xFFFFFFFF;
    }
    fn_802354A0(p->art, part, x, y, 0.0f, (f32)p->w, (f32)p->h, tile);
    if (p->sel && !lbl_8055F3EA) {
        fn_80230F78(&alpha, &grow, &color, 0);
        u32 c = (tile & 0xFF0000) | (alpha << 24) | (tile & 0xFFFF);
        f32 hw = (f32)p->w / 2.0f;
        f32 hh = (f32)p->h / 2.0f;
        f32 nx = x - grow * hw;
        f32 ny = y + grow * hh;
        fn_802354A0(p->art, part, nx, ny, 0.0f, (1.0f + grow) * (f32)p->w,
                    (1.0f + grow) * (f32)p->h, c);
    }
    fn_80232BA4(EV->Name(), p->font, 15.0f + x, y + sTextY, color, 0x11, 0.65f, 0.0f, 1.0f);
    const char* grade = EV->Grade() != -1 ? GetGradeDisplayName(EV->Grade()) : "-";
    fn_80232BA4(grade, p->font, 205.0f + x, y + sTextY, gradeColor, 0x14, 0.65f, 0.0f, 1.0f);
    fn_801308F4(target, 30, EV->Target());
    if (EV->Score() <= 0) {
        fn_801345D8(line, 30, "0/%s", target + 1);
    } else {
        fn_801308F4(line, 30, EV->Score());
        fn_801345D8(line, 30, "%s/%s", line, target + 1);
    }
    fn_80232BA4(line, p->font, 350.0f + x, y + sTextY, color, 0x12, 0.65f, 0.0f, 1.0f);
}

extern "C" {
#pragma pop

}  // extern "C"

void LevelNode::Enter(void) {}


void LevelNode::Exit(void) {}


int LevelNode::Key(int key) { return 0; }


void LevelNode::Update(void) {
    if (f1F && lbl_8055F3E9) {
        fn_80230E90(0);
        f1F = 0;
    }
}



// Odd smoothstep: -2|x|^3 + 3x^2 with the sign of x, for x in [-1, 1].
static inline f32 Ease(f32 x) {
    if (x < -1.0f) {
        return -1.0f;
    } else if (x > 1.0f) {
        return 1.0f;
    } else if (x > 0.0f) {
        return x * (-2.0f * x * x) + 3.0f * x * x;
    } else {
        return x * (-2.0f * x * x) - 3.0f * x * x;
    }
}

// The node's label source (slot 0xC returns the text).
class NodeText {
public:
    virtual ~NodeText(void);  // 0x08 (the level's Bracket (ChalLevel): slot 0 is __dt__7Bracket)
    virtual const char* GetText(void);  // 0xC
};

extern "C" {
f32 fn_8023659C(void);
void fn_80230F6C(u32* color);

// Difficulty node: its tile and label; while selected a highlight frame grows and shrinks around it.
}  // extern "C"

void LevelNode::Draw(void) {
    LevelNode* p = this;
    f32 x;
    f32 y;
    u32 color;
    int part;
    u32 tile;
    fn_80230C30(&x, &y, p->x, p->y);
    tile = 0xFFFFFFFF;
    part = lbl_80345B40[p->index] + 1;
    color = 0xFFFFFFFF;
    if (p->sel) {
        part = lbl_80345B40[p->index];
        color = 0xFFFFFFFF;
        tile = 0xFFFFFFFF;
    } else if (p->open) {
        tile = 0xFFFFFFFF;
    }
    static f32 sPeriod = kGrow + kShrink;
    static f32 sSmallW = kSmall * (f32)p->w;
    static f32 sSmallH = kSmall * (f32)p->h;
    static f32 sBigW = kBig * (f32)p->w;
    static f32 sBigH = kBig * (f32)p->h;
    if (p->f1E && lbl_8055F3E9) {
        fn_80214AF0(p->index + 10);
    } else {
        fn_80214A9C(p->index + 10);
    }
    if (p->sel) {
        f32 fx = x - sBigW / 2.0f;
        f32 fy = y + sBigH / 2.0f;
        f32 fw = sBigW + (f32)p->w;
        f32 fh = sBigH + (f32)p->h;
        if (!p->f1E) {
            f32 now = fn_8023659C();
            if (now - sStart > sPeriod) {
                sStart = now;
            }
            f32 t = now - sStart;
            if (t < kGrow) {
                t = InvLerp(0.0f, kGrow, t);
                t = MIN(1.0f, t);
                t = MAX(t, 0.0f);
                f32 s = Lerp(0.0f, 1.0f, t);
                fx = Lerp(x - sSmallW / 2.0f, x - sBigW / 2.0f, Ease(s));
                fy = Lerp(y + sSmallH / 2.0f, y + sBigH / 2.0f, Ease(s));
                fw = Lerp(sSmallW + (f32)p->w, sBigW + (f32)p->w, Ease(s));
                fh = Lerp(sSmallH + (f32)p->h, sBigH + (f32)p->h, Ease(s));
            } else {
                t = InvLerp(kGrow, kGrow + kShrink, t);
                t = MIN(1.0f, t);
                t = MAX(t, 0.0f);
                f32 s = Lerp(0.0f, 1.0f, t);
                fx = Lerp(x - sBigW / 2.0f, x - sSmallW / 2.0f, Ease(s));
                fy = Lerp(y + sBigH / 2.0f, y + sSmallH / 2.0f, Ease(s));
                fw = Lerp(sBigW + (f32)p->w, sSmallW + (f32)p->w, Ease(s));
                fh = Lerp(sBigH + (f32)p->h, sSmallH + (f32)p->h, Ease(s));
            }
            fn_80230F6C(&color);
        } else {
            color = 0xFFFFCA44;
        }
        fn_802354A0(p->art, 0x12, fx, fy, 0.0f, fw, fh, tile);
    }
    fn_802354A0(p->art, part, x, y, 0.0f, (f32)p->w, (f32)p->h, tile);
    fn_80232BA4(((NodeText*)p->lv)->GetText(), p->font, 22.0f + x, -16.0f + y, color, 0x11, 0.6f, 0.0f,
                1.0f);
}

// Defined below the functions that own the six static sizes: retail's .sbss has those between the flags and this.
u8* lbl_8055F420;  // the challenge map screen



