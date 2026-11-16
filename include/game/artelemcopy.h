#ifndef GAME_ARTELEMCOPY_H
#define GAME_ARTELEMCOPY_H
// ArtElem's member-wise copy and assignment (declared in game/textelem.h). A unit includes this where retail's
// code has the copy available: before its uses to inline it (ovlayout), after them to call the out-of-line weak copy
// (betarace, crashmode).
#include "game/textelem.h"

inline ArtElem::ArtElem(const ArtElem& o) : BoxElem(o), u0(o.u0), v0(o.v0), u1(o.u1), v1(o.v1) {}

inline ArtElem& ArtElem::operator=(const ArtElem& o) {
    BoxElem::operator=(o);
    u0 = o.u0;
    v0 = o.v0;
    u1 = o.u1;
    v1 = o.v1;
    return *this;
}

#endif
