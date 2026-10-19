// Physics driver input (0x8017EB80-0x8018185C): the per-player DriverInterface objects that turn controller
// input (dead spots, overshoot, smoothing) into steering/throttle/brake for the physics, plus rumble and
// the button event messages. Names are guesses apart from the strings. FIRST DRAFT: the five large update
// and config functions (fn_8017ED00, fn_8017F718, fn_8017FF4C, fn_801808DC, fn_8018108C) are not written.
#include "game/common.h"

struct DButton;

void* operator new(size_t size, const char* tag);

extern "C" {
void fn_802E8518(int ch);
void fn_802E30C8(const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);
void fn_802E8524(int a, int ch);
void fn_802E8528(int ch, f32 a, f32 b);
void fn_802E85A8(int ch, f32 a, f32 b, f32 c);
void fn_802E85AC(int ch, f32 a, f32 b);
void fn_802E85F4(int ch);
int fn_801769F8(void);
int fn_8027AA48(void);
int fn_801B526C(void);
int fn_801C5B60(void);
int fn_801A8EE4(void* b);  // button held
int fn_801A8E68(void* b);  // button pressed
void fn_801797F8(int car, u32 kind, u32 idx);
s32 fn_8026A368(int car);
void fn_801808DC(void);
int fn_801333E8(void* t, const char* name, u8* out);
int fn_801331C4(void* t, const char* name, f32* out);
s32 fn_80137CFC(s32 car);
f32 fn_8014C8F8(f32 x, f32 lo, f32 hi);
f32 fn_801429D0(f32 a, f32 b);
void fn_801A7DFC(const char* a, const char* b, void* t);
void fn_801A7974(DButton* b, const char* name, void* t);
void fn_801A9110(DButton* b, const char* name);
int sprintf(char* buf, const char* fmt, ...);
int fn_801298E8(void* out, s32 ch);
void fn_801A7844(int a);
int fn_801A8E38(void* b);  // button down
int fn_801A8F10(void* b);  // button bound
f32 fn_801A7F98(void* b);  // button analog value
f32 fn_80144C80(f32 a, f32 b, f32 t);
int fn_802E85B0(s32 ch);
int fn_80179300(void);
}

struct DButton {
    DButton(s32 c = 0);
    u8 pad0[0x4];
    s32 ch;  // offset 0x4
    u8 pad8[0x4];
    u8 bC;   // offset 0xC
    u8 padD[3];
};

struct ValTable;

class DriverInterface {
public:
    DriverInterface(void);
    virtual void SetGear(int g) = 0;     // 0x8
    virtual f32 Steer(void) = 0;         // 0xC
    virtual f32 Throttle(void) = 0;      // 0x10
    virtual f32 Brake(void) = 0;         // 0x14
    virtual f32 Grip(void) = 0;          // 0x18
    virtual f32 vf1C(void) = 0;          // 0x1C
    virtual s32 MaxGear(void) = 0;       // 0x20
    virtual void SetMaxGear(s32 g) = 0;  // 0x24
    virtual void SetThrottle(f32 v) = 0;  // 0x28
    virtual u8 vf2C(void) = 0;           // 0x2C
    virtual u8 vf30(void) = 0;           // 0x30
    virtual u8 vf34(void) = 0;           // 0x34
    virtual u8 vf38(void) = 0;           // 0x38
    virtual u8 vf3C(void) = 0;           // 0x3C
    virtual f32 vf40(void) = 0;          // 0x40
    virtual f32 vf44(void) = 0;          // 0x44
    virtual f32 vf48(void) = 0;          // 0x48
    virtual u8 vf4C(void) = 0;           // 0x4C
    virtual void SetRumbleA(f32 a, f32 b, f32 c) = 0;  // 0x50
    virtual void SetRumbleB(f32 a, f32 b, f32 c) = 0;  // 0x54
    virtual void SetRumbleC(f32 a, f32 b) = 0;         // 0x58
    virtual int vf5C(void) = 0;          // 0x5C
    virtual int vf60(void) = 0;          // 0x60
    virtual int vf64(void) = 0;          // 0x64
    virtual u8 vf68(void) = 0;           // 0x68
    virtual s32 vf6C(void) = 0;          // 0x6C
    virtual s32 vf70(void) = 0;          // 0x70
    virtual void Buttons(void);          // 0x74
    virtual void SetRumble(u8 on);       // 0x78
    virtual void Stop(void);             // 0x7C
    virtual ~DriverInterface(void) {}    // 0x80
};

