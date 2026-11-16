#ifndef GAME_CARBODY_H
#define GAME_CARBODY_H

#include "game/common.h"
#include "game/rigid.h"

// The car body (0x2700 bytes; vtable emitted by carbody.cpp): the rigid body every car, net car and trailer is.
// One declaration for carbody, netcar, springcar and trailer (port ODR work, 2026-11-16). Most of carbody.cpp still
// reaches the body through byte offsets; the named fields are the ones the other units read. Names unproven.

class StatsSrc;

// The contact plane under a wheel.
struct CbPlane {
    f32 nRight;     // offset 0x0, ground normal . body right
    f32 nFwd;       // offset 0x4, ground normal . body forward
    f32 ground[3];  // offset 0x8, contact plane anchor
};

// A wheel's per-step state in the car body (4 of them at 0x23AC).
struct CbWheel {
    CbWheel(void) {}
    f32 load;        // offset 0x0
    f32 loadN;       // offset 0x4
    f32 spin;        // offset 0x8
    f32 slip;        // offset 0xC
    f32 v10;         // offset 0x10
    f32 steer;       // offset 0x14
    f32 v18;         // offset 0x18
    f32 prevSlip;    // offset 0x1C
    u8 grounded;     // offset 0x20
    f32 contact[3];  // offset 0x24
    f32 local[3];    // offset 0x30, body-space mount point
    f32 v3C;         // offset 0x3C
    f32 v40;         // offset 0x40
    CbPlane plane;   // offset 0x44
    f32 normal[3];   // offset 0x58, contact plane normal
    s32 groundH;     // offset 0x64, fn_802B03C0's result (an int)
    f32 comp;        // offset 0x68
    s8 sound;        // offset 0x6C
    u32 flags;       // offset 0x70
};

// The state handed to a body's SetState (vtable 0x54).
struct BodyState {
    RMtx mtx;   // offset 0x0
    RVec3 vel;  // offset 0x30
    s32 a3C;    // offset 0x3C
    s16 hint;   // offset 0x40
};

class CarBody : public RigidBody {
public:
    CarBody(void);
    // The destructor is declared first: MWCC emits the vtable where the first-declared non-inline virtual is
    // defined, and Init's body is an extern "C" aid in carbody.cpp. (Overrides keep their base slots in any order.)
    virtual ~CarBody(void);           // 0x0C
    virtual void Init(BodyDesc* d);   // 0x08
    virtual void Reset(void);         // 0x10
    virtual void vf14(void);          // 0x14
    virtual void Update(void);        // 0x1C (empty)
    virtual void vf20(void);          // 0x20
    virtual void Notify(const RVec3* v, const RVec3* at, int kind, void* c, void* by);  // 0x24
    virtual void vf28(u8* buf);       // 0x28
    virtual s32 vf2C(RPacket* pk);    // 0x2C: 0x44 bytes
    virtual void vf30(f32 t, RPacket* a, const RPacket* b, s32* na, s32* nb);  // 0x30
    virtual s32 vf34(RPacket* pk);    // 0x34
    virtual void OnHit(const RVec3* a, const RVec3* b, int c, void* d);  // 0x48
    virtual void OnImpulse(const RVec3* v, const RVec3* at, int kind, void* c, void* by);  // 0x4C
    virtual void SetState(const BodyState* s);  // 0x54: places the car at a pose
    virtual void vf58(void);          // 0x58
    virtual void vf5C(void);          // 0x5C

    s32 index;             // offset 0x1EC
    u8 pad1F0[0x270 - 0x1F0];
    StatsSrc* src;         // offset 0x270
    u8 pad274[0x59C - 0x274];
    s32 model;             // offset 0x59C
    u8 pad5A0[0x23AC - 0x5A0];
    CbWheel wheels[4];     // offset 0x23AC
    u8 pad257C[0x25D4 - 0x257C];
    f32 speed;             // offset 0x25D4
    u8 pad25D8[0x2610 - 0x25D8];
    s32 link;              // offset 0x2610
    u8 pad2614[0x2620 - 0x2614];
    s32 airborne;          // offset 0x2620
    u8 pad2624[0x267C - 0x2624];
    s32 f267C;             // offset 0x267C
    u8 pad2680[0x268C - 0x2680];
    s32 a268C;             // offset 0x268C
    f32 f2690;             // offset 0x2690
    f32 f2694;             // offset 0x2694
    u8 pad2698[0x26B4 - 0x2698];
    RVec3 push;            // offset 0x26B4
    u8 pad26C0[0x2700 - 0x26C0];
};

#endif
