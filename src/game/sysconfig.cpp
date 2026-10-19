// System settings (0x802E98C8-0x802E9A3C): reads the console settings (aspect ratio, sound mode,
// progressive scan, Wii Remote rumble and speaker volume) once and caches them. Names are guesses apart from
// the message.
#include "game/common.h"

extern "C" {
void fn_802E317C(const char* fmt, ...);  // panic
void SCInit(void);
u32 SCCheckStatus(void);
u8 SCGetAspectRatio(void);
u8 SCGetSoundMode(void);
u8 SCGetProgressiveMode(void);
u8 SCGetWpadMotorMode(void);
u8 SCGetWpadSpeakerVolume(void);
}

ModuleTag lbl_80560038;
u8 lbl_80560040;   // read
s32 lbl_80560044;  // widescreen
s32 lbl_80560048;  // sound mode
bool lbl_8056004C;  // progressive
bool lbl_8056004D;  // rumble
f32 lbl_80560050;  // speaker volume

extern "C" {

int fn_802E98C8(void) { return 0; }

void fn_802E99AC(void);

void fn_802E98D0(void) {
    u32 status;
    SCInit();
    do {
        status = SCCheckStatus();
    } while (status == 1);
    if (status == 2) {
        fn_802E317C("config: SCCheckStatus returns error!");
    }
    switch (SCGetAspectRatio()) {
    default:
        lbl_80560044 = 0;
        break;
    case 1:
        lbl_80560044 = 1;
        break;
    }
    switch (SCGetSoundMode()) {
    case 0:
        lbl_80560048 = 0;
        break;
    case 1:
    default:
        lbl_80560048 = 1;
        break;
    case 2:
        lbl_80560048 = 2;
        break;
    }
    lbl_8056004C = SCGetProgressiveMode() == 1;
    lbl_80560040 = 1;
    fn_802E99AC();
}

void fn_802E99A8(void) {}

void fn_802E99AC(void) {
    lbl_8056004D = SCGetWpadMotorMode() == 1;
    lbl_80560050 = SCGetWpadSpeakerVolume() / 127.0f;
}

s32 fn_802E9A0C(void) { return lbl_80560044; }

s32 fn_802E9A14(void) { return lbl_80560048; }

bool fn_802E9A1C(void) { return lbl_8056004C; }

bool fn_802E9A24(void) { return lbl_8056004D; }

f32 fn_802E9A2C(void) { return lbl_80560050; }

}  // extern "C"
