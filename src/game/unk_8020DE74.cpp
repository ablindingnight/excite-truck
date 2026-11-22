// Code 0x8020DE74-0x8020F3AC, imported verbatim from the upstream project (github.com/orangedude27/excite-truck,
// CC0; file src/auto/text_8020DE74_.c), where its functions match exactly. Unit name is provisional.
// CLEANUP: automated-decompiler source with raw offsets and placeholder types; rewrite with real types
// before the PC port. Tricks are tagged MATCHING-AID.
#include "types.h"
#include "game/common.h"
#include <revolution/MTX/vec.h>
#include "game/carbody.h"
#include "game/gamedb.h"
#include "game/mathutil.h"

// Compiled as C++ (2026-11-03): the unit owns a ModuleTag, registered by its static initializer (the 8-byte
// function after the code, formerly the separate split unk_8020F3AC). The functions keep C linkage.
ModuleTag lbl_8055EEA8;

extern "C" {
// Code of main that had no source file yet: decomp.wtf gave it this one.
// split_file divides it into the game's real files (file_status shows its range).

extern const float lbl_80563368;
extern const float lbl_80563370;
extern const float lbl_805633DC;
extern const float lbl_805633F4;
extern const float lbl_805633F8;
extern const float lbl_805633FC;

#pragma cplusplus on
extern char lbl_8033C7A0[];
extern const float lbl_8056336C;
extern "C++" void *operator new(unsigned long size, const char *tag);
extern "C" void *memset(void *, int, unsigned long);
extern "C" void fn_801345D8(char *, int, const char *, ...);
extern "C" void *Create3DSound(const char *name, long kind, void *a, void *b);
extern "C" void *CreateSoundDash(const char *name, long kind, void *owner, long mode);

// A part created by Create3DSound: owner at 0x10, a level at 0x04 with a dirty flag at 0x4E.
struct HudPart {
    virtual ~HudPart();     /* 0x00: vptr (the parts are deleted through slot 0) */
    float level;            /* 0x04 */
    float alpha;            /* 0x08 */
    long padC;
    void *owner;            /* 0x10 */
    unsigned char pad14[5];
    unsigned char playing;  /* 0x19 */
    unsigned char pad1A[0x2F];
    unsigned char visible;  /* 0x49 */
    unsigned char hidden;   /* 0x4A */
    unsigned char shown;    /* 0x4B */
    unsigned char b4C;      /* 0x4C */
    unsigned char alphaDirty; /* 0x4D */
    unsigned char dirty;    /* 0x4E */
};

static inline void HudPart_SetLevel(HudPart *part, float level) {
    if (level != part->level) {
        part->dirty = 1;
        part->level = level;
    }
}

// The vehicle's sound set (the heap tag is "carsnd"): CarBody owns it at 0x2584 and drives it through the 13
// virtual slots below. Class and method names are unproven (/* Helper name unproven */), except where carbody.cpp's
// CbFx view already names a slot (Update 0x0C, Reset 0x10, Land 0x2C).
class CarSoundBase {  /* Helper name unproven */
public:
    virtual ~CarSoundBase() {}  // fn_801837DC (its weak copy sits after ~CarBody)
    virtual void Update() = 0;  // the interface CarBody drives (pure slots are 0 in its table)
    virtual void Reset() = 0;
    virtual void vf14() = 0;
    virtual void vf18() = 0;
    virtual void vf1C() = 0;
    virtual void vf20() = 0;
    virtual void vf24() = 0;
    virtual void vf28() = 0;
    virtual void Land(float sink) = 0;
    virtual void vf30() = 0;
    virtual void vf34() = 0;
    virtual void vf38() = 0;
};

namespace {  // private to this file: CarBody reaches it only through fn_8020DE74 and its own view
class CarSound : public CarSoundBase {  /* Helper name unproven */
public:
    CarSound(unsigned char *vehicle);
    virtual ~CarSound();        // 0x08
    virtual void Update();      // 0x0C
    virtual void Reset();       // 0x10
    virtual void vf14();
    virtual void vf18();
    virtual void vf1C();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void Land(float sink);  // 0x2C
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();

    unsigned char *vehicle;  /* 0x04 */
    HudPart *p08;
    HudPart *flash;          /* 0x0C: the flash picked from flashes[] */
    HudPart *flashes[2];     /* 0x10 */
    HudPart *speedLines;     /* 0x18 */
    HudPart *p1C, *p20, *p24, *p28, *p2C;
    HudPart *air;            /* 0x30 */
    HudPart *splash;         /* 0x34 */
    HudPart *dive;           /* 0x38 */
    HudPart *surface;        /* 0x3C */
    HudPart *drift;          /* 0x40 */
    HudPart *p44;
    HudPart *boost;          /* 0x48 */
    HudPart *tilt;           /* 0x4C */
    HudPart *p50;
    HudPart *start;          /* 0x54 */
    unsigned char started;   /* 0x58 */
    long flashCount;         /* 0x5C */
    float driftTime;         /* 0x60 */
    float flashLevel;        /* 0x64 */
    float speedAlpha;        /* 0x68 */
    float boostLevel;        /* 0x6C */
    unsigned char underwater;/* 0x70 */
    unsigned char b71;
    unsigned char startPending; /* 0x72 */
};
}  // namespace

static inline HudPart *CreateFlash(HudPart *&slot, const char *name, void *res, void *res2) {
    slot = (HudPart *)Create3DSound(name, 5, res, res2);
    return slot;
}

// Builds the sound parts for a vehicle; the extra ones exist only for the 'PCAR' type.
inline CarSound::CarSound(unsigned char *vehicle) {
    void *owner;
    void *res;
    bool isCar;
    char buf[0x20];
    unsigned long i;
    void *res2;

    this->vehicle = vehicle;
    res = (unsigned char *)&((CarBody *)vehicle)->vel;
    res2 = (unsigned char *)&((CarBody *)vehicle)->mtx.pos;
    owner = (void *)((CarBody *)vehicle)->index;
    const long type = ((CarBody *)vehicle)->type;
    isCar = type == 0x50434152;
    if (isCar) {
        this->p28 = (HudPart *)Create3DSound("waterbounce.sfx", 5, res, res2);
        this->air = (HudPart *)Create3DSound("airtime.sfx", 8, res, res2);
    } else {
        this->p28 = 0;
        this->air = 0;
    }
    this->p08 = (HudPart *)Create3DSound("backfire.sfx", 5, res, res2);
    this->flashCount = 0;
    memset(this->flashes, 0, 8);
    this->flash = 0;
    for (i = 0; i < 2; i++) {
        fn_801345D8(buf, 0x20, "boost%d.sfx", i + 1);
        HudPart *part = CreateFlash(this->flashes[i], buf, res, res2);
        if (part == 0) break;
        part->owner = owner;
        HudPart_SetLevel(this->flashes[i], lbl_80563368);
        this->flash = this->flashes[i];
        this->flashCount++;
    }
    if (isCar) {
        this->speedLines = (HudPart *)Create3DSound("boost_manual.sfx", 5, res, res2);
    } else {
        this->speedLines = 0;
    }
    if (this->p08 != 0) {
        this->p08->owner = owner;
        HudPart_SetLevel(this->p08, lbl_8056336C);
    }
    if (this->p28 != 0) this->p28->owner = owner;
    if (this->air != 0) this->air->owner = owner;
    if (isCar) {
        this->p1C = (HudPart *)Create3DSound("car_gnd1.sfx", 5, res, res2);
    } else {
        this->p1C = 0;
    }
    this->p20 = (HudPart *)Create3DSound("badland.sfx", 5, res, res2);
    this->splash = (HudPart *)Create3DSound("waterdrive.sfx", 5, res, res2);
    if (isCar) {
        this->dive = (HudPart *)Create3DSound("water_in.sfx", 5, res, res2);
        this->surface = (HudPart *)Create3DSound("water_out.sfx", 5, res, res2);
    } else {
        this->dive = 0;
        this->surface = 0;
    }
    if (this->splash != 0) this->splash->owner = owner;
    if (this->dive != 0) this->dive->owner = owner;
    if (this->surface != 0) this->surface->owner = owner;
    this->drift = (HudPart *)Create3DSound("dirtskid.sfx", 5, res, res2);
    if (this->drift != 0) this->drift->owner = owner;
    if (isCar) {
        this->p24 = (HudPart *)Create3DSound("niceland.sfx", 5, res, res2);
        this->p2C = (HudPart *)Create3DSound("boost_turbotap.sfx", 5, res, res2);
        this->boost = (HudPart *)CreateSoundDash("heatbeep.sfx", 1, owner, 2);
        this->tilt = (HudPart *)CreateSoundDash("overheat.sfx", 1, owner, 2);
        this->p50 = (HudPart *)CreateSoundDash("overheat_start.sfx", 1, owner, 2);
        this->p44 = (HudPart *)CreateSoundDash("watersink.sfx", 1, owner, 0);
        this->start = (HudPart *)CreateSoundDash("invulnerable.sfx", 1, owner, 0);
    } else {
        this->p24 = 0;
        this->p2C = 0;
        this->boost = 0;
        this->tilt = 0;
        this->p50 = 0;
        this->p44 = 0;
        this->start = 0;
    }
    this->driftTime = lbl_80563370;
    this->underwater = 0;
    this->flashLevel = lbl_80563370;
    this->speedAlpha = lbl_80563370;
    this->boostLevel = lbl_80563370;
    this->started = 0;
    this->startPending = 0;
    this->b71 = (FindVehicle(((CarBody *)vehicle)->ident.vehicle)->flags >> 2) & 1;
}

extern "C" void *fn_8020DE74(unsigned char *vehicle) {
    return new ("carsnd") CarSound(vehicle);
}

CarSound::~CarSound() {
    unsigned long i;
    if (p08 != 0) delete p08;
    for (i = 0; i < 2; i++) {
        if (flashes[i] != 0) delete flashes[i];
    }
    if (speedLines != 0) delete speedLines;
    if (p28 != 0) delete p28;
    if (air != 0) delete air;
    if (p1C != 0) delete p1C;
    if (p20 != 0) delete p20;
    if (p24 != 0) delete p24;
    if (p2C != 0) delete p2C;
    if (boost != 0) delete boost;
    if (tilt != 0) delete tilt;
    if (p50 != 0) delete p50;
    if (p44 != 0) delete p44;
    if (splash != 0) delete splash;
    if (dive != 0) delete dive;
    if (surface != 0) delete surface;
    if (drift != 0) delete drift;
    if (start != 0) delete start;
}


typedef struct HudVec { float x, y, z; } HudVec;

__declspec(section ".data") extern float __float_epsilon;
extern float lbl_8055E3C8;
extern const float lbl_80563374, lbl_80563378, lbl_8056337C, lbl_80563380, lbl_80563384, lbl_80563388;
extern const float lbl_8056338C, lbl_80563390, lbl_80563394, lbl_80563398, lbl_8056339C, lbl_805633A0;
extern const float lbl_805633A4, lbl_805633A8, lbl_805633AC, lbl_805633B0, lbl_805633B4, lbl_805633B8;
extern const float lbl_805633BC, lbl_805633C0, lbl_805633C4, lbl_805633C8, lbl_805633CC, lbl_805633D0;
extern const float lbl_805633D4, lbl_805633D8, lbl_805633DC, lbl_805633E0, lbl_805633E4, lbl_805633E8;
extern const float lbl_805633EC, lbl_805633F0;
extern "C" int fn_80129D18(int a, int b);
extern "C" int fn_80178CA4(void);
extern "C" int fn_801796C4(void);
extern "C" float fn_801E9FDC(short *cell, float x, float z);
extern "C" void fn_8020C910(float);
extern "C" long fn_802104B8(void);
extern "C" long fn_802104FC(void);
extern "C" int fn_802BF4F0(void);
extern "C" float fn_802BF590(void);

static inline float HudClamp01(float t) {
    float r = lbl_80563368 < t ? lbl_80563368 : t;
    return r > lbl_80563370 ? r : lbl_80563370;
}

static inline void HudPart_Show(HudPart *p) {
    p->hidden = 0;
    p->shown = 1;
    p->visible = 1;
}

static inline void HudPart_Hide(HudPart *p) {
    p->hidden = 1;
    p->b4C = 0;
}

static inline void HudPart_Play(HudPart *p) {
    p->visible = 1;
    p->shown = 0;
    p->hidden = 0;
}

static inline void HudPart_SetAlpha(HudPart *p, float alpha) {
    if (alpha != p->alpha) {
        p->alphaDirty = 1;
        p->alpha = alpha;
    }
}

static inline void HudXform(HudVec *out, const HudVec *in, const float *m) {
    if (out == in) {
        float x = Dot3(in->x, m[0], in->y, m[3], in->z, m[6]);
        float y = Dot3(in->x, m[1], in->y, m[4], in->z, m[7]);
        float z = Dot3(in->x, m[2], in->y, m[5], in->z, m[8]);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m[0], in->y, m[3], in->z, m[6]);
        out->y = Dot3(in->x, m[1], in->y, m[4], in->z, m[7]);
        out->z = Dot3(in->x, m[2], in->y, m[5], in->z, m[8]);
    }
}

