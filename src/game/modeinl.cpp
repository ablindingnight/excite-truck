// Shared inline helpers (0x80144504-0x80144E38). Everything this unit defined itself was dead-stripped;
// what survives are the weak copies other units use: the GameMode base constructor and a few of its
// default virtuals, 3x3 rotation matrix and vector helpers, the ease curve, Lerp/Round, the colour
// packer and the sound stop helper. Names are guesses.
#include <math.h>
#include "game/common.h"

ModuleTag lbl_8055E1D0;
static UnkStatic123 lbl_8042FB88;
UnkStaticFFFF lbl_8055E1D8;
UnkStatic7F lbl_8055E1DC;

struct Vec3 {
    f32 x;
    f32 y;
    f32 z;
};

static inline f32 Dot(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f);

struct VecC {
    VecC(void) {}
    f32 x;
    f32 y;
    f32 z;
};

struct Mtx33 {
    void SetRotX(f32 a) {
        f32 c = cos(a);
        f32 s = sin(a);
        xx = 1.0f;
        xy = 0.0f;
        xz = 0.0f;
        yx = 0.0f;
        yy = c;
        yz = s;
        zx = 0.0f;
        zy = -s;
        zz = c;
    }
    void SetRotY(f32 a) {
        f32 c = cos(a);
        f32 s = sin(a);
        xx = c;
        xy = 0.0f;
        xz = -s;
        yx = 0.0f;
        yy = 1.0f;
        yz = 0.0f;
        zx = s;
        zy = 0.0f;
        zz = c;
    }
    void SetRotZ(f32 a) {
        f32 c = cos(a);
        f32 s = sin(a);
        xx = c;
        xy = s;
        xz = 0.0f;
        yx = -s;
        yy = c;
        yz = 0.0f;
        zx = 0.0f;
        zy = 0.0f;
        zz = 1.0f;
    }
    static Mtx33 RotX(f32 a) {
        Mtx33 m;
        m.SetRotX(a);
        return m;
    }
    static Mtx33 RotZ(f32 a) {
        Mtx33 m;
        m.SetRotZ(a);
        return m;
    }
    void SetEuler(f32 y, f32 x, f32 z);
    f32 xx, xy, xz;
    f32 yx, yy, yz;
    f32 zx, zy, zz;
};

struct Mtx34 {
    f32 m[12];
};

// Two all-pure interfaces whose (zeroed) vtables are all that is left of this unit's .data; the
// constructors below are stripped by the linker.
class IShort {
public:
    virtual void vf08(void) = 0;
    virtual void vf0C(void) = 0;
    virtual void vf10(void) = 0;
};

class ILong {
public:
    virtual void vf08(void) = 0;
    virtual void vf0C(void) = 0;
    virtual void vf10(void) = 0;
    virtual void vf14(void) = 0;
    virtual void vf18(void) = 0;
};

class LongImpl : public ILong {
public:
    LongImpl(void);
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
};

class ShortImpl : public IShort {
public:
    ShortImpl(void);
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
};

LongImpl::LongImpl(void) {}

ShortImpl::ShortImpl(void) {}

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

GameMode::GameMode(void) {}

extern "C" {


void fn_80144B44(Vec3* v, f32 x, f32 y, f32 z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

void fn_80144B54(Mtx34* d, const Mtx34* s) { *d = *s; }

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

struct Sound {
    u8 pad0[0x4A];
    u8 b4A;  // offset 0x4A
    u8 pad4B;
    u8 b4C;  // offset 0x4C
};

void fn_80144C54(Sound* s) {
    s->b4A = 1;
    s->b4C = 0;
}

u32 fn_80144C68(u8 r, u8 g, u8 b, u8 a) { return (r << 24) | (g << 16) | (b << 8) | a; }

f32 fn_80144C80(f32 a, f32 b, f32 t) { return a + t * (b - a); }

int fn_80144C90(f32 x) { return x < 0.0f ? x - 0.5f : x + 0.5f; }

f32 fn_80144CC8(f32 x) { return x * x; }

void fn_80144CD0(Vec3* out, const Vec3* p, const f32* m) {
    f32 dx = p->x - m[9];
    f32 dy = p->y - m[10];
    f32 dz = p->z - m[11];
    out->x = dz * m[2] + (dx * m[0] + dy * m[1]);
    out->y = dz * m[5] + (dx * m[3] + dy * m[4]);
    out->z = dz * m[8] + (dx * m[6] + dy * m[7]);
}

struct W3 {
    u32 a, b, c;
};
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

void fn_80144DC8(void) {}

int GameMode::vf3C(int p) { return 0; }

void GameMode::vf50(int p) {}

void* GameMode::vfE8(void) { return 0; }

}  // extern "C"

// d = a * b
static inline void MtxMul(Mtx33& d, const Mtx33& a, const Mtx33& b) {
    if (&d == &a || &d == &b) {
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

void Mtx33::SetEuler(f32 y, f32 x, f32 z) {
    Mtx33 t;
    SetRotY(y);
    t.SetRotX(x);
    MtxMul(*this, t, *this);
    t.SetRotZ(z);
    MtxMul(*this, t, *this);
}

static inline f32 Dot(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f) { return e * f + (a * b + c * d); }
