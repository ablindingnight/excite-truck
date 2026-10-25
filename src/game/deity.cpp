// Deity (0x801B06D8-0x801B32C8): the race overseer. One DeityCar (0x80 bytes) per car tracks its centre-line
// and racing-line followers, laps and finish; it teleports cars back onto the track (avoiding the start/finish
// line and the "bad_teleport" regions, lifting cars onto bridges), queues lap crossings in time order, ranks the
// cars and locks the finish order, and runs the race intro countdown ("green flag"). The per-car update, the
// lap-crossing scan and the qualify helper are placeholders. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
void* memset(void* dst, int c, size_t n);
void* memmove(void* dst, const void* src, size_t n);
void qsort(void* base, size_t n, size_t size, int (*cmp)(const void*, const void*));
void fn_802E30C8(const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);
}

void* operator new(size_t size, const char* name);
void* operator new[](size_t size, const char* name);
void operator delete(void* p);
void operator delete[](void* p);

extern f32 lbl_8055E3C8;    // time
extern s32 lbl_8055E3C4;    // frame
extern f32 lbl_80558DC4[];  // FLT_EPSILON

struct V2 {
    f32 x;
    f32 y;
};

struct IdealPoint {
    V2 pos;            // offset 0x0
    V2 dir;            // offset 0x8
    u32 ilpos;         // offset 0x10 (16.16)
    IdealPoint* next;  // offset 0x14
    f32 len;           // offset 0x18
    f32 dist;          // offset 0x1C
};

struct CenterLine {
    u8 pad0[0x18];
    u32 total;           // offset 0x18 (16.16)
    IdealPoint* points;  // offset 0x1C
};

struct ILPos {
    IdealPoint* seg;  // offset 0x0
    f32 t;            // offset 0x4
    s32 mode;         // offset 0x8
};

class LineFollower {
public:
    virtual ~LineFollower(void);
    virtual int SetLine(CenterLine* line);
    virtual void Attach(CenterLine* line, V2* pos, V2* dir);
    void SetPosition(V2* pos, V2* dir, IdealPoint* start);
    ILPos il;          // offset 0x4
    V2 dir;            // offset 0x10
    V2 off;            // offset 0x18
    f32 x20;           // offset 0x20
    u8 pad24[0x2C - 0x24];
    CenterLine* line;  // offset 0x2C
    LineFollower* x30; // offset 0x30
};

class IdealFollower : public LineFollower {
public:
    IdealFollower(int withTimes, CenterLine* line);
    void Reset(void);
    void Start(V2* pos, V2* dir, int resetTimes, IdealPoint* start);
};

class PathSet {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual CenterLine* Center(int i);  // 0x10
    virtual CenterLine* Racing(int i);  // 0x14
    virtual void Locate(int il, void* pos);  // 0x18
    virtual void v1C(void);
    virtual void Reset(void);                // 0x20
};

// The car a DeityCar oversees (offsets only).
class DCar {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual void v28(void);
    virtual void v2C(void);
    virtual void v30(void);
    virtual void v34(void);
    virtual void v38(void);
    virtual void v3C(void);
    virtual void v40(void);
    virtual void v44(void);
    virtual void v48(void);
    virtual void v4C(void);
    virtual void v50(void);
    virtual void Teleport(void* info);  // 0x54
    virtual void Lap(void);             // 0x58
    u8 pad4[0x10 - 0x4];
    u32 type;  // offset 0x10
    u8 pad14[0x54 - 0x14];
    f32 fwd[3];  // offset 0x54
    u8 pad60[0x94 - 0x60];
    f32 pos[3];  // offset 0x94
    u8 padA0[0x1EC - 0xA0];
    s32 il;      // offset 0x1EC
    u8 pad1F0[0x5BC - 0x1F0];
    f32 ahead;   // offset 0x5BC
};

class DAI {
public:
    virtual void v08(void);
    virtual int v0C(void* a, void* b, int c, void* d, void* e);
    virtual int v10(void* a, void* b, int c, void* d, void* e);
    virtual int v14(int a, int b);
    virtual void v18(int a, int b, int c, int d);
    virtual void v1C(void);
    virtual void v20(void);
    virtual int v24(void);
    virtual int v28(void* a, int b, int c, int d);
};

class DMaster {
public:
    virtual void m08(void);
    virtual void m0C(void);
    virtual void m10(void);
    virtual void m14(void);
    virtual void m18(void);
    virtual void m1C(void);
    virtual void m20(void);
    virtual void m24(void);
    virtual void m28(void);
    virtual void m2C(void);
    virtual void m30(void);
    virtual void m34(void);
    virtual void m38(void);
    virtual void m3C(void);
    virtual void m40(void);
    virtual void m44(void);
    virtual void m48(void);
    virtual void m4C(void);
    virtual void m50(void);
    virtual void m54(void);
    virtual void m58(void);
    virtual void m5C(void);
    virtual void m60(void);
    virtual void m64(void);
    virtual void m68(void);
    virtual void m6C(void);
    virtual void m70(void);
    virtual void m74(void);
    virtual void m78(void);
    virtual void m7C(void);
    virtual void m80(void);
    virtual void m84(void);
    virtual void m88(void);
    virtual void m8C(void);
    virtual void m90(void);
    virtual void Finished(DCar* car, s32 place);  // 0x94
    virtual void m98(void);
    virtual void m9C(void);
    virtual void mA0(void);
    virtual void mA4(void);
    virtual void mA8(void);
    virtual void mAC(void);
    virtual void mB0(void);
    virtual void mB4(void);
    virtual void mB8(void);
    virtual void mBC(void);
    virtual void mC0(void);
    virtual void mC4(void);
    virtual void mC8(void);
    virtual void mCC(void);
    virtual void mD0(void);
    virtual void mD4(void);
    virtual void mD8(void);
    virtual void mDC(void);
    virtual void mE0(void);
    virtual void mE4(void);
    virtual DAI* GetAI(void);  // 0xE8
};