// Per-frame sound update (field names are from an earlier HUD reading; the parts are sounds, see the constructor).

void CarSound::Update() {
    CarSound *h = this;
    bool boost;
    unsigned char *v;
    int underwater;
    unsigned char *wheel;
    bool single;
    int k;

    if (h->start != 0) {
        int begin = 0;
        if (!h->b71 && h->startPending) {
            begin = 1;
        }
        if (begin) {
            if (h->started) {
                HudPart_Hide(h->start);
                h->started = 0;
            } else {
                HudPart_Play(h->start);
                h->started = 1;
                h->startPending = 0;
            }
        } else {
            int moving = ((CarBody *)h->vehicle)->f25BC > __float_epsilon;
            if (!moving) {
                h->started = 0;
                HudPart_Hide(h->start);
            }
        }
    }

    if (h->boost != 0) {
        float base;
        float t;
        float step;
        v = h->vehicle;
        base = ((CarBody *)v)->f259C == 0 ? lbl_80563374 : lbl_80563378;
        t = HudClamp01((((CarBody *)v)->f25A0 - base) / ((lbl_8056337C + base) - base));
        step = (lbl_80563370 + t * lbl_80563368) - h->boostLevel;
        step = lbl_80563380 < step ? lbl_80563380 : step;
        step = step > lbl_80563384 ? step : lbl_80563384;
        h->boostLevel += step;
        if (h->boostLevel > lbl_80563388) {
            HudPart_SetAlpha(h->boost, h->boostLevel);
            HudPart_Show(h->boost);
        } else {
            HudPart_Hide(h->boost);
        }
    }

    if (h->flash != 0) {
        int hit = ((CarBody *)h->vehicle)->f258C > lbl_8056338C;
        if (hit) {
            h->flashLevel = lbl_80563368;
        } else {
            float f = h->flashLevel - lbl_80563390;
            h->flashLevel = lbl_80563370 > f ? lbl_80563370 : f;
        }
        if (hit || h->flashLevel > lbl_8056338C) {
            if (!h->flash->playing) {
                int i;
                for (i = 0; i < h->flashCount; i++) {
                    HudPart_Hide(h->flashes[i]);
                }
                h->flash = h->flashes[fn_80129D18(6, h->flashCount)];
            }
            HudPart_SetAlpha(h->flash, h->flashLevel);
            HudPart_SetLevel(h->flash, lbl_80563368);
            HudPart_Show(h->flash);
        } else {
            HudPart_Hide(h->flash);
        }
    }

    if (h->speedLines != 0) {
        v = h->vehicle;
        int enabled = ((CarBody *)v)->f259C;
        h->speedAlpha = lbl_80563368;
        if (enabled) {
            float x = ((CarBody *)v)->f25A0;
            float level;
            HudPart_Show(h->speedLines);
            level = lbl_80563374 + lbl_80563374 * (x * x);
            HudPart_SetAlpha(h->speedLines, h->speedAlpha);
            HudPart_SetLevel(h->speedLines, level);
        } else {
            HudPart_Hide(h->speedLines);
        }
    }

    if (h->air != 0) {
        float speed;
        float ground;
        float mag;
        float inv;
        speed = ((CarBody *)h->vehicle)->f2618;
        if (speed > lbl_80563394) {
            float airGain = HudClamp01((((CarBody *)h->vehicle)->speed - lbl_80563398) / lbl_8056339C);
            float airGainScale = lbl_80563368;
            float a = lbl_80563370 + airGain * airGainScale;
            float airSpeedGain = HudClamp01((speed - lbl_80563394) / lbl_805633A0);
            float airSpeedScale = lbl_80563368;
            float b = lbl_80563370 + airSpeedGain * airSpeedScale;
            HudPart_SetAlpha(h->air, b * a);
            HudPart_Show(h->air);
        } else {
            HudPart_Hide(h->air);
        }
        ground = fn_801E9FDC((short *)((unsigned char *)&((CarBody *)h->vehicle)->hint), ((CarBody *)h->vehicle)->mtx.pos.x, ((CarBody *)h->vehicle)->mtx.pos.z);
        HudVec dir = *(HudVec *)&((CarBody *)h->vehicle)->vel;
        mag = PSVECMag((const Vec*)&dir);
        mag = mag > lbl_80563388 ? mag : lbl_80563388;
        inv = lbl_80563368 / mag;
        dir.x *= inv;
        dir.y *= inv;
        dir.z *= inv;
        single = fn_80178CA4() == 1;
        boost = 0;
        if (!fn_801796C4() && single && ((CarBody *)h->vehicle)->mtx.pos.y - ground > lbl_805633A4 && speed > lbl_805633A8 &&
            dir.y > lbl_805633AC && (!fn_802104B8() || fn_802104FC())) {
            boost = 1;
        }
        if (boost) {
            float boostWeight = HudClamp01((speed - lbl_805633B0) / lbl_805633B4);
            float boostScale = lbl_805633B8;
            fn_8020C910(lbl_80563368 + boostWeight * boostScale);
        } else if (speed < lbl_80563394) {
            fn_8020C910(lbl_80563368);
        }
    }

    if (h->tilt != 0) {
        float x = ((CarBody *)h->vehicle)->f2588;
        int on = x > lbl_80563370;
        if (on) {
            if (x > lbl_805633BC) {
                HudPart_SetAlpha(h->tilt, lbl_80563368);
            } else {
                float tiltWeight = HudClamp01((x - lbl_805633BC) / lbl_805633C0);
                float tiltScale = lbl_805633C4;
                HudPart_SetAlpha(h->tilt, lbl_80563368 + tiltWeight * tiltScale);
            }
            HudPart_Show(h->tilt);
        } else {
            HudPart_Hide(h->tilt);
        }
    }

    underwater = 0;
    wheel = (unsigned char *)&((CarBody *)h->vehicle)->wheels;
    for (k = 0; k < 4; k++) {
        HudVec p;
        v = h->vehicle;
        HudVec *in = (HudVec *)((CbWheel *)wheel)->local;
        HudXform(&p, in, (float *)((unsigned char *)&((CarBody *)v)->mtx));
        p.x += ((CarBody *)v)->mtx.pos.x;
        p.y += ((CarBody *)v)->mtx.pos.y;
        p.z += ((CarBody *)v)->mtx.pos.z;
        if (p.y < fn_802BF590()) {
            underwater = 1;
            break;
        }
        wheel += 0x74;
    }

    if (h->splash != 0 && fn_802BF4F0()) {
        if (underwater) {
            float splashSpeedScale = lbl_805633C8;
            float verticalSpeed = ((CarBody *)h->vehicle)->speed;
            float s = splashSpeedScale * verticalSpeed;
            float splashAlphaWeight = HudClamp01((s - lbl_80563370) / lbl_805633CC);
            float splashAlphaScale = lbl_805633D0;
            HudPart_SetAlpha(h->splash, lbl_80563370 + splashAlphaWeight * splashAlphaScale);
            float splashLevelWeight = HudClamp01((s - lbl_80563370) / lbl_805633D4);
            float splashLevelScale = lbl_805633D8;
            HudPart_SetLevel(h->splash, lbl_80563368 + splashLevelWeight * splashLevelScale);
            HudPart_Show(h->splash);
        } else {
            HudPart_Hide(h->splash);
        }
    }
    if (h->dive != 0 && fn_802BF4F0() && underwater && !h->underwater) {
        HudPart_Play(h->dive);
    }
    if (h->surface != 0 && fn_802BF4F0() && h->underwater && !underwater && lbl_8055E3C8 > lbl_80563380) {
        HudPart_Play(h->surface);
    }
    h->underwater = underwater;

    if (h->drift != 0) {
        int shown = 0;
        float spd;
        v = h->vehicle;
        spd = ((CarBody *)v)->speed;
        if (FAbs(((CarBody *)v)->f2690) <= __float_epsilon && spd > lbl_805633DC && ((CarBody *)v)->airborne > 1 &&
            !underwater) {
            float dot = FAbs(PSVECDotProduct((const Vec*)((HudVec *)((unsigned char *)&((CarBody *)v)->vel)), (const Vec*)((HudVec *)((unsigned char *)&((CarBody *)v)->mtx))));
            float lim = spd * (FAbs(h->driftTime) <= __float_epsilon ? lbl_805633E0 : lbl_805633E4);
            shown = dot > lim;
            if (shown) {
                float t = HudClamp01(lbl_805633DC * (dot / lim - lbl_80563368));
                float fade = lbl_805633E8 * h->driftTime;
                fade = fade < lbl_80563368 ? fade : lbl_80563368;
                HudPart_SetAlpha(h->drift, lbl_805633EC * (t * fade));
                HudPart_Show(h->drift);
                h->driftTime += lbl_805633F0;
            }
        }
        if (!shown) {
            HudPart_Hide(h->drift);
            h->driftTime = lbl_80563370;
        }
    }
}

