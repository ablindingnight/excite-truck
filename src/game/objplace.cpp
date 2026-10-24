// Ground object placement (0x802B1D38-0x802B3FC4): sets of ground objects ("gobject") per ground material.
// A set has one MatPlacer per "matN" entry; each holds weighted ObjectPlacers (model, weight, sway, scale
// range, axis facing) and a shuffled 128-slot table saying which object goes on which slot. Instances are
// queued per frame into blocks from a shared allocator and drawn with 16 random rotations per object,
// swaying or facing the camera. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
void* memset(void* dst, int c, size_t n);
void* memcpy(void* dst, const void* src, size_t n);
int strcasecmp(const char* a, const char* b);
double cos(double x);
double sin(double x);
double fmod(double x, double y);
int fn_801345D8(char* dst, int n, const char* fmt, ...);
void fn_802E30C8(const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);  // fatal
int fn_8012C33C(const char* name);       // file exists
s32 fn_802CAC28(const char* name, int flags);
void fn_802CAC54(s32 tex);
void* fn_802C6B44(const char* name);  // load a model
void fn_802C6C20(void* model);
const char* fn_802C9C24(void* info);  // texture name
s32 fn_802B110C(s32 (*load)(const char*), const char* name);
void fn_802B8AE0(const char* name, s32* value);
int fn_80129D90(int seed, int lo, int hi);    // random int
f32 fn_80129DC4(int seed, f32 lo, f32 hi);  // random float
f32 fn_801E7E18(void);                      // time
void fn_801F1C24(void* m);                  // camera facing matrix
s32 fn_802C28C0(void);
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
void fn_802DC71C(void* info, s32 tex);
void fn_802DCAE4(void* info, const void* xf, f32 scale);
void fn_802DC8B4(void);
}

void* operator new(size_t size, const char* name);
void* operator new[](size_t size, const char* name);
void operator delete(void* p);
void operator delete[](void* p);

namespace {

struct OVec {
    f32 x;
    f32 y;
    f32 z;
};

// One queued instance.
struct ObjLoc {
    ObjLoc(void);
    u16 kind;   // offset 0x0: rotation index
    OVec pos;   // offset 0x4
    f32 scale;  // offset 0x10
};

struct ObjBlock {
    ObjBlock(void) {
        data = 0;
        cap = 0;
        count = 0;
    }
    ~ObjBlock(void) {
        if (this) {
            data = 0;
            count = 0;
            cap = 0;
        }
    }
    void Init(ObjLoc* p, int n) {
        data = p;
        cap = n;
        count = 0;
    }
    ObjLoc* data;    // offset 0x0
    s32 count;       // offset 0x4
    s32 cap;         // offset 0x8
    ObjBlock* next;  // offset 0xC
};

template <class T>
struct DynVec {
    DynVec(int n) {
        data = new ("dsa_buf") T[n];
        cap = n;
        count = 0;
        buf = data;
    }
    ~DynVec(void) {
        delete[] buf;
        if (this) {
            data = 0;
            count = 0;
            cap = 0;
        }
    }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
    T* buf;     // offset 0xC
};

template <class T, int N>
struct InlineVec {
    InlineVec(void) {
        data = buf;
        cap = N;
        count = 0;
    }
    ~InlineVec(void) {
        if (this) {
            data = 0;
            count = 0;
            cap = 0;
        }
    }
    inline T* Push(const T& v);
    inline int Count(void);
    inline T* At(int i);
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
    T buf[N];   // offset 0xC
};

template <class T>
void SafeDelete(T*& p) {
    if (p) {
        delete p;
        p = 0;
    }
}

template <class T>
void SafeDeleteArray(T*& p) {
    if (p) {
        delete[] p;
        p = 0;
    }
}

static inline void Iota(s32* a, int n) {
    for (int i = 0; i < n; i++) {
        a[i] = i;
    }
}

template <class T>
inline void Swap(T& a, T& b) {
    T t = a;
    a = b;
    b = t;
}

template <class T>
void Shuffle(T* a, int n) {
    // MATCHING-AID: byte-offset counter divided back to an index (found by upstream); revisit.
    int last;
    int off;
    int i;
    i = 0;
    last = n - 1;
    off = 0;
    while (i < last) {
        int j = fn_80129D90(0, i + 1, n);
        T t = a[off / (int)sizeof(T)];
        a[off / (int)sizeof(T)] = a[j];
        a[j] = t;
        i++;
        off += sizeof(T);
    }
}

}  // namespace

