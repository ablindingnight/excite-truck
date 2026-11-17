#ifndef GAME_STOPSOUND_H
#define GAME_STOPSOUND_H
// Null-checked stop of an optional game sound (betarace, stunts, zone carried identical copies); one definition for
// the port (aids audit 3, 2026-11-17: each unit converted only where code-identical).
#include "game/sound.h"

/* Helper name unproven */
static inline void StopSound(GameSound* s) {
    if (s) {
        s->stop = 1;
        s->restart = 0;
    }
}

#endif
