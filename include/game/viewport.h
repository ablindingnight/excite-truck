#ifndef GAME_VIEWPORT_H
#define GAME_VIEWPORT_H

// Scoped GX viewport and perspective setters (mrender.cpp defines the constructors/destructors; retail's weak
// __ct__8ViewportFiiii): construct to set, destroy to restore. Built-in types only: widgets.cpp includes it
// without common.h. Names are guesses.
struct Viewport {
    int x;  // offset 0x0
    int y;  // offset 0x4
    int w;  // offset 0x8
    int h;  // offset 0xC
    Viewport(int x, int y, int w, int h);
    ~Viewport(void);
};

struct Perspective {
    Perspective(void) {}
    Perspective(float near, float far, float fov);
    ~Perspective(void);
    float near;  // offset 0x0
    float far;   // offset 0x4
    float fov;   // offset 0x8
};

#endif
