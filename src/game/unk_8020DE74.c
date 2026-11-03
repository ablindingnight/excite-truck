// Code 0x8020DE74-0x8020F3AC, imported verbatim from the upstream project (github.com/orangedude27/excite-truck,
// CC0; file src/auto/text_8020DE74_.c), where its functions match exactly. Unit name is provisional.
// CLEANUP: automated-decompiler source with raw offsets and placeholder types; rewrite with real types
// before the PC port. Tricks are tagged MATCHING-AID.
#include "types.h"
// Code of main that had no source file yet: decomp.wtf gave it this one.
// split_file divides it into the game's real files (file_status shows its range).

extern const float lbl_80563368;
extern const float lbl_80563370;
extern const float lbl_805633DC;
extern const float lbl_805633F4;
extern const float lbl_805633F8;
extern const float lbl_805633FC;

#pragma cplusplus on
extern void *lbl_8033C8C4[];
extern char lbl_8033C7A0[];
extern char lbl_8055B648[7];
extern const float lbl_8056336C;
void *operator new(unsigned long size, const char *tag);
extern "C" void *memset(void *, int, unsigned long);
extern "C" void fn_801345D8(char *, int, const char *, ...);
extern "C" void *fn_802077A4(const char *name, long kind, void *a, void *b);
extern "C" void *fn_802076A0(const char *name, long kind, void *owner, long mode);
long *FindVehicle(long);

