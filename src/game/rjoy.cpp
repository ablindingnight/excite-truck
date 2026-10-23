// Remote input ("rjoy", 0x802E77B8-0x802E86D4): per-socket Wii Remote state on top of KPAD/WPAD. It polls
// the remotes every 15 ms, tracks connect/disconnect and probe errors, turns the remote's tilt into a
// steering value and an analogue throttle, keeps a short history of the tilt angle, drives the rumble motor
// with a PWM accumulator and watches the radio sensitivity. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
void* memset(void* dst, int c, size_t n);
double atan2(double y, double x);
void fn_802E30C8(const char* fmt, ...);
s32 fn_802E2C20(void);  // milliseconds
void fn_802D9BF0(void);
u8 fn_802E9A24(void);
void fn_802E2E0C(int (*cb)(int, int), const char* name);
int fn_802ECC10(void);
void fn_802ECBF8(void);
void fn_802E2E38(int a);
void fn_802EC884(void);
void fn_802E6B44(s32 chan, s32 result);
void fn_801282EC(int a);
void fn_801282F4(void);
void* fn_801284E8(const char* tag, u32 size);
void fn_80128658(void* p);
void fn_80128684(void);
void fn_800485AC(s32 chan);
void fn_800484E4(s32 chan);
s32 fn_80047C0C(s32 chan, void* status, u32 n);  // KPADRead

typedef void (*WPADConnectCallback)(s32 chan, s32 result);
typedef void (*WPADInfoCallback)(s32 chan, s32 result);
void WPADRegisterAllocator(void* (*alloc)(u32), int (*free)(void*));
void PADInit(void);
void KPADInit(void);
s32 WPADGetStatus(void);
WPADConnectCallback WPADSetConnectCallback(s32 chan, WPADConnectCallback cb);
void WPADDisconnect(s32 chan);
void __WPADShutdown(void);
void __WPADReconnect(void);
s32 WPADProbe(s32 chan, s32* type);
s32 WPADGetInfoAsync(s32 chan, void* info, WPADInfoCallback cb);
void WPADControlMotor(s32 chan, u32 cmd);
u8 WPADGetRadioSensitivity(s32 chan);
}

// One KPAD status record (only the fields this unit reads).
struct KStatus {
    u32 hold;     // offset 0x0
    u32 trig;     // offset 0x4
    u32 release;  // offset 0x8
    f32 accX;     // offset 0xC
    f32 accY;     // offset 0x10
    f32 accZ;     // offset 0x14
    u8 pad18[0x5C - 0x18];
    s32 devType;  // offset 0x5C
    u8 pad60[0x85 - 0x60];
    s8 err;       // offset 0x85
    u8 pad86[0x88 - 0x86];
};

// One remote socket: 0x638 bytes.
struct Remote {
    u8 connected;   // offset 0x0
    u8 pad1[3];
    f32 steer;      // offset 0x4
    f32 throttle;   // offset 0x8
    u8 padC[0x24 - 0xC];
    u8 b24;         // offset 0x24: buttons
    u8 b25;
    u8 b26;
    u8 b27;
    u8 b28;
    u8 b29;
    u8 b2A;
    u8 b2B;
    u8 pad2C[0x44 - 0x2C];
    u32 dir;        // offset 0x44
    u32 a48;        // offset 0x48
    u32 a4C;        // offset 0x4C
    u8 pad50[0x54 - 0x50];
    u32 flags;      // offset 0x54
    f32 acc[3];     // offset 0x58
    u8 isExt;       // offset 0x64
    u8 pad65[3];
    s32 histPos;    // offset 0x68
    f32 angle;      // offset 0x6C
    f32 hist[16];   // offset 0x70
    u16 rumble;     // offset 0xB0
    u8 padB2[2];
    s32 pwm;        // offset 0xB4
    s32 lastOn;     // offset 0xB8
    s32 lastOff;    // offset 0xBC
    s32 lastHome;   // offset 0xC0
    u8 present;     // offset 0xC4
    u8 home;        // offset 0xC5
    u8 padC6[2];
    KStatus kpad[10];  // offset 0xC8
    u8 info[0x18];  // offset 0x618
    u8 sens;        // offset 0x630
    u8 infoBusy;    // offset 0x631
    u8 pad632[2];
    s32 lastInfo;   // offset 0x634
};

