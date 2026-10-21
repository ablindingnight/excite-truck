// Boost areas (0x801E0710-0x801E1874): the "BoostMan" race listener. It reads the event's booster
// file ("boosters.val"), turns each three-point ArenaBound into a quad laid along the ground (a grid of
// ground-snapped vertices), tells whether a car's wheels are on one, and draws the grids with the
// line/arrow textures in a per-player colour (yellow while boosting). Names are guesses apart from
// the strings.
#include "game/common.h"

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

extern "C" {
double sqrt(double x);
void fn_802E30C8(const char* fmt, ...);  // debug print
void* fn_801284E8(const char* tag, s32 size);  // MemAlloc
void fn_80128658(void* p);                     // MemFree
ValueTable* fn_80131810(const char* name);     // load value file, quiet
void fn_80131818(ValueTable* t);
}

void* GetEvent(int event);
void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

struct V2 {
    f32 x;
    f32 y;
};

class Vec2 : public V2 {
public:
    Vec2(void);
    Vec2(const V2& v) { *this = (const Vec2&)v; }
    Vec2& operator=(const Vec2& v);
};

struct V3 {
    f32 x;
    f32 y;
    f32 z;
};

struct Mat3 {
    f32 m[3][3];
};

struct Bound {
    s32 n;      // offset 0x0
    s32 cap;    // offset 0x4
    V2 pts[1];  // offset 0x8
};

// One grid vertex of a boost area.
struct BoostVtx {
    void Emit(u32 c, int idx);
    V3 pos;     // offset 0x0
    u32 color;  // offset 0xC
    f32 u;      // offset 0x10
    f32 v;      // offset 0x14
};

struct BoostArea {
    BoostArea(void) {
        bound = 0;
        verts = 0;
    }
    ~BoostArea(void) { Free(); }
    void Build(Bound* b);
    BoostVtx* Vtx(int x, int y) { return &verts[x * ny + y]; }
    void Free(void) {
        if (bound) {
            fn_80128658(bound);
            bound = 0;
            delete[] verts;
            verts = 0;
        }
    }
    Bound* bound;     // offset 0x0
    BoostVtx* verts;  // offset 0x4
    s32 nx;           // offset 0x8
    s32 ny;           // offset 0xC
    V3 center;        // offset 0x10
};

template <typename T>
struct ArrayRef {
    ArrayRef(T* d, int n) {
        data = d;
        cap = n;
        count = 0;
    }
    ~ArrayRef(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    int Count(void);
    T* At(int i);
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
};

template <typename T, int N>
struct FixedArr : ArrayRef<T> {
    FixedArr(void);
    T storage[N];
};

struct PlayerBoost {
    void Reset(void);
    void Clear(void) {
        flag = 0;
        color0 = 0;
        color1 = 0;
        last = -1.0f;
        timer = 0.0f;
    }
    s32 car;     // offset 0x0
    u8 flag;     // offset 0x4
    f32 last;    // offset 0x8
    f32 timer;   // offset 0xC
    u32 color0;  // offset 0x10: arrows
    u32 color1;  // offset 0x14: lines
};

class IRaceListener {
public:
    IRaceListener(void);
    virtual void a0(void) = 0;
    virtual void a1(void) = 0;
    virtual void a2(void) = 0;
    virtual void a3(void) = 0;
    virtual void a4(void) = 0;
};

namespace {
class BoostMan;
}

extern "C" {
void fn_801777CC(IRaceListener* l);  // add race listener
void fn_801777D0(IRaceListener* l);  // remove race listener
s32 fn_802CAC28(const char* name, int flags);  // load texture
void fn_802CAC54(s32 tex);
void fn_802C8BD0(s32 tex);
int fn_802C8DBC(int n);
void fn_802C8E00(u32 color, int idx, f32 x, f32 y, f32 z, f32 u, f32 v);
void fn_802C8E48(void);
void fn_802C8D74(void);
void fn_80160B14(void* p);
void fn_80160B18(V2* v);
void fn_80160B7C(V2* r, const V2* a, const V2* b);
void fn_80160BA0(V2* r, const V2* a, f32 s);
void fn_80160BBC(V2* r, const V2* a, const V2* b);
f32 fn_80160BE0(const V2* a, const V2* b);
void fn_80160230(V3* d, const V2* xz);
Bound* fn_801399FC(ValueNode* n);
int fn_80139B70(Bound* b, f32 x, f32 y);
V2 fn_80139C5C(Bound* b);
f32 fn_801DD88C(f32 x, f32 z);  // ground height
void fn_801EA1E0(f32 x, f32 z, f32* y, int a);
u8* fn_8017960C(int car);
int fn_801E85A4(void);       // player views
int fn_801E85B4(void);       // current view
int fn_801E866C(int view);  // car of a view
void fn_801E0890(BoostMan* m);
void fn_801E1384(BoostArea* a, u32 color);
}

V2 LerpV2(const V2& a, const V2& b, f32 t);

struct LocalCarDesc {
    s32 event;  // offset 0x0
    u32 flags;  // offset 0x4
};

extern "C" LocalCarDesc* fn_801769CC(void);

struct EventInfo {
    u8 pad0[0x20];
    ValueNode* values;  // offset 0x20
};

namespace {
class BoostMan : public IRaceListener {
public:
    BoostMan(void) {
        int i;
        lines = fn_802CAC28("bstrlines.tex", 0);
        arrows = fn_802CAC28("bstrarrows.tex", 0);
        fn_801E0890(this);
        for (i = 0; i < 4; i++) {
            cars[i] = -1;
            players[i].Reset();
        }
        fn_801777CC(this);
    }
    void Init(int i) {
        cars[i] = -1;
        players[i].Reset();
    }
    virtual void a0(void);
    virtual void a1(void);
    virtual void a2(void);
    virtual void a3(void);
    virtual void a4(void);
    virtual ~BoostMan(void);
    void Clear(void) {
        for (int i = 0; i < areas.count; i++) {
            areas.data[i].Free();
        }
        areas.count = 0;
    }

