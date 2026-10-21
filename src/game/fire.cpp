// Fire effects (0x80297D5C-0x8029C1C4): engine fires, ground (ring) fires under burning cars,
// the big ring fires, boost flames from the rear wheels, exhaust backfire torches drawn as
// textured cross quads, and event-driven torch fires. Names are guesses apart from the
// allocation tags, value keys, file names and messages.
#include "game/common.h"
#include "macros.h"
#include <revolution/MTX.h>

struct Mat3 {
    f32 m[3][3];
};

// Car state as returned per car index.
struct FireCar {
    Vec axes[3];  // offset 0x0
    Vec pos;      // offset 0x24
    s16 hint;     // offset 0x30
    Vec vel;      // offset 0x34
    u8 pad40[0x50 - 0x40];
    f32 speed;  // offset 0x50
    u8 pad54[0x58 - 0x54];
    f32 boost;  // offset 0x58
    s32 state;  // offset 0x5C
    u8 pad60[0x70 - 0x60];
    struct {
        Vec pos;  // offset 0x0
        u8 padC[0x3C - 0xC];
    } wheels[4];  // offset 0x70
};

struct FireCarEntry {
    FireCar* car;  // offset 0x0
    s32 index;     // offset 0x4
    f32 dist;      // offset 0x8
    u8 active;     // offset 0xC
};

struct FireMtx {
    Vec rot[3];  // offset 0x0
    Vec pos;     // offset 0x24
};

struct TorchPart {
    Mat3 rot;  // offset 0x0
    Vec pos;   // offset 0x24
};

struct FireModel {
    u8 pad0[0x24];
    s32 nParts;  // offset 0x24
};

struct FireOpts {
    s32 unk0;
    u32 mode;  // offset 0x4
};

struct FColor {
    FColor(void) {}
    FColor(f32 a, f32 b, f32 c) {
        r = a;
        g = b;
        b_ = c;
    }
    f32 r;
    f32 g;
    f32 b_;
};

struct FVec {
    FVec(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
    f32 x;
    f32 y;
    f32 z;
};

// A vector passed by value (the default of the model vector lookup).
struct V3 {
    V3(f32 x_, f32 y_, f32 z_) {
        x = x_;
        y = y_;
        z = z_;
    }
    f32 x;
    f32 y;
    f32 z;
};

struct FireDesc {
    s32 kind;       // offset 0x0
    s32 max;        // offset 0x4
    f32 size0;      // offset 0x8
    f32 size1;      // offset 0xC
    FColor c0;      // offset 0x10
    FColor c1;      // offset 0x1C
    f32 gravity;    // offset 0x28
    f32 unk2C;      // offset 0x2C
    f32 life;       // offset 0x30
    f32 drag;       // offset 0x34
    u8 pad38[0x44 - 0x38];
    s32 frames;     // offset 0x44
    f32 shading;    // offset 0x48
    u8 pad4C[0x50 - 0x4C];
    f32 grow;       // offset 0x50
    f32 fade;       // offset 0x54
    f32 spinMin;    // offset 0x58
    f32 spinMax;    // offset 0x5C
    f32 a60;        // offset 0x60
    f32 a64;        // offset 0x64
    f32 a68;        // offset 0x68
    f32 a6C;        // offset 0x6C
    f32 a70;        // offset 0x70
    f32 a74;        // offset 0x74
    u8 pad78[0x80 - 0x78];
    f32 a80;        // offset 0x80
    s32 i84;        // offset 0x84
    u8 b88;         // offset 0x88
    u8 b89;         // offset 0x89
};

class FXSequence {
public:
    static FXSequence* Load(const char* name);
    void Free(void);
    int Play(f32 t, const ValVec3* pos);
};

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
};

extern "C" {
f32 fn_80129DC4(int seed, f32 lo, f32 hi);  // random float
f32 fn_80144514(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz);
void fn_801BFD34(Vec* out, const Vec* v, const void* m);
f32 fn_801E7E18(void);
f32 fn_801E7E24(void);
FireOpts* fn_801E87A8(void);
void fn_801E9DD4(const char* a, const char* b);
void fn_801E9DD8(void);
int fn_801E9E30(s16* h, Vec* pos);
s32 fn_801EE0C8(void* model);
void fn_801EEFDC(int (*h)(const void* ev), u8 type);
void fn_801EF024(int (*h)(const void* ev));
s32 fn_80287B14(void);
const FireCar* fn_80287B24(s32 i);
void fn_80287B38(void (*cb)(FireCarEntry* e, int n));
int fn_80287B5C(int n);
f32 fn_80287BC8(f32 lo, f32 hi);
int fn_80287E20(FireModel* model, const char* name);
ValueNodeV* fn_80287E80(FireModel* model, const char* name);
ValVec3 fn_80287ED0(FireModel* model, const char* node, const char* key);
ValVec3 fn_80287F90(FireModel* model, const char* node, const char* key, V3 def);
const TorchPart* fn_80288060(FireModel* model, int i);
FireModel* fn_80288208(s32 i);
void fn_80288A40(const Vec* p, const Vec* v);
void fn_80288A58(const Vec* p, const Vec* v);
void* fn_8028E2F8(FireDesc* desc, const char* name, const char* tex);
void fn_8028E398(void* e);
void fn_8028E584(void* e, const Vec* p, const Vec* v);
void fn_8028E61C(FireDesc* desc);
void fn_802C77F0(s32 model, f32* box);
const FireMtx* fn_802C86DC(void);
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
int fn_802C8DBC(int n);
void fn_802C8E00(u32 color, int idx, f32 x, f32 y, f32 z, f32 u, f32 v);
void fn_802C8E48(void);
f32 fn_802C9A4C(f32 lod);
s32 fn_802CAC28(const char* name, int flags);
void fn_802CAC54(s32 tex);
void fn_802E317C(const char* fmt, ...);
double sqrt(double x);
double cos(double x);
double sin(double x);
void* memset(void* dst, int c, size_t n);
}

void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);
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

static inline void AddScaled(Vec* out, const Vec* in, const Vec* d, f32 s) {
    out->x = in->x + d->x * s;
    out->y = in->y + d->y * s;
    out->z = in->z + d->z * s;
}

static inline void Scale(Vec* out, const Vec* in, f32 s) {
    out->x = in->x * s;
    out->y = in->y * s;
    out->z = in->z * s;
}

static inline f32 Clamp01(f32 x) {
    x = 1.0f < x ? 1.0f : x;
    return x > 0.0f ? x : 0.0f;
}

