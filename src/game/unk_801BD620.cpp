// The car's rigid-body settle step and its module tag: code 0x801BA088-0x801BD628, literal pool (.sdata2
// 0x805627A8-0x80562838) and module tag (.sbss 0x8055E778). Unit name is provisional. C++ since 2026-11-02: the
// target's stack-copy pattern comes from a small by-value vector class (KC's analysis,
// handoff/KC_2026-11-02/KC_fn_801BA088_ANALYSIS_2026-11-02.md).

#include "game/common.h"

struct MVec4 {
    f32 x, y, z, w;
};

// The by-value vector the original was written with (KC): a user copy constructor copies xyz only, so every
// by-value argument/result of an inline op leaves an stfs-only 16-byte stack copy (the 8 KB frame).
struct Vec {
    f32 x, y, z, w;
    Vec() {}
    Vec(f32 s) : x(s) {}
    Vec(f32 a, f32 b, f32 c) : x(a), y(b), z(c) {}
    Vec(f32 a, f32 b, f32 c, f32 d) : x(a), y(b), z(c), w(d) {}
    Vec(const Vec& o) : x(o.x), y(o.y), z(o.z) {}
    Vec& operator=(const Vec& o) {
        x = o.x;
        y = o.y;
        z = o.z;
        return *this;
    }
};

static inline Vec Load(const MVec4& m) { return Vec(m.x, m.y, m.z); }
static inline void Store(MVec4& m, Vec v) {
    m.x = v.x;
    m.y = v.y;
    m.z = v.z;
}
static inline Vec operator*(Vec a, const Vec& k) { return Vec(a.x * k.x, a.y * k.x, a.z * k.x); }

ModuleTag lbl_8055E778;  // registered by this unit's __sinit (0x801BD620)

