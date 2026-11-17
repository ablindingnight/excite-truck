#ifndef GAME_IDRIVER_H
#define GAME_IDRIVER_H
// A car's driver (aidriver.cpp): the AI driver (aicar), the player's driver and the player controller that runs the
// drivers (playcar), the null and park drivers (aidriver). The defaults are inline, emitted weak in aidriver.
// Each slot takes the widest argument list any caller or override uses (the controller passes Init a second
// pointer and vf1C a time; AIDriver's vf38/vf3C take the hit vectors). Names unproven.
#include "game/common.h"

struct RVec3;

extern "C" void fn_802E317C(const char* fmt, ...);  // fatal

class IDriver {
public:
    virtual ~IDriver(void) {}
    virtual void Init(void* car, void* q) = 0;
    virtual void SetCar(void* car) { fn_802E317C("idriver: SetCar not overridden"); }
    virtual u32 Flags(void) { return 0; }
    virtual int vf18(int x) { return 0; }
    virtual void vf1C(int a, int b, int* c, int* d, f32 t) {
        *d = 0;
        *c = 0;
    }
    virtual int vf20(int x) { return 0; }
    virtual void Start(void) = 0;
    virtual void Stop(void) = 0;
    virtual void vf2C(void) {}
    virtual void vf30(void) {}
    virtual void vf34(void) {}
    virtual void vf38(const RVec3* dir, RVec3* b, int kind, u8* other) {}
    virtual void vf3C(const RVec3* pt, RVec3* imp, int a, u8* other) {}
    virtual void vf40(void) {}
    virtual void vf44(void) {}
};

#endif
