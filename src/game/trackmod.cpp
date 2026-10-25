// Track model handlers (0x801F1C24-0x801F4CD8, "trackmod"): the per-type handlers ("TMH") for
// special track model objects: camera-facing sprites (point, axis and multi-angle facing), the sky
// cone mask, the sun and the lighting colours, rotators, cross-faders, flare sources, scrolling
// textures, the facing test and its per-frame instances. Objects are created by name; their data is
// byte-swapped on load. Names are guesses apart from the strings.
#include "game/common.h"

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

struct TVec {
    TVec(void);
    f32 x, y, z;
};

struct PVec {
    f32 x, y, z;
};

struct TFrame {
    PVec x;    // offset 0x0
    PVec y;    // offset 0xC
    PVec z;    // offset 0x18
    PVec pos;  // offset 0x24
};

struct TMat3 {
    f32 m[3][3];
};

struct TMtx {
    TMat3 r;   // offset 0x0
    f32 t[3];  // offset 0x24
};

struct TCam {
    u8 pad0[0x18];
    f32 dir[3];  // offset 0x18
    f32 pos[3];  // offset 0x24
};

// A track object instance handed to its handler.
struct TObj {
    u8 pad0[4];
    void* model;  // offset 0x4
    u8 pad8[4];
    TObj* next;   // offset 0xC
    u8 pad10[4];
    s32 slot;     // offset 0x14
    u8 pad18[8];
    f32 pos[3];   // offset 0x20
    s32 d2C;      // offset 0x2C
    f32 d30;      // offset 0x30
    TMat3 rot;    // offset 0x34
};

struct TNode {
    TObj* obj;     // offset 0x0
    TNode* left;   // offset 0x4
    TNode* right;  // offset 0x8
};

extern "C" {
TCam* fn_802C86DC(void);
void PSVECCrossProduct(const void* a, const void* b, void* out);
f32 PSVECDotProduct(const void* a, const void* b);
void PSVECNormalize(const void* src, void* dst);
void fn_802C7730(TFrame* f);
void fn_802C7040(void* model);
void* fn_802CAC28(const char* name, int a);
void fn_802CAC54(void* tex);
}

// Every handler's base (vtable slots 0x8-0x2C).
class TrackMod {
public:
    virtual ~TrackMod(void) {}
    virtual void Swap(TObj* o);                              // 0xC
    virtual void Init(TObj* o);                              // 0x10
    virtual void Unk14(void);                                // 0x14
    virtual void Apply(void);                                // 0x18
    virtual void Unk1C(void);                                // 0x1C
    virtual void Draw(TObj* list);                           // 0x20
    virtual void DrawTree(TNode* tree);                      // 0x24
    virtual void Add(void* scene, TObj* o, s32 id);          // 0x28
    virtual void Link(int a, void* b, int c, void* d, int e);  // 0x2C
};

// A per-scene draw instance created by Add (vtable slots 0x8-0x14).
class TrackInst {
public:
    virtual ~TrackInst(void) {}
    virtual void Draw(void* ctx);           // 0xC
    virtual int Kind(void);                 // 0x10
    virtual void Draw2(void* ctx);          // 0x14
};

class GenericTMH : public TrackMod {
public:
    virtual ~GenericTMH(void);
    virtual void Draw(TObj* list);
};

class PointFacingTMH : public TrackMod {
public:
    virtual ~PointFacingTMH(void);
    virtual void Draw(TObj* list);
};

class AxisFacingTMH : public TrackMod {
public:
    virtual ~AxisFacingTMH(void);
    virtual void Draw(TObj* list);
    virtual void DrawTree(TNode* tree);
};

class SkyTMH : public TrackMod {
public:
    SkyTMH(void) {
        obj = 0;
        mtx.r.m[0][0] = 1.0f;
        mtx.r.m[0][1] = 0.0f;
        mtx.r.m[0][2] = 0.0f;
        mtx.r.m[1][0] = 0.0f;
        mtx.r.m[1][1] = 1.0f;
        mtx.r.m[1][2] = 0.0f;
        mtx.r.m[2][0] = 0.0f;
        mtx.r.m[2][1] = 0.0f;
        mtx.r.m[2][2] = 1.0f;
        mtx.t[0] = 0.0f;
        mtx.t[1] = 0.0f;
        mtx.t[2] = 0.0f;
        mode = 1;
    }
    virtual ~SkyTMH(void);
    virtual void Swap(TObj* o);
    virtual void Init(TObj* o);
    virtual void Draw(TObj* list);

    s32 visible;  // offset 0x4
    s32 mode;     // offset 0x8
    TMtx mtx;     // offset 0xC
    TObj* obj;    // offset 0x3C
};

class SunTMH : public TrackMod {
public:
    SunTMH(void) { color = 0; }
    virtual ~SunTMH(void);
    virtual void Swap(TObj* o);
    virtual void Init(TObj* o);
    virtual void Apply(void);
    virtual void Draw(TObj* list);

    f32 dir[3];  // offset 0x4
    u32 color;   // offset 0x10
};

