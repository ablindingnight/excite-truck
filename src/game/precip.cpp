// Precipitation (0x802A4A50-0x802A7BD8, users of the "weather" table): rain and snow particle
// volumes that follow the camera (points kept inside the view frustum, refilled as they fall out,
// pushed by wind and camera motion), rain splashes spawned from "droplet" sources on track objects,
// "mist" point clouds read from the track effect tables, a full-screen flash/tint and a lightning
// model. Names are guesses apart from the strings.
#include "game/common.h"

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

extern "C" {
int sprintf(char* s, const char* fmt, ...);
int strncmp(const char* a, const char* b, u32 n);
double sqrt(double x);
double cos(double x);
double sin(double x);
double tan(double x);
}

struct PVec {
    void Set(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
    f32 x, y, z;
};

// A point in a particle array (constructed by new[]).
struct WPt {
    WPt(void);
    void Set(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
    f32 x, y, z;
};

// Axes (rows) and the origin.
struct WFrame {
    ValVec3 x;    // offset 0x0
    ValVec3 y;    // offset 0xC
    ValVec3 z;    // offset 0x18
    ValVec3 pos;  // offset 0x24
};

// A track object carrying droplet sources.
struct DropObj {
    WFrame frame;  // offset 0x0
    u8 pad30[4];
    ValVec3 vel;   // offset 0x34
};

struct DropHit {
    DropObj* obj;  // offset 0x0
    s32 source;    // offset 0x4
    f32 dist;      // offset 0x8
    u8 enabled;    // offset 0xC
};

// One rectangular droplet emitter of a source (0x30).
struct Droplet {
    ValVec3 n;       // offset 0x0
    ValVec3 origin;  // offset 0xC
    ValVec3 sv;      // offset 0x18
    ValVec3 tv;      // offset 0x24
};

struct DropSource {
    DropSource(void);
    Droplet drops[4];  // offset 0x0
    s32 count;         // offset 0xC0
};

struct WeatherCfg {
    s32 type;     // offset 0x0
    u8 pad4[4];
    u32 color;    // offset 0x8
    f32 fog[3];   // offset 0xC
    u8 fogOn;     // offset 0x18
};

// Per-view screen effects (0xE0); the lightning angle is the first word of the next entry.
struct ScreenFx {
    u8 pad0[0xD8];
    f32 lightning;  // offset 0xD8
    f32 flash;      // offset 0xDC
};

struct ParticleDesc {
    ParticleDesc(void);
    s32 a28;     // offset 0x0
    s32 a2C;     // offset 0x4
    f32 a30;     // offset 0x8
    f32 a34;     // offset 0xC
    f32 a38[3];  // offset 0x10
    f32 a44[3];  // offset 0x1C
    u8 pad50[8];
    f32 a58;     // offset 0x30
    u8 pad5C[0x10];
    s32 a6C;     // offset 0x44
    u8 pad70[8];
    f32 a78;     // offset 0x50
    f32 a7C;     // offset 0x54
    f32 a80;     // offset 0x58
    f32 a84;     // offset 0x5C
    u8 pad88[4];
    f32 a8C;     // offset 0x64
    u8 pad90[4];
    f32 a94;     // offset 0x6C
    u8 pad98[4];
    f32 a9C;     // offset 0x74
    u8 padA0[0x1C];
};

class SnowSystem {
public:
    SnowSystem(int n, int view);
    ~SnowSystem(void);
    void Reset(void);
    void Update(void);
    void Draw(void);
    void Wobble(void);
    void Frustum(void);
    void Drift(void);
    void Move(f32 turn, const WFrame* cam, const PVec* d);
    void Add(const PVec* p);
    void Fill(void);
    void Spawn(void);

    WPt* pts;        // offset 0x0
    s32 count;       // offset 0x4
    s32 max;         // offset 0x8
    s32 flakeTex;    // offset 0xC
    s32 quadTex;     // offset 0x10
    f32 driftX;      // offset 0x14
    f32 driftY;      // offset 0x18
    s32 view;        // offset 0x1C
    f32 tanHalf;     // offset 0x20
    f32 near;        // offset 0x24
    f32 far;         // offset 0x28
    ValVec3 lastPos;  // offset 0x2C
    ValVec3 lastDir;  // offset 0x38
    ValVec3 vel;      // offset 0x44
    ValVec3 grav;     // offset 0x50
    PVec wob[8];      // offset 0x5C
};

class RainSystem {
public:
    RainSystem(int n, int view);
    ~RainSystem(void);
    void Reset(void);
    void Update(void);
    void Draw(void);
    void Frustum(void);
    void Move(const PVec* d);
    void Add(const PVec* p);
    void Fill(void);
    void Spawn(void);

