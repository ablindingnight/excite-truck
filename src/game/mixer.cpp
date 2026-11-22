// Mixer (0x80208A18-0x80209448): the list of sound devices ("mixers"), the effects volume setting and
// the volume handles that scale it, plus the null mixer and its null sounds used when there is no
// sound device. Names are guesses apart from the keys and messages.
#include "game/common.h"
#include "game/mixer.h"
#include "game/locstring.h"
#include "macros.h"

extern "C" {
void* memset(void* dst, int c, size_t n);
int strncmp(const char* a, const char* b, size_t n);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
s32 fn_802E2C20(void);                   // milliseconds
int fn_802E9A14(void);                   // sound mode
void fn_801331C4(int owner, const char* key, f32* value);
void SetOptionFloatValue(int owner, const char* key, f32 value);
void fn_801332D0(int owner, const char* key, s32* value);
void fn_80133348(int owner, const char* key, s32 value);
int fn_8013365C(const char* name, void (*cb)(void));
void fn_801336E0(int handle, void (*cb)(void));
void fn_8020A72C(void);
}

void IdentRegisterCallback(void (*cb)(int, int));
void* operator new(size_t size, const char* tag);

class LocString;
extern "C" {
}

class Sound {
public:
    Sound(void) {}
    virtual ~Sound(void) {}
    virtual void vf0C(void) { delete this; }
    virtual int vf10(void) = 0;
    virtual int vf14(void) { return 0; }
    virtual void Play(void) = 0;
    virtual void Start(void) = 0;
    virtual void Stop(void) = 0;
    virtual int State(void) = 0;
    virtual void Pause(void) = 0;
    virtual void Resume(void) = 0;
    virtual void vf30(void) = 0;
    virtual void vf34(void) = 0;
    virtual int vf38(void) = 0;
    virtual void vf3C(void) = 0;
    virtual void vf40(void) = 0;
    virtual void vf44(f32 v) {}
    virtual void vf48(f32 v) {}
    virtual void vf4C(f32 v) {}
    virtual void vf50(u32 mask) {}
    virtual void vf54(void) = 0;
    virtual void vf58(void) = 0;
    virtual void vf5C(void) = 0;
    virtual f32 vf60(void) { return 0.0f; }
};

// A sound that only keeps its state.
class NullSound : public Sound {
public:
    NullSound(void) { state = 3; }
    virtual ~NullSound(void) {}
    virtual int vf10(void) { return 1; }
    virtual void Play(void) {
        start = fn_802E2C20();
        state = 1;
    }
    virtual void Start(void) {
        start = fn_802E2C20();
        state = 1;
    }
    virtual void Stop(void) { state = 3; }
    virtual void Pause(void) { state = 2; }
    virtual void Resume(void) { state = 1; }
    virtual int State(void) {
        if (start + 2000 < fn_802E2C20()) {
            state = 3;
        }
        return state;
    }
    virtual void vf30(void) {}
    virtual void vf3C(void) {}
    virtual void vf40(void) {}
    virtual void vf34(void) {}
    virtual int vf38(void) { return 0; }
    virtual void vf54(void) {}
    virtual void vf58(void) {}
    virtual void vf5C(void) {}

    s32 start;  // offset 0x4
    s32 state;  // offset 0x8
};

ModuleTag lbl_8055EDA0;
extern LocString lbl_8048929C;

// The mixer used when there is no sound device.
class NullMixer : public Mixer {
public:
    NullMixer(void) { memset(data, 0, sizeof(data)); }
    virtual ~NullMixer(void);
    virtual const char* Name(void) { return lbl_8048929C.Get(); }
    virtual MixerCaps* Caps(void) { return (MixerCaps*)data; }
    virtual int vf14(void) { return 1; }
    virtual int Usable(void) { return 1; }
    virtual void SetListeners(void* listeners) {}
    virtual void Shutdown(void) {}
    virtual Sound* CreateSound(const char* name, int mode, int unscaled) { return new ("Null Sound") NullSound; }

    u32 data[3];  // offset 0x4: the caps
};

