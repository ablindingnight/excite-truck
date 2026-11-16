// Track obstacles (0x801A9660-0x801ADB44): the obstacle factory ("fencepst", "obst", "DynamicObstacle"),
// static and dynamic obstacle rigid bodies, the extents/plane/polyvol/pmshvol readers and the
// fence-pole binding ("fencepole: binding to %p"). Some names are descriptive.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

class VO {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(const char* a, const char* b);
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
    virtual void v48(const f32* push, const f32* at, int force, int flags);
};

// A 12-word body pose.
struct Pose12 {
    u32 w[12];
};

extern s32 lbl_8055E3C4;

extern "C" {
f32 PSVECDotProduct(const f32* a, const f32* b);
void PSVECCrossProduct(const f32* a, const f32* b, f32* out);
}

struct ObVec3 {
    f32 x, y, z;
};

// A visitor called for each obstacle in a linked chain.
class ObstacleSink;

namespace {
class VVisit;
}

struct CVec3 {
    f32 x, y, z;
};

struct BodyDesc;
struct CollShape;
struct ObDesc;

struct RPacket;

// The collision volume base (collvol.cpp). The slot names are the ones retail's tables use here; the
// vtable itself is emitted by collvol.cpp.
class CollVolume {
public:
    virtual void Init(BodyDesc* d);  // 0x08
    virtual ~CollVolume(void);       // 0x0C
    virtual void Reset(void);        // 0x10
    virtual void vf14(void);         // 0x14
    virtual void vf18(void);         // 0x18
    virtual void Update(void);       // 0x1C
    virtual void vf20(void);         // 0x20
    virtual void vf24(void);         // 0x24
    virtual void vf28(u8* buf);      // 0x28 writes the draw state
    virtual s32 vf2C(void);          // 0x2C
    virtual void vf30(f32 t, RPacket* a, RPacket* b, s32* na, s32* nb);  // 0x30
    virtual s32 vf34(RPacket* p);    // 0x34
    virtual void vf38(u8* p);        // 0x38
    virtual void vf3C(void);         // 0x3C
    virtual int vf40(void);          // 0x40
    virtual int vf44(void);          // 0x44
    void AddShape(CollShape* s);
    // The i-th collision shape, or 0 when out of range.
    CollShape* Shape(int i) {
        if (i >= 0 && i < count) {
            return shapes[i];
        }
        return 0;
    }
    u8 pad4[0x14 - 0x4];
    CollShape* shapes[4];  // offset 0x14
    s32 count;             // offset 0x24
    u8 pad28[0x3C - 0x28];
    f32 frame[12];  // offset 0x3C
    void Xform(CVec3* out, const CVec3* in);
};