// out = b * a, through a temporary when out aliases an input.
static inline void MulMat(Mat3* out, const Mat3* a, const Mat3* b) {
    if (out == b || out == a) {
        f32 m00 = fn_80144514(a->m[0][0], b->m[0][0], a->m[1][0], b->m[0][1], a->m[2][0], b->m[0][2]);
        f32 m10 = fn_80144514(a->m[0][0], b->m[1][0], a->m[1][0], b->m[1][1], a->m[2][0], b->m[1][2]);
        f32 m20 = fn_80144514(a->m[0][0], b->m[2][0], a->m[1][0], b->m[2][1], a->m[2][0], b->m[2][2]);
        f32 m01 = fn_80144514(a->m[0][1], b->m[0][0], a->m[1][1], b->m[0][1], a->m[2][1], b->m[0][2]);
        f32 m11 = fn_80144514(a->m[0][1], b->m[1][0], a->m[1][1], b->m[1][1], a->m[2][1], b->m[1][2]);
        f32 m21 = fn_80144514(a->m[0][1], b->m[2][0], a->m[1][1], b->m[2][1], a->m[2][1], b->m[2][2]);
        f32 m02 = fn_80144514(a->m[0][2], b->m[0][0], a->m[1][2], b->m[0][1], a->m[2][2], b->m[0][2]);
        f32 m12 = fn_80144514(a->m[0][2], b->m[1][0], a->m[1][2], b->m[1][1], a->m[2][2], b->m[1][2]);
        f32 m22 = fn_80144514(a->m[0][2], b->m[2][0], a->m[1][2], b->m[2][1], a->m[2][2], b->m[2][2]);
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
        out->m[0][0] = fn_80144514(a->m[0][0], b->m[0][0], a->m[1][0], b->m[0][1], a->m[2][0], b->m[0][2]);
        out->m[1][0] = fn_80144514(a->m[0][0], b->m[1][0], a->m[1][0], b->m[1][1], a->m[2][0], b->m[1][2]);
        out->m[2][0] = fn_80144514(a->m[0][0], b->m[2][0], a->m[1][0], b->m[2][1], a->m[2][0], b->m[2][2]);
        out->m[0][1] = fn_80144514(a->m[0][1], b->m[0][0], a->m[1][1], b->m[0][1], a->m[2][1], b->m[0][2]);
        out->m[1][1] = fn_80144514(a->m[0][1], b->m[1][0], a->m[1][1], b->m[1][1], a->m[2][1], b->m[1][2]);
        out->m[2][1] = fn_80144514(a->m[0][1], b->m[2][0], a->m[1][1], b->m[2][1], a->m[2][1], b->m[2][2]);
        out->m[0][2] = fn_80144514(a->m[0][2], b->m[0][0], a->m[1][2], b->m[0][1], a->m[2][2], b->m[0][2]);
        out->m[1][2] = fn_80144514(a->m[0][2], b->m[1][0], a->m[1][2], b->m[1][1], a->m[2][2], b->m[1][2]);
        out->m[2][2] = fn_80144514(a->m[0][2], b->m[2][0], a->m[1][2], b->m[2][1], a->m[2][2], b->m[2][2]);
    }
}

// Ground fire patches: a parallelogram of flames snapped to the ground.
struct FireRing {
    Vec p0;     // offset 0x0
    Vec e1;     // offset 0xC
    Vec e2;     // offset 0x18
    f32 life;   // offset 0x24
    u8 active;  // offset 0x28
};

class RingFireMan {
public:
    ~RingFireMan(void);
    void Reset(void);
    void Update(void);
    int Add(const Vec* p, const Vec* e1, const Vec* e2, s16 hint, f32 life);
    void Remove(int i);
    void Move(int i, const Vec* p, const Vec* e1, const Vec* e2, s16 hint);
    int Snap(FireRing* r, const Vec* pos, const Vec* d1, const Vec* d2, s16* hint);
    void Tick(FireRing* r, f32 dt);
    FireRing* FindFree(void);

    FireRing rings[4];  // offset 0x0
    void* emitter;      // offset 0xB0
};

// The scorch decals left by the ground fires.
struct RingSlot {
    s32 id;      // offset 0x0
    Vec pts[4];  // offset 0x4
    s32 state;   // offset 0x34
    f32 time;    // offset 0x38
    f32 alpha;   // offset 0x3C
};

class RingSlots {
public:
    ~RingSlots(void);
    void Add(int id, const Vec* p, const Vec* e1, const Vec* e2, s16 hint);
    void Remove(int id);
    void Reset(void);
    void Update(void);
    void Draw(void);
    int FindSlot(int* out);
    RingSlot* Find(int id);
    void DrawSlot(int i);

    RingSlot slots[4];  // offset 0x0
    s32 tex;            // offset 0x100
};

struct EngineEntry {
    EngineEntry(void);
    ValVec3 pos;     // offset 0x0
    ValVec3 spread;  // offset 0xC
    u8 has;          // offset 0x18
    u8 on;           // offset 0x19
    f32 t;           // offset 0x1C
};

class EngineFireMan {
public:
    EngineFireMan(void);
    ~EngineFireMan(void);
    void Reset(void);
    void Update(void);
    void Draw(void);
    void Set(int i, u8 on);
    void Emit(EngineEntry* e);

    EngineEntry* cars;  // offset 0x0
    void* emitter;      // offset 0x4
    FXSequence* seq;    // offset 0x8
};

struct BigRing {
    void* emitter;  // offset 0x0
    Vec pts[12];    // offset 0x4
    s32 count;      // offset 0x94
    f32 t;          // offset 0x98
};

class BigRingMan {
public:
    ~BigRingMan(void);
    void Reset(void);
    void Update(void);
    void Spawn(const FireCar* m, f32 radius);
    void Make(BigRing* r, const FireCar* m, f32 radius);
    void Free(BigRing* r);
    void Tick(BigRing* r, f32 now);

    BigRing rings[2];  // offset 0x0
    s32 count;         // offset 0x138
};

class GroundFire {
public:
    void Reset(void);
    void Update(void);
    void Toggle(int idx, int on);
    int Moved(const FireCar* c);
    void Corners(Vec* p, Vec* e1, Vec* e2);
    void Start(int idx);
    void Refresh(const FireCar* c);

    FireMtx m;  // offset 0x0
    Vec off;    // offset 0x30
    s32 car;    // offset 0x3C
    s32 ring;   // offset 0x40
};

struct BoostEntry {
    Vec pts[2];  // offset 0x0
    u8 pad18[0x24 - 0x18];
    f32 t;      // offset 0x24
    u8 active;  // offset 0x28
};

class BoostFireMan {
public:
    BoostFireMan(void);
    ~BoostFireMan(void);
    void SetT(int i);
    void SetActive(int i);
    void Reset(void);
    void Update(int i, const FireCar* car, f32 dt, f32 dist);
    void Calc(int i, const FireCar* car);

    BoostEntry entries[8];  // offset 0x0
    void* emitter;          // offset 0x160
};

struct TorchEntry {
    Vec rot[3];  // offset 0x0
    Vec pos;     // offset 0x24
    f32 len;     // offset 0x30
    u8 frame;    // offset 0x34
    u8 on;       // offset 0x35
};

class ExhaustTorchMan {
public:
    ExhaustTorchMan(void);
    ~ExhaustTorchMan(void);
    void Reset(void);
    void Draw(void);
    void Set(int i, const TorchPart* part);
    void CellUV(int idx, f32* u0, f32* u1, f32* v0, f32* v1);
    void Load(void);
    void DrawEdge(const Vec* p, int frame, const Vec* a, const Vec* b, f32 len);
    void DrawFace(const TorchEntry* m, int frame, f32 len);
    void DrawEdges(void);
    void DrawFaces(void);

    TorchEntry entries[8];  // offset 0x0
    f32 sizes[8];           // offset 0x1C0
    s32 count;              // offset 0x1E0
    s32 faceTex;            // offset 0x1E4
    s32 edgeTex;            // offset 0x1E8
    s32 grid;               // offset 0x1EC
    s32 shift;              // offset 0x1F0
    s32 unk1F4;             // offset 0x1F4
    f32 cell;               // offset 0x1F8
};

