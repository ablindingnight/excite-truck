// Shared inline helpers (0x80144504-0x80144E38). Everything this unit defined itself was dead-stripped;
// what survives are the weak copies other units use: the GameMode base constructor and a few of its
// default virtuals, 3x3 rotation matrix and vector helpers, the ease curve, Lerp/Round, the colour
// packer and the sound stop helper. Names are guesses.
#include <math.h>
#include "game/common.h"
#include "game/mtx33.h"
#include "game/rvec.h"
#include "game/sound.h"
#include "game/vpos.h"
#include "game/camframe34.h"
#include "game/math_types.h"

ModuleTag lbl_8055E1D0;
static inline u32 InitialFFFFWord() { return 0xFFFF0000u; }
extern const u32 lbl_8055E1D8 = InitialFFFFWord();
static UnkStatic123 lbl_8042FB88;
static inline u32 Initial7FWord() { return 0x7F000000u; }
extern const u32 lbl_8055E1DC = Initial7FWord();

// These shared helpers keep retail weak binding through DECL_WEAK.
// Their original inline/header declaration context remains unproven.
f32 Dot(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f);

// Mtx33: mtx33.h.
#include "game/mtx34.h"

// The race and tree-hit listener interfaces: this is the first unit that emits their weak all-pure (zeroed)
// vtables, so boostman/treehit and the rest link to these copies. The constructors below are stripped by the linker.
class IHitListener {
public:
    virtual void vf08(void) = 0;
    virtual void vf0C(void) = 0;
    virtual void vf10(void) = 0;
};

class IRaceListener {
public:
    virtual void vf08(void) = 0;
    virtual void vf0C(void) = 0;
    virtual void vf10(void) = 0;
    virtual void vf14(void) = 0;
    virtual void vf18(void) = 0;
};

// MWCC only: dead constructors whose vptr stores make this unit emit the two interface tables (retail strips them).
// LongImpl/ShortImpl are placeholders with no defined virtuals, so a PC build would need their vtables; it gets the
// interface tables from boostman/treehit instead.
#ifdef __MWERKS__
class LongImpl : public IRaceListener {
public:
    LongImpl(void);
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
};

class ShortImpl : public IHitListener {
public:
    ShortImpl(void);
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
};

LongImpl::LongImpl(void) {}

ShortImpl::ShortImpl(void) {}
#endif

// Stripped by the linker; keeps the constant pool in target order.
extern "C" int fn_modeinl_Consts(f32 x) {
    return (x == -1.0f) + (x == -2.0f) + (x == 3.0f) + (x == 0.5f) + (x == 0.0f) + (x == 1.0f);
}

class GameMode {
public:
    int vf3C(int p);
    void vf50(int p);
    void* vfE8(void);
    GameMode(void);
    virtual ~GameMode(void);
};

DECL_WEAK GameMode::GameMode(void) {}

DECL_WEAK f32 Dot(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f) { return e * f + (a * b + c * d); }


// d = a * b
static inline void MtxMul(Mtx33* dp, const Mtx33* ap, const Mtx33* bp) {
    Mtx33& d = *dp;
    const Mtx33& a = *ap;
    const Mtx33& b = *bp;
    if (dp == ap || dp == bp) {
        f32 t00 = Dot(b.xx, a.xx, b.yx, a.xy, b.zx, a.xz);
        f32 t10 = Dot(b.xx, a.yx, b.yx, a.yy, b.zx, a.yz);
        f32 t20 = Dot(b.xx, a.zx, b.yx, a.zy, b.zx, a.zz);
        f32 t01 = Dot(b.xy, a.xx, b.yy, a.xy, b.zy, a.xz);
        f32 t11 = Dot(b.xy, a.yx, b.yy, a.yy, b.zy, a.yz);
        f32 t21 = Dot(b.xy, a.zx, b.yy, a.zy, b.zy, a.zz);
        f32 t02 = Dot(b.xz, a.xx, b.yz, a.xy, b.zz, a.xz);
        f32 t12 = Dot(b.xz, a.yx, b.yz, a.yy, b.zz, a.yz);
        f32 t22 = Dot(b.xz, a.zx, b.yz, a.zy, b.zz, a.zz);
        d.xx = t00;
        d.yx = t10;
        d.zx = t20;
        d.xy = t01;
        d.yy = t11;
        d.zy = t21;
        d.xz = t02;
        d.yz = t12;
        d.zz = t22;
    } else {
        d.xx = Dot(b.xx, a.xx, b.yx, a.xy, b.zx, a.xz);
        d.yx = Dot(b.xx, a.yx, b.yx, a.yy, b.zx, a.yz);
        d.zx = Dot(b.xx, a.zx, b.yx, a.zy, b.zx, a.zz);
        d.xy = Dot(b.xy, a.xx, b.yy, a.xy, b.zy, a.xz);
        d.yy = Dot(b.xy, a.yx, b.yy, a.yy, b.zy, a.yz);
        d.zy = Dot(b.xy, a.zx, b.yy, a.zy, b.zy, a.zz);
        d.xz = Dot(b.xz, a.xx, b.yz, a.xy, b.zz, a.xz);
        d.yz = Dot(b.xz, a.yx, b.yz, a.yy, b.zz, a.yz);
        d.zz = Dot(b.xz, a.zx, b.yz, a.zy, b.zz, a.zz);
    }
}

