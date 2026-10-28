// Trees (0x802BC334-0x802BE30C): tree types from "treeclass.val" ("dyno" entries: near/far models and
// their textures, trunk cylinder "cyl_p1"/"cyl_p2"/"rad", "base_rad", "sway_radius", "tree_run",
// "surf_type", "tree_shadow"), the placed trees from "treelist.trl" ('TLST' v2, byte-swapped and relocated
// on load) with their collision cylinders ("tree-volume") and tree-hit registration, the fan-cell grid the
// near/far draw lists are kept in, drawing (far models fade in between 0.375 and 0.475, near models fade
// out between 0.525 and 0.625) and the shadow pass, plus the preload job. Names are guesses apart from
// the strings.
#include "game/common.h"

extern "C" {
double sqrt(double x);
int strcmp(const char* a, const char* b);
int strcasecmp(const char* a, const char* b);
void PSVECCrossProduct(const void* a, const void* b, void* out);
void PSVECNormalize(const void* src, void* dst);
f32 PSVECMag(const void* v);
f32 PSVECSquareDistance(const void* a, const void* b);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
int fn_8012C33C(const char* name);       // resource exists
void* fn_8012C370(const char* name, u32 magic, s32* version, int a, int b, u8* swapped);  // load resource
void fn_8012C4D8(void* p);                                                               // free resource
void fn_80131810(void);
void fn_80131818(ValueTable* table);
s32 fn_802CAC28(const char* name, int a);  // texture handle
void fn_802CAC54(s32 tex);
int fn_801D04D0(const char* name);  // surface type by name
void fn_80127464(void);             // keep alive
void fn_802B8AE0(const char* name, s32* value);
int fn_802B05E4(void);
void fn_802C126C(void);
void fn_802C13A4(void);
void fn_802C1550(void);
int fn_802C143C(ValueNode* cfg);
u8* fn_802C1500(void* tree, int cls);
void fn_802C173C(f32 time);
f32 fn_801E7E18(void);
int fn_8020403C(void);
void* fn_802C28C0(void);
void fn_802C8BD0(void* p);
void fn_802C8D74(void);
void fn_802DC71C(s32 model, s32 tex);
void fn_802DC8B4(void);
void fn_802DBE4C(void);
void fn_802DBECC(void);
s32 fn_801E8718(int i);
}

void* operator new(size_t size, const char* tag);
void operator delete(void* p);

struct TVec {
    f32 x;
    f32 y;
    f32 z;
};

struct TMtx {
    TVec rot[3];  // offset 0x0
    TVec pos;     // offset 0x24
};

struct TModel {
    u8 pad0[0x4];
    struct TModelData* data;  // offset 0x4
};

struct TModelData {
    u8 pad0[0xC];
    s32 id;  // offset 0xC
};

extern "C" {
const char* fn_802C9C24(TModelData* d);
s32 fn_802B110C(s32 (*load)(const char*), const char* name);
TModel* fn_802B2C34(void (*load)(void), const char* name);
void fn_802C6B44(void);
void fn_802C6C20(TModel* m);
s32 fn_802B1DA0(s32 id);
ValueTable* fn_802B561C(void (*load)(void), const char* name);
void fn_802DCAE4(s32 model, const TMtx* m, f32 alpha);
void fn_801F1C24(TMtx* m);
}

// A fan-grid draw cell (0x10): which placed trees it covers.
struct TCell {
    TCell* next;  // offset 0x0
    s16 x;        // offset 0x4
    s16 z;        // offset 0x6
    s16 count;    // offset 0x8
    s16 first;    // offset 0xA
    f32 fade;     // offset 0xC
};

struct FanCell {
    u8 pad0[0x4];
    TCell* head;  // offset 0x4
};

extern "C" FanCell* fn_802BE844(s16 x, s16 z, s16 size);

class CollShape;
class TreeVol {
public:
    u8 pad0[0x28];
    f32 y;  // offset 0x28
};

// A placed tree (0x44).
struct Tree {
    TVec normal;   // offset 0x0
    TMtx mtx;      // offset 0xC
    f32 scale;     // offset 0x3C (in the file: radius)
    f32 drop;      // offset 0x40
};

