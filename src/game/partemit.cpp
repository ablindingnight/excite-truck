// Track particle emitters (0x8029F03C-0x802A0940): "trkeffect_*" emitter definitions from the track
// tables (spew counts, spread points, offsets, particle system and/or chunk spawner, fog fade near the
// camera), the "emitter" instances placed by "partemit.val" (optionally attached to moving objects and
// always on), and the world events that fire them. Names are guesses apart from the strings.
#include "game/common.h"

void* operator new[](size_t size, const char* tag);

struct PVec {
    PVec(void) {}
    f32 x, y, z;
};

// What an emitter fires from: position, velocity, a sound/slot handle and the emitter id.
struct Spawn {
    PVec pos;   // offset 0x0
    PVec vel;   // offset 0xC
    s16 slot;   // offset 0x18
    s32 id;     // offset 0x1C
};

struct SpawnEvent {
    SpawnEvent(void) {}
    s32 w[6];  // offset 0x0
    s16 slot;  // offset 0x18
    s32 id;    // offset 0x1C
};

class Chunks {
public:
    virtual void vf08(void);
    virtual void Update(void);                                                   // 0xC
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void Spawn(const PVec* pos, const PVec* vel, int slot, int a);      // 0x18
};

struct ParticleRef {
    u8 enabled;  // offset 0x0
    s32 sys;     // offset 0x4
    u8 rotating; // offset 0x8
};

struct ChunkRef {
    u8 enabled;    // offset 0x0
    Chunks* sys;   // offset 0x4
    f32 upwards;   // offset 0x8
};

// An emitter definition (0x78).
struct EmitterDef {
    s32 id;            // offset 0x0
    f32 speedScale;    // offset 0x4
    f32 spewK;         // offset 0x8
    f32 spewM;         // offset 0xC
    ValPoint ptK;      // offset 0x10
    ValPoint ptM;      // offset 0x18
    ValPoint vK;       // offset 0x20
    ValPoint vM;       // offset 0x28
    PVec offset;       // offset 0x30
    ValPoint vScale;   // offset 0x3C
    f32 radius;        // offset 0x44
    f32 fogDensity;    // offset 0x48
    u32 fogColor;      // offset 0x4C
    s32 repeat;        // offset 0x50
    PVec dpos;         // offset 0x54
    ParticleRef part;  // offset 0x60
    ChunkRef chunk;    // offset 0x6C
};

struct NameEntry {
    char name[32];  // offset 0x0
    s32 id;         // offset 0x20
};

// A placed emitter (0x34).
struct EmitterInst {
    s32 fx;          // offset 0x0
    PVec pos;        // offset 0x4
    PVec vel;        // offset 0x10
    PVec v;          // offset 0x1C
    s32 emitter;     // offset 0x28
    s16 slot;        // offset 0x2C
    u8 alwaysOn;     // offset 0x2E
    struct Obj* obj; // offset 0x30
};

struct Obj {
    u8 pad[0x60];
    PVec pos;  // offset 0x60
};

// Spawn parameters for one firing.
struct SpawnParams {
    s32 count;      // offset 0x0
    PVec pos;       // offset 0x4
    PVec vel;       // offset 0x10
    ValPoint pt;    // offset 0x1C
    ValPoint v;     // offset 0x24
};

struct CamView {
    ValVec3 axes[3];  // offset 0x0
    ValVec3 pos;      // offset 0x24
};

template <class T>
struct TArray {
    TArray(T* d, s32 c) {
        data = d;
        cap = c;
        n = 0;
    }
    ~TArray(void) {
        data = 0;
        n = 0;
        cap = 0;
    }
    T* data;  // offset 0x0
    s32 n;    // offset 0x4
    s32 cap;  // offset 0x8
};

template <class T, int N>
struct TFixed : TArray<T> {
    TFixed(void) : TArray<T>(items, N) {}
    T items[N];  // offset 0xC
};

typedef int (*EventHandler)(const void* e);

