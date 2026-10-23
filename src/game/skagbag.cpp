// Skag bags (0x802B3FC4-0x802B4FB0): grids of randomly scattered placement points from "skagbag.val".
// Each level of detail ("rad" 1, 2, 4, ...) is a 32x32 grid of cells; a cell points at a bag of points in a
// 2x2 square, split into four quadrants. Root bags are random; a child bag at radius r is built from the four
// parent bags around it, taking points quadrant by quadrant and rescaling them. Each point then looks up six
// ids for the caller. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
void* memset(void* dst, int c, size_t n);
void* memcpy(void* dst, const void* src, size_t n);
int strcasecmp(const char* a, const char* b);
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* t);
int fn_80129D90(int seed, int lo, int hi);    // random int
f32 fn_80129DC4(int seed, f32 lo, f32 hi);  // random float
s32 fn_802B3DA4(void* ctx, u16 id, int slot);
void fn_802E30C8(const char* fmt, ...);
}

void* operator new(size_t size, const char* name);
void* operator new[](size_t size, const char* name);
void operator delete(void* p);
void operator delete[](void* p);

namespace {

// A point in a bag.
struct SkagPoint {
    SkagPoint(void);
    f32 x;         // offset 0x0
    f32 y;         // offset 0x4
    u16 id;        // offset 0x8
    u16 quad;      // offset 0xA
    s32 slots[6];  // offset 0xC

    void Place(const f32* pt, f32 size);
    void Unplace(f32* out, f32 size) const;
};

template <class T>
struct SkagVec {
    SkagVec(int n) {
        data = new ("dsa_buf") T[n];
        cap = n;
        count = 0;
        buf = data;
    }
    ~SkagVec(void) {
        delete[] buf;
        if (this) {
            data = 0;
            count = 0;
            cap = 0;
        }
    }
    void Push(T v) { data[count++] = v; }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
    T* buf;     // offset 0xC
};

struct SkagTables {
    s32 dx[4];         // offset 0x0
    s32 dy[4];         // offset 0x10
    f32 axes[4][2];    // offset 0x20
    f32 scales[4][2];  // offset 0x40
};

static const SkagTables lbl_802FAAD0 = {
    {0, 1, 1, 0},
    {0, 0, 1, 1},
    {{1.0f, 1.0f}, {-1.0f, 1.0f}, {-1.0f, -1.0f}, {1.0f, -1.0f}},
    {{1.5f, 1.0f}, {1.0f, 1.5f}, {1.0f, 0.5f}, {0.5f, 1.0f}},
};

extern "C" u16 fn_802B4004(void);

static inline bool Less(int a, int b) { return a < b; }
static inline bool Retry(int& t, int n) { return t++ < n; }

static inline int Twice(int v) { return v * 2 - v; }

// Number of points in a root bag.
static inline int RootCount(ValueNode* node) {
    int lo = node->GetIntValue("min_elems");
    lo = (lo * 2 - lo) / 10;
    int hi = node->GetIntValue("max_elems");
    hi = (hi * 2 - hi) / 10;
    return fn_80129D90(0, lo, hi + 1);
}

struct SkagBag : SkagVec<SkagPoint> {
    // A root bag: random points in a 2x2 square, kept apart.
    SkagBag(ValueNode* node)
        : SkagVec<SkagPoint>(RootCount(node)) {
        int j = 0;
        if (j < cap)
        for (; j < cap; j++) {
            SkagPoint* p = &data[count++];
            int tries = 0;
            while (Retry(tries, 500)) {
                p->x = fn_80129DC4(0, 0.0f, 2.0f);
                p->y = fn_80129DC4(0, 0.0f, 2.0f);
                bool far = false;
                for (int k = 0; !far && k < count - 1; k++) {
                    f32 d[2];
                    d[1] = data[j].y - p->y;
                    d[0] = data[j].x - p->x;
                    far = d[0] * d[0] + d[1] * d[1] > 0.25f;
                }
                if (!far) {
                    break;
                }
            }
            p->id = fn_802B4004();
            memset(p->slots, 0, sizeof(p->slots));
        }
        f32 size = 2.0f;
        for (int k = 0; k < count; k++) {
            data[k].Place(&data[k].x, size);
        }
    }
    SkagBag(SkagBag** src, ValueNode* node, int rad, s32** used);
};

struct SkagCell {
    SkagBag* bag;  // offset 0x0
    s32 used;      // offset 0x4
};

template <class T>
void SafeDelete(T*& p) {
    if (p) {
        delete p;
        p = 0;
    }
}

extern "C" SkagBag* fn_802B475C(const SkagVec<SkagBag*>* v);

struct SkagGrid {
    // The root grid: a random root bag in every cell.
    SkagGrid(SkagVec<SkagBag*>* bags) {
        for (int x = 0; x < 32; x++) {
            for (int y = 0; y < 32; y++) {
                SkagCell* c = &cells[y & 31][x & 31];
                c->bag = fn_802B475C(bags);
                c->used = 0;
            }
        }
        owns = 0;
    }
    SkagGrid(ValueNode* node, int rad);
    ~SkagGrid(void) {
        if (owns) {
            SkagCell* c = &cells[0][0];
            for (u32 k = 0; k < 1024; k++, c++) {
                SafeDelete(c->bag);
            }
        }
    }
    SkagCell cells[32][32];  // offset 0x0
    u8 owns;                 // offset 0x2000
    inline SkagCell* At(s16 x, s16 y);
};

}  // namespace

