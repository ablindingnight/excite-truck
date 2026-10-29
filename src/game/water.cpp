// Water (0x802BECE4-0x802C0D70): the track's water plane. Colours and height come from the
// "Graphics" table of track.val. Each viewport gets a WaterSystem that draws a 64x16 vertex grid
// from the camera out to the horizon, coloured between the near and far water colours and textured
// with a reflection render target, plus a shimmer quad. The reflection is a WaterEnvMap that renders
// the mirrored world and a shared sky cube ("skybox") into a 256x256 target. A gen_surf model adds
// animated surface rings. Names are guesses apart from the strings.
#include "game/common.h"

void* operator new(size_t size, const char* tag);

extern "C" {
int sscanf(const char* s, const char* fmt, ...);
double fmod(double x, double y);
double cos(double x);
double tan(double x);
double atan(double x);
double atan2(double y, double x);
double sqrt(double x);
}

struct PVec {
    f32 x, y, z;
};

// Three basis vectors and the origin (0x30).
struct Frame {
    PVec x;    // offset 0x0
    PVec y;    // offset 0xC
    PVec z;    // offset 0x18
    PVec pos;  // offset 0x24
};

struct Mat3 {
    f32 m[3][3];
};

// A coloured, textured vertex (0x18).
struct WVert {
    PVec pos;   // offset 0x0
    u32 color;  // offset 0xC
    f32 u;      // offset 0x10
    f32 v;      // offset 0x14
};

// A 2x4 texture matrix.
struct TexMtx {
    f32 m[2][4];
};

struct Viewport {
    s32 x, y, w, h;
};

struct Perspective {
    Perspective(f32 near, f32 far, f32 fov);
    ~Perspective(void);
    f32 near;  // offset 0x0
    f32 far;   // offset 0x4
    f32 fov;   // offset 0x8
};

void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);

extern "C" {
void PSVECCrossProduct(const void* a, const void* b, void* out);
f32 PSVECMag(const void* v);
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* t);
void fn_802E30C8(const char* fmt, ...);
void* fn_801E866C(int view);
u8* fn_801EA9A0(void* cam);
f32 fn_801E7E24(void);
f32 fn_801E7E18(void);
int fn_801E85AC(void);
int fn_801E85B4(void);
int fn_801E85A4(void);
int fn_8012C33C(const char* name);
s32 fn_802C6FC0(const char* model, int a, int b);
void fn_802C6CF8(s32 model);
void fn_802C7040(s32 model);
void fn_802C9648(void);
void fn_802C967C(void);
void fn_802D3D6C(void);
void fn_802D3D80(void);
void fn_802D3D94(int slot, const TexMtx* m);
void fn_802C9900(f32 a);
void fn_802C9904(void);
int fn_802C2C00(int mode);
s32 fn_802CB128(int w, int h, int a, int b);
void fn_802CB228(s32 target);
void fn_802CB274(s32 target);
void fn_802C9294(s32 target, int a);
s32 fn_802CAC28(const char* name, int a);
void fn_802CAC54(s32 tex);
void fn_802C86F0(Viewport* v, int x, int y, int w, int h);
void fn_802C8750(Viewport* v, int del);
void fn_802C8B08(Frame* cam);
void fn_802C8638(const Frame* cam);
void fn_802C88BC(int mode);
void fn_802C10C8(void);
void fn_802BDD8C(f32 dist);
void fn_80144DC8(Frame* f);
void fn_80144DAC(PVec* out, const PVec* in);
void fn_802C8B98(s32 tex, const Frame* m);
void fn_802C8DB8(const WVert* v, int nv, const u16* idx, int ni);
void fn_802C8D74(void);
void fn_80015A20(Mat3* out, const PVec* angles);
void fn_802C98DC(f32* a, f32* b, f32* c, f32* d);
WVert* fn_802DD42C(int size);
int fn_802C9C50(void);
int fn_802C9C40(void);
int fn_802C9C38(void);
int fn_802C9C48(void);
u32 fn_80147C50(f32 t, u32 a, u32 b);
void fn_802C8E70(const PVec* p, PVec* out);
void fn_802DBE4C(void);
void fn_802DBECC(void);
void fn_802C8BD0(s32 tex);
void fn_802DCFCC(s32 target, s32 dtex, s32 mask, u32 color, f32 a, f32 b);
void fn_802DD27C(WVert* v, int nv, const u16* idx, int ni);
void fn_802DD228(void);
}

