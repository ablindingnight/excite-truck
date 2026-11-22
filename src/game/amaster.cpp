// Arena masters (0x801B81E0-0x801B8B84): the current event's game mode ("master"), created by
// event type, and the global functions that forward to it; a few GameMode helpers; and the null
// master used when the event has none. Names are guesses apart from the messages and keys.
#include "game/common.h"
#include "game/gamemode.h"
#include "game/gamemodector.h"
#include "game/racesetup.h"
#include "game/gamedb.h"

extern "C" {
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void fn_80161618(void);
void fn_801616FC(void);
void fn_801619A8(void);
void fn_80161A18(void);
void fn_80161A70(void);
f32 fn_80161B70(void);
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

int GetNumPlayers(void);

void* operator new(size_t size, const char* tag);

struct Damage {
    u8 pad0[0x50];
    f32 amount;  // offset 0x50
};

// The master for events without one of their own.
class NullArenaMaster : public GameMode {
public:
    virtual ~NullArenaMaster(void) {}
    virtual int vfE4(void) { return 1; }
    virtual void vf10(void) {}
    virtual void vf0C(void) {}
    virtual int vf20(int car) const { return 1; }
    virtual int vf24(void) const { return 0; }
    virtual void vf4C(int p) const {}
    virtual void vfB8(ZoneResults* r) {}
    virtual void vfC0(void) {}
    virtual void vfC4(void) {}
    virtual int Collide(int a, int b) {
#ifndef __MWERKS__
        // PORT: retail's body is a bare blr, so the (unused) result is r3 = this; give the PC the same value.
        return (int)(size_t)this;
#endif
    }
    virtual f32 vf38(void) { return 4.0f; }
    virtual int vf54(int p) { return (GetNumPlayers() > 1) ? 0x160B : 0x564B; }
    virtual int vf100(void) { return 1; }
    virtual int Started1C(int car) { return 1; }
    virtual int Started20(int car) { return 1; }
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
f32 damageFactor = 1.0f;    // damage factor
f32 lbl_80436D08[8];

extern "C" {

GameMode* CreateArenaMaster(const RaceSetup* s) {
    switch (s->opts.type) {
    case 0x180201:
        return fn_8014145C(s);
    case 0x180303:
        return fn_8016BE60(s);
    case 0x180302:
        return fn_8014DE50(s);
    case 0x180304:
        return fn_80172C28(s);
    default:
        fn_802E317C("Can't create master for arena event 0x%x", s->opts.type);
        return NULL;
    }
}

void InitializeArenaMaster(const RaceSetup* s) {
    u32 type = s->opts.type;
    lbl_8055E74C = type;
    if (type & 0x200) {
        lbl_8055E744 = CreateArenaMaster(s);
    } else {
        lbl_8055E744 = new ("null_amaster") NullArenaMaster;
    }
    damageFactor = GetEvent(s->opts.event)->node->GetFloat("damage_factor", 1.0f);
    lbl_8055E748 = 0;
    fn_80161618();
    lbl_8055E750 = 0.0f;
}

void fn_801B8318(void) { lbl_8055E744->vfF4(); }

void ResetRaceState(void) {
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

void GameMode::vf10(void) {}

void fn_801B841C(void) {
    lbl_8055E744->vf0C();
    fn_80161A18();
    if (lbl_8055E744->vf24()) {
        lbl_8055E750 += 0.016683351f;
    } else {
        lbl_8055E750 = 0.0f;
    }
}

void GameMode::vf0C(void) {}

void fn_801B848C(int p) { lbl_8055E744->vf14(p); }

void fn_801B84A4(int p) { lbl_8055E744->vf4C(p); }

void GameMode::vf4C(int p) const {}

void fn_801B84C0(int p) { lbl_8055E744->vf50(p); }

int fn_801B84D8(int p) { return lbl_8055E744->vf54(p); }

int GameMode::vf54(int p) { return 0; }

void fn_801B84F8(void) {
    lbl_8055E748 = 1;
    lbl_8055E744->vf18();
}

int GetFrameCounter(void) { return lbl_8055E748; }

int fn_801B851C(int car) {
    if (fn_80161BB8(car)) {
        return 0;
    }
    return lbl_8055E744->Started1C(car);
}

int fn_801B8570(int car) { return lbl_8055E744->Started20(car) && !fn_80161B94(); }

int fn_801B85CC(void) { return lbl_8055E744->vf24(); }

void fn_801B85E0(int a, int b, int c, int d) { lbl_8055E744->vf90((RaceCarRec*)a, (RaceCarRec*)b, (void*)c, (const f32*)d); }

void fn_801B8610(int a, int b) { lbl_8055E744->Collide(a, b); }

int fn_801B8630(int a, int b, int c) {
    if (fn_80161BDC(a, b, c)) {
        return 1;
    }
    return lbl_8055E744->vfA0((Respawn*)a, b, c);
}

void fn_801B86A4(int car) { lbl_8055E744->vfAC(car); }

void ApplyDamageFactor(int car, Damage* d) {
    lbl_8055E744->vfB4(car, d);
    d->amount *= damageFactor;
    fn_8027DED8(car, d);
}

int fn_801B8728(int p) {
    lbl_8055E744->vfB8((ZoneResults*)p);
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

int GameMode::vf8C(int p, int key) { return 0; }

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

f32 fn_801B88BC(void) {
    if (fn_80161C38()) {
        return fn_80161B70();
    } else {
        return lbl_8055E744->vf38();
    }
}

int fn_801B8900(void) { return lbl_8055E744->vf80(); }

int fn_801B8914(void) { return lbl_8055E744->vf30(); }

int fn_801B8928(int a, int b, int c) { return lbl_8055E744->vfB0(a, (Respawn*)b, c); }

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
