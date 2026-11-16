#ifndef GAME_SPRINGCAR_H
#define GAME_SPRINGCAR_H

#include "game/common.h"
#include "game/carbody.h"

// The player/AI-driven car body (springcar.cpp; vtable there): a CarBody steered by a Controller, kept on the track
// by spring walls and a lift. One declaration for every unit (port ODR work, 2026-11-16). Names unproven.

class SpringCar;

// The object steering the car (vtable at 0x0).
class Controller {
public:
    virtual ~Controller(void);
    virtual void Init(SpringCar* car, void* desc);                       // 0x0C
    virtual void Attach(SpringCar* car);                                 // 0x10
    virtual int vf14(void);                                              // 0x14
    virtual int vf18(u8* buf);                                           // 0x18
    virtual void vf1C(f32 t, int a, int b, int* na, int* nb);            // 0x1C
    virtual int vf20(u8* p);                                             // 0x20
    virtual void Reset(void);                                            // 0x24
    virtual void Detach(void);                                           // 0x28
    virtual void Update(void);                                           // 0x2C
    virtual void vf30(void);
    virtual void vf34(void);                                             // 0x34
    virtual void vf38(const RVec3* a, const RVec3* b, int kind, int c);    // 0x38
    virtual void vf3C(const RVec3* a, const RVec3* b, int kind, int c);    // 0x3C
    virtual void vf40(void);                                             // 0x40
    virtual void vf44(void);                                             // 0x44
};

struct WallPlane {
    u8 pad0[0x24];
    RVec3 origin;  // offset 0x24
};

struct WallObj {
    u8 pad0[0xC];
    WallPlane* plane;  // offset 0xC
};

struct Lift {
    void Reset(void) {
        state = 1;
        count = 0;
        t = 0.0f;
        f = 0.0f;
    }
    void Apply(SpringCar* car);
    void Step(SpringCar* car, f32 rate);

    s32 state;  // offset 0x0
    f32 f;      // offset 0x4
    f32 t;      // offset 0x8
    s32 count;  // offset 0xC
};

struct SpringWall {
public:
    int Ramping(void);
    void Apply(SpringCar* car, RVec3* acc);
    bool Free(void) const { return owner == 0; }
    void Set(WallObj* w, SpringCar* car, const RVec3* other);

    RVec3 n;           // offset 0x0
    f32 vel;          // offset 0xC
    f32 depth;        // offset 0x10
    f32 dist;         // offset 0x14
    f32 time;         // offset 0x18
    f32 ramp;         // offset 0x1C
    WallObj* owner;   // offset 0x20
};

class SpringCar : public CarBody {
public:
    SpringCar(Controller* c);
    virtual void Init(BodyDesc* desc);
    virtual ~SpringCar(void);
    virtual void Reset(void);
    virtual void vf14(void);
    virtual void vf28(u8* buf);
    virtual s32 vf2C(RPacket* pk);
    virtual void vf30(f32 t, RPacket* a, const RPacket* b, s32* na, s32* nb);
    virtual s32 vf34(RPacket* pk);
    virtual void OnHit(const RVec3* a, const RVec3* b, int c, void* d);  // 0x48
    virtual void OnImpulse(const RVec3* v, const RVec3* at, int kind, void* c, void* by);  // 0x4C
    virtual void SetState(const BodyState* s);  // 0x54
    virtual void vf58(void);
    virtual void vf5C(void);  // 0x5C: one frame (the controller's update)
    void SetController(Controller* c);
    void CheckLost(void);
    void HitWall(WallObj* w);
    void ClearWalls(void) {
        int i;
        for (i = 0; i < 8; i++) {
            walls[i].owner = 0;
        }
    }

    s32 lost;              // offset 0x2700
    u8 dead;               // offset 0x2704
    s32 f2708;             // offset 0x2708
    Controller* ctl;       // offset 0x270C
    u8 lifting;            // offset 0x2710
    Lift lift;             // offset 0x2714
    SpringWall walls[8];   // offset 0x2724
};

#endif
