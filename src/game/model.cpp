// Models (0x802C5390-0x802C82C0, model.cpp): loading and byte-swapping "MODL" ModelInfo resources
// (elements = display-list prims with their surface lists, plus MRJT/MRBN/SPHL extras), model instances
// with per-instance texture nodes, and the render queue: surfaces are bucketed per texture node, sorted by
// priority band and drawn texture by texture. Names are guesses apart from the strings.
#include "game/common.h"

void* operator new[](size_t size, const char* tag);

extern "C" {
void* memset(void* p, int c, u32 n);
int strcasecmp(const char* a, const char* b);
void qsort(void* base, u32 n, u32 size, int (*cmp)(const void*, const void*));
void DCStoreRangeNoSync(void* p, u32 n);
}

struct MVec3 {
    f32 x, y, z;
};

struct MSphere {
    MVec3 c;  // offset 0x0
    f32 r;    // offset 0xC
};

struct MMtx33 {
    f32 xx, xy, xz;
    f32 yx, yy, yz;
    f32 zx, zy, zz;
};

struct MFrame {
    MMtx33 r;  // offset 0x0
    MVec3 t;   // offset 0x24
};

struct MPrim {
    u8 pad[0x18];
    f32 size;  // offset 0x18
};

struct TexNode;

// One render request: a prim queued on a texture node (0x14).
struct RNode {
    MPrim* prim;   // offset 0x0
    u8 clip;       // offset 0x4
    void* mtx;     // offset 0x8
    u32 mat;       // offset 0xC
    RNode* next;   // offset 0x10
};

struct TexNode {
    TexNode(void) { refs = 0; }
    s32 tex;       // offset 0x0
    u16 refs;      // offset 0x4
    s16 prio;      // offset 0x6
    RNode* list;   // offset 0x8
    u32 key;       // offset 0xC
};

struct MSurf {
    u32 type;           // offset 0x0
    char name[16];      // offset 0x4
    TexNode* tex[1];    // offset 0x14 (one per instance)
};

struct MSurfList {
    s32 n;           // offset 0x0
    MSurf** surfs;   // offset 0x4
};

struct MElem {
    u32 bit;          // offset 0x0
    u32 flags;        // offset 0x4
    MSurfList* list;  // offset 0x8
    MPrim* prim;      // offset 0xC
};

struct MExtra {
    u32 tag;     // offset 0x0
    u32 size;    // offset 0x4
    s32 n;       // offset 0x8
};

struct ModelInfo {
    s32 nElems;           // offset 0x0
    MElem* elems;         // offset 0x4
    s32 nLists;           // offset 0x8
    MSurfList** lists;    // offset 0xC
    s32 nExtras;          // offset 0x10
    MExtra** extras;      // offset 0x14
    s32 maxInst;          // offset 0x18
    f32 radius;           // offset 0x1C
    MVec3 center;         // offset 0x20
    f32 box[6];           // offset 0x2C
    u32 pad44;
};

struct ModelNode {
    ModelNode(void) { refs = 0; }
    s32 refs;            // offset 0x0
    ModelInfo* info;     // offset 0x4
    s32 inst;            // offset 0x8
    MSphere* spheres;    // offset 0xC
};

struct Remap {
    char from[16];   // offset 0x0
    s32 inst;        // offset 0x10
    char to[16];     // offset 0x14
    s32 unk24;       // offset 0x24
    s32* out;        // offset 0x28
};

struct Bin {
    TexNode** first;  // offset 0x0
    s32 count;        // offset 0x4
};

struct LightSrc {
    u32 mtx;        // offset 0x0
    MVec3 pos;      // offset 0x4
    u8 pad[0x10];
};

template <class T>
struct Pool {
    Pool(void) {
        count = 0;
        data = 0;
        max = 0;
    }
    T* Alloc(void) {
        if (count < max) {
            return &data[count++];
        }
        return 0;
    }
    s32 count;  // offset 0x0
    s32 max;    // offset 0x4
    T* data;    // offset 0x8
};

