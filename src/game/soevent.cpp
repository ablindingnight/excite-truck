// Scripted-object events (0x801D21C0-0x801D32D0): "soevent" entries from race.res/sobj.val, trk_sobj.val
// and trackwob.val, sorted by id. When a car hits a scripted object whose id has an event class, the
// event can play hit effects (some held on the car for "dur" seconds, following it) and spawn up to 12
// mutable physics objects ("chunk%d", see mutphob) thrown off the object. Names are guesses apart
// from the strings.
#include "game/common.h"

extern "C" {
int strncmp(const char* a, const char* b, size_t n);
void qsort(void* base, size_t n, size_t size, int (*cmp)(const void*, const void*));
void* fn_800241DC(const void* key, const void* base, u32 n, u32 size, int (*cmp)(const void*, const void*));  // bsearch
f32 fn_80129DC4(int seed, f32 lo, f32 hi);  // random float
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* table);
int fn_801345D8(char* buf, int size, const char* fmt, ...);  // snprintf
void fn_801796CC(u8 type, const void* data, u32 n);
void fn_801A4868(void* obj, int a);
int fn_801B2F24(void);  // racer count
int fn_802C6FC0(const char* name, int a, int b);  // load a model
void fn_802C6CF8(int model);
int fn_802C7830(const char* name);  // chunk mask
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
f32 PSVECMag(const void* v);
f32 PSVECDotProduct(const void* a, const void* b);
void PSVECNormalize(const void* in, void* out);
}

extern f32 lbl_8055E3C8;      // race time
extern f32 lbl_80558DC4[];    // FLT_EPSILON

struct Vec {
    Vec(void) {}
    void Set(f32 x_, f32 y_, f32 z_) {
        x = x_;
        y = y_;
        z = z_;
    }
    Vec& operator=(const ValVec3& v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
    f32 x;
    f32 y;
    f32 z;
};

// A vector passed by value (GetVec3Or's default).
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

// GetVec3Or takes its default by value.
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
    virtual ValVec3 GetVec3(const char* key);
    virtual ValVec3 GetVec3Or(const char* key, V3 def);
    virtual ValPoint GetPoint(const char* key);
    virtual ValPoint GetPointOr(const char* key, const ValPoint& def);
    virtual ValMatrix GetMatrix(const char* key);
    virtual ValMatrix GetMatrixOr(const char* key, const ValMatrix& def);
    virtual f32 GetReal(const char* key);
    virtual f32 GetRealOr(const char* key, f32 def);
    virtual int GetEnum(const char* key, const char* names);
    virtual f32 GetScale(const char* key, f32 t);
    virtual ValueNodeV* FindChild(const char* name);
};

class ValueTableV {
public:
    virtual ValueNodeV* FindChild(const char* name);  // 0x8
    virtual ValueNodeV* GetChild(int index);          // 0xC
};

struct Car {
    u8 pad0[0x10];
    u32 type;  // offset 0x10
    u8 pad14[0x3C - 0x14];
    Vec rot[3];     // offset 0x3C
    Vec origin;     // offset 0x60
    s16 surf;       // offset 0x6C
    u8 pad6E[0xD4 - 0x6E];
    f32 mass;       // offset 0xD4
    u8 padD8[0x118 - 0xD8];
    Vec vel;        // offset 0x118
    u8 pad124[0x1EC - 0x124];
    s32 index;      // offset 0x1EC
};

static inline int IsCar(const Car* o) {
    u32 t = o->type;
    int r = 0;
    if (t == 'CAR ' || t == 'PCAR' || t == 'ACAR' || t == 'NCAR') {
        r = 1;
    }
    return r;
}

struct Basis {
    Basis(void) {}
    Basis(const Basis& o) : x(o.x), y(o.y), z(o.z) {}
    ValVec3 x, y, z;
};

struct Xform {
    Xform(const Basis& r) : rot(r) {}
    Basis rot;  // offset 0x0
    Vec pos;    // offset 0x24
};

// The scripted object that was hit.
struct SObj {
    u8 pad0[0x4];
    Xform xf;  // offset 0x4
    s32 id;    // offset 0x34
};

// A spawned physics object (mutphob).
class SpawnPhob {
public:
    virtual void vf08(void);
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
    virtual void vf38(void);
    virtual void vf3C(void);
    virtual void vf40(void);
    virtual void vf44(void);
    virtual void Push(const Vec* force, const Vec* at, int surf, int a);  // 0x48

