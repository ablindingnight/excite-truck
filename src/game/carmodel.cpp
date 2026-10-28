// Car models (0x801EC1D4-0x801EEFC8, carmodel.cpp): the drawable car bound to a racer's physics state -
// LOD models from "<car>.car/lod.val" (with mirror/shadow LODs and per-LOD wheel/rider flags), bone
// surfaces, the xray efx mesh, the four wheels and the cockpit gauges (speedometer, tachometer, oil,
// water, fuel, volt needles and the speedo slider). Car 0 draws every car each pass, the player's own
// car last. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
int sscanf(const char* s, const char* fmt, ...);
char* strchr(const char* s, int c);
double atan2(double y, double x);
f32 PSVECDotProduct(const void* a, const void* b);
f32 PSVECMag(const void* v);
}

void* operator new(size_t size, const char* tag);

struct Vec {
    f32 x;
    f32 y;
    f32 z;
};

struct Frame {
    Vec axes[3];  // offset 0x0
    Vec pos;      // offset 0x24
};

struct AnimSlot {
    Frame frame;  // offset 0x0
    s16 flags;    // offset 0x30
};

class AnimObj {
public:
    AnimObj(void);
    virtual ~AnimObj(void) {}
    virtual void vf0C(void) {}
    virtual void Draw(void) = 0;
    virtual void vf14(void) {}
    virtual f32 Depth(void);
    virtual void vf1C(void);
    virtual void Set(const AnimSlot* s);
    void Bind(const char* name);
    s16 Flags(void);

    Frame frame;  // offset 0x4
    s32 offset;   // offset 0x34
    s16 flags;    // offset 0x38
};

struct WheelState {
    u8 pad0[0x34];
    f32 spin;  // offset 0x34
    u8 pad38[0x34];
};

struct BoneFrame {
    Frame frame;  // offset 0x0
    u32 mask;     // offset 0x30
};

// The racer's physics/animation state the model is bound to.
struct CarState {
    Frame frame;              // offset 0x0
    u8 pad30[4];
    f32 rpm;                  // offset 0x34
    f32 x38;                  // offset 0x38
    f32 wheelTurn;            // offset 0x3C
    f32 x40;                  // offset 0x40
    f32 x44;                  // offset 0x44
    u8 pad48[4];
    f32 speed;                // offset 0x4C
    u8 pad50[0x1E];
    u16 flags6E;              // offset 0x6E
    u8 pad70[0xC];
    WheelState wheels[4];     // offset 0x7C
    f32 x22C;                 // offset 0x22C
    u8 pad230[0x5C];
    f32 x28C;                 // offset 0x28C
    u8 pad290[0xC];
    u32 groupMask;            // offset 0x29C
    f32 waterTemp;            // offset 0x2A0
    u8 pad2A4[8];
    f32 oilTemp;              // offset 0x2AC
    f32 oilPressure;          // offset 0x2B0
    u8 pad2B4[4];
    f32 fuel;                 // offset 0x2B8
    u8 pad2BC[4];
    u32 flags2C0;             // offset 0x2C0
    u8 pad2C4[8];
    f32 alpha;                // offset 0x2CC
    f32 scale;                // offset 0x2D0
    f32 x2D4;                 // offset 0x2D4
    u8 pad2D8[8];
    Vec bones[162];           // offset 0x2E0
    s32 numBoneFrames;        // offset 0xA78
    BoneFrame boneFrames[18]; // offset 0xA7C
    Vec effect[32];           // offset 0xE24
    u8 padFA4[4];
    s32 carIndex;             // offset 0xFA8
};

struct DriverInfo {
    u8 pad[0x50];
    s32 color;  // offset 0x50
};

DriverInfo* FindDriver(s32 id);

// The racer description the model is built for.
struct Racer {
    char name[0xE0];     // offset 0x0
    s32 index;           // offset 0xE0
    s32 lodModel;        // offset 0xE4
    s32 color;           // offset 0xE8
    char xrayMesh[32];   // offset 0xEC
    f32 wheelSize[4];    // offset 0x10C
};

struct CarDef {
    u8 pad[0x44];
    char name[0x24];  // offset 0x44
    s32 driver;       // offset 0x68
    s32 unk6C;        // offset 0x6C
};

struct CarRes {
    s32 numBones;           // offset 0x0
    u8 pad4[0x2130];
    ValueTable* values;     // offset 0x2134
};

struct GaugePair {
    f32 val;    // offset 0x0
    f32 angle;  // offset 0x4
};

// A needle bone driven by one float through a piecewise-linear value->angle table.
struct Gauge {
    Gauge(void) {
        bone = 0;
        src = 0;
    }
    ~Gauge(void);

    s32 bone;              // offset 0x0
    f32 min;               // offset 0x4
    f32 max;               // offset 0x8
    GaugePair pairs[8];    // offset 0xC
    s32 count;             // offset 0x4C
    const f32* src;        // offset 0x50
};

struct Slider {
    const f32* src;  // offset 0x0
    f32 ky;          // offset 0x4
    f32 by;          // offset 0x8
    f32 kz;          // offset 0xC
    f32 bz;          // offset 0x10
};

struct LodEntry {
    f32 dist;     // offset 0x0
    u8 noMirror;  // offset 0x4
    u8 rider;     // offset 0x5
    u8 wheels;    // offset 0x6
};

struct ShadowLod {
    s32 model;  // offset 0x0
    f32 dist;   // offset 0x4
};

struct BoneSurf {
    s32 first;  // offset 0x0
    s32 last;   // offset 0x4
    u32 mask;   // offset 0x8
};

