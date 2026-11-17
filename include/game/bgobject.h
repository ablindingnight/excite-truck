#ifndef GAME_BGOBJECT_H
#define GAME_BGOBJECT_H

#include "game/common.h"

// A background-updated object: bg.cpp keeps a list of them (fn_80127CF4 adds one under a name, fn_80127E28 removes
// it) and calls Update each frame. res.cpp's AsyncSetQueue and soundmgr.cpp's SoundManager derive from it. Retail's
// weak __dt__8BgObjectFv / __vt__8BgObject come from the inline destructor. Names are guesses.
class BgObject {
public:
    virtual ~BgObject(void) {}
    virtual void Update(void) = 0;  // 0xC
};

extern "C" {
void fn_80127CF4(BgObject* obj, const char* name);  // add
void fn_80127E28(BgObject* obj);                    // remove
}

#endif