class PhysDriver : public DriverInterface {
public:
    PhysDriver(int i);
    void Init(void);
    virtual void SetGear(int g);
    virtual f32 Steer(void);
    virtual f32 Throttle(void);
    virtual f32 Brake(void);
    virtual f32 Grip(void);
    virtual f32 vf1C(void);
    virtual s32 MaxGear(void);
    virtual void SetMaxGear(s32 g);
    virtual void SetThrottle(f32 v);
    virtual u8 vf2C(void);
    virtual u8 vf30(void);
    virtual u8 vf34(void);
    virtual u8 vf38(void);
    virtual u8 vf3C(void);
    virtual f32 vf40(void);
    virtual f32 vf44(void);
    virtual f32 vf48(void);
    virtual u8 vf4C(void);
    virtual void SetRumbleA(f32 a, f32 b, f32 c);
    virtual void SetRumbleB(f32 a, f32 b, f32 c);
    virtual void SetRumbleC(f32 a, f32 b);
    virtual int vf5C(void);
    virtual int vf60(void);
    virtual int vf64(void);
    virtual u8 vf68(void);
    virtual s32 vf6C(void);
    virtual s32 vf70(void);
    virtual void Buttons(void);
    virtual void SetRumble(u8 on);
    virtual void Stop(void);
    virtual ~PhysDriver(void);
    s32 car;        // offset 0x4
    s32 ch;         // offset 0x8
    f32 steer;      // offset 0xC
    f32 steerAdd;   // offset 0x10
    f32 brake;      // offset 0x14
    f32 brakeAdd;   // offset 0x18
    f32 grip;       // offset 0x1C
    f32 throttle;   // offset 0x20
    f32 throttleAdd;  // offset 0x24
    f32 f28;        // offset 0x28
    f32 steerDead;    // offset 0x2C
    f32 steerOver;    // offset 0x30
    f32 steerSpeed;   // offset 0x34
    f32 thrDead;      // offset 0x38
    f32 thrOver;      // offset 0x3C
    f32 thrSpeed;     // offset 0x40
    f32 brkDead;      // offset 0x44
    f32 brkOver;      // offset 0x48
    f32 brkSpeed;     // offset 0x4C
    s32 maxGear;    // offset 0x50
    s32 gear;       // offset 0x54
    u8 b58;         // offset 0x58
    u8 b59;         // offset 0x59
    u8 b5A;         // offset 0x5A
    u8 b5B;         // offset 0x5B
    u8 b5C;         // offset 0x5C
    u8 b5D;         // offset 0x5D
    u8 b5E;         // offset 0x5E
    u8 pad5F;
    f32 nonlin;     // offset 0x60
    f32 nonlin2;    // offset 0x64
    u8 b68;         // offset 0x68
    u8 pad69[0x6C - 0x69];
    DButton b6C;    // offset 0x6C
    DButton b7C;    // offset 0x7C
    DButton b8C;    // offset 0x8C
    DButton b9C;    // offset 0x9C
    DButton bAC;    // offset 0xAC
    DButton bBC;    // offset 0xBC
    DButton bCC;    // offset 0xCC
    DButton bDC;    // offset 0xDC
    DButton bEC;    // offset 0xEC
    DButton bFC;    // offset 0xFC
    DButton b10C;   // offset 0x10C
    DButton b11C;   // offset 0x11C
    DButton b12C;   // offset 0x12C
    DButton b13C;   // offset 0x13C
    DButton b14C;   // offset 0x14C
    DButton b15C;   // offset 0x15C
    DButton b16C;   // offset 0x16C
    DButton b17C;   // offset 0x17C
    DButton b18C;   // offset 0x18C
    DButton b19C;   // offset 0x19C
    DButton b1AC;   // offset 0x1AC
    DButton b1BC;   // offset 0x1BC
    DButton b1CC;   // offset 0x1CC
    DButton freeG[6];  // offset 0x1DC
    DButton freeRev;   // offset 0x23C
    DButton b24C;   // offset 0x24C
    DButton b25C;   // offset 0x25C
    DButton b26C;   // offset 0x26C
    DButton b27C;   // offset 0x27C
    DButton b28C;   // offset 0x28C
    DButton b29C;   // offset 0x29C
    DButton b2AC;   // offset 0x2AC
    DButton b2BC;   // offset 0x2BC
    DButton dpad[4];  // offset 0x2CC
    f32 rumA[3];    // offset 0x30C
    f32 rumB[3];    // offset 0x318
    f32 rumC[2];    // offset 0x324
    f32 f32C;       // offset 0x32C
    f32 f330;       // offset 0x330
    f32 f334;       // offset 0x334
    u8 b338;        // offset 0x338
    u8 pad339[3];
    f32 f33C;       // offset 0x33C
    u8 forced;      // offset 0x340
    u8 freeShift;   // offset 0x341
    u8 b342;        // offset 0x342
    u8 b343;        // offset 0x343
    u8 b344;        // offset 0x344
    u8 digThrottle;  // offset 0x345
    u8 digBraking;  // offset 0x346
    u8 pad347;
    s32 f348;       // offset 0x348
    s32 f34C;       // offset 0x34C
    u8 rumble;      // offset 0x350
};