struct PtTable {
    struct {
        f32 x, y;
    } p[7];
};

struct RingTable {
    struct {
        f32 scale, y, phase;
    } r[3];
};

static const PtTable sAlpha = {{
    {0.0f, 0.6f}, {0.62831855f, 0.4f}, {1.2566371f, 0.0f}, {3.1415927f, 0.0f},
    {5.340708f, 1.0f}, {5.9690256f, 1.0f}, {6.2831855f, 0.6f},
}};

static const RingTable sRings = {{
    {0.65f, 0.0f, 0.0f}, {0.65f, 0.25f, 2.0943952f}, {0.65f, 0.65f, 4.1887903f},
}};

static const TexMtx sRingMtx = {{{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}}};

struct IdxTable {
    u16 i[6];
};

static const IdxTable sQuad = {{0, 1, 2, 2, 1, 3}};

static const TexMtx sShimMtx = {{{0.04f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.04f, 0.0f}}};

static const IdxTable sShimQuad = {{0, 1, 2, 2, 1, 3}};

// The shared reflection cube (0x1B8).
struct Skybox {
    Skybox(void);
    ~Skybox(void) {
        for (int i = 0; i < 6; i++) {
            fn_802CB228(faces[i]);
        }
        if (sun != -2) {
            fn_802CAC54(sun);
        }
    }
    s32 faces[6];      // offset 0x0
    s32 sun;           // offset 0x18
    Frame frames[6];   // offset 0x1C
    WVert quad[4];     // offset 0x13C
    u16 idx[6];        // offset 0x19C
    u8 drawn;          // offset 0x1A8
};

class WaterBase {
public:
    virtual ~WaterBase(void);
    virtual void Reflect(void* cam, f32 fov) = 0;
    virtual void Draw(void) = 0;
    virtual int IsValid(void) = 0;
};

class EnvBase {
public:
    virtual ~EnvBase(void);
    virtual void Render(void* cam, f32 fov) = 0;
    virtual s32 Target(void) = 0;
    virtual int IsValid(void) = 0;
    virtual f32 Scale(void) = 0;
    virtual void Mirror(Frame* out, const Frame* in, const PVec* up);
};

class WaterEnvMap : public EnvBase {
public:
    WaterEnvMap(void);
    virtual ~WaterEnvMap(void);
    virtual void Render(void* cam, f32 fov);
    virtual s32 Target(void);
    virtual f32 Scale(void);
    virtual int IsValid(void);

    Skybox* sky;   // offset 0x4
    s32 target;    // offset 0x8
    s32 world;     // offset 0xC
    s32 envModel;  // offset 0x10
    f32 scale;     // offset 0x14
};

class TexEnv : public EnvBase {
public:
    virtual ~TexEnv(void);
    virtual void Render(void* cam, f32 fov);
    virtual s32 Target(void);
    virtual f32 Scale(void);
    virtual int IsValid(void);

    s32 tex;  // offset 0x4
};

class WaterSystem : public WaterBase {
public:
    WaterSystem(void);
    virtual ~WaterSystem(void);
    virtual void Reflect(void* cam, f32 fov);
    virtual void Draw(void);
    virtual int IsValid(void);

    u16 idx[63][15][6];  // offset 0x4
    s32 mask;            // offset 0x2C50
    s32 deep;            // offset 0x2C54
    EnvBase* env;        // offset 0x2C58
    u8 valid;            // offset 0x2C5C
};

const char* lbl_8055CF68 = "gen_surf.mod";
// MATCHING-AID: unused in this build; keeps the "gen_surf.tex" literal at its pool slot (its user is gone).
static const char* sSurfTex = "gen_surf.tex";
s32 lbl_8055CF6C = -1;
u8 lbl_8055CF70 = 1;
u8 lbl_8055CF71 = 1;
f32 lbl_8055CF74 = 4.0f;

