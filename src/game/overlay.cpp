// Race flow screens (0x800053CC-0x80007D94): the per-player "overlay" state machine that picks
// which screen each viewport shows (intro, pre-race, racing, closure, post-race, results...),
// creates and destroys those screens, and drives the per-frame draw and key routing. Class names
// come from the screens' GetName strings; the file and function names are guesses.
//
// CodeWarrior emits an inline function right after the first function that references it (in
// reverse order of first reference), and defers the rest to the end of the unit; the order of the
// definitions below follows from that.
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "game/common.h"
#include "macros.h"

void* operator new(size_t size, const char* tag);

// One element of a layout set (0x20 bytes).
struct LayoutPos {
    f32 x;
    f32 y;
};

struct LayoutItem {
    u32 unk0;       // offset 0x0
    LayoutPos pos;  // offset 0x4
    u32 unkC;   // offset 0xC
    f32 scale;  // offset 0x10
    u32 unk14;  // offset 0x14
    u32 color;  // offset 0x18
    u32 unk1C;  // offset 0x1C
};

// A layout art element (0x1C bytes).
struct LayoutArtItem {
    u32 unk0;       // offset 0x0
    LayoutPos pos;  // offset 0x4
    u32 unkC;       // offset 0xC
    f32 w;      // offset 0x10
    f32 h;      // offset 0x14
    u32 color;  // offset 0x18
};

class LayoutArt;

// Its vtable (0x8032D260) lives in the layout unit; only the inline dtor is emitted here.
class Layout {
public:
    virtual ~Layout(void) {}
    virtual LayoutItem* Find(const char* name) = 0;            // 0xC
    virtual LayoutArt* FindArt(const char* name) = 0;          // 0x10
    virtual LayoutArtItem* FindArtItem(const char* name) = 0;  // 0x14
};

struct Event {
    u8 pad0[0x20];
    ValueNode* values;  // offset 0x20
};

struct EventInfo {
    u32 unk0;  // offset 0x0
    u32 type;  // offset 0x4
};

struct RacerState {
    u8 pad0[0x30];
    s32 state;  // offset 0x30
    u8 done;    // offset 0x34
};

struct RacerUnk8 {
    u8 pad0[0x2C0];
    u32 flags;  // offset 0x2C0
};

struct RacerUnkC {
    u8 pad0[0x28];
    u8 impaired;  // offset 0x28
};

struct Racer {
    u8 pad0[0x8];
    RacerUnk8* unk8;  // offset 0x8
    RacerUnkC unkC;   // offset 0xC
};

struct ResultRacer {
    u8 pad0[0xC];
    u32 flags;  // offset 0xC
    u8 pad10[0x14 - 0x10];
    s32 place;  // offset 0x14
    f32 time;   // offset 0x18
    u8 pad1C[0x48 - 0x1C];
    char name[1];  // offset 0x48
};

// The race UI / game controller returned by fn_801B83A0 (its vtable lives elsewhere; these
// defaults are inline in its header, so this unit emits the ones it calls).
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
    virtual void vf58(void);
    virtual int vf5C(int player) { return 0; }
    virtual void vf60(int player) {}
    virtual int vf64(void) { return 0; }
    virtual int vf68(void) { return 1; }
    virtual void vf6C(void);
    virtual void vf70(void);
    virtual void vf74(void);
    virtual void vf78(void);
    virtual void vf7C(void);
    virtual void vf80(void);
    virtual void vf84(void);
    virtual void vf88(void);
    virtual void vf8C(void);
    virtual void vf90(void);
    virtual void vf94(void);
    virtual void vf98(void);
    virtual void vf9C(void);
    virtual void vfA0(void);
    virtual void vfA4(void);
    virtual void vfA8(void);
    virtual void vfAC(void);
    virtual void vfB0(void);
    virtual void vfB4(void);
    virtual void vfB8(void);
    virtual void vfBC(void);
    virtual void vfC0(void);
    virtual void vfC4(void);
    virtual void vfC8(void);
    virtual void vfCC(void);
    virtual void vfD0(void);
    virtual void vfD4(void);
    virtual void vfD8(void);
    virtual void vfDC(void);
    virtual void vfE0(void);
    virtual void vfE4(void);
    virtual void vfE8(void);
    virtual void vfEC(void);
    virtual void vfF0(void);
    virtual void vfF4(void);
    virtual void vfF8(void) {}
    virtual void vfFC(void) {}
};

struct Viewport {
    u8 data[0x10];
};

struct Camera {
    u8 data[0x30];
};

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
    virtual void Update(void);
    u8 pad4[0x54 - 0x4];
};

struct RaceInfo {
    u8 split;           // offset 0x0
    u8 active;          // offset 0x1
    s32 mode;           // offset 0x4
    EventInfo* event;   // offset 0x8
    void* info;         // offset 0xC
    f32 time;           // offset 0x10
    s32 numPlayers;     // offset 0x14
    s32 numViews;       // offset 0x18
    s32 player;         // offset 0x1C
    u32 flags;          // offset 0x20
    s32 unk24;          // offset 0x24
    Racer* racer;       // offset 0x28
    RacerUnk8* unk2C;   // offset 0x2C
    RacerUnkC* unk30;   // offset 0x30
};

struct FrameTimes {
    f32 values[128];  // offset 0x0
    s32 unk200;       // offset 0x200
    s32 unk204;       // offset 0x204
    s32 unk208;       // offset 0x208
};

struct RetryFlags {
    u8 restart;  // offset 0x0
    u8 active;   // offset 0x1
    u8 retry;    // offset 0x2
};