    s32 lines;                          // offset 0x4: texture
    s32 arrows;                         // offset 0x8: texture
    PlayerBoost players[4];             // offset 0xC
    s32 cars[4];                        // offset 0x6C
    FixedArr<BoostArea, 100> areas;  // offset 0x7C
};
}  // namespace

static inline void Vertex(u32 c, int idx, const V3& p, f32 u, f32 v) { fn_802C8E00(c, idx, p.x, p.y, p.z, u, v); }

inline void BoostVtx::Emit(u32 c, int idx) {
    color = c;
    Vertex(color, idx, pos, u, v);
}

ModuleTag lbl_8055EA08;
BoostMan* lbl_8055EA10;

inline f32 Dist(const V2* a, const V2* b);
inline int Max(int a, int b);
inline void AddScaled(V2* r, const V2* a, f32 s, const V2* b);
inline f32 Dot(const V2* a, const V2* b);
inline Bound* NewBound(s32 n);

static inline void MulVec(V3* out, const V3* in, const Mat3* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m->m[0][0], in->y, m->m[1][0], in->z, m->m[2][0]);
        f32 y = Dot3(in->x, m->m[0][1], in->y, m->m[1][1], in->z, m->m[2][1]);
        f32 z = Dot3(in->x, m->m[0][2], in->y, m->m[1][2], in->z, m->m[2][2]);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m->m[0][0], in->y, m->m[1][0], in->z, m->m[2][0]);
        out->y = Dot3(in->x, m->m[0][1], in->y, m->m[1][1], in->z, m->m[2][1]);
        out->z = Dot3(in->x, m->m[0][2], in->y, m->m[1][2], in->z, m->m[2][2]);
    }
}

struct Wheel {
    u8 pad0[0x30];
    V3 pos;  // offset 0x30
    u8 pad3C[0x38];
};

struct CarRec {
    u8 pad0[0x3C];
    Mat3 rot;   // offset 0x3C
    V3 pos;     // offset 0x60
    u8 pad6C[0x2340];
    Wheel wheels[4];  // offset 0x23AC
};

static inline void SetVtx(BoostVtx* v, const V2* p) {
    fn_80160230(&v->pos, p);
    v->pos.y = fn_801DD88C(v->pos.x, v->pos.z);
    fn_801EA1E0(v->pos.x, v->pos.z, &v->pos.y, 0);
}


extern "C" {

void fn_801E0710(void) {
    if (fn_801769CC()->flags & 0x200000) {
        lbl_8055EA10 = new ("BoostMan") BoostMan();
    }
}

}  // extern "C"

inline void PlayerBoost::Reset(void) {
    car = -1;
    Clear();
}

inline IRaceListener::IRaceListener(void) {}

template <typename T, int N>
inline FixedArr<T, N>::FixedArr(void) : ArrayRef<T>(storage, N) {}

