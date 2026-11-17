#ifndef GAME_MIXER_H
#define GAME_MIXER_H
// A sound device (mixer.cpp's NullMixer, unk_8020A6AC's VoiceSoundMixer): creates the device Sounds and is told about
// the listeners. sound.cpp and soundmgr.cpp define a few defaults; the vtable lives with mixer.cpp's vf14. Slot
// signatures follow VoiceSoundMixer's overrides. One declaration for every unit (port ODR work, 2026-11-16).
// vfXX names unproven.
#include "game/common.h"

class Sound;

// What a mixer supports (field names from soundmgr.cpp).
struct MixerCaps {
    u8 noPositional;  // offset 0x0
    s32 has3D;        // offset 0x4
    s32 hasPan;       // offset 0x8 (the voice mixer reports 0x20)
};

class Mixer {
public:
    Mixer(void) {}
    virtual ~Mixer(void) {}
    virtual const char* Name(void) = 0;
    virtual MixerCaps* Caps(void) = 0;
    virtual int vf14(void);
    virtual int Usable(void) = 0;
    virtual Sound* CreateSound(const char* name, int mode, int unscaled) = 0;
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

#endif
