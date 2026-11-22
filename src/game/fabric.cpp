// Fabric meshes (0x801F4CD8-0x801F7A0C, "mesh"): cloth flags and banners simulated as Verlet
// particle grids with distance sticks and pinned anchors. Uniform meshes share the global wind,
// non-uniform meshes ("flag") get a random per-particle wind field, and manual meshes are added from
// a value table. FabricTMH is the track model handler that creates them from track objects. Names
// are guesses apart from the strings.
#include "game/common.h"
#include "game/rmtx.h"
#include "game/vec3_init.h"
#include "game/plainvec.h"
#include "game/byteswap.h"
#include "game/mathutil.h"

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

extern "C" {
void* memset(void* p, int c, u32 n);
int strcasecmp(const char* a, const char* b);
double fabs(double x);
}

// One corner of a fabric quad (0x18).
struct FabricCorner {
    PVec pos;   // offset 0x0
    u32 color;  // offset 0xC
    f32 u;      // offset 0x10
    f32 v;      // offset 0x14
};

struct Quad {
    FabricCorner c[4];     // offset 0x0: ul, ur, ll, lr
    f32 colDensity;  // offset 0x60
    f32 rowDensity;  // offset 0x64
};

struct Stick {
    u16 a;     // offset 0x0
    u16 b;     // offset 0x2
    f32 rest;  // offset 0x4
};

struct Anchor {
    Anchor(void);
    u16 index;  // offset 0x0
    PVec pos;   // offset 0x4
};

struct DrawVert {
    f32 u;      // offset 0x0
    f32 v;      // offset 0x4
    u32 color;  // offset 0x8
};

struct Fabric {
    Vec3Storage* verts;      // offset 0x0
    Vec3Storage* cur;        // offset 0x4
    Vec3Storage* prev;       // offset 0x8
    DrawVert* draw;   // offset 0xC
    s32 count;        // offset 0x10
    Stick* sticks;    // offset 0x14
    s32 numSticks;    // offset 0x18
    Anchor* anchors;  // offset 0x1C
    s32 numAnchors;   // offset 0x20
    void* tex;        // offset 0x24
    f32 lod;          // offset 0x28
    s32 cols;         // offset 0x2C
    s32 rows;         // offset 0x30
    s32 id;           // offset 0x34
    u8 visible;       // offset 0x38
    f32 gravity;      // offset 0x3C
    f32 wind;         // offset 0x40
    f32 drag;         // offset 0x44
};

struct WindFabric {
    Fabric f;     // offset 0x0
    PVec random;  // offset 0x48
};

#include "game/tobj.h"

// Track object data for a fabric: corner records indexed by the three corner fields.
struct FabricData {
    u8 pad0[0x10];
    FabricCorner corners[3];  // offset 0x10
    s32 indices[3];  // offset 0x58
};

class UniformFabricMan {
public:
    UniformFabricMan(void);
    ~UniformFabricMan(void);
    void Update(void* ctx);
    void Draw(void);
    void Add(Quad* q, void* tex, ValueNode* vn, s32 id);

    Fabric fabrics[32];  // offset 0x0
    s32 count;           // offset 0x900
};

class NonUniformFabricMan {
public:
    NonUniformFabricMan(void);
    ~NonUniformFabricMan(void);
    void Update(void* ctx);
    void Draw(void);
    void Add(Quad* q, void* tex, ValueNode* vn, s32 id);

    WindFabric fabrics[8];  // offset 0x0
    s32 count;              // offset 0x2A0
};

class ManualFabricMan {
public:
    ManualFabricMan(void);
    ~ManualFabricMan(void);
    void Update(void);
    void Draw(void);
    void Add(ValueNode* vn, RMtx* m);

    Fabric fabrics[4];  // offset 0x0
    s32 count;          // offset 0x120
};

// fabric keeps its dead weak TrackMod and TrackInst tables (zero blocks after its own; FACTS 2026-11-17 08:29).
#define TRACKMOD_DECL
#define TRACKINST_DECL
#include "game/trackmod.h"
#include "game/trackinst.h"

class UniformFabricInst : public TrackInst {
public:
    virtual ~UniformFabricInst(void);
    virtual void Update(void* ctx);
    virtual int Kind(void);
    virtual void Draw2(void* ctx);
};

class NonUniformFabricInst : public TrackInst {
public:
    virtual ~NonUniformFabricInst(void);
    virtual void Update(void* ctx);
    virtual int Kind(void);
    virtual void Draw2(void* ctx);
};

