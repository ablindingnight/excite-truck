// Driver model (0x801FFB08-0x80202218): the rider skeleton helpers ("butt", "wheel", "Lhand",
// "Rhand", "hand", "neck", "drivmod: can't find helper %s in %s"), pose blending and teardown. Written
// against raw field offsets; the large routines are still stubs. Names are guesses apart from the
// strings.
#include "game/common.h"
#include <string.h>

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

void fn_801FFB6C(u8* p) {}

void fn_801FFB70(u8* p) {}

}  // extern "C"

// A plain vector (copyable aggregate) and a rotation + position matrix.
struct DVec3 {
    f32 x, y, z;
};

struct DMtx {
    DVec3 row[3];  // offset 0x0
    DVec3 pos;     // offset 0x24
};

// One posed skeleton bone (fn_801E5998 output, 0x38 bytes).
struct DBone {
    DMtx m;        // offset 0x0
    u8 pad30[0x8];
};

// Particle-system descriptors (ppsys.cpp).
struct DVertDesc {
    DVec3 pos;  // offset 0x0
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
    const DMtx* mtx;   // offset 0x4
    const DVec3* at;   // offset 0x8
};

// The driver model: its ragdoll, model, skeleton and the bike helpers it hangs from.
struct DriverModel {
    u8* sys;         // offset 0x0, the ragdoll particle system
    u8 active;       // offset 0x4
    s32 model;       // offset 0x8
    u8* skel;        // offset 0xC
    DVec3 butt;      // offset 0x10, seat offset from the root bone
    u8* wheel;       // offset 0x1C
    u8* lhand;       // offset 0x20
    u8* rhand;       // offset 0x24
    u8* hand;        // offset 0x28
    u8 pad2C[0x4];
    f32 yaw;         // offset 0x30
    DMtx body;       // offset 0x34
    DMtx wheelMtx;   // offset 0x64
    DMtx rest;       // offset 0x94
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
void fn_801E5998(u8* skel, DBone* out, int* n);
void fn_801E3488(DVec3* out, u8* helper);
void fn_801E33E4(DMtx* out, u8* helper);
void fn_802E317C(const char* fmt, ...);
u8* fn_801C1BA4(int nPart, int nStick, int nAnchor);
void fn_801C1CBC(u8* sys, int nv, const DVertDesc* v, int ne, const DEdgeDesc* e, int np, const DPinDesc* p);
}
void* operator new(size_t size, const char* name);

// out = in * m (row vector), safe when out aliases in.
static inline void MulVec(DVec3* out, const DVec3* in, const DMtx* m) {
    if (out == in) {
        f32 x = in->x * m->row[0].x + in->y * m->row[1].x + in->z * m->row[2].x;
        f32 y = in->x * m->row[0].y + in->y * m->row[1].y + in->z * m->row[2].y;
        f32 z = in->x * m->row[0].z + in->y * m->row[1].z + in->z * m->row[2].z;
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = in->x * m->row[0].x + in->y * m->row[1].x + in->z * m->row[2].x;
        out->y = in->x * m->row[0].y + in->y * m->row[1].y + in->z * m->row[2].y;
        out->z = in->x * m->row[0].z + in->y * m->row[1].z + in->z * m->row[2].z;
    }
}

static inline DVec3 Mid(const DVec3& a, const DVec3& b) {
    DVec3 r;
    r.x = a.x + 0.5f * (b.x - a.x);
    r.y = a.y + 0.5f * (b.y - a.y);
    r.z = a.z + 0.5f * (b.z - a.z);
    return r;
}

