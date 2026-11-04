#ifndef GAME_MENUWIDGET_H
#define GAME_MENUWIDGET_H

#include "types.h"

// A menu value widget: an option cycler shown on a menu row (the vptr follows the base data, at +0xC).
// The base virtuals are menu.cpp's vf08__10MenuWidgetFv..vf14__10MenuWidgetFv / vf28__10MenuWidgetFv /
// vf2C__10MenuWidgetFv (unk_80215E68.c); __vt__10MenuWidget is at 0x8033D720. Derived widgets: controls'
// ControlsChoiceFields, popwarn's ValueLabel, setmenu's StringMulti, unitmenu's ValueMulti.
class MenuWidget {
public:
    MenuWidget(void) {
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
    virtual ~MenuWidget(void) {}
    virtual const char* Current(void) = 0;  // the label shown for the current value
    virtual void Prev(void) = 0;
    virtual void Next(void) = 0;
    virtual int vf28(void);
    virtual int vf2C(void);
};

#endif