// Per-car race state (the vtable 0x10 accessor returns it). NOTE: the offset comments are relative
// to the owning DeityCar (State() returns car + 4); subtract 4 for offsets within DState.
struct DState {
    u32 flags;   // offset 0x4
    s32 place;   // offset 0x8
    u8 padC[0x14 - 0xC];
    s32 lap;     // offset 0x14
    u8 pad18[0x1C - 0x18];
    f32 best;    // offset 0x1C
    f32 last;    // offset 0x20
    f32 qual;    // offset 0x24
    s32 mode;    // offset 0x28
    u8 hold;     // offset 0x2C
    f32 holdTime;  // offset 0x30
    u8 pad34[0x3C - 0x34];
};

class DeityCar {
public:
    DeityCar(void);
    virtual void Reset(void);             // 0x08
    virtual void Update(void);            // 0x0C
    virtual DState* State(void);          // 0x10
    virtual int v14(void);                // 0x14
    virtual IdealFollower* Ideal(void);   // 0x18
    virtual LineFollower* Line(void);     // 0x1C
    virtual DCar* Car(void);              // 0x20
    virtual void v24(void);               // 0x24
    virtual void v28(void* a, void* b, s32* out);  // 0x28
    virtual void v2C(void* a, void* b, s32* out);  // 0x2C
    virtual void Hold(int who);           // 0x30
    virtual void Release(void);           // 0x34
    virtual u8 Held(void);                // 0x38
    virtual ~DeityCar(void);              // 0x3C
    virtual DState* State2(void);         // 0x40
    u8 state[0x38];        // offset 0x4
    LineFollower* line;    // offset 0x3C
    ILPos ideal;           // offset 0x40
    ILPos racing;          // offset 0x4C
    IdealFollower* follower;  // offset 0x58
    DCar* car;             // offset 0x5C
    s32 index;             // offset 0x60
    s32 stuck;             // offset 0x64
    s32 finishLap;         // offset 0x68
    s32 start;             // offset 0x6C
    f32 finishTime;        // offset 0x70
    s32 rank;              // offset 0x74
    u8 locked;             // offset 0x78
    u8 finished;           // offset 0x79
    s32 ai;                // offset 0x7C
};

#define DS(c) ((DState*)((u8*)(c)))

struct Crossing {
    s32 car;
    f32 time;
};

struct CrossQueue {
    Crossing e[8];  // offset 0x0
    s32 n;          // offset 0x40
};

struct TeleportInfo {
    u8 pad0[0x18];
    f32 dir[3];    // offset 0x18
    f32 pos[3];    // offset 0x24
    f32 vel[3];    // offset 0x30
    s32 a3C;       // offset 0x3C
    u8 pad40[0x5C - 0x40];
};

struct RaceInfo {
    s32 a0;
    u32 flags;  // offset 0x4
};

struct RaceSetup {
    s32 cars;  // offset 0x0
};

ModuleTag lbl_8055E660;
UnkStatic7F lbl_8055E668;
void* lbl_8055E66C;  // bad-teleport regions
void* lbl_8055E670;  // bridge-teleport regions
u8 lbl_8055E674;
s32 lbl_8055E678;    // cars
s32 lbl_8055E67C;    // player cars
s32 lbl_8055E680;    // laps
DeityCar* lbl_8055E684;
s32 lbl_8055E688;    // centre-line points
DAI* lbl_8055E68C;
u8 lbl_8055E690;
u8 lbl_8055E691;
u8 lbl_8055E692;
s32 lbl_8055E694;    // leader lap
u8 lbl_8055E698;
s32 lbl_8055E69C;    // player cars finished
u8 lbl_8055E6A0;     // countdown set
f32 lbl_8055E6A4;    // green flag time
s32 lbl_8055E6A8;
f32 lbl_8055E6AC;
CrossQueue lbl_80435BE8;
s32 lbl_80435C30[8];  // cars by place

