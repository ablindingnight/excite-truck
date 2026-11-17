#ifndef GAME_PRELOAD_H
#define GAME_PRELOAD_H
// Preloaders (preload.cpp): objects that link themselves into a list; a pass totals their sizes, reserves the
// memory and has each one load. Its table has three slots (Size, Load pure; the destructor), as the
// tree and ringchal arena tables show (the 4th word after it is alignment). Names unproven.
#include "game/common.h"

struct ListNode {
    ListNode* next;  // offset 0x0
};

class Preloader : public ListNode {
public:
    Preloader(void);
    virtual s32 Size(void) = 0;  // 0x08
    virtual void Load(void) = 0;  // 0x0C
    virtual ~Preloader(void);     // 0x10
};

#endif