static inline void BuildArea(BoostArea* a, Bound* b) {
    int best = -1;
    int k;
    for (k = 0; k < b->n; k++) {
        if (best < 0 || fn_80160BE0(&b->pts[best], &b->pts[best % b->n]) > fn_80160BE0(&b->pts[k], &b->pts[k % b->n])) {
            best = k;
        }
    }
    Vec2 tip(b->pts[(best + 2) % b->n]);
    Vec2 mid;
    fn_80160BBC(&mid, &b->pts[best], &b->pts[(best + 1) % b->n]);
    fn_80160BA0(&mid, &mid, 0.5f);
    Vec2 dir;
    fn_80160B7C(&dir, &tip, &mid);
    fn_80160B18(&dir);
    Vec2 side;
    fn_80160B7C(&side, &b->pts[best], &mid);
    AddScaled(&side, &side, -Dot(&dir, &side), &dir);
    a->bound = NewBound(4);
    fn_80160BBC(&a->bound->pts[0], &mid, &side);
    AddScaled(&a->bound->pts[1], &mid, -1.0f, &side);
    AddScaled(&a->bound->pts[2], &tip, -1.0f, &side);
    fn_80160BBC(&a->bound->pts[3], &tip, &side);
    a->bound->n = 4;
    a->ny = Max(2, (int)((2.5f + Dist(&a->bound->pts[0], &a->bound->pts[1])) / 5.0f));
    a->nx = Max(2, (int)((2.5f + Dist(&a->bound->pts[1], &a->bound->pts[2])) / 5.0f));
    a->verts = new ("BoostArea_v") BoostVtx[a->ny * a->nx];
    int x;
    for (x = 0; x < a->nx; x++) {
        f32 t = (f32)x / (f32)(a->nx - 1);
        Vec2 l(LerpV2(a->bound->pts[1], a->bound->pts[2], t));
        Vec2 r(LerpV2(a->bound->pts[0], a->bound->pts[3], t));
        f32 v = 1.0f - t;
        int y;
        for (y = 0; y < a->ny; y++) {
            t = (f32)y / (f32)(a->ny - 1);
            Vec2 p(LerpV2(l, r, t));
            BoostVtx* vt = &a->verts[x * a->ny + y];
            SetVtx(vt, &p);
            vt->color = -1;
            vt->u = t;
            vt->v = v;
        }
    }
    fn_80160B18(&side);
    AddScaled(&a->bound->pts[0], &a->bound->pts[0], 1.0f, &side);
    AddScaled(&a->bound->pts[0], &a->bound->pts[0], -2.0f, &dir);
    AddScaled(&a->bound->pts[1], &a->bound->pts[1], -1.0f, &side);
    AddScaled(&a->bound->pts[1], &a->bound->pts[1], -2.0f, &dir);
    AddScaled(&a->bound->pts[2], &a->bound->pts[2], -1.0f, &side);
    AddScaled(&a->bound->pts[2], &a->bound->pts[2], 2.0f, &dir);
    AddScaled(&a->bound->pts[3], &a->bound->pts[3], 1.0f, &side);
    AddScaled(&a->bound->pts[3], &a->bound->pts[3], 2.0f, &dir);
    Vec2 c(fn_80139C5C(a->bound));
    fn_80160230(&a->center, &c);
    a->center.y = fn_801DD88C(a->center.x, a->center.z);
}

extern "C" {

void fn_801E0890(BoostMan* m) {
    BoostArea* a;
    Bound* b;
    ValueTable* t;
    int ok;
    int i;
    ValueNode* n;
    const char* name = ((EventInfo*)GetEvent(fn_801769CC()->event))->values->GetString("booster_file", "boosters.val");
    t = fn_80131810(name);
    if (t) {
        for (i = 0; (n = t->GetChild(i)) != 0; i++) {
            a = &m->areas.data[m->areas.count++];
            b = fn_801399FC(n);
            ok = 0;
            if (b->n == 3) {
                BuildArea(a, b);
                ok = 1;
            } else {
                fn_802E30C8("BoostArea: table %s is not an ArenaBound with three points!", n->GetName());
            }
            fn_80160B14(b);
            if (!ok) {
                m->areas.RemoveAt(m->areas.count - 1);
            }
        }
        fn_80131818(t);
    }
}

}  // extern "C"

inline f32 Dist(const V2* a, const V2* b) {
    V2 d;
    d.y = a->y - b->y;
    d.x = a->x - b->x;
    return sqrt(d.x * d.x + d.y * d.y);
}

inline int Max(int a, int b) { return b > a ? b : a; }

inline void AddScaled(V2* r, const V2* a, f32 s, const V2* b) {
    f32 x = a->x + b->x * s;
    f32 y = a->y + b->y * s;
    r->x = x;
    r->y = y;
}

inline f32 Dot(const V2* a, const V2* b) {
    f32 x;
    f32 y;
    y = b->y;
    x = b->x;
    return a->x * x + a->y * y;
}

inline Bound* NewBound(s32 n) {
    Bound* b = (Bound*)fn_801284E8("arenabnd", n * sizeof(V2) + 8);
    b->cap = n;
    b->n = n;
    return b;
}