extern "C" {
int fn_802B04D4(void* pos);
int fn_80139EC4(void* regions, f32 x, f32 z);
void* fn_80139D4C(const char* name);
void fn_80139E18(void* regions);
void fn_801B8040(TeleportInfo* t, ILPos* at, f32 a);
int fn_801784DC(f32* pos, f32* h, int a, int b, int c);
void fn_8018A08C(DCar* car);
void fn_8018AF04(DCar* car);
void fn_8018B0D0(DCar* car, int mode);
RaceInfo* fn_801769CC(void);
RaceSetup* fn_801769D8(void);
int fn_801769F8(void);
int fn_80179630(int view);  // car of a local player
int fn_800081F0(RaceInfo* r);
DMaster* fn_801B83A0(void);
DAI* fn_8013C9F4(int a);
void fn_80204A1C(const char* key, u8* out);
PathSet* fn_80156A40(void);
void* fn_801352B8(void* mem);
int fn_801B8928(int car, TeleportInfo* t, int a);
int fn_801B526C(void);
int fn_8026A344(void);
void fn_8026A248(int car, int slot);
int fn_8026A27C(int slot);
int fn_801366C0(IdealFollower* f, void* p, f32 k);
void fn_8000D7F8(int car, f32 t);
void fn_801CB55C(int car, int place);
void fn_801CB360(int car);
void fn_801CB3AC(int car);
void fn_801CB6B4(int car, int a, int b);
void fn_80178C9C(void);
int fn_801E89EC(void);
void fn_801E89F4(int a);
}  // extern "C"
int IsLoadingActive(void);
extern "C" {
void fn_801B84F8(void);
f32 fn_801365C4(IdealFollower* a, IdealFollower* b);

int fn_801B2F40(int place);
int fn_801B0DF0(int car);
void fn_801B30D0(int car);
void fn_801B30E0(int car);

int fn_801B06D8(const V2* pos, const V2* dir) {
    for (f32 s = 0.0f; s < 33.333332f; s += 6.0f) {
        V2 p;
        p.x = pos->x + dir->x * s;
        p.y = pos->y + dir->y * s;
        f32 v[3];
        v[0] = p.x;
        v[1] = 0.0f;
        v[2] = p.y;
        if (fn_802B04D4(v)) {
            return 1;
        }
        if (lbl_8055E66C && fn_80139EC4(lbl_8055E66C, p.x, p.y)) {
            return 1;
        }
    }
    return 0;
}

void fn_801B07B4(TeleportInfo* t, const ILPos* at) {
    ILPos p = *at;
    while (fn_801B06D8(&p.seg->pos, &p.seg->dir)) {
        p.seg = p.seg->next;
    }
    fn_801B8040(t, &p, 0.0f);
    t->a3C = 0;
    t->vel[0] = t->dir[0] * 26.666666f;
    t->vel[1] = t->dir[1] * 26.666666f;
    t->vel[2] = t->dir[2] * 26.666666f;
    f32 v[3];
    v[0] = t->pos[0];
    v[1] = t->pos[1];
    v[2] = t->pos[2];
    if (lbl_8055E670 && fn_80139EC4(lbl_8055E670, v[0], v[2])) {
        v[1] += 200.0f;
        f32 h;
        if (fn_801784DC(v, &h, 0, 0, 0)) {
            t->pos[1] = 0.6286f + h;
            t->pos[1] += 7.625f;
            t->vel[1] = -4.905f;
        }
    }
}

u32 fn_801B08E8(u32 a, u32 b, u32 n) {
    if (b < a) {
        b += n;
    }
    return b - a;
}

void fn_801B08FC(DeityCar* c, TeleportInfo* t) {
    IdealPoint* from = c->Ideal()->il.seg;
    c->Car()->Teleport(t);
    fn_8018A08C(c->Car());
    IdealPoint* to = c->Ideal()->il.seg;
    s32 idx = c->index;
    u32 n1 = c->Ideal()->line->total;
    u32 n2 = c->Ideal()->line->total;
    u32 fwd = fn_801B08E8(from->ilpos, to->ilpos, n1);
    u32 back = fn_801B08E8(to->ilpos, from->ilpos, n2);
    if (back < fwd) {
        for (IdealPoint* p = to->next; p != from->next; p = p->next) {
            if (p->ilpos == 0) {
                DState* s = c->State();
                s32 l = DS(c)->lap - 1;
                *(s32*)((u8*)c + 0x14) = l > 0 ? l : 0;
                break;
            }
        }
    } else {
        for (IdealPoint* p = from->next; p != to->next; p = p->next) {
            if (p->ilpos == 0) {
                f32 tm = lbl_8055E3C8 - 0.016683417f * (f32)(idx + 1) * 0.0625f;
                CrossQueue* q = &lbl_80435BE8;
                if ((u32)q->n < 8) {
                    Crossing e;
                    e.car = idx;
                    e.time = tm;
                    int i;
                    for (i = 0; i < q->n; i++) {
                        if (q->e[i].time > tm) {
                            break;
                        }
                    }
                    memmove(&q->e[i + 1], &q->e[i], (q->n - i) * 8);
                    q->e[i] = e;
                    q->n++;
                }
                break;
            }
        }
    }
    if (!(fn_801769CC()->flags & 0x4000)) {
        fn_8018AF04(c->Car());
    }
    c->Release();
    c->stuck = 0;
    *(u32*)((u8*)c + 0x4) &= ~4;
}

}  // extern "C"

void DeityCar::Release(void) {
    *(u8*)((u8*)this + 0x2C) = 0;
    *(f32*)((u8*)this + 0x30) = -1.0f;
}

DCar* DeityCar::Car(void) { return car; }

IdealFollower* DeityCar::Ideal(void) { return follower; }