// A part created by fn_802077A4: owner at 0x10, a level at 0x04 with a dirty flag at 0x4E.
struct HudPart {
    long pad0;
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

// Builds the HUD parts for a vehicle; the extra gauges exist only for the 'PCAR' type.
extern "C" void *fn_8020DE74(unsigned char *vehicle) {
    const char *names = lbl_8033C7A0;
    unsigned char *hud = (unsigned char *)operator new(0x74, lbl_8055B648);
    if (hud != 0) {
        long isCar;
        void *res2;
        void *owner;
        void *res;
        unsigned long i;
        char buf[0x20];

        *(void ***)hud = lbl_8033C8C4;
        *(unsigned char **)(hud + 4) = vehicle;
        res = vehicle + 0x118;
        res2 = vehicle + 0x60;
        owner = *(void **)(vehicle + 0x1EC);
        isCar = *(long *)(vehicle + 0x10) == 0x50434152;
        if (isCar) {
            *(void **)(hud + 0x28) = fn_802077A4(names + 0x00, 5, res, res2);
            *(void **)(hud + 0x30) = fn_802077A4(names + 0x10, 8, res, res2);
        } else {
            *(void **)(hud + 0x28) = 0;
            *(void **)(hud + 0x30) = 0;
        }
        *(void **)(hud + 8) = fn_802077A4(names + 0x1C, 5, res, res2);
        *(long *)(hud + 0x5C) = 0;
        memset(hud + 0x10, 0, 8);
        *(void **)(hud + 0xC) = 0;
        for (i = 0; i < 2; i++) {
            HudPart *part;
            fn_801345D8(buf, 0x20, names + 0x2C, i + 1);
            part = (HudPart *)fn_802077A4(buf, 5, res, res2);
            ((HudPart **)(hud + 0x10))[i] = part;
            if (part == 0) break;
            part->owner = owner;
            HudPart_SetLevel(((HudPart **)(hud + 0x10))[i], lbl_80563368);
            *(void **)(hud + 0xC) = ((HudPart **)(hud + 0x10))[i];
            (*(long *)(hud + 0x5C))++;
        }
        if (isCar) {
            *(void **)(hud + 0x18) = fn_802077A4(names + 0x38, 5, res, res2);
        } else {
            *(void **)(hud + 0x18) = 0;
        }
        if (*(HudPart **)(hud + 8) != 0) {
            (*(HudPart **)(hud + 8))->owner = owner;
            HudPart_SetLevel(*(HudPart **)(hud + 8), lbl_8056336C);
        }
        if (*(HudPart **)(hud + 0x28) != 0) (*(HudPart **)(hud + 0x28))->owner = owner;
        if (*(HudPart **)(hud + 0x30) != 0) (*(HudPart **)(hud + 0x30))->owner = owner;
        if (isCar) {
            *(void **)(hud + 0x1C) = fn_802077A4(names + 0x4C, 5, res, res2);
        } else {
            *(void **)(hud + 0x1C) = 0;
        }
        *(void **)(hud + 0x20) = fn_802077A4(names + 0x5C, 5, res, res2);
        *(void **)(hud + 0x34) = fn_802077A4(names + 0x68, 5, res, res2);
        if (isCar) {
            *(void **)(hud + 0x38) = fn_802077A4(names + 0x78, 5, res, res2);
            *(void **)(hud + 0x3C) = fn_802077A4(names + 0x88, 5, res, res2);
        } else {
            *(void **)(hud + 0x38) = 0;
            *(void **)(hud + 0x3C) = 0;
        }
        if (*(HudPart **)(hud + 0x34) != 0) (*(HudPart **)(hud + 0x34))->owner = owner;
        if (*(HudPart **)(hud + 0x38) != 0) (*(HudPart **)(hud + 0x38))->owner = owner;
        if (*(HudPart **)(hud + 0x3C) != 0) (*(HudPart **)(hud + 0x3C))->owner = owner;
        *(void **)(hud + 0x40) = fn_802077A4(names + 0x98, 5, res, res2);
        if (*(HudPart **)(hud + 0x40) != 0) (*(HudPart **)(hud + 0x40))->owner = owner;
        if (isCar) {
            *(void **)(hud + 0x24) = fn_802077A4(names + 0xA8, 5, res, res2);
            *(void **)(hud + 0x2C) = fn_802077A4(names + 0xB8, 5, res, res2);
            *(void **)(hud + 0x48) = fn_802076A0(names + 0xCC, 1, owner, 2);
            *(void **)(hud + 0x4C) = fn_802076A0(names + 0xDC, 1, owner, 2);
            *(void **)(hud + 0x50) = fn_802076A0(names + 0xEC, 1, owner, 2);
            *(void **)(hud + 0x44) = fn_802076A0(names + 0x100, 1, owner, 0);
            *(void **)(hud + 0x54) = fn_802076A0(names + 0x110, 1, owner, 0);
        } else {
            *(void **)(hud + 0x24) = 0;
            *(void **)(hud + 0x2C) = 0;
            *(void **)(hud + 0x48) = 0;
            *(void **)(hud + 0x4C) = 0;
            *(void **)(hud + 0x50) = 0;
            *(void **)(hud + 0x44) = 0;
            *(void **)(hud + 0x54) = 0;
        }
        *(float *)(hud + 0x60) = lbl_80563370;
        *(unsigned char *)(hud + 0x70) = 0;
        *(float *)(hud + 0x64) = lbl_80563370;
        *(float *)(hud + 0x68) = lbl_80563370;
        *(float *)(hud + 0x6C) = lbl_80563370;
        *(unsigned char *)(hud + 0x58) = 0;
        *(unsigned char *)(hud + 0x72) = 0;
        *(unsigned char *)(hud + 0x71) = (FindVehicle(*(long *)(vehicle + 0x25C))[0xA0 / 4] >> 2) & 1;
    }
    return hud;
}

extern "C" void fn_80128668(void *);
extern "C" void *fn_8020E288(void *arg0, s32 arg1) {
    struct Dummy { virtual ~Dummy(); };
    void *child;
    u8 *p;
    u32 i;
    if (arg0 != 0) {
        child = *(void **)((u8 *)arg0 + 8);
        *(void **)arg0 = lbl_8033C8C4;
        if (child != 0) delete (Dummy *)child;
        p = (u8 *)arg0;
        i = 0;
        do {
            child = *(void **)(p + 0x10);
            if (child != 0) delete (Dummy *)child;
            p += 4;
            i++;
        } while (i < 2);
        child = *(void **)((u8 *)arg0 + 0x18);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x28);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x30);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x1C);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x20);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x24);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x2C);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x48);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x4C);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x50);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x44);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x34);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x38);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x3C);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x40);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x54);
        if (child != 0) delete (Dummy *)child;
        if (arg1 > 0) fn_80128668(arg0);
    }
    return arg0;
}

