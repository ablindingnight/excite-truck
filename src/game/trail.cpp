// Tyre trails (0x802A1590-0x802A4A50): per-terrain trail systems built from the "trail" child of each
// track "terrain_%d" table (dust, asphalt smoke, grass, sand, water and mud spray), fed every frame
// for each wheel in contact with the ground and by landing events, plus the "bilinear partsys"
// ground marks used by the dust trail. Names are guesses apart from the strings.
#include "game/common.h"

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

struct TVec {
    TVec(void) {}
    TVec(f32 ax, f32 ay, f32 az) {
        x = ax;
        y = ay;
        z = az;
    }
    f32 x, y, z;
};

struct TMtx33 {
    f32 xx, xy, xz;
    f32 yx, yy, yz;
    f32 zx, zy, zz;
};

struct TPos {
    f32 x, y, z;
};

struct TWheel {
    TVec local;  // offset 0x0
    TPos world;  // offset 0xC
    u8 pad18[0x24 - 0x18];
    f32 speed;  // offset 0x24
    f32 slip;   // offset 0x28
    u8 pad2C[0x30 - 0x2C];
    u8 contact;  // offset 0x30
    u8 pad31[0x38 - 0x31];
    s32 surface;  // offset 0x38
};

struct TBody {
    TMtx33 m;  // offset 0x0
    TVec pos;  // offset 0x24
    u8 pad30[0x34 - 0x30];
    TVec vel;  // offset 0x34
    u8 pad40[0x50 - 0x40];
    f32 speed;  // offset 0x50
    u8 pad54[0x70 - 0x54];
    TWheel wheels[4];  // offset 0x70
};

struct TRacer {
    TBody* body;  // offset 0x0
    s32 index;    // offset 0x4
    f32 camDist;  // offset 0x8
    u8 active;    // offset 0xC
};

struct TrailEvent {
    TPos pos;     // offset 0x0
    TVec vel;     // offset 0xC
    TVec dir;     // offset 0x18
    f32 force;    // offset 0x24
    s32 who;      // offset 0x28
    s32 surface;  // offset 0x2C
};

// Particle system description (filled with defaults by fn_8028E61C).
struct PartDesc {
    s32 flags;   // offset 0x0
    s32 max;     // offset 0x4
    f32 f08;     // offset 0x8
    f32 f0C;     // offset 0xC
    TVec v10;    // offset 0x10
    TVec v1C;    // offset 0x1C
    f32 f28;     // offset 0x28
    f32 f2C;     // offset 0x2C
    f32 f30;     // offset 0x30
    f32 f34;     // offset 0x34
    u8 pad38[0x44 - 0x38];
    s32 i44;     // offset 0x44
    f32 f48;     // offset 0x48
    u8 pad4C[0x50 - 0x4C];
    f32 f50;     // offset 0x50
    f32 f54;     // offset 0x54
    f32 f58;     // offset 0x58
    f32 f5C;     // offset 0x5C
    f32 f60;     // offset 0x60
    f32 f64;     // offset 0x64
    f32 f68;     // offset 0x68
    f32 f6C;     // offset 0x6C
    f32 f70;     // offset 0x70
    f32 f74;     // offset 0x74
    f32 f78;     // offset 0x78
    f32 f7C;     // offset 0x7C
    u8 pad80[0x90 - 0x80];
};

// Ground-mark particle system description.
struct PaintDesc {
    s32 max;        // offset 0x0
    f32 life;       // offset 0x4
    f32 rate;       // offset 0x8
    TVec velScale;  // offset 0xC
    TVec jitter;    // offset 0x18
    f32 lift;       // offset 0x24
    f32 drag;       // offset 0x28
    f32 gravity;    // offset 0x2C
    f32 size0;      // offset 0x30
    f32 size1;      // offset 0x34
    f32 size2;      // offset 0x38
    f32 sizeMid;    // offset 0x3C
    f32 alpha0;     // offset 0x40
    f32 alpha1;     // offset 0x44
    f32 alpha2;     // offset 0x48
    f32 alphaMid;   // offset 0x4C
};

struct Mark {
    Mark(void) {}
    TVec pos;  // offset 0x0
    TVec vel;  // offset 0xC
    f32 born;  // offset 0x18
    f32 alpha; // offset 0x1C
};

struct Paint {
    Paint(const PaintDesc* d, const char* tex);
    ~Paint(void);
    void Reset(void);
    void Add(TVec* pos, TVec* vel, f32 alpha);
    void Update(void);
    void Draw(void);

    Mark* marks;     // offset 0x0
    s32 n;           // offset 0x4
    s32 cap;         // offset 0x8
    f32 life;        // offset 0xC
    f32 invLife;     // offset 0x10
    f32 rate;        // offset 0x14
    f32 accum;       // offset 0x18
    TVec velScale;   // offset 0x1C
    TVec jitter;     // offset 0x28
    f32 lift;        // offset 0x34
    f32 gravity;     // offset 0x38
    f32 drag;        // offset 0x3C
    f32 size0;       // offset 0x40
    f32 sizeA;       // offset 0x44
    f32 sizeB;       // offset 0x48
    f32 sizeMid;     // offset 0x4C
    f32 alpha0;      // offset 0x50
    f32 alphaA;      // offset 0x54
    f32 alphaB;      // offset 0x58
    f32 alphaMid;    // offset 0x5C
    s32 tex;         // offset 0x60
    f32 pointSize;   // offset 0x64
};

typedef int (*EventHandler)(const TrailEvent* e);
typedef void (*RacerHook)(TRacer* r, int n);

