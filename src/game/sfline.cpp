// Start/finish line (0x802A0D60-0x802A1590): a checkerboard of textured squares laid across the
// track where the racing line crosses its first node, faded out towards the edges. Names are
// guesses apart from the config key and texture.
#include "game/common.h"
#include "game/idealline.h"
#include "game/centerline.h"

struct SFPath {
    u8 pad0[0x18];
    u32 flags;  // offset 0x18: node count in the high half
    IdealPoint* nodes;  // offset 0x1C
};

class SFPathSet {
public:
    virtual SFPath* GetPath(int i);  // 0x08
    virtual SFPath* GetLine(int i);  // 0x0C
};

// Ideal-line walker (idealline.cpp).
struct V2;
struct CenterLine;
struct SFConfig {
    u8 pad0[0x8];
    u32 mode;  // offset 0x8
    u8 padC[0x20 - 0xC];
    ValueNode* vals;  // offset 0x20
};

struct DrawList;

extern "C" {
}  // extern "C"
SFConfig* fn_8000AC38(void);
extern "C" {
int fn_80161C38(void);
SFPathSet* fn_80156A40(void);
int fn_801E9E30(s16* h, ValVec3* pos);
void fn_801E9DC4(s16* h);
double log2(double x);
double sqrt(double x);
DrawList* fn_802CA04C(int n);
void fn_802CA094(DrawList* d);
void fn_802CA09C(DrawList* d, u32 color, int flag, f32 x, f32 y, f32 z, f32 u, f32 v);
void fn_802CA0A0(DrawList* d);
s32 fn_802CAC28(const char* name, int flags);
void gxForgetTexture(s32 tex);
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
void fn_802A92CC(void);
void fn_802A93A4(void);
void fn_802A9428(void);
}

extern f32 __float_epsilon[];  // FLT_EPSILON

static inline int Round(f32 x) { return x < 0.0f ? x - 0.5f : 0.5f + x; }

static inline void SFCorner(V2* q, V2* o, V2* a, V2* b, f32 s, f32 t) {
    q->x = o->x + a->x * s;
    q->y = o->y + a->y * s;
    q->x += b->x * t;
    q->y += b->y * t;
}
static inline f32 SFMul(f32 a, f32 b) { return a * b; }

static inline int OddCount(f32 len, f32 size) { return (int)(len / size) | 1; }

ModuleTag lbl_8055F838;
DrawList* lbl_8055F840;
s32 lbl_8055F844;
f32 lbl_8055F848;
V2 lbl_8055F850;  // line centre
V2 lbl_8055F858;  // along the track
V2 lbl_8055F860;  // across the track

