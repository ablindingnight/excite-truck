// Car lights (0x80294970-0x80296058): head, tail and accessory light glows read from each car
// model's light nodes, drawn as camera-facing flares and car-aligned glow quads. Names are guesses
// apart from the node names and value keys.
#include "game/common.h"
#include "game/plainvec.h"
#include "game/vecmatrix.h"
#include "game/carlights.h"
#include "game/weather.h"
#include "macros.h"
#include <revolution/MTX.h>
#include "game/mathutil.h"
#include "game/vecmul.h"

// LightFrame: vecmatrix.h's VecMatrix.
struct LightCar {
    Vec axes[3];  // offset 0x0
    Vec pos;      // offset 0x24
    u8 pad30[0x68 - 0x30];
    u32 flags;  // offset 0x68
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
// WeatherDef: game/weather.h.
WeatherDef* fn_801E1F44(void);
f32 fn_801E88F0(s32 a);
s32 fn_80287B14(void);
LightCar* fn_80287B24(s32 i);
int CheckEffectKeyExists(s32 model, const char* name);
ValueNode* FindEffectNode(s32 model, const char* name);
int fn_80288070(s32 model, const char* name, const VecMatrix** frame);
s32 GetEffectInfo(s32 i);
const VecMatrix* fn_802C86DC(void);
f32 fn_802C88E8(const Vec* p);
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
void fn_802C8DB8(void* v, int nv, u16* idx, int ni);
s32 fn_802CAC28(const char* name, int flags);
void gxForgetTexture(s32 tex);
}




// PROVISIONAL helper name/matrix-input API and local declaration grouping; actual geometry unchanged.
static inline void Rotate(Vec* out, const Vec* axes, f32 x, f32 y, f32 z) {
    Vec v;
    SetVec(&v, x, y, z);
    MulVec(out, &v, axes);
}

static inline int ColorByte(f32 f) {
    int v = f;
    v = v > 0 ? v : 0;
    return v < 255 ? v : 255;
}

// Clamp written with the conversion repeated (the default-colour path): the target converts twice for the last
// channel and branches, where ColorByte keeps one int and clamps branch-free.
static inline int ColorByte2(f32 f) {  /* Helper name unproven */
    int v = (int)f > 0 ? (int)f : 0;
    return v < 255 ? v : 255;
}

static inline u32 MakeColor(u8 r, u8 g, u8 b) { return ARGB(0xFF, r, g, b); }

static const QuadIdx lbl_802FA9B0 = {{0, 1, 2, 1, 2, 3}};

s32 lbl_8055C990 = -1;  // flare texture
s32 lbl_8055C994 = -1;  // glow texture
ModuleTag lbl_8055F768;
LightSet lbl_804B42F0[8];

