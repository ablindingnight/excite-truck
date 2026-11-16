// Code 0x8020A6AC-0x8020BCE4: the voice sound mixer (streamed voice/sample sounds) and its sounds, plus the
// sample volume table loaded from sounds.val. Imported from the upstream project (github.com/orangedude27/excite-truck,
// CC0; file src/auto/text_8020A6AC_.c) as C and rewritten as the classes its vtables prove. Unit name is provisional;
// class names follow the unit's own strings ("VoiceSoundMixer::End", "VoiceSound::pool"); the rest are guesses.
#include "game/common.h"
#include "game/pool.h"

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal
int strcasecmp(const char* a, const char* b);
int strcmp(const char* a, const char* b);
char* strchr(const char* s, int c);
void* memset(void* dst, int c, size_t n);
f32 fn_80209028(void);
f32 fn_802091CC(void);
int fn_802090CC(void);
s32 fn_802E2C20(void);
void fn_802E625C(int mode);
void fn_802E6264(void);
void fn_802E6B04(void);
void fn_802E6B34(void);
void fn_802E58B8(int a);
void fn_802E5950(int a, int b);
void fn_802E59BC(void* a);
void fn_802E5A54(void);
void fn_802E5824(void);
void fn_802E5828(void);
void* fn_802E5AE8(const char* name);
void fn_802E5D5C(void* stream);
f32 fn_802E6040(void* stream);
void* fn_802E6058(void* stream);
const char* fn_802E6ABC(void* stream);
void fn_80207938(const char*);
void fn_802E6138(void* handle);
void fn_802E61A8(void* handle);
void fn_802E61BC(void* handle);
void fn_802E61D8(void* handle);
void fn_802E61E4(void* handle, f32 v);
void fn_802E61EC(void* handle, f32 v);
void fn_802E620C(void* handle, f32 v);
void fn_802E622C(void* handle, f32 v);
void fn_802E6234(void* handle, f32 v);
void fn_802E6238(void* handle, f32 v);
void fn_802E623C(void* handle, f32 v);
void fn_802E6240(void* handle, int v);
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* table);
}

int OptNoSound(void);
void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

// One entry of the sample table: a sample name and its volume.
struct SoundSample {
    char name[0x20];  // offset 0x0
    f32 volume;       // offset 0x20
};

// What a mixer supports (field names from soundmgr.cpp; guesses).
struct MixerCaps {
    u8 noPositional;  // offset 0x0
    s32 has3D;        // offset 0x4
    s32 hasPan;       // offset 0x8 (the voice mixer reports 0x20)
};

// A mixer's sound (only the slots this unit overrides are named; slot 0x14 rebinds a sound to another sample).
class Sound {
public:
    Sound(void) {}
    virtual ~Sound(void) {}
    virtual void vf0C(void) { delete this; }
    virtual int vf10(void) = 0;
    virtual int vf14(const char* name);
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

// A sound device (mixer.cpp). Slot signatures follow this unit's overrides (sound.cpp names slot 0x1C
// CreateSound(desc, mode, loop) and declares vf30/vf34 with other parameters).
class Mixer {
public:
    Mixer(void) {}
    virtual ~Mixer(void) {}
    virtual const char* Name(void) = 0;
    virtual MixerCaps* Caps(void) = 0;
    virtual int vf14(void);
    virtual int Usable(void) = 0;
    virtual Sound* CreateSound(const char* name, int mode, u8 unscaled) = 0;
    virtual void SetListeners(void* listeners) = 0;
    virtual void Shutdown(void) = 0;
    virtual void vf28(void);
    virtual void vf2C(int a);
    virtual void vf30(int a, int b);
    virtual void vf34(void* a);
    virtual void vf38(void);
    virtual void vf3C(void);
    virtual void vf40(void);
    virtual void vf44(void);
};

extern "C" {
extern MixerCaps lbl_80489BD0;
extern int lbl_8055EDF8;
extern int lbl_8055EDFC;
extern unsigned int lbl_8055EE00;
extern s32 lbl_8055EE08;
extern int lbl_8055EE0C;
extern Pool* lbl_8055EE10;
extern f32 __float_epsilon[];
extern SoundSample lbl_804892D0[64];
}

class VoiceSound;

// The voice mixer: 512 sound slots.
class VoiceSoundMixer : public Mixer {
public:
    VoiceSoundMixer(void);
    virtual void vf3C(void) {}
    virtual ~VoiceSoundMixer(void) {}
    virtual const char* Name(void) { return "Voice Mixer"; }
    virtual MixerCaps* Caps(void) {
        MixerCaps* caps = &lbl_80489BD0;
        caps->noPositional = 0;
        caps->has3D = 0;
        caps->hasPan = 0x20;
        return caps;
    }
    virtual int Usable(void);
    virtual Sound* CreateSound(const char* name, int mode, u8 unscaled);
    virtual void SetListeners(void* listeners);
    virtual void Shutdown(void);
    virtual void vf28(void);
    virtual void vf2C(int a);
    virtual void vf30(int a, int b);
    virtual void vf34(void* a);
    virtual void vf38(void);
    virtual void vf40(void) { fn_802E5824(); }
    virtual void vf44(void) { fn_802E5828(); }
    void FreeSlot(Sound* s, int slot);

