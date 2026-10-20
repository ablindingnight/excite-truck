// Car type helpers (0x80279464-0x8027953C): subsystem start/stop and CarTypeIsReal. Names are guesses apart
// from the message.
#include "game/common.h"

extern "C" {
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void fn_80279370(void);
void fn_802793AC(void);
void fn_80279CE4(void);
void fn_80279D3C(void);
}

struct CarDesc {
    u8 pad0[0x10];
    u32 flags;  // offset 0x10
};

extern "C" CarDesc* fn_801E87A8(void);

ModuleTag lbl_8055F600;

extern "C" {

void fn_80279464(void) {
    fn_80279370();
    fn_80279CE4();
}

void fn_80279488(void) {
    fn_80279D3C();
    fn_802793AC();
}

int fn_802794AC(int type, CarDesc* car) {
    switch (type) {
    case 'P':
    case 'Q':
    case 'R':
        return 1;
    case 'T':
        return 0;
    case 'S':
        if (!car) {
            car = fn_801E87A8();
        }
        return car->flags & 1;
    default:
        fn_802E317C("CarTypeIsReal: Unrecognized CarType: %d", type);
        return 0;
    }
}

}  // extern "C"
