// Arena bounds (0x8013990C-0x8013A1E8): polygon bounds read from "boundN" entries of value tables
// ("x, y" pairs), point-in-polygon tests against one bound or a whole set, and the per-event external
// table lookup. Names are guesses apart from the message prefixes.
#include "game/common.h"
#include "game/carbody.h"
#include "game/v2.h"
#include "game/gamedb.h"

extern "C" {
int sprintf(char* s, const char* fmt, ...);
int sscanf(const char* s, const char* fmt, ...);
char* strcpy(char* d, const char* s);
char* strncpy(char* d, const char* s, size_t n);
char* strchr(const char* s, int c);
char* strrchr(const char* s, int c);
void fn_802E30C8(const char* fmt, ...);  // debug print
void* fn_801284E8(const char* tag, s32 size);  // MemAlloc
void fn_80128658(void* p);                     // MemFree
void fn_801286A0(void* p, s32 shrink);         // MemShrink
ValueTable* fn_80131810(const char* name);     // load value file, quiet
void fn_80131818(ValueTable* t);
}  // extern "C"
const char* fn_8000AC2C(void);
extern "C" {
}

const char* GetEventTypeName(u32 type);
void* operator new(size_t size, const char* tag);

template <typename T, int N>
class FixedArray {
public:
    FixedArray(void) {
        data = storage;
        max = N;
        count = 0;
    }
    ~FixedArray(void) {
        data = 0;
        count = 0;
        max = 0;
    }
    T* data;     // offset 0x0
    s32 count;   // offset 0x4
    s32 max;     // offset 0x8
    T storage[N];
};

#include "game/bound.h"


extern "C" Bound* ParseBoundPoints(ValueNode* n);

template <typename T, int N>
struct ArrayWrap {
    FixedArray<T, N> a;
};

struct BoundSet {
    BoundSet(ValueTable* t) {
        int i;
        ValueNode* n;
        table = t;
        for (i = 0; (n = t->GetChild(i)) != 0; i++) {
            bounds.a.data[bounds.a.count++] = ParseBoundPoints(n);
        }
    }
    ~BoundSet(void) {
        int i;
        for (i = 0; i < bounds.a.count; i++) {
            fn_80128658(bounds.a.data[i]);
        }
        bounds.a.count = 0;
        if (table) {
            fn_80131818(table);
        }
        table = 0;
    }
    ValueTable* table;                // offset 0x0
    ArrayWrap<Bound*, 32> bounds;     // offset 0x4
};

#include "game/tableref.h"
#include "game/mathutil.h"

ModuleTag lbl_8055E130;
ValueTable* lbl_8055E138;  // external bound table
ValueNode* lbl_8055E13C;   // event values
char lbl_8042F7C0[0x80];   // external table path

static inline s32 BoundBytes(s32 n) { return n * sizeof(BoundPt) + 8; }

// Shared with boostman.cpp: retail's weak NewBound__Fl uses this unit's "arenabnd" string object.
inline Bound* NewBound(s32 n) {
    Bound* b = (Bound*)fn_801284E8("arenabnd", n * sizeof(BoundPt) + 8);
    b->cap = n;
    b->n = n;
    return b;
}

static inline void Perp(V2* v) {
    const f32 x = v->y;
    v->y = -v->x;
    v->x = x;
}