extern "C" {

struct LinePlace {
    IdealPoint* seg;
    f32 t;
    s32 mode;
};

void fn_801B0BB0(LinePlace* out, const LineFollower* f) {
    IdealPoint* seg = f->il.seg;
    out->seg = seg;
    out->t = f->il.t;
    out->mode = f->il.mode;
    f32 lim = f->line->points->dist - 100.0f;
    if (seg->dist > lim) {
        fn_802E30C8("deity: would have teleported too close to sfline, rewinding");
        IdealPoint* p = f->line->points;
        IdealPoint* found = 0;
        int i = 0;
        while (p->dist < lim) {
            found = p;
            if (++i > 3000) {
                found = 0;
                break;
            }
            p = p->next;
        }
        if (found) {
            out->seg = found;
        }
    }
}

void fn_801B0C78(int i) {
    DeityCar* c = &lbl_8055E684[i];
    TeleportInfo t;
    *(s16*)((u8*)&t + 0x40) = -1;
    int isPlayer = c->Car()->type == 'PCAR';
    if (isPlayer) {
        fn_801B30D0(i);
        fn_801B30E0(i);
    }
    if (!fn_801B8928(i, &t, 0)) {
        if (!lbl_8055E68C || !lbl_8055E68C->v28(&t, i, lbl_8055E684->ai, 0)) {
            LinePlace p;
            p.seg = 0;
            p.t = 0.0f;
            p.mode = 10000;
            if (isPlayer) {
                LinePlace q;
                fn_801B0BB0(&q, c->Line());
                p = q;
            } else {
                p.seg = c->racing.seg;
                p.t = c->racing.t;
                p.mode = c->racing.mode;
            }
            fn_801B07B4(&t, (ILPos*)&p);
        }
    }
    fn_801B08FC(c, &t);
}

}  // extern "C"

LineFollower* DeityCar::Line(void) { return line; }

DState* DeityCar::State(void) { return (DState*)((u8*)this + 0x4); }

extern "C" {

int fn_801B0DF0(int i) { return lbl_8055E684[i].Car()->type == 'PCAR'; }

int fn_801B0E40(int i, s32* place) {
    int p = lbl_8055E684[i].State()->place;
    int k;
    for (k = p; k <= lbl_8055E678; k++) {
        if (fn_801B0DF0(fn_801B2F40(k - 1))) {
            return 0;
        }
    }
    int lo = *place > 1 ? *place : 1;
    for (k = p - 1; k >= lo; k--) {
        if (fn_801B0DF0(fn_801B2F40(k - 1))) {
            *place = k + 1;
            return 1;
        }
    }
    return 1;
}

IdealPoint* fn_801B0F18(int view) { return lbl_8055E684[fn_80179630(view)].racing.seg; }

int fn_801B0F48(u32 a, u32 b) {
    u32 d = b - a;
    if (a > b) {
        d = a - b;
    }
    return d < 0x20000;
}

void fn_801B0F70(int i, s32 place) {
    DeityCar* c = &lbl_8055E684[i];
    DCar* car = c->Car();
    int isPlayer = car->type == 'PCAR';
    LinePlace p;
    p.seg = 0;
    p.t = 0.0f;
    p.mode = 10000;
    if (isPlayer) {
        fn_801B30D0(i);
        fn_801B30E0(i);
        LinePlace q;
        fn_801B0BB0(&q, c->Line());
        p = q;
    } else {
        p.seg = c->racing.seg;
        p.t = c->racing.t;
        p.mode = c->racing.mode;
    }
    int quick = 0;
    if (!isPlayer) {
        if (fn_801B0E40(i, &place)) {
            int near = 0;
            u32 il = fn_801B0F18(0)->ilpos;
            IdealPoint* s = p.seg;
            while (fn_801B06D8(&s->pos, &s->dir)) {
                if (fn_801B0F48(s->ilpos, il)) {
                    near = 1;
                    break;
                }
                s = s->next;
            }
            p.seg = s;
            if (near) {
                fn_802E30C8("deity: bad-teleport AI is quick-repairing, not teleporting. (%d@%d)", i,
                            lbl_8055E3C4);
                quick = 1;
            }
        } else {
            quick = 1;
        }
    }
    if (quick) {
        fn_8018AF04(car);
    } else {
        TeleportInfo t;
        *(s16*)((u8*)&t + 0x40) = -1;
        fn_801B07B4(&t, (ILPos*)&p);
        fn_801B08FC(c, &t);
    }
}

int fn_801B1130(int i) {
    int ok = 1;
    int busy = 0;
    if (lbl_8055E68C && lbl_8055E68C->v14(i, lbl_8055E684[i].ai)) {
        busy = 1;
    }
    if (!busy) {
        int r = 0;
        if (!lbl_8055E68C && lbl_8055E690) {
            r = 1;
        }
        if (!r) {
            ok = 0;
        }
    }
    return ok;
}

int fn_801B11DC(const void* pa, const void* pb) {
    DeityCar* a = &lbl_8055E684[*(const s32*)pa];
    DeityCar* b = &lbl_8055E684[*(const s32*)pb];
    int aFin = a->finishLap == lbl_8055E680 + 1;
    int bFin = b->finishLap == lbl_8055E680 + 1;
    u32 ap = ((DState*)a->State2())->place;
    u32 bp = ((DState*)b->State2())->place;
    s32 ar = a->rank;
    s32 br = b->rank;
    if (aFin && bFin) {
        if ((f32)__fabs(a->finishTime - b->finishTime) <= lbl_80558DC4[0]) {
            return 0;
        }
        if (b->finishTime < a->finishTime) {
            return 1;
        }
        return -1;
    }
    if (aFin || bFin) {
        return aFin ? -1 : 1;
    }
    if (ap != bp) {
        return bp - ap;
    }
    return br - ar;
}

}  // extern "C"

