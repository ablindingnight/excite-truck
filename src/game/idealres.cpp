// Ideal line resources (0x80137D58-0x80138BB8): loads 'ILIN' resources (version 30), byte-swaps and
// relocates the center line points and its extension records, smooths the line around "type 1"
// extensions, and assigns each point its monotonic ilpos by driving an IdealFollower around the line.
// Names are guesses apart from IdealLineRes (from its messages).
#include "game/common.h"
#include "game/centerline.h"
#include "game/v2.h"
#include "game/byteswap.h"

extern "C" {
double sqrt(double x);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void* fn_8012C370(const char* name, u32 type, u32* info, s32* size, bool* first, bool* swap);
int fn_8012C4D8(void* p);
void* fn_8013BFE8(const char* name);
void fn_8013C428(void* p);
}

// A V2 read from a big-endian file (was V2::Swap).
static inline void SwapV2(V2* v) {
    SwapWord(&v->x);
    SwapWord(&v->y);
}

#include "game/resext.h"
#include "game/makeilpos.h"

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

void SwapIdealPointData(IdealPoint* p, IdealLineRes* res) {
    for (int i = 0; i < 2; i++) {
        SwapV2(&(&p->pos)[i]);
    }
    SwapWord(&p->ilpos);
    SwapWord(&p->len);
    SwapWord(&p->dist);
    SwapWord(&p->speed);
    SwapWord(&p->unk24);
    SwapWord(&p->width);
    SwapWord(&p->height);
    SwapWord(&p->lon);
}

void fn_80137F2C(void* p) {}

void fn_80137F30(ResExt* e) {
    SwapWord(&e->data[0]);
    SwapWord(&e->data[1]);
}

void fn_80137F74(u32* v) {
    SwapWord(&v[0]);
    SwapWord(&v[1]);
    SwapWord(&v[2]);
}

// MATCHING-AID: machine-extracted inline helper (loopext); it raised the score but nothing proves the original had it.

/* Helper and record names unproven */
struct IrRec { u16 a; u16 b; u32 c; };
static inline void IrItem(IrRec* r) {
    SwapHalf(&r->a);
    SwapHalf(&r->b);
    SwapWord(&r->c);
}
void ProcessResourceData(ResExt* e) {
    int i;
    SwapWord(&e->data[0]);
    SwapWord(&e->data[1]);
    SwapWord(&e->data[2]);
    for (i = 0; i < (s32)e->data[0]; i++) {
        IrItem(&((IrRec*)&e->data[3])[i]);
    }
}