extern "C" {
void* memset(void* p, int c, u32 n);
int strcasecmp(const char* a, const char* b);
double sin(double x);
double cos(double x);
double log2(double x);
int fn_801345D8(char* buf, int size, const char* fmt, ...);  // snprintf
f32 fn_80129DC4(int seed, f32 lo, f32 hi);                    // random float
}  // extern "C"
f32 Dot(float, float, float, float, float, float);
extern "C" {
f32 fn_801E7E18(void);                                        // time
f32 fn_801E7E24(void);                                        // frame time
void fn_801EEFDC(EventHandler h, u8 type);
void fn_801EF024(EventHandler h);
int fn_80287B14(void);
TBody* fn_80287B24(int i);
void fn_80287B38(RacerHook h);
f32 fn_80287BC8(f32 lo, f32 hi);  // random range
s32 fn_8028E2F8(PartDesc* d, const char* name, const char* tex);
void fn_8028E398(s32 sys);
void fn_8028E584(s32 sys, const TVec* pos, const TVec* vel);
void fn_8028E58C(s32 sys, const TVec* pos, const TVec* vel, f32 a);
void fn_8028E590(s32 sys, const TVec* pos, const TVec* vel, f32 a);
void fn_8028E61C(PartDesc* d);
ValueNode* fn_802AE414(const char* name);  // track effect table lookup
int fn_802BF4F0(void);                     // has water
f32 fn_802BF590(void);                     // water level
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
f32 fn_802C9A4C(f32 size);
s32 fn_802CAC28(const char* name, int flags);
void fn_802CAC54(s32 tex);
int fn_802CE860(int n);
void fn_802CE868(void);
void fn_802CE86C(u32 color, u32 flags, f32 size, f32 x, f32 y, f32 z);
void fn_802E317C(const char* fmt, ...);  // fatal
void PSVECCrossProduct(const TVec* a, const TVec* b, TVec* out);
}

extern u32 lbl_8055D430;
extern f32 lbl_80558DC4[];  // FLT_EPSILON

#define NearZero(x) ((f32)__fabs(x) <= lbl_80558DC4[0])

inline bool DbgFlag(u32 m) {
    int r = 0;
    if ((lbl_8055D430 & 1) && (lbl_8055D430 & m)) {
        r = 1;
    }
    return r;
}

static inline int Full(f32 v) { return v > 1.0f - lbl_80558DC4[0]; }

static inline void Wrap(f32& v, f32 step) {
    if (v > 1.0f - lbl_80558DC4[0]) {
        v = v - 1.0f;
    }
    v = v + step;
}

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void MulVec(TVec* out, const TVec* in, const TMtx33* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m->xx, in->y, m->yx, in->z, m->zx);
        f32 y = Dot3(in->x, m->xy, in->y, m->yy, in->z, m->zy);
        f32 z = Dot3(in->x, m->xz, in->y, m->yz, in->z, m->zz);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m->xx, in->y, m->yx, in->z, m->zx);
        out->y = Dot3(in->x, m->xy, in->y, m->yy, in->z, m->zy);
        out->z = Dot3(in->x, m->xz, in->y, m->yz, in->z, m->zz);
    }
}

// out = a * b
static inline void MulMtx(TMtx33* out, const TMtx33* a, const TMtx33* b) {
    if (out == a || out == b) {
        f32 xx = Dot(b->xx, a->xx, b->yx, a->xy, b->zx, a->xz);
        f32 yx = Dot(b->xx, a->yx, b->yx, a->yy, b->zx, a->yz);
        f32 zx = Dot(b->xx, a->zx, b->yx, a->zy, b->zx, a->zz);
        f32 xy = Dot(b->xy, a->xx, b->yy, a->xy, b->zy, a->xz);
        f32 yy = Dot(b->xy, a->yx, b->yy, a->yy, b->zy, a->yz);
        f32 zy = Dot(b->xy, a->zx, b->yy, a->zy, b->zy, a->zz);
        f32 xz = Dot(b->xz, a->xx, b->yz, a->xy, b->zz, a->xz);
        f32 yz = Dot(b->xz, a->yx, b->yz, a->yy, b->zz, a->yz);
        f32 zz = Dot(b->xz, a->zx, b->yz, a->zy, b->zz, a->zz);
        out->xx = xx;
        out->xy = xy;
        out->xz = xz;
        out->yx = yx;
        out->yy = yy;
        out->yz = yz;
        out->zx = zx;
        out->zy = zy;
        out->zz = zz;
    } else {
        out->xx = Dot(b->xx, a->xx, b->yx, a->xy, b->zx, a->xz);
        out->yx = Dot(b->xx, a->yx, b->yx, a->yy, b->zx, a->yz);
        out->zx = Dot(b->xx, a->zx, b->yx, a->zy, b->zx, a->zz);
        out->xy = Dot(b->xy, a->xx, b->yy, a->xy, b->zy, a->xz);
        out->yy = Dot(b->xy, a->yx, b->yy, a->yy, b->zy, a->yz);
        out->zy = Dot(b->xy, a->zx, b->yy, a->zy, b->zy, a->zz);
        out->xz = Dot(b->xz, a->xx, b->yz, a->xy, b->zz, a->xz);
        out->yz = Dot(b->xz, a->yx, b->yz, a->yy, b->zz, a->yz);
        out->zz = Dot(b->xz, a->zx, b->yz, a->zy, b->zz, a->zz);
    }
}

struct TBasis {
    TVec x, y, z;
};

// out = a * b
static inline void MulBasis(TBasis* out, const TBasis* a, const TBasis* b) {
    if (out == a || out == b) {
        f32 xx = Dot(b->x.x, a->x.x, b->y.x, a->x.y, b->z.x, a->x.z);
        f32 yx = Dot(b->x.x, a->y.x, b->y.x, a->y.y, b->z.x, a->y.z);
        f32 zx = Dot(b->x.x, a->z.x, b->y.x, a->z.y, b->z.x, a->z.z);
        f32 xy = Dot(b->x.y, a->x.x, b->y.y, a->x.y, b->z.y, a->x.z);
        f32 yy = Dot(b->x.y, a->y.x, b->y.y, a->y.y, b->z.y, a->y.z);
        f32 zy = Dot(b->x.y, a->z.x, b->y.y, a->z.y, b->z.y, a->z.z);
        f32 xz = Dot(b->x.z, a->x.x, b->y.z, a->x.y, b->z.z, a->x.z);
        f32 yz = Dot(b->x.z, a->y.x, b->y.z, a->y.y, b->z.z, a->y.z);
        f32 zz = Dot(b->x.z, a->z.x, b->y.z, a->z.y, b->z.z, a->z.z);
        out->x.x = xx;
        out->x.y = xy;
        out->x.z = xz;
        out->y.x = yx;
        out->y.y = yy;
        out->y.z = yz;
        out->z.x = zx;
        out->z.y = zy;
        out->z.z = zz;
    } else {
        out->x.x = Dot(b->x.x, a->x.x, b->y.x, a->x.y, b->z.x, a->x.z);
        out->y.x = Dot(b->x.x, a->y.x, b->y.x, a->y.y, b->z.x, a->y.z);
        out->z.x = Dot(b->x.x, a->z.x, b->y.x, a->z.y, b->z.x, a->z.z);
        out->x.y = Dot(b->x.y, a->x.x, b->y.y, a->x.y, b->z.y, a->x.z);
        out->y.y = Dot(b->x.y, a->y.x, b->y.y, a->y.y, b->z.y, a->y.z);
        out->z.y = Dot(b->x.y, a->z.x, b->y.y, a->z.y, b->z.y, a->z.z);
        out->x.z = Dot(b->x.z, a->x.x, b->y.z, a->x.y, b->z.z, a->x.z);
        out->y.z = Dot(b->x.z, a->y.x, b->y.z, a->y.y, b->z.z, a->y.z);
        out->z.z = Dot(b->x.z, a->z.x, b->y.z, a->z.y, b->z.z, a->z.z);
    }
}

