#ifndef GAME_GAMEMODE_H
#define GAME_GAMEMODE_H
// The race mode interface (GameUI) and its common base (GameMode): one per mode (amaster arena, betarace,
// crushmaster, ringchal, zone; aidriver/aiutil/modeinl define a few defaults). __vt__8GameMode lives with amaster.cpp,
// which defines the first out-of-line virtual (vf0C). The inline members are exactly retail's weak copies (GameUI:
// in overlay/racing; GameMode: in zone, betarace); the others are defined once (amaster, modeinl, aiutil, aidriver).
// Slot signatures take the widest argument list a caller passes (amaster's forwarders pass every register) and
// the overrides' pointer types. One declaration for every unit (port ODR work, 2026-11-16). vfXX names unproven.
#include "game/common.h"
#include "game/respawn.h"

struct RaceCarRec;
struct Damage;
struct ZoneResults;
extern "C" int fn_801B8514(void);

class GameUI {
public:
    virtual ~GameUI(void) {}
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(int p);
    virtual void vf18(void);
    virtual int vf1C(int car);
    virtual int vf20(int car) const;
    virtual int vf24(void) const;
    virtual int Started1C(int car);
    virtual int Started20(int car);
    virtual int vf30(void);
    virtual int vf34(void);
    virtual f32 vf38(void);
    virtual int vf3C(int p);
    virtual void vf40(void);
    virtual void vf44(void);
    virtual void vf48(void);
    virtual void vf4C(int p) const;
    virtual void vf50(int p);
    virtual int vf54(int p);
    virtual int vf58(void) { return 1; }
    virtual int vf5C(int p) const { return 0; }
    virtual void vf60(int view) {}
    virtual int vf64(void) { return 0; }
    virtual int vf68(void) const { return 1; }
    virtual void vf6C(void);
    virtual void vf70(void);
    virtual void vf74(void);
    virtual void vf78(void);
    virtual int vf7C(int p);
    virtual int vf80(void);
    virtual int vf84(void);
    virtual int vf88(void) const;
    virtual int vf8C(int p, int key);
    virtual void vf90(RaceCarRec* a, RaceCarRec* b, void* p, const f32* hit);
    virtual void vf94(RaceCarRec* rc);
    virtual int Collide(int a, int b);
    virtual void vf9C(RaceCarRec* rc, int type, void* other);
    virtual int vfA0(Respawn* out, int idx, int c);
    virtual void vfA4(RaceCarRec* rc);
    virtual void vfA8(int a, int b);
    virtual void vfAC(int car);
    virtual int vfB0(int car, Respawn* r, int c);
    virtual void vfB4(int car, Damage* d);
    virtual void vfB8(ZoneResults* r);
    virtual int vfBC(void);
    virtual void vfC0(void);
    virtual void vfC4(void);
    virtual void vfC8(void);
    virtual int vfCC(void);
    virtual int vfD0(void);
    virtual int vfD4(int car);
    virtual int vfD8(void);
    virtual int vfDC(void);
    virtual int vfE0(int car, u8* out);
    virtual int vfE4(void);
    virtual void* vfE8(void);
    virtual int vfEC(void);
    virtual int vfF0(void);
    virtual void vfF4(void);
    virtual void vfF8(void) {}
    virtual void vfFC(void) {}
};

class GameMode : public GameUI {
public:
    GameMode(void);  // inline body: game/gamemodector.h
    virtual ~GameMode(void) {}
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(int p) {}
    virtual void vf18(void) {}
    virtual int vf1C(int car) { return vf20(car); }
    virtual int vf20(int car) const = 0;
    virtual int vf24(void) const;
    virtual int Started1C(int car);
    virtual int Started20(int car);
    virtual int vf30(void) { return 0; }
    virtual int vf34(void) { return 0; }
    virtual f32 vf38(void);
    virtual int vf3C(int p);
    virtual void vf40(void) {}
    virtual void vf44(void);
    virtual void vf48(void) {}
    virtual void vf4C(int p) const;
    virtual void vf50(int p);
    virtual int vf54(int p);
    virtual void vf6C(void) {}
    virtual void vf70(void) {}
    virtual void vf74(void) {}
    virtual void vf78(void) {}
    virtual int vf7C(int p) { return 0; }
    virtual int vf80(void) { return 0; }
    virtual int vf84(void) { return 0; }
    virtual int vf88(void) const { return 0; }
    virtual int vf8C(int p, int key);
    virtual void vf90(RaceCarRec* a, RaceCarRec* b, void* p, const f32* hit) {}
    virtual void vf94(RaceCarRec* rc) {}
    virtual int Collide(int a, int b);
    virtual void vf9C(RaceCarRec* rc, int type, void* other) {}
    virtual int vfA0(Respawn* out, int idx, int c) { return 0; }
    virtual void vfA4(RaceCarRec* rc) {}
    virtual void vfA8(int a, int b);
    virtual void vfAC(int car);
    virtual int vfB0(int car, Respawn* r, int c) { return 0; }
    virtual void vfB4(int car, Damage* d) {}
    virtual void vfB8(ZoneResults* r) = 0;
    virtual int vfBC(void) { return 0; }
    virtual void vfC0(void) {}
    virtual void vfC4(void) {}
    virtual void vfC8(void) {}
    virtual int vfCC(void) { return 1; }
    virtual int vfD0(void) { return 0; }
    virtual int vfD4(int car) {
        int r = 0;
        if (fn_801B8514() && !vf20(car)) {
            r = 1;
        }
        return r;
    }
    virtual int vfD8(void) { return 0; }
    virtual int vfDC(void) { return 1; }
    virtual int vfE0(int car, u8* out) { return 0; }
    virtual int vfE4(void) { return 0; }
    virtual void* vfE8(void);
    virtual int vfEC(void);
    virtual int vfF0(void) { return 0; }
    virtual void vfF4(void) {}

    void FillResults(void* r);
    int Reset(void);
};

#endif