#pragma cplusplus reset

extern "C++" long fn_80009C84(void);  /* replay.cpp (C++ linkage) */
extern void fn_80206750(void *, void *, long);

// Applies the current mode (0 while fn_80009C84 is set, otherwise 2) to the three optional parts.
void CarSound::Reset() {
    void *owner = (void *)((CarBody *)vehicle)->index;
    long busy = fn_80009C84();
    long mode = 2;
    if (busy) mode = 0;
    if (boost != 0)
        fn_80206750(boost, owner, mode);
    if (tilt != 0)
        fn_80206750(tilt, owner, mode);
    if (p50 != 0)
        fn_80206750(p50, owner, mode);
}

void CarSound::vf14() {
    HudPart *p = p50;
    if (p != 0) {
        p->visible = 1;
        p->shown = 0;
        p->hidden = 0;
    }
}

void CarSound::vf18() {
    HudPart *p = p28;
    if (p != 0) {
        p->visible = 1;
        p->shown = 0;
        p->hidden = 0;
    }
}

void CarSound::vf1C() {
    HudPart *p = p08;
    if (p != 0) {
        float value = lbl_80563368;
        if (value != p->alpha) {
            p->alphaDirty = 1;
            p->alpha = value;
        }
        p = p08;
        p->visible = 1;
        p->shown = 0;
        p->hidden = 0;
    }
}
void CarSound::Land(float farg0) {
    HudPart *p = p1C;
    if (p != 0) {
        float t = lbl_80563368;
        float r = (farg0 - lbl_805633DC) / lbl_805633F4;
        t = t < r ? t : r;
        t = t > lbl_80563370 ? t : lbl_80563370;
        float alpha = lbl_805633F8 + (t * lbl_805633FC);
        if (alpha != p->alpha) {
            p->alphaDirty = 1;
            p->alpha = alpha;
        }
        p = p1C;
        p->visible = 1;
        p->shown = 0;
        p->hidden = 0;
    }
}

