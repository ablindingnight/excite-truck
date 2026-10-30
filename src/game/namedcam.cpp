// Named cameras (0x80276068-0x80278214): the named camera sets ("namedcam", "rddefault", "raceday",
// "house") and the timed camera transitions ("physcam: Clock looped during camera transition!!").
// Written partly against raw field offsets (the camera record layout is in game/camrec.h). Names are
// guesses apart from the strings.
#include "game/common.h"
#include <string.h>
#include "game/camrec.h"

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

// A named camera setup ("namedcam" entries in the camera file).
struct NamedCam {
    NamedCam(void) {}
    u8 valid;       // offset 0x0
    u8 pad1[0x3];
    CamPose pose;   // offset 0x4
    f32 fov;        // offset 0x34
    char name[16];  // offset 0x38
};

// Camera definition reader (only the slot used here is named).
class CamReader {
public:
    virtual const char* Tag(void);  // 0x08
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual const char* GetString(const char* key, const char* def);  // 0x18
    virtual void v1C(void);
    virtual void v20(void);
    virtual f32 GetFloat(const char* key);  // 0x24
};

extern "C" {
int strcasecmp(const char* a, const char* b);
char* strncpy(char* d, const char* s, u32 n);
void* memcpy(void* d, const void* s, u32 n);
void fn_8026A4A0(int i, f32* pose, f32* fov);
}

// A timed camera transition slot (0x50).
// A timed camera transition: the start and end rotations as quaternions (x, y, z, w), positions and
// fields of view, the duration and the start time.
struct CamShot {
    CamShot(void) {}
    u8 active;       // offset 0x0
    u8 pad1[3];
    f32 q0[4];       // offset 0x4
    f32 q1[4];       // offset 0x14
    CamVec3 p0;      // offset 0x24
    CamVec3 p1;      // offset 0x30
    f32 fov0;        // offset 0x3C
    f32 fov1;        // offset 0x40
    f32 dur;         // offset 0x44
    f32 start;       // offset 0x48
    s32 kind;        // offset 0x4C
};


// A 3x4 frame; its empty constructor lives in modeinl.cpp (fn_80144DC8).
struct CamFrame34 {
    CamFrame34(void);
    f32 m[12];
};

ModuleTag lbl_8055F590;
static CamShot lbl_8049FD58[4];
static NamedCam lbl_8049FE98;
static NamedCam lbl_8049FEE0[48];
static CamRec lbl_804A0C60[4];
static CamFrame34 lbl_804A1500[4];

extern f32 lbl_8055E3C8;
extern f32 lbl_80558DC4[];  // FLT_EPSILON
extern NamedCam* lbl_8055F598;
extern NamedCam* lbl_8055F59C;
extern s32 lbl_8055F5A0;

