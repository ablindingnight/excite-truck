#ifndef GAME_MENU_H
#define GAME_MENU_H

#include "game/common.h"

// MenuItem::arg0: an int, or for type-16 items (sub-menu / picker) a pointer to the item's state. Pointer-sized on
// the PC; plain int under MWCC (the Wii layout, code-identical). Port hazard ptr2int, 2026-11-16.
#ifdef __MWERKS__
typedef int MenuArg;
#else
#include <stdint.h>
typedef intptr_t MenuArg;
#endif

// Menu records handed to fn_802146BC: five words per item, ended by an item whose type is
// 0x7B (each unit's UnkStatic123 object).
struct CaptionContext {
    int value;
    CaptionContext(int v = 0) : value(v) {}
};

struct MenuItem {
    MenuItem(const char* n, CaptionContext context, const char* t) {
        name = n;
        type = 3;
        data = t;
        arg0 = context.value;
        arg1 = 0;
    }

    // A text item (type 3).
    MenuItem(const char* n, const char* t, CaptionContext context = CaptionContext()) {
        type = 3;
        name = n;
        data = t;
        arg0 = (int)context.value;
        arg1 = 0;
    }
    MenuItem(const char* n, f32* v, const struct MenuRange& r);  // a slider item (defined out of line)
    MenuItem(const char* n, int t, const void* d, MenuArg a0, int a1) {
        type = t;
        name = n;
        data = d;
        arg0 = a0;
        arg1 = a1;
    }
    MenuItem(const UnkStatic123& end) {
        name = (const char*)end.unk0;
        type = end.unk4;
        data = (const void*)end.unk8;
        arg0 = end.unkC;
        arg1 = end.unk10;
    }
    MenuItem(const UnkStatic123s& end) {
        name = (const char*)end.unk0;
        type = end.unk4;
        data = (const void*)end.unk8;
        arg0 = end.unkC;
        arg1 = end.unk10;
    }
    const char* name;  // offset 0x0
    int type;          // offset 0x4
    const void* data;  // offset 0x8
    MenuArg arg0;      // offset 0xC (a pointer for type-16 items)
    int arg1;          // offset 0x10
};

struct MenuDesc {
    const char* layout;  // offset 0x0
    MenuItem* items;     // offset 0x4
    int (*poll)(int*);   // offset 0x8
    int unkC;            // offset 0xC
    int unk10;           // offset 0x10
};

extern "C" int fn_802146BC(MenuDesc* desc, int x, int y, int modal);

#endif
