// World (0x801E78D8-0x801E9C84): begin/end of a race world (loading the track, creating and
// deleting world objects and animations), the per-frame update and draw of every player view, the
// per-view accessors (viewport, camera fields, player index), the wind, and the track walls from
// the event's "wall_file". Names are guesses apart from the strings.
#include "game/common.h"

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

extern "C" void* memset(void* p, int c, u32 n);

struct WVec {
    f32 x, y, z;
};

struct WMtx {
    WVec x;    // offset 0x0
    WVec y;    // offset 0xC
    WVec z;    // offset 0x18
    WVec pos;  // offset 0x24
};

// The camera part of a view (0x30).
struct WRot {
    WVec right;  // offset 0x0
    WVec up;     // offset 0xC
    WVec dir;    // offset 0x18
};

struct WCam {
    WRot rot;    // offset 0x0
    WVec pos;    // offset 0x24
};

// One view of the world as the renderer sees it (0xE0).
struct WView {
    f32 time;      // offset 0x0
    WCam cam;      // offset 0x4
    f32 fov;       // offset 0x34
    f32 aspect;    // offset 0x38
    s32 vx;        // offset 0x3C
    s32 vy;        // offset 0x40
    s32 vw;        // offset 0x44
    s32 vh;        // offset 0x48
    f32 f4C;       // offset 0x4C
    f32 f50;       // offset 0x50
    f32 f54;       // offset 0x54
    f32 f58;       // offset 0x58
    u8 pad5C[0x64 - 0x5C];
    s32 player;    // offset 0x64
    u8 pad68[0x6C - 0x68];
    u32 flags;     // offset 0x6C
    u32 tex;       // offset 0x70
    u32 size;      // offset 0x74
    u8 pad78[0xE0 - 0x78];
};

struct WorldState {
    WView views[4];      // offset 0x0
    s32 f380;
    s32 cams[4];         // offset 0x384
    s32 numViews;        // offset 0x394
    s32 f398;            // offset 0x398
    u8 pad39C[0x3A0 - 0x39C];
    u8 racers[8][0x38];  // offset 0x3A0
    u8 pad560[0xDB0 - 0x560];
    s32 fDB0;            // offset 0xDB0
    f32 fDB4[75];        // offset 0xDB4
    u8 bufEE0[0x1004];   // offset 0xEE0
    u8 buf1EE4[0x1004];  // offset 0x1EE4
    u8 anim[4];          // offset 0x2EE8
};

// The part of a view kept between frames (0x38).
struct ViewState {
    ViewState(void) {}
    WCam cam;    // offset 0x0
    u32 flags;   // offset 0x30
    u32 prev;    // offset 0x34
};

struct EventCfg {
    u8 pad0[0x10];
    u32 flags;  // offset 0x10
};

// One car of the race (0x78).
struct CarDef {
    CarDef(void);
    s32 type;      // offset 0x0
    u8 b4[0x60];   // offset 0x4
    s32 f64;       // offset 0x64
    s32 f68;       // offset 0x68
    s32 f6C;       // offset 0x6C
    u8 f70;        // offset 0x70
    s32 f74;       // offset 0x74
};

struct RaceCars {
    CarDef defs[8];  // offset 0x0
    s32 count;       // offset 0x3C0
};

struct RaceSetup {
    RaceSetup(void);
    u8 pad0[0x20];
    RaceCars cars;   // offset 0x20
    s32 event;       // offset 0x3E4
    u8 pad3E8[0x3F8 - 0x3E8];
};

struct StartOpts {
    s32 f0;
};

struct EventInfo {
    u8 pad0[0x20];
    ValueNode* node;  // offset 0x20
};

struct CarTypeMap {
    s32 tag;
    s32 type;
    s32 f8;
};

extern "C" CarTypeMap* fn_801E8EA4(int type);
extern "C" void fn_801E9DC4(void* p);

// What a car model is built from (0xE4).
struct Racer {
    Racer(const CarDef& d, int i) : def(d), index(i) {
        CarTypeMap* m = fn_801E8EA4(d.type);
        model = m->f8;
        tag = m->tag;
        memset(zero, 0, sizeof(zero));
        fn_801E9DC4(zero + 0x40);
    }
    s32 tag;          // offset 0x0
    s32 model;        // offset 0x4
    u8 pad8[0x10];
    u8 zero[0x50];    // offset 0x18
    CarDef def;       // offset 0x68
    s32 index;        // offset 0xE0
};

class WObj {
public:
    virtual ~WObj(void);
    virtual void Update(void);    // 0xC
    virtual void Draw(void);      // 0x10
    virtual void PostDraw(void);  // 0x14
};

class AnimObj : public WObj {
public:
    void Update(u8* buf);
};

class WheelAnim : public AnimObj {
public:
    WheelAnim(const char* name);
    u8 pad4[0x84 - 0x4];
};

class AnimPoint : public AnimObj {
public:
    AnimPoint(const char* name);
    u8 pad4[0x7C - 0x4];
};

class CarModel : public AnimObj {
public:
    CarModel(Racer* r, CarDef* d);
    u8 pad4[0x4E4 - 0x4];
};

class PlayerCarModel : public CarModel {
public:
    PlayerCarModel(Racer* r, CarDef* d);
};

struct WheelSlot {
    s32 tag;   // offset 0x0
    s32 size;  // offset 0x4
    f32 f8;
    f32 fC;
    f32 f10;
    f32 f14;
    f32 f18;
    f32 f1C;
    f32 f20;
    f32 f24;
};

struct AnimSlot {
    s32 tag;   // offset 0x0
    s32 size;  // offset 0x4
};

