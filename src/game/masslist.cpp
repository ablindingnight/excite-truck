// Car mass lists (0x801BD628-0x801BF214): per-car point-mass tables ("masslist") and the contact lists
// built from them each frame ("cslist", "cclist", "cxlist", "alist"), plus a 256-entry scratch table of
// 4-float records. Names are guesses apart
// from the strings.
#include "game/common.h"
#include "game/wheelsetup.h"
#include "game/rvec.h"
#include <revolution/MTX/vec.h>
#include "game/vec3_init.h"



// MVec3: valvec3.h's ValVec3.
#include "game/mvec4.h"

// MLV3: rvec.h's RVec3.
struct MLF {
    MLF(f32 a) { v = a; }
    f32 v;
};

/* Helper names unproven (RVec3, MLF, MLVec4, Set): the solver-record 4-vector family (carbody CbVec4, minworld MWV4). */
struct MLVec4 {
    MLVec4(MVec4 s) {
        MLF f(s.w);
        Set(RVec3(s.x, s.y, s.z), f);
    }
    void Set(const RVec3& p, MLF w_) {
        x = p.x;
        y = p.y;
        z = p.z;
        w = w_.v;
    }
    f32 x, y, z, w;
};

struct MassSet {
    MassSet(void);
    MVec4 pos;      // offset 0x0
    MVec4 rot[3];   // offset 0x10 (rows)
    MVec4 quat;     // offset 0x40
    MVec4 vel;      // offset 0x50
    MVec4 v60;      // offset 0x60
    MVec4 mass;     // offset 0x70 (inertia x, y, z, mass)
    MVec4 invMass;  // offset 0x80 (reciprocals)
    MVec4 v90;      // offset 0x90
    MVec4 vA0;      // offset 0xA0
};

struct ContactPt {
    ValVec3 p;
    f32 w;
};

struct ContactSet {
    ContactSet(void);
    MVec4 v0;             // offset 0x0
    MVec4 v10;            // offset 0x10
    MVec4 v20;            // offset 0x20
    MVec4 v30;            // offset 0x30
    ContactPt pts[13];    // offset 0x40
    ContactPt nrm[4];     // offset 0x110, per-wheel contact plane normal
    ContactPt ref[4];     // offset 0x150, per-wheel contact plane anchor
    MVec4 v190;           // offset 0x190
    MVec4 v1A0;           // offset 0x1A0
    u8 pad1B0[0x40];
};

struct CrossSet {
    CrossSet(void);
    MVec4 row[25];   // offset 0x0, the integrator's per-car parameter rows (fn_801BA088 t[]): 0-3 wheel anchors
                     // (body space), 7-9 and 12-17 per-wheel suspension lanes, 10-11 tyre force fit, ...
};

struct CxEntry {
    u8 data[0x4];
    s32 surface[4];  // offset 0x4, per wheel
};

ModuleTag lbl_8055E780;
UnkStaticFFFF lbl_8055E788;
s32 lbl_8055E78C;
s32 lbl_8055E790;           // cars
MassSet* lbl_8055E794;      // per car
ContactSet* lbl_8055E798;   // per car
CrossSet* lbl_8055E79C;     // per car
u8* lbl_8055E7A0;           // per-car flags
CxEntry* lbl_8055E7A4;      // per car
u8 lbl_8055E7A8;
MVec4 lbl_80436D48[0x100];

#include "game/carsetup.h"
#include "game/mathutil.h"

