// Mutable physics objects (0x801D32D0-0x801D4734): the chunks a scripted-object event (soevent) knocks off
// an object. A pool of rigid bodies, each with one inline collision shape (sphere, tube, hinged tube or a
// box bounded by a sphere) set up from a "phob_config" (inline or by name from trk_sobj.val or
// race.res/sobj.val). A chunk lives "max_life" seconds and is then parked below the world (y -1000);
// a hinged chunk instead swings back to its rest pose and is parked once it is upright again and no
// car is touching it. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
double sqrt(double x);
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* table);
void fn_801796CC(u8 type, const void* data, u32 n);
void fn_801A4868(void* obj, int a);
int fn_801D04D0(const char* name);  // surface type by name
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
f32 PSVECDotProduct(const void* a, const void* b);
void PSVECCrossProduct(const void* a, const void* b, void* out);
f32 PSVECSquareDistance(const void* a, const void* b);
}


extern f32 lbl_8055E3C8;    // time
extern f32 lbl_8055E460;    // tick alpha
extern f32 lbl_80558DC4[];  // FLT_EPSILON

// ValueNode/ValueTable without the inline GetFloat(key, def) body (no weak copy here).
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
    virtual s32 GetIntValue2(const char* key);
    virtual int GetBool(const char* key, int def);
};

class ValueTableV {
public:
    virtual ValueNodeV* FindChild(const char* name);  // 0x8
    virtual ValueNodeV* GetChild(int index);          // 0xC
    virtual int HasChild(const char* name);           // 0x10
};

