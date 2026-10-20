// Video modes (0x802CE344-0x802CE3A4): the table of supported display modes and its lookup. Names are
// guesses apart from the message.
#include "game/common.h"

extern "C" void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)

struct VideoModeInfo {
    s32 width;   // offset 0x0
    s32 height;  // offset 0x4
    s32 bpp;     // offset 0x8
    u32 flags;   // offset 0xC
    s32 index;   // offset 0x10
    u32 magic;   // offset 0x14
};

VideoModeInfo lbl_8034BC00[7] = {
    {0, 0, 0, 0x04000000, 6, 0x000F1CCE},
    {0, 0, 0, 0x04000000, 6, 0x000F1CCE},
    {640, 480, 16, 0x04000000, 0, 0x000F1CCE},
    {800, 600, 16, 0x04000000, 1, 0x000F1CCE},
    {1024, 768, 16, 0x04000000, 2, 0x000F1CCE},
    {1280, 1024, 16, 0x04000000, 3, 0x000F1CCE},
    {640, 480, 32, 0x05000000, 4, 0x000F1CCE},
};

ModuleTag lbl_8055FCE8;

extern "C" VideoModeInfo* fn_802CE344(int mode) {
    if (mode >= 0 && mode < 7) {
        return &lbl_8034BC00[mode];
    }
    fn_802E317C("VidGetVideoModeInfo(%d): Unknown video mode", mode);
    return 0;
}
