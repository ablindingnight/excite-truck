#ifndef GAME_SOUND_H
#define GAME_SOUND_H
// A device sound: one voice a Mixer creates (mixer.cpp's NullSound, unk_8020A6AC's VoiceSound). sound.cpp's game
// sounds drive one through this interface. One declaration for every unit (port ODR work, 2026-11-16). The vfXX
// names are unproven; vf14 rebinds the voice to another sample (VoiceSound's "voicesnd: Can't rebind to %s"), the
// base returns 0 (collisionsound.cpp's fn_801B3FD0). mixer.cpp and collisionsound.cpp still declare their own
// copies: retail's local __vt__5Sound in mixer needs every virtual inline there, while this declaration's
// out-of-line vf14/vf44.. would make collisionsound emit the vtable (see handoff PORT_ODR_NAMES).
//
// A game sound (sound.cpp): the named, class-filtered sound the game code holds, driving a device Sound each frame
// from its pitch/volume/flags. Sound2D adds pan and depth, Sound3D a position and velocity heard from the
// listeners. The inline helpers are the game units' own requests (set a flag, the sound manager acts on it).
#include "game/common.h"

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

struct Listener;
struct ValVec3;

class GameSound {
public:
    GameSound(const char* name, int cls);
    virtual ~GameSound(void);
    virtual void Update(Listener* l, int n);
    virtual void Apply(Listener* l, int n);
    virtual void PreUpdate(Listener* l, int n);

    void Cull(Listener* l, int n);
    void Step(Listener* l, int n);

    void Play(void) {
        start = 1;
        loop = 0;
        stop = 0;
    }
    void PlayLoop(void) {
        stop = 0;
        loop = 1;
        start = 1;
    }
    void Stop(void) {
        stop = 1;
        restart = 0;
    }
    void Restart(void) {
        restart = 1;
        stop = 0;
    }
    void SetVolume(f32 v) {
        if (v != volume) {
            volumeDirty = 1;
            volume = v;
        }
    }
    void SetPitch(f32 v) {
        if (v != pitch) {
            pitchDirty = 1;
            pitch = v;
        }
    }
    int Playing(void) { return playing; }

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

class Sound2D : public GameSound {
public:
    Sound2D(const char* name, int cls);
    virtual ~Sound2D(void) {}
    virtual void Update(Listener* l, int n);

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

    u8 pad54[0x10];
    f32 pan;        // offset 0x64
    f32 depth;      // offset 0x68
    u8 panDirty;    // offset 0x6C
    u8 depthDirty;  // offset 0x6D
};

#endif
