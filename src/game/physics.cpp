// Physics/race state (0x80176460-0x80177814): the race-wide physics state machine (init stream, running,
// restart, post-race), the race mode requests with their screen fader, the frame clock and the physics
// state listeners. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
char* strcpy(char* dst, const char* src);
void* memset(void* dst, int c, u32 n);
void* memcpy(void* dst, const void* src, u32 n);
void* memmove(void* dst, const void* src, u32 n);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void fn_801E1D20(void);
void fn_801E1D2C(void);
void fn_80178DD0(void);
s32 fn_8012F31C(s32 size);
s32 fn_8012F3CC(void);
void fn_8012F43C(void);
void fn_8012F384(s32 s);
void fn_8012F418(s32 s);
void fn_8012F458(s32 s, s32 pos);
s32 fn_8012F450(s32 s);
void fn_8012F4AC(s32 s, void* data, s32 size);
void fn_8017EBE0(void);
void fn_8017EC30(void);
void fn_801AE5A8(void);
void fn_801AE5AC(void);
void fn_801B6604(void);
void fn_801B6614(void);
void fn_801D0BE4(void);
void fn_801D0BF4(void);
s32 fn_802090D4(const char* name);
void fn_80209110(s32* h);
void fn_80209190(s32 h, f32 v);
s32 fn_80179658(s32 a);
void fn_8013A208(void);
s32 fn_802E2C20(void);
s64 fn_802E2C9C(void);
s32 fn_802D3EBC(void);
void fn_802E2D74(int a);
f32 fn_8017970C(void);
struct StatePacket* fn_80179824(void);
void fn_801C6AD0(s32* p);
void fn_8017ED00(f32 dt);
void fn_80179320(void);
void fn_802E4C14(int a);
void fn_80178CAC(s32 a, s32 b);
void fn_80178DD4(s32 s);
void fn_8017952C(void);
void fn_80179110(void);
void fn_801791A8(void);
void fn_801B235C(void);
void fn_80179140(void);
void fn_80179308(void);
void fn_801AED68(void);
void fn_801AED5C(void);
int fn_801CF050(void);
int fn_802ECC10(void);
f32 __cvt_sll_flt(s64 v);
}

struct StatePacket {
    u8 pad0[0xDB0];
    s32 fade;  // offset 0xDB0
};

struct LocString {
    LocString(const char* key);
    u8 pad0[0x110];
};

struct RaceMode {
    void Clear(void) {
        mode = 4;
        flags = 0;
        memset(extra, 0, sizeof(extra));
    }
    s32 mode;     // offset 0x0
    u32 flags;    // offset 0x4
    u8 extra[1];  // offset 0x8
};

struct EventSetup {
    s32 event;  // offset 0x0
    s32 w4;     // offset 0x4
    s32 w8;     // offset 0x8
    s32 wC;     // offset 0xC
    u32 flags;  // offset 0x10
};

struct EventDesc {
    EventSetup setup;  // offset 0x0
    u8 pad14[0x18 - 0x14];
};

struct PendingCall {
    u8 set;  // offset 0x0
    s32 a;   // offset 0x4
    s32 b;   // offset 0x8
    u8 pad[0x30 - 0xC];
};

struct Fader {
    s32 frame;          // offset 0x0
    s32 time;           // offset 0x4
    s32 hold;           // offset 0x8
    s32 state;          // offset 0xC
    void (*done)(void);  // offset 0x10
    u8 alpha;           // offset 0x14
};

class PhysListener {
public:
    virtual void Init(void);     // 0x8
    virtual void Start(void);    // 0xC
    virtual void Restart(void);  // 0x10
    virtual void Pause(void);    // 0x14
    virtual void End(void);      // 0x18
};

ModuleTag lbl_8055E3B0;
UnkStaticFFFF lbl_8055E3B8;
u8 lbl_8055E3BC;  // active
u8 lbl_8055E3BD;
s32 lbl_8055E3C0;
s32 lbl_8055E3C4;  // frame
f32 lbl_8055E3C8;  // time
s32 lbl_8055E3CC;
f32 lbl_8055E3D0;
u8 lbl_8055E3D4;
s32 lbl_8055E3D8;
s32 lbl_8055E3DC;
s32 lbl_8055E3E0;
u8 lbl_8055E3E4;
s32 lbl_8055E3E8;
u8 lbl_8055E3EC;
s32 lbl_8055E3F0;
volatile s32 lbl_8055E3F4;  // state
s32 lbl_8055E3F8;
f32 lbl_8055E3FC;
volatile s32 lbl_8055E400;

