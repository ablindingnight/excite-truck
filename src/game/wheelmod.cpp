// Wheel models (0x801EFF4C-0x801F1C24, "wheelmod"): wheel classes loaded from "<class>.val" (tire/rim
// diameter and width, up to 8 distance LODs of left/right models in slow- and fast-spin remaps, and a
// shadow), the wheels of each car (registered per car and wheel slot), their pose from the car's wheel
// state, LOD/shadow drawing, and an animation object that draws an attached car wheel. Names are
// guesses apart from the strings.
#include "game/common.h"

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

extern "C" {
void* memset(void* p, int c, u32 n);
char* strncpy(char* d, const char* s, u32 n);
char* strchr(const char* s, int c);
int strcasecmp(const char* a, const char* b);
int sprintf(char* buf, const char* fmt, ...);
int sscanf(const char* s, const char* fmt, ...);
int fn_801345D8(char* buf, int size, const char* fmt, ...);  // snprintf
int fn_8012C33C(const char* name);                            // file exists
ValueTable* fn_80131808(const char* name);                    // load a .val
void fn_80131818(ValueTable* t);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
int fn_802C6FC0(const char* name, int remap, int b);  // load a model
void fn_802C6CF8(int model);
void fn_802C70AC(int model, const ValMatrix* m, int a, u32 mask);
void fn_802C7AE0(int model, const ValMatrix* m, f32 height, int a);  // shadow
void fn_802C77F0(int model, f32* bounds);
f32 fn_802C88E8(const ValVec3* p);  // squared distance to the camera
f32 fn_801E88F0(int view);          // zoom
int fn_801EA03C(s16* hint, ValVec3* p);  // ground below
struct Mat3;
void fn_80015A20(Mat3* out, const ValVec3* angles);
void fn_80297988(void* obj);
}

struct Mat3 {
    ValVec3 axis[3];
};

void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);

extern f32 lbl_80558DC4[];  // FLT_EPSILON

// A model's material list (fn_802C6B44).
struct MatList {
    s32 count;      // offset 0x0
    char* names;    // offset 0x4: 0x10 bytes each
};

// A material remap entry (fn_802C7888).
struct Remap {
    char from[0x10];  // offset 0x0
    s32 remap;        // offset 0x10
    char to[0x10];    // offset 0x14
    u8 pad24[0x2C - 0x24];
};

extern "C" {
MatList* fn_802C6B44(const char* model);
void fn_802C6C20(MatList* l);
const char* fn_802C9C24(const char* name);
void fn_802C7888(Remap* r, int n);
}

// A ValueNode without inline bodies (no weak copies here).
class WNode {
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
    virtual ValVec3 GetVec3Or(const char* key, const ValVec3& def);
    virtual ValPoint GetPoint(const char* key);
    virtual ValPoint GetPointOr(const char* key, const ValPoint& def);
    virtual ValMatrix GetMatrix(const char* key);
    virtual ValMatrix GetMatrixOr(const char* key, const ValMatrix& def);
    virtual f32 GetReal(const char* key);
    virtual f32 GetRealOr(const char* key, f32 def);
    virtual int GetEnum(const char* key, const char* names);
    virtual f32 GetScale(const char* key, f32 t);
    virtual WNode* FindChild(const char* name);
};

class WTable {
public:
    virtual WNode* FindChild(const char* name);  // 0x8
    virtual WNode* GetChild(int index);          // 0xC
    virtual int HasChild(const char* name);      // 0x10
};

struct WheelLod {
    s32 slow[2];  // offset 0x0: left/right model, slow-spin remap
    s32 fast[2];  // offset 0x8: left/right model, fast-spin remap
    f32 dist2;    // offset 0x10
    u8 spin;      // offset 0x14: the lod's flags contain 'y'
};

struct ShadowLod {
    s32 model;  // offset 0x0
    f32 dist2;  // offset 0x4
};

struct WheelShadow {
    ShadowLod lods[8];  // offset 0x0
    s32 count;          // offset 0x40
    f32 baseLen;        // offset 0x44
    f32 offDist2;       // offset 0x48
};

struct WheelClass {
    char name[0x10];     // offset 0x0
    s32 slowRemap;       // offset 0x10
    s32 fastRemap;       // offset 0x14
    f32 tireRadius;      // offset 0x18
    f32 rimRadius;       // offset 0x1C
    f32 width;           // offset 0x20
    f32 scale;           // offset 0x24
    WheelLod lods[8];    // offset 0x28
    s32 numLods;         // offset 0xE8
    WheelShadow shadow;  // offset 0xEC
};