ModuleTag lbl_8055E4B0;
u8 lbl_8055E4B8;
u8 lbl_8055E4B9;
u8 lbl_8055E4BA;

PhysDriver* lbl_804348D8[4];

struct Names4 {
    const char* n[4];
};

const Names4 lbl_802F9530 = {{"sel_up", "sel_down", "sel_left", "sel_right"}};
const Names4 lbl_802F9540 = {{"POV Front", "POV Back", "POV Left", "POV Right"}};

extern f32 lbl_8055A334;

extern "C" {

f32 fn_8017EB80(f32 x, f32 dead, f32 over) {
    f32 t = (1.0f + dead + over) * (f32)__fabs(x) - dead;
    t = MIN(1.0f, t);
    t = MAX(t, 0.0f);
    if (x >= 0.0f) {
        return t;
    }
    return -t;
}

int fn_8017EBE0(void) {
    lbl_8055E4B8 = 0;
    lbl_8055E4BA = 1;
    lbl_8055E4B9 = 0;
    lbl_804348D8[0] = 0;
    lbl_804348D8[1] = 0;
    lbl_804348D8[2] = 0;
    lbl_804348D8[3] = 0;
    fn_801808DC();
    return 1;
}

void fn_8017EC30(void) {
    int i;
    for (i = 0; i < 4; i++) {
        fn_802E8524(0, i);
        if (lbl_804348D8[i]) {
            delete lbl_804348D8[i];
            lbl_804348D8[i] = 0;
        }
    }
    lbl_8055E4BA = 0;
}

}  // extern "C"

PhysDriver::~PhysDriver(void) {}

inline f32 Clamp(f32 x, f32 lo, f32 hi) {
    x = MIN(hi, x);
    x = MAX(x, lo);
    return x;
}

inline f32 Max(f32 a, f32 b) { return a > b ? a : b; }

inline f32 Min(f32 a, f32 b) { return a < b ? a : b; }

extern "C" {
f32 fn_8017EB80(f32 x, f32 dead, f32 over);
f32 fn_8017F70C(f32 x);
void fn_8017F628(PhysDriver* d);
}

