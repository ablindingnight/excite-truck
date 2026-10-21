// Car lights (0x80294970-0x80296058): head, tail and accessory light glows read from each car
// model's light nodes, drawn as camera-facing flares and car-aligned glow quads. Names are guesses
// apart from the node names and value keys.
#include "game/common.h"
#include "macros.h"
#include <revolution/MTX.h>

struct LightFrame {
    Vec axes[3];  // offset 0x0
    Vec pos;      // offset 0x24
};

struct LightCar {
    Vec axes[3];  // offset 0x0
    Vec pos;      // offset 0x24
    u8 pad30[0x68 - 0x30];
    u32 flags;  // offset 0x68
};

struct Light {
    const LightFrame* frame;  // offset 0x0
    Vec off;                  // offset 0x4
    f32 h;                    // offset 0x10
    f32 w;                    // offset 0x14
    u8 r;                     // offset 0x18
    u8 g;                     // offset 0x19
    u8 b;                     // offset 0x1A
    s32 type;                 // offset 0x1C
};

class LightSet {
public:
    LightSet(void);
    Light lights[12];  // offset 0x0
    s32 count;         // offset 0x180
};

// GetVec3Or takes its default by value.
class ValueNodeV {
public:
    virtual const char* GetName(void);
    virtual int Has(const char* key);
    virtual int GetPair(int index, const char** key, const char** value);
    virtual const char* GetValue(const char* key);
    virtual const char* GetString(const char* key, const char* def);
    virtual s32 GetIntValue(const char* key);
    virtual s32 GetInt(const char* key, s32 def);
    virtual f32 GetFloat(const char* key);
    virtual f32 GetFloat(const char* key, f32 def);
    virtual s32 GetIntValue2(const char* key);
    virtual int GetBool(const char* key, int def);
    virtual ValVec3 GetVec3(const char* key);
    virtual ValVec3 GetVec3Or(const char* key, ValVec3 def);
};

struct LightVtx {
    Vec pos;    // offset 0x0
    u32 color;  // offset 0xC
    f32 s;      // offset 0x10
    f32 t;      // offset 0x14
};

struct QuadIdx {
    u16 i[6];
};

extern "C" {
int fn_801345D8(char* buf, int size, const char* fmt, ...);  // snprintf
struct WeatherDef { s32 precip; u8 night; };
WeatherDef* fn_801E1F44(void);
f32 fn_801E88F0(s32 a);
s32 fn_80287B14(void);
LightCar* fn_80287B24(s32 i);
int fn_80287E20(s32 model, const char* name);
ValueNodeV* fn_80287E80(s32 model, const char* name);
int fn_80288070(s32 model, const char* name, const LightFrame** frame);
s32 fn_80288208(s32 i);
const LightFrame* fn_802C86DC(void);
f32 fn_802C88E8(const Vec* p);
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
void fn_802C8DB8(void* v, int nv, u16* idx, int ni);
s32 fn_802CAC28(const char* name, int flags);
void fn_802CAC54(s32 tex);
}

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

