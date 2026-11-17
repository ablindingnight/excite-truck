#ifndef GAME_PROXER_H
#define GAME_PROXER_H
// The race's car proximity tracker (proxer.cpp): per-car records (CarRec, stride-spaced) and the car pairs it
// keeps sorted by closeness for the AI. One declaration for every unit (port ODR work, 2026-11-16). Names unproven.
#include "game/common.h"
#include "game/v2.h"

struct Pair {
    Pair(void) {
        b = -1;
        dvel.y = 0.0f;
        dvel.x = 0.0f;
        dpos.y = 0.0f;
        dpos.x = 0.0f;
        a = -1;
        close = 0;
    }
    void UpdateDist(void);
    void SetPos(const struct CarRec* a, const struct CarRec* b);

    u8 close;    // offset 0x0
    f32 time;    // offset 0x4
    u8 aFlag;    // offset 0x8
    s32 a;       // offset 0xC
    u8 bFlag;    // offset 0x10
    s32 b;       // offset 0x14
    f32 dist2;   // offset 0x18
    V2 dpos;     // offset 0x1C
    V2 dvel;     // offset 0x24
};

struct CarRec {
    V2 pos;       // offset 0x0
    V2 vel;       // offset 0x8
    f32 accel;    // offset 0x10
    u8 flipped;   // offset 0x14
    u8 active;    // offset 0x15
    u8 sec;       // offset 0x16
    Pair* table[1];  // offset 0x18 ([3][count])

    void ClearVel(void) {
        vel.x = 0.0f;
        vel.y = 0.0f;
    }
};

class Proxer;
typedef int (Proxer::*PairCmp)(Pair* a, Pair* b);

class Proxer {
public:
    /* Helper name unproven */
    Pair* PairAt(int index) { return &pairs[index]; }
    Proxer(s32 n);
    ~Proxer(void);
    void Build(void);
    void Activate(int i);
    void Update(int i);
    void SetActive(int i, u8 on);
    void SortCar(int i);
    void UpdateAll(void);
    void Process(void);
    int Compare(Pair* a, Pair* b);
    void Sort(Pair** list, PairCmp cmp, int key);

    CarRec* Rec(int i) { return (CarRec*)(recs + i * stride); }

    s32 count;    // offset 0x0
    s32 key;      // offset 0x4
    Pair* pairs;  // offset 0x8
    s32 stride;   // offset 0xC
    u8* recs;     // offset 0x10
    f32 lapLen;   // offset 0x14
};

#endif