    VoiceSound** voices;  // offset 0x4: 0x200 slots
    s32 lastTime;         // offset 0x8: last update time (ms)
};

extern "C" {
extern VoiceSoundMixer* lbl_8055EE04;
extern f32 lbl_8055B590;
extern f32 lbl_8055B594;
extern f32 lbl_8055B598;
SoundSample* fn_8020A6AC(const char* name);
}

// A voice: a stream played through one sound handle, or two (left and right) for stereo samples.
class VoiceSound : public Sound {
public:
    static void* operator new(size_t size, const char* tag);
    static void operator delete(void* p);
    VoiceSound(const char* name, int slot, u8 unscaled);
    virtual ~VoiceSound(void);
    virtual int vf10(void);
    virtual void vf0C(void) { delete this; }
    virtual void Play(void) {
        paused = 0;
        Begin();
    }
    virtual void Start(void) {
        paused = 1;
        Begin();
    }
    virtual int vf14(const char* name) {
        int result = 0;
        void* created = fn_802E5AE8(name);
        if (created != 0) {
            fn_802E5D5C(stream0);
            stream0 = created;
            sample = fn_8020A6AC(name);
            result = 1;
        } else {
            fn_802E30C8("voicesnd: Can't rebind to %s", name);
        }
        return result;
    }
    virtual void Stop(void);
    // Accessor name unproven: State() tests through it and then returns the field directly, which is why
    // retail loads the state twice (this used to be forced with a volatile pointer).
    int Playing(void) const { return state == 1; }
    virtual int State(void);
    virtual void Pause(void) {
        if (muted == 0 && state != 3) {
            muted = 1;
            lbl_8055EDFC++;
            Release();
            if (handle0 != 0) fn_802E61E4(handle0, 0.0f);
            if (handle1 != 0) fn_802E61E4(handle1, 0.0f);
        }
    }
    // Unmutes a voice; a playing voice that lost its handles is reopened and restarted.
    virtual void Resume(void) {
        if (muted) {
            muted = 0;
            if (state == 1 && handle0 == 0) {
                Open();
                if (handle0 != 0) {
                    Apply();
                }
            }
        }
    }
    virtual void SetPitch(f32 v) { pitch = v; }
    virtual void SetVolume(f32 v) { volume = v; }
    virtual int Has3D(void) { return 0; }
    virtual void SetPan(f32 v) { pan = v; }
    virtual void SetDepth(f32 v) { param28 = v; }
    virtual void vf44(f32 v) { param2C = v; }
    virtual void vf48(f32 v) { param30 = v; }
    virtual void vf4C(f32 v) { param34 = v; }
    virtual void vf50(u32 mask) { param38 = mask; }
    virtual void Set3D(void* vel, void* pos) { fn_802E317C("SndVoice::Set3D"); }
    virtual f32 vf60(void) { return length; }
    virtual void vf58(void) {}
    virtual void Event(s32* ev) {}

    // Stops both handles and closes them.
    /* Helper name unproven */
    void Release(void) {
        if (handle0 != 0) {
            fn_802E61D8(handle0);
            if (handle1 != 0) fn_802E61D8(handle1);
            if (handle1 != 0) {
                fn_802E6138(handle1);
                handle1 = 0;
            }
            if (handle0 != 0) {
                fn_802E6138(handle0);
                handle0 = 0;
            }
        }
    }

    // Opens a handle for each stream that does not have one yet.
    /* Helper name unproven */
    void Open(void) {
        if (stream0 != 0 && handle0 == 0) {
            handle0 = fn_802E6058(stream0);
            if (handle0 == 0) {
                fn_80207938(fn_802E6ABC(stream0));
            } else if (stream1 != 0) {
                handle1 = fn_802E6058(stream1);
                if (handle1 == 0) {
                    fn_802E6138(handle0);
                    handle0 = 0;
                }
            }
        }
    }

