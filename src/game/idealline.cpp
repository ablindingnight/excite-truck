// Ideal-line followers (0x80135278-0x801368F8): track a position along the track's center line ("ilpos"
// = segment + fraction, or 16.16 fixed point), project world positions onto it, step forward/backward
// across segments, look ahead, and keep per-segment pass times to compute gaps between cars. Names are
// guesses apart from CenterLine::convert_meters_to_ilpos (from its message).
#include "game/common.h"

extern "C" {
double sqrt(double x);
double fmod(double x, double y);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_801564B0(void);
void fn_801569FC(void);
}

void* operator new[](size_t size, const char* tag);

extern f32 lbl_8055E3C8;  // current race time

struct V3 {
    f32 x;
    f32 y;
    f32 z;
};

struct V2 {
    V2(void) {}
    V2(f32 ax, f32 ay) {
        x = ax;
        y = ay;
    }
    f32 x;
    f32 y;
};

static inline V2 Sub(const V2& a, const V2& b) { return V2(a.x - b.x, a.y - b.y); }

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

struct IdealPoint {
    V2 pos;            // offset 0x0
    V2 dir;            // offset 0x8
    u32 ilpos;         // offset 0x10 (16.16)
    IdealPoint* next;  // offset 0x14
    f32 len;           // offset 0x18
    f32 dist;          // offset 0x1C
    f32 speed;         // offset 0x20
    u8 pad24[4];
    f32 width;         // offset 0x28
    u8 pad2C[8];
    u8 type;           // offset 0x34
    u8 section;        // offset 0x35
    u8 pad36[6];
};

struct CenterLine {
    u8 pad0[8];
    f32 len[4];          // offset 0x8 (per point type)
    u32 total;           // offset 0x18 (16.16)
    IdealPoint* points;  // offset 0x1C
    IdealPoint* start;   // offset 0x20
};

class PathSet {
public:
    virtual CenterLine* GetLine(int index);  // offset 0x8
};

extern "C" PathSet* fn_80156A40(void);

struct ILPos {
    ILPos(void) {
        seg = 0;
        mode = 10000;
        t = 0.0f;
    }
    ILPos(IdealPoint* s, f32 f, s32 m) {
        seg = s;
        t = f;
        mode = m;
    }
    IdealPoint* seg;  // offset 0x0
    f32 t;            // offset 0x4
    s32 mode;         // offset 0x8
};

class LineFollower {
public:
    LineFollower(void);
    virtual ~LineFollower(void);
    virtual int SetLine(CenterLine* line);
    virtual void Attach(CenterLine* line, V2* pos, V2* dir);

    void Clear(void);
    void Init(void);
    int Update(V2* pos);
    void SetPosition(V2* pos, V2* dir, IdealPoint* start);
    void Locate(V2* pos, V2* dir, ILPos* out, int unused);
    int Step(V2* pos);

    ILPos il;      // offset 0x4
    V2 dir;        // offset 0x10
    V2 off;        // offset 0x18
    V2 pos;        // offset 0x20
    u8 wide;       // offset 0x28
    CenterLine* line;  // offset 0x2C
    s32 unk30;     // offset 0x30
};

struct TimeArray {
    TimeArray(void) {
        cap = 0;
        n = 0;
        data = 0;
    }
    ~TimeArray(void) {
        if (data) {
            delete[] data;
        }
        data = 0;
    }
    void Init(s32 count, s32 extra);

    f32* data;  // offset 0x0
    s32 n;      // offset 0x4
    s32 cap;    // offset 0x8
};

class IdealFollower : public LineFollower {
public:
    IdealFollower(int withTimes, CenterLine* line);
    virtual ~IdealFollower(void);
    virtual int SetLine(CenterLine* line);
    virtual void Attach(CenterLine* line, V2* pos, V2* dir);

    void Reset(void);
    void Start(V2* pos, V2* dir, int resetTimes, IdealPoint* start);
    void ClearFlag(void);
    int Track(V2* pos, f32* progress);
    void UpdatePerp(void);

    u32 Rel(u32 x) {
        u32 b = base;
        if (b) {
            return (x - b + line->total) % line->total;
        }
        return x;
    }

