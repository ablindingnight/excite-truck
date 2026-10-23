// Stunt car effects (0x802A9F30-0x802AC0E8): the air-stream trails behind a car doing a stunt,
// re-entry puffs, the invincible-car sparks and the stunt fog colour, plus the menu copies of
// the emitter setup. Names are guesses apart from the allocation tags and file names.
#include "game/common.h"
#include "macros.h"
#include <revolution/MTX.h>

struct StuntCar {
    Vec axes[3];  // offset 0x0
    Vec pos;      // offset 0x24
    u8 pad30[0x34 - 0x30];
    ValVec3 vel;  // offset 0x34
    ValVec3 spin; // offset 0x40
    f32 height;   // offset 0x4C
    u8 pad50[0x68 - 0x50];
    u32 flags;  // offset 0x68
    u8 pad6C[0xAC - 0x6C];
    Vec wheel1;  // offset 0xAC
    u8 padB8[0x118 - 0xB8];
    u8 crashed;  // offset 0x118
    u8 pad119[0x124 - 0x119];
    Vec wheel3;  // offset 0x124
};

struct StuntMtx {
    ValVec3 rot[3];  // offset 0x0
    ValVec3 pos;     // offset 0x24
};

struct SColor {
    SColor(void) {}
    SColor(f32 a, f32 b, f32 c) {
        r = a;
        g = b;
        b_ = c;
    }
    f32 r;
    f32 g;
    f32 b_;
};

struct StuntDesc {
    s32 kind;       // offset 0x0
    s32 max;        // offset 0x4
    f32 size0;      // offset 0x8
    f32 size1;      // offset 0xC
    SColor c0;      // offset 0x10
    SColor c1;      // offset 0x1C
    f32 gravity;    // offset 0x28
    f32 unk2C;      // offset 0x2C
    f32 life;       // offset 0x30
    f32 drag;       // offset 0x34
    u8 pad38[0x44 - 0x38];
    s32 frames;     // offset 0x44
    f32 shading;    // offset 0x48
    f32 a4C;        // offset 0x4C
    f32 a50;        // offset 0x50
    f32 a54;        // offset 0x54
    f32 a58;        // offset 0x58
    f32 a5C;        // offset 0x5C
    f32 a60;        // offset 0x60
    f32 a64;        // offset 0x64
    f32 a68;        // offset 0x68
    f32 a6C;        // offset 0x6C
    f32 a70;        // offset 0x70
    f32 a74;        // offset 0x74
    f32 a78;        // offset 0x78
    f32 a7C;        // offset 0x7C
    u8 pad80[0x8C - 0x80];
};

extern "C" {
int fn_80129D90(int seed, int lo, int hi);  // random int
f32 fn_80129DC4(int seed, f32 lo, f32 hi);  // random float
int fn_801E866C(int view);
f32 fn_801E7E24(void);
int fn_801E86F8(int view);
struct StuntOpts {
    s32 unk0;
    u32 mode;  // offset 0x4
};
StuntOpts* fn_801E87A8(void);
s32 fn_801EE0C8(void* model);
void fn_801EEFDC(int (*h)(const void* ev), u8 type);
void fn_801EF024(int (*h)(const void* ev));
s32 fn_80287B14(void);
const StuntCar* fn_80287B24(s32 i);
f32 fn_80287BC8(f32 lo, f32 hi);
void fn_8028DFFC(void);
void fn_8028E024(void);
void fn_8028E070(void);
void fn_8028E080(void);
void fn_8028E240(int a);
void* fn_8028E2F8(StuntDesc* desc, const char* name, const char* tex);
void fn_8028E398(void* e);
void fn_8028E584(void* e, ValVec3 p, ValVec3 v);
void fn_8028E590(void* e, ValVec3 p, ValVec3 v, f32 spin);
void fn_8028E61C(StuntDesc* desc);
u8 fn_802C2C00(u8 a);
void fn_802C2B78(f32 a, f32 b, f32 c);
void fn_802C2BB8(f32* a, f32* b, f32* c);
void fn_802C77F0(s32 model, f32* box);
void* fn_802C86DC(void);
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
int fn_802C8DBC(int n);
void fn_802C8E00(u32 color, int idx, f32 x, f32 y, f32 z, f32 u, f32 v);
void fn_802C8E48(void);
u32 fn_802C990C(u32 color);
s32 fn_802CAC28(const char* name, int flags);
void fn_802CAC54(s32 tex);
double cos(double x);
double sin(double x);
double fmod(double x, double y);
}