static inline f32 Ramp(f32 x, f32 lo, f32 range) { return (x - lo) / range; }

static inline f32 MaxF(f32 a, f32 b) { return a > b ? a : b; }

static inline f32 Mul(f32 a, f32 b) { return a * b; }

static inline void Scale(TVec* o, const f32* v, f32 k) {
    o->x = v[0] * k;
    o->y = v[1] * k;
    o->z = v[2] * k;
}

class TrailSys {
public:
    virtual ~TrailSys(void);
    virtual u32 GetType(void) = 0;
    virtual void Reset(void);                            // 0x10
    virtual void Update(void);                           // 0x14
    virtual void Draw(void);                             // 0x18
    virtual void Emit(const TRacer* r, int wheel) = 0;         // 0x1C
    virtual void Impact(const TrailEvent* e);            // 0x20
    virtual void Touch(const TrailEvent* e);             // 0x24
};

class VirtualTrail : public TrailSys {
public:
    VirtualTrail(TrailSys* s) { real = s; }
    virtual ~VirtualTrail(void) {}
    virtual u32 GetType(void) { return 0; }
    virtual void Reset(void) { real->Reset(); }
    virtual void Update(void) { real->Update(); }
    virtual void Draw(void) { real->Draw(); }
    virtual void Emit(const TRacer* r, int wheel) { real->Emit(r, wheel); }
    virtual void Impact(const TrailEvent* e) { real->Impact(e); }
    virtual void Touch(const TrailEvent* e) { real->Touch(e); }

    TrailSys* real;  // offset 0x4
};

struct DustState {
    f32 left;    // offset 0x0
    f32 right;   // offset 0x4
    f32 lastHit; // offset 0x8
    f32 touch;   // offset 0xC
};

class DustTrail : public TrailSys {
public:
    DustTrail(ValueNode* cfg);
    virtual ~DustTrail(void);
    virtual u32 GetType(void) { return 1; }
    virtual void Reset(void);
    virtual void Update(void);
    virtual void Draw(void);
    virtual void Emit(const TRacer* r, int wheel);
    virtual void Impact(const TrailEvent* e);
    virtual void Touch(const TrailEvent* e);
    void InitSmall(const char* tex);
    void InitMedium(const char* tex);
    void InitEvent(const char* tex);
    void InitPaint(const char* tex);

    s32 small;         // offset 0x4
    s32 medium;        // offset 0x8
    s32 event;         // offset 0xC
    Paint* paint;      // offset 0x10
    DustState st[8];   // offset 0x14
};

class AsphaltTrail : public TrailSys {
public:
    AsphaltTrail(ValueNode* cfg);
    virtual ~AsphaltTrail(void);
    virtual u32 GetType(void) { return 2; }
    virtual void Emit(const TRacer* r, int wheel);
    void InitSmall(const char* tex);
    void InitLarge(const char* tex);

    s32 small;  // offset 0x4
    s32 large;  // offset 0x8
};

class GrassTrail : public TrailSys {
public:
    GrassTrail(ValueNode* cfg);
    virtual ~GrassTrail(void);
    virtual u32 GetType(void) { return 3; }
    virtual void Emit(const TRacer* r, int wheel);
    void Init(const char* tex);

    s32 sys;  // offset 0x4
};

class SandTrail : public TrailSys {
public:
    SandTrail(ValueNode* cfg);
    virtual ~SandTrail(void);
    virtual u32 GetType(void) { return 4; }
    virtual void Emit(const TRacer* r, int wheel);
    void Init(const char* tex);

    s32 sys;  // offset 0x4
};

class WaterTrail : public TrailSys {
public:
    WaterTrail(ValueNode* cfg);
    virtual ~WaterTrail(void);
    virtual u32 GetType(void) { return 5; }
    virtual void Emit(const TRacer* r, int wheel);
    void Init(const char* tex);

    s32 sys;  // offset 0x4
};

class MudTrail : public TrailSys {
public:
    MudTrail(ValueNode* cfg);
    virtual ~MudTrail(void);
    virtual u32 GetType(void) { return 6; }
    virtual void Reset(void);
    virtual void Emit(const TRacer* r, int wheel);
    virtual void Touch(const TrailEvent* e);
    void Init(const char* tex);

    s32 sys;       // offset 0x4
    f32 touch[8];  // offset 0x8
};

struct TrailType {
    const char* name;
    s32 type;
};

ModuleTag lbl_8055F868;
s32 lbl_8055F870;           // paint trail particles
TrailSys* lbl_804BCF88[8];  // per terrain

static inline void Xform(TVec* p, const TBody* b) {
    MulVec(p, p, &b->m);
    p->x = p->x + b->pos.x;
    p->y = p->y + b->pos.y;
    p->z = p->z + b->pos.z;
}