struct CarColors {
    u32 c0;     // offset 0x0
    u32 c1;     // offset 0x4
    f32 alpha;  // offset 0x8
};

struct BoneArg {
    s32 a;
    s32 b;
};

class CarModel : public AnimObj {
public:
    CarModel(Racer* r, CarDef* d);
    virtual ~CarModel(void);
    virtual void Draw(void);
    virtual void vf14(void);
    virtual void vf1C(void);
    virtual void Set(const AnimSlot* s);
    virtual void vf24(void);

    CarState* car;          // offset 0x3C
    s32 index;              // offset 0x40
    s32 lodModels[16];      // offset 0x44
    s32 drawModels[16];     // offset 0x84
    s32 unkC4;              // offset 0xC4
    LodEntry lodTable[16];  // offset 0xC8
    s32 numLods;            // offset 0x148
    s32 mirrorLod;          // offset 0x14C
    s32 numBoneSurfs;       // offset 0x150
    BoneSurf boneSurfs[2];  // offset 0x154
    s32* models;            // offset 0x16C
    LodEntry* lods;         // offset 0x170
    s32* lodCount;          // offset 0x174
    s32 chassis;            // offset 0x178
    s32 wheelBone;          // offset 0x17C
    s32 shiftLight;         // offset 0x180
    void* rider;            // offset 0x184
    CarRes* res;            // offset 0x188
    Gauge speedometer;      // offset 0x18C
    Gauge tachometer;       // offset 0x1E0
    Gauge oilPressure;      // offset 0x234
    Gauge oilTemp;          // offset 0x288
    Gauge waterTemp;        // offset 0x2DC
    Gauge fuel;             // offset 0x330
    Gauge volts;            // offset 0x384
    Slider speedoSlider;    // offset 0x3D8
    s32 color;              // offset 0x3EC
    void* tire;             // offset 0x3F0
    u8 drawWheel;           // offset 0x3F4
    s32 curLod;             // offset 0x3F8
    s32 minLod;             // offset 0x3FC
    ShadowLod shadows[16];  // offset 0x400
    s32 numShadows;         // offset 0x480
    f32 shadowRadius;       // offset 0x484
    s32 xray;               // offset 0x488
    s32 facingPos;          // offset 0x48C
    s32 facingNeg;          // offset 0x490
    s32 xrayBones;          // offset 0x494
    u32 suspGroup;          // offset 0x498
    u32 xrayMask;           // offset 0x49C
    Frame scaled;           // offset 0x4A0
    f32 scale;              // offset 0x4D0
    void* wheels[4];        // offset 0x4D4
};

class PlayerCarModel : public CarModel {
public:
    PlayerCarModel(Racer* r, CarDef* d);
    virtual ~PlayerCarModel(void);
    virtual void Set(const AnimSlot* s);
    virtual int vf28(void);
};

extern "C" {
s32 fn_801E3004(s32 model, const char* name, s32 a);
int fn_801E320C(s32 model, const char* name);
void fn_801E323C(s32 bone);
void fn_801E34E4(s32 bone, const Frame* f);
void fn_801E353C(s32 bone, f32 angle);
s32 fn_801E7E2C(void);
s32 fn_801E866C(s32 a);
f32 fn_801E8738(s32 a);
struct ViewInfo* fn_801E87A8(void);
f32 fn_801E88F0(s32 a);
int fn_801E8954(s32 a);
int fn_801E897C(s32 a);
int fn_801E89A4(s32 a);
int fn_801E89C8(s32 a);
int fn_801E9E78(void* a, Vec* p, Vec* n, s32* c);
void fn_801EA59C(s32 index, int a, CarDef* d);
void fn_801EAB10(int idx, s32 x8);  // carinfo.cpp
void* fn_801EF1F0(void* p, const char* name);
void fn_801EF428(void* p, int del);
void fn_801EFC90(void* p, CarState* c, void** wheels);
void* fn_801EFF94(const char* cls, s32 index, int i);
void fn_801EFFF8(void* w);
void fn_801F0F78(void* w, WheelState* s, const Frame* f, f32 scale);
void fn_801F131C(void* w, f32 spin, int mine);
void fn_801F1378(void* w, f32 spin);
void fn_801F13F4(void* w, f32 a, f32 b);
f32 fn_801F159C(void* w);
void* fn_802002D4(const char* name, s32 model, s32 inst);
void fn_8020095C(void* p);
void fn_80201018(void* p, CarState* c, int a);
void* fn_802792E4(const char* name);
void fn_802792EC(void* p);
void fn_80297988(CarModel* m);
void fn_802AB770(s32 index);
void fn_802AB994(void);
ValueTable* fn_80131808(const char* name);
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* t);
void fn_8012E81C(const char* name);
int fn_8012C33C(const char* name);
int fn_801333E8(int a, const char* name, u8* flag);
int fn_801345D8(char* buf, int n, const char* fmt, ...);
void* fn_80179CB0(void);
struct ModelInfo* fn_802C6C34(s32 model);
void fn_802C6CF8(s32 model);
s32 fn_802C6FC0(const char* name, s32 inst, const char* prefix);
void fn_802C70AC(s32 model, const Frame* f, s32 mat, u32 mask);
void fn_802C7AE0(s32 model, const Frame* f, u32 mask, f32 t);
u32 fn_802C7830(const char* s);
s32 fn_802C7A34(int a, void* p, s32 n);
s32 fn_802C7A40(int a, void* p, s32 n, int b, BoneArg* c, int d);
const Frame* fn_802C86DC(void);
void fn_802C88BC(int a);
f32 fn_802C88D4(const Vec* p);
f32 fn_802C88E8(const Vec* p);
void fn_802C8B98(s32 tex, const Frame* f);
void fn_802C8D74(void);
void fn_802C8DB8(void* v, int nv, u16* idx, int ni);
void fn_802C974C(f32 a, f32 b, f32 c);
u32 fn_802C9950(u32 c);
u32 fn_802C9994(u32 c);
f32 fn_802C99E8(f32 a);
const Vec* fn_802C9A40(void);
void fn_802DAC54(void);
void fn_802E317C(const char* fmt, ...);
}

