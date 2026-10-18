// Main game loop (0x8000AC98-0x8000C7CC): the table of game modes (draw / key / update / enter /
// leave hooks), mode switching, key routing, per-viewport racing cameras, the idle timeout and the
// race run loop with its post-race bookkeeping. "gameloop" strings name the unit; function names
// are guesses.
#include <string.h>
#include "game/common.h"

struct GameMode {
    void (*draw)(void);         // offset 0x0
    int (*handleKey)(int key);  // offset 0x4
    void (*update)(void);       // offset 0x8
    void (*enter)(void);        // offset 0xC
    void (*leave)(void);        // offset 0x10
};

struct ViewCam {
    s32 cam;       // offset 0x0
    s32 next;  // offset 0x4
};

struct Racer {
    u8 pad0[0x30];
    s32 state;  // offset 0x30
    u8 pad34[0xAC - 0x34];
    s32 unkAC;  // offset 0xAC
};

struct RaceResultEntry {
    u8 pad0[0x10];
    s32 value;  // offset 0x10
};

struct RaceResult {
    s32 count;     // offset 0x0
    s32 unk4;      // offset 0x4
    s32 races;     // offset 0x8
    u8 unkC;       // offset 0xC
    u8 finished;   // offset 0xD
    u8 aborted;    // offset 0xE
    u8 padF[0xA4 - 0xF];
};

struct PadState {
    u8 pad0[0x20];
    u8 buttons[8];  // offset 0x20
    u8 pad28[0x68 - 0x28];
};

struct Camera {
    u8 data[0x30];
};

struct RacerSlot {
    s32 type;              // offset 0x0
    char driverName[64];   // offset 0x4
    char vehicleName[32];  // offset 0x44
    struct {
        f32 v;
    } unk64;      // offset 0x64
    s32 driver;   // offset 0x68
    s32 vehicle;  // offset 0x6C
    u8 flags;     // offset 0x70
    s32 unk74;    // offset 0x74
};

struct RacerList {
    RacerSlot slots[8];  // offset 0x0
    s32 count;           // offset 0x3C0
};

struct RaceSetup {
    char track[0x1C];  // offset 0x0
    RacerList racers __attribute__((aligned(8)));  // offset 0x20
    s32 event;         // offset 0x3E4
    u32 flags;         // offset 0x3E8
    s32 unk3EC;        // offset 0x3EC
    s32 unk3F0;        // offset 0x3F0
    s32 unk3F4;        // offset 0x3F4
};

class GameListener {
public:
    virtual void Begin(void);                   // 0x8
    virtual void End(RaceResult* result);       // 0xC
    virtual void Finish(void);                  // 0x10
};

struct Vec3 {
    f32 x;
    f32 y;
    f32 z;
};

