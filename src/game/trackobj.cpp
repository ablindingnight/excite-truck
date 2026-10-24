// Track objects (0x801F7A0C-0x801FC8A8, "eventwob" / "trackwob"): object classes read from value
// tables (frames, shadows, composite/animation/sound/obstacle descriptions with their physical
// shapes), placing objects from the objects files with difficulty and token filtering, the static
// object lists with their collision volumes, animated and sound objects, and drawing. Names are
// guesses apart from the strings.
#include "game/common.h"
#include <stdarg.h>

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

extern "C" {
void* memset(void* p, int c, u32 n);
void* memcpy(void* d, const void* s, u32 n);
int strcasecmp(const char* a, const char* b);
int strcmp(const char* a, const char* b);
char* strrchr(const char* s, int c);
int sprintf(char* s, const char* fmt, ...);
int vsprintf(char* buf, const char* fmt, va_list args);
void qsort(void* base, u32 n, u32 size, int (*cmp)(const void*, const void*));
double sqrt(double x);
}

static inline void StrNCopy(char* dst, const char* src, int len) {
    char* d = dst;
    int n = len;
    const char* s = src;
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

static inline void StrNCat(char* d, const char* s, int n) {
    while (*d) {
        n--;
        d++;
    }
    if (n > 0) {
        StrNCopy(d, s, n);
    }
}

struct PVec {
    void Set(const ValVec3& v) {
        x = v.x;
        y = v.y;
        z = v.z;
    }
    f32 x, y, z;
};

// An object's placement: three basis vectors and the origin (0x30).
struct OFrame {
    PVec x;    // offset 0x0
    PVec y;    // offset 0xC
    PVec z;    // offset 0x18
    PVec pos;  // offset 0x24
};

// An object being placed (0x3C).
struct PlacedObj {
    PlacedObj(void) {
        node = 0;
        flags = 0;
    }
    OFrame frame;     // offset 0x0
    u32 flags;        // offset 0x30
    ValueNode* node;  // offset 0x34
    s32 level;        // offset 0x38
};

struct ShadowGroup {
    PVec normal;  // offset 0x0
    s32 id;       // offset 0xC
};

struct ShadowDesc {
    const char* model;      // offset 0x0
    f32 extend;             // offset 0x4
    f32 cutoff;             // offset 0x8
    ShadowGroup groups[8];  // offset 0xC
    s32 count;              // offset 0x8C
};

// An object class read from a value table (0x10C).
class ObjDesc {
public:
    char name[0x20];    // offset 0x0
    ObjDesc* next;      // offset 0x20: next class
    ObjDesc* sibling;   // offset 0x24: next part of a composite
    f32 drop;           // offset 0x28
    u8 forbidden;       // offset 0x2C
    OFrame frame;       // offset 0x30
    u8 hasFrame;        // offset 0x60
    char model[0x10];   // offset 0x61
    void* handle;       // offset 0x74
    ShadowDesc shadow;  // offset 0x78

    ObjDesc(void);
    ObjDesc(int) {
        shadow.model = 0;
        shadow.extend = 0.0f;
        shadow.cutoff = 0.0f;
        shadow.count = 0;
        sibling = 0;
        next = 0;
        drop = -10.0f;
        hasFrame = 1;
        memset(name, 'a', sizeof(name));
        memset(model, 'a', sizeof(model));
        handle = (void*)-1;
    }
    virtual ~ObjDesc(void) {}
    virtual void Read(PlacedObj* o, ValueNode* vn);
    virtual void Create(const PlacedObj* o) = 0;
    virtual void Draw(const OFrame* f);
    virtual void Props(void* p, PlacedObj* o);
    virtual f32 vf1C(void);
    virtual void vf20(void);
    virtual void vf24(void);
};

// The physical description of an obstacle (0x84).
struct ObstacleDesc {
    ObstacleDesc(void);
    void Place(ValueNode* n, OFrame f, u32 fl) {
        frame = f;
        node = n;
        if (fl & 1) {
            flags = fl;
        }
    }
    u32 tag;          // offset 0x0
    u32 size;         // offset 0x4
    f32 mass;         // offset 0x8
    f32 inertia[3];   // offset 0xC
    OFrame frame;     // offset 0x18
    s32 surface;      // offset 0x48
    f32 friction;     // offset 0x4C
    s32 kind;         // offset 0x50
    u32 flags;        // offset 0x54
    union {
        f32 f[9];
        char name[0x24];
    } shape;          // offset 0x58
    s32 solId;        // offset 0x7C
    ValueNode* node;  // offset 0x80
};

class AnimDesc : public ObjDesc {
public:
    AnimDesc(void) : ObjDesc(0) {}
    virtual ~AnimDesc(void);
    virtual void Create(const PlacedObj* o);
    OFrame place;            // offset 0x10C
    const char* animModel;   // offset 0x13C
    const char* streams[8];  // offset 0x140
    s32 numStreams;          // offset 0x160
    u8 looped;               // offset 0x164
    u8 streaming;            // offset 0x165
};

class SoundDesc : public ObjDesc {
public:
    SoundDesc(void) : ObjDesc(0) {}
    virtual ~SoundDesc(void);
    virtual void Create(const PlacedObj* o);
    virtual void Props(void* p, PlacedObj* o);
};

class ObstacleObjDesc : public ObjDesc {
public:
    virtual ~ObstacleObjDesc(void);
    virtual void Read(PlacedObj* o, ValueNode* vn);
    virtual void Create(const PlacedObj* o);
    virtual void Draw(const OFrame* f);
    virtual void Props(void* p, PlacedObj* o);
    ObstacleDesc phys;   // offset 0x10C
    u32 flags;           // offset 0x190
    const char* effect;  // offset 0x194
    s32 event;           // offset 0x198
    ValueNode* props;    // offset 0x19C
};

struct ClassReader {
    int (*match)(const char* type);
    ObjDesc* (*read)(ValueNode* vn);
};

struct SkipList {
    const char* names[3];
};

struct TypeList {
    const char* names[24];
};

struct ReaderList {
    ClassReader r[4];
};

static const SkipList sSkip = {{"floater", "gfx", 0}};
static const TypeList sTypes = {{
    "ball",  "box1",    "box2",    "wedge",         "cone",      "barrel",      "wall",    "post",
    "horse", "ring",    "polyhedra", "barrier",     "wobble",    "tube",        "so_ball", "so_wall",
    "torch", "do_ball", "do_tube", "static_sphere", "phys_mesh", "corner_ball", "so_tube", "void",
}};

extern "C" {
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* t);
void fn_802C6CF8(void* model);
void* fn_802C6FC0(const char* model, int a, int b);
void fn_801E6CD0(void* anim, void* stream, f32 t);
void fn_801E6CC4(void* stream);
void fn_801E6B98(void* anim);
}

// A loaded class file (8).
struct FileEntry {
    FileEntry(const char* path) { table = fn_80131810(path); }
    ~FileEntry(void) {
        if (table) {
            fn_80131818(table);
        }
        table = 0;
    }
    ValueTable* table;  // offset 0x0
    FileEntry* next;    // offset 0x4
};

typedef int (*FacingFn)(const OFrame* f);

// A static object drawn from the lists (0x48).
struct StaticOb {
    StaticOb(ObstacleObjDesc* d, const OFrame* f, OFrame* v, s32 i);
    ~StaticOb(void) { fn_802C6CF8(model); }
    void* model;       // offset 0x0
    OFrame frame;      // offset 0x4
    OFrame* vol;       // offset 0x34
    FacingFn facing;   // offset 0x38
    s32 id;            // offset 0x3C
    u8 visible;        // offset 0x40
    u8 hidden;         // offset 0x41
    StaticOb* next;    // offset 0x44
};