extern "C" {

// NonMatching: the p.y = 0 store is scheduled one slot later than in the target.
void fn_802A0D60(s16* h, V2* o, V2* a, V2* __restrict b, f32 size, f32 alpha, int white) {
    int i;
    int al;
    u32 c;
    if (white && !((f32)__fabs(alpha) <= __float_epsilon[0])) {
        al = Round(255.0f * alpha);
        al = al > 0 ? al : 0;
        al = al < 255 ? al : 255;
        c = (u8)-white;
        for (i = 0; i < 4; i++) {
            V2 q;
            ValVec3 p;
            f32 u;
            f32 v;
            SFCorner(&q, o, a, b, size * ((i & 1) == 0), size * ((i & 2) == 0));
            p.x = q.x;
            p.y = 0.0f;
            p.z = q.y;
            fn_801E9E30(h, &p);
            u = (i & 1) ? 1.0f : 0.0f;
            v = (i & 2) ? 1.0f : 0.0f;
            fn_802CA09C(lbl_8055F840, (al << 24) | (c << 16) | (c << 8) | c, i > 1, p.x, p.y, p.z, u, v);
        }
    }
}

// Projects (a - b) onto dir.
f32 fn_802A0F7C(const V2* dir, const V2* a, const V2* b) {
    V2 d;
    d.x = a->x - b->x;
    d.y = a->y - b->y;
    f32 dx = dir->x;
    f32 dy = dir->y;
    return dx * d.x + dy * d.y;
}

int fn_802A0FBC(void) {
    SFConfig* c = fn_8000AC38();
    int show = c->vals->GetBool("show_ground_sfline", 1);
    u32 mode = c->mode;
    return show && mode != 0x180304 && (mode & 0x100) && !fn_80161C38();
}

IdealPoint* fn_802A1040(IdealPoint* first, IdealPoint* n, SFPath* path) {
    int i = 0;
    do {
        f32 d0 = fn_802A0F7C(&first->dir, &n->pos, &first->pos);
        f32 d1 = fn_802A0F7C(&first->dir, &n->next->pos, &first->pos);
        if (d0 < 0.0f && d1 > 0.0f) {
            break;
        }
        if (d0 < 0.0f) {
            n = n->next;
        } else if (d0 > 0.0f) {
            if (n == path->nodes) {
                n = path->nodes + (path->flags >> 16) - 1;
            } else {
                n--;
            }
        }
    } while (i++ < 10);
    return n;
}

void fn_802A1120(V2* pos, V2* dir) {
    SFPath* path;
    SFPath* line;
    IdealPoint* n;
    f32 t;
    path = fn_80156A40()->GetPath(0);
    {
        IdealPoint* f = path->nodes;
        dir->x = f->dir.x;
        dir->y = f->dir.y;
    }
    LineFollower it;
    line = fn_80156A40()->GetLine(0);
    it.SetLine((CenterLine*)line);
    ILPos h;
    it.Locate((V2*)path->nodes, 0, &h, 0);
    n = fn_802A1040(path->nodes, h.seg, line);
    for (t = 0.0f; t < 1.0f; t += 0.025f) {
        pos->x = n->pos.x + t * (n->next->pos.x - n->pos.x);
        pos->y = n->pos.y + t * (n->next->pos.y - n->pos.y);
        if (fn_802A0F7C(&path->nodes->dir, pos, &path->nodes->pos) >= 0.0f) {
            break;
        }
    }
}

// NonMatching: the hoisted loop constants get different callee-saved float registers.
void fn_802A1290(void) {
    fn_802A92CC();
    if (fn_802A0FBC()) {
        int n;
        int i;
        int j;
        f32 width;
        f32 half;
        f32 inner;
        f32 range;
        s16 h;
        f32 t;
        lbl_8055F844 = fn_802CAC28("sfsquare.tex", 0);
        f32 len = 45.0f;
        f32 sz = 1.5f;
        n = (int)(len / sz) | 1;
        width = sz * n;
        lbl_8055F840 = fn_802CA04C(n * 12 / 2);
        lbl_8055F848 = log2(0.33333334f);
        fn_802A1120(&lbl_8055F850, &lbl_8055F858);
        lbl_8055F860 = lbl_8055F858;
        t = lbl_8055F860.x;
        lbl_8055F860.x = lbl_8055F860.y;
        lbl_8055F860.y = -t;
        fn_801E9DC4(&h);
        half = SFMul(width, 0.5f);
        inner = 0.5f * (0.75f * width);
        range = half - inner;
        for (i = 0; i < n; i++) {
            for (j = 0; j < 3; j++) {
                V2 q;
                V2 d;
                f32 a;
                SFCorner(&q, &lbl_8055F850, &lbl_8055F858, &lbl_8055F860, 1.5f * j, 1.5f * i - half);
                f32 dy = q.y - lbl_8055F850.y;
                d.x = q.x - lbl_8055F850.x;
                d.y = dy;
                a = ((f32)sqrt(d.x * d.x + d.y * d.y) - inner) / range;
                if (1.0f < a) {
                    a = 1.0f;
                }
                a = a > 0.0f ? a : 0.0f;
                fn_802A0D60(&h, &q, &lbl_8055F858, &lbl_8055F860, 1.5f, 1.0f + SFMul(a, -1.0f), (i + j) & 1);
            }
        }
    } else {
        lbl_8055F840 = 0;
        lbl_8055F844 = -2;
    }
}

void fn_802A14F8(void) {
    if (lbl_8055F844 != -2) {
        gxForgetTexture(lbl_8055F844);
        lbl_8055F844 = -2;
    }
    if (lbl_8055F840) {
        fn_802CA094(lbl_8055F840);
        lbl_8055F840 = 0;
    }
    fn_802A93A4();
}

void fn_802A1548(void) {
    if (lbl_8055F840) {
        fn_802C8BD0(lbl_8055F844);
        fn_802CA0A0(lbl_8055F840);
        fn_802C8D74();
    }
}

void fn_802A1584(void) { fn_802A9428(); }
}
