#ifndef GAME_CAMFRAME34_H
#define GAME_CAMFRAME34_H

#include <types.h>
#include "game/rmtx.h"

// A 3x4 frame (rotation rows, then position) with the out-of-line, empty constructor retail calls before filling one
// in (__ct__10CamFrame34Fv, defined in modeinl.cpp). It IS an RMtx (rmtx.h): retail copies one as two runs of words,
// 9 for the rotation and 3 for the position (bonemgr fn_802D9DE4 EXACT, 2026-11-17: MWCC copies a struct one
// top-level member at a time, so a flat 12-float array copies as one run and scores 96.48). One declaration for
// every unit (port ODR work, 2026-11-16). Name unproven.

struct CamFrame34 : RMtx {
    CamFrame34(void);
    void Identity(void) {
        rot.x.x = 1.0f;
        rot.x.y = 0.0f;
        rot.x.z = 0.0f;
        rot.y.x = 0.0f;
        rot.y.y = 1.0f;
        rot.y.z = 0.0f;
        rot.z.x = 0.0f;
        rot.z.y = 0.0f;
        rot.z.z = 1.0f;
        pos.x = 0.0f;
        pos.y = 0.0f;
        pos.z = 0.0f;
    }
    RMtx* Mtx(void) { return this; }
};

#endif