// A car's wheel state (the physics side).
struct WheelState {
    ValVec3 pos;   // offset 0x0
    u8 padC[0x28 - 0xC];
    f32 steer;     // offset 0x28
    f32 roll;      // offset 0x2C
    f32 speed;     // offset 0x30
    f32 spin;      // offset 0x34
    u8 pad38[0x60 - 0x38];
    u32 flags;     // offset 0x60
};

struct Wheel {
    WheelClass* cls;  // offset 0x0
    u32 state;        // offset 0x4: 3 = hidden, 0x10 = fast
    s32 car;          // offset 0x8
    s32 index;        // offset 0xC
    s32 side;         // offset 0x10: 0 left, 1 right
    u8 detached;      // offset 0x14
    ValMatrix mtx;    // offset 0x18
    f32 spin;         // offset 0x48
};

static inline void Identity(ValMatrix* m) {
    m->basis[0].x = 1.0f;
    m->basis[0].y = 0.0f;
    m->basis[0].z = 0.0f;
    m->basis[1].x = 0.0f;
    m->basis[1].y = 1.0f;
    m->basis[1].z = 0.0f;
    m->basis[2].x = 0.0f;
    m->basis[2].y = 0.0f;
    m->basis[2].z = 1.0f;
    m->origin.x = 0.0f;
    m->origin.y = 0.0f;
    m->origin.z = 0.0f;
}

struct PiOver4 {
    PiOver4(void) { value = 0.7853982f; }
    f32 value;
};

ModuleTag lbl_8055EBE0;
PiOver4 lbl_8055EBE8;
WheelClass* lbl_8055EBEC;  // class table
s32 lbl_8055EBF0;          // classes loaded
Wheel* lbl_80488798[64];   // by car * 4 + wheel