struct ViewInfo {
    u8 pad[4];
    u32 mode;    // offset 0x4
    u8 pad8[8];
    u32 flags;   // offset 0x10
};

struct ModelInfo {
    u8 pad[0x1C];
    f32 radius;  // offset 0x1C
    Vec center;  // offset 0x20
};

extern f32 lbl_80558DC4[];  // FLT_EPSILON
extern u8 lbl_8055E614;

static inline f32 Angle(f32 a) { return a; }
static inline s32 Mask(s32 v) { return v; }

CarModel* lbl_80488648[8];
CarColors lbl_80488668;

ModuleTag lbl_8055EBA8;
UnkStaticFFFF lbl_8055EBB0;
f32 lbl_8055EBB4 = Angle(0.7853982f);
UnkStatic7F lbl_8055EBB8;
f32 lbl_8055EBBC = Angle(1.0471976f);  // wheel turn scale
s32 lbl_8055EBC0 = Mask(-0x100);
u8 lbl_8055EBC4;
CarModel* lbl_8055EBC8;  // the player's own car

u8 lbl_8055AFC8 = 1;      // draw the cockpit
f32 lbl_8055B088 = 12.3f;  // battery volts

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void MulVec(Vec* out, const Vec* in, const Vec* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m[0].x, in->y, m[1].x, in->z, m[2].x);
        f32 y = Dot3(in->x, m[0].y, in->y, m[1].y, in->z, m[2].y);
        f32 z = Dot3(in->x, m[0].z, in->y, m[1].z, in->z, m[2].z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m[0].x, in->y, m[1].x, in->z, m[2].x);
        out->y = Dot3(in->x, m[0].y, in->y, m[1].y, in->z, m[2].y);
        out->z = Dot3(in->x, m[0].z, in->y, m[1].z, in->z, m[2].z);
    }
}

static inline void MulVecT(Vec* out, const Vec* in, const Vec* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m[0].x, in->y, m[0].y, in->z, m[0].z);
        f32 y = Dot3(in->x, m[1].x, in->y, m[1].y, in->z, m[1].z);
        f32 z = Dot3(in->x, m[2].x, in->y, m[2].y, in->z, m[2].z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m[0].x, in->y, m[0].y, in->z, m[0].z);
        out->y = Dot3(in->x, m[1].x, in->y, m[1].y, in->z, m[1].z);
        out->z = Dot3(in->x, m[2].x, in->y, m[2].y, in->z, m[2].z);
    }
}

static inline void StrCopy(char* d, const char* s, int n) {
    char* p = d;
    while ((*p++ = *s++) != 0) {
        if (--n <= 0) {
            p[-1] = 0;
            break;
        }
    }
}

static inline void StrCat(char* d, const char* s, int n) {
    while (*d) {
        n--;
        d++;
    }
    if (n > 0) {
        StrCopy(d, s, n);
    }
}

static inline CarModel* CarAt(u32 i) { return i < 8 ? lbl_80488648[i] : 0; }

extern "C" {

void fn_801EC1D4(void) {
    lbl_80488668.c0 = 0x00555766;
    lbl_80488668.c1 = 0x00FFF5D9;
}

CarColors* fn_801EC1F8(void) { return &lbl_80488668; }

u32* fn_801EC204(void) { return &lbl_80488668.c1; }

void fn_801EC214(Gauge* g, ValueNode* n, s32 model, const char* name, f32 scale, f32 offset, const f32* src) {
    g->src = src;
    if (!n->Has(name)) {
        g->bone = 0;
        return;
    }
    f32 cx = 0.0f;
    f32 cy = 0.0f;
    if (sscanf(n->GetValue(name), "%f, %f", &cx, &cy) != 2) {
        fn_802E317C("%s: bad format", name);
    }
    g->bone = fn_801E3004(model, name, -1);
    g->count = 0;
    for (; g->count < 8; g->count++) {
        char key[32];
        fn_801345D8(key, 32, "%s%d", name, g->count + 1);
        if (!n->Has(key)) {
            break;
        }
        GaugePair* p = &g->pairs[g->count];
        f32 x = 0.0f;
        f32 y = 0.0f;
        if (sscanf(n->GetValue(key), "%f %f,%f", &p->val, &x, &y) != 3) {
            fn_802E317C("gauge %s: bad format", key);
        }
        p->angle = atan2(x - cx, -(y - cy));
        p->val = p->val * (scale - offset) + offset;
    }
    if (g->count < 2) {
        fn_802E317C("gauge %s: pair_count < 2", name);
    }
    g->min = g->pairs[0].val;
    g->max = g->pairs[g->count - 1].val;
}

}  // extern "C"

Gauge::~Gauge(void) {
    if (bone) {
        fn_801E323C(bone);
    }
}