extern "C" {

// Builds the driver model on skeleton skel: finds the wheel/hand helpers, poses the model and turns its hips,
// knees and feet (relative to the seat) into a 7-point ragdoll pinned to the bike.
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
    DVec3 lh;
    fn_801E3488(&lh, d->lhand);
    DVec3 lfoot;
    lfoot.x = lh.x;
    lfoot.y = lh.y;
    lfoot.z = lh.z;
    DVec3 rh;
    fn_801E3488(&rh, d->rhand);
    DVec3 rfoot;
    rfoot.x = rh.x;
    rfoot.y = rh.y;
    rfoot.z = rh.z;
    DBone bones[32];
    int n = 32;
    fn_801E5998(d->skel, bones, &n);
    u8* butt;
    if (!fn_801E2CCC(skel, "butt", &butt)) {
        fn_802E317C("drivmod: can't get butt helper from %s", name);
    }
    d->butt.x = ((DMtx*)butt)->pos.x;
    d->butt.y = ((DMtx*)butt)->pos.y;
    d->butt.z = ((DMtx*)butt)->pos.z;
    d->yaw = atan2(((DMtx*)butt)->row[1].z, ((DMtx*)butt)->row[1].y);
    d->butt.x = d->butt.x - bones[0].m.pos.x;
    d->butt.y = d->butt.y - bones[0].m.pos.y;
    d->butt.z = d->butt.z - bones[0].m.pos.z;
    DVec3 lhip;
    lhip.x = bones[7].m.pos.x + d->butt.x;
    lhip.y = bones[7].m.pos.y + d->butt.y;
    lhip.z = bones[7].m.pos.z + d->butt.z;
    DVec3 lknee;
    lknee.x = bones[8].m.pos.x + d->butt.x;
    lknee.y = bones[8].m.pos.y + d->butt.y;
    lknee.z = bones[8].m.pos.z + d->butt.z;
    DVec3 rhip;
    rhip.x = bones[4].m.pos.x + d->butt.x;
    rhip.y = bones[4].m.pos.y + d->butt.y;
    rhip.z = bones[4].m.pos.z + d->butt.z;
    DVec3 rknee;
    rknee.x = bones[5].m.pos.x + d->butt.x;
    rknee.y = bones[5].m.pos.y + d->butt.y;
    rknee.z = bones[5].m.pos.z + d->butt.z;
    lfoot.x = bones[9].m.pos.x;
    lfoot.y = bones[9].m.pos.y;
    lfoot.z = bones[9].m.pos.z;
    DVec3 m1 = Mid(lfoot, lknee);
    lfoot.x = m1.x;
    lfoot.y = m1.y;
    lfoot.z = m1.z;
    DVec3 m2 = Mid(bones[6].m.pos, rknee);
    rfoot.x = m2.x;
    rfoot.y = m2.y;
    rfoot.z = m2.z;
    DVec3 pelvis;
    pelvis.x = 0.5f * lhip.x + 0.5f * rhip.x;
    pelvis.y = 0.5f * lhip.y + 0.5f * rhip.y - 1.27f;
    pelvis.z = 0.5f * lhip.z + 0.5f * rhip.z;
    d->body.row[0].x = 1.0f;
    d->body.row[0].y = 0.0f;
    d->body.row[0].z = 0.0f;
    d->body.row[1].x = 0.0f;
    d->body.row[1].y = 1.0f;
    d->body.row[1].z = 0.0f;
    d->body.row[2].x = 0.0f;
    d->body.row[2].y = 0.0f;
    d->body.row[2].z = 1.0f;
    d->body.pos.x = 0.0f;
    d->body.pos.y = 0.0f;
    d->body.pos.z = 0.0f;
    DVec3 off;
    MulVec(&off, &d->butt, &d->body);
    d->body.pos.x = d->body.pos.x + off.x;
    d->body.pos.y = d->body.pos.y + off.y;
    d->body.pos.z = d->body.pos.z + off.z;
    d->rest = d->body;
    fn_801E33E4(&d->wheelMtx, d->wheel);
    DEdgeDesc edges[6] = {
        {0, 1, 0.0f, 0.0f}, {1, 2, 0.0f, 0.0f}, {3, 4, 0.0f, 0.0f},
        {4, 5, 0.0f, 0.0f}, {1, 6, -0.5f, 0.1f}, {4, 6, -0.5f, 0.1f},
    };
    DVertDesc verts[7];
    verts[0].pos = lhip;
    verts[0].w34 = 1.0f;
    verts[0].w30 = 1.0f;
    verts[1].pos = lknee;
    verts[1].w34 = 1.0f;
    verts[1].w30 = 1.0f;
    verts[2].pos = lfoot;
    verts[2].w34 = 1.0f;
    verts[2].w30 = 1.0f;
    verts[3].pos = rhip;
    verts[3].w34 = 1.0f;
    verts[3].w30 = 1.0f;
    verts[4].pos = rknee;
    verts[4].w34 = 1.0f;
    verts[4].w30 = 1.0f;
    verts[5].pos = rfoot;
    verts[5].w34 = 1.0f;
    verts[5].w30 = 1.0f;
    verts[6].pos = pelvis;
    verts[6].w34 = 1.0f;
    verts[6].w30 = 1.0f;
    DPinDesc pins[5] = {
        {0, &d->rest, 0}, {2, &d->wheelMtx, &lh}, {3, &d->rest, 0}, {5, &d->wheelMtx, &rh}, {6, &d->body, 0},
    };
    d->sys = fn_801C1BA4(7, 6, 5);
    fn_801C1CBC(d->sys, 7, verts, 6, edges, 5, pins);
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
    DVec3 ls = *(DVec3*)fn_801FFB08(skel, "lshoulder");
    DVec3 lh;
    fn_801E3488(&lh, d->lhand);
    DVec3 lmid;
    lmid.x = 0.5f * lh.x + 0.5f * ls.x;
    lmid.y = 0.5f * lh.y + 0.5f * ls.y - 0.2032f;
    lmid.z = 0.5f * lh.z + 0.5f * ls.z;
    DVec3 rs = *(DVec3*)fn_801FFB08(skel, "rshoulder");
    DVec3 rh;
    fn_801E3488(&rh, d->rhand);
    DVec3 rmid;
    rmid.x = 0.5f * rh.x + 0.5f * rs.x;
    rmid.y = 0.5f * rh.y + 0.5f * rs.y - 0.2032f;
    rmid.z = 0.5f * rh.z + 0.5f * rs.z;
    DVec3 chest;
    chest.x = 0.5f * ls.x + 0.5f * rs.x;
    chest.y = 0.5f * ls.y + 0.5f * rs.y - 1.27f;
    chest.z = 0.5f * ls.z + 0.5f * rs.z;
    DBone bones[32];
    int n = 32;
    fn_801E5998(d->skel, bones, &n);
    d->butt.x = rs.x - bones[4].m.pos.x;
    d->butt.y = rs.y - bones[4].m.pos.y;
    d->butt.z = rs.z - bones[4].m.pos.z;
    d->butt.y = d->butt.y + 0.0762f;
    d->body.row[0].x = 1.0f;
    d->body.row[0].y = 0.0f;
    d->body.row[0].z = 0.0f;
    d->body.row[1].x = 0.0f;
    d->body.row[1].y = 1.0f;
    d->body.row[1].z = 0.0f;
    d->body.row[2].x = 0.0f;
    d->body.row[2].y = 0.0f;
    d->body.row[2].z = 1.0f;
    d->body.pos.x = 0.0f;
    d->body.pos.y = 0.0f;
    d->body.pos.z = 0.0f;
    DVec3 off;
    MulVec(&off, &d->butt, &d->body);
    d->body.pos.x = d->body.pos.x + off.x;
    d->body.pos.y = d->body.pos.y + off.y;
    d->body.pos.z = d->body.pos.z + off.z;
    d->rest = d->body;
    fn_801E33E4(&d->wheelMtx, d->wheel);
    DEdgeDesc edges[6] = {
        {0, 1, 0.0f, 0.0f}, {1, 2, 0.0f, 0.0f}, {3, 4, 0.0f, 0.0f},
        {4, 5, 0.0f, 0.0f}, {1, 6, -0.5f, 0.1f}, {4, 6, -0.5f, 0.1f},
    };
    DVertDesc verts[7];
    verts[0].pos = ls;
    verts[0].w34 = 1.0f;
    verts[0].w30 = 1.0f;
    verts[1].pos = lmid;
    verts[1].w34 = 1.0f;
    verts[1].w30 = 1.0f;
    verts[2].pos = lh;
    verts[2].w34 = 1.0f;
    verts[2].w30 = 1.0f;
    verts[3].pos = rs;
    verts[3].w34 = 1.0f;
    verts[3].w30 = 1.0f;
    verts[4].pos = rmid;
    verts[4].w34 = 1.0f;
    verts[4].w30 = 1.0f;
    verts[5].pos = rh;
    verts[5].w34 = 1.0f;
    verts[5].w30 = 1.0f;
    verts[6].pos = chest;
    verts[6].w34 = 1.0f;
    verts[6].w30 = 1.0f;
    DPinDesc pins[5] = {
        {0, &d->rest, 0}, {2, &d->wheelMtx, 0}, {3, &d->rest, 0}, {5, &d->wheelMtx, 0}, {6, &d->body, 0},
    };
    d->sys = fn_801C1BA4(7, 6, 5);
    fn_801C1CBC(d->sys, 7, verts, 6, edges, 5, pins);
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

struct BVec {
    f32 x, y, z;
};

// A bone's rotation, as three rows.
struct BRot {
    BVec r[3];
};

// A posed bone: rotation and position in the model's frame.
struct BonePose {
    BRot rot;  // offset 0x0
    BVec pos;  // offset 0x24
};

// One bone of the rider: which pose slot, the skeleton point it sits on and the child point it aims at.
struct BoneLink {
    s32 index;  // offset 0x0
    s32 point;  // offset 0x4
    s32 child;  // offset 0x8, -1: none
};

f32 Dot(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz);
double sin(double x);
double cos(double x);
extern "C" {
f32 PSVECMag(const f32* v);
void PSVECCrossProduct(const f32* a, const f32* b, f32* out);
}
extern const f32 lbl_80563058;  // 1.0
extern const f32 lbl_8056305C;  // 0.0
extern const f32 lbl_80563068;  // 0.001
extern const f32 lbl_8056306C;  // -1.0
extern const f32 lbl_80563070;  // 0.0001

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
    m[0] = lbl_80563058;
    m[1] = lbl_8056305C;
    m[2] = lbl_8056305C;
    m[3] = lbl_8056305C;
    m[4] = lbl_80563058;
    m[5] = lbl_8056305C;
    m[6] = lbl_8056305C;
    m[7] = lbl_8056305C;
    m[8] = lbl_80563058;
}

