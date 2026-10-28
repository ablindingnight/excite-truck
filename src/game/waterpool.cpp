// Water pools (0x8029C1C4-0x8029EF70): the flat water plane, rippling 3D pools (a damped wave
// height field drawn as an environment-mapped strip mesh) and flat shallows, with wheel and body
// splashes from the cars. Names are guesses apart from the file names, value keys and messages.
#include "game/common.h"
#include "macros.h"
#include <revolution/MTX.h>

struct WaterCar {
    Vec axes[3];  // offset 0x0
    Vec pos;      // offset 0x24
    u8 pad30[0x34 - 0x30];
    Vec vel;  // offset 0x34
    u8 pad40[0x50 - 0x40];
    f32 speed;  // offset 0x50
    u8 pad54[0x70 - 0x54];
    struct {
        Vec pos;  // offset 0x0
        u8 padC[0x2C - 0xC];
        u32 flags;  // offset 0x2C
        u8 pad30[0x3C - 0x30];
    } wheels[4];  // offset 0x70
};

// One car in the per-frame car list.
struct WaterCarEntry {
    WaterCar* car;  // offset 0x0
    u32 index;      // offset 0x4
    f32 dist;       // offset 0x8
    u8 active;      // offset 0xC
};

struct ModelInfo {
    u8 pad0[0x1C];
    f32 radius;  // offset 0x1C
    Vec center;  // offset 0x20
};

struct PoolDesc {
    f32 scale;      // offset 0x0
    f32 visc;       // offset 0x4
    f32 rate;       // offset 0x8
    f32 damping;    // offset 0xC
    f32 amount;     // offset 0x10
    f32 unk14;      // offset 0x14
    u32 color;      // offset 0x18
    u8 minAlpha;    // offset 0x1C
    u8 maxAlpha;    // offset 0x1D
};

// Origin plus the in-plane right and up extents (x, z).
struct PoolGeom {
    ValVec3 origin;  // offset 0x0
    ValPoint right;  // offset 0xC
    ValPoint up;     // offset 0x14
};

struct PoolVtx {
    Vec pos;    // offset 0x0
    u32 color;  // offset 0xC
    f32 s;      // offset 0x10
    f32 t;      // offset 0x14
};

class WaterPool {
public:
    WaterPool(const PoolDesc* d, const PoolGeom* g, const char* tex);
    ~WaterPool(void);

    s16* buf;          // offset 0x0
    s16* cur;          // offset 0x4
    s16* prev;         // offset 0x8
    s32 nx;            // offset 0xC
    s32 ny;            // offset 0x10
    PoolDesc desc;     // offset 0x14
    f32 timer;         // offset 0x34
    Vec origin;        // offset 0x38
    f32 dxx;           // offset 0x44
    f32 dxz;           // offset 0x48
    f32 dyx;           // offset 0x4C
    f32 dyz;           // offset 0x50
    Vec center;        // offset 0x54
    f32 radius;        // offset 0x60
    Vec eye;           // offset 0x64
    PoolVtx* verts;    // offset 0x70
    s32 tex;           // offset 0x74
};

class WaterShallow {
public:
    WaterShallow(const PoolGeom* g, const char* tex);
    ~WaterShallow(void);

    Vec corners[4];  // offset 0x0
    Vec center;      // offset 0x30
    f32 radius;      // offset 0x3C
    s32 tex;         // offset 0x40
};

// The water editing interface (only its destructor and accessors survive).
class WaterIface {
public:
    virtual ~WaterIface(void);
    virtual s32 Name(void) { return name; }
    virtual s32 Data(void) { return data; }
    virtual int Unk14(void) { return 0; }
    virtual void Unk18(void) { Begin(); }
    virtual void Unk1C(void) { End(); }
    virtual void Show(void) { shown = 1; }
    virtual void Hide(void) { shown = 0; }
    virtual u8 Shown(void) { return shown; }
    void Close(void);
    void Begin(void);
    void End(void);

    u8 pad4[0x10 - 0x4];
    s32 name;  // offset 0x10
    u8 pad14[0x64 - 0x14];
    s32 data;  // offset 0x64
    u8 shown;  // offset 0x68
};