struct VolumeHandle {
    f32 volume;   // offset 0x0
    void* owner;  // offset 0x4
};

struct VolumeHandles {
    VolumeHandle h[8];  // offset 0x0
    s32 count;          // offset 0x40
};

struct MixerList {
    Mixer* list[6];  // offset 0x0
    s32 count;       // offset 0x18
    s32 current;     // offset 0x1C
};

f32 lbl_8055B4D0 = 1.0f;  // effects volume setting
f32 lbl_8055B4D4 = 1.0f;  // effective effects volume
s32 lbl_8055B4D8 = 1;     // sound mode
int lbl_8055B4DC = -1;    // settings callback handle
s32 lbl_8055EDA8;         // quality
MixerList lbl_80489238;
VolumeHandles lbl_80489258;
LocString lbl_8048929C("Sound:NoSound");

extern "C" {

void SetEffectsVolume(f32 v);
void SetSoundOption(s32 q);

f32 ApplyVolumeModifiers(f32 v) {
    VolumeHandle* h = lbl_80489258.h;
    for (int i = 0; i < lbl_80489258.count; i++, h++) {
        if (h->owner) {
            v *= h->volume;
        }
    }
    return v;
}

void fn_80208A50(void) { lbl_8055B4D4 = ApplyVolumeModifiers(lbl_8055B4D0); }

void LoadAudioSettingsFromConfig(void) {
    fn_801331C4(4, "effects_volume", &lbl_8055B4D0);
    SetEffectsVolume(lbl_8055B4D0);
    fn_801332D0(4, "quality", &lbl_8055EDA8);
    SetSoundOption(lbl_8055EDA8);
}

void UpdateSoundVolumes(int ident, int event) {
    f32 v = 1.0f;
    switch (event) {
    case 3:
        fn_801331C4(ident + 0xC, "player_sfx_vol", &v);
        SetOptionFloatValue(4, "effects_volume", v);
        SetEffectsVolume(v);
        break;
    case 1:
    case 2:
        fn_801331C4(4, "effects_volume", &v);
        SetOptionFloatValue(ident + 0xC, "player_sfx_vol", v);
        break;
    case 0:
        SetOptionFloatValue(4, "effects_volume", v);
        SetEffectsVolume(v);
        break;
    }
}

void InitializeAudioMixer(int create) {
    switch (fn_802E9A14()) {
    default:
    case 1:
        lbl_8055B4D8 = 1;
        break;
    case 2:
        lbl_8055B4D8 = 2;
        break;
    case 0:
        lbl_8055B4D8 = 0;
        break;
    }
    lbl_80489258.count = 0;
    if (create) {
        memset(&lbl_80489238, 0, sizeof(lbl_80489238));
        Mixer* m = new ("Null Mixer") NullMixer;
        lbl_80489238.list[lbl_80489238.count] = m;
        lbl_80489238.count++;
        fn_8020A72C();
    }
    fn_801332D0(4, "mixer", &lbl_80489238.current);
    if (lbl_80489238.current < 0 || lbl_80489238.current >= lbl_80489238.count) {
        lbl_80489238.current = 0;
    }
    SetOptionFloatValue(4, "effects_volume", 1.0f);
    LoadAudioSettingsFromConfig();
    lbl_8055B4DC = fn_8013365C("Mixer", LoadAudioSettingsFromConfig);
    IdentRegisterCallback(UpdateSoundVolumes);
}

s32 fn_80208D3C(void) { return lbl_80489238.count; }

Mixer* GetMixerByIndex(int i) { return lbl_80489238.list[i]; }

void SetCurrentSoundOption(s32 i) { lbl_80489238.current = i; }

s32 fn_80208D70(void) { return lbl_80489238.current; }

void fn_80208D80(int destroy) {
    Mixer** m;
    VolumeHandle* h;
    int i;
    int j;
    h = lbl_80489258.h;
    for (i = 0; i < lbl_80489258.count; h++, i++) {
        if (h->owner) {
            fn_802E317C("mixer: \"%s\" never destroyed their volume handle", h->owner);
        }
    }
    if (destroy) {
        m = lbl_80489238.list;
        for (j = 0; j < lbl_80489238.count; m++, j++) {
            delete *m;
        }
    }
    fn_80133348(4, "mixer", lbl_80489238.current);
    SetOptionFloatValue(4, "effects_volume", lbl_8055B4D0);
    fn_80133348(4, "quality", lbl_8055EDA8);
    fn_801336E0(lbl_8055B4DC, LoadAudioSettingsFromConfig);
}

void ProcessSoundMixer(Mixer* m, int dontCare) {
    if (strncmp(m->Name(), "Dream 9407,", 11) == 0) {
        fn_802E30C8("tossing bogus mixer: %s", m->Name());
        delete m;
    } else if ((u32)lbl_80489238.count < 6) {
        int ok = dontCare == 0;
        if (dontCare == 1 && m->Usable()) {
            ok = 1;
            m->Shutdown();
        }
        if (ok) {
            lbl_80489238.list[lbl_80489238.count++] = m;
        } else {
            delete m;
        }
    } else {
        fn_802E30C8("too many sound devices: %s", m->Name());
        delete m;
    }
}

// Dead-stripped in the game; kept so their strings land in the pool in the original order.
const char* MixerLocationName(int loc) {
    switch (loc) {
    case 0:
        return "LOC_HARDWARE3D";
    case 1:
        return "LOC_HARDWARE";
    case 2:
        return "LOC_SOFTWARE";
    }
    return "LOC_DONTCARE";
}

const char* MixerQualityName(int q) {
    if (q == 0) {
        return "Sound:Quality:Great";
    }
    if (q == 1) {
        return "Sound:Quality:Awesome";
    }
    return "Sound:Quality:Incredible";
}

f32 fn_80209028(void) { return lbl_8055B4D4; }

void SetEffectsVolume(f32 v) {
    f32 t = MIN(1.0f, v);
    lbl_8055B4D0 = MAX(t, 0.0f);
    SetOptionFloatValue(4, "effects_volume", lbl_8055B4D0);
    fn_80208A50();
}

void SetSoundOption(s32 q) {
    if (q >= 0 && q <= 2) {
        lbl_8055EDA8 = q;
    } else {
        lbl_8055EDA8 = 0;
    }
}

s32 GetSoundChannelCount(void) { return lbl_8055EDA8; }

void fn_802090BC(int) { fn_802E30C8("mixer: MixerSetSoundMode is a NOOP"); }

// Dead-stripped in the game (sound mode name).
const char* MixerSoundModeName(void) { return "Surround"; }

s32 GetMasterVolume(void) { return lbl_8055B4D8; }

VolumeHandle* AllocateVolumeHandle(void* owner) {
    VolumeHandle* h = NULL;
    if ((u32)lbl_80489258.count < 8) {
        h = &lbl_80489258.h[lbl_80489258.count++];
        h->owner = owner;
        h->volume = 1.0f;
    }
    return h;
}

void ClearAudioHandle(VolumeHandle** ph) {
    if (*ph) {
        (*ph)->owner = NULL;
        while (lbl_80489258.count > 0) {
            if (lbl_80489258.h[lbl_80489258.count - 1].owner) {
                break;
            }
            lbl_80489258.count--;
        }
        fn_80208A50();
        *ph = NULL;
    }
}

void SetVolumeClamped(VolumeHandle* h, f32 v) {
    if (h) {
        f32 t = MIN(1.0f, v);
        h->volume = MAX(t, 0.0f);
        fn_80208A50();
    }
}

f32 fn_802091CC(void) { return ApplyVolumeModifiers(1.0f); }

}  // extern "C"

NullMixer::~NullMixer(void) {}

int Mixer::vf14(void) { return 0; }

// NonMatching (link): NullSound's vtable slot 0x14 is the shared `return 0` fn_801B3FD0 (defined in
// collisionsound.cpp); our Sound::vf14 has no symbol to bind to. Code is 100%.