extern "C" {
extern f32 lbl_8055D0B4;  // last DMA draw time
void fn_802E30C8(const char* fmt, ...);  // debug print
int fn_802044E8(void);
int fn_80204870(void);
int fn_802E4A10(void);
void fn_80129374(int i, int x);
int fn_80204024(void);
void fn_801E7C1C(RaceSetup* rs);
void fn_801E8818(void (*fn)(void), const char* name);
void fn_801E8820(void (*fn)(void));
void fn_8012E81C(const char* name);
void fn_8023889C(void);
void fn_801CB808(void);
void fn_801617BC(void);
void fn_802061F8(void);
void fn_8020D178(void);
void fn_802108C8(void);
void fn_802B8744(void);
void fn_801769EC(void);
void fn_801769E0(void);
void fn_801332D0(int slot, const char* name, s32* value);
void fn_80133348(int slot, const char* name, s32 value);
void fn_80265D74(void);
void fn_80128678(void);
void fn_80128684(void);
void fn_80206454(void);
void fn_80206444(void);
void fn_80127930(void);
void fn_80127954(void);
void fn_802D3E9C(int unk);
void fn_80265D78(void);
void fn_802B87A0(void);
void fn_80210904(void);
void fn_8020D1C8(void);
void fn_80206228(void);
void fn_801618B4(void);
void fn_801CB8C0(void);
void fn_80238920(void);
void fn_801E7DEC(RaceSetup* rs);
void fn_8017685C(int unk);
void fn_801AF6FC(void);
void fn_801AF92C(RaceResult* res);
Racer* fn_801EA9A0(int index);
int fn_80231504(int unk);
void fn_801DF110(void);
void fn_802C8530(void);
int fn_801DF060(void);
void fn_801E81B8(int a, void (*draw)(void));
void fn_801DFFB4(void);
void fn_802C9A70(int unk);
int fn_801E8838(void);
void fn_802C942C(u32 color);
void fn_802C855C(void);
int fn_801E85A4(void);
void fn_802D08C4(int unk);
void fn_802C2B48(void);
void fn_8020DE10(int (*fn)(int, int), void (*draw)(void));
int fn_8016212C(void);
int fn_80265D64(void);
void fn_80176734(void);
int fn_80265D6C(void);
void fn_80265D54(int unk);
void fn_80265D50(void);
void fn_801CB3F8(void);
void fn_80210950(int (*fn)(int), int unk);
int fn_80137C98(int pad);
int fn_801DF238(int key);
int fn_801769F8(void);
int fn_801E86F8(int player);
void fn_801E86E8(int index, int player);
int fn_801E868C(int player, s32* type, s32* arg);
int fn_801E7DF0(int player);
void fn_801E7E04(int player, int unk);
int fn_802ECC10(void);
int fn_80161C70(void);
void fn_80129AEC(int unk);
void fn_80129B60(void);
int fn_801769A8(void);
int fn_802E47B8(void);
u32 fn_802E46F0(void);
int fn_8020DE1C(void);
void fn_80176780(int a, int b);
int fn_802E2C20(void);
int fn_801298E8(PadState* pad, int index);
int fn_801E85AC(void);
void fn_8026A1F0(int cam, int player, int unk);
void fn_801E7E48(void);
void fn_80176BAC(void);
int fn_801E89EC(void);
int fn_8012742C(void);
void fn_8017691C(void);
void fn_801CB4DC(void);
void fn_80230B5C(Camera* cam);
void fn_802C8638(Camera* cam);
void fn_802C974C(f32 a, f32 b, f32 c);
void fn_80010074(u32 color, int flags, const char* fmt, f32 scale, f32 x, f32 y, ...);
void fn_80011A2C(void);
void fn_802C9904(void);

void fn_80007238(void);
int fn_800074EC(int key);
void fn_80007498(void);
void fn_800070DC(void);
int fn_800075A0(int player);
int fn_80007554(int player, u32 type, int key);
void fn_80006D9C(void);
void fn_80006E54(void);
void fn_8000AB44(void);
void fn_8000CE60(void);
void fn_8000CEF0(void);
void fn_8000D000(int (*callback)(int), void (*draw)(void), int item);
void fn_8000D048(int (*callback)(int), void (*draw)(void), int item);
void fn_8000D070(void);
void fn_8000D100(void);
void fn_8000D19C(void);
void fn_8000D1DC(void);
int fn_8000D540(int key);
int fn_8000D75C(void);
int fn_8000D764(void);
int fn_800074C4(void);
int fn_8000749C(void);
void fn_80009C80(void);
int fn_80009C8C(void);
int fn_80009CB8(void);

void fn_8013A1F4(void);
int fn_8013A1F8(int key);
void fn_801AE5A4(void);
int fn_801AE59C(int key);
void fn_801E45CC(void);
int fn_801E47BC(int key);
void fn_801B3284(void);
int fn_801B327C(int key);
void fn_80127958(void);
int fn_8012795C(int key);
void fn_8018BF5C(void);
int fn_8018BF54(int key);
void fn_8020BF58(void);
int fn_8020BDB8(int key);
void fn_80296070(void);
int fn_80296068(int key);
void fn_80204008(void);
int fn_8020400C(int key);
void fn_80204004(void);
void fn_80203FFC(void);
void fn_80204000(void);
void fn_801CA67C(void);
int fn_801CB114(int key);
void fn_801B8978(void);
int fn_801B89C8(int key);
void fn_801B89B4(void);
void fn_801B898C(void);
void fn_801B89A0(void);
void fn_802ADBB4(void);
int fn_802ADCF8(int key);
void fn_80265D94(void);
void fn_8020D2D4(void);
int fn_8020DB0C(int key);
void fn_8020DABC(void);
void fn_8020D224(void);
void fn_8020D29C(void);
void fn_80204A50(void);
void fn_801DE154(void);
int fn_801DE5C4(int key);
void fn_801DE12C(void);
void fn_801DE150(void);
void fn_802B8848(void);
int fn_802B8998(int key);
void fn_802B87F4(void);
void fn_802B8C88(void);
void fn_802B8C98(void);
void fn_80148218(void);
int fn_8014821C(int key);
void fn_80148224(void);
void fn_80148228(void);
void fn_8014822C(void);
void fn_8021095C(void);
int fn_802111E4(int key);
void fn_802112AC(void);
void fn_802113D8(void);
void fn_802114C4(void);
}
int IsLoadingDone(void);
int IsLoadingActive(void);
void LoadingUpdate(void);
void AchievementsClearRace(void);
void AchievementsCommitRace(void);
int OptE3Demo(void);
void OptDirectToTrack(void);
int OptProfile(void);