extern "C" {

void fn_801F0AA0(void);
void fn_801F0CC4(void);
void fn_801F17C8(void);
void fn_801F17DC(void);
Wheel* fn_801F0DC0(Wheel* w, const char* cls, s32 car, s32 index);
Wheel* fn_801F0E88(Wheel* w, const char* cls);
Wheel* fn_801F0F18(Wheel* w, int del);
WheelClass* fn_801F09FC(const char* name);
void fn_801F1878(Wheel* w);
void fn_801F18F0(Wheel* w);
int fn_801F15A8(Wheel* w, const ValVec3* pos);
void fn_801F163C(Wheel* w, int lod, f32 spin);
s32 fn_801F196C(s32 car, s32 index);
u32 fn_801F1978(u32 state);
f32 fn_801F159C(Wheel* w);
void fn_801F1394(Wheel* w, const ValMatrix* m);
void fn_801F1494(Wheel* w, const ValMatrix* m, s16 hint);
Wheel* fn_801F1848(s32 car, s32 index);

void fn_801EFF4C(void) {
    fn_801F0AA0();
    fn_801F17C8();
}

void fn_801EFF70(void) {
    fn_801F17DC();
    fn_801F0CC4();
}

Wheel* fn_801EFF94(const char* cls, s32 car, s32 index) {
    Wheel* w = (Wheel*)operator new(sizeof(Wheel), "wheel");
    if (w) {
        w = fn_801F0DC0(w, cls, car, index);
    }
    return w;
}

void fn_801EFFF8(Wheel* w) { fn_801F0F18(w, 1); }

void fn_801F0000(Wheel* w) { fn_801F0F18(w, -1); }

s32 fn_801F0008(void) { return sizeof(Wheel); }

// A detached wheel built in 'mem'.
Wheel* fn_801F0010(const char* cls, Wheel* mem) {
    if (mem) {
        mem = fn_801F0E88(mem, cls);
    }
    return mem;
}

// From a function the linker stripped (a heap-allocated detached wheel).
Wheel* WheelModStripped0(const char* cls) { return fn_801F0010(cls, (Wheel*)operator new(sizeof(Wheel), "detached wheel")); }

// Adds the material remaps for 'model' listed in 't' to remap set 'remap'.
void fn_801F004C(const char* model, s32 remap, WNode* t) {
    MatList* l = fn_802C6B44(model);
    for (int i = 0; i < l->count; i++) {
        const char* name = fn_802C9C24(l->names + i * 0x10);
        if (t->Has(name)) {
            const char* to = t->GetValue(name);
            Remap r;
            memset(&r, 0, sizeof(Remap));
            strncpy(r.from, name, 0x10);
            r.from[0xF] = 0;
            strncpy(r.to, to, 0x10);
            r.to[0xF] = 0;
            r.remap = remap;
            fn_802C7888(&r, 1);
        }
    }
    fn_802C6C20(l);
}

void fn_801F0148(WheelClass* c, WNode* t) {
    c->tireRadius = 0.0254f * t->GetFloat("tire_diameter") / 2;
    c->rimRadius = 0.0254f * t->GetFloat("rim_diameter") / 2;
    c->width = 0.0254f * t->GetFloat("width");
    c->scale = 1.0f;
}

void fn_801F0208(WheelClass* c, WNode* t, const char* file) {
    WheelLod* lod = c->lods;
    int i;
    for (i = 0; i < 8; i++) {
        char key[0x20];
        sprintf(key, "lod%d", i);
        if (!t->Has(key)) {
            break;
        }
        f32 dist = 0.0f;
        char flags[0x20] = "n";
        if (sscanf(t->GetValue(key), "%f %s", &dist, flags) != 2) {
            fn_802E317C("wheelmod: bad lod entry in %s: %s", file, t->GetValue(key));
        }
        lod->dist2 = dist * dist;
        lod->spin = strchr(flags, 'y') != 0;
        lod++;
    }
    c->numLods = i;
}

void fn_801F0488(WheelClass* c, WNode* t) {
    const char* base = t->GetValue("base");
    int first = 0;
    for (int i = 0; i < c->numLods; i++) {
        for (int j = 0; j < 2; j++) {
            const char* sides[2] = {"L", "R"};
            char path[0x40];
            sprintf(path, "%s%d%s.mod", base, i, sides[j]);
            if (!fn_8012C33C(path)) {
                sprintf(path, "%s%d.mod", base, i);
            }
            if (!fn_8012C33C(path) && i == 0) {
                sprintf(path, "%s1%s.mod", base, sides[j]);
                if (!fn_8012C33C(path)) {
                    sprintf(path, "%s1.mod", base);
                }
                first = 1;
            }
            if (i != 1 || !first) {
                if (t->FindChild("slow_remap")) {
                    fn_801F004C(path, c->slowRemap, t->FindChild("slow_remap"));
                }
                if (t->FindChild("fast_remap")) {
                    fn_801F004C(path, c->fastRemap, t->FindChild("fast_remap"));
                }
            }
            c->lods[i].slow[j] = fn_802C6FC0(path, c->slowRemap, 0);
            c->lods[i].fast[j] = fn_802C6FC0(path, c->fastRemap, 0);
        }
    }
    if (!((f32)__fabs(c->width) <= lbl_80558DC4[0])) {
        f32 b[6];
        fn_802C77F0(c->lods[0].slow[0], b);
        c->scale = c->width / (b[1] - b[0]);
    }
}

void fn_801F06DC(WheelShadow* s, WNode* t) {
    if (t->Has("model_base")) {
        const char* base = t->GetValue("model_base");
        for (s->count = 0; s->count < 8; s->count++) {
            char path[0x40];
            char key[0x20];
            fn_801345D8(path, 0x40, "%s%d.mod", base, s->count + 1);
            fn_801345D8(key, 0x20, "lod%d", s->count);
            if (!fn_8012C33C(path)) {
                break;
            }
            s->lods[s->count].model = fn_802C6FC0(path, 0, 0);
            f32 d = t->GetFloat(key);
            s->lods[s->count].dist2 = d * d;
        }
        s->baseLen = t->GetFloat("base_len");
        f32 off = t->GetFloat("off_dist", 500.0f);
        s->offDist2 = off * off;
    } else {
        s->lods[0].model = fn_802C6FC0(t->GetValue("model"), 0, 0);
        s->lods[0].dist2 = 0.0f;
        s->count = 1;
        s->baseLen = t->GetFloat("dist");
    }
}

void fn_801F08AC(WheelClass* c, const char* name) {
    strncpy(c->name, name, 0x10);
    c->name[0xF] = 0;
    char file[0x20];
    sprintf(file, "%s.val", name);
    if (!fn_8012C33C(file)) {
        fn_802E317C("wheelmod: invalid wheel class '%s'", name);
    }
    WTable* t = (WTable*)fn_80131808(file);
    fn_801F0148(c, t->FindChild("dimensions"));
    fn_801F0208(c, t->FindChild("lod"), file);
    fn_801F0488(c, t->FindChild("model"));
    if (t->HasChild("shadow")) {
        fn_801F06DC(&c->shadow, t->FindChild("shadow"));
    } else {
        c->shadow.count = 0;
    }
    fn_80131818((ValueTable*)t);
}

WheelClass* fn_801F09FC(const char* name) {
    for (int i = 0; i < lbl_8055EBF0; i++) {
        if (!strcasecmp(lbl_8055EBEC[i].name, name)) {
            return &lbl_8055EBEC[i];
        }
    }
    WheelClass* c = &lbl_8055EBEC[lbl_8055EBF0];
    lbl_8055EBF0++;
    fn_801F08AC(c, name);
    return c;
}

void fn_801F0AA0(void) {
    lbl_8055EBEC = new ("wheelmod class tab") WheelClass[8];
    lbl_8055EBF0 = 0;
    for (int i = 0; i < 8; i++) {
        WheelClass* c = &lbl_8055EBEC[i];
        c->name[0] = 0;
        c->slowRemap = i * 2;
        c->fastRemap = i * 2 + 1;
        for (int l = 0; l < 8; l++) {
            for (int j = 0; j < 2; j++) {
                c->lods[l].slow[j] = -1;
                c->lods[l].fast[j] = -1;
            }
            c->lods[l].dist2 = -1.0f;
            c->lods[l].spin = 0;
        }
        c->numLods = 0;
        c->shadow.count = 0;
    }
}

// From a function the linker stripped.
void WheelModStripped(void) { fn_802E317C("wheelmod: can't change classes of an attached wheel"); }

void fn_801F0CC4(void) {
    WheelLod* lod;
    int i;
    int l;
    int j;
    int k;
    for (i = 0; i < lbl_8055EBF0; i++) {
        lod = lbl_8055EBEC[i].lods;
        for (l = 0; l < lbl_8055EBEC[i].numLods; l++) {
            for (j = 0; j < 2; j++) {
                if (lod->slow[j] != -1) {
                    fn_802C6CF8(lod->slow[j]);
                }
                if (lod->fast[j] != -1) {
                    fn_802C6CF8(lod->fast[j]);
                }
            }
            lod++;
        }
        for (k = 0; k < lbl_8055EBEC[i].shadow.count; k++) {
            fn_802C6CF8(lbl_8055EBEC[i].shadow.lods[k].model);
        }
    }
    delete[] lbl_8055EBEC;
    lbl_8055EBEC = 0;
    lbl_8055EBF0 = 0;
}

Wheel* fn_801F0DC0(Wheel* w, const char* cls, s32 car, s32 index) {
    w->cls = fn_801F09FC(cls);
    int left = 0;
    w->state = left;
    w->car = car;
    w->index = index;
    if (index == 0 || index == 2) {
        left = 1;
    }
    w->side = !left;
    w->detached = 0;
    Identity(&w->mtx);
    fn_801F1878(w);
    return w;
}

Wheel* fn_801F0E88(Wheel* w, const char* cls) {
    w->cls = fn_801F09FC(cls);
    w->state = 0;
    w->car = -1;
    w->index = -1;
    w->side = 0;
    w->detached = 1;
    Identity(&w->mtx);
    return w;
}

Wheel* fn_801F0F18(Wheel* w, int del) {
    if (w) {
        if (!w->detached) {
            fn_801F18F0(w);
        }
        if (del > 0) {
            operator delete(w);
        }
    }
    return w;
}

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void MulVec(ValVec3* out, const ValVec3* in, const ValVec3* m) {
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

static inline void SetVec(ValVec3* v, f32 x, f32 y, f32 z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

static inline void AddVec(ValVec3* v, const ValVec3* a) {
    v->x += a->x;
    v->y += a->y;
    v->z += a->z;
}

// Poses wheel 'w' from its state 's' in the car frame 'm', scaled by 'scale'.
void fn_801F0F78(Wheel* w, const WheelState* s, const ValMatrix* m, f32 scale) {
    if (s->flags & 8) {
        w->state = 3;
        return;
    } else if (s->flags & 2) {
        w->state = 2;
    } else if (s->flags & 4) {
        w->state = 1;
    } else {
        w->state = 0;
    }
    if (s->speed > 30.0f) {
        w->state |= 0x10;
    }
    ValVec3 pos = s->pos;
    pos.y += fn_801F159C(w);
    w->spin = s->spin;
    int left = w->index == 0 || w->index == 2;
    MulVec(&w->mtx.origin, &pos, m->basis);
    w->mtx.origin.x += m->origin.x;
    w->mtx.origin.y += m->origin.y;
    w->mtx.origin.z += m->origin.z;
    *(Mat3*)w->mtx.basis = *(const Mat3*)m->basis;
    ValVec3 roll;
    SetVec(&roll, 0.0f, 0.0f, s->roll);
    Mat3 r;
    fn_80015A20(&r, &roll);
    Mat3Mul((Mat3*)&w->mtx, &r, (Mat3*)&w->mtx);
    ValVec3 steer;
    SetVec(&steer, 0.0f, s->steer + (left ? 0.0f : 3.1415927f), 0.0f);
    Mat3 r2;
    fn_80015A20(&r2, &steer);
    Mat3Mul((Mat3*)&w->mtx, &r2, (Mat3*)&w->mtx);
    if (!((f32)__fabs(scale - 1.0f) <= lbl_80558DC4[0])) {
        w->mtx.basis[0].x *= scale;
        w->mtx.basis[0].y *= scale;
        w->mtx.basis[0].z *= scale;
        w->mtx.basis[1].x *= scale;
        w->mtx.basis[1].y *= scale;
        w->mtx.basis[1].z *= scale;
        w->mtx.basis[2].x *= scale;
        w->mtx.basis[2].y *= scale;
        w->mtx.basis[2].z *= scale;
    }
}

void fn_801F131C(Wheel* w, f32 unused, int lod0) {
    if (w->state != 3) {
        fn_801F163C(w, lod0 ? 0 : fn_801F15A8(w, &w->mtx.origin), w->spin);
    }
}

void fn_801F1378(Wheel* w, f32 unused) {
    if (w->state != 3) {
        fn_801F163C(w, 0, w->spin);
    }
}

void fn_801F1394(Wheel* w, const ValMatrix* m) {
    fn_802C70AC(w->cls->lods[0].slow[w->side], m, 0, fn_801F1978(0));
}

void fn_801F13F4(Wheel* w, f32 d2, f32 height) {
    WheelShadow* s = &w->cls->shadow;
    if (d2 > s->offDist2 || w->state == 3) {
        return;
    }
    int i;
    for (i = s->count - 1; i >= 0 && d2 < s->lods[i].dist2; i--) {
    }
    if (i < 0) {
        return;
    }
    fn_802C7AE0(s->lods[i].model, &w->mtx, s->baseLen + height, -1);
}

void fn_801F1474(Wheel* w, const ValMatrix* m, f32 height) {
    if (w->cls->shadow.count > 0) {
        fn_802C7AE0(w->cls->shadow.lods[0].model, m, height, -1);
    }
}

static inline f32 Mul(f32 a, f32 b) { return a * b; }

void fn_801F1494(Wheel* w, const ValMatrix* m, s16 hint) {
    WheelShadow* s = &w->cls->shadow;
    f32 zoom = fn_801E88F0(-1);
    zoom = zoom * zoom;
    f32 d2 = Mul(fn_802C88E8(&w->mtx.origin), zoom);
    int i;
    for (i = s->count - 1; i >= 0 && d2 < s->lods[i].dist2; i--) {
    }
    if (d2 < s->offDist2 && i >= 0) {
        ValVec3 p = m->origin;
        if (fn_801EA03C(&hint, &p) && m->origin.y > p.y) {
            fn_802C7AE0(s->lods[i].model, m, s->baseLen + m->origin.y - p.y, -1);
        }
    }
}

f32 fn_801F159C(Wheel* w) { return w->cls->tireRadius; }

int fn_801F15A8(Wheel* w, const ValVec3* pos) {
    f32 zoom = fn_801E88F0(-1);
    zoom = zoom * zoom;
    f32 d2 = Mul(fn_802C88E8(pos), zoom);
    for (int i = 0; i < w->cls->numLods; i++) {
        if (d2 < w->cls->lods[i].dist2) {
            return i;
        }
    }
    return w->cls->numLods - 1;
}

void fn_801F163C(Wheel* w, int lod, f32 spin) {
    int left = w->index == 0 || w->index == 2;
    u32 fast = (w->state >> 4) & 1;
    WheelLod* l = &w->cls->lods[lod];
    ValMatrix m = w->mtx;
    if (l->spin) {
        ValVec3 a;
        a.x = spin * (left ? 1.0f : -1.0f);
        a.y = 0.0f;
        a.z = 0.0f;
        Mat3 r;
        fn_80015A20(&r, &a);
        Mat3Mul((Mat3*)&m, &r, (Mat3*)&m);
    }
    f32 k = w->cls->scale;
    m.basis[0].x *= k;
    m.basis[0].y *= k;
    m.basis[0].z *= k;
    int model;
    if (fast) {
        model = l->fast[w->side];
    } else {
        model = l->slow[w->side];
    }
    fn_802C70AC(model, &m, 0, fn_801F1978(w->state));
}

void fn_801F17C8(void) { memset(lbl_80488798, 0, sizeof(lbl_80488798)); }

void fn_801F17DC(void) {
    for (int i = 0; i < 64; i++) {
        if (lbl_80488798[i]) {
            fn_802E317C("wheelmod: not all wheels deallocated!");
        }
    }
}

Wheel* fn_801F1848(s32 car, s32 index) { return lbl_80488798[fn_801F196C(car, index)]; }

void fn_801F1878(Wheel* w) {
    s32 i = fn_801F196C(w->car, w->index);
    if (lbl_80488798[i]) {
        fn_802E317C("wheelmod: duplicate wheel entry: car %d, wheel %d", w->car, w->index);
    }
    lbl_80488798[i] = w;
}

void fn_801F18F0(Wheel* w) {
    s32 i = fn_801F196C(w->car, w->index);
    if (!lbl_80488798[i]) {
        fn_802E317C("wheelmod: could not find wheel entry: car %d, wheel %d", w->car, w->index);
    }
    lbl_80488798[i] = 0;
}

s32 fn_801F196C(s32 car, s32 index) { return index + car * 4; }

u32 fn_801F1978(u32 state) {
    u32 mask = -1;
    switch (state & ~0x10) {
    case 0:
        mask = 1;
        mask |= 0x100;
        break;
    case 1:
        mask = 2;
        mask |= 0x100;
        break;
    case 2:
        mask = 0x200;
        break;
    default:
        fn_802E317C("wheelmod: can't create model mask (invalid state %x)", state & ~0x10);
        break;
    }
    return mask;
}

}  // extern "C"

struct AnimSlot {
    ValMatrix frame;  // offset 0x0
    s16 flags;        // offset 0x30
};

class AnimObj {
public:
    AnimObj(void);
    virtual ~AnimObj(void) {}
    virtual void vf0C(void);
    virtual void Draw(void) = 0;
    virtual void vf14(void);
    virtual f32 Depth(void);
    virtual void vf1C(void);
    virtual void Set(const AnimSlot* s);
    void Bind(const char* name);
    s16 Flags(void);

    ValMatrix frame;  // offset 0x4
    s32 offset;       // offset 0x34
    s16 flags;        // offset 0x38
};

// The animation slot of a wheel: the frame plus the car and wheel it belongs to.
struct WheelSlot {
    ValMatrix frame;  // offset 0x0
    s16 flags;        // offset 0x30
    s32 car;          // offset 0x34
    s32 index;        // offset 0x38
    f32 f3C;          // offset 0x3C
    u8 attached;      // offset 0x40
};

class WheelAnim : public AnimObj {
public:
    WheelAnim(const char* name);
    virtual ~WheelAnim(void);
    virtual void Draw(void);
    virtual void vf1C(void);
    virtual void Set(const AnimSlot* s);
    WheelSlot slot;  // offset 0x3C
    Wheel* wheel;    // offset 0x80
};

WheelAnim::WheelAnim(const char* name) {
    Bind(name);
    memset(&slot, 0, sizeof(WheelSlot));
    wheel = 0;
    fn_80297988(this);
}

WheelAnim::~WheelAnim(void) {}

void WheelAnim::Draw(void) {
    if (wheel) {
        fn_801F1394(wheel, &frame);
    }
}

void WheelAnim::vf1C(void) {
    if (wheel) {
        fn_801F1494(wheel, &frame, Flags());
    }
}

void WheelAnim::Set(const AnimSlot* s) {
    AnimObj::Set(s);
    slot = *(const WheelSlot*)s;
    if (slot.attached) {
        wheel = fn_801F1848(slot.car, slot.index);
    } else {
        wheel = 0;
    }
}
