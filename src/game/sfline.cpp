// Start/finish line (0x802A0D60-0x802A1590): a checkerboard of textured squares laid across the
// track where the racing line crosses its first node, faded out towards the edges. Names are
// guesses apart from the config key and texture.
#include "game/common.h"

struct SFVec2 {
    f32 x, y;
};

struct SFVec3 {
    f32 x, y, z;
};

struct SFNode {
    SFVec2 pos;  // offset 0x0
    SFVec2 dir;  // offset 0x8
    u8 pad10[0x14 - 0x10];
    SFNode* next;  // offset 0x14
    u8 pad18[0x3C - 0x18];
};

struct SFPath {
    u8 pad0[0x18];
    u32 flags;  // offset 0x18: node count in the high half
    SFNode* nodes;  // offset 0x1C
};

class SFPathSet {
public:
    virtual SFPath* GetPath(int i);  // 0x08
    virtual SFPath* GetLine(int i);  // 0x0C
};

struct SFIter {
    u8 pad[0x3C];
};

struct SFHit {
    SFNode* node;  // offset 0x0
    f32 t;         // offset 0x4
    s32 max;       // offset 0x8
};

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
void fn_801352B8(SFIter* it);
int fn_80135D60(SFIter* it, SFPath* path);
void fn_801355C0(SFIter* it, SFNode* pos, int a, SFHit* hit, int b);
void fn_8013530C(SFIter* it, int unk);
int fn_801E9E30(s16* h, SFVec3* pos);
void fn_801E9DC4(s16* h);
double fn_8002F208(double x);
double sqrt(double x);
DrawList* fn_802CA04C(int n);
void fn_802CA094(DrawList* d);
void fn_802CA09C(DrawList* d, u32 color, int flag, f32 x, f32 y, f32 z, f32 u, f32 v);
void fn_802CA0A0(DrawList* d);
s32 fn_802CAC28(const char* name, int flags);
void fn_802CAC54(s32 tex);
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
void fn_802A92CC(void);
void fn_802A93A4(void);
void fn_802A9428(void);
}

extern f32 lbl_80558DC4[];  // FLT_EPSILON

static inline int Round(f32 x) { return x < 0.0f ? x - 0.5f : 0.5f + x; }

static inline void SFCorner(SFVec2* q, SFVec2* o, SFVec2* a, SFVec2* b, f32 s, f32 t) {
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
SFVec2 lbl_8055F850;  // line centre
SFVec2 lbl_8055F858;  // along the track
SFVec2 lbl_8055F860;  // across the track

extern "C" {

// NonMatching: the p.y = 0 store is scheduled one slot later than in the target.
void fn_802A0D60(s16* h, SFVec2* o, SFVec2* a, SFVec2* b, f32 size, f32 alpha, int white) {
    int i;
    int al;
    u32 c;
    if (white && !((f32)__fabs(alpha) <= lbl_80558DC4[0])) {
        al = Round(255.0f * alpha);
        al = al > 0 ? al : 0;
        al = al < 255 ? al : 255;
        c = (u8)-white;
        for (i = 0; i < 4; i++) {
            SFVec2 q;
            SFVec3 p;
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

// NonMatching: the target allocates the frame after the loads and stores d last.
f32 fn_802A0F7C(const SFVec2* dir, const SFVec2* a, const SFVec2* b) {
    SFVec2 d;
    d.x = a->x - b->x;
    d.y = a->y - b->y;
    return dir->x * d.x + dir->y * d.y;
}

int fn_802A0FBC(void) {
    SFConfig* c = fn_8000AC38();
    int show = c->vals->GetBool("show_ground_sfline", 1);
    u32 mode = c->mode;
    return show && mode != 0x180304 && (mode & 0x100) && !fn_80161C38();
}

SFNode* fn_802A1040(SFNode* first, SFNode* n, SFPath* path) {
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

void fn_802A1120(SFVec2* pos, SFVec2* dir) {
    SFPath* path;
    SFPath* line;
    SFNode* n;
    SFIter it;
    SFHit h;
    f32 t;
    path = fn_80156A40()->GetPath(0);
    {
        SFNode* f = path->nodes;
        dir->x = f->dir.x;
        dir->y = f->dir.y;
    }
    fn_801352B8(&it);
    line = fn_80156A40()->GetLine(0);
    fn_80135D60(&it, line);
    h.node = 0;
    h.max = 10000;
    h.t = 0.0f;
    fn_801355C0(&it, path->nodes, 0, &h, 0);
    n = fn_802A1040(path->nodes, h.node, line);
    for (t = 0.0f; t < 1.0f; t += 0.1f) {
        pos->x = n->pos.x + t * (n->next->pos.x - n->pos.x);
        pos->y = n->pos.y + t * (n->next->pos.y - n->pos.y);
        if (fn_802A0F7C(&path->nodes->dir, pos, &path->nodes->pos) >= 0.0f) {
            break;
        }
    }
    fn_8013530C(&it, -1);
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
        f32 len = 90.0f;
        f32 sz = 1.5f;
        n = (int)(len / sz) | 1;
        width = sz * n;
        lbl_8055F840 = fn_802CA04C(n * 12 / 2);
        lbl_8055F848 = fn_8002F208(0.3333333333333333);
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
                SFVec2 q;
                SFVec2 d;
                f32 a;
                SFCorner(&q, &lbl_8055F850, &lbl_8055F858, &lbl_8055F860, 1.5f * j, 1.5f * i - half);
                d.y = q.y - lbl_8055F850.y;
                d.x = q.x - lbl_8055F850.x;
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
        fn_802CAC54(lbl_8055F844);
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
