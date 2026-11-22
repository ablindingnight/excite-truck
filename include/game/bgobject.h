#ifndef GAME_BGOBJECT_H
#define GAME_BGOBJECT_H

#include "game/common.h"

// A background-updated object: bg.cpp keeps a list of them (RegisterBgTaskObject adds one under a name, RemoveBgObject removes
// it) and calls Update each frame. res.cpp's AsyncSetQueue and soundmgr.cpp's SoundManager derive from it. Retail's
// weak __dt__8BgObjectFv / __vt__8BgObject come from the inline destructor. Names are guesses.
class BgObject {
public:
    virtual ~BgObject(void) {}
    virtual void Update(void) = 0;  // 0xC
};

extern "C" {
void RegisterBgTaskObject(BgObject* obj, const char* name);  // add
void RemoveBgObject(BgObject* obj);                    // remove
}

#endif