extern "C" {

// Poses the rider's bones on the skeleton points: each bone sits on its point (in the model's frame f)
// and, when it has a child, is turned from its bind direction to aim at the child.
void fn_802009CC(BonePose* out, const BVec* pts, const BoneLink* links, int n, const u8* bind, const BonePose* f) {
    for (int i = 0; i < n; i++, links++) {
        BonePose* o = &out[links->index];
        const BRot* b = (const BRot*)(bind + links->index * 0x38);
        BVec v;
        v.x = pts[links->point].x - f->pos.x;
        v.y = pts[links->point].y - f->pos.y;
        v.z = pts[links->point].z - f->pos.z;
        {
            f32 x = v.z * f->rot.r[0].z + (v.x * f->rot.r[0].x + v.y * f->rot.r[0].y);
            f32 y = v.z * f->rot.r[1].z + (v.x * f->rot.r[1].x + v.y * f->rot.r[1].y);
            f32 z = v.z * f->rot.r[2].z + (v.x * f->rot.r[2].x + v.y * f->rot.r[2].y);
            v.x = x;
            v.y = y;
            v.z = z;
        }
        o->pos = v;
        if (links->child >= 0) {
            BVec c;
            c.x = pts[links->child].x - f->pos.x;
            c.y = pts[links->child].y - f->pos.y;
            c.z = pts[links->child].z - f->pos.z;
            {
                f32 x = c.z * f->rot.r[0].z + (c.x * f->rot.r[0].x + c.y * f->rot.r[0].y);
                f32 y = c.z * f->rot.r[1].z + (c.x * f->rot.r[1].x + c.y * f->rot.r[1].y);
                f32 z = c.z * f->rot.r[2].z + (c.x * f->rot.r[2].x + c.y * f->rot.r[2].y);
                c.x = x;
                c.y = y;
                c.z = z;
            }
            BVec d;
            d.x = c.x - v.x;
            d.y = c.y - v.y;
            d.z = c.z - v.z;
            f32 len = PSVECMag(&d.x);
            BRot m;
            if (len > lbl_80563068) {
                f32 k = lbl_8056306C / len;
                BVec u;
                u.x = d.x * k;
                u.y = d.y * k;
                u.z = d.z * k;
                BVec axis;
                PSVECCrossProduct(&b->r[0].x, &u.x, &axis.x);
                f32 a = PSVECMag(&axis.x);
                BRot r;
                if (a > lbl_80563070) {
                    f32 inv = lbl_80563058 / a;
                    f32 ax = inv * axis.x;
                    f32 ay = inv * axis.y;
                    f32 az = inv * axis.z;
                    f32 sn = sin(a);
                    f32 cs = cos(a);
                    f32 t = lbl_80563058 - cs;
                    r.r[0].x = cs + t * (ax * ax);
                    r.r[0].y = ay * (t * ax) + sn * az;
                    r.r[0].z = az * (t * ax) - sn * ay;
                    r.r[1].x = ay * (t * ax) - sn * az;
                    r.r[1].y = cs + t * (ay * ay);
                    r.r[1].z = az * (t * ay) + sn * ax;
                    r.r[2].x = az * (t * ax) + sn * ay;
                    r.r[2].y = az * (t * ay) - sn * ax;
                    r.r[2].z = cs + t * (az * az);
                } else {
                    SetIdentity(&r.r[0].x);
                }
                MulM(&m.r[0].x, &b->r[0].x, &r.r[0].x);
            } else {
                SetIdentity(&m.r[0].x);
            }
            o->rot = m;
        } else {
            o->rot.r[0] = b->r[0];
            o->rot.r[1] = b->r[1];
            o->rot.r[2] = b->r[2];
        }
    }
}

}  // extern "C"

