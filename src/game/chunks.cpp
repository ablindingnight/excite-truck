// Chunk systems (0x802924E8-0x80294970): debris pieces thrown off cars, terrain and wood. Systems
// are created by name from the track effect tables ("type", "model_base", "instance",
// "remap_texture"), shared through a reference-counted bucket, and each keeps a pool of chunks that
// tumble with one of six random spins, fall, bounce once on the ground and sink away. Car chunks are
// tinted with a dimmed copy of each car's colour. Names are guesses apart from the strings.
#include "game/common.h"

void* operator new(size_t size, const char* tag);

extern "C" {
void* memset(void* p, int c, u32 n);
int strcmp(const char* a, const char* b);
int strncasecmp(const char* a, const char* b, u32 n);
double sqrt(double x);
double sin(double x);
double cos(double x);
}

static inline void StrNCopy(char* dst, const char* src, int len) {
    char* d = dst;
    int n = len;
    const char* s = src;
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

struct PVec {
    f32 x, y, z;
};

struct Mat3 {
    PVec x, y, z;
};

// A random tumble: an axis, a speed and this frame's rotation (0x34).
struct Spin {
    Spin(void) {}
    PVec axis;  // offset 0x0
    f32 speed;  // offset 0xC
    Mat3 rot;   // offset 0x10
};

// A car or terrain chunk (0x4C).
struct Chunk {
    Mat3 rot;     // offset 0x0
    PVec pos;     // offset 0x24
    PVec vel;     // offset 0x30
    f32 age;      // offset 0x3C
    s16 slot;     // offset 0x40
    s32 state;    // offset 0x44
    u8 model;     // offset 0x48
    u8 spin;      // offset 0x49
    u8 color;     // offset 0x4A
};

// A wood chunk (0x44).
struct WoodChunk {
    Mat3 rot;   // offset 0x0
    PVec pos;   // offset 0x24
    PVec vel;   // offset 0x30
    f32 age;    // offset 0x3C
    u8 model;   // offset 0x40
    u8 spin;    // offset 0x41
};

class Chunks {
public:
    virtual ~Chunks(void) {}
    virtual void Reset(void) = 0;
    virtual void Update(void) = 0;
    virtual void Draw(void) = 0;
    virtual void Spawn(const PVec* pos, const PVec* vel, int slot, int a) = 0;
};

struct CarColor {
    u32 dim;   // offset 0x0
    u32 full;  // offset 0x4
};

class CarChunks : public Chunks {
public:
    CarChunks(ValueNode* vn, const char* base);
    virtual ~CarChunks(void);
    virtual void Reset(void);
    virtual void Update(void);
    virtual void Draw(void);
    virtual void Spawn(const PVec* pos, const PVec* vel, int slot, int color);
    void LoadModels(const char* base);
    void FreeModels(void);
    void InitColors(void);
    void Step(void);

    f32 grav;               // offset 0x4
    s32 models[8];          // offset 0x8
    s32 numModels;          // offset 0x28
    Chunk chunks[160];      // offset 0x2C
    s32 count;              // offset 0x2FAC
    CarColor colors[8];     // offset 0x2FB0
};

class TerrainChunks : public Chunks {
public:
    TerrainChunks(ValueNode* vn, const char* base, int inst, const char* remap);
    virtual ~TerrainChunks(void);
    virtual void Reset(void);
    virtual void Update(void);
    virtual void Draw(void);
    virtual void Spawn(const PVec* pos, const PVec* vel, int slot, int a);
    void LoadModels(const char* base, int inst, const char* remap);
    void FreeModels(void);
    void Step(void);

    f32 grav;               // offset 0x4
    s32 models[8];          // offset 0x8
    s32 numModels;          // offset 0x28
    Chunk chunks[256];      // offset 0x2C
    s32 count;              // offset 0x4C2C
};

class WoodChunks : public Chunks {
public:
    WoodChunks(ValueNode* vn, const char* base, int inst, const char* remap);
    virtual ~WoodChunks(void);
    virtual void Reset(void);
    virtual void Update(void);
    virtual void Draw(void);
    virtual void Spawn(const PVec* pos, const PVec* vel, int slot, int a);
    void LoadModels(const char* base, int inst, const char* remap);
    void FreeModels(void);
    void Step(void);

    f32 grav;                // offset 0x4
    s32 models[8];           // offset 0x8
    s32 numModels;           // offset 0x28
    WoodChunk chunks[128];   // offset 0x2C
    s32 count;               // offset 0x222C
};

struct BucketEntry {
    char name[0x10];  // offset 0x0
    Chunks* sys;      // offset 0x10
    s32 refs;         // offset 0x14
};

struct Bucket {
    Bucket(void) { count = 0; }
    void Clear(void);
    void Reset(void);
    int Find(const char* name, int* idx);
    void Add(Chunks* sys, const char* name);
    int Release(Chunks* sys);

    BucketEntry entries[16];  // offset 0x0
    s32 count;                // offset 0x180
};

struct TypeName {
    const char* name;
    s32 type;
};

struct TypeTable {
    TypeName t[3];
};

struct CarInfo {
    u8 pad0[0x3EC];
    u32 color;  // offset 0x3EC
};

struct ModelInfo {
    s32 count;   // offset 0x0
    u8* texs;    // offset 0x4
};

// A texture remap request (0x2C).
struct TexRemap {
    char name[0x10];     // offset 0x0
    s32 id;              // offset 0x10
    char newName[0x10];  // offset 0x14
    u8 pad24[8];
};

extern "C" {
f32 fn_80129DC4(int seed, f32 lo, f32 hi);
int fn_80129D18(int seed, int range);
f32 fn_801E7E18(void);
f32 fn_801E7E24(void);
f32 PSVECMag(const PVec* v);
f32 PSVECDotProduct(const PVec* a, const PVec* b);
void fn_802E317C(const char* fmt, ...);
ValueNode* fn_802AE414(const char* name);
ModelInfo* fn_802C6B44(const char* model);
void fn_802C6C20(ModelInfo* m);
const char* fn_802C9C24(const u8* tex);
void fn_802C7888(TexRemap* r, int n);
int fn_801345D8(char* buf, int size, const char* fmt, ...);
int fn_8012C33C(const char* name);
s32 fn_802C6FC0(const char* model, int a, int b);
void fn_802C6CF8(s32 model);
u32 fn_802C9950(u32 color);
u32 fn_802C9994(u32 color);
void fn_802C744C(s32 model, const Chunk* c);
int fn_80287B5C(int n);
void fn_801E9DD4(const char* a, const char* b);
void fn_801E9DD8(void);
int fn_801E9E78(s16* slot, PVec* hit, PVec* n, s32* id);
}

extern f32 lbl_80558DC4[];
extern CarInfo* lbl_80488648[8];

const char* lbl_8055C958 = "Andrew";
const char* lbl_8055C964 = "chunk";

static const TypeTable sTypes = {{{"car", 1}, {"terrain", 2}, {"wood", 3}}};

ModuleTag lbl_8055F760;
Spin lbl_804B4030[6];
Bucket lbl_804B4168;

static inline f32 Rand(f32 lo, f32 hi) { return fn_80129DC4(0, lo, hi); }

extern "C" {

void fn_802924E8(void) { lbl_804B4168.Clear(); }

void fn_802924F4(void) {
    Spin* s = lbl_804B4030;
    for (int i = 0; i < 6; i++, s++) {
        f32 x = Rand(-1.0f, 1.0f);
        f32 r = sqrt(1.0f - x * x - 0.0f);
        f32 y = Rand(-(f32)sqrt(1.0f - x * x - 0.0f), r);
        f32 z = sqrt(1.0f - x * x - y * y);
        f32 sign = Rand(-1.0f, 1.0f) > 0.0f ? 1.0f : -1.0f;
        PVec a;
        a.x = x;
        a.y = y;
        a.z = z * sign;
        s->axis = a;
        s->speed = Rand(3.0f, 10.0f);
    }
}

void fn_80292618(void) {}

void fn_8029261C(void) { lbl_804B4168.Reset(); }

void fn_80292628(void) {
    for (int i = 0; i < lbl_804B4168.count; i++) {
        lbl_804B4168.entries[i].sys->Reset();
    }
}

void fn_80292698(void) {
    f32 t = fn_801E7E18();
    Spin* s = lbl_804B4030;
    for (int i = 0; i < 6; i++, s++) {
        PVec v;
        v.x = s->axis.x * (t * s->speed);
        v.y = s->axis.y * (t * s->speed);
        v.z = s->axis.z * (t * s->speed);
        f32 ang = PSVECMag(&v);
        if (ang > 0.0001f) {
            f32 inv = 1.0f / ang;
            f32 x = inv * v.x;
            f32 y = inv * v.y;
            f32 z = inv * v.z;
            f32 sn = sin(ang);
            f32 c = cos(ang);
            f32 k = 1.0f - c;
            s->rot.x.x = c + k * (x * x);
            s->rot.x.y = y * (k * x) + sn * z;
            s->rot.x.z = z * (k * x) - sn * y;
            s->rot.y.x = y * (k * x) - sn * z;
            s->rot.y.y = c + k * (y * y);
            s->rot.y.z = z * (k * y) + sn * x;
            s->rot.z.x = z * (k * x) + sn * y;
            s->rot.z.y = z * (k * y) - sn * x;
            s->rot.z.z = c + k * (z * z);
        } else {
            s->rot.x.x = 1.0f;
            s->rot.x.y = 0.0f;
            s->rot.x.z = 0.0f;
            s->rot.y.x = 0.0f;
            s->rot.y.y = 1.0f;
            s->rot.y.z = 0.0f;
            s->rot.z.x = 0.0f;
            s->rot.z.y = 0.0f;
            s->rot.z.z = 1.0f;
        }
    }
    for (int j = 0; j < lbl_804B4168.count; j++) {
        lbl_804B4168.entries[j].sys->Update();
    }
}

void fn_80292864(void) {
    for (int i = 0; i < lbl_804B4168.count; i++) {
        lbl_804B4168.entries[i].sys->Draw();
    }
}

Chunks* fn_802928D4(ValueNode* vn, int type, const char* base, int inst, const char* remap) {
    switch (type) {
    case 1:
        return new ("car chunk sys") CarChunks(vn, base);
    case 2:
        return new ("ter chunk sys") TerrainChunks(vn, base, inst, remap);
    case 3:
        return new ("wood chunk sys") WoodChunks(vn, base, inst, remap);
    default:
        fn_802E317C("chunk: invalid chunk type %d", type);
        return 0;
    }
}

Chunks* fn_802929D8(int type, const char* base, int inst, const char* remap) {
    return fn_802928D4(0, type, base, inst, remap);
}

int fn_80292DE8(const char* name);

Chunks* fn_802929F8(const char* name) {
    int idx = -1;
    if (lbl_804B4168.Find(name, &idx)) {
        lbl_804B4168.entries[idx].refs++;
        return lbl_804B4168.entries[idx].sys;
    }
    ValueNode* vn = fn_802AE414(name);
    if (!vn) {
        fn_802E317C("chunk: could not find table %s", name);
    }
    int type = fn_80292DE8(vn->GetValue("type"));
    const char* base = vn->GetValue("model_base");
    int inst = vn->GetInt("instance", 0);
    Chunks* sys = fn_802928D4(vn, type, base, inst, vn->GetString("remap_texture", 0));
    lbl_804B4168.Add(sys, name);
    return sys;
}

void fn_80292B48(Chunks* sys) {
    if (lbl_804B4168.Release(sys)) {
        delete sys;
    }
}

int fn_80292BA8(PVec* pos, PVec* vel, const PVec* hit, const PVec* n) {
    PVec d;
    d.x = pos->x - hit->x;
    d.y = pos->y - hit->y;
    d.z = pos->z - hit->z;
    f32 dist = PSVECDotProduct(&d, n);
    if (dist < 0.0f) {
        f32 vn = PSVECDotProduct(vel, n);
        f32 back = -dist;
        pos->x = pos->x + n->x * back;
        pos->y = pos->y + n->y * back;
        pos->z = pos->z + n->z * back;
        f32 k = 0.4f * (f32)__fabs(vn) - vn;
        vel->x = vel->x + n->x * k;
        vel->y = vel->y + n->y * k;
        vel->z = vel->z + n->z * k;
        return 1;
    }
    return 0;
}

void fn_80292CD8(const char* model, int id, const char* tex) {
    ModelInfo* m = fn_802C6B44(model);
    TexRemap r;
    memset(&r, 0, sizeof(r));
    StrNCopy(r.newName, tex, 16);
    r.id = id;
    for (int i = 0; i < m->count; i++) {
        StrNCopy(r.name, fn_802C9C24(m->texs + i * 0x10), 16);
        fn_802C7888(&r, 1);
    }
    fn_802C6C20(m);
}

int fn_80292DE8(const char* name) {
    TypeTable t = sTypes;
    for (u32 i = 0; i < 3; i++) {
        if (strcmp(name, t.t[i].name) == 0) {
            return t.t[i].type;
        }
    }
    fn_802E317C("chunk: invalid type '%s'", name);
    return 0;
}
}

void Bucket::Clear(void) { count = 0; }

void Bucket::Reset(void) { count = 0; }

int Bucket::Find(const char* name, int* idx) {
    for (int i = 0; i < count; i++) {
        if (strncasecmp(name, entries[i].name, 15) == 0) {
            *idx = i;
            return 1;
        }
    }
    return 0;
}

void Bucket::Add(Chunks* sys, const char* name) {
    StrNCopy(entries[count].name, name, 16);
    entries[count].sys = sys;
    entries[count].refs = 1;
    count++;
}

int Bucket::Release(Chunks* sys) {
    for (int i = 0; i < count; i++) {
        if (entries[i].sys == sys) {
            return --entries[i].refs == 0;
        }
    }
    fn_802E317C("chunk: could not find system %x in bucket", sys);
    return 0;
}

CarChunks::CarChunks(ValueNode* vn, const char* base) {
    LoadModels(base);
    InitColors();
    grav = 1.0f;
    if (vn) {
        grav = vn->GetFloat("grav_f", grav);
    }
    count = 0;
}

CarChunks::~CarChunks(void) { FreeModels(); }

void CarChunks::Reset(void) { count = 0; }

void CarChunks::Update(void) {
    if ((f32)__fabs(fn_801E7E24()) <= lbl_80558DC4[0]) {
        return;
    }
    Step();
}

void CarChunks::Draw(void) {
    Chunk* c;
    u32 c0 = fn_802C9950(0);
    u32 c1 = fn_802C9994(0);
    c = chunks;
    for (int i = 0; i < count; c++, i++) {
        fn_802C9950(colors[c->color].dim);
        fn_802C9994(colors[c->color].full);
        fn_802C744C(models[c->model], c);
    }
    fn_802C9994(c1);
    fn_802C9950(c0);
}

void CarChunks::Spawn(const PVec* pos, const PVec* vel, int slot, int color) {
    if (count >= 160 || !numModels) {
        return;
    }
    Chunk* c = &chunks[count];
    int s = fn_80129D18(0, 6);
    c->pos = *pos;
    c->rot = lbl_804B4030[s].rot;
    c->vel = *vel;
    c->age = 0.0f;
    c->slot = slot;
    c->state = fn_80287B5C(3) ? 2 : 3;
    c->model = fn_80287B5C(numModels);
    c->spin = s;
    c->color = color;
    count++;
}

void CarChunks::LoadModels(const char* base) {
    char name[32];
    for (numModels = 0; numModels < 8; numModels++) {
        fn_801345D8(name, sizeof(name), "%s%d.mod", base, numModels + 1);
        if (!fn_8012C33C(name)) {
            break;
        }
        models[numModels] = fn_802C6FC0(name, 0, 0);
    }
    for (int i = numModels; i < 8; i++) {
        models[i] = -1;
    }
}

void CarChunks::FreeModels(void) {
    for (int i = 0; i < numModels; i++) {
        fn_802C6CF8(models[i]);
        models[i] = -1;
    }
}

static inline int Dim(u32 c) {
    int v = 0.5f * (f32)(c & 0xFF);
    return v > 0xFF ? 0xFF : v;
}

static inline u32 DimColor(int r, int g, int b) { return 0xFF000000 | (r & 0xFF) << 16 | (g & 0xFF) << 8 | (b & 0xFF); }

void CarChunks::InitColors(void) {
    CarInfo** list = lbl_80488648;
    CarColor* cc = colors;
    for (u32 i = 0; (i < 8 ? list[i] : 0) != 0; i++, cc++) {
        u32 c = list[i]->color;
        cc->dim = DimColor(Dim(c >> 16), Dim(c >> 8), Dim(c));
        cc->full = c;
    }
}

void CarChunks::Step(void) {
    f32 dt = fn_801E7E24();
    f32 drag = 1.0f - 0.2f * dt;
    fn_801E9DD4(lbl_8055C958, lbl_8055C964);
    Chunk* in = chunks;
    Chunk* out = chunks;
    int n = 0;
    for (int i = 0; i < count; i++, in++) {
        f32 age = in->age;
        if (age < 0.0f || age >= 5.0f) {
            continue;
        }
        f32 nage = age + dt;
        u8 spin = in->spin;
        s32 state = in->state;
        PVec v;
        v.x = in->vel.x * drag;
        v.y = in->vel.y * drag;
        v.z = in->vel.z * drag;
        v.y = v.y - 9.81f * dt * grav;
        PVec p = in->pos;
        s16 slot = in->slot;
        if (state == 2) {
            p.x = p.x + v.x * dt;
            p.y = p.y + v.y * dt;
            p.z = p.z + v.z * dt;
            PVec hit;
            PVec nrm;
            nrm.x = 0.0f;
            hit = p;
            nrm.y = 1.0f;
            nrm.z = 0.0f;
            s32 id = 0;
            if (fn_801E9E78(&slot, &hit, &nrm, &id)) {
                if (fn_80292BA8(&p, &v, &hit, &nrm)) {
                    spin = fn_80287B5C(6);
                    if (v.y < 0.2f) {
                        nage = 0.0f;
                        state = 1;
                    }
                }
            } else {
                state = 3;
            }
        } else if (state == 3) {
            p.x = p.x + v.x * dt;
            p.y = p.y + v.y * dt;
            p.z = p.z + v.z * dt;
        } else {
            if (!(nage < 2.0f)) {
                continue;
            }
            if (nage > 0.0f) {
                p.y = p.y - 0.0762f * dt;
            }
            v.x = 0.0f;
            v.y = 0.0f;
            v.z = 0.0f;
        }
        out->pos = p;
        int fly = state == 2 || state == 3;
        const Mat3* rot = fly ? &lbl_804B4030[spin].rot : &in->rot;
        out->rot = *rot;
        out->vel = v;
        out->age = nage;
        out->slot = slot;
        out->state = state;
        out->model = in->model;
        out->spin = spin;
        out->color = in->color;
        out++;
        n++;
    }
    count = n;
    fn_801E9DD8();
}

extern "C" void fn_80292CD8(const char* model, int id, const char* tex);

TerrainChunks::TerrainChunks(ValueNode* vn, const char* base, int inst, const char* remap) {
    LoadModels(base, inst, remap);
    grav = 2.1f;
    if (vn) {
        grav = vn->GetFloat("grav_f", grav);
    }
    count = 0;
}

TerrainChunks::~TerrainChunks(void) { FreeModels(); }

void TerrainChunks::Reset(void) { count = 0; }

void TerrainChunks::Update(void) {
    if ((f32)__fabs(fn_801E7E24()) <= lbl_80558DC4[0]) {
        return;
    }
    Step();
}

void TerrainChunks::Draw(void) {
    Chunk* c = chunks;
    for (int i = 0; i < count; c++, i++) {
        fn_802C744C(models[c->model], c);
    }
}

void TerrainChunks::Spawn(const PVec* pos, const PVec* vel, int slot, int a) {
    if (count >= 256) {
        return;
    }
    Chunk* c = &chunks[count];
    int s = fn_80129D18(0, 6);
    c->pos = *pos;
    c->rot = lbl_804B4030[s].rot;
    c->vel = *vel;
    c->age = 0.0f;
    c->slot = slot;
    c->state = 2;
    c->model = fn_80129D18(0, numModels);
    c->spin = s;
    count++;
}

void TerrainChunks::LoadModels(const char* base, int inst, const char* remap) {
    char name[32];
    for (numModels = 0; numModels < 8; numModels++) {
        fn_801345D8(name, sizeof(name), "%s%d.mod", base, numModels + 1);
        if (!fn_8012C33C(name)) {
            break;
        }
        if (remap && remap[0]) {
            fn_80292CD8(name, inst, remap);
        }
        models[numModels] = fn_802C6FC0(name, inst, 0);
    }
    for (int i = numModels; i < 8; i++) {
        models[i] = -1;
    }
}

void TerrainChunks::FreeModels(void) {
    for (int i = 0; i < numModels; i++) {
        fn_802C6CF8(models[i]);
        models[i] = -1;
    }
}

void TerrainChunks::Step(void) {
    f32 dt = fn_801E7E24();
    f32 drag = 1.0f - 0.2f * dt;
    fn_801E9DD4(lbl_8055C958, lbl_8055C964);
    Chunk* in = chunks;
    Chunk* out = chunks;
    int n = 0;
    for (int i = 0; i < count; i++, in++) {
        f32 age = in->age;
        if (age < 0.0f || age >= 5.0f) {
            continue;
        }
        f32 nage = age + dt;
        u8 spin = in->spin;
        s32 state = in->state;
        PVec v;
        v.x = in->vel.x * drag;
        v.y = in->vel.y * drag;
        v.z = in->vel.z * drag;
        v.y = v.y - 9.81f * dt * grav;
        PVec p = in->pos;
        s16 slot = in->slot;
        if (state == 2) {
            p.x = p.x + v.x * dt;
            p.y = p.y + v.y * dt;
            p.z = p.z + v.z * dt;
            PVec hit;
            PVec nrm;
            nrm.x = 0.0f;
            hit = p;
            nrm.y = 1.0f;
            nrm.z = 0.0f;
            s32 id = 0;
            if (fn_801E9E78(&slot, &hit, &nrm, &id)) {
                if (fn_80292BA8(&p, &v, &hit, &nrm)) {
                    spin = fn_80287B5C(6);
                    if (v.y < 0.2f) {
                        nage = 0.0f;
                        state = 1;
                    }
                }
            } else {
                state = 3;
            }
        } else if (state == 3) {
            p.x = p.x + v.x * dt;
            p.y = p.y + v.y * dt;
            p.z = p.z + v.z * dt;
        } else {
            if (!(nage < 2.0f)) {
                continue;
            }
            if (nage > 0.0f) {
                p.y = p.y - 0.0762f * dt;
            }
            v.x = 0.0f;
            v.y = 0.0f;
            v.z = 0.0f;
        }
        out->pos = p;
        int fly = state == 2 || state == 3;
        const Mat3* rot = fly ? &lbl_804B4030[spin].rot : &in->rot;
        out->rot = *rot;
        out->vel = v;
        out->age = nage;
        out->slot = slot;
        out->state = state;
        out->model = in->model;
        out->spin = spin;
        out++;
        n++;
    }
    count = n;
    fn_801E9DD8();
}

WoodChunks::WoodChunks(ValueNode* vn, const char* base, int inst, const char* remap) {
    LoadModels(base, inst, remap);
    grav = 1.0f;
    if (vn) {
        grav = vn->GetFloat("grav_f", grav);
    }
    count = 0;
}

WoodChunks::~WoodChunks(void) { FreeModels(); }

void WoodChunks::Reset(void) { count = 0; }

void WoodChunks::Update(void) {
    if ((f32)__fabs(fn_801E7E24()) <= lbl_80558DC4[0]) {
        return;
    }
    Step();
}

void WoodChunks::Draw(void) {
    WoodChunk* c = chunks;
    for (int i = 0; i < count; c++, i++) {
        fn_802C744C(models[c->model], (const Chunk*)c);
    }
}

void WoodChunks::Spawn(const PVec* pos, const PVec* vel, int slot, int a) {
    if (count >= 128 || !numModels) {
        return;
    }
    WoodChunk* c = &chunks[count];
    int s = fn_80129D18(0, 6);
    c->pos = *pos;
    c->rot = lbl_804B4030[s].rot;
    c->vel = *vel;
    c->age = 0.0f;
    c->model = fn_80287B5C(numModels);
    c->spin = s;
    count++;
}

void WoodChunks::LoadModels(const char* base, int inst, const char* remap) {
    char name[32];
    for (numModels = 0; numModels < 8; numModels++) {
        fn_801345D8(name, sizeof(name), "%s%d.mod", base, numModels + 1);
        if (!fn_8012C33C(name)) {
            break;
        }
        if (remap && remap[0]) {
            fn_80292CD8(name, inst, remap);
        }
        models[numModels] = fn_802C6FC0(name, inst, 0);
    }
    for (int i = numModels; i < 8; i++) {
        models[i] = -1;
    }
}

void WoodChunks::FreeModels(void) {
    for (int i = 0; i < numModels; i++) {
        fn_802C6CF8(models[i]);
        models[i] = -1;
    }
}

void WoodChunks::Step(void) {
    f32 dt = fn_801E7E24();
    f32 drag = 1.0f - 1.5f * dt;
    WoodChunk* in = chunks;
    WoodChunk* out = chunks;
    for (int i = 0; i < count; i++, in++) {
        if (in->age < 0.0f || in->age >= 5.0f) {
            continue;
        }
        PVec v;
        v.x = in->vel.x * drag;
        v.y = in->vel.y * drag - 9.81f * dt * grav;
        v.z = in->vel.z * drag;
        out->pos.x = in->pos.x + v.x * dt;
        out->pos.y = in->pos.y + v.y * dt;
        out->pos.z = in->pos.z + v.z * dt;
        out->rot = lbl_804B4030[in->spin].rot;
        out->vel = v;
        out->age = in->age + dt;
        out->model = in->model;
        out->spin = in->spin;
        out++;
    }
    count = out - chunks;
}
