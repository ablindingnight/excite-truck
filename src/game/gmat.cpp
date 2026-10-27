// Ground materials (0x802B0628-0x802B1D38): "umat" ground materials from the world's material table. Each
// material ("matN" with a "ground" subtable) is a layer of up to six blended textures with uv scales, a
// detail texture and on/off-road surface types. Per frame the ground builder adds vertices and 24-index
// triangle batches into 65 lists keyed by layer mask; drawing walks the layers and their lists, then draws
// the world map tint and the ocean quad. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
void* memset(void* dst, int c, size_t n);
int fn_801345D8(char* dst, int n, const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);  // fatal
int fn_8012C33C(const char* name);       // file exists
s32 fn_802CAC28(const char* name, int flags);
void fn_802CAC54(s32 tex);
int fn_802CAF8C(s32 tex);  // texture has alpha
ValueNode* fn_802B59CC(void);
int fn_802B05E4(void);
int fn_802B05EC(void);
void fn_802B8AE0(const char* name, s32* value);
void* fn_802CFFA0(u32 size);  // frame allocation
void fn_802BAFA0(void);
void fn_802BAFF8(void* ctx, void* batch);
s32 fn_802C2C00(s32 mode);
s32 fn_802C28C0(void);
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
void fn_802DBF00(void);
void fn_802DBF30(void);
void fn_802CADD0(s32 tex, int stage);
s32 fn_802BC30C(int layer);
void fn_802DC078(void* verts, int nverts, u16* idx, int n, int first, int layer);
void fn_802BB0FC(void* verts, int n);
void fn_802BB3B8(void* verts, int n);
void fn_802DA704(int n, const f32* m, int k);
void fn_802DAA18(void);
void fn_802DC3C0(s32 tex);
void fn_802DC464(void* verts, int nverts, const u16* idx, int n);
void fn_802DC440(void);
f32 fn_801DCF48(int a, f32 x, f32 z);  // ground height
s32 fn_802B110C(s32 (*load)(const char*), const char* name);
int strcasecmp(const char* a, const char* b);
}

void* operator new(size_t size, const char* name);
void* operator new[](size_t size, const char* name);
void operator delete(void* p);
void operator delete[](void* p);

template <class T>
void SafeDeleteArray(T*& p);
extern "C" void fn_802AE64C(s16** p);

template <class T>
struct IdxBlock {
    IdxBlock(void) {
        data = 0;
        cap = 0;
        count = 0;
    }
    ~IdxBlock(void) {
        if (this) {
            data = 0;
            count = 0;
            cap = 0;
        }
    }
    T* At(int i);
    int Count(void);
    bool Fits(int n) { return count + n <= cap; }
    T* Take(int n) {
        T* p = &data[count];
        count += n;
        return p;
    }
    T* data;         // offset 0x0
    s32 count;       // offset 0x4
    s32 cap;         // offset 0x8
    IdxBlock* next;  // offset 0xC
};