extern "C" {

void fn_801EC4A8(Gauge* g, const Frame* f) {
    if (g->bone) {
        f32 v = g->max < *g->src ? g->max : *g->src;
        v = v > g->min ? v : g->min;
        int i;
        for (i = 0; i < g->count - 2; i++) {
            if (g->pairs[i + 1].val > v) {
                break;
            }
        }
        GaugePair* a = &g->pairs[i];
        GaugePair* b = &g->pairs[i + 1];
        fn_801E353C(g->bone, -(a->angle + (b->angle - a->angle) * ((v - a->val) / (b->val - a->val))));
        fn_801E34E4(g->bone, f);
    }
}

void fn_801EC59C(Slider* s, ValueNode* n, const char* name, const f32* src, f32 scale, f32 offset) {
    s->src = src;
    ValueNode* c = n->FindChild(name);
    if (c) {
        ValVec3 p0 = c->GetVec3("val1");
        ValVec3 p1 = c->GetVec3("val2");
        p1.x = p1.x * scale - offset;
        p0.x = p0.x * scale - offset;
        p0.y = p0.y * 0.0254f;
        p1.y = p1.y * 0.0254f;
        p0.z = p0.z * 0.0254f;
        p1.z = p1.z * 0.0254f;
        f32 dx = p1.x - p0.x;
        s->ky = (p1.y - p0.y) / dx;
        s->kz = (p1.z - p0.z) / dx;
        s->by = p0.y - s->ky * p0.x;
        s->bz = p0.z - s->kz * p0.x;
    } else {
        s->bz = 0.0f;
        s->kz = 0.0f;
        s->by = 0.0f;
        s->ky = 0.0f;
    }
}

void fn_801EC714(const Slider* s, f32* y, f32* z) {
    *y = s->by + *s->src * s->ky;
    *z = s->bz + *s->src * s->kz;
}

}  // extern "C"

extern "C" {
void fn_801EE0E0(CarModel* m, const char* name, s32 x, s32 inst, const char* dir);
void fn_801EE974(CarModel* m, Racer* r);
void fn_801EEA64(CarModel* m, Racer* r, const char* name, const char* dir);
void fn_801EEC68(CarModel* m);
void fn_801ECCE0(CarModel* m, u32 pass);
}

CarModel::CarModel(Racer* r, CarDef* d) {
    car = 0;
    index = r->index;
    if (r->index == 0) {
        fn_801EC1D4();
    }
    char buf[64];
    StrCopy(buf, d->name, 64);
    StrCat(buf, ".car", 64);
    fn_8012E81C(buf);
    res = (CarRes*)fn_802792E4(d->name);
    fn_801EA59C(r->index, 0, d);
    models = drawModels;
    lods = lodTable;
    lodCount = &numLods;
    s32 inst = d->driver >= 0 ? r->index : 0;
    char dir[64];
    fn_801345D8(dir, 64, "%s.car", d->name);
    fn_801EE0E0(this, d->name, d->unk6C, inst, dir);
    fn_801EE974(this, r);
    fn_801EEA64(this, r, d->name, dir);
    lbl_80488648[index] = this;
    r->lodModel = lodModels[1];
    s32 c = FindDriver(d->driver)->color;
    color = c;
    r->color = c;
    Bind(r->name);
    drawWheel = 1;
    if (*(u32*)r->name == 'PCRA') {
        fn_801333E8(1, "draw_player_wheel", &drawWheel);
    }
    if (*(u32*)r->name == 'PCRA' && fn_801E87A8()->mode == 0x180304) {
        lbl_8055EBC8 = this;
    }
}

CarModel::~CarModel(void) {
    fn_801EEC68(this);
    int i;
    for (i = 0; i < numLods; i++) {
        drawModels[i] = -1;
        fn_802C6CF8(lodModels[i]);
    }
    for (i = 0; i < numShadows; i++) {
        fn_802C6CF8(shadows[i].model);
    }
    fn_802C6CF8(chassis);
    fn_801E323C(wheelBone);
    fn_801E323C(shiftLight);
    fn_8020095C(rider);
    for (i = 0; i < 4; i++) {
        fn_801EFFF8(wheels[i]);
        wheels[i] = 0;
    }
    lbl_80488648[index] = 0;
    fn_801EF428(tire, 1);
    fn_802792EC(res);
    res = 0;
    if (lbl_8055EBC8 == this) {
        lbl_8055EBC8 = 0;
    }
}