DState* DeityCar::State2(void) { return (DState*)((u8*)this + 0x4); }

extern "C" {

void fn_801B1318(void) {
    qsort(lbl_80435C30, lbl_8055E678, 4, fn_801B11DC);
    for (int i = 0; i < lbl_8055E678; i++) {
        lbl_8055E684[lbl_80435C30[i]].State()->place = i + 1;
    }
}

void fn_801B1390(int slot, int car) {
    int base = fn_8026A344();
    if (fn_801B526C()) {
        fn_8026A248((lbl_8055E678 + (car + base)) % lbl_8055E678, slot);
    } else {
        int k = 0;
        if (base >= 0 && base < lbl_8055E678) {
            k = lbl_8055E684[base].State()->place - 1;
        }
        fn_8026A248(lbl_80435C30[(lbl_8055E678 + (car + k)) % lbl_8055E678], slot);
    }
}

void fn_801B146C(void) {
    for (int i = 0; i < 4; i++) {
        int c = fn_8026A27C(i);
        if (c) {
            fn_801B1390(i, c);
        }
    }
}

void fn_801B14BC(u32* flags, u32 bit, int on) {
    if (on) {
        *flags |= bit;
    } else {
        *flags &= ~bit;
    }
}

void fn_801B14E4(DeityCar* c) {
    if (fn_801366C0(c->follower, (u8*)c->car + 0x118, 4.444444f)) {
        c->stuck++;
    } else {
        c->stuck = 0;
    }
    fn_801B14BC((u32*)((u8*)c + 0x4), 4, (f32)c->stuck > 29.97f);
}

}  // extern "C"

void DeityCar::Hold(int who) {
    *(u8*)((u8*)this + 0x2C) = 1;
    *(f32*)((u8*)this + 0x30) = lbl_8055E3C8;
    if (lbl_8055E690) {
        fn_801CB6B4(index, who, 2);
    }
}

extern "C" {

void fn_801B15A8(DeityCar* c, f32 t) {
    if (!c->locked && !c->finished) {
        s32 lap = *(s32*)((u8*)c + 0x14);
        if (lap == c->finishLap) {
            if (lap > 0) {
                f32 dt = t - c->finishTime;
                f32 best = *(f32*)((u8*)c + 0x1C);
                *(f32*)((u8*)c + 0x20) = dt;
                if (best > 0.1f) {
                    *(f32*)((u8*)c + 0x1C) = best < dt ? best : dt;
                } else {
                    *(f32*)((u8*)c + 0x1C) = dt;
                }
                c->finishTime = t;
                c->car->Lap();
                fn_8000D7F8(c->index, dt);
            }
            s32 l = *(s32*)((u8*)c + 0x14) + 1;
            *(s32*)((u8*)c + 0x14) = l;
            if (l > lbl_8055E694) {
                lbl_8055E694 = l;
                if (l == lbl_8055E680 + 1) {
                    lbl_8055E698 = 1;
                }
            }
            s32 nl = *(s32*)((u8*)c + 0x14);
            c->finishLap = nl;
            if (nl == lbl_8055E680 + 1) {
                fn_802E30C8("deity: car %d finishes at %1.2f on lap %d", c->index, t, nl);
                c->finished = 1;
                *(s32*)((u8*)c + 0x28) = 3;
                fn_8018B0D0(c->car, 3);
                fn_801B83A0()->Finished(c->car, *(s32*)((u8*)c + 0x8));
                fn_801CB55C(c->index, *(s32*)((u8*)c + 0x8));
                fn_801CB360(c->index);
                if (c->car->type == 'PCAR') {
                    if (++lbl_8055E69C == lbl_8055E67C) {
                        fn_802E30C8("deity: locking current finish order");
                        for (int i = 0; i < lbl_8055E678; i++) {
                            lbl_8055E684[i].locked = 1;
                        }
                    }
                }
            }
        } else {
            *(s32*)((u8*)c + 0x14) = lap + 1;
        }
    }
}

void fn_801B179C(DeityCar* c, int index, DCar* car) {
    c->index = index;
    c->car = car;
    c->start = index;
    *(s32*)((u8*)c + 0x8) = index + 1;
    PathSet* ps = fn_80156A40();
    void* mem = operator new(0x68, "Deity:center_line");
    if (mem) {
        mem = new (mem) IdealFollower(1, ps->Center(index));
    }
    c->follower = (IdealFollower*)mem;
    void* m2 = operator new(0x34, "Deity:racing_line");
    if (m2) {
        m2 = fn_801352B8(m2);
    }
    c->line = (LineFollower*)m2;
    c->line->x30 = c->follower;
    c->line->SetLine(ps->Racing(index));
    if (car->type == 'PCAR') {
        lbl_8055E67C++;
    }
}

void fn_801B18B8(DeityCar* c, IdealPoint* start, int reset) {
    DCar* car = c->car;
    V2 pos;
    f32 k = car->ahead;
    pos.x = car->pos[0] + car->fwd[0] * k;
    pos.y = car->pos[2] + car->fwd[2] * k;
    V2 dir;
    dir.x = c->car->fwd[0];
    dir.y = c->car->fwd[2];
    c->follower->Start(&pos, &dir, reset, start);
    c->line->SetPosition(&pos, &dir, 0);
}

void fn_801B196C(DeityCar* c) {
    DCar* car = c->car;
    fn_80156A40()->Locate(car->il, (u8*)car + 0x60);
    DCar* k = c->car;
    f32 a = k->ahead;
    V2 p;
    p.x = k->pos[0] + k->fwd[0] * a;
    p.y = k->pos[2] + k->fwd[2] * a;
    c->line->Attach(fn_80156A40()->Racing(c->index), &p, 0);
    LineFollower* l = c->line;
    c->racing.seg = l->il.seg;
    c->racing.t = l->il.t;
    c->racing.mode = l->il.mode;
}

void fn_801B1A5C(DeityCar* c) {
    PathSet* ps = fn_80156A40();
    c->follower->SetLine(ps->Center(c->index));
    c->line->SetLine(ps->Racing(c->index));
    c->follower->Reset();
    IdealFollower* f = c->follower;
    LineFollower* l = c->line;
    c->ideal.seg = f->il.seg;
    c->ideal.t = f->il.t;
    c->ideal.mode = f->il.mode;
    c->racing.seg = l->il.seg;
    c->racing.t = l->il.t;
    c->racing.mode = l->il.mode;
}

}  // extern "C"

