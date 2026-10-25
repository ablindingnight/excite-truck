// Sounds (0x802062E8-0x80207960): starting and stopping the sound system, the game sound classes
// (2D, 3D with distance, panning and doppler from the listeners, and the "dash" sounds heard only by
// one player) and their state machine against the device sound, plus defaults of the Mixer and Sound
// interfaces. Names are guesses apart from the class names, pool and messages.
#include "game/common.h"

extern "C" {
char* strncpy(char* dst, const char* src, size_t n);
void fn_802E30C8(const char* fmt, ...);  // debug print
f32 PSVECMag(const ValVec3* v);
f32 PSVECDotProduct(const ValVec3* a, const ValVec3* b);
extern u32 lbl_8055D430;  // debug flags
void fn_8020A368(void);   // music init
void fn_8020A400(void);   // music shutdown
}

void* operator new(size_t size, const char* tag);

class Pool {
public:
    Pool(const char* name, s32 count, s32 size);
    virtual ~Pool(void);
    void* Alloc(void);
    void Free(void* p);

    u8 pad4[0x1C];
};

// A device sound (see mixer.cpp).
class Sound {
public:
    virtual ~Sound(void) {}
    virtual void vf0C(void) { delete this; }
    virtual int vf10(void) = 0;
    virtual void vf14(void);
    virtual void Play(void) = 0;
    virtual void Start(void) = 0;
    virtual void Stop(void) = 0;
    virtual int State(void) = 0;
    virtual void Pause(void) = 0;
    virtual void Resume(void) = 0;
    virtual void SetPitch(f32 v) = 0;
    virtual void SetVolume(f32 v) = 0;
    virtual int Has3D(void) = 0;
    virtual void SetPan(f32 v) = 0;
    virtual void SetDepth(f32 v) = 0;
    virtual void vf44(f32 v);
    virtual void vf48(f32 v);
    virtual void vf4C(f32 v);
    virtual void vf50(u32 mask);
    virtual void Set3D(void* vel, void* pos) = 0;
    virtual void vf58(void) = 0;
    virtual void Event(s32* ev) = 0;
    virtual f32 vf60(void);
};

class Mixer {
public:
    virtual ~Mixer(void);
    virtual const char* Name(void) = 0;
    virtual void* Caps(void) = 0;
    virtual int vf14(void);
    virtual int Usable(void) = 0;
    virtual Sound* CreateSound(void* desc, int mode, int loop) = 0;
    virtual void SetListeners(void* listeners) = 0;
    virtual void Shutdown(void) = 0;
    virtual void vf28(void);
    virtual void vf2C(int a);
    virtual void vf30(void);
    virtual void vf34(int a);
    virtual void vf38(void);
    virtual void vf3C(void);
    virtual void vf40(void);
    virtual void vf44(void);
};

struct Listener {
    s32 id;          // offset 0x0
    ValMatrix* mtx;  // offset 0x4
    ValVec3* vel;    // offset 0x8
};

class GameSound;

class SoundManager {
public:
    SoundManager(Mixer* mixer);
    virtual ~SoundManager(void);
    virtual void Update(void);
    void Remove(GameSound* s);
    void Add2(GameSound* s);
    void Add3(GameSound* s);
    Sound* Create2(void* desc, int cls);
    Sound* Create3(void* desc, int cls);
    void ReleaseSound(Sound* s);
    void SetListener(const Listener& l, int i);
    void SetListenerCount(s32 n);

    u8 enabled;      // offset 0x4
    u8 primaryOnly;  // offset 0x5
    u8 pad6[0x112];
};

extern "C" {
void fn_80208B8C(int create);
Mixer* fn_80208D4C(int i);
s32 fn_80208D70(void);
void fn_80208D80(int destroy);
}


class GameSound;
extern "C" void fn_80206750(GameSound* s, s32 player, int on);

class GameSound {
public:
    GameSound(const char* name, int cls);
    virtual ~GameSound(void);
    virtual void Update(Listener* l, int n);
    virtual void Apply(Listener* l, int n);
    virtual void PreUpdate(Listener* l, int n);

    void Cull(Listener* l, int n);
    void Step(Listener* l, int n);