static inline void UpdateAll(f32 dt) {
    PhysDriver* d;
    int i;
    fn_801A7844(1);
    for (i = 0; i < 4; i++) {
        d = lbl_804348D8[i];
        if (d) {
            u8 dthr;
            u8 dbrk;
            DButton* b;
            DButton* bb;
            if (fn_80137CFC(d->car) != d->ch) {
                fn_802E30C8("driver: controller changed for player %d", d->car);
                d->Init();
            }
            dthr = d->digThrottle;
            dbrk = d->digBraking;
            d->b342 = 0;
            d->b343 = 0;
            {
                s32 c = d->b8C.ch;
                u8 info[0x70];
                if (fn_802E85B0(c) == 4) {
                    d->b343 = 1;
                    dthr = 0;
                    dbrk = 0;
                }
                if (fn_801298E8(info, c)) {
                    if (*(u32*)(info + 0x50) & 2) {
                        d->b343 = 1;
                        dthr = 0;
                        dbrk = 0;
                    }
                    if (*(u32*)(info + 0x50) & 8) {
                        d->b342 = 1;
                        d->b343 = 1;
                    }
                }
            }
            if (d->forced || fn_801A8EE4(&d->b6C) || fn_801A8EE4(&d->b7C)) {
                {
                    f32 rate = 2.5f * d->steerSpeed;
                    f32 ret = 1.9f * rate;
                    f32 tot = ret + rate;
                    if (d->steer > 0.0f) {
                        d->steer = Max(d->steer - ret * dt, 0.0f);
                    }
                    if (d->steer < 0.0f) {
                        d->steer = Min(d->steer + ret * dt, 0.0f);
                    }
                    if (fn_801A8E38(&d->b6C)) {
                        d->steer -= tot * dt;
                    }
                    if (fn_801A8E38(&d->b7C)) {
                        d->steer += tot * dt;
                    }
                    if (d->steer > 1.0f) {
                        d->steer = 1.0f;
                    }
                    if (d->steer < -1.0f) {
                        d->steer = -1.0f;
                    }
                }
            } else {
                f32 prev = d->steer;
                f32 r = fn_801A7F98(&d->b7C);
                f32 l = fn_801A7F98(&d->b6C);
                f32 v = -l + r;
                if (d->b344) {
                    d->steer = fn_8017EB80(v, d->steerDead, d->steerOver);
                } else if (!d->b343) {
                    d->steer = fn_8017EB80(v, d->steerDead, d->steerOver);
                    d->steer = d->steer * (1.0f + d->nonlin * fn_8017F70C(d->steer) - d->nonlin);
                    d->steer = d->steer * (1.0f + d->nonlin2 * fn_8017F70C(d->steer) - d->nonlin2);
                } else {
                    d->steer = v;
                    if (d->b342) {
                        d->steer *= 1.5f;
                    }
                }
                if (lbl_8055E4B9) {
                    d->steer = fn_80144C80(d->steer, prev, 0.4f);
                }
            }
            {
                f32 rate = 2.5f * d->steerSpeed;
                f32 ret = 1.9f * rate;
                f32 tot = ret + rate;
                if (d->steerAdd > 0.0f) {
                    d->steerAdd = Max(d->steerAdd - ret * dt, 0.0f);
                }
                if (d->steerAdd < 0.0f) {
                    d->steerAdd = Min(d->steerAdd + ret * dt, 0.0f);
                }
                if (fn_801A8E38(&d->b18C)) {
                    d->steerAdd -= tot * dt;
                }
                if (fn_801A8E38(&d->b19C)) {
                    d->steerAdd += tot * dt;
                }
                if (d->steerAdd > 1.0f) {
                    d->steerAdd = 1.0f;
                }
                if (d->steerAdd < -1.0f) {
                    d->steerAdd = -1.0f;
                }
            }
            b = &d->b8C;
            if (!fn_801A8F10(&d->b8C)) {
                b = &d->bBC;
            } else if (dthr && fn_801A8F10(&d->bBC)) {
                b = &d->bBC;
            }
            if (d->forced || dthr || fn_801A8EE4(b)) {
                {
                    f32 up = 6.0f * d->thrSpeed;
                    f32 down = 1.5f * up;
                    if (fn_801A8E38(b)) {
                        d->throttle = Min(d->throttle + up * dt, 1.0f);
                    } else {
                        d->throttle = Max(d->throttle - down * dt, 0.0f);
                    }
                }
            } else {
                d->throttle = fn_8017EB80(fn_801A7F98(b), d->thrDead, d->thrOver);
            }
            if (d->forced || fn_801A8EE4(&d->bDC)) {
                {
                    f32 up = 6.0f * d->thrSpeed;
                    f32 down = 1.5f * up;
                    if (fn_801A8E38(&d->bDC)) {
                        d->throttleAdd = Min(d->throttleAdd + up * dt, 1.0f);
                    } else {
                        d->throttleAdd = Max(d->throttleAdd - down * dt, 0.0f);
                    }
                }
            } else {
                d->throttleAdd = fn_8017EB80(fn_801A7F98(&d->bDC), d->thrDead, d->thrOver);
            }
            bb = &d->b9C;
            if (!fn_801A8F10(&d->b9C)) {
                bb = &d->bCC;
            } else if (dbrk && fn_801A8F10(&d->bCC)) {
                bb = &d->bCC;
            }
            if (d->forced || dbrk || fn_801A8EE4(bb)) {
                {
                    f32 up = 12.0f * d->brkSpeed;
                    f32 down = 6.0f * d->brkSpeed;
                    if (d->brake > 0.0f) {
                        d->brake = Max(d->brake - down * dt, 0.0f);
                    }
                    if (fn_801A8E38(bb)) {
                        d->brake += up * dt;
                    }
                    if (d->brake > 1.0f) {
                        d->brake = 1.0f;
                    }
                }
            } else {
                d->brake = fn_8017EB80(fn_801A7F98(bb), d->brkDead, d->brkOver);
            }
            if (d->forced || fn_801A8EE4(&d->bEC)) {
                {
                    f32 up = 12.0f * d->brkSpeed;
                    f32 down = 6.0f * d->brkSpeed;
                    if (d->brakeAdd > 0.0f) {
                        d->brakeAdd = Max(d->brakeAdd - down * dt, 0.0f);
                    }
                    if (fn_801A8E38(&d->bEC)) {
                        d->brakeAdd += up * dt;
                    }
                    if (d->brakeAdd > 1.0f) {
                        d->brakeAdd = 1.0f;
                    }
                }
            } else {
                d->brakeAdd = fn_8017EB80(fn_801A7F98(&d->bEC), d->brkDead, d->brkOver);
            }
            d->f28 = fn_801A7F98(&d->bAC);
            if (!d->freeShift) {
                u8 rev = 0;
                if (fn_801A8E38(&d->bFC) || fn_801A8E38(&d->b10C)) {
                    rev = 1;
                }
                d->b5A = rev;
            }
            if (fn_801A8EE4(&d->b15C)) {
                if (fn_801A8E38(&d->b15C)) {
                    d->grip += 10.0f * dt;
                } else if (d->grip > 0.0f) {
                    d->grip = Max(d->grip - 5.0f * dt, 0.0f);
                }
                if (d->grip > 1.0f) {
                    d->grip = 1.0f;
                }
            } else {
                d->grip = fn_801A7F98(&d->b15C);
            }
            d->b5B = fn_801A8E38(&d->b16C);
            {
                u8 horn = 0;
                if (fn_801A8E38(&d->b11C) || fn_801A8E38(&d->b12C)) {
                    horn = 1;
                }
                d->b58 = horn;
            }
            d->b5D = fn_801A8E68(&d->b8C);
            if (fn_80179300() < 40) {
                d->b58 = 0;
            }
            d->b68 = fn_801A8E38(&d->b17C);
            {
                f32 l = fn_801A7F98(&d->b1AC);
                f32 v = fn_8017EB80(-l + fn_801A7F98(&d->b1BC), 0.4f, 0.07f);
                f32 k = 4.0f * dt;
                d->f32C += Clamp(v - d->f32C, -k, k);
            }
            d->b338 = fn_801A8E38(&d->b1CC);
            if (fn_801A8E38(&d->b27C)) {
                d->b338 = 1;
            }
            {
                f32 a = fn_801A7F98(&d->b28C);
                d->f330 = fn_801A7F98(&d->b29C) - a;
            }
            {
                f32 a = fn_801A7F98(&d->b2AC);
                d->f334 = fn_801A7F98(&d->b2BC) - a;
            }
            if (!d->freeShift) {
                if (fn_801A8E38(&d->b13C)) {
                    if (!d->b59) {
                        if (d->maxGear < d->gear) {
                            d->maxGear++;
                        }
                        d->b59 = 1;
                    }
                } else if (fn_801A8E38(&d->b14C)) {
                    if (!d->b59) {
                        if (d->maxGear > -1) {
                            d->maxGear--;
                        }
                        d->b59 = 1;
                    }
                } else {
                    d->b59 = 0;
                }
            } else {
                int k;
                d->b59 = 0;
                d->maxGear = 0;
                for (k = 0; k < d->gear; k++) {
                    if (fn_801A8E38(&d->freeG[k])) {
                        d->maxGear = k + 1;
                    }
                }
                if (fn_801A8E38(&d->freeRev)) {
                    d->maxGear = -1;
                }
            }
            fn_8017F628(d);
        }
    }
}

