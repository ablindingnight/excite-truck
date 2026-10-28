// Particle manager (0x8028DF58-0x80291784): particle systems built from .val tables ("Max_particles",
// "Spew_rate", colour/size/alpha ramps...), shared by name with a reference count, kept in a ring buffer
// of particles per system, sorted by priority bucket and texture for drawing, and drawn as camera-facing
// quads in batches of eight (plain, animated flipbook, and rotating/shaded variants). Names are guesses
// apart from the strings.
#include "game/common.h"
#include <string.h>

extern "C" {
void fn_802E317C(const char* fmt, ...);  // fatal
void fn_802E30C8(const char* fmt, ...);  // warning print
int strcasecmp(const char* a, const char* b);
int strncasecmp(const char* a, const char* b, size_t n);
double fmod(double x, double y);
ValueNode* fn_802AE414(const char* name);
f32 fn_80129DC4(int seed, f32 lo, f32 hi);  // random float
f32 fn_80287BC8(f32 lo, f32 hi);            // random float
int fn_80287B5C(int n);                     // random int
f32 fn_801E7E18(void);                      // game time
s32 fn_802CAC28(const char* name, int flags);  // load texture
void fn_802CAC54(s32 tex);
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
f32 fn_802C9A4C(f32 x);
void fn_802C8DB8(void* verts, int nverts, const u16* idx, int n);
f32 PSVECDotProduct(const void* a, const void* b);
}

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);
void operator delete(void* p);

extern f32 lbl_80558DC4[];  // FLT_EPSILON

typedef ValVec3 V3;

struct CamMtx {
    V3 right;  // offset 0x0
    V3 up;     // offset 0xC
    V3 fwd;    // offset 0x18
    V3 pos;    // offset 0x24
};

extern "C" CamMtx* fn_802C86DC(void);

struct PartParams {
    PartParams(void);
    s32 type;          // offset 0x0
    s32 maxParticles;  // offset 0x4
    f32 maxLife;       // offset 0x8
    f32 spewRate;      // offset 0xC
    V3 spread;         // offset 0x10
    V3 velScale;       // offset 0x1C
    f32 upKick;        // offset 0x28
    f32 angSpread;     // offset 0x2C
    f32 gravity;       // offset 0x30
    f32 drag;          // offset 0x34
    u8 randomRot;      // offset 0x38
    f32 farDist;       // offset 0x3C
    f32 nearAlpha;     // offset 0x40
    s32 priority;      // offset 0x44
    f32 shading;       // offset 0x48
    f32 sizeAccel;     // offset 0x4C
    f32 sizeRamp;      // offset 0x50
    f32 sizeConst;     // offset 0x54
    f32 alphaRamp;     // offset 0x58
    f32 alphaConst;    // offset 0x5C
    f32 redRamp;       // offset 0x60
    f32 redConst;      // offset 0x64
    f32 greenRamp;     // offset 0x68
    f32 greenConst;    // offset 0x6C
    f32 blueRamp;      // offset 0x70
    f32 blueConst;     // offset 0x74
    f32 angRamp;       // offset 0x78
    f32 angConst;      // offset 0x7C
    f32 animRamp;      // offset 0x80
    s32 frames;        // offset 0x84
    u8 randomFrame;    // offset 0x88
    u8 pad89;          // offset 0x89
};

struct Particle {
    Particle(void);
    V3 pos;      // offset 0x0
    V3 vel;      // offset 0xC
    f32 birth;   // offset 0x18
    u8 rot;      // offset 0x1C
    u8 frame;    // offset 0x1D
    u8 spin;     // offset 0x1E
    f32 angVel;  // offset 0x20
    f32 size;    // offset 0x24
};

struct PVtx {
    V3 pos;     // offset 0x0
    u32 color;  // offset 0xC
    f32 u, v;   // offset 0x10
};

struct QuadBatch {
    QuadBatch(void) {
        for (int i = 0; i < 8; i++) {
            idx[i * 6 + 0] = i * 4;
            idx[i * 6 + 1] = i * 4 + 1;
            idx[i * 6 + 2] = i * 4 + 2;
            idx[i * 6 + 3] = i * 4;
            idx[i * 6 + 4] = i * 4 + 2;
            idx[i * 6 + 5] = i * 4 + 3;
        }
        n = 0;
    }
    void Add(const PVtx* q) {
        if (n >= 8) {
            fn_802C8DB8(v, n * 4, idx, n * 6);
            n = 0;
        }
        v[n * 4 + 0] = q[0];
        v[n * 4 + 1] = q[1];
        v[n * 4 + 2] = q[2];
        v[n * 4 + 3] = q[3];
        n++;
    }
    void Flush(void) {
        if (n > 0) {
            fn_802C8DB8(v, n * 4, idx, n * 6);
            n = 0;
        }
    }
    PVtx v[32];   // offset 0x0
    u16 idx[48];  // offset 0x300
    s32 n;        // offset 0x360
};