void* operator new(size_t size, const char* name);
void* operator new[](size_t size, const char* name);

extern void* lbl_80488648[];  // car models
extern f32 lbl_80558DC4[];    // FLT_EPSILON

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void MulVec(Vec* out, const Vec* in, const Vec* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m[0].x, in->y, m[1].x, in->z, m[2].x);
        f32 y = Dot3(in->x, m[0].y, in->y, m[1].y, in->z, m[2].y);
        f32 z = Dot3(in->x, m[0].z, in->y, m[1].z, in->z, m[2].z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m[0].x, in->y, m[1].x, in->z, m[2].x);
        out->y = Dot3(in->x, m[0].y, in->y, m[1].y, in->z, m[2].y);
        out->z = Dot3(in->x, m[0].z, in->y, m[1].z, in->z, m[2].z);
    }
}

static inline void Scale(Vec* out, const Vec* in, f32 s) {
    out->x = in->x * s;
    out->y = in->y * s;
    out->z = in->z * s;
}

static inline void AddScaled(Vec* out, const Vec* in, const Vec* d, f32 s) {
    out->x = in->x + d->x * s;
    out->y = in->y + d->y * s;
    out->z = in->z + d->z * s;
}

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

static inline int Round(f32 x) { return (int)(x < 0.0f ? x - 0.5f : 0.5f + x); }

static inline int IsZero(f32 x) { return (f32)__fabs(x) <= lbl_80558DC4[0]; }

static inline u32 TrailColor(int a, u8 r, u8 g, u8 b) { return a << 24 | r << 16 | g << 8 | b; }


// One car's stunt effects.
struct StuntPart {
    StuntPart(void);
    Vec pts[16];      // offset 0x0
    f32 phase;        // offset 0xC0
    u8 trig;          // offset 0xC4
    f32 grow;         // offset 0xC8
    f32 alpha;        // offset 0xCC
    f32 facing;       // offset 0xD0
    f32 power;        // offset 0xD4
    f32 hold;         // offset 0xD8
    u8 reentry;       // offset 0xDC
    f32 reentryAmt;   // offset 0xE0
    f32 fogT;         // offset 0xE4
    u32 fogColor;     // offset 0xE8
    u8 spark;         // offset 0xEC
    f32 sparkAmt;     // offset 0xF0
    u8 sparkFlip;     // offset 0xF4
    f32 sizeX;        // offset 0xF8
    f32 sizeZ;        // offset 0xFC
    f32 sizeY;        // offset 0x100
    s32 index;        // offset 0x104
};

struct StuntEmitters {
    StuntEmitters(void) {
        StuntDesc r;
        StuntDesc e;
        fn_8028E61C(&e);
        f32 t = 0.3f;
        f32 t2 = 0.3f * 0.3f;
        e.kind = 2;
        e.max = 300;
        e.size0 = t;
        e.size1 = 1.0f;
        e.frames = 5;
        e.shading = 0.5f;
        e.c0 = SColor(0.0f, 1.0f, 0.0f);
        e.c1 = SColor(1.05f, 1.0f, 1.05f);
        e.gravity = 0.0f;
        e.unk2C = 4.0f;
        e.life = 0.0f;
        e.drag = 0.1f;
        e.a60 = -0.2f;
        e.a68 = -0.2f;
        e.a70 = -0.2f;
        e.a64 = 1.0f;
        e.a6C = 1.0f;
        e.a74 = 1.0f;
        e.a58 = 0.0f;
        e.a5C = 3.9f;
        f32 g = -1.499f * (4.0f / t2);
        e.a4C = g;
        e.a50 = (-0.001f - 0.5f * g * t2) / t;
        e.a54 = 0.001f;
        e.a78 = 0.0f;
        e.a7C = 2.0f;
        e0 = fn_8028E2F8(&e, "invcblespark", "spark.tex");
        fn_8028E61C(&r);
        r.kind = 2;
        r.max = 0x60;
        r.size0 = 0.5f;
        r.size1 = 1.0f;
        r.frames = 5;
        r.shading = 0.5f;
        r.c0 = SColor(0.0f, 0.0f, 0.0f);
        r.c1 = SColor(1.0f, 1.0f, 1.0f);
        r.gravity = 0.0f;
        r.unk2C = 3.0f;
        r.life = 0.0f;
        r.drag = 0.1f;
        r.a70 = -0.2f;
        r.a68 = -0.2f;
        r.a60 = -0.2f;
        r.a74 = 1.1f;
        r.a6C = 1.1f;
        r.a64 = 1.1f;
        r.a58 = -0.2f;
        r.a5C = 0.2f;
        r.a50 = 0.2f;
        r.a54 = 1.8f;
        r.a78 = 0.8f;
        r.a7C = 1.0f;
        e1 = fn_8028E2F8(&r, "reentry", "entrypuff.tex");
    }
    ~StuntEmitters(void) {
        fn_8028E398(e0);
        fn_8028E398(e1);
    }
    void* e0;  // offset 0x0
    void* e1;  // offset 0x4
};

