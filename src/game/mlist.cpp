// Model lists (0x802CCBC0-0x802CE344, mlist.cpp): a baked, byte-swapped list of draw batches for a scene
// (bounding spheres, portal-ish boxes, culled pieces, display-list strips grouped by texture) plus a set of
// render objects. Calc culls everything against the camera once a frame and fills the shared visibility
// bytes; Flush draws one texture-priority band. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
void* memset(void* p, int c, u32 n);
void* memcpy(void* d, const void* s, u32 n);
int strcasecmp(const char* a, const char* b);
void qsort(void* base, u32 n, u32 size, int (*cmp)(const void*, const void*));
double tan(double x);
}

struct MVec3 {
    f32 x, y, z;
};

struct MVec4 {
    f32 x, y, z, w;
};

struct MMtx {
    f32 m[12];
};

struct MMtx33 {
    f32 xx, xy, xz;
    f32 yx, yy, yz;
    f32 zx, zy, zz;
};

struct MCam {
    MMtx33 r;  // offset 0x0
    MVec3 t;   // offset 0x24
};

struct F2 {
    F2(f32 x) { a = x; }
    F2(f32 x, f32 y) {
        a = x;
        b = y;
    }
    f32 a, b;
};

struct F4 {
    F4(const F2& l, const F2& h) : lo(l), hi(h) {}
    F2 lo, hi;
};

class MList;

class MLObj {
public:
    virtual ~MLObj(void);
    virtual void Calc(MList* m);
    virtual int Priority(void);
    virtual void Flush(MList* m);
};

// A (0x10): bounding sphere of a group.
struct MLSphere {
    f32 x, y, z, r;
};

// B (0x20): a box tested against the eye, gated by its sphere.
struct MLBox {
    MVec3 ext;    // offset 0x0
    u16 sphere;   // offset 0xC
    u16 padE;
    MVec3 c;      // offset 0x10
    f32 pad1C;
};

// C (0x20): a culled piece.
struct MLPiece {
    MLSphere s;   // offset 0x0
    u16 ref;      // offset 0x10 (visibility slot of its parent)
    u16 flags;    // offset 0x12 (0x40: hidden)
    u8 pad[0xC];
};

struct MLPrim {
    u8 pad[8];
    f32 size;     // offset 0x8
};

// D (0xC): one strip.
struct MLStrip {
    u16 ref;        // offset 0x0
    u16 flags;      // offset 0x2
    u16 mtx;        // offset 0x4
    u16 pad6;
    MLPrim* prim;   // offset 0x8
};

// E (0x1C): a texture batch over a run of strips.
struct MLBatch {
    char name[16];  // offset 0x0
    s32 tex;        // offset 0x10
    s32 start;      // offset 0x14
    s32 end;        // offset 0x18
};

struct MLRange {
    s32 lo;     // offset 0x0 (first batch)
    s32 hi;     // offset 0x4 (last batch + 1)
    s32 first;  // offset 0x8 (first object)
    s32 end;    // offset 0xC (last object + 1)
};

template <class T>
struct MLArray {
    s32 n;
    T* p;
};

class MList {
public:
    u32 magic;                 // offset 0x0
    u32 version;               // offset 0x4
    u32 flags;                 // offset 0x8 (1: fixed up, 2: prepared)
    u32 unkC;                  // offset 0xC
    MLArray<MLSphere> a;       // offset 0x10
    MLArray<MLBox> b;          // offset 0x18
    MLArray<MLPiece> c;        // offset 0x20
    MLArray<MLStrip> d;        // offset 0x28
    MLArray<MLBatch> e;        // offset 0x30
    MLRange ranges[5];         // offset 0x38
    s32 numObjs;               // offset 0x88
    s32 maxObjs;               // offset 0x8C
    MLObj** objs;              // offset 0x90
};

struct RenderCtx {
    u8 vis[0x64];      // offset 0x0
    void* mtx[256];    // offset 0x64
};

