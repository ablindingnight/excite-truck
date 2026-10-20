// Racing HUD (0x80013BD0-0x80016354): the per-player RacingOverlay drawn during a race - lap counter,
// lap/best/last timer box, place, damage text, progress bar and standings, wrong-way/reset prompts,
// the arena score, the track map and the engine temperature gauge with its 3D needle. Names are
// guesses.
#include <math.h>
#include "game/common.h"

// One element of a layout set (0x20 bytes); copied, moved and drawn.
struct LayoutItem {
    u32 unk0;  // offset 0x0
    struct {
        f32 x;
        f32 y;
    } pos;      // offset 0x4
    u32 unkC;   // offset 0xC
    f32 scale;  // offset 0x10
    u32 unk14;  // offset 0x14
    u32 color;  // offset 0x18
    u32 unk1C;  // offset 0x1C
};

// A box element of a layout set (0x1C bytes).
struct LayoutBox {
    u32 unk0;  // offset 0x0
    struct {
        f32 x;
        f32 y;
    } pos;      // offset 0x4
    s32 unkC;   // offset 0xC
    f32 w;      // offset 0x10
    f32 h;      // offset 0x14
    u32 color;  // offset 0x18
};

class Layout {
public:
    virtual ~Layout(void);
    virtual LayoutItem* Find(const char* name);    // 0xC
    virtual void* FindArt(const char* name);       // 0x10
    virtual LayoutBox* FindBox(const char* name);  // 0x14
};

struct EventInfo {
    u32 unk0;  // offset 0x0
    u32 type;  // offset 0x4
};

struct RaceProgress {
    s32 leaderLap;  // offset 0x0
    u8 pad4[0x4];
    s32 laps;     // offset 0x8
    f32 start;    // offset 0xC
    f32 elapsed;  // offset 0x10
};

struct ResultRacer {
    u8 pad0[0x48];
    char name[1];  // offset 0x48
};

struct TruckState {
    u8 pad0[0x34];
    f32 unk34;  // offset 0x34
    u8 pad38[0x4C - 0x38];
    f32 speed;  // offset 0x4C
    u8 pad50[0x28C - 0x50];
    f32 damage[3];  // offset 0x28C
    u8 pad298[0x2A8 - 0x298];
    f32 heat;  // offset 0x2A8
    u8 pad2AC[0x2C0 - 0x2AC];
    u32 flags;     // offset 0x2C0
    f32 overheat;  // offset 0x2C4
    f32 lapDist;   // offset 0x2C8
};

struct RacerStats {
    u32 flags;  // offset 0x0
    s32 place;  // offset 0x4
    u8 pad8[0x10 - 0x8];
    s32 lap;      // offset 0x10
    f32 lapTime;  // offset 0x14
    f32 best;     // offset 0x18
    f32 last;     // offset 0x1C
    u8 pad20[0x24 - 0x20];
    s32 state;  // offset 0x24
};

struct RaceInfo {
    u8 split;             // offset 0x0
    u8 active;            // offset 0x1
    s32 mode;             // offset 0x4
    EventInfo* event;     // offset 0x8
    RaceProgress* info;   // offset 0xC
    f32 time;             // offset 0x10
    s32 numPlayers;       // offset 0x14
    s32 numViews;         // offset 0x18
    s32 player;           // offset 0x1C
    u32 flags;            // offset 0x20
    s32 view;             // offset 0x24
    ResultRacer* racer;   // offset 0x28
    TruckState* truck;    // offset 0x2C
    RacerStats* stats;    // offset 0x30
};

struct Racer {
    u8 human;  // offset 0x0
    u8 pad1[0x8 - 0x1];
    TruckState* truck;  // offset 0x8
    u8 padC[0x1C - 0xC];
    s32 lap;  // offset 0x1C
};

struct HitInfo {
    s32 id;  // offset 0x0
    u8 pad4[0x4];
    f32 a;  // offset 0x8
    f32 b;  // offset 0xC
    u8 pad10[0x4];
    s32 type;  // offset 0x14
};

struct PlayerHits {
    u8 pad0[0x78];
    HitInfo hits[4];  // offset 0x78
    u8 padD8[0xE0 - 0xD8];
};

// The race UI / game controller returned by fn_801B83A0.
class GameUI {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void vf1C(void);
    virtual void vf20(void);
    virtual void vf24(void);
    virtual void vf28(void);
    virtual void vf2C(void);
    virtual void vf30(void);
    virtual void vf34(void);
    virtual void vf38(void);
    virtual void vf3C(void);
    virtual void vf40(void);
    virtual void vf44(void);
    virtual void vf48(void);
    virtual void vf4C(void);
    virtual void vf50(void);
    virtual void vf54(void);
    virtual int vf58(void) { return 1; }
};

struct Vec3 {
    f32 x;
    f32 y;
    f32 z;
};

struct Mat3 {
    f32 m[3][3];
};

class TrackMap;

