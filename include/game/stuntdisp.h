#ifndef GAME_STUNTDISP_H
#define GAME_STUNTDISP_H
// A stunt pop-up / results line (StuntDisplay) and what hands them out (StuntFactory: the HUD, the results
// list). betarace implements both; stunts' trackers fill them. Token-identical to betarace.cpp's copies until
// betarace includes this. Names unproven.
#include "game/common.h"

class StuntDisplay {
public:
    virtual ~StuntDisplay(void) {}
    virtual void Text0(const char* fmt, ...) = 0;
    virtual void Text1(const char* fmt, ...) = 0;
    virtual void Text2(const char* fmt, ...) = 0;
    virtual void SetValue(f32 v) = 0;
    virtual void SetPoints(s32 p) = 0;
    virtual void SetType(s32 t) = 0;
    virtual void Stop(void) = 0;
    virtual void SetReady(u8 r) = 0;
};

class StuntFactory {
public:
    virtual ~StuntFactory(void) {}
    virtual StuntDisplay* Alloc(void) = 0;
};

#endif
