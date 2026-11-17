#ifndef GAME_UPDATABLE_H
#define GAME_UPDATABLE_H
// An object updated every frame (updlist.cpp keeps the list): constructing one registers it, the destructor
// unregisters it. Some units' retail code saw other member forms (a pure or inline destructor); those keep
// their own declaration under __MWERKS__. Slot names unproven.
#include "game/common.h"

class Updatable;
extern "C" void fn_801D0DDC(Updatable* u);  // register (updlist.cpp)

class Updatable {
public:
    Updatable(void) { fn_801D0DDC(this); }
    virtual ~Updatable(void);
    virtual void vf0C(void) = 0;
    virtual void vf10(void) = 0;
    virtual void vf14(void) = 0;
    virtual void vf18(void) = 0;
};

#endif
