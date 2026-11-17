#ifndef GAME_WHEEL_H
#define GAME_WHEEL_H

#include "game/common.h"
#include "game/rigid.h"

struct BodyDesc;
struct RPacket;

// A loose wheel phob (wheel.cpp; vtable emitted there by Init, its first non-inline virtual): a rigid body knocked
// off a car (phob.cpp's fn_80178290 creates one under the tag "WheelPhob"). Names are guesses.
class Wheel : public RigidBody {
public:
    Wheel(void);
    virtual void Init(BodyDesc* d);
    virtual ~Wheel(void);
    virtual void Reset(void);
    virtual void Update(void);
    virtual void vf20(void);
    virtual void vf28(u8* buf);
    virtual s32 vf2C(RPacket* p);
    virtual void vf30(f32 t, RPacket* a, const RPacket* b, s32* na, s32* nb);
    virtual s32 vf34(RPacket* p);
    f32 time;   // offset 0x1EC
    s32 car;    // offset 0x1F0
    s32 index;  // offset 0x1F4
    u8 live;    // offset 0x1F8
};

#endif
