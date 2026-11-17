#ifndef GAME_HASCHILD_H
#define GAME_HASCHILD_H
// True when a settings node has the named child (eventpath, gmat, pathcareer, difficulty carried identical copies);
// one definition for the port (aids audit 3, 2026-11-17: each unit converted only where code-identical).
#include "game/common.h"

/* Helper name unproven */
static inline bool HasChild(ValueNode* n, const char* name) { return n->FindChild(name) != 0; }

#endif