    u32 base;       // offset 0x34
    V2 perp;        // offset 0x38
    f32 lateral;    // offset 0x40
    f32 latSpeed;   // offset 0x44
    f32 travel;     // offset 0x48
    f32 speed;      // offset 0x4C
    TimeArray times;  // offset 0x50
    IdealPoint* last;  // offset 0x5C
    IdealPoint* prev;  // offset 0x60
    u8 flag;        // offset 0x64
};

ModuleTag lbl_8055E0B8;

extern "C" {

static inline f32 Dot4(f32 ax, f32 ay, f32 bx, f32 by) { return ax * bx + ay * by; }
static inline f32 Sq(f32 x) { return x * x; }
static inline f32 Sum(f32 a, f32 b) { return a + b; }
static inline f32 Len2(f32 x, f32 y) { return x * x + y * y; }
f32 fn_80135278(const V2* dir, const V2* p, const V2* origin) {
    V2 d;
    d.x = p->x - origin->x;
    d.y = p->y - origin->y;
    return Dot4(dir->x, dir->y, d.x, d.y);
}

}  // extern "C"

LineFollower::LineFollower(void) { Init(); }

LineFollower::~LineFollower(void) { line = 0; }

void LineFollower::Clear(void) {
    il.t = 0.0f;
    il.seg = 0;
    il.mode = 0;
    pos.x = 0.0f;
    pos.y = 0.0f;
    dir.x = 1.0f;
    dir.y = 0.0f;
    off.x = 0.0f;
    off.y = 0.0f;
    wide = 0;
    line = 0;
}

void LineFollower::Init(void) {
    Clear();
    unk30 = 0;
}

int LineFollower::Update(V2* p) { return Step(p); }

void LineFollower::SetPosition(V2* p, V2* d, IdealPoint* start) {
    il.seg = start;
    il.t = 0.0f;
    Locate(p, d, &il, 0);
    Step(p);
}

extern "C" {

IdealPoint* fn_80135420(V2* p, V2* d, CenterLine* line) {
    IdealPoint* found;
    f32 best = 1000.0f;
    IdealPoint* seg = line->points;
    found = 0;
    do {
        if (fn_80135278(&seg->dir, p, &seg->pos) > 0.0f && fn_80135278(&seg->next->dir, p, &seg->next->pos) < 0.0f) {
            V2 v;
            f32 x;
            v.x = p->x - seg->pos.x;
            v.y = p->y - seg->pos.y;
            x = __fabs(Dot4(seg->dir.y, -seg->dir.x, v.x, v.y));
            if (x < best) {
                found = seg;
                best = x;
            }
        }
        seg = seg->next;
    } while (seg != line->points);
    if (!found) {
        best = 4000000.0f;
        seg = line->points;
        do {
            V2 v;
            f32 a;
            v.x = p->x - seg->pos.x;
            v.y = p->y - seg->pos.y;
            a = Sum(Sq(fn_80135278(&seg->dir, p, &seg->pos)), Sq(Dot4(seg->dir.y, -seg->dir.x, v.x, v.y)));
            if (a < best) {
                found = seg;
                best = a;
            }
            seg = seg->next;
        } while (seg != line->points);
    }
    if (!found) {
        fn_802E30C8("ideal: Can't find a good pair for %.2f,%.2f", p->x, p->y);
        fn_802E30C8("ideal: Someone is probably going to be upset about this.");
    }
    return found;
}

int fn_801355B8(LineFollower* f, V2* p, IdealPoint** seg, f32* t) { return 0; }

}  // extern "C"

void LineFollower::Locate(V2* p, V2* d, ILPos* out, int unused) {
    out->mode = il.mode;
    if (fn_801355B8(this, p, &out->seg, &out->t)) {
        return;
    }
    if (!out->seg) {
        out->seg = fn_80135420(p, d, line);
        out->t = 0.0f;
    }
    if (out->seg) {
        int i;
        for (i = 0; i < 10; i++) {
            IdealPoint* s = out->seg;
            f32 t = out->t;
            IdealPoint* n = s->next;
            V2 pt;
            V2 dv;
            V2 v;
            f32 inv;
            f32 a;
            dv.x = s->dir.x + t * (n->dir.x - s->dir.x);
            dv.y = s->dir.y + t * (n->dir.y - s->dir.y);
            pt.x = s->pos.x + t * (n->pos.x - s->pos.x);
            pt.y = s->pos.y + t * (n->pos.y - s->pos.y);
            inv = 1.0f / (f32)sqrt(dv.x * dv.x + dv.y * dv.y);
            dv.x *= inv;
            dv.y *= inv;
            v.x = p->x - pt.x;
            v.y = p->y - pt.y;
            a = dv.x * v.x + dv.y * v.y;
            if ((f32)__fabs(a) < 0.0005f) {
                break;
            }
            if (a < 0.0f) {
                out->t = 0.0f;
                break;
            }
            while (a > out->seg->len) {
                a -= out->seg->len;
                out->seg = out->seg->next;
            }
            out->t = a / out->seg->len;
        }
    }
}