extern "C" {

// Mode set as a bit mask over modes 4..23 (the original tests a mask, not a switch).
#define CAM_MODE_BIT(m) (1 << ((m) - 4))

int fn_80276068(int mode) {
    int r = 0;
    u32 i = mode - 4;
    if (i <= 19 && ((1 << i) & (CAM_MODE_BIT(4) | CAM_MODE_BIT(5) | CAM_MODE_BIT(6) | CAM_MODE_BIT(9) | CAM_MODE_BIT(23)))) {
        r = 1;
    }
    return r;
}

extern "C" f32 PSVECSquareDistance(const void* a, const void* b);
extern "C" void fn_8026A49C(int slot, CamPose* pose, f32* fov);
extern "C" void fn_8012F57C(f32* out, const f32* a, const f32* b, f32 t);  // slerp
extern "C" void fn_802E30C8(const char* fmt, ...);  // debug print
extern "C" f32 fn_80277704(int i);
extern "C" double sqrt(double x);

static inline f32 AbsF(f32 x) { return (f32)__fabs(x); }

static inline int Near(f32 x) { return AbsF(x) <= lbl_80558DC4[0]; }

static inline int SamePose(CamPose* a, CamPose* b) {
    int same = Near(PSVECSquareDistance(&a->pos, &b->pos));
    if (same) {
        same = Near(PSVECSquareDistance(&a->rot.m[0], &b->rot.m[0]));
    }
    if (same) {
        same = Near(PSVECSquareDistance(&a->rot.m[3], &b->rot.m[3]));
    }
    if (same) {
        same = Near(PSVECSquareDistance(&a->rot.m[6], &b->rot.m[6]));
    }
    return same;
}

// The rotation of a pose as a quaternion (x, y, z, w).
static inline void PoseToQuat(const CamPose* p, f32* q) {
    const f32(*m)[3] = (const f32(*)[3])p->rot.m;
    f32 tr = m[0][0] + m[1][1] + m[2][2];
    if (tr > 0.0f) {
        f32 s = sqrt(1.0f + tr);
        q[3] = 0.5f * s;
        s = 0.5f / s;
        q[0] = s * (m[1][2] - m[2][1]);
        q[1] = s * (m[2][0] - m[0][2]);
        q[2] = s * (m[0][1] - m[1][0]);
    } else {
        int i = 0;
        if (m[1][1] > m[0][0]) {
            i = 1;
        }
        if (m[2][2] > m[i][i]) {
            i = 2;
        }
        int j = (i + 1) % 3;
        int k = (j + 1) % 3;
        f32 s = sqrt(1.0f + (m[i][i] - (m[j][j] + m[k][k])));
        q[i] = 0.5f * s;
        s = 0.5f / s;
        q[3] = s * (m[j][k] - m[k][j]);
        q[j] = s * (m[i][j] + m[j][i]);
        q[k] = s * (m[i][k] + m[k][i]);
    }
}

// Starts transition `slot` from pose a to pose b over `dur` (nothing to do when they already match).
void fn_80276098(CamPose* a, CamPose* b, int slot, int kind, f32 fovA, f32 fovB, f32 dur) {
    if (AbsF(fovA - fovB) <= lbl_80558DC4[0] && SamePose(a, b)) {
        return;
    }
    CamShot* s = &lbl_8049FD58[slot];
    s->dur = dur;
    s->start = lbl_8055E3C8;
    f32 q[4];
    PoseToQuat(a, q);
    s->q0[0] = q[0];
    s->q0[1] = q[1];
    s->q0[2] = q[2];
    s->q0[3] = q[3];
    s->p0.v[0] = a->pos.v[0];
    s->p0.v[1] = a->pos.v[1];
    s->p0.v[2] = a->pos.v[2];
    s->fov0 = fovA;
    PoseToQuat(b, q);
    s->q1[0] = q[0];
    s->q1[1] = q[1];
    s->q1[2] = q[2];
    s->q1[3] = q[3];
    s->p1.v[0] = b->pos.v[0];
    s->p1.v[1] = b->pos.v[1];
    s->p1.v[2] = b->pos.v[2];
    s->fov1 = fovB;
    s->kind = kind;
    s->active = 1;
}

// The rotation of a unit quaternion (x, y, z, w).
static inline void QuatToPose(CamPose* p, const f32* q) {
    f32(*m)[3] = (f32(*)[3])p->rot.m;
    f32 x2 = q[0] + q[0];
    f32 y2 = q[1] + q[1];
    f32 z2 = q[2] + q[2];
    f32 xx = q[0] * x2;
    f32 xy = q[0] * y2;
    f32 xz = q[0] * z2;
    f32 yy = q[1] * y2;
    f32 yz = q[1] * z2;
    f32 zz = q[2] * z2;
    f32 wx = q[3] * x2;
    f32 wy = q[3] * y2;
    f32 wz = q[3] * z2;
    m[0][0] = 1.0f - (yy + zz);
    m[1][0] = xy - wz;
    m[2][0] = xz + wy;
    m[0][1] = xy + wz;
    m[1][1] = 1.0f - (xx + zz);
    m[2][1] = yz - wx;
    m[0][2] = xz - wy;
    m[1][2] = yz + wx;
    m[2][2] = 1.0f - (xx + yy);
}

// Smoothstep on [0, 1], mirrored for negative t and clamped to [-1, 1].
static inline f32 Ease(f32 t) {
    f32 e;
    if (t < -1.0f) {
        e = -1.0f;
    } else if (t > 1.0f) {
        e = 1.0f;
    } else if (t > 0.0f) {
        e = t * (-2.0f * t * t) + 3.0f * t * t;
    } else {
        e = t * (-2.0f * t * t) - 3.0f * t * t;
    }
    return e;
}

// The current pose and field of view of transition `slot`; follows a moving target unless the transition
// is to a fixed camera (kind 0x22), and ends the transition once it is complete.
void fn_8027656C(CamPose* out, f32* fov, int slot) {
    f32 t = fn_80277704(slot);
    if (lbl_8049FD58[slot].kind != 0x22) {
        CamPose p;
        f32 f;
        fn_8026A49C(slot, &p, &f);
        PoseToQuat(&p, lbl_8049FD58[slot].q1);
        lbl_8049FD58[slot].p1.v[0] = p.pos.v[0];
        lbl_8049FD58[slot].p1.v[1] = p.pos.v[1];
        lbl_8049FD58[slot].p1.v[2] = p.pos.v[2];
        lbl_8049FD58[slot].fov1 = f;
    }
    if (t >= 1.0f || t < 0.0f) {
        if (t < 0.0f) {
            fn_802E30C8("physcam: Clock looped during camera transition!!!");
        }
        CamShot* s = &lbl_8049FD58[slot];
        QuatToPose(out, s->q1);
        out->pos.v[0] = s->p1.v[0];
        out->pos.v[1] = s->p1.v[1];
        out->pos.v[2] = s->p1.v[2];
        *fov = s->fov1;
        lbl_8049FD58[slot].active = 0;
        return;
    }
    f32 e = Ease(t);
    f32 q[4];
    fn_8012F57C(q, lbl_8049FD58[slot].q0, lbl_8049FD58[slot].q1, e);
    CamShot* s = &lbl_8049FD58[slot];
    QuatToPose(out, q);
    CamVec3 v;
    v.v[2] = s->p0.v[2] + e * (s->p1.v[2] - s->p0.v[2]);
    v.v[1] = s->p0.v[1] + e * (s->p1.v[1] - s->p0.v[1]);
    v.v[0] = s->p0.v[0] + e * (s->p1.v[0] - s->p0.v[0]);
    out->pos = v;
    *fov = s->fov0 + e * (s->fov1 - s->fov0);
}

NamedCam* fn_80276A44(const char* name) {
    NamedCam* c = lbl_8049FEE0;
    for (int i = 0; i < lbl_8055F5A0; i++) {
        if (!strcasecmp(c->name, name)) {
            return &lbl_8049FEE0[i];
        }
        c++;
    }
    return 0;
}

void fn_80277F84(NamedCam* c, CamReader* r, int n);
extern const char* lbl_8055C638;

// Reads one named camera and remembers the special "house" and default ones.
void fn_80276ACC(CamReader* r, const char* def) {
    const char* name = r->GetString("name", def);
    NamedCam* c = &lbl_8049FEE0[lbl_8055F5A0++];
    fn_80277F84(c, r, lbl_8055F5A0);
    strncpy(c->name, name, 0x10);
    if (!strcasecmp(name, "house")) {
        lbl_8055F59C = c;
    } else if (!strcasecmp(name, lbl_8055C638)) {
        lbl_8055F598 = c;
    }
}

void fn_80276B8C(void) {
    u8* e = ((u8*)lbl_8049FD58);
    for (int i = 0; i < 4; i++) {
        memset(e, 0, 0x50);
        e += 0x50;
    }
}

void fn_80276BE4(CamPose* out, f32* fov) {
    if (((u8*)lbl_8049FD58)[0]) {
        fn_8027656C(&lbl_8049FE98.pose, &lbl_8049FE98.fov, 0);
    }
    out->rot = lbl_8049FE98.pose.rot;
    out->pos = lbl_8049FE98.pose.pos;
    *fov = lbl_8049FE98.fov;
}

void fn_80276CB0(const char* name, int blend) {
    NamedCam* c = fn_80276A44(name);
    if (!c) {
        c = lbl_8055F598;
    }
    if (blend) {
        fn_80276098(&lbl_8049FE98.pose, &c->pose, 0, 0x22, lbl_8049FE98.fov, c->fov, 1.0f);
    } else {
        memcpy(&lbl_8049FE98, c, 0x48);
        ((u8*)lbl_8049FD58)[0] = 0;
    }
}

void fn_80276D3C(const char* name, int mode) {
    NamedCam* c = fn_80276A44(name);
    if (mode == 1) {
        f32 pose[12];
        f32 fov;
        fn_8026A4A0(0, pose, &fov);
        fn_80276098((CamPose*)pose, &c->pose, 0, 0x22, fov, c->fov, 2.0f);
    } else {
        memcpy(&lbl_8049FE98, c, 0x48);
        ((u8*)lbl_8049FD58)[0] = 0;
    }
}

}  // extern "C"