extern "C" {

TrailSys* fn_802A194C(ValueNode* cfg);
void fn_802A1B20(TRacer* r, int n);
int fn_802A1C30(const TrailEvent* e);
void fn_802A1CC4(void);
void fn_802A1DC0(void);

void fn_802A1590(void) {
    int i;
    memset(lbl_804BCF88, 0, sizeof(lbl_804BCF88));
    for (i = 0; i < 8; i++) {
        char buf[32];
        ValueNode* t;
        fn_801345D8(buf, sizeof(buf), "terrain_%d", i);
        t = fn_802AE414(buf);
        if (t) {
            t = t->FindChild("trail");
        }
        if (t) {
            lbl_804BCF88[i] = fn_802A194C(t);
        }
    }
    fn_80287B38(fn_802A1B20);
    fn_802A1CC4();
    fn_801EEFDC(fn_802A1C30, 5);
}

void fn_802A1660(void) {
    int i;
    fn_801EF024(fn_802A1C30);
    fn_802A1DC0();
    for (i = 0; i < 8; i++) {
        if (lbl_804BCF88[i]) {
            delete lbl_804BCF88[i];
            lbl_804BCF88[i] = 0;
        }
    }
}
}

inline TrailSys::~TrailSys(void) {}

extern "C" void fn_802A172C(void) {
    int i;
    for (i = 0; i < 8; i++) {
        if (lbl_804BCF88[i]) {
            lbl_804BCF88[i]->Reset();
        }
    }
}

void TrailSys::Reset(void) {}

extern "C" void fn_802A1794(void) {
    int i;
    for (i = 0; i < 8; i++) {
        if (lbl_804BCF88[i]) {
            lbl_804BCF88[i]->Update();
        }
    }
}

void TrailSys::Update(void) {}

extern "C" void fn_802A17FC(void) {
    int i;
    for (i = 0; i < 8; i++) {
        if (lbl_804BCF88[i]) {
            lbl_804BCF88[i]->Draw();
        }
    }
}

void TrailSys::Draw(void) {}

extern "C" {

u32 fn_802A1864(const char* name) {
    TrailType types[6] = {
        {"dust", 1}, {"asphalt", 2}, {"grass", 3}, {"sand", 4}, {"water", 5}, {"mud", 6},
    };
    u32 i;
    for (i = 0; i < 6; i++) {
        if (!strcasecmp(name, types[i].name)) {
            return types[i].type;
        }
    }
    fn_802E317C("trail: invalid system type %s", name);
    return 0;
}

TrailSys* fn_802A194C(ValueNode* cfg) {
    u32 type = fn_802A1864(cfg->GetValue("type"));
    int i;
    for (i = 0; i < 8; i++) {
        if (lbl_804BCF88[i] && type == lbl_804BCF88[i]->GetType()) {
            return new ("virtual trail sys") VirtualTrail(lbl_804BCF88[i]);
        }
    }
    switch (type) {
    case 1:
        return new ("dust trail sys") DustTrail(cfg);
    case 2:
        return new ("apshalt trail sys") AsphaltTrail(cfg);
    case 3:
        return new ("grass trail sys") GrassTrail(cfg);
    case 4:
        return new ("sand trail sys") SandTrail(cfg);
    case 5:
        return new ("water trail sys") WaterTrail(cfg);
    case 6:
        return new ("mud trail sys") MudTrail(cfg);
    default:
        fn_802E317C("trail: invalid type %d", type);
        return 0;
    }
}

void fn_802A1B20(TRacer* r, int n) {
    int i;
    if (NearZero(fn_801E7E24())) {
        return;
    }
    for (i = 0; i < n; r++, i++) {
        if (r->active) {
            int w;
            for (w = 0; w < 4; w++) {
                TWheel* wh = &r->body->wheels[w];
                if (wh->contact && wh->surface >= 0 && wh->surface < 8 && lbl_804BCF88[wh->surface]) {
                    if (!fn_802BF4F0() || wh->world.y > fn_802BF590()) {
                        lbl_804BCF88[wh->surface]->Emit(r, w);
                    }
                }
            }
        }
    }
}

int fn_802A1C30(const TrailEvent* e) {
    if (e->surface >= 0 && e->surface < 8 && lbl_804BCF88[e->surface]) {
        TrailSys* s = lbl_804BCF88[e->surface];
        if (e->force > 200.0f) {
            s->Impact(e);
        } else {
            s->Touch(e);
        }
    }
    return sizeof(TrailEvent);
}
}

void TrailSys::Touch(const TrailEvent* e) {}

void TrailSys::Impact(const TrailEvent* e) {}

extern "C" {

void fn_802A1CC4(void) {
    const char* tex = "splash.tex";
    if (DbgFlag(8)) {
        PartDesc d;
        fn_8028E61C(&d);
        d.max = 0x80;
        d.f08 = 0.35f;
        d.f0C = 1.0f;
        d.flags = 0;
        d.v10.x = 1.0f;
        d.v10.y = 1.0f;
        d.v10.z = 1.0f;
        d.v1C.x = 1.0f;
        d.v1C.y = 1.0f;
        d.v1C.z = 1.0f;
        d.f30 = 0.5f;
        d.i44 = 5;
        d.f50 = 1.0f;
        d.f54 = 0.75f;
        d.f58 = -0.25f;
        d.f5C = 0.25f;
        d.f70 = 0.0f;
        d.f68 = 0.0f;
        d.f60 = 0.0f;
        d.f6C = 1.0f;
        d.f64 = 1.0f;
        d.f74 = 0.0f;
        lbl_8055F870 = fn_8028E2F8(&d, "paint trail", tex);
    } else {
        lbl_8055F870 = 0;
    }
}

void fn_802A1DC0(void) {
    if (lbl_8055F870) {
        fn_8028E398(lbl_8055F870);
        lbl_8055F870 = 0;
    }
}
}

DustTrail::DustTrail(ValueNode* cfg) {
    const char* tex = cfg->GetValue("texture");
    InitSmall(tex);
    InitMedium(tex);
    InitEvent(tex);
    InitPaint(tex);
}

DustTrail::~DustTrail(void) {
    delete paint;
    fn_8028E398(event);
    fn_8028E398(medium);
    fn_8028E398(small);
}

void DustTrail::Reset(void) {
    int i;
    for (i = 0; i < fn_80287B14(); i++) {
        st[i].left = 0.0f;
        st[i].right = 0.5f;
        st[i].lastHit = 0.0f;
        st[i].touch = 0.0f;
    }
    paint->Reset();
}