extern "C" {
int fn_801298E8(f32* out, int unk);
void fn_80010B80(f32 x, f32 y, f32 w, f32 h, u32 color);
int fn_801769B8(void);
int fn_801EA994(void);
int fn_802E2C20(void);
EventInfo* fn_801E87A8(void);
int fn_801E86F8(int player);
RacerState* fn_801EA9CC(int index);
int fn_801B8570(int index);
int fn_801E866C(int player);
Racer* fn_801EA9A0(int index);
u32 fn_801B84D8(int player);
extern u8 lbl_8055F024;
void fn_80230DA0(void);
int fn_801769A8(void);
int fn_801E85A4(void);
void fn_80010EC8(void);
extern s32 lbl_8055CFF0;
extern s32 lbl_8055CFF4;
void fn_801E88B4(int* x, int* y, int* w, int* h, int player);
void fn_802C86F0(Viewport* vp, int x, int y, int w, int h);
int fn_801E89EC(void);
void fn_8000D75C(void);
void fn_8001123C(int player);
void fn_80011A2C(void);
void fn_802C8750(Viewport* vp, int unk);
int* fn_801769CC(void);
void fn_800105A0(const char* text, int unk);
int fn_801B8914(void);
void fn_80176780(int a, int b);
int fn_801B8900(void);
int fn_8023512C(const char* name);
void fn_80235380(int art);
Layout* fn_80145E70(const char* name);
void fn_801769E0(void);
void fn_8017691C(void);
GameUI* fn_801B83A0(void);
void fn_8001071C(const char* text, int player);
int fn_801B885C(int player);
void fn_8000FBE4(void);
void fn_801C6B70(void);
void fn_80011A5C(int numPlayers);
void fn_801B84A4(int player);
int fn_801B87A0(int player, u16 key);
int fn_80009C84(void);
void fn_801CB844(void);
int fn_801B8874(void);
void fn_800121A4(void);
void fn_8027AA48(void);
void fn_80230E90(int unk);
void fn_80012158(void);
int fn_801CB880(void);
void fn_80011E70(void);
void fn_80011E90(void);
void fn_800109E4(f32 x, f32 y, f32 w, f32 h, u32 color);
void fn_8014784C(Layout* layout, const char* name, const char* set, int index);
void fn_80230F78(u8* alpha, f32* scale, u32* color, int unk);
void fn_80147940(Layout* layout, LayoutArtItem* item, const char* set, int index);
void fn_801476C4(LayoutItem* item);
void fn_8001DFCC(void);
f32 fn_80011E34(void);
f32 fn_80011E54(void);
ResultRacer* fn_801EA9B0(int index);
void fn_80010E94(int art, int unk, f32 x, f32 y, f32 w, f32 h);
void fn_8000FF88(u32 color, int flags, const char* fmt, f32 scale, f32 x, f32 y, ...);
int fn_801345D8(char* buf, int size, const char* fmt, ...);
const char* fn_800122B4(int place, char* buf);
void fn_80010074(u32 color, int flags, const char* fmt, f32 scale, f32 x, f32 y, ...);
void fn_801A7F8C(void);
void fn_8000F0E4(const char* name);
void fn_8000F884(void);
void fn_80011F64(void);
int fn_80137C74(void);
void* fn_802090D4(const char* name);
void fn_801331C4(int unk, const char* name, f32* value);
void fn_8001DB54(void);
void fn_8001DD6C(void);
void fn_80209110(void** p);
void fn_8000FB44(void);
void fn_800120BC(void);
void fn_8000F158(void);
void fn_80209190(void* p, f32 value);
void fn_8001DEA8(void);
void fn_80011A50(void);
void* fn_801E7E3C(void);
f32 fn_801E7E18(void);
void fn_80230B5C(Camera* cam);
void fn_802C8638(Camera* cam);
void fn_802C974C(f32 a, f32 b, f32 c);
void fn_80161C18(void);
int fn_801DF110(void);
int fn_80161A70(void);
int fn_8023D6FC(void);
void fn_802C9904(void);
int fn_801DF238(int key);
int fn_802E4A10(void);
int fn_80137C98(int unk);
void fn_80276DC8(int player);
void fn_8000C630(int a, int b);
f32 fn_80011E44(void);
f32 fn_80011E60(void);
void fn_8000FBE8(f32 x, f32 y, f32 scale);
void fn_801B84C0(int player);
int fn_801B8848(void);
int fn_801B880C(void);
int fn_801CB8FC(void);
}
int OptPaTest(void);
Event* GetEvent(int id);
void LoadTipsSequence(void);
void LoadingStop2(void);
int LoadingDraw(void);
int LoadingHandleKey(int key);
void LoadingFadeOut(void);


extern RaceInfo* lbl_8055D350;
extern RetryFlags lbl_8055D340;
extern s32 lbl_8055D348;

static inline int IsFreeRun(void) { return fn_801769B8() == 0; }

static inline int GetTime(void) { return fn_80009C84() ? lbl_8055D348 : fn_802E2C20(); }

// The closure screen's base: adds the "done" query and the continue prompt. Neither is defined
// anywhere, so the original link left their vtable slots null.
class EndOverlay : public GameOverlay {
public:
    EndOverlay(void) {}
    virtual int IsDone(void) = 0;         // 0x38
    virtual void DrawContinue(void) = 0;  // 0x3C
};

class ClosureOverlay : public EndOverlay {
public:
    ClosureOverlay(void) { Reset(); }
    virtual void Enter(void);
    virtual void Reset(void) {
        done = 0;
        times[0] = -1.0f;
        times[1] = -1.0f;
        times[2] = -1.0f;
        times[3] = -1.0f;
        endTime = -1.0f;
    }
    virtual void Draw(void);
    virtual void Update(void);
    virtual void DrawPlayer(int player);
    virtual int HandleKey(int player, u32 type, int key);
    virtual void DrawContinue(void);
    virtual int IsDone(void);
    virtual const char* GetName(void);

