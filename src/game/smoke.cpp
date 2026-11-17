// Car smoke (0x802888D8-0x8028A664): engine smoke by damage state, exhaust puffs from the
// exhaust parts, fire smoke and radiator steam, plus the shared small/large fire smoke
// emitters. Names are guesses apart from the allocation tags, emitter names and value keys.
#include "game/common.h"
#include "game/partparams.h"
#include "game/plainvec.h"
#include "game/vecmatrix.h"
#include "macros.h"
#include <revolution/MTX.h>

struct SmokeCar {
    Vec axes[3];  // offset 0x0
    Vec pos;      // offset 0x24
    u8 pad30[0x34 - 0x30];
    Vec vel;  // offset 0x34
    u8 pad40[0x5C - 0x40];
    s32 state;  // offset 0x5C
};

struct SmokeEntry {
    SmokeCar* car;  // offset 0x0
    s32 index;      // offset 0x4
    f32 dist;       // offset 0x8
    u8 active;      // offset 0xC
};

// SmokePart: vecmatrix.h's VecMatrix.
struct SmokeModel {
    u8 pad0[0x24];
    s32 nParts;  // offset 0x24
};

#include "game/scolor.h"

struct SVec {
    SVec(void) {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }
    SVec(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
    f32 x;
    f32 y;
    f32 z;
};

class ValueNodeV {
public:
    virtual const char* GetName(void);
    virtual int Has(const char* key);
    virtual int GetPair(int index, const char** key, const char** value);
    virtual const char* GetValue(const char* key);
    virtual const char* GetString(const char* key, const char* def);
    virtual s32 GetIntValue(const char* key);
    virtual s32 GetInt(const char* key, s32 def);
    virtual f32 GetFloat(const char* key);
    virtual f32 GetFloat(const char* key, f32 def);
    virtual s32 GetIntValue2(const char* key);
    virtual int GetBool(const char* key, int def);
    virtual ValVec3 GetVec3(const char* key);
    virtual ValVec3 GetVec3Or(const char* key, ValVec3 def);
    virtual void Unk3C(void);
    virtual void Unk40(void);
    virtual void Unk44(void);
    virtual void Unk48(void);
    virtual void Unk4C(void);
    virtual void Unk50(void);
    virtual void Unk54(void);
    virtual void Unk58(void);
    virtual void Unk5C(void);
    virtual ValueNodeV* GetChild(int i);
};

extern "C" {
f32 fn_801E7E24(void);
s32 fn_80287B14(void);
const SmokeCar* fn_80287B24(s32 i);
void fn_80287B38(void (*cb)(SmokeEntry* e, int n));
int fn_80287B5C(int n);
int fn_80287E20(SmokeModel* model, const char* name);
ValueNodeV* fn_80287E80(SmokeModel* model, const char* name);
ValVec3 fn_80287ED0(SmokeModel* model, const char* node, const char* key);
const VecMatrix* fn_80288060(SmokeModel* model, int i);
SmokeModel* fn_80288208(s32 i);
void* fn_8028E2F8(PartParams* desc, const char* name, const char* tex);
void fn_8028E398(void* e);
void fn_8028E584(void* e, const Vec* p, const Vec* v);
void* memset(void* dst, int c, size_t n);
}

void* operator new(size_t size, const char* name);
void* operator new[](size_t size, const char* name);

extern f32 __float_epsilon[];  // FLT_EPSILON

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

static inline void Scale(Vec* out, const Vec* in, f32 s) {
    out->x = in->x * s;
    out->y = in->y * s;
    out->z = in->z * s;
}

static inline void AddScaled(Vec* out, const Vec* in, const Vec* d, f32 s) {
    out->x = in->x + d->x * s;
    out->y = in->y + d->y * s;
    out->z = in->z + d->z * s;
}

class EngineSmokeMan {
public:
    EngineSmokeMan(void);
    ~EngineSmokeMan(void);
    void Update(int i, int state);
    void InitPos(void);
    void InitLight(void);
    void InitHeavy(void);
    void InitFire(void);

