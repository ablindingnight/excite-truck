// Sparks and debris (0x8028A664-0x8028CED8): short and long impact sparks drawn as streaks,
// glass shards, terrain chunks thrown up by skidding or fast wheels, the debris chunk system,
// paint splats and the explosion sequences. Names are guesses apart from the allocation tags,
// file names and value keys.
#include "game/common.h"
#include "macros.h"
#include <revolution/MTX.h>

struct Mat3 {
    f32 m[3][3];
};

struct SparkMtx {
    Vec rot[3];  // offset 0x0
    Vec pos;     // offset 0x24
};

struct SparkWheel {
    Vec pos;     // offset 0x0
    Vec v0C;     // offset 0xC
    Vec v18;     // offset 0x18
    f32 speed;   // offset 0x24
    f32 slip;    // offset 0x28
    u32 flags;   // offset 0x2C
    u8 ground;   // offset 0x30
    f32 size;    // offset 0x34
    s32 terrain; // offset 0x38
};

struct SparkCar {
    Vec axes[3];  // offset 0x0
    Vec pos;      // offset 0x24
    s16 hint;     // offset 0x30
    Vec vel;      // offset 0x34
    u8 pad40[0x70 - 0x40];
    SparkWheel wheels[4];  // offset 0x70
};

struct SparkEntry {
    SparkCar* car;  // offset 0x0
    s32 index;      // offset 0x4
    f32 dist;       // offset 0x8
    u8 active;      // offset 0xC
};

class Chunks {
public:
    virtual ~Chunks(void);
    virtual void Reset(void) = 0;
    virtual void Update(void) = 0;
    virtual void Draw(void) = 0;
    virtual void Spawn(const Vec* pos, const Vec* vel, int slot, int a) = 0;
};

class SparkNode {
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
    virtual void Unk3C(void);
    virtual void Unk40(void);
    virtual void Unk44(void);
    virtual void Unk48(void);
    virtual void Unk4C(void);
    virtual void Unk50(void);
    virtual void Unk54(void);
    virtual void Unk58(void);
    virtual SparkNode* Find(const char* name);
};

class FXSequence {
public:
    static FXSequence* Load(const char* name);
    void Free(void);
    int Play(f32 t, const ValVec3* pos);
};

struct SparkDesc {
    s32 kind;       // offset 0x0
    s32 max;        // offset 0x4
    f32 size0;      // offset 0x8
    f32 size1;      // offset 0xC
    Vec c0;         // offset 0x10
    Vec c1;         // offset 0x1C
    f32 gravity;    // offset 0x28
    f32 unk2C;      // offset 0x2C
    f32 life;       // offset 0x30
    f32 drag;       // offset 0x34
    u8 pad38[0x44 - 0x38];
    s32 frames;     // offset 0x44
    f32 shading;    // offset 0x48
    f32 a4C;        // offset 0x4C
    f32 a50;        // offset 0x50
    f32 a54;        // offset 0x54
    f32 a58;        // offset 0x58
    f32 a5C;        // offset 0x5C
    f32 a60;        // offset 0x60
    f32 a64;        // offset 0x64
    f32 a68;        // offset 0x68
    f32 a6C;        // offset 0x6C
    f32 a70;        // offset 0x70
    f32 a74;        // offset 0x74
    f32 a78;        // offset 0x78
    f32 a7C;        // offset 0x7C
    u8 pad80[0x8C - 0x80];
};

extern "C" {
double fn_8002F208(double x);
int fn_801345D8(char* buf, int size, const char* fmt, ...);  // snprintf
}  // extern "C"
f32 Dot(float, float, float, float, float, float);
extern "C" {
int fn_801E85B4(void);
const SparkMtx* fn_801E8718(int view);
int fn_801E8758(int view);
f32 fn_801E7E18(void);
f32 fn_801E7E24(void);
void fn_801EEFDC(int (*h)(const void* ev), u8 type);
void fn_801EF024(int (*h)(const void* ev));
const SparkCar* fn_80287B24(s32 i);
void fn_80287B38(void (*cb)(SparkEntry* e, int n));
int fn_80287B5C(int n);
int fn_80287B94(int lo, int hi);
f32 fn_80287BC8(f32 lo, f32 hi);
void fn_80288A70(const void* ev, const Vec* v);
void* fn_8028E2F8(SparkDesc* desc, const char* name, const char* tex);
void fn_8028E398(void* e);
void fn_8028E584(void* e, const void* p, const void* v);
void fn_8028E61C(SparkDesc* desc);
Chunks* fn_802929D8(int type, const char* base, int inst, const char* remap);
SparkNode* fn_802AE414(const char* name);
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
int fn_802C8DBC(int n);
void fn_802C8E00(u32 color, int idx, f32 x, f32 y, f32 z, f32 u, f32 v);
void fn_802C8E48(void);
const Vec* fn_802C9A40(void);
f32 fn_802C9A4C(f32 lod);
s32 fn_802CAC28(const char* name, int flags);
void fn_802CAC54(s32 tex);
double sin(double x);
double cos(double x);
}