    int Finished(void) {
        f32 t = times[lbl_8055D350->player];
        int finished = 0;
        if (t >= 0.0f && lbl_8055D350->time - t > 0.0f) {
            finished = 1;
        }
        return finished;
    }
    void ShowMessage(int player) {
        const char* msg = 0;
        if (lbl_8055D350->unk30->impaired) {
            msg = "Your car is impaired";
        }
        if (msg) {
            fn_8001071C(msg, player);
        }
    }

    f32 times[4];  // offset 0x4
    bool done;     // offset 0x14
    f32 endTime;   // offset 0x18
};

class ArenaIntroOverlay : public GameOverlay {
public:
    ArenaIntroOverlay(void) { Reset(); }
    virtual ~ArenaIntroOverlay(void);
    virtual void Update(void);
    virtual int IsFullScreen(void);
    virtual const char* GetName(void);
};

class IntroOverlay : public GameOverlay {
public:
    virtual void Update(void);
    virtual int HandleKey(int player, u32 type, int key);
    virtual const char* GetName(void);
};

struct MenuItem {
    s32 index;           // offset 0x0
    f32 x;               // offset 0x4
    const char* text;    // offset 0x8
    const char* button;  // offset 0xC
};

class ResultsOverlay : public GameOverlay {
public:
    ResultsOverlay(void) {
        art = fn_8023512C("uielems.art");
        popup = fn_8023512C("popup1.art");
        layout = fn_80145E70("resmenu_lyt.val");
        Reset();
    }
    virtual ~ResultsOverlay(void);
    virtual void Enter(void);
    virtual void Reset(void) {
        lbl_8055D340.active = 0;
        shown = 0;
    }
    virtual void Update(void);
    virtual int IsFullScreen(void);
    virtual void Draw(void);
    virtual int HandleKey(int player, u32 type, int key);
    virtual const char* GetName(void);

    void SetState(int s) {
        if (shown != s) {
            shown = s;
            start = GetTime();
        }
    }
    void MoveSelection(int dir);
    int HandleMenuKey(int player, u32 type, int key);
    void DrawMenu(void);
    void DrawTimes(void);

    s32 state;       // offset 0x4
    s32 shown;       // offset 0x8
    s32 start;       // offset 0xC
    s32 selection;   // offset 0x10
    s32 art;         // offset 0x14
    s32 popup;       // offset 0x18
    Layout* layout;  // offset 0x1C
};

class ReplayOverlay : public GameOverlay {
public:
    virtual void Update(void);
};

class ArenaOverlay : public GameOverlay {
public:
    virtual void Update(void);
    virtual int HandleKey(int player, u32 type, int key);
    virtual const char* GetName(void);
};

class KioskDeadDriverOverlay : public GameOverlay {
public:
    virtual void Update(void);
    virtual void Draw(void);
    virtual const char* GetName(void);
};

// The pre-race screen's base, with the same two (undefined) extra slots.
class WaitOverlay : public GameOverlay {
public:
    WaitOverlay(void) {}
    virtual int IsDone(void) = 0;         // 0x38
    virtual void DrawContinue(void) = 0;  // 0x3C
};

class InterludeOverlay : public GameOverlay {
public:
    virtual void Update(void);
    virtual const char* GetName(void);
};

class ArenaPreRaceOverlay : public WaitOverlay {
public:
    ArenaPreRaceOverlay(void) {
        unk4 = 0;
        unk5 = 0;
        fn_80176780(0x1000, 0);
    }
    virtual void Reset(void);
    virtual void Draw(void);
    virtual int IsDone(void);
    virtual void DrawContinue(void);
    virtual void Update(void);
    virtual int HandleKey(int player, u32 type, int key);
    virtual const char* GetName(void);

    bool unk4;  // offset 0x4
    bool unk5;  // offset 0x5
};

class ArenaPostRaceOverlay : public GameOverlay {
public:
    ArenaPostRaceOverlay(void) { unk4 = 0; }
    virtual void Reset(void);
    virtual int IsDone(void);
    virtual void Draw(void);
    virtual void Update(void);
    virtual int HandleKey(int player, u32 type, int key);
    virtual const char* GetName(void);

    u8 unk4;  // offset 0x4
};

struct PlayerOverlays {
    GameOverlay* next;    // offset 0x0
    GameOverlay* cur;     // offset 0x4
    GameOverlay* racing;  // offset 0x8
    GameOverlay* active;  // offset 0xC
};

struct Overlays {
    GameOverlay* interlude;          // offset 0x0
    WaitOverlay* preRace;            // offset 0x4
    WaitOverlay* postRace;           // offset 0x8
    GameOverlay* intro;              // offset 0xC
    PlayerOverlays players[4];       // offset 0x10
    GameOverlay* arena;              // offset 0x50
    GameOverlay* replay;             // offset 0x54
    EndOverlay* closure;             // offset 0x58
    GameOverlay* results;            // offset 0x5C
    GameOverlay* unk60;              // offset 0x60
};

const char* lbl_80558928 = "the  A  Button";
static const char* GetClassicButtonName(void) { return "the  Square  button"; }
f32 lbl_8055892C = 1.0f;

ModuleTag lbl_8055D330;
UnkStaticFFFF lbl_8055D338;
UnkStatic7F lbl_8055D33C;
RetryFlags lbl_8055D340;
GameOverlay* lbl_8055D344;
s32 lbl_8055D348;
f32 lbl_8055D34C;

FrameTimes lbl_8034EC78;
RaceInfo lbl_8034EE84;

Overlays lbl_8034EEB8;
static inline RaceInfo* GetRaceInfo(void) { return &lbl_8034EE84; }
RaceInfo* lbl_8055D350 = GetRaceInfo();
void* lbl_8055D354;

