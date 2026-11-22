// Physics objects (0x80177814-0x80179D08): the "phob" list built from the track's init stream (cars, trailers,
// statics, balls, wheels, obstacles, panels, mutables by four-character type), the fixed-rate physics tick
// (tick scheduling against the hires clock, per-frame subsystem updates, force channels), the state packet
// sent to the renderer, the view->car map and the event message buffers. Names are guesses apart from the
// strings.
#include "game/common.h"
#include "game/statepacket.h"
#include "game/phob.h"
#include "game/wheel.h"
#include "game/raceopts.h"
#include <revolution/MTX/vec.h>
#include "game/collvol.h"
#include "game/rigid.h"
#include "game/springcar.h"
#include "game/plainvec.h"
#include "game/mathutil.h"


BOOL OptProfile(void);  // cmdline.cpp

extern "C" {
void* memset(void* dst, int c, u32 n);
void* memcpy(void* dst, const void* src, u32 n);
double sin(double x);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
s32 fn_802E2C20(void);
s64 fn_802E2C9C(void);
void fn_802E2D74(int ms);
void fn_802E8518(int i);
void fn_801DCEC8(int i, f32 v);
void fn_801DCEDC(int i, f32 v);
int fn_801CF064(void);
int fn_8018FD38(void);
int fn_801B526C(void);
void fn_801AD5F0(void);
void fn_80186DE8(void);
void fn_801D0F38(void);
void fn_801B535C(void* list, int n);
void fn_801B50B4(void* list, int n);
void fn_8018A180(void* car);
void fn_80176A6C(void);
void fn_8026C014(int a);
int fn_802D3EBC(void);
void fn_80265D84(void);
void ProcessGreenFlagActivation(void);
void fn_8026B6E4(void);
void fn_801CEDDC(void);
void fn_801D2214(void);
void fn_8020C59C(void);
void fn_801DEFE0(void);
void fn_801EA1F0(void);
void fn_80178DBC(int i, f32 v);
void ProcessAlarmEvents(void);
int fn_801C5B60(void);
void fn_801B841C(void);
void debug_event(void);
void fn_801D47F8(void);
void UpdateAICarStates(void);
void fn_80134B50(void);
void fn_8017FF4C(f32 dt);
void fn_801E22D0(void* list, int n);
void fn_801E1E64(void);
void fn_801B24B8(void);
void fn_80265D88(void);
void fn_801777B4(void);
void fn_801777A8(void);
void fn_80176A34(s32 frame);
void fn_801279D0(void);
void fn_80179D8C(void* p, s32 pos);
int fn_801E2640(const f32* a, const f32* b, f32* hit, f32* n, s32* tri, s32* x, void* list, int count, s32 g);
void fn_801B5318(void* p);
s32 fn_8012F500(s32 s, s32* pos);
s32 fn_8012F450(s32 s);
void fn_8012F550(s32 s, void* dst, s32 n);
void fn_8012F448(s32 s, s32 pos);
s32 fn_8012F3CC(s32 a);
void fn_8012F43C(s32 s);
void fn_8012F418(s32 s);
int fn_80278820(void);
int fn_802787FC(void);
s32 fn_80176A1C(void);
f32 fn_80176A2C(void);
int fn_80265D64(void);
s64 __div2i(s64 a, s64 b);
void fn_8026A248(int car, int view);
void fn_801EAA94(int car, s32 kind);
void fn_80177990(void);
void fn_801D0CF0(void);
s32* fn_801769D8(void);
void fn_80181FEC(void);
void InitializeCrashMode(void);
void fn_801DA6EC(void);
void fn_801D0E0C(void);
void fn_80127464(void);
void fn_801CEFAC(void);
void fn_80129C80(int a, s32 b, s32 c);
int fn_80137C74(void);
void fn_801B5274(void);
void fn_801B5FA0(void);
void fn_801B3448(void);
void fn_801A4E28(void);
void fn_801AFAC8(void);
void fn_801B06C8(void);
void InitializeAlarmEvents(void);
void InitializeDamageInfo(void);
void fn_80181F30(void);
void LoadGarageValues(void);
void InitializeRaceSettings(EventDesc* d);  // deity.cpp
void fn_801C4AD0(void);
void fn_8020C41C(void);
void fn_8026B644(void);
void fn_801C1B9C(void);
void fn_801D21C0(void);
void fn_801D32D0(void);
void fn_801D9284(void);
void InitializeRumbleSound(void);
void fn_801CB2D8(void);
void fn_801DEC18(void);
void CreateBoostManager(void);
void fn_801B8318(void);
void fn_801FA978(void);
void fn_801E2188(void);
void InitializeCornerPool(void);
void fn_801B52CC(void);
void fn_801E21BC(void);
void fn_8017D2CC(int n);
void InitializeCameraManager(int nViews);  // camera manager start-up; retail passes the view count, the callee ignores it
void* fn_801284E8(const char* tag, s32 size);
void fn_801AD5EC(void);
void fn_801B2320(void);
void fn_801C4B10(void);
void fn_801E1D38(void);
void fn_801D47F4(void);
void fn_801B5364(int type, s32 (*fn)(void*));
void fn_80179320(void);
void fn_801B2C6C(void);
void fn_801B5354(void);
void fn_80177A18(void);
void PhysReplayUninstallEventHandler(s32 (*fn)(void*));
void fn_801E1D9C(void);
void CleanupCameraManager(void);
void fn_8017D334(void);
void fn_80128658(void* p);
void fn_801AE50C(void);
void fn_801E222C(void);
void ClearBoostManager(void);
void fn_801DEE3C(void);
void fn_801CB518(void);
void ResetSoundSystem(void);
void fn_801D9828(void);
void fn_801D32DC(void);
void fn_801D2210(void);
void fn_801C1BA0(void);
void fn_8026B69C(void);
void fn_801B228C(void);
void fn_801AFBE8(void);
void fn_801B06CC(void);
void fn_801C4B74(void);
void fn_8020C45C(void);
void fn_801DE0E8(void);
void fn_80181FC4(void);
void fn_801CECB4(void);
void fn_801B8BBC(void);
void fn_801A4E54(void);
void UninstallCollisionSoundHandlers(void);
void fn_801B60A8(void);
void fn_801B531C(void);
void fn_801CEFC4(void);
void fn_80206550(int a);
void fn_801D0E70(void);
void fn_801D0CFC(void);
void fn_801E21B4(void*);  // Group* (physcol.cpp)
int IsReplayActive(void);
void fn_801D9A08(void);
void fn_801B8BE8(void);
void fn_801DEF98(void);
void fn_801B83A8(void);
void fn_801B34A4(void);
void fn_8026B6D0(void);
void fn_802C1928(void);
void fn_80134998(void);
void InitializeDeityTestMode(void);
void fn_801AED74(void);
void fn_801E2294(void);
void fn_801B2910(void);
void InitializeCameraStructures(void);
void fn_80265D60(void);
void InitializeDamageSlots(void);
void fn_801349C4(void);
void fn_801CB320(void);
void fn_801D481C(void);
void fn_801C4BAC(void);
void fn_8020C54C(void);
void fn_801E1E30(void);
void fn_801D0ED4(void);
void fn_802E4C18(int a);
void StartReplayPlayback(void);
void fn_801AED5C(void);
void fn_801CB464(void);
void EndReplayPlayback(void);
void fn_801CB4A0(void);
void fn_801AED68(void);
s32 fn_8026A368(int a);
void UpdateCameraTracking(void* car, int i);
void InitializeRaceState(void* p);
void fn_801CEEC8(void* p);
void fn_801B848C(void* p);
void fn_801E1EA4(void* p);
void* fn_80176C0C(void* dst, const void* src);
}

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