    f32 pitch;        // offset 0x4
    f32 volume;       // offset 0x8
    Sound* dev;       // offset 0xC
    s32 player;       // offset 0x10
    u32 mask;         // offset 0x14
    u8 active;        // offset 0x18
    u8 playing;       // offset 0x19
    f32 unk1C;        // offset 0x1C
    f32 unk20;        // offset 0x20
    f32 length;       // offset 0x24
    f32 param[3];     // offset 0x28
    u8 paramDirty[3]; // offset 0x34
    char name[0xD];   // offset 0x37
    s32 cls;          // offset 0x44
    u8 paused;        // offset 0x48
    u8 start;         // offset 0x49
    u8 stop;          // offset 0x4A
    u8 loop;          // offset 0x4B
    u8 restart;       // offset 0x4C
    u8 volumeDirty;   // offset 0x4D
    u8 pitchDirty;    // offset 0x4E
    u8 maskDirty;     // offset 0x4F
    s32 event;        // offset 0x50
};

class Sound2D : public GameSound {
public:
    Sound2D(const char* name, int cls);
    virtual ~Sound2D(void) {}
    virtual void Update(Listener* l, int n);

    u8 pad54[0x10];
    f32 pan;        // offset 0x64
    f32 depth;      // offset 0x68
    u8 panDirty;    // offset 0x6C
    u8 depthDirty;  // offset 0x6D
};

class Sound3D : public GameSound {
public:
    Sound3D(const char* name, int cls, ValVec3* vel, ValVec3* pos);
    virtual ~Sound3D(void) {}
    virtual void Update(Listener* l, int n);
    virtual void PreUpdate(Listener* l, int n);
    static void* operator new(size_t size, const char* tag);
    static void operator delete(void* p);

    u8 pad54[0x10];
    u8 audible;     // offset 0x64
    f32 gain;       // offset 0x68
    f32 radius;     // offset 0x6C
    ValVec3* vel;   // offset 0x70
    ValVec3* pos;   // offset 0x74
};

class SoundDash : public Sound2D {
public:
    SoundDash(const char* name, int cls, s32 player, int on);
    virtual void Update(Listener* l, int n);
    virtual void PreUpdate(Listener* l, int n);

    bool heard;  // offset 0x6E
};

ModuleTag lbl_8055ED78;
SoundManager* lbl_8055ED80;  // sound manager
u8 lbl_8055ED84;             // no sound devices
s32 lbl_8055ED88;            // players
s32 lbl_804891E8[4];         // player ids
s32 lbl_8055ED8C;            // live 3D sounds
Pool* lbl_8055ED90;          // 3D sound pool
s32 lbl_8055ED94;            // failed grabs

extern "C" {

int fn_802062E8(void) {
    fn_80208B8C(lbl_8055ED84 == 0);
    fn_802E30C8("using sound mixer %s", fn_80208D4C(fn_80208D70())->Name());
    lbl_8055ED80 = new ("SoundManager") SoundManager(fn_80208D4C(fn_80208D70()));
    if (lbl_8055ED80) {
        if (lbl_8055ED80->enabled) {
            fn_8020A368();
            return 1;
        }
        delete lbl_8055ED80;
        lbl_8055ED80 = NULL;
    }
    fn_80208D80(lbl_8055ED84 == 0);
    return 0;
}

void fn_802063D8(void) {
    fn_8020A400();
    delete lbl_8055ED80;
    lbl_8055ED80 = NULL;
    fn_80208D80(lbl_8055ED84 == 0);
}

int fn_80206430(void) { return lbl_8055ED80 != NULL; }

void fn_80206444(void) { lbl_8055ED80->primaryOnly = 1; }

void fn_80206454(void) { lbl_8055ED80->primaryOnly = 0; }

void fn_80206464(s32 n) { lbl_8055ED88 = n; }

void fn_8020646C(int i, s32 id) { lbl_804891E8[i] = id; }

}  // extern "C"

void Mixer::vf3C(void) {}

extern "C" void fn_80206484(int a) { fn_80208D4C(fn_80208D70())->vf2C(a); }

void Mixer::vf2C(int a) {}

void Mixer::vf30(void) {}

