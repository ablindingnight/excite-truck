#ifndef GAME_FLOATVALUE_H
#define GAME_FLOATVALUE_H

#include "game/common.h"
#include "game/optionvalue.h"

// A float option value: range, step, display decimals (uitable.cpp defines it; uiapi builds them). Names are guesses.
class FloatValue : public OptionValue {
public:
    FloatValue(f32* v, f32 lo, f32 hi, f32 step, f32 min, f32 max);
    virtual ~FloatValue(void) {}
    virtual void GetPtr(void** p, int* size);
    virtual void Format(char* buf);
    virtual void Inc(void);
    virtual void Dec(void);
    virtual int CanInc(void);
    virtual int CanDec(void);
    virtual f32 GetFrac(void);
    virtual void SetFrac(f32 f);

    f32 lo;        // offset 0x4
    f32 hi;        // offset 0x8
    f32 min;       // offset 0xC
    f32 max;       // offset 0x10
    f32 step;      // offset 0x14
    f32* value;    // offset 0x18
    s32 decimals;  // offset 0x1C
};

#endif
