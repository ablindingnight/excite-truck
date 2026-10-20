// Arena masters (0x801B81E0-0x801B8B84): the current event's game mode ("master"), created by
// event type, and the global functions that forward to it; a few GameMode helpers; and the null
// master used when the event has none. Names are guesses apart from the messages and keys.
#include "game/common.h"

extern "C" {
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void fn_80161618(void);
void fn_801616FC(void);
void fn_801619A8(void);
void fn_80161A18(void);
void fn_80161A70(void);
void fn_80161B70(void);
int fn_80161B0C(int a, int b);
int fn_80161B94(void);
int fn_80161BB8(int car);
int fn_80161BDC(int a, int b, int c);
int fn_80161C38(void);
void fn_8027DED8(int car, struct Damage* d);
int fn_801CB6B4(int a, int b, int c);
void fn_801B0C78(int car);
void fn_801B0F70(int a, int b);
void fn_801B2C70(void* r);
int fn_8013C9F4(int a);
}

struct EventInfo {
    u8 pad0[0x20];
    ValueNode* node;  // offset 0x20
};

EventInfo* GetEvent(int i);
int GetNumPlayers(void);

void* operator new(size_t size, const char* tag);

struct Damage {
    u8 pad0[0x50];
    f32 amount;  // offset 0x50
};

// The race setup the master is created for.
struct RaceSetup {
    u8 pad0[0x3E4];
    s32 event;  // offset 0x3E4
    u32 type;   // offset 0x3E8
};

class GameMode {
public:
    virtual ~GameMode(void) {}
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(int p);
    virtual void vf18(void);
    virtual int vf1C(int car);
    virtual int vf20(int car);
    virtual int vf24(void);
    virtual int vf28(int car);
    virtual int vf2C(int car);
    virtual int vf30(void);
    virtual int vf34(void);
    virtual f32 vf38(void);
    virtual int vf3C(int p);
    virtual void vf40(void);
    virtual void vf44(void);
    virtual void vf48(void);
    virtual void vf4C(int p);
    virtual void vf50(int p);
    virtual int vf54(int p);
    virtual int vf58(void);
    virtual int vf5C(int p);
    virtual void vf60(int view);
    virtual int vf64(void);
    virtual int vf68(void);
    virtual void vf6C(void);
    virtual void vf70(void);
    virtual void vf74(void);
    virtual void vf78(void);
    virtual int vf7C(int p);
    virtual int vf80(void);
    virtual int vf84(void);
    virtual int vf88(void);
    virtual int vf8C(int p, int key);
    virtual void vf90(int a, int b, int c, int d);
    virtual void vf94(int p);
    virtual void vf98(int a, int b);
    virtual void vf9C(int p, int type);
    virtual int vfA0(int a, int b, int c);
    virtual void vfA4(int p);
    virtual void vfA8(int a, int b);
    virtual void vfAC(int car);
    virtual int vfB0(int a, int b, int c);
    virtual void vfB4(int car, Damage* d);
    virtual void vfB8(int p);
    virtual int vfBC(void);
    virtual void vfC0(void);
    virtual void vfC4(void);
    virtual void vfC8(void);
    virtual int vfCC(void);
    virtual int vfD0(void);
    virtual int vfD4(int car);
    virtual int vfD8(void);
    virtual int vfDC(void);
    virtual int vfE0(void);
    virtual int vfE4(void);
    virtual void* vfE8(void);
    virtual int vfEC(void);
    virtual int vfF0(void);
    virtual void vfF4(void);
    virtual void vfF8(void);
    virtual void vfFC(void);

    int Collide(int a, int b);
    int Started1C(int car);
    int Started20(int car);
    void FillResults(void* r);
    int Reset(void);
};