// An animated object (0x6C).
struct AnimInst {
    ~AnimInst(void) {
        fn_801E6CD0(anim, 0, 0.0f);
        for (int i = 0; i < count; i++) {
            fn_801E6CC4(streams[i]);
        }
        fn_801E6B98(anim);
    }
    OFrame frame;       // offset 0x0
    void* anim;         // offset 0x30
    void* streams[8];   // offset 0x34
    s32 count;          // offset 0x54
    u8 tag;             // offset 0x58
    u8 live;            // offset 0x59
    u8 f5A;             // offset 0x5A
    f32 liveRad2;       // offset 0x5C
    s32 state;          // offset 0x60
    const char* name;   // offset 0x64
    AnimInst* next;     // offset 0x68
};

class Sound3D {
public:
    virtual ~Sound3D(void);
    u8 pad4[4];
    f32 volume;      // offset 0x8
    u8 padC[0x49 - 0xC];
    u8 f49;          // offset 0x49
    u8 f4A;          // offset 0x4A
    u8 f4B;          // offset 0x4B
    u8 f4C;          // offset 0x4C
    u8 dirty;        // offset 0x4D
    u8 pad4E[0x6C - 0x4E];
    f32 nearDist;    // offset 0x6C
};

// A placed sound (0x18).
struct EWSound {
    ~EWSound(void) {
        if (snd) {
            delete snd;
        }
        snd = 0;
    }
    PVec pos;        // offset 0x0
    u8 tag;          // offset 0xC
    Sound3D* snd;    // offset 0x10
    EWSound* next;   // offset 0x14
};