struct PartSys : PartParams {
    PartSys(const PartParams* p, const char* tex);
    ~PartSys(void);
    void SetParams(const PartParams* p);
    void Reset(void);
    void Update(f32 t);
    void Draw(void);
    void Add(const V3* pos, const V3* vel, f32 angVel, f32 size);
    void Integrate(V3* out, const Particle* p, f32 age);
    void DrawSprites(Particle* p, int n);
    void DrawAnimated(Particle* p, int n);
    void DrawRotating(Particle* p, int n);
    int CanEmit(void) { return accum >= 1.0f - lbl_80558DC4[0] && head != tail; }
    Particle* parts;  // offset 0x8C
    s32 head;         // offset 0x90
    s32 tail;         // offset 0x94
    s32 cap;          // offset 0x98
    f32 accum;        // offset 0x9C
    f32 invLife;      // offset 0xA0
    s32 tex;          // offset 0xA4
    s32 grid;         // offset 0xA8
    f32 cell;         // offset 0xAC
};

struct SysList {
    SysList(void) { count = 0; }
    PartSys* items[64];  // offset 0x0
    s32 count;        // offset 0x100
};

struct TypeEntry {
    char name[16];  // offset 0x0
    PartSys* sys;      // offset 0x10
    s32 refs;       // offset 0x14
};

struct TypeTable {
    TypeTable(void) { count = 0; }
    TypeEntry e[64];  // offset 0x0
    s32 count;        // offset 0x600
    virtual void Add(PartSys* s, const char* name);
    virtual int Release(PartSys* s);
};

typedef void (PartSys::*DrawFn)(Particle*, int);

ModuleTag lbl_8055F730;
f32 lbl_8055F738;  // particle clock
u8 lbl_8055F73C;   // between pre- and post-update
u8 lbl_8055F73D;
PartSys* lbl_8055F740;  // first system in draw order
u8 lbl_8055F744;     // needs sorting
s32 lbl_8055F748;
u8 lbl_8055F74C;  // warned about adds outside the update

SysList lbl_804B34B0;    // every system, in draw order
SysList lbl_804B35B4;    // systems made from explicit params
TypeTable lbl_804B36B8;  // systems shared by table name

