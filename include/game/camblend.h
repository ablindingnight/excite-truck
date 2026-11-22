#ifndef GAME_CAMBLEND_H
#define GAME_CAMBLEND_H

#include "types.h"

struct RMtx;  // game/rmtx.h

// camframe.cpp's camera-frame blend: out = a..b at t (rotation through quaternions, position lerped). One declaration
// for every caller (cscript, loading, namedcam, racerslot); each passes its own 0x30 pose view as an RMtx
// (port ODR work, 2026-11-16).
extern "C" void InterpolateCameraMatrix(RMtx* out, const RMtx* a, const RMtx* b, f32 t);

#endif
