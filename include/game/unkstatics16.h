#ifndef GAME_UNKSTATICS16_H
#define GAME_UNKSTATICS16_H
// UnkStaticS16: one declaration for the units that each carried an identical copy (dbglod, romfont, stub18, vidsetup). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"

class UnkStaticS16 {
public:
    UnkStaticS16(s16 v) { val = v; }
    s16 val;
};

#endif
