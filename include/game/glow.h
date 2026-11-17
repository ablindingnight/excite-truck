#ifndef GAME_GLOW_H
#define GAME_GLOW_H

#include "game/common.h"

/* The vector without constructors: by-value copies of it are word copies (DrawNextGate). */
struct PlainVec3 {
    f32 x;
    f32 y;
    f32 z;
};

struct ZoneVec3 : PlainVec3 {
    ZoneVec3(void) {}
    ZoneVec3(f32 x_, f32 y_, f32 z_) {
        x = x_;
        y = y_;
        z = z_;
    }
    void Set(f32 x, f32 y, f32 z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    ZoneVec3 operator+(const ZoneVec3& b) const { return ZoneVec3(x + b.x, y + b.y, z + b.z); }
    ZoneVec3 operator-(const ZoneVec3& b) const { return ZoneVec3(x - b.x, y - b.y, z - b.z); }
    ZoneVec3 operator*(f32 s) const { return ZoneVec3(x * s, y * s, z * s); }
};

// The gate and trigger-zone outline renderer.
class Glow {
public:
    Glow(void);
    ~Glow(void);
    void Draw(ZoneVec3* pts, int n, int c);
    s32 lite;  // offset 0x0
    s32 glow;  // offset 0x4
};

#endif