// Per-car update (placeholder).
void DeityCar::Update(void) {}

void DeityCar::v28(void* a, void* b, s32* out) {
    *out = 0;
    if (lbl_8055E68C) {
        lbl_8055E68C->v0C(a, b, index, &ai, out);
    }
}

void DeityCar::v2C(void* a, void* b, s32* out) {
    *out = 0;
    if (lbl_8055E68C) {
        lbl_8055E68C->v10(a, b, index, &ai, out);
    }
}

extern "C" {

void fn_801B21B8(DeityCar* c);
void fn_801B2118(DeityCar* c, short del);

void fn_801B2024(RaceInfo* r) {
    lbl_8055E66C = fn_80139D4C("bad_teleport.val");
    lbl_8055E670 = fn_80139D4C("bridge_teleport.val");
    fn_80204A1C("Unlimited Laps", &lbl_8055E692);
    u32 f = r->flags;
    lbl_8055E690 = (f >> 8) & 1;
    lbl_8055E691 = (f >> 9) & 1;
    s32 n = fn_801769D8()->cars;
    lbl_8055E678 = n;
    lbl_8055E67C = 0;
    lbl_8055E684 = new ("proxy_list") DeityCar[n];
    lbl_8055E680 = fn_800081F0(fn_801769CC());
    lbl_8055E68C = fn_801B83A0()->GetAI();
    if (!lbl_8055E68C) {
        lbl_8055E68C = fn_8013C9F4(0);
    }
    lbl_8055E674 = 0;
}

}  // extern "C"

DeityCar::~DeityCar(void) {
    delete follower;
    delete line;
}

DeityCar::DeityCar(void) {
    ideal.seg = 0;
    ideal.mode = 10000;
    ideal.t = 0.0f;
    racing.seg = 0;
    racing.mode = 10000;
    racing.t = 0.0f;
    memset(state, 0, sizeof(state));
    car = 0;
    finishTime = 0.0f;
    index = -1;
    start = -1;
    follower = 0;
    line = 0;
}

struct DeityBlob {
    virtual ~DeityBlob(void) {}
};

extern "C" {

void fn_801B228C(void) {
    delete[] lbl_8055E684;
    lbl_8055E684 = 0;
    lbl_8055E678 = 0;
    if (lbl_8055E68C) {
        delete (DeityBlob*)lbl_8055E68C;
        lbl_8055E68C = 0;
    }
    if (lbl_8055E66C) {
        fn_80139E18(lbl_8055E66C);
        lbl_8055E66C = 0;
    }
    if (lbl_8055E670) {
        fn_80139E18(lbl_8055E670);
        lbl_8055E670 = 0;
    }
}

void fn_801B2320(void) {
    struct T {
        u8 pad[0x2C];
        CenterLine* line;
    };
    T* t = (T*)((void* (*)(void))fn_80178C9C)();
    lbl_8055E688 = t->line->total;
}

void fn_801B234C(f32 t) {
    lbl_8055E6A4 = t;
    lbl_8055E6A0 = 1;
}

void fn_801B235C(void) { fn_801E89F4(3); }

// Hand a car back to the AI and its HUD slot.
static inline void ReleaseCar(DeityCar* c) {
    fn_8018B0D0(c->car, 2);
    fn_801CB3AC(c->index);
}

extern const f32 lbl_805625DC;  // 1 / 59.94 (one frame)

void fn_801B2364(void) {
    fn_801B146C();
    if (fn_801769F8()) {
        return;
    }
    int r = fn_801E89EC();
    f32 now = lbl_8055E3C8;
    if (!lbl_8055E6A0) {
        return;
    }
    if (!(now >= lbl_8055E6A4)) {
        return;
    }
    if (!r) {
        fn_801E89F4(1);
        for (int i = 0; i < lbl_8055E678; i++) {
            ReleaseCar(&lbl_8055E684[i]);
        }
    }
    if (lbl_8055E6A8 == 1 && IsLoadingActive()) {
        lbl_8055E6AC += lbl_805625DC;
    }
    if (lbl_8055E6A8 == 1 && now > lbl_8055E6AC) {
        fn_802E30C8("deity: green flag!");
        lbl_8055E6A8 = 0;
        for (int i = 0; i < lbl_8055E678; i++) {
            *(s32*)((u8*)&lbl_8055E684[i] + 0x28) = lbl_8055E6A8;
        }
        if (lbl_8055E691) {
            fn_801B84F8();
        }
    }
}

void fn_801B24B8(void) {
    if (fn_801769F8()) {
        return;
    }
    CrossQueue* q = &lbl_80435BE8;
    Crossing* e = q->e;
    for (int i = 0; i < q->n; i++, e++) {
        DeityCar* c = &lbl_8055E684[e->car];
        if (!c->locked) {
            c->rank = lbl_8055E688 - i + 9;
        }
    }
    fn_801B1318();
    e = lbl_80435BE8.e;
    for (int i = 0; i < lbl_80435BE8.n; i++, e++) {
        fn_801B15A8(&lbl_8055E684[e->car], e->time);
    }
    lbl_80435BE8.n = 0;
    if (lbl_8055E68C) {
        lbl_8055E68C->v20();
    }
}

int fn_801B25B0(const s32* pa, const s32* pb) {
    DeityCar* ca = &lbl_8055E684[*pa];
    DeityCar* cb = &lbl_8055E684[*pb];
    f32 a = ca->State()->qual;
    f32 b = cb->State()->qual;
    if (a < 1.0f) {
        a = 100000.0f;
    }
    if (b < 1.0f) {
        b = 100000.0f;
    }
    if (a < b) {
        return -1;
    }
    return a > b;
}

}  // extern "C"

