#ifndef GAME_MENUWIDGET_H
#define GAME_MENUWIDGET_H

#include "types.h"

// A menu value widget: an option cycler shown on a menu row (the vptr follows the base data, at +0xC).
// MenuWidgetBase's virtuals vf08..vf14 (its 0x18 vtable is __vt__14MenuWidgetBase, 0x8033D750) and MenuWidget's
// vf28/vf2C are defined in unk_80215E68.c / widgetwin.cpp; __vt__10MenuWidget is at 0x8033D720. Derived widgets:
// controls' ControlsChoiceFields, popwarn's ValueLabel, setmenu's StringMulti, unitmenu's ValueMulti.
//
// This header only DECLARES the virtuals. A unit that also includes menuwidget_inl.h sees their inline bodies and
// emits weak copies of both vtables; the linker keeps the menu unit's first copies and zero-fills the duplicates'
// space (the 0x48-byte .data tails of controls and setmenu). A unit that sees only the declarations (unitmenu)
// refers to the shared tables and emits none, like the original (KC 2026-11-04).
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

    virtual u8 vf08(void);
    virtual u8 vf0C(void);
    virtual int vf10(void);
    virtual int vf14(void);
};

class MenuWidget : public MenuWidgetBase {
public:
    MenuWidget(void) {}

    virtual ~MenuWidget(void) {}
    virtual const char* Current(void) = 0;  // the label shown for the current value
    virtual void Prev(void) = 0;
    virtual void Next(void) = 0;
    virtual int vf28(void);
    virtual int vf2C(void);
};

#endif
