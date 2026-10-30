// Code 0x80262F8C-0x80263080, imported verbatim from the upstream project (github.com/orangedude27/excite-truck,
// CC0; file src/auto/text_80262F8C_.c), where its functions match exactly. Unit name is provisional.
// CLEANUP: automated-decompiler source with raw offsets and placeholder types; rewrite with real types
// before the PC port. Tricks are tagged MATCHING-AID.

typedef signed int s32;
typedef unsigned char u8;
typedef float f32;
extern s32 lbl_8055F434;
extern s32 lbl_8055C440;
const f32 lbl_80563BC8 = 1.5f;  // flash time
extern s32 fn_80230E90(s32);
extern f32 fn_8023659C(void);
extern s32 fn_80214AF0(s32);
extern void __dl__FPv(void *p);  /* operator delete */
extern const char *fn_80131234(const char *key);
extern void fn_8022BAD0(s32, s32, s32, s32, s32, s32, s32, s32, f32, s32);
extern s32 fn_802328D4(s32);
extern const char lbl_80346324[];
extern const u8 lbl_80346120[];
extern f32 lbl_80563BD4;
s32 fn_80262F8C(s32 arg0, s32 arg1) {
    if (arg0 != 0 && arg1 > 0) {
        __dl__FPv((void *)arg0);
    }
    return arg0;
}

void fn_80262FCC(void *arg0) {
    *(f32 *)((u8 *)arg0 + 0x18) = fn_8023659C();
    *((u8 *)arg0 + 0x1C) = 0;
    fn_80214A9C(0x1D);
}

void fn_8026300C(void *arg0) {
    f32 temp_f0;
    f32 temp_f1;
    temp_f1 = fn_8023659C();
    temp_f0 = *(f32 *)((u8 *)arg0 + 0x18);
    if ((temp_f1 < temp_f0) || ((temp_f1 - temp_f0) > lbl_80563BC8)) {
        *((u8 *)arg0 + 0x1C) = 1;
    }
    if (*((u8 *)arg0 + 0x1C) != 0) {
        fn_80214AF0(0x1D);
    }
}

s32 fn_80263070(void *arg0) {
    return *((u8 *)arg0 + 0x1C) == 0;
}