extern "C" {
void* fn_8012C370(const char* name, u32 type, u32* version, u32* size, u8* first, u8* swapped);
void fn_8012C4E4(void* res, void (*cb)(void*, void*), void* user);
const char* fn_8012C58C(void* res);
int fn_8012C33C(const char* name);
int fn_801345D8(char* buf, int n, const char* fmt, ...);
const MFrame* fn_802C86DC(void);
void fn_802C8B70(s32 tex);
void fn_802C8B98(s32 tex, const MFrame* m);
void fn_802C8D74(void);
int fn_802C90E0(void);
int fn_802C94A0(const MVec3* c, f32 r, u8* clip, f32* dist);
int fn_802C9638(const MVec3* c, f32 r, u8* clip);
const MVec3* fn_802C9A40(void);
s32 fn_802C9C58(s32 v);
s32 fn_802CAC28(const char* name, s32 key);
s32 fn_802CAC44(const char* name, s32 kind);
void fn_802CAC54(s32 id);
const char* fn_802CAE34(s32 id);
int fn_802CAF8C(s32 id);
s32 fn_802CAFC8(s32 id);
s32 fn_802CAFF0(s32 id);
void fn_802CC550(s32 id, f32 dist, f32 scale);
void* fn_802D0514(const MFrame* m);
void fn_802D0610(void* m);
void fn_802D6B08(MPrim* p);
void fn_802D6B80(MPrim* p, u32 base, const char* name);
void fn_802D6C44(MPrim* p);
void fn_802D6C48(MPrim* p);
void fn_802D6C4C(f32 f);
void fn_802D754C(void);
void fn_802DA550(void);
void fn_802DA704(void);
void fn_802DA80C(void);
s32 fn_802DA918(void);
void fn_802DAA18(u32 mat);
void fn_802DABA4(void);
void fn_802DB2B4(void);
void fn_802DB2B8(void);
void fn_802DB2BC(u8 clip);
void fn_802E30C8(const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);
f32 PSVECSquareDistance(const MVec3* a, const MVec3* b);
f32 PSVECDistance(const MVec3* a, const MVec3* b);
}

ModuleTag lbl_8055FC18;
Pool<RNode> lbl_804C1338;
u32 lbl_804C1344;
Bin lbl_804C1348[9];
Pool<TexNode*> lbl_804C1390;    // sorted texture nodes
Pool<ModelNode> lbl_804C139C;   // model instances
Pool<TexNode> lbl_804C13A8;     // texture nodes
Pool<Remap> lbl_804C13B4;       // texture remaps
LightSrc lbl_804C13C0;

u8 lbl_8055FC20;   // queueing (draw later)
u8 lbl_8055FC21;   // texture nodes changed

u32 lbl_8055D008 = 0xFFFFFFFF;  // element mask
s32 lbl_8055D00C = 1;

template <class T>
inline void Relocate(T*& p, u32 base);

static inline void Swap32(void* p) {
    u8* b = (u8*)p;
    u8 t = b[0];
    b[0] = b[3];
    b[3] = t;
    t = b[1];
    b[1] = b[2];
    b[2] = t;
}

static inline void SwapVec(MVec3* v) {
    Swap32(&v->x);
    Swap32(&v->y);
    Swap32(&v->z);
}

static inline void StrCopy(char* d, const char* s, int n) {
    char* p = d;
    while ((*p++ = *s++) != 0) {
        if (--n <= 0) {
            p[-1] = 0;
            break;
        }
    }
}