extern "C" {
double cos(double x);
double sin(double x);
double fmod(double x, double y);
char* strcpy(char* dst, const char* src);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void fn_801476C4(LayoutItem* item);
void fn_801473F8(Layout* layout, const char* name, const char* fmt, ...);
void fn_80147230(LayoutItem* item, const char* fmt, ...);
void fn_80130FCC(char* buf, int size, s32 value);
void fn_80130978(char* buf, int size, f32 value);
const char* fn_80130FE0(s32 place);
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);
f32 fn_801E7E24(void);
int fn_802C9A58(void);
f32 fn_80011EB0(int player);
int fn_8023512C(const char* name);
void fn_80235380(int art);
s32 fn_802CAC28(const char* name, int flags);
void fn_802CAC54(s32 tex);
void fn_801333E8(int type, const char* name, u8* out);
TrackMap* fn_80019C68(int player);
void fn_8001A04C(TrackMap* map);
void fn_8001A1F4(TrackMap* map, f32 x, f32 y, f32 w, f32 h);
Layout* fn_80145E70(const char* name);
void fn_8000C630(s32 cam, int player);
int fn_8026B0C0(int player);
GameUI* fn_801B83A0(void);
void fn_801DF93C(int player);
void fn_8000FBE4(void);
int fn_801E866C(int player);
int fn_801E89C8(int player);
EventInfo* fn_801E87A8(void);
int fn_8000D75C(void);
void fn_8001DF84(int view);
int fn_8027AA48(void);
void fn_800111A4(const char* text, int a, int b, int c, int flags, f32 scale);
void fn_80010F38(const char* text, int a, int b, int flags, f32 scale);
PlayerHits* fn_801E7E34(void);
int fn_801E89EC(void);
int fn_80144F2C(void);
int fn_80144F34(int view, int team, u8* out);
void fn_80148134(f32* ox, f32* oy, f32 x, f32 y, s32 unk);
void fn_801D490C(void);
f32 fn_80235EF0(int art, int frame);
f32 fn_80235F64(int art, int frame);
void fn_80010E94(int art, int unk, f32 x, f32 y, f32 w, f32 h);
f32 fn_80011E60(void);
void fn_80010B80(f32 x, f32 y, f32 w, f32 h, u32 color);
f32 fn_8013672C(void);
RaceProgress* fn_801E7E3C(void);
Racer* fn_801EA9A0(int index);
int fn_801EA994(void);
ResultRacer* fn_801EA9B0(int index);
void fn_8000FF88(u32 color, int flags, const char* fmt, f32 scale, f32 x, f32 y, ...);
f32 fn_80011DEC(int player);
f32 fn_80011DC0(int player);
void fn_802354A0(int art, int frame, u32 color, f32 x, f32 y, f32 z, f32 w, f32 h);
void fn_802C8BD0(s32 tex);
void fn_801E4F20(int n);
void fn_801E4F3C(u32 color, int back, f32 x, f32 y, f32 z, f32 u, f32 v);
void fn_801E4FF4(void);
void fn_802C8D74(void);
void fn_801478E0(Layout* layout, const char* name, int art, int frame);
void fn_80147F30(f32 x, f32 y, f32 w, f32 h, u32 color, int segs, s32 tex, f32 start, f32 step,
                 f32 u0, f32 v0, f32 u1, f32 v1);
void fn_80011E04(int unk);
void fn_80010FDC(const char* text, int flags);
extern u8 lbl_8055E520;
}

int GetNumPlayers(void);

extern RaceInfo* lbl_8055D350;
extern f32 lbl_80562068;

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
    int raceInfoArt;  // offset 0x34
    int arrowsArt;    // offset 0x38
    int uiArt;        // offset 0x3C
    int zoneArt;      // offset 0x40
    s32 zoneTex;      // offset 0x44
    TrackMap* map;    // offset 0x48
    u8 units;         // offset 0x4C
    Layout* layout;   // offset 0x50
};

// HUD colours (not referenced by the surviving code).
class HudColors {
public:
    HudColors(void) {
        c[0] = 0xDC000000;
        c[1] = 0x90900000;
        c[2] = 0xC8800000;
        c[3] = 0xC8000060;
        c[4] = 0x7F000000;
        c[5] = 0x90000000;
        c[6] = 0xFFFFD03F;
        c[7] = 0xFFFFFFFF;
        c[8] = 0x64FFFFFF;
        c[9] = 0xA0000000;
        c[10] = 0x82000000;
        c[11] = 0x6E000000;
        c[12] = 0x58000000;
        c[13] = 0x2D000000;
        c[14] = 0x64000000;
    }
    u32 c[15];
};

ModuleTag lbl_8055D520;
UnkStaticFFFF lbl_8055D528;
HudColors lbl_803577A0;
UnkStatic7F lbl_8055D52C;

RacingOverlay* lbl_803577E0[4];

static inline f32 Min(f32 a, f32 b) { return a < b ? a : b; }
static inline f32 Max(f32 a, f32 b) { return a > b ? a : b; }
static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

