#ifndef GAME_TRACKINST_H
#define GAME_TRACKINST_H
// A per-scene track-model instance (trackmod.cpp's lists, fabric's fabric instances, drone's drone managers):
// slot 0x0C steps it (the drone managers advance their pools, the fabric instances forward to their manager's
// Update), 0x14 draws it. Names unproven.

class TrackInst {
public:
    virtual ~TrackInst(void) {}
    virtual void Update(void* ctx);  // 0x0C
    virtual int Kind(void);          // 0x10
    virtual void Draw2(void* ctx);   // 0x14
};

#endif