static void fn_800053CC(void) {
    f32 v[28];
    f32 len;
    f32 r;
    f32 x;
    f32 y;
    if (fn_801298E8(v, 0)) {
        r = 10.0f;
        x = 300.0f * v[0];
        y = 200.0f * v[1];
        len = sqrt(v[0] * v[0] + v[1] * v[1]);
        fn_80010B80(x, y, r, r, 0x800000FF);
    }
}

static void fn_8000545C(void) {}

static void fn_80005460(void) {}

void fn_80005464(void) {
    lbl_8034EE84.mode = fn_801769B8();
    lbl_8034EE84.split = 0;
    if (lbl_8034EE84.mode == 2 && fn_801EA994() > 1) {
        lbl_8034EE84.split = 1;
    }
    lbl_8034EE84.active = 1;
    if (lbl_8034EE84.mode == 0) {
        lbl_8034EE84.active = 0;
    }
}

void fn_800054E0(void) {
    int i;
    for (i = 0; i < sizeof(lbl_8034EC78.values) / sizeof(lbl_8034EC78.values[0]); i++) {
        lbl_8034EC78.values[i] = 0.0f;
    }
    lbl_8034EC78.unk200 = 0;
    lbl_8034EC78.unk204 = fn_802E2C20();
    lbl_8034EC78.unk208 = 0;
}

int fn_80005564(void) { return lbl_8034EEB8.arena != 0; }

int fn_80005580(int player) {
    u32 flags = fn_801E87A8()->type;
    int index = fn_801E86F8(player);
    RacerState* r = fn_801EA9CC(index);
    if (!fn_801B8570(index)) {
        return 1;
    }
    if (flags & 0x100) {
        return r->state == 3 || r->done;
    }
    return 0;
}

int fn_8000561C(void) {
    int i;
    for (i = 0; i < lbl_8055D350->numPlayers; i++) {
        if (!fn_80005580(i)) {
            return 0;
        }
    }
    return 1;
}

int fn_80005678(void) { return lbl_8034EEB8.closure->IsDone(); }

int fn_80005694(void) { return fn_801EA9CC(0)->state == 1; }

void fn_800056C8(int player, GameOverlay* overlay) { lbl_8034EEB8.players[player].next = overlay; }

void fn_800056E0(GameOverlay* overlay) {
    int n = lbl_8055D350->numPlayers;
    int i;
    for (i = 0; i < n; i++) {
        fn_800056C8(i, overlay);
    }
}

void fn_80005740(int player) {
    Racer* racer;
    lbl_8034EE84.player = player;
    lbl_8034EE84.unk24 = fn_801E866C(player);
    racer = fn_801EA9A0(lbl_8034EE84.unk24);
    lbl_8034EE84.racer = racer;
    lbl_8034EE84.unk2C = racer->unk8;
    lbl_8034EE84.unk30 = &racer->unkC;
    lbl_8034EE84.flags = fn_801B84D8(player);
    if (fn_80005564()) {
        fn_801B8570(lbl_8034EE84.unk24);
    }
}

void fn_800057BC(void) {
    if (lbl_8055F024) {
        fn_80230DA0();
    }
}

void fn_800057D0(void) {
    int n;
    int i;
    if (fn_801769A8() & 0x4000) {
        fn_800056E0(lbl_8034EEB8.interlude);
    } else if (lbl_8034EEB8.preRace && !lbl_8034EEB8.preRace->IsDone()) {
        fn_800056E0(lbl_8034EEB8.preRace);
    } else if (fn_80005694()) {
        fn_800056E0(lbl_8034EEB8.intro);
    } else if (fn_80005678()) {
        if (lbl_8034EEB8.postRace && !lbl_8034EEB8.postRace->IsDone()) {
            fn_800056E0(lbl_8034EEB8.postRace);
        } else {
            fn_800056E0(lbl_8034EEB8.results);
        }
    } else {
        n = lbl_8055D350->numPlayers;
        for (i = 0; i < n; i++) {
            if (fn_80005580(i)) {
                fn_800056C8(i, lbl_8034EEB8.closure);
            } else {
                fn_800056C8(i, lbl_8034EEB8.players[i].active);
            }
        }
    }
}

void fn_80005930(void) {
    int i;
    int n = fn_801E85A4();
    GameOverlay* overlay2;
    GameOverlay* overlay;
    int j;
    int x;
    int y;
    int w;
    int h;
    Viewport vp;
    Viewport vp2;

    for (i = 0; i < n; i++) {
        fn_80005740(i);
        overlay = lbl_8034EEB8.players[i].next;
        if (lbl_8034EEB8.players[i].cur != overlay) {
            if (lbl_8034EEB8.players[i].cur) {
                lbl_8034EEB8.players[i].cur->Leave();
            }
            overlay->Enter();
            lbl_8034EEB8.players[i].cur = overlay;
        }
        fn_80010EC8();
        if (overlay->IsFullScreen()) {
            x = 0;
            y = 0;
            w = lbl_8055CFF0;
            h = lbl_8055CFF4;
        } else {
            fn_801E88B4(&x, &y, &w, &h, i);
        }
        fn_802C86F0(&vp, x, y, w, h);
        overlay->Draw();
        if (fn_801E89EC() != 3) {
            fn_8000D75C();
            if (!overlay->IsFullScreen() || i == 0) {
                overlay->Update();
            }
        }
        fn_8001123C(lbl_8055D350->player);
        fn_80011A2C();
        fn_802C8750(&vp, -1);
    }
    for (j = 0; j < n; j++) {
        fn_80005740(j);
        overlay2 = lbl_8034EEB8.players[j].next;
        fn_80010EC8();
        fn_802C86F0(&vp2, 0, 0, lbl_8055CFF0, lbl_8055CFF4);
        overlay2->Draw();
        if (fn_801E89EC() != 3) {
            fn_8000D75C();
            overlay2->DrawPlayer(j);
        }
        fn_8001123C(lbl_8055D350->player);
        fn_80011A2C();
        fn_802C8750(&vp2, -1);
    }
}

