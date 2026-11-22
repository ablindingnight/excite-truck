// Camera frames (0x8026E22C-0x8026E870): blending two frames (orientation through quaternion slerp,
// position linearly) and building a frame looking from one point at another. Names are guesses.
#include "game/common.h"
#include "game/rvec.h"
#include "game/rmtx.h"
#include "game/camblend.h"
#include <revolution/MTX/vec.h>
#include "game/mathutil.h"

extern "C" {
double sqrt(double x);
}

// CfVec: rvec.h's RVec3.
#include "game/quat.h"

// CamFrame: rmtx.h's RMtx.
// The camera record (cammgr/namedcam): game/camrec.h's CamRec (its pose at 0x58).
#include "game/camrec.h"

extern "C" {
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

static inline void MtxToQuat(const RMtx* f, Quat* q) {
    const f32(*m)[3] = (const f32(*)[3])f;
    f32 tr = f->rot.x.x + f->rot.y.y + f->rot.z.z;
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
static inline void QuatToMtx(RMtx* f, const Quat* q) {
    f32(*m)[3] = (f32(*)[3])f;
    f32 x2 = q->x + q->x;
    f32 y2 = q->y + q->y;
    f32 z2 = q->z + q->z;
    f32 xx = q->x * x2;
    f32 yy = q->y * y2;
    f32 zz = q->z * z2;
    f32 wx = q->w * x2;
    f32 wy = q->w * y2;
    f32 xz = q->x * z2;
    f32 yz = q->y * z2;
    f32 wz = q->w * z2;
    f32 xy = q->x * y2;
    m[0][0] = 1.0f - (yy + zz);
    m[1][1] = 1.0f - (xx + zz);
    m[2][0] = xz + wy;
    m[2][1] = yz - wx;
    m[1][0] = xy - wz;
    m[0][1] = xy + wz;
    m[0][2] = xz - wy;
    m[1][2] = yz + wx;
    m[2][2] = 1.0f - (xx + yy);
}

ModuleTag lbl_8055F518;

extern "C" {


void InterpolateCameraMatrix(RMtx* out, const RMtx* a, const RMtx* b, f32 t) {
    Quat qa;
    Quat qb;
    Quat q;
    RVec3 pos;
    MtxToQuat(a, &qa);
    MtxToQuat(b, &qb);
    fn_8012F57C(&q, &qa, &qb, t);
    QuatToMtx(out, &q);
    pos.Set(Lerp(a->pos.x, b->pos.x, t), Lerp(a->pos.y, b->pos.y, t), Lerp(a->pos.z, b->pos.z, t));
    out->pos = pos;
}

void CalculateCameraMatrix(RMtx* out, const RVec3* eye, const RVec3* target, const RVec3* up) {
    RVec3 d;
    RVec3 fwd;
    RVec3 upv;
    out->pos = *eye;
    d.x = target->x - eye->x;
    d.y = target->y - eye->y;
    d.z = target->z - eye->z;
    out->rot.x.x = 1.0f;
    out->rot.x.y = 0.0f;
    out->rot.x.z = 0.0f;
    out->rot.y.x = 0.0f;
    out->rot.y.y = 1.0f;
    out->rot.y.z = 0.0f;
    out->rot.z.x = 0.0f;
    out->rot.z.y = 0.0f;
    out->rot.z.z = 1.0f;
    if (PSVECDotProduct((const Vec*)&d, (const Vec*)&d) < 1.0f) {
        return;
    }
    if (fabsf(d.x) < 0.01f && fabsf(d.z) < 0.01f) {
        return;
    }
    out->rot.z = d;
    PSVECCrossProduct((const Vec*)up, (const Vec*)&out->rot.z, (Vec*)&out->rot.x);
    PSVECCrossProduct((const Vec*)&out->rot.z, (const Vec*)&out->rot.x, (Vec*)&out->rot.y);
    RVec3 right;
    right.Set(out->rot.x.x, out->rot.x.y, out->rot.x.z);
    fwd.Set(out->rot.z.x, out->rot.z.y, out->rot.z.z);
    PSVECCrossProduct((const Vec*)&fwd, (const Vec*)&right, (Vec*)&upv);
    PSVECNormalize((const Vec*)&right, (Vec*)&right);
    PSVECNormalize((const Vec*)&upv, (Vec*)&upv);
    PSVECCrossProduct((const Vec*)&right, (const Vec*)&upv, (Vec*)&fwd);
    out->rot.x = right;
    out->rot.y = upv;
    out->rot.z = fwd;
}

void fn_8026E850(const RVec3* eye, const RVec3* target, CamRec* cam, const RVec3* up) {
    CalculateCameraMatrix((RMtx*)&cam->pose, eye, target, up);  // the pose as a 3x4 frame
}

}  // extern "C"