struct RVec3 {
    RVec3(void) {}
    RVec3(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
    void Set(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
    f32 x;
    f32 y;
    f32 z;
};

struct RMtx {
    RVec3 rot[3];  // offset 0x0
    RVec3 pos;     // offset 0x24
};

struct RPacket;
struct BodyDesc;
class CollVolume;

extern "C" void fn_8019EECC(CollVolume* v, RVec3* c);

class CollShape {
public:
    CollShape(CollVolume* o, const RMtx* m, u32 t, f32 k, f32 s) {
        mtx = m;
        owner = o;
        a8 = 0;
        type = t;
        stiff = k;
        skin = s;
        a20 = 100;
        a1C = 0;
    }
    virtual ~CollShape(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void vf1C(void);
    virtual void vf20(void);
    virtual void vf24(void);
    virtual void vf28(void);
    virtual void vf2C(void);
    virtual void vf30(void);
    virtual void vf34(void);
    virtual void vf38(void) = 0;
    virtual f32 Radius(void);    // 0x3C
    virtual void SetId(int id);  // 0x40
    CollVolume* owner;  // offset 0x4
    s32 a8;             // offset 0x8
    const RMtx* mtx;    // offset 0xC
    f32 stiff;          // offset 0x10
    f32 skin;           // offset 0x14
    u32 type;           // offset 0x18
    s32 a1C;            // offset 0x1C
    s32 a20;            // offset 0x20
};

class SphereShape : public CollShape {
public:
    SphereShape(CollVolume* o, const RMtx* m, f32 k, f32 s, f32 r, const RVec3& c)
        : CollShape(o, m, 0x53504852, k, s), center(c) {
        radius = r;
        f34 = 0.0f;
        f38 = 0.0f;
        f3C = 0.0f;
        f40 = 0.0f;
        f44 = 0.0f;
        f48 = 0.0f;
        b4C = 0;
        f50 = 0.0f;
        center.x = c.x;
        center.y = c.y;
        center.z = c.z;
        if (o) {
            fn_8019EECC(o, &center);
        }
    }
    virtual ~SphereShape(void);
    virtual void vf38(void);
    f32 radius;    // offset 0x24
    RVec3 center;  // offset 0x28
    f32 f34;       // offset 0x34
    f32 f38;       // offset 0x38
    f32 f3C;       // offset 0x3C
    f32 f40;       // offset 0x40
    f32 f44;       // offset 0x44
    f32 f48;       // offset 0x48
    u8 b4C;        // offset 0x4C
    f32 f50;       // offset 0x50
};

class TubeBase : public CollShape {
public:
    TubeBase(CollVolume* o, const RMtx* m, f32 k, f32 s, f32 r, f32 h) : CollShape(o, m, 'TUBE', k, s) {
        rad = r;
        half = h;
    }
    f32 rad;   // offset 0x24
    f32 half;  // offset 0x28: half the length between the end spheres
};

class TubeShape : public TubeBase {
public:
    TubeShape(CollVolume* o, const RMtx* m, f32 k, f32 s, f32 r, f32 h) : TubeBase(o, m, k, s, r, h) {}
    virtual ~TubeShape(void);
    virtual void vf38(void);
};

// Builds, in 's', a sphere bounding a box of half-size (hx, hy, hz).
extern "C" void fn_8019F564(CollShape* s, const RMtx* m, const RVec3* c, CollVolume* o, f32 k, f32 skin, f32 a,
                            f32 hx, f32 hy, f32 hz);

class CollVolume {
public:
    virtual void Init(BodyDesc* d);   // 0x8
    virtual ~CollVolume(void);        // 0xC
    virtual void Reset(void);         // 0x10
    virtual void vf14(void);          // 0x14
    virtual void vf18(void);          // 0x18
    virtual void vf1C(void);          // 0x1C
    virtual void vf20(void);          // 0x20
    virtual void vf24(void);          // 0x24
    virtual void vf28(u8* buf);       // 0x28
    virtual s32 vf2C(RPacket* p);     // 0x2C
    virtual void vf30(f32 t, RPacket* a, RPacket* b, s32* na, s32* nb);  // 0x30
    virtual s32 vf34(RPacket* p);     // 0x34
    virtual void vf38(u8* p);         // 0x38
    virtual void vf3C(void);          // 0x3C
    virtual int vf40(void);           // 0x40
    virtual int vf44(void);           // 0x44
    void Update(void);
    void AddShape(CollShape* s);
    void SetActive(int on) { nAdded = on ? nSub : 0; }
    s32 a4;             // offset 0x4
    s32 a8;             // offset 0x8
    void* desc;         // offset 0xC
    u32 type;           // offset 0x10
    CollShape* sub[4];  // offset 0x14
    s32 nAdded;         // offset 0x24
    s32 nSub;           // offset 0x28
    f32 radius;         // offset 0x2C
    RVec3 offset;       // offset 0x30
    RMtx mtx;           // offset 0x3C
    s16 hint[2];        // offset 0x6C
    RMtx world;         // offset 0x70
    RMtx prev;          // offset 0xA0
};

class RigidBody : public CollVolume {
public:
    RigidBody(void);
    ~RigidBody(void) {}
    virtual void Init(BodyDesc* d);
    virtual void Reset(void);
    virtual void vf30(f32 t, RPacket* a, RPacket* b, s32* na, s32* nb);
    virtual s32 vf34(RPacket* p);
    virtual void OnHit(const RVec3* f, const RVec3* at, int kind, void* c);  // 0x48
    virtual void OnImpulse(const RVec3* v, const RVec3* at, int kind, void* c, void* by);  // 0x4C
    virtual void OnImpulses(RVec3* f, RVec3* t);  // 0x50
    void Update(void);
    void SetMass(f32 m, f32 ix, f32 iy, f32 iz);
    u8 padD0[0x118 - 0xD0];
    RVec3 vel;     // offset 0x118
    RVec3 avel;    // offset 0x124
    RVec3 force;   // offset 0x130
    RVec3 torque;  // offset 0x13C
    u8 pad148[0x16C - 0x148];
    RVec3 accel;   // offset 0x16C
    RVec3 aaccel;  // offset 0x178
    u8 pad184[0x1E4 - 0x184];
    s32 nImp;  // offset 0x1E4
    u8 b1E8;   // offset 0x1E8
    u8 b1E9;   // offset 0x1E9: ground test
};

static inline int IsCar(const CollVolume* o) {
    u32 t = o->type;
    int r = 0;
    if (t == 'CAR ' || t == 'PCAR' || t == 'ACAR' || t == 'NCAR') {
        r = 1;
    }
    return r;
}

struct HitMsg {
    RVec3 pos;   // offset 0x0
    RVec3 vel;   // offset 0xC
    s16 surf;    // offset 0x18
    s32 effect;  // offset 0x1C
};

enum {
    MP_SPHERE,
    MP_TUBE,
    MP_HINGED,
    MP_CUBE,
};

// A chunk's settings: a "phob_config" entry.
struct PhobConfig {
    f32 maxLife;    // offset 0x0
    f32 mass;       // offset 0x4
    f32 mx;         // offset 0x8: inertia
    f32 my;         // offset 0xC
    f32 mz;         // offset 0x10
    f32 spring;     // offset 0x14
    f32 friction;   // offset 0x18
    s32 surf;       // offset 0x1C
    u8 groundTest;  // offset 0x20
    s32 effect;     // offset 0x24: hit effect, <= 0 = none
    s32 kind;       // offset 0x28
    f32 a;          // offset 0x2C: rad (sphere, tube, hinged) or len (cube)
    f32 b;          // offset 0x30: len (tube) or hit (hinged)
};

// The render state, written at the body's offset in the frame buffer.
struct MutPose {
    RMtx mtx;    // offset 0x0
    s16 hint;    // offset 0x30
    s32 model;   // offset 0x34
    s32 mask;    // offset 0x38
    s32 unk3C;   // offset 0x3C
};

class MutPhob : public RigidBody {
public:
    virtual void Init(BodyDesc* d);
    virtual ~MutPhob(void);
    virtual void Reset(void);
    virtual void vf1C(void);
    virtual void vf28(u8* buf);
    virtual s32 vf2C(RPacket* p);
    virtual void vf30(f32 t, RPacket* a, RPacket* b, s32* na, s32* nb);
    virtual s32 vf34(RPacket* p);
    virtual int vf44(void);
    virtual void OnHit(const RVec3* f, const RVec3* at, int kind, void* c);
    s32 model;        // offset 0x1EC
    s32 mask;         // offset 0x1F0
    RMtx home;        // offset 0x1F4
    f32 time;         // offset 0x224
    void* src;        // offset 0x228: the scripted object
    CollVolume* car;  // offset 0x22C: the car that hit a hinged chunk
    s32 kind;         // offset 0x230
    f32 maxLife;      // offset 0x234
    s32 effect;       // offset 0x238
    u32 shapeType;    // offset 0x23C
    f32 shapeRadius;  // offset 0x240
    CollShape* shape; // offset 0x244
    u8 shapeBuf[0x54];  // offset 0x248
};

ModuleTag lbl_8055E938;
s32 lbl_8055E940;  // chunks in the pool
MutPhob* lbl_804421D8[64];

extern "C" {

MutPhob* fn_801D36E8(void);
void fn_801D3FA4(MutPhob* p, int model, int mask, const RMtx* xf, const RVec3* offset, const PhobConfig* c, void* src);
void fn_801D411C(MutPhob* p);
void fn_801D4294(MutPhob* p, const PhobConfig* c);
void fn_801D4598(MutPhob* p, f32 now);
int fn_801D37C4(const char* name);

void fn_801D32D0(void) { lbl_8055E940 = 0; }

void fn_801D32DC(void) {}

MutPhob* fn_801D32E0(int model, int mask, const RMtx* xf, const RVec3* offset, const PhobConfig* c, void* src) {
    MutPhob* p = fn_801D36E8();
    fn_801D3FA4(p, model, mask, xf, offset, c, src);
    return p;
}

void fn_801D334C(PhobConfig* c, ValueNodeV* t) {
    c->maxLife = t->GetFloat("max_life");
    c->mass = t->GetFloat("mass");
    c->mx = t->GetFloat("mx");
    c->my = t->GetFloat("my");
    c->mz = t->GetFloat("mz");
    c->spring = t->GetFloat("spring", 1.5f * (9.81f * c->mass / 0.03f));
    c->friction = t->GetFloat("friction", 1.0f);
    if (t->Has("surf_type")) {
        c->surf = fn_801D04D0(t->GetValue("surf_type"));
    } else {
        c->surf = 100;
    }
    c->groundTest = t->GetBool("ground_test", 1);
    c->effect = t->GetInt("effect_id", -1);
    int kind = fn_801D37C4(t->GetValue("type"));
    c->kind = kind;
    switch (kind) {
    case MP_SPHERE:
        c->a = t->GetFloat("rad");
        break;
    case MP_TUBE:
        c->a = t->GetFloat("rad");
        c->b = t->GetFloat("len");
        break;
    case MP_HINGED:
        c->a = t->GetFloat("rad");
        c->b = t->GetFloat("hit");
        break;
    case MP_CUBE:
        c->a = t->GetFloat("len");
        break;
    default:
        fn_802E317C("mut_phob: invalid type %d", kind);
        break;
    }
}

void fn_801D3610(PhobConfig* c, const char* name) {
    const char* files[2] = {"trk_sobj.val", "race.res/sobj.val"};
    for (u32 i = 0; i < 2; i++) {
        ValueTableV* t = (ValueTableV*)fn_80131810(files[i]);
        if (t) {
            if (t->HasChild(name)) {
                fn_801D334C(c, t->FindChild(name));
                fn_80131818((ValueTable*)t);
                return;
            }
            fn_80131818((ValueTable*)t);
        }
    }
    fn_802E317C("mut_phob: could not find config '%s'", name);
}

// A free chunk, or else the oldest one (parked first).
MutPhob* fn_801D36E8(void) {
    MutPhob* oldest = 0;
    f32 t = 0.0f;
    for (int i = 0; i < lbl_8055E940; i++) {
        if (!lbl_804421D8[i]->vf44()) {
            return lbl_804421D8[i];
        }
        if (!oldest || lbl_804421D8[i]->time < t) {
            oldest = lbl_804421D8[i];
            t = oldest->time;
        }
    }
    fn_801D411C(oldest);
    return oldest;
}

}  // extern "C"

int MutPhob::vf44(void) { return model != -1; }

struct MutKind {
    const char* name;
    s32 kind;
};

extern "C" int fn_801D37C4(const char* name) {
    MutKind kinds[4] = {
        {"sphere", MP_SPHERE},
        {"tube", MP_TUBE},
        {"hinged", MP_HINGED},
        {"cube", MP_CUBE},
    };
    for (u32 i = 0; i < 4; i++) {
        if (!strcasecmp(name, kinds[i].name)) {
            return kinds[i].kind;
        }
    }
    fn_802E317C("mut_phob: invalid type '%s'", name);
    return 0;
}

MutPhob::~MutPhob(void) {
    nSub = 0;
    SetActive(0);
    radius = 0.0f;
}

void MutPhob::Init(BodyDesc* d) {
    RigidBody::Init(d);
    shape = (CollShape*)shapeBuf;
    new (shape) SphereShape(this, &mtx, 1.0f, 1.0f, 1.0f, RVec3(0.0f, 0.0f, 0.0f));
    AddShape(shape);
    b1E8 = 1;
    lbl_804421D8[lbl_8055E940] = this;
    lbl_8055E940++;
}

void MutPhob::Reset(void) {
    model = -1;
    mask = -1;
    time = 0.0f;
    src = 0;
    car = 0;
    shapeType = 0;
    SetActive(1);
    new (shape) SphereShape(this, &mtx, 1.0f, 1.0f, 1.0f, RVec3(0.0f, 0.0f, 0.0f));
    home.pos.z = 0.0f;
    SetActive(0);
    f32 one = 1.0f;  // keeps 0.0 in f1 and 1.0 in f2, as in the target
    home.rot[0].Set(one, 0.0f, 0.0f);
    home.rot[1].Set(0.0f, 1.0f, 0.0f);
    home.rot[2].Set(0.0f, 0.0f, 1.0f);
    home.pos.Set(0.0f, -1000.0f, 0.0f);
    offset = RVec3(0.0f, 0.0f, 0.0f);
    CollVolume::vf20();
    mtx = home;
    CollVolume::Update();
    RigidBody::Reset();
}

void MutPhob::vf1C(void) {
    f32 now = lbl_8055E3C8;
    if (kind == MP_HINGED) {
        RigidBody::Update();
        fn_801D4598(this, now);
    } else if (now < time + maxLife) {
        RigidBody::Update();
        vel.x *= 0.99983317f;
        vel.y *= 0.99983317f;
        vel.z *= 0.99983317f;
        avel.x *= 0.99983317f;
        avel.y *= 0.99983317f;
        avel.z *= 0.99983317f;
    } else {
        fn_801D411C(this);
        nImp = 0;
    }
}

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

static inline void LerpMtx(RMtx* out, const RMtx* a, const RMtx* b, f32 t) {
    f32* d = (f32*)out;
    const f32* x = (const f32*)a;
    const f32* y = (const f32*)b;
    for (int i = 0; i < 12; i++) {
        d[i] = Lerp(x[i], y[i], t);
    }
}

void MutPhob::vf28(u8* buf) {
    MutPose* p = (MutPose*)(buf + a8);
    if ((f32)__fabs(lbl_8055E460 - 1.0f) <= lbl_80558DC4[0]) {
        p->mtx = world;
    } else {
        RMtx m;
        LerpMtx(&m, &prev, &world, lbl_8055E460);
        p->mtx = m;
    }
    p->hint = hint[0];
    p->model = model;
    p->mask = mask;
    p->unk3C = 0;
}

void MutPhob::OnHit(const RVec3* f, const RVec3* at, int k, void* c) {
    RigidBody::OnHit(f, at, k, c);
    RigidBody* o = (RigidBody*)c;
    if (effect > 0 && o) {
        HitMsg m;
        m.pos = *at;
        m.vel = o->vel;
        m.surf = o->hint[0];
        m.effect = effect;
        fn_801796CC(10, &m, 0x20);
    }
    if (kind == MP_HINGED && car != o && o && IsCar(o)) {
        car = o;
        SetActive(0);
    }
}

extern "C" {

// Places chunk 'p' at 'xf' with the settings in 'c'.
void fn_801D3FA4(MutPhob* p, int model, int mask, const RMtx* xf, const RVec3* offset, const PhobConfig* c, void* src) {
    p->model = model;
    p->mask = mask;
    p->home = *xf;
    p->time = lbl_8055E3C8;
    p->src = src;
    p->car = 0;
    p->kind = c->kind;
    p->maxLife = c->maxLife;
    p->effect = c->effect;
    p->SetMass(c->mass, c->mx, c->my, c->mz);
    fn_801D4294(p, c);
    p->offset = *offset;
    p->CollVolume::vf20();
    p->mtx = *xf;
    p->CollVolume::Update();
}

// Parks chunk 'p' below the world.
void fn_801D411C(MutPhob* p) {
    if (p->kind == MP_HINGED && p->src) {
        fn_801A4868(p->src, 1);
    }
    p->home.pos.z = 0.0f;
    p->vel.Set(0.0f, 0.0f, 0.0f);
    p->avel.Set(0.0f, 0.0f, 0.0f);
    p->force.Set(0.0f, 0.0f, 0.0f);
    p->torque.Set(0.0f, 0.0f, 0.0f);
    p->accel.Set(0.0f, 0.0f, 0.0f);
    p->aaccel.Set(0.0f, 0.0f, 0.0f);
    p->home.rot[0].Set(1.0f, 0.0f, 0.0f);
    p->home.rot[1].Set(0.0f, 1.0f, 0.0f);
    p->home.rot[2].Set(0.0f, 0.0f, 1.0f);
    p->home.pos.Set(0.0f, -1000.0f, 0.0f);
    p->offset = RVec3(0.0f, 0.0f, 0.0f);
    p->CollVolume::vf20();
    p->mtx = p->home;
    p->CollVolume::Update();
    p->SetActive(0);
    p->model = -1;
}

// Sets up the chunk's collision shape.
static inline f32 SumSquares(f32 a, f32 b) {
    f32 aa = a * a;
    f32 bb = b * b;
    return aa + bb;
}

void fn_801D4294(MutPhob* p, const PhobConfig* c) {
    RVec3 center(0.0f, 0.0f, 0.0f);
    switch (c->kind) {
    case MP_SPHERE:
        p->shapeType = 0x53504852;
        p->shapeRadius = c->a;
        new (p->shape) SphereShape(p, &p->mtx, c->spring, c->friction, c->a, center);
        break;
    case MP_TUBE:
        p->shapeType = 'TUBE';
        p->shapeRadius = sqrt(c->a * c->a + (0.5f * c->b) * (0.5f * c->b));
        new (p->shape) TubeShape(p, &p->mtx, c->spring, c->friction, c->a, 0.5f * c->b - c->a);
        break;
    case MP_HINGED:
        p->shapeType = 'TUBE';
        p->shapeRadius = sqrt(SumSquares(c->a, c->b));
        new (p->shape) TubeShape(p, &p->mtx, c->spring, c->friction, c->a, c->b);
        break;
    case MP_CUBE: {
        f32 h = 0.5f * c->a;
        p->shapeType = 0x53504852;
        p->shapeRadius = h;
        if (p->shape) {
            fn_8019F564(p->shape, &p->mtx, &center, p, c->spring, c->friction, 1.0f, h, h, h);
        }
        break;
    }
    default:
        fn_802E317C("mut_phob: invalid type %d", c->kind);
        break;
    }
    p->shape->SetId(c->surf);
    p->b1E9 = c->groundTest;
    p->SetActive(1);
}

static inline void AddScaled(RVec3* v, const RVec3* a, f32 s) {
    v->x += a->x * s;
    v->y += a->y * s;
    v->z += a->z * s;
}

// A hinged chunk swings back upright; parked when it is and no car is near.
void fn_801D4598(MutPhob* p, f32 now) {
    f32 d = PSVECDotProduct(&p->mtx.rot[2], &p->home.rot[2]);
    if (now < p->time + p->maxLife || d < 0.99f ||
        (p->car && PSVECSquareDistance(&p->car->mtx.pos, &p->home.pos) < (1.0f + p->shapeRadius) * (1.0f + p->shapeRadius))) {
        RVec3 axis;
        PSVECCrossProduct(&p->mtx.rot[2], &p->home.rot[2], &axis);
        p->vel.Set(0.0f, 0.0f, 0.0f);
        f32 k = d - 0.25f;
        k = (0.0f < k) ? 0.0f : k;
        k = 0.9f + k;
        k = (0.0f > k) ? 0.0f : k;
        p->avel.x *= k;
        p->avel.y *= k;
        p->avel.z *= k;
        AddScaled(&p->avel, &axis, 0.2f);
        p->mtx.pos = p->home.pos;
        p->CollVolume::Update();
    } else {
        fn_801D411C(p);
    }
}

}  // extern "C"

s32 MutPhob::vf2C(RPacket* p) { return 0; }

void MutPhob::vf30(f32 t, RPacket* a, RPacket* b, s32* na, s32* nb) {
    *nb = 0;
    *na = 0;
}

s32 MutPhob::vf34(RPacket* p) { return 0; }