struct RVec3 {
    RVec3(void) {}
    RVec3(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
    f32 x;
    f32 y;
    f32 z;
};

// The rigid body (rigid.cpp); its vtable is emitted there.
class RigidBody : public CollVolume {
public:
    RigidBody(void);
    virtual void Init(BodyDesc* d);
    virtual ~RigidBody(void) {}
    virtual void Reset(void);
    virtual void vf18(void);
    virtual void Update(void);
    virtual s32 vf2C(void);
    virtual void vf30(f32 t, RPacket* a, RPacket* b, s32* na, s32* nb);
    virtual s32 vf34(RPacket* p);
    virtual void vf3C(void);
    virtual int vf40(void);
    virtual void OnHit(const RVec3* v, const RVec3* at, int force, void* extra);                // 0x48
    virtual void OnImpulse(const RVec3* v, const RVec3* at, int force, void* extra, void* by);  // 0x4C
    virtual void OnImpulses(RVec3* f, RVec3* t);                                              // 0x50
    void Flush(RVec3* a, RVec3* b);
    void VelAt(RVec3* out, const RVec3* at);
    void AddForceAt(const RVec3* f, const RVec3* at);
    u8 pad6C[0x1EC - 0x6C];
};

extern "C" void fn_801AC240(u8* p);

namespace {

/* Class name unproven */
// The interface a dynamic (networked, movable) obstacle exposes to its owner, at 0x23C.
class DynamicLink {
public:
    virtual u32 Owner(void) = 0;                                                        // 0x08
    virtual void Describe(void) = 0;                                                    // 0x0C
    virtual u8* Position(void) = 0;                                                     // 0x10
    virtual u8* Velocity(void) = 0;                                                     // 0x14
    virtual s32 PartId(void) = 0;                                                       // 0x18
    virtual u8 IsAsleep(void) = 0;                                                      // 0x1C
    virtual void SetAwake(int on) = 0;                                                  // 0x20
    virtual void Push(f32* v, f32* off) = 0;                                            // 0x24
    virtual void SetState(const Pose12* pose, const ObVec3* lo, const ObVec3* hi) = 0;  // 0x28
    virtual void Rest(void) = 0;                                                        // 0x2C
    u32 owner;  // offset 0x4
};

// A visitor called for each obstacle in a linked chain.
class VVisit {
public:
    virtual void Visit(ObstacleSink* o) = 0;
};


/* Class name unproven */
// A rigid body with no network state: the packet slots report nothing.
class ObstacleBody : public RigidBody {
public:
    ObstacleBody(void) {}
    virtual ~ObstacleBody(void);
    virtual s32 vf2C(void) { return 0; }
    virtual void vf30(f32 t, RPacket* a, RPacket* b, s32* na, s32* nb) {
        *nb = 0;
        *na = 0;
    }
    virtual s32 vf34(RPacket* p) { return 0; }
};

// A track obstacle ("obst"): a sleeping rigid body that wakes when hit, optionally linked in a chain.
class Obstacle : public ObstacleBody {
public:
    Obstacle(void);
    virtual void Init(BodyDesc* d);
    virtual ~Obstacle(void);
    virtual void Reset(void);
    virtual void Update(void);
    virtual void vf20(void);
    virtual int vf44(void) { return asleep; }
    virtual void OnHit(const RVec3* v, const RVec3* at, int force, void* extra);
    Pose12 restPose;    // offset 0x1EC
    f32 sleepTime;      // offset 0x21C
    u8 asleep;          // offset 0x220
    u8 u221;            // offset 0x221
    u8 u222;            // offset 0x222
    u8 chainArmed;      // offset 0x223
    Obstacle* chain;    // offset 0x224
    f32 hitOffset[3];   // offset 0x228
    u8 u234;            // offset 0x234
    u8 u235;            // offset 0x235
    f32 u238;           // offset 0x238
};

/* Class name unproven */
// A fence pole ("fencepst"): an obstacle tied to its neighbour by a rope cloth.
class FencePole : public Obstacle {
public:
    FencePole(void);
    virtual void Init(BodyDesc* d);
    virtual ~FencePole(void);
    virtual void Reset(void);
    virtual void Update(void);
    virtual void vf28(u8* buf);
    FencePole* prev;    // offset 0x23C
    u32 u240;           // offset 0x240
    f32 anchor[3];      // offset 0x244
    s32 index;          // offset 0x250
    s32 ropeIndex;      // offset 0x254
};

/* Class name unproven */
// A visitor that hits the obstacles of a chain (an abstract step between VVisit and HitVisit).
class HitVisitBase : public VVisit {};

/* Class name unproven */
// Wakes each obstacle of a chain.
class WakeVisit : public VVisit {
public:
    virtual void Visit(ObstacleSink* o) { fn_801AC240((u8*)o); }
};

}  // namespace

extern "C" {
s32 fn_801C1BA4(int nPart, int nStick, int nAnchor);  // ppsys.cpp: new PSysObj
void fn_801D08C0(u8* p);
void fn_801D0A24(u8* p, int a);
void fn_801C1C7C(u8* p);
void fn_802E30C8(const char* fmt, ...);
f32 PSVECSquareDistance(const u8* a, const u8* b);
void fn_801AC240(u8* p);
void fn_801ABFF8(u8* p, VVisit* f);
void fn_801ACF94(void);
void fn_801D09A8(u8* p, s32 id);
}

void operator delete(void* p);
void* operator new(unsigned long n, const char* tag);

u8* lbl_80435628[64];

// Cars, player cars, AI cars and network cars all carry a car tag.
static inline int IsCar(u8* o) {
    u32 t = U32(o, 0x10);
    return t == 'CAR ' || t == 'PCAR' || t == 'ACAR' || t == 'NCAR';
}
extern f32 lbl_8055E3C8;

ModuleTag lbl_8055E5D8;
UnkStaticFFFF lbl_8055E5E0;
s32 lbl_8055E5E4;
s32 lbl_8055E5E8;
s32 lbl_8055E5EC;
s32 lbl_8055E5F0;

extern "C" {

u8* fn_801792F8(void);
void fn_802E317C(const char* fmt, ...);
u8* fn_801AC26C(void);

// Creates the obstacle object for the current track's obstacle type.
u8* fn_801A9660(void) {
    u8* o;
    switch (S32(fn_801792F8(), 0x50)) {
    case 12:
        return (u8*)new ("fencepst") FencePole;
    case 17:
    case 18:
    case 21:
        return fn_801AC26C();
    case 6:
    case 9:
    case 14:
    case 15:
    case 19:
    case 22:
        fn_802E317C("obstacle: tried to create an Obstacle intended only for the SOL");
        return 0;
    default:
        return (u8*)new ("obst") Obstacle;
    }
}

}  // extern "C"

Obstacle::Obstacle(void) {}

// Destructors are written out of line and also expanded into the derived destructors.
#pragma push
#pragma auto_inline on
ObstacleBody::~ObstacleBody(void) {}
#pragma pop


// A contact corner (corners.cpp), kept in a body's list at 0xD0.
// Keep the scale operand in MWCC's floating-point pool.
static inline f32 ScaleOperand(f32 value) { return value; }
struct BodyCorner {
    BodyCorner* next;  // offset 0x0
    RVec3 pos;         // offset 0x4
    f32 spring;        // offset 0x10
    f32 friction;      // offset 0x14
    f32 damping;       // offset 0x18
    ObVec3 vel;        // offset 0x1C
};

extern "C" BodyCorner* fn_801AE180(void);  // new corner

static inline void AddCorner(u8* body, f32 x, f32 y, f32 z, f32 spring) {
    RVec3 pos;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    BodyCorner* c = fn_801AE180();
    c->pos = pos;
    c->spring = spring;
    c->friction = 0.5f;
    c->vel.x = 0.0f;
    c->vel.y = 0.0f;
    c->vel.z = 0.0f;
    c->damping = 0.0f;
    c->next = *(BodyCorner**)(body + 0xD0);
    *(BodyCorner**)(body + 0xD0) = c;
}

extern "C" {

// Puts a contact corner at each of the eight corners of a box with half extents (x, y, z).
void fn_801A9798(u8* p, f32 x, f32 y, f32 z, f32 spring) {
    AddCorner(p, -x, y, -z, spring);
    AddCorner(p, x, y, -z, spring);
    AddCorner(p, -x, y, z, spring);
    AddCorner(p, x, y, z, spring);
    AddCorner(p, -x, -y, -z, spring);
    AddCorner(p, x, -y, -z, spring);
    AddCorner(p, -x, -y, z, spring);
    AddCorner(p, x, -y, z, spring);
}

}  // extern "C"



// A body rotation (3x3) and position, copied separately from a placement record.
struct ObRot {
    f32 m[9];
};
struct ObPos {
    f32 v[3];
};

// An obstacle placement record (the ".obs" entry the factory is given).
struct ObDesc {
    u8 pad0[0x8];
    f32 mass;          // offset 0x8
    u8 padC[0xC];
    ObRot rot;         // offset 0x18
    ObPos pos;         // offset 0x3C
    s32 kind;          // offset 0x48, surface kind of the collision shape
    f32 skin;          // offset 0x4C
    s32 type;          // offset 0x50
    u32 flags;         // offset 0x54
    f32 a, b, c;       // offset 0x58 shape sizes (meaning varies by type)
    f32 d, e, f, g;    // offset 0x64
    u8 pad74[0xC];
    ValueNode* extra;  // offset 0x80
};

// A bounding plane of a polygon volume: normal and offset.
struct ObPlane {
    f32 x, y, z, d;
};

// An obstacle mesh (fn_802C6B44) and its joint parts ('MRJT': corner points every 0x50 bytes from +0x50).
struct ObMeshPart {
    u32 tag;           // offset 0x0
    u8 pad4[0x4];
    s32 count;         // offset 0x8
};
struct ObMesh {
    u8 pad0[0x10];
    s32 count;         // offset 0x10
    ObMeshPart** parts;  // offset 0x14
};

extern f32 __float_epsilon[];      // FLT_EPSILON

extern "C" {
void fn_8019EECC(u8* owner, ObVec3* c);
// sphcoll constructors, called by their linker names: SphereGroup(frame, owner), PolyShape(frame, planes, n, lo, hi, k, skin).
CollShape* __ct__11SphereGroupFUlUl(void* p, u8* frame, u8* owner);
void fn_801A0258(CollShape* grp, const RVec3& c, const RVec3& scale, f32 r, f32 k, f32 skin, f32 m, f32 im);
void fn_801A0444(CollShape* grp, CollShape* box);
// OrientedBoxShape::OrientedBoxShape(frame, centre, owner, k, skin, hx, hy, hz) in sphcoll, called by its linker name.
CollShape* __ct__16OrientedBoxShapeFUlPC5RVec3Ulfffff(void* p, u8* frame, const RVec3& c, u8* owner, f32 k, f32 skin, f32 hx, f32 hy, f32 hz);
CollShape* __ct__9PolyShapeFUlPC9PolyPlanelPC5RVec3PC5RVec3ff(void* p, u8* frame, const ObPlane* planes, s32 n, const ValVec3* lo, const ValVec3* hi, f32 k, f32 skin);
// MeshShape::MeshShape(frame, name, owner) in sphcoll, called by its linker name.
CollShape* __ct__9MeshShapeFUlPCcUl(void* p, u8* frame, const char* name, u8* owner);
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* t);
ObMesh* fn_802C6B44(const char* name);
void fn_802C6C20(ObMesh* m);
void fn_801DA758(u8* p, const char* id, const char* path);
double cos(double x);
double sin(double x);
double sqrt(double x);
}

static inline void AddCornerV(u8* body, const RVec3& v, f32 spring) {
    BodyCorner* c = fn_801AE180();
    c->pos = v;
    c->spring = spring;
    c->friction = 0.5f;
    c->vel.x = 0.0f;
    c->vel.y = 0.0f;
    c->vel.z = 0.0f;
    c->damping = 0.0f;
    c->next = *(BodyCorner**)(body + 0xD0);
    *(BodyCorner**)(body + 0xD0) = c;
}

static inline void AddCornerV(u8* body, const ObVec3& v, f32 spring) {
    BodyCorner* c = fn_801AE180();
    c->pos = (const RVec3&)v;
    c->spring = spring;
    c->friction = 0.5f;
    c->vel.x = 0.0f;
    c->vel.y = 0.0f;
    c->vel.z = 0.0f;
    c->damping = 0.0f;
    c->next = *(BodyCorner**)(body + 0xD0);
    *(BodyCorner**)(body + 0xD0) = c;
}

static inline ObVec3 MakeVec(f32 x, f32 y, f32 z) {
    ObVec3 v;
    v.z = z;
    v.x = x;
    v.y = y;
    return v;
}

struct RMtx;