ModuleTag lbl_8055FAD0;

struct WaterTint {
    WaterTint(void) { rgba = 0xFFFF0000; }
    u32 rgba;
};

static WaterTint lbl_8055FAD8;
s32 lbl_8055FADC;
f32 lbl_8055FAE0;
u32 lbl_8055FAE4;  // far water color (the log line labels it N)
u32 lbl_8055FAE8;  // near water color
u32 lbl_8055FAEC;
u32 lbl_8055FAF0;
f32 lbl_8055FAF4;
f32 lbl_8055FAF8;
s32 lbl_8055FAFC;
Skybox* lbl_8055FB00;
f32 lbl_8055FB04;
WaterBase* lbl_804C07F0[4];

static inline f32 Mul(f32 a, f32 b) { return a * b; }
static inline f32 FMin(f32 a, f32 b) { return b < a ? b : a; }
static inline f32 FMax(f32 a, f32 b) { return a > b ? a : b; }

extern "C" {

u32 fn_802BECE4(const char* s, u32 def) {
    int r = 0;
    int g = 0;
    int b = 0;
    int a = 0;
    if (sscanf(s, "%d,%d,%d,%d", &r, &g, &b, &a) == 4) {
        def = (r & 0xFF) << 24 | (g & 0xFF) << 16 | (b & 0xFF) << 8 | (a & 0xFF);
    } else {
        sscanf(s, "0x%x", &def);
    }
    return def;
}

void fn_802BED90(PVec* v) {
    f32 s = 1.0f / FMax(0.001f, PSVECMag(v));
    v->x *= s;
    v->y *= s;
    v->z *= s;
}

void fn_802BEDFC(void) {
    lbl_8055FAE4 = 0xE0005030;
    lbl_8055FAE0 = 0.7f;
    lbl_8055FAE8 = 0xE0FFFFFF;
    lbl_8055FAEC = 0x00202040;
    lbl_8055FAF0 = 0xE0001040;
    ValueTable* t = fn_80131808("track.val");
    if (t) {
        ValueNode* g = t->HasChild("Graphics") ? t->FindChild("Graphics") : 0;
        if (g) {
            lbl_8055FAE0 = g->GetFloat("water_height", lbl_8055FAE0);
            lbl_8055FAE8 = fn_802BECE4(g->GetString("near_water_color", ""), lbl_8055FAE8);
            lbl_8055FAE4 = fn_802BECE4(g->GetString("far_water_color", ""), lbl_8055FAE4);
            lbl_8055FAEC = fn_802BECE4(g->GetString("deep_water_sub_color", ""), lbl_8055FAEC);
            lbl_8055FAF0 = fn_802BECE4(g->GetString("shim_water_color", ""), lbl_8055FAF0);
            fn_802E30C8("water colors: N %08x, F %08x DS %08x SH %08x", lbl_8055FAE4, lbl_8055FAE8,
                        lbl_8055FAEC, lbl_8055FAF0);
        }
        fn_80131818(t);
    }
    lbl_8055FAF4 = lbl_8055FAE0;
}

f32 fn_802BEFB4(void) {
    u8* c = fn_801EA9A0(fn_801E866C(-1));
    f32 h = *(f32*)(*(u8**)(c + 8) + 0x64);
    f32 t = FMax(FMin((h - 22.222223f) / 44.444443f, 1.0f), 0.0f);
    return 0.7f + Mul(t, 1.3f);
}

f32 fn_802BF02C(f32 x) {
    PtTable t = sAlpha;
    for (u32 i = 1; i < 7; i++) {
        if (t.p[i].x > x) {
            f32 u = FMax(FMin((x - t.p[i - 1].x) / (t.p[i].x - t.p[i - 1].x), 1.0f), 0.0f);
            return t.p[i - 1].y + u * (t.p[i].y - t.p[i - 1].y);
        }
    }
    return t.p[6].y;
}

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

void fn_802BF160(void) {
    if (lbl_8055CF6C == -1) {
        return;
    }
    f32 k = fn_802BEFB4();
    lbl_8055FAF8 += k * fn_801E7E24();
    RingTable rings = sRings;
    fn_802C9648();
    fn_802D3D6C();
    for (u32 i = 0; i < 3; i++) {
        f32 a = fmod(lbl_8055FAF8 + rings.r[i].phase, 6.283185307179586);
        f32 c = cos(a);
        f32 w = 1.0f - rings.r[i].scale;
        f32 s = FMax(FMin((c - -1.0f) * 0.5f, 1.0f), 0.0f);
        f32 h = Lerp(0.0f, w, s);
        f32 inv = 1.0f / rings.r[i].scale;
        TexMtx m = sRingMtx;
        f32 off = -inv * h;
        m.m[0][3] = rings.r[i].y;
        m.m[1][1] = (1.0f + inv * (1.0f - (h + rings.r[i].scale))) - off;
        m.m[1][3] = off;
        fn_802C9900(fn_802BF02C(a));
        fn_802D3D94(0, &m);
        fn_802C7040(lbl_8055CF6C);
    }
    fn_802C9904();
    fn_802D3D80();
    fn_802C967C();
}

WaterBase* fn_802C00A4(void);

void fn_802BF350(void) {
    fn_802BEDFC();
    lbl_8055FADC = fn_801E85AC();
    for (int i = 0; i < lbl_8055FADC; i++) {
        lbl_804C07F0[i] = fn_802C00A4();
        if (i == 0 && lbl_804C07F0[i]) {
            fn_802E30C8("water: system is active");
        }
    }
    lbl_8055CF6C = fn_8012C33C(lbl_8055CF68) ? fn_802C6FC0(lbl_8055CF68, 0, 0) : -1;
}

void fn_802BF40C(void) {
    for (int i = 0; i < lbl_8055FADC; i++) {
        if (lbl_804C07F0[i]) {
            delete lbl_804C07F0[i];
            lbl_804C07F0[i] = 0;
        }
    }
    lbl_8055FADC = 0;
    if (lbl_8055CF6C != -1) {
        fn_802C6CF8(lbl_8055CF6C);
        lbl_8055CF6C = -1;
    }
}
}