void DustTrail::Update(void) {
    TWheel* wh;
    f32 rate = 0.075f * fn_801E7E24();
    int i;
    for (i = 0; i < fn_80287B14(); i++) {
        TBody* b = fn_80287B24(i);
        f32 slip;
        f32 r;
        if (fn_802BF4F0()) {
            wh = b->wheels;
            int n = 4;
            int w;
            for (w = 0; w < 4; w++, wh++) {
                if (wh->world.y < fn_802BF590() || !wh->contact) {
                    n--;
                }
            }
            if (n == 0) {
                continue;
            }
        }
        slip = 0.0005f * MaxF(b->wheels[2].slip, b->wheels[3].slip);
        r = b->speed * rate;
        r = r > slip ? r : slip;
        if (0.1f < r) {
            r = 0.1f;
        }
        Wrap(st[i].left, r);
        Wrap(st[i].right, r);
    }
    if (!NearZero(fn_801E7E24())) {
        paint->Update();
    }
}

void DustTrail::Draw(void) { paint->Draw(); }

void DustTrail::Emit(const TRacer* r, int wheel) {
    TWheel* wh = &r->body->wheels[wheel];
    int rear;
    f32 a;
    if (r->camDist > 48400.0f || wh->speed * wh->speed < 100.0f) {
        return;
    }
    rear = 1;
    if (wheel != 2 && wheel != 3) {
        rear = 0;
    }
    {
        f32 side = wh->local.x > 0.0f ? 1.0f : -1.0f;
        f32 back = wh->speed > 0.0f ? 1.0f : -1.0f;
        TVec p(wh->local.x, 0.2f + wh->local.y, wh->local.z - 0.4f * back);
        f32 t;
        Xform(&p, r->body);
        t = Ramp(MaxF(r->body->wheels[2].slip, r->body->wheels[3].slip), 0.0f, 2.7f);
        t = 1.0f < t ? 1.0f : t;
        t = t > 0.0f ? t : 0.0f;
        a = 0.25f + Mul(t, 0.75f);
        if (r->camDist < 2500.0f) {
            fn_8028E58C(small, &p, &r->body->vel, a);
            if (rear) {
                fn_8028E58C(medium, &p, &r->body->vel, a);
            }
        }
        if (rear && wh->speed * wh->speed > 900.0f) {
            f32* side = &st[r->index].left;
            if (Full(side[wheel - 2])) {
                paint->Add(&p, &r->body->vel, a);
            }
        }
    }
}

void DustTrail::Impact(const TrailEvent* e) {
    f32 now = fn_801E7E18();
    int n = 4;
    int k = 0.0125f * e->force;
    if (k < 4) {
        n = k;
    }
    if (e->who >= 0 && n > 0 && st[e->who].lastHit + 0.01f < now) {
        TBasis m;
        TBasis rot;
        TVec v;
        f32 h;
        f32 cs;
        f32 sn;
        int i;
        fn_80287B24(e->who);
        h = 0.5f * (f32)__fabs(e->vel.y);
        m.y = e->dir;
        PSVECCrossProduct(&m.y, &TVec(0.0f, 0.0f, 1.0f), &m.x);
        PSVECCrossProduct(&m.x, &m.y, &m.z);
        cs = cos(3.1415927f / n);
        sn = sin(3.1415927f / n);
        rot.x.x = cs;
        rot.x.z = -sn;
        rot.x.y = 0.0f;
        rot.y.x = 0.0f;
        rot.y.y = 1.0f;
        rot.y.z = 0.0f;
        rot.z.x = sn;
        rot.z.y = 0.0f;
        rot.z.z = cs;
        TPos c = e->pos;
        c.y += 0.35f;
        v.x = 0.5f * e->vel.x;
        v.y = 0.0f;
        v.z = 0.5f * e->vel.z;
        for (i = 0; i < n; i++) {
            TVec v1;
            TVec v2;
            TVec q;
            v1.x = v.x + m.x.x * h;
            v1.y = v.y + m.x.y * h;
            v1.z = v.z + m.x.z * h;
            v2.x = v.x + m.x.x * -h;
            v2.y = v.y + m.x.y * -h;
            v2.z = v.z + m.x.z * -h;
            q.x = c.x + m.x.x * 0.15f;
            q.y = c.y + m.x.y * 0.15f;
            q.z = c.z + m.x.z * 0.15f;
            fn_8028E584(event, &q, &v1);
            q.x = c.x + m.x.x / 2.0f;
            q.y = c.y + m.x.y / 2.0f;
            q.z = c.z + m.x.z / 2.0f;
            fn_8028E584(event, &q, &v1);
            q.x = c.x + m.x.x * -0.15f;
            q.y = c.y + m.x.y * -0.15f;
            q.z = c.z + m.x.z * -0.15f;
            fn_8028E584(event, &q, &v2);
            q.x = c.x + m.x.x * -0.5f;
            q.y = c.y + m.x.y * -0.5f;
            q.z = c.z + m.x.z * -0.5f;
            fn_8028E584(event, &q, &v2);
            MulBasis(&m, &rot, &m);
        }
        st[e->who].lastHit = now;
    }
}

void DustTrail::Touch(const TrailEvent* e) {
    if (Full(st[e->who].touch)) {
        fn_8028E584(event, (TVec*)&e->pos, &e->vel);
    }
    Wrap(st[e->who].touch, 0.1f);
}

void DustTrail::InitSmall(const char* tex) {
    PartDesc d;
    fn_8028E61C(&d);
    d.max = 0x200;
    d.f08 = 0.25f;
    d.f0C = 1.0f;
    d.flags = 0;
    d.f48 = 0.9f;
    d.i44 = 4;
    d.v10 = TVec(0.6f, 0.4f, 0.6f);
    d.v1C = TVec(0.9f, 0.5f, 0.9f);
    d.f28 = 1.0f;
    d.f2C = 4.25f;
    d.f30 = 0.0f;
    d.f34 = 0.25f;
    d.f74 = 1.0f;
    d.f6C = 1.0f;
    d.f64 = 1.0f;
    d.f58 = -1.0f;
    d.f5C = 1.0f;
    d.f50 = 2.5f;
    d.f54 = 0.5f;
    d.f78 = -0.2f;
    d.f7C = 0.2f;
    small = fn_8028E2F8(&d, "small trail dust", tex);
}