template <class T, int N>
struct BlockAlloc {
    BlockAlloc(void) : blocks(0x60) {
        pool = new ("buffer_list[]") T[blocks.cap << 6];
        for (int i = 0; i < blocks.cap; i++) {
            T* p = &pool[i * N];
            ObjBlock* b = &blocks.data[blocks.count++];
            b->data = p;
            b->cap = N;
            b->count = 0;
        }
        blocks.count = 0;
    }
    ~BlockAlloc(void) { SafeDeleteArray(pool); }
    T* pool;                  // offset 0x0
    DynVec<ObjBlock> blocks;  // offset 0x4
};

namespace {

typedef BlockAlloc<ObjLoc, 0x40> ObjAllocator;

struct ObjModel {
    ObjModel(void) { tex = -2; }
    ~ObjModel(void) {
        if (tex != -2) {
            fn_802CAC54(tex);
            tex = -2;
        }
        if (model) {
            fn_802C6C20(model);
        }
    }
    void* model;  // offset 0x0
    void* info;   // offset 0x4
    s32 tex;      // offset 0x8
};

struct ObjRot {
    f32 m[9];   // offset 0x0
    f32 scale;  // offset 0x24
};

struct ObjXform {
    ObjRot rot;  // offset 0x0 (scale slot unused)
    OVec pos;    // offset 0x24
};

struct ObjList {
    ObjBlock* head;  // offset 0x0
    ObjBlock* tail;  // offset 0x4
};

struct ObjectPlacer {
    ObjectPlacer(ValueNode* node);
    void Spin(void);
    s32 weight;        // offset 0x0
    u8 facing;         // offset 0x4
    f32 sway;          // offset 0x8
    ObjModel* model;   // offset 0xC
    ObjList list;      // offset 0x10
    ObjRot rots[16];   // offset 0x18
    f32 minScale;      // offset 0x298
    f32 maxScale;      // offset 0x29C
    u8 respin;         // offset 0x2A0
    const char* name;  // offset 0x2A4
};

struct MatPlacer {
    MatPlacer(ValueNode* node, int index);
    void Rebuild(void);
    int TotalWeight(void) {
        int total = 0;
        for (int i = 0; i < placers.count; i++) {
            total += placers.data[i]->weight;
        }
        return total;
    }
    void Draw(void);
    void Update(void);
    s32 index;                                // offset 0x0
    u8 clumpy;                                // offset 0x4
    u8 wasClumpy;                             // offset 0x5
    s32 order[128];                           // offset 0x8
    ObjectPlacer* slots[128];                 // offset 0x208
    InlineVec<ObjectPlacer*, 16> placers;     // offset 0x408
};

struct SetPlacer : InlineVec<MatPlacer*, 6> {
    SetPlacer(void);
    const char* name;  // offset 0x24
};

struct ObjTweak {
    const char* name;      // offset 0x0
    ObjectPlacer* placer;  // offset 0x4
    f32* scales;           // offset 0x8
};

}  // namespace

extern "C" {
bool fn_802B2C44(ValueNode* n, const char* name);
void* fn_802B2C34(void* (*f)(const char*), const char* a);
}

ModuleTag lbl_8055F9A8;
s32 lbl_8055F9B0;  // triangles drawn
s32 lbl_8055F9B4;  // objects drawn
s32 lbl_8055F9B8;  // instances dropped
ObjAllocator* lbl_8055F9BC;
InlineVec<SetPlacer*, 16> lbl_804BE1FC;

extern "C" {

// Only its string survives in the game.
void ObjDebug(void) { fn_802E30C8("gmat_set"); }

s32 fn_802B1D38(const char* name) {
    bool ok = false;
    if (name && fn_8012C33C(name)) {
        ok = true;
    }
    if (ok) {
        return fn_802CAC28(name, 0);
    }
    return -2;
}

s32 fn_802B1DA0(s32* m) {
    switch (m[0]) {
    case 'SIND':
        return m[8];
    case 'NDL2':
        return m[7] - 2;
    }
    return 1000;
}

bool fn_802B1DE8(ValueNode* n) { return strcasecmp("object", n->GetName()) == 0; }

void fn_802B1E28(void) {
    fn_802B8AE0("gobjects", &lbl_8055F9B4);
    fn_802B8AE0("gobject-tris", &lbl_8055F9B0);
    lbl_8055F9BC = new ("objloc-allocator") ObjAllocator;
}

}  // extern "C"

ObjLoc::ObjLoc(void) {}

