// Physics objects (0x80177814-0x80179D08): the "phob" list built from the track's init stream (cars, trailers,
// statics, balls, wheels, obstacles, panels, mutables by four-character type), the fixed-rate physics tick
// (tick scheduling against the hires clock, per-frame subsystem updates, force channels), the state packet
// sent to the renderer, the view->car map and the event message buffers. Names are guesses apart from the
// strings.
#include "game/common.h"

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
void PSVECNormalize(const void* src, void* dst);
void PSVECCrossProduct(const void* a, const void* b, void* out);
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
void fn_801B2364(void);
void fn_8026B6E4(void);
void fn_801CEDDC(void);
void fn_801D2214(void);
void fn_8020C59C(void);
void fn_801DEFE0(void);
void fn_801EA1F0(void);
void fn_80178DBC(int i, f32 v);
void fn_801B8BF4(void);
int fn_801C5B60(void);
void fn_801B841C(void);
void fn_801C4CE8(void);
void fn_801D47F8(void);
void fn_80134A8C(void);
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
int OptProfile__Fv(void);
int fn_80278820(void);
int fn_802787FC(void);
s32 fn_80176A1C(void);
f32 fn_80176A2C(void);
int fn_80265D64(void);
s64 __div2i(s64 a, s64 b);
void fn_8026A248(int car, int view);
void fn_801A5300(void* car, s32 x);
void fn_801EAA94(int car, s32 kind);
void fn_80177990(void);
void fn_801D0CF0(void);
s32* fn_801769D8(void);
void fn_80181FEC(void);
void fn_801DF6FC(void);
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
void fn_801B8B84(void);
void fn_801CEC80(void);
void fn_80181F30(void);
void fn_801DE0A4(void);
void* fn_801769CC(void);
void fn_801B2024(void);
void fn_801C4AD0(void);
void fn_8020C41C(void);
void fn_8026B644(void);
void fn_801C1B9C(void);
void fn_801D21C0(void);
void fn_801D32D0(void);
void fn_801D9284(void);
void fn_801D4734(void);
void fn_801CB2D8(void);
void fn_801DEC18(void);
void fn_801E0710(void);
void fn_801B8318(void);
void fn_801FA978(void);
void fn_801E2188(void);
void fn_801AE4C4(void);
void fn_801B52CC(void);
void fn_801E21BC(void);
void fn_8017D2CC(int n);
void fn_80267420(void);
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
void fn_801B537C(s32 (*fn)(void*));
void fn_801E1D9C(void);
void fn_8026785C(void);
void fn_8017D334(void);
void fn_80128658(void* p);
void fn_801AE50C(void);
void fn_801E222C(void);
void fn_801E1520(void);
void fn_801DEE3C(void);
void fn_801CB518(void);
void fn_801D483C(void);
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
void fn_801B3B28(void);
void fn_801B60A8(void);
void fn_801B531C(void);
void fn_801CEFC4(void);
void fn_80206550(int a);
void fn_801D0E70(void);
void fn_801D0CFC(void);
void fn_801E21B4(void);
int fn_801CF050(void);
void fn_801D9A08(void);
void fn_801B8BE8(void);
void fn_801DEF98(void);
void fn_801B83A8(void);
void fn_801B34A4(void);
void fn_8026B6D0(void);
void fn_802C1928(void);
void fn_80134998(void);
void fn_801B265C(void);
void fn_801AED74(void);
void fn_801E2294(void);
void fn_801B2910(void);
void fn_80267680(void);
void fn_80265D60(void);
void fn_801CECE0(void);
void fn_801349C4(void);
void fn_801CB320(void);
void fn_801D481C(void);
void fn_801C4BAC(void);
void fn_8020C54C(void);
void fn_801E1E30(void);
void fn_801D0ED4(void);
void fn_802E4C18(int a);
void fn_801CEFE8(void);
void fn_801AED5C(void);
void fn_801CB464(void);
void fn_801CF01C(void);
void fn_801CB4A0(void);
void fn_801AED68(void);
s32 fn_8026A368(int a);
void fn_802678E8(void* car, int i);
void fn_801B2D28(void* p);
void fn_801CEEC8(void* p);
void fn_801B848C(void* p);
void fn_801E1EA4(void* p);
void* fn_80176C0C(void* dst, const void* src);
void fn_8017B0B0(void* p);
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
extern f32 lbl_80558DC4[];  // FLT_EPSILON
extern void* lbl_80338400[];

struct PVec3 {
    f32 x;
    f32 y;
    f32 z;
};

struct PMtx {
    PVec3 rot[3];  // offset 0x0
    PVec3 pos;     // offset 0x24
};

