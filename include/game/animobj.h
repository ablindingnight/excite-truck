#ifndef GAME_ANIMOBJ_H
#define GAME_ANIMOBJ_H
// World objects (world.cpp keeps the lists) and animated objects (animobj.cpp): a pose (VecMatrix frame) driven
// from an animation slot. WObj is all-inline (world.cpp emits the weak Update/PostDraw); AnimObj's vtable lives
// with its out-of-line Depth in animobj.cpp. Port ODR work, 2026-11-16; names unproven. animobj.cpp, trackobj.cpp and
// wheelmod.cpp still declare their own copies: retail has NO WObj vtable, and animobj.cpp only avoids emitting one
// with Update declared out of line, while world.cpp emits the weak inline Update/PostDraw (a port decision, see
// handoff PORT_ODR_NAMES); trackobj/wheelmod also view frame as OFrame/ValMatrix.
#include "game/common.h"
#include "game/vecmatrix.h"

struct AnimSlot;

class WObj {
public:
    virtual ~WObj(void) {}
    virtual void Update(void) {}    // 0xC
    virtual void Draw(void) = 0;    // 0x10
    virtual void PostDraw(void) {}  // 0x14
};

class AnimObj : public WObj {
public:
    AnimObj(void);
    virtual ~AnimObj(void) {}
    virtual void Draw(void) = 0;
    virtual f32 Depth(void);
    virtual void vf1C(void);
    virtual void Set(const AnimSlot* s);
    void Bind(const char* name);
    void Update(u8* data);
    s16 Flags(void);

    VecMatrix frame;  // offset 0x4
    s32 offset;       // offset 0x34
    s16 flags;        // offset 0x38
};

#endif