DECL_WEAK void Mtx33::SetEuler(f32 y, f32 x, f32 z) {
    f32 t[9];
    SetRotY(y);
    Mtx33* rx = (Mtx33*)t;
    rx->SetRotX(x);
    MtxMul(this, rx, this);
    Mtx33* rz = (Mtx33*)t;
    rz->SetRotZ(z);
    MtxMul(this, rz, this);
}



extern "C" {


VPos::VPos(f32 x, f32 y, f32 z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

void CopyMatrix34(Mtx34* d, const Mtx34* s) { *d = *s; }

f32 fn_80144BD8(f32 x) {
    if (x < -1.0f) {
        return -1.0f;
    }
    if (x > 1.0f) {
        return 1.0f;
    }
    if (x > 0.0f) {
        return x * (x * -2.0f * x) + x * 3.0f * x;
    }
    return x * (x * -2.0f * x) - x * 3.0f * x;
}

void StopGameSound(GameSound* s) {
    s->stop = 1;
    s->restart = 0;
}

u32 fn_80144C68(u8 r, u8 g, u8 b, u8 a) { return (r << 24) | (g << 16) | (b << 8) | a; }

f32 fn_80144C80(f32 a, f32 b, f32 t) { return a + t * (b - a); }

int fn_80144C90(f32 x) { return x < 0.0f ? x - 0.5f : x + 0.5f; }

f32 fn_80144CC8(f32 x) { return x * x; }

// PROVISIONAL matching aid. These scalar scratch values reuse the disjoint
// lifetimes seen in retail. Original temporary ownership and optimizer policy
// are unproven. With lifetime splitting enabled this source is not exact.
// Pragma unproven: this scoped setting is required by the reconstructed reuse.
#pragma push
#pragma opt_lifetimes off
void fn_80144CD0(Vec3* out, const Vec3* p, const f32* m) {
    const RMtx& frame = *(const RMtx*)m;
    f32 yWork;
    f32 rowWork;
    f32 term;
    f32 ySum;
    f32 yProduct;
    f32 xSum;
    f32 frameTerm;
    f32 zDelta;
    f32 axisWork;
    f32 zCoefficient;


    ySum = p->x;
    term = frame.pos.x;
    rowWork = p->y;
    axisWork = ySum - term;  // x delta
    yWork = frame.pos.y;
    term = frame.rot.x.x;
    yWork = rowWork - yWork;  // y delta
    rowWork = frame.rot.x.y;
    ySum = frame.rot.y.x;
    xSum = axisWork * term;
    yProduct = yWork * rowWork;
    frameTerm = frame.rot.y.y;
    ySum = axisWork * ySum;
    rowWork = frame.rot.z.x;
    term = yWork * frameTerm;
    zCoefficient = frame.rot.z.y;
    rowWork = axisWork * rowWork;
    zDelta = p->z;
    xSum = xSum + yProduct;
    yWork = yWork * zCoefficient;
    frameTerm = frame.pos.z;
    axisWork = frame.rot.y.z;
    ySum = ySum + term;
    zDelta = zDelta - frameTerm;
    frameTerm = frame.rot.x.z;
    yWork = rowWork + yWork;  // y delta is now consumed, reuse for the z sum
    zCoefficient = frame.rot.z.z;
    yProduct = zDelta * axisWork;
    frameTerm = zDelta * frameTerm;
    term = zDelta * zCoefficient;
    rowWork = yProduct + ySum;
    xSum = frameTerm + xSum;
    yWork = term + yWork;
    out->y = rowWork;
    out->x = xSum;
    out->z = yWork;
}
#pragma pop

#include "game/w3.h"
void fn_80144D64(W3* d, const W3* s) { *d = *s; }

void fn_80144D80(Mtx33* m, f32 x, f32 y, f32 z) {
    m->xx = x;
    m->xy = 0.0f;
    m->xz = 0.0f;
    m->yx = 0.0f;
    m->yy = y;
    m->yz = 0.0f;
    m->zx = 0.0f;
    m->zy = 0.0f;
    m->zz = z;
}

void fn_80144DAC(Vec3* d, const Vec3* s) {
    d->x = s->x;
    d->y = s->y;
    d->z = s->z;
}

CamFrame34::CamFrame34(void) {}

int GameMode::vf3C(int p) { return 0; }

void GameMode::vf50(int p) {}

void* GameMode::vfE8(void) { return 0; }

}  // extern "C"

