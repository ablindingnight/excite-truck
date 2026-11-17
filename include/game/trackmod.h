#ifndef GAME_TRACKMOD_H
#define GAME_TRACKMOD_H

#include "game/common.h"

// The track-model handler base (trackmod.cpp), shared by trackmod, drone and fabric, which each had their own copy.
// graf.cpp keeps a declaration without virtuals: it defines the empty default slots, and retail graf emits no TrackMod
// table (the copies are the linker-blanked weak tables in drone/fabric), so including this there would add one.
// Slot names are guesses.
struct TObj;
struct TNode;

class TrackMod {
public:
    virtual ~TrackMod(void) {}
    virtual void Swap(TObj* o);                                // 0xC
    virtual void Init(TObj* o);                                // 0x10
    virtual void Unk14(void);                                  // 0x14
    virtual void Apply(void);                                  // 0x18
    virtual void Unk1C(void);                                  // 0x1C
    virtual void Draw(TObj* list);                             // 0x20
    virtual void DrawTree(TNode* tree);                        // 0x24
    virtual void Add(void* scene, const TObj* o, s32 id);      // 0x28
    virtual void Link(int a, void* b, int c, void* d, int e);  // 0x2C
};

#endif
