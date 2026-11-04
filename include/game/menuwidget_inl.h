#ifndef GAME_MENUWIDGET_INL_H
#define GAME_MENUWIDGET_INL_H

#include "game/menuwidget.h"

// Inline bodies of the menu widget base virtuals (see menuwidget.h for who includes this and why).
extern "C" int fn_802E4A10(void);

inline u8 MenuWidgetBase::vf08(void) { return flag8; }

inline u8 MenuWidgetBase::vf0C(void) { return flag0; }

inline int MenuWidgetBase::vf10(void) {
    int result = 0;
    if (flag0 != 0 && id == fn_802E4A10()) {
        result = 1;
    }
    return result;
}

inline int MenuWidgetBase::vf14(void) { return 1; }

inline int MenuWidget::vf28(void) { return 1; }

inline int MenuWidget::vf2C(void) { return 1; }

#endif
