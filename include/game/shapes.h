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

extern "C" {
void fn_8019EECC(u8* owner, RVec3* c);  // adds the owner's offset (CollVolume 0x30) to a centre (sphcoll)
double sqrt(double x);
}

// The classes below are declared in sphcoll's original order: MWCC emits a unit's vtables in reverse declaration
// order, so this order is what puts sphcoll's tables where retail has them.

// A sphere shape ('SPHR'; vtable __vt__11SphereShape): radius 0x24, local centre 0x28, world centre 0x34.
class SphereShape : public CollShape {
public:
    SphereShape(u32 data, u32 swap);  // from file data, byte-swapped when `swap`
    // Parameter form, inlined into every builder (ball, mutphob, obstacles, trackobj, wheel). The owner test reads
    // the member: trackobj passes a null owner and retail still keeps the call.
    SphereShape(CollVolume* o, const RMtx* m, f32 k, f32 s, f32 r, const RVec3& c)
        : CollShape(m, o, 'SPHR', k, s) {
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
    virtual ~SphereShape(void);
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

// A flat sphere ('FLSR', 0x5C bytes; vtable __vt__15FlatSphereShape): radius `across` across its axis, `along`
// along it; bounded by the larger. Built only by obstacles' inlined constructor.
inline f32 FlatSphereBound(f32 a, f32 b) { return a > b ? a : b; }  // helper name unproven
class FlatSphereShape : public SphereShape {
public:
    FlatSphereShape(CollVolume* o, const RMtx* m, f32 k, f32 s, f32 a, f32 b, const RVec3& c)
        : SphereShape(o, m, k, s, FlatSphereBound(a, b), c) {
        across = a;
        along = b;
        type = 'FLSR';
    }
    virtual void vf10(u8* other);
    virtual void vf14(void);
    virtual ~FlatSphereShape(void);
    f32 across;  // offset 0x54
    f32 along;   // offset 0x58
};

// A sphere re-centred on its attachment (+0x54) each step (vtable 0x80333AA8). Nothing constructs it in retail;
// its vtable is emitted because its members are defined in sphcoll. Class name unproven.
class AttachedSphereShape : public SphereShape {
public:
    virtual void vf0C(void);
    virtual void vf14(void);
    virtual void vf24(const RVec3* v, const RVec3* at, int kind, void* c, void* by);
    virtual ~AttachedSphereShape(void);
    u8* attach;  // offset 0x54: the attachment
    f32 f58;     // offset 0x58
};

// A sphere bounding a box, with the box half sizes and a 24-float cache (vtable 0x80333A58; constructor at 0x8019F564,
// which mutphob calls). Class name unproven.
class BoxSphereShape : public SphereShape {
public:
    BoxSphereShape(const RMtx* m, const RVec3* c, CollVolume* o, f32 k, f32 skin, f32 a, f32 hx, f32 hy, f32 hz);
    virtual void vf14(void);
    virtual void vf28(void);
    virtual ~BoxSphereShape(void);
    PlainRVec3 halfBox;  // offset 0x54: the box half sizes
    f32 cache[24];       // offset 0x60
    f32 fC0;             // offset 0xC0
};

// A sphere whose local centre is reset from +0x54 (vtable 0x80333A08). Nothing constructs it in retail.
// Class name unproven.
class OffsetSphereShape : public SphereShape {
public:
    virtual void vf1C(void);
    virtual void vf28(void);
    virtual ~OffsetSphereShape(void);
    PlainRVec3 localOffset;  // offset 0x54: the local centre is reset from here
    u8 pad60[0x78 - 0x60];
    u8 b78;                  // offset 0x78
};

// A group of collision volumes ('GRUP', 0x184 bytes; vtable 0x803339B8): parts at 0x178 (count 0x174) and a
// child set at 0x17C. Built by fn_8019FFD8, which carbody and obstacles call. Class name unproven.
class SphereGroup : public CollShape {
public:
    SphereGroup(const RMtx* frame, CollVolume* owner);
    virtual ~SphereGroup(void);
    virtual void vf0C(void);
    virtual void vf10(u8* other);
    virtual void vf14(void);
    virtual int vf18(const RVec3* from, const RVec3* dir, f32* t, RVec3* n);
    virtual void vf28(void);
    virtual void vf38(RVec3* lo, RVec3* hi) const;
    virtual f32 Radius(void);
    virtual void SetSurface(int m);
    virtual void vf44(u32 v);
    virtual void vf48(void);
    virtual void vf4C(void);
    u8 pad24[0x164 - 0x24];
    f32 f164;   // offset 0x164
    f32 f168;   // offset 0x168
    f32 f16C;   // offset 0x16C
    f32 f170;   // offset 0x170
    s32 count;  // offset 0x174: parts
    f32 f178;   // offset 0x178
    u8* child;  // offset 0x17C: the child set
    s32 f180;   // offset 0x180
};

// The capsule base of TubeShape (vtable 0x80333968): radius 0x24, half length 0x28 along the frame's axis.
// Abstract (vf10 and vf38 stay pure). Class name unproven.
class TubeBase : public CollShape {
public:
    TubeBase(u32 data, u32 swap);  // from file data, byte-swapped when `swap`
    TubeBase(CollVolume* o, const RMtx* m, f32 k, f32 s, f32 r, f32 h) : CollShape(m, o, 'TUBE', k, s) {
        rad = r;
        half = h;
    }
    TubeBase(f32 k, f32 s, f32 r, f32 h) : CollShape(0, 0, 'TUBE', k, s) {
        rad = r;
        half = h;
    }
    TubeBase(f32 k, f32 r, f32 h) : CollShape(0, 0, 'TUBE', k, 0.2f) {  // tree's form
        rad = r;
        half = h;
    }
    virtual int vf18(const RVec3* from, const RVec3* dir, f32* t, RVec3* n);
    virtual f32 Radius(void);
    virtual void vf4C(void);
    virtual ~TubeBase(void) {}
    f32 rad;   // offset 0x24
    f32 half;  // offset 0x28: half the length between the end spheres
};

// A tube ('TUBE'; vtable __vt__9TubeShape).
class TubeShape : public TubeBase {
public:
    TubeShape(u32 data, u32 swap);  // from file data, byte-swapped when `swap`
    TubeShape(CollVolume* o, const RMtx* m, f32 k, f32 s, f32 r, f32 h) : TubeBase(o, m, k, s, r, h) {}
    TubeShape(f32 k, f32 s, f32 r, f32 h) : TubeBase(k, s, r, h) {}
    TubeShape(f32 k, f32 r, f32 h) : TubeBase(k, r, h) {}
    virtual void vf10(u8* other);
    virtual void vf14(void);
    virtual void vf38(RVec3* lo, RVec3* hi) const;
    virtual void vf4C(void);
    virtual ~TubeShape(void);
};

// A box ('BOX '; vtable __vt__8BoxShape): half sizes 0x24, bounding radius 0x30. Adds slot vf50. Each builder
// inlines its own constructor form (overloads below; their bodies differ in retail, e.g. the radius sum order).
enum BoxWallForm { BOX_WALL };  // selects BoxShape's track wall constructor
class BoxShape : public CollShape {
public:
    BoxShape(u32 data, u32 swap);  // from file data, byte-swapped when `swap`
    // Owner and frame form (obstacles; sphcoll's OrientedBoxShape constructor).
    BoxShape(CollVolume* o, const RMtx* m, f32 k, f32 s, f32 x, f32 y, f32 z) : CollShape(m, o, 'BOX ', k, s) {
        half.x = x;
        half.y = y;
        half.z = z;
        a34 = 0;
        radius = 0.2f + (f32)sqrt(z * z + (x * x + y * y));
    }
    // Frame-only form with the default stiffness and skin (collvol's box volume).
    BoxShape(const RMtx* m, f32 x, f32 y, f32 z) : CollShape(m, 0, 'BOX ', 50000.0f, 0.2f) {
        half.x = x;
        half.y = y;
        half.z = z;
        a34 = 0;
        radius = 0.2f + (f32)sqrt(x * x + y * y + z * z);
    }
    // Half sizes form with every field written here (betarace's spin volume; store order as retail).
    BoxShape(f32 x, f32 y, f32 z) {
        owner = 0;
        data = 0;
        frame = 0;
        stiff = 0.0f;
        skin = 0.0f;
        type = 'BOX ';
        x1C = 0;
        surface = 100;
        half.x = x;
        half.y = y;
        half.z = z;
        a34 = 0;
        radius = 0.2f + (f32)sqrt(x * x + (y * y + z * z));
    }
    // Full sizes form, halved here (trackobj's object volumes).
    BoxShape(f32 k, f32 s, const f32* dims) : CollShape(0, 0, 'BOX ', k, s) {
        f32 h = 0.5f;
        const f32 halfY = dims[1] * h;
        const f32 halfX = dims[0] * h;
        const f32 halfZ = dims[2] * h;
        half.x = halfX;
        half.y = halfY;
        half.z = halfZ;
        a34 = 0;
        radius = 0.2f + (f32)sqrt(half.x * half.x + half.y * half.y + half.z * half.z);
    }
    // Track wall form (world's "wallvol" boxes): width, height, length; the radius pads by the width.
    BoxShape(BoxWallForm, f32 w, f32 h, f32 l) : CollShape(0, 0, 'BOX ', 0.0f, 0.0f) {
        half.y = h;
        half.x = w;
        half.z = l;
        a34 = 0;
        radius = w + (f32)sqrt(half.x * half.x + half.y * half.y + half.z * half.z);
    }
    virtual void vf10(u8* other);
    virtual int vf18(const RVec3* from, const RVec3* dir, f32* t, RVec3* n);
    virtual void vf38(RVec3* lo, RVec3* hi) const;
    virtual f32 Radius(void);
    virtual void vf4C(void);
    virtual ~BoxShape(void) {}
    virtual u32 vf50(void);
    RVec3 half;  // offset 0x24
    f32 radius;  // offset 0x30
    s32 a34;     // offset 0x34
};

// A box that keeps its own copy of the frame's rotation (0x44) and world centre (0x68) (vtable 0x80333868; built
// by fn_801A2354). Adds slot vf54 (that rotation). Class name unproven.
class OrientedBoxShape : public BoxShape {
public:
    OrientedBoxShape(const RMtx* frame, const RVec3* c, CollVolume* owner, f32 k, f32 skin, f32 hx, f32 hy, f32 hz);
    virtual void vf0C(void);
    virtual u8* vf54(void);
    virtual ~OrientedBoxShape(void);
    PlainRVec3 centre;   // offset 0x38
    f32 rot[9];          // offset 0x44: its own copy of the frame's rotation (vf54)
    PlainRVec3 world;    // offset 0x68: world centre
};

// A ragdoll limb volume ('RAGD'; vtable 0x80333818; built by panelvol's strips). Class name unproven.
class RagdollShape : public CollShape {
public:
    RagdollShape(const RMtx* frame, void* strip, CollVolume* owner, f32 r);
    virtual void vf10(u8* other);
    virtual void vf38(RVec3* lo, RVec3* hi) const;
    virtual f32 Radius(void);
    virtual void vf4C(void);
    virtual ~RagdollShape(void);
    f32 radius;  // offset 0x24
    s32 f28;     // offset 0x28
    s32 f2C;     // offset 0x2C
    s32 f30;     // offset 0x30
};

// A ring ('RING'; vtable __vt__7RingVol): inner radius 0x24, ring width 0x28.
class RingVol : public CollShape {
public:
    RingVol(u32 data, u32 swap);  // from file data, byte-swapped when `swap`
    RingVol(f32 k, f32 s, f32 r, f32 w) : CollShape(0, 0, 'RING', k, s) {  // trackobj's form
        inner = r;
        ring = w;
    }
    virtual void vf10(u8* other);
    virtual void vf14(void);
    virtual void vf38(RVec3* lo, RVec3* hi) const;
    virtual f32 Radius(void);
    virtual void vf4C(void);
    virtual ~RingVol(void);
    f32 inner;  // offset 0x24
    f32 ring;   // offset 0x28
};

struct PolyPlane;  // a plane: normal + distance (sphcoll)

// A convex polygon volume ('POLY', 0xE0 bytes; vtable 0x80333778; built by fn_801A3190 / the named-table constructor at 0x801A324C). Class name
// unproven.
class PolyShape : public CollShape {
public:
    PolyShape(const RMtx* frame, const PolyPlane* planes, s32 n, const RVec3* lo, const RVec3* hi, f32 k, f32 skin);
    PolyShape(const RMtx* frame, const char* name, f32 k, f32 skin);  // from the named .val table (trackobj)
    virtual void vf10(u8* other);
    virtual void vf38(RVec3* lo, RVec3* hi) const;
    virtual f32 Radius(void);
    virtual void vf4C(void);
    virtual ~PolyShape(void);
    PlainRVec3 lo;   // offset 0x24: bounds
    PlainRVec3 hi;   // offset 0x30
    u8 pad3C[0xBC - 0x3C];
    s32 n;           // offset 0xBC: planes
    u8 padC0[0xDC - 0xC0];
    s32 fDC;         // offset 0xDC
};

// A mesh volume ('PMSH'; vtable 0x80333728; built from a named mesh by obstacles, or from file data); owns its mesh
// at 0x24. Class name unproven.
class MeshShape : public CollShape {
public:
    MeshShape(const RMtx* frame, const char* name, CollVolume* owner);  // from the named mesh (0x40 bytes)
    MeshShape(u32 data, u32 inPlace);  // from file data; the mesh is built in place at 0x40 when `inPlace`
    virtual ~MeshShape(void);
    virtual void vf10(u8* other);
    virtual int vf18(const RVec3* from, const RVec3* dir, f32* t, RVec3* n);
    virtual void vf38(RVec3* lo, RVec3* hi) const;
    virtual f32 Radius(void);
    virtual void vf4C(void);
    u8* mesh;  // offset 0x24: its mesh (built in place at 0x40 from file data)
    u8 pad28[0x40 - 0x28];
};

// A static world object with a collision volume (vtable 0x803336F8; built by the constructor at 0x801A4A8C, or from file data):
// pose 0x04, its collision shape 0x3C (sol bounds and deletes it through it), the next object 0x44. Not a CollShape.
// Declared last, as in sphcoll. Class and slot names unproven.
class StaticCollObject {
public:
    StaticCollObject(bool swap, u8* vol);  // from file data; its volume is built in place at vol
    StaticCollObject(u32 a, u32 b, CollShape* s, const PlainValMatrix* f, ValueNode* c);  // trackobj's solid objects
    virtual u32 vf08(void);
    virtual void vf0C(void);
    virtual u8* vf10(void);
    virtual u8 vf14(void);
    virtual void vf18(u32 v);
    virtual void vf1C(int surface);  // sets the shape's surface (CollShape::SetSurface); actseq's SurfBody 0x1C
    virtual s32 vf20(void);
    virtual u32 vf24(void);
    virtual void vf28(u32 v);
    virtual ~StaticCollObject(void);
    u8 pad4[0x34 - 0x4];
    u32 f34;                 // offset 0x34
    u32 f38;                 // offset 0x38
    CollShape* shape;        // offset 0x3C
    u8 b40;                  // offset 0x40
    u8 pad41[0x44 - 0x41];
    StaticCollObject* next;  // offset 0x44
    u8* vol;                 // offset 0x48: its volume, built in place
};

#endif