class AmbientColorTMH : public TrackMod {
public:
    AmbientColorTMH(void) {
        color = 0;
        set = 0;
    }
    virtual ~AmbientColorTMH(void);
    virtual void Init(TObj* o);
    virtual void Apply(void);
    virtual void Draw(TObj* list);

    u8 set;     // offset 0x4
    u32 color;  // offset 0x8
};

class ShadowColorTMH : public TrackMod {
public:
    ShadowColorTMH(void) {
        color = 0;
        set = 0;
    }
    virtual ~ShadowColorTMH(void);
    virtual void Swap(TObj* o);
    virtual void Init(TObj* o);
    virtual void Apply(void);
    virtual void Draw(TObj* list);

    u8 set;     // offset 0x4
    u32 color;  // offset 0x8
};

class SunIntensityTMH : public TrackMod {
public:
    SunIntensityTMH(void) {
        value = 0.0f;
        set = 0;
    }
    virtual ~SunIntensityTMH(void);
    virtual void Swap(TObj* o);
    virtual void Init(TObj* o);
    virtual void Apply(void);
    virtual void Draw(TObj* list);

    u8 set;    // offset 0x4
    f32 value; // offset 0x8
};

class FogColorTMH : public TrackMod {
public:
    FogColorTMH(void) {
        color = 0;
        set = 0;
    }
    virtual ~FogColorTMH(void);
    virtual void Swap(TObj* o);
    virtual void Init(TObj* o);
    virtual void Apply(void);
    virtual void Draw(TObj* list);

    u8 set;     // offset 0x4
    u32 color;  // offset 0x8
};

// One rotating model (0x18).
struct Rot {
    Rot(void);
    s32 id;        // offset 0x0
    f32 pos[3];    // offset 0x4
    TMat3* rot;    // offset 0x10
    f32 speed;     // offset 0x14
};

class RotList : public TrackInst {
public:
    RotList(void) {
        rots = new ("rotator tab") Rot[16];
        count = 0;
    }
    virtual ~RotList(void);
    virtual void Draw(void* ctx);

    Rot* rots;  // offset 0x4
    s32 count;  // offset 0x8
};

class RotatorTMH : public TrackMod {
public:
    virtual ~RotatorTMH(void);
    RotatorTMH(void) { list = 0; }
    virtual void Swap(TObj* o);
    virtual void Draw(TObj* list);
    virtual void Add(void* scene, TObj* o, s32 id);

    RotList* list;  // offset 0x4
};

class MultiFacingTMH : public TrackMod {
public:
    MultiFacingTMH(void) { tex = fn_802CAC28("turn.tex", 0); }
    virtual ~MultiFacingTMH(void);
    virtual void Draw(TObj* list);

    void* tex;  // offset 0x4
};

class OptTerrainTMH : public TrackMod {
public:
    virtual ~OptTerrainTMH(void);
    virtual void Swap(TObj* o);
    virtual void Draw(TObj* list);
    virtual void Link(int a, void* b, int c, void* d, int e);
};

// One cross-fading model (0x1C).
struct XFade {
    f32 pos[3];    // offset 0x0
    void* model;   // offset 0xC
    f32 scale;     // offset 0x10
    f32 start;     // offset 0x14
    s32 id;        // offset 0x18
};

class XFadeList : public TrackInst {
public:
    XFadeList(void) { count = 0; }
    virtual ~XFadeList(void);
    virtual void Draw(void* ctx);
    virtual int Kind(void);
    virtual void Draw2(void* ctx);

    XFade fades[48];  // offset 0x4
    s32 count;        // offset 0x544
};

struct XFader {
    f32 pos[3];   // offset 0x0
    void* model;  // offset 0xC
    f32 scale;    // offset 0x10
    f32 start;    // offset 0x14
};

class XFaderTMH : public TrackMod {
public:
    XFaderTMH(void) {
        faders = 0;
        list = 0;
    }
    virtual ~XFaderTMH(void);
    virtual void Swap(TObj* o);
    virtual void Init(TObj* o);
    virtual void Draw(TObj* list);
    virtual void Add(void* scene, TObj* o, s32 id);

    XFader* faders;   // offset 0x4
    XFadeList* list;  // offset 0x8
};

class FlareSourceTMH : public TrackMod {
public:
    virtual ~FlareSourceTMH(void);
    virtual void Swap(TObj* o);
    virtual void Init(TObj* o);
    virtual void Draw(TObj* list);
};

class WaterTMH : public TrackMod {
public:
    virtual ~WaterTMH(void);
    virtual void Swap(TObj* o);
    virtual void Draw(TObj* list);
};

class PinkTriangleTMH : public TrackMod {
public:
    virtual ~PinkTriangleTMH(void);
    virtual void Draw(TObj* list);
};

class FacingTestInst : public TrackInst {
public:
    FacingTestInst(u8* d, s32 i) {
        data = d;
        id = i;
    }
    virtual ~FacingTestInst(void);
    virtual void Draw(void* ctx);

    u8* data;  // offset 0x4
    s32 id;    // offset 0x8
};

class FacingTestTMH : public TrackMod {
public:
    virtual ~FacingTestTMH(void);
    virtual void Draw(TObj* list);
    virtual void Add(void* scene, TObj* o, s32 id);
};

