// Fabric meshes (0x801F4CD8-0x801F7A0C, "mesh"): cloth flags and banners simulated as Verlet
// particle grids with distance sticks and pinned anchors. Uniform meshes share the global wind,
// non-uniform meshes ("flag") get a random per-particle wind field, and manual meshes are added from
// a value table. FabricTMH is the track model handler that creates them from track objects. Names
// are guesses apart from the strings.
#include "game/common.h"

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

extern "C" {
void* memset(void* p, int c, u32 n);
int strcasecmp(const char* a, const char* b);
double fabs(double x);
}

struct TVec {
    TVec(void);
    f32 x, y, z;
};

struct PVec {
    f32 x, y, z;
};

// One corner of a fabric quad (0x18).
struct Corner {
    PVec pos;   // offset 0x0
    u32 color;  // offset 0xC
    f32 u;      // offset 0x10
    f32 v;      // offset 0x14
};

struct Quad {
    Corner c[4];     // offset 0x0: ul, ur, ll, lr
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
    TVec* verts;      // offset 0x0
    TVec* cur;        // offset 0x4
    TVec* prev;       // offset 0x8
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

struct TMtx {
    f32 m[3][3];
    f32 t[3];
};

struct TObj {
    u8 pad0[8];
    void** model;  // offset 0x8
};

// Track object data for a fabric: corner records indexed by the three corner fields.
struct FabricData {
    u8 pad0[0x10];
    Corner corners[3];  // offset 0x10
    u8 pad58[0x58 - 0x58];
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
    void Add(ValueNode* vn, TMtx* m);