    Vec pos[8];      // offset 0x0
    void* light;     // offset 0x60
    void* heavy;     // offset 0x64
    void* fire;      // offset 0x68
};

struct ExhaustState {
    f32 t;        // offset 0x0
    u32 idx;      // offset 0x4
    s32 count;    // offset 0x8
    f32 burst;    // offset 0xC
    s32 part;     // offset 0x10
};

class ExhaustSmokeMan {
public:
    ExhaustSmokeMan(void);
    ~ExhaustSmokeMan(void);
    void Reset(void);
    void Update(int i);
    void Clear(void);
    void InitLight(void);
    void InitDark(void);
    void Emit(void* e, int car, int part);

    ExhaustState cars[8];  // offset 0x0
    void* light;           // offset 0xA0
    void* dark;            // offset 0xA4
};

class FireSmokeMan {
public:
    FireSmokeMan(void);
    ~FireSmokeMan(void);
    void InitSmall(void);
    void InitLarge(void);

    void* small;  // offset 0x0
    void* large;  // offset 0x4
};

struct RadiatorPoint {
    Vec pos;  // offset 0x0
    Vec dir;  // offset 0xC
};

struct RadiatorCar {
    RadiatorCar(void);
    RadiatorPoint pts[4];  // offset 0x0
    s32 count;             // offset 0x60
    RadiatorPoint* cur;    // offset 0x64
};

class RadiatorSmokeMan {
public:
    RadiatorSmokeMan(void);
    ~RadiatorSmokeMan(void);
    void Reset(void);
    void Update(int i, int state);
    void InitEmitter(void);
    void InitCars(void);
    void Emit(int i);