volatile s32 lbl_8055A2F0 = 1;
f32 lbl_8055A2F4 = 3.0f;

RaceMode lbl_804303C8;
RaceMode lbl_804303D4;
EventDesc lbl_804303E0;
char lbl_804303F8[0x20];
PendingCall lbl_80430418;
LocString lbl_80430448("High_Scores:DNF");
Fader lbl_80430558;
PhysListener* lbl_80430570[8];

extern f32 lbl_8055A3A0;
extern f32 lbl_8055E460;

static inline s32 SpeedMode(void) { return lbl_8055A2F0; }

extern "C" {

void fn_80176610(void);
void fn_80176A34(s32 frame);
void fn_80176CC8(void);
void fn_80176DE0(void);
int fn_80176F9C(void);
void fn_80176F24(void);
void fn_80176FC4(void);
void fn_801775EC(void);
void fn_80177048(void);
void fn_801770C8(void);
void fn_80177138(void);
void fn_801771A8(void);
int fn_801773E4(void);
void fn_80177790(void);
void fn_8017779C(void);
void fn_801777C0(void);
void fn_801769E0(void);
void fn_801769EC(void);
s32 fn_80176A1C(void);
f32 fn_80176A2C(void);

void fn_80176460(const EventSetup* s, const char* name, const s32* seed) {
    lbl_8055E3BC = 1;
    lbl_8055E3F8 = *seed;
    lbl_804303E0.setup = *s;
    strcpy(lbl_804303F8, name);
    lbl_8055E3F4 = 0;
    lbl_8055E3BD = 1;
    lbl_8055E3C0 = 0;
    lbl_8055E3D4 = 0;
    lbl_8055E3EC = 0;
    lbl_8055E3E8 = 0;
    lbl_8055E3D8 = 1;
    lbl_8055A2F0 = 1;
    lbl_8055A2F4 = 3.0f;
    fn_80176A34(0);
    fn_801E1D20();
    fn_80178DD0();
    lbl_804303C8.Clear();
    lbl_804303D4.Clear();
    lbl_804303C8.mode = 0;
    lbl_804303C8.flags = 0x2800;
    if (!(s->flags & 1)) {
        lbl_804303C8.flags |= 0x400;
    }
    lbl_8055E3E4 = 1;
    lbl_8055A3A0 = 1.0f;
    lbl_8055E3CC = 0;
    lbl_8055E3DC = fn_8012F31C(0x10000);
    lbl_8055E3E0 = fn_8012F3CC();
    fn_8012F43C();
    fn_8017EBE0();
    fn_801AE5A8();
    fn_801B6604();
    fn_801D0BE4();
    fn_80177790();
}

void fn_801765B8(void) {
    fn_801777C0();
    lbl_8055E3F4 = 0;
    fn_801D0BF4();
    fn_801B6614();
    fn_801AE5AC();
    fn_8017EC30();
    fn_8012F418(lbl_8055E3E0);
    fn_8012F384(lbl_8055E3DC);
    fn_801E1D2C();
    lbl_8055E3BC = 0;
}

void fn_80176610(void) { fn_80176CC8(); }

void fn_80176614(void) {
    int tries;
    lbl_8055E3F4 = 1;
    fn_8012F458(lbl_8055E3E0, -1);
    fn_802E30C8("physics: using %d of %d for init stream", fn_8012F450(lbl_8055E3E0), 0x10000);
    tries = 0;
    while (lbl_8055E3F4 == 1) {
        fn_80176CC8();
        if (++tries >= 10) {
            fn_802E317C("can't get physics out of init");
        }
    }
    lbl_8055E3F4 = 3;
    fn_80176DE0();
    lbl_8055E3F0 = fn_802090D4("physics");
    fn_8017779C();
}

void fn_801766C4(void) {
    int tries = 0;
    lbl_8055E3F4 = 7;
    while (lbl_8055E3F4 != 8) {
        fn_80176CC8();
        if (++tries >= 10) {
            fn_802E317C("can't get physics to end");
        }
    }
    fn_80209110(&lbl_8055E3F0);
}

void fn_80176734(void) {
    if (fn_80176F9C()) {
        fn_802E30C8("physics: hey, phys-fader was active (restart)");
    }
    lbl_804303D4.mode = 4;
    fn_80177048();
}

void fn_8017677C(int mode, u32 flags) {
    const char* modes[] = {"Long Intro", "Player-drives", "Pack-off-track", "Immediate"};
    const char* flagNames[] = {"", "Restart Weekend", "Interlude", "No Pause", "Fade From Black", "PRACTICE"};
}

void fn_80176780(u32 req, const u8* extra) {
    u32 flags;
    int mode;
    mode = req & 0xFF;
    flags = (req & 0x1FF00) & ~(lbl_8055E3E4 ? 0x100 : 0);
    fn_8017677C(mode, flags);
    if (fn_80176F9C()) {
        fn_802E30C8("physics: hey, phys-fader was active! (new race mode)");
    }
    lbl_804303D4.mode = mode;
    lbl_804303D4.flags = flags;
    if (extra) {
        lbl_804303D4.extra[0] = *extra;
    }
    if (flags & 0x1000) {
        fn_80176F24();
    } else if (flags & 0x8000) {
        fn_80176FC4();
    } else if (flags & 0x10000) {
        fn_801775EC();
        fn_80177048();
    } else {
        fn_80177048();
    }
}

void fn_80176858(void) { fn_80177138(); }

void fn_8017685C(int arg) {
    int tries;
    if (arg != 3) {
        fn_801769E0();
        lbl_8055E3F4 = 5;
        tries = 0;
        while (lbl_8055E3F4 == 5) {
            fn_80176CC8();
            if (++tries >= 10) {
                fn_802E317C("can't get physics to restart");
            }
        }
    }
    lbl_8055E3E8 = arg;
    lbl_8055E3EC = 1;
    tries = 0;
    while (lbl_8055E3EC) {
        fn_80176CC8();
        if (++tries >= 10) {
            fn_802E317C("can't get physics to postrace phobs");
        }
    }
}

void fn_8017691C(void) {
    if (!fn_80176F9C()) {
        fn_801770C8();
    }
}

void fn_80176948(s32* data, s32* pos) {
    s32 p = lbl_8055E3CC;
    *pos = p;
    fn_8012F458(lbl_8055E3E0, p);
    fn_8012F4AC(lbl_8055E3E0, data, data[1]);
    lbl_8055E3CC += fn_80179658(data[0]);
}

s32 fn_801769A8(void) { return lbl_804303C8.flags; }

s32 fn_801769B8(void) { return lbl_804303C8.mode; }

s32 fn_801769C4(void) { return lbl_8055E3E8; }

EventDesc* fn_801769CC(void) { return &lbl_804303E0; }

s32* fn_801769D8(void) { return &lbl_8055E3F8; }

void fn_801769E0(void) { lbl_8055E3C0 = 1; }

void fn_801769EC(void) {
    lbl_8055E3C0 = 2;
    fn_8013A208();
}

u8 fn_801769F8(void) { return lbl_8055E3BD; }

void fn_80176A00(s32 m) {
    lbl_8055A2F0 = m;
    if (SpeedMode() == 1) {
        lbl_8055E460 = 1.0f;
    }
}

s32 fn_80176A1C(void) { return lbl_8055A2F0; }

void fn_80176A24(f32 v) { lbl_8055A2F4 = v; }

f32 fn_80176A2C(void) { return lbl_8055A2F4; }

void fn_80176A34(s32 frame) {
    lbl_8055E3C4 = frame;
    lbl_8055E3C8 = 0.016683351f * (f32)frame;
}

void fn_80176A6C(void) { fn_801771A8(); }

StatePacket* fn_80176A70(void) {
    s32 frame = lbl_8055E3C4;
    s32 t0 = fn_802E2C20();
    int n = 0;
    s64 start = fn_802E2C9C();
    s64 now = start;
    StatePacket* pkt;
    for (;;) {
        fn_80176610();
        if (fn_80176A1C() == 2 || lbl_8055E3BD || frame != lbl_8055E3C4 || fn_802D3EBC()) {
            break;
        }
        fn_802E2D74(0);
        if (fn_802E2C20() - t0 > 10000) {
            fn_802E30C8("PhysicsGetStatePacket(): detail: deviation %f seconds", fn_8017970C());
            fn_802E317C("PhysicsGetStatePacket(): waited for more than %d seconds", 10);
        }
        if (((u16)++n) == 0) {
            fn_802E30C8("PhysicsGetStatePacked: waiting...%d", n);
        }
        now = fn_802E2C9C();
    }
    {
        f32 dt = 0.001f * __cvt_sll_flt(now - start);
        lbl_8055E3D0 = dt + 0.95f * (lbl_8055E3D0 - dt);
    }
    pkt = fn_80179824();
    pkt->fade = fn_801773E4();
    fn_801C6AD0(&pkt->fade);
    return pkt;
}

void fn_80176BA8(void) {
    const char* fmts[] = {"%s%s%01d:%02d%c%02d", "%s%02d%c%02d", "%s%s%01d:%02d%c%01d", "%s%02d%c%01d"};
}

void fn_80176BAC(void) {
    f32 t = lbl_8055E3C8;
    f32 dt = t - lbl_8055E3FC;
    dt = MIN(0.25f, dt);
    dt = MAX(0.01f, dt);
    fn_8017ED00(dt);
    lbl_8055E3FC = t;
}

s32* fn_80176C0C(s32* dst, const s32* src) {
    *dst = *src;
    memcpy(dst + 1, src + 1, *src);
    return dst;
}

void fn_80176C4C(void) {
    if (lbl_804303D4.mode != 4) {
        lbl_804303C8 = lbl_804303D4;
        lbl_804303D4.Clear();
    }
    fn_80179320();
    lbl_8055E3F4 = 3;
}

void fn_80176CC8(void) {
    fn_802E4C14(1);
    if (lbl_80430418.set) {
        fn_80178CAC(lbl_80430418.a, lbl_80430418.b);
        lbl_80430418.set = 0;
    }
    switch (lbl_8055E3F4) {
    case 0:
        break;
    case 1:
        fn_80178DD4(lbl_8055E3DC);
        lbl_8055E3F4 = 2;
        break;
    case 2:
        break;
    case 3:
        fn_8017952C();
        break;
    case 4:
        fn_80176C4C();
        break;
    case 5:
        fn_80179110();
        fn_8017952C();
        lbl_8055E3F4 = 6;
        break;
    case 6:
        fn_8017952C();
        break;
    case 7:
        fn_801791A8();
        lbl_8055E3F4 = 8;
        break;
    case 8:
        break;
    default:
        fn_802E317C("Unknown physics state: %d", lbl_8055E3F4);
        break;
    }
    if (lbl_8055E3D4) {
        fn_801B235C();
        lbl_8055E3D4 = 0;
    }
    if (lbl_8055E3EC) {
        fn_80179140();
        lbl_8055E3EC = 0;
    }
}

void fn_80176DD4(void) { lbl_8055E3D4 = 1; }

static inline void FadeIn(void) {
    lbl_80430558.state = 1;
    lbl_80430558.done = 0;
    lbl_80430558.time = fn_802E2C20() + 50;
    lbl_80430558.frame = lbl_8055E3C4 + 3;
    lbl_80430558.hold = 0;
    lbl_80430558.alpha = 0;
}

void fn_80176DE0(void) { FadeIn(); }

void fn_80176E4C(void) {
    fn_80179308();
    fn_801769EC();
    FadeIn();
}

void fn_80176EC0(void) {
    if (lbl_804303D4.mode != 4) {
        fn_801AED68();
        if (lbl_804303D4.flags & 0x200) {
            fn_801AED5C();
        }
    }
    if (!fn_801CF050()) {
        fn_80176A34(0);
    }
    fn_80176C4C();
}

void fn_80176F24(void) { fn_80176EC0(); }

void fn_80176F28(void) {
    fn_80176EC0();
    fn_801769EC();
    FadeIn();
}

int fn_80176F9C(void) { return lbl_80430558.state != 0 && lbl_80430558.state != 3; }

static inline void FadeOut(void (*done)(void)) {
    lbl_80430558.done = done;
    lbl_80430558.state = 2;
    lbl_80430558.time = fn_802E2C20();
    lbl_80430558.frame = lbl_8055E3C4;
    lbl_80430558.hold = 7;
    lbl_80430558.alpha = 0;
    fn_801769E0();
}

void fn_80176FC4(void) {
    if (lbl_80430558.state == 3) {
        fn_80176F28();
    } else {
        FadeOut(fn_80176F28);
    }
    fn_801769EC();
}

void fn_80177048(void) {
    if (lbl_80430558.state == 3) {
        fn_80176F28();
    } else {
        FadeOut(fn_80176F28);
    }
}

void fn_801770C8(void) {
    FadeOut(fn_80176DD4);
    fn_801769EC();
}

void fn_80177138(void) {
    FadeOut(fn_80176E4C);
    fn_801769EC();
}

static inline void FadeStep(void) {
    if (lbl_80430558.state - 1 <= 1u) {
        if (fn_802ECC10()) {
            if (lbl_80430558.state == 2) {
                int a = lbl_80430558.alpha;
                lbl_80430558.hold = 7;
                lbl_80430558.alpha = MIN(a, 250);
            }
        } else if (!lbl_8055E3BD) {
            f32 speed = fn_80176A1C() == 2 ? fn_80176A2C() : 1.0f;
            int frames;
            int d;
            int a;
            s32 now;
            speed = MAX(speed, 0.001f);
            frames = (int)(45.0f / speed);
            now = lbl_8055E3C4;
            d = MIN(2, now - lbl_80430558.frame);
            a = lbl_80430558.alpha + (d * 255) / frames;
            a = a > 0 ? a : 0;
            a = MIN(a, 255);
            lbl_80430558.frame = now;
            lbl_80430558.alpha = a;
        } else {
            s32 t = fn_802E2C20();
            int d = MIN(33, t - lbl_80430558.time);
            int a = lbl_80430558.alpha + (d * 255) / 750;
            a = a > 0 ? a : 0;
            lbl_80430558.alpha = MIN(a, 255);
            lbl_80430558.time = t;
        }
    }
}

void fn_801771A8(void) {
    FadeStep();
    if (lbl_80430558.alpha == 255 && lbl_80430558.hold > 0) {
        lbl_80430558.hold--;
    }
    if (lbl_80430558.alpha == 255 && lbl_80430558.hold <= 0 && lbl_80430558.state - 1 <= 1u) {
        s32 ns = 3;
        if (lbl_80430558.state == 1) {
            ns = 0;
        }
        lbl_80430558.state = ns;
        if (lbl_80430558.done) {
            lbl_80430558.done();
            lbl_80430558.done = 0;
        }
    }
}

int fn_801773E4(void) {
    u8 a;
    FadeStep();
    if (lbl_80430558.state <= 1u) {
        a = 255 - lbl_80430558.alpha;
    } else {
        a = lbl_80430558.alpha;
    }
    fn_80209190(lbl_8055E3F0, (f32)(255 - a) / 255.0f);
    return a;
}

void fn_801775EC(void) {
    lbl_80430558.alpha = 255;
    lbl_80430558.state = 3;
}

void fn_80177608(PhysListener* l) { l->Init(); }

void fn_80177618(PhysListener* l) { l->Start(); }

void fn_80177628(PhysListener* l) { l->Restart(); }

void fn_80177638(PhysListener* l) { l->Pause(); }

void fn_80177648(PhysListener* l) { l->End(); }

void fn_80177658(void (*fn)(PhysListener*)) {
    int i;
    for (i = 0; i < lbl_8055E400; i++) {
        fn(lbl_80430570[i]);
    }
}

void fn_801776C4(PhysListener* l) {
    int n = lbl_8055E400;
    int i;
    for (i = 0; i < n; i++) {
        if (l == lbl_80430570[i]) {
            fn_802E317C("Physics: tried to register a state listener that was already registered!");
            return;
        }
    }
    lbl_80430570[n] = l;
    lbl_8055E400++;
}

void fn_80177720(PhysListener* l) {
    int n = lbl_8055E400;
    int i;
    for (i = 0; i < n; i++) {
        if (l == lbl_80430570[i]) {
            int c = lbl_8055E400 - 1;
            lbl_8055E400 = c;
            memmove(&lbl_80430570[i], &lbl_80430570[i] + 1, (c - i) * 4);
            return;
        }
    }
    fn_802E317C("Physics: tried to unregister a state listener that was not registered!");
}

void fn_80177790(void) { fn_80177658(fn_80177608); }

void fn_8017779C(void) { fn_80177658(fn_80177618); }

void fn_801777A8(void) { fn_80177658(fn_80177628); }

void fn_801777B4(void) { fn_80177658(fn_80177638); }

void fn_801777C0(void) { fn_80177658(fn_80177648); }

void fn_801777CC(PhysListener* l) { fn_801776C4(l); }

void fn_801777D0(PhysListener* l) { fn_80177720(l); }

}  // extern "C"