    WPt* pts;         // offset 0x0
    s32 count;        // offset 0x4
    s32 max;          // offset 0x8
    s32 tex;          // offset 0xC
    s32 view;         // offset 0x10
    f32 tanHalf;      // offset 0x14
    f32 near;         // offset 0x18
    f32 far;          // offset 0x1C
    ValVec3 lastPos;  // offset 0x20
    ValVec3 vel;      // offset 0x2C
    ValVec3 dir;      // offset 0x38
};

class Mist {
public:
    Mist(ValueNode* vn);
    ~Mist(void);
    void Reset(void);
    void Update(void);
    void Draw(void);

    WPt* pts;         // offset 0x0
    s32 count;        // offset 0x4
    f32 size;         // offset 0x8
    ValVec3 center;   // offset 0xC
    f32 rad;          // offset 0x18
    f32 hit;          // offset 0x1C
    ValVec3 v;        // offset 0x20
    s32 tex;          // offset 0x2C
    f32 pointScale;   // offset 0x30
    u32 color;        // offset 0x34
    f32 density;      // offset 0x38
    f32 alpha;        // offset 0x3C
};

int IsLoading(void);
int GetNumPlayers(void);

extern "C" {
void PSVECCrossProduct(const void* a, const void* b, void* out);
f32 PSVECDotProduct(const void* a, const void* b);
f32 PSVECMag(const void* v);
f32 PSVECDistance(const void* a, const void* b);
f32 PSVECSquareDistance(const void* a, const void* b);
s32 fn_802E2C20(void);
f32 fn_801E7E24(void);
WeatherCfg* fn_801E1F44(void);
void fn_80287B38(void (*cb)(DropHit* hits, int n));
s32 fn_802CAC28(const char* name, int a);
void fn_802CAC54(s32 tex);
ValueNode* fn_802AE414(const char* name);
s32 fn_802C6FC0(const char* model, int a, int b);
void fn_802C6CF8(s32 model);
void fn_8028E398(s32 sys);
int fn_802C2C00(int mode);
void fn_802C990C(u32 color);
void fn_802C2B78(f32 a, f32 b, f32 c);
void fn_802D3D50(void);
ScreenFx* fn_801E7E34(void);
void fn_802C90F0(s32 tex);
void fn_802C9124(int x0, int y0, int x1, int y1, f32 u0, f32 v0, f32 u1, f32 v1, u32 color);
void fn_802C9118(void);
int fn_801E85B4(void);
const WFrame* fn_802C86DC(void);
void fn_802C9900(f32 a);
void fn_802C744C(s32 model, const WFrame* f);
void fn_802C9904(void);
void fn_802E317C(const char* fmt, ...);
void fn_8028E61C(ParticleDesc* d);
s32 fn_8028E2F8(ParticleDesc* d, const char* name, const char* tex);
ValueNode* fn_80288208(int i);
int fn_80287B14(void);
int fn_80287E20(ValueNode* vn, const char* key);
ValueNode* fn_80287E80(ValueNode* vn, const char* key);
int fn_80287B5C(int n);
f32 fn_80287BC8(f32 lo, f32 hi);
void fn_8028E584(s32 sys, const PVec* pos, const ValVec3* vel);
ValueTable* fn_802AE4C8(int i);
const WFrame* fn_801E8718(int view);
int fn_801E8758(int view);
f32 fn_802C9A4C(f32 size);
f32 fn_802C9A50(void);
void fn_802C8B98(s32 tex, const WFrame* m);
int fn_801E501C(int n, const ValVec3* vel, f32 a, u8 alpha);
void fn_801E5174(f32 x, f32 y, f32 z);
void fn_801E5348(void);
void fn_802C8D74(void);
f32 fn_80129DC4(int seed, f32 lo, f32 hi);
f32 fn_801E8738(int view);
f32 fn_801E8874(int view);
f32 fn_801E8894(int view);
void fn_801E88B4(s32* x, s32* y, s32* w, s32* h, int view);
void fn_801E8C84(PVec* wind);
double fn_8002F208(double x);
int fn_801EA140(ValVec3* p);
void fn_802C8BD0(s32 tex);
int fn_802CE860(int n);
void fn_802CE86C(u32 color, int i, f32 size, f32 x, f32 y, f32 z);
void fn_802CE868(void);
}

extern f32 lbl_80558DC4[];
extern s32 lbl_8055CFF0;
extern s32 lbl_8055CFF4;

const char* lbl_8055CBB8 = "snowquad.tex";
const char* lbl_8055CBBC = "snow.tex";
const char* lbl_8055CBC0 = "rain.tex";

ModuleTag lbl_8055F878;
f32 lbl_8055F880;
f32 lbl_8055F884;
SnowSystem* lbl_8055F888;
RainSystem* lbl_8055F88C;
s32 lbl_8055F890;
s32 lbl_8055F894;
s32 lbl_8055F898;
s32 lbl_8055F89C;
f32 lbl_8055F8A0;
s32 lbl_8055F8A4;
Mist* lbl_804BCFA8[4];

struct WeatherScales {
    WeatherScales(void) {
        lbl_8055F880 = fn_8002F208(10.0);
        lbl_8055F884 = fn_8002F208(2.0);
    }
};

static WeatherScales sScales;

DropSource lbl_804BCFB8[8];



static inline f32 FMin(f32 a, f32 b) { return b < a ? b : a; }
static inline f32 FMax(f32 a, f32 b) { return a > b ? a : b; }

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

// The vector in the frame's axes (multiply by the transpose).
static inline void MulT(const WFrame* m, const ValVec3* v, ValVec3* out) {
    if (out == v) {
        f32 x = Dot3(v->x, m->x.x, v->y, m->x.y, v->z, m->x.z);
        f32 y = Dot3(v->x, m->y.x, v->y, m->y.y, v->z, m->y.z);
        f32 z = Dot3(v->x, m->z.x, v->y, m->z.y, v->z, m->z.z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(v->x, m->x.x, v->y, m->x.y, v->z, m->x.z);
        out->y = Dot3(v->x, m->y.x, v->y, m->y.y, v->z, m->y.z);
        out->z = Dot3(v->x, m->z.x, v->y, m->z.y, v->z, m->z.z);
    }
}

// The vector out of the frame's axes.
static inline void Mul(const WFrame* m, const ValVec3* v, ValVec3* out) {
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

static inline int Alpha(f32 a) {
    int i = (int)a;
    i = i > 0 ? i : 0;
    return i < 0xFF ? i : 0xFF;
}

extern "C" {

f32 fn_802A4A50(void) {
    if (IsLoading()) {
        s32 now = fn_802E2C20();
        s32 d = now - lbl_8055F8A4;
        lbl_8055F8A4 = now;
        return MIN((f32)d / 1000.0f, 0.05f);
    }
    return fn_801E7E24();
}

f32 fn_802A4AD4(void) { return lbl_8055F8A0; }

void fn_802A5384(WeatherCfg* c);
void fn_802A53B0(DropHit* hits, int n);
void fn_802A5878(void);

void fn_802A4ADC(void) {
    WeatherCfg* c = fn_801E1F44();
    if (GetNumPlayers() == 1) {
        fn_802A5384(c);
    }
    fn_80287B38(fn_802A53B0);
    fn_802A5878();
    lbl_8055F89C = fn_802CAC28("white_st.tex", 0);
    const char* model = "litening.mod";
    ValueNode* w = fn_802AE414("weather");
    if (w) {
        model = w->GetString("lightning_model", model);
    }
    lbl_8055F898 = fn_802C6FC0(model, 0, 0);
}

void fn_802A597C(void);

void fn_802A4B90(void) {
    if (lbl_8055F898 != -1) {
        fn_802C6CF8(lbl_8055F898);
        lbl_8055F898 = -1;
    }
    fn_802CAC54(lbl_8055F89C);
    fn_802A597C();
    if (lbl_8055F894) {
        fn_8028E398(lbl_8055F894);
        lbl_8055F894 = 0;
    }
    if (lbl_8055F888) {
        delete lbl_8055F888;
        lbl_8055F888 = 0;
    }
    if (lbl_8055F88C) {
        delete lbl_8055F88C;
        lbl_8055F88C = 0;
    }
}

void fn_802A59E0(void);
void fn_802A5A38(void);
void fn_802A5A90(void);

void fn_802A4C20(void) {
    if (lbl_8055F88C) {
        lbl_8055F88C->Reset();
    }
    if (lbl_8055F888) {
        lbl_8055F888->Reset();
    }
    fn_802A59E0();
}

void fn_802A4C60(void) {
    lbl_8055F8A0 = fn_802A4A50();
    if (lbl_8055F88C) {
        lbl_8055F88C->Update();
    }
    if (lbl_8055F888) {
        lbl_8055F888->Update();
    }
    fn_802A5A38();
}

void fn_802A4CA8(void) {
    WeatherCfg* c = fn_801E1F44();
    if (c->fogOn) {
        fn_802C2C00(1);
        fn_802C990C(c->color);
        fn_802C2B78(c->fog[0], c->fog[1], c->fog[2]);
    }
}

void fn_802A4D00(void) { fn_802C2C00(0); }

void fn_802A4D08(void) {
    WeatherCfg* c = fn_801E1F44();
    fn_802A5A90();
    if (c->fogOn) {
        fn_802D3D50();
    }
    if (lbl_8055F88C) {
        lbl_8055F88C->Draw();
    } else if (lbl_8055F888) {
        lbl_8055F888->Draw();
    }
    f32 f = fn_801E7E34()->flash;
    if (!((f32)__fabs(f) <= lbl_80558DC4[0])) {
        int a = Alpha(0.5f * (255.0f * f));
        fn_802C90F0(lbl_8055F89C);
        fn_802C9124(0, 0, lbl_8055CFF0, lbl_8055CFF4, 0.0f, 0.0f, 1.0f, 1.0f, a << 24 | 0xFFFFFF);
        fn_802C9118();
    }
}

void fn_802A4E08(void) {
    ScreenFx* s = &fn_801E7E34()[fn_801E85B4()];
    if (!((f32)__fabs(s->flash) <= lbl_80558DC4[0])) {
        int a = Alpha(255.0f * s->flash);
        fn_802C90F0(lbl_8055F89C);
        fn_802C9124(0, 0, lbl_8055CFF0, lbl_8055CFF4, 0.0f, 0.0f, 1.0f, 1.0f, a << 24 | 0xE6F0FF);
        fn_802C9118();
    }
    if (!((f32)__fabs(s->lightning) <= lbl_80558DC4[0]) && lbl_8055F898 != -1) {
        WFrame m;
        m.pos = fn_802C86DC()->pos;
        f32 ang = *(f32*)(s + 1);
        f32 c = cos(ang);
        f32 sn = sin(ang);
        m.x.x = c;
        m.x.y = 0.0f;
        m.x.z = -sn;
        m.y.x = 0.0f;
        m.y.y = 1.0f;
        m.y.z = 0.0f;
        m.z.x = sn;
        m.z.y = 0.0f;
        m.z.z = c;
        fn_802C9900(FMin(2.0f * s->lightning, 1.0f));
        fn_802C744C(lbl_8055F898, &m);
        fn_802C9904();
    }
}

int fn_802A4FA8(Droplet* d, ValueNode* vn) {
    PVec c;
    if (vn) {
        d->n = vn->GetVec3("n");
        d->origin = vn->GetVec3("origin");
        d->sv = vn->GetVec3("sv");
        d->tv = vn->GetVec3("tv");
        PSVECCrossProduct(&d->sv, &d->tv, &c);
        if (2.5f * PSVECMag(&c) < 2.0f) {
            fn_802E317C("weather: droplet source is too small");
        }
        return 1;
    }
    return 0;
}

void fn_802A50F8(void) {
    ParticleDesc pd;
    char key[32];
    pd.a2C = 0x100;
    pd.a28 = 0;
    pd.a30 = 0.35f;
    pd.a34 = 1.0f;
    pd.a38[0] = 1.65f;
    pd.a38[1] = 1.65f;
    pd.a38[2] = 1.65f;
    pd.a44[0] = 1.0f;
    pd.a44[1] = 1.0f;
    pd.a44[2] = 1.0f;
    pd.a58 = 1.0f;
    pd.a6C = 5;
    pd.a7C = 0.035f;
    pd.a78 = 0.035f;
    pd.a84 = 1.0f;
    pd.a80 = -1.0f;
    pd.a9C = 1.0f;
    pd.a94 = 1.0f;
    pd.a8C = 1.0f;
    lbl_8055F894 = fn_8028E2F8(&pd, "droplets", "rain.tex");
    DropSource* s = lbl_804BCFB8;
    for (int i = 0; i < fn_80287B14(); s++, i++) {
        ValueNode* t = fn_80288208(i);
        s->count = 0;
        if (fn_80287E20(t, "droplet")) {
            ValueNode* d = fn_80287E80(t, "droplet");
            for (s->count = 0; s->count < 4; s->count++) {
                sprintf(key, "source%d", s->count + 1);
                if (!fn_802A4FA8(&s->drops[s->count], d->FindChild(key))) {
                    break;
                }
            }
        }
    }
}

void fn_802A5270(f32 density) {
    f32 d = FMax(FMin(density, 1.0f), 0.0f);
    int n = 512.0f * d;
    lbl_8055F88C = new ("rain system") RainSystem(n, 0);
    fn_802A50F8();
}

void fn_802A52FC(f32 density) {
    f32 d = FMax(FMin(density, 1.0f), 0.0f);
    int n = 512.0f * d;
    lbl_8055F888 = new ("snow system") SnowSystem(n, 0);
}

void fn_802A5384(WeatherCfg* c) {
    switch (c->type) {
    case 1:
        fn_802A5270(1.0f);
        break;
    case 2:
        fn_802A52FC(1.0f);
        break;
    }
}

void fn_802A53B0(DropHit* hits, int n) {
    if ((f32)__fabs(fn_802A4AD4()) <= lbl_80558DC4[0]) {
        return;
    }
    if (!lbl_8055F894) {
        return;
    }
    RainSystem* r = lbl_8055F88C;
    f32 inv = -(1.0f / (f32)sqrt(PSVECDotProduct(&r->vel, &r->vel)));
    ValVec3 dir;
    dir.x = r->vel.x * inv;
    dir.y = r->vel.y * inv;
    dir.z = r->vel.z * inv;
    for (int i = 0; i < n; i++, hits++) {
        DropSource* s = &lbl_804BCFB8[hits->source];
        if (hits->dist > 56.25f || !hits->enabled || s->count < 1) {
            continue;
        }
        DropObj* o = hits->obj;
        Droplet* d = s->drops;
        for (int j = 0; j < s->count; j++, d++) {
            ValVec3 nrm;
            Mul(&o->frame, &d->n, &nrm);
            f32 dot = PSVECDotProduct(&dir, &nrm);
            PVec c;
            PSVECCrossProduct(&d->sv, &d->tv, &c);
            int cnt = -dot * (2.5f * PSVECMag(&c));
            if (cnt < 1) {
                continue;
            }
            f32 k = -2.0f * dot;
            ValVec3 vel;
            vel.x = dir.x + nrm.x * k;
            vel.y = dir.y + nrm.y * k;
            vel.z = dir.z + nrm.z * k;
            vel.x = o->vel.x + vel.x * 3.0f;
            vel.y = o->vel.y + vel.y * 3.0f;
            vel.z = o->vel.z + vel.z * 3.0f;
            int m = fn_80287B5C(cnt) + 1;
            for (int k2 = 0; k2 < m; k2++) {
                f32 a = fn_80287BC8(0.0f, 1.0f);
                f32 b = fn_80287BC8(0.0f, 1.0f);
                ValVec3 p;
                p.x = d->origin.x + d->sv.x * a + d->tv.x * b;
                p.y = d->origin.y + d->sv.y * a + d->tv.y * b;
                p.z = d->origin.z + d->sv.z * a + d->tv.z * b;
                Mul(&o->frame, &p, &p);
                p.x = p.x + o->frame.pos.x;
                p.y = p.y + o->frame.pos.y;
                p.z = p.z + o->frame.pos.z;
                fn_8028E584(lbl_8055F894, (const PVec*)&p, &vel);
            }
        }
    }
}

void fn_802A5878(void) {
    lbl_8055F890 = 0;
    if (GetNumPlayers() > 1) {
        return;
    }
    ValueTable* t;
    for (int i = 0; (t = fn_802AE4C8(i)) != 0; i++) {
        for (int j = 0; t->GetChild(j); j++) {
            ValueNode* c = t->GetChild(j);
            if (strncmp(c->GetName(), "mist", 4) == 0) {
                lbl_804BCFA8[lbl_8055F890++] = new ("mist") Mist(c);
            }
        }
    }
}

void fn_802A597C(void) {
    for (int i = 0; i < lbl_8055F890; i++) {
        delete lbl_804BCFA8[i];
    }
    lbl_8055F890 = 0;
}

void fn_802A59E0(void) {
    for (int i = 0; i < lbl_8055F890; i++) {
        lbl_804BCFA8[i]->Reset();
    }
}

void fn_802A5A38(void) {
    for (int i = 0; i < lbl_8055F890; i++) {
        lbl_804BCFA8[i]->Update();
    }
}

void fn_802A5A90(void) {
    for (int i = 0; i < lbl_8055F890; i++) {
        lbl_804BCFA8[i]->Draw();
    }
}
}

SnowSystem::SnowSystem(int n, int v) {
    pts = new ("snowflakes") WPt[n];
    max = n;
    flakeTex = fn_802CAC28(lbl_8055CBBC, 0);
    quadTex = fn_802CAC28(lbl_8055CBB8, 0);
    view = v;
    Reset();
}

SnowSystem::~SnowSystem(void) {
    fn_802CAC54(quadTex);
    quadTex = -1;
    fn_802CAC54(flakeTex);
    flakeTex = -1;
    delete[] pts;
    pts = 0;
}

void SnowSystem::Reset(void) {
    tanHalf = 1.0f;
    near = 0.5f;
    far = 5.0f;
    lastPos.x = 0.0f;
    lastPos.y = 0.0f;
    lastPos.z = 0.0f;
    lastDir.x = 0.0f;
    lastDir.y = 0.0f;
    lastDir.z = 0.0f;
    vel.x = 0.0f;
    vel.y = 0.0f;
    vel.z = 0.0f;
    grav.x = 0.0f;
    grav.y = 1.0f;
    grav.z = 0.0f;
    driftY = 0.0f;
    driftX = 0.0f;
    Wobble();
    Fill();
}

void SnowSystem::Update(void) {
    WFrame cam = *fn_801E8718(view);
    Frustum();
    ValVec3 g;
    g.x = 0.0f;
    g.y = -1.0f;
    g.z = 0.0f;
    MulT(&cam, &g, &grav);
    vel.x = grav.x * 0.05f;
    vel.y = grav.y * 0.05f;
    vel.z = grav.z * 0.05f;
    if (!fn_801E8758(view)) {
        ValVec3 d;
        d.x = lastPos.x - cam.pos.x;
        d.y = lastPos.y - cam.pos.y;
        d.z = lastPos.z - cam.pos.z;
        MulT(&cam, &d, &d);
        vel.x = vel.x + d.x * 0.2f;
        vel.y = vel.y + d.y * 0.2f;
        vel.z = vel.z + d.z * 0.2f;
        PVec c;
        PSVECCrossProduct(&cam.z, &lastDir, &c);
        Move(PSVECDotProduct(&cam.y, &c), &cam, (const PVec*)&d);
        Spawn();
        Drift();
    } else {
        Fill();
    }
    lastPos = cam.pos;
    lastDir = cam.z;
}

void SnowSystem::Draw(void) {
    if (count < 1) {
        return;
    }
    f32 old = fn_802C9A4C(lbl_8055F880);
    fn_802C8B98(flakeTex, fn_802C86DC());
    f32 a = 1.0f / (4.0f * PSVECMag(&vel));
    int i = 255.0f * a;
    i = i < 0 ? 0 : i;
    if (fn_801E501C(count, &vel, 0.05f, i < 0xFF ? i : 0xFF)) {
        for (int j = 0; j < count; j++) {
            fn_801E5174(pts[j].x, pts[j].y, pts[j].z);
        }
        fn_801E5348();
    }
    fn_802C8D74();
    fn_802C9A4C(old);
    fn_802C90F0(quadTex);
    for (int gx = 0; gx < lbl_8055CFF0 / 64 + 1; gx++) {
        for (int gy = 0; gy < lbl_8055CFF4 / 64 + 1; gy++) {
            int x1 = (gx + 1) * 64;
            int y1 = (gy + 1) * 64;
            f32 u = (f32)(gx * 7) / 3.0f;
            f32 v = (f32)(gy * 7) / 5.0f;
            fn_802C9124(gx * 64, gy * 64, lbl_8055CFF0 < x1 ? lbl_8055CFF0 : x1, lbl_8055CFF4 < y1 ? lbl_8055CFF4 : y1,
                        driftX + u, driftY + v, (1.0f + driftX) + u, (1.0f + driftY) + v, 0x6EFFFFFF);
        }
    }
    fn_802C9118();
}

void SnowSystem::Wobble(void) {
    for (int i = 0; i < 8; i++) {
        wob[i].Set(fn_80129DC4(0, -0.1f, 0.1f), fn_80129DC4(0, -0.1f, 0.1f), fn_80129DC4(0, -0.1f, 0.1f));
    }
}

void SnowSystem::Frustum(void) {
    f32 half = 0.5f * fn_801E8738(view);
    f32 n = fn_801E8874(view);
    f32 f = fn_801E8894(view);
    s32 x, y, w, h;
    fn_801E88B4(&x, &y, &w, &h, view);
    f32 t = tan(half);
    tanHalf = t;
    near = 0.3f + n;
    f32 s = f + n;
    f32 b = -0.1f * s - 2.0f * f * n;
    f32 c = n * (f * 0.005f) - 0.05f * ((f32)w * (4.0f * n)) * (f - n) / (2.0f * t);
    far = (-b + (f32)sqrt(b * b - 4.0f * s * c)) / (2.0f * s);
}

void SnowSystem::Drift(void) {
    if ((f32)__fabs(fn_802A4AD4()) <= lbl_80558DC4[0]) {
        return;
    }
    driftX = fn_80287BC8(-1.5f, 1.5f);
    driftY = fn_80287BC8(-1.5f, 1.5f);
}

void SnowSystem::Move(f32 turn, const WFrame* cam, const PVec* d) {
    f32 tx = tanHalf / fn_802C9A50();
    f32 ty = tanHalf;
    f32 dt = fn_802A4AD4();
    ValVec3 wind;
    fn_801E8C84((PVec*)&wind);
    wind.x = wind.x * (0.1f * dt);
    wind.y = wind.y * (0.1f * dt) - 1.2f * dt;
    wind.z = wind.z * (0.1f * dt);
    MulT(cam, &wind, &wind);
    wind.x = wind.x + d->x * 0.25f;
    wind.y = wind.y + d->y * 0.25f;
    wind.z = wind.z + d->z * 0.25f;
    WPt* out = pts;
    for (int i = 0; i < count; i++) {
        WPt* p = &pts[i];
        PVec* w = &wob[i % 8];
        f32 z = p->z + wind.z + w->z * dt;
        f32 x = (p->x + wind.x + w->x * dt) + turn * z;
        f32 y = p->y + wind.y + w->y * dt;
        if (z > near && z < far && x * x < (z * tx) * (z * tx) && y * y < (z * ty) * (z * ty)) {
            out->x = x;
            out->y = y;
            out->z = z;
            out++;
        }
    }
    count = out - pts;
}

void SnowSystem::Add(const PVec* p) {
    *(PVec*)&pts[count] = *p;
    count++;
}

void SnowSystem::Fill(void) {
    count = 0;
    while (count < max) {
        f32 z = fn_80287BC8(near, far);
        f32 r = tanHalf * z;
        f32 x = fn_80287BC8(-r, r);
        PVec p;
        p.Set(x, fn_80287BC8(-r, r), z);
        Add(&p);
    }
}

void SnowSystem::Spawn(void) {
    f32 ty = tanHalf;
    f32 tx = ty / fn_802C9A50();
    while (count < max) {
        f32 r = fn_80287BC8(0.0f, 1.0f);
        f32 k = 0.5f - 0.5f * r;
        f32 gx = grav.x * k;
        f32 gy = grav.y * k;
        f32 ax = 1.0f - (f32)__fabs(gx);
        f32 ay = 1.0f - (f32)__fabs(gy);
        f32 rx = fn_80287BC8(-ax, ax);
        f32 ry = fn_80287BC8(-ay, ay);
        f32 z = near + r * (far - near);
        PVec p;
        p.z = z;
        p.y = ty * z * (gy + ry);
        p.x = tx * z * (gx + rx);
        Add(&p);
    }
}

RainSystem::RainSystem(int n, int v) {
    pts = new ("raindrops") WPt[n];
    max = n;
    tex = fn_802CAC28(lbl_8055CBC0, 0);
    view = v;
    Reset();
}

RainSystem::~RainSystem(void) {
    fn_802CAC54(tex);
    delete[] pts;
}

void RainSystem::Reset(void) {
    tanHalf = 1.0f;
    near = 0.5f;
    far = 5.0f;
    lastPos.x = 0.0f;
    lastPos.y = 0.0f;
    lastPos.z = 0.0f;
    dir.x = 0.0f;
    dir.y = 1.0f;
    dir.z = 0.0f;
    Fill();
}

void RainSystem::Update(void) {
    const WFrame* cam = fn_801E8718(view);
    Frustum();
    ValVec3 g;
    g.x = 0.0f;
    g.y = 1.0f;
    g.z = 0.0f;
    MulT(cam, &g, &dir);
    if (!fn_801E8758(view)) {
        ValVec3 d;
        d.x = lastPos.x - cam->pos.x;
        d.y = lastPos.y - cam->pos.y;
        d.z = lastPos.z - cam->pos.z;
        MulT(cam, &d, &d);
        vel.x = dir.x + d.x * -1.5f;
        vel.y = dir.y + d.y * -1.5f;
        vel.z = dir.z + d.z * -1.5f;
        f32 inv = 1.0f / (f32)sqrt(PSVECDotProduct(&vel, &vel));
        vel.x = vel.x * (0.25f * inv);
        vel.y = vel.y * (0.25f * inv);
        vel.z = vel.z * (0.25f * inv);
        Move((const PVec*)&d);
        Spawn();
    } else {
        vel.x = dir.x * 0.25f;
        vel.y = dir.y * 0.25f;
        vel.z = dir.z * 0.25f;
        Fill();
    }
    lastPos = cam->pos;
}

void RainSystem::Draw(void) {
    if (count < 1) {
        return;
    }
    f32 old = fn_802C9A4C(lbl_8055F884);
    fn_802C8B98(tex, fn_802C86DC());
    if (fn_801E501C(count, &vel, 0.015f, 0xFF)) {
        for (int i = 0; i < count; i++) {
            fn_801E5174(pts[i].x, pts[i].y, pts[i].z);
        }
        fn_801E5348();
    }
    fn_802C8D74();
    fn_802C9A4C(old);
}

void RainSystem::Frustum(void) {
    f32 half = 0.5f * fn_801E8738(view);
    f32 n = fn_801E8874(view);
    f32 f = fn_801E8894(view);
    s32 x, y, w, h;
    fn_801E88B4(&x, &y, &w, &h, view);
    f32 t = tan(half);
    tanHalf = t;
    near = 1.0f + n;
    f32 s = f + n;
    f32 b = -0.5f * s - 2.0f * f * n;
    f32 c = n * (f * 0.125f) - 0.015f * ((f32)w * (4.0f * n)) * (f - n) / (2.0f * t);
    far = (-b + (f32)sqrt(b * b - 4.0f * s * c)) / (2.0f * s);
}

void RainSystem::Move(const PVec* d) {
    f32 ty = tanHalf;
    f32 tx = ty / fn_802C9A50();
    f32 k = -10.0f * fn_802A4AD4();
    f32 mx = d->x + vel.x * k;
    f32 my = d->y + vel.y * k;
    f32 mz = d->z + vel.z * k;
    WPt* out = pts;
    for (int i = 0; i < count; i++) {
        WPt* p = &pts[i];
        f32 x = p->x + mx;
        f32 y = p->y + my;
        f32 z = p->z + mz;
        if (z > near && z < far && x * x < (0.25f + z * tx) * (0.25f + z * tx) &&
            y * y < (0.25f + z * ty) * (0.25f + z * ty)) {
            out->x = x;
            out->y = y;
            out->z = z;
            out++;
        }
    }
    count = out - pts;
}

void RainSystem::Add(const PVec* p) {
    *(PVec*)&pts[count] = *p;
    count++;
}

void RainSystem::Fill(void) {
    count = 0;
    while (count < max) {
        f32 z = fn_80287BC8(near, far);
        f32 r = tanHalf * z;
        f32 x = fn_80287BC8(-r, r);
        PVec p;
        p.Set(x, fn_80287BC8(-r, r), z);
        Add(&p);
    }
}

void RainSystem::Spawn(void) {
    f32 ty = tanHalf;
    f32 tx = ty / fn_802C9A50();
    if (vel.z >= -0.01f) {
        while (count < max) {
            f32 r = fn_80287BC8(0.0f, 1.0f);
            f32 k = 0.5f - 0.5f * r;
            f32 gx = dir.x * k;
            f32 gy = dir.y * k;
            f32 ax = 1.0f - (f32)__fabs(gx);
            f32 ay = 1.0f - (f32)__fabs(gy);
            f32 rx = fn_80287BC8(-ax, ax);
            f32 ry = fn_80287BC8(-ay, ay);
            f32 z = near + r * (far - near);
            PVec p;
            p.z = z;
            p.y = ty * z * (gy + ry);
            p.x = tx * z * (gx + rx);
            Add(&p);
        }
    } else {
        while (count < max) {
            f32 r = fn_80287BC8(0.0f, 1.0f);
            f32 z = near + (r + -vel.z * (r * r - r)) * (far - near);
            f32 a = fn_80287BC8(-z, z);
            f32 b = fn_80287BC8(-z, z);
            PVec p;
            p.z = z;
            p.x = tx * a;
            p.y = ty * b;
            Add(&p);
        }
    }
}

static inline int ClampA(f32 v, int a) {
    int b = (int)v;
    b = b > 0 ? b : 0;
    return b < a ? b : a;
}

static inline u32 MistColor(int r, int g, int b) { return 0xFF000000 | (r & 0xFF) << 16 | (g & 0xFF) << 8 | (b & 0xFF); }

Mist::Mist(ValueNode* vn) {
    count = vn->GetIntValue("max");
    pts = new ("mist pts") WPt[count];
    size = vn->GetFloat("size");
    center = vn->GetVec3("center");
    rad = vn->GetFloat("rad");
    hit = vn->GetFloat("hit");
    v = vn->GetVec3("v");
    tex = fn_802CAC28(vn->GetValue("texture"), 0);
    pointScale = fn_8002F208(0.5f / size);
    ValVec3 c = vn->GetVec3("color");
    color = MistColor(Alpha(c.x), Alpha(c.y), Alpha(c.z));
    density = vn->GetFloat("density");
    if (!fn_801EA140(&center)) {
        fn_802E317C("weather: terrain lookup failed for '%s'", vn->GetName());
    }
}

Mist::~Mist(void) {
    fn_802CAC54(tex);
    delete[] pts;
}

void Mist::Reset(void) {
    for (int i = 0; i < count; i++) {
        pts[i].Set(center.x + fn_80287BC8(0.7f * -rad, 0.7f * rad), center.y + fn_80287BC8(0.0f, hit),
                   center.z + fn_80287BC8(0.7f * -rad, 0.7f * rad));
    }
    alpha = 0.0f;
}

void Mist::Update(void) {
    f32 d2 = PSVECSquareDistance(&fn_801E8718(0)->pos, &center);
    f32 a;
    if (d2 > rad * rad) {
        f32 dt = fn_802A4AD4();
        ValVec3 dv;
        dv.x = v.x * dt;
        dv.y = v.y * dt;
        dv.z = v.z * dt;
        f32 k = -rad * (1.0f / (f32)sqrt(v.x * v.x + v.z * v.z));
        ValPoint back;
        back.x = v.x * k;
        back.y = v.z * k;
        ValPoint side = back;
        f32 t = side.x;
        side.x = side.y;
        side.y = -t;
        for (int i = 0; i < count; i++) {
            WPt* p = &pts[i];
            if (PSVECSquareDistance(p, &center) < rad * rad) {
                p->x = p->x + dv.x;
                p->y = p->y + dv.y;
                p->z = p->z + dv.z;
            } else {
                f32 r = fn_80287BC8(-0.75f, 0.75f);
                f32 s = 1.0f - r * r;
                f32 z = center.z + side.y * r + back.y * s;
                f32 y = center.y + fn_80287BC8(0.0f, hit);
                p->x = center.x + side.x * r + back.x * s;
                p->y = y;
                p->z = z;
            }
        }
        a = 0.0f;
    } else {
        a = density * (1.0f - (f32)sqrt(d2) / rad);
    }
    if (((f32)__fabs(a) <= lbl_80558DC4[0]) != ((f32)__fabs(alpha) <= lbl_80558DC4[0])) {
        fn_802C990C(color);
    }
    fn_802C2B78(0.5f, 50.0f, a);
    alpha = a;
}

void Mist::Draw(void) {
    int a = Alpha(0.01f * (255.0f * (PSVECDistance(&fn_802C86DC()->pos, &center) - 0.5f * rad)));
    if (a > 0) {
        f32 old = fn_802C9A4C(pointScale);
        fn_802C8BD0(tex);
        if (fn_802CE860(count)) {
            f32 k = (f32)a / (rad * rad);
            for (int i = 0; i < count; i++) {
                WPt* p = &pts[i];
                int b = (int)((f32)a - k * PSVECSquareDistance(p, &center));
                b = b > 0 ? b : 0;
                fn_802CE86C((b < a ? b : a) << 24 | 0xFFFFFF, i, 0.5f * size, p->x, p->y, p->z);
            }
            fn_802CE868();
        }
        fn_802C8D74();
        fn_802C9A4C(old);
    }
    fn_802D3D50();
}

