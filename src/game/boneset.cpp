// Model bone sets (0x801E592C-0x801E68D4): the 'MRBN' chunk of a model (bones with a rest frame,
// parent and skin-matrix slot), building world matrices from an animation pose (root translation plus
// one quaternion per bone) and from given world frames, then handing the skin matrices to the model.
// Names are guesses.
#include "game/common.h"

struct BVec {
    f32 x, y, z;
};

struct BMtx33 {
    f32 xx, xy, xz;
    f32 yx, yy, yz;
    f32 zx, zy, zz;
};

struct BFrame {
    BMtx33 r;  // offset 0x0
    BVec t;    // offset 0x24
};

struct BQuat {
    f32 x, y, z, w;
};

struct Bone {
    BFrame rest;  // offset 0x0
    s32 parent;   // offset 0x30
    s32 dest;     // offset 0x34
};

struct BoneSet {
    u32 tag;       // offset 0x0
    u32 pad4;      // offset 0x4
    s32 numBones;  // offset 0x8
    s32 numDest;   // offset 0xC
    Bone bones[1]; // offset 0x10
};

struct ModelChunks {
    u8 pad[0x10];
    s32 count;      // offset 0x10
    u32** chunks;   // offset 0x14
};

extern "C" {
ModelChunks* fn_802C6C34(s32 model);
s32 fn_802C7A38(int a, const BFrame* m, s32 n);
f32 fn_80144514(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f);  // a*b + c*d + e*f
void fn_801BFD34(BVec* out, const BVec* v, const BFrame* m);
void fn_80015B20(BFrame* out, const BFrame* a, const BFrame* b);
void fn_801C7254(BMtx33* m);
void fn_801BFE7C(BVec* out, const BVec* a, const BVec* b);
void fn_801C0E60(BVec* out, const BVec* a, const BVec* b);
void StatsUpdate__Fv(BVec* v);
}

static inline f32 Dot(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f) { return fn_80144514(a, b, c, d, e, f); }