// A collision shape on a volume's frame (same layout as in mutphob).
class CollShape {
public:
    CollShape(CollVolume* o, const RMtx* m, u32 t, f32 k, f32 s) {
        mtx = m;
        owner = o;
        a8 = 0;
        type = t;
        stiff = k;
        skin = s;
        a20 = 100;
        a1C = 0;
    }
    virtual ~CollShape(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void vf1C(void);
    virtual void vf20(void);
    virtual void vf24(void);
    virtual void vf28(void);
    virtual void vf2C(void);
    virtual void vf30(void);
    virtual void vf34(void);
    virtual void vf38(void) = 0;
    virtual f32 Radius(void);    // 0x3C
    virtual void SetSurface(int id);  // 0x40 (sphcoll SetSurface__9CollShapeFi)
    CollVolume* owner;  // offset 0x4
    s32 a8;             // offset 0x8
    const RMtx* mtx;    // offset 0xC
    f32 stiff;          // offset 0x10
    f32 skin;           // offset 0x14
    u32 type;           // offset 0x18
    s32 a1C;            // offset 0x1C
    s32 a20;            // offset 0x20
};

// A sphere ('SPHR', 0x54 bytes).
class SphereShape : public CollShape {
public:
    SphereShape(CollVolume* o, const RMtx* m, f32 k, f32 s, f32 r, const RVec3& c)
        : CollShape(o, m, 'SPHR', k, s), center(c) {
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
        if (o) {
            fn_8019EECC((u8*)o, (ObVec3*)&center);
        }
    }
    virtual ~SphereShape(void);
    virtual void vf38(void);
    f32 radius;     // offset 0x24
    RVec3 center;   // offset 0x28
    f32 f34;        // offset 0x34
    f32 f38;        // offset 0x38
    f32 f3C;        // offset 0x3C
    f32 f40;        // offset 0x40
    f32 f44;        // offset 0x44
    f32 f48;        // offset 0x48
    u8 b4C;         // offset 0x4C
    f32 f50;        // offset 0x50
};

static inline f32 ObAbsDiff(f32 a, f32 b) { return (f32)__fabs(a - b); }

// The frame (pose matrix) a body's collision shapes hang off.
static inline const RMtx* ObFrameOf(u8* p) { return (const RMtx*)(p + 0x3C); }

static inline f32 ObMax(f32 a, f32 b) { return a > b ? a : b; }

// A flattened sphere ('FLSR', 0x5C bytes): radius a across, b along its axis; bounded by the larger.
class FlatSphereShape : public SphereShape {
public:
    FlatSphereShape(CollVolume* o, const RMtx* m, f32 k, f32 s, f32 a, f32 b, const RVec3& c)
        : SphereShape(o, m, k, s, ObMax(a, b), c) {
        across = a;
        along = b;
        type = 'FLSR';
    }
    virtual ~FlatSphereShape(void);
    virtual void vf38(void);
    f32 across;  // offset 0x54
    f32 along;   // offset 0x58
};

// A box ('BOX ', 0x38 bytes) with half extents (hx, hy, hz) and a bounding radius.
class BoxShape : public CollShape {
public:
    BoxShape(CollVolume* o, const RMtx* m, f32 k, f32 s, f32 x, f32 y, f32 z) : CollShape(o, m, 'BOX ', k, s) {
        hx = x;
        hy = y;
        hz = z;
        a34 = 0;
        bound = 0.2f + (f32)sqrt(z * z + (x * x + y * y));
    }
    virtual ~BoxShape(void);
    virtual void vf38(void);
    f32 hx, hy, hz;  // offset 0x24
    f32 bound;       // offset 0x30
    s32 a34;         // offset 0x34
};

class TubeBase : public CollShape {
public:
    TubeBase(CollVolume* o, const RMtx* m, f32 k, f32 s, f32 r, f32 h) : CollShape(o, m, 'TUBE', k, s) {
        rad = r;
        half = h;
    }
    virtual ~TubeBase(void);
    f32 rad;   // offset 0x24
    f32 half;  // offset 0x28: half the length between the end spheres
};

// A tube ('TUBE', 0x2C bytes).
class TubeShape : public TubeBase {
public:
    TubeShape(CollVolume* o, const RMtx* m, f32 k, f32 s, f32 r, f32 h) : TubeBase(o, m, k, s, r, h) {}
    virtual ~TubeShape(void);
    virtual void vf38(void);
};

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

// out = m * v (m: 3x3 rows at 0, 3, 6), safe when out aliases v.
static inline void ObRotate(ObVec3* out, const f32* m, const ObVec3* v) {
    if (out == v) {
        f32 x = Dot3(v->x, m[0], v->y, m[3], v->z, m[6]);
        f32 y = Dot3(v->x, m[1], v->y, m[4], v->z, m[7]);
        f32 z = Dot3(v->x, m[2], v->y, m[5], v->z, m[8]);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(v->x, m[0], v->y, m[3], v->z, m[6]);
        out->y = Dot3(v->x, m[1], v->y, m[4], v->z, m[7]);
        out->z = Dot3(v->x, m[2], v->y, m[5], v->z, m[8]);
    }
}

// Builds an obstacle's body from its placement record: rigid body, pose, mass-derived spring, then the contact
// corners and collision shape for its type (ball, boxes, post, wall, horse, cone, barrel, wedge, poly volume,
// barrier, tube, mesh corners), the shared surface kind, corner friction, the linked obstacle chain and its path.
void Obstacle::Init(BodyDesc* d) {
    f32 cs;
    RVec3 origin;
    CollShape* shape;
    int corners;
    const char* path;
    const char* id;
    f32 k;
    f32 spring;
    f32 skin;
    ObDesc* const q = (ObDesc*)d;
    RigidBody::Init(d);
    U8(this, 0x221) = 0;
    U8(this, 0x220) = 1;
    U8(this, 0x222) = 0;
    F32(this, 0x21C) = 0.0f;
    U8(this, 0x223) = 1;
    S32(this, 0x224) = 0;
    F32(this, 0x228) = 0.0f;
    F32(this, 0x22C) = 0.0f;
    F32(this, 0x230) = 0.0f;
    U8(this, 0x235) = 0;
    U8(this, 0x234) = 0;
    F32(this, 0x238) = 0.0f;
    fn_801AC240((u8*)this);
    *(ObRot*)((u8*)frame) = q->rot;
    *(ObPos*)((u8*)this + 0x60) = q->pos;
    CollVolume::Update();
    *(RVec3*)((u8*)this + 0x30) = RVec3(0.0f, 0.0f, 0.0f);
    CollVolume::vf20();
    shape = 0;
    corners = 1;
    k = 1.5f * (9.81f * q->mass / 0.03f);
    skin = q->skin;
    origin.x = 0.0f;
    origin.y = 0.0f;
    origin.z = 0.0f;
    spring = 0.4f * k;
    s32 kind = q->kind;
    switch (q->type) {
    case 0:
    case 17:
        shape = new ("ball:ball-volume") SphereShape(this, ObFrameOf((u8*)this), k, skin, q->a, origin);
        corners = 1;
        break;
    case 1:
    case 2: {
        f32 x = q->a;
        f32 y = q->b;
        f32 z = q->c;
        fn_801A9798((u8*)this, x * ScaleOperand(0.5f), y * ScaleOperand(0.5f), z * ScaleOperand(0.5f), spring);
        if (q->type == 1) {
            shape = new ("box1:box-volume") BoxShape(this, ObFrameOf((u8*)this), k, skin, x * ScaleOperand(0.5f), y * ScaleOperand(0.5f), z * ScaleOperand(0.5f));
            corners = 0;
        } else {
            f32 r = (z + (y + x)) / 3.0f / 2.0f;
            shape = new ("box2:ball-volume") SphereShape(this, ObFrameOf((u8*)this), k, skin, r, origin);
            corners = 0;
        }
        break;
    }
    case 7:
        shape = new ("obstacle-post:tube") TubeShape(this, ObFrameOf((u8*)this), 437992.1f, 0.8f, q->a, q->b);
        U8(this, 0x221) = 1;
        corners = 0;
        break;
    case 12: {
        CollShape* s = (CollShape*)operator new(0x74, "obstacle-wall:box");
        if (s) {
            const f32& half = 0.5f;
            s = __ct__16OrientedBoxShapeFUlPC5RVec3Ulfffff(s, (u8*)frame, RVec3(0.0f, half * q->b, 0.0f), (u8*)this, 50000.0f,
                            0.2f, q->a, half * q->b, q->a);
        }
        shape = s;
        kind = 110;
        corners = 0;
        break;
    }
    case 8: {
        RVec3 c;
        c = RVec3(q->b * ScaleOperand(0.5f), -q->a * ScaleOperand(0.5f), q->c * ScaleOperand(0.5f));
        AddCornerV((u8*)this, c, spring);
        c.z *= (-1.0f);
        AddCornerV((u8*)this, c, spring);
        c.x *= (-1.0f);
        AddCornerV((u8*)this, c, spring);
        c.z *= (-1.0f);
        AddCornerV((u8*)this, c, spring);
        c = RVec3(0.0762f, q->a * ScaleOperand(0.5f), q->c * ScaleOperand(0.5f));
        AddCornerV((u8*)this, c, spring);
        c.z *= (-1.0f);
        AddCornerV((u8*)this, c, spring);
        c.x *= (-1.0f);
        AddCornerV((u8*)this, c, spring);
        c.z *= (-1.0f);
        AddCornerV((u8*)this, c, spring);
        shape = (CollShape*)operator new(0x184, "horse-volume");
        if (shape) {
            shape = __ct__11SphereGroupFUlUl(shape, (u8*)frame, (u8*)this);
        }
        const f32 directionComponent = 1.0f;
        f32 r = q->a * ScaleOperand(0.5f);
        const f32 firstCenterXY = 0.0f;
        fn_801A0258(shape, RVec3(firstCenterXY, firstCenterXY, -(q->c * ScaleOperand(0.5f) - r)),
                    RVec3(directionComponent, directionComponent, directionComponent), r, k, skin, directionComponent, firstCenterXY);
        const f32 secondCenterXY = 0.0f;
        fn_801A0258(shape, RVec3(secondCenterXY, secondCenterXY, q->c * ScaleOperand(0.5f) - r),
                    RVec3(1.0f, 1.0f, 1.0f), r, k, skin, 1.0f, secondCenterXY);
        CollShape* box = (CollShape*)operator new(0x74, "horse-box");
        if (box) {
            box = __ct__16OrientedBoxShapeFUlPC5RVec3Ulfffff(box, (u8*)frame, origin, (u8*)this, k, skin, 0.3f, 0.3f, 0.3f);
        }
        fn_801A0444(shape, box);
        corners = 0;
        break;
    }
    case 4: {
        f32 w = q->b;
        f32 h = q->c;
        f32 r = q->a * ScaleOperand(0.5f);
        ObVec3 start;
        start.x = q->a;
        start.z = q->a;
        start.y = -h * ScaleOperand(0.5f);
        ObVec3 v = start;
        AddCornerV((u8*)this, v, spring);
        f32 m[9];
        cs = cos(1.5707963705062866);
        f32 sn = sin(1.5707963705062866);
        m[0] = cs;
        m[1] = 0.0f;
        m[2] = -sn;
        m[3] = 0.0f;
        m[4] = 1.0f;
        m[5] = 0.0f;
        m[6] = sn;
        m[7] = 0.0f;
        m[8] = cs;
        ObRotate(&v, &m[0], &v);
        AddCornerV((u8*)this, v, spring);
        ObRotate(&v, &m[0], &v);
        AddCornerV((u8*)this, v, spring);
        ObRotate(&v, &m[0], &v);
        AddCornerV((u8*)this, v, spring);
        AddCornerV((u8*)this, RVec3(-w, h * ScaleOperand(0.5f), -w), spring);
        AddCornerV((u8*)this, RVec3(w, h * ScaleOperand(0.5f), -w), spring);
        AddCornerV((u8*)this, RVec3(-w, h * ScaleOperand(0.5f), w), spring);
        AddCornerV((u8*)this, RVec3(w, h * ScaleOperand(0.5f), w), spring);
        shape = new ("cone:ball-volume") SphereShape(this, ObFrameOf((u8*)this), k, skin, r, origin);
        corners = 0;
        break;
    }
    case 5:
        fn_801A9798((u8*)this, q->a, q->b * ScaleOperand(0.5f), q->a, spring);
        shape = new ("barrel:flat-volume") FlatSphereShape(this, ObFrameOf((u8*)this), k, skin, q->a, q->b * ScaleOperand(0.5f), origin);
        corners = 0;
        break;
    case 3: {
        f32 x = q->a;
        f32 y = q->b;
        f32 z = q->c;
        f32 r = y * ScaleOperand(0.5f);
        AddCornerV((u8*)this, RVec3(-x * ScaleOperand(0.5f), -y * ScaleOperand(0.5f), -z * ScaleOperand(0.5f)), spring);
        AddCornerV((u8*)this, RVec3(x * ScaleOperand(0.5f), -y * ScaleOperand(0.5f), -z * ScaleOperand(0.5f)), spring);
        AddCornerV((u8*)this, RVec3(-x * ScaleOperand(0.5f), -y * ScaleOperand(0.5f), z * ScaleOperand(0.5f)), spring);
        AddCornerV((u8*)this, RVec3(x * ScaleOperand(0.5f), -y * ScaleOperand(0.5f), z * ScaleOperand(0.5f)), spring);
        AddCornerV((u8*)this, RVec3(0.0f, y * ScaleOperand(0.5f), -z * ScaleOperand(0.5f)), spring);
        AddCornerV((u8*)this, RVec3(0.0f, y * ScaleOperand(0.5f), z * ScaleOperand(0.5f)), spring);
        shape = new ("wedge:ball-volume") SphereShape(this, ObFrameOf((u8*)this), k, skin, r, origin);
        corners = 0;
        break;
    }
    case 10: {
        ObPlane* out;
        ValueTable* t;
        int n;
        int i;
        ValueNode* e;
        ValueNode* ext;
        ObPlane pl[8];
        t = fn_80131808((const char*)&q->a);
        n = 0;
        ext = t->FindChild("extents");
        ValVec3 lo = ext->GetVec3("min");
        ValVec3 hi = ext->GetVec3("max");
        out = pl;
        for (i = 0; (e = t->GetChild(i)) != 0; i++) {
            if (strcmp(e->GetName(), "plane") == 0) {
                ValVec3 nv = e->GetVec3("n");
                f32 d = e->GetFloat("k");
                out->x = nv.x;
                out->y = nv.y;
                out->z = nv.z;
                out->d = d;
                n++;
                out++;
            }
        }
        fn_80131818(t);
        CollShape* s = (CollShape*)operator new(0xE0, "polyvol");
        if (s) {
            s = __ct__9PolyShapeFUlPC9PolyPlanelPC5RVec3PC5RVec3ff(s, (u8*)frame, pl, n, &lo, &hi, k, skin);
        }
        shape = s;
        U8(this, 0x221) = 1;
        corners = 0;
        break;
    }
    case 20: {
        CollShape* s = (CollShape*)operator new(0x40, "pmshvol");
        if (s) {
            s = __ct__9MeshShapeFUlPCcUl(s, (u8*)frame, (const char*)&q->a, (u8*)this);
        }
        shape = s;
        U8(this, 0x221) = 1;
        corners = 0;
        break;
    }
    case 11: {
        f32 hx = (q->b - q->a) / 2.0f;
        f32 hy = (q->d - q->c) / 2.0f;
        RVec3 c(origin.x, 0.305f + q->c, q->f - 0.305f);
        CollShape* ball = new ("barrier-sphere") SphereShape(this, ObFrameOf((u8*)this), k, skin, 0.305f, c);
        AddShape(ball);
        CollShape* s = (CollShape*)operator new(0x74, "barrier-wall");
        if (s) {
            s = __ct__16OrientedBoxShapeFUlPC5RVec3Ulfffff(s, (u8*)frame, RVec3(hx - q->a, hy - q->c, q->f - 0.305f), (u8*)this, k, skin, hx, hy,
                            0.61f);
        }
        shape = s;
        AddCornerV((u8*)this, RVec3(q->a, q->c - 0.0254f, q->e), spring);
        AddCornerV((u8*)this, RVec3(q->b, q->c - 0.0254f, q->e), spring);
        AddCornerV((u8*)this, RVec3(q->a, q->c - 0.0254f, q->f), spring);
        AddCornerV((u8*)this, RVec3(q->b, q->c - 0.0254f, q->f), spring);
        AddCornerV((u8*)this, RVec3(q->a, q->d - 0.0254f, q->f + q->g), spring);
        AddCornerV((u8*)this, RVec3(q->b, q->d - 0.0254f, q->f + q->g), spring);
        corners = 0;
        break;
    }
    case 13:
    case 18:
        shape = new ("obs-tubevolume") TubeShape(this, ObFrameOf((u8*)this), 0.9f * k, skin, q->a, q->b * ScaleOperand(0.5f) - q->a);
        F32(this, 0x238) = 0.016683351f * q->c / q->a;
        AddCornerV((u8*)this, RVec3(q->a, q->a, 0.0f), spring * ScaleOperand(0.5f));
        corners = 1;
        break;
    case 16: {
        f32 x = q->a;
        f32 y = q->b;
        f32 z = q->c;
        fn_801A9798((u8*)this, x * ScaleOperand(0.5f), y * ScaleOperand(0.5f), z * ScaleOperand(0.5f), spring);
        shape = new ("box1:box-volume") BoxShape(this, ObFrameOf((u8*)this), k, skin, x * ScaleOperand(0.5f), y * ScaleOperand(0.5f), z * ScaleOperand(0.5f));
        F32(this, 0x228) = q->d;
        F32(this, 0x22C) = q->e;
        F32(this, 0x230) = q->f;
        U8(this, 0x235) = 1;
        U8(this, 0x234) = 1;
        corners = 0;
        break;
    }
    case 21: {
        ObMeshPart* part;
        u8* pt;
        ObMesh* m;
        int i;
        int j;
        corners = 0;
        m = fn_802C6B44((const char*)&q->f);
        for (i = 0; i < m->count; i++) {
            part = m->parts[i];
            if (part->tag == 'MRJT') {
                pt = (u8*)part;
                for (j = 0; j < part->count; j++) {
                    AddCornerV((u8*)this, *(ObVec3*)(pt + 0x50), spring);
                    pt += 0x50;
                }
            }
        }
        fn_802C6C20(m);
        if (ObAbsDiff(q->b, q->a) <= __float_epsilon[0]) {
            shape = new ("obstacle:cornerball") SphereShape(this, ObFrameOf((u8*)this), k, skin, q->a, *(const RVec3*)&q->c);
        } else {
            shape = new ("obstacle:cornerball") FlatSphereShape(this, ObFrameOf((u8*)this), k, skin, q->a, q->b, *(const RVec3*)&q->c);
        }
        break;
    }
    case 23:
        corners = 0;
        break;
    }
    if (q->flags & 4) {
        corners = 1;
    }
    if (q->flags & 8) {
        corners = 0;
    }
    if (shape) {
        shape->SetSurface(kind);
        AddShape(shape);
        U8(this, 0x1E8) = corners;
    }
    for (BodyCorner* c = *(BodyCorner**)((u8*)this + 0xD0); c; c = c->next) {
        c->friction = 2.0f;
    }
    *(Pose12*)((u8*)this + 0x1EC) = *(Pose12*)((u8*)frame);
    S32(this, 0x224) = 0;
    if (q->flags & 1) {
        u32 group = q->flags & 2;
        if (lbl_8055E5E4 && group == (u32)lbl_8055E5E8) {
            S32(this, 0x224) = lbl_8055E5E4;
            u8* o = (u8*)lbl_8055E5E4;
            while (PTR(o, 0x224) != (u8*)lbl_8055E5E4) {
                o = PTR(o, 0x224);
            }
            PTR(o, 0x224) = ((u8*)this);
        } else {
            PTR(this, 0x224) = ((u8*)this);
        }
        lbl_8055E5E8 = group;
        lbl_8055E5E4 = (s32)((u8*)this);
    } else {
        lbl_8055E5E4 = 0;
    }
    if (q->extra && q->extra->Has("path")) {
        int i;
        path = q->extra->GetValue("path");
        id = q->extra->GetString("path_id", q->extra->GetString("dyno", ""));
        fn_801DA758((u8*)this, id, path);
        U8(this, 0x222) = 1;
        for (i = 0; i < count; i++) {
            Shape(i)->SetSurface(0x72);
        }
    }
}



static inline double AbsD(double v) { return __fabs(v); }

extern "C" {

void fn_801ABB54(u8* p) {
    f32 d = PSVECDotProduct((f32*)(p + 0x124), (f32*)(p + 0x54));
    f32 imp = 0.0f;
    f32 ad = (f32)AbsD(d);
    f32 lim = F32(p, 0x238);
    if (ad > lim) {
        lim = lim < ad ? lim : ad;
        f32 sign = d > 0.0f ? 1.0f : (-1.0f);
        imp = -sign * lim;
    } else if (ad > 0.01f) {
        imp = -d * ScaleOperand(0.5f);
    }
    F32(p, 0x124) = F32(p, 0x124) + F32(p, 0x54) * imp;
    F32(p, 0x128) = F32(p, 0x128) + F32(p, 0x58) * imp;
    F32(p, 0x12C) = F32(p, 0x12C) + F32(p, 0x5C) * imp;
}

}  // extern "C"

// Message sent when a solid obstacle wants its hit point reported.
struct ObHitMsg {
    ObVec3 pos;  // offset 0x0
    u8* obj;     // offset 0xC
    u8 flag;     // offset 0x10
};

extern "C" {
void fn_801AC254(u8* p);
void fn_801796CC(int kind, void* msg, int size);
}  // extern "C"

// One frame of a sleeping/settled obstacle (rest pose at 0x1EC, rest timer at 0x21C).
void Obstacle::Update(void) {
    u8* p = (u8*)this;
    if (U8(p, 0x222)) {
        ObVec3 lin;
        ObVec3 ang;
        U8(p, 0x1E9) = 0;
        ((RigidBody*)p)->Flush((RVec3*)&lin, (RVec3*)&ang);
        return;
    }
    if (PSVECDotProduct((f32*)(p + 0x118), (f32*)(p + 0x118)) > 0.1f * 0.1f ||
        PSVECDotProduct((f32*)(p + 0x124), (f32*)(p + 0x124)) > 0.1f * 0.1f) {
        if (F32(p, 0x64) > -500.0f) {
            fn_801AC240(p);
        }
    }
    if (U8(p, 0x220)) {
        if (lbl_8055E3C4 == 1 || lbl_8055E3C8 > 1.0f + F32(p, 0x21C)) {
            fn_801AC254(p);
            F32(p, 0x124) = 0.0f;
            F32(p, 0x128) = 0.0f;
            F32(p, 0x12C) = 0.0f;
            F32(p, 0x118) = 0.0f;
            F32(p, 0x11C) = 0.0f;
            F32(p, 0x120) = 0.0f;
        } else if (!U8(p, 0x221)) {
            ((RigidBody*)p)->RigidBody::Update();
            F32(p, 0x118) *= 0.99983317f;
            F32(p, 0x11C) *= 0.99983317f;
            F32(p, 0x120) *= 0.99983317f;
            F32(p, 0x124) *= 0.99983317f;
            F32(p, 0x128) *= 0.99983317f;
            F32(p, 0x12C) *= 0.99983317f;
            if (F32(p, 0x238) > 0.0f) {
                fn_801ABB54(p);
            }
            int up = 0;
            if (U8(p, 0x234) && F32(p, 0x4C) > 0.5f) {
                up = 1;
            }
            U8(p, 0x234) = up;
        } else {
            ((RigidBody*)p)->RigidBody::Update();
            F32(p, 0x124) = 0.0f;
            F32(p, 0x128) = 0.0f;
            F32(p, 0x12C) = 0.0f;
            F32(p, 0x118) = 0.0f;
            F32(p, 0x11C) = 0.0f;
            F32(p, 0x120) = 0.0f;
            *(Pose12*)(p + 0x3C) = *(Pose12*)(p + 0x1EC);
            ((CollVolume*)p)->CollVolume::Update();
        }
        if (F32(p, 0x64) < -500.0f) {
            ((Obstacle*)p)->Reset();
        }
    }
    if (U8(p, 0x235)) {
        ObHitMsg m;
        ObRotate(&m.pos, (f32*)(p + 0x70), (ObVec3*)(p + 0x228));
        m.pos.x += F32(p, 0x94);
        m.pos.y += F32(p, 0x98);
        m.pos.z += F32(p, 0x9C);
        m.obj = p;
        m.flag = U8(p, 0x234);
        fn_801796CC(9, &m, sizeof(ObHitMsg));
    }
    S32(p, 0x1E4) = 0;
}

void Obstacle::vf20(void) { CollVolume::vf20(); }

extern "C" {

void fn_801ABFF8(u8* p, VVisit* f) {
    u8* o = PTR(p, 0x224);
    if (o && U8(p, 0x223) && lbl_8055E3C4 > 1) {
        while (o != p) {
            U8(o, 0x223) = 0;
            f->Visit((ObstacleSink*)o);
            o = PTR(o, 0x224);
        }
        U8(p, 0x223) = 0;
    }
    fn_801AC240(p);
}

}  // extern "C"

// Visitor handed to the linked obstacles when this one is hit hard.
namespace {

struct HitVisit : HitVisitBase {
    HitVisit(const f32* velocity, const f32* at_, int force_, u8* extra_) {
        v[0] = velocity[0];
        v[1] = velocity[1];
        v[2] = velocity[2];
        at[0] = at_[0];
        at[1] = at_[1];
        at[2] = at_[2];
        force = force_;
        extra = extra_;
    }
    virtual void Visit(ObstacleSink* o);
    f32 v[3];
    f32 at[3];
    s32 force;
    u8* extra;
};

}  // namespace

extern "C" void fn_801D0934(u8* p);

namespace {

// A dynamic obstacle ("DynamicObstacle"): an obstacle whose state is shared through a DynamicLink.
// Every member is defined in the class, so the compiler emits them (and WakeVisit's table) at the end of the unit.
class DynamicObstacle : public Obstacle, public DynamicLink {
public:
    virtual ~DynamicObstacle(void) {
        fn_801D0934((u8*)(DynamicLink*)this);
        owner = 0;
    }
    virtual void Init(BodyDesc* d) {
        Obstacle::Init(d);
        owner = (u32)((ObDesc*)d)->extra;
        fn_801D08C0((u8*)(DynamicLink*)this);
    }
    virtual void OnImpulse(const RVec3* v, const RVec3* at, int force, void* extra, void* by) {
        if (force < 8) {
            fn_801D0A24((u8*)(DynamicLink*)this, (int)at);
        }
    }
    virtual void OnHit(const RVec3* v, const RVec3* at, int force, void* extra) {
        u8* car = (u8*)extra;
        Obstacle::OnHit(v, at, force, extra);
        if (car && IsCar(car)) {
            s32 id = S32(car, 0x1EC);
            fn_801D09A8((u8*)(DynamicLink*)this, id);
        }
    }
    // Declared in this order (the table's appended entries follow it); the slot order is DynamicLink's.
    virtual u32 Owner(void) { return owner; }
    virtual void Describe(void) { ((VO*)owner)->v18("dyno", "unknown"); }
    virtual u8 IsAsleep(void) { return asleep; }
    virtual u8* Position(void) { return (u8*)this + 0x60; }
    virtual u8* Velocity(void) { return (u8*)this + 0x118; }
    virtual s32 PartId(void) { return *(s16*)((u8*)this + 0x6C); }
    virtual void SetAwake(int on) {
        if (on) {
            WakeVisit wake;
            fn_801ABFF8((u8*)this, &wake);
        } else {
            asleep = 0;
        }
    }
    virtual void Push(f32* v, f32* off) {
        f32 push[3];
        f32 at[3];
        f32 s = F32(this, 0xD4) / (1.0f / 59.94f);
        push[0] = v[0] * s;
        push[1] = v[1] * s;
        push[2] = v[2] * s;
        at[0] = F32(this, 0x60) + off[0];
        at[1] = F32(this, 0x64) + off[1];
        at[2] = F32(this, 0x68) + off[2];
        OnHit((const RVec3*)push, (const RVec3*)at, 100, 0);
    }
    virtual void SetState(const Pose12* pose, const ObVec3* lo, const ObVec3* hi) {
        *(Pose12*)((u8*)this + 0x3C) = *pose;
        CollVolume::Update();
        F32(this, 0x118) = lo->x;
        F32(this, 0x11C) = lo->y;
        F32(this, 0x120) = lo->z;
        F32(this, 0x124) = hi->x;
        F32(this, 0x128) = hi->y;
        F32(this, 0x12C) = hi->z;
        fn_801AC240((u8*)this);
    }
    virtual void Rest(void) {
        Reset();
        fn_801AC254((u8*)this);
    }
};

}  // namespace


void Obstacle::OnHit(const RVec3* v_, const RVec3* at_, int force, void* extra_) {
    if (force >= 100 && !U8(this, 0x220)) {
        f32 r = F32(this, 0xD4);
        f32 k = 1.7777778f * r;
        f32 k2 = k * k;
        if (PSVECDotProduct(((const f32*)v_), ((const f32*)v_)) > k2) {
            HitVisit h(((const f32*)v_), ((const f32*)at_), force, ((u8*)extra_));
            fn_801ABFF8((u8*)this, &h);
        }
    }
    RigidBody::OnHit((const RVec3*)((const f32*)v_), (const RVec3*)((const f32*)at_), force, 0);
}

void Obstacle::Reset(void) {
    u8* p = (u8*)this;
    fn_801AC240(p);
    *(Pose12*)(p + 0x3C) = *(Pose12*)(p + 0x1EC);
    ((CollVolume*)p)->CollVolume::Update();
    ((RigidBody*)p)->RigidBody::Reset();
    U8(p, 0x223) = 1;
    U8(p, 0x234) = U8(p, 0x235);
}

extern "C" {

void fn_801AC240(u8* p) {
    U8(p, 0x220) = 1;
    F32(p, 0x21C) = lbl_8055E3C8;
}

void fn_801AC254(u8* p) {
    if (U8(p, 0x220)) {
        U8(p, 0x220) = 0;
    }
}

u8* fn_801AC26C(void) { return (u8*)new ("DynamicObstacle") DynamicObstacle; }

}  // extern "C"

#pragma push
#pragma auto_inline on
Obstacle::~Obstacle(void) { lbl_8055E5E4 = 0; }
#pragma pop

FencePole::FencePole(void) {
    u8* p = (u8*)this;
    if (!lbl_8055E5EC) {
        lbl_8055E5F0 = 0;
    }
    S32(p, 0x250) = lbl_8055E5EC++;
    lbl_80435628[S32(p, 0x250)] = p;
    S32(p, 0x254) = -1;
}

FencePole::~FencePole(void) {
    if (--lbl_8055E5EC == 0 && lbl_8055E5F0) {
        fn_801C1C7C((u8*)lbl_8055E5F0);
        lbl_8055E5F0 = 0;
    }
}

void FencePole::Init(BodyDesc* d) {
    U32(this, 0x23C) = 0;
    U32(this, 0x240) = 0;
    ((Obstacle*)((u8*)this))->Obstacle::Init(d);
    s32 idx = S32(this, 0x250);
    F32(this, 0x248) = F32(((u8*)d), 0x5C);
    F32(this, 0x244) = 0.0f;
    F32(this, 0x24C) = 0.0f;
    U32(this, 0x23C) = 0;
    if (idx > 0) {
        int last = idx - 1;
        u8* prev = lbl_80435628[last];
        if (PSVECSquareDistance(prev + 0x60, (u8*)this + 0x60) < 16.0f) {
            PTR(this, 0x23C) = prev;
            fn_802E30C8("fencepole: binding to %p", prev);
        }
    }
}

extern "C" {

void fn_801AC508(void) {
    s32 n = lbl_8055E5EC;
    if (n > 0) {
        s32 t = n * 5;
        lbl_8055E5F0 = fn_801C1BA4(t, t - 1, n);
    }
}

}  // extern "C"

static inline void Mix(ObVec3* o, const ObVec3* a, f32 ka, const ObVec3* b, f32 kb) {
    o->x = a->x * ka + b->x * kb;
    o->y = a->y * ka + b->y * kb;
    o->z = a->z * ka + b->z * kb;
}
static inline void Mix2(ObVec3* o, const ObVec3* a, const ObVec3* b, f32 t) {
    f32 s = 1.0f - t;
    o->x = a->x * s + b->x * t;
    o->y = a->y * s + b->y * t;
    o->z = a->z * s + b->z * t;
}

// Cloth setup records for ppsys.
struct FenceVert {
    ObVec3 pos;  // offset 0x0
    f32 w34;     // offset 0xC
    f32 w30;     // offset 0x10
};

struct FenceEdge {
    s32 a;      // offset 0x0
    s32 b;      // offset 0x4
    f32 extra;  // offset 0x8
    f32 stiff;  // offset 0xC: negative = slack
};

struct FencePin {
    s32 idx;       // offset 0x0
    u8* mtx;       // offset 0x4
    ObVec3* at;    // offset 0x8
};

extern "C" {
f32 PSVECDistance(const void* a, const void* b);
void fn_801A2674(u8* p, f32 r, u32 a, u32 b);
void fn_801C1CBC(s32 sys, int nv, FenceVert* v, int ne, FenceEdge* e, int np, FencePin* pins);

// Once the last fence pole of the course has registered, builds the rope cloth that joins them: one particle
// pinned at each pole top, four free particles strung between neighbouring bound poles (slack sticks), and the
// collision sphere range of each span.
}  // extern "C"

#pragma push
#pragma opt_lifetimes off
void FencePole::Reset(void) {
    u8* p = (u8*)this;
    ((Obstacle*)p)->Obstacle::Reset();
    if (S32(p, 0x250) == lbl_8055E5EC - 1 && lbl_8055E5F0) {
        FenceVert verts[320];
        FenceEdge edges[320];
        FencePin pins[64];
        int nv = 0;
        int ne = 0;
        int np = 0;
        for (int i = 0; i < lbl_8055E5EC; i++) {
            u8* o = lbl_80435628[i];
            ObVec3 a;
            ObRotate(&a, (f32*)(o + 0x1EC), (ObVec3*)(o + 0x244));
            a.x += F32(o, 0x210);
            a.y += F32(o, 0x214);
            a.z += F32(o, 0x218);
            if (PTR(o, 0x23C)) {
                f32 r = 0.5f + 1.1f * PSVECDistance(o + 0x210, PTR(o, 0x23C) + 0x210);
                if (PTR(o, 0x240)) {
                    fn_801A2674(PTR(o, 0x240), r, nv, nv + 5);
                }
                u8* prev = PTR(o, 0x23C);
                ObVec3 b;
                ObVec3* lp = (ObVec3*)(prev + 0x244);
                ObRotate(&b, (f32*)(prev + 0x1EC), lp);
                b.x += F32(prev, 0x210);
                b.y += F32(prev, 0x214);
                b.z += F32(prev, 0x218);
                f32 weightedZ;
                for (int j = 0; j < 5; j++) {
                    if (j > 0) {
                        f32 weightedY;
                        f32 weightedX;
                        f32 weight = (f32)j;
                        const f32 nextWeight = weight / 5.0f;
                        weight = 1.0f - nextWeight;
                        FenceVert* v = &verts[nv++];
                        weightedY = b.y;
                        weightedX = b.x;
                        weightedZ = b.z;
                        weightedY *= weight;
                        weightedX *= weight;
                        weightedZ *= weight;
                        v->pos.x = weightedX + a.x * nextWeight;
                        v->pos.y = weightedY + a.y * nextWeight;
                        v->pos.z = weightedZ + a.z * nextWeight;
                        v->w34 = 1.0f;
                        v->w30 = 1.0f;
                    }
                    FenceEdge* e = &edges[ne++];
                    e->a = nv;
                    e->b = nv - 1;
                    e->extra = 0.0f;
                    e->stiff = -0.5f;
                }
            }
            FencePin* pin = &pins[np++];
            pin->idx = nv;
            pin->mtx = o + 0x3C;
            pin->at = 0;
            S32(o, 0x254) = nv;
            FenceVert* v = &verts[nv++];
            v->pos.x = a.x;
            v->pos.y = a.y;
            v->pos.z = a.z;
            v->w34 = 1.0f;
            v->w30 = 1.0f;
        }
        for (int i = 0; i < ne; i++) {
            PSVECDistance(&verts[edges[i].a], &verts[edges[i].b]);
            edges[i].extra = 0.0f;
        }
        fn_801C1CBC(lbl_8055E5F0, nv, verts, ne, edges, np, pins);
    }
}
#pragma pop

// Helper name unproven: the scale arrives as a parameter, so it stays the right operand of each multiply.
static inline void AddScaled(ObVec3* o, const ObVec3* v, f32 s) {
    o->x += v->x * s;
    o->y += v->y * s;
    o->z += v->z * s;
}
void FencePole::Update(void) {
    u8* p = (u8*)this;
    ((Obstacle*)p)->Obstacle::Update();
    if (U8(p, 0x220)) {
        F32(p, 0x118) = 0.0f;
        F32(p, 0x11C) = 0.0f;
        F32(p, 0x120) = 0.0f;
        F32(p, 0x124) *= 0.9f;
        F32(p, 0x128) *= 0.9f;
        F32(p, 0x12C) *= 0.9f;
        ObVec3 c;
        PSVECCrossProduct((f32*)(p + 0x48), (f32*)(p + 0x1F8), &c.x);
        if (F32(p, 0x4C) < 0.92f) {
            AddScaled((ObVec3*)(p + 0x124), &c, 0.1f);
        }
        F32(p, 0x60) = F32(p, 0x210);
        F32(p, 0x64) = F32(p, 0x214);
        F32(p, 0x68) = F32(p, 0x218);
    }
}

static inline f32 ObLerp( f32 a,f32 b, f32 t) { return a + t * (b - a); }

// A pose as stored for drawing (rotation rows + position).
struct ObPose {
    ObRot rot;   // offset 0x0
    ObVec3 pos;  // offset 0x24
};

extern f32 lbl_8055E460;    // draw interpolation factor (1 = current frame)
extern f32 __float_epsilon[];  // FLT_EPSILON

static inline void BlendFencePose(ObPose* dest, const ObPose* previous, const ObPose* current, f32 f) {
    f32* d = (f32*)dest;
    const f32* prev = (const f32*)previous;
    const f32* cur = (const f32*)current;
    for (int i = 0; i < 12; i++) {
        *d++ = ObLerp(*prev++, *cur++, f);
    }
}

// Copies or blends the frame pose before writing its associated part ID.
static inline void WriteFencePose(u8* buf, u8* p) {
    f32 f = lbl_8055E460;
    u8* out = buf + S32(p, 0x8);
    if ((f32)__fabs(f - 1.0f) <= __float_epsilon[0]) {
        *(ObPose*)out = *(ObPose*)(p + 0x70);
    } else {
        ObPose t;
        BlendFencePose(&t, (const ObPose*)(p + 0xA0), (const ObPose*)(p + 0x70), f);
        *(ObPose*)out = t;
    }
    *(s16*)(out + 0x30) = *(s16*)(p + 0x6C);
}

// Writes this obstacle's draw state into the frame buffer: its pose (blended from the previous one when the
// draw falls between physics steps) and part id; links also work out both anchor points.
void FencePole::vf28(u8* buf) {
    u8* p = (u8*)this;
    WriteFencePose(buf, p);
    if (PTR(p, 0x23C)) {
        ObVec3 a;
        ObRotate(&a, (f32*)(p + 0x3C), (ObVec3*)(p + 0x244));
        a.x += F32(p, 0x60);
        a.y += F32(p, 0x64);
        a.z += F32(p, 0x68);
        const ObVec3* anchor;
        u8* o = PTR(p, 0x23C);
        ObVec3 b;
        anchor = (const ObVec3*)(o + 0x244);
        ObRotate(&b, (const f32*)(o + 0x3C), anchor);
        b.x += F32(o, 0x60);
        b.y += F32(o, 0x64);
        b.z += F32(o, 0x68);
    }
}

extern "C" {

// Fence span update sent to the renderer: the posts between two linked fence posts.
struct FenceMsg {
    s32 kind;         // offset 0x0
    s32 post;         // offset 0x4
    s32 count;        // offset 0x8
    f32 pts[6][3];    // offset 0xC
};

void fn_801C2694(s32 rope, f32 (*out)[3], s32* n, f32* d, f32* c, f32* b, f32* a);
void fn_801796CC(int kind, void* msg, int size);

// Samples the fence rope and sends each linked post pair its span of points.
void fn_801ACF94(void) {
    if (lbl_8055E5F0) {
        f32 out[0x13C][3];
        FenceMsg m;
        f32 d[3];
        f32 c[3];
        f32 b[3];
        f32 a[3];
        s32 n = 0x13C;
        a[0] = 0.0f;
        a[1] = 0.0f;
        a[2] = 0.0f;
        b[0] = 0.0f;
        b[1] = 0.0f;
        b[2] = 0.0f;
        c[0] = 0.0f;
        c[1] = 1.0f;
        c[2] = 0.0f;
        d[0] = 0.0f;
        d[1] = -10000.0f;
        d[2] = 0.0f;
        fn_801C2694(lbl_8055E5F0, out, &n, d, c, b, a);
        for (int i = 0; i < lbl_8055E5EC; i++) {
            u8* next;
            u8* post;
            post = lbl_80435628[i];
            next = PTR(post, 0x23C);
            if (next) {
                int first = S32(next, 0x254);
                int last = S32(post, 0x254);
                m.kind = 0;
                m.post = i;
                m.count = last - first + 1;
                for (int k = 0; k < m.count; k++) {
                    m.pts[k][0] = out[first + k][0];
                    m.pts[k][1] = out[first + k][1];
                    m.pts[k][2] = out[first + k][2];
                }
                fn_801796CC(6, &m, 0x54);
            }
        }
    }
}

void fn_801AD0E0(u8* p, const f32* a, s32 id, const f32* b, f32 x, f32 y) {
    f32 t = 0.02f * x;
    S32(p, 0xC) = 0;
    F32(p, 0x28) = 0.0f;
    F32(p, 0x2C) = 0.0f;
    F32(p, 0x0) = a[0];
    F32(p, 0x4) = a[1];
    F32(p, 0x8) = a[2];
    S32(p, 0xC) = id;
    F32(p, 0x10) = b[0];
    F32(p, 0x14) = b[1];
    F32(p, 0x18) = b[2];
    F32(p, 0x1C) = x;
    F32(p, 0x24) = y;
    F32(p, 0x20) = 0.1f * t;
}

// Links this joint between a point on our volume and the same point in `body`'s local frame.
void fn_801AD148(u8* p, const CVec3* a, u8* body, f32 x, f32 y) {
    CVec3 w;
    CVec3 l;
    ((CollVolume*)PTR(p, 0x34))->Xform(&w, a);
    if (body) {
        l.x = w.x - F32(body, 0x60);
        l.y = w.y - F32(body, 0x64);
        l.z = w.z - F32(body, 0x68);
        f32* m = (f32*)(body + 0x3C);
        f32 lx = Dot3(l.x, m[0], l.y, m[1], l.z, m[2]);
        f32 ly = Dot3(l.x, m[3], l.y, m[4], l.z, m[5]);
        f32 lz = Dot3(l.x, m[6], l.y, m[7], l.z, m[8]);
        l.x = lx;
        l.y = ly;
        l.z = lz;
    } else {
        l = w;
    }
    fn_801AD0E0(p, (const f32*)&l, (s32)body, (const f32*)a, x, y);
}

f32 PSVECMag(const CVec3* v);

// Links this joint between a point on our volume and one on `other` (or a world point).
void fn_801AD28C(u8* p, const CVec3* a, CollVolume* other, const CVec3* b, f32 x, f32 y) {
    CVec3 wa;
    CVec3 wb;
    CVec3 d;
    ((CollVolume*)PTR(p, 0x34))->Xform(&wa, a);
    if (other) {
        other->Xform(&wb, b);
    } else {
        wb = *b;
    }
    d.x = wb.x - wa.x;
    d.y = wb.y - wa.y;
    d.z = wb.z - wa.z;
    F32(p, 0x28) = PSVECMag(&d);
    f32 len = F32(p, 0x28);
    fn_801AD0E0(p, (const f32*)b, (s32)other, (const f32*)a, x, y);
    F32(p, 0x28) = len;
}

double sqrt(double x);

// The joint threshold occupies the final eight bytes of the small-data pool.
static const struct { f32 value; u32 reserved; } MinJointLength = { 0.001f, 0 };

// Applies the joint's spring (beyond the rest length) and damping between the two attachment points.
void fn_801AD394(u8* p) {
    CVec3 wa;
    CVec3 wb;
    CVec3 d;
    ((CollVolume*)PTR(p, 0x34))->Xform(&wa, (const CVec3*)(p + 0x10));
    if (PTR(p, 0xC)) {
        ((CollVolume*)PTR(p, 0xC))->Xform(&wb, (const CVec3*)p);
    } else {
        wb = *(const CVec3*)p;
    }
    d.x = wb.x - wa.x;
    d.y = wb.y - wa.y;
    d.z = wb.z - wa.z;
    f32 len = PSVECMag(&d);
    const f32& dx = d.x;
    f32 dz = d.z;
    f32 hxx = dx * dx;
    f32 hzz = dz * dz;
    F32(p, 0x30) = (f32)sqrt(hxx + hzz);
    f32 stretch = len - F32(p, 0x28);
    F32(p, 0x2C) = stretch;
    if (len > MinJointLength.value && len < F32(p, 0x28) + F32(p, 0x24)) {
        CVec3 dir;
        CVec3 force;
        CVec3 va;
        CVec3 vb;
        CVec3 dv;
        f32 inv = 1.0f / len;
        f32 zero = 0.0f;
        force.x = zero;
        force.y = zero;
        force.z = zero;
        dir.x = d.x * inv;
        dir.y = d.y * inv;
        dir.z = d.z * inv;
        ((RigidBody*)PTR(p, 0x34))->VelAt((RVec3*)&va, (const RVec3*)&wa);
        if (PTR(p, 0xC)) {
            ((RigidBody*)PTR(p, 0xC))->VelAt((RVec3*)&vb, (const RVec3*)&wb);
        } else {
            vb.x = 0.0f;
            vb.y = 0.0f;
            vb.z = 0.0f;
        }
        dv.x = vb.x - va.x;
        dv.y = vb.y - va.y;
        dv.z = vb.z - va.z;
        f32 c = F32(p, 0x20);
        force.x = dv.x * c;
        force.y = dv.y * c;
        force.z = dv.z * c;
        if (len > F32(p, 0x28)) {
            f32 k = F32(p, 0x1C) * (len - F32(p, 0x28));
            force.x = force.x + dir.x * k;
            force.y = force.y + dir.y * k;
            force.z = force.z + dir.z * k;
        }
        ((RigidBody*)PTR(p, 0x34))->AddForceAt((const RVec3*)&force, (const RVec3*)&wa);
        if (PTR(p, 0xC)) {
            force.x = force.x * ScaleOperand(-1.0f);
            force.y = force.y * ScaleOperand(-1.0f);
            force.z = force.z * ScaleOperand(-1.0f);
            ((RigidBody*)PTR(p, 0xC))->AddForceAt((const RVec3*)&force, (const RVec3*)&wa);
        }
    }
}

void fn_801AD5EC(void) { fn_801AC508(); }

void fn_801AD5F0(void) { fn_801ACF94(); }

}  // extern "C"

class ObstacleSink {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
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
    virtual void Hit(const f32* push, const f32* at, u32 force, void* extra);  // 0x48
};

extern "C" {

static inline f32 LerpT(f32 t, f32 a, f32 b) { return a + t * (b - a); }
static inline void Set3(f32* v, f32 x, f32 y, f32 z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
}
}  // extern "C"

void HitVisit::Visit(ObstacleSink* q) {

    f32 push[3];
    push[0] = v[0] * ScaleOperand(5.0f);
    push[1] = v[1] * ScaleOperand(5.0f);
    push[2] = v[2] * ScaleOperand(5.0f);
    f32 adjusted[3];
    Set3(adjusted, LerpT(0.8f, at[0], F32((u8*)q, 0x60)), LerpT(0.8f, at[1], F32((u8*)q, 0x64)), LerpT(0.8f, at[2], F32((u8*)q, 0x68)));

    q->Hit(push, adjusted, force, extra);
}
