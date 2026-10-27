// AI obstacles (0x8015EC50-0x8016071C): the computer drivers' list of things to steer around. Upright tubes
// in the track's object groups and the circles in aiobst.val become round obstacles (or one-sided arrows),
// sorted by track segment with a per-segment index; aiobst.val "eraser"s delete what they cover. Each frame
// a driver projects the obstacles ahead into 64 lateral bins and picks the nearest free gap. Names are
// guesses apart from the strings.
#include "game/common.h"

extern "C" {
int strcmp(const char* a, const char* b);
int strcasecmp(const char* a, const char* b);
void fn_802E30C8(const char* fmt, ...);  // debug print
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* t);
double sqrt(double x);
f32 PSVECDotProduct(const void* a, const void* b);
void fn_80265DCC(void* base, int num, int size, int (*cmp)(const void*, const void*));  // qsort
}

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

#define CLAMP01(x) ((x) = MIN(1.0f, (x)), (x) = MAX((x), 0.0f))
#define LERP(a, b, t) ((a) + (t) * ((b) - (a)))

struct Vec3 {
    f32 x;
    f32 y;
    f32 z;
};

struct Pt {
    f32 x;
    f32 y;
};

struct Vec2 {
    Vec2(void) {}
    Vec2(f32 a, f32 b) {
        v.x = a;
        v.y = b;
    }
    void Set(f32 a, f32 b) {
        v.x = a;
        v.y = b;
    }
    void SetXZ(const Vec3& p) {
        v.x = p.x;
        v.y = p.z;
    }
    Pt v;
};

// A difference of two points (kept in memory, like the original's temporaries).
struct Delta {
    f32 x;
    f32 z;
};


struct Frame {
    Vec3* Right(void) { return &basis[0]; }
    Vec3 basis[3];  // offset 0x0
    Vec3 origin;    // offset 0x24
};

struct Seg {
    u8 pad0[0x3C];
};

struct Path {
    u8 pad0[0x18];
    u32 count : 16;  // offset 0x18
    u32 pad18 : 16;
    Seg* segs;       // offset 0x1C
};

struct Track {
    u8 pad0[0x4];
    Seg* seg;  // offset 0x4
    u8 pad8[0x2C - 0x8];
    Path* path;  // offset 0x2C
};

struct TrackSample {
    Seg* seg;  // offset 0x0
    f32 t;     // offset 0x4
};

class TrackPosOwner {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual Track* GetTrack(void);  // 0x18
};

struct RaceCarRec {
    u8 pad0[0x3C];
    Frame mtx;  // offset 0x3C
    u8 pad6C[0x118 - 0x6C];
    Vec3 vel;  // offset 0x118
    u8 pad124[0x270 - 0x124];
    TrackPosOwner* pos;  // offset 0x270
    u8 pad274[0x25D4 - 0x274];
    f32 speed;  // offset 0x25D4
    Frame* Mtx(void) { return &mtx; }
};

// An object placed on the track (a group member's model instance).
struct TrackObj {
    u8 pad0[0xC];
    Frame* mtx;  // offset 0xC
    u8 pad10[0x18 - 0x10];
    u32 tag;  // offset 0x18
    u8 pad1C[0x20 - 0x1C];
    s32 id;    // offset 0x20
    f32 rad;   // offset 0x24
    f32 half;  // offset 0x28
};

struct GroupItem {
    u8 pad0[0x3C];
    TrackObj* obj;  // offset 0x3C
};

struct Group {
    s32 count;          // offset 0x0
    GroupItem** items;  // offset 0x4
};

extern "C" {
Track* fn_80178C9C(void);
void fn_80136594(TrackSample* out, Track* trk, const Vec2* xz);
int fn_80179650(void);
int fn_80179630(int view);
RaceCarRec* fn_8017960C(int id);
void fn_8019321C(Vec2* out, const Vec2* local, Frame* f);
void fn_8014C75C(void* steer, const Vec2* target);
void StatsUpdate__Fv(Vec3* v);
}