void fn_80005B6C(void) {}

void fn_80005B70(int unk) {}

void fn_80005B74(void) {
    char buf[32];
    f32 t;
    f32 now;
    if (OptPaTest()) {
        t = GetEvent(*fn_801769CC())->values->GetFloat("pa_sample_request_t");
        now = lbl_8055D350->time;
        if (now > t) {
            if (now - t < 2.0f) {
                fn_800105A0("Please trigger PA", 0);
            }
        } else {
            sprintf(buf, "%.1f seconds til grab", t - now);
            fn_800105A0(buf, 0);
        }
    }
}

// Not referenced in the final game; kept for their strings.
static void DrawWaitingForOthers(void) { fn_800105A0("Other players are still racing", 0); }

static void DrawPressPrompt(char* buf, const char* button, const char* action) {
    sprintf(buf, "Press  %s  to  %s", button, action);
}

static void DrawReplayPrompt(char* buf, const char* button) { sprintf(buf, "Press %s to view replay", button); }

void fn_80005C20(void) {
    int now = fn_802E2C20();
    f32 dt = (now - lbl_8055D348) / 1000.0f;
    lbl_8055D34C = MAX(0.0f, dt);
    lbl_8055D348 = now;
}

void fn_80005C88(void) {
    int arena = (lbl_8055D350->event->type >> 9) & 1;
    int n;
    int i;

    if (arena) {
        lbl_8034EEB8.intro = new ("ArenaIntroOverlay") ArenaIntroOverlay;
    } else {
        lbl_8034EEB8.intro = new ("IntroOverlay") IntroOverlay;
    }
    lbl_8034EEB8.interlude = new ("InterludeOverlay") InterludeOverlay;
    if (fn_801B8914()) {
        lbl_8034EEB8.preRace = new ("PreRaceOverlay") ArenaPreRaceOverlay;
    } else {
        lbl_8034EEB8.preRace = 0;
    }
    lbl_8034EEB8.arena = arena ? new ("ArenaOverlay") ArenaOverlay : 0;
    lbl_8034EEB8.replay = new ("ReplayOverlay") ReplayOverlay;
    n = lbl_8055D350->numPlayers;
    for (i = 0; i < sizeof(lbl_8034EEB8.players) / sizeof(lbl_8034EEB8.players[0]); i++) {
        lbl_8034EEB8.players[i].next = 0;
        lbl_8034EEB8.players[i].cur = 0;
        if (i < n) {
            lbl_8034EEB8.players[i].racing = new ("RacingOverlay") RacingOverlay(i);
        } else {
            lbl_8034EEB8.players[i].racing = 0;
        }
        lbl_8034EEB8.players[i].active = arena ? lbl_8034EEB8.arena : lbl_8034EEB8.players[i].racing;
    }
    lbl_8034EEB8.closure = new ("ClosureOverlay") ClosureOverlay;
    if (fn_801B8900()) {
        lbl_8034EEB8.postRace = (WaitOverlay*)new ("PostRaceOverlay") ArenaPostRaceOverlay;
    } else {
        lbl_8034EEB8.postRace = 0;
    }
    if (lbl_8055D344) {
        lbl_8055D344->Attach();
        lbl_8034EEB8.results = lbl_8055D344;
    } else {
        lbl_8034EEB8.results = new ("ResultsOverlay") ResultsOverlay;
    }
    lbl_8034EEB8.unk60 = 0;
}

void fn_800060E0(bool restart, bool retry) {
    lbl_8055D340.restart = restart;
    lbl_8055D340.retry = retry;
    fn_801769E0();
    fn_8017691C();
}

void ClosureOverlay::Update(void) {
    const char* msg;
    if (!Finished()) {
        if (!fn_80005564()) {
            lbl_8034EEB8.players[lbl_8055D350->player].active->Update();
        }
    } else if (fn_80005564() && fn_801B83A0()->vf64()) {
        done = fn_801B83A0()->vf5C(lbl_8055D350->player) == 0;
    } else {
        done = 1;
        ShowMessage(lbl_8055D350->player);
    }
}

void ClosureOverlay::DrawPlayer(int player) {
    if (Finished() && fn_80005564()) {
        fn_801B83A0()->vf60(player);
    }
}

void ArenaIntroOverlay::Update(void) {
    if (!fn_801B885C(lbl_8055D350->player) && lbl_8055D350->numViews == lbl_8055D350->numPlayers) {
        fn_8000FBE4();
    }
}

void IntroOverlay::Update(void) {
    lbl_8034EE84.flags &= ~0x87;
    lbl_8034EE84.flags &= ~0x600;
    lbl_8034EEB8.players[lbl_8055D350->player].active->Update();
}

int IntroOverlay::HandleKey(int player, u32 type, int key) {
    return lbl_8034EEB8.players[player].active->HandleKey(player, type, key);
}

void InterludeOverlay::Update(void) { fn_801C6B70(); }

void ArenaOverlay::Update(void) {
    int player = lbl_8055D350->player;
    fn_80011A5C(lbl_8055D350->numPlayers);
    lbl_8034EEB8.players[player].racing->Update();
    fn_801B84A4(player);
}

int ArenaOverlay::HandleKey(int player, u32 type, int key) {
    if ((lbl_8055D350->flags & 4) && lbl_8034EEB8.players[player].racing->HandleKey(player, type, key)) {
        return 1;
    }
    if (type == 1000) {
        return fn_801B87A0(player, key);
    }
    return 0;
}