void DrawLapCounter(Layout* layout, int lap, int total, int showTotal) {
    char num[20];
    char den[20];

    fn_801476C4(layout->Find("lap_title"));
    fn_801476C4(layout->Find("lap_fwd_slash"));
    fn_80130FCC(num, sizeof(num), lap);
    fn_801473F8(layout, "lap_numerator", num);
    if (showTotal) {
        fn_80130FCC(den, sizeof(den), total);
        fn_801473F8(layout, "lap_denominator", den);
    }
}

void FadeIn(f32* v) {
    if (fn_801E7E24() > 0.0f) {
        *v = Min(*v + fn_801E7E24() / 0.35f, 1.0f);
    }
}

void FadeOut(f32* v) {
    if (fn_801E7E24() > 0.0f) {
        *v = Max(*v - fn_801E7E24() / 0.35f, 0.0f);
    }
}

f32 Ease(f32 t) {
    f32 r;
    if (0.0f == t || 1.0f == t) {
        return t;
    }
    if (t < -1.0f) {
        r = -1.0f;
    } else if (t > 1.0f) {
        r = 1.0f;
    } else if (t > 0.0f) {
        r = t * (-2.0f * t * t) + 3.0f * t * t;
    } else {
        r = t * (-2.0f * t * t) - 3.0f * t * t;
    }
    return r;
}

// Stripped from ET; only its strings remain.
LayoutItem* GetDash(int player, int i) {
    char name[32];
    fn_801345D8(name, sizeof(name), "preferred_dash%d", i);
    return lbl_803577E0[player]->layout->Find(name);
}

LayoutItem* GetLapBody(int player) {
    return lbl_803577E0[player]->layout->Find("lap_body_0");
}

LayoutItem* GetLapBody1(int player) {
    if (fn_802C9A58()) {
        return lbl_803577E0[player]->layout->Find("lap_body_1_ws");
    } else {
        return lbl_803577E0[player]->layout->Find("lap_denominator");
    }
}

void DrawPlayerLapCounter(int player, int lap, int total) {
    DrawLapCounter(lbl_803577E0[player]->layout, lap < 1 ? 1 : lap, total, 0);
}

static inline void PrintInt(LayoutItem* item, s32 value) {
    char buf[20];
    fn_80130FCC(buf, sizeof(buf), value);
    fn_80147230(item, buf);
}

static inline void PrintTime(LayoutItem* item, f32 value) {
    char buf[20];
    fn_80130978(buf, sizeof(buf), value);
    fn_80147230(item, buf);
}

void RacingOverlay::DrawTimerBox(f32 fade, const char* title, int showLaps, const char* label1,
                                 f32 time1, const char* label2, f32 time2) {
    LayoutBox box;
    LayoutBox* p;
    int row;

    p = layout->FindBox("timer_box");
    if (p) {
        box = *p;
    } else {
        fn_802E317C("RaceOverlay: required layout element \"timer_box\" was not found!");
        box.pos.x = 433.0f;
        box.pos.y = 130.0f;
        box.w = 174.0f;
        box.h = 20.0f;
        box.unkC = 1;
        box.color = -1;
    }
    fade = Ease(fade);
    box.pos.x = Lerp(box.pos.x, 50.0f + (box.pos.x + box.w) + fn_80011EB0(lbl_8055D350->player), fade);
    row = 0;
    if (lbl_8055D350->active) {
        if (showLaps) {
            LayoutItem* it = layout->Find("laps_to_go_title");
            if (it) {
                LayoutItem item;
                item = *it;
                item.pos.x = box.pos.x;
                item.pos.y = box.pos.y + box.h * row;
                fn_801476C4(&item);
            }
            it = layout->Find("laps_to_go");
            if (it) {
                LayoutItem item;
                item = *it;
                item.pos.x = box.pos.x + box.w;
                item.pos.y = box.pos.y + box.h * row;
                PrintInt(&item, lbl_8055D350->info->laps - lbl_8055D350->stats->lap + 1);
            }
        } else {
            LayoutItem* it = layout->Find("laps_to_go_last_lap");
            if (it) {
                LayoutItem item;
                item = *it;
                item.pos.x = box.pos.x;
                item.pos.y = box.pos.y + box.h * row;
                fn_801476C4(&item);
            }
        }
        row = 1;
    }
    if (label1 && *label1) {
        LayoutItem* it = layout->Find("best_lap_title");
        if (it) {
            LayoutItem item;
            item = *it;
            item.pos.x = box.pos.x;
            item.pos.y = box.pos.y + box.h * row;
            fn_801476C4(&item);
        }
        it = layout->Find("best_lap");
        if (it) {
            LayoutItem item;
            item = *it;
            item.pos.x = box.pos.x + box.w;
            item.pos.y = box.pos.y + box.h * row;
            PrintTime(&item, time1);
        }
        row++;
    }
    if (label2 && *label2) {
        LayoutItem* it = layout->Find("timer_box_3_title");
        if (it) {
            LayoutItem item;
            item = *it;
            item.pos.x = box.pos.x;
            item.pos.y = box.pos.y + box.h * row;
            fn_801476C4(&item);
        }
        it = layout->Find("timer_box_3");
        if (it) {
            LayoutItem item;
            item = *it;
            item.pos.x = box.pos.x + box.w;
            item.pos.y = box.pos.y + box.h * row;
            PrintTime(&item, time2);
        }
    }
}

