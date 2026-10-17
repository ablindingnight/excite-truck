// Loading-stage timer and full-screen fade (0x800123EC-0x800126B4). AppInit brackets its steps with
// LoadTimerBegin("app_begin", ...) / LoadTimerProgress(). Names are guesses.
#include <string.h>
#include "game/common.h"

extern "C" {
void fn_802C92D0(void);
void fn_802C92DC(int x0, int y0, int x1, int y1, u32 color);
void fn_802C92D8(void);
u32 fn_802E2C20(void);  // tick count
}

extern s32 lbl_8055FB9C;  // screen width
extern s32 lbl_8055FBA0;  // screen height

struct LoadTimer {
    u8 active;       // offset 0x0
    char name[0x80]; // offset 0x1
    f32 progress;    // offset 0x84
    f32 duration;    // offset 0x88
    u32 start;       // offset 0x8C
    u32 elapsed;     // offset 0x90
};

ModuleTag lbl_8055D4E0;
UnkStatic7F lbl_8055D4E8;
u8 lbl_8055D4EC;
u8 lbl_80558AD0 = 1;
LoadTimer lbl_80351790;

void DrawFade(f32 alpha) {
    int a;

    fn_802C92D0();
    a = (int)(255.0f * alpha) > 0 ? (int)(255.0f * alpha) : 0;
    fn_802C92DC(-20, -20, lbl_8055FB9C + 20, lbl_8055FBA0 + 20, (a < 255 ? a : 255) << 24);
    fn_802C92D8();
}

void LoadTimerInit(void) {
    lbl_80351790.active = 0;
    memset(lbl_80351790.name, 0, sizeof(lbl_80351790.name));
    lbl_80351790.start = 0;
    lbl_80351790.duration = 0.0f;
    lbl_80351790.progress = 0.0f;
    lbl_8055D4EC = 1;
}

void LoadTimerBegin(const char* name, int, int, f32 duration) {
    lbl_80558AD0 = 1;
    strncpy(lbl_80351790.name, name, sizeof(lbl_80351790.name) - 1);
    lbl_80351790.name[sizeof(lbl_80351790.name) - 1] = '\0';
    lbl_80351790.duration = duration;
    lbl_80351790.progress = 0.0f;
    lbl_80351790.start = fn_802E2C20();
    lbl_80351790.active = 1;
}

void LoadTimerMark(const char* name, f32 step) {
    f32 progress = lbl_80351790.progress + step;

    strncpy(lbl_80351790.name, name, sizeof(lbl_80351790.name) - 1);
    lbl_80351790.progress = progress;
    lbl_80351790.name[sizeof(lbl_80351790.name) - 1] = '\0';
    lbl_80351790.elapsed = fn_802E2C20() - lbl_80351790.start;
}

void LoadTimerProgress(void) {
    LoadTimerMark("ProgressUpdate", 100.0f);
}

void LoadTimerEnd(const char* name) {
    strncpy(lbl_80351790.name, name, sizeof(lbl_80351790.name) - 1);
    lbl_80351790.name[sizeof(lbl_80351790.name) - 1] = '\0';
    lbl_80351790.active = 0;
}

void DrawFade2(f32 alpha) {
    DrawFade(alpha);
}

void LoadTimerShutdown(void) {
    lbl_80351790.active = 0;
    memset(lbl_80351790.name, 0, sizeof(lbl_80351790.name));
    lbl_80351790.start = 0;
    lbl_80351790.duration = 0.0f;
    lbl_80351790.progress = 0.0f;
    lbl_8055D4EC = 0;
}