extern "C" void fn_802064D0(int a) { fn_80208D4C(fn_80208D70())->vf34(a); }

void Mixer::vf34(int a) {}

extern "C" void fn_80206518(void) { fn_80208D4C(fn_80208D70())->vf38(); }

void Mixer::vf38(void) {}

extern "C" {

void fn_80206550(s32 n) { lbl_8055ED80->SetListenerCount(n); }

void fn_8020655C(const Listener& l, int i) { lbl_8055ED80->SetListener(l, i); }

const char* fn_80206570(int cls) {
    const char* names[10] = {
        "SNDCLASS_UI",           "SNDCLASS_COUNTDOWN",  "SNDCLASS_ENGINE_2D", "SNDCLASS_ENGINE",
        "SNDCLASS_TIRESQUEAL",   "SNDCLASS_CRASH",      "SNDCLASS_ENGINE_START",
        "SNDCLASS_GEARCHANGE",   "SNDCLASS_WIND",       "SNDCLASS_COUNT",
    };
    if (cls < 10) {
        return names[cls];
    }
    return "<invalid>";
}

}  // extern "C"

GameSound::GameSound(const char* name, int cls) {
    strncpy(this->name, name, 0xC);
    this->name[0xC] = 0;
    dev = NULL;
    this->cls = cls;
    pitch = 1.0f;
    volume = 1.0f;
    start = 0;
    paused = 0;
    stop = 1;
    restart = 0;
    loop = 0;
    pitchDirty = 1;
    volumeDirty = 1;
    maskDirty = 1;
    player = -100;
    mask = 0;
    active = 0;
    playing = 0;
    unk20 = 0.0f;
    unk1C = 1.0f;
    length = 0.0f;
    event = 0;
    param[0] = 0.0f;
    paramDirty[0] = 1;
    param[1] = 0.0f;
    paramDirty[1] = 1;
    param[2] = 0.0f;
    paramDirty[2] = 1;
}

GameSound::~GameSound(void) {
    if (dev) {
        lbl_8055ED80->Remove(this);
        lbl_8055ED80->ReleaseSound(dev);
        dev = NULL;
    }
}

extern "C" void fn_80206750(GameSound* s, s32 player, int on) {
    int idx = -1;
    for (int i = 0; i < lbl_8055ED88; i++) {
        if (player == lbl_804891E8[i]) {
            idx = i;
            break;
        }
    }
    if (idx >= 0 && on) {
        s->maskDirty = 1;
        s->mask = 1 << idx;
    } else {
        s->mask = 0;
        s->maskDirty = 1;
    }
}

extern "C" void fn_802067C8(GameSound* s, Listener* l, int n) {
    int music = s->cls == 0;
    int on = s->playing && (s->volume > 0.05f || music);
    s->active = on;
    if (on) {
        s->PreUpdate(l, n);
    }
}

void GameSound::PreUpdate(Listener* l, int n) {}

extern "C" void fn_80206824(GameSound* s, Listener* l, int n) {
    int state = s->dev->State();
    s->playing = 1;
    switch (state) {
    case 1:
        if (s->stop) {
            s->dev->Stop();
        } else if (s->paused) {
            s->dev->Pause();
        } else if (s->restart) {
            s->restart = 0;
            s->dev->Stop();
        }
        break;
    case 3:
        if (s->start) {
            int muted = (lbl_8055D430 & 1) && (lbl_8055D430 & 0x200);
            if (!muted || s->cls == 0) {
                s->start = 0;
                if (s->loop) {
                    s->dev->Start();
                } else {
                    s->dev->Play();
                }
                goto paused;
            }
        }
        s->playing = 0;
    paused:
        if (s->paused) {
            s->dev->Pause();
        }
        break;
    case 2:
        if (!s->paused) {
            if (s->stop) {
                s->dev->Stop();
            } else {
                s->dev->Resume();
            }
        }
        break;
    }
    if (s->event) {
        s->dev->Event(&s->event);
        s->event = 0;
    }
    if (!s->paused) {
        s->Update(l, n);
    }
}