// One scrolling-texture model (0x10).
struct Scroll {
    void* model;  // offset 0x0
    s32 id;       // offset 0x4
    f32 u;        // offset 0x8
    f32 v;        // offset 0xC
};

class ScrollList : public TrackInst {
public:
    ScrollList(void) { count = 0; }
    virtual ~ScrollList(void);
    virtual void Draw(void* ctx);
    virtual int Kind(void);
    virtual void Draw2(void* ctx);

    Scroll scrolls[16];  // offset 0x4
    s32 count;           // offset 0x104
};

class ScrollingTMH : public TrackMod {
public:
    ScrollingTMH(void) { list = 0; }
    virtual ~ScrollingTMH(void);
    virtual void Draw(TObj* list);
    virtual void Add(void* scene, TObj* o, s32 id);

    ScrollList* list;  // offset 0x4
};

static inline f32 Angle(f32 a) { return a; }

ModuleTag lbl_8055EBF8;
UnkStaticFFFF lbl_8055EC00;
f32 lbl_8055EC04 = Angle(0.7853982f);  // lowest sun elevation

extern "C" {
void fn_801C0E60(TVec* out, const f32* a, const f32* b);
void fn_80144B54(TMtx* m, TCam* c);
f32* fn_80144B44(TVec* v, f32 x, f32 y, f32 z);
u32 fn_80144C68(int r, int g, int b, int a);
void fn_80144DAC(f32* dst, const f32* src);
void fn_80144DC8(TMat3* m);
void fn_801BFD34(void* vtx, const f32* p, TMtx* m);
void fn_802C8BD0(void* tex);
void fn_802C8DB8(void* vtx, int n, u16* idx, int ni);
void fn_802C8D74(void);
double atan2(double y, double x);
double atan(double x);
double cos(double x);
double sin(double x);
double sqrt(double x);
double fmod(double x, double y);
f32 fn_801E8738(int view);
void fn_802C70AC(void* model, void* mtx, s32 a, u32 mask);
void fn_802C9994(u32 color);
void fn_802E30C8(const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);
s32* fn_801E1F44(void);
void fn_802923C4(u32 color);
void fn_802C9A1C(TVec* dir);
void fn_8029231C(f32* dir);
void fn_802C9950(u32 color);
void fn_80297990(u32 color);
void fn_802923EC(f32 v);
void fn_802C990C(u32 color);
f32 fn_801E7E18(void);
void Mat3RotY(TMat3* m, f32 a);
void Mat3Mul(TMat3* out, const TMat3* a, const TMat3* b);
void fn_802C744C(void* model, TMtx* m);
void fn_802CD320(void* scene, TrackInst* inst);
void fn_802CD33C(void* a, int b, int c);
f32 fn_802C88D4(const void* p);
void fn_802C9900(f32 a);
void fn_802C9904(void);
void fn_802C77F0(void* model, f32* bounds);
ValueNode* fn_802AE414(void* data);
f32 fn_80144514(f32 a0, f32 b0, f32 a1, f32 b1, f32 a2, f32 b2);
void fn_802CDAB4(void* ctx, s32 id, TMtx* m);
int fn_802CDB20(void* ctx, s32 id);
void fn_802CDAFC(void* ctx, s32 id);
void* fn_802C7A3C(f32 u, f32 v, f32 w);
extern u16 lbl_802F9B20[6];
}

static inline void Swap32(u8* p) {
    u8 t = p[0];
    p[0] = p[3];
    p[3] = t;
    t = p[1];
    p[1] = p[2];
    p[2] = t;
}

static inline f32 FMin(f32 a, f32 b) { return b < a ? b : a; }

static inline f32 FMax(f32 a, f32 b) { return a > b ? a : b; }

static inline f32 Clamp01(f32 a) { return FMax(FMin(a, 1.0f), 0.0f); }

