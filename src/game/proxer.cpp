// Proxer (0x801368F8-0x80137930): proximity tracking between every pair of AI cars. Keeps a per-car
// record (ideal-line position/velocity, lap flip, section flags) and a pair table with the time until
// one car reaches the other along the track, plus per-car sorted pair lists. Names are guesses apart
// from "Proxer" (its allocation tag).
#include "game/common.h"

extern "C" {
void* memset(void* p, int c, size_t n);
void* fn_801284E8(const char* tag, s32 size);  // MemAlloc
void fn_80128658(void* p);                     // MemFree
f32 fn_8013672C(void);                         // center line length
u8* fn_8017960C(int i);
void* fn_802C6C34(int id);
f32 PSVECDotProduct(const void* a, const void* b);
}

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

struct V2 {
    f32 x;
    f32 y;
};

struct V3 {
    f32 x;
    f32 y;
    f32 z;
};

struct Body {
    u8 pad0[0x18];
    V3 vel;  // offset 0x18
    V3 pos;  // offset 0x24
};

struct TuneParams {
    u8 pad0[4];
    f32 gap;  // offset 0x4
};

extern "C" TuneParams* fn_80137C54(int i);

struct IdealPointP {
    u8 pad0[0x14];
    IdealPointP* next;  // offset 0x14
    u8 pad18[0x14];
    f32 lat;            // offset 0x2C
    f32 lon;            // offset 0x30
    u8 pad34;
    u8 section;         // offset 0x35
};

struct Follower {
    u8 pad0[4];
    IdealPointP* seg;  // offset 0x4
    f32 t;             // offset 0x8
    u8 padC[0x34];
    f32 lateral;       // offset 0x40
    f32 latSpeed;      // offset 0x44
    f32 travel;        // offset 0x48
    f32 speed;         // offset 0x4C
};

struct CarPhys {
    u8 pad0[0x60];
    f32 x;   // offset 0x60
    u8 pad64[4];
    f32 z;   // offset 0x68
    u8 pad6C[0xAC];
    f32 vx;  // offset 0x118
    u8 pad11C[4];
    f32 vz;  // offset 0x120
};

class AICar {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual Follower* GetFollower(void);  // offset 0x18
    virtual void vf1C(void);
    virtual CarPhys* GetPhys(void);       // offset 0x20
};

extern "C" AICar* fn_801B2ED0(int i);

struct Pair {
    Pair(void) {
        b = -1;
        dvel.y = 0.0f;
        dvel.x = 0.0f;
        dpos.y = 0.0f;
        dpos.x = 0.0f;
        a = -1;
        close = 0;
    }
    void UpdateDist(void);
    void SetPos(const struct CarRec* a, const struct CarRec* b);

    u8 close;    // offset 0x0
    f32 time;    // offset 0x4
    u8 aFlag;    // offset 0x8
    s32 a;       // offset 0xC
    u8 bFlag;    // offset 0x10
    s32 b;       // offset 0x14
    f32 dist2;   // offset 0x18
    V2 dpos;     // offset 0x1C
    V2 dvel;     // offset 0x24
};

struct CarRec {
    V2 pos;       // offset 0x0
    V2 vel;       // offset 0x8
    f32 accel;    // offset 0x10
    u8 flipped;   // offset 0x14
    u8 active;    // offset 0x15
    u8 sec;       // offset 0x16
    Pair* table[1];  // offset 0x18 ([3][count])

    void ClearVel(void) {
        vel.x = 0.0f;
        vel.y = 0.0f;
    }
};

inline void Pair::SetPos(const CarRec* a, const CarRec* b) {
    dpos.x = b->pos.x - a->pos.x;
    dpos.y = b->pos.y - a->pos.y;
}

class Proxer;
typedef int (Proxer::*PairCmp)(Pair* a, Pair* b);

class Proxer {
public:
    Proxer(s32 n);
    ~Proxer(void);
    void Build(void);
    void Activate(int i);
    void Update(int i);
    void SetActive(int i, u8 on);
    void SortCar(int i);
    void UpdateAll(void);
    void Process(void);
    int Compare(Pair* a, Pair* b);
    void Sort(Pair** list, PairCmp cmp, int key);

    CarRec* Rec(int i) { return (CarRec*)(recs + i * stride); }

