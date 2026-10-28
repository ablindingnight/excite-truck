// Path drones (0x801FC914-0x801FFB08): track model handlers for linear and Hermite object paths
// ("LinearPathTMH", "HermitePathTMH") that spawn drone objects from named pools at random
// intervals, and the drone handlers ("LinearDroneTMH", "HermiteDroneTMH") whose managers own the
// drones and move them along the paths: linear segments with interpolated speed and up vector, or
// Hermite curves with slerped key orientations that loop, reverse (turning the key rotations around)
// or restock at the end. Names are guesses apart from the strings.
#include "game/common.h"

void* operator new(size_t size, const char* tag);

extern "C" {
int strcasecmp(const char* a, const char* b);
char* strncpy(char* d, const char* s, u32 n);
void* memset(void* p, int c, u32 n);
double sqrt(double x);
double cos(double x);
double sin(double x);
void* __construct_array(void* p, void* ctor, void* dtor, u32 size, u32 n);
void __destroy_arr(void* p, void* dtor, u32 size, u32 n);
}

struct PVec {
    f32 x, y, z;
};

struct Quat {
    f32 x, y, z, w;
};

// An object's placement: three basis vectors and the origin (0x30).
struct OFrame {
    PVec x;    // offset 0x0
    PVec y;    // offset 0xC
    PVec z;    // offset 0x18
    PVec pos;  // offset 0x24
};

struct BMtx33 {
    f32 xx, xy, xz;
    f32 yx, yy, yz;
    f32 zx, zy, zz;
};

// A track object instance handed to its handler.
struct TObj {
    u8 pad0[4];
    void* model;  // offset 0x4
    u8 pad8[4];
    TObj* next;   // offset 0xC
    u8 pad10[0x2C - 0x10];
    u8 data[1];   // offset 0x2C
};

// Every handler's base (vtable slots 0x8-0x2C).
class TrackMod {
public:
    virtual ~TrackMod(void) {}
    virtual void Swap(TObj* o);
    virtual void Init(TObj* o);
    virtual void Unk14(void);
    virtual void Apply(void);
    virtual void Unk1C(void);
    virtual void Draw(TObj* list);
    virtual void DrawTree(void* tree);
    virtual void Add(void* scene, TObj* o, s32 id);
    virtual void Link(int a, void* b, int c, void* d, int e);
};

// A per-scene instance (vtable slots 0x8-0x14).
class TrackInst {
public:
    virtual ~TrackInst(void) {}
    virtual void Update(void* ctx);
    virtual int Kind(void);
    virtual void Draw2(void* ctx);
};

// One key of a linear path (0x20).
struct LinearKey {
    PVec pos;    // offset 0x0
    PVec up;     // offset 0xC
    f32 speed0;  // offset 0x18
    f32 speed1;  // offset 0x1C
};

// A linear path's object data; the name is the drone pool.
struct LinearPathData {
    char name[0x10];       // offset 0x0
    LinearKey keys[24];    // offset 0x10
    s32 count;             // offset 0x310
    f32 intervalMin;       // offset 0x314
    f32 intervalMax;       // offset 0x318
};

// One key of a Hermite path (0x24).
struct HermiteKey {
    PVec pos;   // offset 0x0
    Quat rot;   // offset 0xC
    f32 v;      // offset 0x1C
    f32 rate;   // offset 0x20
};

struct HermitePathData {
    char name[0x10];       // offset 0x0
    HermiteKey keys[48];   // offset 0x10
    s32 count;             // offset 0x6D0

    HermiteKey* Loop(int i) { return &keys[(count + i) % count]; }
    HermiteKey* Ping(int i) {
        if (i >= count && i < count * 2) {
            return &keys[count * 2 - i - 1];
        }
        if (i >= 0) {
            return &keys[i % count];
        }
        return &keys[-i];
    }
    HermiteKey* Clamp(int i) {
        if (i >= 0 && i < count) {
            return &keys[i];
        }
        return 0;
    }
};

// A Hermite path entry of the handler (0x24).
struct HermiteEntry {
    HermitePathData* data;  // offset 0x0
    char pool[0x10];        // offset 0x4
    u16 start;              // offset 0x14
    u16 finish;             // offset 0x16
    f32 intervalMin;        // offset 0x18
    f32 intervalMax;        // offset 0x1C
    f32 timer;              // offset 0x20
};

// A drone moving along a linear path (0x40).
struct LinearDrone {
    LinearDrone(void) {
        name = 0;
        path = 0;
    }
    ~LinearDrone(void);
    void Init(TObj* o, LinearPathData* d);
    void Update(f32 dt);
    void Draw(void);
    void Start(LinearPathData* d);
    void Advance(f32 dt);

    const char* name;       // offset 0x0
    const LinearPathData* path;   // offset 0x4
    void* model;            // offset 0x8
    OFrame frame;           // offset 0xC
    s32 seg;                // offset 0x3C
};