void ResultsOverlay::Update(void) {
    if (state != shown) {
        shown = state;
        start = GetTime();
    }
    fn_80011A2C();
    switch (state) {
    case 3:
        DrawMenu();
        break;
    case 2:
        fn_801CB844();
        break;
    case 1:
        if (!fn_801B8874()) {
            DrawTimes();
        }
        break;
    }
}

void ResultsOverlay::MoveSelection(int dir) {
    selection += dir;
    if (selection < 0) {
        selection = 0;
    }
    if (selection >= 3) {
        selection = 2;
    }
}

int ResultsOverlay::HandleMenuKey(int player, u32 type, int key) {
    int handled = 0;
    int i;
    if (type == 1000) {
        u16 k = key;
        switch (state) {
        case 3:
            if (k == 'X') {
                fn_800060E0(selection == 2, selection == 1);
                fn_800121A4();
                handled = 1;
            } else if (k == 0x126 || k == 0x128) {
                MoveSelection(k == 0x126 ? -1 : 1);
                for (i = 0; i < 3; i++) {
                    if (selection == 2) {
                        fn_8027AA48();
                    }
                }
                fn_80230E90(1);
                fn_80012158();
            }
            break;
        case 2:
            if (k == 'X' && fn_801CB880()) {
                state = 3;
                handled = 1;
            }
            break;
        case 1:
            if (k == 'X') {
                state = 2;
                handled = 1;
            }
            break;
        }
    }
    return handled;
}

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

static inline f32 Clamp01(f32 x) {
    f32 v = MIN(1.0f, x);
    return MAX(v, 0.0f);
}

static inline f32 SlideIn(f32 t, f32 delay) { return Lerp(700.0f, 0.0f, Clamp01((t - delay) / 0.25f)); }

void ResultsOverlay::DrawMenu(void) {
    MenuItem* item;
    u32 i;
    f32 elapsed;
    bool selected;
    int frame;
    LayoutItem text;
    LayoutArtItem button;
    u8 alpha;
    f32 pulse;

    fn_8027AA48();
    fn_80011E70();
    fn_80011E90();
    int layer = 2;
    f32 z = layer;
    fn_800109E4(-428.0f, 241.0f, 855.0f, 482.0f, 0x7F000000);
    if (layout) {
        fn_8014784C(layout, "back_art", "back_artset", 0);
    }
    sin(4.0f * (0.001f * fn_802E2C20()));
    if (layout) {
        elapsed = 0.001f * (GetTime() - start);
        if (elapsed < 0.2f) {
            fn_80230E90(1);
        }
        MenuItem items[3] = {
            {0, 0.0f, "continue_text", "continue_button"},
            {1, 0.0f, "restart_text", "restart_button"},
            {2, 0.0f, "replay_text", "replay_button"},
        };
        items[0].x = SlideIn(elapsed, 0.0f);
        items[1].x = SlideIn(elapsed, 0.05f);
        items[2].x = SlideIn(elapsed, 0.1f);
        for (i = 0, item = items; i < 3; i++, item++) {
            if (item->index == 2) {
                fn_8027AA48();
            }
            text = *layout->Find(item->text);
            text.pos.x += item->x;
            selected = selection == item->index;
            alpha = 0xFF;
            pulse = 0.0f;
            if (selected) {
                fn_80230F78(&alpha, &pulse, &text.color, 1);
            }
            frame = selected ? 1 : 2;
            button = *layout->FindArtItem(item->button);
            button.pos.x += item->x;
            fn_80147940(layout, &button, "menu_artset", frame);
            if (selected) {
                button.pos.x -= 0.5f * button.w * pulse;
                button.pos.y -= 0.5f * button.h * pulse;
                button.w *= 1.0f + pulse;
                button.h *= 1.0f + pulse;
                button.color = (button.color & 0xFF0000) | (alpha << 24) | (button.color & 0xFFFF);
                fn_80147940(layout, &button, "menu_artset", 1);
            }
            fn_801476C4(&text);
        }
        fn_8001DFCC();
    }
}

void ResultsOverlay::DrawTimes(void) {
    f32 x0 = fn_80011E34();
    f32 y0 = fn_80011E54() - 24.0f;
    char place[16];
    f32 x;
    f32 y;
    int i;
    ResultRacer* r;
    char buf[8];
    f32 t;

    for (i = 0; i < fn_801EA994(); i++) {
        r = fn_801EA9B0(i);
        y = y0 - 54.0f * i;
        fn_80010E94(art, 0, x0, y - 21.6f, 200.0f, 27.0f);
        x = x0;
        x += 54.0f;
        if (r->flags & 1) {
            fn_8000FF88(0xFFFFFF00, 0x41, "%d", 1.5f, x, y - 21.6f - 27.0f, r->place);
            x += 85.0f;
        } else if (r->flags & 2) {
            t = r->time;
            if (t < 0.0f) {
                fn_801345D8(buf, sizeof(buf), "--:--");
            } else {
                int sec = t;
                fn_801345D8(buf, sizeof(buf), "%d:%02d", sec, (int)(100.0f * t) - sec * 100);
            }
            fn_8000FF88(0xFFFFFF00, 0x41, "%s", 1.5f, x, y - 21.6f - 27.0f, buf);
            x += 135.0f;
        } else {
            fn_8000FF88(0xFFFFFF00, 0x41, "%s", 1.5f, x, y - 21.6f - 27.0f, fn_800122B4(i + 1, place));
            x += 85.0f;
        }
        fn_8000FF88(0xFFFFFF00, 0x21, "%s", 1.0f, x, y - 21.6f - 27.0f, r->name);
    }
}

