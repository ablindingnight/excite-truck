#ifndef GAME_GAMEOVERLAY_H
#define GAME_GAMEOVERLAY_H
// GameOverlay: one declaration for the units that each carried an identical copy (overlay, racing). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"

// Camera: common.h's PlainValMatrix.
class GameOverlay {
public:
    virtual ~GameOverlay(void) {}
    virtual void Attach(void) {}                                          // 0xC
    virtual void Detach(void) {}                                          // 0x10
    virtual void Enter(void) {}                                           // 0x14
    virtual void Leave(void) {}                                           // 0x18
    virtual void Reset(void) {}                                           // 0x1C
    virtual void Draw(void) {}                                            // 0x20
    virtual void Update(void) = 0;                                        // 0x24
    virtual int IsFullScreen(void) { return 0; }                          // 0x28
    virtual void DrawPlayer(int player) {}                                // 0x2C
    virtual int HandleKey(int player, u32 type, int key) { return 0; }   // 0x30
    virtual const char* GetName(void) { return "<unknown>"; }            // 0x34
};

#endif