void fn_8000AC98(void);
int fn_8000AC9C(int key);
void fn_8000ACA4(void);
void fn_8000ACA8(void);
void fn_8000ACAC(void);
void fn_8000ACB0(void);
int fn_8000ACDC(int key);
void fn_8000AD20(void);
int fn_8000AD24(int key);
void fn_8000C5CC(void);

GameMode lbl_802FBC90[25] = {
    {fn_80007238, fn_800074EC, NULL, NULL, NULL},
    {fn_8000D1DC, fn_8000D540, fn_8000D19C, fn_8000D070, fn_8000D100},
    {fn_8013A1F4, fn_8013A1F8, NULL, NULL, NULL},
    {fn_801AE5A4, fn_801AE59C, NULL, NULL, NULL},
    {fn_801E45CC, fn_801E47BC, NULL, NULL, NULL},
    {fn_80007498, NULL, NULL, NULL, NULL},
    {fn_8000AD20, fn_8000AD24, NULL, NULL, NULL},
    {fn_801B3284, fn_801B327C, NULL, NULL, NULL},
    {fn_80127958, fn_8012795C, NULL, NULL, NULL},
    {fn_8018BF5C, fn_8018BF54, NULL, NULL, NULL},
    {fn_8020BF58, fn_8020BDB8, NULL, NULL, NULL},
    {fn_80296070, fn_80296068, NULL, NULL, NULL},
    {fn_8000C5CC, NULL, NULL, NULL, NULL},
    {fn_80204008, fn_8020400C, fn_80204004, fn_80203FFC, fn_80204000},
    {fn_801CA67C, fn_801CB114, NULL, NULL, NULL},
    {fn_801B8978, fn_801B89C8, fn_801B89B4, fn_801B898C, fn_801B89A0},
    {fn_802ADBB4, fn_802ADCF8, NULL, NULL, NULL},
    {fn_80265D94, NULL, NULL, NULL, NULL},
    {fn_8020D2D4, fn_8020DB0C, fn_8020DABC, fn_8020D224, fn_8020D29C},
    {fn_8000ACB0, fn_8000ACDC, NULL, fn_80204A50, NULL},
    {fn_801DE154, fn_801DE5C4, NULL, fn_801DE12C, fn_801DE150},
    {fn_802B8848, fn_802B8998, fn_802B87F4, fn_802B8C88, fn_802B8C98},
    {fn_80148218, fn_8014821C, fn_80148224, fn_80148228, fn_8014822C},
    {fn_8000AC98, fn_8000AC9C, fn_8000ACA4, fn_8000ACA8, fn_8000ACAC},
    {fn_8021095C, fn_802111E4, fn_802112AC, fn_802113D8, fn_802114C4},
};

// Not referenced in the final game; kept for their strings.
static void DrawPressTwo(void) {
    fn_802E30C8("gameloop: press 2 to start!");
    fn_8012E81C("press2.tex");
}