void KioskDeadDriverOverlay::Update(void) {
    char buf[0x48];
    if (lbl_8055D350->unk2C->flags & 0x10) {
        sprintf(buf, "Press %s to RESET", lbl_80558928);
        fn_80010074(0xFFFFFFFF, 0x14, "%s", 1.2f, 0.0f, 125.0f, buf);
    }
}

void fn_800070DC(void);

void fn_80006D9C(void) {
    LoadTipsSequence();
    fn_801A7F8C();
    fn_800054E0();
    fn_8000F0E4("uifnt_o.gtx");
    fn_8000F884();
    fn_80011F64();
    lbl_8034EE84.event = fn_801E87A8();
    lbl_8034EE84.numPlayers = fn_80137C74();
    lbl_8055D354 = fn_802090D4("dashboard");
    fn_801331C4(4, "effects_volume", &lbl_8055892C);
    fn_80005464();
    fn_80005C88();
    fn_80011A5C(lbl_8055D350->numPlayers);
    fn_8001DB54();
    fn_801B83A0()->vfF8();
    fn_800070DC();
}

void fn_80006E54(void) {
    int i;
    fn_801B83A0()->vfFC();
    fn_8001DD6C();
    for (i = 0; i < lbl_8055D350->numPlayers; i++) {
        delete lbl_8034EEB8.players[i].racing;
    }
    delete lbl_8034EEB8.intro;
    if (lbl_8034EEB8.interlude) {
        delete lbl_8034EEB8.interlude;
        lbl_8034EEB8.interlude = 0;
    }
    if (lbl_8034EEB8.preRace) {
        delete lbl_8034EEB8.preRace;
        lbl_8034EEB8.preRace = 0;
    }
    if (lbl_8055D344) {
        lbl_8055D344->Detach();
    } else {
        delete lbl_8034EEB8.results;
    }
    delete lbl_8034EEB8.replay;
    if (lbl_8034EEB8.arena) {
        delete lbl_8034EEB8.arena;
    }
    delete lbl_8034EEB8.closure;
    if (lbl_8034EEB8.postRace) {
        delete lbl_8034EEB8.postRace;
        lbl_8034EEB8.postRace = 0;
    }
    if (lbl_8034EEB8.unk60) {
        delete lbl_8034EEB8.unk60;
    }
    memset(&lbl_8034EEB8, 0, sizeof(lbl_8034EEB8));
    fn_80209110(&lbl_8055D354);
    fn_8000FB44();
    fn_800120BC();
    fn_8000F158();
    fn_801A7F8C();
    LoadingStop2();
}

void fn_800070DC(void) {
    int i;
    fn_80209190(lbl_8055D354, 1.0f);
    lbl_8034EEB8.intro->Reset();
    lbl_8034EEB8.results->Reset();
    lbl_8034EEB8.closure->Reset();
    if (lbl_8034EEB8.arena) {
        lbl_8034EEB8.arena->Reset();
    }
    if (lbl_8034EEB8.postRace) {
        lbl_8034EEB8.postRace->Reset();
    }
    if (lbl_8034EEB8.preRace) {
        lbl_8034EEB8.preRace->Reset();
    }
    for (i = 0; i < lbl_8055D350->numPlayers; i++) {
        lbl_8034EEB8.players[i].racing->Reset();
        lbl_8034EEB8.players[i].next = 0;
    }
    fn_8001DEA8();
    fn_80011A50();
    fn_80005464();
}

void fn_80007210(void) {
    fn_800070DC();
    lbl_8055D348 = fn_802E2C20();
}

void fn_80007238(void) {
    Camera cam;
    lbl_8034EE84.info = fn_801E7E3C();
    lbl_8034EE84.time = fn_801E7E18();
    lbl_8034EE84.numViews = fn_801E85A4();
    fn_80230B5C(&cam);
    fn_802C8638(&cam);
    f32 wide = 1.4396f;
    f32 narrow = 0.82625633f;
    fn_802C974C(1.0f, 1000.0f, lbl_8034EE84.numViews > 2 ? narrow : wide);
    fn_800057D0();
    fn_80161C18();
    if (!LoadingDraw() && !fn_801DF110() && !fn_80161A70()) {
        fn_80005930();
    }
    fn_80005B6C();
    fn_80005B70(0);
    fn_8000545C();
    fn_80005460();
    if (fn_8023D6FC() == 3) {
        fn_800053CC();
    }
    fn_80005B74();
    fn_80011A2C();
    fn_800057BC();
    LoadingFadeOut();
}

void fn_80007318(void) {
    Camera cam;
    fn_80005C20();
    fn_80230B5C(&cam);
    fn_802C8638(&cam);
    fn_802C974C(1.0f, 1000.0f, 1.4398966f);
    lbl_8034EE84.player = 0;
    lbl_8034EE84.unk24 = fn_801E866C(0);
    lbl_8034EE84.racer = fn_801EA9A0(lbl_8055D350->unk24);
    lbl_8034EE84.unk2C = lbl_8055D350->racer->unk8;
    lbl_8034EE84.unk30 = &lbl_8055D350->racer->unkC;
    lbl_8034EE84.time = fn_801E7E18();
    lbl_8034EEB8.replay->Update();
    fn_800057BC();
    fn_80011A2C();
    fn_802C9904();
}

void fn_800073D8(int selection) {
    ResultsOverlay* results;
    fn_80005C20();
    results = (ResultsOverlay*)lbl_8034EEB8.results;
    results->SetState(3);
    switch (selection) {
    case 0:
        results->selection = 0;
        break;
    case 1:
        results->selection = 1;
        break;
    case 2:
        results->selection = 2;
        break;
    }
    results->DrawMenu();
    fn_80011A2C();
}

void fn_80007498(void) { fn_80011A50(); }