template <class T>
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
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

template <class T>
struct DynArray {
    DynArray(int n) : a(new ("dsa_buf") T[n], n) { alloc = a.data; }
    ~DynArray(void) { delete[] alloc; }
    T* Add(void) { return &a.data[a.count++]; }
    void Push(T v) {
        T* p = &a.data[a.count++];
        *p = v;
    }
    int Count(void) { return a.count; }
    T* At(int i) { return &a.data[i]; }
    void Remove(int k) {
        if (k >= 0 && k < a.count) {
            int last = a.count - 1;
            if (k != last) {
                a.data[k] = a.data[last];
            }
            a.count--;
        }
    }
    ArrayRef<T> a;  // offset 0x0
    T* alloc;       // offset 0xC
};

struct Obst {
    u16 seg;   // offset 0x0
    s16 type;  // offset 0x2 (0 circle, 1 left arrow, 2 right arrow)
    Vec2 pos;  // offset 0x4
    f32 rad;   // offset 0xC
};

ModuleTag lbl_8055E290;
UnkStaticFFFF lbl_8055E298;
DynArray<Obst>* lbl_8055E29C;   // all obstacles, by segment
DynArray<Obst*>* lbl_8055E2A0;  // per segment: the first obstacle at or after it

extern const f32 kWidth;
extern const f32 kBin;
extern const f32 kGap;
f32 lbl_8055E2A4 = kWidth * kBin;               // bin width
s32 lbl_8055E2A8 = (s32)(64.0f * (kGap / kWidth)) + 1;  // bins a car needs

static inline const char* Plural(int n) { return n == 1 ? "" : "s"; }