extern "C" void fn_8017ED00(f32 dt) {
    if (!lbl_8055E4B8) {
        fn_802E317C("I don't expect PhysDriverUpdate (fg) to be used");
        UpdateAll(dt);
    }
}


static inline f32 Abs(f32 x) { return (f32)__fabs(x); }

extern "C" {

void fn_8017F628(PhysDriver* d) {
    f32 k = lbl_8055A334;
    if (!fn_801769F8() && !fn_801B526C() && !fn_801C5B60() && d->b5C && d->rumble) {
        fn_802E85AC(d->ch, d->rumA[2] + d->rumA[0] * k, d->rumA[1] * k);
        fn_802E85A8(d->ch, d->rumB[0] * k, d->rumB[1], d->rumB[2] * k);
        fn_802E8528(d->ch, 0.4f * Abs(d->rumC[0] * k), Abs(d->rumC[1]));
    } else {
        fn_802E8518(d->ch);
    }
}

f32 fn_8017F70C(f32 x) {
    double r = __fabs(x);
    return r;
}

}  // extern "C"


extern f32 lbl_8055A330;
extern f32 lbl_8055A338;
extern f32 lbl_8055A33C;
extern f32 lbl_8055A340;
extern f32 lbl_8055A344;

inline void PhysDriver::Init(void) {
    int k;
    u8 pedals;
    u8 info[0x60];
    char buf[0x40];
    ch = fn_80137CFC(car);
    steerDead = 0.05f;
    thrDead = 0.05f;
    brkDead = 0.15f;
    steerSpeed = 1.0f;
    thrSpeed = 0.9f;
    brkSpeed = 2.0f;
    steerOver = 0.1f;
    thrOver = 0.5f;
    brkOver = 0.1f;
    forced = 0;
    freeShift = 0;
    fn_801331C4((void*)f34C, "steer_dead_spot", &steerDead);
    fn_801331C4((void*)f34C, "throttle_dead_spot", &thrDead);
    fn_801331C4((void*)f34C, "brake_dead_spot", &brkDead);
    fn_801331C4((void*)f34C, "steer_overshoot", &steerOver);
    fn_801331C4((void*)f34C, "throttle_overshoot", &thrOver);
    fn_801331C4((void*)f34C, "brake_overshoot", &brkOver);
    fn_801331C4((void*)f34C, "steer_speed", &steerSpeed);
    fn_801331C4((void*)f34C, "throttle_speed", &thrSpeed);
    fn_801331C4((void*)f34C, "brake_speed", &brkSpeed);
    fn_801331C4((void*)f34C, "nonlinear_steering", &nonlin);
    b5C = 1;
    fn_801333E8((void*)f34C, "game_pad", &forced);
    fn_801333E8((void*)f34C, "free_shift", &freeShift);
    b5E = 1;
    nonlin2 = Clamp(nonlin - 1.0f, 0.0f, 1.0f);
    nonlin = Clamp(nonlin, 0.0f, 1.0f);
    steerSpeed = Max(steerSpeed, 0.02f);
    thrSpeed = Max(thrSpeed, 0.02f);
    brkSpeed = Max(brkSpeed, 0.02f);
    if (vf5C()) {
        nonlin = 0.0f;
    }
    fn_802E8524(b5C, ch);
    {
        Names4 sel = lbl_802F9530;
        Names4 pov = lbl_802F9540;
        for (k = 0; k < 4; k++) {
            fn_801A7DFC(sel.n[k], pov.n[k], (void*)f34C);
            fn_801A7974(&dpad[k], sel.n[k], (void*)f34C);
        }
    }
    fn_801A7974((&b6C), "steer_left", (void*)f34C);
    fn_801A7974((&b7C), "steer_right", (void*)f34C);
    fn_801A7974((&b18C), "steer2_left", (void*)f34C);
    fn_801A7974((&b19C), "steer2_right", (void*)f34C);
    fn_801A7974((&b8C), "throttle", (void*)f34C);
    fn_801A7974((&b9C), "braking", (void*)f34C);
    fn_801A7974((&bBC), "dig_throttle", (void*)f34C);
    fn_801A7974((&bCC), "dig_braking", (void*)f34C);
    fn_801A7974((&bDC), "throttle2", (void*)f34C);
    fn_801A7974((&bEC), "braking2", (void*)f34C);
    fn_801A7974((&b24C), "view", (void*)f34C);
    fn_801A7974((&b25C), "race_info", (void*)f34C);
    fn_801A7974((&b26C), "car_info", (void*)f34C);
    fn_801A7974((&b27C), "mirror_control", (void*)f34C);
    fn_801A7974((&bAC), "e_brake", (void*)f34C);
    fn_801A7974((&bFC), "reverse", (void*)f34C);
    fn_801A7974((&b10C), "alt_reverse", (void*)f34C);
    fn_801A7974((&b15C), "clutch", (void*)f34C);
    fn_801A7974((&b13C), "upshift", (void*)f34C);
    fn_801A7974((&b14C), "downshift", (void*)f34C);
    fn_801A7974((&b16C), "help", (void*)f34C);
    fn_801A7974((&b17C), "airlift", (void*)f34C);
    fn_801A7974((&b11C), "horn", (void*)f34C);
    fn_801A7974((&b12C), "horn2", (void*)f34C);
    fn_801A7974((&b1AC), "look_left", (void*)f34C);
    fn_801A7974((&b1BC), "look_right", (void*)f34C);
    fn_801A7974((&b1CC), "look_back", (void*)f34C);
    fn_801A7974((&b28C), "pitch_up", (void*)f34C);
    fn_801A7974((&b29C), "pitch_down", (void*)f34C);
    fn_801A7974((&b2AC), "roll_left", (void*)f34C);
    fn_801A7974((&b2BC), "roll_right", (void*)f34C);
    fn_801331C4((void*)f34C, "ff_deadband_width", &lbl_8055A330);
    fn_801331C4((void*)f34C, "ff_torque_gain", &lbl_8055A334);
    fn_801331C4((void*)f34C, "ff_inband_damping", &lbl_8055A338);
    fn_801331C4((void*)f34C, "ff_outband_damping", &lbl_8055A33C);
    fn_801331C4((void*)f34C, "ff_inband_spring", &lbl_8055A340);
    fn_801331C4((void*)f34C, "ff_outband_spring", &lbl_8055A344);
    freeShift = 0;
    fn_801333E8((void*)f34C, "free_shift", &freeShift);
    digBraking = 0;
    digThrottle = 1;
    fn_801333E8((void*)f34C, "digital_braking", &digBraking);
    fn_801333E8((void*)f34C, "digital_throttle", &digThrottle);
    pedals = 0;
    fn_801333E8((void*)f34C, "has_pedals", &pedals);
    if (pedals) {
        fn_802E30C8("has_pedals");
        digBraking = 0;
        digThrottle = 0;
    }
    fn_801A9110((&freeG[0]), "Button 6");
    fn_801A9110((&freeG[1]), "Button 7");
    fn_801A9110((&freeG[2]), "Button 1");
    fn_801A9110((&freeG[3]), "Button 2");
    fn_801A9110((&freeG[4]), "POV Front");
    fn_801A9110((&freeG[5]), "POV Left");
    fn_801A9110((&freeRev), "POV Right");
    {
        int j;
        for (j = 0; j < 6; j++) {
            sprintf(buf, "free_g%d", j + 1);
            fn_801A7974((&freeG[j]), buf, (void*)f34C);
        }
    }
    fn_801A7974((&freeRev), "free_reverse", (void*)f34C);
    {
        s32 c = ch;
        b6C.ch = c;
        b7C.ch = c;
        b18C.ch = c;
        b19C.ch = c;
        b8C.ch = c;
        b9C.ch = c;
        bAC.ch = c;
        bDC.ch = c;
        bEC.ch = c;
        bBC.ch = c;
        bCC.ch = c;
        bFC.ch = c;
        b10C.ch = c;
        b13C.ch = c;
        b14C.ch = c;
        b15C.ch = c;
        b11C.ch = c;
        b12C.ch = c;
        b17C.ch = c;
        b24C.ch = c;
        b25C.ch = c;
        b26C.ch = c;
        b27C.ch = c;
        b16C.ch = c;
        b1AC.ch = c;
        b1BC.ch = c;
        b1CC.ch = c;
        b28C.ch = c;
        b29C.ch = c;
        b2AC.ch = c;
        b2BC.ch = c;
        dpad[0].ch = c;
        dpad[1].ch = c;
        dpad[2].ch = c;
        dpad[3].ch = c;
    }
    b344 = 0;
    if (fn_801298E8(info, ch) && (*(u32*)(info + 0x50) & 0x10)) {
        b344 = 1;
    }
    b344 = 1;
}