extern "C" {
void* memset(void* p, int c, size_t n);
double cos(double x);
double sin(double x);
double sqrt(double x);
void fn_801E9DC4(CxEntry* e);
void fn_801BDA70(CarSetup* out);
void fn_801BDA74(int car, const CarSetup* out);
int fn_801E9E78(void* h, ValVec3* p, ValVec3* n, s32* surface);
void InitializeVehiclePhysicsParams(CarSetup* out);
void* memcpy(void* dst, const void* src, size_t n);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_801BA088(MVec4* t, int a, int b, s16 c);
void __ct__4VPosFfff(ValVec3* v, f32 x, f32 y, f32 z);
s32 lbl_8055A920 = -1;
MVec4* CopyVector4(MVec4* out, const MVec4* in);
MVec4* SetVector4Components(MVec4* out, f32 x, f32 y, f32 z, f32 w);
MVec4* fn_801BF164(MVec4* out, const MVec4* in);
MVec4* SetVector3Components(MVec4* out, f32 x, f32 y, f32 z);
void fn_801BF198(ValVec3* out, const ValVec3* in);
void fn_801BF0C4(ValVec3* p, ValVec3* n, void* h, s32* surface);
}

void* operator new[](size_t size, const char* name);
void operator delete[](void* p);



extern "C" {

int fn_801BD628(const ValVec3* v) {
    int r = 0;
    if (v->x >= -100000.0f && v->x <= 100000.0f && v->y >= -100000.0f && v->y <= 100000.0f &&
        v->z >= -100000.0f && v->z <= 100000.0f) {
        r = 1;
    }
    return r;
}

void fn_801BD694(int n) {
    lbl_8055E790 = n;
    lbl_8055E78C = 0;
    lbl_8055E794 = new ("masslist") MassSet[n];
    lbl_8055E798 = new ("cslist") ContactSet[lbl_8055E790];
    lbl_8055E79C = new ("cclist") CrossSet[lbl_8055E790];
    lbl_8055E7A4 = new ("cxlist") CxEntry[lbl_8055E790];
    lbl_8055E7A0 = new ("alist") u8[lbl_8055E790];
    lbl_8055E7A8 = 1;
}

}  // extern "C"

CrossSet::CrossSet(void) {}

ContactSet::ContactSet(void) {}

MassSet::MassSet(void) {}

