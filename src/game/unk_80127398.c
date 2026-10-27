// Code 0x80127398-0x801274C8: the callback lists run from the Bink/streaming alarms and the static
// initializer of their queue. Compiled with the game flags (-proc generic); split from unk_80116BD4, whose
// Bink code is built for gekko scheduling. fn_801274C0 imported verbatim from the upstream project
// (github.com/orangedude27/excite-truck, CC0; file src/auto/text_801274C0_.c). Unit name is provisional.
#include <types.h>
#include <revolution/OS/OSAlarm.h>

extern void* fn_802ECC74(void*);
extern void fn_80127F70(void*);
extern void fn_80127F94(void*);
extern void fn_802E300C(s32);
typedef struct CallbackList {
    s32 count;
    void (*fns[4])(void);
} CallbackList;
extern CallbackList lbl_8041D728[];

extern char lbl_8055DF08;
// Callback list `id` (-1..3), or 0.
static inline CallbackList *GetCallbackList(s32 id) {
    if (id >= -1 && id < 4) {
        return lbl_8041D728 + id + 1;
    }
    return 0;
}

// Calls every callback registered in list `arg0`.
void fn_80127398(s32 arg0) {
    CallbackList *list = GetCallbackList(arg0);
    s32 i;
    if (list != 0) {
        for (i = 0; i < list->count; i++) {
            list->fns[i]();
        }
    }
}

char fn_8012742C(void) {
    return lbl_8055DF08;
}

void fn_80127434(void) {
    lbl_8055DF08 = 0;
}

void fn_80127440(void* alarm) {
    fn_80127F94(fn_802ECC74(alarm));
}

void fn_80127464(void *alarm) {
    fn_80127440(alarm);
}

void fn_80127468(OSAlarm *alarm, OSContext *ctx) {
#pragma unused(ctx)
    fn_80127F70(fn_802ECC74(alarm));
}

void fn_8012748C(s32 arg0) {
    fn_80127398(arg0);
    fn_802E300C(arg0);
}

extern char lbl_8055DF00[4];
extern void fn_8000D838(void *queue);
void fn_801274C0(void) {
    fn_8000D838(lbl_8055DF00);
}