extern "C" {
u8* fn_801F1CE8(TObj* o);
int fn_801F1CF0(u8* d);
void fn_801F2570(TNode* n);
void fn_801F3124(void* dst, const void* src);
void fn_801F3158(f32* dst, const f32* src);
f32 fn_801F3174(f32 y, f32 x);
void fn_801F3198(TVec* v);
void fn_801F3E60(void);
void fn_801F3E64(void);

extern const f32 lbl_80562F48;  // -1.0

// Billboard frame facing the camera, upright.
void fn_801F1C24(TFrame* f) {
    TCam* cam = fn_802C86DC();
    f->y.x = 0.0f;
    f->y.y = 1.0f;
    f->y.z = 0.0f;
    f->z.x = cam->dir[0] * lbl_80562F48;
    f->z.y = cam->dir[1] * lbl_80562F48;
    f->z.z = cam->dir[2] * lbl_80562F48;
    PSVECCrossProduct(&f->y, &f->z, &f->x);
    if (PSVECDotProduct(&f->x, &f->x) > 0.0001f) {
        PSVECNormalize(&f->x, &f->x);
    } else {
        f->x.x = 1.0f;
        f->x.y = 0.0f;
        f->x.z = 0.0f;
    }
    PSVECCrossProduct(&f->z, &f->x, &f->y);
}

u8* fn_801F1CE8(TObj* o) { return (u8*)&o->d2C; }

// Whether the data's normal faces away from the camera.
int fn_801F1CF0(u8* d) {
    TCam* cam = fn_802C86DC();
    TVec v;
    v.x = ((f32*)d)[9] - cam->pos[0];
    v.y = ((f32*)d)[10] - cam->pos[1];
    v.z = ((f32*)d)[11] - cam->pos[2];
    return PSVECDotProduct(&v, d + 0x18) < 0.0f;
}

TrackMod* fn_801F1D68(void) { return new ("Generic") GenericTMH; }

TrackMod* fn_801F1DA4(void) { return new ("PointFacing") PointFacingTMH; }

TrackMod* fn_801F1DE4(void) { return new ("AxisFacing") AxisFacingTMH; }

TrackMod* fn_801F1E24(void) { return new ("Sky") SkyTMH; }

TrackMod* fn_801F1EA8(void) { return new ("Sun") SunTMH; }

TrackMod* fn_801F1EEC(void) { return new ("AmbientColor") AmbientColorTMH; }

TrackMod* fn_801F1F38(void) { return new ("ShadowColor") ShadowColorTMH; }

TrackMod* fn_801F1F84(void) { return new ("SunIntensity") SunIntensityTMH; }

TrackMod* fn_801F1FD4(void) { return new ("FogColor") FogColorTMH; }

TrackMod* fn_801F2020(void) { return new ("Rotator") RotatorTMH; }

TrackMod* fn_801F2064(void) { return new ("MultiFacing") MultiFacingTMH; }

TrackMod* fn_801F20C8(void) { return new ("OptTerrain") OptTerrainTMH; }

TrackMod* fn_801F2108(void) { return new ("XFader") XFaderTMH; }

TrackMod* fn_801F2150(void) { return new ("FlareSource") FlareSourceTMH; }

TrackMod* fn_801F2190(void) { return new ("Water") WaterTMH; }

TrackMod* fn_801F21CC(void) { return new ("PinkTriangle") PinkTriangleTMH; }

TrackMod* fn_801F220C(void) { return new ("FacingTest") FacingTestTMH; }

TrackMod* fn_801F224C(void) { return new ("Scrolling") ScrollingTMH; }
}

FacingTestTMH::~FacingTestTMH(void) {}

PinkTriangleTMH::~PinkTriangleTMH(void) {}

WaterTMH::~WaterTMH(void) {}

FlareSourceTMH::~FlareSourceTMH(void) {}

OptTerrainTMH::~OptTerrainTMH(void) {}

RotatorTMH::~RotatorTMH(void) {}

PointFacingTMH::~PointFacingTMH(void) {}

AxisFacingTMH::~AxisFacingTMH(void) {}

GenericTMH::~GenericTMH(void) {}

void GenericTMH::Draw(TObj* list) {
    for (; list; list = list->next) {
        fn_802C7040(list->model);
    }
}

void AxisFacingTMH::DrawTree(TNode* tree) {
    TFrame f;
    fn_801F1C24(&f);
    fn_802C7730(&f);
    if (tree) {
        fn_801F2570(tree->left);
        fn_802C7040(tree->obj->model);
        fn_801F2570(tree->right);
    }
}

extern "C" inline void fn_801F2570(TNode* n) {
    if (n) {
        fn_801F2570(n->left);
        fn_802C7040(n->obj->model);
        fn_801F2570(n->right);
    }
}

void AxisFacingTMH::Draw(TObj* list) {
    TFrame f;
    fn_801F1C24(&f);
    fn_802C7730(&f);
    for (; list; list = list->next) {
        fn_802C7040(list->model);
    }
}

void PointFacingTMH::Draw(TObj* list) {
    TFrame f;
    fn_801F1C24(&f);
    fn_802C7730(&f);
    for (; list; list = list->next) {
        fn_802C7040(list->model);
    }
}

MultiFacingTMH::~MultiFacingTMH(void) { fn_802CAC54(tex); }

// A textured quad: position, colour and texture coordinates (0x18).
struct TVtx {
    f32 pos[3];  // offset 0x0
    u32 color;   // offset 0xC
    f32 u;       // offset 0x10
    f32 v;       // offset 0x14
};

