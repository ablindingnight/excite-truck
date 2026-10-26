// Triangle strip batcher (0x801E4E44-0x801E592C): a 60-vertex / 174-index buffer flushed to the GPU
// when full, used for plain strips, two-sided skid quads and Hermite-spline skid ribbons (fading
// with alpha along the curve). Names are guesses apart from the string.
#include "game/common.h"

struct TVec3 {
    f32 x, y, z;
};

struct TVtx {
    TVec3 pos;  // offset 0x0
    u32 color;  // offset 0xC
    f32 u, v;   // offset 0x10
};

struct SkidQuad {
    TVec3 dir;  // offset 0x0
    f32 half;   // offset 0xC
};

// The spline ribbon: the previous edge points, tangent and alpha.
struct SkidSpline {
    TVec3 p0;     // offset 0x0
    TVec3 p1;     // offset 0xC
    TVec3 tan;    // offset 0x18
    f32 alpha;    // offset 0x24
    s32 count;    // offset 0x28
    s32 max;      // offset 0x2C
    s32 subdiv;   // offset 0x30
    f32 scale;    // offset 0x34
};

extern "C" {
void fn_802D68C4(TVtx* v, int nv, const u16* idx, int ni);
f32 fn_802C88D4(const SkidSpline* s);
void fn_802E317C(const char* fmt, ...);
}

ModuleTag lbl_8055EAA0;
s32 lbl_8055EAA8;  // vertices
s32 lbl_8055EAAC;  // indices
s32 lbl_8055EAB0;  // mode

TVtx lbl_80487578[60];
u16 lbl_80487B18[174];
SkidQuad lbl_80487C78;
SkidSpline lbl_80487C88;

static inline void AddTri(int n) {
    if (n > 2) {
        lbl_80487B18[lbl_8055EAAC++] = n - 3;
        lbl_80487B18[lbl_8055EAAC++] = n - 2;
        lbl_80487B18[lbl_8055EAAC++] = n - 1;
    }
}

static inline int AlphaByte(f32 a) {
    int v = (int)(255.0f * a);
    v = v > 0 ? v : 0;
    int c = 0xFF;
    if (v < 0xFF) {
        c = v;
    }
    return c;
}