struct BackfireEntry {
    u32 state;             // offset 0x0
    f32 a;                 // offset 0x4
    f32 b;                 // offset 0x8
    s32 c;                 // offset 0xC
    const TorchPart* d;    // offset 0x10
};

struct BackfireEvt {
    s32 car;   // offset 0x0
    f32 x;     // offset 0x4
    s32 unk8;  // offset 0x8
    s32 mode;  // offset 0xC
};

class BackfireMan {
public:
    BackfireMan(void);
    ~BackfireMan(void);
    void Reset(void);
    void Draw(void);
    void Event(const BackfireEvt* ev);
    void Tick(int i, int on);

    ExhaustTorchMan* torch;     // offset 0x0
    BackfireEntry entries[8];   // offset 0x4
};

struct TorchFire {
    ValVec3 pos;  // offset 0x0
    u32 key;      // offset 0xC
    u8 on;        // offset 0x10
};

class TorchFireMan {
public:
    TorchFireMan(void);
    ~TorchFireMan(void);
    void Reset(void);
    void Update(void);
    void Add(const TorchFire* ev);
    void InitEmitter(void);

    void* emitter;          // offset 0x0
    TorchFire entries[8];   // offset 0x4
    s32 count;              // offset 0xA4
};

ModuleTag lbl_8055F7B8;
RingFireMan* lbl_8055F7C0;
RingSlots* lbl_8055F7C4;
EngineFireMan* lbl_8055F7C8;
BigRingMan* lbl_8055F7CC;
GroundFire* lbl_8055F7D0;
BoostFireMan* lbl_8055F7D4;
BackfireMan* lbl_8055F7D8;
TorchFireMan* lbl_8055F7DC;
void* lbl_8055F7E0;  // torch emitter

const char* lbl_8055CA70 = "Andrew";
const char* lbl_8055CA7C = "fire";
const char* lbl_8055CA80 = "firefull.tex";
const char* lbl_8055CA84 = "bfface.tex";
const char* lbl_8055CA88 = "bfedge.tex";
const char* lbl_8055CA8A = "firebase.tex";
const char* lbl_8055CA8C = "fireboost.tex";

static const int lbl_802FA9F0[8] = {0, 7, 3, 2, 5, 4, 6, 1};

extern "C" {
int fn_8029BFF8(const void* ev);
int fn_8029C024(const void* ev);
int fn_8029C054(const void* ev);
void fn_8029C084(FireCarEntry* e, int n);
int fn_8029C190(const void* ev);

int fn_80297D5C(s32* out, int n) {
    for (int i = 1; i < 16; i++) {
        if (n == i * i) {
            *out = i;
            return 1;
        }
    }
    return 0;
}

int fn_80297D90(s32* out, int n) {
    for (int i = 0; i < 10; i++) {
        if (1 << i == n) {
            *out = i;
            return 1;
        }
    }
    return 0;
}

void fn_80297DC8(FireDesc* e) {
    e->max = 0xC0;
    e->size0 = 1.0f;
    e->size1 = 0.25f;
    e->kind = 1;
    e->c0 = FColor(0.5f, 0.25f, 0.5f);
    e->c1 = FColor(1.0f, 1.0f, 1.0f);
    e->gravity = 0.0f;
    e->life = -0.05f;
    e->grow = 1.0f;
    e->fade = 1.25f;
    e->spinMin = -3.0f;
    e->spinMax = 3.0f;
    e->a70 = -0.4f;
    e->a68 = -0.4f;
    e->a60 = -0.4f;
    e->a74 = 0.6f;
    e->a6C = 0.6f;
    e->a64 = 0.6f;
    e->a80 = 1.0f;
    e->i84 = 0x40;
    e->b88 = 1;
    e->frames = 2;
    e->b89 = 1;
}

void fn_80297E90(void) {
    FireDesc e;
    fn_8028E61C(&e);
    fn_80297DC8(&e);
    lbl_8055F7E0 = fn_8028E2F8(&e, "torch fire", lbl_8055CA80);
}

void fn_80297ED4(void) {}

void fn_80297ED8(void) {
    lbl_8055F7C8 = new ("EngineFireMan") EngineFireMan;
    lbl_8055F7D8 = new ("BackfireMan") BackfireMan;
    lbl_8055F7DC = new ("TorchFireMan") TorchFireMan;
    lbl_8055F7D4 = new ("BoostFireMan") BoostFireMan;
    if (fn_801E87A8()->mode == 0x180201) {
        fn_80297E90();
    } else {
        lbl_8055F7E0 = 0;
    }
    fn_80287B38(fn_8029C084);
}

void fn_80297FA4(void) {
    if (lbl_8055F7E0) {
        fn_8028E398(lbl_8055F7E0);
        lbl_8055F7E0 = 0;
    }
    if (lbl_8055F7D4) {
        delete lbl_8055F7D4;
        lbl_8055F7D4 = 0;
    }
    if (lbl_8055F7D0) {
        delete lbl_8055F7D0;
        lbl_8055F7D0 = 0;
    }
    if (lbl_8055F7CC) {
        delete lbl_8055F7CC;
        lbl_8055F7CC = 0;
    }
    if (lbl_8055F7C4) {
        delete lbl_8055F7C4;
        lbl_8055F7C4 = 0;
    }
    if (lbl_8055F7C0) {
        delete lbl_8055F7C0;
        lbl_8055F7C0 = 0;
    }
    delete lbl_8055F7DC;
    lbl_8055F7DC = 0;
    delete lbl_8055F7D8;
    lbl_8055F7D8 = 0;
    delete lbl_8055F7C8;
    lbl_8055F7C8 = 0;
}

void fn_802980A0(void) {
    lbl_8055F7C8->Reset();
    lbl_8055F7D8->Reset();
    lbl_8055F7DC->Reset();
    if (lbl_8055F7C4) {
        lbl_8055F7C4->Reset();
    }
    if (lbl_8055F7C0) {
        lbl_8055F7C0->Reset();
    }
    if (lbl_8055F7CC) {
        lbl_8055F7CC->Reset();
    }
    if (lbl_8055F7D0) {
        lbl_8055F7D0->Reset();
    }
    if (lbl_8055F7D4) {
        lbl_8055F7D4->Reset();
    }
}

void fn_80298124(void) {
    lbl_8055F7C8->Draw();
    lbl_8055F7D8->Draw();
    if (lbl_8055F7C4) {
        lbl_8055F7C4->Draw();
    }
}

void fn_80298160(void) {
    if ((f32)__fabs(fn_801E7E24()) <= lbl_80558DC4[0]) {
        return;
    }
    lbl_8055F7C8->Update();
    lbl_8055F7DC->Update();
    if (lbl_8055F7C4) {
        lbl_8055F7C4->Update();
    }
    if (lbl_8055F7C0) {
        lbl_8055F7C0->Update();
    }
    if (lbl_8055F7CC) {
        lbl_8055F7CC->Update();
    }
    if (lbl_8055F7D0) {
        lbl_8055F7D0->Update();
    }
}

void fn_802981EC(const FireCar* m, f32 radius) { lbl_8055F7CC->Spawn(m, radius); }

void fn_802981F8(const Vec* p) {
    Vec v;
    v.x = 0.0f;
    v.y = 1.0f;
    v.z = 0.0f;
    fn_8028E584(lbl_8055F7E0, p, &v);
    fn_80288A40(p, &v);
}

// Ground fire manager setup; dead-stripped from the game but its name remains.
void fn_ground_dead(RingFireMan* m) {
    FireDesc e;
    fn_8028E61C(&e);
    m->emitter = fn_8028E2F8(&e, "ground fire", lbl_8055CA80);
}

}  // extern "C"