struct PhobMsg {
    u32 type;  // offset 0x0
    s32 size;  // offset 0x4
    u8 pad8[0x50 - 0x8];
    s32 f50;  // offset 0x50
};

class Phob {
public:
    virtual void Init(PhobMsg* m);    // 0x8
    virtual ~Phob(void);              // 0xC
    virtual void Reset(void);         // 0x10
    virtual void vf14(void);          // 0x14
    virtual void vf18(void);          // 0x18
    virtual void vf1C(void);          // 0x1C
    virtual void vf20(void);          // 0x20
    virtual void vf24(void);          // 0x24
    virtual void vf28(u8* buf);       // 0x28
    virtual s32 vf2C(void);           // 0x2C
    virtual void vf30(void);          // 0x30
    virtual void vf34(void);          // 0x34
    virtual void vf38(void);          // 0x38
    virtual struct PhobCar* vf3C(void);  // 0x3C
    virtual int vf40(void);           // 0x40
    virtual int vf44(void);           // 0x44
    s32 a4;          // offset 0x4
    s32 a8;          // offset 0x8
    u8 padC[0x3C - 0xC];
    PMtx mtx;        // offset 0x3C
    s16 hint[2];     // offset 0x6C
    PMtx world;      // offset 0x70
    PMtx prev;       // offset 0xA0
};

struct PhobCar {
    u8 pad0[0x1E4];
    s32 f1E4;  // offset 0x1E4
};

struct RaceCarRec {
    u8 pad0[0x10];
    s32 model;  // offset 0x10
    u8 pad14[0x1F0 - 0x14];
    s32 kind;  // offset 0x1F0
    u8 pad1F4[0x260 - 0x1F4];
    u8 slot;  // offset 0x260
    u8 pad261[0x270C - 0x261];
    s32 f270C;  // offset 0x270C
};

struct CarStateOut {
    u8 pad0[0x6C];
    s32 count;  // offset 0x6C
    u32 id;     // offset 0x70
    u8 pad74[0xE0 - 0x74];
};

struct PhysStats {
    s32 a;  // offset 0x0
    s32 b;  // offset 0x4
    f32 c;  // offset 0x8
    f32 d;  // offset 0xC
    f32 e;  // offset 0x10
};

struct StatePacket {
    f32 time;                  // offset 0x0
    CarStateOut cars[4];       // offset 0x4
    s32 views[4];              // offset 0x384
    s32 nViews;                // offset 0x394
    s32 serial;                // offset 0x398
    u8 pad39C[0x57C - 0x39C];
    PhysStats stats;           // offset 0x57C
    u8 pad590[0xDB4 - 0x590];
    f32 forces[75];            // offset 0xDB4
    u8 msgs[0x1004];           // offset 0xEE0
    u8 events[0x1004];         // offset 0x1EE4
    u8 phobs[1];               // offset 0x2EE8
};

struct MsgBuf {
    s32 len;          // offset 0x0
    u8 data[0x1000];  // offset 0x4
};