void CarSound::vf20() {
    HudPart *p = p24;
    if (p != 0) {
        p->visible = 1;
        p->shown = 0;
        p->hidden = 0;
    }
}

void CarSound::vf24() {
    HudPart *p = p20;
    if (p != 0) {
        p->visible = 1;
        p->shown = 0;
        p->hidden = 0;
    }
}
void CarSound::vf28() {
    HudPart *p = p2C;
    if (p != 0) {
        p->visible = 1;
        p->shown = 0;
        p->hidden = 0;
    }
}
void CarSound::vf30() {
    HudPart *p = p44;
    if (p != 0) {
        p->visible = 1;
        p->shown = 0;
        p->hidden = 0;
    }
}
void CarSound::vf34() {

}

void CarSound::vf38() {
    startPending = 1;
}
extern const float lbl_80563368;
extern const float lbl_80563370;
extern const float lbl_805633DC;
extern const float lbl_805633F4;
extern const float lbl_805633F8;
extern const float lbl_805633FC;

// .data (0x8033C7A0-0x8033C940): the constructor's sound-name strings, then the compiler's vtables for CarSound
// (0x8033C8C4) and the abstract CarSoundBase (0x8033C900: its inline dtor fn_801837DC, then 12 pure slots = zeros),
// then 4 bytes of alignment padding.