// A drone moving along a Hermite path (0x70).
struct HermiteDrone {
    HermiteDrone(void) {
        name = 0;
        entry = 0;
    }
    ~HermiteDrone(void);
    void Init(TObj* o, HermitePathData* d);
    void Update(f32 dt);
    void Draw(void);
    void Start(HermiteEntry* e);
    void Curve(const HermiteKey* k0, const HermiteKey* k1, const HermiteKey* k2, const HermiteKey* k3, f32 t);
    void Turn(f32 t);
    void UpdateLoop(f32 dt);
    void UpdateReverse(f32 dt);
    void UpdateRestock(f32 dt);

    const char* name;     // offset 0x0
    HermiteEntry* entry;  // offset 0x4
    void* model;          // offset 0x8
    OFrame frame;         // offset 0xC
    s32 seg;              // offset 0x3C
    f32 t;                // offset 0x40
    Quat q0;              // offset 0x44
    Quat q1;              // offset 0x54
    f32 v0;               // offset 0x64
    f32 v1;               // offset 0x68
    f32 v;                // offset 0x6C
};

// A named pool of linear drones (0x98).
struct LinearPool {

    char name[0x10];          // offset 0x0
    LinearDrone* unused[16];  // offset 0x10
    LinearDrone* inUse[16];   // offset 0x50
    s32 unusedCount;          // offset 0x90
    s32 inUseCount;           // offset 0x94
};

struct LinearPools {
    LinearPools(void);
    ~LinearPools(void);
    void Update(f32 dt);
    void Draw(void);
    void Add(LinearDrone* d);
    void InitPool(LinearPool* p, const char* n);
    LinearPool* Find(const char* name) {
        for (int i = 0; i < count; i++) {
            if (strcasecmp(name, pools[i].name) == 0) {
                return &pools[i];
            }
        }
        return 0;
    }
    void UpdatePool(LinearPool* p, f32 dt);
    void DrawPool(LinearPool* p);

    LinearPool pools[8];  // offset 0x0
    s32 count;            // offset 0x4C0
};

// A named pool of Hermite drones (0x58).
struct HermitePool {

    char name[0x10];           // offset 0x0
    HermiteDrone* unused[8];   // offset 0x10
    HermiteDrone* inUse[8];    // offset 0x30
    s32 unusedCount;           // offset 0x50
    s32 inUseCount;            // offset 0x54
};

struct HermitePools {
    HermitePools(void);
    ~HermitePools(void);
    void Update(f32 dt);
    void Draw(void);
    void Add(HermiteDrone* d);
    void InitPool(HermitePool* p, const char* n);
    HermitePool* Find(const char* name) {
        for (int i = 0; i < count; i++) {
            if (strcasecmp(name, pools[i].name) == 0) {
                return &pools[i];
            }
        }
        return 0;
    }
    void UpdatePool(HermitePool* p, f32 dt);
    void DrawPool(HermitePool* p);

    HermitePool pools[8];  // offset 0x0
    s32 count;             // offset 0x2C0
};

class LinearDroneMan : public TrackInst {
public:
    LinearDroneMan(void) : count(0) {}
    virtual ~LinearDroneMan(void);
    virtual void Update(void* ctx);
    virtual int Kind(void);
    virtual void Draw2(void* ctx);
    void AddDrone(TObj* o, LinearPathData* d);

    LinearDrone drones[64];  // offset 0x4
    s32 count;               // offset 0x1004
    LinearPools pools;       // offset 0x1008
};

class HermiteDroneMan : public TrackInst {
public:
    HermiteDroneMan(void) : count(0) {}
    virtual ~HermiteDroneMan(void);
    virtual void Update(void* ctx);
    virtual int Kind(void);
    virtual void Draw2(void* ctx);
    void AddDrone(TObj* o, HermitePathData* d);

    HermiteDrone drones[32];  // offset 0x4
    s32 count;                // offset 0xE04
    HermitePools pools;       // offset 0xE08
};

struct LinearPathSlot {
    const LinearPathData* data;  // offset 0x0
    f32 timer;             // offset 0x4
};

class LinearPathTMH : public TrackMod {
public:
    LinearPathTMH(void);
    virtual ~LinearPathTMH(void);
    virtual void Swap(TObj* o);
    virtual void Init(TObj* o);
    virtual void Unk1C(void);
    virtual void Draw(TObj* list);

    LinearPathSlot paths[6];  // offset 0x4
    s32 count;                // offset 0x34
};

class LinearDroneTMH : public TrackMod {
public:
    LinearDroneTMH(void) { man = 0; }
    virtual ~LinearDroneTMH(void);
    virtual void Swap(TObj* o);
    virtual void Init(TObj* o);
    virtual void Unk1C(void);
    virtual void Draw(TObj* list);
    virtual void Add(void* scene, TObj* o, s32 id);

    LinearDroneMan* man;  // offset 0x4
};

class HermitePathTMH : public TrackMod {
public:
    HermitePathTMH(void);
    virtual ~HermitePathTMH(void);
    virtual void Swap(TObj* o);
    virtual void Init(TObj* o);
    virtual void Apply(void);
    virtual void Unk1C(void);
    virtual void Draw(TObj* list);

    HermiteEntry paths[4];  // offset 0x4
    s32 count;              // offset 0x94
};

