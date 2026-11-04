#ifndef GAME_MENUWIDGET_H
#define GAME_MENUWIDGET_H

#include "types.h"

// A menu value widget: an option cycler shown on a menu row (the vptr follows the base data, at +0xC).
// The base virtuals are menu.cpp's vf08__10MenuWidgetFv..vf14__10MenuWidgetFv / vf28__10MenuWidgetFv /
// vf2C__10MenuWidgetFv (unk_80215E68.c); __vt__10MenuWidget is at 0x8033D720. Derived widgets: controls'
// ControlsChoiceFields, popwarn's ValueLabel, setmenu's StringMulti, unitmenu's ValueMulti.
extern "C" int fn_802E4A10(void);

// The 4-virtual base of every menu value widget (its 0x18 vtable is lbl_8033D750). All virtuals are inline, so every
// unit that constructs a widget emits weak copies of this table and MenuWidget's; the linker keeps the first
// (the menu unit's, 0x8033D720/0x8033D750) and leaves the other copies' space zero-filled (controls/setmenu 0x48
// .data tails; KC 2026-11-04).
class MenuWidgetBase {
public:
    MenuWidgetBase(void) {
        flag0 = 0;
        id = -1;
        flag8 = 0;
    }

    u8 flag0;  // offset 0x0
    s32 id;    // offset 0x4
    u8 flag8;  // offset 0x8

    virtual u8 vf08(void) { return flag8; }
    virtual u8 vf0C(void) { return flag0; }
    virtual int vf10(void) {
        int result = 0;
        if (flag0 != 0 && id == fn_802E4A10()) {
            result = 1;
        }
        return result;
    }
    virtual int vf14(void) { return 1; }
};

class MenuWidget : public MenuWidgetBase {
public:
    MenuWidget(void) {}

    virtual ~MenuWidget(void) {}
    virtual const char* Current(void) = 0;  // the label shown for the current value
    virtual void Prev(void) = 0;
    virtual void Next(void) = 0;
    virtual int vf28(void) { return 1; }
    virtual int vf2C(void) { return 1; }
};

#endif