static inline f32 MinF(f32 a, f32 b) { return a < b ? a : b; }
static inline f32 MaxF(f32 a, f32 b) { return a > b ? a : b; }

// The six rider bones aimed by the ragdoll (fn_802009CC).
struct RiderLinks {
    BoneLink l[6];
};

struct Mat3;
struct Vec3;
void Mat3FromEuler(Mat3* out, const Vec3* e);
void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);
extern const f32 lbl_80563050;  // 0.5
extern const f32 lbl_80563064;  // 0.0762 (3 in)
extern const f32 lbl_80563074;  // 0.1
extern const f32 lbl_80563078;  // 0.9
extern const f32 lbl_8056307C;  // pi/3
extern const f32 lbl_80563080;  // 10 deg
extern const f32 lbl_80563084;  // -1000
extern const f32 lbl_80563088;  // 2.5
extern const f32 lbl_8056308C;  // -10 deg
extern const f32 lbl_80563090;  // 15 deg
extern const f32 lbl_80563094;  // 8
extern const f32 lbl_80563098;  // -15 deg
extern const f32 lbl_8056309C;  // 1.5

extern "C" {
extern const RiderLinks lbl_802F9D00;
void fn_801E353C(u8* helper, f32 angle);
void fn_801E34E4(u8* helper, DMtx* m);
void fn_801E3284(DMtx* out, u8* helper, const DMtx* frame);
void fn_801C2694(u8* sys, BVec* pts, int* n, const BVec* g, const BVec* up, const BVec* a, const BVec* b);
s32 fn_801E65BC(u8* skel, BonePose* bones, int n, int x);
void fn_802C70AC(s32 model, const DMtx* m, s32 a, int b);

// Per-frame rider update for car state `car`: steering lean, seat frame, the body twist from yaw and roll,
// the ragdoll step, then the posed skeleton (legs aimed by the ragdoll, hands slid along the arms, upper body
// bent by speed and pitch) drawn at the car.
void fn_80201018(DriverModel* d, u8* car, int show) {
    BonePose pose[32];
    DBone bones[32];
    BVec pts[32];
    RiderLinks links;
    DMtx m;
    BRot rot;
    DMtx m2;
    BRot tilt;
    BRot turn;
    DVec3 root;
    BVec v;
    BVec g;
    BVec up;
    BVec a;
    BVec b;
    BVec e1;
    BVec e2;
    DVec3 off;
    DVec3 off2;
    int cnt;
    int n;
    if (!show) {
        return;
    }
    if (!d) {
        return;
    }
    m = *(DMtx*)car;
    f32 oldx = d->fC8;
    f32 oldy = d->fCC;
    d->fC8 = lbl_8056305C;
    d->fCC = lbl_8056305C;
    switch (S32(car, 0x48)) {
    case -1:
        d->fC8 = lbl_80563058;
        d->fCC = lbl_8056306C;
        break;
    case 0:
        d->fC8 = oldx;
        d->fCC = lbl_8056305C;
        break;
    case 1:
        d->fC8 = lbl_8056306C;
        d->fCC = lbl_80563058;
        break;
    case 2:
        d->fC8 = lbl_8056306C;
        d->fCC = lbl_8056306C;
        break;
    case 3:
        d->fC8 = lbl_8056305C;
        d->fCC = lbl_80563058;
        break;
    case 4:
        d->fC8 = lbl_8056305C;
        d->fCC = lbl_8056306C;
        break;
    case 5:
        d->fC8 = lbl_80563058;
        d->fCC = lbl_80563058;
        break;
    case 6:
        d->fC8 = lbl_80563058;
        d->fCC = lbl_8056306C;
        break;
    }
    f32 ny = d->fCC;
    d->fCC = lbl_80563050 * ny + lbl_80563050 * oldy;
    if ((f32)__fabs(d->fCC) < lbl_80563074 || ny * oldy > lbl_8056305C) {
        if (S32(car, 0x48) == 0) {
            d->fC8 = lbl_80563078 * oldx;
        }
    } else {
        d->fC8 = oldx;
    }
    fn_801E353C(d->wheel, -lbl_8056307C * F32(car, 0x3C));
    fn_801E34E4(d->wheel, &m);
    d->body = m;
    MulVec(&off, &d->butt, &d->body);
    d->body.pos.x = d->body.pos.x + off.x;
    d->body.pos.y = d->body.pos.y + off.y;
    d->body.pos.z = d->body.pos.z + off.z;
    f32 roll = -F32(car, 0x238);
    roll = MaxF(MinF(lbl_80563080, roll), -lbl_80563080);
    v.z = roll;
    v.y = lbl_8056305C;
    v.x = F32(car, 0x240) + d->yaw;
    f32 ang = PSVECMag(&v.x);
    if (ang > lbl_80563070) {
        f32 inv = lbl_80563058 / ang;
        f32 ax = inv * v.x;
        f32 ay = inv * v.y;
        f32 az = inv * v.z;
        f32 sn = sin(ang);
        f32 cs = cos(ang);
        f32 t = lbl_80563058 - cs;
        rot.r[0].x = cs + t * (ax * ax);
        rot.r[0].y = ay * (t * ax) + sn * az;
        rot.r[0].z = az * (t * ax) - sn * ay;
        rot.r[1].x = ay * (t * ax) - sn * az;
        rot.r[1].y = cs + t * (ay * ay);
        rot.r[1].z = az * (t * ay) + sn * ax;
        rot.r[2].x = az * (t * ax) + sn * ay;
        rot.r[2].y = az * (t * ay) - sn * ax;
        rot.r[2].z = cs + t * (az * az);
    } else {
        SetIdentity(&rot.r[0].x);
    }
    MulM(&d->rest.row[0].x, &d->body.row[0].x, &rot.r[0].x);
    d->rest.pos = d->body.pos;
    fn_801E3284(&d->wheelMtx, d->wheel, &m);
    a.x = lbl_8056305C;
    a.y = lbl_8056305C;
    a.z = lbl_8056305C;
    b.x = lbl_8056305C;
    b.y = lbl_8056305C;
    b.z = lbl_8056305C;
    up.x = lbl_8056305C;
    up.y = lbl_80563058;
    up.z = lbl_8056305C;
    g.x = lbl_8056305C;
    g.y = lbl_80563084;
    g.z = lbl_8056305C;
    fn_801C2694(d->sys, pts, &cnt, &g, &up, &b, &a);
    n = 32;
    fn_801E5998(d->skel, bones, &n);
    for (int k = 0; k < n; k++) {
        if (k) {
            MulM(&pose[k].rot.r[0].x, &bones[k].m.row[0].x, &rot.r[0].x);
            MulVec((DVec3*)&pose[k].pos, &bones[k].m.pos, (DMtx*)&rot);
        } else {
            pose[k] = *(BonePose*)&bones[k];
        }
        pose[k].pos.x = pose[k].pos.x + d->butt.x;
        pose[k].pos.y = pose[k].pos.y + d->butt.y;
        pose[k].pos.z = pose[k].pos.z + d->butt.z;
    }
    f32 lean = lbl_80563094 * F32(car, 0x23C);
    lean = MaxF(MinF(lbl_80563090, lean), lbl_80563098);
    e1.y = F32(car, 0x3C) * lbl_80563090;
    e1.z = lbl_80563088 * -F32(car, 0x238) + F32(car, 0x3C) * lbl_8056308C;
    e1.x = lbl_8056309C * F32(car, 0x240) - lean;
    Mat3FromEuler((Mat3*)&tilt, (Vec3*)&e1);
    Mat3Mul((Mat3*)&pose[3].rot, (Mat3*)&tilt, (Mat3*)&pose[3].rot);
    links = lbl_802F9D00;
    fn_802009CC(pose, pts, links.l, 6, (u8*)bones, (BonePose*)&m);
    pose[9].pos.x = pose[9].pos.x + pose[8].rot.r[0].x * lbl_80563064;
    pose[9].pos.y = pose[9].pos.y + pose[8].rot.r[0].y * lbl_80563064;
    pose[9].pos.z = pose[9].pos.z + pose[8].rot.r[0].z * lbl_80563064;
    pose[6].pos.x = pose[6].pos.x + pose[5].rot.r[0].x * lbl_80563064;
    pose[6].pos.y = pose[6].pos.y + pose[5].rot.r[0].y * lbl_80563064;
    pose[6].pos.z = pose[6].pos.z + pose[5].rot.r[0].z * lbl_80563064;
    e2.x = d->yaw;
    e2.y = lbl_8056305C;
    e2.z = lbl_8056305C;
    Mat3FromEuler((Mat3*)&turn, (Vec3*)&e2);
    Mat3Mul((Mat3*)&pose[0].rot, (Mat3*)&turn, (Mat3*)&pose[0].rot);
    root = *(DVec3*)&pose[0].pos;
    for (int i = 0; i < n; i++) {
        pose[i].pos.x -= root.x;
        pose[i].pos.y -= root.y;
        pose[i].pos.z -= root.z;
    }
    s32 r = fn_801E65BC(d->skel, pose, n, 0);
    m2 = m;
    MulVec(&off2, &root, &m2);
    m2.pos.x = m2.pos.x + off2.x;
    m2.pos.y = m2.pos.y + off2.y;
    m2.pos.z = m2.pos.z + off2.z;
    fn_802C70AC(d->model, &m2, r, -1);
}

}  // extern "C"