RacingOverlay::RacingOverlay(int player) {
    lbl_803577E0[player] = this;
    unk8 = 0;
    raceInfoArt = fn_8023512C("race_info.art");
    arrowsArt = fn_8023512C("arrows.art");
    uiArt = fn_8023512C("uielems.art");
    zoneArt = fn_8023512C("zonetex3.art");
    zoneTex = fn_802CAC28("zonetex3.tex", 0);
    this->player = player;
    units = 0;
    fn_801333E8(5, "units", &units);
    map = fn_80019C68(player);
    if (GetNumPlayers() == 1) {
        layout = fn_80145E70("raceovr.val");
    } else {
        layout = fn_80145E70("raceovr_2.val");
    }
    Reset();
}

void RacingOverlay::Reset(void) {
    unk14 = 6;
    unk1C = -1000.0f;
    unk20 = 0;
    fade = 1.0f;
    unk28 = 1.0f;
    state = 0;
}

RacingOverlay::~RacingOverlay(void) {
    lbl_803577E0[player] = 0;
    delete layout;
    fn_8001A04C(map);
    map = 0;
    fn_802CAC54(zoneTex);
    zoneTex = -2;
    fn_80235380(zoneArt);
    zoneArt = 0;
    fn_80235380(uiArt);
    uiArt = 0;
    fn_80235380(raceInfoArt);
    raceInfoArt = 0;
    fn_80235380(arrowsArt);
    arrowsArt = 0;
}

void RacingOverlay::Draw(void) {
    if (unk8) {
        if (lbl_8055D350->time - lbl_8055D350->info->start < lbl_80562068 - 0.25f) {
            fn_8000C630(0x17, -1);
        }
    } else {
        f32 t = lbl_8055D350->info->elapsed;
        int show = 0;
        int ok;
        int draw;
        if ((t > 0.25f && t < 10.0f) || t > 99.0f) {
            show = 1;
        }
        ok = !(lbl_8055D350->truck->flags & 0x20);
        draw = 0;
        if (show && ok) {
            draw = 1;
        }
        if (draw) {
            fn_8000C630(0x17, -1);
        }
    }
}

void RacingOverlay::Enter(void) { speed = lbl_8055D350->truck->speed; }

void RacingOverlay::UpdateSpeed(void) {
    f32 k;
    unkC = lbl_8055D350->truck->unk34;
    k = 0.44444445f * ((f32)fabs(speed - lbl_8055D350->truck->speed) / 10.0f);
    k = Min(1.0f, k);
    k = Max(k, 0.05f);
    speed = Lerp(speed, lbl_8055D350->truck->speed, k);
}

void RacingOverlay::DrawLapInfo(void) {
    char title[64];
    char label1[64];
    char label2[64];
    f32 time1 = 0.0f;
    f32 time2 = time1;
    int showLaps = 1;
    RaceInfo* ri;

    label1[0] = 0;
    label2[0] = 0;
    ri = lbl_8055D350;
    if (ri->mode != 1) {
        strcpy(title, "Lap  Info");
        if (lbl_8055D350->stats->best > 0.1f) {
            strcpy(label1, "Best:");
            time1 = lbl_8055D350->stats->best;
        }
    }
    if (lbl_8055D350->stats->last > 0.1f &&
        (lbl_8055D350->mode != 1 || lbl_8055D350->info->laps > 1)) {
        strcpy(label2, "LAST:");
        time2 = lbl_8055D350->stats->last;
    }
    if (lbl_8055D350->stats->state == 3) {
        if (lbl_8055D350->mode != 1) {
            strcpy(title, "RACE  OVER");
        } else {
            strcpy(title, "LAP  INFO");
            strcpy(label1, "QUALIFY  TIME:");
            time1 = lbl_8055D350->stats->best;
        }
    } else if (lbl_8055D350->mode == 1) {
        strcpy(title, "QUALIFY");
    } else if (lbl_8055D350->stats->lap == lbl_8055D350->info->laps ||
               lbl_8055D350->info->leaderLap >= lbl_8055D350->info->laps) {
        strcpy(title, "LAST  LAP");
        showLaps = 0;
    }
    if (IsTimedLap()) {
        time1 = lbl_8055D350->stats->state == 3 ? lbl_8055D350->stats->last
                                                : lbl_8055D350->stats->lapTime;
        strcpy(label1, "TIMED  LAP:");
    }
    DrawTimerBox(fade, title, showLaps, label1, time1, label2, time2);
}