struct SavedFog {
    u32 color;
    f32 a;
    f32 b;
    f32 c;
};

struct StuntPowerEv {
    s32 index;  // offset 0x0
    f32 power;  // offset 0x4
    u8 hold;    // offset 0x8
};

struct StuntSparkEv {
    s32 index;  // offset 0x0
    u8 flip;    // offset 0x4
};

ModuleTag lbl_8055F918;
UnkStaticFFFF lbl_8055F920;
StuntEmitters* lbl_8055F924;
StuntPart* lbl_8055F928;
bool lbl_8055F92C;
u8 lbl_8055F92D;
SavedFog lbl_804BD7B0;

s32 lbl_8055CC60 = -2;  // trail texture
s32 lbl_8055CC64 = -2;  // spark texture
f32 lbl_8055CC68 = 1.0f / 7.0f;
f32 lbl_8055CC6C = 0.8f;
s32 lbl_8055CC70 = 150;
s32 lbl_8055CC74 = 255;
s32 lbl_8055CC78 = 150;

extern "C" {

int fn_802A9F30(const void* ev) {
    const StuntPowerEv* e = (const StuntPowerEv*)ev;
    lbl_8055F928[e->index].trig = 1;
    lbl_8055F928[e->index].power = e->power;
    lbl_8055F928[e->index].hold = e->hold;
    return 0xC;
}

int fn_802A9F90(const void* ev) {
    lbl_8055F928[*(const s32*)ev].reentry = 1;
    return 4;
}

int fn_802A9FB0(const void* ev) {
    const StuntSparkEv* e = (const StuntSparkEv*)ev;
    lbl_8055F928[e->index].spark = 1;
    lbl_8055F928[e->index].sparkFlip = e->flip;
    return 8;
}

void fn_802A9FE0(StuntPart* p) {
    for (int i = 0; i < 16; i++) {
        p->pts[i].x = 0.0f;
        p->pts[i].y = 0.0f;
        p->pts[i].z = 0.0f;
    }
    p->phase = 0.0f;
    p->trig = 0;
    p->alpha = 0.0f;
    p->grow = 0.0f;
    p->facing = 0.0f;
    p->power = 0.0f;
    p->hold = 0.0f;
}

void fn_802AA0C8(StuntPart* p) {
    p->reentry = 0;
    p->reentryAmt = 0.0f;
    p->fogT = 0.5f;
    p->fogColor = -1;
}

void fn_802AA0EC(StuntPart* p) {
    p->spark = 0;
    p->sparkAmt = 0.0f;
}

// Projects v onto the plane of a and b.
void fn_802AA100(Vec* out, const Vec* v, const Vec* a, const Vec* b) {
    f32 da = PSVECDotProduct(v, a);
    f32 db = PSVECDotProduct(v, b);
    Vec ta;
    Vec tb;
    Scale(&ta, a, da);
    Scale(&tb, b, db);
    out->x = ta.x + tb.x;
    out->y = ta.y + tb.y;
    out->z = ta.z + tb.z;
}

void fn_802AA1D0(int idx) {
    StuntPart* p = &lbl_8055F928[idx];
    f32 box[6];
    fn_802C77F0(fn_801EE0C8((u32)idx < 8 ? lbl_80488648[idx] : 0), box);
    p->sizeX = box[1] - box[0];
    p->sizeY = box[3] - box[2];
    p->sizeZ = box[5] - box[4];
}

void fn_802AA25C(StuntPart* p, const StuntCar* car, f32 dt) {
    p->phase = fmod(p->phase + 8.0f * dt, 1.0);
    if (p->power >= 0.35f && p->hold == 0.0f && p->trig) {
        p->trig = 0;
        p->grow = MIN(p->grow + 2.0f * dt, 1.0f);
        f32 s = MIN(1.0f, (p->power - 0.35f) / 0.65f);
        s = MAX(s, 0.0f);
        p->alpha = Lerp(0.0f, p->grow, s);
    } else {
        p->grow = MAX(p->grow - 5.0f * dt, 0.0f);
        p->alpha = MAX(p->alpha - 5.0f * dt, 0.0f);
    }
    if (car->crashed) {
        p->alpha = 0.0f;
    }
    if (car->flags & 8) {
        p->alpha = 0.0f;
    }
    if (p->alpha >= 0.01f) {
        Vec w[2];
        Vec s[2];
        Vec n;
        Vec c;
        Vec sum;
        w[0].x = car->wheel1.x;
        w[0].y = 0.0f;
        w[0].z = car->wheel1.z;
        w[1].x = car->wheel3.x;
        w[1].y = 0.0f;
        w[1].z = car->wheel3.z;
        MulVec(&w[0], &w[0], car->axes);
        MulVec(&w[1], &w[1], car->axes);
        fn_802AA100(&n, (const Vec*)&car->spin, &car->axes[0], &car->axes[2]);
        if (PSVECDotProduct(&n, &n) > 0.01f) {
            PSVECNormalize(&n, &n);
            p->facing = MAX(0.0f, PSVECDotProduct(&n, &car->axes[2]));
            f32 best = -9999.9f;
            PSVECCrossProduct(&n, &car->axes[1], &c);
            for (u32 j = 0; j < 2; j++) {
                f32 m = PSVECMag(&w[j]);
                f32 d = (f32)__fabs(PSVECDotProduct(&c, &w[j]) / m);
                if (d > best) {
                    best = d;
                    s[0] = w[j];
                    Scale(&s[1], &w[j], -1.0f);
                }
            }
            p->pts[0].x = s[0].x + car->pos.x;
            p->pts[0].y = s[0].y + car->pos.y;
            p->pts[0].z = s[0].z + car->pos.z;
            p->pts[1].x = s[1].x + car->pos.x;
            p->pts[1].y = s[1].y + car->pos.y;
            p->pts[1].z = s[1].z + car->pos.z;
            f32 adt = (f32)__fabs(dt);
            for (int i = 1; i < 8; i++) {
                f32 t = i / 8.0f;
                Vec a = *(const Vec*)&car->spin;
                Vec b = n;
                f32 k = -PSVECMag((const Vec*)&car->vel) * lbl_8055CC68;
                Scale(&a, &a, 1.0f * (t * k));
                Scale(&b, &b, 1.0f * ((1.0f - t) * k));
                sum.x = b.x + a.x;
                sum.y = b.y + a.y;
                sum.z = b.z + a.z;
                AddScaled(&p->pts[i * 2], &p->pts[i * 2 - 2], &sum, 0.125f);
                AddScaled(&p->pts[i * 2 + 1], &p->pts[i * 2 - 1], &sum, 0.125f);
                if (!(adt <= lbl_80558DC4[0]) && fn_80287BC8(0.0f, 1.0f) < p->alpha * p->facing &&
                    fn_80287BC8(0.0f, 1.0f) > t) {
                    f32 r = fn_80287BC8(0.25f, 0.75f);
                    Vec q;
                    const Vec* a0 = &p->pts[i * 2];
                    const Vec* a1 = &p->pts[i * 2 + 1];
                    q.x = Lerp(a0->x, a1->x, r);
                    q.y = Lerp(a0->y, a1->y, r);
                    q.z = Lerp(a0->z, a1->z, r);
                    AddScaled(&q, &q, &car->axes[1], -1.0f);
                    fn_8028E584(lbl_8055F924->e1, *(ValVec3*)&q, car->vel);
                }
            }
        } else {
            p->facing = 0.0f;
        }
    }
}

void fn_802AA9D4(StuntPart* p, const StuntCar* car, f32 dt) {
    f32 hx = 0.5f * p->sizeX;
    f32 hz = 0.5f * p->sizeZ;
    f32 hy = 0.5f * p->sizeY;
    if (p->reentry) {
        p->reentry = 0;
        p->reentryAmt = MIN(p->reentryAmt + 8.0f * dt, 1.0f);
    } else {
        p->reentryAmt = MAX(p->reentryAmt - 8.0f * dt, 0.0f);
    }
    if ((f32)__fabs(dt) <= lbl_80558DC4[0]) {
        return;
    }
    if (fn_80287BC8(0.0f, 1.0f) < p->reentryAmt) {
        f32 ang = fn_80287BC8(-3.1415927f, 3.1415927f);
        Vec x = *(const Vec*)&car->axes[0];
        Vec z = *(const Vec*)&car->axes[2];
        ValVec3 vel;
        Vec pos = car->pos;
        AddScaled(&pos, &pos, &x, hx * (f32)cos(ang));
        AddScaled(&pos, &pos, &z, hz * (f32)sin(ang));
        AddScaled(&pos, &pos, &car->axes[1], fn_80287BC8(0.0f, 2.0f * hy));
        vel = car->vel;
        if (car->height >= 1.5f - lbl_80558DC4[0]) {
            fn_8028E590(lbl_8055F924->e0, *(ValVec3*)&pos, vel, 1.0f);
        }
    }
}

void fn_802AAC80(StuntPart* p, const StuntCar* car, f32 dt) {
    fn_801E86F8(0);
    if (p->spark) {
        p->spark = 0;
        p->sparkAmt = MIN(p->sparkAmt + 2.0f * dt, 1.0f);
    } else if (car->crashed) {
        p->sparkAmt = 0.0f;
    } else {
        p->sparkAmt = MAX(p->sparkAmt - 2.0f * dt, 0.0f);
    }
    if (car->flags & 8) {
        p->sparkAmt = 0.0f;
    }
}

void fn_802AAD5C(void* cam, StuntPart* p, const StuntCar* car) {
    fn_802C8BD0(lbl_8055CC60);
    fn_801E866C(-1);
    if (p->alpha > 0.01f) {
        fn_802C8DBC(16);
        f32 phase = p->phase;
        for (int i = 0; i < 16; i++) {
            Vec v;
            v.x = p->pts[i].x;
            v.y = p->pts[i].y;
            v.z = p->pts[i].z;
            int k = 0;
            if (i > 1) {
                k = (16 - (i + 1)) / 2;
            }
            f32 f = k / 8.0f;
            u32 color = TrailColor(Round(lbl_8055CC6C * (f * (255.0f * (p->alpha * p->facing)))), lbl_8055CC70,
                                   lbl_8055CC78, lbl_8055CC74);
            fn_802C8E00(color, 1, v.x, v.y, v.z, (i % 2) ? 0.0f : 1.0f, phase - f);
        }
        fn_802C8E48();
    }
    fn_802C8D74();
}

void fn_802AAF00(void* cam, StuntPart* p, const StuntCar* car) {}

void fn_802AAF04(void* cam, const StuntPart* p, const StuntCar* car) {
    StuntMtx m = *(const StuntMtx*)car;
    if (p->sparkAmt > 0.001f) {
        fn_802C8BD0(lbl_8055CC64);
        for (int k = 0; k < 2; k++) {
            fn_802C8DBC(10);
            for (int j = 0; j < 5; j++) {
                f32 fj = j;
                f32 ang = 1.5707964f + 1.75f * ((fj - 1.0f) / 5.0f) + (k ? 3.1415927f : 0.0f);
                if (p->sparkFlip) {
                    ang *= -1.0f;
                }
                f32 cx = p->sizeZ * (f32)cos(ang) * 0.5f;
                f32 sz = p->sizeZ * (f32)sin(ang) * 0.5f;
                Vec base;
                base.x = m.pos.x + m.rot[0].x * cx + m.rot[2].x * sz;
                base.y = m.pos.y + m.rot[0].y * cx + m.rot[2].y * sz;
                base.z = m.pos.z + m.rot[0].z * cx + m.rot[2].z * sz;
                fj *= 0.25f;
                Vec v[2];
                for (int i = 0; i < 2; i++) {
                    v[i] = base;
                    f32 h = i ? p->sizeY * 0.5f - 0.4f : 0.4f + p->sizeY * 0.5f;
                    AddScaled(&v[i], &v[i], (const Vec*)&m.rot[1], h);
                    u32 color = Round(255.0f * p->sparkAmt) << 24 | 0xFFFFFF;
                    fn_802C8E00(color, 1, v[i].x, v[i].y, v[i].z, i ? 1.0f : 0.0f, 1.0f - fj);
                }
            }
            fn_802C8E48();
        }
        fn_802C8D74();
    }
}

void fn_802AB5B8(void);

void fn_802AB21C(void) {
    lbl_8055F928 = new ("efxstuntcar") StuntPart[8];
    lbl_8055F924 = new ("stuntpart") StuntEmitters;
    fn_802AB5B8();
    lbl_8055CC60 = fn_802CAC28("airstrm.tex", 0);
    lbl_8055CC64 = fn_802CAC28("entrypuff.tex", 0);
    fn_801EEFDC(fn_802A9F30, 15);
    fn_801EEFDC(fn_802A9F90, 16);
    fn_801EEFDC(fn_802A9FB0, 17);
    lbl_8055F92C = fn_801E87A8()->mode == 0x180304;
}

}  // extern "C"

