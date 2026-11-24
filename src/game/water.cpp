// Water (0x802BECE4-0x802C0D70): the track's water plane. Colours and height come from the
// "Graphics" table of track.val. Each viewport gets a WaterSystem that draws a 64x16 vertex grid
// from the camera out to the horizon, coloured between the near and far water colours and textured
// with a reflection render target, plus a shimmer quad. The reflection is a WaterEnvMap that renders
// the mirrored world and a shared sky cube ("skybox") into a 256x256 target. A gen_surf model adds
// animated surface rings. Names are guesses apart from the strings.
#include "game/common.h"
#include <revolution/MTX/vec.h>
// World units per grid cell (name unproven), a one-element const array: Draw loads it and multiplies by 1024 at run
// time (a scalar const would fold into 1/2048). A named constant with external linkage defined before any code: retail
// keeps it as the FIRST .sdata2 entry, separate from the pooled 2.0f literal (same pattern as heightfld).
extern const f32 lbl_80564D78[1];
const f32 lbl_80564D78[1] = {2.0f};
#include "game/math_types.h"
#include "game/plainvec.h"
#include "game/mathutil.h"


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

// Frame: common.h's PlainValMatrix (x/y/z rows -> basis[0..2], pos -> origin).
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
// Viewport's constructor and destructor (mrender.cpp), called by their mangled names: declaring them in the class
// and scoping vp as an object moves RenderBody's stack slots (frame 0x150 -> 0x140), so the calls stay explicit.
extern "C" void __ct__8ViewportFiiii(Viewport* v, int x, int y, int w, int h);
extern "C" void __dt__8ViewportFv(Viewport* v, int del);

struct Perspective {
    Perspective(f32 near, f32 far, f32 fov);
    ~Perspective(void);
    f32 near;  // offset 0x0
    f32 far;   // offset 0x4
    f32 fov;   // offset 0x8
};

extern "C" {
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* t);
void fn_802E30C8(const char* fmt, ...);
void* fn_801E866C(int view);
u8* GetCarInfo(void* cam);
f32 fn_801E7E24(void);
f32 fn_801E7E18(void);
int fn_801E85AC(void);
int fn_801E85B4(void);
int fn_801E85A4(void);
int fn_8012C33C(const char* name);
s32 fn_802C6FC0(const char* model, int a, int b);
void mrModelUnload(s32 model);
void fn_802C7040(s32 model);
void fn_802C9648(f32 scale);
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
void gxForgetTexture(s32 tex);
void fn_802C8B08(PlainValMatrix* cam);
void fn_802C8638(const PlainValMatrix* cam);
void fn_802C88BC(int mode);
void fn_802C10C8(void);
void RenderFarTreeBatch(f32 dist);
void __ct__10CamFrame34Fv(PlainValMatrix* f);
void fn_80144DAC(PVec* out, const PVec* in);
void fn_802C8B98(s32 tex, const PlainValMatrix* m);
void fn_802C8DB8(const WVert* v, int nv, const u16* idx, int ni);
void fn_802C8D74(void);
void fn_802C98DC(f32* a, f32* b, f32* c, f32* d);
WVert* fn_802DD42C(int size);
int fn_802C9C50(void);
int fn_802C9C40(void);
int fn_802C9C38(void);
int fn_802C9C48(void);
u32 fn_80147C50(u32 a, u32 b, f32 t);
void TransformWorldToScreen(const PVec* p, PVec* out);
void fn_802DBE4C(void);
void fn_802DBECC(void);
void fn_802C8BD0(s32 tex);
void fn_802DCFCC(s32 target, s32 dtex, f32 a, f32 b, s32 mask, u32 color);
void DrawWaterGeometry(WVert* v, int nv, const u16* idx, int ni);
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




struct IdxTable {
    u16 i[6];
};