void MultiFacingTMH::Draw(TObj* list) {
    for (; list; list = list->next) {
        TCam* cam = fn_802C86DC();
        TVec dir;
        TVec c0;
        TVec c1;
        TVec c2;
        TVec c3;
        TVec c4;
        TVtx base;
        TMtx m;
        TVtx v[4];
        u16 idx[6];
        int seg;
        f32 u0;
        f32 v0;
        f32 u1;
        f32 v1;
        fn_801C0E60(&dir, list->pos, cam->pos);
        fn_801F3198(&dir);
        seg = (int)(32.0f * (-fn_801F3174(dir.x, dir.z) / 6.2831855f));
        if (seg < 0) {
            seg += 32;
        }
        u0 = 0.125f * (seg % 8);
        v0 = 0.25f * (seg / 8);
        u1 = 0.125f + u0;
        v1 = 0.25f + v0;
        fn_801F3158(base.pos, fn_80144B44(&c0, 0.0f, 0.0f, 0.0f));
        base.u = u0;
        base.v = v0;
        base.color = fn_80144C68(0xFF, 0xFF, 0xFF, 0xFF);
        fn_801F3124(&v[0], &base);
        fn_801F3124(&v[1], &base);
        fn_801F3124(&v[2], &base);
        fn_801F3124(&v[3], &base);
        idx[0] = lbl_802F9B20[0];
        idx[1] = lbl_802F9B20[1];
        idx[2] = lbl_802F9B20[2];
        idx[3] = lbl_802F9B20[3];
        idx[4] = lbl_802F9B20[4];
        idx[5] = lbl_802F9B20[5];
        v[3].u = u1;
        v[1].u = u1;
        v[3].v = v1;
        v[2].v = v1;
        fn_80144B54(&m, cam);
        fn_80144DAC(m.t, list->pos);
        fn_801BFD34(&v[0], fn_80144B44(&c1, -0.5f, 1.0f, 0.0f), &m);
        fn_801BFD34(&v[1], fn_80144B44(&c2, 0.5f, 1.0f, 0.0f), &m);
        fn_801BFD34(&v[2], fn_80144B44(&c3, -0.5f, -1.0f, 0.0f), &m);
        fn_801BFD34(&v[3], fn_80144B44(&c4, 0.5f, -1.0f, 0.0f), &m);
        fn_802C8BD0(tex);
        fn_802C8DB8(v, 4, idx, 6);
        fn_802C8D74();
    }
}

extern "C" {
void fn_801F3124(void* dst, const void* src) { *(TVtx*)dst = *(const TVtx*)src; }

void fn_801F3158(f32* dst, const f32* src) {
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}

f32 fn_801F3174(f32 y, f32 x) { return atan2(y, x); }

void fn_801F3198(TVec* v) { PSVECNormalize(v, v); }
}

SkyTMH::~SkyTMH(void) { obj = 0; }

void SkyTMH::Swap(TObj* o) {
    u8* p = (u8*)&o->d2C;
    int i;
    for (i = 0; i < 32; i++, p += 16) {
        Swap32(p);
        Swap32(p + 4);
        Swap32(p + 8);
        Swap32(p + 12);
    }
}

void SkyTMH::Init(TObj* o) { obj = o; }

void SkyTMH::Draw(TObj* list) {
    visible = 0;
    if (obj && mode) {
        TCam* cam = fn_802C86DC();
        u32 mask;
        mtx.t[0] = cam->pos[0];
        mtx.t[1] = cam->pos[1];
        mtx.t[2] = cam->pos[2];
        if (mode == 1) {
            f32* cone = (f32*)&obj->d2C;
            f32* c = cone;
            f32 half;
            u32 i;
            mask = 1;
            half = 0.5f * (1.25f * fn_801E8738(-1));
            for (i = 0; i < 32; i++, cone += 4, c += 4) {
                f32 d = PSVECDotProduct(cam->dir, cone);
                if (d > (f32)cos(half + c[3])) {
                    mask |= 1 << i;
                    visible++;
                }
            }
        } else {
            visible = 32;
            mask = -1;
        }
        fn_802C70AC(obj->model, &mtx, -1, mask);
    }
}

void SunTMH::Swap(TObj* o) {
    u8* d = fn_801F1CE8(o);
    Swap32(d);
    Swap32(d + 4);
    Swap32(d + 8);
    Swap32(d + 12);
}

SunTMH::~SunTMH(void) { fn_802C9994(-1); }

void SunTMH::Init(TObj* o) {
    u8* d = fn_801F1CE8(o);
    color = *(u32*)d;
    dir[0] = ((f32*)d)[1];
    dir[1] = ((f32*)d)[2];
    dir[2] = ((f32*)d)[3];
}

void SunTMH::Apply(void) {
    TVec d;
    f32 h;
    f32 ny;
    if (color == 0) {
        color = 0xFDFEE7;
        dir[0] = 0.0f;
        dir[1] = 250.0f;
        dir[2] = -150.0f;
        fn_802E30C8("trackmod: track has no sun, making one up.");
    }
    if (*fn_801E1F44() == 1) {
        color = 0x75889B;
        dir[0] = 0.0f;
        dir[1] = 100.0f;
        dir[2] = 30.0f;
    }
    fn_802C9994(color);
    fn_802923C4(color);
    d.x = dir[0] * -1.0f;
    d.y = dir[1] * -1.0f;
    d.z = dir[2] * -1.0f;
    PSVECNormalize(&d, &d);
    h = sqrt(d.x * d.x + d.z * d.z);
    ny = -d.y;
    if ((f32)atan(ny / h) < lbl_8055EC04) {
        fn_802E30C8("trackmod: bad sun direction (%.0f < %.0f degrees); adjusting", 180.0f * (f32)atan(ny / h) / 3.1415927f,
                    180.0f * lbl_8055EC04 / 3.1415927f);
        d.x *= (f32)cos(lbl_8055EC04) / h;
        d.y = -(f32)sin(lbl_8055EC04);
        d.z *= (f32)cos(lbl_8055EC04) / h;
    }
    fn_802E30C8("suntmh: sun envmap coords are %.3f %.3f", 0.5f + 0.28902f * -d.x / (1.0f + -d.y),
                0.5f + 0.28902f * -d.z / (1.0f + -d.y));
    fn_802C9A1C(&d);
}