// Six-word static built by an inline constructor (first word stored first).
class GameLoopStatic {
public:
    GameLoopStatic(void) {
        unk0 = 0;
        unk4 = 123;
        unk8 = 0;
        unkC = 0;
        unk10 = 0;
    }
    u32 unk0;
    u32 unk4;
    u32 unk8;
    u32 unkC;
    u32 unk10;
    u32 unk14;
};

ModuleTag lbl_8055D3C0;
UnkStaticFFFF lbl_8055D3C8;
static GameLoopStatic lbl_8034F5A0;
UnkStatic7F lbl_8055D3CC;

ViewCam lbl_8034F5B8[4];
s32 lbl_8034F5D8[8];  // allowed racing cameras
Vec3 lbl_8034F5F8;
Vec3 lbl_8034F604;

GameMode* lbl_8055D3D0;  // current mode
u8 lbl_8055D3D4;         // race running
u8 lbl_8055D3D5;
s32 lbl_8055D3D8;        // last input time
s32 lbl_8055D3DC;        // last mode
s32 lbl_8055D3E0;        // camera count
s32 lbl_8055D3E4;        // mode owner pad
s32 lbl_8055D3E8;        // consecutive races
u8 lbl_8055D3EC;
u8 lbl_8055D3ED;

void fn_8000AC98(void) {}

int fn_8000AC9C(int key) { return 0; }

void fn_8000ACA4(void) {}

void fn_8000ACA8(void) {}

void fn_8000ACAC(void) {}

void fn_8000ACB0(void) {
    if (!fn_802044E8()) {
        fn_80007238();
    }
}

int fn_8000ACDC(int key) {
    if (fn_80204870()) {
        return 1;
    }
    return fn_800074EC(key);
}

void fn_8000AD20(void) {}

int fn_8000AD24(int key) { return 0; }

int fn_8000AD2C(void) {
    switch (lbl_8055D3D0 - lbl_802FBC90) {
    case 0:
    case 1:
    case 0x12:
        return 0;
    }
    return 1;
}

int fn_8000AD84(void) {
    switch (lbl_8055D3D0 - lbl_802FBC90) {
    case 1:
    case 0x12:
    case 0x18:
        return 1;
    }
    return 0;
}

void fn_8000ADE0(int mode) {
    if (mode != 1 && mode != 0x12 && mode != 0x18) {
        lbl_8055D3DC = mode;
    }
    if (lbl_8055D3D0 && lbl_8055D3D0->leave) {
        lbl_8055D3D0->leave();
    }
    if ((lbl_8055D3D0 = &lbl_802FBC90[mode]) && lbl_8055D3D0->enter) {
        lbl_8055D3D0->enter();
    }
    lbl_8055D3E4 = fn_802E4A10();
}

void fn_8000AE78(int x) {
    int i;
    for (i = 0; i < 2; i++) {
        fn_80129374(i, x);
    }
}

void fn_8000AEC4(void) { fn_800070DC(); }

int fn_8000AEC8(s32 cam) {
    int i;
    for (i = 0; i < lbl_8055D3E0; i++) {
        if (cam == lbl_8034F5D8[i]) {
            return 1;
        }
    }
    return 0;
}

void fn_8000AF04(u32 flags) {
    lbl_8034F5D8[0] = 4;
    lbl_8055D3E0 = 1;
}

enum { MODE_RACE = 0, MODE_INTRO = 0xD };