extern "C" {

int fn_801ECB60(CarModel* m) { return m->car->x2D4 > lbl_80558DC4[0]; }

void fn_801ECB80(CarModel* m) {
    f32 d = fn_802C88D4(&m->frame.pos);
    d *= fn_801E88F0(-1);
    m->curLod = *m->lodCount - 1;
    for (int i = 0; i < *m->lodCount; i++) {
        if (d < m->lods[i].dist) {
            m->curLod = i;
            break;
        }
    }
    if (m->curLod < m->minLod) {
        m->curLod = m->minLod;
    }
    if (m->curLod >= *m->lodCount) {
        m->curLod = *m->lodCount - 1;
    }
    if (m->index == fn_801E866C(-1)) {
        m->curLod = m->curLod > 1 ? 1 : m->curLod;
    }
}

f32 fn_801ECC70(CarModel* m) { return fn_802C99E8(lbl_80488668.alpha * m->car->alpha); }

int fn_801ECC8C(CarModel* m) {
    int r = 0;
    if (fn_801ECB60(m) || m == lbl_8055EBC8) {
        r = 1;
    }
    return r;
}

void fn_801ECF58(CarModel* m);
void fn_801ECFE0(CarModel* m);
void fn_801ED2D0(CarModel* m);
void fn_801ED354(CarModel* m);
void fn_801ED46C(CarModel* m);

void fn_801ECCE0(CarModel* m, u32 pass) {
    if (m->index != 0) {
        return;
    }
    CarModel** p = lbl_80488648;
    for (int i = 0; *p && i < 8; p++, i++) {
        if (pass != fn_801ECC8C(*p)) {
            fn_801ECB80(*p);
        }
    }
    CarModel* mine = CarAt(fn_801E866C(-1));
    CarModel* list[8];
    int n = 0;
    p = lbl_80488648;
    CarModel** q = list;
    for (int i = 0; *p && i < 8; p++, i++) {
        if (*p != mine) {
            *q++ = *p;
            n++;
        }
    }
    lbl_80488668.alpha = fn_802C99E8(1.0f);
    u32 c0 = fn_802C9950(lbl_80488668.c0);
    u32 c1 = fn_802C9994(lbl_80488668.c1);
    for (int i = 0; i < n; i++) {
        CarModel* c = list[i];
        u32 t = fn_801ECC8C(c);
        if (t != pass) {
            if (t) {
                fn_802AB770(c->index);
            }
            fn_801ECF58(c);
            if (t) {
                fn_802AB994();
            }
        }
    }
    for (int i = 0; i < n; i++) {
        CarModel* c = list[i];
        u32 t = fn_801ECC8C(c);
        if (t != pass) {
            if (t) {
                fn_802AB770(c->index);
            }
            fn_801ECFE0(c);
            if (t) {
                fn_802AB994();
            }
        }
    }
    for (int i = 0; i < n; i++) {
        CarModel* c = list[i];
        u32 t = fn_801ECC8C(c);
        if (t != pass) {
            if (t) {
                fn_802AB770(c->index);
            }
            fn_801ED2D0(c);
            if (t) {
                fn_802AB994();
            }
        }
    }
    u32 t = fn_801ECC8C(mine);
    if (t != pass) {
        if (t) {
            fn_802AB770(mine->index);
        }
        fn_801ED354(mine);
        if (t) {
            fn_802AB994();
        }
    }
    fn_802C99E8(lbl_80488668.alpha);
    fn_802C9994(c1);
    fn_802C9950(c0);
}

void fn_801ECF58(CarModel* m) {
    if (m->rider) {
        int hide = 0;
        if (m->lods[m->curLod].rider && !(m->car->flags2C0 & 0x100)) {
            hide = 1;
        }
        fn_801ECC70(m);
        fn_80201018(m->rider, m->car, hide);
    }
}

static inline f32 Abs(f32 x) { return __fabs(x); }

void fn_801ECFE0(CarModel* m) {
    if (m->car->x22C < 0.01f) {
        return;
    }
    fn_801ECC70(m);
    int mirror = (fn_801E87A8()->flags >> 1) & 1;
    BoneSurf* bs = m->boneSurfs;
    s32 tint = lbl_8055EBC0;
    for (int i = 0; i < m->numBoneSurfs; bs++, i++) {
        u32 mask = m->car->groupMask & bs->mask;
        BoneArg arg;
        arg.a = m->color;
        arg.b = tint;
        if (Abs(m->car->x28C) <= lbl_80558DC4[0]) {
            fn_802DAC54();
        }
        s32 h = fn_802C7A40(1, &m->car->bones[bs->first + 1], bs->last - bs->first, 1, &arg, 2);
        for (int j = 0; j < m->car->numBoneFrames; j++) {
            BoneFrame* bf = &m->car->boneFrames[j];
            if (bf->mask & mask) {
                if (mirror) {
                    Frame f = bf->frame;
                    f.axes[0].x *= -1.0f;
                    f.axes[0].y *= -1.0f;
                    f.axes[0].z *= -1.0f;
                    fn_802C70AC(m->models[m->curLod], &f, h, bf->mask);
                } else {
                    fn_802C70AC(m->models[m->curLod], &bf->frame, h, bf->mask);
                }
                mask &= ~bf->mask;
            }
        }
        if (!lbl_8055E614) {
            if (mirror) {
                Frame f = m->scaled;
                f.axes[0].x *= -1.0f;
                f.axes[0].y *= -1.0f;
                f.axes[0].z *= -1.0f;
                fn_802C70AC(m->models[m->curLod], &f, h, mask);
            } else {
                fn_802C70AC(m->models[m->curLod], &m->scaled, h, mask);
            }
        }
    }
    fn_801ED46C(m);
}

void fn_801ED2D0(CarModel* m) {
    fn_801ECC70(m);
    int mine = fn_801E866C(-1) == m->index;
    for (int i = 0; i < 4; i++) {
        fn_801F131C(m->wheels[i], m->car->wheels[i].spin, mine);
    }
}

void fn_801EDD68(CarModel* m);

void fn_801ED354(CarModel* m) {
    if (fn_801E8954(-1)) {
        fn_801ECF58(m);
        fn_801ECFE0(m);
        if (m->lods[m->curLod].noMirror) {
            for (int i = 0; i < 4; i++) {
                fn_801F131C(m->wheels[i], m->car->wheels[i].spin, 1);
            }
        }
    } else if (fn_801E897C(-1) || lbl_8055EBC4) {
        fn_801ECC70(m);
        for (int i = 0; i < 4; i++) {
            fn_801F1378(m->wheels[i], m->car->wheels[i].spin);
        }
        fn_801EDD68(m);
    }
}

void fn_801ED46C(CarModel* m) {
    if (m->xray != -1 && m->curLod <= 2) {
        CarState* c = m->car;
        Vec v;
        MulVecT(&v, &fn_802C86DC()->axes[0], c->frame.axes);
        Vec* e = &m->car->effect[m->facingPos];
        e->x = v.x;
        e->y = v.y;
        e->z = v.z;
        e = &m->car->effect[m->facingNeg];
        f32 neg = -1.0f;
        e->x = v.x * neg;
        e->y = v.y * neg;
        e->z = v.z * neg;
        s32 h = fn_802C7A34(1, &m->car->effect[1], m->xrayBones);
        if (m->curLod <= 1 && !(m->car->flags6E & 0x20)) {
            fn_802C70AC(m->xray, &m->scaled, h, m->suspGroup);
        } else {
            fn_802C70AC(m->xray, &m->frame, h, m->xrayMask);
        }
    }
}

}  // extern "C"

