// Camera projection setup (0x801EA1FC-0x801EA4B4): field of view, near/far planes and clear color
// for the race, the menus and the intro. Names are guesses.
#include "game/common.h"

int GetEventSizeClass(int type);

extern "C" {
f32 fn_801E8738(int cam);
f32 fn_801E8874(int cam);
f32 fn_801E8894(int cam);
void fn_802C974C(f32 a, f32 b, f32 c);
void fn_8026A54C(f32 farPlane);
void fn_801EBC2C(void);
void fn_801EBC70(void);
void fn_801EBCAC(void);
void fn_802C86E8(u32 color);
void fn_802C990C(u32 color);
void fn_802C2B78(f32 r, f32 g, f32 b);
void fn_801FC904(void);
void fn_801FC908(int a);
void fn_801EBF00(void);
}

struct ProjState {
    f32 farPlane;  // offset 0x0
    bool large;    // offset 0x4
};

// volatile keeps MWCC from folding the multiply below; the target does it at run time.
const volatile f32 lbl_80562E38 = 61.5f;  // default field of view (degrees)
f32 lbl_8055AF68 = 61.5f;  // field of view (radians once set)

inline f32 DegToRad(f32 d) { return 0.017453292f * d; }

ModuleTag lbl_8055EB50;
u8 lbl_8055EB58;
u8 lbl_8055EB59;
ProjState lbl_8055EB60;

extern "C" {

void fn_801EA1FC(void) { lbl_8055AF68 = DegToRad(lbl_80562E38); }

void fn_801EA210(int cam, f32 fov) {
    if (!lbl_8055EB58) {
        f32 nearPlane = fn_801E8874(cam);
        fn_802C974C(nearPlane, fn_801E8894(cam), fov);
    } else {
        fn_802C974C(1.0f, 150.0f, lbl_8055AF68);
    }
}

void fn_801EA28C(int cam, f32 fov) {
    f32 nearPlane = fn_801E8874(cam);
    fn_802C974C(nearPlane, fn_801E8894(cam), fov);
}

void fn_801EA2E8(void) {
    fn_801EA1FC();
    fn_802C974C(1.0f, 1000.0f, 1.1780972f);
}

void fn_801EA318(void) {}

void fn_801EA31C(int* event) {
    lbl_8055EB60.farPlane = 2250.0f;
    fn_8026A54C(lbl_8055EB60.farPlane);
    lbl_8055EB60.large = GetEventSizeClass(*event) == 2;
    fn_802C974C(1.0f, lbl_8055EB60.farPlane, 0.65449846f);
    lbl_8055EB58 = 0;
    lbl_8055EB59 = 1;
}

void fn_801EA38C(void) { fn_801EBC2C(); }

void fn_801EA390(void) { fn_801EBC70(); }

void fn_801EA394(void) {
    if (lbl_8055EB60.large) {
        fn_802C86E8(0xFF000000);
    } else {
        fn_802C86E8(0xFFC9C9EA);
    }
}

void fn_801EA3B8(void) {
    fn_802C990C(0x00C9C9EB);
    fn_802C86E8(0xFF000000);
    fn_802C2B78(0.0f, 100.0f, 0.0f);
    lbl_8055EB59 = 0;
}

void fn_801EA400(void) { fn_801EBCAC(); }

void fn_801EA404(int unused, int arg) {
    f32 fov = fn_801E8738(-1);
    fn_801EA210(-1, fov);
    fn_801FC904();
    fn_801EA28C(-1, fov);
    fn_801EBF00();
    fn_801EA210(-1, fov);
    fn_801FC908(arg);
}

u8 fn_801EA470(void) { return lbl_8055EB58; }

void fn_801EA478(int cam) { fn_801EA210(cam, fn_801E8738(cam)); }

}  // extern "C"