extern "C" {

float fn_8002B0B0(double x);  // sqrtf wrapper (out of line in the original)

// Steps the car's rigid body 12 times over one frame: clamps and integrates the velocities, integrates the
// orientation quaternion and rebuilds the rotation rows from it. Rows (base a): 0 position, 1-3 rotation, 4 quaternion,
// 5 velocity, 6 angular velocity, 8 inverse inertia (w = inverse mass), 9 force and 10 torque accumulators, which are
// spread over the steps (x12 before, /12 after).
// NonMatching: the per-step contact/suspension pass over the b/c rows (four wheels) is not written yet.
void fn_801BA088(MVec4* t, int a, int b, int c) {
    Vec ident(0.0f, 0.0f, 0.0f, 1.0f);
    Vec zero(0.0f, 0.0f, 0.0f, 0.0f);
    MVec4* force;
    MVec4* torque;
    int step;

    force = &t[a + 9];
    {
        Vec v = Load(*force);
        Vec r = v * Vec(zero.x + 12.0f);
        Store(*force, r);
    }
    torque = &t[a + 10];
    {
        Vec v = Load(*torque);
        Vec r = v * Vec(zero.x + 12.0f);
        Store(*torque, r);
    }

    for (step = 12; step != 0; step--) {
        MVec4* pos = &t[a];
        MVec4* r1 = &t[a + 1];
        MVec4* r2 = &t[a + 2];
        MVec4* r3 = &t[a + 3];
        MVec4* q = &t[a + 4];
        MVec4* vel = &t[a + 5];
        MVec4* spin = &t[a + 6];
        MVec4* inv = &t[a + 8];
        f32 dt = ident.w * ((ident.x + 0.0192f) * (ident.x + 0.08333f));
        f32 vx, vy, vz, wx, wy, wz;
        f32 ax, ay, az;
        f32 lx, ly, lz;
        f32 ox, oy, oz, ow;
        f32 qx, qy, qz, qw;
        f32 dx, dy, dz, dw;
        f32 n;

        ax = force->x * inv->w;
        ay = force->y * inv->w - (ident.x + 9.81f);
        az = force->z * inv->w;

        vx = vel->x < ident.x + 200.0f ? vel->x : ident.x + 200.0f;
        vy = vel->y < ident.x + 200.0f ? vel->y : ident.x + 200.0f;
        vz = vel->z < ident.x + 200.0f ? vel->z : ident.x + 200.0f;
        vx = vx > ident.x + -200.0f ? vx : ident.x + -200.0f;
        vy = vy > ident.x + -200.0f ? vy : ident.x + -200.0f;
        vz = vz > ident.x + -200.0f ? vz : ident.x + -200.0f;
        wx = spin->x < ident.x + 20.0f ? spin->x : ident.x + 20.0f;
        wy = spin->y < ident.x + 20.0f ? spin->y : ident.x + 20.0f;
        wz = spin->z < ident.x + 20.0f ? spin->z : ident.x + 20.0f;
        wx = wx > ident.x + -20.0f ? wx : ident.x + -20.0f;
        wy = wy > ident.x + -20.0f ? wy : ident.x + -20.0f;
        wz = wz > ident.x + -20.0f ? wz : ident.x + -20.0f;

        // torque into the body frame, through the inverse inertia, and back out
        lx = r1->x * torque->x + r1->y * torque->y + r1->z * torque->z;
        ly = r2->y * torque->y + r2->z * torque->z + r2->x * torque->x;
        lz = r3->z * torque->z + r3->x * torque->x + r3->y * torque->y;
        lx = inv->x * lx;
        ly = inv->y * ly;
        lz = inv->z * lz;
        wx += (r1->x * lx + r2->x * ly + r3->x * lz) * dt;
        wy += (r1->y * lx + r2->y * ly + r3->y * lz) * dt;
        wz += (r1->z * lx + r2->z * ly + r3->z * lz) * dt;

        vx += ax * dt;
        vy += ay * dt;
        vz += az * dt;
        pos->x += vx * dt;
        pos->y += vy * dt;
        pos->z += vz * dt;

        // q -= (0.5 w) * q * dt, then renormalise
        ox = wx * (ident.x + 0.5f);
        oy = wy * (ident.x + 0.5f);
        oz = wz * (ident.x + 0.5f);
        ow = ident.w * (2.0f * ident.x);
        qx = q->x;
        qy = q->y;
        qz = q->z;
        qw = q->w;
        dx = qy * oz + qx * ow + ox * qw - oy * qz;
        dy = qz * ox + qy * ow + oy * qw - oz * qx;
        dz = qx * oy + qz * ow + oz * qw - ox * qy;
        dw = qw * ow - qz * oz - ident.w * (qy * oy) - ident.w * (qx * ox);
        qx -= dx * dt;
        qy -= dy * dt;
        qz -= dz * dt;
        qw -= dw * dt;
        n = ident.w / fn_8002B0B0(qx * qx + qy * qy + qz * qz + qw * qw);
        qx *= n;
        qy *= n;
        qz *= n;
        qw *= n;

        vel->x = vx;
        vel->y = vy;
        vel->z = vz;
        spin->x = wx;
        spin->y = wy;
        spin->z = wz;
        q->x = qx;
        q->y = qy;
        q->z = qz;
        q->w = qw;
        {
            f32 sx = qx * 1.4142135f;
            f32 sy = qy * 1.4142135f;
            f32 sz = qz * 1.4142135f;
            f32 sw = qw * 1.4142135f;
            f32 xx = 2.0f * (qx * qx);
            f32 yy = 2.0f * (qy * qy);
            f32 zz = 2.0f * (qz * qz);
            r1->x = (ident.w - ident.w * yy) - ident.w * zz;
            r1->y = sx * sy - sz * sw;
            r1->z = ident.x + (sz * sx + sy * sw);
            r2->x = sx * sy + sz * sw;
            r2->y = (ident.w - ident.w * xx) - ident.w * zz;
            r2->z = sy * sz - sx * sw;
            r3->x = sz * sx - sy * sw;
            r3->y = ident.x + (sy * sz + sx * sw);
            r3->z = (ident.w - ident.w * xx) - ident.w * yy;
        }
        force->x = ident.x;
        force->y = ident.x;
        force->z = ident.x;
        torque->x = ident.x;
        torque->y = ident.x;
        torque->z = ident.x;
    }

    {
        Vec v = Load(*force);
        Vec r = v * Vec(zero.x + 0.08333f);
        Store(*force, r);
    }
    {
        Vec v = Load(*torque);
        Vec r = v * Vec(zero.x + 0.08333f);
        Store(*torque, r);
    }
}

}  // extern "C"