extern "C" {

void fn_802B1FD4(void) {
    SafeDelete(lbl_8055F9BC);
    for (int i = 0; i < lbl_804BE1FC.count; i++) {
        SetPlacer* set = lbl_804BE1FC.data[i];
        if (set) {
            for (int j = 0; j < set->count; j++) {
                MatPlacer* m = set->data[j];
                if (m) {
                    for (int k = 0; k < m->placers.count; k++) {
                        ObjectPlacer* p = m->placers.data[k];
                        if (p) {
                            SafeDelete(p->model);
                            delete p;
                        }
                    }
                    m->placers.count = 0;
                    m->placers.~InlineVec();
                    operator delete(m);
                }
            }
            set->count = 0;
            delete set;
        }
    }
    lbl_804BE1FC.count = 0;
}

void fn_802B224C(ValueNode* node) {
    if (lbl_804BE1FC.count + 1 > lbl_804BE1FC.cap) {
        return;
    }
    SetPlacer* set = new ("umat-DLO-SetPlacer") SetPlacer;
    if (set) {
        for (int i = 1;; i++) {
            char name[16];
            fn_801345D8(name, 16, "mat%d", i);
            if (!fn_802B2C44(node, name)) {
                break;
            }
            MatPlacer* m = new ("ObjectSetPlacer") MatPlacer(node->FindChild(name), i - 1);
            set->Push(m);
        }
        set->name = node->GetValue("name");
    }
    lbl_804BE1FC.data[lbl_804BE1FC.count++] = set;
}

}  // extern "C"

MatPlacer::MatPlacer(ValueNode* node, int i) {
    clumpy = 0;
    wasClumpy = 0;
    int n = 0;
    for (u32 k = 0; k < 128; k++) {
        order[k] = n++;
    }
    Shuffle(order, 128);
    clumpy = wasClumpy = node->GetBool("clumpy", 1);
    ValueNode* c;
    for (int k = 0; (c = node->GetChildAt(k)) != 0; k++) {
        if (fn_802B1DE8(c)) {
            ObjectPlacer* p = new ("ObjectPlacer") ObjectPlacer(c);
            placers.Push(p);
        }
    }
    Rebuild();
    index = i;
}

// Spreads the weighted objects over the 128 slots, with gaps when the weights leave room.
inline void MatPlacer::Rebuild(void) {
    int total = 0;
    for (int i = 0; i < placers.count; i++) {
        total += placers.data[i]->weight;
    }
    if (total > 128) {
        int k = fn_80129D90(0, 0, placers.count);
        do {
            ObjectPlacer* p = placers.data[k];
            if (p->weight > 0) {
                p->weight--;
                total--;
            }
            k = (k + 1) % placers.count;
        } while (total > 128);
    }
    memset(slots, 0, sizeof(slots));
    int spread = (128 - total) * 2;
    int free = 128 - total;
    int at = 0;
    for (int i = 0; i < placers.count; i++) {
        int share = spread / placers.count;
        ObjectPlacer* p = placers.data[i];
        int lim = free;
        if (share < free) {
            lim = share;
        }
        if (lim > 0) {
            int gap = order[i & 127] % lim;
            for (int k = 0; k < gap; k++) {
                slots[at++] = 0;
            }
            free -= gap;
        }
        for (int k = 0; k < p->weight; k++) {
            slots[at++] = p;
        }
    }
    if (!clumpy) {
        Shuffle(slots, 128);
    }
}

// Sixteen random rotations about the vertical axis with random scales.
static inline void ScaledRow(f32* d, double x, double y, double z, f32 k) {
    f32 a = (f32)x * k;
    f32 b = (f32)y * k;
    f32 c = (f32)z * k;
    d[0] = a;
    d[1] = b;
    d[2] = c;
}

inline void ObjectPlacer::Spin(void) {
    f32 zero = 0.0f;
    f32 one = 1.0f;
    ObjRot* r = rots;
    for (u32 k = 0; k < 16; k++, r++) {
        r->scale = fn_80129DC4(0, minScale, maxScale);
        f32 a = fn_80129DC4(0, 0.0f, 6.2831855f);
        f32 c = cos(a);
        f32 s = sin(a);
        ScaledRow(&r->m[0], c, zero, -s, r->scale);
        ScaledRow(&r->m[3], zero, one, zero, r->scale);
        ScaledRow(&r->m[6], s, zero, c, r->scale);
    }
}

