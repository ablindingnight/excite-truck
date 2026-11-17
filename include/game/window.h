#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

// The menu window (window.cpp 0x80216D14-0x80218E98), shared by window.cpp, widgetwin.cpp and uiapi.cpp (each had its own
// view of the 0xDE8 class). Field names are guesses apart from the strings. Widget is window.cpp's view of the widget base.
#include "game/common.h"

// A text style (0x3C): copied memberwise, so the floats go through lfs/stfs.
struct StyleBody {
    u32 w[12];
};

struct Style {
    Style(void) {}
    StyleBody body;  // offset 0x0
    f32 f30;         // offset 0x30
    f32 f34;         // offset 0x34
    f32 f38;         // offset 0x38
};

class Widget;
class Window;

struct MenuEntry {
    s32 id;     // offset 0x0
    Widget* w;  // offset 0x4
};

class Window {
public:
    Window(const char* bg, s32 x, s32 y, s32 w, s32 h, s32 f490);
    ~Window(void);

    void* f0;                // offset 0x0
    void* f4;                // offset 0x4
    void* image;             // offset 0x8
    u8 fC[0x30 - 0xC];       // offset 0xC
    Widget* widgets[256];    // offset 0x30
    s32 count;               // offset 0x430
    void (*update)(Window* w);  // offset 0x434
    void (*open)(void);      // offset 0x438
    void (*close)(void);     // offset 0x43C
    void (*fade)(int phase, void* fade, f32 t);  // offset 0x440
    void (*fadeStyle)(Style* s, u32 mask, s32 x, s32 y, s32 h, s32 dh, int a, f32 t);  // offset 0x444
    Widget* hover;           // offset 0x448
    Widget* pressed;         // offset 0x44C
    Widget* focus;           // offset 0x450
    s32 x;                   // offset 0x454
    s32 y;                   // offset 0x458
    s32 w;                   // offset 0x45C
    s32 h;                   // offset 0x460
    u32 hiddenGroups;        // offset 0x464
    u32 disabledGroups;      // offset 0x468
    u8 b46C;                 // offset 0x46C
    u8 b46D[0x20];           // offset 0x46D
    u8 pad48D[0x490 - 0x48D];
    s32 f490;                // offset 0x490
    s32 menuCount;           // offset 0x494
    s32 menuCur;             // offset 0x498
    u8 pad49C[0x4A0 - 0x49C];
    MenuEntry menu[64];      // offset 0x4A0
    Style styles[31];        // offset 0x6A0
    u8 noSound;              // offset 0xDE4
};

#endif
