#ifndef GAME_STATBAR_H
#define GAME_STATBAR_H

#include "game/common.h"
#include "game/menustate.h"
#include "game/roll_strings.h"

extern "C" {
void fn_802328D4(u8* p);
u8* fn_80232578(const char* name);
void fn_80230C30(f32* x, f32* y, s32 a, s32 b);
void fn_80232BA4(const char* text, u8* font, f32 x, f32 y, s32 color, int align, f32 size, f32 a, f32 b);
int fn_801345D8(char* buf, int n, const char* fmt, ...);
const char* fn_80131234(const char* key);
extern const s32 lbl_802FA300[3];  // stat level thresholds (mroll.cpp)
extern const f32 lbl_80563758;     // 0.0
extern const f32 lbl_8056375C;     // 0.43
extern const f32 lbl_80563760;     // 0.37
extern const f32 lbl_80563764;     // 1.0
}

// One stat bar: a menu state that draws one stat's caption (vtable lbl_80342734 in mroll). Every member is inline:
// mroll.cpp (car select) and garage.cpp both build StatPanels, so each emits weak copies of the constructor, the
// destructor, the virtuals and the table. The linker keeps mroll's; garage's table copy is the blanked 0x64 block
// between SlotStrip's and SessionScreen's tables, and its "uifnt_o.gtx" / "UICar:STAT_%i_TEXT_%i" strings stay in
// garage's .data. FACTS 2026-11-17 "zero blocks".
class StatBar : public MenuState {
public:
    StatBar(void) {
        index = -1;
        value = 0;
        flag = 0;
        font = fn_80232578(RollStrings::StatFont());
    }
    virtual ~StatBar(void) {
        value = 0;
        flag = 0;
        fn_802328D4(font);
        font = 0;
    }
    virtual void Exit(void) {}
    virtual void Update(void) {}
    virtual void Draw(void) {
        f32 x, y;
        fn_80230C30(&x, &y, this->x, this->y);
        if (index >= 0) {
            int lvl = 0;
            for (int i = 0; i < 3; i++) {
                s32 v = *value;
                if (v && v >= lbl_802FA300[i]) {
                    lvl = i + 1;
                }
            }
            char buf[0x1E];
            fn_801345D8(buf, 0x1E, RollStrings::StatKey(), index, lvl);
            int align = 10;
            f32 size = lbl_8056375C;
            if (*flag) {
                size = lbl_80563760;
                align = 9;
            }
            fn_80232BA4(fn_80131234(buf), font, x, y, -0x100, align, size, lbl_80563758, lbl_80563764);
        }
    }
    virtual void Set(int i, s32* v, u8* f) {
        value = v;
        index = i + 1;
        flag = f;
    }

    u8* font;    // offset 0x18
    s32 index;        // offset 0x1C: stat number + 1 (-1 = none)
    u8* flag;         // offset 0x20
    s32* value;       // offset 0x24
};

#endif