void SunTMH::Draw(TObj* list) { fn_8029231C(dir); }

AmbientColorTMH::~AmbientColorTMH(void) {}

void AmbientColorTMH::Init(TObj* o) {
    u8* d = fn_801F1CE8(o);
    color = *(u32*)d;
    set = 1;
}

void AmbientColorTMH::Apply(void) {
    if (*fn_801E1F44() == 1) {
        fn_802C9950(0xFF646464);
    } else {
        fn_802C9950(0xFF909090);
    }
}

void AmbientColorTMH::Draw(TObj* list) { fn_802E317C("AmbientColorTMH: shouldn't draw!"); }

ShadowColorTMH::~ShadowColorTMH(void) {}

void ShadowColorTMH::Init(TObj* o) {
    u8* d = fn_801F1CE8(o);
    color = *(u32*)d;
    set = 1;
}

void ShadowColorTMH::Swap(TObj* o) {
    u8* d = fn_801F1CE8(o);
    Swap32(d);
    Swap32(d + 4);
    Swap32(d + 8);
    Swap32(d + 12);
}

void ShadowColorTMH::Apply(void) {
    if (set) {
        fn_80297990(color);
    } else {
        fn_80297990(0x80000000);
    }
}

void ShadowColorTMH::Draw(TObj* list) { fn_802E317C("ShadowColorTMH: shouldn't draw!"); }

SunIntensityTMH::~SunIntensityTMH(void) {}

void SunIntensityTMH::Swap(TObj* o) {
    u8* d = fn_801F1CE8(o);
    Swap32(d);
    Swap32(d + 4);
    Swap32(d + 8);
    Swap32(d + 12);
}

void SunIntensityTMH::Init(TObj* o) {
    u8* d = fn_801F1CE8(o);
    value = (u8)*(u32*)d / 255.0f;
    set = 1;
}

void SunIntensityTMH::Apply(void) {
    if (set) {
        fn_802923EC(value);
    } else {
        fn_802923EC(1.0f);
    }
}

void SunIntensityTMH::Draw(TObj* list) { fn_802E317C("SunIntensityTMH: shouldn't draw!"); }

FogColorTMH::~FogColorTMH(void) {}

void FogColorTMH::Init(TObj* o) {
    u8* d = fn_801F1CE8(o);
    color = *(u32*)d;
    set = 1;
}

void FogColorTMH::Swap(TObj* o) {
    u8* d = fn_801F1CE8(o);
    Swap32(d);
    Swap32(d + 4);
    Swap32(d + 8);
    Swap32(d + 12);
}

void FogColorTMH::Apply(void) {
    if (set) {
        fn_802C990C(color);
    } else {
        fn_802C990C(0xC9C9EB);
    }
}

void FogColorTMH::Draw(TObj* list) { fn_802E317C("FogColorTMH: shouldn't draw!"); }

void RotatorTMH::Swap(TObj* o) {
    u8* p = (u8*)o + 0x18;
    int i;
    for (i = 0; i < 3; i++, p += 12) {
        Swap32(p);
        Swap32(p + 4);
        Swap32(p + 8);
    }
    Swap32((u8*)&o->slot);
}

void RotatorTMH::Draw(TObj* list) {
    for (; list; list = list->next) {
        TMtx m;
        f32 a = list->d30 * fn_801E7E18();
        fn_80144DC8(&m.r);
        Mat3RotY(&m.r, a);
        Mat3Mul(&m.r, &m.r, &list->rot);
        fn_80144DAC(m.t, list->pos);
        fn_802C744C(list->model, &m);
    }
}

void RotatorTMH::Add(void* scene, TObj* o, s32 id) {
    RotList* l;
    f32 speed;
    s32 rid;
    Rot* r;
    if (!list) {
        RotList* nl = new ("mlro") RotList;
        list = nl;
        fn_802CD320(scene, nl);
    }
    l = list;
    speed = o->d30;
    rid = o->d2C;
    if (l->count >= 16) {
        fn_802E317C("trackmod: exceeded rotator limit");
    }
    r = &l->rots[l->count];
    r->id = rid;
    r->pos[0] = o->pos[0];
    r->pos[1] = o->pos[1];
    r->pos[2] = o->pos[2];
    r->rot = &o->rot;
    r->speed = speed;
    l->count++;
}

Rot::Rot(void) {}

extern "C" void fn_801F3E64(void) {}

void OptTerrainTMH::Swap(TObj* o) {
    u8* p = (u8*)&o->d2C;
    Swap32(p);
    o->d2C = 1 << (o->d2C - 1);
}

void OptTerrainTMH::Draw(TObj* list) {}

void OptTerrainTMH::Link(int a, void* b, int c, void* d, int e) {
    fn_802CD33C(b, c, 0);
    fn_802CD33C(d, e, 0);
}

XFaderTMH::~XFaderTMH(void) {}

void XFaderTMH::Swap(TObj* o) {
    Swap32((u8*)&o->d2C);
    Swap32((u8*)&o->d30);
}

