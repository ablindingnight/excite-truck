// Ideal line resources (0x80137D58-0x80138BB8): loads 'ILIN' resources (version 30), byte-swaps and
// relocates the center line points and its extension records, smooths the line around "type 1"
// extensions, and assigns each point its monotonic ilpos by driving an IdealFollower around the line.
// Names are guesses apart from IdealLineRes (from its messages).
#include "game/common.h"

extern "C" {
double sqrt(double x);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void* fn_8012C370(const char* name, u32 type, u32* info, s32* size, bool* first, bool* swap);
int fn_8012C4D8(void* p);
void* fn_8013BFE8(const char* name);
void fn_8013C428(void* p);
}

static inline void Swap4(void* v) {
    u8* p = (u8*)v;
    u8 t = p[0];
    p[0] = p[3];
    p[3] = t;
    t = p[1];
    p[1] = p[2];
    p[2] = t;
}

static inline void Swap2(void* v) {
    u8* p = (u8*)v;
    u8 t = p[0];
    p[0] = p[1];
    p[1] = t;
}

struct V2 {
    void Swap(void) {
        Swap4(&x);
        Swap4(&y);
    }
    f32 x;
    f32 y;
};

struct IdealPoint {
    V2 pos;            // offset 0x0
    V2 dir;            // offset 0x8
    u32 ilpos;         // offset 0x10
    IdealPoint* next;  // offset 0x14
    f32 len;           // offset 0x18
    f32 dist;          // offset 0x1C
    f32 speed;         // offset 0x20
    f32 unk24;         // offset 0x24
    f32 width;         // offset 0x28
    f32 lat;           // offset 0x2C
    f32 lon;           // offset 0x30
    u8 type;           // offset 0x34
    u8 section;        // offset 0x35
    u8 pad36[6];
};

struct ResExt {
    u32 type;      // offset 0x0
    ResExt* next;  // offset 0x4
    u32 data[1];   // offset 0x8
};

struct ResHeader {
    u32 resSize;
    u32 segSize;
};

struct IdealLineRes {
    ResHeader hdr;       // offset 0x0
    f32 len[4];          // offset 0x8
    u32 total;           // offset 0x18
    IdealPoint* points;  // offset 0x1C
    IdealPoint* start;   // offset 0x20
    ResExt* ext;         // offset 0x24
    void* blips;         // offset 0x28
    s32 nrelocs;         // offset 0x2C
    u32 relocs[1];       // offset 0x30
};

struct ILPos {
    IdealPoint* seg;
    f32 t;
    s32 mode;
};

struct TimeArray {
    ~TimeArray(void) {
        if (data) {
            delete[] data;
        }
        data = 0;
    }
    f32* data;
    s32 n;
    s32 cap;
};

class CenterLine;

class LineFollower {
public:
    ~LineFollower(void);

    void* vtbl;     // offset 0x0
    ILPos il;       // offset 0x4
    u8 pad10[0x30];
};

class IdealFollower : public LineFollower {
public:
    IdealFollower(int withTimes, CenterLine* line);
    ~IdealFollower(void) {}
    void Start(V2* pos, V2* dir, int resetTimes, IdealPoint* start);
    int Track(V2* pos, f32* progress);

    f32 lateral;    // offset 0x40
    u8 pad44[0xC];
    TimeArray times;  // offset 0x50
    u8 pad5C[0xC];
};

ModuleTag lbl_8055E0E8;

static inline f32 Dot4(f32 ax, f32 ay, f32 bx, f32 by) { return ax * bx + ay * by; }

enum { FALSE_, TRUE_ };

