#ifndef GAME_TABLEREF_H
#define GAME_TABLEREF_H
// TableRef: one declaration for the units that each carried an identical copy (arenabnd, trackobj). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"

struct TableRef {
    TableRef(void) { node = 0; }
    TableRef(const char* what, const char* name);
    ~TableRef(void);
    ValueNode* node;
};

#endif
