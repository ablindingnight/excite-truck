#ifndef GAME_CAMFRAME34_H
#define GAME_CAMFRAME34_H

#include <types.h>
#include "game/rmtx.h"

// A 3x4 frame (rotation rows, then position) with the out-of-line, empty constructor retail calls before filling one
// in (__ct__10CamFrame34Fv, defined in modeinl.cpp). Its storage is a 12-float array: retail copies it as words
// (bonemgr fn_802D9DE4), which an RMtx assignment does not; Mtx() views the same bytes as rmtx.h's RMtx
// (panelvol's identity frames). One declaration for every unit (port ODR work, 2026-11-16). Name unproven.

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
    RMtx* Mtx(void) { return (RMtx*)m; }
    f32 m[12];
};

#endif