RingFireMan::~RingFireMan(void) { fn_8028E398(emitter); }

void RingFireMan::Reset(void) {
    for (int i = 0; i < 4; i++) {
        rings[i].active = 0;
    }
}

void RingFireMan::Update(void) {
    f32 dt = fn_801E7E24();
    for (int i = 0; i < 4; i++) {
        if (rings[i].active) {
            Tick(&rings[i], dt);
        }
    }
}

int RingFireMan::Add(const Vec* p, const Vec* e1, const Vec* e2, s16 hint, f32 life) {
    FireRing* r = FindFree();
    if (r == 0 || !Snap(r, p, e1, e2, &hint)) {
        return -1;
    }
    r->life = life;
    r->active = 1;
    int idx = r - rings;
    lbl_8055F7C4->Add(idx, &r->p0, &r->e1, &r->e2, hint);
    return idx;
}

void RingFireMan::Remove(int i) {
    lbl_8055F7C4->Remove(i);
    rings[i].active = 0;
}

void RingFireMan::Move(int i, const Vec* p, const Vec* e1, const Vec* e2, s16 hint) {
    FireRing* r = &rings[i];
    if (Snap(r, p, e1, e2, &hint)) {
        lbl_8055F7C4->Remove(i);
        lbl_8055F7C4->Add(i, &r->p0, &r->e1, &r->e2, hint);
    }
}

int RingFireMan::Snap(FireRing* r, const Vec* pos, const Vec* d1, const Vec* d2, s16* hint) {
    ValVec3 p = *(const ValVec3*)pos;
    Vec a;
    Vec b;
    Vec e1;
    Vec e2;
    a.x = p.x + d1->x;
    a.y = p.y + d1->y;
    a.z = p.z + d1->z;
    b.x = p.x + d2->x;
    b.y = p.y + d2->y;
    b.z = p.z + d2->z;
    fn_801E9DD4(lbl_8055CA70, lbl_8055CA7C);
    if (!fn_801E9E30(hint, (Vec*)&p) || !fn_801E9E30(hint, &a) || !fn_801E9E30(hint, &b)) {
        fn_801E9DD8();
        return 0;
    }
    fn_801E9DD8();
    e1.x = a.x - p.x;
    e1.y = a.y - p.y;
    e1.z = a.z - p.z;
    e2.x = b.x - p.x;
    e2.y = b.y - p.y;
    e2.z = b.z - p.z;
    if ((f32)__fabs(PSVECDotProduct(&e1, &e1)) <= lbl_80558DC4[0] ||
        (f32)__fabs(PSVECDotProduct(&e2, &e2)) <= lbl_80558DC4[0]) {
        return 0;
    }
    r->p0.x = p.x;
    r->p0.y = p.y;
    r->p0.z = p.z;
    f32 l2 = PSVECDotProduct(&e2, &e2);
    if (PSVECDotProduct(&e1, &e1) > l2) {
        r->e1 = e1;
        r->e2 = e2;
    } else {
        r->e1 = e2;
        r->e2 = e1;
    }
    return 1;
}

void RingFireMan::Tick(FireRing* r, f32 dt) {
    if (dt > r->life) {
        Remove(r - rings);
        return;
    }
    r->life = r->life - dt;
    f32 a = fn_80129DC4(0, -1.0f, 1.0f);
    f32 s = sqrt(1.0f - a * a);
    f32 b = fn_80129DC4(0, -s, s);
    Vec v;
    Vec p;
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = 0.0f;
    AddScaled(&p, &r->p0, &r->e1, a);
    AddScaled(&p, &p, &r->e2, b);
    p.y = p.y + 0.125f;
    fn_8028E584(emitter, &p, &v);
    fn_80288A40(&p, &v);
}

FireRing* RingFireMan::FindFree(void) {
    for (int i = 0; i < 4; i++) {
        if (!rings[i].active) {
            return &rings[i];
        }
    }
    return 0;
}

extern "C" void fn_802991BC(EngineFireMan* m);

EngineFireMan::EngineFireMan(void) {
    int n = fn_80287B14();
    cars = new ("EngineFireMan tab") EngineEntry[n];
    for (int i = 0; i < n; i++) {
        FireModel* model = fn_80288208(i);
        if (fn_80287E20(model, "engine")) {
            cars[i].pos = fn_80287ED0(model, "engine", "arm");
            cars[i].spread = fn_80287F90(model, "engine", "spread", V3(0.0f, 0.0f, 0.0f));
            cars[i].has = 1;
        } else {
            cars[i].has = 0;
        }
        cars[i].on = 0;
        cars[i].t = -1.0f;
    }
    fn_802991BC(this);
    seq = FXSequence::Load("expseq.val");
}

EngineEntry::EngineEntry(void) {}

EngineFireMan::~EngineFireMan(void) {
    if (seq) {
        seq->Free();
        seq = 0;
    }
    fn_8028E398(emitter);
    delete[] cars;
    cars = 0;
}

void EngineFireMan::Reset(void) {
    for (int i = 0; i < fn_80287B14(); i++) {
        cars[i].on = 0;
    }
}

void EngineFireMan::Update(void) {
    for (int i = 0; i < fn_80287B14(); i++) {
        if (cars[i].on) {
            Emit(&cars[i]);
        }
    }
}

void EngineFireMan::Draw(void) {
    f32 now = fn_801E7E18();
    Vec p;
    for (int i = 0; i < fn_80287B14(); i++) {
        if (cars[i].t > 0.0f) {
            const FireCar* c = fn_80287B24(i);
            MulVec(&p, (const Vec*)&cars[i].pos, c->axes);
            p.x = p.x + c->pos.x;
            p.y = p.y + c->pos.y;
            p.z = p.z + c->pos.z;
            if (!seq->Play(now - cars[i].t, (const ValVec3*)&p)) {
                cars[i].t = -1.0f;
            }
        }
    }
}

void EngineFireMan::Set(int i, u8 on) {
    if (on != cars[i].on) {
        if (on) {
            cars[i].t = fn_801E7E18();
            const FireCar* c = fn_80287B24(i);
            Vec p;
            MulVec(&p, (const Vec*)&cars[i].pos, c->axes);
            p.x = p.x + c->pos.x;
            p.y = p.y + c->pos.y;
            p.z = p.z + c->pos.z;
        }
        cars[i].on = on;
    }
}