class HermiteDroneTMH : public TrackMod {
public:
    HermiteDroneTMH(void) { man = 0; }
    virtual ~HermiteDroneTMH(void);
    virtual void Swap(TObj* o);
    virtual void Init(TObj* o);
    virtual void Unk1C(void);
    virtual void Draw(TObj* list);
    virtual void Add(void* scene, TObj* o, s32 id);

    HermiteDroneMan* man;  // offset 0x4
};

extern "C" {
void PSVECCrossProduct(const void* a, const void* b, void* out);
f32 PSVECDotProduct(const void* a, const void* b);
void PSVECNormalize(const void* src, void* dst);
f32 PSVECMag(const void* v);
f32 PSVECDistance(const void* a, const void* b);
void fn_802C744C(void* model, const OFrame* f);
void fn_802C7040(void* model);
f32 fn_802C99E8(f32 v);
void fn_802E317C(const char* fmt, ...);
f32 fn_801E7E24(void);
f32 fn_80129DC4(int seed, f32 lo, f32 hi);
int fn_80129D18(int seed, int range);
ValueNode* fn_802AE414(const char* name);
void fn_802CD320(void* scene, TrackInst* inst);
void fn_8012F57C(Quat* out, Quat* a, Quat* b, f32 t);
f32 fn_80144514(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f);
}

extern f32 lbl_80558DC4[];

ModuleTag lbl_8055EC88;

struct DroneColor {
    DroneColor(void) { rgba = 0xFFFF0000; }
    u32 rgba;
};

static DroneColor lbl_8055EC90;
LinearPools* lbl_8055EC94;
HermitePools* lbl_8055EC98;

static inline f32 Dot(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f) { return fn_80144514(a, b, c, d, e, f); }

// d = a * b
static inline void MtxMul(BMtx33* d, const BMtx33* a, const BMtx33* b) {
    if (d == a || d == b) {
        f32 t00 = Dot(b->xx, a->xx, b->yx, a->xy, b->zx, a->xz);
        f32 t10 = Dot(b->xx, a->yx, b->yx, a->yy, b->zx, a->yz);
        f32 t20 = Dot(b->xx, a->zx, b->yx, a->zy, b->zx, a->zz);
        f32 t01 = Dot(b->xy, a->xx, b->yy, a->xy, b->zy, a->xz);
        f32 t11 = Dot(b->xy, a->yx, b->yy, a->yy, b->zy, a->yz);
        f32 t21 = Dot(b->xy, a->zx, b->yy, a->zy, b->zy, a->zz);
        f32 t02 = Dot(b->xz, a->xx, b->yz, a->xy, b->zz, a->xz);
        f32 t12 = Dot(b->xz, a->yx, b->yz, a->yy, b->zz, a->yz);
        f32 t22 = Dot(b->xz, a->zx, b->yz, a->zy, b->zz, a->zz);
        d->xx = t00;
        d->yx = t10;
        d->zx = t20;
        d->xy = t01;
        d->yy = t11;
        d->zy = t21;
        d->xz = t02;
        d->yz = t12;
        d->zz = t22;
    } else {
        d->xx = Dot(b->xx, a->xx, b->yx, a->xy, b->zx, a->xz);
        d->yx = Dot(b->xx, a->yx, b->yx, a->yy, b->zx, a->yz);
        d->zx = Dot(b->xx, a->zx, b->yx, a->zy, b->zx, a->zz);
        d->xy = Dot(b->xy, a->xx, b->yy, a->xy, b->zy, a->xz);
        d->yy = Dot(b->xy, a->yx, b->yy, a->yy, b->zy, a->yz);
        d->zy = Dot(b->xy, a->zx, b->yy, a->zy, b->zy, a->zz);
        d->xz = Dot(b->xz, a->xx, b->yz, a->xy, b->zz, a->xz);
        d->yz = Dot(b->xz, a->yx, b->yz, a->yy, b->zz, a->yz);
        d->zz = Dot(b->xz, a->zx, b->yz, a->zy, b->zz, a->zz);
    }
}