void CarModel::Draw(void) {
    fn_801ECCE0(this, 1);
}

void CarModel::vf24(void) {
    fn_801ECCE0(this, 0);
}

extern "C" void fn_801ED718(CarModel* m);

void CarModel::vf14(void) {
    if (index == fn_801E866C(-1) && fn_801E89C8(-1)) {
        lbl_80488668.alpha = fn_802C99E8(car->alpha);
        fn_801ED718(this);
        fn_802C99E8(lbl_80488668.alpha);
    }
}

struct P2 {
    P2(void) {}
    P2(f32 x, f32 y) {
        a = x;
        b = y;
    }
    P2(f32 x, u32 y, int) {
        a = x;
        ib = y;
    }
    union {
        f32 a;
        u32 ia;
    };
    union {
        f32 b;
        u32 ib;
    };
};

static inline void SetVtx(P2* v, f32 x, f32 y, f32 s, f32 t) {
    v[0] = P2(x, y);
    v[1] = P2(0.0f, 0xFFFFFFFF, 0);
    v[2] = P2(s, t);
}

extern "C" {

void fn_801ED718(CarModel* m) {
    fn_802C974C(0.1f, 20.0f, fn_801E8738(-1));
    fn_802C88BC(2);
    s32 tex = fn_801E7E2C();
    if (tex != -1) {
        P2 v[12];
        for (int i = 0; i < 12; i++) {
            v[i].ia = 0;
            v[i].ib = 0;
        }
        SetVtx(&v[0], -0.09f, -0.0225f, 0.0f, 0.25f);
        SetVtx(&v[3], 0.09f, -0.0225f, 1.0f, 0.25f);
        SetVtx(&v[6], 0.09f, 0.0225f, 1.0f, 0.0f);
        SetVtx(&v[9], -0.09f, 0.0225f, 0.0f, 0.0f);
        u16 idx[6] = {0, 3, 1, 1, 3, 2};
        Frame f = m->frame;
        Vec off;
        Vec w;
        off.x = -0.3f;
        off.y = 1.1f;
        off.z = 0.12f;
        MulVec(&w, &off, f.axes);
        f.pos.x = f.pos.x + w.x;
        f.pos.y = f.pos.y + w.y;
        f.pos.z = f.pos.z + w.z;
        fn_802C8B98(tex, &f);
        fn_802C8DB8(v, 4, idx, 6);
        fn_802C8D74();
    }
    if (!lbl_8055AFC8) {
        fn_802C70AC(m->chassis, &m->frame, 0, 5);
    } else {
        Vec b[23];
        int i;
        for (i = 0; i < 23; i++) {
            b[i].x = 0.0f;
            b[i].y = 0.0f;
            b[i].z = 0.0f;
        }
        Vec k;
        k.x = 0.0f;
        k.y = 0.0254f;
        k.z = 0.127f;
        f32 s = m->car->x38;
        b[1].x = k.x * s;
        b[1].y = k.y * s;
        b[1].z = k.z * s;
        s = m->car->x40;
        b[2].x = k.x * s;
        b[2].y = k.y * s;
        b[2].z = k.z * s;
        s = 1.0f - m->car->x44;
        b[3].x = k.x * s;
        b[3].y = k.y * s;
        b[3].z = k.z * s;
        b[20] = b[1];
        fn_801EC714(&m->speedoSlider, &b[4].y, &b[5].x);
        fn_802C70AC(m->chassis, &m->frame, fn_802C7A34(1, &b[1], 22), 0xFFFFFFFF);
        fn_802C70AC(m->models[1], &m->frame, 0, m->car->groupMask & 0xFFFDFF9F);
    }
    fn_801EC4A8(&m->speedometer, &m->frame);
    fn_801EC4A8(&m->tachometer, &m->frame);
    fn_801EC4A8(&m->oilPressure, &m->frame);
    fn_801EC4A8(&m->oilTemp, &m->frame);
    fn_801EC4A8(&m->waterTemp, &m->frame);
    fn_801EC4A8(&m->fuel, &m->frame);
    fn_801EC4A8(&m->volts, &m->frame);
    if (m->car->flags6E & 2) {
        fn_801E34E4(m->shiftLight, &m->frame);
    }
    if (m->drawWheel) {
        fn_801E353C(m->wheelBone, -lbl_8055EBBC * m->car->wheelTurn);
        fn_801E34E4(m->wheelBone, &m->frame);
    }
}

void fn_801EDD68(CarModel* m) { fn_801EFC90(m->tire, m->car, m->wheels); }

}  // extern "C"

void CarModel::Set(const AnimSlot* s) {
    AnimObj::Set((const AnimSlot*)car);
    f32 k = ((const CarState*)s)->scale;
    scale = k;
    if (Abs(k - 1.0f) <= lbl_80558DC4[0]) {
        scaled = frame;
    } else {
        f32 xx = frame.axes[0].x * k;
        f32 xy = frame.axes[0].y * k;
        f32 xz = frame.axes[0].z * k;
        f32 yx = frame.axes[1].x * k;
        f32 yy = frame.axes[1].y * k;
        f32 yz = frame.axes[1].z * k;
        f32 zx = frame.axes[2].x * k;
        f32 zy = frame.axes[2].y * k;
        scaled.pos = frame.pos;
        scaled.axes[0].x = xx;
        scaled.axes[0].y = xy;
        scaled.axes[0].z = xz;
        scaled.axes[1].x = yx;
        scaled.axes[1].y = yy;
        scaled.axes[1].z = yz;
        scaled.axes[2].x = zx;
        scaled.axes[2].y = zy;
        scaled.axes[2].z = (frame.axes[2].z * k);
    }
    for (int i = 0; i < 4; i++) {
        fn_801F0F78(wheels[i], &car->wheels[i], &frame, scale);
    }
}