ObjectPlacer::ObjectPlacer(ValueNode* node) {
    ObjModel* m = new ("GroundObject") ObjModel;
    if (m) {
        const char* file = node->GetValue("model");
        if (strcasecmp(file, "nul") == 0) {
            m->model = 0;
        } else {
            m->model = fn_802B2C34(fn_802C6B44, file);
            if (m->model) {
                void* info = ((void**)m->model)[1];
                const char* tex = fn_802C9C24(info);
                void* surf = ((void**)info)[3];
                bool ok = false;
                if (surf) {
                    m->tex = fn_802B110C(fn_802B1D38, tex);
                    if (m->tex == -2) {
                        fn_802E317C("gobject: Can't find texture %s", tex);
                    }
                    m->info = surf;
                    ok = true;
                }
                if (!ok) {
                    fn_802E317C("gobject: model %s provides neither color nor normal IndexedSurface.", file);
                }
            } else {
                fn_802E317C("gobject: Can't load model %s", file);
            }
        }
    }
    model = m;
    list.tail = 0;
    list.head = 0;
    weight = node->GetInt("weight", 1);
    name = node->GetValue("model");
    sway = node->GetFloat("sway_radius", 0.0f);
    minScale = node->GetFloat("min_scale", 1.0f);
    maxScale = node->GetFloat("max_scale", 1.0f);
    facing = node->GetBool("axis_facing", 0);
    Spin();
    respin = 0;
}

extern "C" {

void* fn_802B2C34(void* (*f)(const char*), const char* a) { return f(a); }

bool fn_802B2C44(ValueNode* n, const char* name) { return n->FindChild(name) != 0; }

}  // extern "C"

SetPlacer::SetPlacer(void) {}

extern "C" {

SetPlacer* fn_802B2C98(const char* name) {
    SetPlacer* set = 0;
    for (int i = 0; i < lbl_804BE1FC.count; i++) {
        if (strcasecmp(name, lbl_804BE1FC.data[i]->name) == 0) {
            set = lbl_804BE1FC.data[i];
            break;
        }
    }
    return set;
}

void fn_802B2D24(SetPlacer* set) {
    for (int i = 0; i < set->count; i++) {
        MatPlacer* m = set->data[i];
        lbl_8055F9BC->blocks.count = 0;
        for (int k = 0; k < m->placers.count; k++) {
            ObjectPlacer* p = m->placers.data[k];
            p->list.tail = 0;
            p->list.head = 0;
        }
    }
    lbl_8055F9B8 = 0;
}

void fn_802B2D94(SetPlacer* set) {
    lbl_8055F9B4 = 0;
    lbl_8055F9B0 = 0;
    for (int i = 0; i < set->Count(); i++) {
        (*set->At(i))->Draw();
    }
}

}  // extern "C"

void MatPlacer::Draw(void) {
    for (int i = 0; i < placers.count; i++) {
        ObjectPlacer* p = placers.data[i];
        ObjBlock* b = p->list.head;
        if (!b) {
            continue;
        }
        if (p->respin) {
            p->Spin();
            p->respin = 0;
        }
        if (p->facing) {
            f32 face[9];
            fn_801F1C24(face);
            ObjRot* r = p->rots;
            for (int k = 0; k < 16; k++, r++) {
                for (int j = 0; j < 9; j++) {
                    r->m[j] = face[j] * r->scale;
                }
            }
        } else if (p->sway > 0.0f) {
            f32 t = fn_801E7E18();
            f32 t2 = 2.0f * t;
            ObjRot* r = p->rots;
            for (u32 k = 0; k < 16; k++, r++) {
                r->m[3] = p->sway * (f32)sin(fmod(3.1415927f * k / 5.0f + t, 6.283185307179586));
                r->m[5] = p->sway * (f32)cos(fmod(3.1415927f * k / 5.0f + t2, 6.283185307179586));
            }
        }
        fn_802C8BD0(fn_802C28C0());
        fn_802DC71C(p->model->info, p->model->tex);
        do {
            int n = b->count;
            for (int k = 0; k < n; k++) {
                ObjLoc* l = &b->data[k];
                ObjXform x;
                x.pos.x = l->pos.x;
                x.pos.y = l->pos.y;
                x.pos.z = l->pos.z;
                *(ObjRot*)&x.rot = p->rots[l->kind & 15];
                fn_802DCAE4(p->model->info, &x, l->scale);
            }
            b = b->next;
        } while (b);
        fn_802DC8B4();
        fn_802C8D74();
    }
}

