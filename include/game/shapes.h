#ifndef GAME_SHAPES_H
#define GAME_SHAPES_H

#include "game/common.h"
#include "game/rvec.h"
#include "game/rmtx.h"
#include "game/collshape.h"

// The collision shape classes (sphcoll.cpp defines their members and emits their vtables). One declaration for
// every unit that builds or uses them (port ODR work, 2026-11-16). Constructors other units inline are declared
// inline here. Field names unproven unless noted.

class CollVolume;

extern "C" void fn_8019EECC(u8* owner, RVec3* c);  // registers a sphere's centre with its owner (sphcoll)

// A sphere shape ('SPHR'; vtable __vt__11SphereShape): radius 0x24, local centre 0x28, world centre 0x34.
class SphereShape : public CollShape {
public:
    SphereShape(u32 data, u32 swap);  // from file data, byte-swapped when `swap`
    // Parameter form, inlined into every builder (ball, mutphob, obstacles, trackobj, wheel). The owner test reads
    // the member: trackobj passes a null owner and retail still keeps the call.
    SphereShape(CollVolume* o, const RMtx* m, f32 k, f32 s, f32 r, const RVec3& c)
        : CollShape((const CFrame*)m, o, 'SPHR', k, s) {
        center = c;
        radius = r;
        f34 = 0.0f;
        f38 = 0.0f;
        f3C = 0.0f;
        f40 = 0.0f;
        f44 = 0.0f;
        f48 = 0.0f;
        b4C = 0;
        f50 = 0.0f;
        center.x = c.x;
        center.y = c.y;
        center.z = c.z;
        if (owner) {
            fn_8019EECC((u8*)owner, &center);
        }
    }
    virtual void vf0C(void);
    virtual void vf10(u8* other);
    virtual void vf14(void);
    virtual int vf18(const RVec3* from, const RVec3* dir, f32* t, RVec3* n);
    virtual void vf28(void);
    virtual void vf38(RVec3* lo, RVec3* hi) const;
    virtual f32 Radius(void);
    virtual void vf4C(void);
    virtual ~SphereShape(void) {}
    f32 radius;    // offset 0x24
    RVec3 center;  // offset 0x28
    f32 f34;       // offset 0x34
    f32 f38;       // offset 0x38
    f32 f3C;       // offset 0x3C
    f32 f40;       // offset 0x40
    f32 f44;       // offset 0x44
    f32 f48;       // offset 0x48
    u8 b4C;        // offset 0x4C
    f32 f50;       // offset 0x50
};

#endif