int PhysDriver::vf5C(void) {
    int r = 0;
    if (forced || fn_801A8EE4(&b6C) || fn_801A8EE4(&b7C)) {
        r = 1;
    }
    return r;
}

extern "C" void fn_8017FF4C(f32 dt) {
    if (lbl_8055E4B8) {
        if (!fn_801769F8() && !fn_801B526C()) {
            UpdateAll(dt);
        } else {
            int i;
            for (i = 0; i < 4; i++) {
                PhysDriver* d = lbl_804348D8[i];
                if (d) {
                    d->rumC[0] = 0.0f;
                    d->rumC[1] = 0.0f;
                    d->rumA[2] = 0.0f;
                    d->rumA[1] = 0.0f;
                    d->rumA[0] = 0.0f;
                    d->rumB[2] = 0.0f;
                    d->rumB[1] = 0.0f;
                    d->rumB[0] = 0.0f;
                    if (d->ch >= 0) {
                        fn_802E8518(d->ch);
                    }
                }
            }
        }
    }
}


DButton::DButton(s32 c) {
    ch = c;
    bC = 0;
}

DriverInterface::DriverInterface(void) {}

void DriverInterface::Buttons(void) {}

void DriverInterface::SetRumble(u8 on) {}

void DriverInterface::Stop(void) {}