inline WaterBase::~WaterBase(void) {}

extern "C" {

int fn_802BF4F0(void) { return lbl_804C07F0 != 0; }

void fn_802BF508(void) {
    int v = fn_801E85B4();
    if (lbl_804C07F0[v] && lbl_8055CF70) {
        int old = fn_802C2C00(0);
        lbl_804C07F0[v]->Draw();
        fn_802C2C00(old);
    }
    fn_802BF160();
}

f32 fn_802BF590(void) { return lbl_8055FAF4; }

void fn_802BF598(void* cam, f32 fov) {
    int v = fn_801E85B4();
    if (lbl_804C07F0[v]) {
        lbl_804C07F0[v]->Reflect(cam, fov);
    }
}
}

void EnvBase::Mirror(Frame* out, const Frame* in, const PVec* up) {
    *out = *in;
    out->x.y *= -1.0f;
    out->y.y *= -1.0f;
    out->z.y *= -1.0f;
    out->pos.y *= -1.0f;
    out->pos.y = out->pos.y + 2.0f * lbl_8055FAE0;
}

extern "C" EnvBase* fn_802BF6AC(void) {
    EnvBase* e = new ("waterenvmap") WaterEnvMap;
    if (e && !e->IsValid()) {
        delete e;
        e = 0;
    }
    return e;
}

inline EnvBase::~EnvBase(void) {}

static inline s32 LoadModel(const char* name) {
    if (fn_8012C33C(name)) {
        return fn_802C6FC0(name, 0, 0);
    }
    return -1;
}

static inline void Identity(Frame* f) {
    f->x.x = 1.0f;
    f->x.y = 0.0f;
    f->x.z = 0.0f;
    f->y.x = 0.0f;
    f->y.y = 1.0f;
    f->y.z = 0.0f;
    f->z.x = 0.0f;
    f->z.y = 0.0f;
    f->z.z = 1.0f;
    f->pos.x = 0.0f;
    f->pos.y = 0.0f;
    f->pos.z = 0.0f;
}