extern "C" {

void fn_8028ED74(SysList* l);
void fn_8028ED80(SysList* l);
void fn_8028ED8C(SysList* l, PartSys* s);
int fn_8028EDA8(SysList* l, PartSys* s);
void fn_8028EE1C(TypeTable* t);
void fn_8028EE28(TypeTable* t);
int fn_8028EE34(TypeTable* t, const char* name, s32* idx);
void fn_8028F2EC(SysList* l);
void fn_8028F5A0(const void* owner, PartSys* s);
void fn_8028F5A4(void);
void fn_8028FAE4(PartSys* s, const V3* pos, const V3* dir, f32 size);
void fn_8028FB98(PartSys* s, const V3* pos, const V3* dir, f32 scale);
void fn_8028FC40(PartSys* s, const V3* pos, const V3* dir, f32 angVel);
void fn_8028E654(PartParams* p, ValueNode* vn);

void fn_8028DF58(void) {
    fn_8028ED74(&lbl_804B34B0);
    fn_8028ED74(&lbl_804B35B4);
    fn_8028EE1C(&lbl_804B36B8);
    lbl_8055F740 = 0;
    lbl_8055F744 = 1;
    lbl_8055F748 = 0;
}

void fn_8028DFB0(void) {
    if (lbl_804B34B0.count > 0) {
        fn_8028F2EC(&lbl_804B34B0);
        lbl_8055F740 = lbl_804B34B0.items[0];
    }
    lbl_8055F744 = 0;
}

void fn_8028DFFC(void) {
    fn_8028DF58();
    lbl_8055F73D = 1;
}

void fn_8028E024(void) {
    if (lbl_804B34B0.count > 0) {
        fn_8028F2EC(&lbl_804B34B0);
        lbl_8055F740 = lbl_804B34B0.items[0];
    }
    lbl_8055F744 = 0;
}

void fn_8028E070(f32 t) {
    lbl_8055F738 = t;
    lbl_8055F73C = 1;
}

void fn_8028E080(void) {
    lbl_8055F73C = 0;
    for (int i = 0; i < lbl_804B34B0.count; i++) {
        lbl_804B34B0.items[i]->Update(lbl_8055F738);
    }
}

void fn_8028E0F0(void) {
    fn_8028F5A4();
    fn_8028EE28(&lbl_804B36B8);
    fn_8028ED80(&lbl_804B35B4);
    fn_8028ED80(&lbl_804B34B0);
}

void fn_8028E138(void) {
    lbl_8055F738 = 0.0f;
    for (int i = 0; i < lbl_804B34B0.count; i++) {
        lbl_804B34B0.items[i]->Reset();
    }
}

void fn_8028E1A4(void) {
    lbl_8055F738 = fn_801E7E18();
    lbl_8055F73C = 1;
}

void fn_8028E1D0(void) {
    lbl_8055F73C = 0;
    for (int i = 0; i < lbl_804B34B0.count; i++) {
        lbl_804B34B0.items[i]->Update(lbl_8055F738);
    }
}

void fn_8028E240(void) {
    s32 t;
    if (lbl_8055F740) {
        f32 old = fn_802C9A4C(0.0f);
        s32 tex = lbl_8055F740->tex;
        fn_802C8BD0(tex);
        for (int i = 0; i < lbl_804B34B0.count; i++) {
            t = lbl_804B34B0.items[i]->tex;
            if (t != tex) {
                fn_802C8D74();
                fn_802C8BD0(t);
                tex = t;
            }
            lbl_804B34B0.items[i]->Draw();
        }
        fn_802C8D74();
        fn_802C9A4C(old);
    }
}

PartSys* fn_8028E2F8(const PartParams* p, const void* owner, const char* tex) {
    PartSys* s = new ("partman") PartSys(p, tex);
    fn_8028ED8C(&lbl_804B35B4, s);
    fn_8028ED8C(&lbl_804B34B0, s);
    fn_8028F5A0(owner, s);
    return s;
}

void fn_8028E398(PartSys* s) {
    if (fn_8028EDA8(&lbl_804B35B4, s)) {
        fn_8028EDA8(&lbl_804B34B0, s);
        delete s;
    }
}

PartSys* fn_8028E3F4(const char* name) {
    s32 idx = -1;
    if (fn_8028EE34(&lbl_804B36B8, name, &idx)) {
        TypeEntry* e = &lbl_804B36B8.e[idx];
        e->refs++;
        return e->sys;
    }
    PartParams p;
    ValueNode* vn = fn_802AE414(name);
    if (!vn) {
        fn_802E317C("particle: could not find table %s", name);
    }
    fn_8028E654(&p, vn);
    PartSys* s = new ("partman") PartSys(&p, vn->GetValue("texture"));
    lbl_804B36B8.Add(s, name);
    fn_8028ED8C(&lbl_804B34B0, s);
    fn_8028F5A0(name, s);
    return s;
}

void fn_8028E528(PartSys* s) {
    if (lbl_804B36B8.Release(s)) {
        fn_8028EDA8(&lbl_804B34B0, s);
        delete s;
    }
}

void fn_8028E584(PartSys* s, const V3* pos, const V3* dir) { fn_8028FB98(s, pos, dir, 1.0f); }

void fn_8028E58C(PartSys* s, const V3* pos, const V3* dir, f32 size) { fn_8028FAE4(s, pos, dir, size); }

void fn_8028E590(PartSys* s, const V3* pos, const V3* dir, f32 angVel) { fn_8028FC40(s, pos, dir, angVel); }

static const char* lbl_80348CF8[3] = {"Simple", "Animated", "Rotating"};

int fn_8028E594(const char* name) {
    for (int i = 0; i < 3; i++) {
        if (strcasecmp(name, lbl_80348CF8[i]) == 0) {
            return i;
        }
    }
    fn_802E317C("particle: could not find type '%s'", name);
    return 3;
}

}  // extern "C"

PartParams::PartParams(void) { memset(this, 0, sizeof(PartParams)); }