struct ShadowQuery {
    s16 flags;
    s32 hit;
};

void CarModel::vf1C(void) {
    f32 s = fn_801E88F0(-1);
    s = s * s;
    s = s * fn_802C88E8(&frame.pos);
    int i = numShadows - 1;
    while (i >= 0 && s < shadows[i].dist * shadows[i].dist) {
        i--;
    }
    if (i < 0 || (index == fn_801E866C(-1) && !fn_801E89A4(-1))) {
        return;
    }
    Vec p = frame.pos;
    ShadowQuery q;
    q.flags = Flags();
    Vec up;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    q.hit = 0;
    f32 k;
    f32 h;
    if (fn_801E9E78(&q, &p, &up, &q.hit)) {
        f32 d = -PSVECDotProduct(&up, fn_802C9A40());
        h = frame.pos.y - p.y;
        k = 1.0f / (0.1f > d ? 0.1f : d);
    } else {
        h = 0.0f;
        k = 1.5f;
    }
    fn_802C7AE0(shadows[i].model, &scaled, 0xFFFFFFFF, k * (h + shadowRadius));
    for (int j = 0; j < 4; j++) {
        fn_801F13F4(wheels[j], s, h * k);
    }
}

extern "C" {

s32 fn_801EE0C8(CarModel* m) { return m->lodModels[1]; }

f32 fn_801EE0D0(CarModel* m, int i) { return fn_801F159C(m->wheels[i]); }

void fn_801EE0E0(CarModel* m, const char* name, s32 x, s32 inst, const char* dir) {
    char path[256];
    fn_801345D8(path, 256, "%s.car/lod.val", name);
    ValueTable* t = fn_80131808(path);
    ValueNode* n;
    int k = 4;
    do {
        char key[64];
        fn_801345D8(key, 64, "detail%d", k + 1);
        n = t->FindChild(key);
        k--;
    } while (!n && k >= 0);
    if (!n) {
        fn_802E317C("Can't get detail table for %s", name);
    }
    ValueNode* use = (ValueNode*)t->FindChild(n->GetValue("use"));
    m->minLod = n->GetIntValue("min");
    n->GetFloat("scale");
    m->mirrorLod = n->GetIntValue("mirror");
    if (m->mirrorLod < m->minLod) {
        m->minLod = m->mirrorLod;
    }
    m->numLods = 0;
    char tex[256];
    fn_801345D8(tex, 256, "%s1.tex", name);
    use->GetString("base_tex", tex);
    use->GetString("base_texa", 0);
    char mdl[64];
    fn_801345D8(mdl, 64, "%s/%s", dir, use->GetValue("model"));
    for (;;) {
        char key[64];
        fn_801345D8(key, 64, "lod%d", m->numLods);
        if (!use->Has(key)) {
            break;
        }
        f32 dist = 0.0f;
        char mode[32] = "n";
        if (sscanf(use->GetValue(key), "%f %s", &dist, mode) != 2) {
            fn_802E317C("Lod table %s: bad value %s", name, use->GetValue(key));
        }
        int y = strchr(mode, 'y') != 0;
        LodEntry* e = &m->lods[m->numLods];
        e->dist = dist;
        e->noMirror = y;
        m->numLods++;
    }
    if (m->mirrorLod >= m->numLods) {
        m->mirrorLod = m->numLods - 1;
    }
    if (t->HasChild("shadow")) {
        ValueNode* sh = t->FindChild("shadow");
        const char* sm = sh->GetValue("model");
        int i;
        for (i = 0; i < 16; i++) {
            char key[16];
            fn_801345D8(key, 16, "lod%d", i);
            char p[64];
            fn_801345D8(p, 64, "%s.car/%s%d.mod", name, sm, i + 1);
            if (!sh->Has(key) || !fn_8012C33C(p)) {
                break;
            }
            m->shadows[i].model = fn_802C6FC0(p, 0, dir);
            m->shadows[i].dist = sh->GetFloat(key);
        }
        m->numShadows = i;
        if (i > 0) {
            ModelInfo* mi = fn_802C6C34(m->shadows[0].model);
            m->shadowRadius = mi->radius + PSVECMag(&mi->center);
            fn_80297988(m);
        } else {
            m->shadowRadius = 0.0f;
        }
    } else {
        m->numShadows = 0;
    }
    fn_80131818(t);
    char cm[32];
    fn_801345D8(cm, 32, "%sc.mod", mdl);
    if (!fn_8012C33C(cm)) {
        fn_801345D8(cm, 32, "%s0.mod", mdl);
    }
    for (int i = 0; i < m->numLods; i++) {
        char p[64];
        fn_801345D8(p, 64, "%s%d.mod", mdl, i);
        m->lodModels[i] = fn_802C6FC0(p, inst, dir);
        m->drawModels[i] = m->lodModels[i];
        m->lods[i].rider = fn_801E320C(m->lodModels[i], "neck") || fn_801E320C(m->lodModels[i], "butt");
        m->lods[i].wheels = fn_801E320C(m->lodModels[i], "wheel");
    }
    s32 m1 = m->lodModels[1];
    m->chassis = fn_802C6FC0(cm, inst, dir);
    m->wheelBone = fn_801E3004(m->chassis, "wheel", -1);
    m->shiftLight = fn_801E3004(m->chassis, "shift_light", -1);
    m->rider = fn_802002D4(name, m1, inst);
    const char* xm = use->GetString("xray_model", "xray.mod");
    void* o = new ("xray_model") u32;
    if (o) {
        o = fn_801EF1F0(o, xm);
    }
    m->tire = o;
    char bp[256];
    fn_801345D8(bp, 256, "%s.car/bone.mvl", name);
    if (!fn_8012C33C(bp)) {
        fn_801345D8(bp, 256, "%s.car/bone.val", name);
    }
    m->numBoneSurfs = 0;
    BoneSurf* bs;
    ValueTable* bt = fn_80131808(bp);
    bs = m->boneSurfs;
    u32 all = 0;
    for (int i = 0;; i++) {
        char key[256];
        fn_801345D8(key, 256, "bonesurf_%d", i + 1);
        if (!bt->HasChild(key)) {
            break;
        }
        ValueNode* b = bt->FindChild(key);
        m->numBoneSurfs = i + 1;
        bs->first = b->GetIntValue("first_bone");
        bs->last = b->GetIntValue("last_bone");
        bs->mask = fn_802C7830(b->GetValue("group_mask"));
        all |= bs->mask;
        bs++;
    }
    m->boneSurfs[0].mask |= ~all & 1;
    if (m->numBoneSurfs == 0) {
        m->numBoneSurfs = 1;
        m->boneSurfs[0].mask = 0xFFFFFFFF;
        m->boneSurfs[0].first = 0;
        m->boneSurfs[0].last = m->res->numBones;
    }
    fn_80131818(bt);
}

void fn_801EE974(CarModel* m, Racer* r) {
    for (int i = 0; i < 4; i++) {
        int front = i == 0 || i == 1;
        const char* cls;
        if (front) {
            cls = m->res->values->FindChild("Front")->GetValue("wheel_class");
        } else {
            cls = m->res->values->FindChild("Rear")->GetValue("wheel_class");
        }
        m->wheels[i] = fn_801EFF94(cls, m->index, i);
        r->wheelSize[i] = fn_801F159C(m->wheels[i]);
    }
}

void fn_801EEA64(CarModel* m, Racer* r, const char* name, const char* dir) {
    char p[256];
    fn_801345D8(p, 256, "%s.car/efxmesh.val", name);
    ValueTable* t = fn_80131810(p);
    if (t && t->HasChild("boneset")) {
        ValueNode* b = t->FindChild("boneset");
        char mp[256];
        fn_801345D8(mp, 256, "%s.car/%s", name, b->GetValue("model_name"));
        m->xray = fn_802C6FC0(mp, 0, dir);
        m->facingPos = b->GetIntValue("facing_pos");
        m->facingNeg = b->GetIntValue("facing_neg");
        m->xrayBones = b->GetIntValue("num_bones");
        if (b->Has("group_mask")) {
            m->suspGroup = fn_802C7830(b->GetValue("group_mask"));
        } else {
            m->suspGroup = 0xFFFFFFFF;
        }
        m->xrayMask = 1 << b->GetIntValue("suspension_group");
        fn_801345D8(r->xrayMesh, 32, "%s.car/%s", name, b->GetValue("mesh_name"));
        fn_80131818(t);
    } else {
        m->xray = -1;
        m->xrayBones = 0;
        r->xrayMesh[0] = 0;
    }
}

void fn_801EEC68(CarModel* m) {
    if (m->xray != -1) {
        fn_802C6CF8(m->xray);
        m->xray = -1;
    }
}

void fn_801EECAC(CarModel* m, u8* data) {
    m->car = (CarState*)(data + m->offset);
    fn_801EAB10(m->index, (s32)m->car);
    ValueNode* ch = m->res->values->FindChild("Chassis");
    fn_801EC214(&m->volts, ch, m->chassis, "volt_gauge", 1.0f, 0.0f, &lbl_8055B088);
    fn_801EC214(&m->speedometer, ch, m->chassis, "speedometer", 0.44444445f, 0.0f, &m->car->speed);
    fn_801EC59C(&m->speedoSlider, ch, "speedo_slider", &m->car->speed, 0.44444445f, 0.0f);
    fn_801EC214(&m->tachometer, ch, m->chassis, "tachometer", 1.0f, 0.0f, &m->car->rpm);
    fn_801EC214(&m->oilPressure, ch, m->chassis, "oil_pressure_gauge", 1.0f, 0.0f, &m->car->oilPressure);
    fn_801EC214(&m->oilTemp, ch, m->chassis, "oil_temp_gauge", 254.77777f, 254.22223f, &m->car->oilTemp);
    fn_801EC214(&m->waterTemp, ch, m->chassis, "water_temp_gauge", 254.77777f, 254.22223f, &m->car->waterTemp);
    fn_801EC214(&m->fuel, ch, m->chassis, "fuel_gauge", 1.0f, 0.0f, &m->car->fuel);
}

void fn_801EEE38(void) {
    u8* data = (u8*)fn_80179CB0();
    for (u32 i = 0; CarAt(i); i++) {
        fn_801EECAC(CarAt(i), data);
    }
}

}  // extern "C"

PlayerCarModel::PlayerCarModel(Racer* r, CarDef* d) : CarModel(r, d) {}

PlayerCarModel::~PlayerCarModel(void) {}

void PlayerCarModel::Set(const AnimSlot* s) {
    CarModel* src = lbl_80488648[((const CarState*)s)->carIndex];
    models = src->lodModels;
    lods = src->lodTable;
    lodCount = &src->numLods;
    CarModel::Set(s);
}

int PlayerCarModel::vf28(void) { return 1; }