extern "C" {

void fn_802B321C(InlineVec<ObjTweak, 1>* out, u8** clumpy, InlineVec<MatPlacer*, 6>* set, int layer) {
    if (layer < 0 || layer >= set->count) {
        fn_802E30C8("GetTweakers(%d): This layer index is out of range (%d->%d)", layer, 0, set->count);
        return;
    }
    MatPlacer* m = set->data[layer];
    if (out) {
        for (int k = 0; k < m->placers.count && out->count + 1 <= out->cap; k++) {
            ObjectPlacer* p = m->placers.data[k];
            ObjTweak* t = &out->data[out->count++];
            t->name = p->name;
            t->placer = p;
            t->scales = &p->minScale;
        }
    }
    if (clumpy) {
        *clumpy = &m->clumpy;
    }
}

void fn_802B32E0(SetPlacer* set) {
    for (int i = 0; i < set->Count(); i++) {
        (*set->At(i))->Update();
    }
}

}  // extern "C"

// A scratch list on the stack.
template <class T, int N>
struct LocalVec {
    LocalVec(void) {
        data = buf;
        cap = N;
        count = 0;
    }
    ~LocalVec(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    void Push(const T& v) { data[count++] = v; }
    void RemoveAt(int i) {
        if (i >= 0 && i < count) {
            if (i != count - 1) {
                data[i] = data[count - 1];
            }
            count--;
        }
    }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
    T buf[N];   // offset 0xC
};

// Rebuilds the slots; if clumping did not change, objects that stay keep their old slots.
void MatPlacer::Update(void) {
    if (wasClumpy != clumpy) {
        Rebuild();
    } else {
        ObjectPlacer* old[128];
        LocalVec<ObjectPlacer*, 128> added;
        LocalVec<ObjectPlacer*, 128> removed;
        memcpy(old, slots, sizeof(slots));
        Rebuild();
        for (int k = 0; k < 128; k++) {
            removed.Push(old[k]);
        }
        for (int k = 0; k < 128; k++) {
            added.Push(slots[k]);
        }
        for (int i = 0; i < removed.count;) {
            int j;
            for (j = 0; j < added.count; j++) {
                if (added.data[j] == removed.data[i]) {
                    break;
                }
            }
            if (j == added.count) {
                j = -1;
            }
            if (j != -1) {
                added.RemoveAt(j);
                removed.RemoveAt(i);
            } else {
                i++;
            }
        }
        for (int k = 0; added.count > 0; k = (k + 1) % 128) {
            ObjectPlacer** s = &old[order[k]];
            if (*s == removed.data[0]) {
                *s = added.data[0];
                added.RemoveAt(0);
                removed.RemoveAt(0);
            }
        }
        memcpy(slots, old, sizeof(slots));
    }
    wasClumpy = clumpy;
}

extern "C" {

ObjList* fn_802B3DA4(InlineVec<MatPlacer*, 6>* set, u16 id, int layer) {
    ObjList* r = 0;
    if (layer >= 0 && layer < set->count) {
        ObjectPlacer* p = set->data[layer]->slots[id & 127];
        r = 0;
        if (p) {
            r = 0;
            if (p->model->model) {
                r = &p->list;
            }
        }
    }
    return r;
}

void fn_802B3E00(ObjList* list, const OVec* pos, u16 kind, f32 scale) {
    ObjLoc* l = 0;
    ObjAllocator* a = lbl_8055F9BC;
    ObjBlock* tail = list->tail;
    if (!tail || tail->count + 1 > tail->cap) {
        ObjBlock* b = 0;
        if (a->blocks.count + 1 <= a->blocks.cap) {
            b = &a->blocks.data[a->blocks.count++];
            b->count = 0;
            b->next = 0;
        }
        if (b) {
            if (!list->head) {
                list->head = b;
            }
            if (list->tail) {
                list->tail->next = b;
            }
            list->tail = b;
            b->next = 0;
            l = &list->tail->data[list->tail->count++];
        }
    } else {
        l = &tail->data[tail->count++];
    }
    if (l) {
        l->pos = *pos;
        l->kind = kind;
        l->scale = scale;
    } else {
        lbl_8055F9B8++;
    }
}

}  // extern "C"

template <class T, int N>
inline T* InlineVec<T, N>::Push(const T& v) {
    T* p = &data[count++];
    *p = v;
    return p;
}

template <class T, int N>
inline int InlineVec<T, N>::Count(void) {
    return count;
}

template <class T, int N>
inline T* InlineVec<T, N>::At(int i) {
    return &data[i];
}



extern "C" int fn_802B3F0C(InlineVec<SetPlacer*, 16>* v) { return v->count; }