inline Skybox::Skybox(void) {
    if (fn_8012C33C("env_sun.tex")) {
        sun = fn_802CAC28("env_sun.tex", 0);
    } else {
        sun = -2;
    }
    for (int i = 0; i < 6; i++) {
        faces[i] = fn_802CB128(0x100, 0x100, 0, 0x60);
        Identity(&frames[i]);
    }
    frames[1].x.x = -1.0f;
    frames[1].x.y = 0.0f;
    frames[1].x.z = 0.0f;
    frames[1].z.x = 0.0f;
    frames[1].z.y = 0.0f;
    frames[1].z.z = -1.0f;
    frames[2].x.x = 0.0f;
    frames[2].x.y = 0.0f;
    frames[2].x.z = -1.0f;
    frames[2].z.x = 1.0f;
    frames[2].z.y = 0.0f;
    frames[2].z.z = 0.0f;
    frames[3].x.x = 0.0f;
    frames[3].x.y = 0.0f;
    frames[3].x.z = 1.0f;
    frames[3].z.x = -1.0f;
    frames[3].z.y = 0.0f;
    frames[3].z.z = 0.0f;
    frames[4].y.x = 0.0f;
    frames[4].y.y = 0.0f;
    frames[4].y.z = -1.0f;
    frames[4].z.x = 0.0f;
    frames[4].z.y = 1.0f;
    frames[4].z.z = 0.0f;
    frames[5].y.x = 0.0f;
    frames[5].y.y = 0.0f;
    frames[5].y.z = 1.0f;
    frames[5].z.x = 0.0f;
    frames[5].z.y = -1.0f;
    frames[5].z.z = 0.0f;
    quad[0].pos.x = -1.0f;
    quad[0].pos.y = 1.0f;
    quad[0].pos.z = 1.0f;
    quad[0].color = 0xFFFFFFFF;
    quad[0].u = 0.0f;
    quad[0].v = 0.0f;
    quad[1].pos.x = 1.0f;
    quad[1].pos.y = 1.0f;
    quad[1].pos.z = 1.0f;
    quad[1].color = 0xFFFFFFFF;
    quad[1].u = 1.0f;
    quad[1].v = 0.0f;
    quad[2].pos.x = -1.0f;
    quad[2].pos.y = -1.0f;
    quad[2].pos.z = 1.0f;
    quad[2].color = 0xFFFFFFFF;
    quad[2].u = 0.0f;
    quad[2].v = 1.0f;
    quad[3].pos.x = 1.0f;
    quad[3].pos.y = -1.0f;
    quad[3].pos.z = 1.0f;
    quad[3].color = 0xFFFFFFFF;
    quad[3].u = 1.0f;
    quad[3].v = 1.0f;
    idx[0] = 0;
    idx[1] = 1;
    idx[2] = 2;
    idx[3] = 1;
    idx[4] = 2;
    idx[5] = 3;
    drawn = 0;
}

WaterEnvMap::WaterEnvMap(void) {
    target = fn_802CB128(0x100, 0x100, 0, 0);
    world = LoadModel("worldmodel.mod");
    envModel = LoadModel("wenv_mod.mod");
    if (lbl_8055FAFC == 0) {
        lbl_8055FB00 = new ("skybox") Skybox;
    }
    lbl_8055FAFC++;
    sky = lbl_8055FB00;
    scale = 1.0f;
}

WaterEnvMap::~WaterEnvMap(void) {
    if (target != -2) {
        fn_802CB228(target);
        target = -2;
    }
    if (world != -1) {
        fn_802C6CF8(world);
        world = -1;
    }
    if (envModel != -1) {
        fn_802C6CF8(envModel);
        envModel = -1;
    }
    if (sky) {
        if (--lbl_8055FAFC == 0) {
            delete lbl_8055FB00;
            lbl_8055FB00 = 0;
        }
        sky = 0;
    }
}