extern "C" {

void fn_80295CC4(LightSet* s, s32 model, const char* name, ValVec3 color, int unused, Vec off, f32 rad);

void fn_80294970(void) {
    LightSet* s = lbl_804B42F0;
    int i;
    for (i = 0; i < fn_80287B14(); s++, i++) {
        s->count = 0;
    }
    lbl_8055C990 = fn_802CAC28("lights.tex", 0);
    lbl_8055C994 = fn_802CAC28("glowz.tex", 0);
    if (fn_801E1F44()->night || fn_801E1F44()->precip == 1) {
        s = lbl_804B42F0;
        for (i = 0; i < fn_80287B14(); i++, s++) {
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
            s32 model = GetEffectInfo(i);
            fn_80295CC4(s, model, "left headlight", white, 0, front, 0.35f);
            fn_80295CC4(s, model, "right headlight", white, 0, front, 0.35f);
            fn_80295CC4(s, model, "left taillight", red, 0, back, 0.15f);
            fn_80295CC4(s, model, "right taillight", red, 0, back, 0.15f);
            const char* accFmt = "acc light %d";
            for (int j = 0; j < 8; j++) {
                char buf[0x40];
                fn_801345D8(buf, 0x40, accFmt, j + 1);
                fn_80295CC4(s, model, buf, acc, 0, up, 0.15f);
            }
        }
    }
}

void fn_80294C68(void) {
    gxForgetTexture(lbl_8055C990);
    gxForgetTexture(lbl_8055C994);
    lbl_8055C990 = -1;
    lbl_8055C994 = -1;
    for (int i = 0; i < fn_80287B14(); i++) {
    }
}

void fn_80294CC0(void) {}

void fn_80294CC4(void) {}

void fn_80294ED4(void);

void fn_80294CC8(int) { fn_80294ED4(); }

void fn_80294CCC(const Vec* p, const Vec* r, const Vec* u, f32 w, f32 h, u32 color, s32 type) {
    QuadIdx idx = lbl_802FA9B0;
    LightVtx v[4];
    // PROVISIONAL coupled source reconstruction; every scalar and seed is consumed.
    f32 negativeHeight;
    f32 lowerRightZ;
    f32 lowerRightY;
    f32 lowerOffsetZ;
    f32 rightAxisY;
    f32 lowerRightX;
    f32 rightAxisX;
    f32 lowerLeftZ;
    f32 rightScaledZ;
    f32 lowerLeftY;
    f32 lowerOffsetY;
    f32 leftScaledZ;
    f32 lowerLeftX;
    f32 centerZ;
    f32 upAxisZ;
    f32 lowerOffsetX;
    f32 upperRightZ;
    f32 upAxisY;
    f32 upperRightY;
    f32 upAxisX;
    f32 upperRightX;
    f32 rightEdgeZ;
    f32 upperOffsetY;
    f32 rightEdgeY;
    f32 centerY;
    f32 leftScaledY;
    f32 leftEdgeZ;
    f32 rightEdgeX;
    f32 upperLeftZ;
    f32 centerX;
    f32 upperLeftY;
    f32 upperLeftX;
    f32 upperOffsetX;
    f32 leftEdgeY;
    f32 rightAxisZ;
    f32 upperOffsetZ;
    f32 leftScaledX;
    f32 negativeWidth;
    f32 leftEdgeX;
    negativeWidth = -w;
    rightAxisX = r->x;
    rightAxisY = r->y;
    leftScaledX = rightAxisX * negativeWidth;
    leftScaledY = rightAxisY * negativeWidth;
    rightAxisZ = r->z;
    centerZ = p->z;
    rightAxisX *= w;
    leftScaledZ = rightAxisZ * negativeWidth;
    leftEdgeZ = centerZ + leftScaledZ;
    v[0].pos.z = leftEdgeZ;
    rightAxisY *= w;
    rightScaledZ = rightAxisZ * w;
    rightEdgeZ = centerZ + rightScaledZ;
    negativeHeight = -h;
    upAxisY = u->y;
    centerX = p->x;
    leftEdgeX = centerX + leftScaledX;
    centerY = p->y;
    leftEdgeY = centerY + leftScaledY;
    v[0].pos.y = leftEdgeY;
    v[0].pos.x = leftEdgeX;
    upAxisX = u->x;
    rightEdgeX = centerX + rightAxisX;
    upperOffsetY = upAxisY * h;
    v[1].pos.x = rightEdgeX;
    upAxisZ = u->z;
    rightEdgeY = centerY + rightAxisY;
    upperOffsetZ = upAxisZ * h;
    upperLeftZ = v[0].pos.z + upperOffsetZ;
    upperOffsetX = upAxisX * h;
    upperRightX = v[1].pos.x + upperOffsetX;
    lowerOffsetX = upAxisX * negativeHeight;
    lowerOffsetY = upAxisY * negativeHeight;
    lowerOffsetZ = upAxisZ * negativeHeight;
    v[1].pos.y = rightEdgeY;
    upperRightY = v[1].pos.y + upperOffsetY;
    v[1].pos.z = rightEdgeZ;
    v[2].pos.x = v[0].pos.x;
    lowerLeftX = v[2].pos.x + lowerOffsetX;
    upperLeftX = v[0].pos.x + upperOffsetX;
    upperLeftY = v[0].pos.y + upperOffsetY;
    upperRightZ = v[1].pos.z + upperOffsetZ;
    v[2].pos.y = v[0].pos.y;
    lowerLeftY = v[2].pos.y + lowerOffsetY;
    v[2].pos.z = v[0].pos.z;
    lowerLeftZ = v[2].pos.z + lowerOffsetZ;
    v[3].pos.x = v[1].pos.x;
    lowerRightX = v[3].pos.x + lowerOffsetX;
    v[3].pos.y = v[1].pos.y;
    lowerRightY = v[3].pos.y + lowerOffsetY;
    v[3].pos.z = v[1].pos.z;
    lowerRightZ = v[3].pos.z + lowerOffsetZ;
    v[0].pos.x = upperLeftX;
    v[0].pos.y = upperLeftY;
    v[0].pos.z = upperLeftZ;
    v[1].pos.x = upperRightX;
    v[1].pos.y = upperRightY;
    v[1].pos.z = upperRightZ;
    v[2].pos.x = lowerLeftX;
    v[2].pos.y = lowerLeftY;
    v[2].pos.z = lowerLeftZ;
    v[3].pos.x = lowerRightX;
    v[3].pos.y = lowerRightY;
    v[3].pos.z = lowerRightZ;
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
    const VecMatrix* cam = fn_802C86DC();
    Vec right = cam->basis[0];
    Vec up = cam->basis[1];
    LightSet* s;
    CarLight* l;
    LightCar* car;
    int i;
    int j;
    fn_802C8BD0(lbl_8055C990);
    s = lbl_804B42F0;
    for (i = 0; i < fn_80287B14(); i++, s++) {
        car = fn_80287B24(i);
        if (fn_802C88E8(&car->pos) * fn_801E88F0(-1) > maxDist || (car->flags & 8)) {
            continue;
        }
        const Vec* axes = car->axes;
        j = 0;
        if (j >= s->count) {
            continue;
        }
        l = s->lights;
        for (; j < s->count; l++, j++) {
            Vec p;
            MulVec(&p, &l->frame->origin, axes);
            p.x = p.x + car->pos.x;
            p.y = p.y + car->pos.y;
            p.z = p.z + car->pos.z;
            Vec d;
            d.x = p.x - cam->origin.x;
            d.y = p.y - cam->origin.y;
            d.z = p.z - cam->origin.z;
            if (PSVECDotProduct(&d, &d) > 0.1f) {
                PSVECNormalize(&d, &d);
                Vec dir;
                MulVec(&dir, &l->off, axes);
                f32 t = PSVECDotProduct(&d, &dir);
                t = MIN(1.0f, t);
                t = MAX(t, 0.0f);
                f32 k = 1.0f - t;
                Vec a;
                Rotate(&a, axes, 0.0f, 0.0f, 1.0f);
                Vec b;
                Rotate(&b, axes, 1.0f, 0.0f, 0.0f);
                int a1 = ColorByte(196.0f * (f32)__fabs(PSVECDotProduct(&d, &a)));
                int a2 = ColorByte(196.0f * (f32)__fabs(PSVECDotProduct(&d, &b)));
                u32 c1 = ARGB(a1, l->r, l->g, l->b);
                u32 c2 = ARGB(a2, l->r, l->g, l->b);
                Vec e;
                Vec g;
                Vec f;
                Rotate(&e, axes, 1.0f, 0.0f, 0.0f);
                Rotate(&f, axes, 0.0f, 0.0f, 1.0f);
                Rotate(&g, axes, 0.0f, 1.0f, 0.0f);
                fn_80294CCC(&p, &e, &g, (l->w + k * l->w) / 2.0f, (l->h + k * l->w) / 2.0f, c1, l->type);
                fn_80294CCC(&p, &f, &g, (l->w + k * l->w) / 2.0f, (l->h + k * l->w) / 2.0f, c2, l->type);
            }
        }
    }
    fn_802C8D74();
    fn_802C8BD0(lbl_8055C994);
    for (i = 0; i < fn_80287B14(); i++) {
        s = &lbl_804B42F0[i];
        LightCar* car = fn_80287B24(i);
        if (fn_802C88E8(&car->pos) * fn_801E88F0(-1) > maxDist || (car->flags & 8)) {
            continue;
        }
        const Vec* axes = car->axes;
        CarLight* l;
        const Vec* sourcePos;
        int j = 0;
        if (j >= s->count) {
            continue;
        }
        l = s->lights;
        for (; j < s->count; l++, j++) {
            Vec p;
            sourcePos = &l->frame->origin;
            MulVec(&p, sourcePos, axes);
            p.x = p.x + car->pos.x;
            p.y = p.y + car->pos.y;
            p.z = p.z + car->pos.z;
            Vec d;
            d.x = p.x - cam->origin.x;
            d.y = p.y - cam->origin.y;
            d.z = p.z - cam->origin.z;
            if (PSVECDotProduct(&d, &d) > 0.1f) {
                PSVECNormalize(&d, &d);
                Vec dir;
                MulVec(&dir, &l->off, axes);
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

void fn_80295C70(LightSet* s, const VecMatrix* frame, const Vec* off, f32 w, f32 h, u32 color, s32 type) {
    CarLight* l = &s->lights[s->count];
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

// PROVISIONAL private default-colour value name/copy API; all three RGB fields are consumed.
struct LightDefaultColor : ValVec3 {
    LightDefaultColor(const ValVec3& v) : ValVec3(v) {}
};

void fn_80295CC4(LightSet* s, s32 model, const char* name, ValVec3 color, int unused, Vec off, f32 rad) {
    const VecMatrix* frame;
    const LightDefaultColor def(color);
    // Start with the real width default; the property path updates it below.
    f32 w = rad;
    if (fn_80288070(model, name, &frame)) {
        ValueNode* n;
        f32 r;
        u32 c;
        f32 h;
        s32 type;
        if (CheckEffectKeyExists(model, name)) {
            n = FindEffectNode(model, name);
            const ValVec3& v = n->GetVec3Or("color", def);
            c = MakeColor(ColorByte(255.0f * v.x), ColorByte(255.0f * v.y), ColorByte(255.0f * v.z));
            r = n->GetFloat("rad", rad);
            w = n->GetFloat("width", -1.0f);
            h = n->GetFloat("height", -1.0f);
            if (w <= 0.0f || h <= 0.0f) {
                h = r;
                w = r;
            }
            type = n->GetInt("light_type", 0);
        } else {
            c = MakeColor(ColorByte2(255.0f * def.x), ColorByte(255.0f * def.y), ColorByte(255.0f * def.z));
            h = rad;
            type = 0;
        }
        fn_80295C70(s, frame, &off, w, h, c, type);
    }
}

}  // extern "C"
