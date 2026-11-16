#ifndef GAME_SOUND_H
#define GAME_SOUND_H
// A device sound: one voice a Mixer creates (mixer.cpp's NullSound, unk_8020A6AC's VoiceSound). sound.cpp's game
// sounds drive one through this interface. One declaration for every unit (port ODR work, 2026-11-16). The vfXX
// names are unproven; vf14 rebinds the voice to another sample (VoiceSound's "voicesnd: Can't rebind to %s"), the
// base returns 0 (defined in collisionsound.cpp).
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

#endif