class FabricTMH : public TrackMod {
public:
    virtual ~FabricTMH(void);
    virtual void Swap(TObj* o);
    virtual void Init(TObj* o);
    virtual void Draw(TObj* list);
    virtual void Add(void* scene, const TObj* o, s32 id);
};

extern "C" {
ValueNode* fn_802AE414(const char* name);
void fn_802E317C(const char* fmt, ...);
int DecodeBase64String(f32* out, int size, const char* text);
f32 PSVECSquareDistance(const void* a, const void* b);
f32 PSVECDistance(const void* a, const void* b);
f32 PSVECDotProduct(const void* a, const void* b);
void* fn_802CAC28(const char* name, int a);
void gxForgetTexture(void* tex);
double log2(double x);
f32 fn_801E7E24(void);
void fn_801E8C84(PVec* out);
void fn_801E8CB8(PVec* dir, f32* strength);
f32 fn_80287BC8(f32 lo, f32 hi);
f32 fn_802C9A4C(f32 lod);
void fn_802C8BD0(void* tex);
int fn_802C8DBC(int n);
void fn_802C8E00(f32 x, f32 y, f32 z, f32 u, f32 v, u32 color, int cont);
void fn_802C8E48(void);
void fn_802C8D74(void);
int fn_802CDB20(void* ctx, s32 id);
void fn_802CD320(void* scene, TrackInst* inst);
void* fn_802C9C24(void* model);
extern f32 __float_epsilon[];
}

ModuleTag lbl_8055EC08;
UniformFabricMan* lbl_8055EC10;
NonUniformFabricMan* lbl_8055EC14;
ManualFabricMan* lbl_8055EC18;
UniformFabricInst* lbl_8055EC1C;
NonUniformFabricInst* lbl_8055EC20;

static const char* lbl_8033AC9C[5] = {"top corners", "all corners", "top row", "left corners", "left column"};



static inline int ClampI(int v, int lo, int hi) {
    int a = v > lo ? v : lo;
    return a < hi ? a : hi;
}

static inline int ToByte(const f32& x) {
    int v = (int)(255.0f * x);
    v = v > 0 ? v : 0;
    return v < 0xFF ? v : 0xFF;
}
static inline int ToByte2(f32 x) {
    int v = MaxI((int)(255.0f * x), 0);
    return v < 0xFF ? v : 0xFF;
}

extern "C" {

void fn_801F4CD8(void) {
    lbl_8055EC10 = new ("UniformFabricMan") UniformFabricMan;
    lbl_8055EC14 = new ("NonUniformFabricMan") NonUniformFabricMan;
    lbl_8055EC18 = new ("ManualFabricMan") ManualFabricMan;
    lbl_8055EC1C = 0;
    lbl_8055EC20 = 0;
}

void fn_801F4D64(void) {
    delete lbl_8055EC18;
    lbl_8055EC18 = 0;
    delete lbl_8055EC14;
    lbl_8055EC14 = 0;
    delete lbl_8055EC10;
    lbl_8055EC10 = 0;
    lbl_8055EC1C = 0;
    lbl_8055EC20 = 0;
}

void fn_801F4DC4(void) { lbl_8055EC18->Update(); }

void fn_801F4DCC(void) { lbl_8055EC18->Draw(); }

void fn_801F4DD4(const char* table, RMtx* m) {
    ValueNode* vn = fn_802AE414(table);
    if (!vn) {
        fn_802E317C("mesh: could not find table %s", table);
    }
    lbl_8055EC18->Add(vn, m);
}

int fn_801F4E44(const char* name) {
    u32 i;
    for (i = 0; i < 5; i++) {
        if (strcasecmp(name, lbl_8033AC9C[i]) == 0) {
            return i + 1;
        }
    }
    fn_802E317C("mesh: invalid anchor style %s", name);
    return 0;
}

u32 fn_801F4ECC(ValueNode* vn) {
    u32 flags = 0;
    if (vn->GetBool("flag", 0)) {
        flags |= 1;
    }
    if (vn->GetBool("career_win", 0)) {
        flags |= 2;
    }
    if (vn->GetBool("rigid", 0)) {
        flags |= 4;
    }
    if ((flags & 1) && (flags & 4)) {
        fn_802E317C("mesh: flags can't be rigid");
    }
    return flags;
}

static inline void MulVec(PVec* out, const PVec* in, const RMtx* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m->rot.x.x, in->y, m->rot.y.x, in->z, m->rot.z.x);
        f32 y = Dot3(in->x, m->rot.x.y, in->y, m->rot.y.y, in->z, m->rot.z.y);
        f32 z = Dot3(in->x, m->rot.x.z, in->y, m->rot.y.z, in->z, m->rot.z.z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m->rot.x.x, in->y, m->rot.y.x, in->z, m->rot.z.x);
        out->y = Dot3(in->x, m->rot.x.y, in->y, m->rot.y.y, in->z, m->rot.z.y);
        out->z = Dot3(in->x, m->rot.x.z, in->y, m->rot.y.z, in->z, m->rot.z.z);
    }
}
/* Helper names unproven: NodeVec3 / NodePoint (by-value wrappers, fn_801F4F9C) */
static inline ValVec3 NodeVec3(ValueNode* vn, const char* k) { return vn->GetVec3(k); }
static inline ValPoint NodePoint(ValueNode* vn, const char* k) { return vn->GetPoint(k); }
void fn_801F4F9C(FabricCorner* out, ValueNode* vn, const RMtx* m) {
    PVec p = *(const PVec*)&static_cast<const ValVec3&>(NodeVec3(vn, "offset"));  // address of the bound temporary
    ValPoint uv = NodePoint(vn, "uv");
    ValVec3 c = vn->GetVec3Or("color", ValV3(1.0f, 1.0f, 1.0f));
    MulVec(&out->pos, &p, m);
    out->pos.x += m->pos.x;
    out->pos.y += m->pos.y;
    out->pos.z += m->pos.z;
    out->u = uv.x;
    out->v = uv.y;
    {
        u8 b = ToByte(c.z);
        u8 g = ToByte(c.y);
        out->color = 0xFF000000 | ((u8)ToByte2(c.x) << 16) | (g << 8) | b;
    }
}