struct CamFrame;
struct Mat3 {
    f32 m[9];
};
struct Vec3 {
    f32 x, y, z;
};
void GetTipsCamera(CamFrame* out, f32* fov);
void Mat3FromEuler(Mat3* out, const Vec3* euler);
void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);

extern "C" {
u8* fn_8017960C(int car);
void fn_8026A51C(CamRec* c, u8* own, int idx);
int fn_801B89F8(void);
f32 fn_801EA0E8(f32 x, f32 z);  // ground height
int fn_80178CA4(void);
f32 PSVECMag(const void* v);
void PSVECCrossProduct(const void* a, const void* b, void* out);
void PSVECNormalize(const void* in, void* out);
u8* fn_802678E4(int slot);
f32 fn_802704E0(int mode, f32 t);
void fn_801E9DC4(s16* h);
void fn_801C7254(Mat3* m);  // transpose
void fn_801BFD34(CamVec3* out, const CamVec3* v, const CamPose* m);
void fn_80144CD0(CamVec3* out, const CamVec3* p, const CamPose* m);
void fn_8026E688(CamPose* out, const CamVec3* eye, const CamVec3* target, const CamVec3* up);
void fn_8026E22C(CamPose* out, const CamPose* a, const CamPose* b, f32 t);
double atan2(double y, double x);
double atan(double x);
double tan(double x);
double cos(double x);
double sin(double x);

f32 lbl_804A15C0[4];  // smoothed pitch per slot

static inline f32 MinF(f32 a, f32 b) { return b < a ? b : a; }
static inline f32 MaxF(f32 a, f32 b) { return b > a ? b : a; }
static inline f32 Atan2F(f32 y, f32 x) { return atan2(y, x); }
static inline f32 CosF(f32 x) { return cos(x); }
static inline f32 SinF(f32 x) { return sin(x); }

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void MulVec(CamVec3* out, const CamVec3* in, const CamPose* m) {
    if (out == in) {
        f32 x = Dot3(in->v[0], m->rot.m[0], in->v[1], m->rot.m[3], in->v[2], m->rot.m[6]);
        f32 y = Dot3(in->v[0], m->rot.m[1], in->v[1], m->rot.m[4], in->v[2], m->rot.m[7]);
        f32 z = Dot3(in->v[0], m->rot.m[2], in->v[1], m->rot.m[5], in->v[2], m->rot.m[8]);
        out->v[0] = x;
        out->v[1] = y;
        out->v[2] = z;
    } else {
        out->v[0] = Dot3(in->v[0], m->rot.m[0], in->v[1], m->rot.m[3], in->v[2], m->rot.m[6]);
        out->v[1] = Dot3(in->v[0], m->rot.m[1], in->v[1], m->rot.m[4], in->v[2], m->rot.m[7]);
        out->v[2] = Dot3(in->v[0], m->rot.m[2], in->v[1], m->rot.m[5], in->v[2], m->rot.m[8]);
    }
}

// Where x lies between a and b (0 at a, 1 at b).
static inline f32 Remap(f32 x, f32 a, f32 b) { return (x - a) / (b - a); }

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

// A camera placed relative to the car: position, rotation (axis times angle, in degrees) and field of
// view (degrees).
struct CamOffset {
    CamVec3 pos;
    CamVec3 rot;
    f32 fov;
};

extern const CamOffset lbl_802FA818;

// Rotation by |axis| radians about axis.
static inline void AxisAngle(CamMat3* m, const CamVec3* axis) {
    f32 angle = PSVECMag(axis);
    if (angle > 0.0001f) {
        f32 inv = 1.0f / angle;
        f32 x = inv * axis->v[0];
        f32 y = inv * axis->v[1];
        f32 z = inv * axis->v[2];
        f32 s = SinF(angle);
        f32 c = CosF(angle);
        f32 t = 1.0f - c;
        m->m[0] = c + t * (x * x);
        m->m[1] = t * x * y + s * z;
        m->m[2] = t * x * z - s * y;
        m->m[3] = t * x * y - s * z;
        m->m[4] = c + t * (y * y);
        m->m[5] = t * y * z + s * x;
        m->m[6] = t * x * z + s * y;
        m->m[7] = t * y * z - s * x;
        m->m[8] = c + t * (z * z);
    } else {
        m->m[1] = 0.0f;
        m->m[0] = 1.0f;
        m->m[2] = 0.0f;
        m->m[3] = 0.0f;
        m->m[4] = 1.0f;
        m->m[5] = 0.0f;
        m->m[6] = 0.0f;
        m->m[7] = 0.0f;
        m->m[8] = 1.0f;
    }
}

// Takes over player `slot`'s camera and starts a transition from it to the fixed camera beside the car
// (lbl_802FA818), keeping the car-relative frame in lbl_804A1500.
void fn_80276DC8(int slot) {
    CamPose look;
    CamPose car;
    CamPose rel;
    CamMat3 inv;
    CamOffset o;
    CamVec3 axis;
    CamRec* src = (CamRec*)fn_802678E4(slot);
    lbl_804A0C60[slot] = *src;
    CamRec* rec = &lbl_804A0C60[slot];
    lbl_804A15C0[slot] = 0.0f;
    look = rec->pose;
    if (!fn_80276068(rec->mode) || !S32(src->car, 0x24)) {
        lbl_804A0C60[slot].mode = 4;
        f32 d = fn_802704E0(4, 0.0f);
        CamPose* f = &lbl_804A0C60[slot].carFrame;
        look.pos.v[0] = f->pos.v[0] + f->rot.m[6] * d;
        look.pos.v[1] = f->pos.v[1] + f->rot.m[7] * d;
        look.pos.v[2] = f->pos.v[2] + f->rot.m[8] * d;
        look.rot.m[6] = f->rot.m[6];
        look.rot.m[7] = f->rot.m[7];
        look.rot.m[8] = f->rot.m[8];
        look.rot.m[3] = 0.0f;
        look.rot.m[4] = 1.0f;
        look.rot.m[5] = 0.0f;
        PSVECCrossProduct(&look.rot.m[3], &look.rot.m[6], &look.rot.m[0]);
        PSVECNormalize(&look.rot.m[0], &look.rot.m[0]);
        PSVECCrossProduct(&look.rot.m[6], &look.rot.m[0], &look.rot.m[3]);
        PSVECNormalize(&look.rot.m[3], &look.rot.m[3]);
    }
    car = lbl_804A0C60[slot].carFrame;
    car.rot.m[3] = 0.0f;
    car.rot.m[4] = 1.0f;
    car.rot.m[5] = 0.0f;
    if (1.0f - car.rot.m[7] < 0.15f) {
        car.rot.m[6] = 0.0f;
        car.rot.m[7] = 0.0f;
        car.rot.m[8] = 1.0f;
    }
    PSVECCrossProduct(&car.rot.m[3], &car.rot.m[6], &car.rot.m[0]);
    PSVECNormalize(&car.rot.m[0], &car.rot.m[0]);
    PSVECCrossProduct(&car.rot.m[0], &car.rot.m[3], &car.rot.m[6]);
    PSVECNormalize(&car.rot.m[6], &car.rot.m[6]);
    fn_801E9DC4(&rec->h1EC);
    o = lbl_802FA818;
    rel.pos = o.pos;
    axis.v[0] = 0.017453292f * o.rot.v[0];
    axis.v[1] = 0.017453292f * o.rot.v[1];
    axis.v[2] = 0.017453292f * o.rot.v[2];
    f32 scale = F32(src->car, 0x5E4);
    rel.pos.v[0] *= scale;
    rel.pos.v[1] *= scale;
    rel.pos.v[2] *= scale;
    AxisAngle(&rel.rot, &axis);
    f32* frame = lbl_804A1500[slot].m;
    CamVec3* framePos = (CamVec3*)(frame + 9);
    fn_801BFD34(framePos, &rel.pos, &car);
    Mat3Mul((Mat3*)frame, (Mat3*)&rel.rot, (Mat3*)&car.rot);
    inv = look.rot;
    fn_801C7254((Mat3*)&inv);
    Mat3Mul((Mat3*)frame, (Mat3*)frame, (Mat3*)&inv);
    fn_80144CD0(framePos, framePos, &look);
    fn_80276098(&src->pose, &rel, slot, 0x22, src->fov, 0.017453292f * o.fov, 2.0f);
}

extern const f32 lbl_80564054;  // -1.0

f32 fn_80277704(int i) {
    if (lbl_8049FD58[i].active) {
        return (lbl_8055E3C8 - lbl_8049FD58[i].start) / lbl_8049FD58[i].dur;
    }
    return lbl_80564054;
}

void fn_80277740(void) {
    lbl_8055F5A0 = 0;
    lbl_8055F598 = 0;
    lbl_8055F59C = 0;
}

// Handles one camera-file block; returns whether the tag was recognised.
int fn_80277754(CamReader* r) {
    if (!strcasecmp(r->Tag(), "raceday")) {
        fn_80276ACC(r, lbl_8055C638);
    } else if (!strcasecmp(r->Tag(), "house")) {
        fn_80276ACC(r, "house");
    } else if (!strcasecmp(r->Tag(), "namedcam")) {
        fn_80276ACC(r, "def_name");
    } else {
        return 0;
    }
    return 1;
}

extern "C" int sprintf(char* buf, const char* fmt, ...);
extern "C" char* strcpy(char* d, const char* s);
extern const f32 lbl_80564048;  // 0.0
extern const f32 lbl_8056404C;  // 1.0
extern const f32 lbl_80564070;  // default fov, pi/2
extern const f32 lbl_80564074;  // house fov, 85 degrees

// Identity pose at the origin with the default fov.
static inline void ResetCam(NamedCam* c) {
    c->pose.rot.m[0] = lbl_8056404C;
    c->pose.rot.m[1] = lbl_80564048;
    c->pose.rot.m[2] = lbl_80564048;
    c->pose.rot.m[3] = lbl_80564048;
    c->pose.rot.m[4] = lbl_8056404C;
    c->pose.rot.m[5] = lbl_80564048;
    c->pose.rot.m[6] = lbl_80564048;
    c->pose.rot.m[7] = lbl_80564048;
    c->pose.rot.m[8] = lbl_8056404C;
    c->pose.pos.v[0] = lbl_80564048;
    c->pose.pos.v[1] = lbl_80564048;
    c->pose.pos.v[2] = lbl_80564048;
    c->fov = lbl_80564070;
    c->name[0] = 0;
}

// Makes sure the default and "house" cameras exist.
void fn_80277828(void) {
    NamedCam* c;
    ResetCam(&lbl_8049FE98);
    lbl_8049FE98.valid = 1;
    if (!lbl_8055F598) {
        int n = lbl_8055F5A0;
        c = &lbl_8049FEE0[n];
        ResetCam(c);
        lbl_8055F5A0 = n + 1;
        lbl_8055F598 = c;
        c->valid = 1;
        sprintf(c->name, lbl_8055C638, c, n);
    }
    c = lbl_8055F59C;
    if (!c) {
        int n = lbl_8055F5A0;
        c = &lbl_8049FEE0[n];
        lbl_8055F59C = c;
        lbl_8055F5A0 = n + 1;
        c->valid = 1;
    }
    c->fov = lbl_80564074;
    strcpy(lbl_8055F59C->name, "house");
}

int fn_80277960(const char* name, CamPose* out, f32* fov) {
    NamedCam* c = fn_80276A44(name);
    if (c) {
        out->rot = c->pose.rot;
        out->pos = c->pose.pos;
        *fov = c->fov;
        return 1;
    }
    return 0;
}

// Camera for slot `slot` in the chase modes: behind and above the car, swinging round with the
// transition and pitched by the car's climb, blended from the slot's camera; the tips camera once the
// race is over.
void fn_80277A14(CamRec* c, int slot) {
    CamPose blend;
    CamPose frame;
    Mat3 swingRot;
    Mat3 tiltRot;
    Vec3 swing;
    Vec3 tilt;
    CamVec3 back;
    CamVec3 up;
    CamVec3 offset;
    u8 own;
    lbl_804A0C60[slot].target = lbl_804A0C60[slot].nextTarget;
    lbl_804A0C60[slot].mode = lbl_804A0C60[slot].nextMode;
    lbl_804A0C60[slot].car = fn_8017960C(lbl_804A0C60[slot].target);
    own = 0;
    fn_8026A51C(&lbl_804A0C60[slot], &own, slot);
    if (fn_801B89F8()) {
        GetTipsCamera((CamFrame*)&c->pose, &c->fov);
        return;
    }
    f32 e = Ease(MaxF(MinF(fn_80277704(slot), 1.0f), 0.0f));
    frame.pos.v[0] = c->carFrame.pos.v[0];
    frame.pos.v[1] = c->carFrame.pos.v[1];
    frame.pos.v[2] = c->carFrame.pos.v[2];
    f32 yaw = Atan2F(c->carFrame.rot.m[6], c->carFrame.rot.m[8]);
    Mat3* rot = (Mat3*)&frame.rot;
    f32 cy = CosF(yaw);
    f32 sy = SinF(yaw);
    frame.rot.m[0] = cy;
    frame.rot.m[1] = 0.0f;
    frame.rot.m[2] = -sy;
    frame.rot.m[3] = 0.0f;
    frame.rot.m[4] = 1.0f;
    frame.rot.m[5] = 0.0f;
    frame.rot.m[6] = sy;
    frame.rot.m[7] = 0.0f;
    frame.rot.m[8] = cy;
    swing.x = 0.0f;
    swing.y = e * 2.6179938f;
    swing.z = 0.0f;
    Mat3FromEuler(&swingRot, &swing);
    Mat3Mul(rot, &swingRot, rot);
    tilt.y = 0.0f;
    tilt.x = 0.2617994f;
    tilt.z = 0.0f;
    Mat3FromEuler(&tiltRot, &tilt);
    Mat3Mul(rot, &tiltRot, rot);
    back.v[0] = 0.0f;
    back.v[1] = 0.0f;
    back.v[2] = -8.0f;
    MulVec(&offset, &back, &frame);
    frame.pos.v[0] += offset.v[0];
    frame.pos.v[1] += offset.v[1];
    frame.pos.v[2] += offset.v[2];
    CamVec3* vel = (CamVec3*)(c->car + 0x118);
    f32 pitch = vel->v[1] / MaxF(PSVECMag(vel), 1.0f);
    if (S32(c->car, 0x2620) == 0 && vel->v[1] < 0.0f) {
        f32 k = MaxF(MinF(Remap(-(c->carFrame.pos.v[1] - fn_801EA0E8(c->carFrame.pos.v[0], c->carFrame.pos.v[2])) / vel->v[1], pitch, 0.0f), 1.0f), 0.0f);
        pitch = Lerp(1.25f, 0.25f, k);
    }
    f32 s = lbl_804A15C0[slot];
    s = Lerp(s, pitch, 0.1f);
    lbl_804A15C0[slot] = s;
    frame.pos.v[1] += 0.65f * (8.0f * s);
    frame.pos.v[1] = MaxF(frame.pos.v[1], fn_801EA0E8(frame.pos.v[0], frame.pos.v[2]) + 4.0f);
    up.v[0] = 0.0f;
    up.v[1] = 1.0f;
    up.v[2] = 0.0f;
    fn_8026E688(&frame, &frame.pos, &c->carFrame.pos, &up);
    fn_8026E22C(&blend, &lbl_804A0C60[slot].pose, &frame, e);
    f32 fov = lbl_804A0C60[slot].fov;
    fov = Lerp(fov, 1.0471976f, e);
    if (fn_80178CA4() == 2) {
        fov = 2.0f * (f32)atan(Lerp(1.0f, 0.65f, e) * (f32)tan(fov * 0.5f));
    }
    c->pose = blend;
    c->fov = fov;
}

extern char lbl_80347378[];  // this unit's string pool ("rddefault", ...)
#define POOL(o) (lbl_80347378 + (o))

void fn_80277F84(NamedCam* c, CamReader* r, int n) {
    c->pose.pos.v[0] = r->GetFloat(POOL(0x178));   // "location_x"
    c->pose.pos.v[1] = r->GetFloat(POOL(0x184));   // "location_y"
    c->pose.pos.v[2] = r->GetFloat(POOL(0x190));   // "location_z"
    c->pose.rot.m[0] = r->GetFloat(POOL(0x19C));   // "basis_x_x"
    c->pose.rot.m[1] = r->GetFloat(POOL(0x1A8));   // "basis_x_y"
    c->pose.rot.m[2] = r->GetFloat(POOL(0x1B4));   // "basis_x_z"
    c->pose.rot.m[3] = r->GetFloat(POOL(0x1C0));   // "basis_y_x"
    c->pose.rot.m[4] = r->GetFloat(POOL(0x1CC));   // "basis_y_y"
    c->pose.rot.m[5] = r->GetFloat(POOL(0x1D8));   // "basis_y_z"
    c->pose.rot.m[6] = r->GetFloat(POOL(0x1E4));   // "basis_z_x"
    c->pose.rot.m[7] = r->GetFloat(POOL(0x1F0));   // "basis_z_y"
    c->pose.rot.m[8] = r->GetFloat(POOL(0x1FC));   // "basis_z_z"
    c->fov = r->GetFloat("fov");
    strncpy(c->name, r->GetString("name", ""), 0x10);
    c->valid = 1;
}

}  // extern "C"