// The shared reflection cube (0x1B8).
struct Skybox {
    Skybox(const char* sunTex);
    ~Skybox(void) {
        for (int i = 0; i < 6; i++) {
            fn_802CB228(faces[i]);
        }
        if (sun != -2) {
            gxForgetTexture(sun);
        }
    }
    s32 faces[6];      // offset 0x0
    s32 sun;           // offset 0x18
    PlainValMatrix frames[6];   // offset 0x1C
    WVert quad[4];     // offset 0x13C
    u16 idx[6];        // offset 0x19C
    u8 drawn;          // offset 0x1A8
    u8 pad1A9[0x1B8 - 0x1A9];  // offset 0x1A9: 12 more bytes, not touched in this unit (target new Skybox = 0x1B8)
};

class EnvBase {
public:
    virtual ~EnvBase(void);
    virtual void Render(void* cam, f32 fov) = 0;
    virtual s32 Target(void) = 0;
    virtual int IsValid(void) = 0;
    virtual f32 Scale(void) = 0;
    virtual void Mirror(PlainValMatrix* out, const PlainValMatrix* in, const PVec* up);
};

class WaterBase {
public:
    virtual ~WaterBase(void);
    virtual void Reflect(void* cam, f32 fov) = 0;
    virtual void Draw(void) = 0;
    virtual int IsValid(void) = 0;
};

class WaterEnvMap : public EnvBase {
public:
    void RenderFaces(f32 h, PlainValMatrix& m);
    void RenderBody(void* cam, f32 fov);
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
// Unused in ET: only its texture name survives in .data.
static const char* SurfTex(void) { return "gen_surf.tex"; }  /* Helper name unproven */
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
f32 waterHeight;
u32 farWaterColor;  // far water color (the log line labels it N)
u32 nearWaterColor;  // near water color
u32 deepWaterSubColor;
u32 shimWaterColor;
f32 lbl_8055FAF4;
f32 lbl_8055FAF8;
s32 lbl_8055FAFC;
Skybox* lbl_8055FB00;
f32 lbl_8055FB04;
WaterBase* lbl_804C07F0[4];


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
    f32 s = 1.0f / MaxF(0.001f, PSVECMag((const Vec*)v));
    v->x *= s;
    v->y *= s;
    v->z *= s;
}

void LoadWaterGraphicsSettings(void) {
    farWaterColor = 0xE0005030;
    waterHeight = 0.7f;
    nearWaterColor = 0xE0FFFFFF;
    deepWaterSubColor = 0x00202040;
    shimWaterColor = 0xE0001040;
    ValueTable* t = fn_80131808("track.val");
    if (t) {
        ValueNode* g = t->HasChild("Graphics") ? t->FindChild("Graphics") : 0;
        if (g) {
            waterHeight = g->GetFloat("water_height", waterHeight);
            nearWaterColor = fn_802BECE4(g->GetString("near_water_color", ""), nearWaterColor);
            farWaterColor = fn_802BECE4(g->GetString("far_water_color", ""), farWaterColor);
            deepWaterSubColor = fn_802BECE4(g->GetString("deep_water_sub_color", ""), deepWaterSubColor);
            shimWaterColor = fn_802BECE4(g->GetString("shim_water_color", ""), shimWaterColor);
            fn_802E30C8("water colors: N %08x, F %08x DS %08x SH %08x", farWaterColor, nearWaterColor,
                        deepWaterSubColor, shimWaterColor);
        }
        fn_80131818(t);
    }
    lbl_8055FAF4 = waterHeight;
}

f32 fn_802BEFB4(void) {
    u8* c = GetCarInfo(fn_801E866C(-1));
    f32 h = *(f32*)(*(u8**)(c + 8) + 0x64);
    f32 t = MaxF(MinFRev((h - 22.222223f) / 44.444443f, 1.0f), 0.0f);
    return 0.7f + Mul(t, 1.3f);
}