extern "C" {

void fn_8015EC50(Obst* o, Track* trk) {
    TrackSample ts;
    fn_80136594(&ts, trk, &o->pos);
    o->seg = ts.seg - trk->path->segs;
}

static inline int IsTube(TrackObj* o) { return o->id < 1000 && o->tag == 'TUBE'; }

int fn_8015ECC0(TrackObj* o) {
    int r = 0;
    if (IsTube(o) && (f32)__fabs(o->mtx->basis[2].y) > 0.9f) {
        r = 1;
    }
    return r;
}

int fn_8015ED18(ValueNode* n) { return strcasecmp(n->GetName(), "circle") == 0; }

int fn_8015ED54(ValueNode* n) { return strcasecmp(n->GetName(), "eraser") == 0; }

int fn_8015ED90(ValueNode* list) {
    int count = 0;
    if (list) {
        int i;
        ValueNode* n;
        for (i = 0; (n = list->GetChildAt(i)) != 0; i++) {
            if (fn_8015ED18(n)) {
                count++;
            }
        }
    }
    return count;
}

void fn_8015EE14(ValueNode* list, Track* trk) {
    if (list) {
        ValueNode* n;
        for (int i = 0; (n = list->GetChildAt(i)) != 0; i++) {
            if (fn_8015ED18(n)) {
                Obst* o = lbl_8055E29C->Add();
                const char* type;
                ValPoint p = n->GetPoint("p");
                o->pos.v.x = p.x;
                o->pos.v.y = p.y;
                fn_8015EC50(o, trk);
                type = n->GetString("type", "");
                if (strcmp(type, "left_arrow") == 0) {
                    o->type = 1;
                    o->rad = 0.0f;
                } else if (strcmp(type, "right_arrow") == 0) {
                    o->type = 2;
                    o->rad = 0.0f;
                } else {
                    o->type = 0;
                    o->rad = n->GetFloat("rad");
                }
            }
        }
    }
}

void fn_8015EF84(ValueNode* list) {
    int erased = 0;
    if (list) {
        int i;
        ValueNode* n;
        for (i = 0; (n = list->GetChildAt(i)) != 0; i++) {
            if (fn_8015ED54(n)) {
                erased++;
                ValPoint p;
                ValPoint t = n->GetPoint("p");
                p = t;
                f32 r2 = n->GetFloat("rad");
                int j;
                r2 = r2 * r2;
                j = 0;
                while (j < lbl_8055E29C->a.count) {
                    Obst* o = &lbl_8055E29C->a.data[j];
                    Vec2 d(p.x - o->pos.v.x, p.y - o->pos.v.y);
                    if (d.v.x * d.v.x + d.v.y * d.v.y < r2) {
                        lbl_8055E29C->Remove(j);
                    } else {
                        j++;
                    }
                }
            }
        }
    }
    if (erased > 0) {
        fn_802E30C8("obst: Read %d eraser%s.", erased, Plural(erased));
    }
}

int fn_8015F148(Group** groups, int n) {
    int count = 0;
    int i;
    for (i = 0; i < n; i++) {
        Group* g = groups[i];
        if (g) {
            int j;
            for (j = 0; j < g->count; j++) {
                if (fn_8015ECC0(g->items[j]->obj)) {
                    count++;
                }
            }
        }
    }
    return count;
}

void fn_8015F1E4(Obst* o, const TrackObj* t) {
    f32 r = t->rad;
    Delta axis;
    Vec3 c;
    axis.x = t->mtx->basis[2].x;
    axis.z = t->mtx->basis[2].z;
    o->rad = r + 0.5f * (3.0500002f * (f32)sqrt(axis.x * axis.x + axis.z * axis.z));
    if (t->mtx->basis[2].y < 0.0f) {
        f32 k = t->half - 1.5250001f;
        c.x = t->mtx->origin.x + t->mtx->basis[2].x * k;
        c.y = t->mtx->origin.y + t->mtx->basis[2].y * k;
        c.z = t->mtx->origin.z + t->mtx->basis[2].z * k;
    } else {
        f32 k = -(t->half - 1.5250001f);
        c.x = t->mtx->origin.x + t->mtx->basis[2].x * k;
        c.y = t->mtx->origin.y + t->mtx->basis[2].y * k;
        c.z = t->mtx->origin.z + t->mtx->basis[2].z * k;
    }
    o->pos.v.x = c.x;
    o->pos.v.y = c.z;
    o->type = 0;
}

void fn_8015F320(Group** groups, int n, Track* trk) {
    int i;
    for (i = 0; i < n; i++) {
        Group* g = groups[i];
        if (g) {
            int j;
            for (j = 0; j < g->count; j++) {
                TrackObj* t = g->items[j]->obj;
                if (fn_8015ECC0(t)) {
                    Obst* o = lbl_8055E29C->Add();
                    fn_8015F1E4(o, t);
                    fn_8015EC50(o, trk);
                }
            }
        }
    }
}

int fn_8015F3EC(const void* a, const void* b) { return ((const Obst*)a)->seg - ((const Obst*)b)->seg; }

void fn_8015F3FC(Track* trk) {
    if (lbl_8055E29C->a.count > 0) {
        int nseg;
        int s;
        int j;
        fn_80265DCC(lbl_8055E29C->a.data, lbl_8055E29C->a.count, sizeof(Obst), fn_8015F3EC);
        nseg = trk->path->count;
        lbl_8055E2A0 = new ("ai-obsindex") DynArray<Obst*>(nseg);
        j = 0;
        for (s = 0; s < nseg; s++) {
            while (j < lbl_8055E29C->a.count - 1 && lbl_8055E29C->a.data[j].seg < s) {
                j++;
            }
            if (j == lbl_8055E29C->a.count - 1 && lbl_8055E29C->a.data[j].seg < s) {
                lbl_8055E2A0->Push(lbl_8055E29C->a.data);
            } else {
                lbl_8055E2A0->Push(&lbl_8055E29C->a.data[j]);
            }
        }
    }
}

void fn_8015F554(void) {}

}  // extern "C"

template <class T>
void Free(T*& p) {
    if (p) {
        delete p;
        p = 0;
    }
}

