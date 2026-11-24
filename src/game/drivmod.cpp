// Driver model (0x801FFB08-0x80202218): the rider skeleton helpers ("butt", "wheel", "Lhand",
// "Rhand", "hand", "neck", "drivmod: can't find helper %s in %s"), pose blending and teardown. Written
// against raw field offsets; the large routines are still stubs. Names are guesses apart from the
// strings.
#include "game/common.h"
#include <revolution/MTX/vec.h>
#include "game/math_types.h"
#include "game/plainvec.h"
#include <string.h>
#include "game/mathutil.h"

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

extern "C" {
int fn_801E2CCC(u8* skel, const char* name, u8** bone);
void fn_802E30C8(const char* fmt, ...);
void fn_801E323C(u8* p);
void fn_801C1C7C(u8* p);
void fn_801E598C(u8* p);
void mrModelUnload(u8* p);
}

void operator delete(void* p);

// A position that starts at the origin.
struct ZeroPos {
    f32 x, y, z;
    ZeroPos(void) {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }
};

extern const char* lbl_8055ECA8;

ModuleTag lbl_8055ECA0;
const char* lbl_8055ECA8;
ZeroPos lbl_804888A8;

extern "C" {

f32* fn_801FFB08(u8* skel, const char* name) {
    u8* bone;
    if (!fn_801E2CCC(skel, name, &bone)) {
        fn_802E30C8("drivmod: can't find helper %s in %s", name, lbl_8055ECA8);
        return &lbl_804888A8.x;
    }
    return (f32*)(bone + 0x24);
}

void fn_801FFB6C(void) {}

void fn_801FFB70(void) {}

}  // extern "C"

// DMtx: common.h's PlainValMatrix (DVec3: valvec3.h's ValVec3).
// One posed skeleton bone (CopyBoneSet output, 0x38 bytes).
struct DBone {
    PlainValMatrix m;        // offset 0x0
    u8 pad30[0x8];
};

// Particle-system descriptors (ppsys.cpp).
struct DVertDesc {
    DVertDesc(void) {}
    DVertDesc(const ValVec3& p, f32 a, f32 b) : pos(p), w34(a), w30(b) {}
    ValVec3 pos;  // offset 0x0
    f32 w34;    // offset 0xC
    f32 w30;    // offset 0x10
};

struct DEdgeDesc {
    s32 a;      // offset 0x0
    s32 b;      // offset 0x4
    f32 extra;  // offset 0x8
    f32 stiff;  // offset 0xC: negative = slack
};

struct DPinDesc {
    s32 idx;           // offset 0x0
    const PlainValMatrix* mtx;   // offset 0x4
    const ValVec3* at;   // offset 0x8
};

// The driver model: its ragdoll, model, skeleton and the bike helpers it hangs from.
struct DriverModel {
    u8* sys;         // offset 0x0, the ragdoll particle system
    u8 active;       // offset 0x4
    s32 model;       // offset 0x8
    u8* skel;        // offset 0xC
    ValVec3 butt;      // offset 0x10, seat offset from the root bone
    u8* wheel;       // offset 0x1C
    u8* lhand;       // offset 0x20
    u8* rhand;       // offset 0x24
    u8* hand;        // offset 0x28
    u8 pad2C[0x4];
    f32 yaw;         // offset 0x30
    PlainValMatrix body;       // offset 0x34
    PlainValMatrix wheelMtx;   // offset 0x64
    PlainValMatrix rest;       // offset 0x94
    f32 fC4;         // offset 0xC4
    f32 fC8;         // offset 0xC8
    f32 fCC;         // offset 0xCC
};

extern "C" {
void* memset(void* p, int c, size_t n);
double atan2(double y, double x);
int fn_801E320C(u8* skel, const char* name);
u8* fn_801E3004(u8* skel, const char* name, int a);
u8* fn_801E31D8(u8* node, const char* name, int a);
s32 fn_802C6FC0(const char* name, int a, int b);
u8* fn_801E592C(s32 model);
void CopyBoneSet(u8* skel, DBone* out, int* n);
void fn_801E3488(ValVec3* out, u8* helper);
void fn_801E33E4(PlainValMatrix* out, u8* helper);
void fn_802E317C(const char* fmt, ...);
u8* fn_801C1BA4(int nPart, int nStick, int nAnchor);
void fn_801C1CBC(u8* sys, int nv, const DVertDesc* v, int ne, const DEdgeDesc* e, int np, const DPinDesc* p);
}
void* operator new(size_t size, const char* name);