int fn_801F52BC(ValueNode* vn, const char* key, f32* out, int size) {
    int r = 0;
    if (vn->Has(key)) {
        r = DecodeBase64String(out, size, vn->GetValue(key));
    }
    return r;
}

void fn_801F5344(Quad* q, ValueNode* vn, const RMtx* m) {
    if (!vn->FindChild("ul") || !vn->FindChild("ur") || !vn->FindChild("ll") || !vn->FindChild("lr")) {
        fn_802E317C("mesh: table %s is missing corner geometry");
    }
    fn_801F4F9C(&q->c[0], vn->FindChild("ul"), m);
    fn_801F4F9C(&q->c[1], vn->FindChild("ur"), m);
    fn_801F4F9C(&q->c[2], vn->FindChild("ll"), m);
    fn_801F4F9C(&q->c[3], vn->FindChild("lr"), m);
    if (!fn_801F52BC(vn, "col_density", &q->colDensity, 4)) {
        q->colDensity = 0.6f;
    }
    if (!fn_801F52BC(vn, "row_density", &q->rowDensity, 4)) {
        q->rowDensity = 0.6f;
    }
}

// Helper name unproven: one distance constraint between two particles.
static inline void SetStick(Stick* s, const Vec3Storage* v, int a, int b) {
    s->a = a;
    s->b = b;
    s->rest = PSVECSquareDistance(&v[a], &v[b]);
}
// PROVISIONAL: row-base scope and direct endpoint input ownership; exact code does not prove original source.
void fn_801F5508(Fabric* f) {
    Stick* s;
    int n;
    n = (f->rows - 1) * f->cols + f->rows * (f->cols - 1);
    s = new ("fabric sticks") Stick[n];
    int k = 0;
    for (int r = 0; r < f->rows; r++) {
        int row = r * f->cols;
        for (int c = 1; c < f->cols; c++) {
            int b = row + c;
            int a = b - 1;
            SetStick(&s[k++], f->cur, a, b);
        }
    }
    for (int c = 0; c < f->cols; c++) {
        for (int r = 1; r < f->rows; r++) {
            SetStick(&s[k++], f->cur, c + (r - 1) * f->cols, c + r * f->cols);
        }
    }
    f->sticks = s;
    f->numSticks = n;
}

// PROVISIONAL: indexed point component ownership; helper name/type/layout remain unproven.
static inline void SetAnchor(Anchor* a, const Vec3Storage* v, int i) {
    a->index = i;
    a->pos.x = v[i].x;
    a->pos.y = v[i].y;
    a->pos.z = v[i].z;
}

/* Helper name unproven */
static inline int Rows(Fabric* f) {
    return f->rows;
}

