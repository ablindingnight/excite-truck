#ifndef GAME_RECORDICON_H
#define GAME_RECORDICON_H
// RecordIcon: one declaration for the units that each carried an identical copy (frname, recscreen). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"
#include "game/caricontable.h"
#include "game/gart.h"
#include "game/menustate.h"

class RecordIcon : public MenuState {
public:
    RecordIcon(void) {
        icons = 0;
        visible = 0;
        art = fn_8023512C("caricona.art");
    }
    virtual ~RecordIcon(void) { fn_80235380(art); }
    virtual void Draw(void);

    CarIconTable* icons;  // offset 0x18
    s32 icon;         // offset 0x1C
    u8 visible;       // offset 0x20
    u32 color;        // offset 0x24
    gArtSet* art;          // offset 0x28
    u8 valid;         // offset 0x2C
};

#endif