void GameSound::Apply(Listener* l, int n) {
    if (paramDirty[0]) {
        paramDirty[0] = 0;
        dev->vf44(param[0]);
    }
    if (paramDirty[1]) {
        paramDirty[1] = 0;
        dev->vf48(param[1]);
    }
    if (paramDirty[2]) {
        paramDirty[2] = 0;
        dev->vf4C(param[2]);
    }
}

void Sound::vf4C(f32 v) {}

void Sound::vf48(f32 v) {}

void Sound::vf44(f32 v) {}

void GameSound::Update(Listener* l, int n) {
    if (volumeDirty) {
        volumeDirty = 0;
        dev->SetVolume(volume);
    }
    if (pitchDirty) {
        pitchDirty = 0;
        dev->SetPitch(pitch);
    }
    if (maskDirty) {
        maskDirty = 0;
        dev->vf50(mask);
    }
    Apply(l, n);
}

void Sound::vf50(u32 mask) {}

void* Sound3D::operator new(size_t size, const char* tag) {
    if (lbl_8055ED8C == 0) {
        lbl_8055ED90 = new ("Sound3D::pool") Pool("Sound3D::pool", 0xC0, 0x78);
    }
    void* p = lbl_8055ED90->Alloc();
    if (p) {
        lbl_8055ED8C++;
    }
    return p;
}

void Sound3D::operator delete(void* p) {
    lbl_8055ED90->Free(p);
    if (--lbl_8055ED8C == 0) {
        delete lbl_8055ED90;
        lbl_8055ED90 = NULL;
    }
}

static f32 Attenuation(const ValVec3* d, f32 radius) {
    f32 front = 0.75f * d->z;
    f32 ahead = (0.0f > front) ? 0.0f : front;
    f32 dist = PSVECMag(d) - ahead;
    return radius / ((dist > radius) ? dist : radius);
}

static inline void Sub(ValVec3* out, const ValVec3& a, const ValVec3& b) {
    out->x = a.x - b.x;
    out->y = a.y - b.y;
    out->z = a.z - b.z;
}

void Sound3D::PreUpdate(Listener* l, int n) {
    audible = 0;
    gain = 0.0f;
    if (active) {
        Listener* p = l;
        for (int i = 0; i < n; p++, i++) {
            ValMatrix* m = p->mtx;
            ValVec3 d;
            Sub(&d, *pos, m->origin);
            f32 nx = d.z * m->basis[0].z + (d.x * m->basis[0].x + d.y * m->basis[0].y);
            f32 ny = d.z * m->basis[1].z + (d.x * m->basis[1].x + d.y * m->basis[1].y);
            f32 nz = d.z * m->basis[2].z + (d.x * m->basis[2].x + d.y * m->basis[2].y);
            d.x = nx;
            d.y = ny;
            d.z = nz;
            if (PSVECDotProduct(&d, &d) < (200.0f + radius) * (200.0f + radius)) {
                f32 v = volume;
                audible = 1;
                v = (v < 1.0f) ? v : 1.0f;
                f32 g = v * Attenuation(&d, radius);
                f32 cur = gain;
                gain = (cur > g) ? cur : g;
            }
        }
        if (!audible) {
            active = 0;
        }
    }
}

