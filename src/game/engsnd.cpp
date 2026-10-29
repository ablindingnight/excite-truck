// Engine sounds (0x80209448-0x80209D4C): a car's engine samples from its engine sound file, each
// faded in and out by rpm and throttle windows and pitched by rpm, louder for the local player's
// car, and all faded out while loading or after the race. Names are guesses apart from the keys and
// messages.
#include "game/common.h"

extern "C" {
void* memset(void* dst, int c, size_t n);
int strcmp(const char* a, const char* b);
void fn_802E30C8(const char* fmt, ...);  // debug print
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* table);
void fn_80127C90(void (*fn)(void), const char* name);
void fn_80127D58(void (*fn)(void));
int fn_801B89F8(void);   // race over
int fn_80178CA4(void);   // local players
int fn_8026A344(int i);  // local player's car
int fn_80137C74(void);   // sound output mode
}

int IsLoadingActive(void);
void* operator new(size_t size, const char* tag);

// A game sound (see sound.cpp).
class GameSound {
public:
    virtual ~GameSound(void);
    void SetPitch(f32 v) {
        if (v != pitch) {
            pitchDirty = 1;
            pitch = v;
        }
    }
    void SetVolume(f32 v) {
        if (v != volume) {
            volumeDirty = 1;
            volume = v;
        }
    }
    void Play(void) {
        stop = 0;
        loop = 1;
        start = 1;
    }

    f32 pitch;   // offset 0x4
    f32 volume;  // offset 0x8
    void* dev;   // offset 0xC
    s32 player;  // offset 0x10
    u8 pad14[0x34];
    u8 paused;        // offset 0x48
    u8 start;         // offset 0x49
    u8 stop;          // offset 0x4A
    u8 loop;          // offset 0x4B
    u8 restart;       // offset 0x4C
    u8 volumeDirty;   // offset 0x4D
    u8 pitchDirty;    // offset 0x4E
    u8 pad4F[0x15];
};

class Sound2D : public GameSound {
public:
    void SetPan(f32 v) {
        if (v != pan) {
            panDirty = 1;
            pan = v;
        }
    }
    void SetDepth(f32 v) {
        if (v != depth) {
            depthDirty = 1;
            depth = v;
        }
    }

    f32 pan;        // offset 0x64
    f32 depth;      // offset 0x68
    u8 panDirty;    // offset 0x6C
    u8 depthDirty;  // offset 0x6D
};

extern "C" {
GameSound* fn_802077A4(const char* name, int cls, void* vel, void* pos);
Sound2D* fn_80207848(const char* name, int cls);
}

struct VehicleDef {
    virtual ValueNode* FindChild(const char* name);
};

struct CarSetup {
    u8 pad0[0x2134];
    VehicleDef* def;  // offset 0x2134
};

struct Car {
    u8 pad0[0x60];
    u8 pos[0xB8];       // offset 0x60
    u8 vel[0xD4];       // offset 0x118
    s32 player;         // offset 0x1EC
    s32 kind;           // offset 0x1F0
    u8 pad1F4[0x6C];
    u8 flags;           // offset 0x260
    u8 pad261[0xB];
    CarSetup* setup;    // offset 0x26C
    u8 pad270[0x2360];
    f32 rpm;            // offset 0x25D0
    u8 pad25D4[0x28];
    f32 throttle;       // offset 0x25FC
};

// A fade window: full between lo and hi, fading out over 1/invLo below and 1/invHi above.
struct Window {
    void Load(ValueNode* n) {
        f32 g = n->GetFloat("gain", 1.0f);
        f32 t4 = n->GetFloat("t4", 110000.0f);
        f32 t3 = n->GetFloat("t3", 100000.0f);
        f32 t2 = n->GetFloat("t2", -100000.0f);
        f32 t1 = n->GetFloat("t1", -110000.0f);
        lo = t2;
        hi = t3;
        invLo = 1.0f / (t2 - t1);
        invHi = 1.0f / (t4 - t3);
        gain = g;
    }

    f32 lo;     // offset 0x0
    f32 hi;     // offset 0x4
    f32 invLo;  // offset 0x8
    f32 invHi;  // offset 0xC
    f32 gain;   // offset 0x10
};

struct EngineSample {
    GameSound* snd;  // offset 0x0
    f32 invRpm;      // offset 0x4
    Window rpm;      // offset 0x8
    Window throttle; // offset 0x1C
    f32 level;       // offset 0x30
};

extern "C" void fn_80209904(void);

const char* lbl_8055B4F8 = "Mus68.val";  // default engine sound file
f32 lbl_8055B4FC = 0.63f;               // engine volume
ModuleTag lbl_8055EDB0;
f32 lbl_8055EDB8;  // engine fade
s32 lbl_8055EDBC;  // engine sounds
static UnkStatic123 lbl_804892A8;