extern "C" {
void fn_801293C8(const char* name);
void fn_801293D0(void);
void fn_801293DC(void* buf);
void fn_801293E8(void* buf);
const MCam* fn_802C86DC(void);
void fn_802C8B70(s32 tex);
void fn_802C8D74(void);
int fn_802C94A0(const MVec3* c, f32 r, u8* clip, f32* dist);
void fn_802C98DC(f32* fov, f32* znear, f32* zfar, f32* aspect);
s32 fn_802CAC28(const char* name, s32 key);
void fn_802CAC54(s32 id);
s32 fn_802CAFC8(s32 id);
s32 fn_802CAFF0(s32 id);
void fn_802CC550(s32 id, f32 dist, f32 scale);
void* fn_802D0514(const void* m);
void fn_802D0610(void* m);
void fn_802D6B08(MLPrim* p);
void fn_802DB2A0(void);
u8* fn_802DB2A8(s32 n);
void fn_802DB2B0(s32 n);
void fn_802E317C(const char* fmt, ...);
f32 PSVECDotProduct(const MVec3* a, const MVec3* b);
}

extern RenderCtx* lbl_8055DFC4;

u8 lbl_804C1480[0x4000];   // profile buffer
f32 lbl_804C5480[100];     // on-screen size per visibility slot
MCam lbl_804C5610;         // camera of the last calc
u8 lbl_804C5640[0x10];

ModuleTag lbl_8055FCC8;
UnkStaticFFFF lbl_8055FCD0;
f32 lbl_8055FCD4 = tan(0.65449846f);  // tan(fov / 2)
MList* lbl_8055FCD8;   // list being drawn
u8 lbl_8055FCDC;       // last cull result
f32 lbl_8055FCE0;      // last cull distance

f32 lbl_8055D0A0 = 0.1f;
f32 lbl_8055D0A4 = 1000.0f;

template <class T>
inline void Relocate(T*& p, u32 base);

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void Swap16(void* p) {
    u8 t = ((u8*)p)[0];
    ((u8*)p)[0] = ((u8*)p)[1];
    ((u8*)p)[1] = t;
}

static inline void Swap32(void* p) {
    u8 t = ((u8*)p)[0];
    ((u8*)p)[0] = ((u8*)p)[3];
    ((u8*)p)[3] = t;
    t = ((u8*)p)[1];
    ((u8*)p)[1] = ((u8*)p)[2];
    ((u8*)p)[2] = t;
}

static inline void SwapL(void* p) {
    u8* b = (u8*)p;
    u8 t = b[0];
    b[0] = b[3];
    b[3] = t;
    t = b[1];
    b[1] = b[2];
    b[2] = t;
}

template <class T>
static inline void SwapItems(T& a, T& b) {
    T t = a;
    a = b;
    b = t;
}

static inline u8 Cull(const MLSphere* sp) {
    MLSphere s = *sp;
    MVec3 c;
    u8 clip;
    c.x = s.x;
    c.y = s.y;
    c.z = s.z;
    if (fn_802C94A0(&c, s.r, &clip, &lbl_8055FCE0)) {
        lbl_8055FCDC = clip ? 0x81 : 1;
    } else {
        lbl_8055FCDC = 0;
    }
    return lbl_8055FCDC;
}