    // Applies volume, pan and effect parameters to the open handles, then plays or pauses them.
    /* Helper name unproven */
    void Apply(void) {
        int i;
        f32 gain;
        f32 v;
        f32 side;

        v = volume * sample->volume;
        if (muted) {
            gain = 0.0f;
        } else {
            gain = lbl_8055B598 * (unscaled ? v : v * lbl_8055B590);
        }
        if (handle1 != 0) {
            side = 1.0f + -pan;
            side = 1.0f < side ? 1.0f : side;
            side = side > 0.0f ? side : 0.0f;
            fn_802E61E4(handle0, gain * side);
            fn_802E61EC(handle0, (-1.0f));
            side = 1.0f + pan;
            side = 1.0f < side ? 1.0f : side;
            side = side > 0.0f ? side : 0.0f;
            fn_802E61E4(handle1, gain * side);
            fn_802E61EC(handle1, 1.0f);
        } else {
            fn_802E61E4(handle0, gain);
            fn_802E61EC(handle0, pan);
        }
        for (i = 0; i < 2; i++) {
            void* handle = i == 0 ? handle0 : handle1;
            if (handle != 0) {
                fn_802E620C(handle, param28);
                fn_802E6234(handle, param2C);
                fn_802E6238(handle, param30);
                fn_802E623C(handle, param34);
                fn_802E622C(handle, pitch);
                fn_802E6240(handle, param38);
            }
        }
        if (paused) {
            fn_802E61BC(handle0);
            if (handle1 != 0) fn_802E61BC(handle1);
        } else {
            fn_802E61A8(handle0);
            if (handle1 != 0) fn_802E61A8(handle1);
        }
    }

    // Opens the voice and starts it (playing or paused).
    /* Helper name unproven */
    void Begin(void) {
        Open();
        if (handle0 != 0) {
            elapsed = 0;
            Apply();
        }
        frames = 0;
        state = 1;
    }

