#ifndef GAME_FXSEQ_H
#define GAME_FXSEQ_H
// A loaded effect sequence (fxseq.cpp): timed effect nodes played at a position; fire and sparks play them.
// FXSequence is retail's name (Load__10FXSequenceFPCc and friends); field names are guesses.
#include "game/common.h"

class FXSeqNode;
struct ValVec3;

class FXSequence {
public:
    static FXSequence* Load(const char* name);
    f32 GetLength(void);
    void Free(void);
    int Play(f32 t, const ValVec3* pos);

    FXSeqNode* nodes;  // offset 0x0
    int count;         // offset 0x4
    f32 length;        // offset 0x8
};

#endif