extern "C" {
void* memset(void* p, int c, u32 n);
u32 strlen(const char* s);
int strcmp(const char* a, const char* b);
int strcasecmp(const char* a, const char* b);
int strncmp(const char* a, const char* b, u32 n);
void qsort(void* base, u32 n, u32 size, int (*cmp)(const void*, const void*));
void* fn_800241DC(const void* key, const void* base, u32 n, u32 size, int (*cmp)(const void*, const void*));  // bsearch
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* t);
int fn_801769F8(void);
int fn_801E85A4(void);  // player views
int fn_801E85AC(void);  // max player views
CamView* fn_801E8718(int i);
void fn_801E88B4(s32* x, s32* y, s32* w, s32* h, int i);
void fn_801E9DC4(s16* h);
void fn_801EEFDC(EventHandler h, u8 type);
void fn_801EF024(EventHandler h);
void fn_802029FC(void);
f32 fn_80287BC8(f32 lo, f32 hi);  // random range
s32 fn_8028E3F4(const char* name);
void fn_8028E528(s32 sys);
void fn_8028E584(s32 sys, const PVec* pos, const PVec* vel);
void fn_8028E590(s32 sys, const PVec* pos, const PVec* vel, f32 a);
Chunks* fn_802929F8(const char* name);
void fn_80292B48(Chunks* c);
ValueTable* fn_802AE4C8(int i);
void fn_802C92D0(void);
void fn_802C92D8(void);
void fn_802C92DC(s32 x, s32 y, s32 w, s32 h, u32 color);
void fn_802E30C8(const char* fmt, ...);
f32 PSVECMag(const PVec* v);
f32 PSVECDotProduct(const PVec* a, const PVec* b);
}

extern u8 lbl_8055F73C;

static inline void StrCopy(char* d, int n, const char* s) {
    char* p = d;
    while ((*p++ = *s++) != 0) {
        if (--n <= 0) {
            p[-1] = 0;
            break;
        }
    }
}

static inline void ScaleAdd(PVec* v, f32 k, const PVec* a) {
    v->x = v->x * k + a->x;
    v->y = v->y * k + a->y;
    v->z = v->z * k + a->z;
}

static inline u32 FogColor(u8 r, u8 g, u8 b) { return 0xFF000000 | (r << 16) | (g << 8) | b; }

ModuleTag lbl_8055F810;
s32 lbl_8055F818;  // effect names
s32 lbl_8055F81C;  // emitter definitions
u32* lbl_8055F820; // per-view fog overlay colour
s32 lbl_8055F824;

EmitterDef lbl_804B5070[64];
NameEntry lbl_804B6E70[64];
NameEntry lbl_804B7770[256];
TFixed<EmitterInst, 256> lbl_804B9B7C;