extern "C" {
int fn_8012C33C(const char* name);
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* t);
void fn_8014E740(const Vec* c, f32 r);
f32 fn_801E7E24(void);
f32 fn_801E88F0(s32 a);
s32 fn_801EE0C8(void* model);
void fn_80287B38(void (*cb)(WaterCarEntry* e, int n));
int fn_80287B94(int lo, int hi);
f32 fn_80287BC8(f32 lo, f32 hi);
void* fn_8028E2F8(void* desc, const char* name, const char* tex);
void fn_8028E398(void* e);
void fn_8028E584(void* e, const Vec* p, const Vec* v);
void fn_8028E590(void* e, const Vec* p, const Vec* v, f32 spin);
void fn_8028E61C(void* desc);
ValueNode* fn_802AE414(const char* name);
int fn_802BF4F0(void);
f32 fn_802BF590(void);
ModelInfo* fn_802C6C34(s32 model);
void fn_802C6CF8(s32 model);
s32 fn_802C6FC0(const char* name, s32 inst, const char* prefix);
void fn_802C7040(s32 model);
void fn_802C8230(f32 a);
void fn_802C8234(f32 a);
const struct WaterFrame* fn_802C86DC(void);
f32 fn_802C88D4(const Vec* p);
f32 fn_802C88E8(const Vec* p);
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
int fn_802C8DBC(int n);
void fn_802C8E00(u32 color, int idx, f32 x, f32 y, f32 z, f32 u, f32 v);
void fn_802C8E48(void);
int fn_802C9640(const Vec* c, f32 r);
s32 fn_802CAC28(const char* name, int flags);
void fn_802CAC54(s32 tex);
void fn_802E317C(const char* fmt, ...);
double sqrt(double x);
f32 PSVECDistance(const Vec* a, const Vec* b);
void* memset(void* dst, int c, size_t n);
}

struct WaterFrame {
    Vec axes[3];  // offset 0x0
    Vec pos;      // offset 0x24
};

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