extern "C" {

void fn_801E4E44(void) {
    if (lbl_8055EAA8 > 2) {
        if (lbl_8055EAAC > 2) {
            fn_802D68C4(lbl_80487578, lbl_8055EAA8, lbl_80487B18, lbl_8055EAAC);
        }
        int n = lbl_8055EAA8;
        lbl_8055EAA8 = 2;
        lbl_80487578[0] = lbl_80487578[n - 2];
        lbl_8055EAAC = 0;
        lbl_80487578[1] = lbl_80487578[n - 1];
    }
}

int fn_801E4F20(void) {
    lbl_8055EAA8 = 0;
    lbl_8055EAAC = 0;
    lbl_8055EAB0 = 1;
    return 1;
}

int fn_801E4F3C(u32 color, int strip, f32 x, f32 y, f32 z, f32 u, f32 v) {
    TVtx* p = &lbl_80487578[lbl_8055EAA8];
    p->pos.x = x;
    p->pos.y = y;
    p->pos.z = z;
    p->u = u;
    p->v = v;
    lbl_8055EAA8++;
    p->color = color;
    if (strip) {
        AddTri(lbl_8055EAA8);
    }
    if (lbl_8055EAA8 == 60 || lbl_8055EAAC == 174) {
        fn_801E4E44();
    }
    return 0;
}

void fn_801E4FF4(void) {
    fn_801E4E44();
    lbl_8055EAB0 = 0;
}

int fn_801E501C(int unused, const TVec3* dir, f32 width, u8 alpha) {
    u32 color = (alpha << 24) | 0xFFFFFF;
    for (int i = 0; i < 60; i++) {
        lbl_80487578[i].color = color;
    }
    lbl_8055EAA8 = 0;
    lbl_8055EAAC = 0;
    lbl_80487C78.dir = *dir;
    lbl_80487C78.half = 0.5f * width;
    lbl_8055EAB0 = 2;
    return 1;
}

void fn_801E5174(f32 x, f32 y, f32 z) {
    SkidQuad* q = &lbl_80487C78;
    if (lbl_8055EAA8 + 4 > 60 || lbl_8055EAAC + 6 > 174) {
        fn_801E4E44();
    }
    int n = lbl_8055EAA8;
    TVtx* v0 = &lbl_80487578[n];
    TVtx* v1 = &lbl_80487578[n + 1];
    TVtx* v2 = &lbl_80487578[n + 2];
    TVtx* v3 = &lbl_80487578[n + 3];
    v0->pos.x = x - q->half;
    v0->pos.y = y;
    v0->pos.z = z;
    v1->pos.x = x + q->half;
    v1->pos.y = y;
    v1->pos.z = z;
    v2->pos.x = v0->pos.x + q->dir.x;
    v2->pos.y = v0->pos.y + q->dir.y;
    v2->pos.z = v0->pos.z + q->dir.z;
    v3->pos.x = v1->pos.x + q->dir.x;
    v3->pos.y = v1->pos.y + q->dir.y;
    v3->pos.z = v1->pos.z + q->dir.z;
    v2->u = 0.0f;
    v0->u = 0.0f;
    v3->u = 1.0f;
    v1->u = 1.0f;
    v1->v = 1.0f;
    v0->v = 1.0f;
    v3->v = 0.0f;
    v2->v = 0.0f;
    n += 3;
    AddTri(n);
    n++;
    lbl_8055EAA8 = n;
    AddTri(n);
}

void fn_801E5348(void) {
    fn_801E4E44();
    lbl_8055EAB0 = 0;
}

int fn_801E5370(int max, int subdiv, f32 scale) {
    SkidSpline* s = &lbl_80487C88;
    lbl_8055EAA8 = 0;
    s->max = max;
    lbl_8055EAAC = 0;
    s->count = 0;
    s->subdiv = subdiv + 1;
    s->scale = scale;
    lbl_8055EAB0 = 3;
    return 1;
}

// Adds a ribbon edge (p - side, p + side) with tangent tan; when drawing, the span from the previous
// edge is tessellated with a Hermite curve.
void fn_801E53A8(const TVec3* p, const TVec3* tan, const TVec3* side, int draw, f32 alpha) {
    SkidSpline* s = &lbl_80487C88;
    if (s->count >= s->max) {
        fn_802E317C("tristrip: exceeded skid vert limit");
    }
    TVec3 a;
    TVec3 b;
    a.x = p->x - side->x;
    b.x = p->x + side->x;
    a.y = p->y - side->y;
    b.y = p->y + side->y;
    a.z = p->z - side->z;
    b.z = p->z + side->z;
    if (s->count > 0 && draw) {
        f32 len = fn_802C88D4(s);
        int n = s->subdiv;
        int m = (int)((f32)n - len * s->scale);
        int k = 1;
        if (m > 1) {
            k = m;
        }
        if (k < n) {
            n = k;
        }
        f32 step = 1.0f / (f32)n;
        if (lbl_8055EAA8 + n * 2 + 2 > 60 || lbl_8055EAAC + n * 6 > 174) {
            fn_801E4E44();
        }
        u32 c0 = (AlphaByte(s->alpha) << 24) | 0xFFFFFF;
        int nv = lbl_8055EAA8;
        TVtx* v = &lbl_80487578[nv];
        v->pos = s->p0;
        v->u = 0.0f;
        v->v = 0.0f;
        v->color = c0;
        v = &lbl_80487578[nv + 1];
        v->pos = s->p1;
        v->u = 1.0f;
        v->v = 0.0f;
        v->color = c0;
        nv += 2;
        lbl_8055EAA8 = nv;
        for (int i = 1; i <= n; i++) {
            f32 t = (f32)i * step;
            f32 t2 = t * t;
            f32 t3 = t2 * t;
            u32 c = (AlphaByte(s->alpha + t * (alpha - s->alpha)) << 24) | 0xFFFFFF;
            f32 h2 = t3 - 2.0f * t2 + t;
            f32 h4 = t3 - t2;
            TVec3 tp;
            tp.x = s->tan.x * h2 + tan->x * h4;
            tp.y = s->tan.y * h2 + tan->y * h4;
            tp.z = s->tan.z * h2 + tan->z * h4;
            f32 h1 = 1.0f + (2.0f * t3 - 3.0f * t2);
            f32 h3 = -2.0f * t3 + 3.0f * t2;
            v = &lbl_80487578[nv];
            v->pos.x = tp.x + s->p0.x * h1 + a.x * h3;
            v->pos.y = tp.y + s->p0.y * h1 + a.y * h3;
            v->pos.z = tp.z + s->p0.z * h1 + a.z * h3;
            v->u = 0.0f;
            v->v = t;
            v->color = c;
            nv++;
            AddTri(nv);
            h1 = 1.0f + (2.0f * t3 - 3.0f * t2);
            h3 = -2.0f * t3 + 3.0f * t2;
            v = &lbl_80487578[nv];
            v->pos.x = tp.x + s->p1.x * h1 + b.x * h3;
            v->pos.y = tp.y + s->p1.y * h1 + b.y * h3;
            v->pos.z = tp.z + s->p1.z * h1 + b.z * h3;
            v->u = 1.0f;
            v->v = t;
            v->color = c;
            nv++;
            AddTri(nv);
        }
        lbl_8055EAA8 = nv;
    }
    s->p0 = a;
    s->p1 = b;
    s->tan = *tan;
    s->alpha = alpha;
    s->count++;
}

void fn_801E58FC(void) {
    fn_801E4E44();
    lbl_8055EAB0 = 0;
}

}  // extern "C"