void RacingOverlay::DrawTimedLap(void) {
    DrawTimerBox(fade, "LAP  INFO", 0, "TIMED  LAP",
                 lbl_8055D350->stats->state == 3 ? lbl_8055D350->stats->last
                                                 : lbl_8055D350->stats->lapTime,
                 "", 0.0f);
}

int RacingOverlay::IsTimedLap(void) { return 0; }

static inline int IsQualifying(void) {
    return lbl_8055D350->mode == 1 && lbl_8055D350->stats->lap > 0;
}

static inline int IsLapDone(void) {
    return lbl_8055D350->stats->last > 1.0f || lbl_8055D350->stats->lap == lbl_8055D350->info->laps;
}

static inline int HasLap(int q) { return IsLapDone() || q; }

static inline int JustFinished(int q) { return HasLap(q) && lbl_8055D350->stats->lapTime < 5.0f; }

int RacingOverlay::ShowLapInfo(void) {
    int q = IsQualifying();
    return JustFinished(q) || lbl_8055D350->stats->lap > lbl_8055D350->info->laps;
}

enum { STATE_IDLE, STATE_LAP, STATE_TIMED };

void RacingOverlay::UpdateLapInfo(void) {
    int timed = IsTimedLap();
    int show = ShowLapInfo();
    if (state == 0) {
        if (timed || show) {
            FadeOut(&fade);
            if (timed) {
                DrawTimedLap();
            } else {
                DrawLapInfo();
            }
            if (0.0f == fade) {
                state = timed ? STATE_TIMED : STATE_LAP;
            }
        } else {
            fade = 1.0f;
        }
    } else if (state == 2) {
        if (timed) {
            if (fade > 0.0f) {
                FadeOut(&fade);
            }
            DrawTimedLap();
        } else {
            FadeIn(&fade);
            DrawTimedLap();
            if (1.0f == fade) {
                state = 0;
            }
        }
    } else if (show) {
        if (fade > 0.0f) {
            FadeOut(&fade);
        }
        DrawLapInfo();
    } else {
        FadeIn(&fade);
        DrawLapInfo();
        if (1.0f == fade) {
            state = 0;
        }
    }
}

void RacingOverlay::Update(void) {
    int i;
    if (!lbl_8055D350->flags) {
        return;
    }
    if (lbl_8055D350->view != view) {
        Enter();
    }
    view = lbl_8055D350->view;
    if (fn_8026B0C0(lbl_8055D350->player)) {
        if (fn_801B83A0()->vf58()) {
            fn_801DF93C(lbl_8055D350->player);
        }
        return;
    }
    if (lbl_8055D350->player == 0) {
        fn_8000FBE4();
    }
    {
        RaceInfo* ri = lbl_8055D350;
        int hidden = 0;
        if (ri->view == fn_801E866C(ri->player) && fn_801E89C8(ri->player)) {
            hidden = 1;
        }
        fn_801E87A8();
        if (!hidden && (lbl_8055D350->flags & 8)) {
            if (!fn_8000D75C()) {
                UpdateSpeed();
            }
            if (lbl_8055D350->event->type & 0x80000) {
                DrawHeat();
                fn_8001DF84(lbl_8055D350->view);
            }
        }
    }
    if (lbl_8055D350->flags & 2) {
        fn_801473F8(layout, "place_numerator", "%d", lbl_8055D350->stats->place);
        fn_801473F8(layout, "place_suffix", "%s", fn_80130FE0(lbl_8055D350->stats->place));
    }
    if (lbl_8055D350->flags & 1) {
        int total = lbl_8055D350->info->laps;
        int lap = lbl_8055D350->stats->lap > 1 ? lbl_8055D350->stats->lap : 1;
        DrawLapCounter(layout, lap < total ? lap : total, total, 1);
    }
    if (lbl_8055D350->flags & 0x10) {
        TruckState* t = lbl_8055D350->truck;
        const char* text = "DAMAGE";
        int r = 100.0f * (1.0f - t->damage[0]);
        int g = 100.0f * (1.0f - t->damage[1]);
        int b = 100.0f * (1.0f - t->damage[2]);
        if (fn_8027AA48()) {
            fn_800111A4(text, r, g, b, 7, 1.0f);
        } else {
            fn_80010F38(text, r, g, 6, 1.0f);
        }
    }
    if (lbl_8055D350->flags & 0x20) {
        PlayerHits* hits = fn_801E7E34() + lbl_8055D350->player;
        HitInfo* hit = hits->hits;
        int j;
        for (j = 0; j < 4; j++, hit++) {
            if (hit->id >= 0) {
                if (hit->type > 9 && hit->type < 13) {
                    fn_80010F38("KILLED", 100.0f * (1.0f - hit->a), 100.0f * (1.0f - hit->b), 8, 1.0f);
                } else {
                    fn_80010F38("DAMAGE", 100.0f * (1.0f - hit->a), 100.0f * (1.0f - hit->b), 8, 1.0f);
                }
            }
        }
    }
    if ((lbl_8055D350->flags & 0x40) && lbl_8055D350->numViews < 3) {
        UpdateLapInfo();
    }
    {
        int warned = 0;
        if ((lbl_8055D350->flags & 0x600) && DrawWarnings()) {
            warned = 1;
        }
        if (fn_801E89EC() == 1 && !warned) {
            DrawTryAgain();
        }
    }
    if ((lbl_8055D350->flags & 0x2000) && lbl_8055D350->numViews == 1) {
        DrawStandings();
    }
    if ((lbl_8055D350->flags & 0x4000) && fn_80144F2C()) {
        DrawScore();
    }
    if (lbl_8055D350->flags & 0x1000) {
        if (map) {
            LayoutBox* box = layout->FindBox("track_map");
            if (box) {
                f32 x;
                f32 y;
                fn_80148134(&x, &y, box->pos.x, box->pos.y, box->unkC);
                fn_8001A1F4(map, x, y, box->w, box->h);
            }
        } else if (lbl_8055D350->player == 0) {
            DrawProgress();
        }
    }
    fn_801D490C();
}

