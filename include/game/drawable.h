#ifndef GAME_DRAWABLE_H
#define GAME_DRAWABLE_H
// An updatable object that also draws (drawlist.cpp keeps the list; carelem and cargear's managers are ones).
// Slot names unproven.
#include "game/common.h"
#include "game/updatable.h"

class Drawable : public Updatable {
public:
    Drawable(void);
    virtual ~Drawable(void);
    virtual void vf1C(s32 a) = 0;
    virtual void vf20(s32 a) = 0;
};

#endif