void DustTrail::InitMedium(const char* tex) {
    PartDesc d;
    fn_8028E61C(&d);
    d.max = 0x200;
    d.f08 = 1.0f;
    d.f0C = 0.25f;
    d.flags = 0;
    d.f48 = 0.9f;
    d.i44 = 4;
    d.v10 = TVec(0.6f, 0.4f, 0.6f);
    d.v1C = TVec(0.9f, 0.5f, 0.9f);
    d.f28 = 2.0f;
    d.f2C = 4.25f;
    d.f30 = 0.0f;
    d.f34 = 0.5f;
    d.f74 = 1.0f;
    d.f6C = 1.0f;
    d.f64 = 1.0f;
    d.f58 = -1.0f;
    d.f5C = 1.0f;
    d.f50 = 6.0f;
    d.f54 = 0.5f;
    d.f78 = -0.2f;
    d.f7C = 0.2f;
    medium = fn_8028E2F8(&d, "medium trail dust", tex);
}

void DustTrail::InitEvent(const char* tex) {
    PartDesc d;
    fn_8028E61C(&d);
    d.max = 0x100;
    d.f08 = 1.0f;
    d.f0C = 1.0f;
    d.flags = 0;
    d.f48 = 0.9f;
    d.i44 = 4;
    d.v10 = TVec(0.6f, 0.4f, 0.6f);
    d.v1C = TVec(1.0f, 1.0f, 1.0f);
    d.f28 = 0.65f;
    d.f30 = 0.0f;
    d.f34 = 0.75f;
    d.f74 = 1.0f;
    d.f6C = 1.0f;
    d.f64 = 1.0f;
    d.f58 = -0.75f;
    d.f5C = 0.75f;
    d.f50 = 1.75f;
    d.f54 = 0.75f;
    d.f78 = -0.2f;
    d.f7C = 0.2f;
    event = fn_8028E2F8(&d, "event dust", tex);
}

void DustTrail::InitPaint(const char* tex) {
    PaintDesc d;
    d.max = 0x834;
    d.life = 3.5f;
    d.rate = 0.8f;
    d.velScale.x = 0.3f;
    d.velScale.y = 0.3f;
    d.velScale.z = 0.3f;
    d.jitter.x = 1.0f;
    d.jitter.y = 0.25f;
    d.jitter.z = 1.0f;
    d.drag = 1.1f;
    d.gravity = 0.0f;
    d.lift = 2.0f;
    d.size0 = 0.5f;
    d.size1 = 3.0f;
    d.size2 = 4.0f;
    d.sizeMid = 0.15f;
    d.alpha0 = 0.25f;
    d.alpha1 = 1.5f;
    d.alpha2 = 0.0f;
    d.alphaMid = 0.3f;
    paint = new ("large dust") Paint(&d, tex);
}

AsphaltTrail::AsphaltTrail(ValueNode* cfg) {
    const char* tex = cfg->GetValue("texture");
    InitSmall(tex);
    InitLarge(tex);
}

AsphaltTrail::~AsphaltTrail(void) {
    fn_8028E398(large);
    fn_8028E398(small);
}

void AsphaltTrail::Emit(const TRacer* r, int wheel) {
    TBody* b = r->body;
    TWheel* wh = &b->wheels[wheel];
    if (r->camDist > 2500.0f || wh->slip < 5.0f) {
        return;
    }
    {
        bool left = wh->local.x < 0.0f;
        TVec* vel = &b->vel;
        TPos p = wh->world;
        f32 s;
        p.y += 0.25f;
        s = 0.02f * wh->speed * (left ? 1 : -1);
        if (wh->slip > 37.5f) {
            fn_8028E590(large, (TVec*)&p, vel, s);
        } else {
            fn_8028E590(small, (TVec*)&p, vel, s);
        }
    }
}

void AsphaltTrail::InitSmall(const char* tex) {
    PartDesc d;
    fn_8028E61C(&d);
    d.max = 0x80;
    d.f08 = 1.0f;
    d.f0C = 0.5f;
    d.flags = 2;
    d.f48 = 0.5f;
    d.i44 = 4;
    d.v10 = TVec(0.9f, 0.4f, 0.9f);
    d.v1C = TVec(0.6f, 0.6f, 0.6f);
    d.f28 = 0.35f;
    d.f2C = 4.5f;
    d.f30 = -0.05f;
    d.f34 = 2.0f;
    d.f70 = 0.1f;
    d.f68 = 0.1f;
    d.f60 = 0.1f;
    d.f74 = 0.9f;
    d.f6C = 0.9f;
    d.f64 = 0.9f;
    d.f58 = -0.8f;
    d.f5C = 0.8f;
    d.f50 = 2.5f;
    d.f54 = 0.5f;
    d.f78 = -0.5f;
    d.f7C = 1.0f;
    small = fn_8028E2F8(&d, "small trail smoke", tex);
}

void AsphaltTrail::InitLarge(const char* tex) {
    PartDesc d;
    fn_8028E61C(&d);
    d.max = 0x40;
    d.f08 = 4.0f;
    d.f0C = 0.34f;
    d.flags = 2;
    d.f48 = 0.5f;
    d.v10 = TVec(0.9f, 0.4f, 0.9f);
    d.v1C = TVec(0.6f, 0.6f, 0.6f);
    d.f28 = 0.6f;
    d.f2C = 1.75f;
    d.f30 = -0.05f;
    d.f34 = 2.0f;
    d.i44 = 4;
    d.f70 = 0.1f;
    d.f68 = 0.1f;
    d.f60 = 0.1f;
    d.f74 = 0.9f;
    d.f6C = 0.9f;
    d.f64 = 0.9f;
    d.f58 = -0.87f;
    d.f5C = 0.87f;
    d.f50 = 7.0f;
    d.f54 = 0.65f;
    d.f78 = -1.0f;
    d.f7C = 1.0f;
    large = fn_8028E2F8(&d, "large trail smoke", tex);
}

GrassTrail::GrassTrail(ValueNode* cfg) { Init(cfg->GetValue("texture")); }

GrassTrail::~GrassTrail(void) { fn_8028E398(sys); }

void GrassTrail::Emit(const TRacer* r, int wheel) {
    TBody* b = r->body;
    TWheel* wh = &b->wheels[wheel];
    if (r->camDist > 2500.0f || wh->slip < 5.0f) {
        return;
    }
    {
        TPos p = wh->world;
        TVec v;
        f32 k;
        Scale(&v, &b->m.yx, 5.5f);
        v.x = v.x + Mul(b->vel.x, 0.8f);
        v.y = v.y + Mul(b->vel.y, 0.8f);
        v.z = v.z + Mul(b->vel.z, 0.8f);
        k = 0.1f * b->speed;
        v.x = v.x + fn_80129DC4(0, -k, k);
        v.z = v.z + fn_80129DC4(0, -k, k);
        fn_8028E590(sys, (TVec*)&p, &v, 5.0f);
    }
}