// out = in * m (row vector), safe when out aliases in.
static inline void MulVec(ValVec3* out, const ValVec3* in, const PlainValMatrix* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m->basis[0].x, in->y, m->basis[1].x, in->z, m->basis[2].x);
        f32 y = Dot3(in->x, m->basis[0].y, in->y, m->basis[1].y, in->z, m->basis[2].y);
        f32 z = Dot3(in->x, m->basis[0].z, in->y, m->basis[1].z, in->z, m->basis[2].z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m->basis[0].x, in->y, m->basis[1].x, in->z, m->basis[2].x);
        out->y = Dot3(in->x, m->basis[0].y, in->y, m->basis[1].y, in->z, m->basis[2].y);
        out->z = Dot3(in->x, m->basis[0].z, in->y, m->basis[1].z, in->z, m->basis[2].z);
    }
}

static inline ValVec3 Mid(const ValVec3& a, const ValVec3& b) {
    ValVec3 r;
    r.x = a.x + 0.5f * (b.x - a.x);
    r.y = a.y + (b.y - a.y) / 2.0f;
    r.z = a.z + 0.5f * (b.z - a.z);
    return r;
}

extern "C" {

// Builds the driver model on skeleton skel: finds the wheel/hand helpers, poses the model and turns its hips,
// knees and feet (relative to the seat) into a 7-point ragdoll pinned to the bike.
// Helper names unproven: vector operations written as inlines, as the target evaluates them.
static inline void CopyD(ValVec3* o, const ValVec3* v) {
    o->x = v->x;
    o->y = v->y;
    o->z = v->z;
}
// Midpoint of two points, halves added lane by lane (name unproven).
static inline void MidPoint(ValVec3* out, const ValVec3* a, const ValVec3* b) {
    for (int j = 0; j < 3; j++) {
        (&out->x)[j] = (&a->x)[j] / 2.0f;
    }
    for (int j = 0; j < 3; j++) {
        (&out->x)[j] += (&b->x)[j] / 2.0f;
    }
}

// Componentwise lerp, lane by lane: the lane value is computed into a local and stored by a separate statement,
// so the block cut can fall between them as in retail (name unproven).
static inline void LerpV(ValVec3* out, const ValVec3& a, const ValVec3& b, f32 t) {
    for (int j = 2; j >= 0; j--) {
        f32 v = t * ((&b.x)[j] - (&a.x)[j]) + (&a.x)[j];
        (&out->x)[j] = v;
    }
}
// Plain float sum / difference (names unproven). Value-returning helpers add ISEL pcodes, which moves the
// scheduler's statement block cut (FACTS STATEMENT-BLOCK-CUT-100).
static inline f32 AddF(f32 a, f32 b) { return a + b; }
static inline f32 SubF(f32 a, const f32& b) { return a - b; }
DriverModel* fn_801FFB74(const char* name, u8* skel, u8* opt) {
    lbl_8055ECA8 = name;
    if (!fn_801E320C(skel, "butt")) {
        fn_802E30C8("drivmod: %s has no butt!", name);
        return 0;
    }
    DriverModel* d = new ("drivrmod") DriverModel;
    memset(d, 0, sizeof(DriverModel));
    d->active = 1;
    d->wheel = fn_801E3004(skel, "wheel", -1);
    d->lhand = fn_801E31D8(d->wheel, "Lhand", -1);
    d->rhand = fn_801E31D8(d->wheel, "Rhand", -1);
    d->hand = fn_801E3004(skel, "hand", -1);
    d->model = fn_802C6FC0("driver01.mod", 0, 0);
    d->skel = fn_801E592C(d->model);
    ValVec3 lh;
    fn_801E3488(&lh, d->lhand);
    ValVec3 lfoot;
    lfoot.x = lh.x;
    lfoot.y = lh.y;
    lfoot.z = lh.z;
    ValVec3 rh;
    fn_801E3488(&rh, d->rhand);
    ValVec3 rfoot;
    rfoot.x = rh.x;
    rfoot.y = rh.y;
    rfoot.z = rh.z;
    DBone bones[32];
    int n = 32;
    CopyBoneSet(d->skel, bones, &n);
    u8* butt;
    if (!fn_801E2CCC(skel, "butt", &butt)) {
        fn_802E317C("drivmod: can't get butt helper from %s", name);
    }
    CopyD(&d->butt, &((PlainValMatrix*)butt)->origin);
    f32 by = ((PlainValMatrix*)butt)->basis[1].y;
    f32 bz = ((PlainValMatrix*)butt)->basis[1].z;
    d->yaw = atan2(bz, by);
    d->butt.x = SubF(d->butt.x, bones[0].m.origin.x);
    d->butt.y = SubF(d->butt.y, bones[0].m.origin.y);
    d->butt.z = SubF(d->butt.z, bones[0].m.origin.z);
    // Retail keeps this store although lfoot is overwritten by m1 below. 472 instrs and retail's mnemonic mix.
    lfoot = bones[9].m.origin;
    ValVec3 lhip;
    ValVec3 lknee;
    ValVec3 rhip;
    ValVec3 rknee;
    // Sums in retail's order: the four z lanes first, then x/y. rknee and rhip are copied, then accumulated: the
    // read-backs are forwarded away but count as ISEL pcodes. With SubF on the three butt lanes the block cut falls
    // between m1.z's lane value and its store, as in retail.
    lknee.z = bones[8].m.origin.z + d->butt.z;
    rknee = bones[5].m.origin;
    rknee.z = AddF(d->butt.z, rknee.z);
    lhip.z = AddF(bones[7].m.origin.z, d->butt.z);
    rhip = bones[4].m.origin;
    rhip.z = AddF(rhip.z, d->butt.z);
    rhip.y = rhip.y + d->butt.y;
    lhip.x = AddF(bones[7].m.origin.x, d->butt.x);
    rhip.x = AddF(rhip.x, d->butt.x);
    lknee.x = bones[8].m.origin.x + d->butt.x;
    rknee.x = AddF(rknee.x, d->butt.x);
    lknee.y = bones[8].m.origin.y + d->butt.y;
    rknee.y = AddF(rknee.y, d->butt.y);
    lhip.y = AddF(bones[7].m.origin.y, d->butt.y);
    ValVec3 pelvis;
    ValVec3 m1;
    LerpV(&m1, lfoot, lknee, 0.5f);
    lfoot.z = m1.z;
    lfoot.y = m1.y;
    lfoot.x = m1.x;
    ValVec3 m2;
    LerpV(&m2, bones[6].m.origin, rknee, 0.5f);
    rfoot.x = m2.x;
    rfoot.y = m2.y;
    rfoot.z = m2.z;
    MidPoint(&pelvis, &lhip, &rhip);
    pelvis.y -= 1.27f;
    d->body.basis[0].x = 1.0f;
    d->body.basis[0].y = 0.0f;
    d->body.basis[1].x = 0.0f;
    d->body.basis[0].z = 0.0f;
    d->body.basis[1].z = 0.0f;
    d->body.basis[1].y = 1.0f;
    d->body.basis[2].z = 1.0f;
    d->body.origin.x = 0.0f;
    d->body.basis[2].x = 0.0f;
    d->body.basis[2].y = 0.0f;
    d->body.origin.y = 0.0f;
    d->body.origin.z = 0.0f;
    ValVec3 off;
    MulVec(&off, &d->butt, &d->body);
    d->body.origin.x = d->body.origin.x + off.x;
    d->body.origin.y = d->body.origin.y + off.y;
    d->body.origin.z = d->body.origin.z + off.z;
    d->rest = d->body;
    fn_801E33E4(&d->wheelMtx, d->wheel);
    DVertDesc verts[7] = {
        DVertDesc(lhip, 1.0f, 1.0f),
        DVertDesc(lknee, 1.0f, 1.0f),
        DVertDesc(lfoot, 1.0f, 1.0f),
        DVertDesc(rhip, 1.0f, 1.0f),
        DVertDesc(rknee, 1.0f, 1.0f),
        DVertDesc(rfoot, 1.0f, 1.0f),
        DVertDesc(pelvis, 1.0f, 1.0f),
    };
    DEdgeDesc edges[6] = {
        {0, 1, 0.0f, 0.0f}, {1, 2, 0.0f, 0.0f}, {3, 4, 0.0f, 0.0f},
        {4, 5, 0.0f, 0.0f}, {1, 6, -0.5f, 0.1f}, {4, 6, -0.5f, 0.1f},
    };
    DPinDesc pins[5] = {
        {0, &d->rest, 0}, {2, &d->wheelMtx, &lh}, {3, &d->rest, 0}, {5, &d->wheelMtx, &rh}, {6, &d->body, 0},
    };
    u8* sys = fn_801C1BA4(7, 6, 5);
    fn_801C1CBC(sys, 7, verts, 6, edges, 5, pins);
    d->sys = sys;
    d->fC4 = 0.0f;
    d->fCC = 0.0f;
    d->fC8 = 0.0f;
    return d;
}

// The upper-body driver (skeletons with a "neck" and no "butt"; others use fn_801FFB74): shoulders, hands and the
// points between them make the 7-point ragdoll.
DriverModel* fn_802002D4(const char* name, u8* skel, u8* opt) {
    lbl_8055ECA8 = name;
    if (fn_801E320C(skel, "butt")) {
        return fn_801FFB74(name, skel, opt);
    }
    if (!fn_801E320C(skel, "neck")) {
        return 0;
    }
    DriverModel* d = new ("drivrmod") DriverModel;
    memset(d, 0, sizeof(DriverModel));
    d->wheel = fn_801E3004(skel, "wheel", -1);
    d->lhand = fn_801E31D8(d->wheel, "Lhand", -1);
    d->rhand = fn_801E31D8(d->wheel, "Rhand", -1);
    d->hand = fn_801E3004(skel, "hand", -1);
    d->model = fn_802C6FC0("driver01.mod", 0, 0);
    d->skel = fn_801E592C(d->model);
    ValVec3 ls = *(ValVec3*)fn_801FFB08(skel, "lshoulder");
    ValVec3 lh;
    fn_801E3488(&lh, d->lhand);
    ValVec3 lmid;
    MidPoint(&lmid, &lh, &ls);
    lmid.y -= 0.2032f;
    ValVec3 rs = *(ValVec3*)fn_801FFB08(skel, "rshoulder");
    ValVec3 rh;
    fn_801E3488(&rh, d->rhand);
    ValVec3 rmid;
    MidPoint(&rmid, &rh, &rs);
    rmid.y -= 0.2032f;
    ValVec3 chest;
    MidPoint(&chest, &ls, &rs);
    chest.y -= 1.27f;
    DBone bones[32];
    int n = 32;
    CopyBoneSet(d->skel, bones, &n);
    d->butt.x = rs.x - bones[4].m.origin.x;
    d->butt.y = rs.y - bones[4].m.origin.y;
    d->butt.z = rs.z - bones[4].m.origin.z;
    d->butt.y += 0.0762f;
    d->body.basis[0].x = 1.0f;
    d->body.basis[0].y = 0.0f;
    d->body.basis[0].z = 0.0f;
    d->body.basis[1].x = 0.0f;
    d->body.basis[1].y = 1.0f;
    d->body.basis[1].z = 0.0f;
    d->body.basis[2].x = 0.0f;
    d->body.basis[2].y = 0.0f;
    d->body.basis[2].z = 1.0f;
    d->body.origin.x = 0.0f;
    d->body.origin.y = 0.0f;
    d->body.origin.z = 0.0f;
    ValVec3 off;
    MulVec(&off, &d->butt, &d->body);
    d->body.origin.x = d->body.origin.x + off.x;
    d->body.origin.y = d->body.origin.y + off.y;
    d->body.origin.z = d->body.origin.z + off.z;
    d->rest = d->body;
    fn_801E33E4(&d->wheelMtx, d->wheel);
    DVertDesc verts[7] = {
        DVertDesc(ls, 1.0f, 1.0f),
        DVertDesc(lmid, 1.0f, 1.0f),
        DVertDesc(lh, 1.0f, 1.0f),
        DVertDesc(rs, 1.0f, 1.0f),
        DVertDesc(rmid, 1.0f, 1.0f),
        DVertDesc(rh, 1.0f, 1.0f),
        DVertDesc(chest, 1.0f, 1.0f),
    };
    DEdgeDesc edges[6] = {
        {0, 1, 0.0f, 0.0f}, {1, 2, 0.0f, 0.0f}, {3, 4, 0.0f, 0.0f},
        {4, 5, 0.0f, 0.0f}, {1, 6, -0.5f, 0.1f}, {4, 6, -0.5f, 0.1f},
    };
    DPinDesc pins[5] = {
        {0, &d->rest, 0}, {2, &d->wheelMtx, 0}, {3, &d->rest, 0}, {5, &d->wheelMtx, 0}, {6, &d->body, 0},
    };
    u8* sys = fn_801C1BA4(7, 6, 5);
    fn_801C1CBC(sys, 7, verts, 6, edges, 5, pins);
    d->sys = sys;
    d->fC4 = 0.0f;
    d->fCC = 0.0f;
    d->fC8 = 0.0f;
    return d;
}

void fn_8020095C(u8* p) {
    if (p) {
        fn_801E323C(PTR(p, 0x1C));
        fn_801E323C(PTR(p, 0x20));
        fn_801E323C(PTR(p, 0x24));
        fn_801E323C(PTR(p, 0x28));
        fn_801C1C7C(PTR(p, 0x0));
        fn_801E598C(PTR(p, 0xC));
        mrModelUnload(PTR(p, 0x8));
        operator delete(p);
    }
}

}  // extern "C"

