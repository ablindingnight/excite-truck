#ifndef GAME_VEHCOLOR_H
#define GAME_VEHCOLOR_H

#include "game/common.h"

// A vehicle's paint in a player's garage slot (driver.cpp owns the table; GetVehicleColor returns an entry).
struct VehicleColor {
    s32 body;  // offset 0x0
    s32 trim;  // offset 0x4
    u8 unk8;   // offset 0x8
};

#endif