void GrassTrail::Init(const char* tex) {
    PartDesc d;
    fn_8028E61C(&d);
    d.max = 0x12C;
    d.f08 = 0.8f;
    d.f0C = 0.5f;
    d.flags = 2;
    d.f48 = 0.6f;
    d.i44 = 4;
    d.v10 = TVec(2.5f, 1.7f, 2.5f);
    d.v1C = TVec(1.0f, 0.65f, 1.0f);
    d.f28 = 1.0f;
    d.f2C = 8.0f;
    d.f30 = 0.7f;
    d.f34 = 1.0f;
    d.f74 = 1.0f;
    d.f6C = 1.0f;
    d.f64 = 1.0f;
    d.f58 = -1.5f;
    d.f5C = 1.5f;
    d.f50 = 0.6f;
    d.f54 = 0.4f;
    d.f78 = 0.0f;
    d.f7C = 1.0f;
    sys = fn_8028E2F8(&d, "trail grass", tex);
}

SandTrail::SandTrail(ValueNode* cfg) { Init(cfg->GetValue("texture")); }

SandTrail::~SandTrail(void) { fn_8028E398(sys); }

void SandTrail::Emit(const TRacer* r, int wheel) {
    TWheel* wh = &r->body->wheels[wheel];
    if (r->camDist > 2500.0f || wh->slip < 5.0f && wh->speed < 10.0f) {
        return;
    }
    {
        f32 side = wh->local.x > 0.0f ? 1.0f : -1.0f;
        TVec v;
        f32 k;
        Scale(&v, &r->body->vel.x, 0.8f);
        k = 0.05f * (side * r->body->speed);
        v.x = v.x + r->body->m.xx * k;
        v.y = v.y + r->body->m.xy * k;
        v.z = v.z + r->body->m.xz * k;
        fn_8028E590(sys, (TVec*)&wh->world, &v, 5.0f);
    }
}

void SandTrail::Init(const char* tex) {
    PartDesc d;
    fn_8028E61C(&d);
    d.max = 0xC0;
    d.f08 = 1.2f;
    d.f0C = 0.6f;
    d.flags = 2;
    d.f48 = 0.5f;
    d.i44 = 4;
    d.v10 = TVec(2.2f, 0.75f, 2.2f);
    d.v1C = TVec(1.0f, 1.0f, 1.0f);
    d.f28 = 2.75f;
    d.f2C = 4.25f;
    d.f30 = 0.5f;
    d.f34 = 0.5f;
    d.f74 = 1.0f;
    d.f6C = 1.0f;
    d.f64 = 1.0f;
    d.f58 = -0.8f;
    d.f5C = 0.8f;
    d.f50 = 2.75f;
    d.f54 = 1.0f;
    d.f78 = -0.75f;
    d.f7C = 1.0f;
    sys = fn_8028E2F8(&d, "trail sand", tex);
}

WaterTrail::WaterTrail(ValueNode* cfg) { Init(cfg->GetValue("texture")); }

WaterTrail::~WaterTrail(void) { fn_8028E398(sys); }

void WaterTrail::Emit(const TRacer* r, int wheel) {
    TWheel* wh = &r->body->wheels[wheel];
    if (r->camDist > 2500.0f || wh->speed * wh->speed < 100.0f) {
        return;
    }
    {
        f32 side = wh->local.x > 0.0f ? 1.0f : -1.0f;
        f32 back = wh->speed > 0.0f ? 1.0f : -1.0f;
        TVec p(wh->local.x, 0.25f + wh->local.y, wh->local.z - 0.4f * back);
        TVec v;
        TBody* b;
        f32 k;
        f32 up;
        Xform(&p, r->body);
        b = r->body;
        k = 0.2f + 0.05f * (side * b->speed);
        v.x = b->vel.x + b->m.xx * k;
        v.y = b->vel.y + b->m.xy * k;
        v.z = b->vel.z + b->m.xz * k;
        up = 0.2f + 0.09f * b->speed;
        up = 2.5f < up ? 2.5f : up;
        v.x = v.x + b->m.yx * up;
        v.y = v.y + b->m.yy * up;
        v.z = v.z + b->m.yz * up;
        fn_8028E590(sys, &p, &v, -side * fn_80287BC8(0.0f, 3.1415927f));
    }
}

void WaterTrail::Init(const char* tex) {
    PartDesc d;
    fn_8028E61C(&d);
    d.flags = 2;
    d.max = 0x100;
    d.f08 = 0.5f;
    d.f0C = 0.85f;
    d.v10 = TVec(1.0f, 1.0f, 1.0f);
    d.v1C = TVec(1.0f, 1.0f, 1.0f);
    d.f30 = 0.75f;
    d.f34 = 0.35f;
    d.i44 = 5;
    d.f48 = 0.75f;
    d.f54 = 0.5f;
    d.f50 = 0.2f;
    d.f5C = 1.0f;
    d.f58 = -1.0f;
    d.f74 = 1.0f;
    d.f6C = 1.0f;
    d.f64 = 1.0f;
    d.f7C = 1.0f;
    sys = fn_8028E2F8(&d, "trail water", tex);
}

MudTrail::MudTrail(ValueNode* cfg) { Init(cfg->GetValue("texture")); }

MudTrail::~MudTrail(void) { fn_8028E398(sys); }

void MudTrail::Reset(void) {
    int i;
    for (i = 0; i < fn_80287B14(); i++) {
        touch[i] = 0.0f;
    }
}