void SwapExtensionData(ResExt* e) {
#if !defined(__MWERKS__) && !defined(GAME_FILE_DATA_NEEDS_SWAP)
    return;  // PORT: this extension's hand-written swap; the data is already native on a little-endian host
#endif
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

void ProcessResourceExtension(ResExt* e) {
    SwapWord(&e->type);
    switch (e->type) {
    case 0:
        fn_80137F30(e);
        break;
    case 1:
        fn_80137F74(e->data);
        break;
    case 2:
        ProcessResourceData(e);
        break;
    case 3:
        SwapExtensionData(e);
        break;
    default:
        fn_802E317C("idealres: unhandled ext type %x", e->type);
        break;
    }
}

void fn_80138434(void* p);

#pragma push
#pragma opt_lifetimes off  /* Pragma unproven: helps but not exact (PRAGSCAN_2026-11-15) */
/* Helper name unproven: one real scratch byte across required word reversals. */
static inline void SwapLoaderWord(void* p, unsigned char& byte) {
#if defined(__MWERKS__) || defined(GAME_FILE_DATA_NEEDS_SWAP)
    unsigned char* b = (unsigned char*)p;
    byte = b[0];
    b[0] = b[3];
    b[3] = byte;
    byte = b[1];
    b[1] = b[2];
    b[2] = byte;
#endif
}

void fn_801381B4(IdealLineRes* res) {
    unsigned char swapByte;
    int i;
    int t;
    IdealPoint* p;
    ResExt* e;
    SwapLoaderWord(&res->hdr.resSize, swapByte);
    SwapLoaderWord(&res->hdr.segSize, swapByte);
    SwapLoaderWord(&res->len[0], swapByte);
    SwapLoaderWord(&res->len[1], swapByte);
    SwapLoaderWord(&res->len[2], swapByte);
    SwapLoaderWord(&res->len[3], swapByte);
    SwapLoaderWord(&res->total, swapByte);
    SwapLoaderWord(&res->nrelocs, swapByte);
    for (i = 0; i < res->nrelocs; i++) {
        SwapLoaderWord(&res->relocs[i], swapByte);
    }
    for (i = 0; i < res->nrelocs; i++) {
        u32* r = (u32*)((u8*)res + res->relocs[i]);
        SwapLoaderWord(r, swapByte);
        *r += (u32)res;
    }
    fn_80138434(&res->points);
    fn_80138434(&res->start);
    SwapIdealPointData(res->points, res);
    for (t = 0; t < 3; t++) {
        bool found = false;
        for (p = res->points->next; p != res->points; p = p->next) {
            if (p->type == t) {
                found = true;
                SwapIdealPointData(p, res);
            } else if (found) {
                break;
            }
        }
    }
    p = res->start;
    if (p) {
        while (p->type == 3) {
            SwapIdealPointData(p, res);
            p = p->next;
        }
    }
    for (e = res->ext; e; e = e->next) {
        ProcessResourceExtension(e);
    }
}
#pragma pop

void fn_80138434(void* p) { fn_80137F2C(p); }

IdealPoint* FindHighestIdealPoint(IdealPoint* start) {
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

void UpdateIdealLineFollower(IdealLineRes* line, IdealLineRes* ref) {
    IdealFollower f(1, (CenterLine*)ref);
    IdealPoint* p;
    IdealPoint* lo;
    u32 mx;
    f.Start(&line->points->pos, &line->points->dir, 0, 0);
    p = line->points;
    do {
        f.Track(&p->pos, 0);
        p->height = f.lateral;
        p->ilpos = f.il.seg->ilpos;
        p = p->next;
    } while (p != line->points);
    lo = fn_8013846C(FindHighestIdealPoint(p));
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

f32 NormalizeAngleDifference(f32 a, f32 lap, f32 b, f32 radius) {
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

static inline V2 MakeV2( f32 x,f32 y) {
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

static inline f32 NextDist(const IdealPoint* q) {
    V2 d;
    d.y = q->pos.y - q->next->pos.y;
    d.x = q->pos.x - q->next->pos.x;
    return sqrt(d.x * d.x + d.y * d.y);
}

int fn_801386D0(IdealLineRes* line, f32 at, f32 amount, f32 radius) {
    IdealPoint* p;
    f32 t = 0.0f;
    int fail = 0;
    do {
        f32 step = MinF(at - t, 0.02f);
        IdealPoint* q = line->points;
        f32 lap = line->len[0];
        p = q->next;
        IdealPoint* end;
        f32 acc;
        do {
            f32 w = NormalizeAngleDifference(p->dist, lap, amount, radius);
            if (w > 0.0f) {
                f32 qx;
                f32 py = p->dir.y;
                f32 qy = q->dir.y;
                f32 px = p->dir.x;
                qx = q->dir.x;
                f32 y = qy + 0.5f * (py - qy);
                V2 m = MakeV2(qx + 0.5f * (px - qx), y);
                V2 n = m;
                Perp(&n);
                p->height += (w * step);
                p->pos.x += n.x * (w * step);
                p->pos.y += n.y * (w * step);
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
            q->len = NextDist(q);
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

IdealLineRes* LoadIdealLineResource(const char* name) {
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
                    SwapWord(&seg);
                    SwapWord(&res);
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

void FreeIdealLineResources(IdealLineRes** pr) {
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

ResExt* FindExtensionByType(IdealLineRes* res, ResExt* prev) {
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
    UpdateIdealLineFollower(a, b);
}

}  // extern "C"


extern const u32 lbl_8055E0F0 = MakeILPos(0xFFFF, 0);
u32 lbl_8055E0F4 = MakeILPos(0x7F00, 0);