void fn_8000AF1C(RaceSetup* rs) {
    int intro;
    ViewCam* c;
    int i;
    s32 cam;

    lbl_8055D3D8 = 0;
    lbl_8055D3E8 = 0;
    intro = fn_80204024();
    lbl_8055D3DC = 0;
    fn_801E7C1C(rs);
    fn_801E8818(fn_8000AEC4, "gameloop");
    fn_8012E81C("overlays.res");
    fn_8000AB44();
    fn_8023889C();
    fn_801CB808();
    fn_801617BC();
    fn_802061F8();
    fn_80006D9C();
    fn_8000CE60();
    fn_8020D178();
    fn_802108C8();
    fn_802B8744();
    fn_801769EC();
    fn_8000AF04(rs->flags);
    for (i = 0, c = lbl_8034F5B8; i < 4; c++, i++) {
        memset(c, 0, sizeof(*c));
        cam = lbl_8034F5D8[0];
        fn_801332D0(i + 12, "last_race_cam", &cam);
        if (cam >= 0x22 || cam < 0) {
            fn_802E30C8("gameloop_begin: Viewport %d has out-of-range racing cam %d, resetting to racing_camera_tab[0]",
                        i, cam);
            cam = lbl_8034F5D8[0];
        } else if (!fn_8000AEC8(cam)) {
            cam = lbl_8034F5D8[0];
        }
        c->cam = cam;
        c->next = 0x22;
    }
    lbl_8055D3D5 = 0;
    lbl_8055D3D4 = 1;
    lbl_8055D3D0 = NULL;
    fn_8000ADE0(intro ? MODE_INTRO : MODE_RACE);
    fn_80265D74();
    fn_80128678();
    fn_80206454();
    fn_80127930();
    fn_802D3E9C(0);
}

void fn_8000B09C(RaceSetup* rs) {
    ViewCam* c;
    int i;
    fn_80127954();
    fn_80128684();
    fn_80265D78();
    for (i = 0, c = lbl_8034F5B8; i < 4; i++, c++) {
        fn_80133348(i + 12, "last_race_cam", c->cam);
    }
    fn_802B87A0();
    fn_80210904();
    fn_8020D1C8();
    fn_8000CEF0();
    fn_80006E54();
    fn_80206228();
    fn_801618B4();
    fn_801CB8C0();
    fn_80238920();
    fn_801E8820(fn_8000AEC4);
    fn_801E7DEC(rs);
}

void fn_8000B150(RaceResult* res) {
    RaceResultEntry* e;
    int i;
    fn_8017685C(1);
    fn_801AF6FC();
    if (res) {
        fn_801AF92C(res);
        res->aborted = lbl_8055D3D5;
        if (!lbl_8055D3D5) {
            lbl_8055D3E8++;
        }
        res->races = lbl_8055D3E8;
        fn_802E30C8("gameloop_postrace: Player ran %i consecutive races.", lbl_8055D3E8);
        for (i = 0, e = (RaceResultEntry*)res; i < res->count; i++, e++) {
            e->value = fn_801EA9A0(i)->unkAC;
        }
    }
}

void fn_8000B210(void) {
    int saved;
    if (lbl_8055D3D0->draw) {
        if (fn_8000AD2C()) {
            saved = fn_80231504(0);
            lbl_8055D3D0->draw();
            fn_80231504(saved);
        } else {
            lbl_8055D3D0->draw();
        }
    }
}

void fn_8000B288(void) {
    fn_8000B210();
    fn_801DF110();
}

void fn_8000B2AC(void) {
    fn_802C8530();
    fn_801E81B8(1, fn_801DF060() ? fn_8000B288 : fn_8000B210);
    fn_801DFFB4();
    fn_802C9A70(0);
    if (fn_801E8838() && IsLoadingDone()) {
        fn_802C942C(fn_801E8838() << 24);
    }
    fn_802C855C();
    fn_801E85A4();
    fn_802D08C4(0);
    fn_802C2B48();
    fn_802D08C4(1);
}

s32 fn_8000B33C(ValueNode* node, const char* key, s32 def) {
    if (node->Has(key)) {
        return node->GetIntValue2(key);
    }
    return def;
}

void fn_8000B3B4(void) {
    if (lbl_802FBC90[lbl_8055D3DC].draw) {
        lbl_802FBC90[lbl_8055D3DC].draw();
    }
}

int fn_8000B5A8(int choice, int key);

void fn_8000B3DC(u8 replay) {
    lbl_8055D3EC = replay;
    fn_8000ADE0(0x12);
    fn_8020DE10(fn_8000B5A8, lbl_8055D3EC ? fn_8000B3B4 : NULL);
}