    Fabric fabrics[4];  // offset 0x0
    s32 count;          // offset 0x120
};

class TrackInst {
public:
    virtual ~TrackInst(void) {}
    virtual void Draw(void* ctx);
    virtual int Kind(void);
    virtual void Draw2(void* ctx);
};

class UniformFabricInst : public TrackInst {
public:
    virtual ~UniformFabricInst(void);
    virtual void Draw(void* ctx);
    virtual int Kind(void);
    virtual void Draw2(void* ctx);
};

class NonUniformFabricInst : public TrackInst {
public:
    virtual ~NonUniformFabricInst(void);
    virtual void Draw(void* ctx);
    virtual int Kind(void);
    virtual void Draw2(void* ctx);
};

class TrackMod {
public:
    virtual ~TrackMod(void) {}
    virtual void Swap(TObj* o);
    virtual void Init(TObj* o);
    virtual void Unk14(void);
    virtual void Apply(void);
    virtual void Unk1C(void);
    virtual void Draw(TObj* list);
    virtual void DrawTree(void* tree);
    virtual void Add(void* scene, TObj* o, s32 id);
    virtual void Link(int a, void* b, int c, void* d, int e);
};

class FabricTMH : public TrackMod {
public:
    virtual ~FabricTMH(void);
    virtual void Swap(TObj* o);
    virtual void Init(TObj* o);
    virtual void Draw(TObj* list);
    virtual void Add(void* scene, TObj* o, s32 id);
};

extern "C" {
ValueNode* fn_802AE414(const char* name);
void fn_802E317C(const char* fmt, ...);
int fn_8012F9E8(f32* out, int size, const char* text);
f32 PSVECSquareDistance(const void* a, const void* b);
f32 PSVECDistance(const void* a, const void* b);
f32 PSVECDotProduct(const void* a, const void* b);
void* fn_802CAC28(const char* name, int a);
void fn_802CAC54(void* tex);
double log2(double x);
f32 fn_801E7E24(void);
void fn_801E8C84(PVec* out);
void fn_801E8CB8(PVec* dir, f32* strength);
f32 fn_80287BC8(f32 lo, f32 hi);
f32 fn_802C9A4C(f32 lod);
void fn_802C8BD0(void* tex);
int fn_802C8DBC(int n);
void fn_802C8E00(u32 color, int cont, f32 x, f32 y, f32 z, f32 u, f32 v);
void fn_802C8E48(void);
void fn_802C8D74(void);
int fn_802CDB20(void* ctx, s32 id);
void fn_802CD320(void* scene, TrackInst* inst);
void* fn_802C9C24(void* model);
extern f32 lbl_80558DC4[];
}

ModuleTag lbl_8055EC08;
UniformFabricMan* lbl_8055EC10;
NonUniformFabricMan* lbl_8055EC14;
ManualFabricMan* lbl_8055EC18;
UniformFabricInst* lbl_8055EC1C;
NonUniformFabricInst* lbl_8055EC20;

static const char* lbl_8033AC9C[5] = {"top corners", "all corners", "top row", "left corners", "left column"};

static inline f32 FMin(f32 a, f32 b) { return a < b ? a : b; }

static inline f32 FMax(f32 a, f32 b) { return a > b ? a : b; }

static inline int ClampI(int v, int lo, int hi) {
    int a = v > lo ? v : lo;
    return a < hi ? a : hi;
}

static inline int ToByte(f32 x) {
    int v = (int)(255.0f * x);
    v = v < 0 ? 0 : v;
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

void fn_801F4DD4(const char* table, TMtx* m) {
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

void fn_801F4F9C(Corner* out, ValueNode* vn, const TMtx* m) {
    ValVec3 off = vn->GetVec3("offset");
    PVec p;
    ValPoint uv;
    ValVec3 c;
    ValVec3 white;
    p.x = off.x;
    p.y = off.y;
    p.z = off.z;
    uv = vn->GetPoint("uv");
    white.x = 1.0f;
    white.y = 1.0f;
    white.z = 1.0f;
    c = vn->GetVec3Or("color", white);
    if ((PVec*)out == &p) {
        PVec t;
        t.x = p.z * m->m[2][0] + (p.x * m->m[0][0] + p.y * m->m[1][0]);
        t.y = p.z * m->m[2][1] + (p.x * m->m[0][1] + p.y * m->m[1][1]);
        t.z = p.z * m->m[2][2] + (p.x * m->m[0][2] + p.y * m->m[1][2]);
        out->pos = t;
    } else {
        out->pos.x = p.z * m->m[2][0] + (p.x * m->m[0][0] + p.y * m->m[1][0]);
        out->pos.y = p.z * m->m[2][1] + (p.x * m->m[0][1] + p.y * m->m[1][1]);
        out->pos.z = p.z * m->m[2][2] + (p.x * m->m[0][2] + p.y * m->m[1][2]);
    }
    out->pos.x += m->t[0];
    out->pos.y += m->t[1];
    out->pos.z += m->t[2];
    out->u = uv.x;
    out->v = uv.y;
    {
        int b = ToByte(c.z);
        int g = ToByte(c.y);
        int r = ToByte(c.x);
        out->color = 0xFF000000 | ((u8)r << 16) | ((u8)g << 8) | (u8)b;
    }
}

int fn_801F52BC(ValueNode* vn, const char* key, f32* out, int size) {
    int r = 0;
    if (vn->Has(key)) {
        r = fn_8012F9E8(out, size, vn->GetValue(key));
    }
    return r;
}

void fn_801F5344(Quad* q, ValueNode* vn, const TMtx* m) {
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

void fn_801F5508(Fabric* f) {
    int n = (f->rows - 1) * f->cols + f->rows * (f->cols - 1);
    Stick* s = new ("fabric sticks") Stick[n];
    int k = 0;
    int r;
    int c;
    for (r = 0; r < f->rows; r++) {
        for (c = 1; c < f->cols; c++) {
            int b = r * f->cols + c;
            int a = b - 1;
            s[k].a = a;
            s[k].b = b;
            s[k].rest = PSVECSquareDistance(&f->cur[a], &f->cur[b]);
            k++;
        }
    }
    for (c = 0; c < f->cols; c++) {
        for (r = 1; r < f->rows; r++) {
            int a = c + (r - 1) * f->cols;
            int b = c + r * f->cols;
            s[k].a = a;
            s[k].b = b;
            s[k].rest = PSVECSquareDistance(&f->cur[a], &f->cur[b]);
            k++;
        }
    }
    f->sticks = s;
    f->numSticks = n;
}

static inline void SetAnchor(Fabric* f, Anchor* a, int i) {
    a->index = i;
    a->pos = *(PVec*)&f->cur[i];
}

void fn_801F5664(Fabric* f, int style) {
    int rows = f->rows;
    int n = f->cols;
    Anchor* a;
    int k;
    int i;
    switch (style) {
    case 1:
        n = 2;
        break;
    case 2:
        n = 4;
        break;
    case 3:
        break;
    case 4:
        n = 2;
        break;
    case 5:
        n = rows;
        break;
    default:
        fn_802E317C("mesh: invalid anchor style %d", style);
        n = 0;
        break;
    }
    a = new ("fabric anchors") Anchor[n];
    k = 0;
    switch (style) {
    case 2:
        SetAnchor(f, &a[0], (f->rows - 1) * f->cols);
        SetAnchor(f, &a[1], f->cols * f->rows - 1);
        k = 2;
    case 1:
        SetAnchor(f, &a[k], 0);
        k++;
        SetAnchor(f, &a[k], f->cols - 1);
        break;
    case 3:
        for (i = 0; i < f->cols; i++) {
            SetAnchor(f, &a[i], i);
        }
        break;
    case 4:
        SetAnchor(f, &a[0], 0);
        SetAnchor(f, &a[1], (f->rows - 1) * f->cols);
        break;
    case 5:
        for (i = 0; i < f->rows; i++) {
            SetAnchor(f, &a[i], f->cols * i);
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

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

static inline u8 LerpB(u8 a, u8 b, f32 t) { return (u8)(int)((f32)b + t * ((f32)a - (f32)b)); }

extern "C" void fn_801F5970(Fabric* f, Quad* q) {
    int n = f->rows * f->cols;
    int r;
    f->verts = new ("fabric verts") TVec[n * 2];
    f->cur = f->verts;
    f->prev = f->verts + n;
    f->draw = (DrawVert*)new ("fabric draw tab") u8[n * sizeof(DrawVert)];
    f->count = n;
    for (r = 0; r < f->rows; r++) {
        f32 t = (f32)r / (f32)(f->rows - 1);
        Corner l;
        Corner rr;
        int c;
        l.pos.x = Lerp(q->c[0].pos.x, q->c[2].pos.x, t);
        l.pos.y = Lerp(q->c[0].pos.y, q->c[2].pos.y, t);
        l.pos.z = Lerp(q->c[0].pos.z, q->c[2].pos.z, t);
        l.u = Lerp(q->c[0].u, q->c[2].u, t);
        l.v = Lerp(q->c[0].v, q->c[2].v, t);
        rr.pos.x = Lerp(q->c[1].pos.x, q->c[3].pos.x, t);
        rr.pos.y = Lerp(q->c[1].pos.y, q->c[3].pos.y, t);
        rr.pos.z = Lerp(q->c[1].pos.z, q->c[3].pos.z, t);
        rr.u = Lerp(q->c[1].u, q->c[3].u, t);
        rr.v = Lerp(q->c[1].v, q->c[3].v, t);
        {
            u8* a = (u8*)&q->c[0].color;
            u8* b = (u8*)&q->c[2].color;
            u8* lc = (u8*)&l.color;
            lc[0] = LerpB(b[0], a[0], t);
            lc[1] = LerpB(b[1], a[1], t);
            lc[2] = LerpB(b[2], a[2], t);
            lc[3] = LerpB(b[3], a[3], t);
            a = (u8*)&q->c[1].color;
            b = (u8*)&q->c[3].color;
            lc = (u8*)&rr.color;
            lc[0] = LerpB(b[0], a[0], t);
            lc[1] = LerpB(b[1], a[1], t);
            lc[2] = LerpB(b[2], a[2], t);
            lc[3] = LerpB(b[3], a[3], t);
        }
        for (c = 0; c < f->cols; c++) {
            f32 s = (f32)c / (f32)(f->cols - 1);
            int i = r * f->cols + c;
            TVec* p = &f->cur[i];
            DrawVert* d = &f->draw[i];
            u8* a = (u8*)&l.color;
            u8* b = (u8*)&rr.color;
            u8* dc = (u8*)&d->color;
            p->x = Lerp(l.pos.x, rr.pos.x, s);
            p->y = Lerp(l.pos.y, rr.pos.y, s);
            p->z = Lerp(l.pos.z, rr.pos.z, s);
            f->prev[i] = *p;
            d->u = Lerp(l.u, rr.u, s);
            d->v = Lerp(l.v, rr.v, s);
            dc[0] = LerpB(b[0], a[0], s);
            dc[1] = LerpB(b[1], a[1], s);
            dc[2] = LerpB(b[2], a[2], s);
            dc[3] = LerpB(b[3], a[3], s);
        }
    }
}

extern "C" {

void fn_801F602C(Fabric* f, Quad* q, const char* tex, int style, u32 flags) {
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
    umin = FMin(q->c[0].u, FMin(q->c[1].u, FMin(q->c[2].u, q->c[3].u)));
    umax = FMax(q->c[0].u, FMax(q->c[1].u, FMax(q->c[2].u, q->c[3].u)));
    vmin = FMin(q->c[0].v, FMin(q->c[1].v, FMin(q->c[2].v, q->c[3].v)));
    vmax = FMax(q->c[0].v, FMax(q->c[1].v, FMax(q->c[2].v, q->c[3].v)));
    f->tex = fn_802CAC28(tex, 0);
    f32 du = (f32)__fabs(umax - umin);
    f32 dv = (f32)__fabs(vmax - vmin);
    span = FMax(du, dv);
    f->lod = log2(span / FMax(w, h));
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
    fn_802CAC54(f->tex);
}

// Verlet step with one force for every particle.
void fn_801F63F0(Fabric* f, const PVec* force, f32 drag) {
    f32 keep = 1.0f - drag;
    PVec a;
    PVec t;
    f32 k1;
    f32 k2;
    int i;
    a.x = force->x * 0.0002777778f;
    a.y = force->y * 0.0002777778f;
    a.z = force->z * 0.0002777778f;
    k1 = 2.0f - drag;
    k2 = -keep;
    for (i = 0; i < f->count; i++) {
        TVec* p = &f->prev[i];
        TVec* c = &f->cur[i];
        t.x = a.x + c->x * k1;
        t.y = a.y + c->y * k1;
        t.z = a.z + c->z * k1;
        p->x = t.x + p->x * k2;
        p->y = t.y + p->y * k2;
        p->z = t.z + p->z * k2;
    }
    {
        TVec* c = f->cur;
        f->cur = f->prev;
        f->prev = c;
    }
}

// Verlet step with a force per particle.
void fn_801F64E4(Fabric* f, const PVec* forces, f32 drag) {
    f32 k1 = 2.0f - drag;
    f32 k2 = -(1.0f - drag);
    PVec t;
    int i;
    for (i = 0; i < f->count; i++, forces++) {
        TVec* c = &f->cur[i];
        TVec* p = &f->prev[i];
        t.x = c->x * k1;
        t.y = c->y * k1;
        t.z = c->z * k1;
        t.x = t.x + p->x * k2;
        t.y = t.y + p->y * k2;
        t.z = t.z + p->z * k2;
        p->x = t.x + forces->x * 0.0002777778f;
        p->y = t.y + forces->y * 0.0002777778f;
        p->z = t.z + forces->z * 0.0002777778f;
    }
    {
        TVec* c = f->cur;
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
        TVec* a = &f->cur[s->a];
        TVec* b = &f->cur[s->b];
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

void fn_801F6880(Fabric* f) {
    f32 old = fn_802C9A4C(f->lod);
    fn_802C8BD0(f->tex);
    if (fn_802C8DBC((f->rows - 1) * (f->cols * 2))) {
        int r;
        for (r = 1; r < f->rows; r++) {
            int c;
            for (c = 0; c < f->cols; c++) {
                int i = c + (r - 1) * f->cols;
                TVec* p = &f->cur[i];
                DrawVert* d = &f->draw[i];
                f32 du = d->u;
                fn_802C8E00(d->color, c > 0, p->x, p->y, p->z, du, d->v);
                i = c + r * f->cols;
                d = &f->draw[i];
                p = &f->cur[i];
                fn_802C8E00(d->color, c > 0, p->x, p->y, p->z, d->u, d->v);
            }
        }
        fn_802C8E48();
        fn_802C8D74();
    }
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

static inline BOOL Paused(void) { return (f32)__fabs(fn_801E7E24()) <= lbl_80558DC4[0]; }

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

void ManualFabricMan::Add(ValueNode* vn, TMtx* m) {
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

void UniformFabricInst::Draw(void* ctx) { lbl_8055EC10->Update(ctx); }

void UniformFabricInst::Draw2(void* ctx) { lbl_8055EC10->Draw(); }

void NonUniformFabricInst::Draw(void* ctx) { lbl_8055EC14->Update(ctx); }

void NonUniformFabricInst::Draw2(void* ctx) { lbl_8055EC14->Draw(); }

extern "C" {

void fn_801F7400(Quad* q, u8* data, ValueNode* vn) {
    s32* idx = (s32*)(data + 0x58);
    Corner* c0 = (Corner*)(data + 0x10) + idx[0];
    Corner* c1 = (Corner*)(data + 0x10) + idx[1];
    Corner* c2 = (Corner*)(data + 0x10) + idx[2];
    q->c[0] = *c0;
    q->c[1] = *c1;
    q->c[2] = *c2;
    q->c[3].pos.x = q->c[1].pos.x + q->c[2].pos.x - q->c[0].pos.x;
    q->c[3].pos.y = q->c[1].pos.y + q->c[2].pos.y - q->c[0].pos.y;
    q->c[3].pos.z = q->c[1].pos.z + q->c[2].pos.z - q->c[0].pos.z;
    q->c[3].color = c1->color;
    q->c[3].u = c1->u + c2->u - c0->u;
    q->c[3].v = c1->v + c2->v - c0->v;
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
    tex = fn_802C9C24(o->model[1]);
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

static inline void SwapN(u8* p, int n) {
    int i;
    for (i = 0; i < n / 2; i++) {
        u8 t = p[i];
        p[i] = p[n - 1 - i];
        p[n - 1 - i] = t;
    }
}

static inline void Swap32(u8* p) {
    u8 t = p[0];
    p[0] = p[3];
    p[3] = t;
    t = p[1];
    p[1] = p[2];
    p[2] = t;
}

void FabricTMH::Swap(TObj* o) {
    u8* d = (u8*)o;
    u8* p;
    int i;
    SwapN(d + 0x2C, 16);
    p = d + 0x3C;
    for (i = 0; i < 3; i++, p += 0x18) {
        Swap32(p);
        Swap32(p + 4);
        Swap32(p + 8);
        Swap32(p + 12);
        Swap32(p + 16);
        Swap32(p + 20);
    }
    SwapN(d + 0x84, 12);
}

void FabricTMH::Init(TObj* o) {}

void FabricTMH::Add(void* scene, TObj* o, s32 id) { fn_801F75A4(o, (u8*)o + 0x2C, scene, id); }

extern "C" TrackMod* fn_801F78F0(void) { return new ("FabricTMH") FabricTMH; }

FabricTMH::~FabricTMH(void) {}

NonUniformFabricInst::~NonUniformFabricInst(void) {}

UniformFabricInst::~UniformFabricInst(void) {}

int UniformFabricInst::Kind(void) { return 3; }

int NonUniformFabricInst::Kind(void) { return 3; }

void FabricTMH::Draw(TObj* list) {}
