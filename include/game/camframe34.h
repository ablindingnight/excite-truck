#ifndef GAME_CAMFRAME34_H
#define GAME_CAMFRAME34_H

#include <types.h>

struct CamFrame34 {
    CamFrame34(void);
    void Identity(void) {
        m[0] = 1.0f;
        m[1] = 0.0f;
        m[2] = 0.0f;
        m[3] = 0.0f;
        m[4] = 1.0f;
        m[5] = 0.0f;
        m[6] = 0.0f;
        m[7] = 0.0f;
        m[8] = 1.0f;
        m[9] = 0.0f;
        m[10] = 0.0f;
        m[11] = 0.0f;
    }
    f32 m[12];
};

#endif