// d = a * b
static inline void MtxMul(BMtx33& d, const BMtx33& a, const BMtx33& b) {
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

static inline void MulVec(BVec* out, const BVec* in, const BMtx33* m) {
    if (out == in) {
        f32 x = in->x * m->xx + in->y * m->yx + in->z * m->zx;
        f32 y = in->x * m->xy + in->y * m->yy + in->z * m->zy;
        f32 z = in->x * m->xz + in->y * m->yz + in->z * m->zz;
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = in->x * m->xx + in->y * m->yx + in->z * m->zx;
        out->y = in->x * m->xy + in->y * m->yy + in->z * m->zy;
        out->z = in->x * m->xz + in->y * m->yz + in->z * m->zz;
    }
}

static inline void QuatToMtx(BMtx33* m, const BQuat* q) {
    f32 x2 = q->x + q->x;
    f32 y2 = q->y + q->y;
    f32 z2 = q->z + q->z;
    f32 xx = q->x * x2;
    f32 yy = q->y * y2;
    f32 zz = q->z * z2;
    f32 xy = q->x * y2;
    f32 xz = q->x * z2;
    f32 yz = q->y * z2;
    f32 wx = q->w * x2;
    f32 wy = q->w * y2;
    f32 wz = q->w * z2;
    m->xx = 1.0f - (yy + zz);
    m->yx = xy - wz;
    m->zx = xz + wy;
    m->xy = xy + wz;
    m->yy = 1.0f - (xx + zz);
    m->zy = yz - wx;
    m->xz = xz - wy;
    m->yz = yz + wx;
    m->zz = 1.0f - (xx + yy);
}

static inline void Transpose(BMtx33* m) {
    f32 t;
    t = m->xy;
    m->xy = m->yx;
    m->yx = t;
    t = m->xz;
    m->xz = m->zx;
    m->zx = t;
    t = m->yz;
    m->yz = m->zy;
    m->zy = t;
}

ModuleTag lbl_8055EAB8;

extern "C" {

void fn_801E67B4(BVec* out, const BVec* in, const BFrame* m);

BoneSet* fn_801E592C(s32 model) {
    ModelChunks* d = fn_802C6C34(model);
    for (int i = 0; i < d->count; i++) {
        u32* c = d->chunks[i];
        if (*c == 'MRBN') {
            return (BoneSet*)c;
        }
    }
    return 0;
}

void fn_801E598C(BoneSet* set) {}

s32 fn_801E5990(BoneSet* set) { return set->numBones; }

void fn_801E5998(BoneSet* set, Bone* out, s32* n) {
    int nb = set->numBones;
    for (int i = 0; i < nb; i++) {
        out[i] = set->bones[i];
    }
    *n = nb;
}

s32 fn_801E5E24(BoneSet* set, const BQuat* pose, int n, BFrame* root) {
    BFrame world[32];
    BFrame skin[32];
    int i;
    for (i = 0; i < n; i++) {
        Bone* b = &set->bones[i];
        BFrame local = b->rest;
        BFrame parent;
        BMtx33 rot;
        BMtx33 m;
        BMtx33 t;
        BVec v;
        BFrame a;
        if (b->parent < 1) {
            parent.r.xx = 1.0f;
            parent.r.xy = 0.0f;
            parent.r.xz = 0.0f;
            parent.r.yx = 0.0f;
            parent.r.yy = 1.0f;
            parent.r.yz = 0.0f;
            parent.r.zx = 0.0f;
            parent.r.zy = 0.0f;
            parent.r.zz = 1.0f;
            parent.t.x = 0.0f;
            parent.t.y = 0.0f;
            parent.t.z = 0.0f;
        } else {
            parent = world[b->parent];
        }
        QuatToMtx(&rot, &pose[i + 1]);
        m = local.r;
        Transpose(&m);
        MtxMul(t, m, rot);
        MulVec(&v, &local.t, &t);
        v.x *= -1.0f;
        v.y *= -1.0f;
        v.z *= -1.0f;
        fn_801BFD34(&a.t, &v, &local);
        fn_80015B20(&a, (BFrame*)&t, &local);
        fn_801BFD34(&world[i].t, &a.t, &parent);
        fn_80015B20(&world[i], &a, &parent);
    }
    world[0].t.x = pose[0].x;
    world[0].t.y = pose[0].y;
    world[0].t.z = pose[0].z;
    if (root) {
        *root = world[0];
    }
    s32 nd = set->numDest;
    for (i = 0; i < n; i++) {
        if (set->bones[i].dest >= 0) {
            skin[set->bones[i].dest] = world[i];
        }
    }
    return fn_802C7A38(0, skin, nd);
}

s32 fn_801E65BC(BoneSet* set, const BFrame* world, int n, BFrame* root) {
    BFrame skin[32];
    if (root) {
        *root = world[0];
    }
    s32 nd = set->numDest;
    Bone* b = set->bones;
    for (int i = 0; i < n; i++, b++, world++) {
        if (b->dest >= 0) {
            BFrame w = *world;
            BMtx33 l = b->rest.r;
            fn_801C7254(&l);
            BFrame* s = &skin[b->dest];
            fn_80015B20(s, &w, (BFrame*)&l);
            BVec tmp;
            StatsUpdate__Fv(&tmp);
            fn_801E67B4(&tmp, &b->rest.t, s);
            fn_801C0E60(&s->t, &b->rest.t, &tmp);
            fn_801BFE7C(&s->t, &s->t, &w.t);
            fn_801C0E60(&s->t, &s->t, &b->rest.t);
        }
    }
    return fn_802C7A38(0, skin, nd);
}

void fn_801E67B4(BVec* out, const BVec* in, const BFrame* m) {
    if (out == in) {
        f32 x = in->y * m->r.yx + in->x * m->r.xx + in->z * m->r.zx;
        f32 y = in->y * m->r.yy + in->x * m->r.xy + in->z * m->r.zy;
        f32 z = in->y * m->r.yz + in->x * m->r.xz + in->z * m->r.zz;
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = m->r.yx * in->y + m->r.xx * in->x + m->r.zx * in->z;
        out->y = m->r.yy * in->y + m->r.xy * in->x + m->r.zy * in->z;
        out->z = m->r.yz * in->y + m->r.xz * in->x + m->r.zz * in->z;
    }
}

}  // extern "C"
