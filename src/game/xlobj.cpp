// Extra ground layers (0x802B8E50-0x802BB87C, "xlobj"): textured decals laid over the ground - the
// racing-line road strips built from the path sets, tree shadows and placed ground objects. Each decal
// is filed under its texture ("xlte" entries sorted by priority) and binned into the ground cells it
// touches at every grid level; per frame the ground builder hands each cell's triangle strips to the
// decals over it, which then draw them with their own texture matrix. Names are guesses apart from the
// strings.
#include "game/common.h"

extern "C" {
void* memset(void* dst, int c, size_t n);
int strcasecmp(const char* a, const char* b);
double sqrt(double x);
double fmod(double x, double y);
double cos(double x);
double sin(double x);
double ceil(double x);
int fn_801345D8(char* dst, int n, const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);  // fatal
void fn_802E30C8(const char* fmt, ...);  // warning print
int fn_802E2C20(void);                   // milliseconds
int fn_8012C33C(const char* name);       // file exists
s32 fn_802CAC28(const char* name, int flags);
void fn_802CAC54(s32 tex);
ValueNode* fn_802B59CC(void);
void fn_802B8AE0(const char* name, s32* value);
s32 fn_802B110C(s32 (*load)(const char*), const char* name);
struct GroundCell* fn_802BE844(s16 x, s16 y, int size);
f32 Floor__Ff_80141C9C(f32 x);
void fn_801602B0(void* v);
void fn_80160BA0(struct V2* r, const struct V2* a, f32 s);
void fn_80160B18(struct V2* v);
void StatsUpdate__Fv(struct Vec3* v);
void fn_80144D64(struct Vec3* d, const struct Vec3* s);
const struct Vec3* fn_802C9A40(void);
f32 fn_801DCF48(struct Vec3* n, f32 x, f32 z);  // ground height
}  // extern "C"
f32 Remap(float, float, float, float, float);
extern "C" {
void fn_801286A0(void* p, s32 shrink);         // MemShrink
void fn_80265DCC(void* base, int num, int size, int (*cmp)(const void*, const void*));  // qsort
void* fn_801352B8(void* it);
void fn_80135D60(void* it, void* line);
void fn_8013530C(void* it, int del);
struct XSeg* fn_80138B4C(void* line, struct XSeg* prev);
void fn_80136414(struct XSample* out, void* it, int idx);
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
s32 fn_802C28C0(void);
s32 fn_802C21B8(void);
void fn_802CADD0(s32 tex, int stage);
void fn_802D3D94(int i, const f32 m[2][4]);
void fn_802DC130(void);
void fn_802DC134(void);
void fn_802DC278(void);
void fn_802DC27C(void);
void fn_802DC138(void* verts, int nverts, const u16* idx, int n);
void fn_802DC280(void* verts, int nverts, const u16* idx, int n);
}

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);
void operator delete(void* p);
void operator delete[](void* p);

struct V2 {
    f32 x;
    f32 y;
};

struct Vec3 {
    f32 x;
    f32 y;
    f32 z;
};

class Vec2 : public V2 {
public:
    Vec2(void) {}
    Vec2(const V2& v) { *this = (const Vec2&)v; }
    Vec2& operator=(const Vec2& v);
    void SetXZ(const Vec3& v);
};

inline void AddScaled(V2* r, const V2* a, f32 s, const V2* b);
inline f32 Dot(const V2* a, const V2* b);

struct GroundCell {
    struct Item** list;  // offset 0x0
    u8 pad4[4];
    u32 flags;  // offset 0x8
};

class XPathSet {
public:
    virtual void f8(void);
    virtual void* Get(int i);  // 0xC
};

extern "C" XPathSet* fn_80156A40(void);

struct XNode {
    f32 x;        // offset 0x0
    f32 y;        // offset 0x4
    f32 dx;       // offset 0x8
    f32 dz;       // offset 0xC
    u8 pad10[4];
    XNode* next;  // offset 0x14
    f32 len;      // offset 0x18
    f32 dist;     // offset 0x1C
};

struct XSample {
    void Pos(V2* out) {
        XNode* n = node;
        f32 u = t;
        out->x = n->x + u * (n->next->x - n->x);
        out->y = n->y + u * (n->next->y - n->y);
    }
    void Dir(V2* out) {
        XNode* n = node;
        f32 u = t;
        out->x = n->dx + u * (n->next->dx - n->dx);
        out->y = n->dz + u * (n->next->dz - n->dz);
    }
    XNode* node;  // offset 0x0
    f32 t;        // offset 0x4
    s32 seg;      // offset 0x8
};