static inline void SetVec(Vec* v, f32 x, f32 y, f32 z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

static inline void Rotate(Vec* out, const LightCar* c, f32 x, f32 y, f32 z) {
    Vec v;
    SetVec(&v, x, y, z);
    MulVec(out, &v, c->axes);
}

static inline int ColorByte(f32 f) {
    int v = f;
    v = v > 0 ? v : 0;
    return v < 255 ? v : 255;
}

static inline u32 MakeColor(int r, int g, int b) { return 0xFF000000 | (u8)r << 16 | (u8)g << 8 | (u8)b; }

static const QuadIdx lbl_802FA9B0 = {{0, 1, 2, 1, 2, 3}};

s32 lbl_8055C990 = -1;  // flare texture
s32 lbl_8055C994 = -1;  // glow texture
ModuleTag lbl_8055F768;
LightSet lbl_804B42F0[8];

extern "C" {

void fn_80295CC4(LightSet* s, s32 model, const char* name, ValVec3 color, int unused, Vec off, f32 rad);

void fn_80294970(void) {
    int i;
    LightSet* s = lbl_804B42F0;
    for (i = 0; i < fn_80287B14(); s++, i++) {
        s->count = 0;
    }
    lbl_8055C990 = fn_802CAC28("lights.tex", 0);
    lbl_8055C994 = fn_802CAC28("glowz.tex", 0);
    if (fn_801E1F44()->night || fn_801E1F44()->precip == 1) {
        for (i = 0; i < fn_80287B14(); i++) {
            ValVec3 white;
            SetVec((Vec*)&white, 1.0f, 1.0f, 1.0f);
            ValVec3 red;
            SetVec((Vec*)&red, 1.0f, 0.0f, 0.0f);
            ValVec3 acc;
            SetVec((Vec*)&acc, 1.0f, 1.0f, 1.0f);
            Vec front;
            SetVec(&front, 0.0f, 0.0f, 1.0f);
            Vec back;
            SetVec(&back, 0.0f, 0.0f, -1.0f);
            Vec up;
            SetVec(&up, 0.0f, 0.0f, 1.0f);
            s32 model = fn_80288208(i);
            fn_80295CC4(&lbl_804B42F0[i], model, "left headlight", white, 0, front, 0.35f);
            fn_80295CC4(&lbl_804B42F0[i], model, "right headlight", white, 0, front, 0.35f);
            fn_80295CC4(&lbl_804B42F0[i], model, "left taillight", red, 0, back, 0.15f);
            fn_80295CC4(&lbl_804B42F0[i], model, "right taillight", red, 0, back, 0.15f);
            for (int j = 0; j < 8; j++) {
                char buf[0x40];
                fn_801345D8(buf, 0x40, "acc light %d", j + 1);
                fn_80295CC4(&lbl_804B42F0[i], model, buf, acc, 0, up, 0.15f);
            }
        }
    }
}

void fn_80294C68(void) {
    fn_802CAC54(lbl_8055C990);
    fn_802CAC54(lbl_8055C994);
    lbl_8055C990 = -1;
    lbl_8055C994 = -1;
    for (int i = 0; i < fn_80287B14(); i++) {
    }
}

void fn_80294CC0(void) {}

void fn_80294CC4(void) {}

void fn_80294ED4(void);

void fn_80294CC8(void) { fn_80294ED4(); }

void fn_80294CCC(const Vec* p, const Vec* r, const Vec* u, f32 w, f32 h, u32 color, s32 type) {
    QuadIdx idx = lbl_802FA9B0;
    LightVtx v[4];
    f32 nw = -w;
    f32 lx = p->x + r->x * nw;
    f32 ly = p->y + r->y * nw;
    f32 lz = p->z + r->z * nw;
    f32 rx = p->x + r->x * w;
    f32 ry = p->y + r->y * w;
    f32 rz = p->z + r->z * w;
    f32 nh = -h;
    v[0].pos.x = lx + u->x * h;
    v[0].pos.y = ly + u->y * h;
    v[0].pos.z = lz + u->z * h;
    v[1].pos.x = rx + u->x * h;
    v[1].pos.y = ry + u->y * h;
    v[1].pos.z = rz + u->z * h;
    v[2].pos.x = lx + u->x * nh;
    v[2].pos.y = ly + u->y * nh;
    v[2].pos.z = lz + u->z * nh;
    v[3].pos.x = rx + u->x * nh;
    v[3].pos.y = ry + u->y * nh;
    v[3].pos.z = rz + u->z * nh;
    v[3].color = color;
    v[2].color = color;
    v[1].color = color;
    v[0].color = color;
    if (type >= 0) {
        f32 s0 = (type % 2) / 2.0f;
        f32 t0 = type / 2;
        v[0].s = v[2].s = s0;
        v[1].s = v[3].s = 0.5f + s0;
        v[0].t = v[1].t = t0;
        v[2].t = v[3].t = 1.0f + t0;
    } else {
        v[0].s = v[2].s = 0.0f;
        v[1].s = v[3].s = 1.0f;
        v[0].t = v[1].t = 0.0f;
        v[2].t = v[3].t = 1.0f;
    }
    fn_802C8DB8(v, 4, idx.i, 6);
}

void fn_80294ED4(void) {
    f32 maxDist = 40000.0f;
    const LightFrame* cam = fn_802C86DC();
    Vec right = cam->axes[0];
    Vec up = cam->axes[1];
    LightCar* car;
    int i;
    int j;
    Light* l;
    fn_802C8BD0(lbl_8055C990);
    LightSet* s = lbl_804B42F0;
    for (i = 0; i < fn_80287B14(); i++, s++) {
        car = fn_80287B24(i);
        if (fn_802C88E8(&car->pos) * fn_801E88F0(-1) > maxDist || (car->flags & 8)) {
            continue;
        }
        j = 0;
        if (j >= s->count) {
            continue;
        }
        l = s->lights;
        for (; j < s->count; l++, j++) {
            Vec p;
            MulVec(&p, &l->frame->pos, car->axes);
            p.x = p.x + car->pos.x;
            p.y = p.y + car->pos.y;
            p.z = p.z + car->pos.z;
            Vec d;
            d.x = p.x - cam->pos.x;
            d.y = p.y - cam->pos.y;
            d.z = p.z - cam->pos.z;
            if (PSVECDotProduct(&d, &d) > 0.1f) {
                PSVECNormalize(&d, &d);
                Vec dir;
                MulVec(&dir, &l->off, car->axes);
                f32 t = PSVECDotProduct(&d, &dir);
                t = MIN(1.0f, t);
                t = MAX(t, 0.0f);
                f32 k = 1.0f - t;
                Vec a;
                Rotate(&a, car, 0.0f, 0.0f, 1.0f);
                Vec b;
                Rotate(&b, car, 1.0f, 0.0f, 0.0f);
                int a1 = ColorByte(196.0f * (f32)__fabs(PSVECDotProduct(&d, &a)));
                int a2 = ColorByte(196.0f * (f32)__fabs(PSVECDotProduct(&d, &b)));
                u32 c1 = a1 << 24 | l->r << 16 | l->g << 8 | l->b;
                u32 c2 = a2 << 24 | l->r << 16 | l->g << 8 | l->b;
                Vec e;
                Vec g;
                Vec f;
                Rotate(&e, car, 1.0f, 0.0f, 0.0f);
                Rotate(&f, car, 0.0f, 0.0f, 1.0f);
                Rotate(&g, car, 0.0f, 1.0f, 0.0f);
                fn_80294CCC(&p, &e, &g, (l->w + k * l->w) / 2.0f, (l->h + k * l->w) / 2.0f, c1, l->type);
                fn_80294CCC(&p, &f, &g, (l->w + k * l->w) / 2.0f, (l->h + k * l->w) / 2.0f, c2, l->type);
            }
        }
    }
    fn_802C8D74();
    fn_802C8BD0(lbl_8055C994);
    s = lbl_804B42F0;
    for (i = 0; i < fn_80287B14(); i++, s++) {
        LightCar* car = fn_80287B24(i);
        if (fn_802C88E8(&car->pos) * fn_801E88F0(-1) > maxDist || (car->flags & 8)) {
            continue;
        }
        int j = 0;
        if (j >= s->count) {
            continue;
        }
        Light* l = s->lights;
        for (; j < s->count; l++, j++) {
            Vec p;
            MulVec(&p, &l->frame->pos, car->axes);
            p.x = p.x + car->pos.x;
            p.y = p.y + car->pos.y;
            p.z = p.z + car->pos.z;
            Vec d;
            d.x = p.x - cam->pos.x;
            d.y = p.y - cam->pos.y;
            d.z = p.z - cam->pos.z;
            if (PSVECDotProduct(&d, &d) > 0.1f) {
                PSVECNormalize(&d, &d);
                Vec dir;
                MulVec(&dir, &l->off, car->axes);
                f32 t = PSVECDotProduct(&d, &dir);
                t = MIN(1.0f, t);
                t = MAX(t, 0.0f);
                f32 k = 1.0f - t;
                fn_80294CCC(&p, &right, &up, (l->w + k * l->w) / 2.0f, (l->h + k * l->h) / 2.0f,
                            0x80000000 | l->r << 16 | l->g << 8 | l->b, -1);
            }
        }
    }
    fn_802C8D74();
}

void fn_80295C70(LightSet* s, const LightFrame* frame, const Vec* off, f32 w, f32 h, u32 color, s32 type) {
    Light* l = &s->lights[s->count];
    l->frame = frame;
    l->off = *off;
    l->h = h;
    l->w = w;
    l->r = color >> 16;
    l->g = color >> 8;
    l->b = color;
    l->type = type;
    s->count++;
}

void fn_80295CC4(LightSet* s, s32 model, const char* name, ValVec3 color, int unused, Vec off, f32 rad) {
    const LightFrame* frame;
    ValVec3 def = color;
    if (fn_80288070(model, name, &frame)) {
        u32 c;
        f32 w;
        f32 h;
        s32 type;
        if (fn_80287E20(model, name)) {
            ValueNodeV* n = fn_80287E80(model, name);
            const ValVec3& v = n->GetVec3Or("color", def);
            c = MakeColor(ColorByte(255.0f * v.x), ColorByte(255.0f * v.y), ColorByte(255.0f * v.z));
            f32 r = n->GetFloat("rad", rad);
            w = n->GetFloat("width", -1.0f);
            h = n->GetFloat("height", -1.0f);
            if (w <= 0.0f || h <= 0.0f) {
                h = r;
                w = r;
            }
            type = n->GetInt("light_type", 0);
        } else {
            c = MakeColor(ColorByte(255.0f * def.x), ColorByte(255.0f * def.y), ColorByte(255.0f * def.z));
            h = rad;
            w = rad;
            type = 0;
        }
        fn_80295C70(s, frame, &off, w, h, c, type);
    }
}

}  // extern "C"