int RacingOverlay::DrawWarnings(void) {
    int wrong;
    if ((lbl_8055D350->flags & 0x400) && (lbl_8055D350->truck->flags & 0x10)) {
        fn_801476C4(layout->Find("reset"));
    }
    wrong = 0;
    if ((lbl_8055D350->stats->flags & 4) && (lbl_8055D350->flags & 0x200)) {
        wrong = 1;
    }
    if (wrong) {
        fn_801476C4(layout->Find("wrong_way"));
    }
    return wrong;
}

void RacingOverlay::DrawMarker(f32 t, int index, f32 x, f32 y, f32 w, f32 h) {
    int valid = index >= 0;
    f32 aw = fn_80235EF0(arrowsArt, 0);
    f32 ah = fn_80235F64(arrowsArt, 0);
    f32 bottom = y - h;
    f32 px = Lerp(x - aw / 2, x + w - aw / 2, t);
    f32 top = y + ah;
    int flip = !valid;
    fn_80010E94(arrowsArt, flip, px, valid ? top : bottom, 0.0f, 0.0f);
}

void RacingOverlay::DrawProgress(void) {
    f32 y;
    int n;
    f32 lapLen;
    f32 total;
    int i;

    if (lbl_8055D350->numViews == 1) {
        y = 210.0f;
    } else {
        y = fn_80011E60();
    }
    n = 0;
    fn_80010B80(-165.0f, y, 330.0f, 5.0f, 0xC80000C8);
    lapLen = fn_8013672C();
    total = lapLen * fn_801E7E3C()->laps;
    for (i = 0; i < fn_801EA994(); i++) {
        TruckState* truck = fn_801EA9A0(i)->truck;
        f32 t = 0.0f;
        int lap = fn_801EA9A0(i)->lap;
        if (lap > 0) {
            t = (truck->lapDist + (lap - 1) * lapLen) / total;
        }
        DrawMarker(t, fn_801EA9A0(i)->human ? n++ : -1, -165.0f, y, 330.0f, 5.0f);
    }
}

void RacingOverlay::DrawStandings(void) {
    int n = fn_801EA994();
    int place = lbl_8055D350->stats->place;
    int before = 2;
    int after = 2;
    int i;
    f32 y;

    if (place <= 2) {
        before = place - 1;
        after += 2 - before;
    } else if (place > n - 2) {
        before += place - (n - 2);
        after = n - place;
    }
    before = before < 0 ? 0 : before;
    i = place - before;
    after = after < 0 ? 0 : after;
    y = 85.0f;
    for (i = i < 1 ? 1 : i; i < lbl_8055D350->stats->place; i++) {
        fn_8000FF88(-1, 9, "%d. %s", 1.0f, -280.0f, y, i, fn_801EA9B0(i - 1)->name);
        y -= 20.0f;
    }
    fn_8000FF88(-0x100, 9, "%d. %s", 1.0f, -280.0f, y, lbl_8055D350->stats->place,
                lbl_8055D350->racer->name);
    y -= 20.0f;
    for (i = lbl_8055D350->stats->place + 1; i <= lbl_8055D350->stats->place + after && i <= fn_801EA994();
         i++) {
        fn_8000FF88(-1, 9, "%d. %s", 1.0f, -280.0f, y, i, fn_801EA9B0(i - 1)->name);
        y -= 20.0f;
    }
}

void RacingOverlay::DrawScore(void) {
    u8 a;
    u8 b;
    int score0 = fn_80144F34(lbl_8055D350->view, 0, &a);
    int score1 = fn_80144F34(lbl_8055D350->view, 1, &b);
    f32 x = fn_80011DEC(lbl_8055D350->player) - 60.0f;
    f32 y = fn_80011DC0(lbl_8055D350->player) - 314.0f;
    fn_802354A0(uiArt, 3, -1, x, y, 0.0f, 60.0f, 45.0f);
    fn_8000FF88(-0x100, 10, "%d", 0.94f, x - 10.0f, y, score0);
    fn_8000FF88(-0x100, 10, "%d", 0.94f, x - 10.0f, y - 22.5f, score1);
}

