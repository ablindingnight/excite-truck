#ifndef GAME_CAMRECT_H
#define GAME_CAMRECT_H

#include <types.h>

class CamRect {
public:
    // Inline: retail emits it once, at the end of camfov (the unit whose __sinit builds the CamRect array), and it
    // reads camfov's 0.0 there.
    CamRect(void) {
        f32 z = 0.0f;
        v[0] = z;
        v[1] = z;
        v[3] = z;
        v[2] = z;
    }
    f32 v[4];
};

#endif