extern "C" void fn_802C00A0(f32* fov);
extern "C" void fn_802C0054(Mat3* out, const Mat3* in);
extern "C" void fn_802BFE48(Skybox* s, const Frame* view, f32 scale);

void WaterEnvMap::Render(void* cam, f32 fov) {
    scale = (fn_801E85A4() == 2) ? 1.5f : 1.0f;
    Skybox* s = sky;
    if (!s->drawn) {
        for (int i = 0; i < 6; i++) {
            fn_802CB274(s->faces[i]);
            fn_802C9294(s->faces[i], 0);
            Perspective p(1.0f, 2000.0f, 1.5707964f);
            Viewport vp;
            fn_802C86F0(&vp, 0, 0, 0x100, 0x100);
            Frame save;
            fn_802C8B08(&save);
            fn_802C8638(&s->frames[i]);
            fn_802C88BC(3);
            fn_802C10C8();
            fn_802C9294(-2, 0);
            fn_802C8638(&save);
            fn_802C8750(&vp, -1);
        }
        s->drawn = 1;
    }
    fn_802CB274(target);
    fn_802C9294(target, 1);
    PVec up;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    Frame m;
    Mirror(&m, (const Frame*)cam, &up);
    f32 h = 2.0f * (f32)atan(scale * (f32)tan(0.5f * fov));
    Viewport vp;
    Perspective p(1.0f, 3000.0f, h);
    fn_802C86F0(&vp, 0, 0, 0x100, 0x100);
    Frame save;
    fn_802C8B08(&save);
    fn_802C8638(&m);
    fn_802C88BC(2);
    if (sky) {
        f32 hf = h;
        Skybox* k = sky;
        fn_802C00A0(&hf);
        Perspective ps(0.1f, 2.0f, hf);
        for (int i = 0; i < 6; i++) {
            Frame f;
            fn_80144DC8(&f);
            fn_802C0054((Mat3*)&f, (const Mat3*)&k->frames[i]);
            fn_80144DAC(&f.pos, &m.pos);
            fn_802C8B98(k->faces[i], &f);
            fn_802C8DB8(k->quad, 4, k->idx, 6);
            fn_802C8D74();
        }
        fn_802BFE48(k, &m, 1.0f);
    }
    fn_802C7040(world);
    if (fn_801E85A4() == 1) {
        fn_802BDD8C(300.0f);
    }
    if (envModel != -1) {
        fn_802C7040(envModel);
    }
    fn_802C9294(-2, 0);
    fn_802C8638(&save);
    fn_802C8750(&vp, -1);
}

extern "C" {

void fn_802BFE48(Skybox* s, const Frame* view, f32 scale) {
    if (s->sun == -2) {
        return;
    }
    IdxTable idx = sQuad;
    f32 r = 0.3f * scale;
    f32 d = 0.9f * scale;
    WVert v[4];
    for (int i = 0; i < 4; i++) {
        v[i].pos.x = (i & 1) ? r : -r;
        v[i].pos.y = (i & 2) ? r : -r;
        v[i].color = 0xFFFFFFFF;
        v[i].pos.z = d;
        v[i].u = (i & 1) ? 1.0f : 0.0f;
        v[i].v = (i & 2) ? 1.0f : 0.0f;
    }
    Frame m;
    m.x.x = 1.0f;
    m.x.y = 0.0f;
    m.x.z = 0.0f;
    m.y.x = 0.0f;
    m.y.y = 1.0f;
    m.y.z = 0.0f;
    m.z.x = 0.0f;
    m.z.y = 0.0f;
    m.z.z = 1.0f;
    m.pos = view->pos;
    PVec a;
    a.x = -0.5235988f;
    a.y = 0.0f;
    a.z = 0.0f;
    Mat3 r30;
    fn_80015A20(&r30, &a);
    Mat3Mul((Mat3*)&m, &r30, (Mat3*)&m);
    fn_802C8B98(s->sun, &m);
    fn_802C8DB8(v, 4, idx.i, 6);
    fn_802C8D74();
}

void fn_802C0054(Mat3* out, const Mat3* in) { *out = *in; }

void fn_802C00A0(f32* fov) {}

WaterBase* fn_802C00A4(void) {
    WaterBase* w = new ("water_quad") WaterSystem;
    if (w && !w->IsValid()) {
        delete w;
        w = 0;
    }
    return w;
}
}