    RadiatorCar* cars;  // offset 0x0
    void* emitter;      // offset 0x4
};

ModuleTag lbl_8055F6D0;
EngineSmokeMan* lbl_8055F6D8;
ExhaustSmokeMan* lbl_8055F6DC;
FireSmokeMan* lbl_8055F6E0;
RadiatorSmokeMan* lbl_8055F6E4;
SVec lbl_804B3220;
SVec lbl_804B322C;

const char* lbl_8055C880 = "smoke.tex";

static const int lbl_802FA978[8] = {0, 7, 3, 2, 5, 4, 6, 1};

extern "C" {

void fn_80288A74(SmokeEntry* e, int n);

void fn_802888D8(void) {
    lbl_8055F6D8 = new ("engine smoke man") EngineSmokeMan;
    lbl_8055F6E0 = new ("misc smoke man") FireSmokeMan;
    lbl_8055F6DC = new ("exhaust smoke man") ExhaustSmokeMan;
    lbl_8055F6E4 = new ("radiator smoke man") RadiatorSmokeMan;
    fn_80287B38(fn_80288A74);
}

void fn_80288988(void) {
    delete lbl_8055F6E4;
    lbl_8055F6E4 = 0;
    delete lbl_8055F6E0;
    lbl_8055F6E0 = 0;
    delete lbl_8055F6DC;
    lbl_8055F6DC = 0;
    delete lbl_8055F6D8;
    lbl_8055F6D8 = 0;
}

void fn_802889F0(void) {
    lbl_8055F6DC->Reset();
    lbl_8055F6E4->Reset();
}

void fn_80288A1C(int mode) {}

void fn_80288A20(void) {
    if ((f32)__fabs(fn_801E7E24()) <= __float_epsilon[0]) {
        return;
    }
}

void fn_80288A40(const Vec* p, const Vec* v) { fn_8028E584(lbl_8055F6E0->large, p, v); }

void fn_80288A58(const Vec* p, const Vec* v) { fn_8028E584(lbl_8055F6E0->small, p, v); }

void fn_80288A70(const void*, const Vec*, f32) {}

void fn_80288A74(SmokeEntry* e, int n) {
    if ((f32)__fabs(fn_801E7E24()) <= __float_epsilon[0]) {
        return;
    }
    SmokeEntry* p = e;
    for (int i = 0; i < n; e++, p++, i++) {
        if (p->active && p->dist < 10000.0f) {
            int state = p->car->state;
            lbl_8055F6D8->Update(e->index, state);
            lbl_8055F6DC->Update(e->index);
            lbl_8055F6E4->Update(e->index, state);
        }
    }
}

}  // extern "C"

EngineSmokeMan::EngineSmokeMan(void) {
    InitPos();
    InitLight();
    InitHeavy();
    InitFire();
}

EngineSmokeMan::~EngineSmokeMan(void) {
    fn_8028E398(fire);
    fn_8028E398(heavy);
    fn_8028E398(light);
}

void EngineSmokeMan::Update(int i, int state) {
    if (state == 2) {
        const SmokeCar* c = fn_80287B24(i);
        Vec p;
        MulVec(&p, &pos[i], c->axes);
        p.x = p.x + c->pos.x;
        p.y = p.y + c->pos.y;
        p.z = p.z + c->pos.z;
        fn_8028E584(light, &p, &c->vel);
    } else if (state == 6 || state == 7) {
        const SmokeCar* c = fn_80287B24(i);
        Vec p;
        MulVec(&p, &pos[i], c->axes);
        p.x = p.x + c->pos.x;
        p.y = p.y + c->pos.y;
        p.z = p.z + c->pos.z;
        fn_8028E584(heavy, &p, &c->vel);
    } else if (state == 10 || state == 11) {
        const SmokeCar* c = fn_80287B24(i);
        Vec p;
        MulVec(&p, &pos[i], c->axes);
        p.x = p.x + c->pos.x;
        p.y = p.y + c->pos.y;
        p.z = p.z + c->pos.z;
        fn_8028E584(fire, &p, &c->vel);
    }
}

void EngineSmokeMan::InitPos(void) {
    Vec* p = pos;
    for (int i = 0; i < fn_80287B14(); p++, i++) {
        SmokeModel* model = fn_80288208(i);
        if (fn_80287E20(model, "engine")) {
            *(ValVec3*)p = fn_80287ED0(model, "engine", "arm");
        } else {
            *(SVec*)p = SVec(0.0f, 2.0f, 0.0f);
        }
    }
}

void EngineSmokeMan::InitLight(void) {
    PartParams e;
    e.maxParticles = 0x60;
    e.maxLife = 1.5f;
    e.spewRate = 0.2f;
    e.type = 2;
    e.shading = 0.5f;
    e.priority = 5;
    e.spread = ValV3(0.9f, 0.9f, 0.9f);
    e.velScale = ValV3(1.0f, 1.0f, 1.0f);
    e.upKick = 0.0f;
    e.angSpread = 3.0f;
    e.gravity = -0.15f;
    e.drag = 1.0f;
    e.blueRamp = 0.8f;
    e.greenRamp = 0.8f;
    e.redRamp = 0.8f;
    e.blueConst = -0.1f;
    e.greenConst = -0.1f;
    e.redConst = -0.1f;
    e.alphaRamp = -0.4f;
    e.alphaConst = 0.4f;
    e.sizeRamp = 1.9f;
    e.sizeConst = 0.8f;
    e.angRamp = -0.5f;
    e.angConst = 1.0f;
    light = fn_8028E2F8(&e, "light engine smoke", lbl_8055C880);
}

void EngineSmokeMan::InitHeavy(void) {
    PartParams e;
    e.maxParticles = 0xA0;
    e.maxLife = 1.5f;
    e.spewRate = 0.25f;
    e.type = 2;
    e.shading = 0.7f;
    e.priority = 5;
    e.spread = ValV3(0.65f, 0.65f, 0.65f);
    e.velScale = ValV3(1.2f, 1.2f, 1.2f);
    e.upKick = 0.75f;
    e.angSpread = 2.5f;
    e.gravity = -0.15f;
    e.drag = 0.1f;
    e.blueRamp = 0.1f;
    e.greenRamp = 0.1f;
    e.redRamp = 0.1f;
    e.blueConst = 0.2f;
    e.greenConst = 0.2f;
    e.redConst = 0.2f;
    e.alphaRamp = -0.9f;
    e.alphaConst = 0.9f;
    e.sizeRamp = 6.3f;
    e.sizeConst = 1.7f;
    e.angRamp = -0.75f;
    e.angConst = 1.0f;
    heavy = fn_8028E2F8(&e, "heavy engine smoke", lbl_8055C880);
}

void EngineSmokeMan::InitFire(void) {
    PartParams e;
    e.maxParticles = 0x100;
    e.maxLife = 1.7f;
    e.spewRate = 0.4f;
    e.type = 2;
    e.shading = 0.98f;
    e.priority = 5;
    e.spread = ValV3(0.95f, 0.75f, 0.95f);
    e.velScale = ValV3(1.2f, 1.2f, 1.2f);
    e.upKick = 0.75f;
    e.angSpread = 2.5f;
    e.gravity = -0.1f;
    e.drag = 0.2f;
    e.blueRamp = 0.1f;
    e.greenRamp = 0.1f;
    e.redRamp = 0.1f;
    e.blueConst = 0.1f;
    e.greenConst = 0.1f;
    e.redConst = 0.1f;
    e.alphaRamp = -0.95f;
    e.alphaConst = 0.95f;
    e.sizeRamp = 7.0f;
    e.sizeConst = 2.3f;
    e.angRamp = -0.75f;
    e.angConst = 1.0f;
    fire = fn_8028E2F8(&e, "engine fire smoke", lbl_8055C880);
}

ExhaustSmokeMan::ExhaustSmokeMan(void) {
    InitLight();
    InitDark();
    Reset();
}

ExhaustSmokeMan::~ExhaustSmokeMan(void) {
    fn_8028E398(dark);
    fn_8028E398(light);
}

void ExhaustSmokeMan::Reset(void) { Clear(); }

void ExhaustSmokeMan::Update(int i) {
    ExhaustState* s = &cars[i];
    if (s->count >= 1) {
        if (s->burst > 0.05f) {
            if (s->t >= 1.0f) {
                Emit(dark, i, s->part);
                s->t = s->t - 1.0f;
            }
            s->t += 0.25f * s->burst;
            s->burst *= 0.75f;
        } else {
            if (s->t >= 1.0f) {
                if (++s->idx >= 8) {
                    s->idx = 0;
                }
                Emit(light, i, lbl_802FA978[s->idx] % s->count);
                s->t = s->t - 1.0f;
            }
            s->t += 0.5f;
        }
    }
}

void ExhaustSmokeMan::Clear(void) {
    memset(cars, 0, sizeof(cars));
    ExhaustState* s = cars;
    for (int i = 0; i < fn_80287B14(); i++, s++) {
        s->count = fn_80288208(i)->nParts;
    }
}

void ExhaustSmokeMan::InitLight(void) {
    PartParams e;
    e.maxParticles = 0x80;
    e.maxLife = 0.2f;
    e.spewRate = 1.0f;
    e.type = 0;
    e.shading = 0.5f;
    e.priority = 6;
    e.spread = ValV3(0.25f, 0.25f, 0.25f);
    e.velScale = ValV3(1.0f, 1.0f, 1.0f);
    e.upKick = 0.0f;
    e.gravity = -0.1f;
    e.drag = 1.0f;
    e.blueRamp = -0.2f;
    e.greenRamp = -0.2f;
    e.redRamp = -0.2f;
    e.blueConst = 0.9f;
    e.greenConst = 0.9f;
    e.redConst = 0.9f;
    e.alphaRamp = -0.25f;
    e.alphaConst = 0.25f;
    e.sizeRamp = 0.4f;
    e.sizeConst = 0.1f;
    light = fn_8028E2F8(&e, "light exhaust", lbl_8055C880);
}

void ExhaustSmokeMan::InitDark(void) {
    PartParams e;
    e.maxParticles = 0x60;
    e.maxLife = 1.75f;
    e.spewRate = 1.0f;
    e.type = 0;
    e.shading = 0.5f;
    e.priority = 6;
    e.spread = ValV3(0.25f, 0.25f, 0.25f);
    e.velScale = ValV3(1.0f, 1.0f, 1.0f);
    e.gravity = -0.1f;
    e.drag = 1.5f;
    e.greenConst = 0.15f;
    e.blueConst = 0.15f;
    e.redConst = 0.15f;
    e.blueRamp = 0.15f;
    e.greenRamp = 0.15f;
    e.redRamp = 0.15f;
    e.alphaRamp = -0.25f;
    e.alphaConst = 0.25f;
    e.sizeRamp = 1.5f;
    e.sizeConst = 0.15f;
    dark = fn_8028E2F8(&e, "dark exhaust", lbl_8055C880);
}

void ExhaustSmokeMan::Emit(void* e, int car, int part) {
    const SmokeCar* c = fn_80287B24(car);
    const VecMatrix* pt = fn_80288060(fn_80288208(car), part);
    Vec p;
    Vec d;
    MulVec(&p, &pt->origin, c->axes);
    p.x = p.x + c->pos.x;
    p.y = p.y + c->pos.y;
    p.z = p.z + c->pos.z;
    MulVec(&d, &pt->basis[1], c->axes);
    AddScaled(&d, &c->vel, &d, 1.75f);
    fn_8028E584(e, &p, &d);
}

FireSmokeMan::FireSmokeMan(void) {
    InitSmall();
    InitLarge();
}

FireSmokeMan::~FireSmokeMan(void) {
    fn_8028E398(large);
    fn_8028E398(small);
}

void FireSmokeMan::InitSmall(void) {
    PartParams e;
    e.maxParticles = 0x60;
    e.maxLife = 1.5f;
    e.spewRate = 0.15f;
    e.type = 0;
    e.shading = 0.5f;
    e.priority = 1;
    e.spread = ValV3(0.35f, 0.35f, 0.35f);
    e.velScale = ValV3(1.0f, 1.0f, 1.0f);
    e.upKick = 1.0f;
    e.angSpread = 3.0f;
    e.gravity = -0.15f;
    e.drag = 0.5f;
    e.blueRamp = 0.0f;
    e.greenRamp = 0.0f;
    e.redRamp = 0.0f;
    e.blueConst = 0.1f;
    e.greenConst = 0.1f;
    e.redConst = 0.1f;
    e.alphaRamp = -0.65f;
    e.alphaConst = 0.65f;
    e.sizeRamp = 2.0f;
    e.sizeConst = 0.75f;
    e.angRamp = -0.75f;
    e.angConst = 1.0f;
    small = fn_8028E2F8(&e, "small fire smoke", lbl_8055C880);
}

void FireSmokeMan::InitLarge(void) {
    PartParams e;
    e.maxParticles = 0x80;
    e.maxLife = 2.75f;
    e.spewRate = 0.1f;
    e.type = 0;
    e.shading = 0.5f;
    e.priority = 1;
    e.spread = ValV3(0.5f, 0.5f, 0.5f);
    e.velScale = ValV3(1.0f, 1.0f, 1.0f);
    e.upKick = 1.0f;
    e.angSpread = 3.0f;
    e.gravity = -0.15f;
    e.drag = 0.25f;
    e.blueRamp = 0.0f;
    e.greenRamp = 0.0f;
    e.redRamp = 0.0f;
    e.blueConst = 0.1f;
    e.greenConst = 0.1f;
    e.redConst = 0.1f;
    e.alphaRamp = -0.65f;
    e.alphaConst = 0.65f;
    e.sizeRamp = 4.0f;
    e.sizeConst = 3.0f;
    e.angRamp = -0.75f;
    e.angConst = 1.0f;
    large = fn_8028E2F8(&e, "large fire smoke", lbl_8055C880);
}

RadiatorSmokeMan::RadiatorSmokeMan(void) {
    InitEmitter();
    InitCars();
    Reset();
}

RadiatorSmokeMan::~RadiatorSmokeMan(void) {
    delete[] cars;
    cars = 0;
    fn_8028E398(emitter);
}

void RadiatorSmokeMan::Reset(void) {
    for (int i = 0; i < fn_80287B14(); i++) {
        cars[i].cur = 0;
    }
}

void RadiatorSmokeMan::Update(int i, int state) {
    RadiatorCar* c = &cars[i];
    if (state == 3 && !c->cur && c->count > 0) {
        c->cur = &c->pts[fn_80287B5C(c->count)];
        Emit(i);
    } else if (state != 3 && c->cur) {
        c->cur = 0;
    } else if (c->cur) {
        Emit(i);
    }
}

void RadiatorSmokeMan::InitEmitter(void) {
    PartParams e;
    e.maxParticles = 0x80;
    e.maxLife = 1.7f;
    e.spewRate = 0.4f;
    e.type = 2;
    e.shading = 0.5f;
    e.priority = 6;
    e.spread = ValV3(0.8f, 0.6f, 0.8f);
    e.velScale = ValV3(1.0f, 1.0f, 1.0f);
    e.upKick = 0.0f;
    e.angSpread = 5.0f;
    e.gravity = -0.05f;
    e.drag = 1.05f;
    e.blueRamp = 0.15f;
    e.greenRamp = 0.15f;
    e.redRamp = 0.15f;
    e.blueConst = 0.85f;
    e.greenConst = 0.85f;
    e.redConst = 0.85f;
    e.alphaRamp = -0.5f;
    e.alphaConst = 0.5f;
    e.sizeRamp = 2.5f;
    e.sizeConst = 0.25f;
    e.angRamp = -0.5f;
    e.angConst = 1.0f;
    emitter = fn_8028E2F8(&e, "radiator smoke", lbl_8055C880);
}

void RadiatorSmokeMan::InitCars(void) {
    SmokeModel* model;
    cars = new ("smoke car tab") RadiatorCar[fn_80287B14()];
    for (int i = 0; i < fn_80287B14(); i++) {
        model = fn_80288208(i);
        RadiatorCar* c = &cars[i];
        if (fn_80287E20(model, "radiator")) {
            RadiatorPoint* p;
            ValueNodeV* node = fn_80287E80(model, "radiator");
            c->count = node->GetIntValue("count");
            p = c->pts;
            for (int k = 0; k < c->count; k++, p++) {
                ValueNodeV* sub = node->GetChild(k);
                *(ValVec3*)&p->pos = sub->GetVec3("arm");
                *(ValVec3*)&p->dir = sub->GetVec3("dir");
            }
        } else {
            c->count = 0;
        }
    }
}

RadiatorCar::RadiatorCar(void) {}

void RadiatorSmokeMan::Emit(int i) {
    const SmokeCar* c = fn_80287B24(i);
    const RadiatorPoint* pt = cars[i].cur;
    Vec p;
    Vec d;
    MulVec(&p, &pt->pos, c->axes);
    p.x = p.x + c->pos.x;
    p.y = p.y + c->pos.y;
    p.z = p.z + c->pos.z;
    Scale(&d, &pt->dir, 0.7f);
    MulVec(&d, &d, c->axes);
    d.x = d.x + c->vel.x;
    d.y = d.y + c->vel.y;
    d.z = d.z + c->vel.z;
    fn_8028E584(emitter, &p, &d);
}