struct TreeRec {
    u8 pad0[0x4];
    void* hit;      // offset 0x4
    u8 pad8[0x4];
    TMtx mtx;       // offset 0xC
    TreeVol* vol;   // offset 0x3C
    f32 drop;       // offset 0x40
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
    T* At(int i) { return &data[i]; }
    T* data;  // offset 0x0
    s32 n;    // offset 0x4
    s32 cap;  // offset 0x8
};

template <class T>
void SafeDelete(T*& p) {
    if (p) {
        delete p;
        p = 0;
    }
}

// The arrays as stored in the file (count, then data offset).
template <class T>
struct TFileArray {
    u32 n;   // offset 0x0
    T* data; // offset 0x4
};

struct TreeSet {
    ~TreeSet(void) {
        SafeDelete(near);
        SafeDelete(frames);
        SafeDelete(list);
    }
    TArray<TreeRec>* list;  // offset 0x0
    TArray<TCell>* frames;  // offset 0x4
    TArray<TCell>* near;    // offset 0x8
};

// A trunk cylinder.
struct TCyl {
    TVec center;  // offset 0x0
    TVec axis;    // offset 0xC
    f32 half;     // offset 0x18
    f32 rad;      // offset 0x1C
};

// A tree type (0x60).
struct TreeType {
    const char* name;  // offset 0x0
    ValueNode* shadow; // offset 0x4
    TModel* near;      // offset 0x8
    s32 nearId;        // offset 0xC
    s32 nearTex;       // offset 0x10
    s32 nearTris;      // offset 0x14
    TModel* far;       // offset 0x18
    s32 farId;         // offset 0x1C
    s32 farTex;        // offset 0x20
    s32 farTris;       // offset 0x24
    s32 cls;           // offset 0x28
    s32 surf;          // offset 0x2C
    u8 run;            // offset 0x30
    TCyl cyl;          // offset 0x34
    f32 baseRad;       // offset 0x54
    TreeSet* set;      // offset 0x58
    f32 sway;          // offset 0x5C
};

template <class T, int N>
struct TFixed {
    TFixed(void) {
        data = items;
        cap = N;
        n = 0;
    }
    ~TFixed(void) {}
    T* data;     // offset 0x0
    s32 n;       // offset 0x4
    s32 cap;     // offset 0x8
    T items[N];  // offset 0xC
};

struct TListEntry {
    char name[0x20];             // offset 0x0
    TFileArray<TreeRec> list;    // offset 0x20
    TFileArray<TCell> frames;    // offset 0x28
    TFileArray<TCell> near;      // offset 0x30
};

struct TListFile {
    u32 n;               // offset 0x0
    TListEntry* entries; // offset 0x4
};

struct TRot {
    f32 x;      // offset 0x0
    f32 z;      // offset 0x4
    f32 dx;     // offset 0x8
    f32 dz;     // offset 0xC
    f32 scale;  // offset 0x10
};

struct TPos2 {
    f32 x;
    f32 z;
};

extern "C" void* fn_802BA580(ValueNode* cfg, const TPos2* p, f32 r);
extern "C" void fn_802BAF98(void* p);