void EngineFireMan::Emit(EngineEntry* e) {
    const FireCar* c = fn_80287B24(e - cars);
    Vec p;
    MulVec(&p, (const Vec*)&e->pos, c->axes);
    p.x = p.x + c->pos.x;
    p.y = p.y + c->pos.y;
    p.z = p.z + c->pos.z;
    p.x = p.x + fn_80129DC4(0, -e->spread.x, e->spread.x);
    p.y = p.y + fn_80129DC4(0, -e->spread.y, e->spread.y);
    p.z = p.z + fn_80129DC4(0, -e->spread.z, e->spread.z);
    fn_8028E584(emitter, &p, &c->vel);
}

extern "C" void fn_802991BC(EngineFireMan* m) {
    FireDesc e;
    fn_8028E61C(&e);
    e.max = 0x80;
    e.size0 = 0.4f;
    e.size1 = 0.5f;
    e.kind = 1;
    e.frames = 2;
    e.c0 = FColor(0.4f, 0.4f, 0.4f);
    e.c1 = FColor(1.0f, 1.0f, 1.0f);
    e.gravity = 0.3f;
    e.life = -0.1f;
    e.drag = 0.0f;
    e.grow = 0.25f;
    e.fade = 0.5f;
    e.spinMin = -1.0f;
    e.spinMax = 1.0f;
    e.a70 = -0.4f;
    e.a68 = -0.4f;
    e.a60 = -0.4f;
    e.a74 = 0.85f;
    e.a6C = 0.85f;
    e.a64 = 0.85f;
    e.a80 = 0.5f;
    e.i84 = 0x40;
    e.b88 = 1;
    e.b89 = 1;
    m->emitter = fn_8028E2F8(&e, "engine fire", lbl_8055CA80);
}

RingSlots::~RingSlots(void) {
    for (int i = 0; i < 4; i++) {
        slots[i].state = 0;
    }
    fn_802CAC54(tex);
    tex = -1;
}

void RingSlots::Add(int id, const Vec* p, const Vec* e1, const Vec* e2, s16 hint) {
    int idx;
    if (FindSlot(&idx)) {
        RingSlot* s = &slots[idx];
        s->id = id;
        s->state = 1;
        s->time = fn_801E7E18();
        s->alpha = 0.0f;
        Vec c[4];
        c[0].x = p->x + e1->x + e2->x;
        c[0].y = p->y + e1->y + e2->y;
        c[0].z = p->z + e1->z + e2->z;
        AddScaled(&c[1], &c[0], e1, -2.0f);
        AddScaled(&c[2], &c[0], e2, -2.0f);
        AddScaled(&c[3], &c[2], e1, -2.0f);
        fn_801E9DD4(lbl_8055CA70, lbl_8055CA7C);
        for (int i = 0; i < 4; i++) {
            fn_801E9E30(&hint, &c[i]);
            c[i].y = c[i].y + 0.03f;
            s->pts[i] = c[i];
        }
        fn_801E9DD8();
    }
}

void RingSlots::Remove(int id) {
    RingSlot* s = Find(id);
    s->id = -1;
    s->state = 2;
    s->time = fn_801E7E18();
}

void RingSlots::Reset(void) {
    for (int i = 0; i < 4; i++) {
        slots[i].id = -1;
        slots[i].state = 0;
        slots[i].time = 0.0f;
    }
}

void RingSlots::Update(void) {
    f32 dt = fn_801E7E24();
    for (int i = 0; i < 4; i++) {
        RingSlot* s = &slots[i];
        if (s->state == 1) {
            s->alpha = Clamp01(s->alpha + dt * 0.25f);
        } else if (s->state == 2) {
            s->alpha = Clamp01(s->alpha + -dt * 0.25f);
            if ((int)(255.0f * s->alpha) < 1) {
                s->state = 0;
            }
        }
    }
}

void RingSlots::Draw(void) {
    fn_802C8BD0(tex);
    if (fn_802C8DBC(20)) {
        for (int i = 0; i < 4; i++) {
            if (slots[i].state) {
                DrawSlot(i);
            }
        }
        fn_802C8E48();
    }
    fn_802C8D74();
}

int RingSlots::FindSlot(int* out) {
    int best = -1;
    int found = 0;
    for (int i = 0; i < 4; i++) {
        if (slots[i].state == 0) {
            *out = i;
            return 1;
        }
        if (slots[i].state == 2 && (!found || slots[i].time < slots[best].time)) {
            best = i;
            found = 1;
        }
    }
    if (found) {
        *out = best;
    }
    return found;
}

RingSlot* RingSlots::Find(int id) {
    for (int i = 0; i < 4; i++) {
        if (id == slots[i].id) {
            return &slots[i];
        }
    }
    return 0;
}

void RingSlots::DrawSlot(int i) {
    RingSlot* s = &slots[i];
    int a = (int)(255.0f * s->alpha);
    if (a >= 1) {
        u32 color = a << 24;
        for (int j = 0; j < 4; j++) {
            f32 u = (j & 1) ? 1.0f : 0.0f;
            f32 v = (j & 2) ? 0.0f : 1.0f;
            fn_802C8E00(color, j > 1, s->pts[j].x, s->pts[j].y, s->pts[j].z, u, v);
        }
    }
}

BigRingMan::~BigRingMan(void) {
    for (int i = 0; i < count; i++) {
        Free(&rings[i]);
    }
    count = 0;
}

void BigRingMan::Reset(void) {}

void BigRingMan::Update(void) {
    f32 now = fn_801E7E18();
    for (int i = 0; i < count; i++) {
        Tick(&rings[i], now);
    }
}

void BigRingMan::Spawn(const FireCar* m, f32 radius) {
    if (count >= 2) {
        fn_802E317C("fire: exceeded ring fire limit");
    }
    Make(&rings[count], m, radius);
    count++;
}

void BigRingMan::Make(BigRing* r, const FireCar* m, f32 radius) {
    FireDesc e;
    Mat3 rot;
    fn_8028E61C(&e);
    e.max = 0xA0;
    e.size0 = 1.0f;
    e.size1 = 1.0f;
    e.kind = 1;
    e.c0 = FColor(0.2f, 0.2f, 0.2f);
    e.c1 = FColor(1.0f, 1.0f, 1.0f);
    e.gravity = 0.0f;
    e.life = -0.05f;
    e.grow = 0.75f;
    e.fade = 0.35f;
    e.spinMin = -3.0f;
    e.spinMax = 3.0f;
    e.a70 = -0.4f;
    e.a68 = -0.4f;
    e.a60 = -0.4f;
    e.a74 = 0.6f;
    e.a6C = 0.6f;
    e.a64 = 0.6f;
    e.a80 = 1.0f;
    e.i84 = 0x40;
    e.b88 = 1;
    e.frames = 2;
    e.b89 = 1;
    r->emitter = fn_8028E2F8(&e, "ring fire", lbl_8055CA80);
    int n = MIN((int)(4.0f * radius), 12);
    n = MAX(n, 1);
    f32 c = cos(6.2831855f / n);
    f32 s = sin(6.2831855f / n);
    rot.m[0][0] = c;
    rot.m[0][1] = s;
    rot.m[0][2] = 0.0f;
    rot.m[1][0] = -s;
    rot.m[1][1] = c;
    rot.m[1][2] = 0.0f;
    rot.m[2][0] = 0.0f;
    rot.m[2][1] = 0.0f;
    rot.m[2][2] = 1.0f;
    Mat3 basis = *(const Mat3*)m->axes;
    for (int i = 0; i < n; i++) {
        r->pts[i].x = m->pos.x + basis.m[1][0] * radius;
        r->pts[i].y = m->pos.y + basis.m[1][1] * radius;
        r->pts[i].z = m->pos.z + basis.m[1][2] * radius;
        MulMat(&basis, &basis, &rot);
    }
    r->count = n;
    r->t = 0.0f;
}