int LineFollower::Step(V2* p) {
    if (!fn_801355B8(this, p, &il.seg, &il.t)) {
        int r;
        IdealPoint* s = il.seg;
        f32 t = il.t;
        r = 2;
        f32 inv;
        f32 x;
        f32 a;
        pos.x = s->pos.x + t * (s->next->pos.x - s->pos.x);
        pos.y = s->pos.y + t * (s->next->pos.y - s->pos.y);
        dir.x = s->dir.x + t * (s->next->dir.x - s->dir.x);
        dir.y = s->dir.y + t * (s->next->dir.y - s->dir.y);
        inv = 1.0f / (f32)sqrt(dir.x * dir.x + dir.y * dir.y);
        off.x = p->x - pos.x;
        dir.x *= inv;
        off.y = p->y - pos.y;
        dir.y *= inv;
        x = __fabs(off.y * dir.x + -off.x * dir.y);
        wide = x > 15.0f * 0.305f + il.seg->width * 0.5f;
        a = (dir.x * off.x + dir.y * off.y) / s->len;
        if (t + a > 1.0f) {
            int i;
            t = 0.0f;
            r = 2;
            i = 1000;
            while (i--) {
                s = s->next;
                if (s->ilpos == 0) {
                    r = 0;
                }
                pos.x = s->pos.x;
                off.x = p->x - pos.x;
                pos.y = s->pos.y;
                off.y = p->y - pos.y;
                dir.x = s->dir.x;
                dir.y = s->dir.y;
                a = (dir.x * off.x + dir.y * off.y) / s->len;
                if (a < 1.0f) {
                    t = t > a ? t : a;
                    break;
                }
            }
        } else {
            t += a;
            if (t < 0.0f) {
                int n;
                r = 3;
                n = 10;
                if (s->type == 0) {
                    while (t < 0.0f) {
                        t += 1.0f;
                        if (s != line->points) {
                            s--;
                        } else {
                            r = 1;
                            s = &line->points[line->total >> 16] - 1;
                        }
                        if (--n == 0) {
                            t = 0.0f;
                            break;
                        }
                    }
                } else {
                    t = 0.0f;
                }
            }
        }
        il.seg = s;
        il.t = t;
        return r;
    } else {
        IdealPoint* s = il.seg;
        f32 t = il.t;
        f32 inv;
        pos.x = s->pos.x + t * (s->next->pos.x - s->pos.x);
        pos.y = s->pos.y + t * (s->next->pos.y - s->pos.y);
        dir.x = s->dir.x + t * (s->next->dir.x - s->dir.x);
        dir.y = s->dir.y + t * (s->next->dir.y - s->dir.y);
        inv = 1.0f / (f32)sqrt(dir.x * dir.x + dir.y * dir.y);
        dir.x *= inv;
        dir.y *= inv;
        off.x = p->x - pos.x;
        off.y = p->y - pos.y;
        return 2;
    }
}