ModuleTag lbl_8055F9C0;
s32 lbl_8055F9C8;                 // only grids built for this owner (-1: any)
u16 lbl_8055F9CC;                 // next point id
SkagVec<SkagBag*>* lbl_8055F9D0;  // root bags
SkagGrid* lbl_804BE248[128];      // by radius

// Moves a point of a size x size square into the frame of its quadrant.
inline void SkagPoint::Place(const f32* pt, f32 size) {
    f32 s[2];
    f32 c = size / 2;
    f32 dx = pt[0] - c;
    f32 dy = pt[1] - c;
    u16 q = 0;
    bool a = dx + dy > 0.0f;
    bool b = -dx + dy > 0.0f;
    if (a) {
        q |= 1;
    }
    if (b) {
        q |= 2;
    }
    u16 map[4] = {2, 3, 1, 0};
    quad = map[q];
    s[1] = (pt[1] - c) / size;
    s[0] = (pt[0] - c) / size;
    const f32* u = lbl_802FAAD0.axes[quad];
    const f32* v = lbl_802FAAD0.axes[(quad + 1) & 3];
    x = u[0] * s[0] + u[1] * s[1];
    y = v[0] * s[0] + v[1] * s[1];
}

// Inverse of Place: back into a size x size square.
inline void SkagPoint::Unplace(f32* out, f32 size) const {
    f32 c = size * 0.5f;
    f32 sx = x * size;
    f32 sy = y * size;
    const f32* u = lbl_802FAAD0.axes[quad];
    const f32* v = lbl_802FAAD0.axes[(quad + 1) & 3];
    out[0] = c + u[0] * (sx / 2) + v[0] * (sy / 2);
    out[1] = c + u[1] * (sx / 2) + v[1] * (sy / 2);
}

// A child grid: one bag per rad x rad block, built from the four parent cells around it.
inline SkagGrid::SkagGrid(ValueNode* node, int rad) {
    SkagGrid* parent = lbl_804BE248[rad / 2];
    memset(this, 0, sizeof(cells));
    for (int x = 0; x < 32; x += rad) {
        for (int y = 0; y < 32; y += rad) {
            SkagBag* bags[4];
            s32* used[4];
            for (int k = 0; k < 4; k++) {
                SkagCell* pc = parent->At(x + rad * lbl_802FAAD0.dx[k] / 2, y + rad * lbl_802FAAD0.dy[k] / 2);
                bags[k] = pc->bag;
                used[k] = &pc->used;
            }
            SkagBag* bag = new ("child-SB") SkagBag(bags, node, rad, used);
            At(x, y)->bag = bag;
            At(x, y)->used = 0;
        }
    }
    owns = 1;
}

extern "C" {

bool fn_802B3FC4(ValueNode* n) { return strcasecmp("skagbag", n->GetName()) == 0; }

u16 fn_802B4004(void) { return lbl_8055F9CC++; }

ValueNode* fn_802B4014(ValueNode* root) {
    ValueNode* n;
    for (int i = 0; (n = root->GetChildAt(i)) != 0; i++) {
        if (fn_802B3FC4(n) && n->GetIntValue("rad") == 1) {
            break;
        }
    }
    return n;
}

SkagGrid* fn_802B40B0(int rad, int owner) {
    if (rad >= 0 && (u32)rad < 128) {
        SkagGrid* g = lbl_804BE248[rad];
        if (g) {
            if (lbl_8055F9C8 == -1 || owner == lbl_8055F9C8) {
                return g;
            }
        }
    }
    return 0;
}

// Only its strings survive in the game.
void SkagDebug(SkagPoint* p, int q) {
    fn_802E30C8("%.2f,%.2f", p->x, p->y);
    fn_802E30C8("quadrant %d", q);
    fn_802E30C8("to-quadpoint %.2f,%.2f", p->x, p->y);
    fn_802E30C8("back-to-skagpoint %.2f,%.2f", p->x, p->y);
}

void fn_802B40F8(void) {
    ValueTable* t = fn_80131810("skagbag.val");
    ValueNode* root = t->FindChild("skagbag_matrix");
    if (root) {
        int n = root->GetIntValue("root_skagbags");
        lbl_8055F9D0 = new ("skagbag-table") SkagVec<SkagBag*>(n);
        ValueNode* first = fn_802B4014(root);
        for (int i = 0; i < lbl_8055F9D0->cap; i++) {
            SkagBag* bag = new ("root-SkagBag") SkagBag(first);
            lbl_8055F9D0->data[lbl_8055F9D0->count++] = bag;
        }
        lbl_804BE248[1] = new ("Root-SBDM") SkagGrid(lbl_8055F9D0);
        ValueNode* c;
        for (int i = 0; (c = root->GetChildAt(i)) != 0; i++) {
            if (fn_802B3FC4(c)) {
                int rad = c->GetIntValue("rad");
                if (rad >= 0 && (u32)rad < 128 && rad != 1) {
                    lbl_804BE248[rad] = new ("Child-SBDM") SkagGrid(c, rad);
                }
            }
        }
    }
    fn_80131818(t);
    lbl_8055F9C8 = -1;
}

SkagBag* fn_802B475C(const SkagVec<SkagBag*>* v) { return v->data[fn_80129D90(0, 0, v->count)]; }

}  // extern "C"

