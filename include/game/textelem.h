#ifndef GAME_TEXTELEM_H
#define GAME_TEXTELEM_H
// A layout's elements (ovlayout.cpp reads them from .val tables): the element base (name, position,
// justification); text elements add scale, justification, colour and default text; art (textured quad) and gart
// (coloured box) elements add a size, then UVs or a colour. One declaration for every unit (port ODR work,
// 2026-11-16). Type names pinned by ovlayout's mangled templates (DynArray<7ArtElem>, ChannelHandler<8GartElem>);
// field names unproven.
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

struct ArtElem;
struct GartElem;
extern "C" void LoadArtElementDimensions(ArtElem* e, ValueNode* n);   // ovlayout: reads an art element
extern "C" void fn_80145538(GartElem* e, ValueNode* n);  // ovlayout: reads a gart element

// Elements with a size (art and boxes).
struct BoxElem {
    BoxElem(void) {}
    const char* name;  // offset 0x0
    V2 pos;            // offset 0x4
    s32 elemJust;      // offset 0xC
    f32 w;  // offset 0x10
    f32 h;  // offset 0x14
};

// Art element (0x28): a textured quad.
struct ArtElem : BoxElem {
    ArtElem(void) {}
    ArtElem(const ArtElem& o);              // defined in game/artelemcopy.h
    ArtElem& operator=(const ArtElem& o);  // defined in game/artelemcopy.h
    void Read(ValueNode* n) { LoadArtElementDimensions(this, n); }
    f32 u0;  // offset 0x18
    f32 v0;  // offset 0x1C
    f32 u1;  // offset 0x20
    f32 v1;  // offset 0x24
};

// Gart element (0x1C): a coloured box.
struct GartElem : BoxElem {
    void Read(ValueNode* n) { fn_80145538(this, n); }
    u32 color;  // offset 0x18
};

#endif
