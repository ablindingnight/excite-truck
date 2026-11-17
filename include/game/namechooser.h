#ifndef GAME_NAMECHOOSER_H
#define GAME_NAMECHOOSER_H
// The on-screen keyboard that edits a name (keyboard.cpp; nameent runs it). Names unproven.
#include "game/common.h"
#include "game/menustate.h"

class NameChooser : public MenuState {
public:
    NameChooser(char* buf, int max);
    virtual ~NameChooser(void);
    virtual void Enter(void);
    virtual void Exit(void);
    virtual void Changed(int key, void* value);
    virtual void Draw(void);
    virtual int Key(int key);
    virtual void Accept(void);     // 0x60
    virtual void Backspace(void);  // 0x64
    virtual void Caps(void);       // 0x68
    int IsDone(void);
    int Length(void);

    char* buf;    // offset 0x18
    int max;      // offset 0x1C
    s32 font;     // offset 0x20
    void* box;    // offset 0x24
    u8 done;      // offset 0x28
    bool caps;    // offset 0x29
    int row;      // offset 0x2C
    int col;      // offset 0x30
    int unk34;    // offset 0x34
};

#endif