static inline int ChildCount(ValueNode* node, int rad) {
    int lo = node->GetIntValue("min_elems");
    lo = rad * (rad * lo) / 10;
    int hi = node->GetIntValue("max_elems");
    hi = rad * (rad * hi) / 10;
    return fn_80129D90(0, lo, hi + 1);
}

SkagBag::SkagBag(SkagBag** src, ValueNode* node, int rad, s32** used)
    : SkagVec<SkagPoint>(ChildCount(node, rad)) {
    f32 half = (rad / 2) * 2.0f;
    f32 size = rad * 2.0f;
    s32 taken[4] = {0, 0, 0, 0};
    int q = fn_80129D90(0, 0, 4);
    int i = 0;
    if (i < cap)
    for (; i < cap; i++) {
        int t = 0;
        do {
            q = (q + 1) & 3;
        } while (Retry(t, 100) && taken[q] >= src[q]->count);
        SkagPoint* p = &data[count++];
        SkagPoint* s = &src[q]->data[taken[q]];
        p->id = s->id;
        memcpy(p->slots, s->slots, sizeof(p->slots));
        f32 at[2];
        f32 pt[2];
        s->Unplace(pt, half);
        at[0] = pt[0] + half * lbl_802FAAD0.dx[q];
        at[1] = pt[1] + half * lbl_802FAAD0.dy[q];
        p->Place(at, size);
        taken[q]++;
    }
    for (int k = 0; k < 4; k++) {
        *used[k] = taken[k];
    }
}

inline SkagCell* SkagGrid::At(s16 x, s16 y) { return &cells[0][(x & 31) + ((y & 31) << 5)]; }

SkagPoint::SkagPoint(void) {}

extern "C" {

void fn_802B4BDC(void) {
    SkagGrid** g = lbl_804BE248;
    for (u32 i = 0; i < 128; i++, g++) {
        SafeDelete(*g);
    }
    for (int i = 0; i < lbl_8055F9D0->count; i++) {
        delete lbl_8055F9D0->data[i];
    }
    SafeDelete(lbl_8055F9D0);
}

void fn_802B4E30(void) { lbl_8055F9C8 = -1; }

void fn_802B4E3C(s32 owner) { lbl_8055F9C8 = owner; }

static inline void ResolveBag(SkagBag* bag, void* ctx) {
    for (int i = 0; i < bag->count; i++) {
        SkagPoint* p = &bag->data[i];
        for (int s = 0; s < 6; s++) {
            p->slots[s] = fn_802B3DA4(ctx, p->id, s);
        }
    }
}

static inline void ResolveGrid(SkagGrid* g, void* ctx) {
    SkagCell* c = g->cells[0];
    for (u32 k = 0; k < 1024; k++, c++) {
        SkagBag* bag = c->bag;
        if (bag) {
            ResolveBag(bag, ctx);
        }
    }
}

void fn_802B4E44(void* ctx) {
    SkagGrid** g = lbl_804BE248;
    for (u32 r = 0; r < 128; r++, g++) {
        SkagGrid* grid = *g;
        if (grid) {
            ResolveGrid(grid, ctx);
        }
    }
}

SkagBag* fn_802B4F10(s32* used, int x, int y, int rad, int owner) {
    SkagBag* bag = 0;
    SkagGrid* g = fn_802B40B0(rad, owner);
    if (g) {
        SkagCell* c = g->At(x, y);
        *used = c->used;
        bag = c->bag;
        bag = bag->count > 0 ? bag : 0;
    }
    return bag;
}

}  // extern "C"
