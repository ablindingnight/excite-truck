// Code 0x8020DE74-0x8020F3AC, imported verbatim from the upstream project (github.com/orangedude27/excite-truck,
// CC0; file src/auto/text_8020DE74_.c), where its functions match exactly. Unit name is provisional.
// CLEANUP: automated-decompiler source with raw offsets and placeholder types; rewrite with real types
// before the PC port. Tricks are tagged MATCHING-AID.
#include "types.h"
// Code of main that had no source file yet: decomp.wtf gave it this one.
// split_file divides it into the game's real files (file_status shows its range).

extern const float lbl_80563368;
extern const float lbl_80563370;
extern const float lbl_805633DC;
extern const float lbl_805633F4;
extern const float lbl_805633F8;
extern const float lbl_805633FC;

#pragma cplusplus on
extern void *lbl_8033C8C4[];
extern char lbl_8033C7A0[];
extern char lbl_8055B648[7];
extern const float lbl_8056336C;
void *operator new(unsigned long size, const char *tag);
extern "C" void *memset(void *, int, unsigned long);
extern "C" void fn_801345D8(char *, int, const char *, ...);
extern "C" void *fn_802077A4(const char *name, long kind, void *a, void *b);
extern "C" void *fn_802076A0(const char *name, long kind, void *owner, long mode);
long *FindVehicle(long);

// A part created by fn_802077A4: owner at 0x10, a level at 0x04 with a dirty flag at 0x4E.
struct HudPart {
    long pad0;
    float level;
    long pad8[2];
    void *owner;
    unsigned char pad14[0x3A];
    unsigned char dirty;
};

static inline void HudPart_SetLevel(HudPart *part, float level) {
    if (part->level != level) {
        part->dirty = 1;
        part->level = level;
    }
}

// Builds the HUD parts for a vehicle; the extra gauges exist only for the 'PCAR' type.
extern "C" void *fn_8020DE74(unsigned char *vehicle) {
    const char *names = lbl_8033C7A0;
    unsigned char *hud = (unsigned char *)operator new(0x74, lbl_8055B648);
    if (hud != 0) {
        long isCar;
        void *res2;
        void *owner;
        void *res;
        unsigned long i;
        char buf[0x20];

        *(void ***)hud = lbl_8033C8C4;
        *(unsigned char **)(hud + 4) = vehicle;
        res = vehicle + 0x118;
        res2 = vehicle + 0x60;
        owner = *(void **)(vehicle + 0x1EC);
        isCar = *(long *)(vehicle + 0x10) == 0x50434152;
        if (isCar) {
            *(void **)(hud + 0x28) = fn_802077A4(names + 0x00, 5, res, res2);
            *(void **)(hud + 0x30) = fn_802077A4(names + 0x10, 8, res, res2);
        } else {
            *(void **)(hud + 0x28) = 0;
            *(void **)(hud + 0x30) = 0;
        }
        *(void **)(hud + 8) = fn_802077A4(names + 0x1C, 5, res, res2);
        *(long *)(hud + 0x5C) = 0;
        memset(hud + 0x10, 0, 8);
        *(void **)(hud + 0xC) = 0;
        for (i = 0; i < 2; i++) {
            HudPart *part;
            fn_801345D8(buf, 0x20, names + 0x2C, i + 1);
            part = (HudPart *)fn_802077A4(buf, 5, res, res2);
            ((HudPart **)(hud + 0x10))[i] = part;
            if (part == 0) break;
            part->owner = owner;
            HudPart_SetLevel(((HudPart **)(hud + 0x10))[i], lbl_80563368);
            *(void **)(hud + 0xC) = ((HudPart **)(hud + 0x10))[i];
            (*(long *)(hud + 0x5C))++;
        }
        if (isCar) {
            *(void **)(hud + 0x18) = fn_802077A4(names + 0x38, 5, res, res2);
        } else {
            *(void **)(hud + 0x18) = 0;
        }
        if (*(HudPart **)(hud + 8) != 0) {
            (*(HudPart **)(hud + 8))->owner = owner;
            HudPart_SetLevel(*(HudPart **)(hud + 8), lbl_8056336C);
        }
        if (*(HudPart **)(hud + 0x28) != 0) (*(HudPart **)(hud + 0x28))->owner = owner;
        if (*(HudPart **)(hud + 0x30) != 0) (*(HudPart **)(hud + 0x30))->owner = owner;
        if (isCar) {
            *(void **)(hud + 0x1C) = fn_802077A4(names + 0x4C, 5, res, res2);
        } else {
            *(void **)(hud + 0x1C) = 0;
        }
        *(void **)(hud + 0x20) = fn_802077A4(names + 0x5C, 5, res, res2);
        *(void **)(hud + 0x34) = fn_802077A4(names + 0x68, 5, res, res2);
        if (isCar) {
            *(void **)(hud + 0x38) = fn_802077A4(names + 0x78, 5, res, res2);
            *(void **)(hud + 0x3C) = fn_802077A4(names + 0x88, 5, res, res2);
        } else {
            *(void **)(hud + 0x38) = 0;
            *(void **)(hud + 0x3C) = 0;
        }
        if (*(HudPart **)(hud + 0x34) != 0) (*(HudPart **)(hud + 0x34))->owner = owner;
        if (*(HudPart **)(hud + 0x38) != 0) (*(HudPart **)(hud + 0x38))->owner = owner;
        if (*(HudPart **)(hud + 0x3C) != 0) (*(HudPart **)(hud + 0x3C))->owner = owner;
        *(void **)(hud + 0x40) = fn_802077A4(names + 0x98, 5, res, res2);
        if (*(HudPart **)(hud + 0x40) != 0) (*(HudPart **)(hud + 0x40))->owner = owner;
        if (isCar) {
            *(void **)(hud + 0x24) = fn_802077A4(names + 0xA8, 5, res, res2);
            *(void **)(hud + 0x2C) = fn_802077A4(names + 0xB8, 5, res, res2);
            *(void **)(hud + 0x48) = fn_802076A0(names + 0xCC, 1, owner, 2);
            *(void **)(hud + 0x4C) = fn_802076A0(names + 0xDC, 1, owner, 2);
            *(void **)(hud + 0x50) = fn_802076A0(names + 0xEC, 1, owner, 2);
            *(void **)(hud + 0x44) = fn_802076A0(names + 0x100, 1, owner, 0);
            *(void **)(hud + 0x54) = fn_802076A0(names + 0x110, 1, owner, 0);
        } else {
            *(void **)(hud + 0x24) = 0;
            *(void **)(hud + 0x2C) = 0;
            *(void **)(hud + 0x48) = 0;
            *(void **)(hud + 0x4C) = 0;
            *(void **)(hud + 0x50) = 0;
            *(void **)(hud + 0x44) = 0;
            *(void **)(hud + 0x54) = 0;
        }
        *(float *)(hud + 0x60) = lbl_80563370;
        *(unsigned char *)(hud + 0x70) = 0;
        *(float *)(hud + 0x64) = lbl_80563370;
        *(float *)(hud + 0x68) = lbl_80563370;
        *(float *)(hud + 0x6C) = lbl_80563370;
        *(unsigned char *)(hud + 0x58) = 0;
        *(unsigned char *)(hud + 0x72) = 0;
        *(unsigned char *)(hud + 0x71) = (FindVehicle(*(long *)(vehicle + 0x25C))[0xA0 / 4] >> 2) & 1;
    }
    return hud;
}

