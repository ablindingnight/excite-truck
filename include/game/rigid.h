#ifndef GAME_RIGID_H
#define GAME_RIGID_H

#include "game/common.h"
#include "game/rmtx.h"
#include "game/collvol.h"

// The rigid body (vtable emitted by rigid.cpp, which defines Init, its first non-inline virtual): a collision
// volume with mass, inertia, velocities, accumulated forces and the impulses other shapes pushed on it this
// step. One declaration for every unit (port ODR work, 2026-11-16). The empty or constant virtuals (vf18, vf2C,
// vf40, vf3C) are inline: retail keeps them as weak copies. Rot, InvInertia and NotifyAll are inline members
// whose bodies sit in rigid.cpp after their first use (MWCC places their code by that order); other units do
// not call them. Field and method names unproven unless a string names them.

class CollShape;
struct Corner;  // a contact point (corners.cpp)
struct ImpRec;  // one pending impulse (rigid.cpp)

// A rigid body's description: mass and the inertia diagonal after the BodyDesc head (Init passes them to SetMass).
struct RigidDesc : BodyDesc {
    f32 mass;  // offset 0x8
    f32 ix;    // offset 0xC
    f32 iy;    // offset 0x10
    f32 iz;    // offset 0x14
};

class RigidBody : public CollVolume {
public:
    RMat33* InvInertia(void);  // inline, defined in rigid.cpp
public:
    RMat33* Rot(void);  // inline, defined in rigid.cpp
public:
    RigidBody(void);
    virtual void Init(BodyDesc* d);
    virtual void Reset(void);
    virtual void vf18(void) {}
    virtual void Update(void);
    virtual s32 vf2C(RPacket* p) { return 0x20; }
    virtual void vf30(f32 t, RPacket* a, const RPacket* b, s32* na, s32* nb);
    virtual s32 vf34(RPacket* p);
    virtual int vf40(void) { return 1; }
    virtual RigidBody* vf3C(void) { return this; }
    virtual void OnHit(const RVec3* f, const RVec3* at, int kind, void* c);  // 0x48
    virtual void OnImpulse(const RVec3* v, const RVec3* at, int kind, void* c, void* by) {}  // 0x4C
    virtual void OnImpulses(RVec3* f, RVec3* t) {}  // 0x50
    void ResetState(void);
    void SetMass(f32 m, f32 ix, f32 iy, f32 iz);
    void AddForceAt(const RVec3* f, const RVec3* at);
    void AddTorque(const RVec3* t);
    void AddImpulse(f32 w, const RVec3* v, const RVec3* at);
    void Push(CollShape* obj, void* by, const RVec3* v, const RVec3* at, int kind, const RVec3* at2, void* c, f32 w);
    void ClearImpulses(void);
    void NotifyAll(void);  // inline, defined in rigid.cpp
    void VelAt(RVec3* out, const RVec3* p);
    void VelAt2(RVec3* out, const RVec3* p);
    void Flush(RVec3* f, RVec3* t);
    void SetState(const RMtx* m, const RVec3* v, const RVec3* w);
    void UpdateCell(void);
    const RVec3* Vel(void);  // inline, defined in ball.cpp after its first use (weak copy 0x8017EAFC)
    Corner* contacts;  // offset 0xD0
    f32 mass;           // offset 0xD4
    RVec3 inertia;      // offset 0xD8
    f32 invMass;        // offset 0xE4
    RVec3 invInertia;   // offset 0xE8
    RMat33 invI;        // offset 0xF4
    RVec3 vel;          // offset 0x118
    RVec3 avel;         // offset 0x124
    RVec3 force;        // offset 0x130
    RVec3 torque;       // offset 0x13C
    RMat33 worldInvI;   // offset 0x148
    RVec3 accel;        // offset 0x16C
    RVec3 aaccel;       // offset 0x178
    void* cell;         // offset 0x184
    RVec3 impF;         // offset 0x188
    RVec3 impT;         // offset 0x194
    f32 impW;           // offset 0x1A0
    ImpRec* imps[16];   // offset 0x1A4
    s32 nImp;           // offset 0x1E4
    u8 b1E8;            // offset 0x1E8
    u8 b1E9;            // offset 0x1E9
};

#endif