extern "C" {

struct V3c : V3 {
    V3c(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
};

void fn_8028E654(PartParams* p, ValueNode* vn) {
    memset(p, 0, sizeof(PartParams));
    p->type = fn_8028E594(vn->GetValue("Type"));
    p->maxParticles = vn->GetIntValue("Max_particles");
    p->maxLife = vn->GetFloat("Max_life");
    p->spewRate = vn->GetFloat("Spew_rate");
    f32 xz = vn->GetFloat("Random_spread_(xz)", 0.0f);
    f32 y = vn->GetFloat("Random_spread_(y)", 0.0f);
    V3c spread(xz, y, xz);
    p->spread = spread;
    xz = vn->GetFloat("Velocity_scale_(xz)", 0.0f);
    y = vn->GetFloat("Velocity_scale_(y)", 0.0f);
    V3c vs(xz, y, xz);
    p->velScale = vs;
    p->upKick = vn->GetFloat("Upwards_kick", 0.0f);
    p->angSpread = vn->GetFloat("Angular_spread", 0.0f);
    p->priority = vn->GetInt("Priority", 5);
    p->shading = vn->GetFloat("Shading", 0.0f);
    p->gravity = vn->GetFloat("Gravity", 0.0f);
    p->drag = vn->GetFloat("Drag", 0.0f);
    p->randomRot = vn->GetBool("Random_init_rot", 1);
    p->sizeAccel = vn->GetFloat("Size_acceleration", 0.0f);
    p->sizeRamp = vn->GetFloat("Size_ramp", 0.0f);
    p->sizeConst = vn->GetFloat("Size_constant", 0.0f);
    p->alphaRamp = vn->GetFloat("Alpha_ramp", 0.0f);
    p->alphaConst = vn->GetFloat("Alpha_constant", 0.0f);
    p->angRamp = vn->GetFloat("Angular_ramp", 0.0f);
    p->angConst = vn->GetFloat("Angular_constant", 0.0f);
    p->farDist = vn->GetFloat("Far_distance", 1.0f);
    p->nearAlpha = vn->GetFloat("Near_alpha", 0.0f);
    if (vn->Has("Bright_ramp") || vn->Has("Bright_constant")) {
        p->blueRamp = p->greenRamp = p->redRamp = vn->GetFloat("Bright_ramp", 0.0f);
        p->blueConst = p->greenConst = p->redConst = vn->GetFloat("Bright_constant", 0.0f);
    } else if (vn->Has("Start_color") || vn->Has("End_color")) {
        ValVec3 start = vn->GetVec3Or("Start_color", V3c(0.0f, 0.0f, 0.0f));
        ValVec3 end = vn->GetVec3Or("End_color", V3c(0.0f, 0.0f, 0.0f));
        p->redRamp = end.x - start.x;
        p->redConst = start.x;
        p->greenRamp = end.y - start.y;
        p->greenConst = start.y;
        p->blueRamp = end.z - start.z;
        p->blueConst = start.z;
    } else {
        p->redRamp = vn->GetFloat("Red_ramp", 0.0f);
        p->redConst = vn->GetFloat("Red_constant", 0.0f);
        p->greenRamp = vn->GetFloat("Green_ramp", 0.0f);
        p->greenConst = vn->GetFloat("Green_constant", 0.0f);
        p->blueRamp = vn->GetFloat("Blue_ramp", 0.0f);
        p->blueConst = vn->GetFloat("Blue_constant", 0.0f);
    }
    if (vn->Has("tint_override")) {
        ValVec3 tint = vn->GetVec3Or("tint_override", V3c(0.0f, 0.0f, 0.0f));
        p->redRamp = 0.0f;
        p->redConst = tint.x;
        p->greenRamp = 0.0f;
        p->greenConst = tint.y;
        p->blueRamp = 0.0f;
        p->blueConst = tint.z;
    }
    p->animRamp = vn->GetFloat("Animation_ramp", 0.0f);
    p->frames = vn->GetInt("#Frames", 1);
    p->randomFrame = vn->GetBool("Random_initial_frame", 0);
}

void fn_8028ED74(SysList* l) { l->count = 0; }

void fn_8028ED80(SysList* l) { l->count = 0; }

void fn_8028ED8C(SysList* l, PartSys* s) {
    l->items[l->count] = s;
    l->count++;
}

int fn_8028EDA8(SysList* l, PartSys* s) {
    for (int i = 0; i < l->count; i++) {
        if (l->items[i] == s) {
            l->items[i] = 0;
            return 1;
        }
    }
    fn_802E317C("particle: could not find system %x in bucket", s);
    return 0;
}

void fn_8028EE1C(TypeTable* t) { t->count = 0; }

void fn_8028EE28(TypeTable* t) { t->count = 0; }

int fn_8028EE34(TypeTable* t, const char* name, s32* idx) {
    for (int i = 0; i < t->count; i++) {
        if (strncasecmp(name, t->e[i].name, 15) == 0) {
            *idx = i;
            return 1;
        }
    }
    return 0;
}

}  // extern "C"

void TypeTable::Add(PartSys* s, const char* name) {
    strncpy(e[count].name, name, 16);
    e[count].name[15] = 0;
    e[count].sys = s;
    e[count].refs = 1;
    count++;
}

int TypeTable::Release(PartSys* s) {
    for (int i = 0; i < count; i++) {
        if (e[i].sys == s) {
            return --e[i].refs == 0;
        }
    }
    fn_802E317C("particle: could not find system %x in bucket", s);
    return 0;
}

extern "C" {

void fn_8028EFC4(SysList* buckets, SysList* l) {
    for (int i = 0; i < l->count; i++) {
        fn_8028ED8C(&buckets[l->items[i]->priority - 1], l->items[i]);
    }
}

s32 fn_8028F040(SysList* a, SysList* b, s32 tex) {
    for (int i = 0; i < a->count; i++) {
        s32 t = a->items[i]->tex;
        if (t != tex) {
            int found = 0;
            PartSys** q = b->items;
            int j = 0;
            while (j < b->count) {
                PartSys* o = *q++;
                j++;
                found = o->tex == t;
                if (found) {
                    break;
                }
            }
            if (found) {
                return t;
            }
        }
    }
    return -2;
}

s32 fn_8028F0C0(SysList* buckets, int i, s32 tex) {
    for (int k = i + 1; k < 9; k++) {
        if (buckets[k].count > 0) {
            return fn_8028F040(&buckets[i], &buckets[k], tex);
        }
    }
    return -2;
}

void fn_8028F114(SysList* l, s32 first, s32 last) {
    PartSys* out[64];
    int nFirst = 0;
    int nMid = 0;
    int nLast = 0;
    memset(out, 0, sizeof(out));
    int i;
    if (first != -2) {
        PartSys** o = out;
        for (i = 0; i < l->count; i++) {
            if (l->items[i]->tex == first) {
                *o++ = l->items[i];
                nFirst++;
            }
        }
    }
    if (last != -2) {
        for (int j = 0; j < l->count; j++) {
            if (l->items[j]->tex == last) {
                out[i - nLast - 1] = l->items[j];
                nLast++;
            }
        }
    }
    PartSys** mid = out + nFirst;
    for (int j = 0; j < l->count; j++) {
        PartSys* s = l->items[j];
        s32 t = s->tex;
        if (t != first && t != last) {
            int found = 0;
            for (int k = 0; k < nMid; k++) {
                found = mid[k]->tex == t;
                if (found) {
                    break;
                }
            }
            if (!found) {
                mid[nMid++] = s;
                for (int k = j + 1; k < l->count; k++) {
                    if (l->items[k]->tex == t) {
                        mid[nMid++] = l->items[k];
                    }
                }
            }
        }
    }
    memcpy(l->items, out, l->count * sizeof(PartSys*));
}

void fn_8028F2EC(SysList* all) {
    SysList buckets[9];
    fn_8028EFC4(buckets, all);
    SysList* b = buckets;
    s32 prev = -2;
    int off = 0;
    for (int i = 0; i < 9; i++) {
        s32 next = fn_8028F0C0(buckets, i, prev);
        fn_8028F114(b, prev, next);
        memcpy(&all->items[off], b->items, b->count * sizeof(PartSys*));
        off += b->count;
        prev = next;
        b++;
    }
}

void fn_8028F444(V3* out, const V3* dir, const V3* spread, const V3* scale, f32 kick, f32 k) {
    out->x = dir->x * scale->x + k * fn_80129DC4(0, -spread->x, spread->x);
    out->y = dir->y * scale->y + k * fn_80129DC4(0, -spread->y, spread->y);
    out->z = dir->z * scale->z + k * fn_80129DC4(0, -spread->z, spread->z);
    out->y += k * kick;
}

int fn_8028F534(int n) {
    for (int i = 0; i < 4; i++) {
        if (n == 1 << (i * 2)) {
            return 1 << i;
        }
    }
    fn_802E317C("particle: power_of_4_sqrt failed on %d", n);
    return 0;
}

void fn_8028F5A0(const void* owner, PartSys* s) {}

void fn_8028F5A4(void) {}

}  // extern "C"

struct PartTweak {
    virtual ~PartTweak(void);
};

extern "C" void fn_802916E4(PartTweak* t);

PartTweak::~PartTweak(void) { fn_802916E4(this); }

static inline f32 Wave(f32 a) {
    f32 x = (f32)fmod(a, 6.283185307179586) - 3.1415927f;
    if (!(*(u32*)&x & 0x80000000)) {
        f32 t = 2.0f * x / 3.1415927f - 1.0f;
        return 1.0f - t * t;
    }
    x = -x;
    f32 t = 2.0f * x / 3.1415927f - 1.0f;
    return -(1.0f - t * t);
}

extern "C" void fn_8028F608(V3* a, V3* b, f32 angle, const CamMtx* m) {
    f32 s = Wave(angle);
    f32 c = Wave(1.5707964f + angle);
    a->x = c * m->right.x + s * m->up.x;
    a->y = c * m->right.y + s * m->up.y;
    a->z = c * m->right.z + s * m->up.z;
    b->x = c * m->up.x - s * m->right.x;
    b->y = c * m->up.y - s * m->right.y;
    b->z = c * m->up.z - s * m->right.z;
}

PartSys::PartSys(const PartParams* p, const char* tex) {
    SetParams(p);
    s32 n = p->maxParticles;
    parts = new ("particles") Particle[n];
    cap = n;
    this->tex = fn_802CAC28(tex, 0);
}

inline Particle::Particle(void) {}

PartSys::~PartSys(void) {
    fn_802CAC54(tex);
    delete[] parts;
}

void PartSys::Reset(void) {
    head = -1;
    tail = 0;
    accum = 0.0f;
}

void PartSys::Update(f32 t) {
    if (accum >= 1.0f - lbl_80558DC4[0]) {
        accum -= 1.0f;
    }
    int i = head;
    accum += spewRate;
    if (i < 0) {
        return;
    }
    f32 cutoff = t - maxLife;
    if (i < tail) {
        while (i < tail && parts[i].birth < cutoff) {
            i++;
        }
        if (i != tail) {
            head = i;
            return;
        }
        head = -1;
        tail = 0;
        return;
    }
    while (i < cap && parts[i].birth < cutoff) {
        i++;
    }
    if (i == cap) {
        int j = 0;
        while (j < tail && parts[j].birth < cutoff) {
            j++;
        }
        if (j != tail) {
            head = j;
            return;
        }
        head = -1;
        tail = 0;
        return;
    }
    head = i;
}

static const DrawFn lbl_80348FD0[3] = {&PartSys::DrawSprites, &PartSys::DrawAnimated, &PartSys::DrawRotating};

void PartSys::Draw(void) {
    if (head < 0) {
        return;
    }
    if (head < tail) {
        (this->*lbl_80348FD0[type])(parts + head, tail - head);
    } else {
        (this->*lbl_80348FD0[type])(parts + head, cap - head);
        (this->*lbl_80348FD0[type])(parts, tail);
    }
}

extern "C" {

void fn_8028FAE4(PartSys* s, const V3* pos, const V3* dir, f32 size) {
    if (s->CanEmit()) {
        V3 v;
        fn_8028F444(&v, dir, &s->spread, &s->velScale, s->upKick, 1.0f);
        s->Add(pos, &v, 0.0f, size);
    }
}

void fn_8028FB98(PartSys* s, const V3* pos, const V3* dir, f32 scale) {
    if (s->CanEmit()) {
        V3 v;
        fn_8028F444(&v, dir, &s->spread, &s->velScale, s->upKick, scale);
        s->Add(pos, &v, 0.0f, 1.0f);
    }
}

void fn_8028FC40(PartSys* s, const V3* pos, const V3* dir, f32 angVel) {
    if (s->CanEmit()) {
        V3 v;
        fn_8028F444(&v, dir, &s->spread, &s->velScale, s->upKick, 1.0f);
        s->Add(pos, &v, angVel + fn_80287BC8(-s->angSpread, s->angSpread), 1.0f);
    }
}

}  // extern "C"

static inline f32 Clamp01(f32 x) {
    x = 1.0f < x ? 1.0f : x;
    return x > 0.0f ? x : 0.0f;
}

void PartSys::SetParams(const PartParams* p) {
    *(PartParams*)this = *p;
    invLife = 1.0f / p->maxLife;
    switch (p->type) {
    case 1:
        grid = fn_8028F534(frames);
        cell = 1.0f / (f32)grid;
        break;
    }
    drag = Clamp01(drag);
    Reset();
}

void PartSys::Add(const V3* pos, const V3* vel, f32 angVel, f32 size) {
    if (!lbl_8055F73C && !lbl_8055F74C) {
        lbl_8055F74C = 1;
        fn_802E30C8("%s", "particle: added particle outside of ParticlePreUpdate() and ParticlePostUpdate()");
    }
    Particle p;
    p.pos = *pos;
    p.vel = *vel;
    p.birth = lbl_8055F738;
    p.size = size;
    switch (type) {
    case 0:
        p.rot = randomRot ? fn_80287B5C(4) : 0;
        break;
    case 1:
        p.frame = randomFrame ? fn_80287B5C(frames) : 0;
        break;
    case 2:
        p.spin = randomRot ? fn_80287B5C(0xFF) : 0;
        p.angVel = angVel;
        break;
    default:
        fn_802E317C("particle: invalid type %d", type);
        break;
    }
    if (head < 0) {
        parts[0] = p;
        head = 0;
        tail = 1;
    } else {
        parts[tail] = p;
        tail = (tail + 1) % cap;
    }
}

void PartSys::Integrate(V3* out, const Particle* p, f32 age) {
    f32 a = age * invLife;
    f32 damp = 1.0f + (0.33f * a - 1.0f) * (drag * a);
    f32 fall = 0.5f * age * (1.0f + (0.33f * a - 0.8888889f) * (1.5f * drag * a));
    V3 v;
    v.x = p->vel.x * damp;
    v.y = p->vel.y * damp - 9.81f * gravity * fall;
    v.z = p->vel.z * damp;
    out->x = p->pos.x + v.x * age;
    out->z = p->pos.z + v.z * age;
    out->y = p->pos.y + v.y * age;
}


static inline u8 Byte(f32 x) {
    int v = (int)x;
    v = v > 0 ? v : 0;
    return v < 0xFF ? v : 0xFF;
}

void PartSys::DrawSprites(Particle* p, int n) {
    CamMtx* cam = fn_802C86DC();
    f32 t0 = lbl_8055F738;
    V3 d1;
    V3 d2;
    d1.x = cam->up.x - cam->right.x;
    d1.y = cam->up.y - cam->right.y;
    d1.z = cam->up.z - cam->right.z;
    d2.x = cam->up.x + cam->right.x;
    d2.y = cam->up.y + cam->right.y;
    d2.z = cam->up.z + cam->right.z;
    QuadBatch batch;
    for (int i = 0; i < n; p++, i++) {
        f32 age = t0 - p->birth;
        f32 t = age * invLife;
        f32 size = 0.5f * (sizeConst + sizeRamp * age + 0.5f * sizeAccel * (age * age));
        V3 pos;
        Integrate(&pos, p, age);
        CamMtx m = *fn_802C86DC();
        V3 d;
        d.x = m.pos.x - pos.x;
        d.y = m.pos.y - pos.y;
        d.z = m.pos.z - pos.z;
        f32 fade = Clamp01(nearAlpha + PSVECDotProduct(&d, &d) / (1.0f + farDist * farDist) * (1.0f - nearAlpha));
        u8 b = Byte(255.0f * (blueConst + blueRamp * t));
        u8 g = Byte(255.0f * (greenConst + greenRamp * t));
        u8 r = Byte(255.0f * (redConst + redRamp * t));
        u8 a = Byte(p->size * (fade * (255.0f * (alphaConst + alphaRamp * t))));
        u32 color = (a << 24) | (r << 16) | (g << 8) | b;
        int ua = ((p->rot + 1) & 2) >> 1;
        int ub = (p->rot >> 1) & 1;
        PVtx q[4];
        f32 d1X = d1.x;
        q[0].pos.x = pos.x + d1X * size;
        q[0].pos.y = pos.y + d1.y * size;
        q[0].pos.z = pos.z + d1.z * size;
        q[1].pos.x = pos.x + d2.x * size;
        q[1].pos.y = pos.y + d2.y * size;
        q[1].pos.z = pos.z + d2.z * size;
        q[2].pos.x = pos.x + d1.x * -size;
        q[2].pos.y = pos.y + d1.y * -size;
        q[2].pos.z = pos.z + d1.z * -size;
        q[3].pos.x = pos.x + d2.x * -size;
        q[3].pos.y = pos.y + d2.y * -size;
        q[3].pos.z = pos.z + d2.z * -size;
        q[3].color = q[2].color = q[1].color = q[0].color = color;
        q[1].v = q[0].u = ua;
        q[3].u = q[0].v = ub;
        q[2].v = q[1].u = 1 - ub;
        q[3].v = q[2].u = 1 - ua;
        batch.Add(q);
    }
    batch.Flush();
}

void PartSys::DrawAnimated(Particle* p, int n) {
    CamMtx* cam = fn_802C86DC();
    f32 t0 = lbl_8055F738;
    V3 d1;
    V3 d2;
    d1.x = cam->up.x - cam->right.x;
    d1.y = cam->up.y - cam->right.y;
    d1.z = cam->up.z - cam->right.z;
    d2.x = cam->up.x + cam->right.x;
    d2.y = cam->up.y + cam->right.y;
    d2.z = cam->up.z + cam->right.z;
    QuadBatch batch;
    for (int i = 0; i < n; p++, i++) {
        f32 age = t0 - p->birth;
        f32 t = age * invLife;
        f32 size = 0.5f * (sizeConst + sizeRamp * age + 0.5f * sizeAccel * (age * age));
        int frame = (p->frame + (int)(t * ((f32)frames * animRamp))) % frames;
        V3 pos;
        Integrate(&pos, p, age);
        CamMtx m = *fn_802C86DC();
        V3 d;
        d.x = m.pos.x - pos.x;
        d.y = m.pos.y - pos.y;
        d.z = m.pos.z - pos.z;
        f32 fade = Clamp01(nearAlpha + PSVECDotProduct(&d, &d) / (1.0f + farDist * farDist) * (1.0f - nearAlpha));
        u8 b = Byte(255.0f * (blueConst + blueRamp * t));
        u8 g = Byte(255.0f * (greenConst + greenRamp * t));
        u8 r = Byte(255.0f * (redConst + redRamp * t));
        u8 a = Byte(p->size * (fade * (255.0f * (alphaConst + alphaRamp * t))));
        u32 color = (a << 24) | (r << 16) | (g << 8) | b;
        int col = frame % grid;
        int row = frame / grid;
        PVtx q[4];
        q[0].pos.x = pos.x + d1.x * size;
        q[0].pos.y = pos.y + d1.y * size;
        q[0].pos.z = pos.z + d1.z * size;
        q[1].pos.x = pos.x + d2.x * size;
        q[1].pos.y = pos.y + d2.y * size;
        q[1].pos.z = pos.z + d2.z * size;
        q[2].pos.x = pos.x + d1.x * -size;
        q[2].pos.y = pos.y + d1.y * -size;
        q[2].pos.z = pos.z + d1.z * -size;
        q[3].pos.x = pos.x + d2.x * -size;
        q[3].pos.y = pos.y + d2.y * -size;
        q[3].pos.z = pos.z + d2.z * -size;
        q[3].color = q[2].color = q[1].color = q[0].color = color;
        q[3].u = q[0].u = (f32)col * cell;
        q[2].u = q[1].u = (f32)(col + 1) * cell;
        q[1].v = q[0].v = (f32)row * cell;
        q[3].v = q[2].v = (f32)(row + 1) * cell;
        batch.Add(q);
    }
    batch.Flush();
}

void PartSys::DrawRotating(Particle* p, int n) {
    CamMtx* cam = fn_802C86DC();
    f32 t0 = lbl_8055F738;
    f32 shade = 0.5f * shading;
    QuadBatch batch;
    for (int i = 0; i < n; p++, i++) {
        f32 age = t0 - p->birth;
        f32 t = age * invLife;
        f32 size = 0.5f * (sizeConst + sizeRamp * age + 0.5f * sizeAccel * (age * age));
        size = size > 0.001f ? size : 0.001f;
        f32 inv = 1.0f / (1.414f * size);
        f32 ang = (f32)fmod(6.2831855f * (f32)p->spin * 0.00390625f + p->angVel * age * (angConst + angRamp * t),
                            6.283185307179586);
        ang = 6.2831855f + ang;
        V3 pos;
        Integrate(&pos, p, age);
        V3 a;
        V3 b;
        fn_8028F608(&a, &b, ang, cam);
        CamMtx m = *fn_802C86DC();
        V3 d;
        d.x = m.pos.x - pos.x;
        d.y = m.pos.y - pos.y;
        d.z = m.pos.z - pos.z;
        f32 fade = Clamp01(nearAlpha + PSVECDotProduct(&d, &d) / (1.0f + farDist * farDist) * (1.0f - nearAlpha));
        PVtx q[4];
        f32 two = 2.0f * size;
        f32 mtwo = -2.0f * size;
        q[0].pos.x = pos.x + a.x * -size + b.x * size;
        q[0].pos.z = pos.z + a.z * -size + b.z * size;
        f32 posY = pos.y;
        q[0].pos.y = posY + a.y * -size + b.y * size;
        q[1].pos.x = q[0].pos.x + a.x * two;
        q[1].pos.y = q[0].pos.y + a.y * two;
        q[1].pos.z = q[0].pos.z + a.z * two;
        q[2].pos.x = q[1].pos.x + b.x * mtwo;
        q[2].pos.y = q[1].pos.y + b.y * mtwo;
        q[2].pos.z = q[1].pos.z + b.z * mtwo;
        q[3].pos.x = q[0].pos.x + b.x * mtwo;
        q[3].pos.y = q[0].pos.y + b.y * mtwo;
        q[3].pos.z = q[0].pos.z + b.z * mtwo;
        q[0].u = 0.0f;
        q[0].v = 0.0f;
        q[1].u = 1.0f;
        q[1].v = 0.0f;
        q[2].u = 1.0f;
        q[2].v = 1.0f;
        q[3].u = 0.0f;
        q[3].v = 1.0f;
        PVtx* v = q;
        for (int k = 0; k < 4; k++, v++) {
            f32 lit = 255.0f * (1.0f - shade * (1.0f - inv * (v->pos.y - pos.y)));
            u8 al = Byte(p->size * (fade * (255.0f * (alphaConst + alphaRamp * t))));
            u8 r = Byte(lit * (redConst + redRamp * t));
            u8 g = Byte(lit * (greenConst + greenRamp * t));
            u8 bl = Byte(lit * (blueConst + blueRamp * t));
            v->color = (al << 24) | (r << 16) | (g << 8) | bl;
        }
        batch.Add(q);
    }
    batch.Flush();
}

struct PartTweak2 {
    virtual ~PartTweak2(void);
};

PartTweak2::~PartTweak2(void) {}

extern "C" {

int fn_802916DC(void* t) { return 0; }

void fn_802916E4(PartTweak* t) {}

void fn_802916E8(void* t) {}

void fn_802916EC(void* t) {}

s32 fn_802916F0(u8* t) { return *(s32*)(t + 0x4); }

s32 fn_802916F8(u8* t) { return *(s32*)(t + 0x118); }

void fn_80291700(void* t) { fn_802916E8(t); }

void fn_80291704(void* t) { fn_802916EC(t); }

void fn_80291708(u8* t) { t[0x11C] = 1; }

void fn_80291714(u8* t) { t[0x11C] = 0; }

u8 fn_80291720(u8* t) { return t[0x11C]; }

}  // extern "C"