extern "C" void fn_80128668(void *);
extern "C" void *fn_8020E288(void *arg0, s32 arg1) {
    struct Dummy { virtual ~Dummy(); };
    void *child;
    u8 *p;
    u32 i;
    if (arg0 != 0) {
        child = *(void **)((u8 *)arg0 + 8);
        *(void **)arg0 = lbl_8033C8C4;
        if (child != 0) delete (Dummy *)child;
        p = (u8 *)arg0;
        i = 0;
        do {
            child = *(void **)(p + 0x10);
            if (child != 0) delete (Dummy *)child;
            p += 4;
            i++;
        } while (i < 2);
        child = *(void **)((u8 *)arg0 + 0x18);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x28);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x30);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x1C);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x20);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x24);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x2C);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x48);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x4C);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x50);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x44);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x34);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x38);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x3C);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x40);
        if (child != 0) delete (Dummy *)child;
        child = *(void **)((u8 *)arg0 + 0x54);
        if (child != 0) delete (Dummy *)child;
        if (arg1 > 0) fn_80128668(arg0);
    }
    return arg0;
}
#pragma scheduling on
#pragma cplusplus reset

extern long fn_80009C84(void);
extern void fn_80206750(void *, void *, long);

// Applies the current mode (0 while fn_80009C84 is set, otherwise 2) to the three optional parts.
void fn_8020F15C(void *arg0) {
    void *owner = *(void **)((char *)*(void **)((char *)arg0 + 4) + 0x1EC);
    long busy = fn_80009C84();
    long mode = 2;
    if (busy) mode = 0;
    if (*(void **)((char *)arg0 + 0x48) != 0)
        fn_80206750(*(void **)((char *)arg0 + 0x48), owner, mode);
    if (*(void **)((char *)arg0 + 0x4C) != 0)
        fn_80206750(*(void **)((char *)arg0 + 0x4C), owner, mode);
    if (*(void **)((char *)arg0 + 0x50) != 0)
        fn_80206750(*(void **)((char *)arg0 + 0x50), owner, mode);
}

