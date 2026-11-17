#ifndef GAME_TRACKMOD_H
#define GAME_TRACKMOD_H

#include "game/common.h"

// The track-model handler base (trackmod.cpp), shared by trackmod, drone and fabric, which each had their own copy.
// Every slot is inline or pure (no key function): fabric and drone keep their dead weak tables as zero blocks
// (FACTS 2026-11-17 08:29) and define TRACKMOD_DECL empty; elsewhere the linker strips the copy and the novtable
// default is the stand-in (MATCHING-AID, as COLLSHAPE_BASE_DECL). graf.cpp's retail copies of the default slots sit
// among graf's own functions, so graf keeps a declaration without virtuals that defines them in place.
// Slot names are guesses.
struct TObj;
struct TNode;

#ifndef TRACKMOD_DECL
#define TRACKMOD_DECL __declspec(novtable)
#endif

class TRACKMOD_DECL TrackMod {
public:
    virtual ~TrackMod(void) {}
    virtual void Swap(TObj* o) {}                                  // 0xC
    virtual void Init(TObj* o) {}                                  // 0x10
    virtual void Unk14(void) {}                                    // 0x14
    virtual void Apply(void) {}                                    // 0x18
    virtual void Unk1C(void) {}                                    // 0x1C
    virtual void Draw(TObj* list) = 0;                             // 0x20
    virtual void DrawTree(TNode* tree) {}                          // 0x24
    virtual void Add(void* scene, const TObj* o, s32 id) {}        // 0x28
    virtual void Link(int a, void* b, int c, void* d, int e) {     // 0x2C
        Add(b, (const TObj*)a, c);
    }
};

#endif