extern "C" {

void fn_8015F558(void) {
    Free(lbl_8055E2A0);
    Free(lbl_8055E29C);
}

void fn_8015F670(Group** groups, int n) {
    Track* trk = fn_80178C9C();
    ValueTable* vt = fn_80131810("aiobst.val");
    ValueNode* objs;
    int total;
    objs = vt ? vt->FindChild("objects") : 0;
    total = fn_8015F148(groups, n);
    total += fn_8015ED90(objs);
    if (total > 0) {
        lbl_8055E29C = new ("ai-obstable") DynArray<Obst>(total);
        fn_8015F320(groups, n, trk);
        fn_8015EF84(objs);
        fn_8015EE14(objs, trk);
        fn_8015F3FC(trk);
    }
    if (vt) {
        fn_80131818(vt);
    }
}

// The lateral bin (-n/2..n/2) that the car's velocity points at.
int fn_8015F7A0(int n, const Vec3* vel, const Vec3* right) {
    f32 half = n / 4.0f;
    f32 t = (PSVECDotProduct(vel, right) - -13.333333f) / 26.666666f;
    CLAMP01(t);
    return LERP(-half, half, t);
}

}  // extern "C"


struct Bin {
    f32 dist;  // offset 0x0
    s32 type;  // offset 0x4
};

extern "C" {

int fn_8015F848(Bin* bins, f32 range) {
    int i;
    int last = -1;
    f32 bw = lbl_8055E2A4;
    for (i = 0; i < 64; i++) {
        f32 d = bins[i].dist;
        if (d < range) {
            if (last > 0 && (f32)__fabs(bins[last].dist - d) < range * 0.5f &&
                bw * (f32)(i - last) < kGap) {
                Bin* a = &bins[last];
                Bin* b = &bins[i];
                for (; last <= i; last++) {
                    bins[last].dist = MIN(a->dist, b->dist);
                }
            }
            last = i;
        }
    }
    return last >= 0;
}

f32 fn_8015F92C(Bin* bins, f32 x) {
    int i;
    for (i = 0; i < lbl_8055E2A8 / 2 + 1; i++) {
        x = MIN(x, bins[32 + i].dist);
        x = MIN(x, bins[32 - i].dist);
    }
    for (i = 0; i < 64; i++) {
        bins[i].dist = bins[i].dist + 1.5f * (bins[i].dist - x);
    }
    return x;
}

int fn_8015FA44(Bin* bins, int i, f32 range) {
    int r = -1;
    if (bins[i].type != 1) {
        int j;
        for (j = i + 1; j < 64; j++) {
            if (bins[j].dist > range) {
                r = j;
                break;
            }
            if (bins[j].type == 1) {
                break;
            }
            if (bins[j].type == 2) {
                r = j;
            }
        }
    }
    return r;
}

int fn_8015FAB8(Bin* bins, int i, f32 range) {
    int r = -1;
    if (bins[i].type != 2) {
        int j;
        for (j = i - 1; j >= 0; j--) {
            if (bins[j].dist > range) {
                r = j;
                break;
            }
            if (bins[j].type == 2) {
                break;
            }
            if (bins[j].type == 1) {
                r = j;
            }
        }
    }
    return r;
}

// The car's frame pushed 3.1 forward.
extern const f32 lbl_80561810;  // 3.1f
void fn_8015FB28(Frame* f, const Frame* m) {
    *f = *m;
    f->origin.x += f->basis[2].x * lbl_80561810;
    f->origin.y += f->basis[2].y * lbl_80561810;
    f->origin.z += f->basis[2].z * lbl_80561810;
}

}  // extern "C"

