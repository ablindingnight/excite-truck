// Debug drawing (0x801E3640-0x801E45CC): a basis from a direction, wire boxes along a frame, arrow
// heads, camera-facing triangles, axis tripods and a scaled "sphere.mod" drawn at a point, all with the
// "white_zr.tex" texture. Names are guesses apart from the strings.
#include "game/common.h"

struct DVec3 {
    f32 x, y, z;
};

struct DBasis {
    DVec3 x;  // offset 0x0
    DVec3 y;  // offset 0xC
    DVec3 z;  // offset 0x18
};

// Three axes and a position.
struct DFrame : DBasis {
    DVec3 pos;  // offset 0x24
};

struct DVtx {
    f32 x, y, z;  // offset 0x0
    u32 color;    // offset 0xC
    f32 u, v;     // offset 0x10
};

struct DCam {
    DVec3 right;  // offset 0x0
    DVec3 up;     // offset 0xC
};

extern "C" {
void PSVECCrossProduct(const DVec3* a, const DVec3* b, DVec3* out);
f32 PSVECMag(const DVec3* v);
s32 fn_802CAC28(const char* name, int a);
void fn_802CAC54(s32 tex);
s32 fn_802C6FC0(const char* name, int a, int b);
void fn_802C6CF8(s32 model);
void fn_802C744C(s32 model, const DFrame* m);
void fn_802C7760(s32 model, f32* x0, f32* y0, f32* z0, f32* x1, f32* y1, f32* z1);
void fn_802C8BD0(s32 tex);
int fn_802C8DBC(int prim);
void fn_802C8E00(u32 color, int a, f32 x, f32 y, f32 z, f32 u, f32 v);
void fn_802C8E48(void);
void fn_802C8D74(void);
void fn_802C8DB8(DVtx* v, int nv, const u16* idx, int ni);
void fn_802C9904(void);
DCam* fn_802C86DC(void);
}

extern f32 lbl_80558DC4[];  // FLT_EPSILON

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void SetLength(DVec3* v, f32 len) {
    f32 m = PSVECMag(v);
    if ((f32)__fabs(m) <= lbl_80558DC4[0]) {
        return;
    }
    f32 s = len / m;
    v->x *= s;
    v->y *= s;
    v->z *= s;
}

static inline void MulVec(DVec3* out, const DVec3* in, const DFrame* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m->x.x, in->y, m->y.x, in->z, m->z.x);
        f32 y = Dot3(in->x, m->x.y, in->y, m->y.y, in->z, m->z.y);
        f32 z = Dot3(in->x, m->x.z, in->y, m->y.z, in->z, m->z.z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m->x.x, in->y, m->y.x, in->z, m->z.x);
        out->y = Dot3(in->x, m->x.y, in->y, m->y.y, in->z, m->z.y);
        out->z = Dot3(in->x, m->x.z, in->y, m->y.z, in->z, m->z.z);
    }
}

static inline void AddScaled(DVtx* o, const DVec3* a, const DVec3* b, f32 k) {
    o->x = a->x + b->x * k;
    o->y = a->y + b->y * k;
    o->z = a->z + b->z * k;
}

static inline u32 DbgColor(u8 a, u8 r, u8 g, u8 b) { return (a << 24) | (r << 16) | (g << 8) | b; }

ModuleTag lbl_8055EA58;
u32 lbl_8055EA60 = DbgColor(0xFF, 0xFF, 0, 0);
s32 lbl_8055EA64;
f32 lbl_8055EA68;
s32 lbl_8055EA6C;