s32 PhysDriver::vf70(void) { return f34C; }

s32 PhysDriver::vf6C(void) { return f348; }

void PhysDriver::SetRumbleA(f32 a, f32 b, f32 c) {
    rumA[0] = a;
    rumA[1] = b;
    rumA[2] = c;
}

void PhysDriver::SetRumbleB(f32 a, f32 b, f32 c) {
    rumB[0] = a;
    rumB[1] = b;
    rumB[2] = c;
}

void PhysDriver::SetRumbleC(f32 a, f32 b) {
    rumC[0] = a;
    rumC[1] = b;
}

void PhysDriver::SetGear(int g) {
    gear = g;
    if (maxGear > g) {
        maxGear = g;
    }
}

f32 PhysDriver::Steer(void) {
    f32 hi = MAX(steer, 1.0f);
    f32 lo = MIN(steer, -1.0f);
    hi = MIN(hi, steer + steerAdd);
    hi = MAX(hi, lo);
    if (vf5C()) {
        hi = steer;
    }
    return hi;
}

f32 PhysDriver::Throttle(void) { return MIN(throttle + throttleAdd, 1.0f); }

f32 PhysDriver::Brake(void) { return MIN(brake + brakeAdd, 1.0f); }

f32 PhysDriver::Grip(void) {
    f32 t = 1.1f - 1.2f * grip;
    t = MAX(0.0f, t);
    return MIN(1.0f, t);
}