int OptDirectToTrack(void);

extern "C" {
f32 fn_801B88BC(void);
f32 fn_8013B650(s32 car);  // AI qualifying time
int fn_801B25B0(const s32* pa, const s32* pb);

static inline int NearZero(f32 x) { return (f32)__fabs(x) <= lbl_80558DC4[0]; }

// Race start: the intro timer (short in test mode), cleared race counters, every car on its line, and the
// starting order from the qualifying times.
void fn_801B265C(void) {
    static u8 warned;
    s32 order[8];
    lbl_8055E6A8 = 1;
    if ((((u32*)fn_801769CC())[4] & 1) || OptDirectToTrack()) {
        if (!warned) {
            warned = 1;
            fn_802E30C8("%s", "deity: using 'test mode' quick intro duration");
        }
        lbl_8055E6AC = lbl_8055E3C8 + 0.5f;
    } else {
        f32 now = lbl_8055E3C8;
        lbl_8055E6AC = now + fn_801B88BC();
    }
    lbl_8055E698 = 0;
    lbl_8055E694 = 0;
    lbl_8055E6A4 = 0.0f;
    lbl_8055E69C = 0;
    lbl_8055E6A0 = 0;
    memset(lbl_80435C30, -1, sizeof(lbl_80435C30));
    fn_80156A40()->Reset();
    for (int i = 0; i < lbl_8055E678; i++) {
        fn_801B1A5C(&lbl_8055E684[i]);
    }
    for (int i = 0; i < lbl_8055E678; i++) {
        f32 v = -1.0f;
        DeityCar* c = &lbl_8055E684[i];
        if (*(s32*)((u8*)c->car + 0x1F0) == 0x51) {
            v = fn_8013B650(c->index);
        }
        f32 q = *(f32*)((u8*)c->car + 0x254);
        if (!NearZero(q)) {
            v = q;
        }
        DS((u8*)c + 0x4)->qual = v;
    }
    for (int i = 0; i < lbl_8055E678; i++) {
        order[i] = i;
    }
    qsort(order, lbl_8055E678, 4, (int (*)(const void*, const void*))fn_801B25B0);
    for (int i = 0; i < lbl_8055E678; i++) {
        lbl_8055E684[order[i]].start = i;
    }
}

void fn_801B2910(void) {}

}  // extern "C"

void DeityCar::Reset(void) {
    memset(state, 0, sizeof(state));
    *(s32*)((u8*)this + 0x8) = start + 1;
    finishLap = 0;
    finishTime = 0.0f;
    rank = 0;
    locked = 0;
    finished = 0;
    ai = 0;
    stuck = 0;
    fn_8018B0D0(car, 0);
    fn_801CB360(index);
    *(s32*)((u8*)this + 0x28) = 1;
}