// Surface types drawn through texture nodes.
static inline int IsQueued(u32 type) {
    int ok = 0;
    if (type < 13) {
        int skip;
        ok = 1;
        switch (type) {
        case 4:
        case 6:
        case 9:
        case 11:
            skip = 1;
            break;
        default:
            skip = 0;
            break;
        }
        if (skip) {
            ok = 0;
        }
    }
    return ok;
}

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void MulVec(MVec3* out, const MVec3* in, const MMtx33* m) {
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

static inline void Transform(MVec3* out, const MVec3* in, const MFrame* m) {
    MulVec(out, in, &m->r);
    out->x = out->x + m->t.x;
    out->y = out->y + m->t.y;
    out->z = out->z + m->t.z;
}

extern "C" {

ModelNode* fn_802C5390(s32 i) { return &lbl_804C139C.data[i]; }

ModelInfo* fn_802C53A8(s32 i) { return lbl_804C139C.data[i].info; }

void fn_802C53C4(MElem* e, TexNode* t, void* mtx, u32 mat, u8 clip) {
    RNode* n = lbl_804C1338.Alloc();
    if (n) {
        n->prim = e->prim;
        n->clip = clip;
        n->mtx = mtx;
        if (e->flags & 1) {
            n->mat = mat;
        } else {
            n->mat = 0xFFFFFFFF;
        }
        n->next = t->list;
        t->list = n;
    }
}

void fn_802C543C(void) { lbl_804C1338.count = 0; }

int fn_802C544C(const void* pa, const void* pb) {
    TexNode* b = *(TexNode**)pb;
    TexNode* a = *(TexNode**)pa;
    if (a->prio != b->prio) {
        return a->prio - b->prio;
    }
    return strcasecmp(fn_802CAE34(a->tex), fn_802CAE34(b->tex));
}

Bin* fn_802C54B4(int pri) { return &lbl_804C1348[pri - 1]; }

int fn_802C54CC(s16 prio) { return (u8)(prio >> 8); }

void fn_802C54D4(void) {
    if (lbl_8055FC21) {
        TexNode* t;
        lbl_804C1390.count = 0;
        for (int i = 0; i < lbl_804C13A8.count; i++) {
            t = &lbl_804C13A8.data[i];
            if (t->refs) {
                TexNode** p = lbl_804C1390.Alloc();
                if (p) {
                    *p = t;
                }
            }
        }
        qsort(lbl_804C1390.data, lbl_804C1390.count, sizeof(TexNode*), fn_802C544C);
        int pri = 1;
        Bin* b = fn_802C54B4(1);
        b->first = lbl_804C1390.data;
        for (int j = 0; j < lbl_804C1390.count; j++) {
            TexNode* t = lbl_804C1390.data[j];
            while (pri < fn_802C54CC(t->prio) && pri <= 9) {
                b->count = &lbl_804C1390.data[j] - b->first;
                b = fn_802C54B4(++pri);
                b->first = &lbl_804C1390.data[j];
            }
        }
        b->count = &lbl_804C1390.data[lbl_804C1390.count] - b->first;
        while (++pri <= 9) {
            fn_802C54B4(pri)->count = 0;
        }
        lbl_8055FC21 = 0;
    }
}

void fn_802C5690(TexNode* t) {
    if (t->list) {
        fn_802C8B70(t->tex);
        for (RNode* n = t->list; n; n = n->next) {
            fn_802D0610(n->mtx);
            if (n->mat != 0xFFFFFFFF) {
                fn_802DAA18(n->mat);
            }
            fn_802DB2BC(n->clip);
            fn_802D6B08(n->prim);
        }
        fn_802C8D74();
        t->list = 0;
    }
}

void fn_802C5720(int pri) {
    s32 n;
    TexNode** first;
    fn_802C54D4();
    if (pri > 0) {
        Bin* b = fn_802C54B4(pri);
        first = b->first;
        n = b->count;
    } else {
        n = lbl_804C1390.count;
        first = lbl_804C1390.data;
    }
    for (int i = 0; i < n; i++) {
        fn_802C5690(*first);
        first++;
    }
    fn_802DB2BC(1);
}

MPrim* fn_802C57B0(MPrim* p) { return p; }

void fn_802C57B4(s32 i, void* mtx, u32 mat, u8 clip, f32 dist) {
    ModelNode* slot = fn_802C5390(i);
    ModelInfo* info = slot->info;
    if (!lbl_8055FC20) {
        fn_802C543C();
    }
    s32 inst = slot->inst;
    for (int e = 0; e < info->nElems; e++) {
        MElem* el = &info->elems[e];
        if (lbl_8055D008 & (1 << el->bit)) {
            u8 c = clip;
            if (slot->spheres && !fn_802C9638(&slot->spheres[e].c, slot->spheres[e].r, &c)) {
                continue;
            }
            u8 cl = c;
            MSurfList* sl = el->list;
            for (int j = 0; j < sl->n; j++) {
                MSurf* s = sl->surfs[j];
                if (IsQueued(s->type)) {
                    TexNode* t = s->tex[inst];
                    fn_802C53C4(el, t, mtx, mat, cl);
                    fn_802CC550(t->tex, dist, fn_802C57B0(el->prim)->size);
                }
            }
        }
    }
    if (!lbl_8055FC20) {
        fn_802C5720(-1);
    }
}

void fn_802C595C(char* dst, int n, const char* name, s32 inst) {
    for (int i = 0; i < lbl_804C13B4.count; i++) {
        Remap* r = &lbl_804C13B4.data[i];
        if (strcasecmp(name, r->from) == 0 && inst == r->inst) {
            StrCopy(dst, r->to, n);
            return;
        }
    }
    StrCopy(dst, name, n);
}

u16 fn_802C5A50(int a, int b) { return a << 8 | b; }

TexNode* fn_802C5A60(s32 tex, u32 key, u16 prio) {
    TexNode* found = 0;
    TexNode* empty = 0;
    TexNode* t = lbl_804C13A8.data;
    for (int i = 0; i < lbl_804C13A8.count; i++, t++) {
        if (t->refs == 0) {
            empty = t;
        } else if (t->key == key && t->prio == prio && t->tex == tex) {
            found = t;
            break;
        }
    }
    if (!found) {
        if (!empty) {
            empty = lbl_804C13A8.Alloc();
            if (!empty) {
                fn_802E317C("model: out of texnodes!");
            }
        }
        empty->tex = tex;
        empty->refs = 0;
        found = empty;
        empty->prio = prio;
        empty->list = 0;
        empty->key = key;
        lbl_8055FC21 = 1;
    }
    found->refs++;
    return found;
}

void fn_802C5BA0(TexNode* t) {
    fn_802CAC54(t->tex);
    if (--t->refs == 0) {
        lbl_8055FC21 = 1;
    }
}

static inline u32 Twice(u32 x) { return x * 2; }

u16 fn_802C5BEC(u32 a, u32 b) { return fn_802C5A50(5, 100) + (u16)Twice(a) + (u16)b; }

void fn_802C5C40(MSurf* s, s32 inst, MSurf* base) {
    u32 type = s->type;
    s32 tex = fn_802CAC28("zonly.tex", 0);
    s->tex[inst] = fn_802C5A60(tex, type, base->tex[inst]->prio - 1);
}

void fn_802C5CC4(MSurf* s, s32 inst, int sorted, const char* prefix) {
    char name[0x100];
    char path[0x100];
    fn_802C595C(name, 0x100, s->name, inst);
    if (prefix) {
        fn_801345D8(path, 0x100, "%s/%s", prefix, name);
        if (fn_8012C33C(path)) {
            StrCopy(name, path, 0x100);
        }
    }
    u32 type = s->type;
    s32 tex = fn_802CAC44(name, type);
    u16 prio;
    if (sorted) {
        prio = fn_802C5BEC(type, fn_802CAF8C(tex));
    } else {
        prio = fn_802C5A50(fn_802CAFC8(tex), fn_802CAFF0(tex));
    }
    s->tex[inst] = fn_802C5A60(tex, type, prio);
}

void fn_802C5DF0(ModelInfo* info, void (*fn)(MPrim*)) {
    for (int i = 0; i < info->nElems; i++) {
        fn(info->elems[i].prim);
    }
}

void fn_802C5E6C(void* res, void* info) { fn_802C5DF0((ModelInfo*)info, fn_802D6C44); }

MExtra* fn_802C5EB8(ModelInfo* info, u32 tag);

MSphere* fn_802C5E7C(ModelInfo* info) {
    MExtra* x = fn_802C5EB8(info, 'SPHL');
    if (x) {
        return (MSphere*)&x->n;
    }
    return 0;
}

MExtra* fn_802C5EB8(ModelInfo* info, u32 tag) {
    MExtra* found = 0;
    for (int i = 0; i < info->nExtras; i++) {
        MExtra* x = info->extras[i];
        if (x->tag == tag) {
            found = x;
            break;
        }
    }
    return found;
}

TexNode* fn_802C5EFC(s32 tex) {
    TexNode* t = lbl_804C13A8.data;
    for (int i = 0; i < lbl_804C13A8.count; i++, t++) {
        if (t->refs && t->tex == tex) {
            return t;
        }
    }
    return 0;
}

void fn_802C5F48(void) {
    lbl_804C139C.max = 640;
    lbl_804C139C.data = new ("model:modelnode[]") ModelNode[640];
    lbl_804C139C.count = 0;
    lbl_804C1338.max = 2048;
    lbl_804C1338.data = new ("model:surfnode[]") RNode[2048];
    lbl_804C1338.count = 0;
    lbl_804C13A8.max = 1024;
    lbl_804C13A8.data = new ("model:texnode[]") TexNode[1024];
    lbl_804C13A8.count = 0;
    lbl_804C1390.max = 1024;
    lbl_804C1390.data = new ("model:sort[]") TexNode*[1024];
    lbl_804C1390.count = 0;
    lbl_804C13B4.max = 128;
    lbl_804C13B4.data = new ("model:remap[]") Remap[128];
    lbl_804C13B4.count = 0;
    lbl_804C13C0.mtx = 0xFFFFFFFF;
    lbl_8055D00C = 1;
}

const char* fn_802C7A10(s32 i);

void fn_802C6080(void) {
    for (int i = 0; i < lbl_804C139C.count; i++) {
        if (lbl_804C139C.data[i].refs > 0) {
            fn_802E30C8("mrModelEnd: model %d not destroyed, will try to get name...", i);
            fn_802E30C8("mrModelEnd: (unfreed model name is %s)", fn_802C7A10(i));
        }
    }
    delete[] lbl_804C13B4.data;
    lbl_804C13B4.data = 0;
    lbl_804C13B4.max = 0;
    lbl_804C13B4.count = 0;
    delete[] lbl_804C13A8.data;
    lbl_804C13A8.count = 0;
    lbl_804C13A8.data = 0;
    lbl_804C13A8.max = 0;
    delete[] lbl_804C1390.data;
    lbl_804C1390.data = 0;
    lbl_804C1390.max = 0;
    lbl_804C1390.count = 0;
    delete[] lbl_804C1338.data;
    lbl_804C1338.data = 0;
    lbl_804C1338.max = 0;
    lbl_804C1338.count = 0;
    delete[] lbl_804C139C.data;
    lbl_804C139C.count = 0;
    lbl_804C139C.data = 0;
    lbl_804C139C.max = 0;
}

void fn_802C61BC(ModelInfo* m, const char* name) {
    if ((u32)m->nElems < 16000 && (u32)m->nExtras < 16000) {
        return;
    }
    u32 base = (u32)m;
    Swap32(&m->nElems);
    Swap32(&m->elems);
    Swap32(&m->nLists);
    Swap32(&m->lists);
    Swap32(&m->nExtras);
    Swap32(&m->extras);
    Swap32(&m->maxInst);
    f32* f = &m->radius;
    for (int k = 0; k < 10; k++) {
        Swap32(&f[k]);
    }
    Swap32(&m->pad44);
    Relocate(m->elems, base);
    int i;
    for (i = 0; i < m->nElems; i++) {
        MElem* e = &m->elems[i];
        Swap32(&e->bit);
        Swap32(&e->flags);
        Swap32(&e->list);
        Swap32(&e->prim);
        Relocate(e->prim, base);
        fn_802D6B80(e->prim, base, name);
        Relocate(e->list, base);
    }
    Relocate(m->extras, base);
    for (i = 0; i < m->nExtras; i++) {
        Swap32(&m->extras[i]);
        Relocate(m->extras[i], base);
        MExtra* x = m->extras[i];
        Swap32(&x->tag);
        Swap32(&x->size);
        switch (x->tag) {
        case 'MRJT': {
            Swap32(&x->n);
            u8* p = (u8*)x + 0x2C;
            for (int j = 0; j < x->n; j++) {
                u32* q = (u32*)p;
                for (int k = 0; k < 12; k++) {
                    Swap32(&q[k]);
                }
                p += 0x50;
            }
            break;
        }
        case 'MRBN': {
            Swap32(&x->n);
            u8* p = (u8*)x + 0x10;
            for (int j = 0; j < x->n; j++) {
                u32* q = (u32*)p;
                for (int k = 0; k < 12; k++) {
                    Swap32(&q[k]);
                }
                Swap32(p + 0x30);
                Swap32(p + 0x34);
                p += 0x38;
            }
            break;
        }
        case 'SPHL': {
            MSphere* s = (MSphere*)&x->n;
            for (int j = 0; j < m->nElems; j++) {
                Swap32(&s->c.x);
                Swap32(&s->c.y);
                Swap32(&s->c.z);
                Swap32(&s->r);
                s++;
            }
            break;
        }
        default:
            fn_802E317C("mrModelFixup: unknown extra type 0x%x in %s", x->tag, name);
            break;
        }
    }
    Relocate(m->lists, base);
    for (i = 0; i < m->nLists; i++) {
        Swap32(&m->lists[i]);
        Relocate(m->lists[i], base);
        MSurfList* l = m->lists[i];
        Relocate(l->surfs, base);
        for (int j = 0; j < l->n; j++) {
            Swap32(&l->surfs[j]);
            Relocate(l->surfs[j], base);
            Swap32(&l->surfs[j]->type);
        }
    }
}

ModelInfo* fn_802C6B44(const char* name) {
    u8 swapped;
    u8 first;
    u32 size;
    u32 ver;
    ModelInfo* m = (ModelInfo*)fn_8012C370(name, 'MODL', &ver, &size, &first, &swapped);
    if (m) {
        if (swapped) {
            if (ver != 12) {
                fn_802E317C("Bad MOD version: %04x; expecting %04x (%s)", ver, 12, name);
            }
            DCStoreRangeNoSync(m, size);
            fn_802C61BC(m, name);
        }
        if (first) {
            fn_802C5DF0(m, fn_802D6C48);
        }
        return m;
    }
    fn_802E317C("Couldn't load ModelInfo %s", name);
    return 0;
}

void fn_802C6C20(ModelInfo* m) { fn_8012C4E4(m, fn_802C5E6C, m); }

void fn_802C6C30(void) {}

ModelInfo* fn_802C6C34(s32 i) { return fn_802C53A8(i); }

void fn_802C6C38(s32 i) {
    ModelNode* slot = fn_802C5390(i);
    if (--slot->refs <= 0) {
        for (int j = 0; j < slot->info->nLists; j++) {
            MSurfList* l = slot->info->lists[j];
            for (int k = 0; k < l->n; k++) {
                MSurf* s = l->surfs[k];
                TexNode* t = s->tex[slot->inst];
                s->tex[slot->inst] = 0;
                fn_802C5BA0(t);
            }
        }
        slot->info = 0;
        slot->spheres = 0;
    }
}

void fn_802C6CF8(s32 i) {
    if (i == -1) {
        fn_802E30C8("mrModelUnload: unloading MODEL_INVALID!!");
        return;
    }
    ModelInfo* m = fn_802C53A8(i);
    fn_802C6C38(i);
    fn_802C6C20(m);
}

int fn_802C6D5C(ModelInfo* m) {
    for (int i = 0; i < m->nLists; i++) {
        MSurfList* l = m->lists[i];
        for (int j = 0; j < l->n; j++) {
            if (l->surfs[j]->type) {
                return 1;
            }
        }
    }
    return 0;
}

s32 fn_802C6DC4(const char* name, ModelInfo* m, s32 inst, const char* prefix) {
    ModelNode* found = 0;
    ModelNode* empty = 0;
    ModelNode* s = lbl_804C139C.data;
    for (int i = 0; i < lbl_804C139C.count; i++, s++) {
        if (s->refs > 0) {
            if (s->inst == inst && s->info == m) {
                found = s;
                break;
            }
        } else {
            empty = s;
        }
    }
    if (!found) {
        if (!empty) {
            empty = lbl_804C139C.Alloc();
            if (!empty) {
                fn_802E317C("Can't build model! (Increase MAX_MODELS)");
            }
        }
        empty->refs = 0;
        empty->info = m;
        empty->inst = inst;
        empty->spheres = fn_802C5E7C(m);
        if (inst >= m->maxInst) {
            fn_802E317C("model: tried to create instance %d, but max is %d: %s", inst, m->maxInst, name);
        }
        int sorted = fn_802C6D5C(m);
        for (int j = 0; j < m->nLists; j++) {
            MSurfList* l = m->lists[j];
            for (int k = 0; k < l->n; k++) {
                MSurf* sf = l->surfs[k];
                if (sf->type == 7) {
                    fn_802C5C40(sf, empty->inst, l->surfs[0]);
                } else {
                    fn_802C5CC4(sf, empty->inst, sorted, prefix);
                }
            }
        }
        found = empty;
    }
    found->refs++;
    return found - lbl_804C139C.data;
}

void fn_802C6FA8(ModelInfo* m) { fn_802C5DF0(m, fn_802D6C48); }

void fn_802C6FB4(ModelInfo* m) { fn_802C5DF0(m, fn_802D6C44); }

s32 fn_802C6FC0(const char* name, s32 inst, const char* prefix) {
    if (!fn_802C90E0()) {
        fn_802E30C8("mrModelLoad(%s) in scene!", name);
    }
    return fn_802C6DC4(name, fn_802C6B44(name), inst, prefix);
}

void fn_802C703C(s32 i) { fn_802C6C38(i); }

void fn_802C7040(s32 i) {
    if (i != -1) {
        ModelInfo* m = fn_802C5390(i)->info;
        u8 clip;
        f32 dist;
        if (fn_802C94A0(&m->center, m->radius, &clip, &dist)) {
            fn_802C57B4(i, 0, 0xFFFFFFFF, clip, dist);
        }
    }
}

void fn_802C70AC(s32 i, const MFrame* f, u32 mat, u32 mask) {
    if (i != -1) {
        u32 old = lbl_8055D008;
        lbl_8055D008 = mask;
        ModelNode* slot = fn_802C5390(i);
        MVec3 c;
        Transform(&c, &slot->info->center, f);
        u8 clip;
        f32 dist;
        if (fn_802C94A0(&c, slot->info->radius, &clip, &dist)) {
            fn_802C57B4(i, fn_802D0514(f), mat, clip, dist);
        }
        lbl_8055D008 = old;
    }
}

void fn_802C729C(s32 i, f32 range) {
    if (lbl_804C13C0.mtx != 0xFFFFFFFF && i != -1) {
        ModelInfo* m = fn_802C53A8(i);
        if (m->nElems > 0) {
            MSphere* sp = fn_802C5E7C(m);
            for (int e = 0; e < m->nElems; e++) {
                if (m->elems[e].bit == 2) {
                    if (sp && !(PSVECSquareDistance(&lbl_804C13C0.pos, &sp[e].c) <
                                (range + sp[e].r) * (range + sp[e].r))) {
                        continue;
                    }
                    MElem* el = &m->elems[e];
                    MSurfList* sl = el->list;
                    void* mtx = (void*)lbl_804C13C0.mtx;
                    for (int j = 0; j < sl->n; j++) {
                        MSurf* s = sl->surfs[j];
                        if (IsQueued(s->type)) {
                            TexNode* t = s->tex[0];
                            fn_802C53C4(el, t, mtx, 0xFFFFFFFF, 0);
                            fn_802CC550(t->tex, 1000.0f, fn_802C57B0(el->prim)->size);
                        }
                    }
                }
            }
        }
    }
}

void fn_802C744C(s32 i, const MFrame* f) { fn_802C70AC(i, f, 0xFFFFFFFF, 0xFFFFFFFF); }

void fn_802C7458(s32 i, u32 mask) {
    if (i != -1) {
        u32 old = lbl_8055D008;
        lbl_8055D008 = mask;
        ModelInfo* m = fn_802C5390(i)->info;
        u8 clip;
        f32 dist;
        if (fn_802C94A0(&m->center, m->radius, &clip, &dist)) {
            fn_802C57B4(i, 0, 0xFFFFFFFF, clip, dist);
        }
        lbl_8055D008 = old;
    }
}

void fn_802C74D8(s32 i, const MFrame* f, s32 tex) {
    ModelNode* slot = fn_802C5390(i);
    ModelInfo* m = slot->info;
    MVec3 c;
    Transform(&c, &slot->info->center, f);
    u8 clip;
    f32 dist;
    if (fn_802C94A0(&c, slot->info->radius, &clip, &dist)) {
        if (lbl_8055FC20) {
            TexNode* t = fn_802C5EFC(tex);
            void* mtx = fn_802D0514(f);
            for (int e = 0; e < m->nElems; e++) {
                fn_802C53C4(&m->elems[e], t, mtx, 0xFFFFFFFF, clip);
            }
        } else {
            fn_802C8B98(tex, f);
            fn_802DB2BC(clip);
            for (int e = 0; e < m->nElems; e++) {
                fn_802D6B08(m->elems[e].prim);
            }
            fn_802C8D74();
        }
    }
}

void fn_802C7730(void) { fn_802DABA4(); }

void fn_802C7734(const MFrame* f) { fn_802D0610(fn_802D0514(f)); }

void fn_802C7758(void) { fn_802D0610(0); }

void fn_802C7760(s32 i, f32* a, f32* b, f32* c, f32* d, f32* e, f32* g) {
    ModelNode* slot = fn_802C5390(i);
    *a = slot->info->box[0];
    *b = slot->info->box[1];
    *c = slot->info->box[2];
    *d = slot->info->box[3];
    *e = slot->info->box[4];
    *g = slot->info->box[5];
}

void fn_802C77F0(s32 i, f32* box) { fn_802C7760(i, box, box + 1, box + 2, box + 3, box + 4, box + 5); }

f32 fn_802C7808(s32 i) { return fn_802C5390(i)->info->radius; }

static inline int IsDigit(int c) { return c >= '0' && c <= '9'; }

u32 fn_802C7830(const char* s) {
    u32 v = 0;
    while (*s) {
        char c = *s;
        v <<= 4;
        v |= IsDigit(c) ? c - '0' : c - 'A' + 10;
        s++;
    }
    return v;
}

void fn_802C7888(Remap* src, int n) {
    for (int i = 0; i < n; i++) {
        if (src->out) {
            *src->out = -1;
        }
        Remap* d = lbl_804C13B4.Alloc();
        *d = *src;
        src++;
    }
}

void fn_802C7A00(void) { lbl_804C13B4.count = 0; }

const char* fn_802C7A10(s32 i) { return fn_8012C58C(fn_802C53A8(i)); }

void fn_802C7A34(void) { fn_802DA704(); }

void fn_802C7A38(void) { fn_802DA80C(); }

s32 fn_802C7A3C(void) { return fn_802DA918(); }

void fn_802C7A40(void) { fn_802DA550(); }

void fn_802C7A44(void) {
    lbl_8055FC20 = 1;
    fn_802C543C();
}

void fn_802C7A50(void) {
    for (int i = 0; i < 9; i++) {
        fn_802C5720(i);
    }
    lbl_8055FC20 = 0;
}

void fn_802C7A94(int pri) {
    fn_802C7758();
    fn_802C5720(pri);
    if (pri == 9) {
        fn_802C543C();
    }
}

void fn_802C7AD4(void) { fn_802C7A94(9); }

void fn_802C7ADC(void) { fn_802D754C(); }

void fn_802C7AE0(s32 i, const MFrame* f, u32 mask, f32 t) {
    ModelNode* slot = fn_802C5390(i);
    u8 clip = 1;
    MVec3 c;
    Transform(&c, &slot->info->center, f);
    const MVec3* d = fn_802C9A40();
    f32 k = 0.5f * t;
    c.x = c.x + d->x * k;
    c.y = c.y + d->y * k;
    c.z = c.z + d->z * k;
    if (fn_802C9638(&c, k + slot->info->radius, &clip)) {
        fn_802D6C4C(t);
        fn_802DB2BC(clip);
        fn_802C7734(f);
        ModelInfo* m = fn_802C6C34(i);
        for (int e = 0; e < m->nElems; e++) {
            if (mask & (1 << m->elems[e].bit)) {
                fn_802D6B08(m->elems[e].prim);
            }
        }
    }
}

void fn_802C7D54(s32 i, const MFrame* f, s32 v, f32 unused1, f32 unused2, f32 range) {
    u32 mat = fn_802C7A3C();
    s32 saved = fn_802C9C58(v);
    u32 old = lbl_8055D008;
    lbl_8055D008 = 8;
    ModelNode* slot = fn_802C5390(i);
    MVec3 c;
    Transform(&c, &slot->info->center, f);
    u8 clip;
    f32 dist;
    if (fn_802C94A0(&c, slot->info->radius, &clip, &dist)) {
        void* mtx = fn_802D0514(f);
        ModelInfo* m = slot->info;
        if (!lbl_8055FC20) {
            fn_802C543C();
        }
        for (int e = 0; e < m->nElems; e++) {
            MElem* el = &m->elems[e];
            if (lbl_8055D008 & (1 << el->bit)) {
                u8 cl = clip;
                int near;
                if (!slot->spheres) {
                    near = 1;
                } else {
                    MSphere* sp = &slot->spheres[e];
                    MVec3 w;
                    Transform(&w, &sp->c, f);
                    near = PSVECDistance(&w, &fn_802C86DC()->t) - sp->r < range;
                }
                if (near) {
                    f32 d = dist;
                    MSurfList* sl = el->list;
                    for (int j = 0; j < sl->n; j++) {
                        MSurf* s = sl->surfs[j];
                        if (IsQueued(s->type)) {
                            TexNode* t = s->tex[slot->inst];
                            fn_802C53C4(el, t, mtx, mat, cl);
                            fn_802CC550(t->tex, d, fn_802C57B0(el->prim)->size);
                        }
                    }
                }
            }
        }
        if (!lbl_8055FC20) {
            fn_802C5720(-1);
        }
    }
    lbl_8055D008 = old;
    fn_802C9C58(saved);
}

void fn_802C8218(int on) { lbl_8055D00C = on ? 2 : 1; }

void fn_802C8230(void) { fn_802DB2B4(); }

void fn_802C8234(void) { fn_802DB2B8(); }

}  // extern "C"

template <class T>
inline void Relocate(T*& p, u32 base) {
    if (p) {
        p = (T*)((u8*)p + base);
    }
}