ModuleTag lbl_80560000;
u8 lbl_80560008;   // motor allowed
u8 lbl_80560009 = 0;  // last pause state
s32 lbl_8056000C;  // allocations
s32 lbl_80560010;  // last poll
s32 lbl_80560014;  // polls
Remote lbl_80505978[4];
void* lbl_80507258[4];
WPADConnectCallback lbl_80507268[4];
s32 lbl_80507278[4];
s8 lbl_8055D230[8] = {6, 6, 6, 6};


extern "C" {

int fn_802E77B8(s32 chan) { return 0; }

const char* fn_802E77C0(s32 err) {
    switch (err) {
    case 0:
        return "NONE";
    case -1:
        return "NO_CONTROLLER";
    case -2:
        return "BUSY";
    case -3:
        return "TRANSFER";
    case -4:
        return "INVALID";
    default:
        return "???";
    }
}

f32 fn_802E7824(f32 x, f32 dead) {
    f32 k = 1.0f / (1.0f - dead);
    if (x >= 0.0f) {
        f32 v = k * (x - dead);
        f32 r = 1.5f;
        if (r >= v) {
            r = v;
        }
        if (r > 0.0f) {
            return r;
        }
        return 0.0f;
    }
    f32 v = k * (x + dead);
    f32 r = 0.0f;
    if (r >= v) {
        r = v;
    }
    if (r <= -1.5f) {
        r = -1.5f;
    }
    return r;
}

f32 fn_802E78A4(f32 x) {
    f32 a = fn_802E7824(0.05f + x, 0.1f);
    f32 v = 0.5f * (fn_802E7824(x - 0.05f, 0.1f) + a);
    return v + 0.0f * ((f32)__fabs(v) * v - v);
}

void fn_802E7920(s32 chan, s32 a) {
    Remote* r = &lbl_80505978[chan];
    s32 now = fn_802E2C20();
    fn_802D9BF0();
    int off = 0;
    if (r->rumble < 0x5000) {
        if (now - r->lastOff > 1000) {
            r->lastOn = now;
        }
    } else {
        r->lastOff = now;
        if (now - r->lastOn > 5000) {
            off = 1;
        }
    }
    if (!lbl_80560008) {
        off = 1;
    }
    u32 cmd;
    if (!off) {
        r->pwm += r->rumble;
        cmd = r->pwm >= 0x8000;
        if (cmd) {
            r->pwm -= 0x10000;
        }
    } else {
        cmd = 0;
        r->pwm = 0;
    }
    WPADControlMotor(chan, cmd);
}

void* fn_802E7A14(u32 size) {
    fn_801282EC(0x80);
    fn_801282F4();
    void* p = fn_801284E8("wpad", size);
    lbl_80507258[lbl_8056000C++] = p;
    return p;
}

int fn_802E7A70(void* p) {
    int k = -1;
    for (int i = 0; i < lbl_8056000C; i++) {
        if (lbl_80507258[i] == p) {
            k = i;
        }
    }
    lbl_80507258[k] = lbl_80507258[--lbl_8056000C];
    fn_80128658(p);
    return 1;
}

int fn_802E7AF4(int phase, int mode) {
    if (phase == 1) {
        fn_80128684();
        if (mode != 2) {
            __WPADReconnect();
        } else {
            __WPADShutdown();
        }
        fn_802E30C8("rjoy: waiting for WPAD to shut down and reconnect");
        while (WPADGetStatus() == 4) {
        }
        fn_802E30C8("rjoy: WPAD has shut itself down");
    }
    return 0;
}

void fn_802E7B6C(s32 chan, s32 result) {
    if (chan < 0 || chan >= 4) {
        return;
    }
    if (lbl_80507268[chan]) {
        lbl_80507268[chan](chan, result);
    }
    Remote* r = &lbl_80505978[chan];
    if (result == -1) {
        r->present = 0;
    } else if (result == 0) {
        r->present = 1;
    }
    fn_802E6B44(chan, result);
}

void fn_802E7C14(s32 chan, s32 result) {
    if (chan >= 0 && chan < 4) {
        lbl_80505978[chan].infoBusy = 0;
    }
}

void fn_802E7C40(void) { lbl_80560008 = fn_802E9A24(); }

void fn_802E7C64(void) {
    WPADRegisterAllocator(fn_802E7A14, fn_802E7A70);
    PADInit();
    KPADInit();
    while (WPADGetStatus() != 3) {
    }
    for (int i = 0; i < 4; i++) {
        lbl_80507268[i] = WPADSetConnectCallback(i, fn_802E7B6C);
    }
    fn_802E2E0C(fn_802E7AF4, "Joy");
    fn_802E7C40();
    lbl_80560009 = 1;
}

void fn_802E7D0C(void) {
    int i;
    for (i = 0; i < 4; i++) {
        WPADSetConnectCallback(i, lbl_80507268[i]);
    }
    for (i = 0; i < 4; i++) {
        WPADDisconnect(i);
    }
    __WPADShutdown();
    fn_802E30C8("rjoy: waiting for WPAD to shut down");
    while (WPADGetStatus() == 4) {
    }
    fn_802E30C8("rjoy: WPAD has shut itself down");
    for (i = 0; i < lbl_8056000C; i++) {
        fn_80128658(lbl_80507258[i]);
    }
    lbl_8056000C = 0;
}

static inline f32 Clamp1(f32 x) {
    f32 r = 1.0f;
    if (r >= x) {
        r = x;
    }
    if (r <= -1.0f) {
        r = -1.0f;
    }
    return r;
}

void fn_802E7DDC(Remote* r, const KStatus* k) {
    u32 hold = k->hold;
    r->a48 = 0x800F;
    r->isExt = *(s32*)(r->info + 0xC) != 0;
    r->a4C = 0xFF;
    r->flags |= 0x10;
    u32 d = 0;
    if (hold & 2) {
        d |= 1;
    }
    if (hold & 1) {
        d |= 4;
    }
    if (hold & 8) {
        d |= 8;
    }
    if (hold & 4) {
        d |= 2;
    }
    r->dir = d;
    f32 ang = -(f32)atan2(-k->accY, k->accX);
    r->angle = ang;
    if (ang > 3.1415927f) {
        ang -= 6.2831855f;
    } else if (ang < -3.1415927f) {
        ang += 6.2831855f;
    }
    r->steer = fn_802E78A4(Clamp1(-2.0f * k->accZ));
    r->throttle = Clamp1(0.16666667f * (-8.0f * ang / 3.1415927f));
    r->b25 = (hold >> 9) & 1;
    r->b24 = (hold >> 8) & 1;
    r->b28 = (hold >> 11) & 1;
    r->b26 = (hold >> 10) & 1;
    r->b2B = (hold >> 4) & 1;
    r->b27 = (hold >> 12) & 1;
    r->b29 = (hold >> 15) & 1;
    r->flags |= 0x20;
    r->acc[0] = 0.01f * -k->accZ;
    r->acc[1] = 0.01f * k->accY;
    r->acc[2] = 0.01f * -k->accX;
}

void fn_802E7FD0(void) {
    s32 now = fn_802E2C20();
    if (lbl_80560010 < now && now - lbl_80560010 < 15) {
        return;
    }
    lbl_80560010 = now;
    lbl_80560014++;
    u32 paused = fn_802ECC10();
    if (paused != lbl_80560009) {
        lbl_80560009 = paused;
        for (int i = 0; i < 4; i++) {
            if (paused) {
                fn_800485AC(i);
            } else {
                fn_800484E4(i);
            }
        }
    }
    Remote* r = lbl_80505978;
    s32* type = lbl_80507278;
    s8* last = lbl_8055D230;
    for (int chan = 0; chan < 4; chan++, r++, type++, last++) {
        memset(r->kpad, 0, sizeof(r->kpad));
        int busy = 0;
        if (lbl_80505978[0].infoBusy) {
            busy = 1;
        }
        if (lbl_80505978[1].infoBusy) {
            busy = 1;
        }
        if (lbl_80505978[2].infoBusy) {
            busy = 1;
        }
        if (lbl_80505978[3].infoBusy) {
            busy = 1;
        }
        if (!busy && now - r->lastInfo > 10000) {
            r->lastInfo = now;
            r->infoBusy = 1;
            s32 res = WPADGetInfoAsync(chan, r->info, fn_802E7C14);
            if (res != 0) {
                r->infoBusy = 0;
                if (res == -1) {
                    memset(r->info, 0, sizeof(r->info));
                }
            }
        }
        s32 probe = WPADProbe(chan, type);
        if (fn_80047C0C(chan, r->kpad, 1) >= 1 && probe == 0 && r->present) {
            int err = r->kpad[0].err;
            if (err != *last) {
                fn_802E30C8("rjoy: #%d: %s -> %s", chan, fn_802E77C0(*last), fn_802E77C0(err));
                *last = err;
            }
            if (err == 0) {
                if (!r->connected) {
                    r->connected = 1;
                    r->pwm = 0;
                    r->home = 1;
                    fn_802E30C8("rjoy: #%d: connected", chan);
                }
                r->sens = WPADGetRadioSensitivity(chan);
                if (!fn_802ECC10()) {
                    fn_802E7920(chan, 0x3091);
                }
                fn_802E7DDC(r, r->kpad);
            }
        } else if (!r->present && r->connected) {
            r->connected = 0;
            fn_802E30C8("rjoy: #%d: disconnected", chan);
        }
        if (r->b29) {
            if (!r->home) {
                r->home = 1;
                fn_802ECBF8();
            }
        } else {
            r->home = 0;
        }
        int hold = 0;
        if (fn_802E77B8(chan) && r->b2B && r->b29 && r->b27) {
            hold = 1;
        }
        s32 t = fn_802E2C20();
        if (!hold) {
            r->lastHome = t;
        } else if (t - r->lastHome > 1000) {
            fn_802E2E38(0);
        }
    }
    fn_802EC884();
}

void fn_802E830C(s32* m) {
    m[0] = 3;
    m[1] = 0;
    m[2] = 2;
    m[9] = -1;
    m[3] = 1;
    m[4] = 7;
    m[12] = 4;
    m[13] = -1;
    m[10] = -1;
    m[11] = -1;
    m[5] = 5;
    m[6] = 6;
}

struct RemoteOut {
    f32 v[8];     // offset 0x0
    u8 b[0x20];   // offset 0x20
    u32 w[8];     // offset 0x40
    u8 isExt;     // offset 0x60
};

u8 fn_802E8364(RemoteOut* out, s32 chan) {
    Remote* r = &lbl_80505978[chan];
    *out = *(RemoteOut*)&r->steer;
    return r->connected;
}

u8 fn_802E8504(s32 chan) { return lbl_80505978[chan].connected; }

void fn_802E8528(s32 chan, f32 strength, f32 b);

void fn_802E8518(s32 chan) { fn_802E8528(chan, 0.0f, 0.0f); }

void fn_802E8524(void) {}

void fn_802E8528(s32 chan, f32 strength, f32 b) {
    f32 v = 65535.0f * (strength - 0.01f);
    int x = (int)(v < 0.0f ? v - 0.5f : 0.5f + v);
    x = x > 0 ? x : 0;
    lbl_80505978[chan].rumble = x < 0xFFFF ? x : 0xFFFF;
}

void fn_802E85A8(void) {}

void fn_802E85AC(void) {}

int fn_802E85B0(void) { return 3; }

const char* fn_802E85B8(s32 chan) {
    const char* names[4] = {"Controller Socket 1", "Controller Socket 2", "Controller Socket 3", "Controller Socket 4"};
    return names[chan];
}

void fn_802E85F4(s32 chan) {
    Remote* r = &lbl_80505978[chan];
    f32 a = 0.2617994f;
    if (a >= r->angle) {
        a = r->angle;
    }
    if (a <= -0.5235988f) {
        a = -0.5235988f;
    }
    r->hist[r->histPos] = a;
    r->histPos = (r->histPos + 1) % 16;
}

void fn_802E8664(void) {}

KStatus* fn_802E8668(s32 chan) {
    Remote* r = &lbl_80505978[chan];
    if (r->connected) {
        return r->kpad;
    }
    return 0;
}

int fn_802E8690(s32 chan) { return lbl_80505978[chan].kpad[0].devType == 1; }

u8 fn_802E86B4(s32 chan) { return lbl_80505978[chan].sens; }

}  // extern "C"