extern "C" {

void fn_802CCBC0(const MMtx33* r, F4 a, F4 b, F4 c) {}

u8 fn_802CCBC4(const MLBox* e, MVec4 eye, f32 a, f32 b) {
    MVec4 w;
    MVec3 d;
    MVec4 v = *(MVec4*)&e->ext;
    memcpy(&w, &v, sizeof(w));
    d.x = eye.x - e->c.x;
    d.y = eye.y - e->c.y;
    d.z = eye.z - e->c.z;
    f32 r = b * (a + PSVECDotProduct(&d, &d));
    MVec4 u = w;
    u32 code = 0;
    if (u.x > r) {
        code |= 1;
    }
    if (u.x < -r) {
        code |= 2;
    }
    if (u.y > r) {
        code |= 4;
    }
    if (u.y < -r) {
        code |= 8;
    }
    if (u.z > r) {
        code |= 0x10;
    }
    if (u.z < -r) {
        code |= 0x20;
    }
    return ~code & 0x15;
}

int fn_802CCD0C(const void* pa, const void* pb) {
    const MLBatch* a = (const MLBatch*)pa;
    const MLBatch* b = (const MLBatch*)pb;
    s32 x = fn_802CAFC8(a->tex);
    s32 y = fn_802CAFC8(b->tex);
    if (x != y) {
        return x - y;
    }
    x = fn_802CAFF0(a->tex);
    y = fn_802CAFF0(b->tex);
    if (x != y) {
        return x - y;
    }
    if (a->tex == -1) {
        return 1;
    }
    if (b->tex == -1) {
        return -1;
    }
    return strcasecmp(a->name, b->name);
}

int fn_802CCDC4(const void* pa, const void* pb) {
    MLObj* a = *(MLObj**)pa;
    MLObj* b = *(MLObj**)pb;
    return a->Priority() - b->Priority();
}

void fn_802CCE24(MList* m) {
    for (int i = 0; i < m->e.n; i++) {
        MLBatch* e = &m->e.p[i];
        e->tex = fn_802CAC28(e->name, 0);
    }
    qsort(m->e.p, m->e.n, sizeof(MLBatch), fn_802CCD0C);
    for (int i = 0; i < m->e.n; i++) {
        MLBatch* e = &m->e.p[i];
        int n = e->end - e->start;
        MLStrip* d = &m->d.p[e->start];
        for (int j = 0; j < n / 2; j++) {
            SwapItems(d[j], d[n - j - 1]);
        }
    }
    qsort(m->objs, m->numObjs, sizeof(MLObj*), fn_802CCDC4);
    memset(m->ranges, 0, sizeof(m->ranges));
    for (int p = 0; p < 5; p++) {
        m->ranges[p].lo = m->e.n;
        m->ranges[p].hi = -1;
        m->ranges[p].first = 0;
        m->ranges[p].end = 0;
        for (int j = 0; j < m->numObjs; j++) {
            if (m->objs[j]->Priority() == p) {
                m->ranges[p].first = j;
                j++;
                m->ranges[p].end = j;
                for (; j < m->numObjs; j++) {
                    if (m->objs[j]->Priority() != p) {
                        break;
                    }
                    m->ranges[p].end++;
                }
                break;
            }
        }
    }
    for (int i = 0; i < m->e.n; i++) {
        s32 pr = fn_802CAFC8(m->e.p[i].tex);
        int cls = 0;
        if (pr == 1) {
            cls = 0;
        } else if (pr == 2 || pr == 3) {
            cls = 1;
        } else if (pr == 4) {
            cls = 2;
        } else if (pr >= 5 && pr <= 8) {
            cls = 3;
        } else if (pr == 9) {
            cls = 4;
        } else {
            fn_802E317C("mlist: unhandled priority %d", pr);
        }
        MLRange* r = &m->ranges[cls];
        r->lo = i < r->lo ? i : r->lo;
        r->hi = i + 1 > r->hi ? i + 1 : r->hi;
    }
    m->flags |= 2;
}

void fn_802CD254(MList* m) {
    int i;
    for (i = 0; i < m->e.n; i++) {
        MLBatch* e = &m->e.p[i];
        fn_802CAC54(e->tex);
        e->tex = -2;
    }
    for (i = 0; i < m->numObjs; i++) {
        delete m->objs[i];
        m->objs[i] = 0;
    }
    m->numObjs = 0;
    m->flags &= ~2;
}

void fn_802CD320(MList* m, MLObj* o) { m->objs[m->numObjs++] = o; }

void fn_802CD33C(MList* m, int i, int on) {
    if (on) {
        m->c.p[i].flags &= ~0x40;
    } else {
        m->c.p[i].flags |= 0x40;
    }
}

static inline void MulVecT(MVec3* out, const MVec3* in, const MMtx33* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m->xx, in->y, m->xy, in->z, m->xz);
        f32 y = Dot3(in->x, m->yx, in->y, m->yy, in->z, m->yz);
        f32 z = Dot3(in->x, m->zx, in->y, m->zy, in->z, m->zz);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m->xx, in->y, m->xy, in->z, m->xz);
        out->y = Dot3(in->x, m->yx, in->y, m->yy, in->z, m->yz);
        out->z = Dot3(in->x, m->zx, in->y, m->zy, in->z, m->zz);
    }
}

