// Alarm events (0x801B8B84-0x801B8CC0): a queue of up to 64 timed callbacks, sorted by due time, fired from
// the game loop. Names are guesses apart from the "alarm_event" allocation tag.
#include "game/common.h"

void* operator new[](size_t size, const char* tag);

struct AlarmEvent {
    s32 time;                         // offset 0x0
    void (*fn)(s32 a, s32 b, s32 c);  // offset 0x4
    s32 a;                            // offset 0x8
    s32 b;                            // offset 0xC
    s32 c;                            // offset 0x10
};

extern s32 lbl_8055E3C4;  // current tick

ModuleTag lbl_8055E758;
AlarmEvent* lbl_8055E760;
s32 lbl_8055E764;

extern "C" {

void fn_801B8B84(void) {
    lbl_8055E760 = new ("alarm_event") AlarmEvent[64];
    lbl_8055E764 = 0;
}

void fn_801B8BBC(void) {
    delete[] lbl_8055E760;
    lbl_8055E760 = 0;
}

void fn_801B8BE8(void) { lbl_8055E764 = 0; }

void fn_801B8BF4(void) {
    s32 now = lbl_8055E3C4;
    while (lbl_8055E764 > 0) {
        AlarmEvent* e = lbl_8055E760;
        int i;
        if (now < e->time) {
            break;
        }
        e->fn(e->a, e->b, e->c);
        lbl_8055E764--;
        for (i = 0; i < lbl_8055E764; i++) {
            lbl_8055E760[i] = lbl_8055E760[i + 1];
        }
    }
}

}  // extern "C"