// The master for events without one of their own.
class NullArenaMaster : public GameMode {
public:
    virtual ~NullArenaMaster(void) {}
    virtual int vfE4(void) { return 1; }
    virtual void vf10(void) {}
    virtual void vf0C(void) {}
    virtual int vf20(int car) { return 1; }
    virtual int vf24(void) { return 0; }
    virtual void vf4C(int p) {}
    virtual void vfB8(int p) {}
    virtual void vfC0(void) {}
    virtual void vfC4(void) {}
    virtual void vf98(int a, int b) {}
    virtual f32 vf38(void) { return 4.0f; }
    virtual int vf54(int p) { return (GetNumPlayers() > 1) ? 0x160B : 0x564B; }
    virtual int vf100(void) { return 1; }
    virtual int vf28(int car) { return 1; }
    virtual int vf2C(int car) { return 1; }
    virtual int vfD4(int car) { return 0; }
};

extern "C" {
GameMode* fn_8014145C(const RaceSetup* s);
GameMode* fn_8016BE60(const RaceSetup* s);
GameMode* fn_8014DE50(const RaceSetup* s);
GameMode* fn_80172C28(const RaceSetup* s);
}

ModuleTag lbl_8055E738;
UnkStaticFFFF lbl_8055E740;
GameMode* lbl_8055E744;     // current master
u8 lbl_8055E748;            // race started
u32 lbl_8055E74C;           // event type
f32 lbl_8055E750;           // time since the master finished
f32 lbl_8055A910 = 1.0f;    // damage factor
f32 lbl_80436D08[8];

