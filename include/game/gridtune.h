#ifndef GAME_GRIDTUNE_H
#define GAME_GRIDTUNE_H

#include "game/common.h"

// The grid lane offset as a one-field const struct (deity lbl_805625B8, placecar lbl_80562700, each 5.0f): retail keeps
// it as the unit's first .sdata2 entry and negates it at run time, which a plain const f32 would fold. Name unproven.
struct GridTune { f32 laneOffset; };

#endif