extern s32 lbl_8055E3C0;
extern s32 lbl_8055E3C4;  // frame
extern f32 lbl_8055E3C8;  // time
extern s32 lbl_8055E3CC;
extern u8 lbl_8055E3BD;
extern s32 lbl_8055E488;
extern u32 lbl_8055D430;
extern s32 lbl_8055E8F8;
extern f32 __float_epsilon[];  // FLT_EPSILON

// PMtx: common.h's PlainValMatrix.
struct PhobMsg {
    u32 type;  // offset 0x0
    s32 size;  // offset 0x4
    u8 pad8[0x50 - 0x8];
    s32 f50;  // offset 0x50
};

struct RPacket;
struct PhobCar {
    u8 pad0[0x1E4];
    s32 f1E4;  // offset 0x1E4
};

struct MsgBuf {
    s32 len;          // offset 0x0
    u8 data[0x1000];  // offset 0x4
};

struct PhysClock {
    PhysClock(void) { Reset(); }
    void Reset(void) {
        resync = 1;
        f10 = 0;
        last = 0;
    }
    s32 frame;   // offset 0x0
    s32 f4;      // offset 0x4
    s64 base;    // offset 0x8
    s32 f10;     // offset 0x10
    u8 resync;   // offset 0x14
    s64 last;    // offset 0x18
};

struct HistEntry {
    char name[0x100];  // offset 0x0
    f32 time;          // offset 0x100
    s32 f104;          // offset 0x104
};

struct ViewMsg {
    u32 id;     // offset 0x0
    s32 count;  // offset 0x4
};

struct PhobType {
    u32 type;               // offset 0x0
    s32 size;               // offset 0x4
    Phob* (*create)(void);  // offset 0x8
};

class CenterLine;

// The utility line follower (idealline.cpp, 0x68).
class IdealFollower {
public:
    IdealFollower(int withTimes, CenterLine* line);
    virtual ~IdealFollower(void);
    u8 pad4[0x64];
};

#include "game/netcar.h"

#include "game/phobstatic.h"

struct BodyDesc;

// A rigid body whose shape can be mutated at run time (mutphob.cpp).
class MutPhob : public RigidBody {
public:
    MutPhob(void) {}
    virtual ~MutPhob(void);
    u8 pad1EC[0x29C - 0x1EC];  // MutPhob's own fields (mutphob.cpp)
};

ModuleTag lbl_8055E408;
UnkStaticFFFF lbl_8055E410;
IdealFollower* lbl_8055E414;
s32 lbl_8055E418[2];
s32 lbl_8055E420[2];
s32 lbl_8055E428[2];
s32 lbl_8055E430;  // car count
Phob** lbl_8055E434;
s32 lbl_8055E438;
s32 lbl_8055E43C;
s32 lbl_8055E440;
s32 lbl_8055E444;
s32 lbl_8055E448;
u8 lbl_8055E44C;
StatePacket* lbl_8055E450;
PhobMsg* lbl_8055E454;
s32 lbl_8055E458;
u8 lbl_8055E45C;
u8 lbl_8055E45D;
u8 lbl_8055E45E;
u8 lbl_8055E45F;
f32 lbl_8055E460;
extern "C" {
void fn_80177814(s32* out) {
    static s32 seed;
    s32 t = fn_802E2C20();
    out[0] = (seed + t) % 30000;
    seed += 0x9947;
    t = fn_802E2C20();
    out[1] = (seed + t) % 30000;
    seed += 0x2F1CD;
}
}

