// Challenge event path screen (0x8025FE8C-0x80262F8C): the challenge map
// ("EventPath:CHALLENGE_DIFFICULTY_%d_UNLOCKED", "EventPath:CONGRATULATIONS"), its title/message
// popup, the "%s/%s" progress counters and the list descriptors behind it. Written against raw field
// offsets; the large screen routines are still stubs. Names are guesses apart from the strings.
#include "game/common.h"
#include "game/menu.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

class VPage {
public:
    virtual void v08(void);
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
void fn_80236914(void);
void fn_80214A9C(int s);
void fn_80230E90(int a);
void fn_8025C204(u8* p, s32 a);
void fn_80235380(u8* p);
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
extern void* lbl_80345E28[];
extern void* lbl_80345EA0[];
extern u8* lbl_8055F420;

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
u8* GetTrack(int i);

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

extern char lbl_80345B40[];  // this unit's string pool
extern s32 lbl_8055C400;

extern "C" {
void fn_802E317C(const char* fmt, ...);
ChassisList* fn_8025007C(int n);
int fn_8024DAE0(u8* a, u8* b, int c, ChassisList* list, u32 d);

// Challenge mode: picks the vehicle through the garage, limited to the track's default vehicle.
int fn_8025FECC(u8* a, u8* b, int track, u32 d) {
    char* pool = lbl_80345B40;
    if (lbl_8055C400 >= 0) {
        return 1;
    }
    SetNumPlayers(1);
    u8* t = GetTrack(track);
    u32 chassis = 0;
    if (t && PTR(t, 0x80) && ((TrackTable*)PTR(t, 0x80))->HasTable(pool + 0x90)) {  // "vehicle_map"
        chassis = ((TrackTable*)PTR(t, 0x80))->GetTable(pool + 0x90)->GetString(pool + 0xA0, 0);  // "default_vehicle"
    } else {
        fn_802E317C(pool + 0xB0);  // "...Expecting vehicle_map table and found none!"
    }
    ChassisList* list = 0;
    if (chassis) {
        list = fn_8025007C(1);
        list->Add(chassis, 1, 0);
    } else {
        fn_802E317C(pool + 0x104);  // "...Default chassis is NULL!"
    }
    int r = fn_8024DAE0(a, b, 0, list, d);
    delete list;
    return r;
}

void fn_8026003C(void) {}  // stub (482)

void fn_802607C4(u8* p) {
    U8(p, 0x1D) = 1;
}

void fn_802607D0(u8* p) {
    U8(p, 0x1E) = 1;
}

void fn_802607DC(u8* p) {
    U8(p, 0x1E) = 0;
    U8(p, 0x1C) = 0;
}

void fn_802607EC(u8* p) {
    U8(p, 0x1D) = 0;
}

void fn_802607F8(u8* p) {
    if (!U8(p, 0x1C)) {
        U8(p, 0x1C) = 1;
        U8(p, 0x1F) = 1;
    }
}

void fn_80260814(u8* p) {
    U8(p, 0x1D) = 0;
}

void fn_80260820(u8* p) {
    if (!U8(p, 0x1C)) {
        U8(p, 0x1E) = 1;
        U8(p, 0x1C) = 1;
    }
}

void fn_8026083C(u8* p) {
    U8(p, 0x1C) = 0;
}

void fn_80260848(u8* p) {
    U8(p, 0x1D) = 1;
}

u8* fn_80260854(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80345E28;
        fn_80235380(PTR(p, 0x24));
        U32(p, 0x24) = 0;
        fn_802328D4(PTR(p, 0x28));
        U32(p, 0x28) = 0;
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_802608D4(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80345EA0;
        fn_80235380(PTR(p, 0x20));
        U32(p, 0x20) = 0;
        fn_802328D4(PTR(p, 0x24));
        U32(p, 0x24) = 0;
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

}  // extern "C"

// A node on the challenge map (event or difficulty marker).
class PathNode {
public:
    virtual ~PathNode(void);
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
    virtual void s38(void);
    virtual void s3C(void);
    virtual void s40(void);
    virtual void s44(void);
    virtual void s48(void);
    virtual void s4C(void);
    virtual void s50(void);
    virtual void s54(void);
    virtual void s58(void);
    virtual void s5C(void);
    virtual void Dim(void);      // 0x60
    virtual void Open(void);     // 0x64
    virtual void Select(void);   // 0x68
    virtual void Refresh(void);  // 0x6C
    virtual void Focus(void);    // 0x70
};

// The challenge map screen (lbl_8055F420).
struct PathScreen {
    s32 event;           // offset 0x0: selected event node
    s32 level;           // offset 0x4: selected difficulty
    s32 numEvents;       // offset 0x8
    s32 numLevels;       // offset 0xC
    u8 pad10[0x8];       // offset 0x10
    PathNode** events;   // offset 0x18
    u8 pad1C[0x20];      // offset 0x1C
    PathNode** levels;   // offset 0x3C
};

#define SCREEN ((PathScreen*)lbl_8055F420)

extern UnkStatic123s lbl_80498F00;  // end-of-menu tag
extern const MenuDesc lbl_802FA6C0;

int GetEveMode(void);
void UnloadEves(void);
void LoadModeEves(int mode);

// A node pointer list with inline storage (initialised by fn_8026213C / fn_80262120).
struct NodeVec {
    PathNode** data;  // offset 0x0
    s32 count;        // offset 0x4
    s32 cap;          // offset 0x8
};

// Node lists with inline storage for 6 events / 3 difficulties.
struct NodeVec6 {
    inline NodeVec6(void);
    PathNode** data;  // offset 0x0
    s32 count;        // offset 0x4
    s32 cap;          // offset 0x8
    PathNode* buf[6]; // offset 0xC
};

struct NodeVec3 {
    inline NodeVec3(void);
    PathNode** data;  // offset 0x0
    s32 count;        // offset 0x4
    s32 cap;          // offset 0x8
    PathNode* buf[3]; // offset 0xC
};

// The whole screen as it sits on fn_80260954's stack (PathScreen is its head).
struct PathScreenBuf {
    PathScreenBuf(int a) {
        lbl_8055F420 = (u8*)this;
        arg = a;
        level = 0;
        event = 0;
        popup = 0;
    }
    ~PathScreenBuf(void) {
        lbl_8055F420 = 0;
        if (popup) {
            delete popup;
            popup = 0;
        }
        lbl_8055F3EC = 0;
        int i;
        for (i = 0; i < numEvents; i++) {
            delete events.data[i];
        }
        for (i = 0; i < numLevels; i++) {
            delete levels.data[i];
        }
        levels.data = 0;
        levels.count = 0;
        levels.cap = 0;
        events.data = 0;
        events.count = 0;
        events.cap = 0;
    }
    s32 event;           // offset 0x0
    s32 level;           // offset 0x4
    s32 numEvents;       // offset 0x8
    s32 numLevels;       // offset 0xC
    s32 arg;             // offset 0x10
    PathNode* popup;     // offset 0x14
    NodeVec6 events;     // offset 0x18
    NodeVec3 levels;     // offset 0x3C
    u8 pad54[0x24];      // offset 0x54
};

// A raw menu record, filled by the out-of-line item initialisers below.
struct PathItem {
    u32 w[5];
};

extern "C" {
int fn_8027B878(int a);
void fn_80261C68(u8* p);
void fn_8026003C(void);
void fn_80261AF0(u8* p, int difficulty);
void fn_80261AE8(u8* p, u32 v);
void fn_80261A7C(u8* p, s32 a);
void fn_80261A18(u8* p, s32 a);
u8* fn_80261A08(u8* p, int i);
u8* fn_802619D8(u8* p, int i);
void fn_802619E8(u8* p, s32 a, s32 b);
void fn_80261980(u8* p, s32 a, s32 b, s32 c);
const char* fn_80261948(s32* p);
const char* fn_802619A0(s32* p);
void fn_80261688(u8* p, s32 a, s32 b, s32 c);
void fn_80261AA0(void);
void fn_80261A40(int x);
int fn_802610C8(void);
int fn_80260E30(void);
int fn_80261178(void);
int fn_802613E0(void);
int fn_802616A8(void);
u8* fn_80260DC4(u8* p, int del);
void fn_80260E04(Words5* d, const Words5* s);
extern u8 lbl_8055F3E8;
extern s32 lbl_8055F3E4;

// The challenge map screen: unlock popups, then the map menu until backed out.
void fn_80260954(int arg) {
    PathItem items[19];
    char* pool = lbl_80345B40;
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
    MenuDesc desc;
    s32 diff;
    fn_80261C68((u8*)&scr);
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
        fn_80261A7C((u8*)&items[0], (s32)fn_80261AA0);
        fn_80261A18((u8*)&items[1], (s32)fn_80261A40);
        fn_802619E8((u8*)&items[2], (s32)(pool + 0x1B8), *(s32*)fn_80261A08((u8*)&scr.levels, 0));
        fn_802619E8((u8*)&items[3], (s32)(pool + 0x1C8), *(s32*)fn_80261A08((u8*)&scr.levels, 1));
        fn_802619E8((u8*)&items[4], (s32)(pool + 0x1D8), *(s32*)fn_80261A08((u8*)&scr.levels, 2));
        fn_802619E8((u8*)&items[5], (s32)(pool + 0x1E8), *(s32*)fn_802619D8((u8*)&scr.events, 0));
        fn_802619E8((u8*)&items[6], (s32)(pool + 0x1F8), *(s32*)fn_802619D8((u8*)&scr.events, 1));
        fn_802619E8((u8*)&items[7], (s32)(pool + 0x208), *(s32*)fn_802619D8((u8*)&scr.events, 2));
        fn_802619E8((u8*)&items[8], (s32)(pool + 0x218), *(s32*)fn_802619D8((u8*)&scr.events, 3));
        fn_802619E8((u8*)&items[9], (s32)(pool + 0x228), *(s32*)fn_802619D8((u8*)&scr.events, 4));
        fn_802619E8((u8*)&items[10], (s32)(pool + 0x238), *(s32*)fn_802619D8((u8*)&scr.events, 5));
        fn_80261980((u8*)&items[11], (s32)(pool + 0x248), (s32)fn_802619A0(&diff), 0);
        fn_80261980((u8*)&items[12], (s32)(pool + 0x258), (s32)fn_80261948(&diff), 0);
        fn_80261688((u8*)&items[13], 0x126, 0, (s32)fn_802616A8);
        fn_80261688((u8*)&items[14], 0x128, 0, (s32)fn_802613E0);
        fn_80261688((u8*)&items[15], 0xD, -2, (s32)fn_80261178);
        fn_80261688((u8*)&items[16], 0x121, 0, (s32)fn_802610C8);
        fn_80261688((u8*)&items[17], 0x1B, -1, (s32)fn_80260E30);
        fn_80260E04((Words5*)&items[18], (const Words5*)&lbl_80498F00);
        desc = *(const MenuDesc*)&lbl_802FA6C0;
        desc.items = (MenuItem*)items;
        for (;;) {
            r = fn_802146BC(&desc, -999, -999, 1);
            if (r == -1) {
                fn_80260DC4((u8*)&diff, -1);
                r = -1;
                break;
            }
            if (r == -2) {
                fn_80260DC4((u8*)&diff, -1);
                r = -2;
                break;
            }
            fn_802E317C(pool + 0x268);  // "MenuDoChallengeMode: invalid return value from dialog!"
        }
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

}  // extern "C"

// Shows the event nodes of the selected difficulty (or all dimmed while picking a difficulty).
static inline void RedrawNodes(void) {
    int i;
    for (i = 0; i < SCREEN->numEvents; i++) {
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
    for (i = 0; i < SCREEN->numLevels; i++) {
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

void fn_80261688(u8* p, s32 a, s32 b, s32 c) {
    S32(p, 0x8) = c;
    S32(p, 0x4) = 6;
    PTR(p, 0x0) = lbl_8055B860;
    S32(p, 0xC) = b;
    S32(p, 0x10) = a;
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

void fn_80261980(u8* p, s32 a, s32 b, s32 c) {
    S32(p, 0x0) = a;
    S32(p, 0x4) = 3;
    S32(p, 0x8) = b;
    S32(p, 0xC) = c;
    S32(p, 0x10) = 0;
}

const char* fn_802619A0(s32* p) {
    const char* t[3] = {"bar4_top_e.art", "bar4_top_s.art", "bar4_top_m.art"};
    return t[*p];
}

u8* fn_802619D8(u8* p, int i) { return PTR(p, 0x0) + i * 4; }

void fn_802619E8(u8* p, s32 a, s32 b) {
    S32(p, 0x0) = a;
    S32(p, 0x4) = 5;
    S32(p, 0x8) = b;
    S32(p, 0xC) = 0;
    S32(p, 0x10) = 0;
}

u8* fn_80261A08(u8* p, int i) { return PTR(p, 0x0) + i * 4; }

void fn_80261A18(u8* p, s32 a) {
    S32(p, 0x8) = a;
    S32(p, 0x4) = 8;
    PTR(p, 0x0) = lbl_8055BAB8;
    S32(p, 0xC) = 2;
    S32(p, 0x10) = 0;
}

void fn_80261A40(int x) {
    fn_80236914();
    lbl_8055F3E9 = x == 2;
}

void fn_80261A7C(u8* p, s32 a) {
    S32(p, 0x8) = a;
    S32(p, 0x4) = 8;
    PTR(p, 0x0) = lbl_8055BAC0;
    S32(p, 0xC) = 0;
    S32(p, 0x10) = 0;
}

void fn_80261AA0(void) {
    for (int i = 0; i < S32(lbl_8055F420, 0xC); i++) {
        fn_80214A9C(i + 10);
    }
}

void fn_80261AE8(u8* p, u32 v) { U32(p, 0x0) = v; }

}  // extern "C"

extern const f32 lbl_80563B90;  // 0.0

// Animated widget base (vtable lbl_8033D618).
class AnimWidget {
public:
    AnimWidget(void) {
        f18 = lbl_80563B90;
        f14 = 0;
    }
    virtual ~AnimWidget(void) {}
    u8 pad4[0x10];  // offset 0x4
    s32 f14;        // offset 0x14
    f32 f18;        // offset 0x18
};

// The unlock animation shown in the challenge unlock box (vtable lbl_80345F70, dtor fn_80262F8C).
class UnlockAnim : public AnimWidget {
public:
    UnlockAnim(void) {}
    virtual ~UnlockAnim(void) {}
};

ModuleTag lbl_8055F3D8;
UnkStatic123s lbl_80498F00;
UnkStatic7F lbl_8055F3E0;
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
    MenuDesc desc = {"newmode1.lyt", 0, 0, 0, 0};
    desc.items = items;
    fn_802146BC(&desc, -999, -999, 1);
}

void fn_80261C68(u8* p) {}  // stub (302)

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

extern "C" {

void fn_80262158(u8* p) {}

void fn_8026215C(u8* p) {}

u32 fn_80262160(u8* p) { return 0; }

void fn_80262168(u8* p) {
    if (U8(p, 0x1E) && lbl_8055F3E9 && !lbl_8055F3EA) {
        fn_80230E90(0);
        U8(p, 0x1E) = 0;
    }
    if (lbl_8055F3EB && !lbl_8055F3EA) {
        fn_80214AF0(0x1E);
    } else {
        fn_80214A9C(0x1E);
    }
}

void fn_802621F0(u8* p) {}  // stub (225)

void fn_80262574(u8* p) {}

void fn_80262578(u8* p) {}

u32 fn_8026257C(u8* p) { return 0; }

void fn_80262584(u8* p) {
    if (U8(p, 0x1F) && lbl_8055F3E9) {
        fn_80230E90(0);
        U8(p, 0x1F) = 0;
    }
}

void fn_802625D4(u8* p) {}  // stub (585)

}  // extern "C"


