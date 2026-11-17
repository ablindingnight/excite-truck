#ifndef GAME_MENUSTATE_H
#define GAME_MENUSTATE_H

#include "game/common.h"

struct gxCanvas;

extern "C" void fn_80221E7C(void*);

// The handler interface: all pure apart from the empty destructor, and novtable (no table in any retail unit and
// no vptr store in MenuState's constructor or destructor).
class __declspec(novtable) IMenuState {
public:
    virtual ~IMenuState(void) = 0;
    virtual void Enter(void) = 0;
    virtual void Exit(void) = 0;
    virtual void Unk14(void) = 0;
    virtual void Changed(int key, void* value) = 0;
    virtual void Update(void) = 0;
    virtual void Unk20(void) = 0;
    virtual void Unk24(gxCanvas*) = 0;
    virtual void Draw(void) = 0;
    virtual void Unk2C(void) = 0;
    virtual void Unk30(void) = 0;
    virtual void Unk34(void) = 0;
    virtual void Unk38(void) = 0;
    virtual void Unk3C(void) = 0;
    virtual void Unk40(void) = 0;
    virtual void Unk44(void) = 0;
    virtual void Unk48(void) = 0;
    virtual void Unk4C(void) = 0;
    virtual void Unk50(void) = 0;
    virtual int Unk54(void) = 0;
    virtual int Key(int key) = 0;
    virtual int Unk5C(void) = 0;
};

inline IMenuState::~IMenuState(void) {}

// A menu widget's handler state. Menu items of type 5 point at one; derived handlers override the slots they need.
// The whole class is header-inline: every unit that builds a derived handler emits its own weak copy of this table
// and of the default slots it inherits. The linker keeps the first copy in link order, which is uiapi's (the table
// at 0x8033D618 and the functions at 0x802153A0 / 0x80215CA0..); the later units' copies of the TABLE keep their
// space and stay zero-filled in the DOL (the 0x60 zero block in each of those units' .data).
// The constructor clears unk14, which is why every derived ctor stores unk14 before its vptr.
class MenuState : public IMenuState {
public:
    MenuState(void) { unk14 = 0; }
    virtual ~MenuState(void) {}
    virtual void Enter(void) {}   // 0x0C
    virtual void Exit(void) {}    // 0x10
    virtual void Unk14(void) {}
    virtual void Changed(int key, void* value) { fn_80221E7C(this); }  // 0x18: a property changed (UICustom::Slot74 forwards here)
    virtual void Update(void) {}  // 0x1C
    virtual void Unk20(void) {}
    virtual void Unk24(gxCanvas*) {}  // 0x24: draw hook, given the canvas (UICustomHandler fills its rect)
    virtual void Draw(void) {}    // 0x28
    virtual void Unk2C(void) {}
    virtual void Unk30(void) {}
    virtual void Unk34(void) {}
    virtual void Unk38(void) {}
    virtual void Unk3C(void) {}
    virtual void Unk40(void) {}
    virtual void Unk44(void) {}
    virtual void Unk48(void) {}
    virtual void Unk4C(void) {}
    virtual void Unk50(void) {}
    virtual int Unk54(void) { return 0; }
    virtual int Key(int key) { return 0; }   // 0x58
    virtual int Unk5C(void) { return 0; }

    void Finish(int r);

    s32 x;      // offset 0x4
    s32 y;      // offset 0x8
    s32 w;      // offset 0xC
    s32 h;      // offset 0x10
    int unk14;  // offset 0x14
};

#endif