/* Helper name unproven */
static inline int AnchorCount(Fabric* f, int style) {
    int rows = Rows(f);
    int cols = f->cols;
    int n;
    switch (style) {
    case 1:
    case 4:
        n = 2;
        break;
    case 2:
        n = 4;
        break;
    case 3:
        n = cols;
        break;
    case 5:
        n = rows;
        break;
    default:
        fn_802E317C("mesh: invalid anchor style %d", style);
        n = 0;
        break;
    }
    return n;
}

// PROVISIONAL: running output counter owns the first two anchors; source ownership remains unproven.
void fn_801F5664(Fabric* f, int style) {
    int n = AnchorCount(f, style);
    Anchor* a;
    int k;
    int i;
    a = new ("fabric anchors") Anchor[n];
    k = 0;
    switch (style) {
    case 2:
        SetAnchor(&a[k++], f->cur, (f->rows - 1) * f->cols);
        SetAnchor(&a[k++], f->cur, f->cols * f->rows - 1);
    case 1:
        SetAnchor(&a[k++], f->cur, 0);
        SetAnchor(&a[k++], f->cur, f->cols - 1);
        break;
    case 3:
        for (i = 0; i < f->cols; i++) {
            SetAnchor(&a[k++], f->cur, i);
        }
        break;
    case 4:
        SetAnchor(&a[0], f->cur, 0);
        SetAnchor(&a[1], f->cur, (f->rows - 1) * f->cols);
        break;
    case 5:
        for (i = 0; i < f->rows; i++) {
            SetAnchor(&a[k++], f->cur, f->cols * i);
        }
        break;
    default:
        fn_802E317C("mesh: invalid anchor style %d", style);
        break;
    }
    f->anchors = a;
    f->numAnchors = n;
}
}

Anchor::Anchor(void) {}


// PROVISIONAL: color input ownership and scalar helper boundaries are unproven.
// Earlier converted-start reference is superseded by the existing float Lerp helper.


// PROVISIONAL: real word inputs use start/end order; original API and scopes unproven.
static inline u32 LerpColor(u32 a,u32 b,f32 t) {
    u32 c;
    u8* pa = (u8*)&a;
    u8* pb = (u8*)&b;
    u8* pc = (u8*)&c;
    pc[0] = Lerp(pa[0], pb[0], t);
    pc[1] = Lerp(pa[1], pb[1], t);
    pc[2] = Lerp(pa[2], pb[2], t);
    pc[3] = Lerp(pa[3], pb[3], t);
    return c;
}



static inline void LerpVert(DrawVert* out, FabricCorner a, FabricCorner b, f32 t) {
    out->u = Lerp(a.u, b.u, t);
    out->v = Lerp(a.v, b.v, t);
    out->color = LerpColor(a.color,b.color,t);
}

static inline void LerpVert2(DrawVert* out, const DrawVert& b, const DrawVert& a, f32 t) {
    out->u = Lerp(a.u, b.u, t);
    out->v = Lerp(a.v, b.v, t);
    out->color = LerpColor(a.color,b.color,t);
}

// PROVISIONAL: direct position values and row-position construction order; original ownership unproven.
// PROVISIONAL: row vector declarations own storage before draw locals; source scopes unproven.
extern "C" void fn_801F5970(Fabric* f, const Quad* q) {
    int n = f->rows * f->cols;
    int r;
    f->verts = new ("fabric verts") Vec3Storage[n * 2];
    f->cur = f->verts;
    f->prev = f->verts + n;
    f->draw = (DrawVert*)new ("fabric draw tab") u8[n * sizeof(DrawVert)];
    f->count = n;
    for (r = 0; r < f->rows; r++) {
        f32 t = (f32)r / (f32)(f->rows - 1);
        int c;
        const ValV3 lp(Lerp(q->c[0].pos.x,q->c[2].pos.x,t), Lerp(q->c[0].pos.y,q->c[2].pos.y,t), Lerp(q->c[0].pos.z,q->c[2].pos.z,t));
        const ValV3 rp(Lerp(q->c[1].pos.x,q->c[3].pos.x,t), Lerp(q->c[1].pos.y,q->c[3].pos.y,t), Lerp(q->c[1].pos.z,q->c[3].pos.z,t));
        DrawVert ld;
        DrawVert rd;
        LerpVert(&ld, q->c[0], q->c[2], t);
        LerpVert(&rd, q->c[1], q->c[3], t);
        for (c = 0; c < f->cols; c++) {
            f32 s = (f32)c / (f32)(f->cols - 1);
            int i = r * f->cols + c;
            ValV3 p(Lerp(lp.x,rp.x,s), Lerp(lp.y,rp.y,s), Lerp(lp.z,rp.z,s));
            Vec3Storage* pv = &f->prev[i];
            *(PVec*)pv = p;
            f->cur[i] = *pv;
            LerpVert2(&f->draw[i], rd, ld, s);
        }
    }
}