f32 PhysDriver::vf1C(void) { return f28; }

s32 PhysDriver::MaxGear(void) { return maxGear; }

void PhysDriver::SetMaxGear(s32 g) { maxGear = g; }

void PhysDriver::SetThrottle(f32 v) {
    v = MIN(1.0f, v);
    v = MAX(v, 0.0f);
    throttle = v;
}

void PhysDriver::Buttons(void) {
    int i;
    if (fn_801A8E68(&b25C)) {
        fn_801797F8(car, 2, 0);
    }
    if (fn_801A8E68(&b26C)) {
        fn_801797F8(car, 1, 0);
    }
    if (fn_801A8E68(&b27C)) {
        fn_801797F8(car, 3, 0);
    }
    if (fn_801A8E68(&b24C)) {
        fn_801797F8(car, 0, 0);
    }
    for (i = 0; i < 4; i++) {
        if (fn_801A8E68(&dpad[i])) {
            fn_801797F8(car, 4, i);
            break;
        }
    }
}

void PhysDriver::SetRumble(u8 on) {
    rumble = on;
    if (!on && ch >= 0) {
        fn_802E8518(ch);
    }
}

void PhysDriver::Stop(void) {
    if (ch >= 0 && fn_8026A368(car) != 0x11) {
        fn_802E85F4(ch);
    }
}

u8 PhysDriver::vf2C(void) { return b58; }

u8 PhysDriver::vf30(void) { return b5D; }

u8 PhysDriver::vf3C(void) { return b68; }

u8 PhysDriver::vf34(void) { return b5A; }

u8 PhysDriver::vf38(void) { return b5B; }

f32 PhysDriver::vf40(void) { return f32C; }

f32 PhysDriver::vf44(void) { return f330; }

f32 PhysDriver::vf48(void) { return f334; }

u8 PhysDriver::vf4C(void) { return b338; }

int PhysDriver::vf60(void) {
    int r = 0;
    if (forced || fn_801A8EE4(&b9C)) {
        r = 1;
    }
    return r;
}

int PhysDriver::vf64(void) {
    int r = 0;
    if (forced || fn_801A8EE4(&b8C)) {
        r = 1;
    }
    return r;
}

u8 PhysDriver::vf68(void) { return b344; }

extern "C" void fn_801808DC(void) {
    int i;
    fn_801333E8((void*)8, "smoothing", &lbl_8055E4B9);
    lbl_8055E4B8 = 1;
    for (i = 0; i < 4; i++) {
        PhysDriver* d = lbl_804348D8[i];
        if (d) {
            d->Init();
        }
    }
}

inline PhysDriver::PhysDriver(int i) {
    car = i;
    ch = fn_80137CFC(i);
    rumble = 0;
    if (fn_8027AA48()) {
        f348 = 0x10;
    } else {
        f348 = i + 0xC;
    }
    f34C = ch + 8;
    f33C = 0.0f;
    b5B = 0;
    b5C = 0;
    b5E = 1;
    nonlin = 1.0f;
    steerDead = 0.0f;
    steerSpeed = 1.0f;
    steerOver = 0.1f;
    thrDead = 0.1f;
    thrSpeed = 1.0f;
    thrOver = 0.1f;
    brkDead = 0.1f;
    brkSpeed = 1.0f;
    brkOver = 0.1f;
    maxGear = 0;
    gear = 0;
    steer = 0.0f;
    steerAdd = 0.0f;
    brake = 0.0f;
    throttle = 0.0f;
    brakeAdd = 0.0f;
    throttleAdd = 0.0f;
    grip = 0.0f;
    f28 = 0.0f;
    b58 = 0;
    b5D = 0;
    b68 = 0;
    b59 = 0;
    b5A = 0;
    rumA[2] = 0.0f;
    rumA[1] = 0.0f;
    rumA[0] = 0.0f;
    rumB[2] = 0.0f;
    rumB[1] = 0.0f;
    rumB[0] = 0.0f;
    rumC[1] = 0.0f;
    rumC[0] = 0.0f;
    f32C = 0.0f;
    b338 = 0;
    forced = 0;
    freeShift = 0;
    b342 = 0;
    b343 = 0;
    b344 = 0;
    Init();
}

extern "C" PhysDriver* fn_8018108C(int i) {
    if (!lbl_804348D8[i]) {
        lbl_804348D8[i] = new ("DriverInterface") PhysDriver(i);
    }
    return lbl_804348D8[i];
}