struct PhysClock {
    PhysClock(void) { Reset(); }
    void Reset(void) {
        f10 = 0;
        resync = 1;
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

class CenterLine {
public:
    CenterLine(void* a, void* b);
    virtual ~CenterLine(void);
    u8 pad4[0x64];
};

class NetCar {
public:
    NetCar(void);
    u8 pad0[0x2780];
};

class StaticPhob {
public:
    StaticPhob(void);
    u8 pad0[0xD0];
};

class WheelPhob {
public:
    WheelPhob(void);
    u8 pad0[0x1FC];
};

ModuleTag lbl_8055E408;
UnkStaticFFFF lbl_8055E410;
CenterLine* lbl_8055E414;
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
s32 lbl_8055E464;
s32 lbl_8055E468;
char lbl_8055E46C[4];

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

void fn_80177814(s32* out) {
    s32 t = fn_802E2C20();
    s32 s = lbl_8055E464;
    out[0] = (s + t) % 30000;
    lbl_8055E464 = s + 0x9947;
    t = fn_802E2C20();
    out[1] = (lbl_8055E464 + t) % 30000;
    lbl_8055E464 = lbl_8055E464 + 0x2F1CD;
}

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

int fn_80177980(Phob* p) { return 0; }

int fn_80177988(Phob* p) { return 1; }

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
        PVec3 x;
        PVec3 y;
        PVec3 z;
        x = p->mtx.rot[0];
        z = p->mtx.rot[2];
        PSVECCrossProduct(&z, &x, &y);
        PSVECNormalize(&x, &x);
        PSVECNormalize(&y, &y);
        PSVECCrossProduct(&x, &y, &z);
        p->mtx.rot[0] = x;
        p->mtx.rot[1] = y;
        p->mtx.rot[2] = z;
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
            lbl_804340C0[i]->vf1C();
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
    saved = lbl_8055E3BD;
    busy = fn_802D3EBC();
    if (busy) {
        lbl_8055E3BD = 1;
    }
    paused = lbl_8055E3BD;
    lbl_80430870.len = 0;
    fn_80265D84();
    fn_801B2364();
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
        int i;
        s32 k;
        for (i = 0; i < 75; i++) {
            fn_801DCEDC(i, 59.94f * (lbl_804305D4[i] - lbl_80430700[i]));
            lbl_80430700[i] = lbl_804305D4[i];
        }
        k = lbl_8055A300;
        if (k >= 0 && k < 75) {
            f32 sn = sin(6.2831855f * (0.25f * lbl_8055E3C8));
            fn_80178DBC(k, 0.5f + 0.5f * sn);
        }
        fn_801B8BF4();
        if (!fn_801C5B60()) {
            fn_801B841C();
        }
        fn_801C4CE8();
        fn_801D47F8();
        fn_801EA1F0();
        if (!net) {
            if (!fn_801C5B60()) {
                fn_80134A8C();
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
        for (i = 0; i < lbl_8055E438; i++) {
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

PhobCar* fn_801781B8(Phob* p) { return 0; }

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
    *(u32*)lbl_8055E46C = v;
    Swap(lbl_8055E46C[0], lbl_8055E46C[3]);
    Swap(lbl_8055E46C[1], lbl_8055E46C[2]);
    return lbl_8055E46C;
}

Phob* fn_80178224(void) { return (Phob*)new ("NetCar") NetCar; }

Phob* fn_80178258(void) { return (Phob*)new ("PhobStatic") StaticPhob; }

Phob* fn_80178290(void) { return (Phob*)new ("WheelPhob") WheelPhob; }

Phob* fn_801782C8(void) {
    void* p = operator new(0x29C, "MutablePhob");
    if (p) {
        fn_8017B0B0(p);
        *(void**)p = lbl_80338400;
    }
    return (Phob*)p;
}

void* fn_80179EE8(void* p, int flag);

void* fn_8017831C(void* p, int flag) {
    if (p) {
        fn_80179EE8(p, 0);
        if (flag > 0) {
            operator delete(p);
        }
    }
    return p;
}

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

int fn_801784DC(const f32* pos, f32* outY, f32* outN, s32* outTri, s32 x) {
    f32 below[3];
    f32 hit[3];
    f32 n[3];
    s32 tri;
    s32 tmp;
    below[0] = pos[0];
    below[1] = pos[1] - 1000.0f;
    below[2] = pos[2];
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
    if (OptProfile__Fv()) {
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
        ti = tick < 0.0f ? (int)(tick - 0.5f) : (int)(0.5f + tick);
        if (!lbl_8055E3BD) {
            lbl_8055E460 += (f32)d / (f32)(ti < 1 ? 1 : ti);
        }
        {
            f32 acc = lbl_8055E460;
            n = (int)acc;
            lbl_8055E460 = acc - (f32)n;
        }
        c->resync = 1;
        if (lbl_8055E3BD) {
            return 1;
        }
        return n;
    }
    lbl_8055E460 = 1.0f;
    {
        int skip = 0;
        if ((lbl_8055D430 & 1) && (lbl_8055D430 & 0x600)) {
            skip = 1;
        }
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
            c->resync = 0;
            c->base = (fn_802E2C9C() + 0x2000) & ~(s64)0x3FFF;
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

void fn_80178C88(void) {
    const char* dump[] = {"PhysTaskDumpCars: %2d - (%3.2f, %3.2f, %3.2f) tri %d"};
}

void fn_80178C8C(Phob* p) {}

PhysStats* fn_80178C90(void) { return &lbl_804305C0; }

CenterLine* fn_80178C9C(void) { return lbl_8055E414; }

s32 fn_80178CA4(void) { return lbl_8055E444; }

void fn_80178CAC(int a, int b) {
    int i;
    RaceCarRec* rb;
    s32 ka;
    s32 kb;
    s32 ma;
    RaceCarRec* ra;
    s32 mb;
    s32 xa;
    s32 xb;
    for (i = 0; i < 4; i++) {
        if (a == lbl_80430590[i]) {
            lbl_80430590[i] = b;
            fn_8026A248(b, i);
        } else if (b == lbl_80430590[i]) {
            lbl_80430590[i] = a;
            fn_8026A248(a, i);
        }
    }
    ra = fn_8017960C(a);
    rb = fn_8017960C(b);
    ka = ra->kind;
    kb = rb->kind;
    ma = ra->model;
    mb = rb->model;
    xb = rb->f270C;
    xa = ra->f270C;
    fn_801A5300(ra, 0);
    fn_801A5300(rb, 0);
    fn_801A5300(ra, xb);
    fn_801A5300(rb, xa);
    ra->kind = kb;
    rb->kind = ka;
    ra->model = mb;
    rb->model = ma;
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
    fn_801DF6FC();
    fn_801DA6EC();
    fn_801D0E0C();
    fn_80127464();
    fn_801CEFAC();
    fn_80177814(lbl_8055E418);
    fn_80177814(lbl_8055E420);
    fn_80177814(lbl_8055E428);
    lbl_8055E414 = new ("util_centerline") CenterLine(0, 0);
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
    fn_801B8B84();
    fn_801CEC80();
    fn_80181F30();
    fn_801DE0A4();
    fn_801769CC();
    fn_801B2024();
    fn_80127464();
    fn_801C4AD0();
    fn_8020C41C();
    fn_8026B644();
    fn_801C1B9C();
    fn_801D21C0();
    fn_801D32D0();
    fn_801D9284();
    fn_801D4734();
    fn_801CB2D8();
    fn_801DEC18();
    fn_80127464();
    fn_801E0710();
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
    fn_801AE4C4();
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
    lbl_8055E448 = 0;
    lbl_8055E444 = lbl_8055E440 = lbl_8055E430 > 1 ? lbl_8055E430 : 1;
    fn_80267420();
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

void fn_801791A4(Phob* p) {}

void fn_801791A8(void) {
    int i;
    fn_80177A18();
    fn_801B537C((s32 (*)(void*))fn_80178C4C);
    fn_801E1D9C();
    fn_8026785C();
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
    fn_801E1520();
    fn_801DEE3C();
    fn_801CB518();
    fn_801D483C();
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
    fn_801B3B28();
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

void fn_801792F4(void) { fn_801E21B4(); }

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
    if (fn_801CF050()) {
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
    fn_801B265C();
    fn_801AED74();
    fn_801E2294();
    for (i = 0; i < lbl_8055E438; i++) {
        lbl_8055E434[i]->Reset();
    }
    fn_801B2910();
    fn_80267680();
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
    fn_801CECE0();
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
    fn_801CEFE8();
    fn_801AED5C();
    fn_801CB464();
}

void fn_801795B0(void) {
    fn_801CF01C();
    fn_801CB4A0();
    fn_801AED68();
}

void fn_801795D8(RaceCarRec* r, int i) {
    lbl_80430830[i] = r;
    if (r->kind == 0x50) {
        lbl_80430590[r->slot & 3] = i;
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

u8 fn_801796B4(void) { return lbl_8055E45D; }

void fn_801796BC(u8 v) { lbl_8055E45E = v; }

u8 fn_801796C4(void) { return lbl_8055E45F; }

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
    if (!fn_801CF050() || fn_801781C0(fn_8026A368(0)) || fn_801B526C()) {
        p->nViews = 1;
    }
    lbl_8055E444 = p->nViews;
    for (i = 0; i < lbl_8055E440; i++) {
        fn_802678E8(&p->cars[i], i);
        p->cars[i].id = lbl_804305A0[i].id;
        p->cars[i].count = lbl_804305A0[i].count;
    }
    fn_80206550(lbl_8055E444);
    p->time = 0.016683351f * (lbl_8055E460 - 1.0f) + lbl_8055E3C8;
    for (i = 0; i < lbl_8055E438; i++) {
        lbl_8055E434[i]->vf28(buf);
    }
    fn_801B2D28(p);
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

void fn_80179AF4(Phob* v, u8* buf) {
    u8* o = buf + v->a8;
    PMtx* out = (PMtx*)o;
    if ((f32)__fabs(lbl_8055E460 - 1.0f) <= lbl_80558DC4[0]) {
        *out = v->world;
    } else {
        PMtx t;
        int i;
        f32* d = &t.rot[0].x;
        const f32* a = &v->world.rot[0].x;
        const f32* b = &v->prev.rot[0].x;
        f32 k = lbl_8055E460;
        for (i = 0; i < 2; i++) {
            d[0] = b[0] + k * (a[0] - b[0]);
            d[1] = b[1] + k * (a[1] - b[1]);
            d[2] = b[2] + k * (a[2] - b[2]);
            d[3] = b[3] + k * (a[3] - b[3]);
            d[4] = b[4] + k * (a[4] - b[4]);
            d[5] = b[5] + k * (a[5] - b[5]);
            d += 6;
            a += 6;
            b += 6;
        }
        *out = t;
    }
    *(s16*)(o + 0x30) = v->hint[0];
}

u8* fn_80179CB0(void) { return lbl_8055E450->phobs; }

}  // extern "C"