int fn_8000749C(void) { return lbl_8055D340.active && lbl_8055D340.restart; }

int fn_800074C4(void) { return lbl_8055D340.active && lbl_8055D340.retry; }

int fn_80007554(int player, u32 type, int key);

int fn_800074EC(int key) {
    if (LoadingHandleKey(key)) {
        return 1;
    }
    if (fn_801DF238(key)) {
        return 1;
    }
    return fn_80007554(fn_80137C98(fn_802E4A10()), 1000, key);
}

int fn_80007554(int player, u32 type, int key) {
    GameOverlay* overlay = lbl_8034EEB8.players[player].next;
    if (overlay) {
        return overlay->HandleKey(player, type, key);
    }
    return 0;
}

int fn_800075A0(int player) {
    GameOverlay* overlay = lbl_8034EEB8.players[player].next;
    return overlay != lbl_8034EEB8.closure && overlay != lbl_8034EEB8.postRace && overlay != lbl_8034EEB8.results;
}

inline void ClosureOverlay::Enter(void) {
    times[lbl_8055D350->player] = lbl_8055D350->time;
    if (fn_8000561C()) {
        endTime = lbl_8055D350->time;
    }
    fn_80276DC8(lbl_8055D350->player);
}

inline void ClosureOverlay::Draw(void) {
    if (!(lbl_8055D350->unk2C->flags & 0x20)) {
        fn_8000C630(0x20, lbl_8055D350->player);
    }
}

inline int ClosureOverlay::HandleKey(int player, u32 type, int key) {
    if (player != 0) {
        return 0;
    }
    if (type == 1000) {
        return fn_801B87A0(player, key);
    }
    return 0;
}

inline void ClosureOverlay::DrawContinue(void) {
    f32 x;
    f32 y;
    if (fn_801B83A0()->vf64() && fn_801B83A0()->vf68() && !IsDone()) {
        x = fn_80011E44() - 132.0f;
        y = 95.0f + fn_80011E60();
        fn_8000FBE8(x - 35.0f, y, 1.0f);
        fn_8000FF88(0xFFFFFFFF, 0x11, "Continue", 0.6f, x, y);
    }
}

inline int ClosureOverlay::IsDone(void) { return done; }

inline const char* ClosureOverlay::GetName(void) { return "ClosureOverlay"; }

inline ArenaIntroOverlay::~ArenaIntroOverlay(void) {}

inline int ArenaIntroOverlay::IsFullScreen(void) { return 1; }

inline const char* ArenaIntroOverlay::GetName(void) { return "ArenaIntroOverlay"; }

inline const char* IntroOverlay::GetName(void) { return "IntroOverlay"; }

inline ResultsOverlay::~ResultsOverlay(void) {
    if (layout) {
        delete layout;
    }
    fn_80235380(popup);
    fn_80235380(art);
}

inline void ResultsOverlay::Enter(void) {
    u32 id;
    Reset();
    id = fn_801E87A8()->type;
    if (lbl_8055D350->numPlayers > 1) {
        state = 3;
    } else if (id == 0x180302 || id == 0x180303 || id == 0x180304 || id == 0x180201) {
        state = 2;
    } else {
        state = 1;
    }
    shown = 0;
    selection = 0;
    if (!fn_801CB8FC()) {
        selection = 1;
    }
    lbl_8055D340.active = 1;
}

inline int ResultsOverlay::IsFullScreen(void) { return 1; }

inline void ResultsOverlay::Draw(void) { fn_8000C630(0x20, -1); }

inline int ResultsOverlay::HandleKey(int player, u32 type, int key) {
    if (player != 0) {
        return 0;
    }
    return HandleMenuKey(player, type, key);
}

inline const char* ResultsOverlay::GetName(void) { return "ResultsOverlay"; }

inline void ReplayOverlay::Update(void) {
    if (lbl_8055D350->event->type & 0x200) {
        fn_801B84C0(lbl_8055D350->player);
    }
}

inline const char* ArenaOverlay::GetName(void) { return "ArenaOverlay"; }

inline void KioskDeadDriverOverlay::Draw(void) {}

inline const char* KioskDeadDriverOverlay::GetName(void) { return "KioskDeadDriverOverlay"; }

inline const char* InterludeOverlay::GetName(void) { return "InterludeOverlay"; }

inline void ArenaPreRaceOverlay::Reset(void) {
    unk4 = !IsFreeRun();
    unk5 = 0;
}

inline void ArenaPreRaceOverlay::Draw(void) { fn_8000C630(0xB, -1); }

inline int ArenaPreRaceOverlay::IsDone(void) { return unk4; }

inline void ArenaPreRaceOverlay::Update(void) {
    if (!fn_801B8848()) {
        unk5 = 1;
    }
}

inline int ArenaPreRaceOverlay::HandleKey(int player, u32 type, int key) {
    if (type == 1000) {
        return fn_801B87A0(player, key);
    }
    return 0;
}

inline const char* ArenaPreRaceOverlay::GetName(void) { return "ArenaPreRaceOverlay"; }

inline void ArenaPostRaceOverlay::Reset(void) { unk4 = 0; }

inline int ArenaPostRaceOverlay::IsDone(void) { return unk4; }

inline void ArenaPostRaceOverlay::Draw(void) { fn_8000C630(0xB, -1); }

inline void ArenaPostRaceOverlay::Update(void) {
    if (!fn_801B880C()) {
        unk4 = 1;
    }
}

inline int ArenaPostRaceOverlay::HandleKey(int player, u32 type, int key) {
    if (type == 1000) {
        return fn_801B87A0(player, key);
    }
    return 0;
}

inline const char* ArenaPostRaceOverlay::GetName(void) { return "ArenaPostRaceOverlay"; }