struct MutSlot {
    s32 tag;   // offset 0x0
    s32 size;  // offset 0x4
    f32 f8;
    f32 fC;
    f32 f10;
    f32 f14;
};

class PlayerObj {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void vf1C(void);
    virtual void vf20(void);
    virtual void Draw(void);  // 0x24
    u8 pad4[0x28 - 0x4];
    u8 f28[4];  // offset 0x28
    f32 fov;    // offset 0x2C
};

extern void* lbl_803338C0[];

extern "C" {
double sqrt(double x);
}

// A box-shaped collision volume along a track wall (0x38).
struct WallVol {
    WallVol(f32 w, f32 h, f32 l) {
        fC = 0;
        f4 = 0;
        f8 = 0;
        shape = 'BOX ';
        f10 = 0.0f;
        f14 = 0.0f;
        f20 = 100;
        f1C = 0;
        vtbl = lbl_803338C0;
        width = w;
        height = h;
        length = l;
        f34 = 0;
        radius = w + (f32)sqrt(w * w + h * h + l * l);
    }
    void SetType(int t) { ((void (*)(WallVol*, int))vtbl[0x40 / 4])(this, t); }
    void** vtbl;  // offset 0x0
    s32 f4;
    s32 f8;
    s32 fC;
    f32 f10;
    f32 f14;
    s32 shape;    // offset 0x18
    s32 f1C;
    s32 f20;
    f32 width;    // offset 0x24
    f32 height;   // offset 0x28
    f32 length;   // offset 0x2C
    f32 radius;   // offset 0x30
    s32 f34;
};

static inline f32 Angle(f32 a) { return a; }
static inline f32 Min(f32 a, f32 b) { return b < a ? b : a; }
static inline f32 Max(f32 a, f32 b) { return a > b ? a : b; }
static inline f32 DMin(f32 a, f32 b) { return b < a ? b : a; }
static inline double DMax(f32 a, f32 b) { return a > b ? a : b; }

int GetNumPlayers(void);
void LoadTimerMark(const char* name, f32 step);
void LoadTimerBegin(const char* name, int a, int b, f32 duration);
void LoadTimerEnd(const char* name);
ValueTable* GetEvent(int i);
BOOL OptPaTest(void);

extern "C" {
void* memcpy(void* d, const void* s, u32 n);
double fmod(double x, double y);
int sprintf(char* s, const char* fmt, ...);
int strcmp(const char* a, const char* b);
int strncasecmp(const char* a, const char* b, u32 n);
char* strstr(const char* s, const char* t);
f32 fn_80129DC4(int seed, f32 lo, f32 hi);  // random float
void PSVECNormalize(const WVec* src, WVec* dst);
f32 PSVECMag(const WVec* v);
void PSVECCrossProduct(const WVec* a, const WVec* b, WVec* out);
void fn_802E30C8(const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);
s32 fn_802E2C20(void);  // milliseconds
s64 fn_802E2C9C(void);  // OSGetTime
void fn_80127464(void);
void fn_80206464(int a);
void fn_8020646C(int i, int a);
void fn_8012E81C(const char* name);
void fn_80287AF0(int a);
void fn_801FA550(void);
void fn_802D6C54(int i, f32 v);
BOOL fn_801B89F8(void);
void fn_801EBD18(int view);
void fn_8026A298(int view);
void fn_8026A2B0(int view);
int fn_8026A2C8(void);
u8* fn_801EA9A0(int i);
void fn_8026A248(int player, int view);
void fn_802C6CF8(s32 h);
s32 fn_802C6FC0(const char* file, int a, int b);
void fn_80204460(void);
void fn_801EA2E8(void);
void fn_801EEFC8(void);
void fn_80204BB8(void);
void fn_80203C50(int a);
void fn_801EFF4C(void);
void fn_801EA4C4(void);
void fn_801AEF28(RaceSetup* s);
void fn_80176460(s32* ev, RaceSetup* s, StartOpts* o);
void fn_8000D7F0(RaceSetup* s);
void fn_801EA1D8(void);
void fn_80287780(RaceSetup* s);
void fn_801F4CD8(void);
void fn_801EA31C(s32* ev);
void fn_801EA394(void);
void fn_801FFB6C(void);
void fn_8000A868(void);
void fn_801B826C(RaceSetup* s);
void fn_80136764(void);
void fn_8013477C(RaceSetup* s);
void fn_801F9E04(RaceSetup* s);
void fn_80265D8C(RaceSetup* s);
void fn_801DCE0C(int a);
void fn_801FC8DC(RaceSetup* s);
void fn_80139F5C(ValueTable* ev);
void fn_801FA298(RaceSetup* s);
void fn_801EC11C(void);
void fn_80287800(RaceSetup* s);
void fn_80176614(void);
void fn_8027AA50(void);
void fn_801FA9AC(void);
void fn_801EEE38(void);
void fn_802C6C30(void);
void fn_801EA38C(void);
void fn_802023D4(void);
void fn_8000D878(void);
void fn_802C2C60(int a);
void fn_802C8218(int a);
void fn_80203FEC(void);
void fn_802ECC18(int a);
void fn_80203FE8(void);
void fn_802025D8(void);
void fn_802C2C6C(void);
void fn_801EA390(void);
void fn_801766C4(void);
void fn_802878A8(void);
void fn_8013490C(void);
void fn_80265D90(void);
void fn_801F4D64(void);
void fn_801F9F08(void);
void fn_801B832C(void);
void fn_801DCE10(void);
void fn_801AF598(void);
void fn_801EA56C(void);
void fn_801EA3B8(void);
void fn_801FFB70(void);
void fn_801EFF70(void);
void fn_80203DC8(void);
void fn_802C7A00(void);
void fn_801FC900(void);
void fn_802877D0(void);
void fn_8000D7F4(RaceSetup* s);
void fn_80136768(void);
void fn_801765B8(void);
void fn_80205208(void);
void fn_801EF184(void);
void fn_801EA318(void);
void fn_801EA1DC(void);
void fn_8013A074(void);
void fn_8020449C(void);
void fn_802E3D00(void);
WorldState* fn_80176A70(void);
int fn_801EA994(void);
void fn_801EA9DC(int i, u8* racer);
void fn_80176BA8(void);
void fn_80287974(void);
void fn_8028797C(void);
void fn_801EA400(void);
void fn_801EF0AC(u8* buf);
void fn_801FA2C4(void);
void fn_801F4DC4(void);
void fn_80202AD4(u8* p);
void fn_80202924(void);
void fn_80134994(void);
void fn_8000D7FC(void);
void fn_802A4CA8(void);
void fn_802C276C(void);
void fn_80203E88(ViewState* v, u8* p, BOOL single);
void fn_8001A1F0(void);
void fn_802C87B4(s32 x, s32 y, s32 w, s32 h);
void fn_802CC73C(BOOL a);
void fn_8020625C(void);
void fn_8029227C(void);
void fn_802A4D08(void);
void fn_802DB2A4(void);
void fn_802DD754(void);
void fn_802A4D00(void);
void fn_80287978(void);
void fn_802C8638(ViewState* s);
void fn_802C2C64(int view, f32 a);
void fn_801EA404(ViewState* s, int a);
void fn_80203EC0(void);
void fn_802C7A44(void);
void fn_802C7A50(void);
void fn_802C7AD4(void);
void fn_801FA618(void);
void fn_802BF508(void);
void fn_802C2488(void);
void fn_802B0254(void);
void fn_802B0218(void);
void fn_80203EF4(void);
void fn_80203EC4(void);
void fn_80287A6C(int a);
void fn_80297984(void);
void fn_802889F0(void);
void fn_80287914(void);
void fn_801FA1D4(void);
void fn_802C1A08(void);
f32 fn_80203FE0(void);
void fn_802C7D54(s32 h, f32 (*m)[3], int mode, f32 a, f32 b, f32 c);
ValueTable* fn_80131808(const char* file);
void fn_80131818(ValueTable* t);
f32 fn_801DCF48(int a, f32 x, f32 z);
void fn_801345D8(char* buf, int n, const char* fmt, ...);
void fn_801FA8E0(WallVol* w, WMtx* m, int a);
}

