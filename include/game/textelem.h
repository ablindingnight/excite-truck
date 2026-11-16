#ifndef GAME_TEXTELEM_H
#define GAME_TEXTELEM_H
// A layout's text element (ovlayout.cpp reads them from .val tables): the element base (name, position,
// justification), then the text's scale, justification, colour and default text. One declaration for every unit
// (port ODR work, 2026-11-16). Names unproven.
#include "game/common.h"
#include "game/v2.h"

struct ElemBase {
    ElemBase(void) {}
    const char* name;  // offset 0x0
    V2 pos;            // offset 0x4
    s32 elemJust;      // offset 0xC
};

struct TextElem;
extern "C" void fn_801455B8(TextElem* e, ValueNode* n);  // ovlayout: reads a text element

// Text element (0x20).
struct TextElem : ElemBase {
    void Read(ValueNode* n) { fn_801455B8(this, n); }
    f32 scale;            // offset 0x10
    u32 just;             // offset 0x14
    u32 color;            // offset 0x18
    const char* defText;  // offset 0x1C
};

#endif