int fn_8000B42C(int choice) {
    int resume = 1;
    if (!fn_8016212C()) {
        switch (choice) {
        case 0:
            break;
        case 1:
            if (!fn_80265D64()) {
                fn_80176734();
            } else if (fn_80265D6C()) {
                fn_80265D54(2);
            } else {
                fn_802E30C8("gameloop: ignoring mp client 'restart' request");
            }
            fn_801CB3F8();
            AchievementsClearRace();
            break;
        case 3:
            if (!fn_80265D64()) {
                lbl_8055D3D4 = 0;
                lbl_8055D3D5 = 1;
            } else if (fn_80265D6C()) {
                lbl_8055D3D4 = 0;
            } else {
                fn_80265D50();
            }
            break;
        case 2:
            resume = 0;
            fn_8000B3DC(1);
            break;
        }
    }
    if (resume) {
        fn_8000ADE0(lbl_8055D3DC);
    }
    return 1;
}

int fn_8000B53C(int choice) {
    fn_8000ADE0(0x12);
    fn_8020DE10(fn_8000B5A8, lbl_8055D3EC ? fn_8000B3B4 : NULL);
    if (choice == 1) {
        return 1;
    }
    return 0;
}

int fn_8000B5A8(int choice, int key) {
    if (choice == 2 && (key == 'X' || key == '\r')) {
        fn_8000ADE0(0x18);
        fn_80210950(fn_8000B53C, 0);
        return 1;
    }
    if (lbl_8055D3EC) {
        fn_8000ADE0(1);
        fn_8000D000(fn_8000B42C, fn_8000B3B4, 2);
    } else {
        fn_8000ADE0(lbl_8055D3DC);
    }
    return choice == 5;
}

int fn_8000BD9C(int player, int key, s32* cam);
int fn_8000BF10(int key, s32* cam);

void fn_8000B64C(int key, int pad) {
    int player = fn_80137C98(pad);
    if (player < 0) {
        return;
    }
    if (fn_801DF238(key)) {
        return;
    }
    if (fn_8000AD84() && pad != lbl_8055D3E4) {
        return;
    }
    if (lbl_8055D3D0->handleKey && lbl_8055D3D0->handleKey(key)) {
        return;
    }
    switch (key) {
    case 0x60:
    case '\n':
    case '~':
    case 0x121:
    case 0x122:
        break;
    case 0x1B:
        if (!fn_8000D75C() && !fn_801E8838() && fn_800075A0(player)) {
            fn_8000D000(fn_8000B42C, fn_8000B3B4, 0);
            fn_8000ADE0(1);
        }
        break;
    case 'P':
    case 'p':
        if (!fn_80265D64() && !fn_8000D75C()) {
            if (fn_801769F8()) {
                fn_801769EC();
                fn_80206454();
            } else {
                fn_801769E0();
                fn_80206444();
            }
        }
        break;
    case 0x124:
        if (fn_801E86F8(player) != -1) {
            fn_801E86E8(fn_801E86F8(player), player);
        }
        break;
    case '\\':
        if (!fn_801769F8()) {
            fn_802E30C8("physics must be paused to start dma test.");
        }
        break;
    default:
        fn_8000BF10(key, &lbl_8034F5B8[0].cam);
        break;
    }
}

void fn_8000B838(void) {
    ViewCam* c;
    int i;
    s32 type;
    s32 arg;
    for (i = 0, c = lbl_8034F5B8; i < fn_801E85A4(); c++, i++) {
        type = 0;
        arg = 0;
        if (fn_801E868C(i, &type, &arg)) {
            switch (type) {
            case 3:
                if (fn_801EA9A0(fn_801E86F8(i))->state != 3) {
                    if (fn_801E7DF0(i)) {
                        fn_801E7E04(i, 0);
                    } else {
                        fn_801E7E04(i, 3);
                    }
                }
                break;
            case 0:
                if (fn_801EA9A0(fn_801E86F8(i))->state != 3) {
                    fn_8000BD9C(i, 0, &c->cam);
                }
                break;
            default:
                fn_80007554(i, type, arg);
                break;
            }
        }
    }
}