extern "C" {

void fn_8029FEF8(void);
void fn_8029FE00(void);
void fn_8029FD04(void);
void fn_8029FDDC(void);
void fn_8029FFBC(void);
void fn_802A002C(void);
int fn_802A0620(const void* e);
int fn_802A0720(const void* e);
void fn_802A0794(EmitterInst* in);
s32 fn_8029FE6C(const char* name);

void fn_8029F03C(void) {
    lbl_8055F81C = 0;
    lbl_8055F818 = 0;
    memset(lbl_804B7770, 0, sizeof(lbl_804B7770));
    memset(lbl_804B6E70, 0, sizeof(lbl_804B6E70));
    fn_8029FEF8();
    fn_8029FE00();
    fn_8029FD04();
    fn_801EEFDC(fn_802A0620, 10);
    fn_801EEFDC(fn_802A0720, 11);
    lbl_8055F820 = new ("fsrecttab") u32[fn_801E85AC()];
}

void fn_8029F0D0(void) {
    if (lbl_8055F820) {
        delete[] lbl_8055F820;
    }
    lbl_8055F820 = 0;
    fn_801EF024(fn_802A0720);
    fn_801EF024(fn_802A0620);
    fn_8029FDDC();
    fn_8029FFBC();
}

void fn_8029F124(void) {
    fn_802A002C();
    for (int i = 0; i < fn_801E85A4(); i++) {
        lbl_8055F820[i] = 0;
    }
}

void fn_8029F188(void) {
    if (lbl_8055F73C != 1) {
        if (lbl_8055F824 == 0) {
            fn_802E30C8("allow_particle_add is false!!");
        }
        lbl_8055F824 = (lbl_8055F824 + 1) % 5;
    }
    EmitterInst* in;
    for (int i = 0; i < lbl_804B9B7C.n; i++) {
        in = &lbl_804B9B7C.data[i];
        Obj* o = in->obj;
        if (o) {
            in->vel.x = o->pos.x - in->pos.x;
            in->vel.y = o->pos.y - in->pos.y;
            in->vel.z = o->pos.z - in->pos.z;
            ScaleAdd(&in->vel, 0.016683351f, &in->v);
            in->pos.x = o->pos.x;
            in->pos.y = o->pos.y;
            in->pos.z = o->pos.z;
        } else {
            in->vel.x = in->v.x;
            in->vel.y = in->v.y;
            in->vel.z = in->v.z;
        }
        if (in->alwaysOn && !fn_801769F8()) {
            fn_802A0794(in);
        }
    }
}

void fn_8029F300(void) {
    fn_802029FC();
    for (int i = 0; i < fn_801E85A4(); i++) {
        if (fn_801E85A4() == 1) {
            fn_802C92D0();
            if (lbl_8055F820[i] >> 24) {
                s32 x, y, w, h;
                fn_801E88B4(&x, &y, &w, &h, i);
                fn_802C92DC(x, y, w, h, lbl_8055F820[i]);
            }
            fn_802C92D8();
        }
        lbl_8055F820[i] = 0;
    }
}

s32 fn_8029F3BC(const char* name) {
    for (int i = 0; i < lbl_804B9B7C.n; i++) {
        if (strcasecmp(lbl_804B7770[i].name, name) == 0) {
            return lbl_804B7770[i].id;
        }
    }
    return -1;
}

// Attaches the emitters of an effect ("$name" or "name") to a moving object.
void fn_8029F458(const char* name, Obj* obj) {
    char buf[32];
    if (*name == '$') {
        name++;
    }
    StrCopy(buf, 32, name);
    s32 fx = fn_8029F3BC(buf);
    for (int i = 0; i < lbl_804B9B7C.n; i++) {
        if (lbl_804B9B7C.data[i].fx == fx) {
            lbl_804B9B7C.data[i].obj = obj;
        }
    }
}

int fn_8029F510(u8* flag, void* node) {
    if (node) {
        *flag = 1;
    } else {
        *flag = 0;
    }
    return *flag;
}

void fn_8029F534(ParticleRef* p, ValueNode* node) {
    if (fn_8029F510(&p->enabled, node)) {
        p->sys = fn_8028E3F4(node->GetValue("sys_name"));
        p->rotating = node->GetBool("rotating", 0);
    } else {
        p->sys = 0;
    }
}

void fn_8029F5C8(ChunkRef* c, ValueNode* node) {
    if (fn_8029F510(&c->enabled, node)) {
        c->sys = fn_802929F8(node->GetValue("sys_name"));
        c->upwards = node->GetFloat("upwards", 0.0f);
    } else {
        c->sys = 0;
    }
}

// Reads one "trkeffect_<name>" emitter definition.
void fn_8029F658(ValueNode* node) {
    ValVec3 zero3;
    ValPoint zero2;
    zero3.x = 0.0f;
    zero3.y = 0.0f;
    zero3.z = 0.0f;
    zero2.x = 0.0f;
    zero2.y = 0.0f;
    EmitterDef* d = &lbl_804B5070[lbl_8055F81C];
    d->id = node->GetIntValue("id");
    const char* name = node->GetName();
    name += strlen("trkeffect");
    if (*name == '_') {
        name++;
    }
    lbl_804B6E70[lbl_8055F81C].id = d->id;
    StrCopy(lbl_804B6E70[lbl_8055F81C].name, 32, name);
    d->speedScale = node->GetFloat("speed_scale", 0.0f);
    d->spewK = node->GetFloat("spew_k", 0.0f);
    d->spewM = node->GetFloat("spew_m", 0.0f);
    d->ptK = node->GetPointOr("pt_k", ValPoint(zero2));
    d->ptM = node->GetPointOr("pt_m", ValPoint(zero2));
    d->vK = node->GetPointOr("v_k", ValPoint(zero2));
    d->vM = node->GetPointOr("v_m", ValPoint(zero2));
    const ValVec3& off = node->GetVec3Or("offset", ValVec3(zero3));
    d->offset.x = off.x;
    d->offset.y = off.y;
    d->offset.z = off.z;
    d->vScale = node->GetPointOr("v_scale", ValPoint(zero2));
    d->radius = node->GetFloat("radius", 0.001f);
    d->fogDensity = node->GetFloat("fog_density", 0.0f);
    ValVec3 white;
    white.x = 1.0f;
    white.y = 1.0f;
    white.z = 1.0f;
    ValVec3 fc = node->GetVec3Or("fog_color", ValVec3(white));
    d->fogColor = FogColor((int)(255.0f * fc.x), (int)(255.0f * fc.y), (int)(255.0f * fc.z));
    d->repeat = node->GetInt("repeat", 1);
    const ValVec3& dp = node->GetVec3Or("dpos", ValVec3(zero3));
    d->dpos.x = dp.x;
    d->dpos.y = dp.y;
    d->dpos.z = dp.z;
    fn_8029F534(&d->part, node->FindChild("particle"));
    fn_8029F5C8(&d->chunk, node->FindChild("chunk"));
    lbl_8055F81C++;
}

// Reads one placed "emitter".
void fn_8029FB00(ValueNode* node) {
    EmitterInst* in;
    s32 fx = lbl_8055F818;
    in = &lbl_804B9B7C.data[lbl_804B9B7C.n++];
    for (int i = 0; i < lbl_8055F818; i++) {
        if (strcmp(lbl_804B7770[i].name, node->GetValue("name")) == 0) {
            fx = lbl_804B7770[i].id;
        }
    }
    in->fx = fx;
    in->alwaysOn = node->GetBool("always_on", 0);
    in->emitter = fn_8029FE6C(node->GetValue("sys_name"));
    const ValVec3& pt = node->GetVec3("pt");
    in->pos.x = pt.x;
    in->pos.y = pt.y;
    in->pos.z = pt.z;
    const ValVec3& v = node->GetVec3("v");
    in->v.x = v.x;
    in->v.y = v.y;
    in->v.z = v.z;
    in->obj = 0;
    in->vel.x = v.x;
    in->vel.y = v.y;
    in->vel.z = v.z;
    fn_801E9DC4(&in->slot);
    if (fx == lbl_8055F818) {
        lbl_804B7770[lbl_8055F818].id = fx;
        StrCopy(lbl_804B7770[lbl_8055F818].name, 32, node->GetValue("name"));
        lbl_8055F818++;
    }
}

void fn_8029FD04(void) {
    lbl_804B9B7C.n = 0;
    ValueTable* t = fn_80131810("partemit.val");
    if (t) {
        int i = 0;
        ValueNode* n = t->GetChild(i++);
        while (n) {
            if (strcasecmp(n->GetName(), "emitter") == 0) {
                fn_8029FB00(n);
            }
            n = t->GetChild(i++);
        }
        fn_80131818(t);
    }
}

void fn_8029FDDC(void) { lbl_804B9B7C.n = 0; }

int fn_8029FDF0(const void* a, const void* b) { return *(const s32*)a - *(const s32*)b; }

void fn_8029FE00(void) { qsort(lbl_804B5070, lbl_8055F81C, sizeof(EmitterDef), fn_8029FDF0); }

EmitterDef* fn_8029FE1C(s32 id) {
    if (id <= 0) {
        return 0;
    }
    return (EmitterDef*)fn_800241DC(&id, lbl_804B5070, lbl_8055F81C, sizeof(EmitterDef), fn_8029FDF0);
}

s32 fn_8029FE6C(const char* name) {
    for (int i = 0; i < lbl_8055F81C; i++) {
        if (strcasecmp(lbl_804B6E70[i].name, name) == 0) {
            return lbl_804B6E70[i].id;
        }
    }
    return -1;
}

void fn_8029FEF8(void) {
    ValueTable* t;
    for (int i = 0; (t = fn_802AE4C8(i)) != 0; i++) {
        for (int j = 0; t->GetChild(j); j++) {
            ValueNode* n = t->GetChild(j);
            if (strncmp(n->GetName(), "trkeffect", 9) == 0) {
                fn_8029F658(n);
            }
        }
    }
}

void fn_8029FFBC(void) {
    EmitterDef* d = lbl_804B5070;
    for (int i = 0; i < lbl_8055F81C; d++, i++) {
        if (d->part.sys) {
            fn_8028E528(d->part.sys);
        }
        if (d->chunk.sys) {
            fn_80292B48(d->chunk.sys);
        }
    }
}

void fn_802A002C(void) {
    EmitterDef* d = lbl_804B5070;
    for (int i = 0; i < lbl_8055F81C; d++, i++) {
        if (d->chunk.sys) {
            d->chunk.sys->Update();
        }
    }
}

void fn_802A0098(SpawnParams* p, const EmitterDef* d, const Spawn* s) {
    f32 k = d->speedScale * PSVECMag(&s->vel);
    k = 1.0f < k ? 1.0f : k;
    p->count = d->spewK + d->spewM * k;
    p->pos.x = s->pos.x + d->offset.x;
    p->pos.y = s->pos.y + d->offset.y;
    p->pos.z = s->pos.z + d->offset.z;
    p->vel.x = s->vel.x * d->vScale.x;
    f32 vy = s->vel.y;
    p->vel.y = vy * d->vScale.y;
    p->vel.z = s->vel.z * d->vScale.x;
    p->pt.x = d->ptK.x + d->ptM.x * k;
    p->pt.y = d->ptK.y + d->ptM.y * k;
    p->v.x = d->vK.x + d->vM.x * k;
    p->v.y = d->vK.y + d->vM.y * k;
}

static inline f32 Rand(f32 r) { return fn_80287BC8(-r, r); }

void fn_802A01C8(const EmitterDef* d, const Spawn* s) {
    SpawnParams p;
    fn_802A0098(&p, d, s);
    for (int i = 0; i < p.count; i++) {
        PVec pos;
        PVec vel;
        f32 z = p.pos.z + Rand(p.pt.x);
        f32 y = p.pos.y + Rand(p.pt.y);
        pos.x = p.pos.x + Rand(p.pt.x);
        pos.y = y;
        pos.z = z;
        z = p.vel.z + Rand(p.v.x);
        y = p.vel.y + Rand(p.v.y);
        vel.x = p.vel.x + Rand(p.v.x);
        vel.y = y;
        vel.z = z;
        if (d->part.rotating) {
            fn_8028E590(d->part.sys, &pos, &vel, 5.0f);
        } else {
            fn_8028E584(d->part.sys, &pos, &vel);
        }
    }
}

void fn_802A02F4(const EmitterDef* d, const Spawn* s) {
    SpawnParams p;
    fn_802A0098(&p, d, s);
    p.vel.y += d->chunk.upwards;
    for (int i = 0; i < p.count; i++) {
        PVec pos;
        PVec vel;
        f32 z = p.pos.z + Rand(p.pt.x);
        f32 y = p.pos.y + Rand(p.pt.y);
        pos.x = p.pos.x + Rand(p.pt.x);
        pos.y = y;
        pos.z = z;
        z = p.vel.z + Rand(p.v.x);
        y = p.vel.y + Rand(p.v.y);
        vel.x = p.vel.x + Rand(p.v.x);
        vel.y = y;
        vel.z = z;
        d->chunk.sys->Spawn(&pos, &vel, s->slot, -1);
    }
}

// Fogs each player view by its distance to the emitter.
void fn_802A042C(const EmitterDef* d, const Spawn* s) {
    f32 a;
    for (int i = 0; i < fn_801E85A4(); i++) {
        CamView v = *fn_801E8718(i);
        PVec p;
        PVec diff;
        p.x = s->pos.x + d->offset.x;
        p.y = s->pos.y + d->offset.y;
        p.z = s->pos.z + d->offset.z;
        diff.x = p.x - v.pos.x;
        diff.y = p.y - v.pos.y;
        diff.z = p.z - v.pos.z;
        f32 curRadius = d->radius;
        f32 r2 = curRadius * d->radius;
        f32 d2 = PSVECDotProduct(&diff, &diff);
        if (d2 < r2) {
            f32 t = d2 / r2;
            if (1.0f < t) {
                t = 1.0f;
            }
            t = t > 0.0f ? t : 0.0f;
            a = 255.0f * d->fogDensity;
            f32 f = a + t * (0.0f - a);
            int add = f < 0.0f ? f - 0.5f : f + 0.5f;
            int al = add + (lbl_8055F820[i] >> 24);
            al = al > 0 ? al : 0;
            int c = 0xFF;
            if (al < 0xFF) {
                c = al;
            }
            lbl_8055F820[i] = (c << 24) | (d->fogColor & 0xFFFFFF);
        }
    }
}

int fn_802A0620(const void* ev) {
    const SpawnEvent* e = (const SpawnEvent*)ev;
    EmitterDef* d = fn_8029FE1C(e->id);
    SpawnEvent se = *e;
    if (d) {
        for (int i = 0; i < d->repeat; i++) {
            if (d->part.enabled) {
                fn_802A01C8(d, (Spawn*)&se);
            }
            if (d->chunk.enabled) {
                fn_802A02F4(d, (Spawn*)&se);
            }
            ((Spawn*)&se)->pos.x = ((Spawn*)&se)->pos.x + d->dpos.x;
            ((Spawn*)&se)->pos.y = ((Spawn*)&se)->pos.y + d->dpos.y;
            ((Spawn*)&se)->pos.z = ((Spawn*)&se)->pos.z + d->dpos.z;
        }
    }
    return 0x20;
}

int fn_802A0720(const void* ev) {
    const s32* e = (const s32*)ev;
    for (int i = 0; i < lbl_804B9B7C.n; i++) {
        EmitterInst* in = &lbl_804B9B7C.data[i];
        if (in->fx == *e) {
            fn_802A0794(in);
        }
    }
    return 4;
}

void fn_802A0794(EmitterInst* in) {
    Spawn s;
    s.slot = in->slot;
    s.pos.x = in->pos.x;
    s.pos.y = in->pos.y;
    s.pos.z = in->pos.z;
    s.vel.x = in->vel.x;
    s.vel.y = in->vel.y;
    s.vel.z = in->vel.z;
    s.id = in->emitter;
    EmitterDef* d = fn_8029FE1C(s.id);
    if (d) {
        fn_802A042C(d, &s);
        if (d->part.enabled) {
            fn_802A01C8(d, &s);
        }
        if (d->chunk.enabled) {
            fn_802A02F4(d, &s);
        }
    } else {
        fn_802E30C8("Hey DUDE!  I can't find an effect with id %d.", s.id);
    }
    in->slot = s.slot;
}

}  // extern "C"