extern "C" {

int ValidateArenaBoundPoints(ValueNode* n) {
    char buf[0x10];
    int i = 0;
    const char* v;
    do {
        sprintf(buf, "bound%d", i++);
        v = n->GetString(buf, 0);
    } while (v && i < 10000);
    if (i > 0x400) {
        fn_802E30C8("WARNING: ARENA_BOUND_MAX_POINTS is %d, table %s has %d points!", 0x400, n->GetName(), i);
    }
    if (v) {
        fn_802E30C8("WARNING: More than 10000 points in bound table %s!!!! Are you insane?!?!?!", n->GetName());
    }
    return i;
}

Bound* ParseBoundPoints(ValueNode* n) {
    char buf[0x10];
    s32 cnt = ValidateArenaBoundPoints(n);
    Bound* b = NewBound(cnt);
    BoundPt* p;
    int i;
    const char* v;
    p = b->pts;
    i = 0;
    do {
        sprintf(buf, "bound%d", i);
        v = n->GetString(buf, 0);
        if (v) {
            if (sscanf(v, "%f, %f", &p->x, &p->y) == 2) {
                p++;
                i++;
            } else {
                fn_802E30C8("arenabnd: table \"%s\": bad scan on \"%s\"", n->GetName(), v);
                break;
            }
        }
    } while (v);
    if (i < 3) {
        fn_802E30C8("arenabnd: table \"%s\": emptying bound, only had %d verts", n->GetName(), b->n);
        i = 0;
    }
    fn_801286A0(b, BoundBytes(b->n) - BoundBytes(i));
    b->cap = i;
    b->n = i;
    return b;
}

int IsPointInBound(const Bound* b, f32 x, f32 y);

int fn_80139B5C(u8* car, Bound* b) { return IsPointInBound(b, ((CarBody*)car)->mtx.pos.x, ((CarBody*)car)->mtx.pos.z); }

int IsPointInBound(const Bound* b, f32 x, f32 y) {
    const BoundPt* p = b->pts;
    int n = b->n;
    int c = 0;
    int i;
    for (i = 0; i < n; p++, i++) {
        const BoundPt* q = &b->pts[(i + 1) % n];
        bool up = p->y < q->y;
        if (up) {
            if (y < p->y || y > q->y) {
                continue;
            }
        } else {
            if (y < q->y || y > p->y) {
                continue;
            }
        }
        {
            V2 e;
            e.x = q->x - p->x;
            e.y = q->y - p->y;
            Perp(&e);
            V2C a(x - p->x, y - p->y);
            const f32 side = e.y;
            if ((e.x * a.x + side * a.y < 0.0f) == up) {
                c++;
            }
        }
    }
    return c & 1;
}

/* Helper name unproven */
static inline f32 MidF(f32 a, f32 b) { return a + 0.5f * (b - a); }

V2 CalculateBoundCenter(const Bound* b) {
    V2C lo;
    V2C hi;
    const BoundPt* p = &b->pts[1];
    int i;
    hi.x = b->pts[0].x;
    hi.y = b->pts[0].y;
    lo.x = b->pts[0].x;
    lo.y = b->pts[0].y;
    int limit = b->n;
    for (i = 1; i < limit; i++, p++) {
        lo.x = MinF(p->x, lo.x);
        lo.y = MinF(p->y, lo.y);
        hi.x = MaxF(p->x, hi.x);
        hi.y = MaxF(p->y, hi.y);
    }
    return V2C(MidF(lo.x, hi.x), MidF(lo.y, hi.y));
}

BoundSet* LoadBoundSet(const char* name) {
    BoundSet* s = 0;
    ValueTable* t = fn_80131810(name);
    if (t) {
        s = new ("bound_set") BoundSet(t);
    }
    return s;
}

void DeleteBoundSet(BoundSet* s) { delete s; }

static inline int AnyBoundHit(BoundSet* s, f32 x, f32 y) {
    int i;
    for (i = 0; i < s->bounds.a.count; i++) {
        if (IsPointInBound(s->bounds.a.data[i], x, y)) {
            return 1;
        }
    }
    return 0;
}

int fn_80139EC4(BoundSet* s, f32 x, f32 y) {
    int i;
    return AnyBoundHit(s, x, y);
}

void LoadEventTable(EventInfo* ev) {
    char buf[0x40];
    char path[0x80];
    const char* name;
    bool full;
    sprintf(buf, "c_%s", GetEventTypeName(ev->type));
    name = ev->node->GetString("external_table", buf);
    full = strchr(name, '/') != 0;
    if (full) {
        strncpy(path, name, 0x80);
    } else {
        sprintf(path, "*%s/%s.va0", fn_8000AC2C(), name);
    }
    lbl_8055E138 = fn_80131810(path);
    if (!lbl_8055E138 && !full) {
        strcpy(strrchr(path, '.'), ".val");
        lbl_8055E138 = fn_80131810(path + 1);
    }
    strcpy(lbl_8042F7C0, path);
    lbl_8055E13C = ev->node;
}

void fn_8013A074(void) {
    if (lbl_8055E138) {
        fn_80131818(lbl_8055E138);
    }
    lbl_8055E13C = 0;
    lbl_8042F7C0[0] = 0;
}

}  // extern "C"

TableRef::TableRef(const char* what, const char* name) {
    node = 0;
    if (lbl_8055E138 && *name) {
        if (lbl_8055E138->HasChild(name)) {
            node = lbl_8055E138->FindChild(name);
        } else {
            fn_802E30C8("%s: no %s table in \"%s\"", what, name, lbl_8042F7C0);
        }
    }
    if (!node) {
        node = lbl_8055E13C->FindChild(name);
    }
}

TableRef::~TableRef(void) { node = 0; }