void fn_8000B950(void) {
    u32 q = fn_802ECC10();
    int m;
    u32 key;
    if (q != lbl_8055D3ED) {
        lbl_8055D3ED = q;
        if (!fn_8000D75C() && !fn_801DF060() && lbl_8055D3D0 == lbl_802FBC90) {
            if (!q) {
                fn_801769EC();
                fn_80206454();
            } else {
                fn_801769E0();
                fn_80206444();
            }
        }
    }
    if (!fn_802ECC10() && !fn_801DF060() && !IsLoadingActive() && fn_80161C70() && !fn_801E8838() &&
        lbl_8055D3D0 == lbl_802FBC90) {
        m = fn_8000D764();
        if (m != -1) {
            fn_8000D048(fn_8000B42C, fn_8000B3B4, 0);
            fn_8000ADE0(1);
            lbl_8055D3E4 = m;
        }
    }
    fn_80129AEC(-1);
    fn_80129B60();
    if (!(fn_801769A8() & 0x4000)) {
        fn_8000B838();
    }
    if (fn_802E47B8()) {
        key = fn_802E46F0();
        key = fn_802ECC10() ? 0 : key;
        if ((u16)key) {
            if ((fn_801769A8() & 0x4000) && ((u16)key == 'X' || (u16)key == '\r') && !fn_8000D75C() &&
                !fn_8020DE1C() && !fn_801E8838()) {
                fn_80176780(2, 0);
            } else {
                fn_8000B64C((u16)key, fn_802E4A10());
            }
        }
    }
}

int fn_8000BB08(void) {
    int now;
    int active;
    int i;
    int j;
    PadState pad;
    if (OptE3Demo()) {
        now = fn_802E2C20();
        active = 0;
        for (i = 0; i < 4; i++) {
            if (fn_801298E8(&pad, i)) {
                for (j = 0; j < 8; j++) {
                    if (pad.buttons[j]) {
                        active = 1;
                    }
                }
            }
        }
        if (!active && lbl_8055D3D8) {
            if (lbl_8055D3D8 && now > lbl_8055D3D8 && now - lbl_8055D3D8 > 60000) {
                return 1;
            }
        } else {
            lbl_8055D3D8 = now;
        }
        return 0;
    }
    return 0;
}


void fn_8000BC44(void) {
    ViewCam* c;
    int i;
    for (i = 0, c = lbl_8034F5B8; i < fn_801E85AC(); c++, i++) {
        if (c->next == 0x22) {
            fn_8026A1F0(c->cam, i, 0);
        } else {
            fn_8026A1F0(c->next, i, 0);
            c->next = 0x22;
        }
    }
    LoadingUpdate();
    fn_801E7E48();
    fn_80176BAC();
    fn_8000AE78(!fn_8000D75C());
    if (lbl_8055D3D0->update) {
        lbl_8055D3D0->update();
    }
    fn_8000B950();
    if (fn_801E89EC() == 3) {
        fn_802E30C8("game_loop: The Race is over");
        lbl_8055D3D4 = 0;
    } else if (fn_8012742C()) {
        lbl_8055D3D4 = 0;
    }
    if (fn_8000BB08()) {
        fn_802E30C8("gameloop: player has been snoozing; aborting race");
        fn_8017691C();
    }
}

void fn_8000BD64(void) {
    lbl_8034F5F8.x = 1000.0f;
    lbl_8034F5F8.z = 0.0f;
    lbl_8034F5F8.y = 0.0f;
    lbl_8034F604.x = 1000.0f;
    lbl_8034F604.z = 0.0f;
    lbl_8034F604.y = 0.0f;
}

void fn_8000BD98(int frames) {}

int fn_8000BD9C(int player, int key, s32* cam) {
    int n;
    int found;
    int i;
    switch (key) {
    case 0:
        n = lbl_8055D3E0;
        found = 0;
        for (i = 0; i < n; i++) {
            if (*cam == lbl_8034F5D8[i]) {
                *cam = lbl_8034F5D8[(i + 1) % n];
                found = 1;
                break;
            }
        }
        if (!found) {
            *cam = lbl_8034F5D8[0];
        }
        break;
    case 0x170:
        *cam = 0;
        return 1;
    case 0x171:
        *cam = 1;
        return 1;
    case 0x172:
        *cam = 2;
        return 1;
    case 0x173:
        *cam = 4;
        return 1;
    case 0x373:
        *cam = 7;
        return 1;
    case 0x174:
        *cam = 5;
        return 1;
    case 0x175:
        *cam = 6;
        return 1;
    case 0x176:
        *cam = 0xA;
        return 1;
    case 0x376:
        *cam = 0x10;
        return 1;
    }
    return 0;
}