    s32 count;    // offset 0x0
    s32 key;      // offset 0x4
    Pair* pairs;  // offset 0x8
    s32 stride;   // offset 0xC
    u8* recs;     // offset 0x10
    f32 lapLen;   // offset 0x14
};

ModuleTag lbl_8055E0C8;
Proxer* lbl_8055E0D0;
Proxer* lbl_8055E0D4;

Proxer::Proxer(s32 n) {
    s32 size;
    count = n;
    pairs = new ("proxer delta") Pair[n * (n - 1) / 2 > 1 ? n * (n - 1) / 2 : 1];
    stride = (n * 3) * sizeof(Pair*) + 0x18;
    size = n * stride;
    recs = (u8*)fn_801284E8("proxer tab", size);
    if (recs && pairs) {
        memset(recs, 0, size);
        Build();
    }
    lapLen = fn_8013672C();
}

Proxer::~Proxer(void) {
    if (recs) {
        fn_80128658(recs);
        recs = 0;
    }
    if (pairs) {
        delete[] pairs;
        pairs = 0;
    }
    lbl_8055E0D0 = 0;
}

void Proxer::Build(void) {
    int i;
    int j;
    int k = 0;
    for (i = 0; i < count; i++) {
        Rec(i)->active = 1;
        Rec(i)->table[i] = 0;
        Rec(i)->table[i + count] = 0;
        Rec(i)->table[i + count * 2] = 0;
        for (j = i + 1; j < count; j++) {
            Rec(i)->table[j] = &pairs[k];
            Rec(i)->table[j + count] = &pairs[k];
            Rec(i)->table[j + count * 2] = &pairs[k];
            Rec(j)->table[i] = &pairs[k];
            Rec(j)->table[i + count] = &pairs[k];
            Rec(j)->table[i + count * 2] = &pairs[k];
            pairs[k].a = (s16)i;
            pairs[k].b = (s16)j;
            k++;
        }
    }
}

void Proxer::Activate(int i) {
    Update(i);
    Rec(i)->ClearVel();
    Rec(i)->accel = 0.0f;
}

static inline f32 LerpF(f32 a, f32 b, f32 t) { return a + t * (b - a); }

void Proxer::Update(int i) {
    CarRec* r = Rec(i);
    Follower* f = fn_801B2ED0(i)->GetFollower();
    if (r->active) {
        IdealPointP* s;
        f32 x;
        f32 y;
        f32 old;
        u8 sec;
        r->flipped = 0;
        s = f->seg;
        x = f->lateral + (s->lat + f->t * (s->next->lat - s->lat));
        y = f->travel + s->lon;
        if (r->flipped) {
            y = lapLen - y;
        }
        r->vel.x = f->latSpeed;
        r->pos.x = x;
        r->pos.y = y;
        old = r->vel.y;
        r->vel.y = f->speed;
        if (r->flipped) {
            r->vel.y *= -1.0f;
        }
        r->accel = LerpF(r->accel, (r->vel.y - old) / (1.0f / 59.94f), 0.8f);
        sec = f->seg->section;
        r->sec = sec;
        if ((sec & ~1) && (sec & 1) && !(f->seg->next->section & 1)) {
            r->sec &= ~1;
        }
    } else {
        r->pos.y = 0.0f;
        r->pos.x = 0.0f;
        r->vel.y = 0.0f;
        r->vel.x = 0.0f;
        r->sec = 0;
    }
}

void Proxer::SetActive(int i, u8 on) { Rec(i)->active = on; }

void Proxer::SortCar(int i) { Sort(&Rec(i)->table[count * 2], &Proxer::Compare, i); }

void Proxer::UpdateAll(void) {
    int i;
    int n;
    int k;
    for (i = 0; i < count; i++) {
        CarRec* r = Rec(i);
        CarPhys* c = fn_801B2ED0(i)->GetPhys();
        r->pos.x = c->x;
        r->pos.y = c->z;
        r->vel.x = c->vx;
        r->vel.y = c->vz;
        r->sec = 1;
    }
    n = count * (count - 1) / 2;
    for (k = 0; k < n; k++) {
        Pair* p = &pairs[k];
        CarRec* a = Rec(p->a);
        CarRec* b = Rec(p->b);
        p->dvel.x = b->vel.x - a->vel.x;
        p->dvel.y = b->vel.y - a->vel.y;
        pairs[k].SetPos(a, b);
        pairs[k].UpdateDist();
    }
}

