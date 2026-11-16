#ifndef GAME_WEATHER_H
#define GAME_WEATHER_H

#include "game/common.h"

// The race's weather settings (weather.cpp; fn_801E1F44 returns the current one). Shared by its readers
// (carlights, sunflare) for the port (one definition).
struct WeatherDef {
    s32 precip;      // offset 0x0 (0 none, 1 rain, 2 snow)
    u8 night;        // offset 0x4
    u32 fogColor;    // offset 0x8
    f32 fogStart;    // offset 0xC
    f32 fogStop;     // offset 0x10
    f32 fogDensity;  // offset 0x14
    u8 hasFog;       // offset 0x18
    int Clear(void) { return night == 0 && precip == 0; }  // dry daylight (sunflare)
};

#endif