extern "C" {

void fn_801F602C(Fabric* f, const Quad* q, const char* tex, int style, u32 flags) {
    f32 w = PSVECDistance(&q->c[2].pos, &q->c[3].pos);
    f32 h;
    f32 d;
    f32 umin;
    f32 umax;
    f32 vmin;
    f32 vmax;
    f32 span;
    d = PSVECDistance(&q->c[0].pos, &q->c[1].pos);
    if (d > w) {
        w = d;
    }
    h = PSVECDistance(&q->c[1].pos, &q->c[3].pos);
    d = PSVECDistance(&q->c[0].pos, &q->c[2].pos);
    if (d > h) {
        h = d;
    }
    if (flags & 4) {
        f->cols = 2;
        f->rows = 2;
    } else if (!(flags & 1)) {
        f->cols = ClampI((int)(w * q->colDensity), 2, 8);
        f->rows = ClampI((int)(h * q->rowDensity), 2, 4);
    } else {
        f->cols = ClampI((int)(3.0f * (w * q->colDensity)), 2, 8);
        f->rows = ClampI((int)(3.0f * (h * q->rowDensity)), 2, 4);
    }
    umin = MinF(q->c[0].u, MinF(q->c[1].u, MinF(q->c[2].u, q->c[3].u)));
    umax = MaxF(q->c[0].u, MaxF(q->c[1].u, MaxF(q->c[2].u, q->c[3].u)));
    vmin = MinF(q->c[0].v, MinF(q->c[1].v, MinF(q->c[2].v, q->c[3].v)));
    vmax = MaxF(q->c[0].v, MaxF(q->c[1].v, MaxF(q->c[2].v, q->c[3].v)));
    f->tex = fn_802CAC28(tex, 0);
    f64 dv = __fabs(vmax - vmin);
    f64 du = __fabs(umax - umin);
    span = MaxF((f32)du, (f32)dv);
    f->lod = log2(span / MaxF(w, h));
    fn_801F5970(f, q);
    fn_801F5508(f);
    fn_801F5664(f, style);
}

void fn_801F6358(Fabric* f) {
    delete[] (u8*)f->draw;
    delete[] f->verts;
}

void fn_801F639C(Fabric* f) {
    delete[] f->anchors;
    delete[] f->sticks;
    fn_801F6358(f);
    gxForgetTexture(f->tex);
}

// Verlet step with one force for every particle.
// Helper name unproven: scales the force by the 1/60 step squared.
static inline void ScaleP(PVec* o, const PVec* v, f32 k) {
    o->x = v->x * k;
    o->y = v->y * k;
    o->z = v->z * k;
}
void fn_801F63F0(Fabric* f, const PVec* force, f32 drag) {
    f32 k1;
    PVec a;
    Vec3Storage* c;
    Vec3Storage* p;
    PVec t;
    f32 k2;
    int i;
    k2 = -(1.0f - drag);
    k1 = 2.0f - drag;
    ScaleP(&a, force, 0.0002777778f);
    for (i = 0; i < f->count; i++) {
        Vec3Storage* c;
        c = &f->cur[i];
        p = &f->prev[i];
        t.x = a.x + c->x * k1;
        t.y = a.y + c->y * k1;
        t.z = a.z + c->z * k1;
        p->x = t.x + p->x * k2;
        p->y = t.y + p->y * k2;
        p->z = t.z + p->z * k2;
    }
    {
        c = f->cur;
        f->cur = f->prev;
        f->prev = c;
    }
}

// Verlet step with a force per particle.
// Helper names unproven: scale-and-add steps of the Verlet integrator.
static inline void AddS(PVec* o, const PVec* v, f32 k) {
    o->x += v->x * k;
    o->y += v->y * k;
    o->z += v->z * k;
}
static inline void AddS2(PVec* o, const PVec* a, const PVec* v, f32 k) {
    o->x = a->x + v->x * k;
    o->y = a->y + v->y * k;
    o->z = a->z + v->z * k;
}
void fn_801F64E4(Fabric* f, const PVec* forces, f32 drag) {
    f32 k1 = 2.0f - drag;
    f32 k2 = -(1.0f - drag);
    f32 step = 0.0002777778f;
    PVec t;
    int i;
    for (i = 0; i < f->count; i++, forces++) {
        Vec3Storage* p;
        Vec3Storage* c = &f->cur[i];
        p = &f->prev[i];
        ScaleP(&t, (const PVec*)c, k1);
        AddS(&t, (const PVec*)p, k2);
        AddS2((PVec*)p, &t, forces, step);
    }
    {
        Vec3Storage* c = f->cur;
        f->cur = f->prev;
        f->prev = c;
    }
}

void fn_801F65D0(Fabric* f) {
    f32 dx;
    f32 half = 0.5f;
    f32 negativeHalf = -0.5f;
    int i;
    for (i = 0; i < f->numSticks; i++) {
        Stick* s = &f->sticks[i];
        Vec3Storage* a = &f->cur[s->a];
        Vec3Storage* b = &f->cur[s->b];
        PVec d;
        f32 k;
        f32 kb;
        f32 ka;
        d.x = b->x - a->x;
        d.y = b->y - a->y;
        d.z = b->z - a->z;
        k = half - s->rest / (s->rest + PSVECDotProduct(&d, &d));
        ka = half * k;
        kb = negativeHalf * k;
        dx = d.x;
        a->x += dx * ka;
        a->y += d.y * ka;
        a->z += d.z * ka;
        b->x += d.x * kb;
        b->y += d.y * kb;
        b->z += d.z * kb;
    }
}

void fn_801F6728(Fabric* f) {
    int i;
    for (i = 0; i < f->numAnchors; i++) {
        Anchor* a = &f->anchors[i];
        *(PVec*)&f->cur[a->index] = a->pos;
    }
}

void fn_801F6778(PVec* out, int n, f32 gravity, f32 wind, const PVec* random) {
    f32 s = 0.0f;
    PVec dir;
    f32 wx;
    f32 wy;
    f32 wz;
    f32 rx;
    f32 ry;
    f32 rz;
    int i;
    fn_801E8CB8(&dir, &s);
    s *= wind;
    wx = dir.x * s;
    wy = dir.y * s - gravity;
    wz = dir.z * s;
    rx = random->x * s;
    ry = random->y * s;
    rz = random->z * s;
    for (i = 0; i < n; i++, out++) {
        out->x = wx + fn_80287BC8(-rx, rx);
        out->y = wy + fn_80287BC8(-ry, ry);
        out->z = wz + fn_80287BC8(-rz, rz);
    }
}

// Helper name unproven. Submit the second vertex of the pair.
static inline void SubmitVertex(const DrawVert& vertex, const Vec3Storage& position, int c) {
    const f32 v = vertex.v;
    const f32 u = vertex.u;
    fn_802C8E00(position.x, position.y, position.z, u, v, vertex.color, c > 0);
}

void fn_801F6880(const Fabric* f) {
    f32 old = fn_802C9A4C(f->lod);
    fn_802C8BD0(f->tex);
    if (fn_802C8DBC((f->rows - 1) * (f->cols * 2))) {
        int r;
        for (r = 1; r < f->rows; r++) {
            int c;
            for (c = 0; c < f->cols; c++) {
                int i = c + (r - 1) * f->cols;
                const Vec3Storage* p = &f->cur[i];
                const DrawVert* d = &f->draw[i];
                const f32 x = p->x;
                const f32 u = d->u;
                const f32 v = d->v;
                fn_802C8E00(x, p->y, p->z, u, v, d->color, c > 0);
                i = c + r * f->cols;
                SubmitVertex(f->draw[i], f->cur[i], c);
            }
        }
        fn_802C8E48();
    }
    fn_802C8D74();
    fn_802C9A4C(old);
}
}