StuntPart::StuntPart(void) {}

extern "C" {

void fn_802AB504(void) {
    lbl_8055F92C = 0;
    fn_801EF024(fn_802A9F30);
    fn_801EF024(fn_802A9F90);
    fn_801EF024(fn_802A9FB0);
    delete lbl_8055F924;
    lbl_8055F924 = 0;
    delete[] lbl_8055F928;
    lbl_8055F928 = 0;
    fn_802CAC54(lbl_8055CC60);
    lbl_8055CC60 = -2;
    fn_802CAC54(lbl_8055CC64);
    lbl_8055CC64 = -2;
}

void fn_802AB5B8(void) {
    for (int i = 0; i < 8; i++) {
        StuntPart* p = &lbl_8055F928[i];
        p->index = i;
        fn_802A9FE0(p);
        fn_802AA0C8(p);
        fn_802AA0EC(p);
    }
}

void fn_802AB628(f32 dt) {
    int n = fn_80287B14();
    for (int i = 0; i < n; i++) {
        const StuntCar* car = fn_80287B24(i);
        StuntPart* p = &lbl_8055F928[i];
        fn_802AA1D0(i);
        fn_802AA25C(p, car, dt);
        fn_802AA9D4(p, car, dt);
        fn_802AAC80(p, car, dt);
    }
}

void fn_802AB6D0(void) {}

void fn_802AB6D4(void) {
    int n = fn_80287B14();
    void* cam = fn_802C86DC();
    for (int i = 0; i < n; i++) {
        const StuntCar* car = fn_80287B24(i);
        StuntPart* p = &lbl_8055F928[i];
        fn_802AAD5C(cam, p, car);
        fn_802AAF00(cam, p, car);
        fn_802AAF04(cam, p, car);
    }
}

static inline u32 MakeFog(u8 b, u8 g, u8 r) { return 0xFF000000 | b << 16 | g << 8 | r; }

static inline u32 FogColor(void) {
    return MakeFog(fn_80129D90(0, 240, 255), fn_80129D90(0, 250, 255), fn_80129D90(0, 140, 255));
}

void fn_802AB770(int idx) {
    const StuntCar* car;
    StuntPart* p = &lbl_8055F928[idx];
    car = fn_80287B24(idx);
    f32 dt = fn_801E7E24();
    if (!((f32)__fabs(dt) <= lbl_80558DC4[0])) {
        u8 r = fn_80129D90(0, 140, 255);
        u8 g = fn_80129D90(0, 250, 255);
        u8 b = fn_80129D90(0, 240, 255);
        p->fogColor = MakeFog(b, g, r);
    }
    f32 h = car->height;
    if (lbl_8055F92C) {
        h = 10.0f;
    }
    if (p->fogT > 0.5f) {
        p->fogT = fn_80287BC8(0.2f, 0.3f);
    } else {
        p->fogT = p->fogT + 0.25f * dt;
    }
    f32 jump = 0.0f;
    if (!IsZero(dt)) {
        jump = 500.0f * (int)fn_80287BC8(0.0f, 1.0f);
    }
    int low = h < 3.0f;
    int on = 1;
    if (low) {
        int up = (f32)fmod(h, 0.5) > 0.25f;
        if (!up) {
            on = 0;
        }
    }
    f32 z = 0.0f;
    if (on) {
        z = -1000.0f * p->fogT - jump;
    }
    u32 color = p->fogColor;
    lbl_8055F92D = fn_802C2C00(1);
    lbl_804BD7B0.color = fn_802C990C(color);
    fn_802C2BB8(&lbl_804BD7B0.a, &lbl_804BD7B0.b, &lbl_804BD7B0.c);
    fn_802C2B78(z, 1000.0f + z, 1.0f);
}

void fn_802AB994(void) {
    fn_802C990C(lbl_804BD7B0.color);
    fn_802C2B78(lbl_804BD7B0.a, lbl_804BD7B0.b, lbl_804BD7B0.c);
    fn_802C2C00(lbl_8055F92D);
}

void fn_802AB9E0(void) { fn_8028E070(); }

void fn_802AB9E4(void) { fn_8028E080(); }

void fn_802AB9E8(void) {
    fn_8028DFFC();
    lbl_8055F924 = new ("uistuntpart") StuntEmitters;
    fn_8028E024();
}

void fn_802ABC2C(void) {
    if (lbl_8055F924) {
        delete lbl_8055F924;
        lbl_8055F924 = 0;
    }
}

void fn_802ABC80(const StuntMtx* m, const ValVec3* vel, f32 a, f32 b, f32 c) {
    f32 ang = fn_80287BC8(-3.1415927f, 3.1415927f);
    Vec pos = *(const Vec*)&m->pos;
    Vec z = *(const Vec*)&m->rot[2];
    Vec x = *(const Vec*)&m->rot[0];
    AddScaled(&pos, &pos, &x, a * (f32)cos(ang));
    AddScaled(&pos, &pos, &z, b * (f32)sin(ang));
    AddScaled(&pos, &pos, (const Vec*)&m->rot[1], fn_80287BC8(0.0f, 2.0f * c));
    fn_8028E590(lbl_8055F924->e0, *(ValVec3*)&pos, *vel, 1.0f);
}

void fn_802ABE50(void) { fn_8028E240(0); }

void fn_802ABE58(f32* t, f32* out, u32* color, f32 dt) {
    *color = FogColor();
    f32 h = 10.0f;
    if (lbl_8055F92C) {
        h = 10.0f;
    }
    if (*t > 0.5f) {
        *t = fn_80287BC8(0.2f, 0.3f);
    } else {
        *t = *t + 0.25f * dt;
    }
    f32 jump = 0.0f;
    if (!IsZero(dt)) {
        jump = 500.0f * (int)fn_80287BC8(0.0f, 1.0f);
    }
    int low = h < 3.0f;
    int on = 1;
    if (low) {
        int up = (f32)fmod(h, 0.5) > 0.25f;
        if (!up) {
            on = 0;
        }
    }
    f32 z = 0.0f;
    if (on) {
        z = -1000.0f * *t - jump;
    }
    *out = z;
}

void fn_802ABFF8(u32 color, f32 z) {
    lbl_8055F92D = fn_802C2C00(1);
    lbl_804BD7B0.color = fn_802C990C(color);
    fn_802C2BB8(&lbl_804BD7B0.a, &lbl_804BD7B0.b, &lbl_804BD7B0.c);
    fn_802C2B78(z, 1000.0f + z, 1.0f);
}

void fn_802AC070(void) {
    fn_802C990C(lbl_804BD7B0.color);
    fn_802C2B78(lbl_804BD7B0.a, lbl_804BD7B0.b, lbl_804BD7B0.c);
    fn_802C2C00(lbl_8055F92D);
}

}  // extern "C"
