#ifndef GAME_AIEXC_H
#define GAME_AIEXC_H
// AI push/pass tuning (pushpass.cpp): the event's default tuning and per-track exception ranges (on_lon..off_lon)
// that override it. LineInfo (lineinfo.cpp) holds one at 0x40; aicar looks up the tuning for a lap position.
// Names unproven apart from the value keys ("exception", on_lon/off_lon).
#include "game/common.h"

struct ValueNode;

struct AITune {
    f32 onT;        // offset 0x0
    f32 moveout;    // offset 0x4
    f32 passout;    // offset 0x8
    f32 pushSens;   // offset 0xC
    f32 lookahead;  // offset 0x10
    f32 checks;     // offset 0x14
    f32 landing;    // offset 0x18
};

struct AIException {
    f32 onLon;          // offset 0x0
    f32 offLon;         // offset 0x4
    AITune tune;        // offset 0x8
    AIException* next;  // offset 0x24
};

class AIExceptions {
public:
    AIExceptions(void);
    ~AIExceptions(void);
    void Load(ValueNode* node);
    AITune* Find(f32 lon);

    AIException* list;  // offset 0x0
    AITune tune;        // offset 0x4
};

#endif
