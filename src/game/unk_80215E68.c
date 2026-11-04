// Code 0x80215E68-0x80216054 (functions past 0x80216054 belong to widgetwin.cpp), imported verbatim from the upstream project (github.com/orangedude27/excite-truck,
// CC0; file src/auto/text_80215E68_.c), where its functions match exactly. Unit name is provisional.
// CLEANUP: automated-decompiler source with raw offsets and placeholder types; rewrite with real types
// before the PC port. Tricks are tagged MATCHING-AID.
extern void fn_8021889C(void);

#include <types.h>

extern void *lbl_8055EF88;
extern s32 lbl_8055EF78;
extern u32 lbl_8055EF7C;
extern u32 lbl_8055EF8C;
extern void (*lbl_8055EF90)(void);
extern u8 lbl_8055EF70;
extern u8 lbl_8055F024;
extern u32 lbl_804908C8[16];
extern s32 fn_802C8530(void);
extern s32 fn_80216244(void);
extern s32 lbl_8055CFF0, lbl_8055CFF4;
extern void fn_802184D0(void);
extern void fn_80218574(void *);
extern void fn_80230B5C(void *);
extern void fn_8023108C(s32);
extern void fn_802C8638(void *);
extern void fn_802C86E8(u32);
extern void fn_802C87B4(s32, s32, s32, s32);
extern void fn_802C88BC(s32);
extern void fn_802C974C(f32, f32, f32);
extern s32 fn_802160AC(void);
extern s32 fn_80211F54(void);
extern s32 fn_80019BA8(void);
extern s32 fn_80230DA0(void);
extern s32 fn_802C855C(void);
extern s32 fn_802C2B48(void);
extern s32 fn_80211F64(void);
extern s32 fn_802164D8(void);
extern s32 fn_80129AFC(void);
extern s32 fn_802161CC(s32);
extern void fn_80218370(void *);
extern s32 fn_802E97B4(void);
extern void fn_802166B8(void);
#pragma small_data off
extern char lbl_8033D690[0x60];
#pragma small_data on
extern s32 fn_80215EC4(s32, s32);
#pragma small_data off
extern u8 lbl_80490710[0x1A0];
#pragma small_data on
extern void __destroy_arr(void *, void *, s32, s32);
extern f32 lbl_8055EF94;
extern s32 lbl_8055EF74;
extern s32 fn_80217270(s32);
extern s32 fn_802E317C(const char *);
extern char lbl_8033D7EC[0x1A];
extern s32 fn_80218464(void);


// Code of main that had no source file yet: decomp.wtf gave it this one.
// split_file divides it into the game's real files (file_status shows its range).

extern s32 __dl__FPv(void);
s32 fn_80215E68(s32 arg0, s32 arg1) {
    if (arg0 != 0 && arg1 > 0) {
        __dl__FPv();
    }
    return arg0;
}

void fn_80215EA8(void) {
    __destroy_arr(lbl_80490710, fn_80215EC4, 0x18, 0x10);
}

s32 fn_80215EC4(s32 arg0, s32 arg1) {
    if (arg0 != 0 && arg1 > 0) {
        __dl__FPv();
    }
    return arg0;
}

void fn_80215F04(void *arg0) {
    *(char **)arg0 = lbl_8033D690;
    *(s32 *)((u8 *)arg0 + 0x14) = 0;
}

s32 fn_80215F1C(s32 arg0, s32 arg1) {
    if (arg0 != 0 && arg1 > 0) {
        __dl__FPv();
    }
    return arg0;
}

s32 fn_80215F5C(s32 arg0, s32 arg1) {
    if (arg0 != 0 && arg1 > 0) {
        __dl__FPv();
    }
    return arg0;
}

extern s32 fn_802C3034(s32);
extern s32 fn_802C3424(s32, s32, s32, s32, s32);
extern s32 lbl_8055FBDC;
extern s32 fn_802E4A10(void);
extern char lbl_8033D768[0x9];
extern char lbl_8033D774[0x9];
char *fn_80215F9C(void *arg0) {
    if (*(s32 *)((u8 *)arg0 + 0x10) != 0) return (char *)lbl_8033D774;
    return (char *)lbl_8033D768;
}

void fn_80215FBC(void *arg0) {
    *(u32 *)((u8 *)arg0 + 0x10) ^= 1;
}

void fn_80215FCC(void *arg0) {
    *(u32 *)((u8 *)arg0 + 0x10) ^= 1;
}

// MenuWidget's out-of-line virtuals (C++ class in the original menu unit; defined here under their mangled names so
// the C++ units that derive from MenuWidget link against them).
u32 vf28__10MenuWidgetFv(void) { return 1; }

u32 vf2C__10MenuWidgetFv(void) { return 1; }

u8 vf08__14MenuWidgetBaseFv(void *arg0) { return *(u8 *)((u8 *)arg0 + 8); }

u8 vf0C__14MenuWidgetBaseFv(u8 *arg0) { return *arg0; }

s32 vf10__14MenuWidgetBaseFv(const void *arg0) {
    s32 result = 0;
    if (*(const u8 *)arg0 != 0 && *(const s32 *)((const u8 *)arg0 + 4) == fn_802E4A10()) result = 1;
    return result;
}