    u8 pad[0xD4 - 0x4];
    f32 mass;  // offset 0xD4
};

// A mutable physics object's settings (mutphob.cpp).
struct PhobConfig {
    u8 pad0[0x1C];
    s32 surf;    // offset 0x1C
    u8 pad20[0x24 - 0x20];
    s32 effect;  // offset 0x24
    u8 pad28[0x34 - 0x28];
};

extern "C" {
void fn_801D334C(PhobConfig* c, ValueNodeV* t);
void fn_801D3610(PhobConfig* c, const char* name);
SpawnPhob* fn_801D32E0(int model, int chunk, const Xform* xf, const Vec* offset, const PhobConfig* c, SObj* obj);
}

struct HitMsg {
    Vec pos;     // offset 0x0
    Vec vel;     // offset 0xC
    s16 surf;    // offset 0x18
    s32 effect;  // offset 0x1C
};

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void Scale(Vec* out, const Vec* in, f32 s) {
    out->x = in->x * s;
    out->y = in->y * s;
    out->z = in->z * s;
}

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

// v = transpose(m) * v, in place.
static inline void MulVecTSelf(Vec* v, const Vec* m) {
    f32 x = Dot3(v->x, m[0].x, v->y, m[0].y, v->z, m[0].z);
    f32 y = Dot3(v->x, m[1].x, v->y, m[1].y, v->z, m[1].z);
    f32 z = Dot3(v->x, m[2].x, v->y, m[2].y, v->z, m[2].z);
    v->x = x;
    v->y = y;
    v->z = z;
}

// A hit effect kept playing on a car, at a point in the car's frame, until 'time'.
struct HeldEffect {
    void Start(s32 e, f32 t, const Vec* p, const Car* car);
    u8 active;   // offset 0x0
    s32 effect;  // offset 0x4
    f32 time;    // offset 0x8
    Vec pos;     // offset 0xC
};

struct SoEffect {
    s32 id;   // offset 0x0: < 0 = the object's own id
    f32 dur;  // offset 0x4: > 0 = held on the car
};

struct SoTrigger {
    u8 enabled;    // offset 0x0
    f32 minSpeed;  // offset 0x4
};

struct SoEffects : SoTrigger {
    SoEffect effects[4];  // offset 0x8
    s32 count;            // offset 0x28
};

struct SoChunk {
    u8 ownConfig;       // offset 0x0
    PhobConfig config;  // offset 0x4
    Vec offset;         // offset 0x38
    Vec throwVel;       // offset 0x44
    f32 forceScale;     // offset 0x50
    f32 randScale;      // offset 0x54
    s32 mask;           // offset 0x58
};

struct SoSpawn : SoTrigger {
    PhobConfig config;  // offset 0x8
    s32 model;          // offset 0x3C
    SoChunk chunks[12]; // offset 0x40
    s32 count;          // offset 0x490
};

struct SoEvent {
    s32 id;            // offset 0x0
    SoEffects effect;  // offset 0x4
    SoSpawn spawn;     // offset 0x30
};

ModuleTag lbl_8055E928;
s32 lbl_8055E930;  // event count
HeldEffect lbl_80438898[8];
SoEvent lbl_80438958[32];

// Starts holding effect 'e' until 't' at world point 'p', kept in the car's frame.
inline void HeldEffect::Start(s32 e, f32 t, const Vec* p, const Car* car) {
    active = 1;
    effect = e;
    time = t;
    pos.x = p->x - car->origin.x;
    pos.y = p->y - car->origin.y;
    pos.z = p->z - car->origin.z;
    MulVecTSelf(&pos, car->rot);
}

extern Car* lbl_804348E8[];

