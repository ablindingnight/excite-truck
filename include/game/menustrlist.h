#ifndef GAME_MENUSTRLIST_H
#define GAME_MENUSTRLIST_H
// MenuStrList: one declaration for the units that each carried an identical copy (mcdialog, nameent, opsound). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"

struct MenuStrList {
    u8 pad0[0x14];
};

#endif
