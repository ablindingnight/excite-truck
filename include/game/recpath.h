#ifndef GAME_RECPATH_H
#define GAME_RECPATH_H
// Recorded paths (path.cpp): a "PPth" file holds named paths of timed position + rotation keys; a PathFollower
// steps along one (pathman.cpp's clients hold one each). Named RecPath here: eventpath.cpp's career bracket path
// owns the name Path (__vt__4Path). One declaration for every unit (port ODR work, 2026-11-16). Names unproven.
#include "game/common.h"

struct PathKey;  // path.cpp: time, position, rotation (0x20)

// A named path (0x18).
struct RecPath {
    char name[0x10];  // offset 0x0
    s32 count;        // offset 0x10
    PathKey* keys;    // offset 0x14
};

// A loaded path file: the path count, then the paths.
struct PathFile {
    s32 count;         // offset 0x0
    RecPath paths[1];  // offset 0x4
};

class PathFollower {
public:
    PathFollower(void);
    ~PathFollower(void);
    PathFile* file;  // offset 0x0
    RecPath* path;   // offset 0x4
    s32 idx;         // offset 0x8
    f32 t;           // offset 0xC
};

#endif