extern "C" {

GameMode* fn_801B81E0(const RaceSetup* s) {
    switch (s->type) {
    case 0x180201:
        return fn_8014145C(s);
    case 0x180303:
        return fn_8016BE60(s);
    case 0x180302:
        return fn_8014DE50(s);
    case 0x180304:
        return fn_80172C28(s);
    default:
        fn_802E317C("Can't create master for arena event 0x%x", s->type);
        return NULL;
    }
}

void fn_801B826C(const RaceSetup* s) {
    u32 type = s->type;
    lbl_8055E74C = type;
    if (type & 0x200) {
        lbl_8055E744 = fn_801B81E0(s);
    } else {
        lbl_8055E744 = new ("null_amaster") NullArenaMaster;
    }
    lbl_8055A910 = GetEvent(s->event)->node->GetFloat("damage_factor", 1.0f);
    lbl_8055E748 = 0;
    fn_80161618();
    lbl_8055E750 = 0.0f;
}

void fn_801B8318(void) { lbl_8055E744->vfF4(); }

void fn_801B832C(void) {
    fn_801616FC();
    delete lbl_8055E744;
    lbl_8055E744 = NULL;
    for (int i = 0; i < 8; i++) {
        lbl_80436D08[i] = 0.0f;
    }
}

GameMode* fn_801B83A0(void) { return lbl_8055E744; }

void fn_801B83A8(void) {
    lbl_8055E744->vf10();
    lbl_8055E748 = 0;
    for (int i = 0; i < 8; i++) {
        lbl_80436D08[i] = 0.0f;
    }
    fn_801619A8();
    lbl_8055E750 = 0.0f;
}

void fn_801B8418(void) {}

void fn_801B841C(void) {
    lbl_8055E744->vf0C();
    fn_80161A18();
    if (lbl_8055E744->vf24()) {
        lbl_8055E750 += 0.016683417f;
    } else {
        lbl_8055E750 = 0.0f;
    }
}

void fn_801B8488(void) {}

void fn_801B848C(int p) { lbl_8055E744->vf14(p); }

void fn_801B84A4(int p) { lbl_8055E744->vf4C(p); }

void fn_801B84BC(void) {}

void fn_801B84C0(int p) { lbl_8055E744->vf50(p); }

int fn_801B84D8(int p) { return lbl_8055E744->vf54(p); }

int fn_801B84F0(void) { return 0; }

void fn_801B84F8(void) {
    lbl_8055E748 = 1;
    lbl_8055E744->vf18();
}

u8 fn_801B8514(void) { return lbl_8055E748; }

int fn_801B851C(int car) {
    if (fn_80161BB8(car)) {
        return 0;
    }
    return lbl_8055E744->vf28(car);
}

int fn_801B8570(int car) { return lbl_8055E744->vf2C(car) && !fn_80161B94(); }

int fn_801B85CC(void) { return lbl_8055E744->vf24(); }

void fn_801B85E0(int a, int b, int c, int d) { lbl_8055E744->vf90(a, b, c, d); }

void fn_801B8610(int a, int b) { lbl_8055E744->vf98(a, b); }

int fn_801B8630(int a, int b, int c) {
    if (fn_80161BDC(a, b, c)) {
        return 1;
    }
    return lbl_8055E744->vfA0(a, b, c);
}

void fn_801B86A4(int car) { lbl_8055E744->vfAC(car); }

void fn_801B86BC(int car, Damage* d) {
    lbl_8055E744->vfB4(car, d);
    d->amount *= lbl_8055A910;
    fn_8027DED8(car, d);
}

int fn_801B8728(int p) {
    lbl_8055E744->vfB8(p);
    return lbl_8055E744->vfE4() == 0;
}

void fn_801B8778(void) { lbl_8055E744->vfC0(); }

void fn_801B878C(void) { lbl_8055E744->vfC4(); }

int fn_801B87A0(int p, int key) {
    if (fn_80161B0C(p, key)) {
        return 1;
    }
    return lbl_8055E744->vf8C(p, key);
}

int fn_801B8804(void) { return 0; }

int fn_801B880C(void) { return lbl_8055E744->vf84(); }

void fn_801B8820(void) { lbl_8055E744->vf40(); }

void fn_801B8834(void) { lbl_8055E744->vf44(); }

int fn_801B8848(void) { return lbl_8055E744->vf34(); }

int fn_801B885C(int p) { return lbl_8055E744->vf3C(p); }

int fn_801B8874(void) {
    if (fn_80161C38()) {
        fn_80161A70();
        return 1;
    }
    return lbl_8055E744->vf88();
}

void fn_801B88BC(void) {
    if (fn_80161C38()) {
        fn_80161B70();
    } else {
        lbl_8055E744->vf38();
    }
}

int fn_801B8900(void) { return lbl_8055E744->vf80(); }

int fn_801B8914(void) { return lbl_8055E744->vf30(); }

int fn_801B8928(int a, int b, int c) { return lbl_8055E744->vfB0(a, b, c); }

f32 fn_801B8950(int i) { return lbl_80436D08[i]; }

void fn_801B8964(int i, f32 v) { lbl_80436D08[i] = v; }

void fn_801B8978(void) { lbl_8055E744->vf6C(); }

void fn_801B898C(void) { lbl_8055E744->vf70(); }

void fn_801B89A0(void) { lbl_8055E744->vf74(); }

void fn_801B89B4(void) { lbl_8055E744->vf78(); }

int fn_801B89C8(int p) { return lbl_8055E744->vf7C(p); }

int fn_801B89E0(void) { return lbl_8055E750 > 4.0f; }

int fn_801B89F8(void) { return lbl_8055E750 > 4.0f; }

}  // extern "C"

int GameMode::Collide(int a, int b) { return fn_801CB6B4(a, b, 2); }

int GameMode::Started1C(int car) {
    if (lbl_8055E748) {
        return vf1C(car);
    }
    return 0;
}

int GameMode::Started20(int car) {
    if (lbl_8055E748) {
        return vf20(car);
    }
    return 0;
}

void GameMode::vfAC(int car) { fn_801B0C78(car); }

void GameMode::vfA8(int a, int b) { fn_801B0F70(a, b); }

void GameMode::FillResults(void* r) { fn_801B2C70(r); }

int GameMode::Reset(void) { return fn_8013C9F4(0); }

// NonMatching: the code matches, but the null master's vtable needs the GameMode default virtuals
// (weak copies in zone, modeinl, aiutil, racing, overlay, betarace and aidriver) named first.