int fn_8000BF10(int key, s32* cam) { return fn_8000BD9C(0, key, cam); }

void fn_8000BF20(RaceSetup* rs, GameListener* listener) {
    int intro = fn_80204024();
    int maxFrames;
    int frames;
    RaceResult result;

    fn_8000AF1C(rs);
    fn_80176780(0x1200, 0);
    if (listener) {
        listener->Begin();
    }
    do {
        do {
            fn_80206454();
            lbl_8055D3D4 = 1;
            if (!intro) {
                OptDirectToTrack();
            }
            fn_80176780(0x10002, 0);
            maxFrames = OptProfile() ? 500 : 0;
            frames = 0;
            fn_8000BD64();
            while (lbl_8055D3D4) {
                fn_8000BC44();
                if (lbl_8055D3D4) {
                    fn_8000B2AC();
                }
                frames++;
                if (maxFrames && frames >= maxFrames) {
                    lbl_8055D3D4 = 0;
                }
            }
            fn_8000BD98(frames);
            fn_802E30C8("gameloop: last frames' DMADrawTime: %1.2f", lbl_8055D0B4);
            fn_8000B150(&result);
            if (listener) {
                listener->End(&result);
            }
            if (result.finished && !result.aborted) {
                fn_801CB4DC();
                AchievementsCommitRace();
            } else {
                AchievementsClearRace();
            }
        } while (fn_800074C4());
        if (fn_8000749C()) {
            fn_80128684();
            do {
                fn_80009C80();
            } while (fn_80009CB8());
            fn_80128678();
        }
        fn_80176780(0x1200, 0);
    } while (fn_80009C8C());
    if (listener) {
        listener->Finish();
    }
    fn_8000B09C(rs);
}

void fn_8000C0E8(RaceSetup* rs) {
    RaceSetup copy = *rs;
    int i;
    RacerSlot* s;
    copy.unk3F4 = 1;
    copy.racers.count = 1;
    for (i = 0, s = rs->racers.slots; i < rs->racers.count; i++, s++) {
        if (s->type == 'P') {
            copy.racers.slots[0] = *s;
        }
    }
    fn_8000BF20(&copy, NULL);
}

void fn_8000C5CC(void) {
    Camera cam;
    fn_80230B5C(&cam);
    fn_802C8638(&cam);
    fn_802C974C(1.0f, 1000.0f, 1.4398966f);
    fn_80010074(0xFFFFFF00, 9, "DEMO", 1.0f, -270.0f, -180.0f);
    fn_80011A2C();
    fn_802C9904();
}

void fn_8000C630(s32 cam, int player) {
    int i;
    if (player == -1) {
        for (i = 0; i < 4; i++) {
            fn_8000C630(cam, i);
        }
    } else {
        ViewCam* c = &lbl_8034F5B8[player];
        c->next = cam;
    }
}

const char* fn_8000C69C(void) {
    switch (lbl_8055D3DC) {
    case 0:
        return "Off";
    case 0x13:
        return "Special";
    case 0x14:
        return "Garage";
    }
    return "Unknown";
}

void fn_8000C6E4(void) {
    switch (lbl_8055D3DC) {
    case 0:
        lbl_8055D3DC = 0x14;
        break;
    case 0x14:
        lbl_8055D3DC = 0x13;
        break;
    case 0x13:
    default:
        lbl_8055D3DC = 0;
        break;
    }
}

void fn_8000C730(void) {
    switch (lbl_8055D3DC) {
    case 0:
        lbl_8055D3DC = 0x13;
        break;
    case 0x13:
        lbl_8055D3DC = 0x14;
        break;
    case 0x14:
    default:
        lbl_8055D3DC = 0;
        break;
    }
}