extern "C" {

void fn_801BD790(void) {
    delete[] lbl_8055E794;
    delete[] lbl_8055E798;
    delete[] lbl_8055E79C;
    delete[] lbl_8055E7A4;
    delete[] lbl_8055E7A0;
    lbl_8055E794 = 0;
    lbl_8055E78C = 0;
    lbl_8055E790 = 0;
}

// Adds a car: clears its records and starts it at rest, upright, 2 units up.
void fn_801BD808(void) {
    CarSetup out;
    int car = lbl_8055E78C++;
    MassSet* m = &lbl_8055E794[car];
    ContactSet* c = &lbl_8055E798[car];
    CrossSet* x = &lbl_8055E79C[car];
    CxEntry* e = &lbl_8055E7A4[car];
    memset(m, 0, sizeof(MassSet));
    memset(c, 0, sizeof(ContactSet));
    memset(x, 0, sizeof(CrossSet));
    memset(e, 0, sizeof(CxEntry));
    fn_801E9DC4(e);
    m->pos = MVec4(0.0f, 2.0f, 0.0f);
    m->vel = MVec4(0.0f, 0.0f, 0.0f);
    m->v60 = MVec4(0.0f, 0.0f, 0.0001f);
    m->vA0 = MVec4(0.0f, 0.0f, 0.0f);
    f32 angle = 0.0f;
    m->quat = MVec4(0.0f, 0.0f, sin(0.5f * angle), cos(0.5f * angle));
    m->rot[0] = MVec4(1.0f, 0.0f, 0.0f);
    m->rot[1] = MVec4(0.0f, 1.0f, 0.0f);
    m->rot[2] = MVec4(0.0f, 0.0f, 1.0f);
    c->v190 = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c->v1A0 = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c->v30 = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
    fn_801BDA70(&out);
    fn_801BDA74(car, &out);
}

void fn_801BDA70(CarSetup* out) { InitializeVehiclePhysicsParams(out); }

// Applies a car setup to car `car`: mass/inertia (and reciprocals) and the integrator's parameter rows - wheel anchors,
// suspension lanes per wheel, a two-point linear tyre force fit, toe angles, brake split, drive split.
#define DEG(d) ((d) * 3.141592653589793 / 180.0)
// Name unproven.
static inline f32 WheelRestHeight(const WheelSetup& wheel, f32 cmHeight, f32 load) {
    return wheel.radius - wheel.rideHeight - cmHeight - load / wheel.spring - load / wheel.unk1C;
}

void fn_801BDA74(int car, const CarSetup* s) {
    MVec4 mass(s->mx, s->my, s->mz, s->mass);
    MVec4 inv(1.0f / s->mx, 1.0f / s->my, 1.0f / s->mz, 1.0f / s->mass);
    CrossSet* x = &lbl_8055E79C[car];
    MassSet* m = &lbl_8055E794[car];
    int i;
    __typeof__(x->row[5]) x_row_5 = MVec4(0.189215347f, 0.189215347f, 0.189215347f, 0.189215347f);
    lbl_8055E7A0[car] = 0;
    m->mass = mass;
    m->invMass = inv;
    x->row[4] = MVec4(0.0440043993f, 0.0440043993f, 0.0440043993f, 0.0440043993f);
    x->row[5] = x_row_5;
    for (i = 0; i < 4; i++) {
        x->row[i] = MVec4(30.0f * 0.0254f * ((i & 1) ? 1.0f : -1.0f), -0.127f - ((i & 1) ? 0.0f : 0.0f),
                          54.0f * 0.0254f * ((i & 2) ? -1.0f : 1.0f));
    }
    f32 lock = sin(s->wheelLock);
    x->row[18] = MVec4(lock, lock, 0.0f, 0.0f);
    f32 a = DEG(-0.1);
    f32 b = DEG(0.1);
    __typeof__(x->row[6]) x_row_6 = MVec4(0.5f * s->airDrag * 1.29f, 0.0f, 0.0f, s->collideCmHeight - s->cmHeight);
    x->row[19] = MVec4(a, -a, b, -b);
    x->row[20] = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
    x->row[21] = MVec4(0.0f, 0.0f, 0.5f, 0.5f);
    x->row[6] = x_row_6;
    x->row[22] = MVec4(s->brake * s->unk11C, s->brake * s->unk11C, s->brake * (1.0f - s->unk11C),
                       s->brake * (1.0f - s->unk11C));
    {
        f32 g = 0.9f * s->grip;
        f32 a1 = 1.25f * g;
        f32 x0 = 3559.99976f * s->loadFactor;
        f32 x1 = 8900.0f * s->loadFactor;
        f32 sl = 0.25f * s->slip;
        f32 ka = (a1 - (1.46f * g)) / (x1 - x0);
        f32 b1 = 161887.922f * g * sl;
        f32 b0 = 104856.312f * g * sl;
        f32 kb = (b1 - b0) / (x1 - x0);
        MVec4 tyre((1.46f * g) - ka * x0, ka, b0 - kb * x0, kb);
        x->row[10] = tyre;
        x->row[11] = tyre;
    }
    for (i = 0; i < 4; i++) {
        const WheelSetup* w = &s->wheels[i];
        int front = !(i & 2);
        int left = !(i & 1);
        f32 share = front ? s->balance : 1.0f - s->balance;
        f32 load = 9.81f * (0.5f * share * s->mass);
        x->row[i] = MVec4(0.5f * (front ? s->ftrack : s->rtrack) * (left ? -1.0f : 1.0f),
                          WheelRestHeight(*w, s->collideCmHeight, load),
                          (1.0f - share) * s->wheelbase * (front ? 1.0f : -1.0f));
    }
    x->row[12] = MVec4(s->wheels[0].spring, s->wheels[1].spring, s->wheels[2].spring, s->wheels[3].spring);
    x->row[13] = MVec4(s->wheels[0].bump, s->wheels[1].bump, s->wheels[2].bump, s->wheels[3].bump);
    x->row[14] = MVec4(s->wheels[0].rebound, s->wheels[1].rebound, s->wheels[2].rebound, s->wheels[3].rebound);
    x->row[15] = MVec4(s->fsway, 0.0f, s->rsway, 0.0f);
    x->row[16] = MVec4(s->wheels[0].bumpstopHeight, s->wheels[1].bumpstopHeight, s->wheels[2].bumpstopHeight,
                       s->wheels[3].bumpstopHeight);
    x->row[17] = MVec4(s->wheels[0].bumpstopRate, s->wheels[1].bumpstopRate, s->wheels[2].bumpstopRate,
                       s->wheels[3].bumpstopRate);
    x->row[8] = MVec4(s->wheels[0].unk1C, s->wheels[1].unk1C, s->wheels[2].unk1C, s->wheels[3].unk1C);
    x->row[9] = MVec4(s->wheels[0].unk20, s->wheels[1].unk20, s->wheels[2].unk20, s->wheels[3].unk20);
    x->row[7] = MVec4(s->wheels[0].radius, s->wheels[1].radius, s->wheels[2].radius, s->wheels[3].radius);
    x->row[21] = MVec4(0.5f * (1.0f - s->torqueBalance), 0.5f * (1.0f - s->torqueBalance), 0.5f * s->torqueBalance,
                       0.5f * s->torqueBalance);
    x->row[23] = MVec4(1.29607821f, 0.0f, 0.0f, 0.216013029f);
}

// Sets a car's body from a matrix (rows + origin) and its linear/angular velocities: the rotation is
// converted to a quaternion (Shoemake) and stored conjugated.
// Matrix -> quaternion (Shoemake); the same inline helpers as interlude's RotToQuat (a shared header originally).
static inline void MtxToQuatAxis(f32 (*m)[3], MVec4* q) {
    f32* v = &q->x;
    int i = 0;
    if (m[1][1] > m[0][0]) {
        i = 1;
    }
    if (m[2][2] > m[i][i]) {
        i = 2;
    }
    int j = (i + 1) % 3;
    int k = (j + 1) % 3;
    f32 s = sqrt(m[i][i] - (m[j][j] + m[k][k]) + 1.0f);
    v[i] = 0.5f * s;
    s = 0.5f / s;
    q->w = (m[j][k] - m[k][j]) * s;
    v[j] = (m[i][j] + m[j][i]) * s;
    v[k] = (m[i][k] + m[k][i]) * s;
}

static inline void MtxToQuat(const ValMatrix* r, MVec4* q) {
    const f32(*m)[3] = (const f32(*)[3])r;
    f32 tr = r->basis[0].x + r->basis[1].y + r->basis[2].z;
    if (tr > 0.0f) {
        f32 s = sqrt(tr + 1.0f);
        q->w = 0.5f * s;
        s = 0.5f / s;
        q->x = (m[1][2] - m[2][1]) * s;
        q->y = (m[2][0] - m[0][2]) * s;
        q->z = (m[0][1] - m[1][0]) * s;
    } else {
        MtxToQuatAxis((f32(*)[3])m, q);
    }
}

void UpdateCarPhysics(int car, const ValMatrix* mtx, const ValVec3* vel, const ValVec3* avel) {
    MassSet* s = &lbl_8055E794[car];
    MVec4 q;
    s->pos = MVec4(mtx->origin.x, mtx->origin.y, mtx->origin.z);
    MtxToQuat(mtx, &q);
    s->quat = MVec4(-q.x, -q.y, -q.z, q.w);
    s->vel = MVec4(vel->x, vel->y, vel->z);
    s->v60 = MVec4(avel->x, avel->y, avel->z);
}

static inline f32 Dot3Rev(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f) { return e * f + (a * b + c * d); }

static inline void MulVec(ValVec3* out, const ValVec3* in, const ValMatrix* m) {
    const f32* r = &m->basis[0].x;
    if (out == in) {
        f32 x = Dot3Rev(in->x, r[0], in->y, r[3], in->z, r[6]);
        f32 y = Dot3Rev(in->x, r[1], in->y, r[4], in->z, r[7]);
        f32 z = Dot3Rev(in->x, r[2], in->y, r[5], in->z, r[8]);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3Rev(in->x, r[0], in->y, r[3], in->z, r[6]);
        out->y = Dot3Rev(in->x, r[1], in->y, r[4], in->z, r[7]);
        out->z = Dot3Rev(in->x, r[2], in->y, r[5], in->z, r[8]);
    }
}

// Resets a car's body to rest at a matrix: position and orientation from it, velocity a along its z axis,
// contact points moved into place.
}
template <class O, class V> static inline void TScale(O* o, const V* v, f32 k) { o->x = Mul(v->x, k); o->y = Mul(v->y, k); o->z = Mul(v->z, k); }  /* Helper name unproven */
/* Helper name unproven */
template <class T>
static inline T& AtR(T* p, int i) {
    return p[i];
}
extern "C" {

/* Helper name unproven: a 4-vector from a 3-vector. Retail builds these five temporaries (three rotation rows, the
   velocity, each wheel point) at the bottom of the frame in call order = the local of one inline helper, while
   the MVec4(...) written in the function sit above them in statement order. */
template <class D> static inline void SetFromV3(D& d, const ValVec3& v) { d = MVec4(v.x, v.y, v.z); }
void fn_801BE400(int car, const ValMatrix* mtx, f32 a) {
    MassSet* s = &lbl_8055E794[car];
    ContactSet* c = &lbl_8055E798[car];
    CxEntry* e = &lbl_8055E7A4[car];
    CrossSet* x = &lbl_8055E79C[car];
    MVec4 q;
    s->pos = MVec4(mtx->origin.x, mtx->origin.y, mtx->origin.z);
    MtxToQuat(mtx, &q);
    s->quat = MVec4(-q.x, -q.y, -q.z, q.w);
    SetFromV3(AtR(s->rot, 0), mtx->basis[0]);
    SetFromV3(s->rot[1], mtx->basis[1]);
    SetFromV3(s->rot[2], mtx->basis[2]);
    s->vA0 = MVec4(0.0f, 0.0f, 0.0f);
    s->v90 = MVec4(0.0f, 0.0f, 0.0f);
    ValVec3 v;
    TScale(&v, &mtx->basis[2], a);
    SetFromV3(s->vel, v);
    s->v60 = MVec4(0.0f, 0.0f, 0.0f);
    MLVec4 k(x->row[7]);
    c->v0 = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c->v20 = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c->v10 = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
    f32 ix = a / k.x;
    f32 iy = a / k.y;
    f32 iz = a / k.z;
    f32 iw = a / k.w;
    c->v30 = MVec4(ix, iy, iz, iw);
    *(MVec4*)&c->pts[4] = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c->v190 = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c->v1A0 = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 4; i++) {
        // The record as 16-byte rows, walked per wheel (retail keeps one pointer c + 16*i): row 4 = pts[i],
        // 9 = pts[5 + i], 13 = pts[9 + i], 17 = nrm[i], 21 = ref[i].
        MVec4* q = (MVec4*)c + i;
        q[17] = MVec4(0.0f, 1.0f, 0.0f, -100.0f);
        q[21] = MVec4(0.0f, -100.0f, 0.0f);
        q[13] = MVec4(0.0f, 0.0f, 0.0f);
        q[9] = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
        ValVec3 in;
        in.Set(x->row[i].x, x->row[i].y, x->row[i].z);
        ValVec3 out;
        MulVec(&out, &in, mtx);
        out.x += mtx->origin.x;
        out.y += mtx->origin.y;
        out.z += mtx->origin.z;
        SetFromV3(q[4], out);
    }
    memset(e, 0, sizeof(CxEntry));
    fn_801E9DC4(e);
    lbl_8055E7A8 = 1;
}