template <class T>
struct DynVec {
    bool Room(void) { return count + 1 <= cap; }
    T* Next(void) { return &data[count++]; }
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

template <class T, int N, int M>
struct BlockAlloc {
    BlockAlloc(void) : blocks(M) {
        pool = new ("idxbuf[]") T[blocks.cap << 9];
        for (int i = 0; i < blocks.cap; i++) {
            T* p = &pool[i * N];
            IdxBlock<T>* b = &blocks.data[blocks.count++];
            b->data = p;
            b->cap = N;
            b->count = 0;
        }
        blocks.count = 0;
    }
    ~BlockAlloc(void) { fn_802AE64C(&pool); }
    IdxBlock<T>* Get(void) {
        IdxBlock<T>* b = 0;
        if (blocks.Room()) {
            b = blocks.Next();
            b->count = 0;
            b->next = 0;
        }
        return b;
    }
    T* pool;                       // offset 0x0
    DynVec<IdxBlock<T> > blocks;  // offset 0x4
};

typedef IdxBlock<s16> GBlock;
typedef BlockAlloc<s16, 0x200, 0x54> GAllocator;

template <class T>
struct VecBase {
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
    int Count(void);
    T* At(int i);
    T buf[N];  // offset 0xC
};

template <class T>
void SafeDelete(T*& p) {
    if (p) {
        delete p;
        p = 0;
    }
}

namespace {

struct GVec {
    f32 x;
    f32 y;
    f32 z;
};

struct GTex {
    s32 tex;    // offset 0x0
    f32 u;      // offset 0x4
    f32 v;      // offset 0x8
    s32 index;  // offset 0xC
};

struct GLayer {
    GLayer(void);
    void Load(ValueNode* ground, const char* mname, ValueNode* tune);
    ~GLayer(void) {}
    u8 pad0[4];
    InlineVec<GTex, 6> texs;  // offset 0x4
    const char* name;         // offset 0x70
    s32 surface[2];           // offset 0x74: on road, off road
    s32 detail;               // offset 0x7C
};

struct GVerts {
    GVerts(void) {
        data = 0;
        cap = 0;
        count = 0;
    }
    ~GVerts(void) {
        if (this) {
            data = 0;
            count = 0;
            cap = 0;
        }
    }
    GVec* At(int i);
    int Count(void);
    GVec* data;  // offset 0x0
    s32 count;   // offset 0x4
    s32 cap;     // offset 0x8
};

// A see-through strip of the ocean: drawn again with its own alpha.
struct GStrip {
    u16* idx;   // offset 0x0
    f32 alpha;  // offset 0x4
};

// Index blocks of one layer mask, newest first.
struct GList {
    void Clear(void) { head = 0; }
    bool Empty(void);
    GBlock* Head(void);
    GBlock* head;  // offset 0x0
};

struct GBatch {
    u16* idx;    // offset 0x0
    u16 used;    // offset 0x4
    u16 count;   // offset 0x6
    u32 flags;   // offset 0x8
};

}  // namespace

template <class T>
inline void ClearRange(T* p, T* end) {
    for (; p < end; p++) {
        p->head = 0;
    }
}

struct GMat {
    GMat(ValueNode* node, ValueNode* tune);
    u8 crystal;                       // offset 0x0
    u8 merged;                        // offset 0x1
    s32 worldmap;                     // offset 0x4
    s32 oceanmap;                     // offset 0x8
    InlineVec<GLayer, 6> layers;      // offset 0xC
    GVerts verts;                     // offset 0x318
    InlineVec<GStrip, 0xC0> strips;   // offset 0x324
    u32 pad930;
    GList lists[65];                  // offset 0x934
    GList* pending;                   // offset 0xA38
    const char* name;                 // offset 0xA3C
};


ModuleTag lbl_8055F990;
s32 lbl_8055F998;  // gmat triangles
s32 lbl_8055F99C;  // world map triangles
GAllocator* lbl_8055F9A0;
extern bool lbl_8055FA59;  // draw debug
InlineVec<GMat*, 32> lbl_804BE104;
static GVec lbl_804BE190[8];  // ocean corners
extern const f32 kWorldScale;
const f32 kWorldScale = 2.0f;

static inline bool HasChild(ValueNode* n, const char* name) { return n->FindChild(name) != 0; }

extern "C" {

s32 fn_802B0628(const char* name) {
    bool ok = false;
    if (name && fn_8012C33C(name)) {
        ok = true;
    }
    if (ok) {
        return fn_802CAC28(name, 0);
    }
    return -2;
}

void fn_802B0690(s32* tex) {
    if (*tex != -2) {
        fn_802CAC54(*tex);
        *tex = -2;
    }
}

int fn_802B06D4(void) { return fn_802B59CC()->GetBool("crystal_mode", 0); }

// Only their strings survive in the game.
void GMatDebug1(void) {
    fn_802E317C("layerset");
    fn_802E317C("ground_material");
}

void fn_802B0710(void) { lbl_8055F9A0 = new ("gmat-tba") GAllocator; }

void fn_802B0874(void) { SafeDelete(lbl_8055F9A0); }

void fn_802B0904(void) { lbl_8055F9A0->blocks.count = 0; }

void fn_802B0914(void) {
    fn_802B8AE0("last-gmat-tris", &lbl_8055F998);
    fn_802B8AE0("last-wmap-tris", &lbl_8055F99C);
    fn_802B0710();
}

void fn_802B0954(void) {
    fn_802B0874();
    for (int i = 0; i < lbl_804BE104.count; i++) {
        GMat* g = lbl_804BE104.data[i];
        if (g) {
            for (int j = 0; j < g->layers.count; j++) {
                GLayer* l = &g->layers.data[j];
                fn_802B0690(&l->detail);
                for (int k = 0; k < l->texs.count; k++) {
                    fn_802B0690(&l->texs.data[k].tex);
                }
                l->texs.count = 0;
            }
            g->layers.count = 0;
            if (g->oceanmap == g->worldmap) {
                g->oceanmap = -2;
            }
            fn_802B0690(&g->worldmap);
            fn_802B0690(&g->oceanmap);
            g->strips.~InlineVec();
            g->verts.~GVerts();
            g->layers.~InlineVec();
            operator delete(g);
        }
    }
    lbl_804BE104.count = 0;
}

inline void GLayer::Load(ValueNode* ground, const char* mname, ValueNode* tune) {
    f32 u[2] = {0.2f, 0.05f};
    f32 v[2] = {0.2f, 0.05f};
    if (tune) {
        u[0] = tune->GetFloat("tex1_uscale");
        u[1] = tune->GetFloat("tex2_uscale");
        v[0] = tune->GetFloat("tex1_vscale");
        v[1] = tune->GetFloat("tex2_vscale");
    }
    detail = fn_802B0628(ground->GetString("detail_tex", "potato.tex"));
    for (int k = 1;; k++) {
        char tname[16];
        fn_801345D8(tname, 16, "tex%d", k);
        if (!ground->Has(tname)) {
            break;
        }
        const char* file = ground->GetValue(tname);
        GTex* t = &texs.data[texs.count++];
        t->u = u[k - 1];
        t->v = v[k - 1];
        t->index = k - 1;
        t->tex = fn_802B110C(fn_802B0628, file);
        if (t->tex == -2) {
            fn_802E317C("gmat: Texture %s does not exist.", file);
        }
        if (k == 1 && t->tex != -2 && fn_802CAF8C(t->tex) && !fn_802B06D4()) {
            fn_802E317C("gmat: texture #1 (\"%s\") has alpha enabled, would be ignored.", file);
        }
    }
    name = mname;
    memset(surface, 0, sizeof(surface));
    surface[1] = ground->GetInt("offroad_surface_type", 2);
    surface[0] = ground->GetInt("onroad_surface_type", 0);
}

inline GMat::GMat(ValueNode* node, ValueNode* tune) {
    lists[0].head = 0;
    GList* end = &lists[65];
    for (GList* p = &lists[1]; p < end; p++) {
        p->head = 0;
    }
    crystal = !fn_802B05E4() && !fn_802B06D4();
    for (int i = 1;; i++) {
        char mat[16];
        fn_801345D8(mat, 16, "mat%d", i);
        if (!HasChild(node, mat)) {
            break;
        }
        if (HasChild(node->FindChild(mat), "ground")) {
            ValueNode* ground = node->FindChild(mat)->FindChild("ground");
            const char* mname = node->FindChild(mat)->GetName();
            layers.data[layers.count++].Load(ground, mname, tune);
        } else {
            fn_802E317C("gmat: umat table: %s does not contain ground subtable", mat);
        }
    }
    if (layers.count == 0) {
        fn_802E317C("gmat: mat %s contains no layers", name);
    }
    name = node->GetValue("name");
    worldmap = fn_802B0628(node->GetString("worldmap", 0));
    oceanmap = fn_802B0628(node->GetString("oceanmap", "oceanmap.tex"));
    if (oceanmap == -2) {
        oceanmap = worldmap;
    }
}

void fn_802B0B20(ValueNode* node, ValueNode* tune) {
    if (lbl_804BE104.count + 1 > lbl_804BE104.cap) {
        return;
    }
    GMat* g = new ("umat-gmaterial") GMat(node, tune);
    lbl_804BE104.data[lbl_804BE104.count++] = g;
}

// Only their strings survive in the game.
void GMatDebug2(void) {
    fn_802E317C("resource");
    fn_802E317C("GroundLayer: Can't load %s");
    fn_802E317C("GMatTGASet");
}

s32 fn_802B110C(s32 (*load)(const char*), const char* name) { return load(name); }

}  // extern "C"

GLayer::GLayer(void) {}

extern "C" {

GMat* fn_802B1138(const char* name) {
    GMat* g = 0;
    for (int i = 0; i < lbl_804BE104.count; i++) {
        if (strcasecmp(name, lbl_804BE104.data[i]->name) == 0) {
            g = lbl_804BE104.data[i];
            break;
        }
    }
    return g;
}

void fn_802B11BC(GMat* g) {
    lbl_8055F99C = 0;
    lbl_8055F998 = 0;
    g->verts.data = (GVec*)fn_802CFFA0(0x24000);
    g->verts.cap = 0x3000;
    g->verts.count = 0;
    fn_802BAFA0();
    fn_802B0904();
    for (int i = 0; i < 65; i++) {
        g->lists[i] = GList();
    }
    bool merged = false;
    g->pending = 0;
    g->strips.count = 0;
    if (g->worldmap == -2 || fn_802B05E4() || fn_802B05EC()) {
        merged = true;
    }
    g->merged = merged;
}

u16 fn_802B127C(GMat* g, const GVec* v) {
    if (!g) {
        return 0;
    }
    if (g->verts.count + 1 <= g->verts.cap) {
        g->verts.data[g->verts.count++] = *v;
    }
    return g->verts.count - 1;
}

bool fn_802B12E0(GMat* g, GBatch* out, GBatch* in, int layer) {
    if (!g) {
        return false;
    }
    int key;
    if (layer <= 4 || g->merged) {
        key = in->flags & 63;
    } else {
        key = 0;
    }
    GAllocator* a = lbl_8055F9A0;
    s16* p = 0;
    GList* list = &g->lists[key];
    GBlock* head = list->head;
    if (!head || !head->Fits(24)) {
        GBlock* b = a->Get();
        if (b) {
            b->next = list->head;
            list->head = b;
            p = b->Take(24);
        }
    } else {
        p = head->Take(24);
    }
    if (p) {
        out->idx = (u16*)p;
        out->count = 24;
    }
    bool ok = p != 0;
    if (ok) {
        g->pending = list;
    }
    return ok;
}

void fn_802B13F8(GMat* g, void* ctx, GBatch* b, int layer, f32 alpha) {
    if (!g) {
        return;
    }
    u16* idx = b->idx;
    g->pending->head->count -= 24 - b->used;
    g->pending = 0;
    bool fade = false;
    if (layer == 4 && alpha < 1.0f) {
        fade = true;
    }
    if (fade && g->strips.count + 1 <= g->strips.cap) {
        GStrip* s = &g->strips.data[g->strips.count++];
        s->alpha = 1.0f - alpha;
        s->idx = idx;
    }
    fn_802BAFF8(ctx, &idx);
}

void fn_802B1B60(GMat* g);
void fn_802B1758(GMat* g);
void fn_802B1BEC(void);

void fn_802B14B4(GMat* g) {
    fn_802B1BEC();
    if (g->verts.Count() > 0) {
        fn_802B1B60(g);
        s32 old = fn_802C2C00(0);
        if (lbl_8055FA59) {
            fn_802C8BD0(g->worldmap);
        } else {
            fn_802C8BD0(fn_802C28C0());
        }
        fn_802B05EC();
        fn_802DBF00();
        for (int i = 0; i < g->layers.Count(); i++) {
            InlineVec<GTex, 6>* texs = &g->layers.At(i)->texs;
            if (lbl_8055FA59) {
                fn_802CADD0(g->layers.At(i)->detail, 1);
                fn_802CADD0(fn_802BC30C(i), 2);
            } else {
                for (int k = 0; k < texs->Count(); k++) {
                    fn_802CADD0(texs->At(k)->tex, k + 1);
                }
                fn_802CADD0(fn_802BC30C(i), 3);
            }
            GVec* verts = g->verts.At(0);
            int n = g->verts.Count();
            int bit = 1 << i;
            GList* lp = &g->lists[bit];
            for (u32 m = bit; m < 65; lp++, m++) {
                if ((m & bit) && !lp->Empty()) {
                    bool first = ((bit - 1) & m) == 0;
                    for (GBlock* b = lp->Head(); b; b = b->next) {
                        int c = b->Count();
                        if (c > 0) {
                            fn_802DC078(verts, n, (u16*)b->At(0), c, first, i);
                        }
                    }
                }
            }
            if (lbl_8055FA59) {
                fn_802CADD0(-1, 1);
                fn_802CADD0(-1, 2);
            } else {
                for (int k = 0; k < texs->Count(); k++) {
                    fn_802CADD0(-1, k + 1);
                }
                fn_802CADD0(-1, 3);
            }
        }
        fn_802DBF30();
        fn_802C8D74();
        fn_802C2C00(old);
        int n = g->verts.Count();
        fn_802BB0FC(g->verts.At(0), n);
        fn_802B1758(g);
        n = g->verts.Count();
        fn_802BB3B8(g->verts.At(0), n);
    }
}

void fn_802B1758(GMat* g) {
    if (g->lists[0].head && g->worldmap != -2) {
        s32 tex = g->worldmap;
        f32 m[3];
        m[0] = 1.0f / (1024.0f * kWorldScale);
        m[1] = 0.0f;
        m[2] = 1.0f;
        fn_802DA704(1, m, 1);
        fn_802DAA18();
        fn_802C8BD0(fn_802C28C0());
        fn_802DC3C0(tex);
        GVec* verts = g->verts.data;
        int n = g->verts.count;
        for (GBlock* b = g->lists[0].head; b; b = b->next) {
            fn_802DC464(verts, n, (u16*)b->data, b->count);
        }
        for (int k = 0; k < g->strips.count; k++) {
            GStrip* s = &g->strips.data[k];
            m[2] = s->alpha;
            fn_802DA704(1, m, 1);
            fn_802DAA18();
            fn_802DC464(verts, n, s->idx, s->idx[0] + 1);
        }
        fn_802DC440();
        fn_802C8D74();
    }
    if (g->crystal && g->oceanmap != -2) {
        s32 tex = g->oceanmap;
        f32 scale;
        if (tex == g->worldmap) {
            scale = 1.0f / (1024.0f * kWorldScale);
        } else {
            scale = 8.0f / (1024.0f * kWorldScale);
        }
        f32 m[3];
        m[0] = scale;
        m[1] = 0.0f;
        m[2] = 1.0f;
        fn_802DA704(1, m, 1);
        fn_802DAA18();
        fn_802C8BD0(fn_802C28C0());
        fn_802DC3C0(tex);
        f32 near = -2.0f + 1024.0f * kWorldScale * 0.5f;
        GVec* c = lbl_804BE190;
        for (int k = 0; k < 4; k++, c++) {
            f32 x = near * (f32)((k & 1) ? -1 : 1);
            f32 z = near * (f32)((k & 2) ? -1 : 1);
            f32 y = fn_801DCF48(0, x, z);
            c->x = x;
            c->y = y;
            c->z = z;
        }
        f32 far = 2048.0f * kWorldScale;
        c = &lbl_804BE190[4];
        for (int k = 4; k < 8; k++, c++) {
            f32 x = (f32)((k & 1) ? -1 : 1) * far;
            f32 z = (f32)((k & 2) ? -1 : 1) * far;
            f32 y = fn_801DCF48(0, x, z);
            c->x = x;
            c->y = y;
            c->z = z;
        }
        u16 idx[14] = {6, 0, 2, 6, 4, 5, 1, 6, 3, 1, 5, 7, 6, 2};
        fn_802DC464(lbl_804BE190, 8, idx, 14);
        fn_802DC440();
        fn_802C8D74();
    }
    g->strips.count = 0;
}

void fn_802B1B60(GMat* g) {
    f32 m[6][3];
    GLayer* l = g->layers.data;
    for (int k = 0; k < l->texs.count; k++) {
        m[k][0] = l->texs.data[k].u;
        m[k][1] = l->texs.data[k].v;
        m[k][2] = 1.0f / 2048.0f;
    }
    fn_802DA704(1, &m[0][0], 2);
    fn_802DAA18();
}

void fn_802B1BEC(void) {}

void fn_802B1BF0(GMat* g, GVec* out, int i) { *out = g->verts.data[i]; }

int fn_802B1C14(GMat* g) { return g->layers.count; }

const char* fn_802B1C1C(GMat* g, int i) {
    if (i >= 0 && i < g->layers.count) {
        return g->layers.data[i].name;
    }
    return "<unknown>";
}

s32 fn_802B1C50(GMat* g, int i, int offroad) {
    if (i >= 0 && i < g->layers.count) {
        return g->layers.data[i].surface[offroad];
    }
    return 0;
}

}  // extern "C"

template <class T>
inline T* IdxBlock<T>::At(int i) {
    return &data[i];
}

template <class T>
inline int IdxBlock<T>::Count(void) {
    return count;
}

template <class T, int N>
inline int InlineVec<T, N>::Count(void) {
    return count;
}

template <class T, int N>
inline T* InlineVec<T, N>::At(int i) {
    return &data[i];
}

inline GVec* GVerts::At(int i) { return &data[i]; }

inline int GVerts::Count(void) { return count; }

inline bool GList::Empty(void) { return head == 0; }

inline GBlock* GList::Head(void) { return head; }
