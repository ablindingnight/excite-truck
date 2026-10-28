// Weather (0x801E1874-0x801E1F8C): fog, precipitation and lightning settings from the track's
// "weather" table, the lightning flash state machine, and rain/thunder sounds. Names are guesses
// apart from the value keys and sound names.
#include "game/common.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
ValueNode* fn_802AE414(const char* name);  // track effect table lookup
int fn_80129D18(int id, int range);        // random int
f32 fn_80129DC4(int id, f32 lo, f32 hi);   // random float
}

class RSound {
public:
    virtual ~RSound(void);
    u8 pad4[0x19 - 0x4];
    u8 busy;  // offset 0x19
    u8 pad1A[0x49 - 0x1A];
    u8 playing;  // offset 0x49
    u8 stop;     // offset 0x4A
    u8 loop;     // offset 0x4B
    u8 f4C;      // offset 0x4C
    void Play(void) {
        playing = 1;
        loop = 0;
        stop = 0;
    }
    void Retrigger(void) {
        if (busy) {
            f4C = 1;
            stop = 0;
        } else {
            Play();
        }
    }
    void PlayLoop(void) {
        stop = 0;
        loop = 1;
        playing = 1;
    }
};

extern "C" RSound* fn_80207848(const char* name, int a);

struct WeatherDef {
    s32 precip;      // offset 0x0 (0 none, 1 rain, 2 snow)
    u8 night;        // offset 0x4
    u32 fogColor;    // offset 0x8
    f32 fogStart;    // offset 0xC
    f32 fogStop;     // offset 0x10
    f32 fogDensity;  // offset 0x14
    u8 hasFog;       // offset 0x18
};

struct WeatherState {
    s32 lightning;   // offset 0x0 (0 off, 1 idle, 2 flash, 3 thunder)
    f32 flash;       // offset 0x4
    f32 target;      // offset 0x8
    f32 thunderAt;   // offset 0xC
    f32 angle;       // offset 0x10
    RSound* rain;    // offset 0x14
    RSound* thunder; // offset 0x18
};

struct WeatherPlayer {
    u8 pad0[0xD4];
    f32 flashA;  // offset 0xD4
    f32 flashB;  // offset 0xD8
    f32 angle;   // offset 0xDC
};

struct WeatherRace {
    u8 pad0[4];
    WeatherPlayer players[4];  // offset 0x4 (stride 0xE0)
    u8 pad384[0x394 - 0x384];
    s32 numPlayers;  // offset 0x394
};

extern f32 lbl_8055E3C8;     // race clock
extern f32 lbl_80558DC4[];   // FLT_EPSILON

#define NearZero(x) ((f32)__fabs(x) <= lbl_80558DC4[0])

static inline int MaxI(int a, int b) { return a > b ? a : b; }
static inline int MinI(int a, int b) { return a < b ? a : b; }
static inline int ColorByte(const f32& f) {
    int v = f * 255.0f;
    v = v > 0 ? v : 0;
    return v < 255 ? v : 255;
}

ModuleTag lbl_8055EA18;
u8 lbl_8055EA20;  // loaded
u8 lbl_8055EA21;  // force a strike
u8 lbl_8055EA22;  // lightning enabled
WeatherState lbl_804874B8;
WeatherDef lbl_804874D4;