// A bone's rotation, as three rows.
// A pose whose rows copy as whole blocks (array rows, like camrec.h's CamVec3); names unproven.
struct DriverRow {
    f32 v[3];
};
struct BRot {
    BVec rx;  // offset 0x0
    BVec ry;  // offset 0xC
    BVec rz;  // offset 0x18
};

// A posed bone: rotation and position in the model's frame.
struct BonePose {
    BRot rot;  // offset 0x0
    DriverRow pos;  // offset 0x24: an array vector, so `root = pose[0].pos` copies as one block (lwz/stw) as in
                    // retail; the ValVec3 helpers (MulVec, TSub, DmPush) take it through a ValVec3 view.
};

// One bone of the rider: which pose slot, the skeleton point it sits on and the child point it aims at.
struct BoneLink {
    s32 index;  // offset 0x0
    s32 point;  // offset 0x4
    s32 child;  // offset 0x8, -1: none
};

f32 Dot(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz);
extern "C" {
double sin(double x);
double cos(double x);
}

static inline void MulM(f32* out, const f32* a, const f32* b) {
    if (out == b || out == a) {
        f32 m0 = Dot(a[0], b[0], a[3], b[1], a[6], b[2]);
        f32 m3 = Dot(a[0], b[3], a[3], b[4], a[6], b[5]);
        f32 m6 = Dot(a[0], b[6], a[3], b[7], a[6], b[8]);
        f32 m1 = Dot(a[1], b[0], a[4], b[1], a[7], b[2]);
        f32 m4 = Dot(a[1], b[3], a[4], b[4], a[7], b[5]);
        f32 m7 = Dot(a[1], b[6], a[4], b[7], a[7], b[8]);
        f32 m2 = Dot(a[2], b[0], a[5], b[1], a[8], b[2]);
        f32 m5 = Dot(a[2], b[3], a[5], b[4], a[8], b[5]);
        f32 m8 = Dot(a[2], b[6], a[5], b[7], a[8], b[8]);
        out[0] = m0;
        out[3] = m3;
        out[6] = m6;
        out[1] = m1;
        out[4] = m4;
        out[7] = m7;
        out[2] = m2;
        out[5] = m5;
        out[8] = m8;
    } else {
        out[0] = Dot(a[0], b[0], a[3], b[1], a[6], b[2]);
        out[3] = Dot(a[0], b[3], a[3], b[4], a[6], b[5]);
        out[6] = Dot(a[0], b[6], a[3], b[7], a[6], b[8]);
        out[1] = Dot(a[1], b[0], a[4], b[1], a[7], b[2]);
        out[4] = Dot(a[1], b[3], a[4], b[4], a[7], b[5]);
        out[7] = Dot(a[1], b[6], a[4], b[7], a[7], b[8]);
        out[2] = Dot(a[2], b[0], a[5], b[1], a[8], b[2]);
        out[5] = Dot(a[2], b[3], a[5], b[4], a[8], b[5]);
        out[8] = Dot(a[2], b[6], a[5], b[7], a[8], b[8]);
    }
}