extern "C" {

bool fn_8013705C(f32 a, f32 b) { return a * b >= 0.0f; }

bool fn_80137078(const Body* a, const Body* b, f32 ta, f32 tb) {
    V3 pa;
    V3 pb;
    V3 d;
    pa.x = a->pos.x + a->vel.x * ta;
    pa.y = a->pos.y + a->vel.y * ta;
    pa.z = a->pos.z + a->vel.z * ta;
    pb.x = b->pos.x + b->vel.x * tb;
    pb.y = b->pos.y + b->vel.y * tb;
    pb.z = b->pos.z + b->vel.z * tb;
    d.x = pa.x - pb.x;
    d.y = pa.y - pb.y;
    d.z = pa.z - pb.z;
    return PSVECDotProduct(&d, &b->vel) > 0.0f;
}

int fn_80137140(int i, int j) {
    u8* ca = fn_8017960C(i);
    u8* cb = fn_8017960C(j);
    u8* ma = (u8*)fn_802C6C34(*(int*)(ca + 0x59C));
    u8* mb = (u8*)fn_802C6C34(*(int*)(cb + 0x59C));
    int r = 0;
    if (fn_80137078((Body*)(ca + 0x3C), (Body*)(cb + 0x3C), *(f32*)(ma + 0x40), *(f32*)(mb + 0x3C)) &&
        fn_80137078((Body*)(cb + 0x3C), (Body*)(ca + 0x3C), *(f32*)(mb + 0x40), *(f32*)(ma + 0x3C))) {
        r = 1;
    }
    return r;
}

}  // extern "C"

void Proxer::Process(void) {
    f32 d;
    f32 gap = fn_80137C54(-1)->gap;
    int k;
    int n;
    fn_80137C54(-1);
    n = count * (count - 1) / 2;
    for (k = 0; k < n; k++) {
        Pair* p = &pairs[k];
        CarRec* a = Rec(p->a);
        CarRec* b = Rec(p->b);
        f32 dy;
        u8 close;
        p->dvel.x = b->vel.x - a->vel.x;
        p->dvel.y = b->vel.y - a->vel.y;
        pairs[k].SetPos(a, b);
        dy = b->pos.y - a->pos.y;
        if ((f32)__fabs(dy) > lapLen / 2.0f && !((a->sec | b->sec) & 8)) {
            dy += lapLen * -(dy > 0.0f ? 1.0f : -1.0f);
        }
        pairs[k].dpos.y = dy;
        close = 0;
        if (__fabs(dy) < gap * 2.0f && fn_80137140(pairs[k].a, pairs[k].b)) {
            close = 1;
        }
        pairs[k].close = close;
        pairs[k].time = 123.0f;
        if (a->sec & b->sec) {
            Pair* q = &pairs[k];
            if (!pairs[k].close) {
                f32 reach = 5.0f * gap;
                if (fn_8013705C(a->vel.y, b->vel.y)) {
                    f32 sgn = a->vel.y > 0.0f ? 1.0f : -1.0f;
                    if (sgn * q->dpos.y > 0.0f) {
                        f32 rel;
                        q->bFlag = 0;
                        rel = sgn * q->dvel.y;
                        if (rel < -0.8888889f) {
                            d = sgn * q->dpos.y;
                            f32 tt = -((d - gap) / rel);
                            if (tt < 8.0f || d <= reach) {
                                q->aFlag = 1;
                                q->time = tt;
                            } else {
                                q->aFlag = 0;
                            }
                        } else if (sgn * q->dpos.y <= reach) {
                            q->time = 8.0f;
                            q->aFlag = 1;
                        } else {
                            q->aFlag = 0;
                        }
                    } else {
                        f32 dv;
                        q->aFlag = 0;
                        dv = q->dvel.y;
                        if (sgn * -dv < -0.8888889f) {
                            f32 tt = -((gap + sgn * q->dpos.y) / (sgn * dv));
                            if (tt < 8.0f || sgn * -q->dpos.y <= reach) {
                                q->bFlag = 1;
                                q->time = tt;
                            } else {
                                q->bFlag = 0;
                            }
                        } else if (sgn * -q->dpos.y <= reach) {
                            q->time = 8.0f;
                            q->bFlag = 1;
                        } else {
                            q->bFlag = 0;
                        }
                    }
                } else if (!fn_8013705C(q->dvel.y, q->dpos.y)) {
                    q->time = ((f32)__fabs(q->dpos.y) - gap) / (f32)__fabs(q->dvel.y);
                    q->bFlag = 1;
                    q->aFlag = 1;
                } else {
                    q->bFlag = 0;
                    q->aFlag = 0;
                }
            } else {
                q->bFlag = 1;
                q->aFlag = 1;
            }
        } else {
            pairs[k].bFlag = 0;
            pairs[k].aFlag = 0;
        }
        pairs[k].UpdateDist();
    }
}

