#ifndef GAME_GAMEMODECTOR_H
#define GAME_GAMEMODECTOR_H
// GameMode's constructor body. Units include this where retail has it: before use to inline it (amaster,
// crushmaster, zone, betarace), at the end to call the out-of-line copy (ringchal). modeinl.cpp defines the
// out-of-line copy itself (port decision: see handoff PORT_ODR_NAMES).
#include "game/gamemode.h"

inline GameMode::GameMode(void) {}

#endif