static inline void SetIdentity(f32* m) {
    m[0] = 1.0f;
    m[1] = 0.0f;
    m[2] = 0.0f;
    m[3] = 0.0f;
    m[4] = 1.0f;
    m[5] = 0.0f;
    m[6] = 0.0f;
    m[7] = 0.0f;
    m[8] = 1.0f;
}

extern "C" {

// Poses the rider's bones on the skeleton points: each bone sits on its point (in the model's frame f)
// and, when it has a child, is turned from its bind direction to aim at the child.
}
template <class O, class A, class B> static inline void TSub(O* o, const A* a, const B* b) { o->x = a->x - b->x; o->y = a->y - b->y; o->z = a->z - b->z; }  /* Helper name unproven */
extern "C" {
/* Helper names unproven. The bone's rotation rows as their own inline, in rigid's exact form (0fccfbd8b).
   What retail's registers say about its shape (numsolve: the first Dot3 block's z, kept in f24 across the calls, is
   coloured after the axis components and before the sine):
   - the axis components are PARAMETER objects of a statement-level inline (created before the Dot3 temporaries),
     given as expressions and in the order z, y, x (the first parameter is numbered highest: az f26, ay f25, ax f24;
     arguments are evaluated right to left, so x is still computed first);
   - the sine comes through an expression inline (SinF), whose result object is created after the Dot3 temporaries.
   The locals are declared in the turned-round order an inline needs (they number upward). */
static inline f32 SinF(f32 x) { return (f32)sin(x); }
static inline void AxisAngleRows(BRot& r, f32 az, f32 ay, f32 ax, f32 a) {
    f32 cs;
    f32 zz;
    f32 yy;
    f32 xx;
    f32 zDiag;
    f32 yDiag;
    f32 t;
    f32 sn;
    sn = SinF(a);
    cs = cos(a);
    xx = ax * ax;
    yy = ay * ay;
    zz = az * az;
    t = 1.0f - cs;
    r.rx.x = cs + t * xx;
    r.rx.y = ay * (t * ax) + sn * az;
    r.rx.z = az * (t * ax) - sn * ay;
    yDiag = cs + t * yy;
    r.ry.x = ay * (t * ax) - sn * az;
    r.ry.y = yDiag;
    r.ry.z = az * (t * ay) + sn * ax;
    zDiag = cs + t * zz;
    r.rz.x = az * (t * ax) + sn * ay;
    r.rz.y = az * (t * ay) - sn * ax;
    r.rz.z = zDiag;
}
void fn_802009CC(BonePose* out, const BVec* pts, const BoneLink* links, int n, const u8* bind, const BonePose* f) {
    int i = 0;
    if (n <= 0) {
        return;
    }
    for (; i < n; i++) {
        const BoneLink* p = &links[i];
        BonePose* o = &out[p->index];
        const BRot* b = (const BRot*)(bind + p->index * 0x38);
        BVec v;
        TSub(&v, &pts[p->point], (const ValVec3*)&f->pos);
        {
            f32 x = Dot3(v.x, f->rot.rx.x, v.y, f->rot.rx.y, v.z, f->rot.rx.z);
            f32 y = Dot3(v.x, f->rot.ry.x, v.y, f->rot.ry.y, v.z, f->rot.ry.z);
            f32 z = Dot3(v.x, f->rot.rz.x, v.y, f->rot.rz.y, v.z, f->rot.rz.z);
            v.x = x;
            v.y = y;
            v.z = z;
        }
        *(ValVec3*)&o->pos = v;
        if (p->child >= 0) {
            BVec c;
            c.x = pts[p->child].x - f->pos.v[0];
            c.y = pts[p->child].y - f->pos.v[1];
            c.z = pts[p->child].z - f->pos.v[2];
            {
                f32 x = Dot3(c.x, f->rot.rx.x, c.y, f->rot.rx.y, c.z, f->rot.rx.z);
                f32 y = Dot3(c.x, f->rot.ry.x, c.y, f->rot.ry.y, c.z, f->rot.ry.z);
                f32 z = Dot3(c.x, f->rot.rz.x, c.y, f->rot.rz.y, c.z, f->rot.rz.z);
                c.x = x;
                c.y = y;
                c.z = z;
            }
            BVec d;
            d.x = c.x - v.x;
            d.y = c.y - v.y;
            d.z = c.z - v.z;
            f32 len = PSVECMag((const Vec*)&d.x);
            BRot m;
            if (len > 0.001f) {
                f32 k = -1.0f / len;
                // Retail keeps the unit direction in m's first row (the same stack slot) until m is computed.
                m.rx.x = d.x * k;
                m.rx.y = d.y * k;
                m.rx.z = d.z * k;
                BVec axis;
                PSVECCrossProduct((const Vec*)&b->rx.x, (const Vec*)&m.rx.x, (Vec*)&axis.x);
                f32 a = PSVECMag((const Vec*)&axis.x);
                BRot r;
                if (a > 0.0001f) {
                    f32 inv = 1.0f / a;
                    AxisAngleRows(r, inv * axis.z, inv * axis.y, inv * axis.x, a);
                } else {
                    SetIdentity(&r.rx.x);
                }
                MulM(&m.rx.x, &b->rx.x, &r.rx.x);
            } else {
                SetIdentity(&m.rx.x);
            }
            o->rot = m;
        } else {
            o->rot = *b;
        }
    }
}

}  // extern "C"