static inline void SetVec(Vec* v, f32 x, f32 y, f32 z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

static inline f32 MinF(f32 a, f32 b) { return a < b ? a : b; }
static inline f32 MaxF(f32 a, f32 b) { return a > b ? a : b; }

static inline int InRange(f32 v, f32 lo, f32 hi) {
    int in = 0;
    if (v >= lo - lbl_80558DC4[0] && v <= hi + lbl_80558DC4[0]) {
        in = 1;
    }
    return in;
}

ModuleTag lbl_8055F7E8;
s32 lbl_8055CAA8 = -1;      // flat water model
WaterPool* lbl_804B5010[8];
WaterShallow* lbl_804B5030[16];
s32 lbl_8055F7F0;           // pool count
s32 lbl_8055F7F4;           // shallow count
void* lbl_8055F7F8;         // splash emitter
s32 lbl_8055F7FC;

// Editing interface hooks; dead-stripped from the game but their strings remain.
extern "C" void fn_8029C1C0(ValueNode* n, WaterIface* w) {
    fn_802E317C("water iface");
    fn_802E317C("Flat Water");
    n->GetReal("amplitude");
    n->GetReal("viscosity");
    n->GetReal("disturbance_rate");
    n->GetReal("disturbance_amount");
    n->GetReal("min_alpha");
    n->GetReal("max_alpha");
    fn_802AE414("flat_water");
}

extern "C" void fn_8029C570(void);
extern "C" void fn_8029C650(void);
extern "C" void fn_8029C664(void);
extern "C" void fn_8029C73C(void);
extern "C" void fn_8029C934(void);
extern "C" void fn_8029C998(void);
extern "C" void fn_8029CB3C(void);
extern "C" void fn_8029CBA0(void);
extern "C" void fn_8029CD14(void);
extern "C" void fn_8029CD48(WaterCar* car);
extern "C" void fn_8029C450(WaterCarEntry* e, int n);
extern "C" void fn_8029C3EC(void);

extern "C" {
int fn_8029D4CC(WaterPool* p);
void fn_8029D550(WaterPool* p, Vec* c, f32* r);
void fn_8029D574(WaterPool* p);
void fn_8029D644(WaterPool* p);
void fn_8029D7D0(WaterPool* p, const WaterCarEntry* e);
void fn_8029E8E0(WaterShallow* s);
void fn_8029EA34(WaterShallow* s, const WaterCarEntry* e);
int fn_8029EAE8(WaterShallow* s);

void fn_8029C1C4(void) {
    lbl_8055F7FC = 0;
    fn_8029C570();
    fn_8029C73C();
    fn_8029C998();
    fn_8029CBA0();
    fn_8029C664();
    if (fn_802BF4F0() || lbl_8055F7F0 > 0 || lbl_8055F7F4 > 0) {
        fn_80287B38(fn_8029C450);
    }
}

void fn_8029C22C(void) {
    fn_8029CD14();
    fn_8029CB3C();
    fn_8029C934();
    fn_8029C650();
    fn_8029C3EC();
}

void fn_8029C25C(void) {
    if ((f32)__fabs(fn_801E7E24()) <= lbl_80558DC4[0]) {
        return;
    }
    for (int i = 0; i < lbl_8055F7F0; i++) {
        fn_8029D574(lbl_804B5010[i]);
    }
}

void fn_8029C2D4(void) {
    if (lbl_8055CAA8 != -1) {
        fn_802C7040(lbl_8055CAA8);
    }
    WaterPool* best = 0;
    f32 bestDist = 0.0f;
    int i;
    for (i = 0; i < lbl_8055F7F0; i++) {
        WaterPool* p = lbl_804B5010[i];
        if (fn_8029D4CC(p)) {
            Vec c;
            f32 r;
            fn_8029D550(p, &c, &r);
            f32 d = fn_802C88D4(&c) - r;
            if (!best || d < bestDist) {
                best = p;
                bestDist = d;
            }
        }
    }
    if (best) {
        fn_8029D644(best);
    }
    for (int k = 0; k < lbl_8055F7F4; k++) {
        if (fn_8029EAE8(lbl_804B5030[k])) {
            fn_8029E8E0(lbl_804B5030[k]);
        }
    }
}

}  // extern "C"

extern "C" void fn_8029C3EC(void) {}

WaterIface::~WaterIface(void) { Close(); }

extern "C" void fn_8029C450(WaterCarEntry* e, int n) {
    f32 far = 2500.0f;
    if ((f32)__fabs(fn_801E7E24()) <= lbl_80558DC4[0]) {
        return;
    }
    for (int i = 0; i < n; e++, i++) {
        if (e->dist < far && e->car->speed > 2.5f && e->active) {
            for (int j = 0; j < lbl_8055F7F0; j++) {
                fn_8029D7D0(lbl_804B5010[j], e);
            }
            for (int j = 0; j < lbl_8055F7F4; j++) {
                fn_8029EA34(lbl_804B5030[j], e);
            }
            if (fn_802BF4F0()) {
                fn_8029CD48(e->car);
            }
        }
    }
}

extern "C" void fn_8029C570(void) {
    f32 period = 2.0f;
    f32 amplitude = 0.025f;
    const char* table = "flat_water";
    const char* model = "flatwtr.mod";
    if (fn_8012C33C(model)) {
        lbl_8055CAA8 = fn_802C6FC0(model, 0, 0);
    } else {
        lbl_8055CAA8 = -1;
    }
    ValueNode* n = fn_802AE414(table);
    if (n) {
        period = n->GetReal("period");
        amplitude = n->GetReal("amplitude");
    }
    fn_802C8230(period);
    fn_802C8234(amplitude);
}

extern "C" void fn_8029C650(void) {
    if (lbl_8055CAA8 != -1) {
        fn_802C6CF8(lbl_8055CAA8);
    }
}

extern "C" void fn_8029C664(void) {
    WaterPool** p = lbl_804B5010;
    WaterPool** q;
    for (int i = 0; i < lbl_8055F7F0; p++, i++) {
        Vec c;
        f32 r;
        fn_8029D550(*p, &c, &r);
        q = &lbl_804B5010[i + 1];
        for (int j = i + 1; j < lbl_8055F7F0; q++, j++) {
            Vec c2;
            f32 r2;
            fn_8029D550(*q, &c2, &r2);
            if (PSVECDistance(&c, &c2) < r2 + (250.0f + r)) {
                fn_802E317C("water: pools are too close");
            }
        }
    }
}

extern "C" void fn_8029C73C(void) {
    lbl_8055F7F0 = 0;
    ValueTable* t = fn_80131810("water3d.val");
    if (t) {
        PoolDesc d;
        memset(&d, 0, sizeof(d));
        d.scale = 0.25f;
        d.visc = 0.0f;
        d.rate = 0.0f;
        d.damping = 0.9875f;
        d.amount = 0.05f;
        d.unk14 = 0.05f;
        d.color = -1;
        d.minAlpha = 0x20;
        d.maxAlpha = 0xE4;
        for (int i = 0; t->GetChild(i); i++) {
            if (lbl_8055F7F0 >= 8) {
                fn_802E317C("water: exceeded pool limit (%d)", 8);
            }
            ValueNode* n = t->GetChild(i);
            const char* tex = n->GetValue("texture_name");
            PoolGeom g;
            g.origin = n->GetVec3("origin");
            g.right = n->GetPoint("right");
            g.up = n->GetPoint("up");
            WaterPool* p = new ("water pool") WaterPool(&d, &g, tex);
            lbl_804B5010[lbl_8055F7F0] = p;
            lbl_8055F7F0++;
        }
        fn_80131818(t);
    }
}

extern "C" void fn_8029C934(void) {
    for (int i = 0; i < lbl_8055F7F0; i++) {
        delete lbl_804B5010[i];
    }
    lbl_8055F7F0 = 0;
}

extern "C" void fn_8029C998(void) {
    lbl_8055F7F4 = 0;
    ValueTable* t = fn_80131810("water2d.val");
    if (t) {
        for (int i = 0; t->GetChild(i); i++) {
            if (lbl_8055F7F4 >= 16) {
                fn_802E317C("water: exceeded shallow limit (%d)", 16);
            }
            ValueNode* n = t->GetChild(i);
            const char* tex = n->GetValue("texture_name");
            PoolGeom g;
            g.origin = n->GetVec3("origin");
            g.right = n->GetPoint("right");
            g.up = n->GetPoint("up");
            WaterShallow* s = new ("water shallow") WaterShallow(&g, tex);
            lbl_804B5030[lbl_8055F7F4] = s;
            lbl_8055F7F4++;
        }
        fn_80131818(t);
    }
}

extern "C" void fn_8029CB3C(void) {
    for (int i = 0; i < lbl_8055F7F4; i++) {
        delete lbl_804B5030[i];
    }
    lbl_8055F7F4 = 0;
}

struct SplashColor {
    SplashColor(void) {}
    SplashColor(f32 a, f32 b, f32 c) {
        r = a;
        g = b;
        b_ = c;
    }
    f32 r;
    f32 g;
    f32 b_;
};

struct EmitterDesc {
    s32 kind;         // offset 0x0
    s32 max;          // offset 0x4
    f32 size0;        // offset 0x8
    f32 size1;        // offset 0xC
    SplashColor c0;   // offset 0x10
    SplashColor c1;   // offset 0x1C
    f32 gravity;      // offset 0x28
    f32 unk2C;        // offset 0x2C
    f32 life;         // offset 0x30
    f32 drag;         // offset 0x34
    u8 pad38[0x44 - 0x38];
    s32 frames;       // offset 0x44
    f32 shading;      // offset 0x48
    u8 pad4C[0x50 - 0x4C];
    f32 grow;         // offset 0x50
    f32 fade;         // offset 0x54
    f32 spinMin;      // offset 0x58
    f32 spinMax;      // offset 0x5C
    u8 pad60[0x64 - 0x60];
    f32 a64;          // offset 0x64
    u8 pad68[0x6C - 0x68];
    f32 a6C;          // offset 0x6C
    u8 pad70[0x74 - 0x70];
    f32 a74;          // offset 0x74
    u8 pad78[0x7C - 0x78];
    f32 a7C;          // offset 0x7C
    u8 pad80[0x90 - 0x80];
};

extern "C" void fn_8029CBA0(void) {
    const char* tex = "splash.tex";
    f32 shading = 0.75f;
    ValueNode* n = fn_802AE414("water");
    if (n) {
        tex = n->GetString("splash_texture", tex);
        shading = n->GetFloat("shading", shading);
    }
    if (fn_802BF4F0() || lbl_8055F7F0 > 0 || lbl_8055F7F4 > 0) {
        EmitterDesc e;
        fn_8028E61C(&e);
        e.kind = 2;
        e.max = 0x200;
        e.size0 = 0.75f;
        e.size1 = 0.75f;
        e.c0 = SplashColor(1.0f, 1.0f, 1.0f);
        e.c1 = SplashColor(1.0f, 1.0f, 1.0f);
        e.gravity = 0.5f;
        e.life = 0.75f;
        e.drag = 0.5f;
        e.frames = 5;
        e.shading = shading;
        e.fade = 0.75f;
        e.grow = 0.25f;
        e.spinMax = 1.0f;
        e.spinMin = -1.0f;
        e.a74 = 1.0f;
        e.a6C = 1.0f;
        e.a64 = 1.0f;
        e.a7C = 1.0f;
        lbl_8055F7F8 = fn_8028E2F8(&e, "splash", tex);
    } else {
        lbl_8055F7F8 = 0;
    }
}

extern "C" void fn_8029CD14(void) {
    if (lbl_8055F7F8) {
        fn_8028E398(lbl_8055F7F8);
        lbl_8055F7F8 = 0;
    }
}

static inline f32 SqrtF(f32 v) { return (f32)sqrt(v); }

extern "C" void fn_8029CD48(WaterCar* car) {
    for (int i = 0; i < 4; i++) {
        if (car->wheels[i].flags & 8) {
            continue;
        }
        const Vec* wp = &car->wheels[i].pos;
        Vec p;
        MulVec(&p, wp, car->axes);
        p.x = p.x + car->pos.x;
        p.y = p.y + car->pos.y;
        p.z = p.z + car->pos.z;
        if (p.y > fn_802BF590()) {
            continue;
        }
        p.y += 0.5f;
        f32 side = wp->x > 0.0f ? 1.0f : -1.0f;
        f32 kick = 0.2f + 0.1f * (car->speed * side);
        f32 up = 0.2f + 0.1f * car->speed;
        Vec v;
        v.x = car->vel.x + car->axes[0].x * kick;
        v.y = car->vel.y + car->axes[0].y * kick;
        v.z = car->vel.z + car->axes[0].z * kick;
        up = 5.0f < up ? 5.0f : up;
        ValPoint flat;
        flat.x = car->vel.x;
        flat.y = car->vel.z;
        v.y = v.y + car->axes[1].y * up;
        v.x = v.x + car->axes[1].x * up;
        v.z = v.z + car->axes[1].z * up;
        f32 s = SqrtF(flat.x * flat.x + flat.y * flat.y);
        int n = (int)(0.25f * s) < 6 ? (int)(0.25f * s) : 6;
        f32 spread = 0.125f * s;
        for (int k = 0; k < n; k++) {
            Vec q = p;
            q.x = q.x + fn_80287BC8(-0.5f, 0.5f);
            q.z = q.z + fn_80287BC8(-0.5f, 0.5f);
            q.y = fn_802BF590();
            f32 a = 2.0f < spread ? 2.0f : spread;
            Vec w = v;
            w.x = w.x + fn_80287BC8(-a, a);
            w.y = 2.0f * fn_80287BC8(0.0f, a);
            w.z = w.z + fn_80287BC8(-a, a);
            f32 dir = wp->x > 0.0f ? 1.0f : -1.0f;
            fn_8028E590(lbl_8055F7F8, &q, &w, -dir * fn_80287BC8(0.0f, 3.1415927f));
        }
    }
}

WaterPool::WaterPool(const PoolDesc* d, const PoolGeom* g, const char* texName) {
    memset(&desc, 0, sizeof(desc));
    f32 rightY = g->right.y;
    nx = 1.333f * (f32)sqrt(g->right.x * g->right.x + rightY * g->right.y);
    ny = 1.333f * (f32)sqrt(g->up.x * g->up.x + g->up.y * g->up.y);
    if (nx < 3 || ny < 3 || nx * ny > 0x200) {
        fn_802E317C("water: too small or too big (%dx%d)", nx, ny);
    }
    buf = new ("water mesh") s16[ny * (nx * 2)];
    memset(buf, 0, ny * (nx * 4));
    cur = buf;
    prev = buf + nx * ny;
    desc = *d;
    desc.color &= 0xFFFFFF;
    timer = 0.0f;
    origin.x = g->origin.x;
    origin.y = g->origin.y;
    origin.z = g->origin.z;
    f32 ix = 1.0f / (nx - 1);
    dxx = g->right.x * ix;
    f32 iy = 1.0f / (ny - 1);
    dxz = g->right.y * ix;
    dyx = g->up.x * iy;
    dyz = g->up.y * iy;
    center.x = g->origin.x + 0.5f * (g->right.x + g->up.x);
    center.y = g->origin.y;
    center.z = g->origin.z + 0.5f * (g->right.y + g->up.y);
    radius = PSVECDistance(&center, &origin);
    verts = new ("water vcache") PoolVtx[nx * 2];
    tex = fn_802CAC28(texName, 0);
    Vec r;
    r.x = g->right.x;
    r.y = 0.0f;
    r.z = g->right.y;
    Vec u;
    u.x = g->up.x;
    u.y = 0.0f;
    u.z = g->up.y;
    Vec c;
    c.x = g->origin.x + r.x * 0.5f + u.x * 0.5f;
    c.y = g->origin.y + r.y * 0.5f + u.y * 0.5f;
    c.z = g->origin.z + r.z * 0.5f + u.z * 0.5f;
    fn_8014E740(&c, radius);
}

// More editing interface hooks.
extern "C" void fn_8029D460(ValueNode* n, f32 t, f32 x, f32 y) {
    fn_802E317C("theta %f", t);
    fn_802E317C("origin %f %f", x, y);
    fn_802E317C("origin = (%0.2f, %0.2f), theta = %0.2f", x, y, t);
    fn_802E317C("Unknown water command");
    fn_802E317C("water: tried to create editing interface in non-edit mode");
}

WaterPool::~WaterPool(void) {
    fn_802CAC54(tex);
    delete[] verts;
    delete[] buf;
}

extern "C" {

int fn_8029D4CC(WaterPool* p) {
    f32 z = fn_801E88F0(-1);
    z = z * z;
    f32 s = fn_802C88E8(&p->center);
    s *= z;
    f32 lim = (250.0f + p->radius) * (250.0f + p->radius);
    int vis = 0;
    if (s < lim &&
        fn_802C9640(&p->center, p->radius)) {
        vis = 1;
    }
    return vis;
}

void fn_8029D550(WaterPool* p, Vec* c, f32* r) {
    c->x = p->center.x;
    c->y = p->center.y;
    c->z = p->center.z;
    *r = p->radius;
}

void fn_8029DF64(WaterPool* p, int x, int y, f32 amount);
void fn_8029DDB8(WaterPool* p);
void fn_8029DB78(WaterPool* p, PoolVtx* v, int x, int y);

void fn_8029D574(WaterPool* p) {
    if (p->timer >= 1.0f - lbl_80558DC4[0]) {
        f32 amount = fn_80287BC8(-p->desc.unk14, p->desc.unk14);
        int y = fn_80287B94(1, p->ny - 2);
        int x = fn_80287B94(1, p->nx - 2);
        fn_8029DF64(p, x, y, amount);
        p->timer = p->timer + (p->desc.amount - 1.0f);
    } else {
        p->timer = p->timer + p->desc.amount;
    }
    fn_8029DDB8(p);
}

void fn_8029D644(WaterPool* p) {
    int n = (p->ny - 1) * (p->nx * 2);
    const WaterFrame* cam = fn_802C86DC();
    p->eye.x = cam->pos.x;
    p->eye.y = cam->pos.y;
    p->eye.z = cam->pos.z;
    fn_802C8BD0(p->tex);
    if (fn_802C8DBC(n)) {
        PoolVtx* rows[2];
        int i;
        rows[0] = p->verts;
        rows[1] = p->verts + p->nx;
        for (i = 0; i < p->nx; i++) {
            fn_8029DB78(p, &rows[0][i], i, 0);
        }
        for (int j = 1; j < p->ny; j++) {
            PoolVtx* a = rows[j & 1];
            PoolVtx* b = rows[1 - (j & 1)];
            for (i = 0; i < p->nx; i++) {
                fn_8029DB78(p, &a[i], i, j);
                fn_802C8E00(b[i].color, i > 0, b[i].pos.x, b[i].pos.y, b[i].pos.z, b[i].s, b[i].t);
                fn_802C8E00(a[i].color, i > 0, a[i].pos.x, a[i].pos.y, a[i].pos.z, a[i].s, a[i].t);
            }
        }
        fn_802C8E48();
    }
    fn_802C8D74();
}

void fn_8029E49C(WaterPool* p, WaterCar* car, int w);
void fn_8029E130(WaterPool* p, const Vec* at, const Vec* vel, f32 r);

void fn_8029D7D0(WaterPool* p, const WaterCarEntry* e) {
    f32 r2 = p->radius * p->radius;
    f32 dz = e->car->pos.z - p->center.z;
    f32 dx = e->car->pos.x - p->center.x;
    if (dz * dz + dx * dx > r2) {
        return;
    }
    for (int w = 0; w < 4; w++) {
        if (!(e->car->wheels[w].flags & 8)) {
            fn_8029E49C(p, e->car, w);
        }
    }
    ModelInfo* m = fn_802C6C34(fn_801EE0C8(e->index < 8 ? lbl_80488648[e->index] : 0));
    WaterCar* car = e->car;
    Vec c;
    MulVec(&c, &m->center, car->axes);
    c.x = c.x + car->pos.x;
    c.y = c.y + car->pos.y;
    c.z = c.z + car->pos.z;
    fn_8029E130(p, &c, &e->car->vel, 0.8f * m->radius);
}

void fn_8029DA24(WaterPool* p, Vec* n, int x, int y) {
    if (x == 0 || x == p->nx - 1 || y == 0 || y == p->ny - 1) {
        n->x = 0.0f;
        n->y = 1.0f;
        n->z = 0.0f;
        return;
    }
    int i = x + y * p->nx;
    s16* h = p->cur;
    f32 gx = 1.5258789e-05f * (0.5f * (h[i - 1] - h[i + 1]));
    f32 gy = 1.5258789e-05f * (0.5f * (h[x + (y - 1) * p->nx] - h[x + (y + 1) * p->nx]));
    n->x = gx * p->dxx + gy * p->dyx;
    n->y = (1.0f - gx * gx) + (1.0f - gy * gy);
    n->z = gx * p->dxz + gy * p->dyz;
    PSVECNormalize(n, n);
}

void fn_8029DB78(WaterPool* p, PoolVtx* v, int x, int y) {
    f32 h = p->cur[x + y * p->nx];
    v->pos.z = p->origin.z + x * p->dxz + y * p->dyz;
    v->pos.y = p->origin.y + 1.5258789e-05f * (p->desc.scale * h);
    v->pos.x = p->origin.x + x * p->dxx + y * p->dyx;
    Vec n;
    fn_8029DA24(p, &n, x, y);
    Vec d;
    d.x = v->pos.x - p->eye.x;
    d.y = v->pos.y - p->eye.y;
    d.z = v->pos.z - p->eye.z;
    f32 k = -2.0f * PSVECDotProduct(&n, &d);
    Vec r;
    r.x = d.x + n.x * k;
    r.y = d.y + n.y * k;
    r.z = d.z + n.z * k;
    PSVECNormalize(&r, &r);
    f32 m = 0.5f / (f32)sqrt((1.0f + r.y) * (1.0f + r.y) + (r.x * r.x + r.z * r.z));
    v->s = 0.5f + r.x * m;
    v->t = 0.5f + r.z * m;
    v->color = p->desc.color | (p->desc.minAlpha + (int)((p->desc.maxAlpha - p->desc.minAlpha) * (1.0f - r.y * r.y))) << 24;
}

void fn_8029DDB8(WaterPool* p) {
    f32 damp = 0.9f + 0.1f * p->desc.damping;
    f32 c = 4.5f * (1.0f - p->desc.rate) * fn_801E7E24();
    c = c * c;
    f32 a = 2.0f - 4.0f * c;
    for (int y = 1; y < p->ny - 1; y++) {
        for (int x = 1; x < p->nx - 1; x++) {
            int i = x + y * p->nx;
            s16* h = p->cur;
            s16* o = p->prev;
            int sum = (h[i + 1] + h[i - 1]) + (h[x + (y + 1) * p->nx] + h[x + (y - 1) * p->nx]);
            int v = MAX((int)(damp * (a * h[i] + c * sum - o[i])), -32768);
            o[i] = MIN(v, 32767);
        }
    }
    s16* t = p->prev;
    p->prev = p->cur;
    p->cur = t;
}

void fn_8029DF64(WaterPool* p, int x, int y, f32 amount) {
    s16* h = p->cur;
    int i = x + y * p->nx;
    int v = (int)(32767.0f * amount + h[i]);
    v = MAX(v, -32768);
    h[i] = MIN(v, 32767);
}

static inline f32 Dot2(f32 ax, f32 ay, f32 bx, f32 by) { return ax * bx + ay * by; }

int fn_8029DFE0(WaterPool* p, const Vec* at) {
    int in = 0;
    ValPoint d;
    d.y = at->z - p->origin.z;
    d.x = at->x - p->origin.x;
    f32 v = Dot2(d.x, d.y, p->dyx, p->dyz);
    f32 u = Dot2(d.x, d.y, p->dxx, p->dxz);
    int inside = 0;
    if (InRange(u, 0.5f, p->nx - 1.5f) && InRange(v, 0.5f, p->ny - 1.5f)) {
        inside = 1;
    }
    if (inside) {
        if (at->y < p->origin.y - 0.1f) {
            in = 1;
        }
    }
    return in;
}

static inline f32 AbsF(f32 v) { return (f32)__fabs(v); }

void fn_8029E130(WaterPool* p, const Vec* at, const Vec* vel, f32 r) {
    Vec base;
    Vec pos;
    Vec w;
    ValPoint d;
    ValPoint v;
    ValPoint q;
    ValPoint dir;
    if (at->y - r > p->origin.y - 0.1f) {
        return;
    }
    v.x = vel->x;
    v.y = vel->z;
    d.x = at->x - p->origin.x;
    d.y = at->z - p->origin.z;
    f32 s2 = v.x * v.x + v.y * v.y;
    if (AbsF(s2) <= lbl_80558DC4[0]) {
        return;
    }
    f32 dy = at->y - p->origin.y;
    f32 depth = sqrt(r * r - dy * dy);
    f32 speed = sqrt(v.x * v.x + v.y * v.y);
    f32 inv = 1.0f / speed;
    q.x = d.x + v.x * (depth * inv);
    q.y = d.y + v.y * (depth * inv);
    f32 fu = q.x * p->dxx + q.y * p->dxz;
    int gx = fu < 0.0f ? fu - 0.5f : 0.5f + fu;
    f32 fv = q.x * p->dyx + q.y * p->dyz;
    int gy = fv < 0.0f ? fv - 0.5f : 0.5f + fv;
    if (gx < 1 || gx >= p->nx - 1 || gy < 1 || gy >= p->ny - 1) {
        return;
    }
    p->cur[gx + gy * p->nx] = -32768;
    dir.x = v.y * inv;
    dir.y = v.x * inv;
    base.x = p->origin.x + q.x;
    base.y = 0.25f + p->origin.y;
    base.z = p->origin.z + q.y;
    f32 a = 0.125f * speed;
    a = 2.0f < a ? 2.0f : a;
    int n = (int)(0.25f * speed) < 10 ? (int)(0.25f * speed) : 10;
    f32 lift = 0.1f * speed;
    for (int k = 0; k < n; k++) {
        SetVec(&pos, base.x + dir.x * fn_80287BC8(-0.5f, 0.5f), base.y, base.z + dir.y * fn_80287BC8(-0.5f, 0.5f));
        SetVec(&w, v.x + dir.x * (4.0f * fn_80287BC8(-a, a)), (3.0f < lift ? 3.0f : lift) + 2.0f * fn_80287BC8(0.0f, a),
               v.y + dir.y * (4.0f * fn_80287BC8(-a, a)));
        fn_8028E584(lbl_8055F7F8, &pos, &w);
    }
}

void fn_8029E49C(WaterPool* p, WaterCar* car, int w) {
    const Vec* wp = &car->wheels[w].pos;
    Vec at;
    MulVec(&at, wp, car->axes);
    at.x = at.x + car->pos.x;
    at.y = at.y + car->pos.y;
    at.z = at.z + car->pos.z;
    if (fn_8029DFE0(p, &at)) {
        at.y += 0.5f;
        f32 kick = 0.2f + 0.1f * (car->speed * (wp->x > 0.0f ? 1.0f : -1.0f));
        Vec v;
        v.x = car->vel.x + car->axes[0].x * kick;
        v.y = car->vel.y + car->axes[0].y * kick;
        v.z = car->vel.z + car->axes[0].z * kick;
        f32 up = 0.2f + 0.1f * car->speed;
        up = 5.0f < up ? 5.0f : up;
        v.x = v.x + car->axes[1].x * up;
        v.y = v.y + car->axes[1].y * up;
        v.z = v.z + car->axes[1].z * up;
        f32 dir = wp->x > 0.0f ? 1.0f : -1.0f;
        fn_8028E590(lbl_8055F7F8, &at, &v, -dir * fn_80287BC8(0.0f, 3.1415927f));
    }
}

}  // extern "C"

WaterShallow::WaterShallow(const PoolGeom* g, const char* texName) {
    Vec r;
    r.x = g->right.x;
    r.y = 0.0f;
    r.z = g->right.y;
    Vec u;
    u.x = g->up.x;
    u.y = 0.0f;
    u.z = g->up.y;
    corners[0].x = g->origin.x;
    corners[0].y = g->origin.y;
    corners[0].z = g->origin.z;
    corners[1].x = corners[0].x + r.x;
    corners[1].y = corners[0].y + r.y;
    corners[1].z = corners[0].z + r.z;
    corners[2].x = corners[0].x + u.x;
    corners[2].y = corners[0].y + u.y;
    corners[2].z = corners[0].z + u.z;
    corners[3].x = corners[1].x + u.x;
    corners[3].y = corners[1].y + u.y;
    corners[3].z = corners[1].z + u.z;
    Vec c;
    c.x = corners[0].x + 0.5f * (corners[3].x - corners[0].x);
    c.y = corners[0].y + 0.5f * (corners[3].y - corners[0].y);
    c.z = corners[0].z + 0.5f * (corners[3].z - corners[0].z);
    center = c;
    radius = PSVECDistance(&corners[0], &center);
    tex = fn_802CAC28(texName, 0);
}

WaterShallow::~WaterShallow(void) { fn_802CAC54(tex); }

extern "C" {

void fn_8029E8E0(WaterShallow* s) {
    fn_802C8BD0(s->tex);
    if (fn_802C8DBC(4)) {
        Vec eye = fn_802C86DC()->pos;
        Vec* c = s->corners;
        for (u32 i = 0; i < 4; i++, c++) {
            Vec d;
            d.y = c->y - eye.y;
            d.x = c->x - eye.x;
            f32 eyeZ = eye.z;
            d.z = c->z - eyeZ;
            Vec r;
            r.x = d.x;
            r.y = -d.y;
            r.z = d.z;
            PSVECNormalize(&r, &r);
            f32 m = 0.5f / (f32)sqrt((1.0f + r.y) * (1.0f + r.y) + (r.x * r.x + r.z * r.z));
            fn_802C8E00(-1, i >= 2, c->x, c->y, c->z, 0.5f + r.x * m, 0.5f + r.z * m);
        }
        fn_802C8E48();
    }
    fn_802C8D74();
}

void fn_8029EC64(WaterShallow* s, WaterCar* car, int w);

void fn_8029EA34(WaterShallow* s, const WaterCarEntry* e) {
    f32 r2 = s->radius * s->radius;
    f32 dz = e->car->pos.z - s->center.z;
    f32 dx = e->car->pos.x - s->center.x;
    if (dz * dz + dx * dx > r2) {
        return;
    }
    for (int w = 0; w < 4; w++) {
        if (!(e->car->wheels[w].flags & 8)) {
            fn_8029EC64(s, e->car, w);
        }
    }
}

int fn_8029EAE8(WaterShallow* s) {
    f32 far = 22500.0f;
    int vis = 0;
    if (fn_802C88E8(&s->center) < far && fn_802C9640(&s->center, s->radius)) {
        vis = 1;
    }
    return vis;
}

int fn_8029EB54(WaterShallow* s, const Vec* at) {
    int in = 0;
    int inside = 0;
    if (InRange(at->x, MinF(s->corners[0].x, s->corners[3].x), MaxF(s->corners[0].x, s->corners[3].x)) &&
        InRange(at->z, MinF(s->corners[0].z, s->corners[3].z), MaxF(s->corners[0].z, s->corners[3].z))) {
        inside = 1;
    }
    if (inside) {
        if (at->y < s->corners[0].y - 0.1f) {
            in = 1;
        }
    }
    return in;
}

void fn_8029EC64(WaterShallow* s, WaterCar* car, int w) {
    const Vec* wp = &car->wheels[w].pos;
    Vec at;
    MulVec(&at, wp, car->axes);
    at.x = at.x + car->pos.x;
    at.y = at.y + car->pos.y;
    at.z = at.z + car->pos.z;
    if (fn_8029EB54(s, &at)) {
        at.y += 0.5f;
        f32 kick = 0.2f + 0.1f * (car->speed * (wp->x > 0.0f ? 1.0f : -1.0f));
        Vec v;
        v.x = car->vel.x + car->axes[0].x * kick;
        v.y = car->vel.y + car->axes[0].y * kick;
        v.z = car->vel.z + car->axes[0].z * kick;
        f32 up = 0.2f + 0.1f * car->speed;
        up = 5.0f < up ? 5.0f : up;
        v.x = v.x + car->axes[1].x * up;
        v.y = v.y + car->axes[1].y * up;
        v.z = v.z + car->axes[1].z * up;
        f32 dir = wp->x > 0.0f ? 1.0f : -1.0f;
        fn_8028E590(lbl_8055F7F8, &at, &v, -dir * fn_80287BC8(0.0f, 3.1415927f));
    }
}

}  // extern "C"

inline void WaterIface::Close(void) {}

inline void WaterIface::Begin(void) {}

inline void WaterIface::End(void) {}