// The rotation of a unit quaternion.
static inline void QuatToMtx(BMtx33* m, const Quat* q) {
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

static inline void MtxToQuatAxis(f32 (*m)[3], Quat* q) {
    f32* v = &q->x;
    int i = 0;
    if (m[1][1] > m[0][0]) {
        i = 1;
    }
    if (m[2][2] > m[i][i]) {
        i = 2;
    }
    int j = (i + 1) % 3;
    int k = (j + 1) % 3;
    f32 s = sqrt(m[i][i] - (m[j][j] + m[k][k]) + 1.0f);
    v[i] = 0.5f * s;
    s = 0.5f / s;
    q->w = (m[j][k] - m[k][j]) * s;
    v[j] = (m[i][j] + m[j][i]) * s;
    v[k] = (m[i][k] + m[k][i]) * s;
}

static inline void MtxToQuat(const BMtx33* f, Quat* q) {
    const f32(*m)[3] = (const f32(*)[3])f;
    f32 tr = f->xx + f->yy + f->zz;
    if (tr > 0.0f) {
        f32 s = sqrt(tr + 1.0f);
        q->w = 0.5f * s;
        s = 0.5f / s;
        q->x = (m[1][2] - m[2][1]) * s;
        q->y = (m[2][0] - m[0][2]) * s;
        q->z = (m[0][1] - m[1][0]) * s;
    } else {
        MtxToQuatAxis((f32(*)[3])f, q);
    }
}

static inline void SetIdentity(OFrame* f) {
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

static inline void SwapName(u8* p) {
    for (int i = 0; i < 8; i++) {
        u8 t = p[i];
        p[i] = p[15 - i];
        p[15 - i] = t;
    }
}

static inline void Swap32(u8* p) {
    u8 t = p[0];
    p[0] = p[3];
    p[3] = t;
    t = p[1];
    p[1] = p[2];
    p[2] = t;
}

extern "C" TrackMod* fn_801FC914(void) { return new ("LinearPathTMH") LinearPathTMH; }

extern "C" TrackMod* fn_801FC94C(void) { return new ("LinearDroneTMH") LinearDroneTMH; }

extern "C" TrackMod* fn_801FC994(void) { return new ("HermitePathTMH") HermitePathTMH; }

extern "C" TrackMod* fn_801FC9CC(void) { return new ("HermiteDroneTMH") HermiteDroneTMH; }

LinearDrone::~LinearDrone(void) {}

void LinearDrone::Init(TObj* o, LinearPathData* d) {
    name = d->name;
    model = o->model;
    path = 0;
    SetIdentity(&frame);
    seg = -1;
}

void LinearDrone::Update(f32 dt) {
    if (path) {
        Advance(dt);
    }
}

void LinearDrone::Draw(void) {
    if (path) {
        fn_802C744C(model, &frame);
    }
}

void LinearDrone::Start(LinearPathData* d) {
    path = d;
    seg = 0;
    SetIdentity(&frame);
    frame.pos = d->keys[0].pos;
}

extern "C" void fn_801FEA28(LinearDrone* d);

void LinearDrone::Advance(f32 dt) {
    const LinearKey* k = &path->keys[seg];
    const PVec* p0 = &k[0].pos;
    const PVec* p1 = &k[1].pos;
    PVec d;
    d.x = p1->x - p0->x;
    d.y = p1->y - p0->y;
    d.z = p1->z - p0->z;
    f32 inv = 1.0f / PSVECMag(&d);
    f32 s = inv * PSVECDistance(&frame.pos, p0);
    f32 step = inv * (dt * (k->speed0 + s * (k->speed1 - k->speed0)));
    PVec np;
    np.x = frame.pos.x + d.x * step;
    np.y = frame.pos.y + d.y * step;
    np.z = frame.pos.z + d.z * step;
    PVec rem;
    rem.x = p1->x - np.x;
    rem.y = p1->y - np.y;
    rem.z = p1->z - np.z;
    if (PSVECDotProduct(&d, &rem) <= 0.0f) {
        seg++;
        if (seg >= path->count - 1) {
            fn_801FEA28(this);
            path = 0;
            return;
        }
        p0 = p1;
        p1 = &path->keys[seg + 1].pos;
        d.x = p1->x - p0->x;
        d.y = p1->y - p0->y;
        d.z = p1->z - p0->z;
        inv = 1.0f / PSVECMag(&d);
        f32 over = inv * PSVECMag(&rem);
        frame.pos.x = p0->x + d.x * over;
        frame.pos.y = p0->y + d.y * over;
        frame.pos.z = p0->z + d.z * over;
    } else {
        frame.pos = np;
    }
    if (seg + 2 < path->count) {
        const PVec* p2 = &path->keys[seg + 2].pos;
        PVec d2;
        d2.x = p2->x - p1->x;
        d2.y = p2->y - p1->y;
        d2.z = p2->z - p1->z;
        f32 u = inv * PSVECDistance(&frame.pos, p0);
        PVec up;
        up.x = ((const LinearKey*)p0)->up.x + u * (((const LinearKey*)p1)->up.x - ((const LinearKey*)p0)->up.x);
        up.y = ((const LinearKey*)p0)->up.y + u * (((const LinearKey*)p1)->up.y - ((const LinearKey*)p0)->up.y);
        up.z = ((const LinearKey*)p0)->up.z + u * (((const LinearKey*)p1)->up.z - ((const LinearKey*)p0)->up.z);
        frame.x = up;
        PVec dir;
        dir.x = d.x + u * (d2.x - d.x);
        dir.y = d.y + u * (d2.y - d.y);
        dir.z = d.z + u * (d2.z - d.z);
        frame.z = dir;
        PSVECNormalize(&frame.z, &frame.z);
    } else {
        frame.x = ((const LinearKey*)p0)->up;
        frame.z.x = d.x * inv;
        frame.z.y = d.y * inv;
        frame.z.z = d.z * inv;
    }
    PSVECCrossProduct(&frame.z, &frame.x, &frame.y);
}

HermiteDrone::~HermiteDrone(void) {}

void HermiteDrone::Init(TObj* o, HermitePathData* d) {
    name = d->name;
    model = o->model;
    entry = 0;
    SetIdentity(&frame);
    seg = -1;
    t = 0.0f;
}

void HermiteDrone::Update(f32 dt) {
    if (!entry) {
        return;
    }
    switch (entry->finish) {
    case 0:
        UpdateLoop(dt);
        break;
    case 1:
        UpdateReverse(dt);
        break;
    case 2:
        UpdateRestock(dt);
        break;
    default:
        fn_802E317C("path: unknown end behavior %d", entry->finish);
        break;
    }
}

void HermiteDrone::Draw(void) {
    if (entry) {
        f32 old = fn_802C99E8(v);
        fn_802C744C(model, &frame);
        fn_802C99E8(old);
    }
}

static inline void QuatToFrame(OFrame* f, const Quat* q) {
    f32 x2 = q->x + q->x;
    f32 y2 = q->y + q->y;
    f32 z2 = q->z + q->z;
    f32 xx = q->x * x2;
    f32 yy = q->y * y2;
    f32 zz = q->z * z2;
    f32 wx = q->w * x2;
    f32 yz = q->y * z2;
    f32 wy = q->w * y2;
    f32 xz = q->x * z2;
    f32 wz = q->w * z2;
    f32 xy = q->x * y2;
    f->x.x = 1.0f - (yy + zz);
    f->y.y = 1.0f - (xx + zz);
    f->z.z = 1.0f - (xx + yy);
    f->z.y = yz - wx;
    f->y.z = yz + wx;
    f->z.x = xz + wy;
    f->x.z = xz - wy;
    f->y.x = xy - wz;
    f->x.y = xy + wz;
}

void HermiteDrone::Start(HermiteEntry* e) {
    entry = e;
    HermitePathData* d = e->data;
    seg = 0;
    t = 0.0f;
    q0 = d->keys[0].rot;
    q1 = d->keys[1].rot;
    v0 = d->keys[0].v;
    v1 = d->keys[1].v;
    v = d->keys[0].v;
    frame.pos = d->keys[0].pos;
    QuatToFrame(&frame, &d->keys[0].rot);
}

void HermiteDrone::Curve(const HermiteKey* k0, const HermiteKey* k1, const HermiteKey* k2, const HermiteKey* k3,
                         f32 u) {
    PVec m0;
    PVec m1;
    if (k0) {
        m0.x = k2->pos.x - k0->pos.x;
        m0.y = k2->pos.y - k0->pos.y;
        m0.z = k2->pos.z - k0->pos.z;
    } else {
        m0.x = k2->pos.x - k1->pos.x;
        m0.y = k2->pos.y - k1->pos.y;
        m0.z = k2->pos.z - k1->pos.z;
    }
    if (k3) {
        m1.x = k3->pos.x - k1->pos.x;
        m1.y = k3->pos.y - k1->pos.y;
        m1.z = k3->pos.z - k1->pos.z;
    } else {
        m1.x = k2->pos.x - k1->pos.x;
        m1.y = k2->pos.y - k1->pos.y;
        m1.z = k2->pos.z - k1->pos.z;
    }
    f32 u2 = u * u;
    f32 u3 = u * u2;
    f32 h00 = 1.0f + (2.0f * u3 - 3.0f * u2);
    f32 h10 = 0.5f * (u + (u3 - 2.0f * u2));
    f32 h11 = 0.5f * (u3 - u2);
    f32 h01 = -2.0f * u3 + 3.0f * u2;
    PVec a;
    a.x = k1->pos.x * h00 + m0.x * h10 + m1.x * h11;
    a.y = k1->pos.y * h00 + m0.y * h10 + m1.y * h11;
    a.z = k1->pos.z * h00 + m0.z * h10 + m1.z * h11;
    frame.pos.x = a.x + k2->pos.x * h01;
    frame.pos.y = a.y + k2->pos.y * h01;
    frame.pos.z = a.z + k2->pos.z * h01;
}

void HermiteDrone::Turn(f32 u) {
    Quat q;
    fn_8012F57C(&q, &q0, &q1, u);
    QuatToFrame(&frame, &q);
}

void HermiteDrone::UpdateLoop(f32 dt) {
    f32 nt = t + dt;
    HermiteKey* k = entry->data->Loop(seg);
    f32 u = k->rate * nt;
    if (u < 1.0f) {
        t = nt;
    } else {
        t = t + (dt - 1.0f / k->rate);
        u = u - 1.0f;
        seg = (seg + 1) % entry->data->count;
        q0 = q1;
        v0 = v1;
        k = entry->data->Loop(seg);
        q1 = entry->data->Loop(seg + 1)->rot;
        v1 = entry->data->Loop(seg + 1)->v;
    }
    HermitePathData* d = entry->data;
    Curve(d->Loop(seg - 1), k, d->Loop(seg + 1), d->Loop(seg + 2), u);
    Turn(u);
    v = v0 + u * (v1 - v0);
}

void HermiteDrone::UpdateReverse(f32 dt) {
    HermitePathData* d = entry->data;
    HermiteKey* k = d->Ping(seg);
    f32 nt = t + dt;
    f32 u = k->rate * nt;
    if (u < 1.0f) {
        t = nt;
    } else {
        t = t + (dt - 1.0f / k->rate);
        u = u - 1.0f;
        seg = (seg + 1) % (d->count * 2);
        k = entry->data->Ping(seg);
        q0 = q1;
        v0 = v1;
        q1 = entry->data->Ping(seg + 1)->rot;
        v1 = entry->data->Ping(seg + 1)->v;
        int next = seg + 1;
        if (next >= d->count && next < d->count * 2) {
            BMtx33 m;
            BMtx33 r;
            QuatToMtx(&m, &q1);
            f32 c = cos(3.141592653589793);
            f32 s = sin(3.141592653589793);
            r.xx = c;
            r.xy = 0.0f;
            r.xz = -s;
            r.yx = 0.0f;
            r.yy = 1.0f;
            r.yz = 0.0f;
            r.zx = s;
            r.zy = 0.0f;
            r.zz = c;
            MtxMul(&m, &r, &m);
            MtxToQuat(&m, &q1);
        }
    }
    if (d->count - 1 != seg % d->count) {
        HermitePathData* p = entry->data;
        Curve(p->Ping(seg - 1), k, p->Ping(seg + 1), p->Ping(seg + 2), u);
    }
    Turn(u);
    v = v0 + u * (v1 - v0);
}

extern "C" void fn_801FEC74(HermiteDrone* d);

void HermiteDrone::UpdateRestock(f32 dt) {
    HermiteKey* k = entry->data->Clamp(seg);
    f32 nt = t + dt;
    f32 u = k->rate * nt;
    if (u < 1.0f) {
        t = nt;
    } else {
        if (seg + 2 >= entry->data->count) {
            fn_801FEC74(this);
            entry = 0;
            seg = -1;
            return;
        }
        t = t + (dt - 1.0f / k->rate);
        u = u - 1.0f;
        seg++;
        q0 = q1;
        v0 = v1;
        q1 = entry->data->Clamp(seg + 1)->rot;
        v1 = entry->data->Clamp(seg + 1)->v;
    }
    HermitePathData* d = entry->data;
    Curve(d->Clamp(seg - 1), d->Clamp(seg), d->Clamp(seg + 1), d->Clamp(seg + 2), u);
    Turn(u);
    v = v0 + u * (v1 - v0);
}

LinearPathTMH::LinearPathTMH(void) {
    memset(paths, 0, sizeof(paths));
    count = 0;
}

void LinearPathTMH::Swap(TObj* o) {
    LinearPathData* d = (LinearPathData*)o->data;
    SwapName((u8*)d->name);
    u8* p = (u8*)d->keys;
    for (int i = 0; i < 32; i++) {
        Swap32(p);
        Swap32(p + 4);
        Swap32(p + 8);
        Swap32(p + 12);
        Swap32(p + 16);
        Swap32(p + 20);
        p += 0x18;
    }
    Swap32((u8*)&d->count);
    Swap32((u8*)&d->intervalMin);
    Swap32((u8*)&d->intervalMax);
}

void LinearPathTMH::Init(TObj* o) {
    const LinearPathData* d = (const LinearPathData*)o->data;
    if (count >= 6) {
        fn_802E317C("exceeded limit of %d paths", 6);
    }
    f32 lo = d->intervalMin;
    f32 hi = d->intervalMax;
    paths[count].data = d;
    paths[count].timer = fn_80129DC4(0, lo, hi);
    count++;
}

extern "C" void fn_801FEB58(LinearPathData* d);

void LinearPathTMH::Unk1C(void) {
    f32 dt = fn_801E7E24();
    if ((f32)__fabs(dt) <= lbl_80558DC4[0]) {
        return;
    }
    LinearPathSlot* s = paths;
    for (int i = 0; i < count; s++, i++) {
        if ((s->timer = s->timer - dt) <= 0.0f) {
            fn_801FEB58((LinearPathData*)s->data);
            s->timer = fn_80129DC4(0, s->data->intervalMin, s->data->intervalMax);
        }
    }
}

void LinearPathTMH::Draw(TObj* list) {
    for (; list; list = list->next) {
        fn_802C7040(list->model);
    }
}

HermitePathTMH::HermitePathTMH(void) { count = 0; }

void HermitePathTMH::Swap(TObj* o) {
    HermitePathData* d = (HermitePathData*)o->data;
    SwapName((u8*)d->name);
    u8* p = (u8*)d->keys;
    for (int i = 0; i < 72; i++) {
        Swap32(p);
        Swap32(p + 4);
        Swap32(p + 8);
        Swap32(p + 12);
        Swap32(p + 16);
        Swap32(p + 20);
        p += 0x18;
    }
    Swap32((u8*)&d->count);
}

extern "C" void fn_801FE88C(HermitePathTMH* t, HermiteEntry* e);

void HermitePathTMH::Init(TObj* o) {
    HermitePathData* d = (HermitePathData*)o->data;
    if (count >= 4) {
        fn_802E317C("path: exceeded hermite path limit");
    }
    HermiteEntry* e = &paths[count];
    e->data = d;
    fn_801FE88C(this, e);
    count++;
}

extern "C" void fn_801FEDA4(HermiteEntry* e);

void HermitePathTMH::Apply(void) {
    HermiteEntry* e = paths;
    for (int i = 0; i < count; e++, i++) {
        if (e->start == 0) {
            fn_801FEDA4(e);
        }
    }
}

void HermitePathTMH::Unk1C(void) {
    HermiteEntry* e = paths;
    for (int i = 0; i < count; e++, i++) {
        if (e->start == 1) {
            if ((e->timer = e->timer - fn_801E7E24()) < 0.0f) {
                fn_801FEDA4(e);
                e->timer = fn_80129DC4(0, e->intervalMin, e->intervalMax);
            }
        }
    }
}

extern "C" {
void fn_801FE88C(HermitePathTMH* t, HermiteEntry* e) {
    static char starts[] = "init\0interval";
    static char finishes[] = "loop\0reverse\0restock";
    ValueNode* vn = fn_802AE414(e->data->name);
    if (!vn) {
        fn_802E317C("path: hermite paths must have an entry in some trkefx file");
    }
    strncpy(e->pool, vn->GetValue("pool"), 0x10);
    e->pool[15] = 0;
    e->start = vn->GetEnum("start", "init\0interval");
    e->finish = vn->GetEnum("finish", "loop\0reverse\0restock");
    if (e->start == 1) {
        e->intervalMin = vn->GetFloat("interval_min");
        e->intervalMax = vn->GetFloat("interval_max");
        e->timer = 0.0f;
    }
}
}

HermitePathTMH::~HermitePathTMH(void) {}

LinearPathTMH::~LinearPathTMH(void) {}


extern "C" {
void fn_801FEA28(LinearDrone* d) {
    LinearPool* p = lbl_8055EC94->Find(d->name);
    if (!p) {
        fn_802E317C("path: restocking object with invalid pool %s", d->name);
    }
    int i;
    int n = p->inUseCount;
    for (i = 0; i < n; i++) {
        if (p->inUse[i] == d) {
            break;
        }
    }
    if (i >= n) {
        fn_802E317C("path: object mismatch; in_use=%d, unused=%d", n, p->unusedCount);
    }
    for (int j = i + 1; j < p->inUseCount; j++) {
        p->inUse[j - 1] = p->inUse[j];
    }
    p->inUseCount--;
    p->unused[p->unusedCount++] = d;
}
}

extern "C" {
void fn_801FEB58(LinearPathData* data) {
    LinearPool* p = lbl_8055EC94->Find(data->name);
    if (!p) {
        fn_802E317C("path: could not find pool %s", data->name);
    }
    if (p->unusedCount > 0) {
        int r = fn_80129D18(0, p->unusedCount);
        LinearDrone* d = p->unused[r];
        for (r++; r < p->unusedCount; r++) {
            p->unused[r - 1] = p->unused[r];
        }
        p->unusedCount--;
        d->Start(data);
        p->inUse[p->inUseCount++] = d;
    }
}
}


extern "C" {
void fn_801FEC74(HermiteDrone* d) {
    HermitePool* p = lbl_8055EC98->Find(d->name);
    if (!p) {
        fn_802E317C("path: restocking object with invalid pool %s", d->name);
    }
    int i;
    int n = p->inUseCount;
    for (i = 0; i < n; i++) {
        if (p->inUse[i] == d) {
            break;
        }
    }
    if (i >= n) {
        fn_802E317C("path: object mismatch; in_use=%d, unused=%d", n, p->unusedCount);
    }
    for (int j = i + 1; j < p->inUseCount; j++) {
        p->inUse[j - 1] = p->inUse[j];
    }
    p->inUseCount--;
    p->unused[p->unusedCount++] = d;
}
}

extern "C" {
void fn_801FEDA4(HermiteEntry* e) {
    HermitePool* p = lbl_8055EC98->Find(e->pool);
    if (!p) {
        fn_802E317C("path: could not find pool %s", e->pool);
    }
    if (p->unusedCount > 0) {
        int r = fn_80129D18(0, p->unusedCount);
        HermiteDrone* d = p->unused[r];
        for (r++; r < p->unusedCount; r++) {
            p->unused[r - 1] = p->unused[r];
        }
        p->unusedCount--;
        d->Start(e);
        p->inUse[p->inUseCount++] = d;
    }
}
}

void LinearDroneTMH::Add(void* scene, TObj* o, s32 id) {
    LinearPathData* d = (LinearPathData*)o->data;
    if (!man) {
        man = new ("DroneManager") LinearDroneMan;
        fn_802CD320(scene, man);
    }
    man->AddDrone(o, d);
}

void LinearDroneTMH::Draw(TObj* list) {
    if (man) {
        man->Draw2(0);
    }
}

void LinearDroneMan::Draw2(void* ctx) { pools.Draw(); }

void LinearDroneTMH::Unk1C(void) {}

void LinearDroneTMH::Init(TObj* o) {}

void LinearDroneTMH::Swap(TObj* o) {}

// MATCHING-AID: the target keeps this dtor's own vtable store, which MWCC only does when the body
// had a call; a stripped (empty inline) debug call stands in for it (cf. smoke SmokeNop, panelvol DbgLine).
static inline void DroneTeardown(void* man) {}

LinearDroneTMH::~LinearDroneTMH(void) { DroneTeardown(man); }

void HermiteDroneTMH::Add(void* scene, TObj* o, s32 id) {
    HermitePathData* d = (HermitePathData*)o->data;
    if (!man) {
        man = new ("DroneManager") HermiteDroneMan;
        fn_802CD320(scene, man);
    }
    man->AddDrone(o, d);
}

void HermiteDroneTMH::Draw(TObj* list) {
    if (man) {
        man->Draw2(0);
    }
}

void HermiteDroneMan::Draw2(void* ctx) { pools.Draw(); }

void HermiteDroneTMH::Unk1C(void) {}

void HermiteDroneTMH::Init(TObj* o) {}

void HermiteDroneTMH::Swap(TObj* o) {}

// MATCHING-AID: stripped debug call, see ~LinearDroneTMH.
HermiteDroneTMH::~HermiteDroneTMH(void) { DroneTeardown(man); }

void HermitePathTMH::Draw(TObj* list) {}

void HermiteDroneMan::Update(void* ctx) {
    f32 dt = fn_801E7E24();
    if ((f32)__fabs(dt) <= lbl_80558DC4[0]) {
        return;
    }
    pools.Update(dt);
}

int HermiteDroneMan::Kind(void) { return 3; }

void HermiteDroneMan::AddDrone(TObj* o, HermitePathData* d) {
    if (count >= 32) {
        fn_802E317C("path: exceeded total drone limit");
    }
    drones[count].Init(o, d);
    pools.Add(&drones[count]);
    count++;
}

void LinearDroneMan::Update(void* ctx) {
    f32 dt = fn_801E7E24();
    if ((f32)__fabs(dt) <= lbl_80558DC4[0]) {
        return;
    }
    pools.Update(dt);
}

int LinearDroneMan::Kind(void) { return 3; }

void LinearDroneMan::AddDrone(TObj* o, LinearPathData* d) {
    if (count >= 64) {
        fn_802E317C("path: exceeded total drone limit");
    }
    drones[count].Init(o, d);
    pools.Add(&drones[count]);
    count++;
}

HermitePools::HermitePools(void) {
    lbl_8055EC98 = this;
    count = 0;
}

HermitePools::~HermitePools(void) { lbl_8055EC98 = 0; }

void HermitePools::Update(f32 dt) {
    for (int i = 0; i < count; i++) {
        UpdatePool(&pools[i], dt);
    }
}

void HermitePools::Draw(void) {
    for (int i = 0; i < count; i++) {
        DrawPool(&pools[i]);
    }
}

void HermitePools::Add(HermiteDrone* d) {
    HermitePool* p = Find(d->name);
    if (!p) {
        if (count >= 8) {
            fn_802E317C("path: exceeded pool limit (%d)", 8);
        }
        p = &pools[count];
        InitPool(p, d->name);
        count++;
    }
    if (p->unusedCount + 1 > 8) {
        fn_802E317C("path: exceeded pool_object limit of %d", 8);
    }
    p->unused[p->unusedCount++] = d;
}

void HermitePools::InitPool(HermitePool* p, const char* n) {
    strncpy(p->name, n, 0x10);
    p->name[15] = 0;
    p->unusedCount = 0;
    p->inUseCount = 0;
}

void HermitePools::UpdatePool(HermitePool* p, f32 dt) {
    for (int i = 0; i < p->inUseCount; i++) {
        p->inUse[i]->Update(dt);
    }
}

void HermitePools::DrawPool(HermitePool* p) {
    for (int i = 0; i < p->inUseCount; i++) {
        p->inUse[i]->Draw();
    }
}

LinearPools::LinearPools(void) {
    lbl_8055EC94 = this;
    count = 0;
}

LinearPools::~LinearPools(void) { lbl_8055EC94 = 0; }

void LinearPools::Update(f32 dt) {
    for (int i = 0; i < count; i++) {
        UpdatePool(&pools[i], dt);
    }
}

void LinearPools::Draw(void) {
    for (int i = 0; i < count; i++) {
        DrawPool(&pools[i]);
    }
}

void LinearPools::Add(LinearDrone* d) {
    LinearPool* p = Find(d->name);
    if (!p) {
        if (count >= 8) {
            fn_802E317C("path: exceeded pool limit (%d)", 8);
        }
        p = &pools[count];
        InitPool(p, d->name);
        count++;
    }
    if (p->unusedCount + 1 > 16) {
        fn_802E317C("path: exceeded pool_object limit of %d", 16);
    }
    p->unused[p->unusedCount++] = d;
}

void LinearPools::InitPool(LinearPool* p, const char* n) {
    strncpy(p->name, n, 0x10);
    p->name[15] = 0;
    p->unusedCount = 0;
    p->inUseCount = 0;
}

void LinearPools::UpdatePool(LinearPool* p, f32 dt) {
    for (int i = 0; i < p->inUseCount; i++) {
        p->inUse[i]->Update(dt);
    }
}

void LinearPools::DrawPool(LinearPool* p) {
    for (int i = 0; i < p->inUseCount; i++) {
        p->inUse[i]->Draw();
    }
}

HermiteDroneMan::~HermiteDroneMan(void) {}

LinearDroneMan::~LinearDroneMan(void) {}
