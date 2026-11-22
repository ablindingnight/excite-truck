// Main-mode startup (0x8023B5EC-0x8023B764): brings up the subsystems and enters the demo or E3 demo flow.
// Names are guesses apart from the messages.
#include "game/common.h"

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E712C(void);
void fn_802E6CA4(int a);
void MemCardSystemLoad(void);
void fn_8023D7CC(void);
void fn_8023D70C(void);
void MenuDoDemo(void);
void HandleKioskAttractLoop(void);
void SetInputChannel(int a);
void ClearInputBuffer(void);
void fn_80127A24(void);
void InitializeCareerEvents(void);
#ifdef __MWERKS__
int fn_8012742C(void);  // defined char in unk_80127398.cpp; MWCC callees extend r3, retail callers read it whole
#else
char fn_8012742C(void);  /* PORT: the definition's width (x86 leaves the upper bits undefined) */
#endif
void fn_80127434(void);
void fn_8027B77C(void);
void fn_802ECEC0(void);
}

void UnloadEves(void);
void LoadDemoEves(void);
void LoadArcadeEves(void);
int OptE3Demo(void);

ModuleTag lbl_8055F120;
static UnkStatic123 lbl_80498758;
UnkStatic7F lbl_8055F128;

extern "C" {

void LoadMemoryCardSystem(void) {
    fn_802E712C();
    fn_802E6CA4(1);
    MemCardSystemLoad();
}

void fn_8023B618(void) {
    LoadMemoryCardSystem();
    fn_8023D7CC();
    fn_8023D70C();
}

void HandleDemoMode(void) {
    fn_802E30C8("mmain: demo mode!");
    MenuDoDemo();
}

void InitDemoMode(void) {
    fn_802E30C8("mmain: e3 demo mode!");
    UnloadEves();
    LoadDemoEves();
    HandleKioskAttractLoop();
    UnloadEves();
    LoadArcadeEves();
}

void fn_8023B6B0(void) {
    SetInputChannel(0);
    ClearInputBuffer();
    fn_80127A24();
    fn_8023B618();
    InitializeCareerEvents();
    if (fn_8012742C()) {
        fn_80127434();
    }
    if (OptE3Demo()) {
        InitDemoMode();
    } else {
        HandleDemoMode();
    }
    fn_8027B77C();
    fn_802ECEC0();
}

}  // extern "C"