    s32 state;            // offset 0x4: 1 playing, 3 stopped
    void* handle0;        // offset 0x8
    void* handle1;        // offset 0xC
    void* stream0;        // offset 0x10
    void* stream1;        // offset 0x14
    SoundSample* sample;  // offset 0x18
    f32 volume;           // offset 0x1C
    f32 pan;              // offset 0x20
    f32 pitch;            // offset 0x24
    f32 param28;          // offset 0x28
    f32 param2C;          // offset 0x2C
    f32 param30;          // offset 0x30
    f32 param34;          // offset 0x34
    s32 param38;          // offset 0x38
    u8 unscaled;          // offset 0x3C: volume not scaled by the effects volume
    u8 paused;            // offset 0x3D
    u8 muted;             // offset 0x3E
    s32 slot;             // offset 0x40
    s32 frames;           // offset 0x44
    f32 length;           // offset 0x48
    u32 maxTime;          // offset 0x4C
    u32 elapsed;          // offset 0x50
};

extern "C" {
f32 lbl_8055B590 = 1.0f, lbl_8055B594 = 1.0f;
f32 lbl_8055B598 = 1.0f;

// The sample used for names missing from the table.
SoundSample lbl_8033C338 = {{0}, 1.0f};

// Looks a sample up by name in the loaded sample table; unknown names get the default entry.
SoundSample* fn_8020A6AC(const char* name) {
    SoundSample* entry = lbl_804892D0;
    int i;
    for (i = 0; i < lbl_8055EE08; i++) {
        if (strcasecmp(name, entry->name) == 0) {
            return entry;
        }
        entry++;
    }
    return &lbl_8033C338;
}

void fn_80208E90(Mixer* m, int dontCare);

void fn_8020A72C(void) {
    fn_80208E90(new ("Voicemixer") VoiceSoundMixer, 0);
}

s32 fn_8020A76C(void) {
    return lbl_8055EE08;
}

SoundSample* fn_8020A774(int i) { return &lbl_804892D0[i]; }

}  // extern "C"

// Voices are 0x54-byte blocks from a shared pool, created on first use and deleted with its last block.
void* VoiceSound::operator new(size_t size, const char* tag) {
    if (lbl_8055EE0C == 0) {
        lbl_8055EE10 = new ("VoiceSound::pool") Pool("VoiceSound::pool", 0x200, 0x54);
    }
    void* p = lbl_8055EE10->Alloc();
    if (p) {
        lbl_8055EE0C++;
    }
    return p;
}

void VoiceSound::operator delete(void* p) {
    lbl_8055EE10->Free(p);
    if (--lbl_8055EE0C == 0) {
        delete lbl_8055EE10;
        lbl_8055EE10 = 0;
    }
}

VoiceSoundMixer::VoiceSoundMixer(void) {
    voices = 0;
}

int VoiceSoundMixer::Usable(void) {
    lbl_8055B598 = OptNoSound() ? 0.0f : 1.0f;
    lbl_8055EE04 = this;
    voices = new ("snd_table") VoiceSound*[0x200];
    memset(voices, 0, 0x800);
    fn_802E6B04();
    return 1;
}

void VoiceSoundMixer::Shutdown(void) {
    int i;
    fn_802E6B34();
    for (i = 0; i < 0x200; i++) {
        if (voices[i] != 0) {
            fn_802E317C("VoiceSoundMixer::End: unfreed sound! (%x)", voices[i]);
        }
    }
    delete[] voices;
    voices = 0;
    lbl_8055EE04 = 0;
}

// Stops every voice that is not already finished (state 3).
void VoiceSoundMixer::vf28(void) {
    for (int i = 0; i < 0x200; i++) {
        if (voices[i] != 0 && voices[i]->State() != 3) {
            voices[i]->Stop();
        }
    }
    fn_802E6264();
}

void VoiceSound::Stop(void) {
    Release();
    state = 3;
    muted = 0;
}

int VoiceSound::State(void) {
    if (Playing() && muted != 0) return 2;
    return state;
}

// Loads the "samples" table (name and volume of each voice sample) from the sound config.
void VoiceSoundMixer::vf2C(int arg1) {
    ValueTable* list = fn_80131808("sounds.val");
    if (list != 0) {
        ValueNode* item;
        int index;
        for (index = 0; (item = list->GetChild(index)) != 0; index++) {
            const char* name = item->GetName();
            if (strcmp(name, "samples") == 0) {
                int n = lbl_8055EE08;
                SoundSample* entry = &lbl_804892D0[n];
                int left;
                const char* s;
                char* dest;
                lbl_8055EE08 = n + 1;
                s = item->GetValue("name");
                left = 0x20;
                dest = entry->name;
                while ((*dest++ = *s++) != 0) {
                    if (--left <= 0) {
                        dest[-1] = 0;
                        break;
                    }
                }
                entry->volume = item->GetFloat("volume", 1.0f);
            }
        }
        fn_80131818(list);
    } else {
        fn_802E317C("can't get sounds.val");
    }
    fn_802E58B8(arg1);
}

void VoiceSoundMixer::vf30(int a, int b) { fn_802E5950(a, b); }

void VoiceSoundMixer::vf34(void* a) { fn_802E59BC(a); }

void VoiceSoundMixer::vf38(void) {
    lbl_8055EE08 = 0;
    fn_802E5A54();
}

void VoiceSoundMixer::SetListeners(void* listeners) {
    VoiceSound* p;
    int i, now;
    void* handle;
    f32 volume, x, v;
    lbl_8055B590 = fn_80209028();
    lbl_8055B594 = fn_802091CC();
    lbl_8055EDF8++;
    now = fn_802E2C20();
    lbl_8055EE00 = now - lastTime;
    if (lbl_8055EE00 > 200U) lbl_8055EE00 = 200;
    lastTime = now;
    for (i = 0; i < 512; i++) {
        p = voices[i];
        if (p) {
            int mode = 1;
            switch (fn_802090CC()) {
            case 0:
                mode = 0;
                break;
            case 1:
                break;
            case 2:
                mode = 2;
                break;
            }
            fn_802E625C(mode);
            handle = p->handle0;
            if (handle) {
                int j;
                v = p->volume * p->sample->volume;
                if (p->muted) {
                    volume = 0.0f;
                } else {
                    f32 w;
                    if (p->unscaled) w = v;
                    else w = v * lbl_8055B590;
                    volume = lbl_8055B598 * w;
                }
                if (p->handle1) {
                    x = 1.0f + -p->pan;
                    if (1.0f < x) x = 1.0f;
                    x = x > 0.0f ? x : 0.0f;
                    fn_802E61E4(handle, volume * x);
                    fn_802E61EC(p->handle0, (-1.0f));
                    x = 1.0f + p->pan;
                    if (1.0f < x) x = 1.0f;
                    x = x > 0.0f ? x : 0.0f;
                    fn_802E61E4(p->handle1, volume * x);
                    fn_802E61EC(p->handle1, 1.0f);
                } else {
                    fn_802E61E4(handle, volume);
                    fn_802E61EC(p->handle0, p->pan);
                }
                for (j = 0; j < 2; j++) {
                    void* h;
                    if (j == 0) h = p->handle0;
                    else h = p->handle1;
                    if (h) {
                        fn_802E620C(h, p->param28);
                        fn_802E6234(h, p->param2C);
                        fn_802E6238(h, p->param30);
                        fn_802E623C(h, p->param34);
                        fn_802E622C(h, p->pitch);
                        fn_802E6240(h, p->param38);
                    }
                }
            }
            if (p->state == 1) {
                p->frames++;
                if (!p->muted && !((f32)__fabs(p->pitch) <= __float_epsilon[0])) p->elapsed += lbl_8055EE00;
                if (!p->paused && p->elapsed > p->maxTime) p->Stop();
            } else {
                p->elapsed = 0;
            }
        }
    }
    fn_802E6264();
}

// Sets up a new voice: the left stream is the named file, the right stream the same name
// with its extension replaced by ".2.sfx".
inline VoiceSound::VoiceSound(const char* name, int slot, u8 unscaled) {
    char buf[0x40];
    char* dest;
    int left;
    const char* src;
    char* ext;

    length = 0.0f;
    this->slot = slot;
    frames = 0;
    state = 3;
    handle1 = 0;
    handle0 = 0;
    stream1 = 0;
    stream0 = 0;
    paused = 0;
    muted = 0;
    this->unscaled = unscaled;
    stream0 = fn_802E5AE8(name);
    if (stream0 == 0) {
        return;
    }
    left = 0x40;
    src = name;
    dest = buf;
    while ((*dest++ = *src++) != 0) {
        if (--left <= 0) {
            dest[-1] = 0;
            break;
        }
    }
    ext = strchr(buf, '.');
    if (ext != 0) {
        if (strcasecmp(ext, ".sfx") != 0) {
            fn_802E317C("voicesnd: sound %s doesn't end in sfx!", name);
        }
        *ext = 0;
        {
            char* d = buf;
            int n = 0x40;
            while (*d != 0) {
                n--;
                d++;
            }
            if (n > 0) {
                const char* s = ".2.sfx";
                while ((*d++ = *s++) != 0) {
                    if (--n <= 0) {
                        d[-1] = 0;
                        break;
                    }
                }
            }
        }
    } else {
        fn_802E317C("voicesnd: sound %s has no extension!", name);
    }
    stream1 = fn_802E5AE8(buf);
    sample = fn_8020A6AC(name);
    length = fn_802E6040(stream0);
    maxTime = 1000.0f * (0.1f + 1.02f * length);
    pan = 0.0f;
    volume = 0.0f;
    pitch = 1.0f;
    param28 = 1.0f;
    param2C = 0.0f;
    param30 = 0.0f;
    param34 = 0.0f;
    param38 = 0;
}

// Creates a voice in the first free slot and initialises it.
Sound* VoiceSoundMixer::CreateSound(const char* name, int mode, u8 unscaled) {
    VoiceSound* voice;
    int i;
    for (i = 0; i < 0x200; i++) {
        if (voices[i] == 0) {
            voice = new ("Voicesnd") VoiceSound(name, i, unscaled);
            if (!voice->vf10()) {
                delete voice;
                return 0;
            }
            voices[i] = voice;
            return voice;
        }
    }
    fn_802E30C8("Out of sounds!");
    return 0;
}

// Voice destructor: releases both sound handles and the two streams, and frees its mixer slot.
VoiceSound::~VoiceSound(void) {
    if (handle0 != 0) fn_802E61D8(handle0);
    if (handle1 != 0) fn_802E61D8(handle1);
    if (handle1 != 0) {
        fn_802E6138(handle1);
        handle1 = 0;
    }
    if (handle0 != 0) {
        fn_802E6138(handle0);
        handle0 = 0;
    }
    if (stream1 != 0) fn_802E5D5C(stream1);
    if (stream0 != 0) {
        fn_802E5D5C(stream0);
        lbl_8055EE04->FreeSlot(this, slot);
    }
}

int VoiceSound::vf10(void) { return stream0 != 0; }

// Empties a sound's slot (the sound is being destroyed).
void VoiceSoundMixer::FreeSlot(Sound* s, int slot) { voices[slot] = 0; }

extern "C" {
/* .bss owned by this unit (0x804892D0-0x80489BE0). */
SoundSample lbl_804892D0[64];  // the sample table
MixerCaps lbl_80489BD0;        // Caps() result
}

/* Module statics (.sbss 0x8055EDF0-0x8055EE18, in address order); their constructors are this unit's __sinit. */
ModuleTag lbl_8055EDF0;
int lbl_8055EDF8;
int lbl_8055EDFC;
unsigned int lbl_8055EE00;
VoiceSoundMixer* lbl_8055EE04;
s32 lbl_8055EE08;
int lbl_8055EE0C;
Pool* lbl_8055EE10;
UnkStatic7F lbl_8055EE14;