// The driver's view: forward and right in the ground plane, and where it is.
struct View {
    View(const Frame& f) {
        f32 len;
        f32 inv;
        fwd.x = f.basis[2].x;
        fwd.y = f.basis[2].z;
        len = sqrt(fwd.x * fwd.x + fwd.y * fwd.y);
        inv = 1.0f / MAX(0.01f, len);
        at.x = f.origin.x;
        at.y = f.origin.z;
        fwd.x *= inv;
        fwd.y *= inv;
        right.x = fwd.y;
        right.y = -fwd.x;
    }
    f32 Ahead(const Vec2* p, const Pt* o) const {
        Delta d;
        d.x = p->v.x - o->x;
        d.z = p->v.y - o->y;
        f32 fx = fwd.x;
        f32 fy = fwd.y;
        return fx * d.x + fy * d.z;
    }
    f32 Side(const Vec2* p, const Pt* o) const {
        Delta d;
        d.x = p->v.x - o->x;
        d.z = p->v.y - o->y;
        f32 sx = fwd.y;
        f32 sz = -fwd.x;
        return sx * d.x + sz * d.z;
    }
    Pt fwd;    // offset 0x0
    Pt right;  // offset 0x8
    Pt at;     // offset 0x10
};

struct Opts {
    void Init(void) {}
    u8 side;
};

struct Bins {
    // Marks the bins an obstacle covers with its distance ahead.
    void Add(Obst* o, View* v, u8 side, f32 range) {
        Vec2* p = &o->pos;
        f32 d = v->Ahead(p, &v->at) - o->rad;
        if (d > 0.0f && d < range) {
            f32 s = v->Side(p, &v->at);
            if (s + o->rad > -kWidth / 2.0f && s - o->rad < kWidth / 2.0f) {
                int lo;
                int hi;
                int k;
                switch (o->type) {
                case 0: {
                    f32 t = (s - o->rad - -kWidth * 0.5f) / (kWidth * 0.5f - -kWidth * 0.5f);
                    CLAMP01(t);
                    lo = LERP(0.0f, 64.0f, t);
                    t = (lbl_8055E2A4 + (s + o->rad) - -kWidth * 0.5f) / (kWidth * 0.5f - -kWidth * 0.5f);
                    CLAMP01(t);
                    hi = LERP(0.0f, 64.0f, t);
                    break;
                }
                case 2: {
                    f32 t = (s + lbl_8055E2A4 - -kWidth * 0.5f) / (kWidth * 0.5f - -kWidth * 0.5f);
                    lo = 0;
                    CLAMP01(t);
                    hi = LERP(0.0f, 64.0f, t);
                    break;
                }
                case 1: {
                    f32 t = (s - -kWidth * 0.5f) / (kWidth * 0.5f - -kWidth * 0.5f);
                    CLAMP01(t);
                    hi = 64;
                    lo = LERP(0.0f, 64.0f, t);
                    break;
                }
                default:
                    hi = 0;
                    lo = 0;
                    break;
                }
                for (k = lo; k < hi; k++) {
                    if (d < bins[k].dist) {
                        bins[k].dist = d;
                        bins[k].type = o->type;
                    }
                }
            }
        }
    }
    Bin bins[64];
};

struct Spot {
    Vec3 pos;
    void From(const Vec2& v) {
        pos.x = v.v.x;
        pos.y = 0.0f;
        pos.z = v.v.y;
    }
};

inline int Abs(int x) { return x < 0 ? -x : x; }
inline int MinI(int a, int b) { return b < a ? b : a; }
inline Track* GetTrack(RaceCarRec* rc) { return rc->pos->GetTrack(); }

