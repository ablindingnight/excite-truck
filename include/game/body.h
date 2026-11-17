#ifndef GAME_BODY_H
#define GAME_BODY_H

#include "game/common.h"

// A physical body as the event code sees it (an interface: no destructor slot; the first virtual is at 0x8). Shared
// by actseq and betarace, which each had their own view of different slots. Vectors are 3 floats. Slot names are guesses.
class Body {
public:
    virtual ValueNode* Desc(void);                                         // 0x08: its parameter node
    virtual void vf0C(void);
    virtual const f32* Pos(void);                                          // 0x10
    virtual f32* Vel(void);                                                // 0x14
    virtual s16 Surface(void);                                             // 0x18
    virtual void vf1C(void);
    virtual void SetActive(int on);                                        // 0x20
    virtual void Throw(const f32* vel, const f32* at);                     // 0x24
    virtual void SetState(const f32* m, const f32* vel, const f32* spin);  // 0x28
    virtual void Respawn(void);                                            // 0x2C
};

#endif