extern "C" {

void fn_801E1874(void) {
    lbl_804874D4.precip = 0;
    lbl_804874D4.hasFog = 0;
    lbl_804874D4.night = 0;
    lbl_8055EA22 = 0;
}

void fn_801E1894(void) {
    fn_801E1874();
    if (fn_802AE414("weather")) {
        ValueNode* w = fn_802AE414("weather");
        ValueNode* fog = w->FindChild("fog");
        const char* p = w->GetString("precip", "none");
        if (!strcasecmp(p, "rain")) {
            lbl_804874D4.precip = 1;
        } else if (!strcasecmp(p, "snow")) {
            lbl_804874D4.precip = 2;
        }
        lbl_8055EA22 = w->Has("lightning") ? w->GetIntValue2("lightning") : lbl_804874D4.precip == 1;
        lbl_804874D4.night = w->GetBool("is_night", 0);
        if (fog) {
            const ValVec3& c = fog->GetVec3("color");
            int r = ColorByte(c.x);
            int g = ColorByte(c.y);
            int b = ColorByte(c.z);
            lbl_804874D4.fogColor = 0xFF000000 | (u8)r << 16 | (u8)g << 8 | (u8)b;
            lbl_804874D4.fogStart = fog->GetFloat("start");
            lbl_804874D4.fogStop = fog->GetFloat("stop");
            lbl_804874D4.fogDensity = fog->GetFloat("density");
            lbl_804874D4.hasFog = 1;
        }
    }
    lbl_8055EA20 = 1;
}

static inline f32 AbsF(f32 v) { return (f32)__fabs(v); }

void fn_801E1B30(void) {
    WeatherState* w = &lbl_804874B8;
    f32 now = lbl_8055E3C8;
    if (!fn_80129D18(4, 300) || lbl_8055EA21) {
        if (w->lightning == 1 || lbl_8055EA21) {
            if (fn_80129D18(4, 10) < 6 || lbl_8055EA21) {
                w->lightning = 2;
                w->angle = fn_80129DC4(4, 0.0f, 6.2831855f);
            } else {
                w->lightning = 3;
                w->thunderAt = now + fn_80129DC4(4, 0.5f, 1.5f);
            }
            w->target = fn_80129DC4(4, 1.5f, 3.0f);
        } else if (w->flash > w->target) {
            w->target = fn_80129DC4(4, w->flash, 1.0f);
        }
    }
    lbl_8055EA21 = 0;
    if (w->lightning != 1) {
        if (AbsF(w->flash - w->target) < 0.001f) {
            if (!NearZero(w->target)) {
                w->target = 0.0f;
            } else {
                w->lightning = 1;
            }
        }
        w->flash = w->flash + 0.5f * (w->target - w->flash);
    }
    if (lbl_804874B8.thunder && w->thunderAt > 0.0f && now > w->thunderAt) {
        lbl_804874B8.thunder->Retrigger();
        w->thunderAt = -1.0f;
    }
}

void fn_801E1D20(void) { lbl_8055EA20 = 0; }

void fn_801E1D2C(void) { lbl_8055EA20 = 0; }

WeatherDef* fn_801E1F44(void);

void fn_801E1D38(void) {
    if (fn_801E1F44()->precip == 1) {
        lbl_804874B8.rain = fn_80207848("rain.sfx", 2);
        lbl_804874B8.thunder = fn_80207848("thunder.sfx", 2);
    }
}

void fn_801E1D9C(void) {
    if (lbl_804874B8.thunder) {
        delete lbl_804874B8.thunder;
        lbl_804874B8.thunder = 0;
    }
    if (lbl_804874B8.rain) {
        delete lbl_804874B8.rain;
        lbl_804874B8.rain = 0;
    }
}

void fn_801E1E30(void) {
    lbl_804874B8.lightning = lbl_8055EA22 != 0;
    lbl_804874B8.flash = 0.0f;
    lbl_804874B8.target = 0.0f;
    lbl_804874B8.thunderAt = -1.0f;
}

void fn_801E1E64(void) {
    if (lbl_804874B8.rain) {
        lbl_804874B8.rain->PlayLoop();
    }
    if (lbl_804874B8.lightning) {
        fn_801E1B30();
    }
}

void fn_801E1EA4(WeatherRace* r) {
    int i;
    for (i = 0; i < r->numPlayers; i++) {
        WeatherPlayer* p = &r->players[i];
        switch (lbl_804874B8.lightning) {
        case 2:
            p->flashA = 0.25f * lbl_804874B8.flash;
            p->flashB = 0.1f * lbl_804874B8.flash;
            p->angle = lbl_804874B8.angle;
            break;
        case 3:
            p->flashA = 0.0f;
            p->flashB = 0.15f * lbl_804874B8.flash;
            p->angle = 0.0f;
            break;
        default:
            p->flashA = 0.0f;
            p->flashB = 0.0f;
            p->angle = 0.0f;
            break;
        }
    }
}

WeatherDef* fn_801E1F44(void) {
    if (!lbl_8055EA20) {
        fn_801E1894();
    }
    return &lbl_804874D4;
}

void fn_801E1F78(void) { lbl_8055EA21 = 1; }

}  // extern "C"
