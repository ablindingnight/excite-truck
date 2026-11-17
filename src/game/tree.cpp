// Trees (0x802BC334-0x802BE30C): tree types from "treeclass.val" ("dyno" entries: near/far models and
// their textures, trunk cylinder "cyl_p1"/"cyl_p2"/"rad", "base_rad", "sway_radius", "tree_run",
// "surf_type", "tree_shadow"), the placed trees from "treelist.trl" ('TLST' v2, byte-swapped and relocated
// on load) with their collision cylinders ("tree-volume") and tree-hit registration, the fan-cell grid the
// near/far draw lists are kept in, drawing (far models fade in between 0.375 and 0.475, near models fade
// out between 0.525 and 0.625) and the shadow pass, plus the preload job. Names are guesses apart from
// the strings.
#include "game/common.h"
#include "game/faninfo.h"
#include <revolution/MTX/vec.h>
// tree.cpp builds trunk TubeShapes through the inline constructors, so it emits the root shape table (weak, 0xC).
// Nothing refers to it after the dead vptr store is removed; the linker keeps its bytes (this .data is addressed
// from the section start) without the relocation: the 12 zero bytes behind __vt__13TreePreloader. The header's
// novtable default is the stand-in for units where the linker strips the table. FACTS 2026-11-17 "zero blocks".
#define COLLSHAPE_BASE_DECL
#include "game/collshape.h"
#include "game/rmtx.h"
#include "game/shapes.h"
#include "game/byteswap.h"
#include "game/mathutil.h"

// Query cell scale. The name is unproven.
extern const f32 kTreeCellScale;
const f32 kTreeCellScale = 2.0f;

extern "C" {
double sqrt(double x);
int strcmp(const char* a, const char* b);
int strcasecmp(const char* a, const char* b);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
int fn_8012C33C(const char* name);       // resource exists
void* fn_8012C370(const char* name, u32 magic, u32* version, int a, int b, u8* swapped);  // load resource
void fn_8012C4D8(void* p);                                                               // free resource
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* table);
s32 fn_802CAC28(const char* name, int a);  // texture handle
void gxForgetTexture(s32 tex);
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

struct TModel {
    u8 pad0[0x4];
    struct TModelData* data;  // offset 0x4
    u8 pad8[0x24];
    f32 x0, x1;  // offset 0x2C: horizontal extent
    f32 y0, y1;  // offset 0x34: vertical extent
};

struct TModelData {
    u8 pad0[0xC];
    s32 id;  // offset 0xC
};

extern "C" {
const char* fn_802C9C24(TModelData* d);
s32 fn_802B110C(s32 (*load)(const char*), const char* name);
TModel* fn_802B2C34(void* (*load)(const char*), const char* name);
struct ModelInfo;
ModelInfo* fn_802C6B44(const char* name);
void fn_802C6C20(TModel* m);
s32 fn_802B1DA0(s32 id);
ValueTable* fn_802B561C(ValueTable* (*load)(const char*), const char* name);
void fn_802DCAE4(s32 model, const RMtx* m, f32 alpha);
void fn_801F1C24(RMtx* m);
}

// A fan-grid draw cell (0x10): which placed trees it covers.
struct TCell {
    TCell* next;  // offset 0x0
    s16 x;        // offset 0x4
    s16 z;        // offset 0x6
    s16 count;    // offset 0x8
    s16 first;    // offset 0xA
    f32 fade;     // offset 0xC
    void Swap();
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
    RVec3 normal;   // offset 0x0
    RMtx mtx;      // offset 0xC
    f32 scale;     // offset 0x3C (in the file: radius)
    f32 drop;      // offset 0x40
};

struct TreeRec {
    u8 pad0[0x4];
    void* hit;      // offset 0x4
    u8 pad8[0x4];
    RMtx mtx;       // offset 0xC
    TreeVol* vol;   // offset 0x3C
    f32 drop;       // offset 0x40
};