void BigRingMan::Free(BigRing* r) {
    fn_8028E398(r->emitter);
    r->emitter = 0;
}

void BigRingMan::Tick(BigRing* r, f32 now) {
    if (r->t > 1.0f - lbl_80558DC4[0]) {
        Vec v;
        v.x = 0.0f;
        v.y = 0.0f;
        v.z = 0.0f;
        for (int i = 0; i < r->count; i++) {
            f32 f = fn_80129DC4(0, 0.0f, 1.0f);
            const Vec* a = &r->pts[i];
            const Vec* b = &r->pts[(i + 1) % r->count];
            Vec p;
            p.x = a->x + f * (b->x - a->x);
            p.y = a->y + f * (b->y - a->y);
            p.z = a->z + f * (b->z - a->z);
            fn_8028E584(r->emitter, &p, &v);
            fn_80288A58(&p, &v);
        }
        r->t = r->t + -0.75f;
    } else {
        r->t = r->t + 0.25f;
    }
}

void GroundFire::Reset(void) {
    m.rot[0].x = 1.0f;
    m.rot[0].y = 0.0f;
    m.rot[0].z = 0.0f;
    m.rot[1].x = 0.0f;
    m.rot[1].y = 1.0f;
    m.rot[1].z = 0.0f;
    m.rot[2].x = 0.0f;
    m.rot[2].y = 0.0f;
    m.rot[2].z = 1.0f;
    m.pos.x = 0.0f;
    m.pos.y = 0.0f;
    m.pos.z = 0.0f;
    *(FVec*)&off = FVec(0.0f, 0.0f, 0.0f);
    car = -1;
    ring = -1;
}

void GroundFire::Update(void) {
    if (ring != -1) {
        if (lbl_8055F7C0->rings[ring].active) {
            const FireCar* c = fn_80287B24(car);
            if (Moved(c)) {
                Refresh(c);
            }
        } else {
            ring = -1;
        }
    }
}

void GroundFire::Toggle(int idx, int on) {
    if (on && ring == -1) {
        Start(idx);
    } else if (!on && idx == car && ring != -1) {
        if (lbl_8055F7C0->rings[ring].active) {
            lbl_8055F7C0->Remove(ring);
        }
        ring = -1;
    }
}

int GroundFire::Moved(const FireCar* c) {
    f32 lim = 0.25f;
    int moved = 1;
    if (!(PSVECSquareDistance(&m.pos, &c->pos) > lim) && !(PSVECDotProduct(&m.rot[0], &c->axes[0]) < 0.9659258f)) {
        moved = 0;
    }
    return moved;
}

void GroundFire::Corners(Vec* p, Vec* e1, Vec* e2) {
    MulVec(p, &off, m.rot);
    p->x = p->x + m.pos.x;
    p->y = p->y + m.pos.y;
    p->z = p->z + m.pos.z;
    Scale(e1, &m.rot[0], 0.4f);
    Scale(e2, &m.rot[2], 0.75f);
}

void GroundFire::Start(int idx) {
    f32 box[6];
    const FireCar* c = fn_80287B24(idx);
    fn_802C77F0(fn_801EE0C8((u32)idx < 8 ? lbl_80488648[idx] : 0), box);
    m = *(const FireMtx*)c;
    *(FVec*)&off = FVec(box[0], box[2], box[4]);
    car = idx;
    Vec p;
    Vec e1;
    Vec e2;
    p.x = 0.0f;
    p.y = 0.0f;
    p.z = 0.0f;
    e1.x = 0.0f;
    e1.y = 0.0f;
    e1.z = 0.0f;
    e2.x = 0.0f;
    e2.y = 0.0f;
    e2.z = 0.0f;
    Corners(&p, &e1, &e2);
    ring = lbl_8055F7C0->Add(&p, &e1, &e2, c->hint, 20.0f);
}

void GroundFire::Refresh(const FireCar* c) {
    Vec p;
    Vec e1;
    Vec e2;
    p.x = 0.0f;
    p.y = 0.0f;
    p.z = 0.0f;
    e1.x = 0.0f;
    e1.y = 0.0f;
    e1.z = 0.0f;
    e2.x = 0.0f;
    e2.y = 0.0f;
    e2.z = 0.0f;
    m = *(const FireMtx*)c;
    Corners(&p, &e1, &e2);
    lbl_8055F7C0->Move(ring, &p, &e1, &e2, c->hint);
}

BoostFireMan::BoostFireMan(void) {
    Reset();
    FireDesc e;
    fn_8028E61C(&e);
    memset(&e, 0, sizeof(e));
    e.kind = 1;
    e.max = 500;
    e.size0 = 0.5f;
    e.size1 = 1.0f;
    e.c0 = FColor(0.0f, 0.0f, 0.0f);
    e.c1 = FColor(1.0f, 1.0f, 1.0f);
    e.life = -1.0f;
    e.drag = 0.1f;
    e.frames = 2;
    e.grow = 1.0f;
    e.fade = 1.0f;
    e.spinMin = -1.0f;
    e.spinMax = 1.0f;
    e.a70 = -1.0f;
    e.a68 = -1.0f;
    e.a60 = -1.0f;
    e.a74 = 1.0f;
    e.a6C = 1.0f;
    e.a64 = 1.0f;
    e.a80 = 2.0f;
    e.i84 = 0x40;
    e.b88 = 1;
    e.b89 = 1;
    emitter = fn_8028E2F8(&e, "boost fire", lbl_8055CA8C);
    fn_801EEFDC(fn_8029C054, 14);
    fn_801EEFDC(fn_8029C024, 13);
}

BoostFireMan::~BoostFireMan(void) {
    fn_801EF024(fn_8029C024);
    fn_801EF024(fn_8029C054);
    fn_8028E398(emitter);
}

void BoostFireMan::SetT(int i) { entries[i].t = 1.0f; }

void BoostFireMan::SetActive(int i) { entries[i].active = 1; }

void BoostFireMan::Reset(void) {
    for (int i = 0; i < 8; i++) {
        entries[i].t = -1.0f;
        entries[i].active = 0;
    }
}

void BoostFireMan::Update(int i, const FireCar* car, f32 dt, f32 dist) {
    BoostEntry* e = &entries[i];
    if (!((f32)__fabs(car->boost) <= lbl_80558DC4[0])) {
        e->active = 0;
        e->t = 0.0f;
    }
    if (e->active || e->t > 0.0f) {
        if (dist < 40000.0f) {
            Calc(i, car);
            const Vec* vel = &car->vel;
            int n = (int)(dt * car->speed);
            n = MAX(n, 1);
            n = MIN(n, 4);
            f32 step = dt / n;
            for (int k = 0; k < n; k++) {
                f32 f = -k * step;
                Vec off;
                off.x = vel->x * f;
                off.y = vel->y * f + -0.5f * (f * f);
                off.z = vel->z * f;
                for (u32 j = 0; j < 2; j++) {
                    Vec p;
                    p.x = off.x + e->pts[j].x;
                    p.y = off.y + e->pts[j].y;
                    p.z = off.z + e->pts[j].z;
                    fn_8028E584(emitter, &p, vel);
                }
            }
        }
        e->active = 0;
        e->t = e->t - dt;
    }
}

