// Named cameras (0x80276068-0x80278214): the named camera sets ("namedcam", "rddefault", "raceday",
// "house") and the timed camera transitions ("physcam: Clock looped during camera transition!!").
// Written partly against raw field offsets; the other large routines and the initialiser are still
// stubs. Names are guesses apart from the strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

// A named camera setup ("namedcam" entries in the camera file).
// A camera pose: rotation then position.
struct CamMat3 {
    f32 m[9];
};

struct CamVec3 {
    f32 v[3];
};

struct CamPose {
    CamMat3 rot;  // offset 0x0
    CamVec3 pos;  // offset 0x24
};

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

// A camera record (racerslot.cpp holds its constructor).
struct CamRec {
    CamRec(void);
    u8 data[0x228];
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

void fn_80276DC8(u8* p) {}  // stub (591)

extern const f32 lbl_80564054;  // -1.0

f32 fn_80277704(int i) {
    if (lbl_8049FD58[i].active) {
        CamShot* e = &lbl_8049FD58[i];
        return (lbl_8055E3C8 - e->start) / e->dur;
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

void fn_80277A14(u8* p) {}  // stub (348)

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