class CollShape {
public:
    CollShape(u32 t, f32 k, f32 s) {
        mtx = 0;
        owner = 0;
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
    void* owner;       // offset 0x4
    s32 a8;            // offset 0x8
    const TMtx* mtx;   // offset 0xC
    f32 stiff;         // offset 0x10
    f32 skin;          // offset 0x14
    u32 type;          // offset 0x18
    s32 a1C;           // offset 0x1C
    s32 a20;           // offset 0x20
};

class TubeShape : public CollShape {
public:
    TubeShape(f32 r, f32 h) : CollShape('TUBE', 50000.0f, 0.2f) {
        rad = r;
        half = h;
    }
    virtual ~TubeShape(void);
    virtual void vf38(void);
    f32 rad;   // offset 0x24
    f32 half;  // offset 0x28
};

extern "C" TreeVol* fn_801FA8E0(CollShape* s, const TMtx* m, u8* hit);

struct PListNode {
    PListNode* next;
};

class Preloader : public PListNode {
public:
    Preloader(void);
    virtual s32 Size(void) = 0;
    virtual void Load(void) = 0;
    virtual ~Preloader(void);
};

class TreePreloader : public Preloader {
public:
    virtual s32 Size(void);
    virtual void Load(void);
    virtual ~TreePreloader(void);
};

ModuleTag lbl_8055FAA0;
s32 lbl_8055FAA8;  // trees drawn
s32 lbl_8055FAAC;  // triangles drawn
bool lbl_8055FAB0; // trees collide
ValueTable* lbl_8055FAB4;
TListFile* lbl_8055FAB8;
TreePreloader* lbl_8055FABC;
TFixed<TreeType, 16> lbl_804C00D4;

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void Swap32(void* v) {
    u8* p = (u8*)v;
    u8 t = p[0];
    p[0] = p[3];
    p[3] = t;
    t = p[1];
    p[1] = p[2];
    p[2] = t;
}

static inline void Swap16(void* v) {
    u8* p = (u8*)v;
    u8 t = p[0];
    p[0] = p[1];
    p[1] = t;
}

static inline f32 Step(f32 x, f32 lo, f32 hi) { return (x - lo) / (hi - lo); }
static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

extern "C" {

s32 fn_802BC334(const char* name) {
    int ok = 0;
    if (name && fn_8012C33C(name)) {
        ok = 1;
    }
    if (ok) {
        return fn_802CAC28(name, 0);
    }
    return -2;
}

int fn_802BC39C(ValueNode* n) { return strcasecmp("dyno", n->GetName()) == 0; }

int fn_802BC3DC(ValueNode* n) { return fn_802BC39C(n); }

void fn_802BC3E0(s16 x, s16 z, s16 size, TCell* c) {
    FanCell* f = fn_802BE844(x, z, size);
    c->next = f->head;
    f->head = c;
}

s16 fn_802BC424(s16 x) {
    int m = 0x1FF;
    s16 v = x + 0x200;
    while (v & m) {
        m >>= 1;
    }
    return m + 1;
}

void fn_802BC44C(TArray<TCell>* a) {
    TCell* c;
    for (int i = 0; i < a->n; i++) {
        c = a->At(i);
        fn_802BC3E0(c->x, c->z, fn_802BC424(c->x), c);
    }
}

s32 fn_802BC4D4(TModel* m) { return m->data->id; }

s32 fn_802BC4E0(const TModel* m) {
    const char* name = fn_802C9C24(m->data);
    if (!fn_8012C33C(name)) {
        fn_802E317C("tree: Missing texture %s", name);
    }
    return fn_802B110C(fn_802BC334, name);
}

f32 fn_802BC540(const TVec* v, f32 k) {
    f32 x;
    f32 z = v->z;
    f32 xx;
    f32 zz;
    x = v->x;
    xx = x * x;
    zz = z * z;
    return k * (f32)sqrt(xx + zz);
}

}  // extern "C"

static inline void MulVec(TVec* out, const TMtx* m, const TVec* v) {
    if (out == v) {
        f32 x = Dot3(v->x, m->rot[0].x, v->y, m->rot[1].x, v->z, m->rot[2].x);
        f32 y = Dot3(v->x, m->rot[0].y, v->y, m->rot[1].y, v->z, m->rot[2].y);
        f32 z = Dot3(v->x, m->rot[0].z, v->y, m->rot[1].z, v->z, m->rot[2].z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(v->x, m->rot[0].x, v->y, m->rot[1].x, v->z, m->rot[2].x);
        out->y = Dot3(v->x, m->rot[0].y, v->y, m->rot[1].y, v->z, m->rot[2].y);
        out->z = Dot3(v->x, m->rot[0].z, v->y, m->rot[1].z, v->z, m->rot[2].z);
    }
}

extern "C" {

void fn_801EA18C(const TVec* pos, TVec* normal, int a);

void fn_802BC588(TMtx* src, TMtx* dst, TVec* normal, const TRot* r, const TreeType* t) {
    src->pos.x = r->x;
    src->pos.y = 0.0f;
    src->pos.z = r->z;
    src->rot[2].x = r->dx;
    src->rot[2].y = 0.0f;
    src->rot[2].z = r->dz;
    src->rot[1].x = 0.0f;
    src->rot[1].y = 1.0f;
    src->rot[1].z = 0.0f;
    PSVECCrossProduct(&src->rot[1], &src->rot[2], &src->rot[0]);
    TVec up;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    fn_801EA18C(&src->pos, &up, 0);
    *normal = up;
    src->pos.y -= fn_802BC540(&up, t->baseRad);
    TVec c;
    MulVec(&c, src, &t->cyl.center);
    f32 curScale = r->scale;
    c.x *= curScale;
    c.y *= r->scale;
    c.z *= r->scale;
    dst->pos.x = src->pos.x + c.x;
    dst->pos.y = src->pos.y + c.y;
    dst->pos.z = src->pos.z + c.z;
    MulVec(&dst->rot[2], src, &t->cyl.axis);
    dst->rot[0].x = 1.0f;
    dst->rot[0].y = 0.0f;
    dst->rot[0].z = 0.0f;
    PSVECCrossProduct(&dst->rot[2], &dst->rot[0], &dst->rot[1]);
    PSVECCrossProduct(&dst->rot[1], &dst->rot[2], &dst->rot[0]);
    PSVECNormalize(&dst->rot[1], &dst->rot[1]);
    PSVECNormalize(&dst->rot[0], &dst->rot[0]);
    f32 s = r->scale;
    src->rot[0].x *= s;
    src->rot[0].y *= s;
    src->rot[0].z *= s;
    src->rot[1].x *= s;
    src->rot[1].y *= s;
    src->rot[1].z *= s;
    src->rot[2].x *= s;
    src->rot[2].y *= s;
    src->rot[2].z *= s;
}

CollShape* fn_802BC9B0(const TCyl* c, int id, f32 scale) {
    f32 r = c->rad * scale;
    f32 h = c->half * scale;
    TubeShape* s = new ("tree-volume") TubeShape(r, h);
    s->SetId(id);
    return s;
}

void fn_802BCA8C(TreeRec* tr, const TRot* r, const TreeType* t) {
    TMtx m;
    fn_802BC588(&tr->mtx, &m, (TVec*)tr, r, t);
    u8* hit = fn_802C1500(&tr->mtx, t->cls);
    if (lbl_8055FAB0) {
        tr->vol = fn_801FA8E0(fn_802BC9B0(&t->cyl, t->surf, r->scale), &m, hit);
        tr->drop = tr->mtx.pos.y - tr->vol->y;
        if (t->run && t->surf != 0x74) {
            TCyl c = t->cyl;
            c.half = t->cyl.half + 2.5f;
            c.rad = t->cyl.rad + 7.5f;
            fn_801FA8E0(fn_802BC9B0(&c, 1003, r->scale), &m, hit);
        }
    } else {
        tr->vol = 0;
        tr->drop = 0.0f;
    }
    tr->hit = 0;
}

void fn_802BCBDC(void) {
    TreeType* t;
    lbl_8055FAB4 = fn_802B561C(fn_80131810, "treeclass.val");
    if (lbl_8055FAB4) {
        ValueNode* n;
        for (int i = 0; (n = lbl_8055FAB4->GetChild(i)) != 0; i++) {
            if (fn_802BC3DC(n)) {
                t = &lbl_804C00D4.data[lbl_804C00D4.n++];
                t->name = n->GetValue("name");
                t->shadow = n->FindChild("tree_shadow");
                t->far = fn_802B2C34(fn_802C6B44, n->GetValue("far_model"));
                t->farId = fn_802BC4D4(t->far);
                t->farTex = fn_802BC4E0(t->far);
                t->farTris = fn_802B1DA0(t->farId);
                t->near = fn_802B2C34(fn_802C6B44, n->GetValue("near_model"));
                t->nearId = fn_802BC4D4(t->near);
                t->nearTex = fn_802BC4E0(t->near);
                t->nearTris = fn_802B1DA0(t->nearId);
                t->sway = n->GetFloat("sway_radius", 0.0f);
                t->run = n->GetBool("tree_run", 1);
                t->surf = fn_801D04D0(n->GetString("surf_type", "tree"));
                t->cls = fn_802C143C(n);
                ValVec3 p1 = n->GetVec3("cyl_p1");
                ValVec3 p2 = n->GetVec3("cyl_p2");
                TVec c;
                c.y = p1.y + 0.5f * (p2.y - p1.y);
                c.z = p1.z + 0.5f * (p2.z - p1.z);
                c.x = p1.x + 0.5f * (p2.x - p1.x);
                t->cyl.center = c;
                t->cyl.axis.x = p1.x - p2.x;
                t->cyl.axis.y = p1.y - p2.y;
                t->cyl.axis.z = p1.z - p2.z;
                t->cyl.half = PSVECMag(&t->cyl.axis) * 0.5f;
                PSVECNormalize(&t->cyl.axis, &t->cyl.axis);
                t->cyl.rad = n->GetFloat("rad");
                t->baseRad = n->GetFloat("base_rad");
                t->set = 0;
            }
        }
    }
}

void fn_802BCF04(void) {
    TreeType* t;
    for (int i = 0; i < lbl_804C00D4.n; i++) {
        t = &lbl_804C00D4.data[i];
        SafeDelete(t->set);
        fn_802CAC54(t->farTex);
        fn_802C6C20(t->far);
        t->far = 0;
        fn_802CAC54(t->nearTex);
        fn_802C6C20(t->near);
        t->near = 0;
    }
    lbl_804C00D4.n = 0;
    if (lbl_8055FAB4) {
        fn_80131818(lbl_8055FAB4);
        lbl_8055FAB4 = 0;
    }
}

s32 fn_802BD0C8(TreeType* t) {
    s32 n = 0;
    if (t->shadow && t->set) {
        n = t->set->list->n;
    }
    return n;
}

void fn_802BD0F4(TreeType* t) {
    TreeRec* tr;
    if (t->shadow && t->set) {
        int n = t->set->list->n;
        for (int i = 0; i < n; i++) {
            tr = &t->set->list->data[i];
            TPos2 p;
            p.x = tr->mtx.pos.x;
            p.z = tr->mtx.pos.z;
            tr->hit = fn_802BA580(t->shadow, &p, tr->mtx.rot[2].y);
        }
    }
}

TListEntry* fn_802BD194(TListFile* f, const char* name) {
    u32 n;
    TListEntry* es;
    u32 i;
    n = f->n;
    for (i = 0; i < n; i++) {
        es = f->entries;
        if (strcmp(es[i].name, name) == 0) {
            return &es[i];
        }
    }
    return 0;
}

TArray<TreeRec>* fn_802BD414(const char* tag, TFileArray<TreeRec>* f);
TArray<TCell>* fn_802BD478(const char* tag, TFileArray<TCell>* f);
void fn_802BD4DC(TListFile* f, u8* base);
void fn_802BCA8C(TreeRec* tr, const TRot* r, const TreeType* t);

void fn_802BD210(void) {
    TreeRec* tr;
    s32 version = 0;
    u8 swapped;
    lbl_8055FAB8 = (TListFile*)fn_8012C370("treelist.trl", 'TLST', &version, 0, 0, &swapped);
    if (lbl_8055FAB8) {
        if (version == 2) {
            if (swapped) {
                fn_802BD4DC(lbl_8055FAB8, (u8*)lbl_8055FAB8 + 8);
            }
            for (int i = 0; i < lbl_804C00D4.n; i++) {
                TreeType* t = &lbl_804C00D4.data[i];
                TListEntry* e = fn_802BD194(lbl_8055FAB8, t->name);
                if (e) {
                    TreeSet* s = new ("treelist") TreeSet;
                    if (s) {
                        s->list = fn_802BD414("tree:frame_list", &e->list);
                        s->frames = fn_802BD478("tree:near_list", &e->frames);
                        s->near = fn_802BD478("tree:near_list", &e->near);
                        int n = s->list->n;
                        for (int j = 0; j < n; j++) {
                            tr = &s->list->data[j];
                            TRot r;
                            r.x = tr->mtx.pos.x;
                            r.z = tr->mtx.pos.z;
                            r.scale = PSVECMag(&tr->mtx.rot[2]);
                            r.dx = tr->mtx.rot[2].x;
                            r.dz = tr->mtx.rot[2].z;
                            f32 inv = 1.0f / (f32)sqrt(r.dx * r.dx + r.dz * r.dz);
                            r.dx *= inv;
                            r.dz *= inv;
                            fn_802BCA8C(&s->list->data[j], &r, t);
                            if ((j & 0x3F) == 0) {
                                fn_80127464();
                            }
                        }
                        fn_802BC44C(s->frames);
                        fn_802BC44C(s->near);
                    }
                    t->set = s;
                } else {
                    t->set = 0;
                }
            }
        } else {
            fn_802E317C("tree: bad treelist version %d", version);
        }
    }
}

TArray<TreeRec>* fn_802BD414(const char* tag, TFileArray<TreeRec>* f) {
    TArray<TreeRec>* a = new (tag) TArray<TreeRec>(f->data, f->n);
    a->n += f->n;
    return a;
}

TArray<TCell>* fn_802BD478(const char* tag, TFileArray<TCell>* f) {
    TArray<TCell>* a = new (tag) TArray<TCell>(f->data, f->n);
    a->n += f->n;
    return a;
}

TListEntry* fn_802BD5A8(TListEntry* off, u8* base);
void fn_802BD5B0(TFileArray<TCell>* a, u8* base);
void fn_802BD69C(TFileArray<TreeRec>* a, u8* base);

void fn_802BD4DC(TListFile* f, u8* base) {
    TListEntry* e;
    Swap32(&f->n);
    Swap32(&f->entries);
    f->entries = fn_802BD5A8(f->entries, base);
    for (u32 i = 0; i < f->n; i++) {
        e = &f->entries[i];
        fn_802BD69C(&e->list, base);
        fn_802BD5B0(&e->frames, base);
        fn_802BD5B0(&e->near, base);
    }
}

TListEntry* fn_802BD5A8(TListEntry* off, u8* base) { return (TListEntry*)((u8*)off + (u32)base); }

TCell* fn_802BD694(TCell* off, u8* base);

void fn_802BD5B0(TFileArray<TCell>* a, u8* base) {
    TCell* c;
    Swap32(&a->n);
    Swap32(&a->data);
    a->data = fn_802BD694(a->data, base);
    for (u32 i = 0; i < a->n; i++) {
        c = &a->data[i];
        Swap16(&c->x);
        Swap16(&c->z);
        Swap16(&c->count);
        Swap16(&c->first);
        c->fade = -1.0f;
    }
}

TCell* fn_802BD694(TCell* off, u8* base) { return (TCell*)((u8*)off + (u32)base); }

TreeRec* fn_802BD80C(TreeRec* off, u8* base);

void fn_802BD69C(TFileArray<TreeRec>* a, u8* base) {
    Swap32(&a->n);
    Swap32(&a->data);
    a->data = fn_802BD80C(a->data, base);
    for (u32 i = 0; i < a->n; i++) {
        f32* m = (f32*)&a->data[i].mtx;
        for (int k = 0; k < 12; k++) {
            Swap32(&m[k]);
        }
    }
}

TreeRec* fn_802BD80C(TreeRec* off, u8* base) { return (TreeRec*)((u8*)off + (u32)base); }

static inline bool Live(const TCell* c) { return c->fade >= 0.0f; }

static inline void Settle(TreeRec* tr) {
    if (tr->vol && tr->mtx.pos.y > -2000.0f) {
        tr->mtx.pos.y = tr->vol->y + tr->drop;
    }
}

void fn_802BD814(void) {
    lbl_8055FAAC = 0;
    lbl_8055FAA8 = 0;
    for (int i = 0; i < lbl_804C00D4.n; i++) {
        TreeType* t = &lbl_804C00D4.data[i];
        if (t->set) {
            TArray<TCell>* frames = t->set->frames;
            TArray<TreeRec>* list = t->set->list;
            fn_802C8BD0(fn_802C28C0());
            fn_802DC71C(t->nearId, t->nearTex);
            for (int j = 0; j < frames->n; j++) {
                TCell* c = &frames->data[j];
                f32 k = Step(c->fade, 0.375f, 0.475f);
                k = (1.0f < k) ? 1.0f : k;
                k = (k > 0.0f) ? k : 0.0f;
                f32 a = Lerp(0.0f, 1.0f, k);
                if (Live(c) && a > 0.1f) {
                    s32 id = t->nearId;
                    s32 tris = t->nearTris;
                    for (int n = c->first; n < c->first + c->count; n++) {
                        TreeRec* tr = &list->data[n];
                        Settle(tr);
                        fn_802DCAE4(id, &tr->mtx, a);
                        lbl_8055FAA8++;
                        lbl_8055FAAC += tris;
                    }
                    c->fade = -1.0f;
                }
            }
            fn_802DC8B4();
            fn_802C8D74();
            TArray<TCell>* near = t->set->near;
            TArray<TreeRec>* list2 = t->set->list;
            if (t->farTris <= 10) {
                fn_802C8BD0(fn_802C28C0());
                fn_802DC71C(t->farId, t->farTex);
                TMtx cam;
                fn_801F1C24(&cam);
                for (int j = 0; j < near->n; j++) {
                    TCell* c = &near->data[j];
                    if (Live(c)) {
                        f32 k = Step(c->fade, 0.525f, 0.625f);
                        k = (1.0f < k) ? 1.0f : k;
                        k = (k > 0.0f) ? k : 0.0f;
                        f32 a = Lerp(1.0f, 0.0f, k);
                        if (a > 0.1f) {
                            s32 id = t->farId;
                            s32 tris = t->farTris;
                            for (int n = c->first; n < c->first + c->count; n++) {
                                TreeRec* tr = &list2->data[n];
                                Settle(tr);
                                TMtx m;
                                f32 s = tr->mtx.rot[2].y;
                                m.rot[0].x = cam.rot[0].x * s;
                                m.rot[0].y = cam.rot[0].y * s;
                                m.rot[0].z = cam.rot[0].z * s;
                                m.rot[1].x = cam.rot[1].x * s;
                                m.rot[1].y = cam.rot[1].y * s;
                                m.rot[1].z = cam.rot[1].z * s;
                                m.rot[2].x = cam.rot[2].x * s;
                                m.rot[2].y = cam.rot[2].y * s;
                                m.rot[2].z = cam.rot[2].z * s;
                                m.pos = tr->mtx.pos;
                                fn_802DCAE4(id, &m, a);
                                lbl_8055FAA8++;
                                lbl_8055FAAC += tris;
                            }
                        }
                        c->fade = -1.0f;
                    }
                }
                fn_802DC8B4();
                fn_802C8D74();
            }
        }
    }
}

void fn_802BDBA8(void) {
    fn_802C126C();
    fn_802BCBDC();
    fn_802B8AE0("tree-count", &lbl_8055FAA8);
    fn_802B8AE0("tree-tri-count", &lbl_8055FAAC);
    int x = fn_802B05E4();
    lbl_8055FAB0 = x == 0;
    if (x) {
        fn_802BD210();
    }
    lbl_8055FABC = new ("tree-ixfg") TreePreloader;
}

void fn_802BDC3C(void) {
    fn_802BD210();
    fn_802C1550();
}

void fn_802BDCA4(TreePreloader** p);

void fn_802BDC60(void) {
    fn_802BCF04();
    if (lbl_8055FAB8) {
        fn_8012C4D8(lbl_8055FAB8);
        lbl_8055FAB8 = 0;
    }
    fn_802C13A4();
    fn_802BDCA4(&lbl_8055FABC);
}

void fn_802BDCA4(TreePreloader** p) {
    if (*p) {
        delete *p;
        *p = 0;
    }
}

}  // extern "C"

TreePreloader::~TreePreloader(void) {}

extern "C" {

void fn_802BDD54(void) { fn_802BD814(); }

void fn_802BDD58(void) {
    fn_802C173C(fn_801E7E18());
    if (fn_8020403C()) {
        fn_802BD814();
    }
}

void fn_802BDD8C(f32 range) {
    fn_802DBE4C();
    if (fn_8020403C()) {
        lbl_8055FAAC = 0;
        lbl_8055FAA8 = 0;
        for (int i = 0; i < lbl_804C00D4.n; i++) {
            TreeType* t = &lbl_804C00D4.data[i];
            if (t->set) {
                TArray<TCell>* near = t->set->near;
                TArray<TreeRec>* list = t->set->list;
                if (t->farTris <= 10) {
                    fn_802C8BD0(fn_802C28C0());
                    fn_802DC71C(t->farId, t->farTex);
                    f32 r2 = range * range;
                    f32 in = range - 50.0f;
                    f32 span = r2 - in * in;
                    TMtx cam;
                    fn_801F1C24(&cam);
                    const TVec* eye = (const TVec*)(fn_801E8718(-1) + 0x24);
                    for (int j = 0; j < near->n; j++) {
                        TCell* c = &near->data[j];
                        if (Live(c)) {
                            s32 id = t->farId;
                            for (int n = c->first; n < c->first + c->count; n++) {
                                TreeRec* tr = &list->data[n];
                                f32 d = PSVECSquareDistance(&tr->mtx.pos, eye);
                                if (d <= r2) {
                                    f32 a = (r2 - d) / span;
                                    a = (1.0f < a) ? 1.0f : a;
                                    TMtx m;
                                    f32 s = tr->mtx.rot[2].y;
                                    m.rot[0].x = cam.rot[0].x * s;
                                    m.rot[0].y = cam.rot[0].y * s;
                                    m.rot[0].z = cam.rot[0].z * s;
                                    m.rot[1].x = cam.rot[1].x * s;
                                    m.rot[1].y = cam.rot[1].y * s;
                                    m.rot[1].z = cam.rot[1].z * s;
                                    m.rot[2].x = cam.rot[2].x * s;
                                    m.rot[2].y = cam.rot[2].y * s;
                                    m.rot[2].z = cam.rot[2].z * s;
                                    m.pos = tr->mtx.pos;
                                    fn_802DCAE4(id, &m, a);
                                }
                            }
                        }
                    }
                    fn_802DC8B4();
                    fn_802C8D74();
                }
            }
        }
    }
    fn_802DBECC();
}

void fn_802BDFBC(FanCell* f, int lod) {
    f32 v;
    if (lod < 8) {
        v = 1.0f;
    } else if (lod != 8) {
        v = 0.0f;
    }
    for (TCell* c = f->head; c; c = c->next) {
        c->fade = v;
    }
}

void fn_802BDFF4(TMtx* m) {
    TreeRec* tr = (TreeRec*)((u8*)m - 0xC);
    if (tr->hit) {
        fn_802BAF98(tr->hit);
    }
}

TreeType* fn_802BE008(TCell* c) {
    TreeType* t = lbl_804C00D4.data;
    for (int i = 0; i < lbl_804C00D4.n; i++, t++) {
        if (t->set) {
            TArray<TCell>* a = t->set->near;
            int in = 0;
            TCell* d = a->data;
            if (c >= d && c < d + a->n) {
                in = 1;
            }
            if (in) {
                return t;
            }
        }
    }
    return 0;
}

typedef void (*TreeQuery)(s32 handle, const TVec* pos, int size, int n, f32 r, f32 h);

void fn_802BE07C(const TVec* pos, TreeQuery cb) {
    s16 cz = (s16)(int)(pos->z / 2.0f);
    s16 cx = (s16)(int)(pos->x / 2.0f);
    s16 bx = (cx / 128) * 128 + 0x40;
    s16 bz = (cz / 128) * 128 + 0x40;
    for (int i = -0x80; i <= 0x80; i += 0x80) {
        for (int j = -0x80; j <= 0x80; j += 0x80) {
            FanCell* f = fn_802BE844(bx + i, bz + j, 0x40);
            for (TCell* c = f->head; c; c = c->next) {
                TreeType* t = fn_802BE008(c);
                if (t->set && t->farTris <= 10) {
                    TreeRec* tr = &t->set->list->data[c->first];
                    f32 h = t->far->data->id;
                    cb(t->farTex, &tr->mtx.pos, sizeof(TreeRec), c->count, 0.5f * h, h);
                }
            }
        }
    }
}

}  // extern "C"

s32 TreePreloader::Size(void) {
    s32 n = 0;
    for (int i = 0; i < lbl_804C00D4.n; i++) {
        n += fn_802BD0C8(&lbl_804C00D4.data[i]);
    }
    return n;
}

void TreePreloader::Load(void) {
    for (int i = 0; i < lbl_804C00D4.n; i++) {
        fn_802BD0F4(&lbl_804C00D4.data[i]);
    }
}