// The six rider bones aimed by the ragdoll (fn_802009CC).
struct RiderLinks {
    BoneLink l[6];
};
template <class O, class D> static inline void DmPush(O* o, const D* d, f32 k) { o->x = o->x + d->x * k; o->y = o->y + d->y * k; o->z = o->z + d->z * k; }  /* Helper name unproven: three inches along an axis */

extern "C" {
extern const RiderLinks lbl_802F9D00;
void fn_801E353C(u8* helper, f32 angle);
void fn_801E34E4(u8* helper, PlainValMatrix* m);
void fn_801E3284(PlainValMatrix* out, u8* helper, const PlainValMatrix* frame);
void fn_801C2694(u8* sys, BVec* pts, int* n, const BVec* g, const BVec* up, const BVec* a, const BVec* b);
s32 UpdateBoneTransforms(u8* skel, BonePose* bones, int n, int x);
void fn_802C70AC(s32 model, const PlainValMatrix* m, s32 a, int b);

// Name unproven.
static inline f32 DriverOriginX(f32 offset, f32 origin) {
    return origin + offset;
}

// Per-frame rider update for car state `car`: steering lean, seat frame, the body twist from yaw and roll,
// the ragdoll step, then the posed skeleton (legs aimed by the ragdoll, hands slid along the arms, upper body
// bent by speed and pitch) drawn at the car.

struct DriverRowPose {
    DriverRow r0;
    DriverRow r1;
    DriverRow r2;
    ValVec3 o;
};

// Moves m's origin by p taken through m's rotation (name unproven).
static inline void OffsetOrigin(PlainValMatrix* m, const ValVec3* p) {
    ValVec3 off;
    MulVec(&off, p, m);
    m->origin.x = m->origin.x + off.x;
    m->origin.y = m->origin.y + off.y;
    m->origin.z = m->origin.z + off.z;
}
void fn_80201018(DriverModel* d, const u8* car, int show) {
    DriverRow root;
    BonePose pose[32];
    DBone bones[32];
    BVec pts[32];
    RiderLinks links;
    PlainValMatrix m;
    BRot rot;
    PlainValMatrix m2;
    DriverRowPose rm;
    BRot tilt;
    BRot turn;
    BVec v;
    BVec g;
    BVec up;
    BVec b;
    BVec a;
    BVec e1;
    BVec e2;
    ValVec3 off;
    int cnt;
    int n;
    if (!show || !d) {
        return;
    }
    m = *(PlainValMatrix*)car;
    f32 oldx = d->fC8;
    f32 oldy = d->fCC;
    d->fC8 = 0.0f;
    d->fCC = 0.0f;
    switch (S32(car, 0x48)) {
    case -1:
        d->fC8 = 1.0f;
        d->fCC = -1.0f;
        break;
    case 0:
        d->fC8 = oldx;
        d->fCC = 0.0f;
        break;
    case 1:
        d->fC8 = -1.0f;
        d->fCC = 1.0f;
        break;
    case 2:
        d->fC8 = -1.0f;
        d->fCC = -1.0f;
        break;
    case 3:
        d->fC8 = 0.0f;
        d->fCC = 1.0f;
        break;
    case 4:
        d->fC8 = 0.0f;
        d->fCC = -1.0f;
        break;
    case 5:
        d->fC8 = 1.0f;
        d->fCC = 1.0f;
        break;
    case 6:
        d->fC8 = 1.0f;
        d->fCC = -1.0f;
        break;
    }
    f32 ny = d->fCC;
    d->fCC = 0.5f * ny + 0.5f * oldy;
    if ((f32)AbsD(d->fCC) < 0.1f || ny * oldy > 0.0f) {
        if (S32(car, 0x48) == 0) {
            d->fC8 = 0.9f * oldx;
        }
    } else {
        d->fC8 = oldx;
    }
    f32 lock = 1.0471976f;  // pi/3; a local, so the negation stays at run time (retail pools +pi/3 only)
    fn_801E353C(d->wheel, -lock * F32(car, 0x3C));
    fn_801E34E4(d->wheel, &m);
    cnt = 32;
    d->body = m;
    MulVec(&off, &d->butt, &d->body);
    d->body.origin.x = DriverOriginX(off.x, d->body.origin.x);
    d->body.origin.y = d->body.origin.y + off.y;
    d->body.origin.z = d->body.origin.z + off.z;
    f32 roll = -F32(car, 0x238);
    f32 lim = 0.17453292f;
    if (lim < roll) {
        roll = lim;
    }
    roll = MaxF(roll, -lim);
    v.x = F32(car, 0x240) + d->yaw;
    v.y = 0.0f;
    v.z = roll;
    f32 ang = PSVECMag((const Vec*)&v.x);
    if (ang > 0.0001f) {
        f32 inv = 1.0f / ang;
        AxisAngleRows(rot, inv * v.z, inv * v.y, inv * v.x, ang);
    } else {
        SetIdentity(&rot.rx.x);
    }
    // Named references: retail keeps both matrix addresses in saved registers through MulM's Dot calls.
    const PlainValMatrix& body = d->body;
    PlainValMatrix& rest = d->rest;
    MulM(&rest.basis[0].x, &body.basis[0].x, &rot.rx.x);
    d->rest.origin = d->body.origin;
    fn_801E3284(&d->wheelMtx, d->wheel, &m);
    a.x = 0.0f;
    a.y = 0.0f;
    a.z = 0.0f;
    b.x = 0.0f;
    b.y = 0.0f;
    b.z = 0.0f;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    g.x = 0.0f;
    g.y = -1000.0f;
    g.z = 0.0f;
    fn_801C2694(d->sys, pts, &cnt, &g, &up, &b, &a);
    n = 32;
    CopyBoneSet(d->skel, bones, &n);
    // Retail builds this pose and never reads it (its user was stripped): the compiler splits it into one home
    // per member (IRO_ScalarizeClassDataMembers), the six dead stores in 240 extra frame bytes. The loop reads rot.
    rm.r0 = *(const DriverRow*)&rot.rx;
    rm.r1 = *(const DriverRow*)&rot.ry;
    rm.r2 = *(const DriverRow*)&rot.rz;
    rm.o.x = 0.0f;
    rm.o.y = 0.0f;
    rm.o.z = 0.0f;
    BonePose* pp = pose;
    DBone* bone = bones;
    for (int k = 0; k < n; bone++, pp++, k++) {
        if (k) {
            MulM(&pp->rot.rx.x, &bone->m.basis[0].x, &rot.rx.x);
            MulVec((ValVec3*)&pp->pos, &bone->m.origin, (PlainValMatrix*)&rot);
        } else {
            *pp = *(BonePose*)bone;
        }
        f32 xv = pp->pos.v[0];
        pp->pos.v[0] = xv + d->butt.x;
        f32 yv = pp->pos.v[1];
        pp->pos.v[1] = yv + d->butt.y;
        f32 zv = pp->pos.v[2];
        pp->pos.v[2] = zv + d->butt.z;
    }
    f32 z = Mul(2.5f, -F32(car, 0x238));
    f32 y = Mul(F32(car, 0x3C), 0.2617994f);
    z += Mul(F32(car, 0x3C), -0.17453292f);
    f32 lean = MaxF(MinF(0.2617994f, 8.0f * F32(car, 0x23C)), -0.2617994f);
    e1.y = y;
    e1.z = z;
    e1.x = 1.5f * F32(car, 0x240) - lean;
    Mat3FromEuler((Mat3*)&tilt, (Vec3*)&e1);
    Mat3Mul((Mat3*)&pose[3].rot, (Mat3*)&tilt, (Mat3*)&pose[3].rot);
    links = lbl_802F9D00;
    fn_802009CC(pose, pts, links.l, 6, (u8*)bones, (BonePose*)&m);
    DmPush((ValVec3*)&pose[9].pos, &pose[8].rot.rx, 0.0762f);
    DmPush((ValVec3*)&pose[6].pos, &pose[5].rot.rx, 0.0762f);
    e2.x = d->yaw;
    e2.y = 0.0f;
    e2.z = 0.0f;
    Mat3FromEuler((Mat3*)&turn, (Vec3*)&e2);
    Mat3Mul((Mat3*)&pose[0].rot, (Mat3*)&turn, (Mat3*)&pose[0].rot);
    root = pose[0].pos;
    for (int i = 0; i < n; i++) {
        pose[i].pos.v[0] -= root.v[0];
        pose[i].pos.v[1] -= root.v[1];
        pose[i].pos.v[2] -= root.v[2];
    }
    s32 r = UpdateBoneTransforms(d->skel, pose, n, 0);
    m2 = m;
    OffsetOrigin(&m2, (const ValVec3*)&root);
    fn_802C70AC(d->model, &m2, r, -1);
}

}  // extern "C"

// Rider bone links (bone, point, child), owned .rodata after the pose templates.
extern const RiderLinks lbl_802F9D00 = {{{7, 0, 1}, {8, 1, 2}, {9, 2, -1}, {4, 3, 4}, {5, 4, 5}, {6, 5, -1}}};
