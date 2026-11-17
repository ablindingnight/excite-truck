#ifndef GAME_ROADSOUND_H
#define GAME_ROADSOUND_H
// A car's road-surface sounds (roadsnd.cpp; tiresnd owns one per car and updates it): one voice per surface,
// with speed/volume/pitch curves from roadsnd.val. The constructor and its string helpers are inline in
// roadsnd.cpp. Names unproven.
#include "game/common.h"

class RoadSoundVoice;

struct RoadSoundEntry {
    RoadSoundVoice* voice;  // offset 0x0
    s32 surface;            // offset 0x4
    f32 speed1;             // offset 0x8
    f32 speed2;             // offset 0xC
    f32 speed3;             // offset 0x10
    f32 vol2;               // offset 0x14
    f32 vol3;               // offset 0x18
    f32 pitch1;             // offset 0x1C
    f32 pitch3;             // offset 0x20
};

class RoadSound {
    static const char* Path();                /* Helper name unproven */
    static const char* SurfaceKey();          /* Helper name unproven */
    static const char* MissingVoiceFormat();  /* Helper name unproven */

public:
    RoadSound(char* car);  // inline in roadsnd.cpp
    virtual ~RoadSound(void);
    virtual void Update(void);

    char* car;                    // offset 0x4
    int count;                    // offset 0x8
    RoadSoundEntry entries[8];    // offset 0xC
};

#endif