WaterSystem::WaterSystem(void) {
    valid = 0;
    mask = -2;
    deep = -2;
    env = 0;
    lbl_8055FB04 = 0.2f;
    if (!fn_8012C33C("gen_water.tex")) {
        return;
    }
    if (fn_8012C33C("watermsk.tex")) {
        mask = fn_802CAC28("watermsk.tex", 0);
    }
    env = fn_802BF6AC();
    if (fn_8012C33C("gen_waterd.tex")) {
        deep = fn_802CAC28("gen_waterd.tex", 0);
    }
    u16* p = &idx[0][0][0];
    for (int r = 0; r < 63; r++) {
        for (int c = 0; c < 15; c++) {
            int a = r * 16 + c;
            int b = (r + 1) * 16 + c;
            p[0] = a;
            p[1] = a + 1;
            p[2] = b;
            p[3] = b;
            p[4] = a + 1;
            p[5] = b + 1;
            p += 6;
        }
    }
    valid = 1;
}

WaterSystem::~WaterSystem(void) {
    if (mask != -2) {
        fn_802CAC54(mask);
    }
    if (deep != -2) {
        fn_802CAC54(deep);
    }
    if (env) {
        delete env;
    }
}

void WaterSystem::Reflect(void* cam, f32 fov) {
    if (env && lbl_8055CF71) {
        env->Render(cam, fov);
    }
}

extern "C" u32 fn_802C03E0(u32 c) {
    return (c & 0xFFFF) << 8 | (c & 0xFF0000) << 8 | (c >> 24 & 0xFF);
}