void BoostFireMan::Calc(int i, const FireCar* car) {
    BoostEntry* e = &entries[i];
    fn_80288208(i);
    MulVec(&e->pts[0], &car->wheels[2].pos, car->axes);
    e->pts[0].x = e->pts[0].x + car->pos.x;
    e->pts[0].y = e->pts[0].y + car->pos.y;
    e->pts[0].z = e->pts[0].z + car->pos.z;
    e->pts[0].y = e->pts[0].y + 0.5f;
    MulVec(&e->pts[1], &car->wheels[3].pos, car->axes);
    e->pts[1].x = e->pts[1].x + car->pos.x;
    e->pts[1].y = e->pts[1].y + car->pos.y;
    e->pts[1].z = e->pts[1].z + car->pos.z;
    e->pts[1].y = e->pts[1].y + 0.5f;
}

ExhaustTorchMan::ExhaustTorchMan(void) {
    Load();
    int n = fn_80287B14();
    for (int i = 0; i < n; i++) {
        FireModel* model = fn_80288208(i);
        sizes[i] = 1.0f;
        if (fn_80287E20(model, "engine")) {
            sizes[i] = fn_80287E80(model, "engine")->GetFloat("exhaust_torch_size", sizes[i]);
        }
    }
    Reset();
}

ExhaustTorchMan::~ExhaustTorchMan(void) {
    fn_802CAC54(faceTex);
    fn_802CAC54(edgeTex);
}

void ExhaustTorchMan::Reset(void) {
    memset(this, 0, sizeof(entries));
    count = 0;
}

void ExhaustTorchMan::Draw(void) {
    if (count >= 1) {
        int total = 0;
        for (int i = 0; i < fn_80287B14(); i++) {
            if (entries[i].on) {
                total += fn_80288208(i)->nParts;
            }
        }
        f32 old = fn_802C9A4C(-grid / 0.325f);
        fn_802C8BD0(edgeTex);
        if (fn_802C8DBC(total * 4)) {
            DrawEdges();
            fn_802C8E48();
        }
        fn_802C8D74();
        fn_802C8BD0(faceTex);
        if (fn_802C8DBC(total * 8)) {
            DrawFaces();
            fn_802C8E48();
        }
        fn_802C8D74();
        fn_802C9A4C(old);
    }
}

void ExhaustTorchMan::Set(int i, const TorchPart* part) {
    TorchEntry* e = &entries[i];
    if (part) {
        const FireCar* c = fn_80287B24(i);
        fn_801BFD34(&e->pos, &part->pos, c);
        Mat3Mul((Mat3*)e->rot, &part->rot, (const Mat3*)c);
        e->len = 0.1625f * sizes[i] * (1.0f + fn_80287BC8(-0.35f, 0.35f));
        e->frame = fn_80287B5C(16);
        if (!e->on) {
            e->on = 1;
            count++;
        }
    } else if (e->on) {
        e->on = 0;
        count--;
    }
}

void ExhaustTorchMan::CellUV(int idx, f32* u0, f32* u1, f32* v0, f32* v1) {
    *u0 = cell * (idx & (grid - 1));
    *u1 = *u0 + cell;
    *v0 = cell * (idx >> shift);
    *v1 = *v0 - cell;
}

void ExhaustTorchMan::Load(void) {
    faceTex = fn_802CAC28(lbl_8055CA84, 0);
    edgeTex = fn_802CAC28(lbl_8055CA88, 0);
    if (!fn_80297D5C(&grid, 16) || !fn_80297D90(&shift, grid)) {
        fn_802E317C("fire: numerical error with backfire textures");
    }
    unk1F4 = 16 - (shift << 1);
    cell = 1.0f / grid;
}

void ExhaustTorchMan::DrawEdge(const Vec* p, int frame, const Vec* a, const Vec* b, f32 len) {
    Vec c[4];
    f32 u0;
    f32 u1;
    f32 v0;
    f32 v1;
    CellUV(frame, &u0, &u1, &v0, &v1);
    AddScaled(&c[0], p, a, -len);
    AddScaled(&c[0], &c[0], b, len);
    AddScaled(&c[1], &c[0], a, 2.0f * len);
    AddScaled(&c[2], &c[0], b, -2.0f * len);
    AddScaled(&c[3], &c[1], b, -2.0f * len);
    fn_802C8E00(0xC0FFFFFF, 0, c[0].x, c[0].y, c[0].z, u0, v0);
    fn_802C8E00(0xC0FFFFFF, 0, c[1].x, c[1].y, c[1].z, u1, v0);
    fn_802C8E00(0xC0FFFFFF, 1, c[2].x, c[2].y, c[2].z, u0, v1);
    fn_802C8E00(0xC0FFFFFF, 1, c[3].x, c[3].y, c[3].z, u1, v1);
}

void ExhaustTorchMan::DrawFace(const TorchEntry* m, int frame, f32 len) {
    Vec c[4];
    f32 u0;
    f32 u1;
    f32 v0;
    f32 v1;
    CellUV(frame, &u0, &u1, &v0, &v1);
    AddScaled(&c[0], &m->pos, &m->rot[2], -len);
    AddScaled(&c[1], &c[0], &m->rot[1], 2.0f * len);
    AddScaled(&c[2], &c[0], &m->rot[2], 2.0f * len);
    AddScaled(&c[3], &c[1], &m->rot[2], 2.0f * len);
    fn_802C8E00(0xC0FFFFFF, 0, c[0].x, c[0].y, c[0].z, u0, v0);
    fn_802C8E00(0xC0FFFFFF, 0, c[1].x, c[1].y, c[1].z, u1, v0);
    fn_802C8E00(0xC0FFFFFF, 1, c[2].x, c[2].y, c[2].z, u0, v1);
    fn_802C8E00(0xC0FFFFFF, 1, c[3].x, c[3].y, c[3].z, u1, v1);
    AddScaled(&c[0], &m->pos, &m->rot[0], -len);
    AddScaled(&c[1], &c[0], &m->rot[1], 2.0f * len);
    AddScaled(&c[2], &c[0], &m->rot[0], 2.0f * len);
    AddScaled(&c[3], &c[1], &m->rot[0], 2.0f * len);
    fn_802C8E00(0xC0FFFFFF, 0, c[0].x, c[0].y, c[0].z, u0, v0);
    fn_802C8E00(0xC0FFFFFF, 0, c[1].x, c[1].y, c[1].z, u1, v0);
    fn_802C8E00(0xC0FFFFFF, 1, c[2].x, c[2].y, c[2].z, u0, v1);
    fn_802C8E00(0xC0FFFFFF, 1, c[3].x, c[3].y, c[3].z, u1, v1);
}