static inline void Transpose(MMtx33* m) {
    f32 t;
    t = m->xy;
    m->xy = m->yx;
    m->yx = t;
    t = m->xz;
    m->xz = m->zx;
    m->zx = t;
    t = m->yz;
    m->yz = m->zy;
    m->zy = t;
}

static inline void SetIdentity(MCam* m) {
    m->r.xx = 1.0f;
    m->r.xy = 0.0f;
    m->r.xz = 0.0f;
    m->r.yx = 0.0f;
    m->r.yy = 1.0f;
    m->r.yz = 0.0f;
    m->r.zx = 0.0f;
    m->r.zy = 0.0f;
    m->r.zz = 1.0f;
    m->t.x = 0.0f;
    m->t.y = 0.0f;
    m->t.z = 0.0f;
}

void fn_802CD37C(MList* m, int unused, const MMtx* mats, int n, f32 a, f32 b) {
    fn_801293C8("mlist: calc");
    MCam ident;
    SetIdentity(&ident);
    lbl_8055FCD8 = m;
    RenderCtx* ctx = lbl_8055DFC4;
    void* id = fn_802D0514(&ident);
    ctx->mtx[0] = id;
    for (int i = 0; i < n; i++) {
        ctx->mtx[i + 1] = fn_802D0514(mats);
        mats++;
    }
    for (int j = n; j < 256; j++) {
        ctx->mtx[j] = id;
    }
    MLArray<MLSphere> sa = m->a;
    const MCam* v = fn_802C86DC();
    MCam cam = *v;
    lbl_804C5610 = cam;
    MMtx33 rt = cam.r;
    Transpose(&rt);
    MVec3 t;
    MulVecT(&t, &cam.t, &cam.r);
    t.x *= -1.0f;
    t.y *= -1.0f;
    t.z *= -1.0f;
    f32 aspect;
    fn_802C98DC(&lbl_8055FCD4, &lbl_8055D0A0, &lbl_8055D0A4, &aspect);
    f32 zn = lbl_8055D0A0;
    f32 zf = lbl_8055D0A4;
    f32 q = (zf + zn) / (zf - zn);
    fn_802CCBC0(&rt, F4(F2(1.0f / lbl_8055FCD4, 1.0f / lbl_8055FCD4), F2(q)),
                F4(F2(0.0f, 0.0f), F2(zf * (1.0f - q))), F4(F2(0.0f, 0.0f), F2(1.0f / aspect)));
    u8* vis = ctx->vis;
    vis[0] = 0x81;
    u8* vp = vis + 1;
    int k = 1;
    const MLSphere* sp = sa.p;
    int i;
    for (i = 0; i < sa.n; i++, k++, vp++) {
        *vp = Cull(sp);
        sp++;
    }
    MVec4 eye;
    eye.x = cam.t.x;
    eye.y = cam.t.y;
    eye.z = cam.t.z;
    eye.w = 0.0f;
    MLArray<MLBox> ba = m->b;
    const MLBox* bp = ba.p;
    vp = vis + k;
    for (i = 0; i < ba.n; i++, k++, vp++) {
        *vp = vis[bp->sphere] ? fn_802CCBC4(bp, eye, a, b) : 0;
        bp++;
    }
    MLArray<MLPiece> ca = m->c;
    const MLPiece* cp = ca.p;
    vp = vis + k;
    for (i = 0; i < ca.n; i++) {
        if (cp->flags == (vis[cp->ref] & 0x7F)) {
            u8 r = Cull(&cp->s);
            lbl_804C5480[k] = lbl_8055FCE0;
            *vp = r;
        } else {
            *vp = 0;
        }
        k++;
        vp++;
        cp++;
    }
    for (i = 0; i < m->numObjs; i++) {
        m->objs[i]->Calc(m);
    }
    fn_801293DC(lbl_804C1480);
    fn_801293D0();
}

void fn_802CDAA8(void) { lbl_8055FCD8 = 0; }

void fn_802CDAB4(MList* m, int i, const MMtx* mtx) {
    RenderCtx* ctx = lbl_8055DFC4;
    ctx->mtx[i] = fn_802D0514(mtx);
}

void fn_802CDAFC(MList* m, int i) { lbl_8055DFC4->vis[i + m->b.n + m->a.n + 1] = 0; }

