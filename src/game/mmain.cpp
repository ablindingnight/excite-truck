// Main-mode startup (0x8023B5EC-0x8023B764): brings up the subsystems and enters the demo or E3 demo flow.
// Names are guesses apart from the messages.
#include "game/common.h"

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E712C(void);
void fn_802E6CA4(int a);
void fn_802E6CA8(void);
void fn_8023D7CC(void);
void fn_8023D70C(void);
void fn_802473A4(void);
void fn_80247210(void);
void fn_80129AEC(int a);
void fn_802E47E8(void);
void fn_80127A24(void);
void fn_8027B690(void);
int fn_8012742C(void);
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

void fn_8023B5EC(void) {
    fn_802E712C();
    fn_802E6CA4(1);
    fn_802E6CA8();
}

void fn_8023B618(void) {
    fn_8023B5EC();
    fn_8023D7CC();
    fn_8023D70C();
}

void fn_8023B640(void) {
    fn_802E30C8("mmain: demo mode!");
    fn_802473A4();
}

void fn_8023B670(void) {
    fn_802E30C8("mmain: e3 demo mode!");
    UnloadEves();
    LoadDemoEves();
    fn_80247210();
    UnloadEves();
    LoadArcadeEves();
}

void fn_8023B6B0(void) {
    fn_80129AEC(0);
    fn_802E47E8();
    fn_80127A24();
    fn_8023B618();
    fn_8027B690();
    if (fn_8012742C()) {
        fn_80127434();
    }
    if (OptE3Demo()) {
        fn_8023B670();
    } else {
        fn_8023B640();
    }
    fn_8027B77C();
    fn_802ECEC0();
}

}  // extern "C"