void WaterSystem::Draw(void) {
    Frame cam;
    fn_802C8B08(&cam);
    f32 h = cam.pos.y - lbl_8055FAF4;
    f32 sx;
    f32 nearZ;
    f32 farZ;
    f32 dz;
    fn_802C98DC(&sx, &nearZ, &farZ, &dz);
    f32 tilt = h * cam.z.y;
    f32 far = 0.95f * farZ;
    sx = sx * (f32)sqrt(1.5625);
    WVert* verts = fn_802DD42C(0x6000);
    f32 vx = fn_802C9C50();
    f32 vy = fn_802C9C40();
    f32 vw = fn_802C9C38();
    f32 vh = fn_802C9C48();
    f32 k = 640.0f / (vh * env->Scale());
    PVec fwd;
    if ((f32)__fabs(cam.z.y) < 0.99f) {
        fwd = cam.z;
    } else {
        fwd = cam.y;
    }
    fwd.y = 0.0f;
    f32 inv = 1.0f / FMax(0.01f, PSVECMag(&fwd));
    fwd.x *= inv;
    fwd.y *= inv;
    fwd.z *= inv;
    PVec up;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    PVec side;
    PSVECCrossProduct(&fwd, &up, &side);
    f32 cx = 0.5f * (640.0f - vh);
    f32 cy = 0.5f * (480.0f - vw);
    for (int r = 0; r < 64; r++) {
        f32 t = (f32)r / 63.0f * ((f32)r / 63.0f);
        PVec p = cam.pos;
        p.y = lbl_8055FAF4;
        if (r == 63) {
            p.y = cam.pos.y;
        }
        f32 dist = nearZ + t * (far - nearZ) + tilt;
        p.x += fwd.x * dist;
        p.y += fwd.y * dist;
        p.z += fwd.z * dist;
        PVec rel;
        rel.x = p.x - cam.pos.x;
        rel.y = p.y - cam.pos.y;
        rel.z = p.z - cam.pos.z;
        f32 depth = rel.x * cam.z.x + rel.y * cam.z.y + rel.z * cam.z.z;
        fn_802BED90(&rel);
        f32 f = FMax(FMin(1.0f - -rel.y, 1.0f), 0.0f);
        u32 col = fn_802C03E0(fn_80147C50(f * f, lbl_8055FAE4, lbl_8055FAE8));
        for (int c = 0; c < 16; c++) {
            WVert* v = &verts[r * 16 + c];
            f32 u = (2.0f * (f32)c / 15.0f - 1.0f) * depth * sx;
            v->pos.x = p.x + side.x * u;
            v->pos.y = p.y + side.y * u;
            v->pos.z = p.z + side.z * u;
            v->color = col;
            PVec s;
            fn_802C8E70(&v->pos, &s);
            v->u = 0.5f + ((s.x - vx + cx) / 640.0f - 0.5f) * k;
            v->v = 0.5f + 0.75f * (((s.y - vy + cy) / 480.0f - 0.5f) * k);
        }
    }
    fn_802DBE4C();
    TexMtx m = sShimMtx;
    m.m[0][3] = fmod(0.4f * fn_801E7E18(), 1.0);
    fn_802D3D94(2, &m);
    f32 sh = FMax(0.25f * cam.pos.y, 1.0f);
    u32 deepCol = fn_80147C50(1.0f / sh, 0, lbl_8055FAEC);
    fn_802C8BD0(env->Target());
    fn_802DCFCC(env->Target(), deep, mask, deepCol, lbl_8055CF74, 1.0f / (1024.0f * 2.0f));
    fn_802DD27C(verts, 0x400, &idx[0][0][0], 0x1626);
    fn_802DD228();
    fn_802C8D74();
    fn_802DBECC();
    Frame cam2;
    fn_802C8B08(&cam2);
    f32 nz;
    f32 dummy;
    fn_802C98DC(&dummy, &nz, &dummy, &dummy);
    nz = nz + 0.001f;
    PVec c;
    c.x = cam2.pos.x + cam2.z.x * nz;
    c.y = cam2.pos.y + cam2.z.y * nz;
    c.z = cam2.pos.z + cam2.z.z * nz;
    f32 sc = (lbl_8055FAF4 - c.y) / FMax(0.001f, cam2.z.y);
    PVec q;
    q.x = c.x + cam2.y.x * sc;
    q.y = c.y + cam2.y.y * sc;
    q.z = c.z + cam2.y.z * sc;
    PVec ax = cam2.x;
    ax.y = 0.0f;
    PVec upv;
    upv.x = 0.0f;
    upv.y = 1.0f;
    upv.z = 0.0f;
    PVec fw;
    PSVECCrossProduct(&upv, &ax, &fw);
    PVec e0;
    PVec e1;
    e0.x = q.x + fw.x * -10.0f;
    e0.y = q.y + fw.y * -10.0f;
    e0.z = q.z + fw.z * -10.0f;
    e1.x = q.x + fw.x * 10.0f;
    e1.y = q.y + fw.y * 10.0f;
    e1.z = q.z + fw.z * 10.0f;
    IdxTable qi = sShimQuad;
    WVert sv[4];
    for (int i = 0; i < 4; i++) {
        PVec* b = (i & 1) ? &e1 : &e0;
        sv[i].pos = *b;
        f32 o = (i & 2) ? -10.0f : 0.0f;
        sv[i].pos.x += cam2.y.x * o;
        sv[i].pos.y += cam2.y.y * o;
        sv[i].pos.z += cam2.y.z * o;
        sv[i].v = 0.0f;
        sv[i].u = 0.0f;
        sv[i].color = lbl_8055FAF0;
    }
    fn_802C8BD0(-1);
    fn_802C8DB8(sv, 4, qi.i, 6);
    fn_802C8D74();
}

s32 WaterEnvMap::Target(void) { return target; }

f32 WaterEnvMap::Scale(void) { return scale; }

int WaterEnvMap::IsValid(void) { return 1; }

TexEnv::~TexEnv(void) {
    if (tex != -2) {
        fn_802CAC54(tex);
    }
}

void TexEnv::Render(void* cam, f32 fov) {}

s32 TexEnv::Target(void) { return tex; }

f32 TexEnv::Scale(void) { return 1.0f; }

int TexEnv::IsValid(void) { return 1; }

int WaterSystem::IsValid(void) { return valid; }
