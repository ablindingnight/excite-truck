#ifndef GAME_ARTPLAYER_H
#define GAME_ARTPLAYER_H

#include "game/common.h"

struct ArtXform;  // artanim.cpp
struct ArtList;   // artanim.cpp

// A player for a set of named art animations (artanim.cpp): add by name, start/pause/resume, update and draw
// through an optional art transform. awards, betarace, crushmaster, ringchal and zone own one. Names are guesses.
class ArtPlayer {
public:
    ArtPlayer(int n);
    ~ArtPlayer(void);
    int Add(const char* name, int map);
    void Start(int i);
    void Pause(int i);
    void Resume(int i);
    void StopAll(void);
    void Rewind(void);
    void Update(void);
    void Draw(ArtXform* xf);  // the art transform (or 0)

    ArtList* list;  // offset 0x0
    f32 t;          // offset 0x4
};

#endif