extern "C" {

void fn_801E41D8(const DVec3* a, const DVec3* b, u32 color, f32 width);

void fn_801E3640(const DVec3* dir, DBasis* out) {
    DVec3 up;
    out->z.x = dir->x;
    out->z.y = dir->y;
    out->z.z = dir->z;
    if ((f32)__fabs(dir->x) < 0.01f && (f32)__fabs(dir->z) < 0.01f) {
        up.x = 0.0f;
        up.y = 0.0f;
        up.z = 1.0f;
    } else {
        up.x = 0.0f;
        up.y = 1.0f;
        up.z = 0.0f;
    }
    PSVECCrossProduct(&up, &out->z, &out->x);
    PSVECCrossProduct(&out->z, &out->x, &out->y);
}

u32 fn_801E36F0(u32 c) { return ((c >> 1) & 0x7F7F7F) | (c & 0xFF000000); }

// Wire box of the unit cube [-1,1]^3 placed by the frame: c0 for the -z face, c1 for the edges along z
// and the +z face.
void fn_801E370C(const DFrame* m, u32 c0, u32 c1) {
    fn_802C8BD0(lbl_8055EA6C);
    if (fn_802C8DBC(0x11)) {
        DVec3 pos;
        DVec3 dir;
        pos.x = 0.0f;
        pos.y = 0.0f;
        pos.z = 0.0f;
        for (int i = 0; i < 5; i++) {
            dir.x = (i + (i >> 1)) & 1 ? 1.0f : -1.0f;
            dir.y = i & 2 ? 1.0f : -1.0f;
            dir.z = -1.0f;
            MulVec(&pos, &dir, m);
            pos.x += m->pos.x;
            pos.y += m->pos.y;
            pos.z += m->pos.z;
            fn_802C8E00(c0, 1, pos.x, pos.y, pos.z, 0.0f, 0.0f);
            pos.x += m->z.x * 2.0f;
            pos.y += m->z.y * 2.0f;
            pos.z += m->z.z * 2.0f;
            fn_802C8E00(c1, 1, pos.x, pos.y, pos.z, 0.0f, 0.0f);
        }
        pos.x += m->x.x * 2.0f;
        pos.y += m->x.y * 2.0f;
        pos.z += m->x.z * 2.0f;
        fn_802C8E00(c1, 0, pos.x, pos.y, pos.z, 0.0f, 0.0f);
        pos.x = pos.x + m->y.x * 2.0f + m->x.x * -2.0f;
        pos.y = pos.y + m->y.y * 2.0f + m->x.y * -2.0f;
        pos.z = pos.z + m->y.z * 2.0f + m->x.z * -2.0f;
        fn_802C8E00(c1, 1, pos.x, pos.y, pos.z, 0.0f, 0.0f);
        pos.x += m->x.x * 2.0f;
        pos.y += m->x.y * 2.0f;
        pos.z += m->x.z * 2.0f;
        fn_802C8E00(c1, 1, pos.x, pos.y, pos.z, 0.0f, 0.0f);
        pos.x += m->z.x * -2.0f;
        pos.y += m->z.y * -2.0f;
        pos.z += m->z.z * -2.0f;
        fn_802C8E00(c0, 0, pos.x, pos.y, pos.z, 0.0f, 0.0f);
        pos.x += m->x.x * -2.0f;
        pos.y += m->x.y * -2.0f;
        pos.z += m->x.z * -2.0f;
        fn_802C8E00(c0, 0, pos.x, pos.y, pos.z, 0.0f, 0.0f);
        pos.x = pos.x + m->y.x * -2.0f + m->x.x * 2.0f;
        pos.y = pos.y + m->y.y * -2.0f + m->x.y * 2.0f;
        pos.z = pos.z + m->y.z * -2.0f + m->x.z * 2.0f;
        fn_802C8E00(c0, 1, pos.x, pos.y, pos.z, 0.0f, 0.0f);
        pos.x += m->x.x * -2.0f;
        pos.y += m->x.y * -2.0f;
        pos.z += m->x.z * -2.0f;
        fn_802C8E00(c0, 1, pos.x, pos.y, pos.z, 0.0f, 0.0f);
        fn_802C8E48();
    }
    fn_802C8D74();
}

void fn_801E3C04(void) {
    f32 x0, y0, x1, y1, z0, z1;
    lbl_8055EA6C = fn_802CAC28("white_zr.tex", 0);
    lbl_8055EA64 = fn_802C6FC0("sphere.mod", 0, 0);
    fn_802C7760(lbl_8055EA64, &x0, &y0, &z0, &x1, &y1, &z1);
    lbl_8055EA68 = (f32)__fabs(y1 - y0) / 2;
}

void fn_801E3C88(void) {
    fn_802CAC54(lbl_8055EA6C);
    fn_802C6CF8(lbl_8055EA64);
}

// Arrow head (a four-sided pyramid) at pos pointing along dir.
void fn_801E3CB4(const DVec3* pos, const DVec3* dir, u32 color, f32 size) {
    DBasis b;
    DVtx v[5];
    fn_801E3640(dir, &b);
    f32 w = 0.66f * size;
    SetLength(&b.z, -size);
    SetLength(&b.x, w / 2);
    SetLength(&b.y, w / 2);
    v[0].x = 0.0f;
    v[0].y = 0.0f;
    v[0].z = 0.0f;
    AddScaled(&v[1], &b.z, &b.x, -1.0f);
    AddScaled(&v[2], &b.z, &b.x, 1.0f);
    AddScaled(&v[3], &b.z, &b.y, -1.0f);
    AddScaled(&v[4], &b.z, &b.y, 1.0f);
    u32 dark = fn_801E36F0(color);
    for (int i = 0; i < 5; i++) {
        v[i].color = color;
        v[i].u = 0.1f * (f32)i;
        v[i].v = 0.1f * (f32)i;
        v[i].x = v[i].x + pos->x;
        v[i].y = v[i].y + pos->y;
        v[i].z = v[i].z + pos->z;
    }
    v[0].color = dark;
    u16 idx[6] = {0, 1, 3, 0, 3, 2};
    fn_802C8BD0(lbl_8055EA6C);
    fn_802C8DB8(v, 5, idx, 6);
    fn_802C8D74();
    fn_802C9904();
}

void fn_801E3FC8(const DVec3* pos, f32 radius) {
    DFrame m;
    f32 s = radius / lbl_8055EA68;
    m.x.x = s;
    m.x.y = 0.0f;
    m.x.z = 0.0f;
    m.y.x = 0.0f;
    m.y.y = s;
    m.y.z = 0.0f;
    m.z.x = 0.0f;
    m.z.y = 0.0f;
    m.z.z = s;
    m.pos.x = pos->x;
    m.pos.y = pos->y;
    m.pos.z = pos->z;
    fn_802C744C(lbl_8055EA64, &m);
}

// Box of the given width from a to b.
void fn_801E4040(const DVec3* a, const DVec3* b, u32 c0, u32 c1, f32 width) {
    DVec3 d;
    DFrame f;
    d.x = b->x - a->x;
    d.y = b->y - a->y;
    d.z = b->z - a->z;
    fn_801E3640(&d, &f);
    f.z.x = d.x / 2;
    f.z.y = d.y / 2;
    f.z.z = d.z / 2;
    SetLength(&f.x, width / 2);
    SetLength(&f.y, width / 2);
    f.pos.x = a->x + f.z.x;
    f.pos.y = a->y + f.z.y;
    f.pos.z = a->z + f.z.z;
    fn_801E370C(&f, c0, c1);
}

void fn_801E41D8(const DVec3* a, const DVec3* b, u32 color, f32 width) { fn_801E4040(a, b, color, color, width); }

// Camera-facing triangle around pos.
void fn_801E41E0(const DVec3* pos, u32 color, f32 size) {
    DVtx t;
    DVtx v[3];
    t.v = 0.0f;
    t.u = 0.0f;
    t.color = color;
    v[0] = t;
    v[1] = t;
    v[2] = t;
    u16 idx[3] = {0, 1, 2};
    DCam* cam = fn_802C86DC();
    v[0].x = pos->x + cam->up.x * size;
    v[0].y = pos->y + cam->up.y * size;
    v[0].z = pos->z + cam->up.z * size;
    v[1].x = pos->x + cam->up.x * (0.5f * -size);
    v[1].y = pos->y + cam->up.y * (0.5f * -size);
    v[1].z = pos->z + cam->up.z * (0.5f * -size);
    v[1].x += cam->right.x * (0.866f * size);
    v[1].y += cam->right.y * (0.866f * size);
    v[1].z += cam->right.z * (0.866f * size);
    v[2].x = pos->x + cam->up.x * (0.5f * -size);
    v[2].y = pos->y + cam->up.y * (0.5f * -size);
    v[2].z = pos->z + cam->up.z * (0.5f * -size);
    v[2].x += cam->right.x * (0.866f * -size);
    v[2].y += cam->right.y * (0.866f * -size);
    v[2].z += cam->right.z * (0.866f * -size);
    fn_802C8BD0(lbl_8055EA6C);
    fn_802C8DB8(v, 3, idx, 3);
    fn_802C8D74();
    fn_802C9904();
}

// Red/green/blue axis bars of the frame.
void fn_801E43BC(const DFrame* m, f32 len, f32 width) {
    DVec3 p;
    p.x = m->pos.x + m->x.x * len;
    p.y = m->pos.y + m->x.y * len;
    p.z = m->pos.z + m->x.z * len;
    fn_801E41D8(&m->pos, &p, 0xFFFF0000, width);
    p.x = m->pos.x + m->y.x * len;
    p.y = m->pos.y + m->y.y * len;
    p.z = m->pos.z + m->y.z * len;
    fn_801E41D8(&m->pos, &p, 0xFF00FF00, width);
    p.x = m->pos.x + m->z.x * len;
    p.y = m->pos.y + m->z.y * len;
    p.z = m->pos.z + m->z.z * len;
    fn_801E41D8(&m->pos, &p, 0xFF0000FF, width);
}

void fn_801E44F4(const DFrame* m, f32 sx, f32 sy, f32 sz, u32 color) {
    DFrame f;
    f.pos = m->pos;
    f.x.x = m->x.x * sx;
    f.x.y = m->x.y * sx;
    f.x.z = m->x.z * sx;
    f.y.x = m->y.x * sy;
    f.y.y = m->y.y * sy;
    f.y.z = m->y.z * sy;
    f.z.x = m->z.x * sz;
    f.z.y = m->z.y * sz;
    f.z.z = m->z.z * sz;
    fn_801E370C(&f, color, color);
}

}  // extern "C"