// The HUD object built by fn_8020DE74.
struct Hud {
    void *vtable;
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

typedef struct HudVec { float x, y, z; } HudVec;

// The vehicle fields read here by member (the rest by offset).
struct HudCar {
    unsigned char pad0[0x118];
    HudVec vel; /* 0x118 */
};

__declspec(section ".data") extern float lbl_80558DC4;
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
extern "C" float PSVECMag(const HudVec *);
extern "C" float PSVECDotProduct(const HudVec *, const HudVec *);

#define VF32(v, off) (*(float *)((v) + (off)))

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
        float x = in->x;
        float y = in->y;
        float z = in->z;
        out->x = x * m[0] + y * m[3] + z * m[6];
        out->y = x * m[1] + y * m[4] + z * m[7];
        out->z = x * m[2] + y * m[5] + z * m[8];
    } else {
        out->x = in->x * m[0] + in->y * m[3] + in->z * m[6];
        out->y = in->x * m[1] + in->y * m[4] + in->z * m[7];
        out->z = in->x * m[2] + in->y * m[5] + in->z * m[8];
    }
}

// Per-frame HUD update: start prompt, boost glow, hit flashes, speed lines, air and water effects, drift meter.
extern "C" void fn_8020E598(Hud *h) {
    unsigned char *v;
    int underwater;
    int k;
    unsigned char *wheel;

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
            int moving = VF32(h->vehicle, 0x25BC) > lbl_80558DC4;
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
        base = v[0x259C] == 0 ? lbl_80563374 : lbl_80563378;
        t = HudClamp01((VF32(v, 0x25A0) - base) / ((lbl_8056337C + base) - base));
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
        int hit = VF32(h->vehicle, 0x258C) > lbl_8056338C;
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
        h->speedAlpha = lbl_80563368;
        if (v[0x259C]) {
            float x = VF32(v, 0x25A0);
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
        HudVec dir;
        float mag;
        float inv;
        int boost;
        int single;
        v = h->vehicle;
        speed = VF32(v, 0x2618);
        if (speed > lbl_80563394) {
            float a = lbl_80563370 + HudClamp01((VF32(v, 0x25D4) - lbl_80563398) / lbl_8056339C) * lbl_80563368;
            float b = lbl_80563370 + HudClamp01((speed - lbl_80563394) / lbl_805633A0) * lbl_80563368;
            HudPart_SetAlpha(h->air, b * a);
            HudPart_Show(h->air);
        } else {
            HudPart_Hide(h->air);
        }
        ground = fn_801E9FDC((short *)(h->vehicle + 0x6C), VF32(h->vehicle, 0x60), VF32(h->vehicle, 0x68));
        dir = ((HudCar *)h->vehicle)->vel;
        mag = PSVECMag(&dir);
        mag = mag > lbl_80563388 ? mag : lbl_80563388;
        inv = lbl_80563368 / mag;
        dir.x *= inv;
        dir.y *= inv;
        dir.z *= inv;
        single = fn_80178CA4() == 1;
        boost = 0;
        if (!fn_801796C4() && single && VF32(h->vehicle, 0x64) - ground > lbl_805633A4 && speed > lbl_805633A8 &&
            dir.y > lbl_805633AC && (!fn_802104B8() || fn_802104FC())) {
            boost = 1;
        }
        if (boost) {
            fn_8020C910(lbl_80563368 + HudClamp01((speed - lbl_805633B0) / lbl_805633B4) * lbl_805633B8);
        } else if (speed < lbl_80563394) {
            fn_8020C910(lbl_80563368);
        }
    }

    if (h->tilt != 0) {
        float x = VF32(h->vehicle, 0x2588);
        int on = x > lbl_80563370;
        if (on) {
            if (x > lbl_805633BC) {
                HudPart_SetAlpha(h->tilt, lbl_80563368);
            } else {
                HudPart_SetAlpha(h->tilt, lbl_80563368 + HudClamp01((x - lbl_805633BC) / lbl_805633C0) * lbl_805633C4);
            }
            HudPart_Show(h->tilt);
        } else {
            HudPart_Hide(h->tilt);
        }
    }

    underwater = 0;
    wheel = h->vehicle + 0x23AC;
    for (k = 0; k < 4; k++) {
        HudVec p;
        v = h->vehicle;
        HudXform(&p, (HudVec *)(wheel + 0x30), (float *)(v + 0x3C));
        p.x += VF32(v, 0x60);
        p.y += VF32(v, 0x64);
        p.z += VF32(v, 0x68);
        if (p.y < fn_802BF590()) {
            underwater = 1;
            break;
        }
        wheel += 0x74;
    }

    if (h->splash != 0 && fn_802BF4F0()) {
        if (underwater) {
            float s = lbl_805633C8 * VF32(h->vehicle, 0x25D4);
            HudPart_SetAlpha(h->splash, lbl_80563370 + HudClamp01((s - lbl_80563370) / lbl_805633CC) * lbl_805633D0);
            HudPart_SetLevel(h->splash, lbl_80563368 + HudClamp01((s - lbl_80563370) / lbl_805633D4) * lbl_805633D8);
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
        spd = VF32(v, 0x25D4);
        if ((float)__fabs(VF32(v, 0x2690)) <= lbl_80558DC4 && spd > lbl_805633DC && *(long *)(v + 0x2620) > 1 &&
            !underwater) {
            float dot = (float)__fabs(PSVECDotProduct((HudVec *)(v + 0x118), (HudVec *)(v + 0x3C)));
            float lim = spd * ((float)__fabs(h->driftTime) <= lbl_80558DC4 ? lbl_805633E0 : lbl_805633E4);
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

#pragma scheduling on
#pragma cplusplus reset

extern long fn_80009C84(void);
extern void fn_80206750(void *, void *, long);

// Applies the current mode (0 while fn_80009C84 is set, otherwise 2) to the three optional parts.
void fn_8020F15C(void *arg0) {
    void *owner = *(void **)((char *)*(void **)((char *)arg0 + 4) + 0x1EC);
    long busy = fn_80009C84();
    long mode = 2;
    if (busy) mode = 0;
    if (*(void **)((char *)arg0 + 0x48) != 0)
        fn_80206750(*(void **)((char *)arg0 + 0x48), owner, mode);
    if (*(void **)((char *)arg0 + 0x4C) != 0)
        fn_80206750(*(void **)((char *)arg0 + 0x4C), owner, mode);
    if (*(void **)((char *)arg0 + 0x50) != 0)
        fn_80206750(*(void **)((char *)arg0 + 0x50), owner, mode);
}

void fn_8020F1F8(void *arg0) {
    void *temp_r3;
    temp_r3 = *(void **)((char *)arg0 + 0x50);
    if (temp_r3 != 0) {
        *(signed char *)((char *)temp_r3 + 0x49) = 1;
        *(signed char *)((char *)temp_r3 + 0x4B) = 0;
        *(signed char *)((char *)temp_r3 + 0x4A) = 0;
    }
}

void fn_8020F21C(void *arg0) {
    void *temp_r3;
    temp_r3 = *(void **)((char *)arg0 + 0x28);
    if (temp_r3 != 0) {
        *(signed char *)((char *)temp_r3 + 0x49) = 1;
        *(signed char *)((char *)temp_r3 + 0x4B) = 0;
        *(signed char *)((char *)temp_r3 + 0x4A) = 0;
    }
}

void fn_8020F240(void *arg0) {
// MATCHING-AID: register pinning from upstream; revisit.
    register void *p;
    float value;
    p = *(void **)((char *)arg0 + 8);
    if (p != 0) {
        value = lbl_80563368;
        if (value != *(float *)((char *)p + 8)) {
            *(signed char *)((char *)p + 0x4D) = 1;
            *(float *)((char *)p + 8) = value;
        }
        p = *(void **)((char *)arg0 + 8);
        *(signed char *)((char *)p + 0x49) = 1;
        *(signed char *)((char *)p + 0x4B) = 0;
        *(signed char *)((char *)p + 0x4A) = 0;
    }
}
void fn_8020F284(void *arg0, float farg0) {
    float temp_f0;
    float temp_f1;
    float var_f2;
    void *temp_r4;
    void *temp_r4_2;
    temp_r4 = *(void **)((char *)arg0 + 0x1C);
    if (temp_r4 != 0) {
        var_f2 = lbl_80563368;
        temp_f0 = (farg0 - lbl_805633DC) / lbl_805633F4;
        var_f2 = var_f2 < temp_f0 ? var_f2 : temp_f0;
        var_f2 = var_f2 > lbl_80563370 ? var_f2 : lbl_80563370;
        temp_f1 = lbl_805633F8 + (var_f2 * lbl_805633FC);
        if (temp_f1 != *(float *)((char *)temp_r4 + 8)) {
            *(signed char *)((char *)temp_r4 + 0x4D) = 1;
            *(float *)((char *)temp_r4 + 8) = temp_f1;
        }
        temp_r4_2 = *(void **)((char *)arg0 + 0x1C);
        *(signed char *)((char *)temp_r4_2 + 0x49) = 1;
        *(char *)((char *)temp_r4_2 + 0x4B) = 0;
        *(char *)((char *)temp_r4_2 + 0x4A) = 0;
    }
}

void fn_8020F30C(void *arg0) {
    void *temp_r3;
    temp_r3 = *(void **)((char *)arg0 + 0x24);
    if (temp_r3 != 0) {
        *(signed char *)((char *)temp_r3 + 0x49) = 1;
        *(signed char *)((char *)temp_r3 + 0x4B) = 0;
        *(signed char *)((char *)temp_r3 + 0x4A) = 0;
    }
}

void fn_8020F330(void *arg0) {
    void *temp_r3;

    temp_r3 = *(void **)((char *)arg0 + 0x20);
    if (temp_r3 != 0) {
        *(signed char *)((char *)temp_r3 + 0x49) = 1;
        *(signed char *)((char *)temp_r3 + 0x4B) = 0;
        *(signed char *)((char *)temp_r3 + 0x4A) = 0;
    }
}
void fn_8020F354(void *arg0) {
    void *temp_r3;
    temp_r3 = *(void **)((char *)arg0 + 0x2C);
    if (temp_r3 != 0) {
        *(signed char *)((char *)temp_r3 + 0x49) = 1;
        *(signed char *)((char *)temp_r3 + 0x4B) = 0;
        *(signed char *)((char *)temp_r3 + 0x4A) = 0;
    }
}
void fn_8020F378(void *arg0) {
    void *temp_r3;

    temp_r3 = *(void **)((char *)arg0 + 0x44);
    if (temp_r3 != 0) {
        *(signed char *)((char *)temp_r3 + 0x49) = 1;
        *(signed char *)((char *)temp_r3 + 0x4B) = 0;
        *(signed char *)((char *)temp_r3 + 0x4A) = 0;
    }
}
void fn_8020F39C(void) {

}

void fn_8020F3A0(void *arg0) {
    *(signed char *)((char *)arg0 + 0x72) = 1;
}
extern const float lbl_80563368;
extern const float lbl_80563370;
extern const float lbl_805633DC;
extern const float lbl_805633F4;
extern const float lbl_805633F8;
extern const float lbl_805633FC;
