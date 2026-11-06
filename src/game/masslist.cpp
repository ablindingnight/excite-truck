// Car mass lists (0x801BD628-0x801BF214): per-car point-mass tables ("masslist") and the contact lists
// built from them each frame ("cslist", "cclist", "cxlist", "alist"), plus a 256-entry scratch table of
// 4-float records. Names are guesses apart
// from the strings.
#include "game/common.h"

// Helper name unproven: arguments evaluate b, a, t, which sets the load order of the expanded form.
static inline f32 LerpT(f32 t, f32 a, f32 b) { return a + t * (b - a); }

struct MVec3 {
    f32 x;
    f32 y;
    f32 z;
};

struct MVec4 {
    MVec4(void) {}
    MVec4(f32 x_, f32 y_, f32 z_) {  // w left unset
        x = x_;
        y = y_;
        z = z_;
    }
    MVec4(f32 x_, f32 y_, f32 z_, f32 w_) {
        x = x_;
        y = y_;
        z = z_;
        w = w_;
    }
    f32 x;
    f32 y;
    f32 z;
    f32 w;
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
    MVec3 p;
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

// A car setup block (carsetup.cpp CarSetup, 0x138 bytes): defaulted by fn_801BDA70, then applied to the car.
struct WheelSetup {
    f32 rideHeight;      // offset 0x0
    f32 spring;          // offset 0x4
    f32 bump;            // offset 0x8
    f32 rebound;         // offset 0xC
    f32 bumpstopHeight;  // offset 0x10
    f32 bumpstopRate;    // offset 0x14
    f32 unk18;           // offset 0x18
    f32 unk1C;           // offset 0x1C
    f32 unk20;           // offset 0x20
};

// carsetup.cpp's car setup block (0x138 bytes): defaulted by fn_801BDA70, then applied to the car.
struct CarSetup {
    f32 mass;             // offset 0x0
    f32 mx;               // offset 0x4
    f32 my;               // offset 0x8
    f32 mz;               // offset 0xC
    f32 ftrack;           // offset 0x10
    f32 rtrack;           // offset 0x14
    f32 wheelbase;        // offset 0x18
    f32 balance;          // offset 0x1C
    f32 collideCmHeight;  // offset 0x20
    f32 cmHeight;         // offset 0x24
    f32 airDrag;          // offset 0x28
    f32 torqueBalance;    // offset 0x2C
    u8 engine[0x48];      // offset 0x30 (gearing and engine; not read here)
    f32 fsway;            // offset 0x78
    f32 rsway;            // offset 0x7C
    WheelSetup wheels[4];  // offset 0x80
    f32 grip;             // offset 0x110
    f32 slip;             // offset 0x114
    f32 loadFactor;       // offset 0x118
    f32 unk11C;           // offset 0x11C
    f32 brake;            // offset 0x120
    f32 speedSteering;    // offset 0x124
    f32 wheelLock;        // offset 0x128
    f32 modelOffset[3];   // offset 0x12C
};

extern "C" {
void* memset(void* p, int c, size_t n);
double cos(double x);
double sin(double x);
double sqrt(double x);
void fn_801E9DC4(CxEntry* e);
void fn_801BDA70(CarSetup* out);
void fn_801BDA74(int car, CarSetup* out);
int fn_801E9E78(void* h, MVec3* p, MVec3* n);
void fn_80279D40(CarSetup* out);
void* memcpy(void* dst, const void* src, size_t n);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_801BA088(MVec4* t, int a, int b, s16 c);
void fn_80144B44(MVec3* v, f32 x, f32 y, f32 z);
void StatsUpdate__Fv(MVec3* v);  // vector default constructor (empty, out of line)
extern s32 lbl_8055A920;
MVec4* fn_801BF12C(MVec4* out, const MVec4* in);
MVec4* fn_801BF150(MVec4* out, f32 x, f32 y, f32 z, f32 w);
MVec4* fn_801BF164(MVec4* out, const MVec4* in);
MVec4* fn_801BF188(MVec4* out, f32 x, f32 y, f32 z);
void fn_801BF198(MVec3* out, const MVec3* in);
void fn_801BF0C4(MVec3* p, MVec3* n, void* h, s32* surface);
}

void* operator new[](size_t size, const char* name);
void operator delete[](void* p);

static inline void SetV(MVec3* o, f32 x, f32 y, f32 z) {
    o->x = x;
    o->y = y;
    o->z = z;
}

static inline void SetVyxz(MVec3* o, f32 x, f32 y, f32 z) {
    o->y = y;
    o->x = x;
    o->z = z;
}

extern "C" {

int fn_801BD628(const MVec3* v) {
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

void fn_801BDA70(CarSetup* out) { fn_80279D40(out); }

// Applies a car setup to car `car`: mass/inertia (and reciprocals) and the integrator's parameter rows - wheel anchors,
// suspension lanes per wheel, a two-point linear tyre force fit, toe angles, brake split, drive split.
#define DEG(d) ((d) * 3.141592653589793 / 180.0)
void fn_801BDA74(int car, CarSetup* s) {
    MVec4 mass(s->mx, s->my, s->mz, s->mass);
    MVec4 inv(1.0f / s->mx, 1.0f / s->my, 1.0f / s->mz, 1.0f / s->mass);
    CrossSet* x = &lbl_8055E79C[car];
    MassSet* m = &lbl_8055E794[car];
    int i;
    lbl_8055E7A0[car] = 0;
    m->mass = mass;
    m->invMass = inv;
    x->row[4] = MVec4(0.0440043993f, 0.0440043993f, 0.0440043993f, 0.0440043993f);
    x->row[5] = MVec4(0.189215347f, 0.189215347f, 0.189215347f, 0.189215347f);
    for (i = 0; i < 4; i++) {
        x->row[i] = MVec4(30.0f * 0.0254f * ((i & 1) ? 1.0f : -1.0f), -0.127f - ((i & 1) ? 0.0f : 0.0f),
                          54.0f * 0.0254f * ((i & 2) ? -1.0f : 1.0f));
    }
    f32 lock = sin(s->wheelLock);
    x->row[18] = MVec4(lock, lock, 0.0f, 0.0f);
    f32 a = DEG(-0.1);
    f32 b = DEG(0.1);
    x->row[19] = MVec4(a, -a, b, -b);
    x->row[20] = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
    x->row[21] = MVec4(0.0f, 0.0f, 0.5f, 0.5f);
    x->row[6] = MVec4(0.5f * s->airDrag * 1.29f, 0.0f, 0.0f, s->collideCmHeight - s->cmHeight);
    x->row[22] = MVec4(s->brake * s->unk11C, s->brake * s->unk11C, s->brake * (1.0f - s->unk11C),
                       s->brake * (1.0f - s->unk11C));
    {
        f32 g = 0.9f * s->grip;
        f32 a0 = 1.46f * g;
        f32 a1 = 1.25f * g;
        f32 x0 = 3559.99976f * s->loadFactor;
        f32 x1 = 8900.0f * s->loadFactor;
        f32 sl = 0.25f * s->slip;
        f32 b0 = 104856.312f * g * sl;
        f32 b1 = 161887.922f * g * sl;
        f32 kb = (b1 - b0) / (x1 - x0);
        f32 ka = (a1 - a0) / (x1 - x0);
        x->row[11] = x->row[10] = MVec4(a0 - ka * x0, ka, b0 - kb * x0, kb);
    }
    for (i = 0; i < 4; i++) {
        WheelSetup* w = &s->wheels[i];
        int front = (i & 2) == 0;
        int left = (i & 1) == 0;
        f32 share = front ? s->balance : 1.0f - s->balance;
        f32 load = 9.81f * (0.5f * share * s->mass);
        x->row[i] = MVec4(0.5f * (front ? s->ftrack : s->rtrack) * (left ? -1.0f : 1.0f),
                          w->unk18 - w->rideHeight - s->collideCmHeight - load / w->spring - load / w->unk1C,
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
    x->row[7] = MVec4(s->wheels[0].unk18, s->wheels[1].unk18, s->wheels[2].unk18, s->wheels[3].unk18);
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

void fn_801BE168(int car, const ValMatrix* mtx, const MVec3* vel, const MVec3* avel) {
    MassSet* s = &lbl_8055E794[car];
    MVec4 q;
    s->pos = MVec4(mtx->origin.x, mtx->origin.y, mtx->origin.z);
    MtxToQuat(mtx, &q);
    s->quat = MVec4(-q.x, -q.y, -q.z, q.w);
    s->vel = MVec4(vel->x, vel->y, vel->z);
    s->v60 = MVec4(avel->x, avel->y, avel->z);
}

static inline f32 Dot3(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f) { return e * f + (a * b + c * d); }

static inline void MulVec(MVec3* out, const MVec3* in, const ValMatrix* m) {
    const f32* r = &m->basis[0].x;
    if (out == in) {
        f32 x = Dot3(in->x, r[0], in->y, r[3], in->z, r[6]);
        f32 y = Dot3(in->x, r[1], in->y, r[4], in->z, r[7]);
        f32 z = Dot3(in->x, r[2], in->y, r[5], in->z, r[8]);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, r[0], in->y, r[3], in->z, r[6]);
        out->y = Dot3(in->x, r[1], in->y, r[4], in->z, r[7]);
        out->z = Dot3(in->x, r[2], in->y, r[5], in->z, r[8]);
    }
}

// NonMatching: 441/443; the target copies x->row[7] through two more temporaries (frame 32 bytes larger).
// Resets a car's body to rest at a matrix: position and orientation from it, velocity a along its z axis,
// contact points moved into place.
void fn_801BE400(int car, const ValMatrix* mtx, f32 a) {
    MassSet* s = &lbl_8055E794[car];
    ContactSet* c = &lbl_8055E798[car];
    CxEntry* e = &lbl_8055E7A4[car];
    CrossSet* x = &lbl_8055E79C[car];
    MVec4 q;
    s->pos = MVec4(mtx->origin.x, mtx->origin.y, mtx->origin.z);
    MtxToQuat(mtx, &q);
    s->quat = MVec4(-q.x, -q.y, -q.z, q.w);
    s->rot[0] = MVec4(mtx->basis[0].x, mtx->basis[0].y, mtx->basis[0].z);
    s->rot[1] = MVec4(mtx->basis[1].x, mtx->basis[1].y, mtx->basis[1].z);
    s->rot[2] = MVec4(mtx->basis[2].x, mtx->basis[2].y, mtx->basis[2].z);
    s->vA0 = MVec4(0.0f, 0.0f, 0.0f);
    s->v90 = MVec4(0.0f, 0.0f, 0.0f);
    MVec3 v;
    v.x = mtx->basis[2].x * a;
    v.y = mtx->basis[2].y * a;
    v.z = mtx->basis[2].z * a;
    s->vel = MVec4(v.x, v.y, v.z);
    s->v60 = MVec4(0.0f, 0.0f, 0.0f);
    c->v0 = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c->v20 = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
    MVec4 k = x->row[7];
    c->v10 = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c->v30 = MVec4(a / k.x, a / k.y, a / k.z, a / k.w);
    *(MVec4*)&c->pts[4] = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c->v190 = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c->v1A0 = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 4; i++) {
        *(MVec4*)&c->nrm[i] = MVec4(0.0f, 1.0f, 0.0f, -100.0f);
        *(MVec4*)&c->ref[i] = MVec4(0.0f, -100.0f, 0.0f);
        *(MVec4*)&c->pts[9 + i] = MVec4(0.0f, 0.0f, 0.0f);
        *(MVec4*)&c->pts[5 + i] = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
        MVec3 in;
        f32 xv = x->row[i].x;
        in.x = xv;
        in.y = x->row[i].y;
        in.z = x->row[i].z;
        MVec3 out;
        MulVec(&out, &in, mtx);
        out.x += mtx->origin.x;
        out.y += mtx->origin.y;
        out.z += mtx->origin.z;
        *(MVec4*)&c->pts[i] = MVec4(out.x, out.y, out.z);
    }
    memset(e, 0, sizeof(CxEntry));
    fn_801E9DC4(e);
    lbl_8055E7A8 = 1;
}

void fn_801BEAEC(int car, int i, MVec3* out) {
    const MVec3* p = &lbl_8055E798[car].pts[i].p;
    SetV(out, p->x, p->y, p->z);
}

void fn_801BEB1C(int car, ContactSet** out) { *out = &lbl_8055E798[car]; }

void fn_801BEB30(int car, CrossSet** out) { *out = &lbl_8055E79C[car]; }

// Post-update passes (placeholders).
// A vector with a field-by-field copy (copied through float registers).
struct CVec3 {
    CVec3(void) {}
    CVec3(const CVec3& o) {
        x = o.x;
        y = o.y;
        z = o.z;
    }
    f32 x, y, z;
};

f32 PSVECDotProduct(const CVec3* a, const CVec3* b);
void PSVECNormalize(const CVec3* src, CVec3* dst);

// Blends contact i's surface normal and anchor towards a new contact (point pt, normal n) when it lies close
// to the old plane: fully replaced from 0.2 below the plane, unchanged above it.
void fn_801BEB44(int car, int i, const CVec3* pt, const CVec3* n) {
    ContactSet* cs = &lbl_8055E798[car];
    CVec3 p(*(CVec3*)&cs->pts[i]);
    CVec3 ref(*(CVec3*)&cs->ref[i]);
    CVec3 nrm(*(CVec3*)&cs->nrm[i]);
    CVec3 d;
    d.x = p.x - ref.x;
    d.y = p.y - ref.y;
    d.z = p.z - ref.z;
    f32 base = PSVECDotProduct(&d, &nrm);
    CVec3 e;
    e.x = p.x - pt->x;
    e.y = p.y - pt->y;
    e.z = p.z - pt->z;
    f32 pen = PSVECDotProduct(&e, n) - base;
    if (pen > 0.2f) {
        return;
    }
    if (PSVECDotProduct(n, &nrm) < -0.2f) {
        return;
    }
    f32 k = 2.5f * (0.2f + pen);
    k = 1.0f < k ? 1.0f : k;
    k = k > 0.0f ? k : 0.0f;
    CVec3 nn;
    nn.z = LerpT(k, n->z, nrm.z);
    f32 yv = nrm.y;
    f32 xv;
    f32 y2 = n->y;
    nn.y = LerpT(k, y2, yv);
    xv = nrm.x;
    f32 x2 = n->x;
    nn.x = LerpT(k, x2, xv);
    nrm = nn;
    PSVECNormalize(&nrm, &nrm);
    CVec3 r;
    r.x = LerpT(k, pt->x, ref.x);
    r.y = LerpT(k, pt->y, ref.y);
    f32 zv = pt->z;
    r.z = LerpT(k, zv, ref.z);
    ref = r;
    *(MVec4*)&cs->ref[i] = MVec4(ref.x, ref.y, ref.z);
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
    MVec3 p, up, pt, n;
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
    StatsUpdate__Fv(&p);
    fn_801BF198(&p, (MVec3*)&m->pos);
    if (!fn_801BD628(&p)) {
        fn_802E30C8("CarPostUpdate: illegal mass position (%f %f %f)", p.x, p.y, p.z);
        fn_801BF164(&m->pos, fn_801BF188(&t0, 0.0f, 100.0f, 0.0f));
        fn_801BF12C(&m->quat, fn_801BF150(&t1, 0.0f, 0.0f, 0.0f, 1.0f));
        fn_801BF164(&m->vel, fn_801BF164(&m->v60, fn_801BF188(&t2, 0.0f, 0.0f, 0.0f)));
        fn_801BF12C(&cs->v0, fn_801BF12C(&cs->v10, fn_801BF12C(&cs->v20, fn_801BF12C(&cs->v30,
            fn_801BF150(&t3, 0.0f, 0.0f, 0.0f, 0.0f)))));
        fn_801BF198(&p, (MVec3*)&m->pos);
    }
    StatsUpdate__Fv(&up);
    fn_801BF198(&up, (MVec3*)&m->rot[2]);
    for (i = 0; i < 4; i++) {
        StatsUpdate__Fv(&pt);
        fn_801BF198(&pt, &cs->pts[i].p);
        surface = 0xFF;
        fn_80144B44(&n, 0.0f, 1.0f, 0.0f);
        fn_801BF0C4(&pt, &n, cx, &surface);
        cx->surface[i] = surface;
        fn_801BF12C((MVec4*)&cs->nrm[i], fn_801BF150(&t4, n.x, n.y, n.z, pt.y));
        fn_801BF164((MVec4*)&cs->ref[i], fn_801BF188(&t5, pt.x, pt.y, pt.z));
    }
}

void fn_801BEDD4(void) {
    for (int car = 0; car < lbl_8055E78C; car++) {
        PostUpdateCar(car);
    }
}

void fn_801BF0C4(MVec3* p, MVec3* n, void* h, s32* surface) {
    if (!fn_801E9E78(h, p, n)) {
        n->x = 0.0f;
        n->y = 1.0f;
        n->z = 0.0f;
        p->y = -100.0f;
    }
}

MVec4* fn_801BF12C(MVec4* out, const MVec4* in) {
    out->x = in->x;
    out->y = in->y;
    out->z = in->z;
    out->w = in->w;
    return out;
}

MVec4* fn_801BF150(MVec4* out, f32 x, f32 y, f32 z, f32 w) {
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

MVec4* fn_801BF188(MVec4* out, f32 x, f32 y, f32 z) {
    out->x = x;
    out->y = y;
    out->z = z;
    return out;
}

void fn_801BF198(MVec3* out, const MVec3* in) { SetVyxz(out, in->x, in->y, in->z); }

void fn_801BF1B4(void) {}

void fn_801BF1B8(int car, u8 on) { lbl_8055E7A0[car] = on; }

}  // extern "C"