extern "C" {

ILPos fn_80135B24(const LineFollower* f, f32 m) {
    ILPos r;
    r.seg = f->line->start;
    if (r.seg) {
        for (;;) {
            if (r.seg->next->dist >= m || r.seg->next->type != 3) {
                break;
            }
            r.seg = r.seg->next;
        }
        r.t = (m - r.seg->dist) / r.seg->len;
    }
    r.mode = 0;
    return r;
}

void fn_80135B9C(LineFollower* f, f32 d, V2* outPos, V2* outDir, f32* outSpeed, ILPos* outIl) {
    IdealPoint* s = f->il.seg;
    f32 t;
    if (!s) {
        if (outDir) {
            outDir->x = 0.0f;
            outDir->y = 1.0f;
        }
        if (outPos) {
            outPos->x = 0.0f;
            outPos->y = 0.0f;
        }
        return;
    }
    t = f->il.t;
    f32 dt = d / s->len;
    if (t + dt > 1.0f) {
        d -= (1.0f - t) * s->len;
        s = s->next;
        while (d > s->len) {
            d -= s->len;
            s = s->next;
        }
        t = d / s->len;
    } else {
        t += dt;
    }
    if (outPos) {
        outPos->x = s->pos.x + t * (s->next->pos.x - s->pos.x);
        outPos->y = s->pos.y + t * (s->next->pos.y - s->pos.y);
    }
    if (outDir) {
        f32 inv;
        outDir->x = s->dir.x + t * (s->next->dir.x - s->dir.x);
        outDir->y = s->dir.y + t * (s->next->dir.y - s->dir.y);
        inv = 1.0f / (f32)sqrt(outDir->x * outDir->x + outDir->y * outDir->y);
        outDir->x *= inv;
        outDir->y *= inv;
    }
    if (outIl) {
        s32 mode = f->il.mode;
        outIl->seg = s;
        outIl->t = t;
        outIl->mode = mode;
    }
    if (outSpeed) {
        *outSpeed = s->speed + t * (s->next->speed - s->speed);
    }
}

}  // extern "C"

int LineFollower::SetLine(CenterLine* l) {
    if (l != line) {
        Clear();
        line = l;
        if (l) {
            return 1;
        }
        return 0;
    }
    return 2;
}

IdealFollower::IdealFollower(int withTimes, CenterLine* l) {
    if (!l) {
        l = fn_80156A40()->GetLine(0);
    }
    if (SetLine(l)) {
        if (withTimes) {
            times.Init((s32)line->total >> 16, 2);
        } else {
            times.cap = 0;
            times.n = 0;
            times.data = 0;
        }
        Reset();
    }
}

void TimeArray::Init(s32 count, s32 extra) {
    n = count;
    cap = count + extra;
    data = new ("dlatlong:times") f32[count + extra];
}

int IdealFollower::SetLine(CenterLine* l) {
    int r = LineFollower::SetLine(l);
    if (r == 1) {
        base = 0;
        last = 0;
        if (times.data) {
            times.n = (s32)line->total >> 16;
        }
    }
    return r;
}

void IdealFollower::Reset(void) { Start(&line->points->pos, &line->points->dir, 1, 0); }

void IdealFollower::Start(V2* p, V2* d, int resetTimes, IdealPoint* start) {
    SetPosition(p, d, start);
    UpdatePerp();
    if (resetTimes && times.data) {
        f32 v = Sum(1000000.0f, lbl_8055E3C8);
        int i;
        for (i = 0; i < times.n; i++) {
            times.data[i] = v;
        }
        ClearFlag();
    } else {
        ClearFlag();
    }
    latSpeed = 0.0f;
    speed = 0.0f;
    travel = il.seg->dist + il.seg->len * il.t;
    lateral = Dot4(dir.y, -dir.x, off.x, off.y);
    last = 0;
    prev = 0;
}

void IdealFollower::ClearFlag(void) { flag = 0; }

int IdealFollower::Track(V2* p, f32* progress) {
    bool diff;
    bool moved;
    int r;
    u32 cur;
    f32 oldTravel;
    f32 oldLateral;
    bool jump;
    IdealPoint* cs;
    if (!line) {
        return 2;
    }
    moved = il.seg != prev;
    ILPos old = il;
    r = Update(p);
    cur = Rel(old.seg->ilpos);
    cs = il.seg;
    if (r == 0 && progress) {
        IdealPoint* s;
        f32 a = 1.0f - old.t;
        f32 b;
        s = old.seg;
        while (s = s->next, Rel(s->ilpos) > cur) {
            a += 1.0f;
        }
        b = 1.0f - old.t;
        s = old.seg;
        while ((s = s->next) != cs) {
            b += 1.0f;
        }
        b += il.t;
        *progress = (1.0f / 59.94f) * (1.0f - a / b);
    }
    if (last && (r == 2 || r == 0)) {
        f32 now = lbl_8055E3C8;
        u32 i = (s32)Rel(last->ilpos) >> 16;
        u32 end = (s32)Rel(il.seg->ilpos) >> 16;
        while (i != end) {
            times.data[i] = now;
            i = (i + 1) % times.n;
        }
    }
    last = il.seg;
    UpdatePerp();
    oldTravel = travel;
    oldLateral = lateral;
    travel = il.seg->dist + il.seg->len * il.t;
    lateral = Dot4(dir.y, -dir.x, off.x, off.y);
    jump = true;
    if (!moved) {
        diff = true;
        if (old.seg->section == il.seg->section && old.seg->type == il.seg->type) {
            diff = false;
        }
        if (!diff) {
            jump = false;
        }
    }
    if (!jump) {
        f32 ls;
        speed = travel - oldTravel;
        if (speed < -line->len[il.seg->type] * 0.5f) {
            speed = speed + line->len[il.seg->type];
        }
        speed *= 59.94f;
        ls = 59.94f * (lateral - oldLateral);
        latSpeed = Lerp(ls, latSpeed, 0.93f);
    }
    prev = il.seg;
    return r;
}

