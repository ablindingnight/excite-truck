#ifndef GAME_TRACKINST_H
#define GAME_TRACKINST_H
// A per-scene track-model instance (trackmod.cpp's lists, fabric's fabric instances, drone's drone managers):
// slot 0x0C steps it (the drone managers advance their pools, the fabric instances forward to their manager's
// Update), 0x14 draws it. Names unproven.
//
// Every slot is inline or pure (no key function), so each unit that builds an instance emits a weak table. Retail
// keeps the dead copies in fabric and drone as zero blocks (their .data is addressed from the section start, FACTS
// 2026-11-17 08:29), so those units define TRACKINST_DECL empty. In units whose .data is not (trackmod) the linker
// strips the copy and the header's novtable default is the stand-in (MATCHING-AID, as COLLSHAPE_BASE_DECL).
#ifndef TRACKINST_DECL
#define TRACKINST_DECL __declspec(novtable)
#endif

class TRACKINST_DECL TrackInst {
public:
    virtual ~TrackInst(void) {}
    virtual void Update(void* ctx) = 0;           // 0x0C
    virtual int Kind(void) { return -1; }         // 0x10
    virtual void Draw2(void* ctx) {}              // 0x14
};

#endif
