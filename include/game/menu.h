#ifndef GAME_MENU_H
#define GAME_MENU_H

#include "game/common.h"

// Menu records handed to fn_802146BC: five words per item, ended by an item whose type is
// 0x7B (each unit's UnkStatic123 object).
struct CaptionContext {
    int value;
    CaptionContext(int v = 0) : value(v) {}
};

struct MenuItem {
    // A text item (type 3).
    MenuItem(const char* n, const char* t, CaptionContext context = CaptionContext()) {
        type = 3;
        name = n;
        data = t;
        arg0 = (int)context.value;
        arg1 = 0;
    }
    MenuItem(const char* n, int t, const void* d, int a0, int a1) {
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
    int arg0;          // offset 0xC
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