s32 lbl_8055E468;

s32 lbl_8055A300 = -1;

s32 lbl_80430590[4];  // view -> car
ViewMsg lbl_804305A0[4];
PhysStats lbl_804305C0;
f32 lbl_804305D4[75];
f32 lbl_80430700[76];
RaceCarRec* lbl_80430830[8];
PhysClock lbl_80430850;
MsgBuf lbl_80430870;
struct MsgBuf2 {
    s32 len;          // offset 0x0
    u8 data[0x1000];  // offset 0x4
    s32 f1004;        // offset 0x1004
};

MsgBuf lbl_80431874;
MsgBuf2 lbl_80432878;
HistEntry lbl_80433880[8];
Phob* lbl_804340C0[0x200];

static inline void Swap(char& a, char& b) {
    char t = a;
    a = b;
    b = t;
}

extern "C" {



void fn_801778C0(void) {
    int i;
    lbl_8055E468 = 0;
    for (i = 0; i < lbl_8055E438; i++) {
        Phob* p = lbl_8055E434[i];
        if (p->vf44() && p->vf40()) {
            lbl_804340C0[lbl_8055E468++] = p;
        }
    }
}

int CollVolume::vf40(void) { return 0; }

int CollVolume::vf44(void) { return 1; }

void fn_80177990(void) {
    int i;
    for (i = 0; i < 75; i++) {
        lbl_80430700[i] = 0.0f;
        lbl_804305D4[i] = 0.0f;
        fn_801DCEC8(i, 0.0f);
        fn_801DCEDC(i, 0.0f);
    }
}

void fn_80177A18(void) {
    int i;
    for (i = 0; i < 4; i++) {
        fn_802E8518(i);
    }
}

void fn_80177A54(void) {
    if (lbl_8055E3C0 == 2 && fn_801CF064() && fn_8018FD38()) {
        lbl_8055E3C0 = 0;
    }
    switch (lbl_8055E3C0) {
    case 1:
        if (!lbl_8055E3BD) {
            fn_80177A18();
        }
        lbl_8055E3BD = 1;
        break;
    case 2:
        lbl_8055E3BD = 0;
        break;
    }
    lbl_8055E3C0 = 0;
}

void fn_80177AE4(int force) {
    if (lbl_80433880[0].name[0]) {
        if (lbl_8055E3C8 > lbl_80433880[0].time + 5.0f || lbl_8055E3C8 < lbl_80433880[0].time ||
            (force && lbl_80433880[7].name[0])) {
            int i;
            for (i = 0; i < 7; i++) {
                lbl_80433880[i] = lbl_80433880[i + 1];
            }
            memset(&lbl_80433880[7], 0, sizeof(HistEntry));
        }
    }
}

void fn_80177CEC(void) {
    Phob* p;
    if (lbl_8055E43C >= lbl_8055E438) {
        lbl_8055E43C = 0;
    }
    p = lbl_8055E434[lbl_8055E43C];
    if (p && p->vf40()) {
        PVec3 z;
        PVec3 y;
        PVec3 x;
        x.Set(p->mtx.basis[0].x, p->mtx.basis[0].y, p->mtx.basis[0].z);
        z.Set(p->mtx.basis[2].x, p->mtx.basis[2].y, p->mtx.basis[2].z);
        PSVECCrossProduct((const Vec*)&z, (const Vec*)&x, (Vec*)&y);
        PSVECNormalize((const Vec*)&x, (Vec*)&x);
        PSVECNormalize((const Vec*)&y, (Vec*)&y);
        PSVECCrossProduct((const Vec*)&x, (const Vec*)&y, (Vec*)&z);
        p->mtx.basis[0] = x;
        p->mtx.basis[1] = y;
        p->mtx.basis[2] = z;
    }
    lbl_8055E43C++;
}

void fn_80177E14(void) {
    int i;
    if (!fn_801B526C()) {
        fn_801AD5F0();
        fn_80186DE8();
        fn_801D0F38();
        for (i = 0; i < lbl_8055E468; i++) {
            lbl_804340C0[i]->Update();
        }
        fn_801B535C(lbl_8055E434, lbl_8055E438);
    } else {
        fn_801B50B4(lbl_8055E434, lbl_8055E438);
    }
    for (i = 0; i < lbl_8055E468; i++) {
        lbl_804340C0[i]->vf20();
    }
}

RaceCarRec* fn_8017960C(int i);

void fn_80177EE4(void) {
    if (lbl_8055E44C) {
        int i;
        for (i = 0; i < 8; i++) {
            RaceCarRec* r = fn_8017960C(i);
            if (r) {
                fn_8018A180(r);
            }
        }
        lbl_8055E44C = 0;
    }
}

/* Helper name unproven. A one-byte flag read through a const by-value formal: the read is wrapped in a
   conversion, so it is loaded into a code-generation temporary and the local that receives it is only a copy
   (retail numbers saved and paused among the temporaries, in statement order: FACTS 2026-11-17, push90-C, "const read = temporary"). */
static inline u8 FlagValue(const u8 f) { return f; }

void fn_80177F40(void) {
    u8 saved;
    int busy;
    u8 paused;
    int net;
    fn_80176A6C();
    fn_80177A54();
    lbl_8055E458 = lbl_8055E3BD ? 0 : lbl_8055E458 + 1;
    fn_8026C014(1);
    lbl_8055E45D = lbl_8055E45C;
    lbl_8055E45F = lbl_8055E45E;
    fn_80177AE4(0);
    saved = FlagValue(lbl_8055E3BD);
    busy = fn_802D3EBC();
    if (busy) {
        lbl_8055E3BD = 1;
    }
    paused = FlagValue(lbl_8055E3BD);
    lbl_80430870.len = 0;
    fn_80265D84();
    ProcessGreenFlagActivation();
    fn_8026B6E4();
    fn_801CEDDC();
    fn_801D2214();
    fn_8020C59C();
    fn_801DEFE0();
    if (paused) {
        fn_801EA1F0();
    }
    net = fn_801B526C();
    if (net || !paused) {
        for (int i = 0; i < 75; i++) {
            fn_801DCEDC(i, 59.94f * (lbl_804305D4[i] - lbl_80430700[i]));
            lbl_80430700[i] = lbl_804305D4[i];
        }
        s32 k = lbl_8055A300;
        if (k >= 0 && k < 75) {
            f32 now = lbl_8055E3C8;
            f32 sn = sin(6.2831855f * (0.25f * now));
            fn_80178DBC(k, 0.5f + 0.5f * sn);
        }
        ProcessAlarmEvents();
        if (!fn_801C5B60()) {
            fn_801B841C();
        }
        debug_event();
        fn_801D47F8();
        fn_801EA1F0();
        if (!net) {
            if (!fn_801C5B60()) {
                UpdateAICarStates();
            }
        } else {
            fn_80134B50();
        }
        fn_80177CEC();
        lbl_80432878.len = 0;
        fn_80177EE4();
        fn_8017FF4C(0.016683351f);
        fn_801778C0();
        if (!net) {
            fn_801E22D0(lbl_804340C0, lbl_8055E468);
        }
        fn_80177E14();
        for (int i = 0; i < lbl_8055E438; i++) {
            PhobCar* c = lbl_8055E434[i]->vf3C();
            if (c) {
                c->f1E4 = 0;
            }
        }
        lbl_8055E488 = 0;
        fn_801E1E64();
    } else {
        fn_8017FF4C(0.016683351f);
    }
    fn_801B24B8();
    fn_80265D88();
    if (!paused) {
        fn_801777B4();
        fn_80176A34(lbl_8055E3C4 + 1);
    }
    fn_801279D0();
    if (busy) {
        lbl_8055E3BD = saved;
    }
}

RigidBody* CollVolume::vf3C(void) { return 0; }

int fn_801781C0(s32 x) {
    switch (x) {
    case 0x13:
    case 0x15:
    case 0x18:
    case 0x1A:
    case 0x1F:
        return 1;
    }
    return 0;
}

char* fn_801781F4(u32 v) {
    static char text[4];
    *(u32*)text = v;
    Swap(text[0], text[3]);
    Swap(text[1], text[2]);
    return text;
}

Phob* fn_80178224(void) { return (Phob*)new ("NetCar") NetCar; }

Phob* fn_80178258(void) { return (Phob*)new ("PhobStatic") PhobStatic; }

Phob* fn_80178290(void) { return (Phob*)new ("WheelPhob") Wheel; }

Phob* fn_801782C8(void) { return (Phob*)new ("MutablePhob") MutPhob; }

Phob* fn_8018FB18(void);
Phob* fn_80134F48(void);
Phob* fn_801B8D80(void);
Phob* fn_8017D688(void);
Phob* fn_801A9660(void);
Phob* fn_801BF214(void);

PhobType lbl_80331950[] = {
    {'CAR ', 0xFA8, 0},
    {'PCAR', 0xFA8, fn_8018FB18},
    {'ACAR', 0xFA8, fn_80134F48},
    {'NCAR', 0xFA8, fn_80178224},
    {'TRLR', 0xFA8, fn_801B8D80},
    {'STAT', 0, fn_80178258},
    {'BALL', 0x34, fn_8017D688},
    {'WHLP', 0x44, fn_80178290},
    {'OBST', 0x34, fn_801A9660},
    {'Panl', 0x40, fn_801BF214},
    {'FGPL', 0x34, 0},
    {'MUTP', 0x40, fn_801782C8},
    {0, 0, 0},
};

PhobType* fn_80178374(u32 type) {
    PhobType* t;
    for (t = lbl_80331950; t->type; t++) {
        if (t->type == type) {
            return t;
        }
    }
    return 0;
}

Phob* fn_801783A8(PhobMsg* m, s32 pos) {
    Phob* p = 0;
    PhobType* t;
    lbl_8055E454 = m;
    t = fn_80178374(m->type);
    if (t) {
        if (t->create) {
            p = t->create();
            if (p) {
                fn_80179D8C(p, pos);
                p->Init(m);
            } else {
                fn_802E317C("Can't create phobtype %s", fn_801781F4(m->type));
            }
        } else {
            fn_802E317C("Creating phobtype %s is forbidden", fn_801781F4(m->type));
        }
    } else {
        fn_802E317C("Can't find phobtype %s", fn_801781F4(m->type));
    }
    lbl_8055E454 = 0;
    return p;
}

int fn_801784AC(const f32* a, const f32* b, f32* hit, f32* n, s32* tri, s32* x, s32 g) {
    return fn_801E2640(a, b, hit, n, tri, x, lbl_804340C0, lbl_8055E468, g);
}

static inline void Set3(f32* v, f32 x, f32 y, f32 z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
}

int fn_801784DC(const f32* pos, f32* outY, f32* outN, s32* outTri, s32 x) {
    f32 below[3];
    f32 hit[3];
    f32 n[3];
    s32 tri;
    s32 tmp;
    Set3(below, pos[0], pos[1] - 1000.0f, pos[2]);
    if (fn_801784AC(pos, below, hit, n, &tri, &tmp, x)) {
        *outY = hit[1];
        if (outN) {
            outN[0] = n[0];
            outN[1] = n[1];
            outN[2] = n[2];
        }
        if (outTri) {
            *outTri = tri;
        }
        return 1;
    }
    return 0;
}

void fn_801785A4(Phob* p) {
    lbl_8055E434[lbl_8055E438++] = p;
    if (p->vf40()) {
        fn_801B5318(p);
    }
}

int fn_80178604(PhobMsg* m) {
    int car = 0;
    u32 t = m->type;
    if (t == 'CAR ' || t == 'PCAR' || t == 'ACAR' || t == 'NCAR') {
        car = 1;
    }
    if (car) {
        return 0;
    }
    if (t == 'OBST' && m->f50 == 0x14) {
        return 2;
    }
    return 1;
}

int fn_80178678(s32 s, Phob** out, int pass) {
    s32 pos;
    PhobMsg* m = 0;
    s32 at;
    Phob* r;
    fn_8012F500(s, &pos);
    if (pos < 0) {
        return 0;
    }
    at = fn_8012F450(s);
    fn_8012F550(s, &m, 8);
    fn_8012F448(s, at);
    fn_8012F550(s, &m, m->size);
    if (pass == fn_80178604(m)) {
        r = fn_801783A8(m, pos);
    } else {
        r = 0;
    }
    *out = r;
    return 1;
}

void fn_8017873C(MsgBuf* b, u8 type, const void* data, u32 n) {
    if (n + b->len + 1 < 0x1000) {
        b->data[b->len] = type;
        memcpy(b->data + b->len + 1, data, n);
        b->len = n + b->len + 1;
    }
}

s64 fn_80178BBC(PhysClock* c);

int fn_801787B4(PhysClock* c) {
    int n;
    int fixed;
    s32 mode;
    if (OptProfile()) {
        return 1;
    }
    if (fn_80278820()) {
        return 10;
    }
    if (fn_802787FC()) {
        return 1;
    }
    if (fn_80176A1C() == 2) {
        f32 speed = fn_80176A2C();
        s64 now = fn_802E2C9C();
        s32 d = (s32)(now - c->last);
        f32 tick;
        int ti;
        d = d > 0 ? d : 0;
        d = d < 50000 ? d : 50000;
        tick = 16683.0f * speed;
        c->last = now;
        ti = (int)(tick < 0.0f ? tick - 0.5f : 0.5f + tick);
        if (!lbl_8055E3BD) {
            lbl_8055E460 += (f32)d / (f32)(ti < 1 ? 1 : ti);
        }
        {
            f32 acc = lbl_8055E460;
            n = (int)acc;
            lbl_8055E460 = acc - (f32)n;
        }
        c->resync = 1;
        return lbl_8055E3BD ? 1 : n;
    }
    lbl_8055E460 = 1.0f;
    {
        int skip = (lbl_8055D430 & 1) && (lbl_8055D430 & 0x600);
        if (skip) {
            c->resync = 1;
            return 3;
        }
    }
    n = 1;
    mode = fn_80176A1C();
    fixed = 1;
    if (!lbl_8055E3BD && mode == 1) {
        fixed = 0;
    }
    if (mode == 0) {
        s32 f = lbl_8055E3C4;
        n = 10;
        if ((u8)f < 0x80 && (u8)(f - 10) > 0x80) {
            fn_802E2D74(20);
        }
    }
    if (!fixed) {
        if (c->resync) {
            c->frame = lbl_8055E3C4;
            c->base = (fn_802E2C9C() + 0x2000) & ~(s64)0x3FFF;
            c->resync = 0;
        } else {
            n = ((s32)fn_80178BBC(c) + 8341) / 16683;
            if (fn_801B526C()) {
                if ((n < 0 ? -n : n) > 10) {
                    c->resync = 1;
                }
                n = n > 0 ? n : 0;
                n = n < 4 ? n : 4;
            } else if (fn_80265D64()) {
                if (n > 12) {
                    n = 12;
                }
            } else {
                if (n > 8) {
                    c->resync = 1;
                }
                if (n < -8) {
                    if (n < -16) {
                        fn_802E30C8("phystask: hey, I needed to do %d ticks. ", n);
                        fn_802E30C8("phystask: deviation is %I64d usec", fn_80178BBC(c));
                        fn_802E30C8("phystask: tick->usec is %I64d usec",
                                    c->base + (s64)(lbl_8055E3C4 - c->frame) * 16683);
                        fn_802E30C8("phystask: hires time is %I64d usec", fn_802E2C9C());
                    }
                    c->resync = 1;
                }
                n = n > 0 ? n : 0;
                n = n < 8 ? n : 8;
            }
        }
    } else {
        c->resync = 1;
    }
    return n;
}

s64 fn_80178BBC(PhysClock* c) {
    s64 e = c->base + (s64)(lbl_8055E3C4 - c->frame) * 16683;
    return fn_802E2C9C() - e;
}

s32 fn_80178C1C(PhysClock* c) { return fn_80178BBC(c) / 16683; }

void fn_80179758(const char* fmt, ...);

s32 fn_80178C4C(u16* m) {
    fn_80179758((const char*)(m + 2));
    return m[0] + 4;
}

// CollShape::vf4C (slot 0x4C of the sphcoll shape tables), by its linker name. Empty in retail; the dead debug
// format string below is all that is left of its body (it was labelled PhysTaskDumpCars after that string).
static char kPhysTaskDumpCarsFormat[] = "PhysTaskDumpCars: %2d - (%3.2f, %3.2f, %3.2f) tri %d";
extern "C" void vf4C__9CollShapeFv(void) {}

void CollVolume::Notify(const RVec3* v, const RVec3* at, int kind, void* c, void* by) {}  // empty; in every collision vtable

PhysStats* fn_80178C90(void) { return &lbl_804305C0; }

IdealFollower* fn_80178C9C(void) { return lbl_8055E414; }

s32 fn_80178CA4(void) { return lbl_8055E444; }

void fn_80178CAC(int a, int b) {
    s32 ma;
    s32 mb;
    Controller* xa;
    int i;
    Controller* xb;
    s32* slot;
    RaceCarRec* ra;
    RaceCarRec* rb;
    s32 ka;
    s32 kb;
    slot = lbl_80430590;
    for (i = 0; i < 4; i++, slot++) {
        if (a == *slot) {
            *slot = b;
            fn_8026A248(b, i);
        } else if (b == *slot) {
            *slot = a;
            fn_8026A248(a, i);
        }
    }
    ra = fn_8017960C(a);
    rb = fn_8017960C(b);
    ka = ra->f1F0;
    kb = rb->f1F0;
    ma = ra->type;
    mb = rb->type;
    xb = ((SpringCar*)rb)->ctl;
    xa = ((SpringCar*)ra)->ctl;
    ((SpringCar*)ra)->SetController(0);
    ((SpringCar*)rb)->SetController(0);
    ((SpringCar*)ra)->SetController(xb);
    ((SpringCar*)rb)->SetController(xa);
    ra->f1F0 = kb;
    rb->f1F0 = ka;
    ra->type = mb;
    rb->type = ma;
    fn_801EAA94(a, kb);
    fn_801EAA94(b, ka);
}

void fn_80178DBC(int i, f32 v) {
    lbl_804305D4[i] = v;
    fn_801DCEC8(i, v);
}

void fn_80178DD0(void) { fn_80177990(); }

static inline void Seeds(void) {
    fn_80129C80(4, lbl_8055E418[0], lbl_8055E418[1]);
    fn_80129C80(3, lbl_8055E420[0], lbl_8055E420[1]);
    fn_80129C80(5, lbl_8055E428[0], lbl_8055E428[1]);
}

void fn_80178DD4(s32 stream) {
    int i;
    s32 s;
    int pass;
    fn_801D0CF0();
    lbl_8055E8F8 = *fn_801769D8();
    fn_80181FEC();
    InitializeCrashMode();
    fn_801DA6EC();
    fn_801D0E0C();
    fn_80127464();
    fn_801CEFAC();
    fn_80177814(lbl_8055E418);
    fn_80177814(lbl_8055E420);
    fn_80177814(lbl_8055E428);
    lbl_8055E414 = new ("util_centerline") IdealFollower(0, 0);
    memset(lbl_80433880, 0, sizeof(lbl_80433880));
    Seeds();
    for (i = 0; i < 4; i++) {
        lbl_80430590[i] = -1;
        memset(&lbl_804305A0[i], 0, sizeof(ViewMsg));
    }
    lbl_8055E430 = fn_80137C74();
    lbl_8055E43C = 0;
    fn_80127464();
    fn_801B5274();
    fn_801B5FA0();
    fn_801B3448();
    fn_801A4E28();
    fn_801AFAC8();
    fn_801B06C8();
    InitializeAlarmEvents();
    InitializeDamageInfo();
    fn_80181F30();
    LoadGarageValues();
    InitializeRaceSettings(fn_801769CC());
    fn_80127464();
    fn_801C4AD0();
    fn_8020C41C();
    fn_8026B644();
    fn_801C1B9C();
    fn_801D21C0();
    fn_801D32D0();
    fn_801D9284();
    InitializeRumbleSound();
    fn_801CB2D8();
    fn_801DEC18();
    fn_80127464();
    CreateBoostManager();
    fn_801B8318();
    fn_801FA978();
    lbl_8055E434 = new ("phob_list") Phob*[0x200];
    fn_801E2188();
    for (i = 0; i < 8; i++) {
        lbl_80430830[i] = 0;
    }
    lbl_8055E45E = 0;
    lbl_8055E45F = 0;
    lbl_8055E45C = 0;
    lbl_8055E45D = 0;
    lbl_8055E438 = 0;
    lbl_80432878.len = 0;
    lbl_80430870.len = 0;
    lbl_80431874.len = 0;
    s = fn_8012F3CC(stream);
    InitializeCornerPool();
    fn_80127464();
    for (pass = 0; pass < 3; pass++) {
        Phob* p;
        fn_8012F43C(s);
        while (fn_80178678(s, &p, pass)) {
            if (p) {
                fn_801785A4(p);
            }
        }
    }
    fn_80127464();
    fn_8012F418(s);
    fn_801B52CC();
    fn_801E21BC();
    fn_80127464();
    fn_8017D2CC(0x200);
    int n = lbl_8055E430;
    lbl_8055E448 = 0;
    lbl_8055E444 = lbl_8055E440 = n > 1 ? n : 1;
    InitializeCameraManager(lbl_8055E444);
    lbl_8055E450 = (StatePacket*)fn_801284E8("PhysMessage", lbl_8055E3CC + 0x2EE8);
    fn_801AD5EC();
    fn_80127464();
    fn_801B2320();
    fn_801C4B10();
    lbl_80430850.Reset();
    fn_801E1D38();
    fn_801D47F4();
    fn_801B5364(9, (s32 (*)(void*))fn_80178C4C);
    Seeds();
    fn_80127464();
    fn_80179320();
}

void fn_80179110(void) {
    fn_801B2C6C();
    fn_801B5354();
    lbl_8055E448++;
}

void fn_80179140(void) {
    int i;
    for (i = 0; i < lbl_8055E438; i++) {
        lbl_8055E434[i]->vf14();
    }
}

void CollVolume::vf14(void) {}  // empty; in every collision vtable

void fn_801791A8(void) {
    int i;
    fn_80177A18();
    PhysReplayUninstallEventHandler((s32 (*)(void*))fn_80178C4C);
    fn_801E1D9C();
    CleanupCameraManager();
    fn_8017D334();
    fn_80128658(lbl_8055E450);
    lbl_8055E450 = 0;
    for (i = 0; i < lbl_8055E438; i++) {
        delete lbl_8055E434[i];
    }
    delete[] lbl_8055E434;
    lbl_8055E434 = 0;
    fn_801AE50C();
    fn_801E222C();
    ClearBoostManager();
    fn_801DEE3C();
    fn_801CB518();
    ResetSoundSystem();
    fn_801D9828();
    fn_801D32DC();
    fn_801D2210();
    fn_801C1BA0();
    fn_8026B69C();
    fn_801B228C();
    fn_801AFBE8();
    fn_801B06CC();
    fn_801C4B74();
    fn_8020C45C();
    fn_801DE0E8();
    fn_80181FC4();
    fn_801CECB4();
    fn_801B8BBC();
    fn_801A4E54();
    UninstallCollisionSoundHandlers();
    fn_801B60A8();
    fn_801B531C();
    delete lbl_8055E414;
    lbl_8055E414 = 0;
    fn_801CEFC4();
    fn_80206550(1);
    fn_801D0E70();
    lbl_8055E8F8 = 0;
    fn_801D0CFC();
}

void fn_801792F4(void* g) { fn_801E21B4(g); }

PhobMsg* fn_801792F8(void) { return lbl_8055E454; }

s32 fn_80179300(void) { return lbl_8055E458; }

void fn_80179308(void) { lbl_8055E448++; }

s32 fn_80179318(void) { return lbl_8055E448; }

void fn_80179320(void) {
    int i;
    lbl_8055E43C = 0;
    lbl_8055E460 = 1.0f;
    lbl_8055E44C = 0;
    lbl_8055E458 = 0;
    fn_80177990();
    if (IsReplayActive()) {
        fn_80177814(lbl_8055E418);
        fn_80177814(lbl_8055E420);
        fn_80177814(lbl_8055E428);
    }
    Seeds();
    fn_80176A34(0);
    fn_801D9A08();
    fn_801B8BE8();
    fn_801DEF98();
    fn_801B83A8();
    fn_801B34A4();
    fn_8026B6D0();
    fn_802C1928();
    fn_80134998();
    InitializeDeityTestMode();
    fn_801AED74();
    fn_801E2294();
    for (i = 0; i < lbl_8055E438; i++) {
        lbl_8055E434[i]->Reset();
    }
    fn_801B2910();
    InitializeCameraStructures();
    if (!fn_801CF064()) {
        for (i = 0; i < lbl_8055E430; i++) {
            fn_8026A248(lbl_80430590[i], i);
        }
    }
    for (i = 0; i < 4; i++) {
        memset(&lbl_804305A0[i], 0, sizeof(ViewMsg));
    }
    for (i = 0; i < 8; i++) {
        lbl_80433880[i].name[0] = 0;
    }
    fn_80265D60();
    InitializeDamageSlots();
    if (!fn_80265D64()) {
        lbl_80430850.Reset();
    }
    lbl_8055E448++;
    fn_801349C4();
    fn_801CB320();
    fn_801D481C();
    fn_801C4BAC();
    fn_8020C54C();
    fn_801E1E30();
    fn_801D0ED4();
    fn_801777A8();
}

void fn_8017952C(void) {
    int n;
    int i;
    fn_802E4C18(1);
    n = fn_801787B4(&lbl_80430850);
    for (i = 0; i < n; i++) {
        fn_80177F40();
    }
}

void fn_80179588(void) {
    StartReplayPlayback();
    fn_801AED5C();
    fn_801CB464();
}

void fn_801795B0(void) {
    EndReplayPlayback();
    fn_801CB4A0();
    fn_801AED68();
}

void fn_801795D8(RaceCarRec* r, int i) {
    lbl_80430830[i] = r;
    if (r->f1F0 == 0x50) {
        lbl_80430590[r->ident.b6C & 3] = i;
    }
}

RaceCarRec* fn_8017960C(int i) {
    if (i < 8) {
        return lbl_80430830[i];
    }
    return 0;
}

s32 fn_80179630(int i) {
    if (i < 0) {
        i = 0;
    }
    return lbl_80430590[i];
}

s32 fn_80179650(void) { return lbl_8055E430; }

s32 fn_80179658(u32 type) {
    PhobType* t = fn_80178374(type);
    if (t) {
        return t->size;
    }
    fn_802E317C("Can't find message size for type %s", fn_801781F4(type));
    return -1;
}

int fn_801796B4(void) { return lbl_8055E45D; }

void fn_801796BC(u8 v) { lbl_8055E45E = v; }

int fn_801796C4(void) { return lbl_8055E45F; }

void fn_801796CC(u8 type, const void* data, u32 n) { fn_8017873C(&lbl_80430870, type, data, n); }

void fn_801796EC(u8 type, const void* data, u32 n) { fn_8017873C(&lbl_80431874, type, data, n); }

f32 fn_8017970C(void) {
    const char* asserts[] = {"ConvertPTimeToPhysicsTime: bad approximation", "ConvertPhysicsTickToPTime: bad approximation",
                             "ConvertPTimeToPhysicsTick: bad approximation"};
    return 0.016683351f * (f32)fn_80178C1C(&lbl_80430850);
}

void fn_80179758(const char* fmt, ...) {}

void fn_801797A8(const char* fmt, ...) {}

void fn_801797F8(int i, u32 hi, u32 lo) {
    lbl_804305A0[i].id = (hi << 16) | (u16)lo;
    lbl_804305A0[i].count++;
}

StatePacket* fn_80179824(void) {
    StatePacket* p = lbl_8055E450;
    u8* buf;
    int i;
    p->serial = lbl_8055E448;
    buf = p->phobs;
    p->nViews = lbl_8055E440;
    p->views[0] = lbl_80430590[0];
    p->views[1] = lbl_80430590[1];
    p->views[2] = lbl_80430590[2];
    p->views[3] = lbl_80430590[3];
    if (!IsReplayActive() || fn_801781C0(fn_8026A368(0)) || fn_801B526C()) {
        p->nViews = 1;
    }
    lbl_8055E444 = p->nViews;
    for (i = 0; i < lbl_8055E440; i++) {
        CarStateOut* c = &p->cars[i];
        UpdateCameraTracking(c, i);
        c->id = lbl_804305A0[i].id;
        c->count = lbl_804305A0[i].count;
    }
    fn_80206550(lbl_8055E444);
    f32 now = lbl_8055E3C8;
    p->time = 0.016683351f * (lbl_8055E460 - 1.0f) + now;
    for (i = 0; i < lbl_8055E438; i++) {
        lbl_8055E434[i]->vf28(buf);
    }
    InitializeRaceState(p);
    fn_801CEEC8(p);
    fn_801B848C(p);
    p->stats = lbl_804305C0;
    fn_801E1EA4(p);
    for (i = 0; i < 75; i++) {
        p->forces[i] = lbl_804305D4[i];
    }
    fn_80176C0C(p->msgs, &lbl_80430870);
    fn_80176C0C(p->events, &lbl_80431874);
    lbl_80431874.len = 0;
    return p;
}

// A volume's draw state in the frame buffer (CollVolume::vf28 writes it). Name unproven.
struct PhobPose {
    PlainValMatrix mtx;  // offset 0x0
    s16 hint;            // offset 0x30
};

/* Helper name unproven: same as ball.cpp's LerpMtx */
static inline void LerpMtx(PlainValMatrix* out, const PlainValMatrix* a, const PlainValMatrix* b, f32 t) {
    f32* d = (f32*)out;
    const f32* x = (const f32*)a;
    const f32* y = (const f32*)b;
    for (int i = 0; i < 12; i++) {
        *d++ = LerpT(t, *x++, *y++);
    }
}

// CollVolume::vf28, by its linker name (the class is named Phob here; port: CollVolume::vf28). The blend is the
// same 12-float LerpMtx loop as Ball::vf28 (exact sibling); with it the old const-pointer crutch is gone.
void vf28__10CollVolumeFPUc(Phob* v, u8* buf) {  // writes the draw state
    u8* o = buf + v->a8;
    PlainValMatrix* out = (PlainValMatrix*)o;
    if ((f32)__fabs(lbl_8055E460 - 1.0f) <= __float_epsilon[0]) {
        *out = v->world;
    } else {
        PlainValMatrix t;
        LerpMtx(&t, &v->prev, &v->world, lbl_8055E460);
        *out = t;
    }
    ((PhobPose*)o)->hint = v->hint[0];
}

u8* fn_80179CB0(void) { return lbl_8055E450->phobs; }

}  // extern "C"