extern "C" {

u32 fn_801363A0(LineFollower* f, const ILPos* p) {
    u32 a = p->seg->ilpos;
    u32 b = p->seg->next->ilpos;
    if (b == 0) {
        b = f->line->total;
    }
    return a + (u32)(p->t * (s32)(b - a));
}

ILPos fn_80136414(const LineFollower* f, u32 x) {
    IdealPoint* s = &f->line->points[x >> 16];
    f32 t = (x & 0xFFFF) / 65535.0f;
    return ILPos(s, t, 0);
}

ILPos fn_80136468(LineFollower* f, f32 m) {
    CenterLine* l = f->line;
    IdealPoint* s;
    IdealPoint* n;
    s32 mode;
    int i;
    f32 t;
    if (!l) {
        return ILPos();
    }
    mode = f->il.mode;
    if (m > 0.0f) {
        m = fmod(m, l->len[0]);
    } else {
        m = l->len[0] - (f32)fmod(-m, l->len[0]);
    }
    s = l->points;
    for (i = 10000;;) {
        n = s->next;
        if (n == f->line->points || n->dist >= m) {
            break;
        }
        s = n;
        if (--i <= 0) {
            fn_802E30C8("CenterLine::convert_meters_to_ilpos(%f/%f) mode %d: INSANE", m, l->len[0], mode);
            s = f->line->points;
            m = 0.0f;
            break;
        }
    }
    if (s->type != n->type) {
        t = 0.0f;
    } else {
        t = (m - s->dist) / s->len;
    }
    return ILPos(s, t, mode);
}

ILPos fn_80136594(LineFollower* f, V2* p) {
    ILPos r;
    f->Locate(p, 0, &r, 0);
    return r;
}

f32 fn_801365C4(IdealFollower* f, IdealFollower* o) {
    bool same;
    int back;
    int i;
    int j;
    s32 n;
    f32 a;
    f32 b;
    if (!f->line || !o->il.seg || !f->il.seg || o->times.n != f->times.n) {
        return 0.0f;
    }
    n = f->times.n;
    same = o->il.seg == f->il.seg;
    back = same ? -2 : -1;
    i = (((s32)f->Rel(f->il.seg->ilpos) >> 16) + (n + back)) % n;
    j = (i + 1) % n;
    a = Lerp(o->times.data[i], o->times.data[j], f->il.t);
    if (same) {
        b = Lerp(f->times.data[i], f->times.data[j], f->il.t);
    } else {
        b = lbl_8055E3C8;
    }
    return b - a;
}

int fn_801366C0(LineFollower* f, V3* v, f32 x) {
    if (!f->il.seg || !f->line->points) {
        return 0;
    }
    return v->x * f->dir.x + v->z * f->dir.y < -x;
}

}  // extern "C"

void IdealFollower::UpdatePerp(void) {
    perp.x = dir.y;
    perp.y = -dir.x;
}

extern "C" {

f32 fn_8013672C(void) { return fn_80156A40()->GetLine(0)->len[0]; }

void fn_80136764(void) { fn_801564B0(); }

void fn_80136768(void) { fn_801569FC(); }

}  // extern "C"

IdealFollower::~IdealFollower(void) {}

void LineFollower::Attach(CenterLine* l, V2* p, V2* d) {
    if (SetLine(l) == 1) {
        SetPosition(p, d, 0);
    }
}

void IdealFollower::Attach(CenterLine* l, V2* p, V2* d) {
    if (SetLine(l) == 1) {
        Start(p, d, 1, 0);
    }
}

UnkStaticFFFF lbl_8055E0C0;
UnkStatic7F lbl_8055E0C4;