extern "C" {

int fn_801E1048(int car) {
    BoostMan* m;
    int r;
    CarRec* rec;
    Wheel* ws;
    int i;
    int j;
    Wheel* w;
    int hit;
    m = lbl_8055EA10;
    r = m != 0;
    if (r) {
        rec = (CarRec*)fn_8017960C(car);
        ws = rec->wheels;
        for (i = 0; i < m->areas.count; i++) {
            w = ws;
            for (j = 0; j < 4; j++, w++) {
                V3 p = w->pos;
                MulVec(&p, &p, &rec->rot);
                p.x += rec->pos.x;
                p.y += rec->pos.y;
                p.z += rec->pos.z;
                hit = 0;
                if (m->areas.data[i].bound && fn_80139B70(m->areas.data[i].bound, p.x, p.z)) {
                    hit = 1;
                }
                if (hit) {
                    r = 1;
                    goto done;
                }
            }
        }
        r = 0;
    }
done:
    return r;
}

void fn_801E1250(int car, u8 on) {
    BoostMan* m = lbl_8055EA10;
    if (m) {
        int i;
        for (i = 0; i < 4; i++) {
            if (car == m->players[i].car) {
                m->players[i].flag = on;
            }
        }
    }
}

void fn_801E12A4(void) {
    BoostMan* m = lbl_8055EA10;
    if (m) {
        int view = fn_801E85B4();
        m->cars[view] = fn_801E866C(view);
        fn_802C8BD0(m->lines);
        PlayerBoost* p = &m->players[view];
        for (int i = 0; i < m->areas.Count(); i++) {
            fn_801E1384(m->areas.At(i), p->color1);
        }
        fn_802C8D74();
        fn_802C8BD0(m->arrows);
        for (int i = 0; i < m->areas.Count(); i++) {
            fn_801E1384(m->areas.At(i), p->color0);
        }
        fn_802C8D74();
    }
}

}  // extern "C"

template <typename T>
inline int ArrayRef<T>::Count(void) {
    return count;
}

template <typename T>
inline T* ArrayRef<T>::At(int i) {
    return &data[i];
}

extern "C" {

void fn_801E1384(BoostArea* a, u32 color) {
    if (a->bound) {
        int i;
        fn_802C8DBC(a->ny * ((a->nx - 1) * 2));
        for (i = 1; i < a->nx; i++) {
            int j;
            a->Vtx(i - 1, 0)->Emit(color, 0);
            a->Vtx(i, 0)->Emit(color, 0);
            for (j = 1; j < a->ny; j++) {
                a->Vtx(i - 1, j)->Emit(color, 1);
                a->Vtx(i, j)->Emit(color, 1);
            }
        }
        fn_802C8E48();
    }
}

void fn_801E1520(void) {
    if (lbl_8055EA10) {
        delete lbl_8055EA10;
        lbl_8055EA10 = 0;
    }
}

}  // extern "C"

BoostMan::~BoostMan(void) {
    fn_801777D0(this);
    Clear();
    if (lines != -2) {
        fn_802CAC54(lines);
        lines = -2;
    }
    if (arrows != -2) {
        fn_802CAC54(arrows);
        arrows = -2;
    }
}

void BoostMan::a0(void) {}

void BoostMan::a1(void) {}

void BoostMan::a2(void) {
    int i;
    for (i = 0; i < 4; i++) {
        cars[i] = -1;
        players[i].Reset();
    }
}

static inline int Boosting(u8* rec) {
    int r = 0;
    bool fast = *(f32*)(rec + 0x258C) > 0.001f;
    if (fast && rec[0x259D]) {
        r = 1;
    }
    return r;
}

void BoostMan::a3(void) {
    int i;
    for (i = 0; i < fn_801E85A4(); i++) {
        int car = cars[i];
        if (car != players[i].car) {
            players[i].Clear();
            players[i].car = car;
        }
        if (players[i].car != -1) {
            u8* rec = fn_8017960C(players[i].car);
            int b = Boosting(rec);
            f32 speed = *(f32*)(rec + 0x258C);
            if (b && speed > players[i].last) {
                players[i].timer = 1.0f;
            } else {
                players[i].timer -= 0.011122234f;
            }
            players[i].last = speed;
            if (players[i].timer > 0.0f) {
                players[i].color1 = -1;
                players[i].color0 = 0xFFFFFF00;
            } else if (players[i].flag) {
                players[i].color1 = -1;
                players[i].color0 = 0xFFFF0000;
            } else {
                players[i].color1 = 0xFF7F7F7F;
                players[i].color0 = 0;
            }
        }
    }
}

void BoostMan::a4(void) {}