// The unit's .sdata2 constants (0x80563368-0x80563400), in address order.
extern const float lbl_80563368 = 1.0f;
extern const float lbl_8056336C = 2.0f;
extern const float lbl_80563370 = 0.0f;
extern const float lbl_80563374 = 0.75f;
extern const float lbl_80563378 = 0.66f;
extern const float lbl_8056337C = 0.05f;
extern const float lbl_80563380 = 0.1f;
extern const float lbl_80563384 = -0.1f;
extern const float lbl_80563388 = 0.01f;
extern const float lbl_8056338C = 0.001f;
extern const float lbl_80563390 = 0.066733405f;
extern const float lbl_80563394 = 0.25f;
extern const float lbl_80563398 = 17.777779f;
extern const float lbl_8056339C = 26.666668f;
extern const float lbl_805633A0 = 2.75f;
extern const float lbl_805633A4 = 75.0f;
extern const float lbl_805633A8 = 1.35f;
extern const float lbl_805633AC = -0.3f;
extern const float lbl_805633B0 = 2.2f;
extern const float lbl_805633B4 = 0.79999995f;
extern const float lbl_805633B8 = -0.8f;
extern const float lbl_805633BC = 0.5f;
extern const float lbl_805633C0 = -0.5f;
extern const float lbl_805633C4 = -1.0f;
extern const float lbl_805633C8 = 0.44444445f;
extern const float lbl_805633CC = 20.0f;
extern const float lbl_805633D0 = 0.9f;
extern const float lbl_805633D4 = 85.0f;
extern const float lbl_805633D8 = 0.45000005f;
extern const float lbl_805633DC = 5.0f;
extern const float lbl_805633E0 = 0.35f;
extern const float lbl_805633E4 = 0.15f;
extern const float lbl_805633E8 = 3.0f;
extern const float lbl_805633EC = 0.4f;
extern const float lbl_805633F0 = 0.016683351f;
extern const float lbl_805633F4 = 15.0f;
extern const float lbl_805633F8 = 0.2f;
extern const float lbl_805633FC = 0.6f;

}  // extern "C"