extern "C" {

void fn_80137D58(IdealPoint* p, IdealLineRes* res) {
    Swap4(&p->pos.x);
    Swap4(&p->pos.y);
    Swap4(&p->dir.x);
    Swap4(&p->dir.y);
    Swap4(&p->ilpos);
    Swap4(&p->len);
    Swap4(&p->dist);
    Swap4(&p->speed);
    Swap4(&p->unk24);
    Swap4(&p->width);
    Swap4(&p->lat);
    Swap4(&p->lon);
}

void fn_80137F2C(void* p) {}

void fn_80137F30(ResExt* e) {
    Swap4(&e->data[0]);
    Swap4(&e->data[1]);
}

void fn_80137F74(u32* v) {
    Swap4(&v[0]);
    Swap4(&v[1]);
    Swap4(&v[2]);
}

void fn_80137FF0(ResExt* e) {
    u8* p;
    int i;
    Swap4(&e->data[0]);
    Swap4(&e->data[1]);
    Swap4(&e->data[2]);
    p = (u8*)&e->data[3];
    for (i = 0; i < (s32)e->data[0]; i++) {
        Swap2(p);
        Swap2(p + 2);
        Swap4(p + 4);
        p += 8;
    }
}

void fn_801380DC(ResExt* e) {
    u8* n = (u8*)&e->data[0];
    u8 a = n[0];
    u8* p = (u8*)&e->data[1];
    u8 b = n[1];
    int i;
    u8 c;
    n[0] = n[3];
    n[3] = a;
    n[1] = n[2];
    n[2] = b;
    for (i = 0; i < (s32)(e->data[0] * e->data[0]); i++) {
        a = p[0];
        c = p[3];
        p[0] = c;
        p[3] = a;
        a = p[1];
        c = p[2];
        p[1] = c;
        p[2] = a;
        p += 4;
    }
}

void fn_80138144(ResExt* e) {
    Swap4(&e->type);
    switch (e->type) {
    case 0:
        fn_80137F30(e);
        break;
    case 1:
        fn_80137F74(e->data);
        break;
    case 2:
        fn_80137FF0(e);
        break;
    case 3:
        fn_801380DC(e);
        break;
    default:
        fn_802E317C("idealres: unhandled ext type %x", e->type);
        break;
    }
}

void fn_80138434(void* p);

void fn_801381B4(IdealLineRes* res) {
    int i;
    int t;
    IdealPoint* p;
    ResExt* e;
    Swap4(&res->hdr.resSize);
    Swap4(&res->hdr.segSize);
    Swap4(&res->len[0]);
    Swap4(&res->len[1]);
    Swap4(&res->len[2]);
    Swap4(&res->len[3]);
    Swap4(&res->total);
    Swap4(&res->nrelocs);
    for (i = 0; i < res->nrelocs; i++) {
        Swap4(&res->relocs[i]);
    }
    for (i = 0; i < res->nrelocs; i++) {
        u32* r = (u32*)((u8*)res + res->relocs[i]);
        Swap4(r);
        *r += (u32)res;
    }
    fn_80138434(&res->points);
    fn_80138434(&res->start);
    fn_80137D58(res->points, res);
    for (t = 0; t < 3; t++) {
        bool found = false;
        for (p = res->points->next; p != res->points; p = p->next) {
            if (p->type == t) {
                found = true;
                fn_80137D58(p, res);
            } else if (found) {
                break;
            }
        }
    }
    p = res->start;
    if (p) {
        while (p->type == 3) {
            fn_80137D58(p, res);
            p = p->next;
        }
    }
    for (e = res->ext; e; e = e->next) {
        fn_80138144(e);
    }
}

void fn_80138434(void* p) { fn_80137F2C(p); }

IdealPoint* fn_80138438(IdealPoint* start) {
    IdealPoint* p = start->next;
    IdealPoint* best = start;
    for (; p != start; p = p->next) {
        if (p->ilpos > best->ilpos) {
            best = p;
        }
    }
    return best;
}

IdealPoint* fn_8013846C(IdealPoint* start) {
    IdealPoint* p = start->next;
    IdealPoint* best = start;
    for (; p != start; p = p->next) {
        if (p->ilpos < best->ilpos) {
            best = p;
        }
    }
    return best;
}

static inline s32 MaxI(s32 a, s32 b) { return b > a ? b : a; }

void fn_801384A0(IdealLineRes* line, IdealLineRes* ref) {
    IdealFollower f(1, (CenterLine*)ref);
    IdealPoint* p;
    IdealPoint* lo;
    u32 mx;
    f.Start(&line->points->pos, &line->points->dir, 0, 0);
    p = line->points;
    do {
        f.Track(&p->pos, 0);
        p->lat = f.lateral;
        p->ilpos = f.il.seg->ilpos;
        p = p->next;
    } while (p != line->points);
    lo = fn_8013846C(fn_80138438(p));
    lo->ilpos = 0;
    p = lo;
    mx = 0;
    do {
        p->ilpos = MaxI(p->ilpos, mx);
        mx = p->ilpos;
        p = p->next;
    } while (p != lo);
}

void fn_80138958(IdealLineRes* res, f32* v);

void fn_80138594(IdealLineRes* res) {
    ResExt* e;
    for (e = res->ext; e; e = e->next) {
        if (e->type == 1) {
            fn_80138958(res, (f32*)e->data);
        }
    }
}

static inline f32 MinF(f32 a, f32 b) { return a < b ? a : b; }
static inline f32 MaxF(f32 a, f32 b) { return a > b ? a : b; }

f32 fn_801385F0(f32 a, f32 lap, f32 b, f32 radius) {
    f32 d = a - b;
    f32 x;
    if ((f32)__fabs(d) > lap / 2) {
        d -= lap * (d > 0.0f ? 1.0f : -1.0f);
    }
    x = MaxF(MinF(1.0f, 1.0f - (f32)__fabs(d) / radius), 0.0f);
    if (x < -1.0f) {
        return -1.0f;
    }
    if (x > 1.0f) {
        return 1.0f;
    }
    if (x > 0.0f) {
        return x * (-2.0f * x * x) + 3.0f * x * x;
    }
    return x * (-2.0f * x * x) - 3.0f * x * x;
}

static inline V2 MakeV2(f32 x, f32 y) {
    V2 r;
    r.x = x;
    r.y = y;
    return r;
}

static inline void Perp(V2* v) {
    f32 t = v->x;
    v->x = v->y;
    v->y = -t;
}

int fn_801386D0(IdealLineRes* line, f32 at, f32 amount, f32 radius) {
    f32 t = 0.0f;
    int fail = 0;
    do {
        f32 step = MinF(at - t, 0.02f);
        IdealPoint* p;
        IdealPoint* q = line->points;
        f32 lap = line->len[0];
        p = q->next;
        IdealPoint* end;
        f32 acc;
        do {
            f32 w = fn_801385F0(p->dist, lap, amount, radius);
            if (w > 0.0f) {
                f32 k = w * step;
                V2 m = MakeV2(q->dir.x + 0.5f * (p->dir.x - q->dir.x), q->dir.y + 0.5f * (p->dir.y - q->dir.y));
                p->lat += k;
                {
                    V2 n = m;
                    Perp(&n);
                    p->pos.x += n.x * k;
                    p->pos.y += n.y * k;
                }
            }
            end = line->points;
            q = p;
            p = p->next;
        } while (q != end);
        acc = 0.0f;
        q = end;
        do {
            IdealPoint* nx = q->next;
            f32 len;
            f32 inv;
            V2 d;
            q->dir.x = nx->pos.x - q->pos.x;
            q->dir.y = nx->pos.y - q->pos.y;
            len = sqrt(q->dir.x * q->dir.x + q->dir.y * q->dir.y);
            if (0.01f > len) {
                len = 0.01f;
            }
            inv = 1.0f / len;
            q->dir.x *= inv;
            q->dir.y *= inv;
            q->dist = acc;
            d.y = q->pos.y - q->next->pos.y;
            d.x = q->pos.x - q->next->pos.x;
            q->len = sqrt(d.x * d.x + d.y * d.y);
            if (q->len < 0.5f) {
                fail = 1;
            }
            acc += q->len;
            q = q->next;
        } while (q != line->points && !fail);
        t += step;
    } while (t < at && !fail);
    return !fail;
}

int fn_801388F4(IdealPoint* p) {
    V2 d = p->dir;
    f32 x = d.x;
    d.x = d.y;
    d.y = -x;
    return Dot4(d.x, d.y, p->next->dir.x, p->next->dir.y) < 0.0f ? TRUE_ : FALSE_;
}

void fn_80138958(IdealLineRes* res, f32* v) { fn_801386D0(res, v[0], v[1], v[2]); }

IdealLineRes* fn_80138968(const char* name) {
    u32 version;
    s32 size;
    u32 seg;
    u32 res;
    bool swap;
    bool first;
    IdealLineRes* r = (IdealLineRes*)fn_8012C370(name, 'ILIN', &version, &size, &first, &swap);
    if (r) {
        int ok = 0;
        if (size) {
            if (version == 30) {
                seg = r->hdr.segSize;
                res = r->hdr.resSize;
                if (swap) {
                    Swap4(&seg);
                    Swap4(&res);
                }
                if (seg == sizeof(IdealPoint) && res == 0x30) {
                    if (swap) {
                        fn_801381B4(r);
                        fn_80138594(r);
                    }
                    r->blips = fn_8013BFE8(name);
                    ok = 1;
                } else {
                    fn_802E30C8("ideal: IdealLineRes has mismatched sizes ilseg:(%x vs %x) res:(%x vs %x) (f%d)",
                                seg, sizeof(IdealPoint), res, 0x30, swap);
                }
            } else {
                fn_802E30C8("ideal: bad IdealLineRes version (%d != %d)", version, 30);
            }
        }
        if (!ok) {
            fn_8012C4D8(r);
            r = 0;
        }
    }
    return r;
}

void fn_80138AD8(IdealLineRes** pr) {
    fn_8013C428((*pr)->blips);
    fn_8012C4D8(*pr);
    *pr = 0;
}

ResExt* fn_80138B1C(IdealLineRes* res) {
    ResExt* r = 0;
    ResExt* e;
    for (e = res->ext; e; e = e->next) {
        if (e->type == 0) {
            r = e;
            break;
        }
    }
    return r;
}

ResExt* fn_80138B4C(IdealLineRes* res, ResExt* prev) {
    ResExt* r;
    ResExt* e = prev ? prev->next : 0;
    r = 0;
    switch ((u32)e) {
    case 0:
        e = res->ext;
    }
    for (; e; e = e->next) {
        if (e->type == 2) {
            r = e;
            break;
        }
    }
    return r;
}

void fn_80138BA0(IdealLineRes* a, IdealLineRes* b) {
    if (a == b) {
        return;
    }
    fn_801384A0(a, b);
}

}  // extern "C"

static inline u32 MakeILPos(u32 seg, u32 frac) { return (seg << 16) | frac; }

u32 lbl_8055E0F0 = MakeILPos(0xFFFF, 0);
u32 lbl_8055E0F4 = MakeILPos(0x7F00, 0);
