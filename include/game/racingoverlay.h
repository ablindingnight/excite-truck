#ifndef GAME_RACINGOVERLAY_H
#define GAME_RACINGOVERLAY_H
// The in-race HUD overlay (racing.cpp; overlay.cpp creates one per player). Names unproven.
#include "game/common.h"
#include "game/gameoverlay.h"

struct gArtSet;
class TrackMap;
class ILayout;

class RacingOverlay : public GameOverlay {
public:
    RacingOverlay(int player);
    virtual ~RacingOverlay(void);
    virtual void Enter(void);
    virtual void Reset(void);
    virtual void Draw(void);
    virtual void Update(void);
    virtual int HandleKey(int player, u32 type, int key) { return 0; }
    virtual const char* GetName(void) { return "RacingOverlay"; }

    void DrawTimerBox(f32 fade, const char* title, int showLaps, const char* label1, f32 time1,
                      const char* label2, f32 time2);
    void UpdateSpeed(void);
    void DrawLapInfo(void);
    void DrawTimedLap(void);
    int IsTimedLap(void);
    int ShowLapInfo(void);
    void UpdateLapInfo(void);
    int DrawWarnings(void);
    void DrawMarker(f32 t, int index, f32 x, f32 y, f32 w, f32 h);
    void DrawProgress(void);
    void DrawStandings(void);
    void DrawScore(void);
    void DrawNeedle(f32 x, f32 y, f32 len, f32 width, f32 angle);
    void DrawHeat(void);
    int DrawTryAgain(void);

    s32 player;  // offset 0x4
    u8 unk8;     // offset 0x8
    f32 unkC;    // offset 0xC
    f32 speed;   // offset 0x10
    s32 unk14;   // offset 0x14
    s32 view;    // offset 0x18
    f32 unk1C;   // offset 0x1C
    u8 unk20;    // offset 0x20
    s32 state;   // offset 0x24
    f32 unk28;   // offset 0x28
    f32 fade;    // offset 0x2C
    u8 pad30[0x34 - 0x30];
    gArtSet* raceInfoArt;  // offset 0x34
    gArtSet* arrowsArt;    // offset 0x38
    gArtSet* uiArt;        // offset 0x3C
    gArtSet* zoneArt;      // offset 0x40
    s32 zoneTex;      // offset 0x44
    TrackMap* map;    // offset 0x48
    u8 units;         // offset 0x4C
    ILayout* layout;   // offset 0x50
};

#endif