void ExhaustTorchMan::DrawEdges(void) {
    const Vec* right = fn_802C86DC()->rot;
    const Vec* up = &fn_802C86DC()->rot[1];
    for (int i = 0; i < fn_80287B14(); i++) {
        if (entries[i].on) {
            FireModel* model = fn_80288208(i);
            int n = model->nParts;
            for (int k = 0; k < n; k++) {
                const TorchPart* part = fn_80288060(model, k);
                const FireCar* c = fn_80287B24(i);
                TorchEntry* e = &entries[i];
                fn_801BFD34(&e->pos, &part->pos, c);
                Mat3Mul((Mat3*)e->rot, &part->rot, (const Mat3*)c);
                Vec p;
                p.x = e->pos.x + e->rot[1].x * entries[i].len;
                p.y = e->pos.y + e->rot[1].y * entries[i].len;
                p.z = e->pos.z + e->rot[1].z * entries[i].len;
                DrawEdge(&p, entries[i].frame, right, up, 0.4f * entries[i].len);
            }
        }
    }
}

void ExhaustTorchMan::DrawFaces(void) {
    for (int i = 0; i < fn_80287B14(); i++) {
        if (entries[i].on) {
            FireModel* model = fn_80288208(i);
            int n = model->nParts;
            for (int k = 0; k < n; k++) {
                const TorchPart* part = fn_80288060(model, k);
                const FireCar* c = fn_80287B24(i);
                TorchEntry* e = &entries[i];
                fn_801BFD34(&e->pos, &part->pos, c);
                Mat3Mul((Mat3*)e->rot, &part->rot, (const Mat3*)c);
                Vec p;
                p.x = e->pos.x + e->rot[1].x * entries[i].len;
                p.y = e->pos.y + e->rot[1].y * entries[i].len;
                p.z = e->pos.z + e->rot[1].z * entries[i].len;
                DrawFace(&entries[i], entries[i].frame, entries[i].len);
            }
        }
    }
}

BackfireMan::BackfireMan(void) {
    torch = new ("ExhaustTorchMan") ExhaustTorchMan;
    fn_801EEFDC(fn_8029BFF8, 4);
    Reset();
}

BackfireMan::~BackfireMan(void) {
    fn_801EF024(fn_8029BFF8);
    delete torch;
}

void BackfireMan::Reset(void) {
    for (int i = 0; i < fn_80287B14(); i++) {
        if (fn_80288208(i)->nParts > 0) {
            entries[i].state = 1;
        } else {
            entries[i].state = 0;
        }
        entries[i].d = 0;
    }
    torch->Reset();
}

void BackfireMan::Draw(void) { torch->Draw(); }

void BackfireMan::Event(const BackfireEvt* ev) {
    if (ev->car < 0 || ev->car >= fn_80287B14()) {
        return;
    }
    if ((entries[ev->car].state & 0xF) != 1) {
        return;
    }
    FireModel* model = fn_80288208(ev->car);
    int n = model->nParts;
    BackfireEntry* e = &entries[ev->car];
    if (ev->mode < 0) {
        e->state = 2;
        e->a = ev->x;
        e->b = fn_80287BC8(0.5f, 1.0f);
        e->c = fn_80287B5C(n);
        e->d = fn_80288060(model, lbl_802FA9F0[e->c] % n);
    } else {
        e->state = 0x12;
        e->a = ev->x;
        e->b = fn_80287BC8(0.05f, 0.1f);
        e->c = 0;
        e->d = fn_80288060(model, ev->mode % n);
    }
}

void BackfireMan::Tick(int i, int on) {
    BackfireEntry* e = &entries[i];
    if (!on) {
        if (e->state != 0) {
            e->state = 3;
        }
        e->d = 0;
    } else if (e->state == 3) {
        e->state = 1;
    } else if ((e->state & 0xF) == 2) {
        e->b = e->b - fn_801E7E24();
        if (e->b < 0.0f) {
            e->state = 1;
            e->b = 0.0f;
            e->d = 0;
        } else if (!(e->state & 0x10) && fn_80287B5C(40) == 0) {
            FireModel* model = fn_80288208(i);
            int n = model->nParts;
            e->c = (e->c + 1) % 8;
            e->d = fn_80288060(model, lbl_802FA9F0[e->c] % n);
        }
    }
    torch->Set(i, e->d);
}

TorchFireMan::TorchFireMan(void) {
    InitEmitter();
    fn_801EEFDC(fn_8029C190, 9);
}

TorchFireMan::~TorchFireMan(void) {
    fn_801EF024(fn_8029C190);
    fn_8028E398(emitter);
}

void TorchFireMan::Reset(void) { count = 0; }

void TorchFireMan::Update(void) {
    Vec v;
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = 0.0f;
    TorchFire* t = entries;
    for (int i = 0; i < count; i++, t++) {
        if (t->on) {
            fn_80288A58((const Vec*)&t->pos, &v);
            fn_8028E584(emitter, (const Vec*)&t->pos, &v);
        }
    }
}

void TorchFireMan::Add(const TorchFire* ev) {
    for (int i = 0; i < count; i++) {
        if (ev->key == entries[i].key) {
            entries[i] = *ev;
            return;
        }
    }
    entries[count] = *ev;
    count++;
}

void TorchFireMan::InitEmitter(void) {
    FireDesc e;
    fn_8028E61C(&e);
    e.max = 0xC0;
    e.size0 = 0.5f;
    e.size1 = 1.0f;
    e.kind = 1;
    e.c0 = FColor(0.2f, 0.2f, 0.2f);
    e.c1 = FColor(1.0f, 1.0f, 1.0f);
    e.gravity = 0.0f;
    e.life = -0.05f;
    e.grow = 0.5f;
    e.fade = 0.2f;
    e.spinMin = -3.0f;
    e.spinMax = 3.0f;
    e.a70 = -0.4f;
    e.a68 = -0.4f;
    e.a60 = -0.4f;
    e.a74 = 0.6f;
    e.a6C = 0.6f;
    e.a64 = 0.6f;
    e.a80 = 1.0f;
    e.i84 = 0x40;
    e.b88 = 1;
    e.frames = 2;
    e.b89 = 1;
    emitter = fn_8028E2F8(&e, "torch fire", lbl_8055CA80);
}

extern "C" {

int fn_8029BFF8(const void* ev) {
    lbl_8055F7D8->Event((const BackfireEvt*)ev);
    return 0x10;
}

int fn_8029C024(const void* ev) {
    lbl_8055F7D4->SetT(*(const int*)ev);
    return 4;
}

int fn_8029C054(const void* ev) {
    lbl_8055F7D4->SetActive(*(const int*)ev);
    return 4;
}

void fn_8029C084(FireCarEntry* e, int n) {
    f32 dt = fn_801E7E24();
    if ((f32)__fabs(dt) <= lbl_80558DC4[0]) {
        return;
    }
    for (int i = 0; i < n; i++, e++) {
        FireCar* car;
        s32 idx;
        u8 on;
        on = e->active;
        idx = e->index;
        car = e->car;
        lbl_8055F7C8->Set(idx, on && car->state == 11);
        lbl_8055F7D8->Tick(idx, on);
        if (lbl_8055F7D0) {
            lbl_8055F7D0->Toggle(idx, on && car->state == 12);
        }
        if (lbl_8055F7D4) {
            lbl_8055F7D4->Update(idx, car, dt, e->dist);
        }
    }
}

int fn_8029C190(const void* ev) {
    lbl_8055F7DC->Add((const TorchFire*)ev);
    return 0x14;
}

}  // extern "C"