void* operator new(size_t size, const char* name);
void* operator new[](size_t size, const char* name);

extern f32 lbl_80558DC4[];  // FLT_EPSILON
extern u32 lbl_8055D430;    // debug flags

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

static inline void Scale(Vec* out, const Vec* in, f32 s) {
    out->x = in->x * s;
    out->y = in->y * s;
    out->z = in->z * s;
}

static inline void AddScaled(Vec* out, const Vec* in, const Vec* d, f32 s) {
    out->x = in->x + d->x * s;
    out->y = in->y + d->y * s;
    out->z = in->z + d->z * s;
}

static inline void SetVec(Vec* v, f32 x, f32 y, f32 z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

// out = b * a, through a temporary when out aliases an input.
static inline void MulMat(Mat3* out, const Mat3* a, const Mat3* b) {
    if (out == b || out == a) {
        f32 m00 = Dot(a->m[0][0], b->m[0][0], a->m[1][0], b->m[0][1], a->m[2][0], b->m[0][2]);
        f32 m10 = Dot(a->m[0][0], b->m[1][0], a->m[1][0], b->m[1][1], a->m[2][0], b->m[1][2]);
        f32 m20 = Dot(a->m[0][0], b->m[2][0], a->m[1][0], b->m[2][1], a->m[2][0], b->m[2][2]);
        f32 m01 = Dot(a->m[0][1], b->m[0][0], a->m[1][1], b->m[0][1], a->m[2][1], b->m[0][2]);
        f32 m11 = Dot(a->m[0][1], b->m[1][0], a->m[1][1], b->m[1][1], a->m[2][1], b->m[1][2]);
        f32 m21 = Dot(a->m[0][1], b->m[2][0], a->m[1][1], b->m[2][1], a->m[2][1], b->m[2][2]);
        f32 m02 = Dot(a->m[0][2], b->m[0][0], a->m[1][2], b->m[0][1], a->m[2][2], b->m[0][2]);
        f32 m12 = Dot(a->m[0][2], b->m[1][0], a->m[1][2], b->m[1][1], a->m[2][2], b->m[1][2]);
        f32 m22 = Dot(a->m[0][2], b->m[2][0], a->m[1][2], b->m[2][1], a->m[2][2], b->m[2][2]);
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
        out->m[0][0] = Dot(a->m[0][0], b->m[0][0], a->m[1][0], b->m[0][1], a->m[2][0], b->m[0][2]);
        out->m[1][0] = Dot(a->m[0][0], b->m[1][0], a->m[1][0], b->m[1][1], a->m[2][0], b->m[1][2]);
        out->m[2][0] = Dot(a->m[0][0], b->m[2][0], a->m[1][0], b->m[2][1], a->m[2][0], b->m[2][2]);
        out->m[0][1] = Dot(a->m[0][1], b->m[0][0], a->m[1][1], b->m[0][1], a->m[2][1], b->m[0][2]);
        out->m[1][1] = Dot(a->m[0][1], b->m[1][0], a->m[1][1], b->m[1][1], a->m[2][1], b->m[1][2]);
        out->m[2][1] = Dot(a->m[0][1], b->m[2][0], a->m[1][1], b->m[2][1], a->m[2][1], b->m[2][2]);
        out->m[0][2] = Dot(a->m[0][2], b->m[0][0], a->m[1][2], b->m[0][1], a->m[2][2], b->m[0][2]);
        out->m[1][2] = Dot(a->m[0][2], b->m[1][0], a->m[1][2], b->m[1][1], a->m[2][2], b->m[1][2]);
        out->m[2][2] = Dot(a->m[0][2], b->m[2][0], a->m[1][2], b->m[2][1], a->m[2][2], b->m[2][2]);
    }
}

struct Spark {
    Spark(void);
    Vec pos;  // offset 0x0
    Vec vel;  // offset 0xC
    f32 t;    // offset 0x18
};

struct XVec {
    f32 v[3];
};

struct Explosion {
    f32 t;            // offset 0x0
    XVec pos;         // offset 0x4
    FXSequence* seq;  // offset 0x10
};

struct ExplosionSet {
    Explosion ex[16];  // offset 0x0
    s32 count;         // offset 0x140
};

struct ImpactEv {
    Vec vel;      // offset 0x0
    Vec pos;      // offset 0xC
    Vec v18;      // offset 0x18
    Vec normal;   // offset 0x24
    s32 car;      // offset 0x30
    s32 terrain;  // offset 0x34
};

struct SparkEv {
    Vec vel;  // offset 0x0
    Vec pos;  // offset 0xC
    u8 pad18[0x20 - 0x18];
};

class SparkSys {
public:
    SparkSys(int n, f32 life);
    ~SparkSys(void);
    void Reset(void);
    void Update(void);
    void Draw(void);
    void Emit(int n, const Vec* pos, const Vec* vel, f32 spread);
    void Step(void);
    void DrawAll(const SparkMtx* cam, const SparkMtx* prev);

    Spark* pts;          // offset 0x0
    s32 count;           // offset 0x4
    s32 max;             // offset 0x8
    f32 life;            // offset 0xC
    f32 invLife;         // offset 0x10
    s32 tex;             // offset 0x14
    f32 lod;             // offset 0x18
    SparkMtx views[4];   // offset 0x1C
};

struct Shard {
    Shard(void);
    Vec pos;   // offset 0x0
    Vec vel;   // offset 0xC
    Vec axis;  // offset 0x18
    f32 t;     // offset 0x24
};

class ShardSys {
public:
    ShardSys(int n);
    ~ShardSys(void);
    void Reset(void);
    void Step(void);
    void Draw(void);
    void Emit(int n, const Vec* pos, const Vec* vel, f32 spread);

    Shard* pts;  // offset 0x0
    s32 count;   // offset 0x4
    s32 max;     // offset 0x8
    s32 tex;     // offset 0xC
    f32 lod;     // offset 0x10
};

class SparkStub {
public:
    ~SparkStub(void);
};

struct TerChunk {
    Chunks* sys;    // offset 0x0
    f32 thresh;     // offset 0x4
    s32 skid;       // offset 0x8
    s32 rnd;        // offset 0xC
};

ModuleTag lbl_8055F6E8;
SparkSys* lbl_8055F6F0;    // short sparks
SparkSys* lbl_8055F6F4;    // long sparks
ShardSys* lbl_8055F6F8;    // glass shards
Chunks* lbl_8055F6FC;      // debris
FXSequence* lbl_8055F700;  // explosion sequence
void* lbl_8055F704;        // paint splat emitter
TerChunk lbl_804B3238[8];
ExplosionSet lbl_804B32B8;

extern "C" {

void fn_8028ABE8(void);
void fn_8028AD78(void);
void fn_8028B3CC(void);
void fn_8028B4CC(void);
void fn_8028AB14(void);
int fn_8028B93C(const void* ev);
int fn_8028B500(const void* ev);
int fn_8028BA44(const void* ev);
int fn_8028BA9C(const void* ev);
int fn_8028BB14(const void* ev);

void fn_8028A664(void) {
    lbl_8055F6F0 = new ("short sparks") SparkSys(0x80, 0.5f);
    lbl_8055F6F4 = new ("long sparks") SparkSys(0x60, 1.0f);
    lbl_8055F6F8 = new ("glass shards") ShardSys(0x60);
    lbl_8055F6FC = fn_802929D8(1, "debris", 0, 0);
    lbl_8055F700 = FXSequence::Load("expseq.val");
    lbl_804B32B8.count = 0;
    fn_8028ABE8();
    fn_8028B3CC();
    fn_801EEFDC(fn_8028B93C, 0);
    fn_801EEFDC(fn_8028B500, 1);
    fn_801EEFDC(fn_8028BA44, 2);
    fn_801EEFDC(fn_8028BA9C, 7);
    fn_801EEFDC(fn_8028BB14, 8);
}

void fn_8028A784(void) {
    fn_801EF024(fn_8028BB14);
    fn_801EF024(fn_8028BA9C);
    fn_801EF024(fn_8028BA44);
    fn_801EF024(fn_8028B500);
    fn_801EF024(fn_8028B93C);
    fn_8028B4CC();
    fn_8028AD78();
    if (lbl_8055F700) {
        lbl_8055F700->Free();
    }
    lbl_8055F700 = 0;
    delete lbl_8055F6FC;
    lbl_8055F6FC = 0;
    delete lbl_8055F6F8;
    lbl_8055F6F8 = 0;
    delete lbl_8055F6F0;
    lbl_8055F6F0 = 0;
    delete lbl_8055F6F4;
    lbl_8055F6F4 = 0;
}

}  // extern "C"

SparkStub::~SparkStub(void) {}

extern "C" {

void fn_8028A89C(void) {
    lbl_8055F6F0->Reset();
    lbl_8055F6F4->Reset();
    lbl_8055F6F8->Reset();
    lbl_8055F6FC->Reset();
    for (int i = 0; i < 8; i++) {
        if (lbl_804B3238[i].sys) {
            lbl_804B3238[i].sys->Reset();
        }
    }
}

void fn_8028A92C(void) {
    lbl_8055F6F0->Update();
    lbl_8055F6F4->Update();
    lbl_8055F6F8->Step();
    lbl_8055F6FC->Update();
    for (int i = 0; i < 8; i++) {
        if (lbl_804B3238[i].sys) {
            lbl_804B3238[i].sys->Update();
        }
    }
}

void fn_8028A9BC(void) {
    lbl_8055F6F0->Draw();
    lbl_8055F6F4->Draw();
    lbl_8055F6F8->Draw();
    fn_8028AB14();
}

void fn_8028A9F4(void) {
    lbl_8055F6FC->Draw();
    for (int i = 0; i < 8; i++) {
        if (lbl_804B3238[i].sys) {
            lbl_804B3238[i].sys->Draw();
        }
    }
}

void fn_8028AA6C(const Vec* pos, FXSequence* seq, f32 scale) {
    if (seq && (u32)lbl_804B32B8.count < 16) {
        lbl_804B32B8.ex[lbl_804B32B8.count].t = fn_801E7E18();
        *(ValVec3*)&lbl_804B32B8.ex[lbl_804B32B8.count].pos = *(const ValVec3*)pos;
        lbl_804B32B8.ex[lbl_804B32B8.count].seq = seq;
        lbl_804B32B8.count++;
    }
}

void fn_8028AB14(void) {
    f32 now = fn_801E7E18();
    Explosion* e = lbl_804B32B8.ex;
    for (int i = 0; i < lbl_804B32B8.count;) {
        if (!e->seq->Play(now - e->t, (const ValVec3*)&e->pos)) {
            Explosion* last = &lbl_804B32B8.ex[--lbl_804B32B8.count];
            e->t = last->t;
            e->pos = last->pos;
            e->seq = last->seq;
        } else {
            e++;
            i++;
        }
    }
}

void fn_8028ADF4(SparkEntry* e, int n);

void fn_8028ABE8(void) {
    TerChunk* tc = lbl_804B3238;
    int n = 0;
    for (int i = 0; i < 8; i++, tc++) {
        char buf[32];
        fn_801345D8(buf, 32, "terrain_%d", i);
        SparkNode* node = fn_802AE414(buf);
        if (node) {
            node = node->Find("chunk");
        }
        if (node) {
            tc->sys = fn_802929D8(2, "terchunk", n, node->GetString("texture", ""));
            tc->thresh = node->GetFloat("skid_thresh", 2.5f);
            int k = node->GetInt("skid_chance", 20);
            k = MAX(k, 0);
            tc->skid = MIN(k, 100);
            k = node->GetInt("random_chance", 2);
            k = MAX(k, 0);
            tc->rnd = MIN(k, 100);
            n++;
        } else {
            tc->sys = 0;
        }
    }
    fn_80287B38(fn_8028ADF4);
}

void fn_8028AD78(void) {
    TerChunk* tc = lbl_804B3238;
    for (int i = 0; i < 8; i++, tc++) {
        if (tc->sys) {
            delete tc->sys;
            tc->sys = 0;
        }
    }
}

void fn_8028ADF4(SparkEntry* e, int n) {
    if ((f32)__fabs(fn_801E7E24()) <= lbl_80558DC4[0]) {
        return;
    }
    for (int i = 0; i < n; i++, e++) {
        const SparkCar* car = e->car;
        if (!e->active || e->dist > 2500.0f) {
            continue;
        }
        const SparkWheel* w = car->wheels;
        for (int j = 0; j < 4; j++, w++) {
            if (!w->ground || w->terrain < 0 || w->terrain >= 8) {
                continue;
            }
            TerChunk* tc = &lbl_804B3238[w->terrain];
            if (!tc->sys || (w->flags & 8)) {
                continue;
            }
            int r = fn_80287B5C(100);
            if (w->slip > tc->thresh && r < tc->skid) {
                Vec p = w->v0C;
                Vec d;
                Vec v;
                Scale(&d, &car->axes[1], fn_80287BC8(0.2f, 1.0f));
                AddScaled(&d, &d, &car->axes[2], fn_80287BC8(-0.2f, -1.0f));
                PSVECNormalize(&d, &d);
                Scale(&d, &d, w->size);
                p.x = p.x + d.x;
                p.y = p.y + d.y;
                p.z = p.z + d.z;
                Scale(&v, &car->vel, 0.87f);
                v.y += 3.0f;
                AddScaled(&v, &v, &car->axes[0], w->pos.x > 0.0f ? 1.0f : -1.0f);
                f32 s = 0.033333335f * w->speed;
                v.x = v.x + fn_80287BC8(-s, s);
                v.z = v.z + fn_80287BC8(-s, s);
                tc->sys->Spawn(&p, &v, car->hint, i);
            } else if (w->speed > 25.0f && r < tc->rnd) {
                Vec p;
                Vec v;
                p.x = w->pos.x;
                p.y = w->pos.y;
                p.z = w->pos.z - 0.2f;
                MulVec(&p, &p, car->axes);
                p.x = p.x + car->pos.x;
                p.y = p.y + car->pos.y;
                p.z = p.z + car->pos.z;
                Scale(&v, &car->axes[2], -1.0f);
                AddScaled(&v, &v, &car->axes[1], fn_80287BC8(0.5f, 1.0f));
                Scale(&v, &v, w->speed * fn_80287BC8(0.1f, 0.65f));
                AddScaled(&v, &v, &car->vel, 0.65f);
                AddScaled(&v, &v, &car->axes[0], w->pos.x > 0.0f ? 1.0f : -1.0f);
                f32 s = 0.033333335f * w->speed;
                v.x = v.x + fn_80287BC8(-s, s);
                v.z = v.z + fn_80287BC8(-s, s);
                tc->sys->Spawn(&p, &v, car->hint, i);
            }
        }
    }
}

static inline int PaintOn(void) {
    int on = 0;
    if ((lbl_8055D430 & 1) && (lbl_8055D430 & 8)) {
        on = 1;
    }
    return on;
}

void fn_8028B3CC(void) {
    const char* tex = "splash.tex";
    if (PaintOn()) {
        SparkDesc e;
        fn_8028E61C(&e);
        e.max = 0x80;
        e.size0 = 0.5f;
        e.size1 = 1.0f;
        e.kind = 0;
        e.c0.x = 5.0f;
        e.c0.y = 5.0f;
        e.c0.z = 5.0f;
        e.c1.x = 0.35f;
        e.c1.y = 0.35f;
        e.c1.z = 0.35f;
        e.life = 0.5f;
        e.frames = 5;
        e.a50 = 2.5f;
        e.a54 = 1.0f;
        e.a58 = -1.5f;
        e.a5C = 1.5f;
        e.a70 = 0.0f;
        e.a68 = 0.0f;
        e.a60 = 0.0f;
        e.a6C = 1.0f;
        e.a64 = 1.0f;
        e.a74 = 0.0f;
        lbl_8055F704 = fn_8028E2F8(&e, "paint splat", tex);
    } else {
        lbl_8055F704 = 0;
    }
}

void fn_8028B4CC(void) {
    if (lbl_8055F704) {
        fn_8028E398(lbl_8055F704);
        lbl_8055F704 = 0;
    }
}

int fn_8028B500(const void* evp) {
    const ImpactEv* ev = (const ImpactEv*)evp;
    f32 mag = PSVECMag(&ev->v18);
    int n = MIN(MAX((int)(0.002f * mag), 0), 0x60);
    s16 hint = fn_80287B24(ev->car)->hint;
    if (n > 0) {
        f32 k = 0.0003f * mag;
        f32 lift = MIN(7.0f, k);
        lift = MAX(lift, 3.0f);
        f32 spread = MIN(k, 5.0f);
        f32 cap = 0.0f;
        if (ev->terrain == 0x6E || ev->terrain < 8) {
            cap = 100.0f;
        }
        f32 zero = 0.0f;
        for (int i = 0; i < n; i++) {
            Vec d;
            Scale(&d, &ev->vel, fn_80287BC8(-0.25f, 1.0f));
            d.x = d.x + fn_80287BC8(-spread, spread);
            d.y = d.y + (lift + fn_80287BC8(-2.5f, 2.5f));
            d.z = d.z + fn_80287BC8(-spread, spread);
            f32 dot = PSVECDotProduct(&ev->normal, &d);
            f32 s = -dot + fn_80287BC8(-1.5f, 3.0f);
            s = MIN(cap, s);
            s = MAX(s, zero);
            AddScaled(&d, &d, &ev->normal, s);
            lbl_8055F6FC->Spawn(&ev->pos, &d, hint, ev->car);
        }
        int m = 1.5f * n;
        k = MIN(k, 8.0f);
        Vec v;
        v.x = ev->vel.x * 0.25f;
        v.y = ev->vel.y * 0.25f + 0.25f * k;
        v.z = ev->vel.z * 0.25f;
        lbl_8055F6F0->Emit(m, &ev->pos, &v, k);
        lbl_8055F6F4->Emit(m, &ev->pos, &v, k);
    }
    Chunks* sys;
    if (ev->terrain >= 0 && ev->terrain < 8 && (sys = lbl_804B3238[ev->terrain].sys) != 0) {
        f32 spread = MIN(0.0003f * mag, 6.0f);
        int m = n / 3;
        f32 zero = 0.0f;
        for (int i = 0; i < m; i++) {
            Vec d;
            Scale(&d, &ev->vel, fn_80287BC8(-0.25f, 1.0f));
            d.x = d.x + fn_80287BC8(-spread, spread);
            d.y = d.y + fn_80287BC8(-2.5f, 2.5f);
            d.z = d.z + fn_80287BC8(-spread, spread);
            f32 s = -PSVECDotProduct(&ev->normal, &d);
            s = MAX(zero, s);
            AddScaled(&d, &d, &ev->normal, s);
            sys->Spawn(&ev->pos, &d, hint, ev->car);
        }
    }
    return 0x38;
}

int fn_8028B93C(const void* evp) {
    const SparkEv* ev = (const SparkEv*)evp;
    Vec v;
    Vec p;
    f32 s;
    int i;
    Scale(&v, &ev->vel, 0.5f);
    s = 0.05f * PSVECMag(&v);
    for (i = 0; i < 2; i++) {
        SetVec(&p, ev->pos.x + fn_80287BC8(-0.15f, 0.15f), ev->pos.y + fn_80287BC8(-0.15f, 0.15f),
               ev->pos.z + fn_80287BC8(-0.15f, 0.15f));
        lbl_8055F6F0->Emit(1, &p, &v, s);
        lbl_8055F6F4->Emit(1, &p, &v, s);
    }
    return 0x20;
}

int fn_8028BA44(const void* evp) {
    const SparkEv* ev = (const SparkEv*)evp;
    Vec v;
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = 0.0f;
    fn_80288A70(ev, &v);
    fn_8028AA6C(&ev->vel, lbl_8055F700, ev->pos.x);
    return 0x14;
}

int fn_8028BA9C(const void* evp) {
    const SparkEv* ev = (const SparkEv*)evp;
    Vec v;
    Scale(&v, &ev->pos, 0.05f);
    lbl_8055F6F8->Emit(fn_80287B94(0x18, 0x30), &ev->vel, &v, 6.0f);
    return 0x1C;
}

int fn_8028BB14(const void* evp) {
    const SparkEv* ev = (const SparkEv*)evp;
    for (int i = 0; i < 20; i++) {
        fn_8028E584(lbl_8055F704, &ev->vel, &ev->pos);
    }
    return 0x18;
}

}  // extern "C"

SparkSys::SparkSys(int n, f32 l) {
    pts = new ("sparks") Spark[n];
    count = 0;
    max = n;
    life = l;
    invLife = 1.0f / l;
    tex = fn_802CAC28("spark.tex", 0);
    lod = fn_8002F208(13.333333333333334);
}

Spark::Spark(void) {}

SparkSys::~SparkSys(void) {
    fn_802CAC54(tex);
    delete[] pts;
}

void SparkSys::Reset(void) {
    count = 0;
    for (int i = 0; i < 4; i++) {
        views[i].rot[0].x = 1.0f;
        views[i].rot[0].y = 0.0f;
        views[i].rot[0].z = 0.0f;
        views[i].rot[1].x = 0.0f;
        views[i].rot[1].y = 1.0f;
        views[i].rot[1].z = 0.0f;
        views[i].rot[2].x = 0.0f;
        views[i].rot[2].y = 0.0f;
        views[i].rot[2].z = 1.0f;
        views[i].pos.x = 0.0f;
        views[i].pos.y = 0.0f;
        views[i].pos.z = 0.0f;
    }
}

void SparkSys::Update(void) { Step(); }

void SparkSys::Draw(void) {
    int view = fn_801E85B4();
    SparkMtx cam = *fn_801E8718(view);
    if (count > 0) {
        f32 old = fn_802C9A4C(lod);
        fn_802C8BD0(tex);
        if (!fn_801E8758(view)) {
            DrawAll(&cam, &views[view]);
        } else {
            DrawAll(&cam, &cam);
        }
        fn_802C8D74();
        fn_802C9A4C(old);
    }
    views[view] = cam;
}

void SparkSys::Emit(int n, const Vec* pos, const Vec* vel, f32 spread) {
    f32 now = fn_801E7E18();
    int room = max - count;
    int m = MAX(n, 0);
    int k = MIN(m, room);
    for (int i = 0; i < k; i++) {
        Spark* s = &pts[count + i];
        s->pos.x = pos->x;
        s->pos.y = pos->y;
        s->pos.z = pos->z;
        SetVec(&s->vel, vel->x + fn_80287BC8(-spread, spread), vel->y + fn_80287BC8(-spread, spread),
               vel->z + fn_80287BC8(-spread, spread));
        s->t = now;
    }
    count += k;
}

void SparkSys::Step(void) {
    f32 age = fn_801E7E18() - life;
    f32 dt = fn_801E7E24();
    f32 g = 9.81f * dt;
    f32 down = 0.2f * g;
    f32 drag = 1.0f - (0.5f * dt);
    Spark* out = pts;
    for (int i = 0; i < count; i++) {
        Spark* s = &pts[i];
        if (s->t > age) {
            Vec v;
            v.x = s->vel.x * drag;
            v.y = s->vel.y * drag - down;
            v.z = s->vel.z * drag;
            out->pos.x = s->pos.x + v.x * dt;
            out->pos.y = s->pos.y + v.y * dt;
            out->pos.z = s->pos.z + v.z * dt;
            out->vel = v;
            out->t = s->t;
            out++;
        }
    }
    count = out - pts;
}

void SparkSys::DrawAll(const SparkMtx* cam, const SparkMtx* prev) {
    if (!fn_802C8DBC(count * 4)) {
        return;
    }
    f32 now = fn_801E7E18();
    f32 dt = fn_801E7E24();
    Mat3 m = *(const Mat3*)prev;
    f32 t;
    t = m.m[0][1];
    m.m[0][1] = m.m[1][0];
    m.m[1][0] = t;
    t = m.m[0][2];
    m.m[0][2] = m.m[2][0];
    m.m[2][0] = t;
    t = m.m[1][2];
    m.m[1][2] = m.m[2][1];
    m.m[2][1] = t;
    MulMat(&m, &m, (const Mat3*)cam);
    f32 back = 5.0f * -dt;
    for (int i = 0; i < count; i++) {
        Spark* s = &pts[i];
        Vec p;
        Vec d;
        p.x = s->pos.x + s->vel.x * back - prev->pos.x;
        p.y = s->pos.y + s->vel.y * back - prev->pos.y;
        p.z = s->pos.z + s->vel.z * back - prev->pos.z;
        f32 a = invLife * (now - s->t);
        u32 color = (int)(255.0f * (1.0f - a * a)) << 24 | 0xFFFFFF;
        MulVec(&p, &p, (const Vec*)&m);
        p.x = p.x + cam->pos.x;
        p.y = p.y + cam->pos.y;
        p.z = p.z + cam->pos.z;
        d.x = p.x - s->pos.x;
        d.y = p.y - s->pos.y;
        d.z = p.z - s->pos.z;
        f32 sign = PSVECDotProduct(&cam->rot[1], &d) > 0.0f ? 1.0f : -1.0f;
        Vec q[4];
        AddScaled(&q[0], &s->pos, &cam->rot[0], -0.0375f);
        AddScaled(&q[1], &s->pos, &cam->rot[0], 0.075f);
        AddScaled(&q[2], &p, &cam->rot[0], -0.0375f * sign);
        AddScaled(&q[3], &p, &cam->rot[0], 0.075f * sign);
        fn_802C8E00(color, 0, q[0].x, q[0].y, q[0].z, 0.0f, 1.0f);
        fn_802C8E00(color, 0, q[1].x, q[1].y, q[1].z, 1.0f, 1.0f);
        fn_802C8E00(color, 1, q[2].x, q[2].y, q[2].z, 0.0f, 0.0f);
        fn_802C8E00(color, 1, q[3].x, q[3].y, q[3].z, 1.0f, 0.0f);
    }
    fn_802C8E48();
}

ShardSys::ShardSys(int n) {
    pts = new ("shard tab") Shard[n];
    count = 0;
    max = n;
    tex = fn_802CAC28("shard.tex", 0);
    lod = fn_8002F208(10.0);
}

Shard::Shard(void) {}

ShardSys::~ShardSys(void) {
    fn_802CAC54(tex);
    delete[] pts;
}

void ShardSys::Reset(void) { count = 0; }

void ShardSys::Step(void) {
    f32 age = fn_801E7E18() - 2.0f;
    f32 dt = fn_801E7E24();
    f32 down = 0.75f * (9.81f * dt);
    Shard* out = pts;
    f32 drag = 1.0f;
    for (int i = 0; i < count; i++) {
        Shard* s = &pts[i];
        if (s->t > age) {
            Vec v;
            v.x = s->vel.x * drag;
            v.y = s->vel.y * drag - down;
            v.z = s->vel.z * drag;
            out->pos.x = s->pos.x + v.x * dt;
            out->pos.y = s->pos.y + v.y * dt;
            out->pos.z = s->pos.z + v.z * dt;
            out->vel = v;
            out->axis = s->axis;
            out->t = s->t;
            out++;
        }
    }
    count = out - pts;
}

void ShardSys::Draw(void) {
    if (count < 1) {
        return;
    }
    f32 old = fn_802C9A4C(lod);
    fn_802C8BD0(tex);
    if (fn_802C8DBC(count * 3)) {
        Vec light = *fn_802C9A40();
        f32 now = fn_801E7E18();
        for (int i = 0; i < count; i++) {
            Shard* s = &pts[i];
            Vec a;
            Mat3 r;
            Scale(&a, &s->axis, now - s->t);
            f32 ang = PSVECMag(&a);
            if (ang > 0.0001f) {
                f32 inv = 1.0f / ang;
                f32 x = inv * a.x;
                f32 y = inv * a.y;
                f32 z = inv * a.z;
                f32 sn = sin(ang);
                f32 cs = cos(ang);
                f32 c1 = 1.0f - cs;
                r.m[0][0] = cs + c1 * (x * x);
                r.m[0][1] = y * (c1 * x) + sn * z;
                r.m[0][2] = z * (c1 * x) - sn * y;
                r.m[1][0] = y * (c1 * x) - sn * z;
                r.m[1][1] = cs + c1 * (y * y);
                r.m[1][2] = z * (c1 * y) + sn * x;
                r.m[2][0] = z * (c1 * x) + sn * y;
                r.m[2][1] = z * (c1 * y) - sn * x;
                r.m[2][2] = cs + c1 * (z * z);
            } else {
                r.m[0][0] = 1.0f;
                r.m[0][1] = 0.0f;
                r.m[0][2] = 0.0f;
                r.m[1][0] = 0.0f;
                r.m[1][1] = 1.0f;
                r.m[1][2] = 0.0f;
                r.m[2][0] = 0.0f;
                r.m[2][1] = 0.0f;
                r.m[2][2] = 1.0f;
            }
            f32 d = PSVECDotProduct((const Vec*)r.m[1], &light);
            int al = (int)(192.0f * (d * d));
            al = MAX(al, 0);
            u32 color = MIN(al, 0xFF) << 24 | 0xFFFFFF;
            Vec p;
            fn_802C8E00(color, 0, s->pos.x, s->pos.y, s->pos.z, 0.0f, 1.0f);
            AddScaled(&p, &s->pos, (const Vec*)r.m[0], 0.1f);
            fn_802C8E00(color, 0, p.x, p.y, p.z, 1.0f, 1.0f);
            AddScaled(&p, &s->pos, (const Vec*)r.m[2], 0.1f);
            fn_802C8E00(color, 1, p.x, p.y, p.z, 0.0f, 0.0f);
        }
        fn_802C8E48();
    }
    fn_802C8D74();
    fn_802C9A4C(old);
}

void ShardSys::Emit(int n, const Vec* pos, const Vec* vel, f32 spread) {
    f32 now = fn_801E7E18();
    int room = max - count;
    int m = MAX(n, 0);
    int k = MIN(m, room);
    for (int i = 0; i < k; i++) {
        Shard* s = &pts[count + i];
        s->pos.x = pos->x;
        s->pos.y = pos->y;
        s->pos.z = pos->z;
        SetVec(&s->vel, vel->x + fn_80287BC8(-spread, spread), vel->y + fn_80287BC8(-spread, spread),
               vel->z + fn_80287BC8(-spread, spread));
        SetVec(&s->axis, fn_80287BC8(-12.566371f, 12.566371f), fn_80287BC8(-12.566371f, 12.566371f),
               fn_80287BC8(-12.566371f, 12.566371f));
        s->t = now;
    }
    count += k;
}