extern "C" {

#pragma dont_inline on
// Picks a gap for the car to steer into; returns whether it had to.
int fn_8015FBCC(RaceCarRec* rc, void* steer, u8 side, int flatten) {
    int found = 0;
    if (lbl_8055E2A0) {
        Track* trk = rc->pos->GetTrack();
        int seg = trk->seg - trk->path->segs;
        Frame f;
        f32 range;
        Opts opts;
        Obst* o;
        Bins bins;
        const Vec3* vel;
        int i;
        int any;
        f32 nearest;
        fn_8015FB28(&f, &rc->mtx);
        range = MAX(20.0f, 0.6f * rc->speed);
        opts.side = side;
        o = lbl_8055E2A0->a.data[seg];
        vel = &rc->vel;
        opts.Init();
        for (i = 0; i < 64; i++) {
            bins.bins[i].dist = range;
            bins.bins[i].type = 0;
        }
        {
            View v(f);
            for (i = 0; i < 50; i++) {
                bins.Add(o, &v, opts.side, range);
                o++;
                if (o > lbl_8055E29C->At(lbl_8055E29C->Count() - 1)) {
                    o = lbl_8055E29C->At(0);
                }
            }
            for (i = 0; i < fn_80179650(); i++) {
                const Vec3& p = fn_8017960C(fn_80179630(i))->Mtx()->origin;
                Vec2 xz;
                xz.SetXZ(p);
                {
                    Obst car = {0};
                    car.pos = xz;
                    car.rad = 2.0f;
                    bins.Add(&car, &v, opts.side, range);
                }
            }
        }
        any = fn_8015F848(bins.bins, range);
        nearest = fn_8015F92C(bins.bins, range);
        if (any) {
            int c;
            int mid;
            if (flatten) {
                for (i = 0; i < 64; i++) {
                    bins.bins[i].dist = bins.bins[i].dist >= range ? nearest : range;
                }
            }
            {
                Vec2 dir(0.0f, 0.0f);
                Vec2 target(0.0f, 0.0f);
                mid = fn_8015F7A0(64, vel, f.Right());
                c = mid + 32;
                if (bins.bins[c].dist < range) {
                    int r = fn_8015FA44(bins.bins, c, range);
                    int l = fn_8015FAB8(bins.bins, c, range);
                    if (r >= 0 && (l < 0 || Abs(r - c) < Abs(l - c))) {
                        target.Set(1.875f + (lbl_8055E2A4 * (f32)r - kWidth * 0.5f), bins.bins[r - 1].dist);
                        dir.Set(1.875f, 0.0f);
                        found = 1;
                    } else if (l >= 0 && (r < 0 || Abs(r - c) > Abs(l - c))) {
                        target.Set(lbl_8055E2A4 * (f32)l - kWidth * 0.5f - 1.875f, bins.bins[l + 1].dist);
                        dir.Set(-1.875f, 0.0f);
                        found = 1;
                    }
                } else {
                    int left = 32 - Abs(mid);
                    int n = MinI(left, lbl_8055E2A8 / 2 + Abs(mid) + 1);
                    int k;
                    for (k = 1; k < n; k++) {
                        if (bins.bins[c + k].dist < range) {
                            target.Set(lbl_8055E2A4 * (f32)(c + k - 1) - kWidth * 0.5f - 1.875f,
                                       bins.bins[c + k].dist);
                            dir.Set(-1.875f, 0.0f);
                            found = 1;
                            break;
                        }
                        if (bins.bins[c - k].dist < range) {
                            target.Set(1.875f + (lbl_8055E2A4 * (f32)(c - k + 1) - kWidth * 0.5f),
                                       bins.bins[c - k].dist);
                            dir.Set(1.875f, 0.0f);
                            found = 1;
                            break;
                        }
                    }
                }
                if (found) {
                    Vec2 w;
                    Spot spot;
                    fn_8019321C(&w, &target, &f);
                    if (steer) {
                        fn_8014C75C(steer, &w);
                    }
                    StatsUpdate__Fv(&spot.pos);
                    spot.From(w);
                }
            }
        }
    }
    return found;
}


}  // extern "C"

// The target never folds these (binSize/gapBins are computed in the sinit, kWidth * 0.5f at run time), yet
// they sit mid-.sdata2 (after fn_8015F7A0's literals). Defined here the code matches; the data order does
// not yet. 3.05/1.525/3.289 are one ulp off the nearest float in the target, hence the odd literals.
extern const f32 kWidth = 18.0f;  // the lane width the bins cover
extern const f32 kBin = 0.015625f;
extern const f32 kGap = 3.2889998f;  // the gap a car fits through

#pragma dont_inline reset