void Mat3FromEuler(Mat3* m, const Vec3* r);

static inline void MulVec(Vec3* out, const Vec3* in, const Mat3* m) {
    if (out == in) {
        f32 x = in->x * m->m[0][0] + in->y * m->m[1][0] + in->z * m->m[2][0];
        f32 y = in->x * m->m[0][1] + in->y * m->m[1][1] + in->z * m->m[2][1];
        f32 z = in->x * m->m[0][2] + in->y * m->m[1][2] + in->z * m->m[2][2];
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = in->x * m->m[0][0] + in->y * m->m[1][0] + in->z * m->m[2][0];
        out->y = in->x * m->m[0][1] + in->y * m->m[1][1] + in->z * m->m[2][1];
        out->z = in->x * m->m[0][2] + in->y * m->m[1][2] + in->z * m->m[2][2];
    }
}

void RacingOverlay::DrawNeedle(f32 x, f32 y, f32 len, f32 width, f32 angle) {
    Vec3 pos;
    Mat3 m;
    Vec3 pts[4];
    Vec3 rot;
    int i;

    rot.x = 0.0f;
    rot.y = 0.0f;
    rot.z = angle;
    Mat3FromEuler(&m, &rot);
    pos.x = x;
    pos.y = y;
    pos.z = 0.0f;
    for (i = 0; i < 4; i++) {
        pts[i].y = (i % 2) ? width / 2 : -width / 2;
        pts[i].x = i < 2 ? len - 14.0f : -14.0f;
        pts[i].z = 0.0f;
        MulVec(&pts[i], &pts[i], &m);
        pts[i].x += pos.x;
        pts[i].y += pos.y;
        pts[i].z += pos.z;
    }
    fn_802C8BD0(zoneTex);
    fn_801E4F20(4);
    for (i = 0; i < 4; i++) {
        int back = i > 1;
        f32 u = (i % 2) ? 0.91015625f : 1.0f;
        f32 v = i < 2 ? 0.609375f : 1.0f;
        fn_801E4F3C(-1, back, pts[i].x, pts[i].y, pts[i].z, u, v);
    }
    fn_801E4FF4();
    fn_802C8D74();
}

void Mat3RotY(Mat3* m, f32 a);
void Mat3RotX(Mat3* m, f32 a);
void Mat3RotZ(Mat3* m, f32 a);
void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);

void Mat3FromEuler(Mat3* m, const Vec3* r) {
    f32 x = r->x;
    f32 y = r->y;
    f32 z = r->z;
    Mat3 t;
    Mat3RotY(m, y);
    Mat3RotX(&t, x);
    Mat3Mul(m, &t, m);
    Mat3RotZ(&t, z);
    Mat3Mul(m, &t, m);
}

void Mat3RotZ(Mat3* m, f32 a) {
    f32 c = cos(a);
    f32 s = sin(a);
    m->m[0][0] = c;
    m->m[0][1] = s;
    m->m[0][2] = 0.0f;
    m->m[1][0] = -s;
    m->m[1][1] = c;
    m->m[1][2] = 0.0f;
    m->m[2][0] = 0.0f;
    m->m[2][1] = 0.0f;
    m->m[2][2] = 1.0f;
}