extern "C" {

void fn_801D2AD4(const char* file);
void fn_801D2B94(void);
void fn_801D2C04(void);
SoEvent* fn_801D2C20(int id);
void fn_801D2C70(SoEffects* s, const Vec* pos, Car* car, int id);
void fn_801D2E38(SoSpawn* s, SObj* obj, const Vec* pos, const Vec* vel, f32 mass);

void fn_801D21C0(void) {
    lbl_8055E930 = 0;
    fn_801D2AD4("race.res/sobj.val");
    fn_801D2AD4("trk_sobj.val");
    fn_801D2AD4("trackwob.val");
    fn_801D2C04();
}

void fn_801D2210(void) { fn_801D2B94(); }

// Replays the held effects at their points on each car.
void fn_801D2214(void) {
    int n = fn_801B2F24();
    f32 now = lbl_8055E3C8;
    HitMsg m;
    for (int i = 0; i < n; i++) {
        HeldEffect* h = &lbl_80438898[i];
        if (h->active) {
            if (now > h->time) {
                h->active = 0;
            } else {
                Car* car = lbl_804348E8[i];
                MulVec(&m.pos, &h->pos, car->rot);
                m.pos.x += car->origin.x;
                m.pos.y += car->origin.y;
                m.pos.z += car->origin.z;
                m.vel = car->vel;
                m.surf = car->surf;
                m.effect = h->effect;
                fn_801796CC(10, &m, 0x20);
            }
        }
    }
}

// A car hit scripted object 'obj' at 'pos'.
void fn_801D2414(SObj* obj, const Vec* pos, Car* car) {
    if (IsCar(car)) {
        SoEvent* e = fn_801D2C20(obj->id);
        if (e) {
            f32 speed = PSVECMag(&car->vel);
            if (e->effect.enabled && speed > e->effect.minSpeed) {
                fn_801D2C70(&e->effect, pos, car, obj->id);
            }
            if (e->spawn.enabled && speed > e->spawn.minSpeed) {
                fn_801D2E38(&e->spawn, obj, pos, &car->vel, car->mass);
                fn_801A4868(obj, 0);
            }
        } else if (obj->id > 0) {
            fn_802E317C("soevent: no such event class (%d)", obj->id);
        }
    }
}

int fn_801D2550(SoTrigger* s, ValueNodeV* t) {
    if (t) {
        s->enabled = 1;
        s->minSpeed = t->GetFloat("min_speed", 0.0f);
    } else {
        s->enabled = 0;
    }
    return s->enabled;
}

void fn_801D25BC(SoEffects* s, ValueNodeV* t) {
    if (fn_801D2550(s, t)) {
        s->effects[s->count = 0].id = -1;
        for (s->count++; s->count < 4; s->count++) {
            char key[0x20];
            fn_801345D8(key, 0x20, "effect%d", s->count);
            ValueNodeV* e = t->FindChild(key);
            if (!e) {
                break;
            }
            s->effects[s->count].id = e->GetIntValue("id");
            s->effects[s->count].dur = e->GetFloat("dur", 0.0f);
        }
    }
}

void fn_801D26C8(SoSpawn* s, ValueNodeV* t) {
    if (fn_801D2550(s, t)) {
        if (t->Has("phob_config")) {
            fn_801D3610(&s->config, t->GetValue("phob_config"));
        } else {
            fn_801D334C(&s->config, t);
        }
        s->config.effect = t->GetInt("effect_id", -1);
        s->model = fn_802C6FC0(t->GetValue("model"), 0, 0);
        for (s->count = 0; s->count < 12; s->count++) {
            char key[0x20];
            fn_801345D8(key, 0x20, "chunk%d", s->count + 1);
            if (!t->FindChild(key)) {
                break;
            }
            ValueNodeV* c = t->FindChild(key);
            SoChunk* k = &s->chunks[s->count];
            k->offset = c->GetVec3Or("offset", V3(0.0f, 0.0f, 0.0f));
            k->throwVel = c->GetVec3Or("throw", V3(0.0f, 0.0f, 0.0f));
            if (c->Has("mask")) {
                k->mask = fn_802C7830(c->GetValue("mask"));
            } else {
                k->mask = -1;
            }
            if (c->FindChild("phob_config") != 0) {
                fn_801D334C(&k->config, c->FindChild("phob_config"));
                k->ownConfig = 1;
            } else {
                k->ownConfig = 0;
            }
            k->forceScale = c->GetFloat("f_scale", 1.0f);
            k->randScale = c->GetFloat("rand_f_scale", 0.0f);
        }
        if (s->count < 1) {
            s->chunks[0].mask = -1;
            s->chunks[0].offset.Set(0.0f, 0.0f, 0.0f);
            s->count = 1;
        }
    } else {
        s->model = -1;
        s->count = 0;
    }
}

void fn_801D2A24(ValueNodeV* t) {
    SoEvent* e = &lbl_80438958[lbl_8055E930];
    e->id = t->GetIntValue("id");
    fn_801D25BC(&e->effect, t->FindChild("effect"));
    fn_801D26C8(&e->spawn, t->FindChild("spawn"));
    lbl_8055E930++;
}

void fn_801D2AD4(const char* file) {
    ValueTableV* table = (ValueTableV*)fn_80131810(file);
    if (table) {
        for (int i = 0; table->GetChild(i); i++) {
            ValueNodeV* t = table->GetChild(i);
            if (strncmp(t->GetName(), "soevent", 7) == 0) {
                fn_801D2A24(t);
            }
        }
        fn_80131818((ValueTable*)table);
    }
}

void fn_801D2B94(void) {
    for (int i = 0; i < lbl_8055E930; i++) {
        if (lbl_80438958[i].spawn.model != -1) {
            fn_802C6CF8(lbl_80438958[i].spawn.model);
        }
    }
}

int fn_801D2BF4(const void* a, const void* b) { return ((const SoEvent*)a)->id - ((const SoEvent*)b)->id; }

void fn_801D2C04(void) { qsort(lbl_80438958, lbl_8055E930, sizeof(SoEvent), fn_801D2BF4); }

SoEvent* fn_801D2C20(int id) {
    if (id <= 0) {
        return 0;
    }
    return (SoEvent*)fn_800241DC(&id, lbl_80438958, lbl_8055E930, sizeof(SoEvent), fn_801D2BF4);
}

// Plays the event's hit effects; the ones with a duration are held on the car.
void fn_801D2C70(SoEffects* s, const Vec* pos, Car* car, int id) {
    HitMsg m;
    m.pos = *pos;
    m.vel = car->vel;
    m.surf = car->surf;
    for (int i = 0; i < s->count; i++) {
        int effect = s->effects[i].id;
        if (effect < 0) {
            effect = id;
        }
        m.effect = effect;
        fn_801796CC(10, &m, 0x20);
        if (IsCar(car)) {
            lbl_80438898[car->index].Start(m.effect, s->effects[i].dur + lbl_8055E3C8, pos, car);
        }
    }
}

// Spawns the event's chunks at the object and throws them.
void fn_801D2E38(SoSpawn* s, SObj* obj, const Vec* pos, const Vec* vel, f32 mass) {
    int i = 0;
    Xform xf(obj->xf.rot);
    Vec v = *vel;
    Scale(&v, &v, mass / 0.016683351f);
    if (s->count > 0) {
    for (; i < s->count; i++) {
        SoChunk* c = &s->chunks[i];
        Vec at;
        Scale(&at, &c->offset, -1.0f);
        MulVec(&xf.pos, &c->offset, (const Vec*)&obj->xf.rot);
        xf.pos.x += obj->xf.pos.x;
        xf.pos.y += obj->xf.pos.y;
        xf.pos.z += obj->xf.pos.z;
        const PhobConfig* cfg = c->ownConfig ? &c->config : &s->config;
        int surf = c->ownConfig ? c->config.surf : s->config.surf;
        SpawnPhob* p = fn_801D32E0(s->model, c->mask, &xf, &at, cfg, obj);
        Vec f;
        Scale(&f, &v, c->forceScale);
        ValVec3 hit = *(const ValVec3*)pos;
        p->Push(&f, (const Vec*)&hit, surf, 0);
        f32 pm = p->mass / 0.016683351f;
        f.x = c->throwVel.x * pm;
        f.y = c->throwVel.y * pm;
        f.z = c->throwVel.z * pm;
        p->Push(&f, &xf.pos, surf, 0);
        f.Set(fn_80129DC4(4, 0.0f, 1.0f), fn_80129DC4(4, 0.0f, 1.0f), fn_80129DC4(4, 0.0f, 1.0f));
        if (!((f32)__fabs(PSVECDotProduct(&f, &f)) <= lbl_80558DC4[0])) {
            PSVECNormalize(&f, &f);
            f32 r = fn_80129DC4(4, 0.0f, c->randScale);
            f.x *= r;
            f.y *= r;
            f.z *= r;
            p->Push(&f, &xf.pos, surf, 0);
        }
    }
    }
}

}