// Maps x from [x0, x1] onto [y0, y1], clamped to the segment.
/* Helper name unproven */
static inline f32 MapClamped_hm(f32 x0, f32 x1, f32 y0, f32 y1, f32 x) {
    f32 u = MaxF(MinFRev(Unlerp(x, x0, x1), 1.0f), 0.0f);
    return y0 + u * (y1 - y0);
}
f32 fn_802BF02C(f32 x) {
    PtTable t = {{
        {0.0f, 0.6f}, {0.62831855f, 0.4f}, {1.2566371f, 0.0f}, {3.1415927f, 0.0f},
        {5.340708f, 1.0f}, {5.969026f, 1.0f}, {6.2831855f, 0.6f},
    }};
    for (unsigned int i = 1; i < 7; i++) {
        if (t.p[i].x > x) {
            return MapClamped_hm(t.p[i - 1].x, t.p[i].x, t.p[i - 1].y, t.p[i].y, x);
        }
    }
    return t.p[6].y;
}


void UpdateWaterRings(void) {
    if (lbl_8055CF6C == -1) {
        return;
    }
    f32 k = fn_802BEFB4();
    lbl_8055FAF8 += k * fn_801E7E24();
    RingTable rings = {{
        {0.65f, 0.0f, 0.0f}, {0.65f, 0.25f, 2.07345128f}, {0.65f, 0.5f, 4.1783185f},
    }};
    fn_802C9648(1.01f);
    fn_802D3D6C();
    for (u32 i = 0; i < 3; i++) {
        f32 a = fmod(lbl_8055FAF8 + rings.r[i].phase, 6.2831855f);
        f32 w;
        f32 c = cos(a);
        w = 1.0f - rings.r[i].scale;
        f32 s = MaxF(MinFRev((c - -1.0f) / 2.0f, 1.0f), 0.0f);
        f32 h = Lerp(0.0f, w, s);
        TexMtx m = {{{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}}};
        f32 top = h + rings.r[i].scale;
        f32 inv = 1.0f / rings.r[i].scale;
        f32 off = -inv * h;
        m.m[0][3] = rings.r[i].y;
        m.m[1][1] = (1.0f + inv * (1.0f - top)) - off;
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
    LoadWaterGraphicsSettings();
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
        mrModelUnload(lbl_8055CF6C);
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
    UpdateWaterRings();
}

f32 fn_802BF590(void) { return lbl_8055FAF4; }

void fn_802BF598(void* cam, f32 fov) {
    int v = fn_801E85B4();
    if (lbl_804C07F0[v]) {
        lbl_804C07F0[v]->Reflect(cam, fov);
    }
}
}