void XFaderTMH::Init(TObj* o) {}

void XFaderTMH::Draw(TObj* list) {
    for (; list; list = list->next) {
        s32 i = list->slot;
        if (i >= 0 && i < 48) {
            XFader* f = &faders[i];
            f32 d = fn_802C88D4(f);
            if (d > f->start) {
                fn_802C9900(Clamp01(f->scale * (d - f->start)));
                fn_802C7040(f->model);
                fn_802C9904();
            }
        }
    }
}

void XFaderTMH::Add(void* scene, TObj* o, s32 id) {
    XFadeList* l;
    XFade* f;
    f32 b[6];
    if (!list) {
        XFadeList* nl = new ("xfaderMLO") XFadeList;
        list = nl;
        fn_802CD320(scene, nl);
    }
    l = list;
    if (l->count >= 48) {
        fn_802E317C("trackmod: exceeded xfader limit!");
    }
    f = &l->fades[l->count];
    fn_802C77F0(o->model, b);
    f->pos[0] = o->pos[0] + (b[0] + 0.5f * (b[1] - b[0]));
    f->pos[1] = o->pos[1] + (b[2] + 0.5f * (b[3] - b[2]));
    f->pos[2] = o->pos[2] + (b[4] + 0.5f * (b[5] - b[4]));
    f->model = o->model;
    f->scale = 1.0f / (*(f32*)&o->d2C - o->d30);
    f->start = o->d30;
    l->fades[l->count].id = id;
    l->count++;
}

void FlareSourceTMH::Swap(TObj* o) {
    u8* p = (u8*)&o->d2C;
    Swap32(p);
    Swap32(p + 4);
    Swap32(p + 8);
    Swap32(p + 12);
    Swap32(p + 16);
}

void FlareSourceTMH::Init(TObj* o) { fn_802E317C("trackmod: local flare sources not support"); }

void FlareSourceTMH::Draw(TObj* list) {}

void WaterTMH::Swap(TObj* o) {
    u8* p;
    int i;
    fn_802E317C("Water objects not currently supported");
    p = (u8*)&o->d2C;
    for (i = 0; i < 3; i++, p += 12) {
        Swap32(p);
        Swap32(p + 4);
        Swap32(p + 8);
    }
}

void WaterTMH::Draw(TObj* list) {}

ScrollingTMH::~ScrollingTMH(void) {}

void ScrollingTMH::Draw(TObj* list) {}

void ScrollingTMH::Add(void* scene, TObj* o, s32 id) {
    ScrollList* l;
    Scroll* s;
    ValueNode* vn;
    u8* data = (u8*)&o->d2C;
    if (!list) {
        ScrollList* nl = new ("ScrollingMLO") ScrollList;
        list = nl;
        fn_802CD320(scene, nl);
    }
    l = list;
    s = &l->scrolls[l->count];
    s->model = o->model;
    s->id = id;
    s->u = 0.0f;
    s->v = 0.0f;
    vn = fn_802AE414(data);
    if (vn) {
        s->u = vn->GetReal("u_speed");
        s->v = vn->GetReal("v_speed");
    }
    l->count++;
}

void PinkTriangleTMH::Draw(TObj* list) {}

void FacingTestTMH::Draw(TObj* list) {
    for (; list; list = list->next) {
        void* model = list->model;
        if (fn_801F1CF0((u8*)&list->d2C)) {
            fn_802C7040(model);
        }
    }
}

void FacingTestTMH::Add(void* scene, TObj* o, s32 id) {
    u8* data = (u8*)&o->d2C;
    fn_802CD320(scene, new ("mlfto") FacingTestInst(data, id));
}

void TrackMod::DrawTree(TNode* tree) {}

FacingTestInst::~FacingTestInst(void) {}

ScrollList::~ScrollList(void) {}

XFadeList::~XFadeList(void) {}

RotList::~RotList(void) { delete[] rots; }