int fn_802CDB20(MList* m, int i) {
    if (i < 0) {
        return 0;
    }
    return lbl_8055DFC4->vis[i + m->b.n + m->a.n + 1] != 0;
}

void fn_802CDB5C(int pri) {
    fn_801293C8("mlist: flush");
    fn_801293E8(lbl_804C1480);
    MList* m = lbl_8055FCD8;
    MLArray<MLStrip> da = m->d;
    MLRange r = m->ranges[pri];
    MLArray<MLBatch> ea = m->e;
    MCam ident;
    SetIdentity(&ident);
    RenderCtx* ctx = lbl_8055DFC4;
    void* id = fn_802D0514(&ident);
    fn_802D0610(id);
    ctx->mtx[0] = id;
    void* cur = 0;
    MLBatch* e = &ea.p[r.lo];
    for (int i = r.lo; i < r.hi; i++, e++) {
        int open = 0;
        u8* dl = 0;
        s32 cnt = 0;
        s32 tex = e->tex;
        MLStrip* sp = &da.p[e->start];
        int n = e->end - e->start;
        for (int j = 0; j < n; j++) {
            MLStrip s = *sp++;
            if (s.flags == (ctx->vis[s.ref] & 0x7F)) {
                if (!open) {
                    fn_802C8B70(tex);
                    open = 1;
                    dl = fn_802DB2A8(n * 2);
                    if (!dl) {
                        fn_802C8D74();
                        open = 0;
                        break;
                    }
                }
                void* mt = ctx->mtx[s.mtx];
                if (cur != mt) {
                    cur = mt;
                    fn_802D0610(mt);
                    dl += 0x10;
                    cnt++;
                }
                MLPrim* pr = s.prim;
                fn_802D6B08(pr);
                fn_802CC550(tex, lbl_804C5480[s.ref], pr->size);
                dl += 0x10;
                cnt++;
            }
        }
        if (open) {
            fn_802DB2B0(cnt);
            fn_802C8D74();
        }
    }
    fn_802DB2A0();
    fn_801293D0();
    for (int o = r.first; o < r.end; o++) {
        m->objs[o]->Flush(m);
    }
}

void fn_802CDDC8(MList* m, u32 base) {
    SwapL(&m->magic);
    SwapL(&m->version);
    SwapL(&m->flags);
    u32* w = (u32*)&m->a;
    for (int i = 0; i < 10; i++) {
        SwapL(&w[i]);
    }
    w = (u32*)m->ranges;
    for (int i = 0; i < 20; i++) {
        SwapL(&w[i]);
    }
    SwapL(&m->numObjs);
    SwapL(&m->maxObjs);
    SwapL(&m->objs);
    Relocate(m->a.p, base);
    Relocate(m->b.p, base);
    Relocate(m->c.p, base);
    Relocate(m->d.p, base);
    int i;
    for (i = 0; i < m->d.n; i++) {
        MLStrip* s = &m->d.p[i];
        Swap16(&s->ref);
        Swap16(&s->flags);
        Swap16(&s->mtx);
        Swap16(&s->pad6);
        Swap32(&s->prim);
        Relocate(s->prim, base);
    }
    Relocate(m->e.p, base);
    for (i = 0; i < m->e.n; i++) {
        MLBatch* e = &m->e.p[i];
        Swap32(&e->start);
        Swap32(&e->end);
        e->tex = -2;
    }
    Relocate(m->objs, base);
    for (i = 0; i < m->numObjs; i++) {
        Swap32(&m->objs[i]);
    }
    m->flags |= 1;
}

}  // extern "C"

template <class T>
inline void Relocate(T*& p, u32 base) {
    if (p) {
        p = (T*)((u8*)p + base);
    }
}

template void Relocate<MLSphere>(MLSphere*&, u32);
template void Relocate<MLBox>(MLBox*&, u32);
template void Relocate<MLPiece>(MLPiece*&, u32);
template void Relocate<MLStrip>(MLStrip*&, u32);
template void Relocate<MLPrim>(MLPrim*&, u32);
template void Relocate<MLBatch>(MLBatch*&, u32);
template void Relocate<MLObj*>(MLObj**&, u32);