extern "C" {

void fn_801B2C6C(void) {}

struct DeitySnap {
    s32 cars;     // offset 0x0
    s32 players;  // offset 0x4
    u8 pad8[4];
    u8 bC;
    u8 leaderDone;  // offset 0xD
    u8 allDone;     // offset 0xE
    u8 padF;
    struct {
        s32 a0;
        s32 place;
        s32 a8;
        s32 aC;
        u8 b10;
        u8 pad11[3];
    } car[8];  // offset 0x10
};

int fn_801B2C70(DeitySnap* s) {
    memset(s, 0, sizeof(DeitySnap));
    s->cars = lbl_8055E678;
    s->players = lbl_8055E67C;
    s->bC = 0;
    s->leaderDone = lbl_8055E698;
    s->allDone = lbl_8055E69C >= lbl_8055E67C;
    for (int i = 0; i < lbl_8055E678; i++) {
        s->car[i].place = *(s32*)((u8*)&lbl_8055E684[i] + 0x8);
        s->car[i].a8 = 0;
        s->car[i].b10 = 0;
    }
    return 1;
}

s32 fn_801B307C(void);
f32 fn_801B306C(void);

struct DeityStatus {
    u8 pad0[0x39C];
    s32 countdown;  // offset 0x39C
    u8 cars[8][0x38];  // offset 0x3A0
    s32 leaderLap;  // offset 0x560
    s32 first;      // offset 0x564
    s32 laps;       // offset 0x568
    f32 green;      // offset 0x56C
    f32 left;       // offset 0x570
    s32 a574;       // offset 0x574
    s32 a578;       // offset 0x578
};

void fn_801B2D28(DeityStatus* s) {
    memset(&s->leaderLap, 0, 0x1C);
    s->leaderLap = fn_801B307C();
    s->first = fn_801B2F40(0);
    if (lbl_8055E691) {
        s32 m = lbl_8055E6A8;
        s->a574 = 0;
        s->a578 = 0;
        s->green = lbl_8055E6A4;
        s->left = fn_801B306C();
        (void)m;
    }
    s->laps = lbl_8055E680;
    for (int i = 0; i < lbl_8055E678; i++) {
        u32* src = (u32*)lbl_8055E684[i].State();
        u32* dst = (u32*)s->cars[i];
        for (int k = 0; k < 14; k++) {
            dst[k] = src[k];
        }
    }
    if (fn_801E89EC()) {
        s->countdown = 0;
    } else if (lbl_8055E6A0) {
        s->countdown = (int)(lbl_8055E6A4 - lbl_8055E3C8) + 1;
    } else {
        s->countdown = -1;
    }
}

DeityCar* fn_801B2EC0(int i) { return &lbl_8055E684[i]; }

DeityCar* fn_801B2ED0(int i) { return fn_801B2EC0(i); }

s32 fn_801B2ED4(void) { return lbl_8055E680; }

DeityCar* fn_801B2EDC(int i, DCar* car) {
    DeityCar* c = &lbl_8055E684[i];
    fn_801B179C(c, i, car);
    return c;
}

s32 fn_801B2F24(void) { return lbl_8055E678; }

u8 fn_801B2F2C(int i) { return lbl_8055E684[i].finished; }

int fn_801B2F40(int place) { return lbl_80435C30[place]; }

u32 fn_801B2F54(int i) { return *(u32*)((u8*)lbl_8055E684[i].State() + 0x10); }

f32 fn_801B2F90(int a, int b) {
    DeityCar* ca = &lbl_8055E684[a];
    DeityCar* cb = &lbl_8055E684[b];
    DeityCar* hi;
    DeityCar* lo;
    if (*(s32*)((u8*)ca + 0x8) - 1 < *(s32*)((u8*)cb + 0x8) - 1) {
        hi = ca;
        lo = cb;
    } else {
        hi = cb;
        lo = ca;
    }
    s32 d = *(s32*)((u8*)hi + 0x14) - *(s32*)((u8*)lo + 0x14) - (lo->rank < hi->rank);
    if (d != 0) {
        return 500.0f * (f32)d;
    }
    return __fabs(fn_801365C4(lo->follower, hi->follower));
}

s32 fn_801B3050(int i) { return lbl_8055E684[i].start; }

f32 fn_801B3064(void) { return lbl_8055E6A4; }

f32 fn_801B306C(void) {
    f32 t = lbl_8055E3C8;
    return lbl_8055E6AC - t;
}

s32 fn_801B307C(void) { return lbl_8055E694; }

void fn_801B3084(DCar* a, DCar* b, int c, int d) {
    if (lbl_8055E68C) {
        lbl_8055E68C->v18(a->il, b->il, c, d);
    }
}

void fn_801B30C0(int i, IdealPoint* start, int reset) { fn_801B18B8(&lbl_8055E684[i], start, reset); }

void fn_801B30D0(int i) { fn_801B196C(&lbl_8055E684[i]); }

void fn_801B30E0(int i) {
    DeityCar* c = &lbl_8055E684[i];
    LineFollower* l = c->Line();
    IdealPoint* best = 0;
    if (c->ideal.seg) {
        IdealPoint* first = l->line->points;
        u32 want = c->ideal.seg->ilpos;
        s32 bestD = 0x7FFFFFFF;
        IdealPoint* p = first;
        do {
            s32 d = p->ilpos - want;
            if (d < 0) {
                d = -d;
            }
            if (d < bestD) {
                bestD = d;
                best = p;
            }
            p = p->next;
        } while (p != first);
    }
    DCar* car = c->Car();
    f32 k = car->ahead;
    V2 pos;
    pos.x = car->pos[0] + car->fwd[0] * k;
    pos.y = car->pos[2] + car->fwd[2] * k;
    l->SetPosition(&pos, 0, best);
}

void fn_801B3208(int i, s32 start, f32 qual) {
    fn_802E317C("+DeityStuffQualifyInfo(%d, %d, %f)", i, start, qual);
    lbl_8055E684[i].start = start;
    *(f32*)((u8*)&lbl_8055E684[i] + 0x24) = qual;
}

int fn_801B327C(void) { return 0; }

void fn_801B3284(void) {}

}  // extern "C"

u8 DeityCar::Held(void) { return *(u8*)((u8*)this + 0x2C); }

int DeityCar::v14(void) { return 1; }

void DeityCar::v24(void) {}
