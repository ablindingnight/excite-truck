// Disc and reset monitor (0x802D98F8-0x802D9C5C): watches the DVD drive state and the reset and
// power buttons from the retrace callback, and shows the disc error screens until the problem is
// gone. Names are guesses apart from the messages.
#include "game/common.h"

extern "C" {
void OSReport(const char* fmt, ...);
s32 DVDGetDriveStatus(void);
BOOL OSGetResetButtonState(void);
u32 GXGetOverflowCount(void);
BOOL OSDisableInterrupts(void);
BOOL OSEnableInterrupts(void);
typedef void (*VIRetraceCallback)(u32 count);
VIRetraceCallback VISetPostRetraceCallback(VIRetraceCallback callback);
typedef void (*OSStateCallback)(void);
OSStateCallback OSSetPowerCallback(OSStateCallback callback);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802D3DAC(void);
void fn_802E2E38(int reason);
void fn_80129AFC(void);
void fn_802E6264(void);
void fn_802E7FD0(void);
int fn_802CF834(void);
void fn_802C2AA8(int mode);
void fn_802C2B18(void);
void fn_802D3E9C(int a);
void fn_802DB03C(const char* msg);  // shows a message on the ROM-font screen
extern u8 lbl_8055FFB4;
}

// Screen text for each disc state.
static const char* lbl_8034C80C[] = {
    "No error",
    "Reset",
    "An error has occured.  Turn the power off\nand refer to the Wii Operations Manual\nfor further "
    "instructions.",
    "Please insert the\nExcite Truck\nGame Disc.",
    "The Disc Cover is open.  If you want to\ncontinue the game, please close the Disc Cover.",
    "Please insert the\nExcite Truck Game Disc.",
    "The Game Disc could not be read.  Please\nread the Wii Operations Manual\nfor more information.",
};

ModuleTag lbl_8055FE30;
u8 lbl_8055FE38;   // reset pending
u8 lbl_8055FE39;   // reset allowed later
int lbl_8055FE3C;  // disc state
u8 lbl_8055FE40;   // reset button held
u8 lbl_8055FE41;   // power button pressed
int lbl_8055FE44;  // last reported state
u8 lbl_8055FE48;   // private GX active
int lbl_8055FE4C;  // reset requests

extern "C" {

void fn_802D98F8(void) {
    OSReport("POWER BUTTON!\n");
    lbl_8055FE41 = 1;
}

void fn_802D992C(u32 count) {
    int wasReset = lbl_8055FE3C == 1;
    switch (DVDGetDriveStatus()) {
    case 0:
        lbl_8055FE3C = 0;
        break;
    case -1:
        lbl_8055FE3C = 2;
        break;
    case 5:
        lbl_8055FE3C = 4;
        break;
    case 6:
        lbl_8055FE3C = 5;
        break;
    case 4:
        lbl_8055FE3C = 3;
        break;
    case 11:
        lbl_8055FE3C = 6;
        break;
    }
    if (OSGetResetButtonState()) {
        lbl_8055FE40 = 1;
    } else if (lbl_8055FE40) {
        lbl_8055FE40 = 0;
        lbl_8055FE3C = 1;
    }
    if (wasReset && lbl_8055FE3C != 2) {
        lbl_8055FE3C = 1;
    }
    if (lbl_8055FE3C != lbl_8055FE44) {
        lbl_8055FE44 = lbl_8055FE3C;
        OSReport("DVDError: %d\n", lbl_8055FE3C);
    }
    if (GXGetOverflowCount()) {
        fn_802D3DAC();
    }
}

void fn_802D9A50(void) {
    if (lbl_8055FFB4) {
        return;
    }
    if (lbl_8055FE38) {
        fn_802E2E38(0);
        lbl_8055FE38 = 0;
    }
    if (lbl_8055FE41) {
        fn_802E2E38(2);
    }
    if (lbl_8055FE3C != 1) {
        lbl_8055FE4C = 0;
    }
    for (;;) {
        int state = lbl_8055FE3C;
        if (state == 0) {
            break;
        }
        if (lbl_8055FE41) {
            fn_802E2E38(2);
        }
        if (state == 1) {
            int immediate = (lbl_8055FE39 == 0) ? 1 : 0;
            lbl_8055FE4C++;
            if (immediate) {
                fn_802E30C8("dr: immediate reset");
                fn_802E2E38(0);
            } else {
                fn_802E30C8("dr: reset requested");
                lbl_8055FE38 = 1;
            }
            break;
        }
        fn_80129AFC();
        fn_802E6264();
        fn_802E7FD0();
        OSDisableInterrupts();
        int idle = fn_802CF834() == 0;
        OSEnableInterrupts();
        if (idle && !lbl_8055FE48) {
            lbl_8055FE48 = 1;
            fn_802E30C8("dr: BEGINNING PRIVATE GX");
            fn_802C2AA8(2);
        }
        fn_802D3E9C(0);
        fn_802DB03C(lbl_8034C80C[state]);
        OSEnableInterrupts();
    }
    OSDisableInterrupts();
    if (lbl_8055FE48 && lbl_8055FE3C == 0) {
        fn_802E30C8("dr: ENDING PRIVATE GX");
        lbl_8055FE48 = 0;
        fn_802C2B18();
    }
    OSEnableInterrupts();
}

int fn_802D9BF0(void) { return lbl_8055FE3C != 0; }

int fn_802D9C04(void) { return lbl_8055FE3C == 1; }

void fn_802D9C18(void) {
    VISetPostRetraceCallback(fn_802D992C);
    OSSetPowerCallback(fn_802D98F8);
}

void fn_802D9C4C(void) {
    // From a reset function the linker stripped.
    static const char* unused[] = {"drQueueReset!"};
    VISetPostRetraceCallback(NULL);
}

}  // extern "C"