extern s32 lbl_8055CFF0;
extern s32 lbl_8055CFF4;
extern char lbl_80339BA0[];
extern char lbl_80339CB4[];
extern char lbl_80339CFC[];

struct F4 {
    f32 v[4];
};

extern PlayerObj* lbl_80488648[8];
extern CarTypeMap lbl_80339CC0[];
extern F4 lbl_802F99F8;

static inline PlayerObj* GetPlayer(u32 i) { return i < 8 ? lbl_80488648[i] : 0; }

ModuleTag lbl_8055EAF0;
UnkStaticFFFF lbl_8055EAF8;
f32 lbl_8055EAFC = Angle(0.7853982f);
UnkStatic7F lbl_8055EB00;
WorldState* lbl_8055EB04;  // the world being run
u8 lbl_8055EB08;
s32 lbl_8055EB0C;
u8 lbl_8055EB10;
u8 lbl_8055EB11;
u8 lbl_8055EB12;
f32 lbl_8055EB14;  // frame time
f32 lbl_8055EB18;  // last world time
s32 lbl_8055EB1C;  // frame number
u8 lbl_8055EB20;
void (*lbl_8055EB24)(void);
f32 lbl_8055EB28;
s32 lbl_8055EB2C;  // views
s32 lbl_8055EB30;  // players
s32 lbl_8055EB34;  // world objects
s32 lbl_8055EB38;  // animated objects
WObj** lbl_8055EB3C;
AnimObj** lbl_8055EB40;

s32 lbl_80487CE0[4];
s32 lbl_80487CF0[4];
ViewState lbl_80487D00[4];
RaceSetup lbl_80487DE0;
f32 lbl_804881D8[4];  // wind direction and strength
char lbl_804881E8[0x40];

s32 lbl_8055AF08;
s32 lbl_8055AF0C = -1;  // the view being drawn
s32 lbl_8055AF10 = -1;