void EnvBase::Mirror(PlainValMatrix* out, const PlainValMatrix* in, const PVec* up) {
    *out = *in;
    out->basis[0].y *= -1.0f;
    out->basis[1].y *= -1.0f;
    out->basis[2].y *= -1.0f;
    out->origin.y *= -1.0f;
    out->origin.y = out->origin.y + 2.0f * waterHeight;
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

static inline void LoadModel(s32* out, const char* name) {
    if (fn_8012C33C(name)) {
        *out = fn_802C6FC0(name, 0, 0);
    } else {
        *out = -1;
    }
}

static inline void LoadTex(s32* out, const char* name) { /* Helper name unproven */
    if (fn_8012C33C(name)) {
        *out = fn_802CAC28(name, 0);
    } else {
        *out = -2;
    }
}

static inline void Identity(PlainValMatrix* f) {
    f->basis[0].x = 1.0f;
    f->basis[0].y = 0.0f;
    f->basis[0].z = 0.0f;
    f->basis[1].x = 0.0f;
    f->basis[1].y = 1.0f;
    f->basis[1].z = 0.0f;
    f->basis[2].x = 0.0f;
    f->basis[2].y = 0.0f;
    f->basis[2].z = 1.0f;
}


inline Skybox::Skybox(const char* sunTex) {
    LoadTex(&sun, sunTex);
    for (int i = 0; i < 6; i++) {
        faces[i] = fn_802CB128(0x100, 0x100, 0, 0x60);
        PlainValMatrix* f = &frames[i];
        Identity(f);
        frames[i].origin.x = 0.0f;
        frames[i].origin.y = 0.0f;
        frames[i].origin.z = 0.0f;
    }
    frames[1].basis[2].x = 0.0f;
    frames[1].basis[2].y = 0.0f;
    frames[1].basis[2].z = -1.0f;
    frames[1].basis[0].x = -1.0f;
    frames[1].basis[0].y = 0.0f;
    frames[1].basis[0].z = 0.0f;
    frames[2].basis[0].x = 0.0f;
    frames[2].basis[0].y = 0.0f;
    frames[2].basis[0].z = -1.0f;
    frames[2].basis[2].x = 1.0f;
    frames[2].basis[2].y = 0.0f;
    frames[2].basis[2].z = 0.0f;
    frames[3].basis[0].x = 0.0f;
    frames[3].basis[0].y = 0.0f;
    frames[3].basis[0].z = 1.0f;
    frames[3].basis[2].x = -1.0f;
    frames[3].basis[2].y = 0.0f;
    frames[3].basis[2].z = 0.0f;
    frames[4].basis[1].x = 0.0f;
    frames[4].basis[1].y = 0.0f;
    frames[4].basis[1].z = -1.0f;
    frames[4].basis[2].x = 0.0f;
    frames[4].basis[2].y = 1.0f;
    frames[4].basis[2].z = 0.0f;
    frames[5].basis[1].x = 0.0f;
    frames[5].basis[1].y = 0.0f;
    frames[5].basis[1].z = 1.0f;
    frames[5].basis[2].x = 0.0f;
    frames[5].basis[2].y = -1.0f;
    frames[5].basis[2].z = 0.0f;
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
    {
        const char* name = "worldmodel.mod";
        LoadModel(&world, name);
    }
    {
        const char* name = "wenv_mod.mod";
        LoadModel(&envModel, name);
    }
    if (lbl_8055FAFC == 0) {
        Skybox* sb = new ("skybox") Skybox("env_sun.tex");
        lbl_8055FB00 = sb;
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
        mrModelUnload(world);
        world = -1;
    }
    if (envModel != -1) {
        mrModelUnload(envModel);
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
extern "C" void fn_802BFE48(Skybox* s, const PlainValMatrix* view, f32 scale);

/* Helper name unproven: renders the six cube faces of the environment map. */
inline void WaterEnvMap::RenderFaces(f32 h, PlainValMatrix& m) {
    int i;
    f32 hf = h;
    Skybox* k = sky;
    fn_802C00A0(&hf);
    Perspective ps(0.1f, 2.0f, hf);
    for (i = 0; i < 6; i++) {
        PlainValMatrix f;
        __ct__10CamFrame34Fv(&f);
        fn_802C0054((Mat3*)&f, (const Mat3*)&k->frames[i]);
        fn_80144DAC(&f.origin, &m.origin);
        fn_802C8B98(k->faces[i], &f);
        fn_802C8DB8(k->quad, 4, k->idx, 6);
        fn_802C8D74();
    }
    fn_802BFE48(k, &m, 1.0f);
}

inline void WaterEnvMap::RenderBody(void* cam, f32 fov) {
    scale = (fn_801E85A4() == 2) ? 1.5f : 1.0f;
    int i;
    Skybox* s = sky;
    if (!s->drawn) {
        for (i = 0; i < 6; i++) {
            fn_802CB274(s->faces[i]);
            fn_802C9294(s->faces[i], 0);
            Perspective p(1.0f, 2000.0f, 1.5707964f);
            Viewport vp;
            __ct__8ViewportFiiii(&vp, 0, 0, 0x100, 0x100);
            PlainValMatrix save;
            fn_802C8B08(&save);
            fn_802C8638(&s->frames[i]);
            fn_802C88BC(3);
            fn_802C10C8();
            fn_802C9294(-2, 0);
            fn_802C8638(&save);
            __dt__8ViewportFv(&vp, -1);
        }
        s->drawn = 1;
    }
    fn_802CB274(target);
    fn_802C9294(target, 1);
    PVec up;
    PlainValMatrix save;
    PlainValMatrix m;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    Mirror(&m, (const PlainValMatrix*)cam, &up);
    f32 h = 2.0f * (f32)atan(scale * (f32)tan(0.5f * fov));
    Viewport vp;
    Perspective p(1.0f, 3000.0f, h);
    __ct__8ViewportFiiii(&vp, 0, 0, 0x100, 0x100);
    fn_802C8B08(&save);
    fn_802C8638(&m);
    fn_802C88BC(2);
    if (sky) {
        RenderFaces(h, m);
    }
    fn_802C7040(world);
    if (fn_801E85A4() == 1) {
        RenderFarTreeBatch(300.0f);
    }
    if (envModel != -1) {
        fn_802C7040(envModel);
    }
    fn_802C9294(-2, 0);
    fn_802C8638(&save);
    __dt__8ViewportFv(&vp, -1);
}

void WaterEnvMap::Render(void* cam, f32 fov) { RenderBody(cam, fov); }

extern "C" {

void fn_802BFE48(Skybox* __restrict s, const PlainValMatrix* view, f32 scale) {
    Mat3* mp;
    if (s->sun == -2) {
        return;
    }
    IdxTable idx = {{0, 1, 2, 2, 1, 3}};
    f32 r = 0.3f * scale;
    WVert v[4];
    for (int i = 0; i < 4; i++) {
        v[i].pos.x = (i & 1) ? r : -r;
        v[i].pos.y = (i & 2) ? r : -r;
        v[i].pos.z = (0.9f * scale);
        v[i].u = (i & 1) ? 1.0f : 0.0f;
        v[i].v = (i & 2) ? 1.0f : 0.0f;
        v[i].color = 0xFFFFFFFF;
    }
    PlainValMatrix m;
    mp = (Mat3*)&m;
    m.origin = view->origin;
    m.basis[0].x = 1.0f;
    m.basis[0].y = 0.0f;
    m.basis[0].z = 0.0f;
    m.basis[1].x = 0.0f;
    m.basis[1].y = 1.0f;
    m.basis[1].z = 0.0f;
    m.basis[2].x = 0.0f;
    m.basis[2].y = 0.0f;
    m.basis[2].z = 1.0f;
    PVec a;
    a.x = -0.5235988f;
    a.y = 0.0f;
    a.z = 0.0f;
    Mat3 r30;
    Mat3FromEuler(&r30, (const Vec3*)&a);
    Mat3Mul(mp, &r30, mp);
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

/* Helper names and scoped auto-inline setting unproven. */
#pragma push
#pragma auto_inline on
// Helper name unproven.
static inline f32 WaterDepthScale(f32 height) { return MaxF(0.25f * height, 1.0f); }

static double WaterScrollOffset(f32 ticks) { return fmod(0.39999998f * ticks, 1.0); }
static f32 WaterCoordinate(int index) { return (f32)index; }
#pragma pop

// Name unproven.
static inline void EmitWaterQuad(u16* p, int a, int b) {
    p[0] = a;
    ++a;
    p[1] = a;
    p[2] = b;
    p[3] = b;
    p[4] = a;
    ++b;
    p[5] = b;
}

WaterSystem::WaterSystem(void) {
    const char* mskName = "watermsk.tex";
    const char* deepName = "gen_waterd.tex";
    valid = 0;
    mask = -2;
    deep = -2;
    env = 0;
    lbl_8055FB04 = 0.2f;
    if (!fn_8012C33C("gen_water.tex")) {
        return;
    }
    if (fn_8012C33C(mskName)) {
        mask = fn_802CAC28(mskName, 0);
    }
    env = fn_802BF6AC();
    if (fn_8012C33C(deepName)) {
        deep = fn_802CAC28(deepName, 0);
    }
    u16* p = &idx[0][0][0];
    for (int r = 0; r < 63; r++) {
        int row = r * 16;
        int next = (r + 1) * 16;
        for (int c = 0; c < 15; c++) {
            EmitWaterQuad(p, row + c, next + c);
            p += 6;
        }
    }
    valid = 1;
}

WaterSystem::~WaterSystem(void) {
    if (mask != -2) {
        gxForgetTexture(mask);
    }
    if (deep != -2) {
        gxForgetTexture(deep);
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
    u32 a = (c >> 24) & 0xFF;
    u32 r = (c >> 16) & 0xFF;
    u32 g = (c >> 8) & 0xFF;
    u32 b = c & 0xFF;
    return (g << 16 | b << 8) | (r << 24 | a);
}

// Helper names unproven: vector steps as the target evaluates them.
static inline void MadV3(void* o_, const void* a_, const void* b_, f32 s) {
    f32* o = (f32*)o_;
    const f32* a = (const f32*)a_;
    const f32* b = (const f32*)b_;
    o[0] = a[0] + b[0] * s;
    o[1] = a[1] + b[1] * s;
    o[2] = a[2] + b[2] * s;
}

template <class O, class A, class D> static inline void AddScaledK(O* o, const A* a, const D* d, f32 s) { o->x = a->x + d->x * s; o->y = a->y + d->y * s; o->z = a->z + d->z * s; }  /* Helper name unproven */
/* Helper name unproven */
static inline void CrossUpR(PVec* out, const PVec* a) {
    PVec up;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    PSVECCrossProduct((const Vec*)a, (const Vec*)&up, (Vec*)out);
}
/* Helper name unproven */
static inline void CrossUpL(PVec* out, const PVec* a) {
    PVec up;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    PSVECCrossProduct((const Vec*)&up, (const Vec*)a, (Vec*)out);
}

/* Helper name unproven */
struct WaterPt {
    WaterPt(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
    f32 x, y, z;
};

void WaterSystem::Draw(void) {
    PlainValMatrix cam;
    fn_802C8B08(&cam);
    f32 h = cam.origin.y - lbl_8055FAF4;
    f32 sx;
    f32 farZ;
    f32 nearZ;
    f32 dz;
    fn_802C98DC(&sx, &nearZ, &farZ, &dz);
    f32 nd = nearZ;
    f32 tilt = h * cam.basis[2].y;
    f32 far = 0.95f * farZ;
    sx = sx * (f32)sqrt(1.5625);
    WVert* verts = fn_802DD42C(0x6000);
    f32 k;
    f32 vx = fn_802C9C50();
    f32 vy = WaterCoordinate(fn_802C9C40());
    f32 vw = fn_802C9C38();
    f32 vh = fn_802C9C48();
    k = 640.0f / (vh * env->Scale());
    PVec fwd;
    if (FAbs(cam.basis[2].y) < 0.99f) {
        fwd = cam.basis[2];
    } else {
        fwd = cam.basis[1];
    }
    fwd.y = 0.0f;
    f32 inv = 1.0f / MaxF(0.01f, PSVECMag((const Vec*)&fwd));
    fwd.x *= inv;
    fwd.y *= inv;
    fwd.z *= inv;
    PVec side;
    CrossUpR(&side, &fwd);
    for (int r = 0; r < 64; r++) {
        f32 t = (f32)r / 63.0f * ((f32)r / 63.0f);
        PVec p = cam.origin;
        p.y = lbl_8055FAF4;
        if (r == 63) {
            p.y = cam.origin.y;
        }
        f32 dist = nd + t * (far - nd) + tilt;
        p.x += fwd.x * dist;
        p.y += fwd.y * dist;
        p.z += fwd.z * dist;
        // The row point in camera space: struct-lane deltas, named Dot3 results copied back (Sound3D::PreUpdate's shape).
        PVec lc;
        lc.x = p.x - cam.origin.x;
        lc.y = p.y - cam.origin.y;
        lc.z = p.z - cam.origin.z;
        f32 x = Dot3(lc.x, cam.basis[0].x, lc.y, cam.basis[0].y, lc.z, cam.basis[0].z);
        f32 y = Dot3(lc.x, cam.basis[1].x, lc.y, cam.basis[1].y, lc.z, cam.basis[1].z);
        f32 z = Dot3(lc.x, cam.basis[2].x, lc.y, cam.basis[2].y, lc.z, cam.basis[2].z);
        lc.x = x;
        lc.y = y;
        lc.z = z;
        PVec rel;
        rel.x = p.x - cam.origin.x;
        rel.y = p.y - cam.origin.y;
        rel.z = p.z - cam.origin.z;
        f32 depth = lc.z;
        fn_802BED90(&rel);
        f32 f = MaxF(MinFRev(1.0f - -rel.y, 1.0f), 0.0f);
        u32 col = fn_802C03E0(fn_80147C50(farWaterColor, nearWaterColor, f * f));
        for (int c = 0; c < 16; c++) {
            WVert* v = &verts[r * 16 + c];
            f32 u = (2.0f * (f32)c / 15.0f - 1.0f) * depth * sx;
            PVec wp;
            wp.x = p.x + side.x * u;
            wp.y = p.y + side.y * u;
            wp.z = p.z + side.z * u;
            v->pos = wp;
            v->color = col;
            PVec s;
            TransformWorldToScreen(&v->pos, &s);
            // Viewport centring offsets, written in the loop (the compiler hoists them; their constants are created here).
            f32 cx = 0.5f * (640.0f - vh);
            f32 cy = 0.5f * (480.0f - vw);
            v->u = (s.x - vx + cx) / 640.0f;
            v->v = (s.y - vy + cy) / 480.0f;
            v->u = 0.5f + (v->u - 0.5f) * k;
            v->v = 0.5f + 0.75f * ((v->v - 0.5f) * k);
        }
    }
    // Built and never read: retail keeps the three constructor stores (sp+0xA4..). Name unproven.
    WaterPt anchor(cam.origin.x, 0.0f, cam.origin.z);
    fn_802DBE4C();
    TexMtx m = {{{0.04f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.04f, 0.0f}}};
    m.m[0][3] = WaterScrollOffset(fn_801E7E18());
    fn_802D3D94(2, &m);
    f32 sh = WaterDepthScale(cam.origin.y);
    u32 deepCol = fn_80147C50(0, deepWaterSubColor, 1.0f / sh);
    fn_802C8BD0(env->Target());
    fn_802DCFCC(env->Target(), deep, lbl_8055CF74, 1.0f / (1024.0f * lbl_80564D78[0]), mask, deepCol);
    DrawWaterGeometry(verts, 0x400, &idx[0][0][0], 0x1626);
    fn_802DD228();
    fn_802C8D74();
    fn_802DBECC();
    PlainValMatrix cam2;
    fn_802C8B08(&cam2);
    f32 nz;
    f32 dummy;
    fn_802C98DC(&dummy, &nz, &dummy, &dummy);
    nz += 0.001f;
    PVec c;
    MadV3(&c.x, &cam2.origin.x, &cam2.basis[2].x, nz);
    f32 sc = (lbl_8055FAF4 - c.y) / MaxF(0.001f, cam2.basis[1].y);
    PVec q;
    MadV3(&q.x, &c.x, &cam2.basis[1].x, sc);
    PVec ax = cam2.basis[2];
    ax.y = 0.0f;
    PVec fw;
    CrossUpL(&fw, &ax);
    PVec e0;
    PVec e1;
    AddScaledK(&e0, &q, &fw, -10.0f);
    AddScaledK(&e1, &q, &fw, 10.0f);
    IdxTable qi = {{0, 1, 2, 2, 1, 3}};
    WVert sv[4];
    for (int i = 0; i < 4; i++) {
        sv[i].pos = *((i & 1) ? &e1 : &e0);
        sv[i].pos.x += cam2.basis[1].x * ((i & 2) ? -10.0f : 0.0f);
        sv[i].pos.y += cam2.basis[1].y * ((i & 2) ? -10.0f : 0.0f);
        sv[i].pos.z += cam2.basis[1].z * ((i & 2) ? -10.0f : 0.0f);
        sv[i].v = 0.0f;
        sv[i].u = 0.0f;
        sv[i].color = shimWaterColor;
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
        gxForgetTexture(tex);
    }
}

void TexEnv::Render(void* cam, f32 fov) {}

s32 TexEnv::Target(void) { return tex; }

f32 TexEnv::Scale(void) { return 1.0f; }

int TexEnv::IsValid(void) { return 1; }

int WaterSystem::IsValid(void) { return valid; }