static inline void Mat3MulD(TMat3* out, const TMat3* l, const TMat3* r) {
    if (out == l || out == r) {
        f32 m00 = fn_80144514(r->m[0][0], l->m[0][0], r->m[1][0], l->m[0][1], r->m[2][0], l->m[0][2]);
        f32 m10 = fn_80144514(r->m[0][0], l->m[1][0], r->m[1][0], l->m[1][1], r->m[2][0], l->m[1][2]);
        f32 m20 = fn_80144514(r->m[0][0], l->m[2][0], r->m[1][0], l->m[2][1], r->m[2][0], l->m[2][2]);
        f32 m01 = fn_80144514(r->m[0][1], l->m[0][0], r->m[1][1], l->m[0][1], r->m[2][1], l->m[0][2]);
        f32 m11 = fn_80144514(r->m[0][1], l->m[1][0], r->m[1][1], l->m[1][1], r->m[2][1], l->m[1][2]);
        f32 m21 = fn_80144514(r->m[0][1], l->m[2][0], r->m[1][1], l->m[2][1], r->m[2][1], l->m[2][2]);
        f32 m02 = fn_80144514(r->m[0][2], l->m[0][0], r->m[1][2], l->m[0][1], r->m[2][2], l->m[0][2]);
        f32 m12 = fn_80144514(r->m[0][2], l->m[1][0], r->m[1][2], l->m[1][1], r->m[2][2], l->m[1][2]);
        f32 m22 = fn_80144514(r->m[0][2], l->m[2][0], r->m[1][2], l->m[2][1], r->m[2][2], l->m[2][2]);
        out->m[0][0] = m00;
        out->m[1][0] = m10;
        out->m[2][0] = m20;
        out->m[0][1] = m01;
        out->m[1][1] = m11;
        out->m[2][1] = m21;
        out->m[0][2] = m02;
        out->m[1][2] = m12;
        out->m[2][2] = m22;
    } else {
        out->m[0][0] = fn_80144514(r->m[0][0], l->m[0][0], r->m[1][0], l->m[0][1], r->m[2][0], l->m[0][2]);
        out->m[1][0] = fn_80144514(r->m[0][0], l->m[1][0], r->m[1][0], l->m[1][1], r->m[2][0], l->m[1][2]);
        out->m[2][0] = fn_80144514(r->m[0][0], l->m[2][0], r->m[1][0], l->m[2][1], r->m[2][0], l->m[2][2]);
        out->m[0][1] = fn_80144514(r->m[0][1], l->m[0][0], r->m[1][1], l->m[0][1], r->m[2][1], l->m[0][2]);
        out->m[1][1] = fn_80144514(r->m[0][1], l->m[1][0], r->m[1][1], l->m[1][1], r->m[2][1], l->m[1][2]);
        out->m[2][1] = fn_80144514(r->m[0][1], l->m[2][0], r->m[1][1], l->m[2][1], r->m[2][1], l->m[2][2]);
        out->m[0][2] = fn_80144514(r->m[0][2], l->m[0][0], r->m[1][2], l->m[0][1], r->m[2][2], l->m[0][2]);
        out->m[1][2] = fn_80144514(r->m[0][2], l->m[1][0], r->m[1][2], l->m[1][1], r->m[2][2], l->m[1][2]);
        out->m[2][2] = fn_80144514(r->m[0][2], l->m[2][0], r->m[1][2], l->m[2][1], r->m[2][2], l->m[2][2]);
    }
}

void RotList::Draw(void* ctx) {
    int i = 0;
    if (count > 0) {
    for (; i < count; i++) {
        Rot* r = &rots[i];
        TMtx m;
        f32 c;
        f32 s;
        f32 a = r->speed * fn_801E7E18();
        c = cos(a);
        s = sin(a);
        m.r.m[0][0] = c;
        m.r.m[0][1] = 0.0f;
        m.r.m[0][2] = -s;
        m.r.m[1][0] = 0.0f;
        m.r.m[1][1] = 1.0f;
        m.r.m[1][2] = 0.0f;
        m.r.m[2][0] = s;
        m.r.m[2][1] = 0.0f;
        m.r.m[2][2] = c;
        Mat3MulD(&m.r, &m.r, r->rot);
        m.t[0] = r->pos[0];
        m.t[1] = r->pos[1];
        m.t[2] = r->pos[2];
        fn_802CDAB4(ctx, r->id, &m);
    }
    }
}
void XFadeList::Draw(void* ctx) {}

void XFadeList::Draw2(void* ctx) {
    XFade* vis[48];
    int n = 0;
    int i;
    for (i = 0; i < count; i++) {
        if (fn_802CDB20(ctx, fades[i].id)) {
            vis[n++] = &fades[i];
        }
    }
    for (i = 0; i < n; i++) {
        XFade* f = vis[i];
        f32 d = fn_802C88D4(f);
        if (d > f->start) {
            fn_802C9900(Clamp01(f->scale * (d - f->start)));
            fn_802C7040(f->model);
            fn_802C9904();
        }
    }
}

int XFadeList::Kind(void) { return 4; }

int ScrollList::Kind(void) { return 3; }

void ScrollList::Draw(void* ctx) {}

void ScrollList::Draw2(void* ctx) {
    f32 t = fn_801E7E18();
    f32 m[3][4];
    int i;
    m[0][1] = 0.0f;
    m[0][0] = 1.0f;
    m[0][2] = 0.0f;
    m[0][3] = 0.0f;
    m[1][0] = 0.0f;
    m[1][1] = 1.0f;
    m[1][2] = 0.0f;
    m[1][3] = 0.0f;
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = 1.0f;
    m[2][3] = 0.0f;
    for (i = 0; i < count; i++) {
        Scroll* s = &scrolls[i];
        if (fn_802CDB20(ctx, s->id)) {
            f32 v = s->v * t;
            f32 u = s->u * t;
            f32 fv = fmod(v, 1.0);
            f32 fu = fmod(u, 1.0);
            fn_802C70AC(s->model, m, (s32)fn_802C7A3C(fu, fv, 100000.0f), -1);
        }
    }
}

void FacingTestInst::Draw(void* ctx) {
    if (fn_802CDB20(ctx, id) && !fn_801F1CF0(data)) {
        fn_802CDAFC(ctx, id);
    }
}

int TrackInst::Kind(void) { return -1; }

void TrackInst::Draw2(void* ctx) {}
