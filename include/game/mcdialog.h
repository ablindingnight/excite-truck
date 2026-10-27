#ifndef GAME_MCDIALOG_H
#define GAME_MCDIALOG_H

#include "game/common.h"

// The memory card dialog: one global MemCardDialogMgr, built by menusel.cpp's static initialiser,
// methods in mcstatus.cpp. When "mc_dialog.lyt" loads, the dialog is a layout menu; otherwise it falls
// back to a plain background drawn by the MemCardDialogBg member. Names are guesses.

// A memory card message: title, text and progress.
struct McMsg {
    const char* title;  // offset 0x0
    const char* text;   // offset 0x4
    f32 frac;           // offset 0x8
};

// One dialog choice: its title and text.
struct McChoice {
    const char* title;  // offset 0x0
    const char* text;   // offset 0x4
};

// A dialog: title, message, layout (0 = "mc_dialog.lyt"), the selection check, the default choice and the
// choices.
struct McDialogCfg {
    const char* title;     // offset 0x0
    const char* text;      // offset 0x4
    const char* layout;    // offset 0x8
    int (*check)(u8* p);   // offset 0xC
    s32 select;            // offset 0x10
    s32 count;             // offset 0x14
    McChoice entries[1];   // offset 0x18
};

// The fallback background ("*uibig.res/backsplt.img") with its two fonts.
class MemCardDialogBg {
public:
    MemCardDialogBg(void) {
        image = 0;
        image2 = 0;
        font = 0;
        font2 = 0;
        fade = 0.0f;
    }
    virtual void Load(void);
    virtual void Unload(void);
    virtual void Setup(McDialogCfg* cfg);
    virtual void Show(const McMsg* m);
    virtual ~MemCardDialogBg(void) {}

    u8* image;   // offset 0x4
    u8* image2;  // offset 0x8
    u8* font;    // offset 0xC
    u8* font2;   // offset 0x10
    f32 fade;    // offset 0x14
};

class MemCardDialogMgr;
extern MemCardDialogMgr* lbl_8055F244;  // the instance

class MemCardDialogMgr {
public:
    MemCardDialogMgr(void) {
        lbl_8055F244 = this;
        open = 0;
        layout = 0;
        cfg = 0;
    }
    virtual void Open(void);
    virtual void Close(void);
    virtual void Setup(McDialogCfg* cfg);
    virtual void ShowMsg(const McMsg* m);
    virtual ~MemCardDialogMgr(void);

    MemCardDialogBg bg;  // offset 0x4
    u8 open;             // offset 0x1C
    u8 layout;           // offset 0x1D: "mc_dialog.lyt" loaded
    u8 bgShown;          // offset 0x1E
    McDialogCfg* cfg;    // offset 0x20
    s32 retries;         // offset 0x24
};

#endif