#include "game/tarray.h"

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
    RVec3 center;  // offset 0x0
    RVec3 axis;    // offset 0xC
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


#include "game/tfixed.h"

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
extern "C" void fn_802BAF98(void* p, u8 on);

extern "C" TreeVol* fn_801FA8E0(CollShape* s, const RMtx* m, u8* hit);

#include "game/preload.h"

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

// Its implicit destructor (TArray's inlined) is emitted at the end of this unit and registered by __sinit; it is
// the function at 0x802BE30C that the split used to give faninfo as "~FanCell" (aids audit 2, 2026-11-17).
TFixed<TreeType, 16> lbl_804C00D4;




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

f32 fn_802BC540(const RVec3* v, f32 k) {
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

static inline void MulVec(RVec3* out, const RMtx* mtx, const RVec3* v) {
    const RVec3* m = &mtx->rot.x;
    if (out == v) {
        f32 x = Dot3(v->x, m[0].x, v->y, m[1].x, v->z, m[2].x);
        f32 y = Dot3(v->x, m[0].y, v->y, m[1].y, v->z, m[2].y);
        f32 z = Dot3(v->x, m[0].z, v->y, m[1].z, v->z, m[2].z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(v->x, m[0].x, v->y, m[1].x, v->z, m[2].x);
        out->y = Dot3(v->x, m[0].y, v->y, m[1].y, v->z, m[2].y);
        out->z = Dot3(v->x, m[0].z, v->y, m[1].z, v->z, m[2].z);
    }
}

extern "C" {

void fn_801EA18C(const RVec3* pos, RVec3* normal, int a);

void fn_802BC588(RMtx* src, RMtx* dst, RVec3* normal, const TRot* r, const TreeType* t) {
    src->pos.x = r->x;
    src->pos.y = 0.0f;
    src->pos.z = r->z;
    src->rot.z.x = r->dx;
    src->rot.z.y = 0.0f;
    src->rot.z.z = r->dz;
    src->rot.y.x = 0.0f;
    src->rot.y.y = 1.0f;
    src->rot.y.z = 0.0f;
    PSVECCrossProduct((const Vec*)&src->rot.y, (const Vec*)&src->rot.z, (Vec*)&src->rot.x);
    RVec3 up;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    fn_801EA18C(&src->pos, &up, 0);
    *normal = up;
    src->pos.y -= fn_802BC540(&up, t->baseRad);
    RVec3 c;
    MulVec(&c, src, &t->cyl.center);
    f32 curScale = r->scale;
    c.x *= curScale;
    c.y *= r->scale;
    c.z *= r->scale;
    dst->pos.x = src->pos.x + c.x;
    dst->pos.y = src->pos.y + c.y;
    dst->pos.z = src->pos.z + c.z;
    MulVec(&dst->rot.z, src, &t->cyl.axis);
    dst->rot.x.x = 1.0f;
    dst->rot.x.y = 0.0f;
    dst->rot.x.z = 0.0f;
    PSVECCrossProduct((const Vec*)&dst->rot.z, (const Vec*)&dst->rot.x, (Vec*)&dst->rot.y);
    PSVECCrossProduct((const Vec*)&dst->rot.y, (const Vec*)&dst->rot.z, (Vec*)&dst->rot.x);
    PSVECNormalize((const Vec*)&dst->rot.y, (Vec*)&dst->rot.y);
    PSVECNormalize((const Vec*)&dst->rot.x, (Vec*)&dst->rot.x);
    f32 s = r->scale;
    src->rot.x.x *= s;
    src->rot.x.y *= s;
    src->rot.x.z *= s;
    src->rot.y.x *= s;
    src->rot.y.y *= s;
    src->rot.y.z *= s;
    src->rot.z.x *= s;
    src->rot.z.y *= s;
    src->rot.z.z *= s;
}

CollShape* fn_802BC9B0(const TCyl* c, int id, f32 scale) {
    f32 k = 50000.0f;
    f32 r = c->rad * scale;
    f32 h = c->half * scale;
    TubeShape* s = new ("tree-volume") TubeShape(k, r, h);
    s->SetSurface(id);
    return s;
}

void fn_802BCA8C(TreeRec* tr, const TRot* r, const TreeType* t) {
    RMtx m;
    fn_802BC588(&tr->mtx, &m, (RVec3*)tr, r, t);
    u8* hit = fn_802C1500(&tr->mtx, t->cls);
    if (lbl_8055FAB0) {
        f32 s = r->scale;
        tr->vol = fn_801FA8E0(fn_802BC9B0(&t->cyl, t->surf, s), &m, hit);
        tr->drop = tr->mtx.pos.y - tr->vol->y;
        if (t->run && t->surf != 0x74) {
            TCyl c;
            c = t->cyl;
            c.rad += 7.5f;
            c.half += 2.5f;
            fn_801FA8E0(fn_802BC9B0(&c, 1003, r->scale), &m, hit);
        }
    } else {
        tr->vol = 0;
        tr->drop = 0.0f;
    }
    tr->hit = 0;
}

static inline f32 MidF(f32 a, f32 b) { return a + 0.5f * (b - a); }

static inline void SetV(RVec3& v, f32 x, f32 y, f32 z) {
    v.x = x;
    v.y = y;
    v.z = z;
}

static inline void ReadCyl(TreeType* t, ValueNode* n, RVec3& c) {
    ValVec3 p1 = n->GetVec3("cyl_p1");
    ValVec3 p2 = n->GetVec3("cyl_p2");
    f32 firstX = p1.x;
    SetV(c, MidF(firstX, p2.x), MidF(p1.y, p2.y), MidF(p1.z, p2.z));
    t->cyl.center = c;
    t->cyl.axis.x = p1.x - p2.x;
    t->cyl.axis.y = p1.y - p2.y;
    t->cyl.axis.z = p1.z - p2.z;
    t->cyl.half = PSVECMag((const Vec*)&t->cyl.axis) / 2.0f;
    PSVECNormalize((const Vec*)&t->cyl.axis, (Vec*)&t->cyl.axis);
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
                t->far = fn_802B2C34((void* (*)(const char*))fn_802C6B44, n->GetValue("far_model"));
                t->farId = fn_802BC4D4(t->far);
                t->farTex = fn_802BC4E0(t->far);
                t->farTris = fn_802B1DA0(t->farId);
                t->near = fn_802B2C34((void* (*)(const char*))fn_802C6B44, n->GetValue("near_model"));
                t->nearId = fn_802BC4D4(t->near);
                t->nearTex = fn_802BC4E0(t->near);
                t->nearTris = fn_802B1DA0(t->nearId);
                t->sway = n->GetFloat("sway_radius", 0.0f);
                t->run = n->GetBool("tree_run", 1);
                t->surf = fn_801D04D0(n->GetString("surf_type", "tree"));
                t->cls = fn_802C143C(n);
                RVec3 c;
                ReadCyl(t, n, c);
                t->cyl.rad = n->GetFloat("rad");
                t->baseRad = n->GetFloat("base_rad");
                t->set = 0;
            }
        }
    }
}