void fn_8020F1F8(void *arg0) {
    void *temp_r3;
    temp_r3 = *(void **)((char *)arg0 + 0x50);
    if (temp_r3 != 0) {
        *(signed char *)((char *)temp_r3 + 0x49) = 1;
        *(signed char *)((char *)temp_r3 + 0x4B) = 0;
        *(signed char *)((char *)temp_r3 + 0x4A) = 0;
    }
}

void fn_8020F21C(void *arg0) {
    void *temp_r3;
    temp_r3 = *(void **)((char *)arg0 + 0x28);
    if (temp_r3 != 0) {
        *(signed char *)((char *)temp_r3 + 0x49) = 1;
        *(signed char *)((char *)temp_r3 + 0x4B) = 0;
        *(signed char *)((char *)temp_r3 + 0x4A) = 0;
    }
}

void fn_8020F240(void *arg0) {
// MATCHING-AID: register pinning from upstream; revisit.
    register void *p;
    float value;
    p = *(void **)((char *)arg0 + 8);
    if (p != 0) {
        value = lbl_80563368;
        if (value != *(float *)((char *)p + 8)) {
            *(signed char *)((char *)p + 0x4D) = 1;
            *(float *)((char *)p + 8) = value;
        }
        p = *(void **)((char *)arg0 + 8);
        *(signed char *)((char *)p + 0x49) = 1;
        *(signed char *)((char *)p + 0x4B) = 0;
        *(signed char *)((char *)p + 0x4A) = 0;
    }
}
void fn_8020F284(void *arg0, float farg0) {
    float temp_f0;
    float temp_f1;
    float var_f2;
    void *temp_r4;
    void *temp_r4_2;
    temp_r4 = *(void **)((char *)arg0 + 0x1C);
    if (temp_r4 != 0) {
        var_f2 = lbl_80563368;
        temp_f0 = (farg0 - lbl_805633DC) / lbl_805633F4;
        var_f2 = var_f2 < temp_f0 ? var_f2 : temp_f0;
        var_f2 = var_f2 > lbl_80563370 ? var_f2 : lbl_80563370;
        temp_f1 = lbl_805633F8 + (var_f2 * lbl_805633FC);
        if (temp_f1 != *(float *)((char *)temp_r4 + 8)) {
            *(signed char *)((char *)temp_r4 + 0x4D) = 1;
            *(float *)((char *)temp_r4 + 8) = temp_f1;
        }
        temp_r4_2 = *(void **)((char *)arg0 + 0x1C);
        *(signed char *)((char *)temp_r4_2 + 0x49) = 1;
        *(char *)((char *)temp_r4_2 + 0x4B) = 0;
        *(char *)((char *)temp_r4_2 + 0x4A) = 0;
    }
}

void fn_8020F30C(void *arg0) {
    void *temp_r3;
    temp_r3 = *(void **)((char *)arg0 + 0x24);
    if (temp_r3 != 0) {
        *(signed char *)((char *)temp_r3 + 0x49) = 1;
        *(signed char *)((char *)temp_r3 + 0x4B) = 0;
        *(signed char *)((char *)temp_r3 + 0x4A) = 0;
    }
}

void fn_8020F330(void *arg0) {
    void *temp_r3;

    temp_r3 = *(void **)((char *)arg0 + 0x20);
    if (temp_r3 != 0) {
        *(signed char *)((char *)temp_r3 + 0x49) = 1;
        *(signed char *)((char *)temp_r3 + 0x4B) = 0;
        *(signed char *)((char *)temp_r3 + 0x4A) = 0;
    }
}
void fn_8020F354(void *arg0) {
    void *temp_r3;
    temp_r3 = *(void **)((char *)arg0 + 0x2C);
    if (temp_r3 != 0) {
        *(signed char *)((char *)temp_r3 + 0x49) = 1;
        *(signed char *)((char *)temp_r3 + 0x4B) = 0;
        *(signed char *)((char *)temp_r3 + 0x4A) = 0;
    }
}
void fn_8020F378(void *arg0) {
    void *temp_r3;

    temp_r3 = *(void **)((char *)arg0 + 0x44);
    if (temp_r3 != 0) {
        *(signed char *)((char *)temp_r3 + 0x49) = 1;
        *(signed char *)((char *)temp_r3 + 0x4B) = 0;
        *(signed char *)((char *)temp_r3 + 0x4A) = 0;
    }
}
void fn_8020F39C(void) {

}

void fn_8020F3A0(void *arg0) {
    *(signed char *)((char *)arg0 + 0x72) = 1;
}
extern const float lbl_80563368;
extern const float lbl_80563370;
extern const float lbl_805633DC;
extern const float lbl_805633F4;
extern const float lbl_805633F8;
extern const float lbl_805633FC;