class CollShape {
public:
    CollShape(void* o, const void* m, u32 t, f32 k, f32 s) {
        mtx = m;
        owner = o;
        a8 = 0;
        type = t;
        stiff = k;
        skin = s;
        a20 = 100;
        a1C = 0;
    }
    virtual ~CollShape(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void vf1C(void);
    virtual void vf20(void);
    virtual void vf24(void);
    virtual void vf28(void);
    virtual void vf2C(void);
    virtual void vf30(void);
    virtual void vf34(void);
    virtual void vf38(void) = 0;
    virtual f32 Radius(void);    // 0x3C
    virtual void SetId(int id);  // 0x40
    void* owner;      // offset 0x4
    s32 a8;           // offset 0x8
    const void* mtx;  // offset 0xC
    f32 stiff;        // offset 0x10
    f32 skin;         // offset 0x14
    u32 type;         // offset 0x18
    s32 a1C;          // offset 0x1C
    s32 a20;          // offset 0x20
};

struct RVec3 {
    RVec3(void) {}
    RVec3(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
    f32 x, y, z;
};

extern "C" void fn_8019EECC(void* v, RVec3* c);

class TubeVol : public CollShape {
public:
    TubeVol(f32 k, f32 s, f32 r, f32 h) : CollShape(0, 0, 'TUBE', k, s) {
        rad = r;
        half = h;
    }
    virtual ~TubeVol(void);
    virtual void vf38(void);
    f32 rad;   // offset 0x24
    f32 half;  // offset 0x28
};

class RingVol : public CollShape {
public:
    RingVol(f32 k, f32 s, f32 r, f32 w) : CollShape(0, 0, 'RING', k, s) {
        inner = r;
        ring = w;
    }
    virtual ~RingVol(void);
    virtual void vf38(void);
    f32 inner;  // offset 0x24
    f32 ring;   // offset 0x28
};

class SphereVol : public CollShape {
public:
    SphereVol(void* o, const void* m, f32 k, f32 s, f32 r, const RVec3& c)
        : CollShape(o, m, 0x53504852, k, s), center(c) {
        radius = r;
        f34 = 0.0f;
        f38 = 0.0f;
        f3C = 0.0f;
        f40 = 0.0f;
        f44 = 0.0f;
        f48 = 0.0f;
        b4C = 0;
        f50 = 0.0f;
        center.x = c.x;
        center.y = c.y;
        center.z = c.z;
        if (o) {
            fn_8019EECC(o, &center);
        }
    }
    virtual ~SphereVol(void);
    virtual void vf38(void);
    f32 radius;    // offset 0x24
    RVec3 center;  // offset 0x28
    f32 f34;       // offset 0x34
    f32 f38;       // offset 0x38
    f32 f3C;       // offset 0x3C
    f32 f40;       // offset 0x40
    f32 f44;       // offset 0x44
    f32 f48;       // offset 0x48
    u8 b4C;        // offset 0x4C
    f32 f50;       // offset 0x50
};

class BoxVol : public CollShape {
public:
    BoxVol(f32 k, f32 s, f32 x, f32 y, f32 z) : CollShape(0, 0, 'BOX ', k, s) {
        hx = x / 2;
        hy = y / 2;
        hz = z / 2;
        f34 = 0;
        radius = 0.2f + (f32)sqrt(hx * hx + hy * hy + hz * hz);
    }
    virtual ~BoxVol(void);
    virtual void vf38(void);
    f32 hx;      // offset 0x24
    f32 hy;      // offset 0x28
    f32 hz;      // offset 0x2C
    f32 radius;  // offset 0x30
    s32 f34;     // offset 0x34
};

// A solid object of the track solution (0x4C).
struct SolObj {
    u8 pad0[4];
    OFrame frame;    // offset 0x4
    u8 pad34[0x44 - 0x34];
    SolObj* next;    // offset 0x44
    u8 pad48[4];
};

struct SolBuilder {
    SolBuilder(void) { head = 0; }
    ~SolBuilder(void) { head = 0; }
    SolObj* head;  // offset 0x0
};

struct SolEntry {
    u8 pad0[4];
    OFrame frame;  // offset 0x4
    u8 pad34[4];
    s32 id;        // offset 0x38
};

struct SolFile {
    s32 count;          // offset 0x0
    SolEntry** ents;    // offset 0x4
    u8 pad8[0x14 - 0x8];
    void (*done)(void);  // offset 0x14
};

struct AnimSlot;

class AnimObj {
public:
    virtual ~AnimObj(void);
    virtual void vf0C(void);
    virtual void Draw(void) = 0;
    virtual void vf14(void);
    virtual f32 Depth(void);
    virtual void vf1C(void);
    virtual void Set(const AnimSlot* s);
    OFrame frame;  // offset 0x4
    s32 offset;    // offset 0x34
    s16 flags;     // offset 0x38
};

class AnimModel : public AnimObj {
public:
    AnimModel(const char* name, const char* model);
    virtual ~AnimModel(void);
    virtual void Draw(void);
    void* model;  // offset 0x3C
};

struct ShadowRef {
    ShadowRef(void) {
        h = -1;
        extend = 0.0f;
        cutoff = 0.0f;
    }
    ~ShadowRef(void) {
        if (h != -1) {
            fn_802C6CF8((void*)h);
        }
    }
    s32 h;       // offset 0x0
    f32 extend;  // offset 0x4
    f32 cutoff;  // offset 0x8
};

// A static object with a shadow, drawn as an animated model (0x58).
class StaticObject : public AnimModel {
public:
    StaticObject(const char* name, const char* model, FacingFn fn, const ShadowDesc* s, int vis);
    virtual ~StaticObject(void);
    virtual void Draw(void);
    virtual void vf1C(void);
    int InitShadow(const ShadowDesc* s) {
        if (s->model) {
            shadow.h = (s32)fn_802C6FC0(s->model, 0, 0);
            shadow.extend = s->extend;
            shadow.cutoff = s->cutoff;
            return 1;
        }
        return 0;
    }
    FacingFn facing;      // offset 0x40
    ShadowRef shadow;     // offset 0x44
    u8 noDraw;            // offset 0x50
    StaticObject* next;   // offset 0x54
};

struct RaceSlot {
    s32 type;       // offset 0x0
    u8 pad4[0x6C - 0x4];
    s32 vehicle;    // offset 0x6C
    u8 pad70[0x78 - 0x70];
};

struct RaceDesc {
    char track[0x20];     // offset 0x0
    RaceSlot slots[8];    // offset 0x20
    s32 count;            // offset 0x3E0
};

struct VehicleInfo {
    u8 pad0[0xA0];
    u32 flags;  // offset 0xA0
};

struct ViewState {
    u8 pad0[0x24];
    PVec pos;  // offset 0x24
};

struct EventCfg {
    u8 pad0[0x10];
    u32 flags;  // offset 0x10
};

struct Settings {
    u8 pad0[0x20];
    ValueNode* vals;  // offset 0x20
};

struct TableRef {
    TableRef(void) { node = 0; }
    ~TableRef(void);
    ValueNode* node;
};

struct ObjMsg {
    s32 id;      // offset 0x0
    u8 visible;  // offset 0x4
};

struct EWState {
    u8 cur;   // tag given to new objects
    u8 view;  // tag being shown
};

VehicleInfo* FindVehicle(s32 id);
TableRef fn_8013A0B0(const char* what, const char* name);

struct Mat3;
void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);

extern "C" {
void PSVECCrossProduct(const void* a, const void* b, void* out);
f32 PSVECDotProduct(const void* a, const void* b);
f32 PSVECSquareDistance(const void* a, const void* b);
int fn_8012F9E8(void* out, int size, const char* text);
s32 fn_80204BB4(const char* level);
s32 fn_80204CF0(void);
void* fn_802C6B44(const char* model);
void fn_802C6C20(void* model);
void fn_802C70AC(void* model, const OFrame* f, int a, int mode);
void fn_802C7AE0(void* model, const OFrame* f, int a, f32 alpha);
void fn_801BFD34(void* out, const void* p, const PlacedObj* m);
s32 fn_801D04D0(const char* surface);
void fn_802E317C(const char* fmt, ...);
void fn_802E30C8(const char* fmt, ...);
ValueNode* fn_8013184C(void);
void fn_80204018(ObjDesc* d, PlacedObj* o);
void fn_80204014(void* p, ValueNode* node, ValueNode* props);
Settings* fn_8000AC38(void);
void fn_8020401C(RaceDesc* r);
void fn_80204020(void);
int fn_8020402C(void);
void fn_80204050(void);
void fn_80203FF8(void);
void fn_801EEFDC(int (*cb)(ObjMsg*), int type);
void fn_801EF024(int (*cb)(ObjMsg*));
void* fn_801E6A3C(const char* model, int streaming, int c);
void* fn_801E6BEC(const char* stream);
void fn_801E6CC8(void* anim, int once);
void fn_801E6E9C(void* anim, f32 dt);
int fn_801E7650(void* anim);
void fn_801E75C8(void* anim, AnimInst* a);
int fn_80129D90(int a, int b, int n);
f32 fn_801E7E24(void);
ViewState* fn_801E8718(int view);
int fn_801E7698(void);
EventCfg* fn_801E87A8(void);
void fn_801E87D4(AnimObj* o);
f32 fn_801E88F0(int view);
int fn_802C9C58(int mode);
void fn_802C7A44(void);
void fn_802C7A50(void);
void fn_802C7AD4(void);
PVec* fn_802C9A40(void);
OFrame* fn_802C86DC(void);
f32 fn_802C88E8(const PVec* p);
SolObj* fn_801A4A8C(SolObj* o, s32 id, s32 event, CollShape* s, const OFrame* f, ValueNode* node);
void fn_801B5D30(SolBuilder* b);
void fn_801792F4(void);
SolFile* fn_801A4548(const char* name);
void fn_801EA1E0(f32 x, f32 z, f32* y, int a);
f32 fn_801EA0E8(f32 x, f32 z);
void fn_8029798C(const char* model, const OFrame* f, u32 mask, f32 extend, f32 cutoff);
void fn_80297988(StaticObject* o);
void fn_802981EC(const PlacedObj* o, f32 a);
void fn_801F4DD4(const char* table, const PlacedObj* o);
CollShape* fn_801A324C(void* p, void* o, f32 k, f32 s, const char* file);
Sound3D* fn_802077A4(const char* name, int cls, const PVec* vel, const PVec* pos);
}

extern f32 lbl_80558DC4[];

ModuleTag lbl_8055EC28;
PVec lbl_80488898 = {0.0f, 0.0f, 0.0f};

struct EWLight {
    EWLight(void) {
        color = 0x7F000000;
        angle = 0.7853982f;
    }
    u32 color;
    f32 angle;
};

static EWLight lbl_8055EC30;
ObjDesc* lbl_8055EC38;
u8 lbl_8055EC3C;
EWState lbl_8055EC40;
u8 lbl_8055EC42;
FileEntry* lbl_8055EC44;
StaticObject* lbl_8055EC48;
StaticOb* lbl_8055EC4C;
StaticOb* lbl_8055EC50;
StaticOb** lbl_8055EC54;
s32 lbl_8055EC58;
AnimInst* lbl_8055EC5C;
AnimInst* lbl_8055EC60;
s32 lbl_8055EC64;
ValueNode* lbl_8055EC68;
EWSound* lbl_8055EC6C;
ValueNode* lbl_8055EC70;
SolBuilder* lbl_8055EC74;
s32 lbl_8055EC78;
u8 lbl_8055EC7C;
s32 lbl_8055B1C8 = -20000;

static inline void Lift(OFrame* f, f32 h) {
    f->pos.x += f->y.x * h;
    f->pos.y += f->y.y * h;
    f->pos.z += f->y.z * h;
}

static inline int IsSkipType(ValueNode* vn) {
    SkipList skip = sSkip;
    const char* type = vn->GetValue("type");
    const char** p;
    for (p = skip.names; *p; p++) {
        if (strcasecmp(*p, type) == 0) {
            return 1;
        }
    }
    return 0;
}

static inline void StartAnim(AnimInst* a) {
    if (a->state != 1) {
        int i = fn_80129D90(0, 0, a->count);
        fn_801E6CD0(a->anim, a->streams[i], 0.0f);
    } else {
        fn_801E6CD0(a->anim, 0, 0.0f);
    }
}

static inline int FacingMode(const OFrame* f, const PVec* axis) {
    OFrame* cam = fn_802C86DC();
    PVec d;
    d.x = f->pos.x - cam->pos.x;
    d.y = f->pos.y - cam->pos.y;
    d.z = f->pos.z - cam->pos.z;
    int behind = PSVECDotProduct(&d, axis) < 0.0f;
    return (behind ? 0x10 : 0x20) | 1;
}

extern "C" {

int fn_801F7A0C(void) { return 1; }

// Make the basis right-handed.
void fn_801F7A14(OFrame* f) {
    PVec c;
    PSVECCrossProduct(&f->x, &f->y, &c);
    if (PSVECDotProduct(&c, &f->z) < 0.0f) {
        f->x.x *= -1.0f;
        f->x.y *= -1.0f;
        f->x.z *= -1.0f;
    }
}

int fn_801F7A88(ValueNode* vn, const char* key, void* out, int size) {
    int r = 0;
    if (vn->Has(key)) {
        r = fn_8012F9E8(out, size, vn->GetValue(key));
    }
    return r;
}

static inline void Swap32(u8* p) {
    u8 t = p[0];
    p[0] = p[3];
    p[3] = t;
    t = p[1];
    p[1] = p[2];
    p[2] = t;
}

void fn_801F7B10(OFrame* f, ValueNode* vn, const char* key) {
    if (fn_801F7A88(vn, key, f, sizeof(OFrame))) {
        u8* p = (u8*)f;
        for (int i = 0; i < 2; i++) {
            Swap32(p);
            Swap32(p + 4);
            Swap32(p + 8);
            Swap32(p + 12);
            Swap32(p + 16);
            Swap32(p + 20);
            p += 0x18;
        }
    } else {
        f->x.Set(vn->GetVec3("basis_x"));
        f->y.Set(vn->GetVec3("basis_y"));
        f->z.Set(vn->GetVec3("basis_z"));
        f->pos.Set(vn->GetVec3("origin"));
    }
    fn_801F7A14(f);
}
}

void ObjDesc::Read(PlacedObj* o, ValueNode* vn) {
    if (vn->Has("unfloat_frame64")) {
        fn_801F7B10(&o->frame, vn, "unfloat_frame64");
        Lift(&o->frame, drop);
    }
    if (vn->Has("difficulty_level")) {
        o->level = fn_80204BB4(vn->GetValue("difficulty_level"));
    } else {
        o->level = fn_80204CF0();
    }
    o->node = vn;
}

extern "C" {

void fn_801F7E20(ValueNode* vn, ObjDesc* d) {
    if (d->model[0]) {
        u8* m = (u8*)fn_802C6B44(d->model);
        d->drop = 0.0254f + -*(f32*)(m + 0x34);
        fn_802C6C20(m);
    } else {
        d->drop = 0.0f;
    }
    d->drop = vn->GetFloat("drop_height", d->drop);
}

int fn_801F7EB0(OFrame* f, ValueNode* vn, const PlacedObj* parent) {
    ValueNode* lf = vn->FindChild("local_frame");
    if (lf) {
        fn_801F7B10(f, lf, "frame64");
    } else {
        f->x.x = 1.0f;
        f->x.y = 0.0f;
        f->x.z = 0.0f;
        f->y.x = 0.0f;
        f->y.y = 1.0f;
        f->y.z = 0.0f;
        f->z.x = 0.0f;
        f->z.y = 0.0f;
        f->z.z = 1.0f;
        f->pos.x = 0.0f;
        f->pos.y = 0.0f;
        f->pos.z = 0.0f;
    }
    if (parent) {
        fn_801BFD34(&f->pos, &f->pos, parent);
        Mat3Mul((Mat3*)f, (const Mat3*)f, (const Mat3*)parent);
    }
    return lf || parent;
}

void fn_801F7FA0(ShadowDesc* s, ValueNode* vn) {
    s->model = vn->GetValue("model");
    s->extend = vn->GetFloat("extend_dist");
    s->cutoff = vn->GetFloat("cutoff_dist");
    int n = 0;
    for (int i = 0; vn->GetChildAt(i); i++) {
        ValueNode* c = vn->GetChildAt(i);
        if (strcasecmp(c->GetName(), "group") == 0) {
            s->groups[n].normal.Set(c->GetVec3("normal"));
            s->groups[n].id = c->GetIntValue("id");
            n++;
        }
    }
    s->count = n;
}

int fn_801F80FC(const char* type) { return strcasecmp(type, "composite") == 0; }

int fn_801F812C(const char* type) { return strcasecmp(type, "animation") == 0; }

int fn_801F815C(const char* type) { return strcasecmp(type, "sound") == 0; }

ObjDesc* fn_801F9338(ValueNode* vn);
ObjDesc* fn_801F83E4(ValueNode* vn);
ObjDesc* fn_801F861C(ValueNode* vn);
ObjDesc* fn_801F86C8(ValueNode* vn);

static const ReaderList sReaders = {{
    {fn_801F80FC, fn_801F9338},
    {fn_801F812C, fn_801F83E4},
    {fn_801F815C, fn_801F861C},
    {0, fn_801F86C8},
}};

ObjDesc* fn_801F8188(ValueNode* vn, const PlacedObj* parent, int named) {
    const char* type = vn->GetValue("type");
    ReaderList readers = sReaders;
    ObjDesc* d = 0;
    u32 i;
    for (i = 0; i < 4; i++) {
        if (!readers.r[i].match || readers.r[i].match(type)) {
            d = readers.r[i].read(vn);
            break;
        }
    }
    if (!fn_801F80FC(type)) {
        StrNCopy(d->model, vn->GetValue("model"), sizeof(d->model));
        d->hasFrame = fn_801F7EB0(&d->frame, vn, parent);
        fn_801F7E20(vn, d);
        d->forbidden = vn->GetBool("forbidden_in_objects_file", 0);
        if (vn->FindChild("shadow")) {
            fn_801F7FA0(&d->shadow, vn->FindChild("shadow"));
        }
    } else {
        d->drop = vn->GetFloat("drop_height", d->drop);
    }
    if (named) {
        StrNCopy(d->name, vn->GetValue("name"), sizeof(d->name));
    }
    return d;
}

ObjDesc* fn_801F83E4(ValueNode* vn) {
    AnimDesc* d;
    if ((d = new ("AnimDI") AnimDesc) != 0) {
        char key[32];
        d->animModel = vn->GetString("anim_model", vn->GetValue("model"));
        d->streams[0] = vn->GetValue("anim_stream");
        for (d->numStreams = 1; d->numStreams < 8; d->numStreams++) {
            sprintf(key, "anim_stream%d", d->numStreams + 1);
            if (!vn->Has(key)) {
                break;
            }
            d->streams[d->numStreams] = vn->GetValue(key);
        }
        d->looped = vn->GetBool("looped", 1);
        d->streaming = vn->GetBool("streaming", 1);
        vn->GetValue("name");
    }
    return d;
}

ObjDesc* fn_801F861C(ValueNode* vn) { return new ("SoundDI") SoundDesc; }

int fn_801F88F4(ValueNode* vn);
int fn_801F8988(ObstacleDesc* d, ValueNode* vn);

ObjDesc* fn_801F86C8(ValueNode* vn) {
    ObstacleObjDesc* d = new ("ObstacleDI") ObstacleObjDesc;
    d->flags = 0;
    d->phys.tag = 0;
    fn_801F8988(&d->phys, vn);
    if (!fn_801F88F4(vn)) {
        d->flags |= 4;
    }
    if (vn->GetBool("facing_test", 0)) {
        d->flags |= 1;
    }
    if (vn->GetBool("refloat_on_load", 0)) {
        d->flags |= 0x100;
    }
    if (vn->GetBool("facing_x_test", 0)) {
        d->flags |= 0x20;
    }
    if (vn->GetBool("fabric", 0)) {
        d->flags |= 0x40;
        d->effect = vn->GetValue("effect");
    } else {
        d->effect = "";
    }
    if (vn->GetBool("draw_last", 0)) {
        d->flags |= 0x80;
    }
    if (vn->GetBool("onesided", 0)) {
        d->flags |= 0x200;
    }
    d->event = vn->GetInt("sobj_event", -1);
    d->props = vn->FindChild("default_properties");
    return d;
}

int fn_801F88F4(ValueNode* vn) {
    SkipList skip = sSkip;
    const char* type = vn->GetValue("type");
    const char** p;
    for (p = skip.names; *p; p++) {
        if (strcasecmp(*p, type) == 0) {
            return 1;
        }
    }
    return 0;
}
}

static inline void ReadShape(ObstacleDesc* d, ValueNode* vn) {
    TypeList names = sTypes;
    const char* type = vn->GetValue("type");
    u32 i = 0;
    while (strcasecmp(names.names[i], type) != 0) {
        if (++i == 24) {
            fn_802E317C("Unknown obj type: \"%s\"", type);
        }
    }
    d->kind = i;
    switch (i) {
    case 0:
    case 14:
    case 17:
    case 19:
        d->shape.f[0] = vn->GetFloat("rad");
        break;
    case 1:
    case 2:
    case 3:
    case 6:
    case 15:
        d->shape.f[0] = vn->GetFloat("dx");
        d->shape.f[1] = vn->GetFloat("dy");
        d->shape.f[2] = vn->GetFloat("dz");
        break;
    case 4:
        d->shape.f[0] = vn->GetFloat("bottom_rad");
        d->shape.f[1] = vn->GetFloat("top_rad");
        d->shape.f[2] = vn->GetFloat("hit");
        break;
    case 8:
        d->shape.f[0] = vn->GetFloat("hit");
        d->shape.f[1] = vn->GetFloat("wid");
        d->shape.f[2] = vn->GetFloat("len");
        break;
    case 5:
    case 7:
        d->shape.f[0] = vn->GetFloat("rad");
        d->shape.f[1] = vn->GetFloat("hit");
        break;
    case 9:
        d->shape.f[0] = vn->GetFloat("inner_rad");
        d->shape.f[1] = vn->GetFloat("ring_rad");
        break;
    case 10:
        StrNCopy(d->shape.name, vn->GetString("phys_model", ""), 16);
        if (!d->shape.name[0]) {
            char* dot;
            StrNCopy(d->shape.name, vn->GetString("model", ""), 16);
            dot = strrchr(d->shape.name, '.');
            if (dot) {
                *dot = 0;
                StrNCat(d->shape.name, ".pln", 16);
            }
        }
        break;
    case 20:
        StrNCopy(d->shape.name, vn->GetString("phys_model", ""), 16);
        if (!d->shape.name[0]) {
            char* dot;
            StrNCopy(d->shape.name, vn->GetString("model", ""), 16);
            dot = strrchr(d->shape.name, '.');
            if (dot) {
                *dot = 0;
                StrNCat(d->shape.name, ".msh", 16);
            }
        }
        break;
    case 11: {
        u8* m = (u8*)fn_802C6B44(vn->GetValue("model"));
        memcpy(&d->shape, m + 0x2C, 0x1C);
        d->shape.f[6] = vn->GetFloat("front_top_z_adjust", 0.0f);
        fn_802C6C20(m);
        break;
    }
    case 12:
        d->shape.f[0] = vn->GetFloat("rad");
        d->shape.f[1] = vn->GetFloat("hit");
        break;
    case 13:
    case 18:
    case 22:
        d->shape.f[0] = vn->GetFloat("rad");
        d->shape.f[1] = vn->GetFloat("len");
        d->shape.f[2] = 9.81f * vn->GetFloat("rolling_linear_decel_g", 0.0f);
        break;
    case 16: {
        d->shape.f[0] = vn->GetFloat("dx");
        d->shape.f[1] = vn->GetFloat("dy");
        d->shape.f[2] = vn->GetFloat("dz");
        ((PVec*)&d->shape.f[3])->Set(vn->GetVec3("fire_offset"));
        break;
    }
    case 21: {
        StrNCopy(&d->shape.name[0x14], vn->GetValue("model"), 16);
        d->shape.f[0] = vn->GetFloat("rad");
        d->shape.f[1] = vn->GetFloat("y_rad", d->shape.f[0]);
        ((PVec*)&d->shape.f[2])->Set(vn->GetVec3("offset"));
        break;
    }
    }
}

extern "C" {

int fn_801F8988(ObstacleDesc* d, ValueNode* vn) {
    if (!IsSkipType(vn)) {
        d->tag = 'OBST';
        d->size = sizeof(ObstacleDesc);
        d->flags = 0;
        if (vn->Has("collide_ground")) {
            if (vn->GetIntValue2("collide_ground")) {
                d->flags |= 4;
            } else {
                d->flags |= 8;
            }
        }
        d->node = 0;
        d->mass = vn->GetFloat("mass", 1000.0f);
        d->inertia[0] = vn->GetFloat("mx", 10000.0f);
        d->inertia[1] = vn->GetFloat("my", 10000.0f);
        d->inertia[2] = vn->GetFloat("mz", 10000.0f);
        d->surface = vn->Has("surf_type") ? fn_801D04D0(vn->GetValue("surf_type")) : 100;
        d->friction = vn->GetFloat("friction", 1.0f);
        ReadShape(d, vn);
        return 1;
    }
    d->tag = 0;
    return 0;
}
}

ObstacleDesc::ObstacleDesc(void) {}

ObjDesc::ObjDesc(void) {
    shadow.model = 0;
    shadow.extend = 0.0f;
    shadow.cutoff = 0.0f;
    shadow.count = 0;
    sibling = 0;
    next = 0;
    drop = -10.0f;
    hasFrame = 1;
    memset(name, 'a', sizeof(name));
    memset(model, 'a', sizeof(model));
    handle = (void*)-1;
}

extern "C" {

ObjDesc* fn_801F9338(ValueNode* vn) {
    PlacedObj f;
    const PlacedObj* parent = 0;
    ObjDesc* list;
    ValueNode* c;
    int i;
    if (fn_801F7EB0(&f.frame, vn, 0)) {
        parent = &f;
    }
    list = 0;
    i = 0;
    while ((c = vn->GetChildAt(i)) != 0) {
        if (strcmp(c->GetName(), "object") == 0) {
            ObjDesc* d = fn_801F8188(c, parent, 0);
            d->sibling = list;
            list = d;
        }
        i++;
    }
    return list;
}

void fn_801F9440(ObjDesc** list, ObjDesc* d);

ObjDesc* fn_801F93FC(ValueNode* vn) {
    ObjDesc* d = fn_801F8188(vn, 0, 1);
    fn_801F9440(&lbl_8055EC38, d);
    return d;
}

void fn_801F9440(ObjDesc** list, ObjDesc* d) {
    d->next = *list;
    *list = d;
}

int fn_801F9450(ValueNode* vn) { return strcasecmp(vn->GetName(), "dyno") == 0; }

int fn_801F948C(ValueNode* vn) { return strcasecmp(vn->GetName(), "object_properties") == 0; }

void fn_801F94CC(const char* fmt, ...) {
    char buf[0x100];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    FileEntry* e = new ("class_file") FileEntry(buf);
    if (e->table) {
        e->next = lbl_8055EC44;
        lbl_8055EC44 = e;
    } else {
        delete e;
    }
}

ValueNode* fn_801F95C4(int (*pred)(ValueNode*), const char* name) {
    FileEntry* e = lbl_8055EC44;
    ValueNode* n = 0;
    while (!n && e) {
        int i = 0;
        while ((n = e->table->GetChild(i)) != 0) {
            if (pred(n) && strcasecmp(n->GetValue("name"), name) == 0) {
                break;
            }
            i++;
        }
        e = e->next;
    }
    if (!n) {
        fn_802E317C("Can't find \"%s\"", name);
    }
    return n;
}

ValueNode* fn_801F96A8(const char* name) { return fn_801F95C4(fn_801F9450, name); }

ValueNode* fn_801F96B8(const char* name) { return fn_801F95C4(fn_801F948C, name); }

ObjDesc* fn_801F96C8(const char* name) {
    ObjDesc* d;
    for (d = lbl_8055EC38; d; d = d->next) {
        if (strcasecmp(name, d->name) == 0) {
            break;
        }
    }
    if (!d) {
        d = fn_801F93FC(fn_801F96A8(name));
    }
    return d;
}

void fn_801F973C(ObjDesc* d, PlacedObj* parent, u32 flags) {
    if (flags & 2) {
        u32 f;
        ObjDesc* c;
        lbl_8055EC7C = !lbl_8055EC7C;
        f = d->sibling != 0;
        if (lbl_8055EC7C) {
            f |= 2;
        }
        for (c = d; c; c = c->sibling) {
            PlacedObj p;
            fn_801BFD34(&p.frame.pos, &c->frame.pos, parent);
            Mat3Mul((Mat3*)&p.frame, (const Mat3*)&c->frame, (const Mat3*)parent);
            p.flags = f;
            p.node = parent->node;
            c->Create(&p);
        }
    }
    if (flags & 1) {
        fn_80204018(d, parent);
    }
}

void fn_801F981C(ValueNode* vn, u32 flags) {
    const char* name = vn->GetValue("dyno");
    ObjDesc* d = fn_801F96C8(name);
    PlacedObj p;
    d->Read(&p, vn);
    if (d->forbidden && !(flags & 4)) {
        fn_802E317C("eventwob: Someone tried to create a \"%s\", which is not allowed.", d);
    }
    if (p.level != fn_80204CF0()) {
        fn_802E30C8("eventwob: rejecting a \"%s\" based on difficulty level", name);
    } else if (lbl_8055EC42 && strcmp(name, "Star") == 0) {
        fn_802E30C8("eventwob: rejecting \"%s\" because invincibility tokens are disabled", name);
    } else {
        fn_801F973C(d, &p, flags);
    }
}

void fn_801F9928(ValueNode* vn, u32 flags) {
    ValueNode* c;
    int i = 0;
    while ((c = vn->GetChildAt(i)) != 0) {
        if (strcasecmp(c->GetName(), "object") == 0) {
            fn_801F981C(c, flags);
        }
        i++;
    }
}

void fn_801F99CC(ObjDesc* d) {
    ObjDesc* c = d->sibling;
    while (c) {
        ObjDesc* n = c->sibling;
        delete c;
        c = n;
    }
}

void fn_801F9A24(ObjDesc** list) {
    ObjDesc* c = *list;
    while (c) {
        fn_801F99CC(c);
        ObjDesc* n = c->next;
        delete c;
        c = n;
    }
    *list = 0;
}

void fn_801F9AA4(const char* file, u32 flags) {
    ValueTable* t = fn_80131810(file);
    if (t) {
        if (t->HasChild("objects")) {
            fn_801F9928(t->FindChild("objects"), flags);
        }
        fn_80131818(t);
    }
}

void fn_801F9B28(void) {
    TableRef r = fn_8013A0B0("eventwob", "objects");
    if (r.node) {
        fn_801F9928(r.node, 3);
    }
    fn_801F9AA4("def_objects.val", 3);
    if (fn_8000AC38()->vals->GetBool("load_bonus_items", 0)) {
        fn_801F9AA4("bonusitems.val", 3);
    }
    fn_801F9AA4("trackwob.val", 2);
}

int fn_801F9BD0(const void* a, const void* b) { return (*(StaticOb**)a)->id - (*(StaticOb**)b)->id; }

void fn_801F9BE8(void) {
    StaticOb* o;
    lbl_8055EC58 = 0;
    for (o = lbl_8055EC4C; o; o = o->next) {
        if (o->id >= 0) {
            lbl_8055EC58++;
        }
    }
    for (o = lbl_8055EC50; o; o = o->next) {
        if (o->id >= 0) {
            lbl_8055EC58++;
        }
    }
    if (lbl_8055EC58 >= 1) {
        int n;
        lbl_8055EC54 = new ("StaticGOB tab") StaticOb*[lbl_8055EC58];
        n = 0;
        for (o = lbl_8055EC4C; o; o = o->next) {
            if (o->id >= 0) {
                lbl_8055EC54[n++] = o;
            }
        }
        for (o = lbl_8055EC50; o; o = o->next) {
            if (o->id >= 0) {
                lbl_8055EC54[n++] = o;
            }
        }
        qsort(lbl_8055EC54, lbl_8055EC58, 4, fn_801F9BD0);
    }
}

int fn_801F9CF8(ObjMsg* m) {
    if (lbl_8055EC54) {
        int lo = 0;
        int hi = lbl_8055EC58 - 1;
        while (lo <= hi) {
            int mid = (lo + hi) / 2;
            StaticOb* o = lbl_8055EC54[mid];
            if (m->id > o->id) {
                lo = mid + 1;
            } else if (m->id < o->id) {
                hi = mid - 1;
            } else {
                o->visible = m->visible;
                return 8;
            }
        }
    }
    return 8;
}

int fn_801F9D70(RaceDesc* r) {
    int found = 0;
    RaceSlot* s = r->slots;
    for (int i = 0; i < r->count; i++, s++) {
        if (s->type == 0x50) {
            found = 1;
            if (!(FindVehicle(s->vehicle)->flags & 4)) {
                return 0;
            }
        }
    }
    return found;
}

void fn_801F9E04(RaceDesc* r) {
    lbl_8055EC38 = 0;
    fn_801F94CC("race.res/obstacle.val");
    fn_801F94CC("%s.trk/obstacle.val", r);
    fn_801F94CC("trackwob.val");
    fn_801F94CC("treeclass.val");
    lbl_8055EC4C = 0;
    lbl_8055EC3C = 1;
    lbl_8055EC50 = 0;
    lbl_8055EC54 = 0;
    lbl_8055EC58 = 0;
    lbl_8055EC42 = fn_801F9D70(r);
    lbl_8055B1C8 = -20000;
    lbl_8055EC74 = new ("SolBuilder") SolBuilder;
    lbl_8055EC78 = 10000;
    lbl_8055EC64 = 0;
    lbl_8055EC68 = fn_801F96B8("anim");
    lbl_8055EC70 = fn_801F96B8("sound");
    fn_8020401C(r);
    fn_801EEFDC(fn_801F9CF8, 0xC);
}

void fn_801F9FA0(StaticOb** list);
void fn_801FA010(AnimInst** list);
void fn_801FA0B0(FileEntry** list);
void fn_801FA138(EWSound** list);

void fn_801F9F08(void) {
    fn_801EF024(fn_801F9CF8);
    fn_80204020();
    lbl_8055EC70 = 0;
    delete lbl_8055EC74;
    lbl_8055EC74 = 0;
    delete[] lbl_8055EC54;
    fn_801F9FA0(&lbl_8055EC4C);
    fn_801F9FA0(&lbl_8055EC50);
    fn_801FA010(&lbl_8055EC5C);
    fn_801FA010(&lbl_8055EC60);
    fn_801F9A24(&lbl_8055EC38);
    fn_801FA0B0(&lbl_8055EC44);
    fn_801FA138(&lbl_8055EC6C);
}

void fn_801F9FA0(StaticOb** list) {
    StaticOb* c = *list;
    while (c) {
        StaticOb* n = c->next;
        delete c;
        c = n;
    }
    *list = 0;
}

void fn_801FA010(AnimInst** list) {
    AnimInst* c = *list;
    while (c) {
        AnimInst* n = c->next;
        delete c;
        c = n;
    }
    *list = 0;
}

void fn_801FA0B0(FileEntry** list) {
    FileEntry* c = *list;
    while (c) {
        FileEntry* n = c->next;
        delete c;
        c = n;
    }
    *list = 0;
}

void fn_801FA138(EWSound** list) {
    EWSound* c = *list;
    while (c) {
        EWSound* n = c->next;
        delete c;
        c = n;
    }
    *list = 0;
}

void fn_801FA1D4(void) {
    StaticOb* o;
    AnimInst* a;
    for (o = lbl_8055EC4C; o; o = o->next) {
        o->visible = 1;
    }
    for (o = lbl_8055EC50; o; o = o->next) {
        o->visible = 1;
    }
    for (a = lbl_8055EC5C; a; a = a->next) {
        if (a->count > 0) {
            StartAnim(a);
        }
    }
}

void fn_801FA298(void) {
    if (!fn_8020402C()) {
        fn_801F9B28();
    }
}

static inline int IsLive(AnimInst* a, int near) { return a->live && near; }

void fn_801FA2C4(void) {
    f32 dt = fn_801E7E24();
    AnimInst* a;
    EWSound* s;
    for (a = lbl_8055EC5C; a; a = a->next) {
        int near = PSVECSquareDistance(&fn_801E8718(0)->pos, &a->frame.pos) < a->liveRad2;
        int state = a->state;
        if (state != 2) {
            int act = 0;
            if (IsLive(a, near) && a->tag == lbl_8055EC40.view) {
                act = 1;
            }
            switch (state) {
            case 0:
                if (!act) {
                    lbl_8055EC64--;
                    a->state = 1;
                    StartAnim(a);
                }
                break;
            case 1:
                if (act && lbl_8055EC64 < fn_801E7698()) {
                    lbl_8055EC64++;
                    a->state = 0;
                    StartAnim(a);
                }
                break;
            }
        }
        fn_801E6E9C(a->anim, dt);
        if (a->count > 1 && fn_801E7650(a->anim)) {
            StartAnim(a);
        }
    }
    for (s = lbl_8055EC6C; s; s = s->next) {
        Sound3D* snd = s->snd;
        if (snd) {
            if (s->tag == lbl_8055EC40.view) {
                snd->f4A = 0;
                snd->f4B = 1;
                snd->f49 = 1;
            } else {
                snd->f4A = 1;
                snd->f4C = 0;
            }
        }
    }
    fn_80203FF8();
}

void fn_801FA550(void) {
    StaticOb* o;
    AnimInst* a;
    for (o = lbl_8055EC50; o; o = o->next) {
        if (!o->hidden && o->visible && fn_801F7A0C()) {
            fn_802C70AC(o->model, o->vol ? o->vol : &o->frame, -1, -15);
        }
    }
    for (a = lbl_8055EC5C; a; a = a->next) {
        if (fn_801F7A0C() && a->tag == lbl_8055EC40.view) {
            fn_801E75C8(a->anim, a);
        }
    }
}

void fn_801FA618(void) {
    StaticObject* s;
    int old = fn_802C9C58((fn_801E87A8()->flags & 2) ? 1 : 2);
    fn_802C7A44();
    for (s = lbl_8055EC48; s; s = s->next) {
        if (s->noDraw && fn_801F7A0C()) {
            fn_802C70AC(s->model, &s->frame, -1, -15);
        }
    }
    fn_802C7A50();
    fn_802C7AD4();
    fn_802C9C58(old);
}

void fn_801FA6B8(void) {
    StaticOb* o;
    for (o = lbl_8055EC4C; o; o = o->next) {
        if (!o->hidden && o->visible && fn_801F7A0C()) {
            fn_802C70AC(o->model, o->vol ? o->vol : &o->frame, -1, -15);
        }
    }
}

void fn_801FA738(void) {
    StaticOb* o;
    int old = fn_802C9C58((fn_801E87A8()->flags & 2) ? 1 : 2);
    fn_802C7A44();
    for (o = lbl_8055EC4C; o; o = o->next) {
        if (o->hidden && o->visible && fn_801F7A0C()) {
            fn_802C70AC(o->model, o->vol ? o->vol : &o->frame, -1, -15);
        }
    }
    fn_802C7A50();
    fn_802C7AD4();
    fn_802C9C58(old);
}

void fn_801FA7F4(const char* name, const OFrame* f, ValueNode* node) {
    PlacedObj p;
    p.frame = *f;
    p.node = node;
    ObjDesc* d = fn_801F96C8(name);
    Lift(&p.frame, d->drop);
    fn_801F973C(d, &p, 2);
}

OFrame* fn_801FA8E0(CollShape* s, const OFrame* f, s32 event) {
    s32 id = lbl_8055EC78++;
    ValueNode* node = fn_8013184C();
    SolBuilder* b = lbl_8055EC74;
    SolObj* o = (SolObj*)operator new(sizeof(SolObj), "StaticObject");
    if (o) {
        o = fn_801A4A8C(o, id, event, s, f, node);
    }
    o->next = b->head;
    b->head = o;
    return &o->frame;
}

void fn_801FA978(void) {
    fn_801F9BE8();
    fn_801B5D30(lbl_8055EC74);
    fn_801792F4();
    lbl_8055EC3C = 0;
}

void fn_801FA9AC(void) {
    SolFile* f = fn_801A4548("track.sol");
    StaticOb* o;
    if (f) {
        for (int i = 0; i < lbl_8055EC58; i++) {
            StaticOb* s = lbl_8055EC54[i];
            for (int j = 0; j < f->count; j++) {
                SolEntry* e = f->ents[j];
                if (e->id == s->id) {
                    s->vol = &e->frame;
                    break;
                }
            }
        }
        f->done();
    }
    fn_80204050();
    for (o = lbl_8055EC4C; o; o = o->next) {
        if (!o->vol) {
            fn_801EA1E0(o->frame.pos.x, o->frame.pos.z, &o->frame.pos.y, 0);
        }
    }
    for (o = lbl_8055EC50; o; o = o->next) {
        if (!o->vol) {
            fn_801EA1E0(o->frame.pos.x, o->frame.pos.z, &o->frame.pos.y, 0);
        }
    }
}

void fn_801FAAC4(ObstacleDesc* d, ValueNode* vn) { ReadShape(d, vn); }

void fn_801FB1F4(s32 n) { lbl_8055B1C8 -= n; }
}

SoundDesc::~SoundDesc(void) {}

ObstacleObjDesc::~ObstacleObjDesc(void) {}

void ObstacleObjDesc::Read(PlacedObj* o, ValueNode* vn) {
    ObjDesc::Read(o, vn);
    if (flags & 0x100) {
        Lift(&o->frame, -drop);
        o->frame.pos.y = fn_801EA0E8(o->frame.pos.x, o->frame.pos.z);
        Lift(&o->frame, drop);
    }
}

void ObstacleObjDesc::Draw(const OFrame* f) {
    int mode;
    if (flags & 1) {
        mode = FacingMode(f, &f->z);
    } else {
        mode = -15;
    }
    fn_802C70AC(handle, f, -1, mode);
}

void ObstacleObjDesc::Props(void* p, PlacedObj* o) { fn_80204014(p, o->node, props); }

static int FacingNone(const OFrame* f);
static int FacingZ(const OFrame* f);
static int FacingX(const OFrame* f);
extern "C" {
void fn_801FBCF4(StaticOb** list, StaticOb* o);
OFrame* fn_801FBE1C(SolBuilder* b, s32 id, s32 event, CollShape* s, const OFrame* f, ValueNode* node);
CollShape* fn_801FBEA8(ObstacleDesc* d);
CollShape* fn_801FBF48(ObstacleDesc* d);
CollShape* fn_801FBFE8(ObstacleDesc* d);
CollShape* fn_801FC048(ObstacleDesc* d);
CollShape* fn_801FC12C(ObstacleDesc* d);
}

static inline FacingFn PickFacing(u32 fl) { return (fl & 0x20) ? FacingX : (fl & 1) ? FacingZ : FacingNone; }

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void RotVec(const OFrame* m, const PVec* v, PVec* out) {
    if (out == v) {
        f32 x = Dot3(v->x, m->x.x, v->y, m->y.x, v->z, m->z.x);
        f32 y = Dot3(v->x, m->x.y, v->y, m->y.y, v->z, m->z.y);
        f32 z = Dot3(v->x, m->x.z, v->y, m->y.z, v->z, m->z.z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(v->x, m->x.x, v->y, m->y.x, v->z, m->z.x);
        out->y = Dot3(v->x, m->x.y, v->y, m->y.y, v->z, m->z.y);
        out->z = Dot3(v->x, m->x.z, v->y, m->y.z, v->z, m->z.z);
    }
}

inline StaticOb::StaticOb(ObstacleObjDesc* d, const OFrame* f, OFrame* v, s32 i) {
    u32 fl = d->flags;
    FacingFn fn = PickFacing(fl);
    model = fn_802C6FC0(d->model, 0, 0);
    frame = *f;
    hidden = (fl >> 9) & 1;
    vol = v;
    facing = fn;
    id = i;
    if (d->shadow.model) {
        PVec* light = fn_802C9A40();
        u32 mask = -1;
        int j = 0;
        if (d->shadow.count > 0) {
            for (; j < d->shadow.count; j++) {
                PVec n;
                RotVec(&frame, &d->shadow.groups[j].normal, &n);
                if (PSVECDotProduct(light, &n) < 0.0f) {
                    mask &= ~(1 << d->shadow.groups[j].id);
                }
            }
        }
        fn_8029798C(d->shadow.model, &frame, mask, d->shadow.extend, d->shadow.cutoff);
    }
}

inline StaticObject::StaticObject(const char* name, const char* mdl, FacingFn fn, const ShadowDesc* s, int vis)
    : AnimModel(name, mdl) {
    facing = fn;
    if (InitShadow(s)) {
        fn_80297988(this);
    }
    noDraw = vis;
    if (vis) {
        next = lbl_8055EC48;
        lbl_8055EC48 = this;
    } else {
        next = 0;
    }
}

static inline int IsVolume(s32 kind) {
    switch (kind) {
    case 6:
    case 9:
    case 10:
    case 14:
    case 15:
    case 19:
    case 22:
        return 1;
    }
    return 0;
}

void ObstacleObjDesc::Create(const PlacedObj* o) {
    s32 sol = -1;
    phys.solId = -1;
    if (flags & 4) {
        int vol;
        if (o->node && o->node->Has("path")) {
            vol = 0;
        } else {
            vol = 0;
            u32 k = phys.kind - 6;
            if (k <= 16 && ((1 << k) & 0x12319)) {
                vol = 1;
            }
        }
        if (vol) {
            OFrame* sx;
            s32 id = lbl_8055EC78++;
            sx = 0;
            CollShape* s;
            switch (phys.kind) {
            case 6:
            case 15:
                s = fn_801FC12C(&phys);
                break;
            case 14:
            case 19:
                s = fn_801FC048(&phys);
                break;
            case 10:
                s = fn_801FBFE8(&phys);
                break;
            case 9:
                s = fn_801FBF48(&phys);
                break;
            case 22:
                s = fn_801FBEA8(&phys);
                break;
            default:
                fn_802E317C("eventwob: unhandled static object subtype %x");
                s = 0;
                break;
            }
            if (s) {
                s->SetId(phys.surface);
                sx = fn_801FBE1C(lbl_8055EC74, id, event, s, &o->frame, o->node);
            }
            StaticOb* so = new ("wall-gob") StaticOb(this, &o->frame, sx, id);
            if (flags & 0x80) {
                fn_801FBCF4(&lbl_8055EC50, so);
            } else {
                fn_801FBCF4(&lbl_8055EC4C, so);
            }
            if (flags & 2) {
                fn_802981EC(o, phys.shape.f[0]);
            }
            if (flags & 0x40) {
                fn_801F4DD4(effect, o);
            }
        } else {
            u32 fl = o->flags;
            phys.frame = o->frame;
            phys.node = o->node;
            if (fl & 1) {
                phys.flags = fl;
            }
            int vis = (flags >> 9) & 1;
            StaticObject* so =
                new ("EWObject") StaticObject((const char*)&phys, model, PickFacing(flags), &shadow, vis);
            fn_801E87D4(so);
        }
    } else {
        if (o->node) {
            sol = o->node->GetInt("sol_id", -1);
        }
        StaticOb* so = new ("wall-gob") StaticOb(this, &o->frame, 0, sol);
        if (flags & 0x80) {
            fn_801FBCF4(&lbl_8055EC50, so);
        } else {
            fn_801FBCF4(&lbl_8055EC4C, so);
        }
        if (flags & 2) {
            fn_802981EC(o, phys.shape.f[0]);
        }
        if (flags & 0x40) {
            fn_801F4DD4(effect, o);
        }
    }
}

extern "C" {

void fn_801FBCF4(StaticOb** list, StaticOb* o) {
    o->next = *list;
    *list = o;
}
}

static int FacingNone(const OFrame* f) { return -15; }

static int FacingZ(const OFrame* f) { return FacingMode(f, &f->z); }

static int FacingX(const OFrame* f) { return FacingMode(f, &f->x); }

extern "C" {

OFrame* fn_801FBE1C(SolBuilder* b, s32 id, s32 event, CollShape* s, const OFrame* f, ValueNode* node) {
    SolObj* o = (SolObj*)operator new(sizeof(SolObj), "StaticObject");
    if (o) {
        o = fn_801A4A8C(o, id, event, s, f, node);
    }
    o->next = b->head;
    b->head = o;
    return &o->frame;
}

CollShape* fn_801FBEA8(ObstacleDesc* d) {
    return new ("ewcv:tubevol") TubeVol(50000.0f, 1.0f, d->shape.f[0], d->shape.f[1]);
}

CollShape* fn_801FBF48(ObstacleDesc* d) {
    return new ("ewcv:ringvol") RingVol(50000.0f, 1.0f, d->shape.f[0], d->shape.f[1]);
}

CollShape* fn_801FBFE8(ObstacleDesc* d) {
    f32 k = 50000.0f;
    void* p = operator new(0xE0, "ewcv:polyvol");
    if (p) {
        p = fn_801A324C(p, 0, k, d->friction, d->shape.name);
    }
    return (CollShape*)p;
}

CollShape* fn_801FC048(ObstacleDesc* d) {
    f32 k = 50000.0f;
    return new ("ewcv:spherevol") SphereVol(0, 0, k, 0.2f, d->shape.f[0], RVec3(0.0f, 0.0f, 0.0f));
}

CollShape* fn_801FC12C(ObstacleDesc* d) {
    f32 k = 50000.0f;
    return new ("ewcv:wallvol") BoxVol(k, 0.2f, d->shape.f[0], d->shape.f[1], d->shape.f[2]);
}
}

AnimDesc::~AnimDesc(void) {}

extern "C" void fn_801FC4D8(AnimInst** list, AnimInst* a);
extern "C" void fn_801FC644(EWSound** list, EWSound* s);

void AnimDesc::Create(const PlacedObj* o) {
    place = o->frame;
    AnimInst* a = new ("val-gob") AnimInst;
    if (a) {
        ValueNode* node = o->node;
        a->name = "unknown";
        a->anim = fn_801E6A3C(animModel, streaming != 0, 0);
        a->count = numStreams;
        for (int i = 0; i < a->count; i++) {
            a->streams[i] = fn_801E6BEC(streams[i]);
        }
        fn_801E6CC8(a->anim, a->count <= 1);
        if (streaming) {
            a->state = 1;
            a->live = 1;
            if (node && lbl_8055EC68) {
                a->liveRad2 = node->GetFloat("live_radius", lbl_8055EC68->GetFloat("live_radius"));
            } else {
                a->liveRad2 = 100.0f;
            }
            a->liveRad2 = a->liveRad2 * a->liveRad2;
        } else {
            a->state = 2;
            StartAnim(a);
        }
        a->f5A = 0;
        a->frame = place;
        a->next = 0;
        a->tag = lbl_8055EC40.cur;
    }
    fn_801FC4D8(&lbl_8055EC5C, a);
}

extern "C" {
void fn_801FC4D8(AnimInst** list, AnimInst* a) {
    a->next = *list;
    *list = a;
}
}

void SoundDesc::Create(const PlacedObj* o) {
    EWSound* s = new ("EWSound") EWSound;
    if (s) {
        s->pos = o->frame.pos;
        ValueNode* node = o->node;
        const char* name = node->GetString("sound", lbl_8055EC70->GetValue("sound"));
        f32 vol = node->GetFloat("volume", lbl_8055EC70->GetFloat("volume"));
        s->snd = fn_802077A4(name, 8, &lbl_80488898, &s->pos);
        if (s->snd) {
            if (vol != s->snd->volume) {
                s->snd->dirty = 1;
                s->snd->volume = vol;
            }
            s->snd->nearDist = node->GetFloat("near_distance", s->snd->nearDist);
        }
        s->tag = lbl_8055EC40.cur;
    }
    fn_801FC644(&lbl_8055EC6C, s);
}

extern "C" {
void fn_801FC644(EWSound** list, EWSound* s) {
    s->next = *list;
    *list = s;
}
}

void SoundDesc::Props(void* p, PlacedObj* o) { fn_80204014(p, o->node, lbl_8055EC70); }

f32 ObjDesc::vf1C(void) { return 0.0f; }

void ObjDesc::vf20(void) {}

void ObjDesc::vf24(void) {}

StaticObject::~StaticObject(void) {
    StaticObject** pp = &lbl_8055EC48;
    StaticObject* s;
    while ((s = *pp) != 0) {
        if (s == this) {
            *pp = next;
            break;
        }
        pp = &s->next;
    }
}

void StaticObject::Draw(void) {
    if (!noDraw && fn_801F7A0C()) {
        fn_802C70AC(model, &frame, -1, -15);
    }
}

static inline f32 FMin(f32 a, f32 b) { return b < a ? b : a; }
static inline f32 FMax(f32 a, f32 b) { return a > b ? a : b; }

void StaticObject::vf1C(void) {
    f32 s = fn_801E88F0(-1);
    f32 s2 = s * s;
    f32 d2 = fn_802C88E8(&frame.pos) * s2;
    f32 a;
    if (d2 > shadow.cutoff * shadow.cutoff) {
        f32 d = (f32)sqrt(d2);
        f32 t = FMax(FMin(((d - shadow.cutoff) - 0.0f) / 5.0f, 1.0f), 0.0f);
        a = shadow.extend + t * (0.0f - shadow.extend);
    } else {
        a = shadow.extend;
    }
    if (!((f32)__fabs(a) <= lbl_80558DC4[0])) {
        fn_802C7AE0((void*)shadow.h, &frame, -1, a);
    }
}