static inline f32 PairProducts(f32 first, f32 second) { return first + second; }

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b) {
    if (out == a || out == b) {
        f32 m00 = Dot3(b->m[0][0], a->m[0][0], b->m[1][0], a->m[0][1], b->m[2][0], a->m[0][2]);
        f32 m10 = Dot3(b->m[0][0], a->m[1][0], b->m[1][0], a->m[1][1], b->m[2][0], a->m[1][2]);
        f32 m20 = Dot3(b->m[0][0], a->m[2][0], b->m[1][0], a->m[2][1], b->m[2][0], a->m[2][2]);
        f32 m01 = Dot3(b->m[0][1], a->m[0][0], b->m[1][1], a->m[0][1], b->m[2][1], a->m[0][2]);
        f32 m11 = Dot3(b->m[0][1], a->m[1][0], b->m[1][1], a->m[1][1], b->m[2][1], a->m[1][2]);
        f32 m21 = Dot3(b->m[0][1], a->m[2][0], b->m[1][1], a->m[2][1], b->m[2][1], a->m[2][2]);
        f32 m02 = Dot3(b->m[0][2], a->m[0][0], b->m[1][2], a->m[0][1], b->m[2][2], a->m[0][2]);
        f32 m12 = Dot3(b->m[0][2], a->m[1][0], b->m[1][2], a->m[1][1], b->m[2][2], a->m[1][2]);
        f32 m22 = Dot3(b->m[0][2], a->m[2][0], b->m[1][2], a->m[2][1], b->m[2][2], a->m[2][2]);
        out->m[0][0] = m00;
        out->m[1][0] = m10;
        out->m[2][0] = m20;
        out->m[0][1] = m01;
        out->m[1][1] = m11;
        out->m[2][1] = m21;
        out->m[0][2] = m02;
        out->m[1][2] = m12;
        out->m[2][2] = m22;
    } else {
        out->m[0][0] = Dot3(b->m[0][0], a->m[0][0], b->m[1][0], a->m[0][1], b->m[2][0], a->m[0][2]);
        out->m[1][0] = Dot3(b->m[0][0], a->m[1][0], b->m[1][0], a->m[1][1], b->m[2][0], a->m[1][2]);
        out->m[2][0] = Dot3(b->m[0][0], a->m[2][0], b->m[1][0], a->m[2][1], b->m[2][0], a->m[2][2]);
        out->m[0][1] = Dot3(b->m[0][1], a->m[0][0], b->m[1][1], a->m[0][1], b->m[2][1], a->m[0][2]);
        out->m[1][1] = Dot3(b->m[0][1], a->m[1][0], b->m[1][1], a->m[1][1], b->m[2][1], a->m[1][2]);
        out->m[2][1] = Dot3(b->m[0][1], a->m[2][0], b->m[1][1], a->m[2][1], b->m[2][1], a->m[2][2]);
        out->m[0][2] = Dot3(b->m[0][2], a->m[0][0], b->m[1][2], a->m[0][1], b->m[2][2], a->m[0][2]);
        out->m[1][2] = Dot3(b->m[0][2], a->m[1][0], b->m[1][2], a->m[1][1], b->m[2][2], a->m[1][2]);
        out->m[2][2] = Dot3(b->m[0][2], a->m[2][0], b->m[1][2], a->m[2][1], b->m[2][2], a->m[2][2]);
    }
}

void Mat3RotX(Mat3* m, f32 a) {
    f32 c = cos(a);
    f32 s = sin(a);
    m->m[0][0] = 1.0f;
    m->m[0][1] = 0.0f;
    m->m[0][2] = 0.0f;
    m->m[1][0] = 0.0f;
    m->m[1][1] = c;
    m->m[1][2] = s;
    m->m[2][0] = 0.0f;
    m->m[2][1] = -s;
    m->m[2][2] = c;
}

void Mat3RotY(Mat3* m, f32 a) {
    f32 c = cos(a);
    f32 s = sin(a);
    m->m[0][0] = c;
    m->m[0][1] = 0.0f;
    m->m[0][2] = -s;
    m->m[1][0] = 0.0f;
    m->m[1][1] = 1.0f;
    m->m[1][2] = 0.0f;
    m->m[2][0] = s;
    m->m[2][1] = 0.0f;
    m->m[2][2] = c;
}

static inline int Round(f32 x) { return x < 0.0f ? x - 0.5f : 0.5f + x; }

void RacingOverlay::DrawHeat(void) {
    f32 heat;
    f32 start = 3.9269907f;
    f32 end = 0.7853982f;
    heat = lbl_8055D350->truck->heat;
    int on = (f32)fmod(lbl_8055D350->time, 1.0) > 0.5f;
    int blink = 0;
    LayoutBox* box;
    LayoutItem* label;

    if (heat > 0.66f && on) {
        blink = 1;
    }
    fn_801478E0(layout, "temp_icon", zoneArt, 3);
    box = layout->FindBox("temp_pie");
    if (box) {
        if (blink || lbl_8055D350->truck->overheat > 0.0f) {
            fn_80147F30(box->pos.x, box->pos.y, box->w, box->h, box->color, Round(80.0f * heat), zoneTex,
                        start, (end - start) / 80.0f, 0.5f, 0.0f, 1.0859375f, 0.5859375f);
        } else {
            fn_80147F30(box->pos.x, box->pos.y, box->w, box->h, box->color, Round(80.0f * heat), zoneTex,
                        start, (end - start) / 80.0f, 0.0f, 0.0f, 0.5859375f, 0.5859375f);
        }
    }
    fn_801478E0(layout, "temp_gauge", zoneArt, 0);
    box = layout->FindBox("temp_needle");
    if (box) {
        f32 y;
        f32 x;
        f32 angle = start + heat * (end - start);
        fn_80148134(&x, &y, box->pos.x, box->pos.y, box->unkC);
        DrawNeedle(x, y, box->w, box->h, angle);
    }
    label = layout->Find("temp_gauge_label");
    if (label) {
        LayoutItem item;
        item = *label;
        if (blink) {
            item.color = 0x80FFFF00;
        }
        fn_801476C4(&item);
    }
}

int RacingOverlay::DrawTryAgain(void) {
    int on = (f32)fmod(lbl_8055D350->time, 1.0) > 0.5f;
    if (on && lbl_8055E520) {
        fn_80011E04(0);
        fn_80010FDC("Please try again", 12);
        return 1;
    }
    return 0;
}