extern "C" {

void fn_801E7984(RaceSetup* s);
void fn_801E7C28(RaceSetup* s);
void fn_801E83D4(void);
void fn_801E8410(void);
int fn_801E866C(int view);
void fn_801E87D4(AnimObj* o);
void fn_801E89FC(void);
void fn_801E8A84(void);
void fn_801E8CE4(void);
void fn_801E8D48(void);
void fn_801E8DD4(void);
void fn_801E8E38(void);
void fn_801E8F10(RaceSetup* s);
void fn_801E9564(RaceSetup* s);
void fn_801E95DC(void);
void fn_801E96D8(RaceSetup* s);
void fn_801E9BE4(void);
EventCfg* fn_801E87A8(void);
void fn_801E88B4(s32* x, s32* y, s32* w, s32* h, int view);
void fn_801E8148(void);
f32 fn_801E7E18(void);
f32 fn_801E8854(int view);

void fn_801E78D8(RaceSetup* s) {
    int n = GetNumPlayers();
    int i;
    fn_80206464(n);
    for (i = 0; i < n; i++) {
        fn_8020646C(i, -1);
    }
    {
        RaceCars* c = &s->cars;
        CarDef* d = c->defs;
        int j;
        for (j = 0; j < c->count; d++, j++) {
            if (d->type == 'P') {
                fn_8020646C(d->f70 & 3, j);
            }
        }
    }
}

void fn_801E797C(void) { fn_80206464(0); }

void fn_801E7984(RaceSetup* s) {
    s32 t0;
    StartOpts o;
    fn_801E78D8(s);
    t0 = fn_802E2C20();
    fn_8012E81C("race.res");
    memcpy(&lbl_80487DE0, s, sizeof(RaceSetup));
    lbl_8055EB12 = 1;
    lbl_8055EB30 = GetNumPlayers();
    lbl_8055EB2C = -1;
    LoadTimerMark("foo", 500.0f);
    fn_80204460();
    fn_80127464();
    lbl_8055EB0C = -1;
    lbl_8055EB04 = 0;
    lbl_8055EB1C = 0;
    lbl_8055EB20 = 0;
    memset(lbl_80487CE0, 0, sizeof(lbl_80487CE0));
    lbl_80487CF0[0] = 0;
    lbl_80487CF0[1] = 0;
    lbl_80487CF0[2] = 0;
    lbl_80487CF0[3] = 0;
    fn_801E89FC();
    lbl_8055EB10 = 1;
    fn_80127464();
    fn_801EA2E8();
    lbl_8055EB3C = new ("world:objects[]") WObj*[720];
    lbl_8055EB40 = new ("world:wobjects[]") AnimObj*[720];
    lbl_8055EB38 = 0;
    lbl_8055EB34 = 0;
    lbl_8055EB08 = 1;
    LoadTimerMark("bar", 200.0f);
    fn_801EEFC8();
    memset(&o, 0, sizeof(o));
    o.f0 = s->cars.count;
    fn_80127464();
    fn_80204BB8();
    fn_80203C50(2);
    fn_801EFF4C();
    fn_801EA4C4();
    fn_801AEF28(s);
    fn_80127464();
    fn_80176460(&s->event, s, &o);
    if (!lbl_8055EB11) {
        fn_8000D7F0(s);
    }
    fn_801EA1D8();
    fn_80287780(s);
    fn_801F4CD8();
    fn_801EA31C(&s->event);
    fn_801EA394();
    fn_801FFB6C();
    fn_8000A868();
    fn_801B826C(s);
    fn_80136764();
    fn_8013477C(s);
    fn_801F9E04(s);
    fn_80265D8C(s);
    LoadTimerMark("load track", 500.0f);
    fn_801DCE0C(0);
    fn_801FC8DC(s);
    fn_80139F5C(GetEvent(s->event));
    fn_80127464();
    fn_801FA298(s);
    fn_801E96D8(s);
    fn_801EC11C();
    fn_801E9564(s);
    fn_80127464();
    fn_80287800(s);
    fn_80127464();
    fn_80176614();
    fn_80127464();
    fn_8027AA50();
    fn_801FA9AC();
    fn_80127464();
    fn_801EEE38();
    fn_802C6C30();
    fn_80127464();
    LoadTimerMark("Textures", 1000.0f);
    fn_801EA38C();
    fn_80127464();
    fn_802023D4();
    fn_80127464();
    LoadTimerEnd("-WorldBeginCommon");
    fn_8000D878();
    fn_802C2C60(1);
    fn_802C8218(0);
    fn_80203FEC();
    fn_80127464();
    fn_802E30C8("WorldBeginCommon: %d msec", fn_802E2C20() - t0);
    fn_802ECC18(1);
    fn_80127464();
}

void fn_801E7C1C(RaceSetup* s) {
    lbl_8055EB11 = 0;
    fn_801E7984(s);
}

void fn_801E7C28(RaceSetup* s) {
    int i;
    fn_802ECC18(0);
    fn_801E797C();
    fn_80203FE8();
    fn_802025D8();
    fn_802C2C6C();
    LoadTimerBegin("WorldEndCommon", 1, 1, 8500.0f);
    fn_801EA390();
    LoadTimerMark("PhysicsStop", 100.0f);
    fn_801766C4();
    LoadTimerMark("EfxEnd", 100.0f);
    fn_802878A8();
    fn_8013490C();
    fn_80265D90();
    fn_801F4D64();
    fn_801E9BE4();
    fn_801F9F08();
    fn_801B832C();
    for (i = 0; i < lbl_8055EB34; i++) {
        delete lbl_8055EB3C[i];
    }
    delete[] lbl_8055EB3C;
    lbl_8055EB3C = 0;
    LoadTimerMark("del wobject", 100.0f);
    delete[] lbl_8055EB40;
    lbl_8055EB40 = 0;
    fn_801DCE10();
    lbl_8055EB38 = 0;
    lbl_8055EB34 = 0;
    lbl_8055EB20 = 0;
    lbl_8055EB04 = 0;
    lbl_8055EB12 = 0;
    lbl_8055EB30 = 0;
    LoadTimerMark("ghost save", 100.0f);
    fn_801AF598();
    fn_801EA56C();
    fn_801EA3B8();
    fn_801FFB70();
    fn_801EFF70();
    fn_80203DC8();
    fn_802C7A00();
    fn_801FC900();
    fn_802877D0();
    if (!lbl_8055EB11) {
        fn_8000D7F4(s);
    }
    fn_80136768();
    LoadTimerMark("+physics end", 100.0f);
    fn_801765B8();
    fn_80205208();
    LoadTimerMark("PhysicsEnd", 500.0f);
    fn_801EF184();
    fn_801EA318();
    fn_801EA1DC();
    lbl_8055EB10 = 0;
    fn_8013A074();
    fn_8020449C();
    fn_802E3D00();
    fn_8000D878();
}

void fn_801E7DEC(RaceSetup* s) { fn_801E7C28(s); }

s32 fn_801E7DF0(int i) { return lbl_80487CF0[i]; }

void fn_801E7E04(int i, s32 v) { lbl_80487CF0[i] = v; }

f32 fn_801E7E18(void) { return lbl_8055EB04->views[0].time; }

f32 fn_801E7E24(void) { return lbl_8055EB14; }

s32 fn_801E7E2C(void) { return -1; }

WorldState* fn_801E7E34(void) { return lbl_8055EB04; }

void* fn_801E7E3C(void) { return lbl_8055EB04->racers[8]; }

static inline void SaveView(ViewState* s, WView* v) {
    if (fn_801E87A8()->flags & 2) {
        v->cam.rot.right.x *= -1.0f;
        v->cam.rot.right.y *= -1.0f;
        v->cam.rot.right.z *= -1.0f;
    }
    s->cam = v->cam;
    s->prev = s->flags;
    s->flags = v->flags;
}

void fn_801E7E48(void) {
    WorldState* w;
    s64 start;
    int i;
    start = fn_802E2C9C();
    w = fn_80176A70();
    lbl_8055EB14 = Max(DMax(DMin(w->views[0].time - lbl_8055EB18, 0.5f), -0.5f), 0.0f);
    lbl_8055EB18 = w->views[0].time;
    lbl_8055EB04 = w;
    if (lbl_8055EB2C != w->numViews) {
        fn_802C2C60(w->numViews);
        lbl_8055EB2C = w->numViews;
    }
    lbl_8055EB20 = 1;
    for (i = 0; i < lbl_8055EB2C; i++) {
        SaveView(&lbl_80487D00[i], &lbl_8055EB04->views[i]);
    }
    if (lbl_8055EB0C != lbl_8055EB04->f398) {
        lbl_8055EB0C = lbl_8055EB04->f398;
        fn_801E8CE4();
    }
    for (i = 0; i < lbl_8055EB38; i++) {
        lbl_8055EB40[i]->Update(w->anim);
    }
    for (i = 0; i < fn_801EA994(); i++) {
        fn_801EA9DC(i, lbl_8055EB04->racers[i]);
    }
    fn_80176BA8();
    fn_80287974();
    fn_8028797C();
    fn_801EA400();
    fn_801EF0AC(w->bufEE0);
    fn_801EF0AC(w->buf1EE4);
    fn_801E8A84();
    fn_801FA2C4();
    fn_801F4DC4();
    for (i = 0; i < lbl_8055EB34; i++) {
        lbl_8055EB3C[i]->Update();
    }
    if (lbl_8055EB2C == 1) {
        fn_80202AD4(GetPlayer(fn_801E866C(0))->f28);
    }
    fn_80202924();
    fn_80134994();
    fn_8000D7FC();
    lbl_8055EB28 = (s32)(fn_802E2C9C() - start) / 1000.0f;
}

void fn_801E8144(void) {}

void fn_801E8148(void) {
    int i;
    for (i = 0; i < lbl_8055EB34; i++) {
        lbl_8055EB3C[i]->Draw();
    }
    fn_80287AF0(0);
    fn_801FA550();
}

void fn_801E81B8(int unused, void (*cb)(void)) {
    int i;
    fn_802E2C9C();
    lbl_8055EB1C++;
    fn_802A4CA8();
    fn_802C276C();
    fn_80203E88(lbl_80487D00, GetPlayer(fn_801E866C(0))->f28, lbl_8055EB2C == 1);
    fn_8001A1F0();
    fn_801EA394();
    fn_802C87B4(0, 0, lbl_8055CFF0, lbl_8055CFF4);
    fn_802CC73C(lbl_8055EB2C <= 2);
    for (i = 0; i < lbl_8055EB2C; i++) {
        lbl_8055AF0C = i;
        fn_801E83D4();
    }
    for (i = 0; i < lbl_8055EB2C; i++) {
        lbl_8055AF0C = i;
        fn_801E8410();
    }
    fn_802CC73C(1);
    lbl_8055AF0C = -1;
    if (!fn_801B89F8() && lbl_8055EB2C > 1) {
        s32 x, y, w, h;
        fn_802C87B4(0, 0, lbl_8055CFF0, lbl_8055CFF4);
        fn_801E88B4(&x, &y, &w, &h, 1);
        fn_8020625C();
    }
    if (lbl_8055EB2C == 1) {
        f32 k;
        fn_8029227C();
        fn_802A4D08();
        k = OptPaTest() ? 0.5f : lbl_8055EB04->views[0].f58;
        if (k > 0.1f) {
            fn_802DB2A4();
        }
        fn_802DD754();
    }
    fn_802A4D00();
    fn_80287978();
    if (cb) {
        cb();
    }
}

void fn_801E837C(void) {
    int i;
    for (i = 0; i < 75; i++) {
        fn_802D6C54(i, lbl_8055EB04->fDB4[i]);
    }
}

void fn_801E83D4(void) {
    if (!fn_801B89F8() || lbl_8055AF0C == 0) {
        fn_801EBD18(lbl_8055AF0C);
    }
}

void fn_801E8410(void) {
    int i;
    fn_801E837C();
    if (!fn_801B89F8() || lbl_8055AF0C == 0) {
        ViewState* s = &lbl_80487D00[lbl_8055AF0C];
        s32 x = 0;
        s32 y = 0;
        s32 w = lbl_8055CFF0;
        s32 h = lbl_8055CFF4;
        if (!fn_801B89F8()) {
            fn_801E88B4(&x, &y, &w, &h, lbl_8055AF0C);
        }
        fn_802C87B4(x, y, w, h);
        fn_802C8638(s);
        fn_802C2C64(lbl_8055AF0C, fn_801E8854(-1));
        fn_801EA404(s, 0);
        fn_80203EC0();
        fn_802C7A44();
        fn_801E8148();
        fn_802C7A50();
        fn_802C7AD4();
        fn_801FA618();
        {
            PlayerObj** pp = lbl_80488648;
            for (i = 0; i < 8; i++, pp++) {
                PlayerObj* p = (u32)i < 8 ? *pp : 0;
                if (p) {
                    p->Draw();
                }
            }
        }
        fn_802BF508();
        fn_802C2488();
        fn_802B0254();
        fn_802B0218();
        fn_80203EF4();
        fn_80203EC4();
        fn_80287A6C(0);
        fn_801E95DC();
        for (i = 0; i < lbl_8055EB34; i++) {
            lbl_8055EB3C[i]->PostDraw();
        }
    }
}

void fn_801E85A0(void) {}

s32 fn_801E85A4(void) { return lbl_8055EB2C; }

s32 fn_801E85AC(void) { return lbl_8055EB30; }

s32 fn_801E85B4(void) { return lbl_8055AF0C; }

void fn_801E85BC(int view) {
    if (view < 0) {
        view = lbl_8055AF0C;
    }
    fn_8026A298(view);
}

void fn_801E85CC(int view) {
    if (view < 0) {
        view = lbl_8055AF0C;
    }
    fn_8026A2B0(view);
}

void fn_801E85DC(int view) {
    int i;
    int start;
    if (view < 0) {
        view = lbl_8055AF0C;
    }
    i = fn_801E866C(view);
    start = i;
    do {
        i++;
        i %= fn_8026A2C8();
    } while (!*fn_801EA9A0(i) && i != start);
    fn_8026A248(i, view);
}

int fn_801E866C(int view) {
    if (view < 0) {
        view = lbl_8055AF0C;
    }
    return lbl_8055EB04->views[view].player;
}

int fn_801E868C(int view, s32* w, s32* h) {
    WView* v = &lbl_8055EB04->views[view];
    u32 tex = v->tex;
    if (tex != lbl_80487CE0[view]) {
        lbl_80487CE0[view] = tex;
        *w = v->size >> 16;
        if (h) {
            *h = v->size & 0xFFFF;
        }
        return 1;
    }
    return 0;
}

void fn_801E86E8(int player, int view) {
    if (view < 0) {
        view = lbl_8055AF0C;
    }
    fn_8026A248(player, view);
}

s32 fn_801E86F8(int view) {
    if (view < 0) {
        view = lbl_8055AF0C;
    }
    return lbl_8055EB04->cams[view];
}

ViewState* fn_801E8718(int view) {
    if (view < 0) {
        view = lbl_8055AF0C;
    }
    return &lbl_80487D00[view];
}

f32 fn_801E8738(int view) {
    if (view < 0) {
        view = lbl_8055AF0C;
    }
    return lbl_8055EB04->views[view].fov;
}

int fn_801E8758(int view) {
    ViewState* s;
    if (view < 0) {
        view = lbl_8055AF0C;
    }
    s = &lbl_80487D00[view];
    return (s->flags & 0x10) || ((s->flags ^ s->prev) & 0x20);
}

RaceSetup* fn_801E879C(void) { return &lbl_80487DE0; }

EventCfg* fn_801E87A8(void) { return (EventCfg*)&lbl_80487DE0.event; }

void fn_801E87B8(WObj* o) { lbl_8055EB3C[lbl_8055EB34++] = o; }

void fn_801E87D4(AnimObj* o) {
    fn_801E87B8((WObj*)o);
    lbl_8055EB40[lbl_8055EB38++] = o;
}

void fn_801E8818(void (*f)(void)) { lbl_8055EB24 = f; }

void fn_801E8820(void) { lbl_8055EB24 = 0; }

void fn_801E882C(void) { lbl_8055EB08 = 1; }

s32 fn_801E8838(void) {
    if (lbl_8055EB04) {
        return lbl_8055EB04->fDB0;
    }
    return 0xFF;
}

f32 fn_801E8854(int view) {
    if (view < 0) {
        view = lbl_8055AF0C;
    }
    return lbl_8055EB04->views[view].f54;
}

f32 fn_801E8874(int view) {
    if (view < 0) {
        view = lbl_8055AF0C;
    }
    return lbl_8055EB04->views[view].f4C;
}

f32 fn_801E8894(int view) {
    if (view < 0) {
        view = lbl_8055AF0C;
    }
    return lbl_8055EB04->views[view].f50;
}

void fn_801E88B4(s32* x, s32* y, s32* w, s32* h, int view) {
    WView* v;
    if (view < 0) {
        view = lbl_8055AF0C;
    }
    v = &lbl_8055EB04->views[view];
    *x = v->vx;
    *y = v->vy;
    *w = v->vw;
    *h = v->vh;
}

f32 fn_801E88F0(int view) {
    F4 k;
    int n;
    if (view < 0) {
        view = lbl_8055AF0C;
    }
    k = lbl_802F99F8;
    n = lbl_8055EB2C - 1;
    return lbl_8055EB04->views[view].aspect * k.v[n];
}

u32 fn_801E8954(int view) {
    if (view < 0) {
        view = lbl_8055AF0C;
    }
    return lbl_80487D00[view].flags & 1;
}

u32 fn_801E897C(int view) {
    if (view < 0) {
        view = lbl_8055AF0C;
    }
    return (lbl_80487D00[view].flags >> 1) & 1;
}

u32 fn_801E89A4(int view) {
    if (view < 0) {
        view = lbl_8055AF0C;
    }
    return (lbl_8055EB04->views[view].flags >> 2) & 1;
}

u32 fn_801E89C8(int view) {
    if (view < 0) {
        view = lbl_8055AF0C;
    }
    return (lbl_8055EB04->views[view].flags >> 3) & 1;
}

s32 fn_801E89EC(void) { return lbl_8055AF08; }

void fn_801E89F4(s32 v) { lbl_8055AF08 = v; }

void fn_801E89FC(void) {
    lbl_804881D8[0] = fn_80129DC4(0, -1.0f, 1.0f);
    lbl_804881D8[1] = fn_80129DC4(0, -0.33f, 0.33f);
    lbl_804881D8[2] = fn_80129DC4(0, -1.0f, 1.0f);
    PSVECNormalize((WVec*)lbl_804881D8, (WVec*)lbl_804881D8);
    lbl_804881D8[3] = fn_80129DC4(0, 0.0f, 3.0f);
}

static inline f32 Sign(f32 x) { return x > 0.0f ? 1.0f : -1.0f; }

void fn_801E8A84(void) {
    lbl_804881D8[0] += fn_80129DC4(0, -0.05f, 0.05f);
    lbl_804881D8[1] += fn_80129DC4(0, -0.05f, 0.05f);
    lbl_804881D8[2] += fn_80129DC4(0, -0.05f, 0.05f);
    if (lbl_804881D8[1] > 0.33f) {
        f32 e = 0.5f * (lbl_804881D8[1] - 0.33f);
        lbl_804881D8[0] += e * Sign(lbl_804881D8[0]);
        lbl_804881D8[1] = 0.33f;
        lbl_804881D8[2] += e * Sign(lbl_804881D8[2]);
    } else if (lbl_804881D8[1] < -0.33f) {
        f32 e = 0.5f * (-0.33f - lbl_804881D8[1]);
        lbl_804881D8[0] += e * Sign(lbl_804881D8[0]);
        lbl_804881D8[1] = -0.33f;
        lbl_804881D8[2] += e * Sign(lbl_804881D8[2]);
    }
    PSVECNormalize((WVec*)lbl_804881D8, (WVec*)lbl_804881D8);
    lbl_804881D8[3] = Max(Min(lbl_804881D8[3] + fn_80129DC4(0, -0.2f, 0.2f), 3.0f), 0.0f);
}

void fn_801E8C84(WVec* out) {
    f32 k = lbl_804881D8[3];
    out->x = lbl_804881D8[0] * k;
    out->y = lbl_804881D8[1] * k;
    out->z = lbl_804881D8[2] * k;
}

void fn_801E8CB8(WVec* dir, f32* strength) {
    dir->x = lbl_804881D8[0];
    dir->y = lbl_804881D8[1];
    dir->z = lbl_804881D8[2];
    *strength = lbl_804881D8[3];
}

void fn_801E8CE4(void) {
    fn_80297984();
    fn_802889F0();
    if (lbl_8055EB24) {
        lbl_8055EB24();
    }
    fn_80287914();
    fn_801FA1D4();
    fn_802C1A08();
    memset(lbl_80487CE0, 0, sizeof(lbl_80487CE0));
    lbl_8055EB14 = lbl_8055EB18 = 0.0f;
}

void fn_801E8D48(void) {
    WheelSlot w;
    w.tag = 'WHLP';
    w.size = sizeof(WheelSlot);
    w.f8 = 60.0f;
    w.fC = 90.0f;
    w.f10 = 15.0f;
    w.f14 = 15.0f;
    w.f18 = 18.0f;
    w.f1C = 500.0f;
    w.f20 = 0.6f;
    w.f24 = 0.8f;
    fn_801E87D4(new ("wheel") WheelAnim((const char*)&w));
}

void fn_801E8DD4(void) {
    AnimSlot a;
    memset(&a, 0, sizeof(a));
    a.tag = 'Panl';
    a.size = sizeof(a);
    fn_801E87D4(new ("MutableWob") AnimPoint((const char*)&a));
}

void fn_801E8E38(void) {
    MutSlot m;
    m.tag = 'MUTP';
    m.size = sizeof(MutSlot);
    m.f8 = 100.0f;
    m.f14 = 1000.0f;
    m.f10 = 1000.0f;
    m.fC = 1000.0f;
    fn_801E87D4(new ("MutableWob") AnimPoint((const char*)&m));
}

CarTypeMap* fn_801E8EA4(int type) {
    CarTypeMap* m = lbl_80339CC0;
    while (m->tag) {
        if (m->type == type) {
            break;
        }
        m++;
    }
    if (!m->tag) {
        fn_802E317C("world: Can't map CarType %d", type);
    }
    return m;
}

void fn_801E8F10(RaceSetup* s) {
    RaceCars* cars;
    CarDef* d;
    int i;
    fn_8012E81C("hobbyset.res");
    fn_8012E81C("part.res");
    cars = &s->cars;
    fn_802E30C8("+create_cars: num_cars = %d", cars->count);
    d = cars->defs;
    for (i = 0; i < cars->count; i++, d++) {
        AnimObj* m;
        sprintf(lbl_804881E8, "Create car %i", i);
        fn_80127464();
        LoadTimerMark(lbl_804881E8, 500.0f);
        {
            Racer r(*d, i);
            switch (d->type) {
            case 'S':
                m = new ("GhostCarObject") PlayerCarModel(&r, d);
                break;
            default:
                m = new ("CarObject") CarModel(&r, d);
                break;
            }
        }
        fn_801E87D4(m);
    }
}

RaceSetup* fn_801E952C(RaceSetup* s) {
    memset(s, 0, sizeof(*s));
    return s;
}

void fn_801E9564(RaceSetup* s) {
    int i;
    for (i = 0; i < 5; i++) {
        fn_801E8D48();
    }
    fn_80127464();
    fn_801E8F10(s);
    for (i = 0; i < 12; i++) {
        fn_801E8DD4();
    }
    for (i = 0; i < 64; i++) {
        fn_801E8E38();
    }
}

void fn_801E95DC(void) {
    if (lbl_8055AF10 != -1) {
        f32 m[4][3];
        f32 a;
        f32 b;
        m[0][0] = 1.0f;
        m[0][1] = 0.0f;
        m[0][2] = 0.0f;
        m[1][0] = 0.0f;
        m[1][1] = 1.0f;
        m[1][2] = 0.0f;
        m[2][0] = 0.0f;
        m[2][1] = 0.0f;
        m[2][2] = 1.0f;
        m[3][0] = 0.0f;
        m[3][1] = 0.0f;
        m[3][2] = 0.0f;
        a = 1.0f - (f32)fmod(fn_801E7E18(), 1.0);
        b = 0.0f;
        m[3][1] = GetPlayer(fn_801E866C(lbl_8055AF0C))->fov;
        b -= m[3][1] / fn_80203FE0();
        {
            u32 f = fn_801E87A8()->flags;
            int mode = 2;
            if (f & 2) {
                mode = 1;
            }
            fn_802C7D54(lbl_8055AF10, m, mode, a, b, 50.0f);
        }
    }
}

void fn_801E96D8(RaceSetup* s) {
    EventInfo* ev = (EventInfo*)GetEvent(s->event);
    const char* file = ev->node->GetString("wall_file", 0);
    if (file) {
        ValueTable* t = fn_80131808(file);
        int n = 0;
        ValueNode* nd = t->GetChild(0);
        while (nd) {
            BOOL right = strncasecmp(nd->GetName(), "Right", 5) == 0;
            BOOL line;
            strncasecmp(nd->GetName(), "Left", 4);
            line = strstr(nd->GetName(), "line") != 0;
            if (strcmp(nd->GetName(), "sfline") != 0 && nd->Has("bound0")) {
                BOOL done = 0;
                int idx = 1;
                int count = 0;
                WVec prev;
                ValPoint p0 = nd->GetPoint("bound0");
                prev.x = p0.x;
                prev.y = 0.0f;
                prev.z = p0.y;
                prev.y = fn_801DCF48(0, p0.x, p0.y);
                do {
                    char name[0x10];
                    BOOL ok;
                    ValPoint p;
                    fn_801345D8(name, sizeof(name), "bound%d", idx);
                    ok = 1;
                    if (nd->Has(name)) {
                        p = nd->GetPoint(name);
                    } else {
                        if (!line) {
                            p = nd->GetPoint("bound0");
                        } else {
                            ok = 0;
                        }
                        done = 1;
                    }
                    if (ok) {
                        WVec cur;
                        WMtx mtx;
                        WVec c;
                        WVec up;
                        f32 len;
                        WallVol* wv;
                        cur.x = p.x;
                        cur.y = 0.0f;
                        cur.z = p.y;
                        cur.y = fn_801DCF48(0, p.x, p.y);
                        c.x = cur.x + 0.5f * (prev.x - cur.x);
                        c.y = cur.y + 0.5f * (prev.y - cur.y);
                        c.z = cur.z + 0.5f * (prev.z - cur.z);
                        mtx.z.x = cur.x - prev.x;
                        mtx.z.y = 0.0f;
                        mtx.z.z = cur.z - prev.z;
                        mtx.pos = c;
                        len = PSVECMag(&mtx.z) * 0.5f;
                        PSVECNormalize(&mtx.z, &mtx.z);
                        up.x = 0.0f;
                        up.y = 1.0f;
                        up.z = 0.0f;
                        mtx.y = up;
                        PSVECCrossProduct(&mtx.y, &mtx.z, &mtx.x);
                        mtx.pos.y += 125.0f;
                        if (right) {
                            mtx.x.x *= -1.0f;
                            mtx.x.y *= -1.0f;
                            mtx.x.z *= -1.0f;
                            mtx.z.x *= -1.0f;
                            mtx.z.y *= -1.0f;
                            mtx.z.z *= -1.0f;
                        }
                        wv = new ("wallvol") WallVol(0.2f, 375.0f, len);
                        wv->SetType(0x3EA);
                        fn_801FA8E0(wv, &mtx, -1);
                        prev = cur;
                        idx++;
                        count++;
                    }
                    fn_80127464();
                } while (!done);
                fn_802E30C8("Loaded %d walls from \"%s\"", count, nd->GetName());
            }
            nd = t->GetChild(++n);
        }
        fn_80131818(t);
    }
    file = ev->node->GetString("wall_model", 0);
    if (file) {
        lbl_8055AF10 = fn_802C6FC0(file, 0, 0);
    }
}

void fn_801E9BE4(void) {
    if (lbl_8055AF10 != -1) {
        fn_802C6CF8(lbl_8055AF10);
    }
    lbl_8055AF10 = -1;
}
}