void Sound3D::Update(Listener* l, int n) {
    int i;
    if (audible) {
        if (dev->Has3D()) {
            GameSound::Update(l, n);
            dev->Set3D(vel, pos);
        } else {
            Apply(l, n);
            dev->SetVolume(gain);
            if (playing) {
                f32 wsum = 0.0f;
                f32 pan = 0.0f;
                f32 depth = 0.0f;
                f32 doppler = 0.0f;
                for (i = 0; i < n; l++, i++) {
                    ValVec3 lp = l->mtx->origin;
                    ValVec3 lv = *l->vel;
                    ValVec3 d;
                    Sub(&d, *pos, lp);
                    f32 dist = PSVECMag(&d);
                    if (dist > 0.1f) {
                        f32 w = 1.0f / dist;
                        d.x *= w;
                        d.y *= w;
                        d.z *= w;
                        wsum += w;
                        f32 p = PSVECDotProduct(&d, &l->mtx->basis[0]);
                        if (dist < 1.5f) {
                            p *= dist / 1.5f;
                        }
                        pan += p * w;
                        f32 q = PSVECDotProduct(&d, &l->mtx->basis[2]);
                        if (dist < 1.5f) {
                            q = 1.0f + (dist / 1.5f) * (q - 1.0f);
                        }
                        depth += q * w;
                        ValVec3 rv;
                        Sub(&rv, *vel, lv);
                        f32 r = 339.97f / (339.97f + PSVECDotProduct(&rv, &d));
                        doppler += r * w;
                    }
                }
                wsum = (wsum > 0.0001f) ? wsum : 0.0001f;
                f32 inv = 1.0f / wsum;
                dev->SetPitch(pitch * (doppler * inv));
                f32 t = pan * inv;
                t = (1.0f < t) ? 1.0f : t;
                t = (t > -1.0f) ? t : -1.0f;
                dev->SetPan(t);
                t = depth * inv;
                t = (1.0f < t) ? 1.0f : t;
                t = (t > -1.0f) ? t : -1.0f;
                dev->SetDepth(t);
            }
        }
    }
}

Sound3D::Sound3D(const char* name, int cls, ValVec3* vel, ValVec3* pos) : GameSound(name, cls) {
    gain = 0.0f;
    radius = 4.5f;
    audible = 1;
    this->vel = vel;
    this->pos = pos;
    dev = lbl_8055ED80->Create3((void*)name, cls);
    if (dev) {
        lbl_8055ED80->Add3(this);
    }
    length = dev ? dev->vf60() : 0.0f;
}

f32 Sound::vf60(void) { return 0.0f; }

Sound2D::Sound2D(const char* name, int cls) : GameSound(name, cls) {
    pan = 0.0f;
    panDirty = 1;
    depth = 1.0f;
    depthDirty = 1;
    dev = lbl_8055ED80->Create2((void*)name, cls);
    if (dev) {
        lbl_8055ED80->Add2(this);
    }
    length = dev ? dev->vf60() : 0.0f;
}

void Sound2D::Update(Listener* l, int n) {
    GameSound::Update(l, n);
    if (panDirty) {
        panDirty = 0;
        dev->SetPan(pan);
    }
    if (depthDirty) {
        depthDirty = 0;
        dev->SetDepth(depth);
    }
}

SoundDash::SoundDash(const char* name, int cls, s32 player, int on) : Sound2D(name, cls) {
    this->player = player;
    fn_80206750(this, player, on);
}

void SoundDash::PreUpdate(Listener* l, int n) {
    heard = false;
    for (int i = 0; i < n; i++) {
        heard = heard | (l[i].id == player);
    }
    active = (active & heard) != 0;
}

void SoundDash::Update(Listener* l, int n) { Sound2D::Update(l, n); }

extern "C" {

SoundDash* fn_802076A0(const char* name, int cls, s32 player, int on) {
    SoundDash* s = new ("SoundDash") SoundDash(name, cls, player, on);
    if (s && !s->dev) {
        delete s;
        return NULL;
    }
    return s;
}

Sound3D* fn_802077A4(const char* name, int cls, ValVec3* vel, ValVec3* pos) {
    Sound3D* s = new ("Sound3D") Sound3D(name, cls, vel, pos);
    if (s && !s->dev) {
        delete s;
        return NULL;
    }
    return s;
}

Sound2D* fn_80207848(const char* name, int cls) {
    Sound2D* s = new ("Sound") Sound2D(name, cls);
    if (s && !s->dev) {
        delete s;
        return NULL;
    }
    return s;
}

}  // extern "C"


extern "C" {

Sound2D* fn_802078CC(const char* name, int cls, void* mem) {
    Sound2D* s = new (mem) Sound2D(name, cls);
    if (s->dev) {
        return s;
    }
    return NULL;
}

void fn_80207924(GameSound* s) { s->~GameSound(); }

void fn_80207938(const char* name) { fn_802E30C8("sound: %08d: grab failed (%s)", ++lbl_8055ED94, name); }

}  // extern "C"

// NonMatching: SoundDash::PreUpdate keeps "heard" in a register across the unrolled copies in the
// target, and the Sound3D updates schedule their
// float loads differently.
