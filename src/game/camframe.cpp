// Camera frames (0x8026E22C-0x8026E870): blending two frames (orientation through quaternion slerp,
// position linearly) and building a frame looking from one point at another. Names are guesses.
#include "game/common.h"

extern "C" {
double sqrt(double x);
}

struct Vec {
    Vec(void) {}
    void Set(f32 x_, f32 y_, f32 z_) {
        x = x_;
        y = y_;
        z = z_;
    }
    f32 x;
    f32 y;
    f32 z;
};

struct Quat {
    f32 x, y, z, w;
};

// Axes (rows) and position.
struct CamFrame {
    Vec axes[3];  // offset 0x0
    Vec pos;      // offset 0x24
};

struct Camera {
    u8 pad[0x58];
    CamFrame frame;  // offset 0x58
};

extern "C" {
void PSVECCrossProduct(const Vec* a, const Vec* b, Vec* out);
void PSVECNormalize(const Vec* in, Vec* out);
f32 PSVECDotProduct(const Vec* a, const Vec* b);
void fn_8012F57C(Quat* out, Quat* a, Quat* b, f32 t);  // slerp
}

static inline f32 fabsf(f32 x) { return __fabs(x); }

static inline void MtxToQuatAxis(f32 (*m)[3], Quat* q) {
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

static inline void MtxToQuat(const CamFrame* f, Quat* q) {
    const f32(*m)[3] = (const f32(*)[3])f;
    f32 tr = f->axes[0].x + f->axes[1].y + f->axes[2].z;
    if (tr > 0.0f) {
        f32 s = sqrt(tr + 1.0f);
        q->w = 0.5f * s;
        s = 0.5f / s;
        q->x = (m[1][2] - m[2][1]) * s;
        q->y = (m[2][0] - m[0][2]) * s;
        q->z = (m[0][1] - m[1][0]) * s;
    } else {
        MtxToQuatAxis((f32(*)[3])f, q);
    }
}

// The rotation of a unit quaternion.
static inline void QuatToMtx(CamFrame* f, const Quat* q) {
    f32(*m)[3] = (f32(*)[3])f;
    f32 x2 = q->x + q->x;
    f32 y2 = q->y + q->y;
    f32 z2 = q->z + q->z;
    f32 xx = q->x * x2;
    f32 xy = q->x * y2;
    f32 xz = q->x * z2;
    f32 yy = q->y * y2;
    f32 yz = q->y * z2;
    f32 zz = q->z * z2;
    f32 wx = q->w * x2;
    f32 wy = q->w * y2;
    f32 wz = q->w * z2;
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

ModuleTag lbl_8055F518;

extern "C" {

void fn_8026E22C(CamFrame* out, const CamFrame* a, const CamFrame* b, f32 t) {
    Quat qa;
    Quat qb;
    Quat q;
    Vec pos;
    MtxToQuat(a, &qa);
    MtxToQuat(b, &qb);
    fn_8012F57C(&q, &qa, &qb, t);
    QuatToMtx(out, &q);
    pos.x = a->pos.x + t * (b->pos.x - a->pos.x);
    pos.y = a->pos.y + t * (b->pos.y - a->pos.y);
    pos.z = a->pos.z + t * (b->pos.z - a->pos.z);
    out->pos = pos;
}

void fn_8026E688(CamFrame* out, const Vec* eye, const Vec* target, const Vec* up) {
    Vec d;
    Vec fwd;
    Vec upv;
    out->pos = *eye;
    d.x = target->x - eye->x;
    d.y = target->y - eye->y;
    d.z = target->z - eye->z;
    out->axes[0].x = 1.0f;
    out->axes[0].y = 0.0f;
    out->axes[0].z = 0.0f;
    out->axes[1].x = 0.0f;
    out->axes[1].y = 1.0f;
    out->axes[1].z = 0.0f;
    out->axes[2].x = 0.0f;
    out->axes[2].y = 0.0f;
    out->axes[2].z = 1.0f;
    if (PSVECDotProduct(&d, &d) < 1.0f) {
        return;
    }
    if (fabsf(d.x) < 0.01f && fabsf(d.z) < 0.01f) {
        return;
    }
    out->axes[2] = d;
    PSVECCrossProduct(up, &out->axes[2], &out->axes[0]);
    PSVECCrossProduct(&out->axes[2], &out->axes[0], &out->axes[1]);
    Vec right;
    right.Set(out->axes[0].x, out->axes[0].y, out->axes[0].z);
    fwd.Set(out->axes[2].x, out->axes[2].y, out->axes[2].z);
    PSVECCrossProduct(&fwd, &right, &upv);
    PSVECNormalize(&right, &right);
    PSVECNormalize(&upv, &upv);
    PSVECCrossProduct(&right, &upv, &fwd);
    out->axes[0] = right;
    out->axes[1] = upv;
    out->axes[2] = fwd;
}

void fn_8026E850(const Vec* eye, const Vec* target, Camera* cam, const Vec* up) {
    fn_8026E688(&cam->frame, eye, target, up);
}

}  // extern "C"

// NonMatching: fn_8026E22C schedules the quaternion-to-matrix tail differently (59 diffs, same instructions).