/* Helper name unproven: releases one tree type's set, textures and models. */
static inline void FreeTreeType(TreeType* t) {
    SafeDelete(t->set);
    gxForgetTexture(t->farTex);
    fn_802C6C20(t->far);
    t->far = 0;
    gxForgetTexture(t->nearTex);
    fn_802C6C20(t->near);
    t->near = 0;
}

void fn_802BCF04(void) {
    for (int i = 0; i < lbl_804C00D4.n; i++) {
        FreeTreeType(&lbl_804C00D4.data[i]);
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

}
template <class T>
/* Helper name unproven: the address of element i; the accessor boundary is needed (written in place -1.12 on fn_802BD0F4) */
static inline T* ElemAt(T* p, int i) {
    return &p[i];
}
extern "C" {

void fn_802BD0F4(TreeType* t) {
    TreeRec* tr;
    if (t->shadow && t->set) {
        int n = t->set->list->n;
        for (int i = 0; i < n; i++) {
            tr = ElemAt(t->set->list->data, i);
            TPos2 p;
            p.x = tr->mtx.pos.x;
            p.z = tr->mtx.pos.z;
            f32 s = tr->mtx.rot.y.y;
            tr->hit = fn_802BA580(t->shadow, &p, s);
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

/* Helper name unproven: builds the tree set of one list entry. */
static inline void LoadTreeSet(TListEntry* e, TreeType* t) {
    TreeRec*tr;
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
                r.scale = PSVECMag((const Vec*)&tr->mtx.rot.z);
                r.dx = tr->mtx.rot.z.x;
                r.dz = tr->mtx.rot.z.z;
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

void fn_802BD210(void) {
    u32 version = 0;
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
                LoadTreeSet(e, t);
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

static inline void FixEntry(TListEntry* e, u8* base) {
    fn_802BD69C(&e->list, base);
    fn_802BD5B0(&e->frames, base);
    fn_802BD5B0(&e->near, base);
}

void fn_802BD4DC(TListFile* f, u8* base) {
    TListEntry* e;
    SwapWord(&f->n);
    SwapWord(&f->entries);
    f->entries = fn_802BD5A8(f->entries, base);
    for (u32 i = 0; i < f->n; i++) {
        FixEntry(&f->entries[i], base);
    }
}

/* Helper name unproven: byte-swaps one cell read from the file and clears its fade. */
inline void TCell::Swap() {
    SwapHalf(&x);
    SwapHalf(&z);
    SwapHalf(&count);
    SwapHalf(&first);
    fade = -1.0f;
}


TListEntry* fn_802BD5A8(TListEntry* off, u8* base) { return (TListEntry*)((u8*)off + (u32)base); }

TCell* fn_802BD694(TCell* off, u8* base);

void fn_802BD5B0(TFileArray<TCell>* a, u8* base) {
    TCell* c;
    SwapWord(&a->n);
    SwapWord(&a->data);
    a->data = fn_802BD694(a->data, base);
    for (u32 i = 0; i < a->n; i++) {
        a->data[i].Swap();
    }
}

TCell* fn_802BD694(TCell* off, u8* base) { return (TCell*)((u8*)off + (u32)base); }

TreeRec* fn_802BD80C(TreeRec* off, u8* base);

#pragma push
#pragma opt_unroll_loops off
void fn_802BD69C(TFileArray<TreeRec>* a, u8* base) {
    SwapWord(&a->n);
    SwapWord(&a->data);
    a->data = fn_802BD80C(a->data, base);
    for (u32 i = 0; i < a->n; i++) {
        f32* m = (f32*)&a->data[i].mtx;
        for (int k = 0; k < 12; k++) {
            SwapWord(&m[k]);
        }
    }
}
#pragma pop

TreeRec* fn_802BD80C(TreeRec* off, u8* base) { return (TreeRec*)((u8*)off + (u32)base); }

static inline bool Live(TCell* c) { return c->fade >= 0.0f; }

static inline void Settle(TreeRec* tr) {
    if (tr->vol && tr->mtx.pos.y > -2000.0f) {
        tr->mtx.pos.y = tr->vol->y + tr->drop;
    }
}

/* Helper name unproven */
static inline void DrawTreeType(int i) {
    // MATCHING-AID: one declaration list in this order (the type pointer t in the middle) gives the saved registers;
    // t is a local of the helper, not its parameter.
    TArray<TreeRec>* list;
    TArray<TCell>* frames;
    s32 id;
    int j;
    TCell* c;
    TCell* c2;
    TreeType* t;
    TArray<TreeRec>* list2;
    TArray<TCell>* near;
    int n;
    s32 tris;
    int n2;
    s32 tris2;
    s32 id2;
    int j2;
    t = &lbl_804C00D4.data[i];
    if (t->set) {
        frames = t->set->frames;
        list = t->set->list;
        fn_802C8BD0(fn_802C28C0());
        fn_802DC71C(t->nearId, t->nearTex);
        for (j = 0; j < frames->n; j++) {
            c = &frames->data[j];
            f32 one = 1.0f;
            f32 k = Unlerp(c->fade, 0.375f, 0.475f);
            k = (one < k) ? one : k;
            k = (k > 0.0f) ? k : 0.0f;
            f32 a = Lerp(0.0f, 1.0f, k);
            if (Live(c) && a > 0.1f) {
                id = t->nearId;
                tris = t->nearTris;
                for (n = c->first; n < c->first + c->count; n++) {
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
        near = t->set->near;
        list2 = t->set->list;
        if (t->farTris <= 10) {
            fn_802C8BD0(fn_802C28C0());
            fn_802DC71C(t->farId, t->farTex);
            RMtx cam;
            RMtx m;
            fn_801F1C24(&cam);
            for (j2 = 0; j2 < near->n; j2++) {
                c2 = &near->data[j2];
                if (Live(c2)) {
                    f32 one = 1.0f;
                    f32 k = Unlerp(c2->fade, 0.525f, 0.625f);
                    k = (one < k) ? one : k;
                    k = (k > 0.0f) ? k : 0.0f;
                    f32 a = Lerp(1.0f, 0.0f, k);
                    if (a > 0.1f) {
                        id2 = t->farId;
                        tris2 = t->farTris;
                        for (n2 = c2->first; n2 < c2->first + c2->count; n2++) {
                            TreeRec* tr = &list2->data[n2];
                            Settle(tr);
                            f32 s = tr->mtx.rot.y.y;
                            m.pos = tr->mtx.pos;
                            m.rot.x.x = cam.rot.x.x * s;
                            m.rot.x.y = cam.rot.x.y * s;
                            m.rot.x.z = cam.rot.x.z * s;
                            m.rot.y.x = cam.rot.y.x * s;
                            m.rot.y.y = cam.rot.y.y * s;
                            m.rot.y.z = cam.rot.y.z * s;
                            m.rot.z.x = cam.rot.z.x * s;
                            m.rot.z.y = cam.rot.z.y * s;
                            m.rot.z.z = cam.rot.z.z * s;
                            fn_802DCAE4(id2, &m, a);
                            lbl_8055FAA8++;
                            lbl_8055FAAC += tris2;
                        }
                    }
                    c2->fade = -1.0f;
                }
            }
            fn_802DC8B4();
            fn_802C8D74();
        }
    }
}

void fn_802BD814(void) {
    lbl_8055FAAC = 0;
    lbl_8055FAA8 = 0;
    for (int i = 0; i < lbl_804C00D4.n; i++) {
        DrawTreeType(i);
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

/* Helper name unproven: the far-billboard pass of one tree type; the boundary is needed (in place -1.77) */
static inline void DrawFarTrees(int i, f32 range) {
    TCell* c;
    TreeType* t;
    const RVec3* eye;
    int j;
    s32 id;
    int n;
    TArray<TCell>* frames;
    TArray<TreeRec>* list;
    f32 r2;
    f32 fade;
    f32 span;
    fade = 50.0f;
    t = &lbl_804C00D4.data[i];
    if (t->set) {
        frames = t->set->frames;
        list = t->set->list;
        if (t->farTris <= 10) {
            fn_802C8BD0(fn_802C28C0());
            fn_802DC71C(t->farId, t->farTex);
            r2 = range * range;
            f32 in = range - fade;
            span = r2 - in * in;
            RMtx cam;
            fn_801F1C24(&cam);
            eye = (const RVec3*)(fn_801E8718(-1) + 0x24);
            for (j = 0; j < frames->n; j++) {
                c = &frames->data[j];
                if (Live(c)) {
                    id = t->farId;
                    for (n = c->first; n < c->first + c->count; n++) {
                        TreeRec* tr = &list->data[n];
                        f32 d = PSVECSquareDistance((const Vec*)&tr->mtx.pos, (const Vec*)eye);
                        if (d <= r2) {
                            f32 a = (r2 - d) / span;
                            a = (1.0f < a) ? 1.0f : a;
                            RMtx m;
                            f32 s = tr->mtx.rot.y.y;
                            m.pos = tr->mtx.pos;
                            m.rot.x.x = cam.rot.x.x * s;
                            m.rot.x.y = cam.rot.x.y * s;
                            m.rot.x.z = cam.rot.x.z * s;
                            m.rot.y.x = cam.rot.y.x * s;
                            m.rot.y.y = cam.rot.y.y * s;
                            m.rot.y.z = cam.rot.y.z * s;
                            m.rot.z.x = cam.rot.z.x * s;
                            m.rot.z.y = cam.rot.z.y * s;
                            m.rot.z.z = cam.rot.z.z * s;
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

void fn_802BDD8C(f32 range) {
    fn_802DBE4C();
    if (fn_8020403C()) {
        lbl_8055FAAC = 0;
        lbl_8055FAA8 = 0;
        for (int i = 0; i < lbl_804C00D4.n; i++) {
            DrawFarTrees(i, range);
        }
    }
    fn_802DBECC();
}

void fn_802BDFBC(FanCell* f, int lod, f32 blend) {
    f32 v;
    if (lod < 8) {
        v = 1.0f;
    } else if (lod == 8) {
        v = blend;
    } else {
        v = 0.0f;
    }
    for (TCell* c = f->head; c; c = c->next) {
        c->fade = v;
    }
}

void fn_802BDFF4(RMtx* m, u8 on) {
    TreeRec* tr = (TreeRec*)((u8*)m - 0xC);
    if (tr->hit) {
        fn_802BAF98(tr->hit, on);
    }
}

TreeType* fn_802BE008(TCell* c) {
    int in;
    TreeType* t = lbl_804C00D4.data;
    for (int i = 0; i < lbl_804C00D4.n; i++, t++) {
        if (t->set) {
            const struct { const TArray<TCell>* cells; } range = {t->set->near};
            in = 0;
            TCell* const start = range.cells->data;
            if (c >= start && c < start + range.cells->n) {
                in = 1;
            }
            if (in) {
                return t;
            }
        }
    }
    return 0;
}

typedef void (*TreeQuery)(s32 handle, const RVec3* pos, int size, int n, f32 r, f32 h);

void fn_802BE07C(const RVec3* pos, TreeQuery cb) {
    f32 x = pos->x;
    const f32& size = kTreeCellScale;
    s16 bx = ((s16)(int)(x / size) / 128) * 128 + 0x40;
    s16 bz = ((s16)(int)(pos->z / size) / 128) * 128 + 0x40;
    for (int i = -0x80; i <= 0x80; i += 0x80) {
        for (int j = -0x80; j <= 0x80; j += 0x80) {
            FanCell* f = fn_802BE844(bx + i, bz + j, 0x40);
            for (TCell* c = f->head; c; c = c->next) {
                TreeType* t = fn_802BE008(c);
                if (t->set && t->farTris <= 10) {
                    TreeRec* tr = &t->set->list->data[c->first];
                    TModel* m = t->far;
                    f32 w = m->x1 - m->x0;
                    f32 h = m->y1 - m->y0;
                    cb(t->farTex, &tr->mtx.pos, sizeof(TreeRec), c->count, w / 2.0f, h);
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