struct XPoint {
    s16 x;    // offset 0x0
    s16 y;    // offset 0x2
    s32 idx;  // offset 0x4
};

struct XSeg {
    u8 pad0[8];
    s32 count;       // offset 0x8
    u8 padC[4];
    f32 width;       // offset 0x10
    XPoint pts[1];   // offset 0x14
};

// Times a block and reports it when it runs long.
struct ScopeTimer {
    ScopeTimer(const char* n) {
        name = n;
        start = fn_802E2C20();
    }
    ~ScopeTimer(void) {
        int t = fn_802E2C20() - start;
        if (t > 500) {
            fn_802E30C8("%s: took %dms", name, t);
        }
    }
    const char* name;  // offset 0x0
    int start;         // offset 0x4
};

template <class T>
struct VecBase {
    VecBase(void) {
        data = 0;
        cap = 0;
        count = 0;
    }
    VecBase(T* d, int n) {
        data = d;
        cap = n;
        count = 0;
    }
    ~VecBase(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

template <class T, int N>
struct InlineVec : VecBase<T> {
    InlineVec(void) : VecBase<T>(buf, N) {}
    T buf[N];  // offset 0xC
};

template <class T>
struct Arr : VecBase<T> {
    Arr(int n, const char* tag = "dsa_buf") : VecBase<T>(new (tag) T[n], n) { alloc = this->data; }
    ~Arr(void) { delete[] alloc; }
    void Shrink(void) {
        if (this->count < this->cap) {
            fn_801286A0(alloc, (this->cap - this->count) * sizeof(T));
            this->cap = this->count;
        }
    }
    T* alloc;  // offset 0xC
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

namespace {

template <class T>
struct Chunk : VecBase<T> {
    void Init(T* d, int n) {
        this->data = d;
        this->cap = n;
        this->count = 0;
    }
    void Reset(void) {
        this->count = 0;
        next = 0;
    }
    Chunk* next;  // offset 0xC
};

template <class T>
struct Pool {
    Pool(int n) : arr(n) {
        buf = new ("buffer_list[]") T[n * 4];
        for (int i = 0; i < arr.cap; i++) {
            arr.data[arr.count++].Init(buf + i * 4, 4);
        }
        arr.count = 0;
    }
    ~Pool(void) { SafeDeleteArray(buf); }
    Chunk<T>* Alloc(void) {
        Chunk<T>* c = 0;
        if (arr.count + 1 <= arr.cap) {
            c = &arr.data[arr.count++];
            c->Reset();
        }
        return c;
    }
    T* buf;                // offset 0x0
    Arr<Chunk<T> > arr;    // offset 0x4
};

template <class T>
struct Cell {
    Cell(void) { head = tail = 0; }
    void Clear(void) { head = tail = 0; }
    T* Alloc(Pool<T>* pool) {
        T* slot = 0;
        if (tail == 0 || tail->count + 1 > tail->cap) {
            Chunk<T>* c = pool->Alloc();
            if (c) {
                if (!head) {
                    head = c;
                }
                if (tail) {
                    tail->next = c;
                }
                tail = c;
                c->next = 0;
                slot = &tail->data[tail->count++];
            }
        } else {
            slot = &tail->data[tail->count++];
        }
        return slot;
    }
    Chunk<T>* head;  // offset 0x0
    Chunk<T>* tail;  // offset 0x4
};

}  // namespace

struct Item : Cell<u16*> {
    Item(void) {
        type = 3;
        on = 1;
    }
    int Type(void);
    struct Shadow* AsShadow(void);
    struct Obj* AsObj(void);
    u32 type;    // offset 0x8
    u8 on;       // offset 0xC
    Item* next;  // offset 0x10
};

struct Shadow : Item {
    void GetBounds(V2* lo, V2* hi);
    V2 lo;           // offset 0x14
    V2 hi;           // offset 0x1C
    f32 mtx[2][4];   // offset 0x24
};

struct XScale {
    f32 u;     // offset 0x0
    f32 v;     // offset 0x4
    s32 prio;  // offset 0x8
};

struct Obj : Item {
    s16 X(void);
    s16 Y(void);
    void Init(int x, int y, XScale* s) {
        type = 2;
        this->x = x;
        this->y = y;
        scale = s;
    }
    void SetMtx(V2 p, V2 d) {
        f32 su = scale->u;
        f32 sv = scale->v;
        mtx[0][0] = d.y * su;
        mtx[0][1] = 0.0f;
        mtx[0][2] = -d.x * su;
        mtx[0][3] = 0.5f + su * (-p.x * d.y + p.y * d.x);
        mtx[1][0] = d.x * sv;
        mtx[1][1] = 0.0f;
        mtx[1][2] = d.y * sv;
        mtx[1][3] = v + sv * (-p.x * d.x - p.y * d.y);
    }
    s32 x;          // offset 0x14
    s32 y;          // offset 0x18
    V2 pos;         // offset 0x1C
    V2 dir;         // offset 0x24
    f32 v;          // offset 0x2C
    XScale* scale;  // offset 0x30
    f32 mtx[2][4];  // offset 0x34
};

extern "C" s32 fn_802B8E50(const char* name);

static inline s32 LoadTex(const char* name) { return *name ? fn_802B110C(fn_802B8E50, name) : -2; }

struct TexEntry {
    TexEntry(const char* n, int p) {
        name = n;
        tex = LoadTex(n);
        if (tex == -2 && *n) {
            fn_802E317C("XLFanEmitter: Can't find texture %s", n);
        }
        head = 0;
        prio = p;
    }
    void SetName(const char* n) {
        tex = -2;
        tex = LoadTex(n);
        name = n;
        if (tex == -2 && *name) {
            fn_802E317C("XLFanEmitter: Can't find texture %s", name);
        }
    }
    const char* name;  // offset 0x0
    s32 tex;           // offset 0x4
    s32 prio;          // offset 0x8
    Item* head;        // offset 0xC
};

struct Grid {
    void Add(Item* it, int x, int y);
    void Put(Item* it, int x, int y) {
        int d = size * 2;
        Cell<Item*>* c = &cells[(y - size + 0x200) / d * (0x400 / d) + (x - size + 0x200) / d];
        Item** slot = c->Alloc(pool);
        if (slot) {
            *slot = it;
        }
    }
    s16 size;               // offset 0x0
    Cell<Item*>* cells;     // offset 0x4
    s32 n;                  // offset 0x8
    Pool<Item*>* pool;      // offset 0xC
};

ModuleTag lbl_8055FA60;
static inline u32 XLWord(u32 v) { return v; }
u32 lbl_8055FA68 = XLWord(0xFFFF0000);
s32 lbl_8055FA6C;  // triangles drawn
s32 lbl_8055FA70;  // fans queued
Pool<u16*>* lbl_8055FA74;
Arr<Obj>* lbl_8055FA78;
Arr<Shadow>* lbl_8055FA7C;
Arr<Item*>* lbl_8055FA80;
s32 lbl_8055FA84;
InlineVec<TexEntry*, 16> lbl_804BFFF4;
InlineVec<XScale, 4> lbl_804C004C;
TexEntry* lbl_804C0088[4];

static inline TexEntry* FindTex(const char* name, int prio) {
    for (int i = 0; i < lbl_804BFFF4.count; i++) {
        TexEntry* e = lbl_804BFFF4.data[i];
        if (strcasecmp(e->name, name) == 0) {
            return e;
        }
    }
    return lbl_804BFFF4.data[lbl_804BFFF4.count++] = new ("xlte") TexEntry(name, prio);
}

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

static inline f32 FMin(f32 a, f32 b) { return a < b ? a : b; }
static inline f32 FMax(f32 a, f32 b) { return a > b ? a : b; }

extern "C" {

s32 fn_802B8E50(const char* name) {
    int ok = 0;
    if (name && fn_8012C33C(name)) {
        ok = 1;
    }
    if (ok) {
        return fn_802CAC28(name, 0);
    }
    return -2;
}

GroundCell* fn_802B8EB8(s16 x, s16 y, int size, int corner) {
    int h = size / 2;
    return fn_802BE844(corner & 1 ? x + h : x - h, corner & 2 ? y + h : y - h, h);
}

TexEntry* fn_802B8EF0(int slot, XScale* sc) {
    if (!lbl_804C0088[slot]) {
        TexEntry* e = new ("xlte") TexEntry("", sc->prio);
        lbl_804C0088[slot] = e;
        lbl_804BFFF4.data[lbl_804BFFF4.count++] = e;
    }
    return lbl_804C0088[slot];
}

XScale* fn_802B8FE0(int i) {
    XScale* e = &lbl_804C004C.data[lbl_804C004C.count++];
    e->prio = 0;
    e->v = 0.0485732f;
    ValueNode* root = fn_802B59CC();
    char name[32];
    fn_801345D8(name, 0x20, "road%d", i + 1);
    ValueNode* n = root->FindChild(name);
    if (!n) {
        n = root->FindChild("road");
    }
    if (n) {
        e->v = n->GetFloat("tex_scale_v", e->v);
        e->prio = n->GetInt("prio", e->prio);
    }
    e->u = 0.5f;
    return e;
}

int fn_802B9100(GroundCell* c) { return 0; }

inline f32 Ceil(f32 x);
inline s16 LowBit(s16 v);

static inline void BinShadows(Grid& g, TexEntry* te, f32 s) {
    f32 h = g.size * s;
    f32 step = 2.0f * h;
    for (Item* it = te->head; it; it = it->next) {
        Vec2 lo, hi;
        fn_801602B0(&lo);
        fn_801602B0(&hi);
        it->AsShadow()->GetBounds(&lo, &hi);
        int x0 = (h + step * Floor__Ff_80141C9C(lo.x / step)) / s;
        int y0 = (h + step * Floor__Ff_80141C9C(lo.y / step)) / s;
        int x1 = (step * Ceil(hi.x / step) - h) / s;
        int y1 = (step * Ceil(hi.y / step) - h) / s;
        for (s16 x = x0; x <= (s16)x1; x += g.size * 2) {
            for (s16 y = y0; y <= (s16)y1; y += g.size * 2) {
                g.Add(it, x, y);
            }
        }
    }
}

void fn_802B9108(s16 size) {
    int d = size * 2;
    Grid g;
    g.size = size;
    int k = 0x400 / d;
    int n = k * k;
    g.n = n;
    g.cells = new ("tmp") Cell<Item*>[n];
    g.pool = new ("tmpalloc") Pool<Item*>(n * 6 / 4);
    for (int i = 0; i < lbl_804BFFF4.count; i++) {
        TexEntry* te = lbl_804BFFF4.data[i];
        switch (te->head->Type()) {
        case 0:
        case 1:
            if (g.size == 4) {
                BinShadows(g, te, 2.0f);
            }
            break;
        case 2:
            for (Item* it = te->head; it; it = it->next) {
                Obj* o = it->AsObj();
                if (g.size == LowBit(o->X())) {
                    g.Add(it, o->X(), o->Y());
                }
            }
            break;
        }
    }
    s16 sz = g.size;
    if (sz > 4 && sz <= 8) {
        for (int x = sz - 0x200; x < 0x200; x += sz * 2) {
            for (int y = sz - 0x200; y < 0x200; y += sz * 2) {
                InlineVec<Item*, 64> near;
                for (int k = 0; k < 4; k++) {
                    for (Item** pp = fn_802B8EB8(x, y, sz, k)->list; *pp; pp++) {
                        u32 t = (*pp)->type;
                        if (t <= 1) {
                            int idx;
                            int j;
                            for (j = 0; j < near.count; j++) {
                                if (near.data[j] == *pp) {
                                    idx = j;
                                    goto found;
                                }
                            }
                            idx = -1;
                        found:
                            if (idx == -1 && near.count + 1 <= near.cap) {
                                if (t != 1 || g.size <= 4) {
                                    near.data[near.count++] = *pp;
                                }
                            }
                        }
                    }
                }
                for (int j = 0; j < near.count; j++) {
                    g.Put(near.data[j], x, y);
                }
            }
        }
    }
    for (int x = size - 0x200; x < 0x200; x += d) {
        for (int y = size - 0x200; y < 0x200; y += d) {
            GroundCell* cell = fn_802BE844(x, y, size);
            int start = lbl_8055FA80->count;
            int s = g.size;
            int dd = s * 2;
            for (Chunk<Item*>* c = g.cells[(y - s + 0x200) / dd * (0x400 / dd) + (x - s + 0x200) / dd].head; c;
                 c = c->next) {
                int cnt = c->count;
                for (int j = 0; j < cnt; j++) {
                    Item* it = c->data[j];
                    if (it->type == 2 && fn_802B9100(cell)) {
                        continue;
                    }
                    lbl_8055FA80->data[lbl_8055FA80->count++] = it;
                    int inhibit = 0;
                    if (it->type == 2 && fn_802B59CC()->GetBool("road_inhibits_skags", 1)) {
                        inhibit = 1;
                    }
                    if (inhibit) {
                        cell->flags |= 0x800000;
                    }
                    if (it->type == 2) {
                        cell->flags |= 0x1000000;
                    }
                }
            }
            lbl_8055FA80->data[lbl_8055FA80->count++] = 0;
            cell->list = &lbl_8055FA80->data[start];
        }
    }
    delete g.pool;
    delete[] g.cells;
}

inline s16 Obj::Y(void) { return y; }

static const s16 lbl_802FAB60[32] = {1, 2, 1, 4, 1, 2, 1, 8, 1, 2, 1, 4, 1, 2, 1, 16,
                                     1, 2, 1, 4, 1, 2, 1, 8, 1, 2, 1, 4, 1, 2, 1, 32};

inline s16 LowBit(s16 v) {
    v += 0x200;
    v %= 32;
    return lbl_802FAB60[v];
}

inline s16 Obj::X(void) { return x; }

inline Obj* Item::AsObj(void) { return (Obj*)this; }

inline void Grid::Add(Item* it, int x, int y) {
    int d = size * 2;
    Cell<Item*>* c = &cells[(y - size + 0x200) / d * (0x400 / d) + (x - size + 0x200) / d];
    Item** slot = c->Alloc(pool);
    if (slot) {
        *slot = it;
    }
}

inline f32 Ceil(f32 x) { return ceil(x); }

inline void Shadow::GetBounds(V2* lo, V2* hi) {
    lo->x = this->lo.x;
    lo->y = this->lo.y;
    hi->x = this->hi.x;
    hi->y = this->hi.y;
}

inline Shadow* Item::AsShadow(void) { return (Shadow*)this; }

inline int Item::Type(void) { return type; }

int fn_802B9B50(const void* a, const void* b) { return (*(TexEntry**)a)->prio - (*(TexEntry**)b)->prio; }

void fn_802B9B68(void) {
    fn_802B8AE0("xlobj-tris", &lbl_8055FA6C);
    fn_802B8AE0("xlobj-fans", &lbl_8055FA70);
    memset(lbl_804C0088, 0, sizeof(lbl_804C0088));
    lbl_8055FA74 = new ("XLOFanAlloc") Pool<u16*>(0x500);
    lbl_8055FA7C = 0;
    lbl_8055FA78 = 0;
}

void fn_802B9D08(void) {
    SafeDelete(lbl_8055FA7C);
    SafeDelete(lbl_8055FA78);
    for (int i = 0; i < lbl_804BFFF4.count; i++) {
        TexEntry* e = lbl_804BFFF4.data[i];
        if (e) {
            e->head = 0;
            if (e->tex != -2) {
                fn_802CAC54(e->tex);
            }
            delete e;
        }
    }
    lbl_804BFFF4.count = 0;
    lbl_804C004C.count = 0;
    SafeDelete(lbl_8055FA80);
    memset(lbl_804C0088, 0, sizeof(lbl_804C0088));
    SafeDelete(lbl_8055FA74);
}

void fn_802BA004(void) { lbl_8055FA84 = 0; }


void fn_802BA010(void* line, int idx) {
    XSample smp;
    ScopeTimer timer("XLAddRacingLine");
    if (!line) {
        line = fn_80156A40()->Get(0);
    }
    u8 it[0x34];
    fn_801352B8(it);
    fn_80135D60(it, line);
    XScale* sc = fn_802B8FE0(idx);
    XSeg* s = fn_80138B4C(line, 0);
    if (s) {
        sc->u = 1.0f / s->width;
        do {
            TexEntry* te = fn_802B8EF0(idx, sc);
            if (te) {
                XPoint* pt = s->pts;
                for (int k = 0; k < s->count; k++, pt++) {
                    fn_80136414(&smp, it, pt->idx);
                    Obj* o = &lbl_8055FA78->data[lbl_8055FA78->count++];
                    o->Init(pt->x, pt->y, sc);
                    smp.Pos(&o->pos);
                    smp.Dir(&o->dir);
                    f32 inv = 1.0f / (f32)sqrt(o->dir.x * o->dir.x + o->dir.y * o->dir.y);
                    o->dir.x *= inv;
                    o->dir.y *= inv;
                    o->v = fmod(sc->v * (smp.node->dist + smp.node->len * smp.t), 1.0);
                    o->SetMtx(o->pos, o->dir);
                    o->next = te->head;
                    te->head = o;
                    lbl_8055FA84++;
                }
            }
            s = fn_80138B4C(line, s);
        } while (s);
    }
    fn_8013530C(it, -1);
}

void fn_802BA348(void) {
    ScopeTimer timer("XLAddRacingLines");
    InlineVec<void*, 4> lines;
    void* first = fn_80156A40()->Get(0);
    void* l = first;
    int i = 0;
    while (lines.count + 1 <= lines.cap && i < 3) {
        if (l) {
            lines.data[lines.count++] = l;
        }
        i++;
        l = fn_80156A40()->Get(i);
        if (l == first || !l) {
            break;
        }
    }
    int total = 0;
    for (int j = 0; j < lines.count; j++) {
        void* ln = lines.data[j];
        for (XSeg* s = fn_80138B4C(ln, 0); s; s = fn_80138B4C(ln, s)) {
            total += s->count;
        }
    }
    lbl_8055FA78 = new ("roadobject[]") Arr<Obj>(total, "roadobject[]");
    for (int j = 0; j < lines.count; j++) {
        fn_802BA010(lines.data[j], j);
    }
}

void fn_802BA848(Shadow* s, const V2* c, const V2* b, const V2* a);
void fn_802BAA50(V2* v);

Shadow* fn_802BA580(ValueNode* node, const V2* pos, f32 scale) {
    Shadow* s = &lbl_8055FA7C->data[lbl_8055FA7C->count++];
    Vec2 org;
    fn_801602B0(&org);
    Vec2 along;
    fn_801602B0(&along);
    Vec2 side;
    fn_801602B0(&side);
    Vec2 size;
    fn_801602B0(&size);
    ValPoint p = node->GetPoint("size");
    fn_80160BA0(&size, (V2*)&p, scale);
    Vec3 sun;
    fn_80144D64(&sun, fn_802C9A40());
    Vec2 dir;
    fn_801602B0(&dir);
    dir.SetXZ(sun);
    fn_80160B18(&dir);
    fn_80160BA0(&along, &dir, size.y);
    fn_80160BA0(&side, &dir, size.x);
    fn_802BAA50(&side);
    AddScaled(&org, pos, -0.5f, &side);
    Vec2 at;
    fn_801602B0(&at);
    AddScaled(&at, pos, 0.5f, &along);
    Vec3 nrm;
    StatsUpdate__Fv(&nrm);
    if (fn_801DCF48(&nrm, at.x, at.y) != 0.0f) {
        Vec2 n2;
        fn_801602B0(&n2);
        n2.SetXZ(nrm);
        f32 k = Remap(-0.6f, -0.1f, 0.4f, 1.0f, Dot(&n2, &dir));
        fn_80160BA0(&along, &along, k);
    }
    fn_802BA848(s, &side, &along, &org);
    s->type = 1;
    TexEntry* te = FindTex(node->GetValue("texture"), 999);
    s->next = te->head;
    te->head = s;
    lbl_8055FA84++;
    return s;
}

void fn_802BA848(Shadow* s, const V2* c, const V2* b, const V2* a) {
    f32 det = c->x * b->y - b->x * c->y;
    f32 i00 = b->y / det;
    f32 i01 = -b->x / det;
    f32 i10 = -c->y / det;
    f32 i11 = c->x / det;
    s->mtx[0][0] = i00;
    s->mtx[0][1] = 0.0f;
    s->mtx[0][2] = i01;
    s->mtx[0][3] = -(a->x * i00 + a->y * i01);
    s->mtx[1][0] = i10 * -1.0f;
    s->mtx[1][1] = 0.0f * -1.0f;
    s->mtx[1][2] = i11 * -1.0f;
    s->mtx[1][3] = (f32)-(a->y * i11 + a->x * i10) * -1.0f + 1.0f;
    s->lo.x = FMin(a->x, FMin(a->x + c->x, FMin(a->x + b->x, b->x + (a->x + c->x))));
    s->lo.y = FMin(a->y, FMin(a->y + c->y, FMin(a->y + b->y, b->y + (a->y + c->y))));
    s->hi.x = FMax(a->x, FMax(a->x + c->x, FMax(a->x + b->x, b->x + (a->x + c->x))));
    s->hi.y = FMax(a->y, FMax(a->y + c->y, FMax(a->y + b->y, b->y + (a->y + c->y))));
}

void fn_802BAA50(V2* v) {
    f32 x = v->x;
    v->x = v->y;
    v->y = -x;
}

void fn_802BAD0C(V2* v, f32 a);
f32 fn_802BAD90(f32 deg);

Shadow* fn_802BAA68(ValueNode* node, const ValMatrix* m) {
    Shadow* s = &lbl_8055FA7C->data[lbl_8055FA7C->count++];
    Vec2 org;
    fn_801602B0(&org);
    Vec2 along;
    fn_801602B0(&along);
    Vec2 side;
    fn_801602B0(&side);
    along.SetXZ(*(const Vec3*)&m->basis[2]);
    fn_80160B18(&along);
    side.SetXZ(*(const Vec3*)&m->basis[0]);
    fn_80160B18(&side);
    org.SetXZ(*(const Vec3*)&m->origin);
    ValPoint p = node->GetPoint("size");
    Vec2 size = *(V2*)&p;
    fn_80160BA0(&along, &along, size.y);
    fn_80160BA0(&side, &side, size.y);
    f32 rot = fn_802BAD90(-1.0f * node->GetFloat("rot", 0.0f));
    fn_802BAD0C(&along, rot);
    fn_802BAD0C(&side, rot);
    AddScaled(&org, &org, -0.5f, &side);
    AddScaled(&org, &org, -0.5f, &along);
    fn_802BA848(s, &side, &along, &org);
    s->type = 0;
    const char* tex = node->GetValue("texture");
    TexEntry* te = FindTex(tex, node->GetInt("prio", 888));
    s->next = te->head;
    te->head = s;
    lbl_8055FA84++;
    return s;
}

void fn_802BAD0C(V2* v, f32 a) {
    f32 x = v->x;
    f32 c = cos(a);
    f32 s = sin(a);
    v->y = x * s + v->y * c;
    v->x = v->x * c - v->y * s;
}

f32 fn_802BAD90(f32 deg) { return 0.017453292f * deg; }

void fn_802BAD9C(int n) { lbl_8055FA7C = new ("treeshdwlst") Arr<Shadow>(n, "treeshdwlst"); }

void fn_802BAE50(void) {
    ScopeTimer timer("XLEndAddObjects");
    int total = 0;
    for (s16 sz = 4; sz <= 0x40; sz *= 2) {
        int k = 0x400 / (sz * 2);
        total += k * k * 5;
    }
    lbl_8055FA80 = new ("gpobject") Arr<Item*>(total);
    for (s16 sz = 4; sz <= 0x40; sz *= 2) {
        fn_802B9108(sz);
    }
    lbl_8055FA80->Shrink();
    fn_80265DCC(lbl_804BFFF4.data, lbl_804BFFF4.count, 4, fn_802B9B50);
}

void fn_802BAF98(Item* it, u8 on) { it->on = on; }

void fn_802BAFA0(void) {
    lbl_8055FA6C = 0;
    lbl_8055FA70 = 0;
    for (int i = 0; i < lbl_804BFFF4.count; i++) {
        for (Item* it = lbl_804BFFF4.data[i]->head; it; it = it->next) {
        }
    }
    lbl_8055FA74->arr.count = 0;
}

void fn_802BAFF8(GroundCell* cell, u16** strip) {
    for (Item** pp = cell->list; *pp; pp++) {
        u16** slot = (*pp)->Alloc(lbl_8055FA74);
        if (slot) {
            *slot = *strip;
            lbl_8055FA70++;
        }
    }
}

static inline int Tris(int n) { return n < 0 ? 0 : n; }

void fn_802BB0FC(void* verts, int nverts) {
    for (int i = 0; i < lbl_804BFFF4.count; i++) {
        TexEntry* te = lbl_804BFFF4.data[i];
        if (te->prio != 999) {
            continue;
        }
        Item* head = te->head;
        if (!head) {
            continue;
        }
        if (te->tex != -2) {
            switch (head->type) {
            case 0:
            case 1:
                fn_802C8BD0(te->tex);
                fn_802DC130();
                for (Item* it = te->head; it; it = it->next) {
                    if (!it->head) {
                        continue;
                    }
                    if (it->on) {
                        fn_802D3D94(0, ((Shadow*)it)->mtx);
                        for (Chunk<u16*>* c = it->head; c; c = c->next) {
                            for (int j = 0; j < c->count; j++) {
                                u16* st = c->data[j];
                                fn_802DC138(verts, nverts, st, st[0] + 1);
                                lbl_8055FA6C += Tris(c->data[j][0] - 1);
                            }
                        }
                    }
                    it->Clear();
                }
                fn_802DC134();
                fn_802C8D74();
                break;
            case 2: {
                fn_802C8BD0(fn_802C28C0());
                fn_802DC278();
                fn_802CADD0(te->tex, 1);
                fn_802CADD0(fn_802C21B8(), 2);
                f32 m[2][4] = {{1.0f / 2048, 0.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 1.0f / 2048, 0.5f}};
                fn_802D3D94(2, m);
                for (Item* it = te->head; it; it = it->next) {
                    if (!it->head) {
                        continue;
                    }
                    fn_802D3D94(0, ((Obj*)it)->mtx);
                    for (Chunk<u16*>* c = it->head; c; c = c->next) {
                        for (int j = 0; j < c->count; j++) {
                            u16* st = c->data[j];
                            fn_802DC280(verts, nverts, st, st[0] + 1);
                            lbl_8055FA6C += Tris(c->data[j][0] - 1);
                        }
                    }
                    it->Clear();
                }
                fn_802CADD0(-1, 1);
                fn_802CADD0(-1, 2);
                fn_802DC27C();
                fn_802C8D74();
                break;
            }
            }
        } else {
            for (Item* it = head; it; it = it->next) {
                it->Clear();
            }
        }
    }
}

void fn_802BB3B8(void* verts, int nverts) {
    for (int i = 0; i < lbl_804BFFF4.count; i++) {
        TexEntry* te = lbl_804BFFF4.data[i];
        if (te->prio == 999) {
            continue;
        }
        Item* head = te->head;
        if (!head) {
            continue;
        }
        if (te->tex != -2) {
            switch (head->type) {
            case 0:
            case 1:
                fn_802C8BD0(te->tex);
                fn_802DC130();
                for (Item* it = te->head; it; it = it->next) {
                    if (!it->head) {
                        continue;
                    }
                    if (it->on) {
                        fn_802D3D94(0, ((Shadow*)it)->mtx);
                        for (Chunk<u16*>* c = it->head; c; c = c->next) {
                            for (int j = 0; j < c->count; j++) {
                                u16* st = c->data[j];
                                fn_802DC138(verts, nverts, st, st[0] + 1);
                                lbl_8055FA6C += Tris(c->data[j][0] - 1);
                            }
                        }
                    }
                    it->Clear();
                }
                fn_802DC134();
                fn_802C8D74();
                break;
            case 2: {
                fn_802C8BD0(fn_802C28C0());
                fn_802DC278();
                fn_802CADD0(te->tex, 1);
                fn_802CADD0(fn_802C21B8(), 2);
                f32 m[2][4] = {{1.0f / 2048, 0.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 1.0f / 2048, 0.5f}};
                fn_802D3D94(2, m);
                for (Item* it = te->head; it; it = it->next) {
                    if (!it->head) {
                        continue;
                    }
                    fn_802D3D94(0, ((Obj*)it)->mtx);
                    for (Chunk<u16*>* c = it->head; c; c = c->next) {
                        for (int j = 0; j < c->count; j++) {
                            u16* st = c->data[j];
                            fn_802DC280(verts, nverts, st, st[0] + 1);
                            lbl_8055FA6C += Tris(c->data[j][0] - 1);
                        }
                    }
                    it->Clear();
                }
                fn_802CADD0(-1, 1);
                fn_802CADD0(-1, 2);
                fn_802DC27C();
                fn_802C8D74();
                break;
            }
            }
        } else {
            for (Item* it = head; it; it = it->next) {
                it->Clear();
            }
        }
    }
}

void fn_802BB674(const char* name, int slot) {
    TexEntry* te = lbl_804C0088[slot];
    if (te && strcasecmp(te->name, name) != 0) {
        if (te->tex != -2) {
            fn_802CAC54(te->tex);
        }
        te->SetName(name);
    }
}

}  // extern "C"