void fn_801BEAEC(int car, int i, ValVec3* out) {
    const ValVec3* p = &lbl_8055E798[car].pts[i].p;
    out->Set(p->x, p->y, p->z);
}

void fn_801BEB1C(int car, ContactSet** out) { *out = &lbl_8055E798[car]; }

void fn_801BEB30(int car, CrossSet** out) { *out = &lbl_8055E79C[car]; }

// Post-update passes (placeholders).
// A vector with a field-by-field copy (copied through float registers).
// MassVec3: rvec.h's RVec3.

// Blends contact i's surface normal and anchor towards a new contact (point pt, normal n) when it lies close
// to the old plane: fully replaced from 0.2 below the plane, unchanged above it.
}
extern "C" {

void fn_801BEB44(int car, int i, const RVec3* pt, const RVec3* n) {
    ContactSet* cs = &lbl_8055E798[car];
    RVec3 p(cs->pts[i].p.x, cs->pts[i].p.y, cs->pts[i].p.z);
    RVec3 ref(((MVec4*)&cs->ref[i])->x, ((MVec4*)&cs->ref[i])->y, ((MVec4*)&cs->ref[i])->z);
    RVec3 nrm(cs->nrm[i].p.x, cs->nrm[i].p.y, cs->nrm[i].p.z);
    RVec3 d;
    d.x = p.x - ref.x;
    d.y = p.y - ref.y;
    d.z = p.z - ref.z;
    f32 base = PSVECDotProduct((const Vec*)&d, (const Vec*)&nrm);
    RVec3 e;
    e.x = p.x - pt->x;
    e.y = p.y - pt->y;
    e.z = p.z - pt->z;
    f32 pen = PSVECDotProduct((const Vec*)&e, (const Vec*)n) - base;
    if (pen > 0.2f || PSVECDotProduct((const Vec*)n, (const Vec*)&nrm) < -0.2f) {
        return;
    }
    f32 k = 2.5f * (0.2f + pen);
    k = 1.0f < k ? 1.0f : k;
    k = k > 0.0f ? k : 0.0f;
    RVec3 nn(LerpT(k, n->x, nrm.x), LerpT(k, n->y, nrm.y), LerpT(k, n->z, nrm.z));
    nrm = nn;
    PSVECNormalize((const Vec*)&nrm, (Vec*)&nrm);
    RVec3 r(LerpT(k, pt->x, ref.x), LerpT(k, pt->y, ref.y), LerpT(k, pt->z, ref.z));
    ref = r;
    {
        MVec4 t(ref.x, ref.y, ref.z);
        cs->ref[i].p.x = t.x;
        cs->ref[i].p.y = t.y;
        cs->ref[i].p.z = t.z;
        cs->ref[i].w = t.w;
    }
    *(MVec4*)&cs->nrm[i] = MVec4(nrm.x, nrm.y, nrm.z);
}

void fn_801BEDA8(MassSet** m, ContactSet** cs, CrossSet** cc, CxEntry** cx, s32* n) {
    *m = lbl_8055E794;
    *cs = lbl_8055E798;
    *cc = lbl_8055E79C;
    *cx = lbl_8055E7A4;
    *n = lbl_8055E78C;
}

// Car post-update for one car: runs the contact solver on a scratch copy of its tables, keeps the
// result unless the car is frozen, resets it if its mass position went out of range and refreshes the
// per-wheel ground contacts.
static inline void PostUpdateCar(int car) {
    int i;
    s32 surface;
    ValVec3 p, up, pt, n;
    MVec4 t0, t1, t2, t3, t4, t5;
    memcpy(&lbl_80436D48[0], &lbl_8055E794[car], sizeof(MassSet));
    memcpy(&lbl_80436D48[0xB], &lbl_8055E798[car], sizeof(ContactSet));
    memcpy(&lbl_80436D48[0x2A], &lbl_8055E79C[car], sizeof(CrossSet));
    lbl_8055A920 = car;
    fn_801BA088(lbl_80436D48, 0, 0xB, 0x2A);
    lbl_8055A920 = -1;
    if (!lbl_8055E7A0[car]) {
        memcpy(&lbl_8055E794[car], &lbl_80436D48[0], sizeof(MassSet));
        memcpy(&lbl_8055E798[car], &lbl_80436D48[0xB], sizeof(ContactSet));
    }
    CxEntry* cx = &lbl_8055E7A4[car];
    ContactSet* cs = &lbl_8055E798[car];
    MassSet* m = &lbl_8055E794[car];
    DefaultConstructVec3(&p);
    fn_801BF198(&p, (ValVec3*)&m->pos);
    if (!fn_801BD628(&p)) {
        fn_802E30C8("CarPostUpdate: illegal mass position (%f %f %f)", p.x, p.y, p.z);
        fn_801BF164(&m->pos, SetVector3Components(&t0, 0.0f, 100.0f, 0.0f));
        CopyVector4(&m->quat, SetVector4Components(&t1, 0.0f, 0.0f, 0.0f, 1.0f));
        fn_801BF164(&m->vel, fn_801BF164(&m->v60, SetVector3Components(&t2, 0.0f, 0.0f, 0.0f)));
        CopyVector4(&cs->v0, CopyVector4(&cs->v10, CopyVector4(&cs->v20, CopyVector4(&cs->v30,
            SetVector4Components(&t3, 0.0f, 0.0f, 0.0f, 0.0f)))));
        fn_801BF198(&p, (ValVec3*)&m->pos);
    }
    DefaultConstructVec3(&up);
    fn_801BF198(&up, (ValVec3*)&m->rot[2]);
    for (i = 0; i < 4; i++) {
        DefaultConstructVec3(&pt);
        fn_801BF198(&pt, &cs->pts[i].p);
        surface = 0xFF;
        __ct__4VPosFfff(&n, 0.0f, 1.0f, 0.0f);
        fn_801BF0C4(&pt, &n, cx, &surface);
        cx->surface[i] = surface;
        CopyVector4((MVec4*)&cs->nrm[i], SetVector4Components(&t4, n.x, n.y, n.z, pt.y));
        fn_801BF164((MVec4*)&cs->ref[i], SetVector3Components(&t5, pt.x, pt.y, pt.z));
    }
}

void fn_801BEDD4(void) {
    for (int car = 0; car < lbl_8055E78C; car++) {
        PostUpdateCar(car);
    }
}

void fn_801BF0C4(ValVec3* p, ValVec3* n, void* h, s32* surface) {
    if (!fn_801E9E78(h, p, n, surface)) {
        n->x = 0.0f;
        n->y = 1.0f;
        n->z = 0.0f;
        p->y = -100.0f;
    }
}

MVec4* CopyVector4(MVec4* out, const MVec4* in) {
    out->x = in->x;
    out->y = in->y;
    out->z = in->z;
    out->w = in->w;
    return out;
}

MVec4* SetVector4Components(MVec4* out, f32 x, f32 y, f32 z, f32 w) {
    out->x = x;
    out->y = y;
    out->z = z;
    out->w = w;
    return out;
}

MVec4* fn_801BF164(MVec4* out, const MVec4* in) {
    out->x = in->x;
    out->y = in->y;
    out->z = in->z;
    out->w = in->w;
    return out;
}

MVec4* SetVector3Components(MVec4* out, f32 x, f32 y, f32 z) {
    out->x = x;
    out->y = y;
    out->z = z;
    return out;
}

void fn_801BF198(ValVec3* out, const ValVec3* in) { out->Set(in->x, in->y, in->z); }

void fn_801BF1B4(void) {}

void fn_801BF1B8(int car, u8 on) { lbl_8055E7A0[car] = on; }

}  // extern "C"
