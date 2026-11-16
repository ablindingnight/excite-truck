#ifndef GAME_DAVMAP_H
#define GAME_DAVMAP_H
// A driver-and-vehicle map (davmap.cpp): which drivers race and which vehicles they get. One declaration for every
// unit (port ODR work, 2026-11-16).
#include "game/common.h"

class DAVMap {
public:
    void Add(s32 driver, s32 vehicle);
    void Append(DAVMap* other);

    s32 num_drivers;   // offset 0x0
    s32 drivers[8];    // offset 0x4
    s32 num_vehicles;  // offset 0x24
    s32 vehicles[8];   // offset 0x28
};

#endif