UniformFabricMan::UniformFabricMan(void) {
    memset(fabrics, 0, sizeof(fabrics));
    count = 0;
}

UniformFabricMan::~UniformFabricMan(void) {
    int i;
    for (i = 0; i < count; i++) {
        fn_801F639C(&fabrics[i]);
    }
    count = 0;
}

static inline BOOL Paused(void) { return (f32)__fabs(fn_801E7E24()) <= __float_epsilon[0]; }

void UniformFabricMan::Update(void* ctx) {
    f32 gravity;
    BOOL run = !Paused();
    Fabric* f = fabrics;
    int i;
    for (i = 0; i < count; f++, i++) {
        if (run) {
            f32 wind = f->wind;
            gravity = f->gravity;
            PVec force;
            PVec w;
            fn_801E8C84(&w);
            force.x = w.x * wind;
            force.y = w.y * wind;
            force.z = w.z * wind;
            force.y -= gravity;
            fn_801F63F0(f, &force, f->drag);
            fn_801F65D0(f);
            fn_801F6728(f);
        }
        f->visible = fn_802CDB20(ctx, f->id);
    }
}

void UniformFabricMan::Draw(void) {
    Fabric* f = fabrics;
    int i;
    for (i = 0; i < count; f++, i++) {
        if (f->visible) {
            fn_801F6880(f);
        }
    }
}