void MudTrail::Emit(const TRacer* r, int wheel) {
    TWheel* wh = &r->body->wheels[wheel];
    if (r->camDist > 2500.0f || wh->speed * wh->speed < 100.0f) {
        return;
    }
    {
        f32 side = wh->local.x > 0.0f ? 1.0f : -1.0f;
        f32 back = wh->speed > 0.0f ? 1.0f : -1.0f;
        TVec p(wh->local.x, 0.25f + wh->local.y, wh->local.z - 0.4f * back);
        TVec v;
        TBody* b;
        f32 k;
        f32 up;
        Xform(&p, r->body);
        b = r->body;
        k = 0.2f + 0.05f * (side * b->speed);
        v.x = b->vel.x + b->m.xx * k;
        v.y = b->vel.y + b->m.xy * k;
        v.z = b->vel.z + b->m.xz * k;
        up = 0.2f + 0.09f * b->speed;
        up = 2.5f < up ? 2.5f : up;
        v.x = v.x + b->m.yx * up;
        v.y = v.y + b->m.yy * up;
        v.z = v.z + b->m.yz * up;
        fn_8028E590(sys, &p, &v, -side * fn_80287BC8(0.0f, 3.1415927f));
    }
}

void MudTrail::Touch(const TrailEvent* e) {
    if (Full(touch[e->who])) {
        fn_8028E590(sys, (TVec*)&e->pos, &e->vel, fn_80287BC8(-3.1415927f, 3.1415927f));
    }
    Wrap(touch[e->who], 0.1f);
}

void MudTrail::Init(const char* tex) {
    PartDesc d;
    fn_8028E61C(&d);
    d.flags = 2;
    d.max = 0x100;
    d.f08 = 0.5f;
    d.f0C = 0.85f;
    d.v10 = TVec(1.0f, 1.0f, 1.0f);
    d.v1C = TVec(1.0f, 1.0f, 1.0f);
    d.f30 = 0.75f;
    d.f34 = 0.35f;
    d.i44 = 5;
    d.f48 = 0.75f;
    d.f54 = 0.3f;
    d.f50 = 0.4f;
    d.f5C = 1.0f;
    d.f58 = -1.0f;
    d.f74 = 1.0f;
    d.f6C = 1.0f;
    d.f64 = 1.0f;
    d.f7C = 1.0f;
    sys = fn_8028E2F8(&d, "trail mud", tex);
}

Paint::Paint(const PaintDesc* d, const char* name) {
    marks = new ("bilinear partsys") Mark[d->max];
    cap = d->max;
    life = d->life;
    invLife = 1.0f / d->life;
    rate = d->rate;
    velScale = d->velScale;
    jitter = d->jitter;
    lift = d->lift;
    gravity = 9.81f * d->gravity;
    drag = d->drag;
    size0 = d->size0;
    sizeA = (d->size1 - d->size0) / d->sizeMid;
    sizeB = (d->size2 - d->size1) / (1.0f - d->sizeMid);
    sizeMid = d->sizeMid;
    alpha0 = d->alpha0;
    alphaA = (d->alpha1 - d->alpha0) / d->alphaMid;
    alphaB = (d->alpha2 - d->alpha1) / (1.0f - d->alphaMid);
    alphaMid = d->alphaMid;
    tex = fn_802CAC28(name, 0);
    {
        f32 hi = 0.5f - sizeMid;
        f32 lo;
        hi = 0.0f > hi ? 0.0f : hi;
        lo = sizeMid < 0.5f ? sizeMid : 0.5f;
        pointSize = log2(2.0f / (size0 + sizeA * lo + sizeB * hi));
    }
    Reset();
}

Paint::~Paint(void) {
    fn_802CAC54(tex);
    delete[] marks;
}

void Paint::Reset(void) {
    n = 0;
    accum = 0.0f;
}

void Paint::Add(TVec* pos, TVec* vel, f32 alpha) {
    if (n < cap && Full(accum)) {
        Mark* m = &marks[n];
        {
            // MATCHING-AID: copy through temporaries in this order (plain m->pos = *pos schedules differently).
            f32 x = pos->x;
            f32 y = pos->y;
            m->pos.x = x;
            f32 z = pos->z;
            m->pos.y = y;
            m->pos.z = z;
        }
        m->vel.x = vel->x * velScale.x + fn_80287BC8(-jitter.x, jitter.x);
        m->vel.y = lift + (vel->y * velScale.y + fn_80287BC8(-jitter.y, jitter.y));
        m->vel.z = vel->z * velScale.z + fn_80287BC8(-jitter.z, jitter.z);
        m->born = fn_801E7E18();
        m->alpha = alpha;
        n++;
    }
}

void Paint::Update(void) {
    f32 cutoff = fn_801E7E18() - life;
    f32 dt = fn_801E7E24();
    f32 g = dt * gravity;
    f32 k = 1.0f - drag * dt;
    Mark* out = marks;
    int i;
    for (i = 0; i < n; i++) {
        Mark* m = &marks[i];
        if (m->born > cutoff) {
            TVec v;
            v.x = m->vel.x * k;
            v.y = m->vel.y * k;
            v.z = m->vel.z * k;
            v.y = v.y - g;
            out->pos.x = m->pos.x + v.x * dt;
            out->pos.y = m->pos.y + v.y * dt;
            out->pos.z = m->pos.z + v.z * dt;
            out->vel.x = v.x;
            out->vel.y = v.y;
            out->vel.z = v.z;
            out->born = m->born;
            out->alpha = m->alpha;
            out++;
        }
    }
    n = out - marks;
    Wrap(accum, rate);
}

void Paint::Draw(void) {
    if (n >= 1) {
        f32 old = fn_802C9A4C(pointSize);
        fn_802C8BD0(tex);
        if (fn_802CE860(n)) {
            f32 now = fn_801E7E18();
            int i;
            for (i = 0; i < n; i++) {
                Mark* m = &marks[i];
                f32 age = invLife * (now - m->born);
                f32 hi = age - alphaMid;
                f32 lo;
                int a;
                u32 color;
                hi = 0.0f > hi ? 0.0f : hi;
                lo = alphaMid < age ? alphaMid : age;
                a = 255.0f * (m->alpha * (alpha0 + alphaA * lo + alphaB * hi));
                a = a > 0 ? a : 0;
                a = a < 255 ? a : 255;
                color = (a << 24) | 0xFFFFFF;
                hi = age - sizeMid;
                hi = 0.0f > hi ? 0.0f : hi;
                lo = sizeMid < age ? sizeMid : age;
                fn_802CE86C(color, 0, size0 + sizeA * lo + sizeB * hi, m->pos.x, m->pos.y, m->pos.z);
            }
            fn_802CE868();
        }
        fn_802C8D74();
        fn_802C9A4C(old);
    }
}