class EngineSound {
public:
    EngineSound(Car* car) {
        if (lbl_8055EDBC == 0) {
            lbl_8055EDB8 = 0.0f;
            fn_80127C90(fn_80209904, "engsnd");
        }
        lbl_8055EDBC++;
        ValueNode* child;
        this->car = car;
        count = 0;
        ValueTable* table = fn_80131808(
            car->setup->def->FindChild("EngineNormal")->GetString("engine_sound", lbl_8055B4F8));
        ValueNode* global = table->FindChild("global");
        rpmFactor = global->GetFloat("rpm_factor", 1.0f);
        focusVolume = global->GetFloat("focus_volume");
        otherVolume = 0.9f * focusVolume;
        for (int i = 0; (child = table->GetChild(i)) != NULL; i++) {
            if (strcmp(child->GetName(), "sample") == 0) {
                EngineSample* e = &samples[count++];
                memset(e, 0, sizeof(EngineSample));
                const char* name = child->GetValue("sound");
                if (this->car->kind != 0x50) {
                    e->snd = fn_802077A4(name, 3, this->car->vel, car->pos);
                } else {
                    Sound2D* s = fn_80207848(name, 2);
                    if (s) {
                        if (fn_80137C74() == 2) {
                            if (!(this->car->flags & 3)) {
                                s->SetPan(-1.0f);
                            } else {
                                s->SetPan(1.0f);
                            }
                        }
                        s->SetDepth(0.0f);
                    }
                    e->snd = s;
                }
                e->level = 0.0f;
                if (!e->snd) {
                    fn_802E30C8("engsnd: skipping %s", name);
                    count--;
                } else {
                    e->snd->player = car->player;
                    e->throttle.Load(child->FindChild("throttle"));
                    e->rpm.Load(child->FindChild("rpm"));
                    e->invRpm = 1.0f / child->GetFloat("sample_rpm");
                }
            }
        }
        fn_80131818(table);
        rpm = 0.0f;
        fadeIn = 0.0f;
    }
    virtual ~EngineSound(void);
    virtual void Update(int muted);

    EngineSample samples[6];  // offset 0x4
    s32 count;                // offset 0x13C
    Car* car;                 // offset 0x140
    f32 focusVolume;          // offset 0x144
    f32 otherVolume;          // offset 0x148
    f32 rpmFactor;            // offset 0x14C
    f32 rpm;                  // offset 0x150
    f32 fadeIn;               // offset 0x154
};

static inline f32 Smooth(f32 a) { return (3.0f - 2.0f * a) * (a * a); }

extern "C" {

EngineSound* fn_80209448(Car* car) { return new ("eng_snd") EngineSound(car); }

void fn_80209904(void) {
    int quiet = 0;
    if (IsLoadingActive() || fn_801B89F8()) {
        quiet = 1;
    }
    if (quiet) {
        f32 f = lbl_8055EDB8 - 1.0f / 59.94f;
        lbl_8055EDB8 = (0.0f > f) ? 0.0f : f;
    } else {
        f32 f = 1.0f / 59.94f + lbl_8055EDB8;
        lbl_8055EDB8 = (lbl_8055B4FC < f) ? lbl_8055B4FC : f;
    }
}

}  // extern "C"

EngineSound::~EngineSound(void) {
    for (int i = 0; i < count; i++) {
        delete samples[i].snd;
    }
    if (--lbl_8055EDBC == 0) {
        fn_80127D58(fn_80209904);
    }
}

void EngineSound::Update(int muted) {
    f32 r;
    f32 t;
    f32 vol;
    int n;
    s32 me;
    int local;
    int i;
    f32 rpmv = car->rpm;
    rpm = rpmFactor * rpmv;
    fadeIn = fadeIn + 0.05f * (1.0f - fadeIn);
    r = rpm;
    t = car->throttle;
    local = 0;
    n = fn_80178CA4();
    for (i = 0; i < n; i++) {
        me = car->player;
        if (fn_8026A344(i) == me) {
            local = 1;
            break;
        }
    }
    vol = local ? focusVolume : otherVolume;
    if (muted) {
        vol = 0.0f;
    }
    EngineSample* e = samples;
    for (int i = 0; i < count; e++, i++) {
        f32 a = 1.0f;
        if (r < e->rpm.lo) {
            a = (e->rpm.lo - r) * e->rpm.invLo;
            a = 1.0f - ((1.0f < a) ? 1.0f : a);
        } else if (r > e->rpm.hi) {
            a = (r - e->rpm.hi) * e->rpm.invHi;
            a = 1.0f - ((1.0f < a) ? 1.0f : a);
        }
        f32 ga = Smooth(a) * e->rpm.gain;
        f32 b = 1.0f;
        if (t < e->throttle.lo) {
            b = (e->throttle.lo - t) * e->throttle.invLo;
            b = 1.0f - ((1.0f < b) ? 1.0f : b);
        } else if (t > e->throttle.hi) {
            b = (t - e->throttle.hi) * e->throttle.invHi;
            b = 1.0f - ((1.0f < b) ? 1.0f : b);
        }
        f32 gb = Smooth(b) * e->throttle.gain;
        f32 level = lbl_8055EDB8 * ga * gb;
        e->snd->Play();
        f32 curInvRpm = e->invRpm;
        e->snd->SetPitch(r * curInvRpm);
        f32 d = level - e->level;
        if (d > 0.2f) {
            d = 0.2f;
        }
        d = (d > -0.2f) ? d : -0.2f;
        e->level += d;
        e->snd->SetVolume((0.5f + 0.5f * fadeIn) * (e->level * vol));
    }
}

// NonMatching: EngineSound::Update numbers its float registers differently (98 diffs, registers only).
