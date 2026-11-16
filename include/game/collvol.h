#ifndef GAME_COLLVOL_H
#define GAME_COLLVOL_H

#include "game/common.h"
#include "game/rmtx.h"

// The collision volume (vtable 0x80331C70, emitted by collvol.cpp): a physics object's set of collision shapes
// with its frame, world frame and last frame's world frame. One declaration for every unit (port ODR work,
// 2026-11-16). collvol.cpp defines most members; phob.cpp defines vf14, vf28, Notify and vf3C/vf40/vf44 (retail
// addresses 0x80177980-0x80179AF4 sit in phob's range). Slot names stay vfNN where no string names them.

class CollShape;
class RigidBody;
struct BodyDesc;
struct RPacket;

class CollVolume {
public:
    CollVolume(void);
    virtual void Init(BodyDesc* d);  // 0x08
    virtual ~CollVolume(void);       // 0x0C
    virtual void Reset(void);        // 0x10
    virtual void vf14(void);         // 0x14 (empty in the base)
    virtual void vf18(void);         // 0x18
    virtual void Update(void);       // 0x1C (calls Transform)
    virtual void vf20(void);         // 0x20
    virtual void Notify(const RVec3* v, const RVec3* at, int kind, void* c, void* by);  // 0x24 (empty in the base)
    virtual void vf28(u8* buf);      // 0x28 writes the draw state
    virtual s32 vf2C(RPacket* p);    // 0x2C packet size
    virtual void vf30(f32 t, RPacket* a, const RPacket* b, s32* na, s32* nb);  // 0x30
    virtual s32 vf34(RPacket* p);    // 0x34
    virtual void vf38(u8* out);      // 0x38
    // 0x3C: the volume as a rigid body: 0 in the base, `this` in RigidBody (retail's override is a bare blr, and
    // phob clears a RigidBody field, 0x1E4, through the result). Name unproven.
    virtual RigidBody* vf3C(void);
    virtual int vf40(void);          // 0x40
    virtual int vf44(void);          // 0x44
    void Transform(void);  // the worker Update calls
    void AddShape(CollShape* s);
    void Xform(RVec3* out, const RVec3* in);
    // Inline accessors defined in ball.cpp after their first use (retail's weak copies sit there, 0x8017EAF4/0x8017EB04).
    const s16* Hint(void);
    const RMtx* World(void);
    void SetActive(int on) { nAdded = on ? nSub : 0; }
    // The first shape, or 0 when none is active.
    CollShape* Shape(void) { return (nAdded > 0) ? sub[0] : 0; }
    // The i-th shape, or 0 when out of range.
    CollShape* Shape(int i) {
        if (i >= 0 && i < nAdded) {
            return sub[i];
        }
        return 0;
    }
    s32 a4;             // offset 0x4
    s32 a8;             // offset 0x8 (draw-state offset)
    BodyDesc* desc;     // offset 0xC
    s32 type;           // offset 0x10
    CollShape* sub[4];  // offset 0x14
    s32 nAdded;         // offset 0x24
    s32 nSub;           // offset 0x28
    f32 radius;         // offset 0x2C
    RVec3 offset;       // offset 0x30
    RMtx mtx;           // offset 0x3C
    s16 hint[2];        // offset 0x6C
    RMtx world;         // offset 0x70
    RMtx prev;          // offset 0xA0
};

#endif