void UniformFabricMan::Add(Quad* q, void* tex, ValueNode* vn, s32 id) {
    Fabric* f;
    int style;
    if (count >= 32) {
        fn_802E317C("mesh: exceeded uniform fabric limit");
    }
    f = &fabrics[count];
    style = fn_801F4E44(vn->GetValue("anchor"));
    fn_801F602C(f, q, (const char*)tex, style, fn_801F4ECC(vn));
    f->id = id;
    f->visible = 1;
    f->gravity = 9.81f * vn->GetFloat("gravity");
    f->wind = vn->GetFloat("wind");
    f->drag = vn->GetFloat("drag");
    count++;
}

NonUniformFabricMan::NonUniformFabricMan(void) {
    memset(fabrics, 0, sizeof(fabrics));
    count = 0;
}

NonUniformFabricMan::~NonUniformFabricMan(void) {
    int i;
    for (i = 0; i < count; i++) {
        fn_801F639C(&fabrics[i].f);
    }
    count = 0;
}

void NonUniformFabricMan::Update(void* ctx) {
    BOOL run = !Paused();
    WindFabric* f = fabrics;
    int i;
    for (i = 0; i < count; f++, i++) {
        if (run) {
            PVec forces[32];
            fn_801F6778(forces, f->f.count, f->f.gravity, f->f.wind, &f->random);
            fn_801F64E4(&f->f, forces, f->f.drag);
            fn_801F65D0(&f->f);
            fn_801F6728(&f->f);
        }
        f->f.visible = fn_802CDB20(ctx, f->f.id);
    }
}

void NonUniformFabricMan::Draw(void) {
    int i;
    for (i = 0; i < count; i++) {
        if (fabrics[i].f.visible) {
            fn_801F6880(&fabrics[i].f);
        }
    }
}

void NonUniformFabricMan::Add(Quad* q, void* tex, ValueNode* vn, s32 id) {
    WindFabric* f;
    int style;
    if (count >= 8) {
        fn_802E317C("mesh: exceeded non-uniform fabric limit");
    }
    f = &fabrics[count];
    style = fn_801F4E44(vn->GetValue("anchor"));
    fn_801F602C(&f->f, q, (const char*)tex, style, fn_801F4ECC(vn));
    f->f.id = id;
    f->f.visible = 1;
    f->f.gravity = 9.81f * vn->GetFloat("gravity");
    f->f.wind = vn->GetFloat("wind");
    f->f.drag = vn->GetFloat("drag");
    {
        const ValVec3& r = vn->GetVec3("random_wind");
        f->random.x = r.x;
        f->random.y = r.y;
        f->random.z = r.z;
    }
    count++;
}

ManualFabricMan::ManualFabricMan(void) {
    memset(fabrics, 0, sizeof(fabrics));
    count = 0;
}

ManualFabricMan::~ManualFabricMan(void) {
    int i;
    for (i = 0; i < count; i++) {
        fn_801F639C(&fabrics[i]);
    }
    count = 0;
}

void ManualFabricMan::Update(void) {
    f32 gravity;
    Fabric* f = fabrics;
    int i;
    for (i = 0; i < count; f++, i++) {
        f32 wind = f->wind;
        gravity = f->gravity;
        PVec force;
        PVec w;
        fn_801E8C84(&w);
        force.x = w.x * wind;
        force.y = w.y * wind;
        force.z = w.z * wind;
        force.y -= gravity;
        fn_801F63F0(f, &force, f->drag);
        fn_801F65D0(f);
        fn_801F6728(f);
    }
}

void ManualFabricMan::Draw(void) {
    Fabric* f = fabrics;
    int i;
    for (i = 0; i < count; f++, i++) {
        fn_801F6880(f);
    }
}

void ManualFabricMan::Add(ValueNode* vn, RMtx* m) {
    u32 flags;
    Fabric* f;
    const char* tex;
    int style;
    Quad q;
    if (count >= 4) {
        fn_802E317C("mesh: exceeded manual fabric limit");
    }
    flags = fn_801F4ECC(vn);
    if (flags & 1) {
        fn_802E317C("mesh: only uniform meshes can be added manually");
    }
    f = &fabrics[count];
    tex = vn->GetValue("texture");
    style = fn_801F4E44(vn->GetValue("anchor"));
    fn_801F5344(&q, vn, m);
    fn_801F602C(f, &q, tex, style, flags);
    f->id = -1;
    f->visible = 1;
    f->gravity = 9.81f * vn->GetFloat("gravity");
    f->wind = vn->GetFloat("wind");
    f->drag = vn->GetFloat("drag");
    count++;
}

