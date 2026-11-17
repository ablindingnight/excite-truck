#ifndef GAME_OPTIONVALUE_H
#define GAME_OPTIONVALUE_H
// OptionValue: one declaration for the units that each carried an identical copy (uiapi, uitable). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"

class OptionValue {
public:
    virtual ~OptionValue(void) {}
    virtual void GetPtr(void** p, int* size) = 0;
    virtual void Format(char* buf) = 0;
    virtual void Inc(void) = 0;
    virtual void Dec(void) = 0;
    virtual int CanInc(void) = 0;
    virtual int CanDec(void) = 0;
    virtual f32 GetFrac(void) = 0;
    virtual void SetFrac(f32 f) = 0;
};

#endif
