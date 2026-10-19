// Physics driver input (0x8017EB80-0x8018185C): the per-player DriverInterface objects that turn controller
// input (dead spots, overshoot, smoothing) into steering/throttle/brake for the physics, plus rumble and
// the button event messages. Names are guesses apart from the strings. FIRST DRAFT: the five large update
// and config functions (fn_8017ED00, fn_8017F718, fn_8017FF4C, fn_801808DC, fn_8018108C) are not written.
#include "game/common.h"

extern "C" {
void fn_802E8518(int ch);
void fn_802E8524(int a, int ch);
void fn_802E8528(int ch, f32 a, f32 b);
void fn_802E85A8(int ch, f32 a, f32 b, f32 c);
void fn_802E85AC(int ch, f32 a, f32 b);
void fn_802E85F4(int ch);
u8 fn_801769F8(void);
int fn_801B526C(void);
int fn_801C5B60(void);
int fn_801A8EE4(void* b);  // button held
int fn_801A8E68(void* b);  // button pressed
void fn_801797F8(int car, u32 kind, u32 idx);
s32 fn_8026A368(int car);
void fn_801808DC(void);
}

struct DButton {
    u8 pad0[0x10];
};

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
    u8 pad2C[0x50 - 0x2C];
    s32 maxGear;    // offset 0x50
    s32 gear;       // offset 0x54
    u8 b58;         // offset 0x58
    u8 pad59;
    u8 b5A;         // offset 0x5A
    u8 b5B;         // offset 0x5B
    u8 b5C;         // offset 0x5C
    u8 b5D;         // offset 0x5D
    u8 pad5E[0x68 - 0x5E];
    u8 b68;         // offset 0x68
    u8 pad69[0x6C - 0x69];
    DButton b6C;    // offset 0x6C
    DButton b7C;    // offset 0x7C
    DButton b8C;    // offset 0x8C
    DButton b9C;    // offset 0x9C
    u8 padAC[0x24C - 0xAC];
    DButton b24C;   // offset 0x24C
    DButton b25C;   // offset 0x25C
    DButton b26C;   // offset 0x26C
    DButton b27C;   // offset 0x27C
    u8 pad28C[0x2CC - 0x28C];
    DButton dpad[4];  // offset 0x2CC
    f32 rumA[3];    // offset 0x30C
    f32 rumB[3];    // offset 0x318
    f32 rumC[2];    // offset 0x324
    f32 f32C;       // offset 0x32C
    f32 f330;       // offset 0x330
    f32 f334;       // offset 0x334
    u8 b338;        // offset 0x338
    u8 pad339[0x340 - 0x339];
    u8 forced;      // offset 0x340
    u8 pad341[3];
    u8 b344;        // offset 0x344
    u8 pad345[3];
    s32 f348;       // offset 0x348
    s32 f34C;       // offset 0x34C
    u8 rumble;      // offset 0x350
};

ModuleTag lbl_8055E4B0;
u8 lbl_8055E4B8;
u8 lbl_8055E4B9;
u8 lbl_8055E4BA;

PhysDriver* lbl_804348D8[4];

const char* const lbl_802F9530[4] = {"sel_up", "sel_down", "sel_left", "sel_right"};
const char* const lbl_802F9540[4] = {"POV Front", "POV Back", "POV Left", "POV Right"};

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

extern "C" {

void fn_8017F628(PhysDriver* d) {
    f32 k = lbl_8055A334;
    if (!fn_801769F8() && !fn_801B526C() && !fn_801C5B60() && d->b5C && d->rumble) {
        fn_802E85AC(d->ch, d->rumA[2] + d->rumA[0] * k, d->rumA[1] * k);
        fn_802E85A8(d->ch, d->rumB[0] * k, d->rumB[1], d->rumB[2] * k);
        fn_802E8528(d->ch, 0.4f * (f32)__fabs(d->rumC[0] * k), (f32)__fabs(d->rumC[1]));
    } else {
        fn_802E8518(d->ch);
    }
}

f32 fn_8017F70C(f32 x) { return (f32)__fabs(x); }

}  // extern "C"

int PhysDriver::vf5C(void) {
    int r = 0;
    if (forced || fn_801A8EE4(&b6C) || fn_801A8EE4(&b7C)) {
        r = 1;
    }
    return r;
}


extern "C" void fn_80181418(u8* p, s32 a) {
    *(s32*)(p + 4) = a;
    p[0xC] = 0;
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