void UniformFabricInst::Update(void* ctx) { lbl_8055EC10->Update(ctx); }

void UniformFabricInst::Draw2(void* ctx) { lbl_8055EC10->Draw(); }

void NonUniformFabricInst::Update(void* ctx) { lbl_8055EC14->Update(ctx); }

void NonUniformFabricInst::Draw2(void* ctx) { lbl_8055EC14->Draw(); }

extern "C" {

void fn_801F7400(Quad* q, const u8* data, ValueNode* vn) {
    const FabricData* mesh = (const FabricData*)data;
    const FabricCorner& c0 = mesh->corners[mesh->indices[0]];
    q->c[0] = c0;
    const FabricCorner& c1 = mesh->corners[mesh->indices[1]];
    q->c[1] = c1;
    const FabricCorner& c2 = mesh->corners[mesh->indices[2]];
    q->c[2] = c2;
    q->c[3].pos.x = q->c[2].pos.x + q->c[1].pos.x;
    q->c[3].pos.y = q->c[2].pos.y + q->c[1].pos.y;
    q->c[3].pos.z = q->c[2].pos.z + q->c[1].pos.z;
    q->c[3].pos.x -= q->c[0].pos.x;
    q->c[3].pos.y -= q->c[0].pos.y;
    q->c[3].pos.z -= q->c[0].pos.z;
    q->c[3].u = q->c[1].u + q->c[2].u - q->c[0].u;
    q->c[3].v = q->c[1].v + q->c[2].v - q->c[0].v;
    q->c[3].color = c1.color;
    if (!fn_801F52BC(vn, "col_density", &q->colDensity, 4)) {
        q->colDensity = 0.6f;
    }
    if (!fn_801F52BC(vn, "row_density", &q->rowDensity, 4)) {
        q->rowDensity = 0.6f;
    }
}

void fn_801F75A4(TObj* o, u8* data, void* scene, s32 id) {
    ValueNode* vn = fn_802AE414((const char*)data);
    void* tex;
    Quad q;
    if (!vn) {
        fn_802E317C("mesh: could not find table %s", data);
    }
    tex = fn_802C9C24(o->models[1]);
    fn_801F7400(&q, data, vn);
    if (!(fn_801F4ECC(vn) & 1)) {
        lbl_8055EC10->Add(&q, tex, vn, id);
        if (scene && !lbl_8055EC1C) {
            UniformFabricInst* inst = new ("uniformfabricMLO") UniformFabricInst;
            lbl_8055EC1C = inst;
            fn_802CD320(scene, inst);
        }
    } else {
        lbl_8055EC14->Add(&q, tex, vn, id);
        if (scene && !lbl_8055EC20) {
            NonUniformFabricInst* inst = new ("nonuniformMLO") NonUniformFabricInst;
            lbl_8055EC20 = inst;
            fn_802CD320(scene, inst);
        }
    }
}
}

static inline void SwapKeys(u8* p, int n) {
    for (int i = 0; i < n; i++, p += 0x18) {
        SwapWord(p);
        SwapWord(p + 4);
        SwapWord(p + 8);
        SwapWord(p + 12);
        SwapWord(p + 16);
        SwapWord(p + 20);
    }
}

void FabricTMH::Swap(TObj* o) {
    u8* d = (u8*)o;
    SwapBytesN(d + 0x2C, 16);
    SwapKeys(d + 0x3C, 3);
    SwapBytesN(d + 0x84, 12);
}

void FabricTMH::Init(TObj* o) {}

void FabricTMH::Add(void* scene, const TObj* o, s32 id) { fn_801F75A4(const_cast<TObj*>(o), (u8*)o + 0x2C, scene, id); }

extern "C" TrackMod* fn_801F78F0(void) { return new ("FabricTMH") FabricTMH; }

FabricTMH::~FabricTMH(void) {}

NonUniformFabricInst::~NonUniformFabricInst(void) {}

UniformFabricInst::~UniformFabricInst(void) {}

int UniformFabricInst::Kind(void) { return 3; }

int NonUniformFabricInst::Kind(void) { return 3; }

void FabricTMH::Draw(TObj* list) {}