int Proxer::Compare(Pair* p, Pair* q) {
    if (p && q) {
        u8 fp = key == p->a ? p->aFlag : p->bFlag;
        u8 fq = key == q->a ? q->aFlag : q->bFlag;
        if (fp ^ fq) {
            return fp ? -1 : 1;
        }
        if (fp && fq) {
            u8 cp = p->close;
            if (cp || q->close) {
                if (cp ^ q->close) {
                    return !cp ? -1 : 1;
                }
                return 0;
            } else {
                f32 d = p->time - q->time;
                if ((f32)__fabs(d) < 0.01f) {
                    return 0;
                }
                if (d > 0.0f) {
                    return 1;
                }
                return -1;
            }
        }
        return 0;
    } else {
        return !p ? 1 : -1;
    }
}

void Proxer::Sort(Pair** list, PairCmp cmp, int k) {
    bool done;
    int lo;
    int hi;
    int newHi;
    int j;
    key = k;
    hi = count;
    lo = 0;
    do {
        newHi = hi;
        done = true;
        for (j = lo + 1; j < hi; j++) {
            if ((this->*cmp)(list[j - 1], list[j]) > 0) {
                Pair* t = list[j - 1];
                list[j - 1] = list[j];
                list[j] = t;
                newHi = j + 1;
                if (done) {
                    lo = j - 2 < 0 ? 0 : j - 2;
                }
                done = false;
            }
        }
        hi = newHi;
    } while (!done);
}

static inline f32 Len2(f32 x, f32 y) { return x * x + y * y; }
void Pair::UpdateDist(void) { dist2 = Len2(dpos.x, dpos.y); }

extern "C" {

static inline void ScaleV(V2* o, const V2& v, f32 s) {
    f32 x = v.x * s;
    f32 y = v.y * s;
    o->x = x;
    o->y = y;
}
static inline void CopyV(V2* o, const V2& v) {
    f32 x = v.x;
    f32 y = v.y;
    o->x = x;
    o->y = y;
}

void fn_80137788(Pair* p, int car, V2* out) {
    if ((car == p->b) ^ lbl_8055E0D0->Rec(car)->flipped) {
        ScaleV(out, p->dpos, -1.0f);
    } else {
        CopyV(out, p->dpos);
    }
}

void fn_801377EC(Pair* p, int car, V2* out) {
    if ((car == p->b) ^ lbl_8055E0D0->Rec(car)->flipped) {
        ScaleV(out, p->dvel, -1.0f);
    } else {
        CopyV(out, p->dvel);
    }
}

struct ProxQuery {
    struct {
        u8 pad0[4];
        f32 t;
    }* when;
    u8 pad4[0x14];
    s32 car;  // offset 0x18
};

static inline f32 Product(f32 a, f32 b) { return a * b; }

f32 fn_80137850(ProxQuery* q) {
    CarRec* r = lbl_8055E0D0->Rec(q->car);
    return r->pos.x + Product(r->vel.x, q->when->t);
}

void fn_80137884(s32 n) { lbl_8055E0D0 = lbl_8055E0D4 = new ("Proxer") Proxer(n); }

void fn_801378D0(void) {
    delete lbl_8055E0D4;
    lbl_8055E0D4 = 0;
    lbl_8055E0D0 = 0;
}

void fn_80137904(void) { lbl_8055E0D4->Build(); }

}  // extern "C"

PairCmp lbl_8042F778 = 0;
