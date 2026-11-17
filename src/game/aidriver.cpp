// AI race setup and driver factories (0x8013471C-0x80135278): reads the event's "ai" tunables (green
// flag distance, pit speed), creates per-car AI info, formats car numbers, and builds the driver object
// behind each local car (AI, arena, park or null). Names are guesses.
#include "game/common.h"
#include "game/racesetup.h"
#include "game/proxer.h"
#include "game/gamemode.h"
#include "game/raceopts.h"
#include "game/gamedb.h"
#include "game/springcar.h"

extern "C" {
int sprintf(char* s, const char* fmt, ...);
char* strcpy(char* d, const char* s);
double atof(const char* s);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void fn_8013B4E0(void* cars, void* race);
f32 fn_8013672C(void);
void fn_80137884(s32 count);
void fn_80139044(int* p);
void fn_8013BAE4(s32 count);
void fn_801663C8(s32 count, void* cars, void* extra);
void fn_8013A1E8(int n);
void fn_8015F554(void);
void fn_80160850(void);
void fn_80160AE4(void);
void fn_8015F558(void);
void fn_8013A1EC(void);
void fn_8013B56C(void);
void fn_80166814(void);
void fn_801378D0(void);
void fn_8013BC34(void);
void fn_80166898(void);
void fn_80137904(void);
void fn_8013A1F0(void);
void fn_8013A200(s32 idx);
void fn_8013A204(s32 idx);
void fn_8013BC54(void);
void fn_80166A24(void);
u8* fn_8017960C(int i);
u32 fn_801769A8(void);
u8* fn_801792F8(void);
void fn_8018ADBC(void* car, f32 v);
void fn_8018ADB4(void* car, f32 v);
void fn_8018ADA4(void* car, f32 v);
}


class IDriver;
extern "C" GameMode* fn_801B83A0(void);
void* operator new(size_t size, const char* tag);

struct AIGrid {
    CarDef cars[8];  // offset 0x0
    s32 count;      // offset 0x3C0
};

struct AIRace {
    u8 pad0[0x20];
    AIGrid grid;   // offset 0x20
    u8 extra[4];   // offset 0x3E4
};

struct AICarInfo {
    AICarInfo(s32 i);
    ~AICarInfo(void) {}
    s32 idx;    // offset 0x0
    u8 active;  // offset 0x4
};

#include "game/idriver.h"

class AIDriver : public IDriver {
public:
    AIDriver(void);
    virtual ~AIDriver(void);
    virtual void Init(void* car, void* q);
    virtual void Start(void);
    virtual void Stop(void);
    u8 pad[0x308];
};

class NullDriver : public IDriver {
public:
    virtual ~NullDriver(void);
    virtual void Init(void* car, void* q) {}
    virtual void Start(void) {}
    virtual void Stop(void) {}
};

class ParkDriver : public IDriver {
public:
    ParkDriver(bool f) { flag = f; }
    virtual ~ParkDriver(void);
    virtual void Init(void* car, void* q) { SetCar(car); }
    virtual void SetCar(void* c) { car = c; }
    virtual void Start(void) {
        fn_8018ADBC(car, 1.0f);
        fn_8018ADB4(car, 0.0f);
        fn_8018ADA4(car, 0.0f);
    }
    virtual void Stop(void) {}
    virtual u32 Flags(void) { return flag ? 0x100 : 0; }

    bool flag;  // offset 0x4
    void* car;  // offset 0x8
};

extern const f32 lbl_80560108;
extern const f32 lbl_8056010C;
extern Proxer* lbl_8055E0D4;  // AI system
extern u32 lbl_8055E3C4;

ModuleTag lbl_8055E098;
f32 lbl_8055E0A0;  // green flag distance
f32 lbl_8055E0A4;  // green flag distance (reverse)
f32 lbl_8055E0A8;  // pit speed
ValueNode* lbl_8055E0AC;
s32 lbl_8055E0B0;  // racing car count
AICarInfo* lbl_8042F758[8];

AICarInfo::AICarInfo(s32 i) {
    idx = i;
    active = 1;
}

extern "C" {
const char* fn_80134B9C(const char* key, const char* def);
f32 fn_80134B58(const char* key, f32 def);
int fn_80134A44(int i);
s32 fn_80134C24(int i);
IDriver* fn_80134E90(void);
IDriver* fn_80134FF8(void);
IDriver* fn_801350C0(void);

void fn_8013472C(void) {
    RaceOpts* ev = fn_801E87A8();
    EventInfo* info = GetEvent(ev->event);
    lbl_8055E0AC = info->node->FindChild("ai");
}

void fn_80134770(void) { lbl_8055E0AC = 0; }

// Stripped by the linker; its literal and constants come first in the pools.
f32 fn_80134770_flag_offset(void) { return 0.305f * fn_80134B58("flag_offset", 350.0f); }

static inline f32 DefaultGreenFlag(void) {
    f32 offset = 0.305f * fn_80134B58("flag_offset", 350.0f);
    return fn_8013672C() - offset;
}

void fn_8013477C(AIRace* race) {
    AIGrid* grid = &race->grid;
    int i;
    fn_8013472C();
    for (lbl_8055E0B0 = grid->count; lbl_8055E0B0 > 0 && grid->cars[lbl_8055E0B0 - 1].type == 'S'; lbl_8055E0B0--) {
    }
    fn_8013B4E0(grid, race);
    f32 g = fn_80134B58("greenflag_lon", -1.0f);
    lbl_8055E0A0 = g;
    if (g < 0.0f) {
        lbl_8055E0A0 = DefaultGreenFlag();
    }
    lbl_8055E0A8 = 0.44444445f * fn_80134B58("pit_speed", 20.0f);
    fn_802E30C8("ai: pit_speed = %1.1f mph", lbl_8055E0A8 / 0.44444445f);
    lbl_8055E0A4 = fn_80134B58("rtt_greenflag_lon", fn_8013672C() / 2);
    fn_80137884(lbl_8055E0B0);
    fn_80139044((int*)fn_801E87A8());
    fn_8013BAE4(lbl_8055E0B0);
    fn_801663C8(lbl_8055E0B0, grid, race->extra);
    for (i = 0; i < grid->count; i++) {
        if (grid->cars[i].type != 'S') {
            lbl_8042F758[i] = new ("AI::AICarInfo") AICarInfo(i);
        }
    }
    fn_8013A1E8(grid->count);
    fn_8015F554();
    fn_80160850();
}

void fn_8013490C(void) {
    int i;
    fn_80160AE4();
    fn_8015F558();
    fn_8013A1EC();
    fn_8013B56C();
    fn_80166814();
    for (i = 0; i < 8; i++) {
        if (lbl_8042F758[i]) {
            delete lbl_8042F758[i];
            lbl_8042F758[i] = 0;
        }
    }
    fn_80134770();
    fn_801378D0();
}

void fn_80134994(void) {}

void fn_80134998(void) {
    fn_8013BC34();
    fn_80166898();
    fn_80137904();
    fn_8013A1F0();
}

void fn_801349C4(void) {
    AICarInfo* p;
    int i;
    for (i = 0; i < 8; i++) {
        p = lbl_8042F758[i];
        if (p) {
            lbl_8055E0D4->Activate(p->idx);
            fn_8013A200(p->idx);
            p->active = 1;
        }
    }
}

int fn_80134A44(int i) { return lbl_8055E3C4 & 1; }

void fn_80134A88(void) {}

void fn_80134A8C(void) {
    AICarInfo* p;
    int i;
    for (i = 0; i < lbl_8055E0B0; i++) {
        p = lbl_8042F758[i];
        if (p) {
            if (p->active) {
                lbl_8055E0D4->Activate(p->idx);
            } else {
                lbl_8055E0D4->Update(p->idx);
            }
            p->active = 0;
            fn_8013A204(p->idx);
        }
    }
    if (fn_80134A44(-1)) {
        lbl_8055E0D4->Process();
    }
    fn_8013BC54();
    fn_80166A24();
}

void fn_80134B50(void) { lbl_8055E0D4->UpdateAll(); }

f32 fn_80134B58(const char* key, f32 def) {
    const char* s = fn_80134B9C(key, 0);
    if (s) {
        def = atof(s);
    }
    return def;
}

const char* fn_80134B9C(const char* key, const char* def) {
    if (lbl_8055E0AC && lbl_8055E0AC->Has(key)) {
        return lbl_8055E0AC->GetValue(key);
    }
    return def;
}

f32 fn_80134C1C(void) { return lbl_8055E0A8; }

s32 fn_80134C24(int i) {
    if (i >= 0 && i < 8) {
        return *(s32*)(fn_8017960C(i) + 0x258);
    }
    return -1;
}

char* fn_80134C60(int n, char* buf) {
    if (n >= 0) {
        sprintf(buf, "#%3d", n);
    } else {
        switch (n) {
        case -20:
            strcpy(buf, "#GST");
            break;
        default:
            strcpy(buf, "#---");
            break;
        }
    }
    return buf;
}

char* fn_80134CD4(int i, char* buf) {
    if (i >= 0) {
        fn_80134C60(fn_80134C24(i), buf);
    } else {
        fn_80134C60(-40, buf);
    }
    return buf;
}

f32 fn_80134D20(void) {
    if (fn_801769A8() & 0x800) {
        return lbl_8055E0A4;
    }
    return lbl_8055E0A0;
}

u32 fn_80134D54(void) {
    fn_801769A8();
    return 0;
}

f32 fn_80134D78(void) {
    u32 x = fn_80134D54();
    int n = (lbl_8055E0B0 + 1) / 2;
    return 4.0f + lbl_80560108 * x + n * lbl_8056010C;
}

f32 fn_80134DF8(void) { return 4.0f; }

f32 fn_80134E00(void) {
    if ((fn_801769A8() >> 8) & 1) {
        return fn_80134D78();
    }
    return fn_80134DF8();
}

f32 fn_80134E34(void*, f32 d) {
    f32 t = d < 0.0f ? fn_80134E00() : d;
    f32 speed = fn_80134C1C();
    return fn_80134D20() - t * speed;
}

IDriver* fn_80134E90(void) { return new ("AIDriver") AIDriver; }

IDriver* fn_80134EC8(u32 type) {
    IDriver* d = fn_801B83A0()->vfEC();
    if (!d) {
        switch (type) {
        case 0x180201:
            d = fn_80134E90();
            break;
        default:
            fn_802E30C8("WARNING: No arena driver for type 0x%x", type);
            d = fn_80134FF8();
            break;
        }
    }
    return d;
}

IDriver* GameMode::vfEC(void) { return 0; }

void* fn_80134F48(void) {
    EventDesc* desc = fn_801769CC();
    IDriver* d;
    bool park = false;
    if (fn_801792F8()[0xD8] & 4) {
        park = true;
    }
    if (park) {
        d = fn_801350C0();
    } else if (desc->type & 0x100) {
        d = fn_80134E90();
    } else if (desc->type & 0x200) {
        d = fn_80134EC8(desc->type);
    } else {
        d = fn_80134FF8();
    }
    return new ("LocalCar (ai)") SpringCar((Controller*)d);
}

IDriver* fn_80134FF8(void) { return new ("NullDriver") NullDriver; }

IDriver* fn_80135078(void) { return new ("ParkDriver") ParkDriver(false); }

IDriver* fn_801350C0(void) { return new ("ParkDriver") ParkDriver(true); }

// Stripped by the linker.
const char* fn_801350C0_rtt_pack_lon(void) { return fn_80134B9C("rtt_pack_lon", 0); }

s32 fn_80135108(void) { return lbl_8055E0B0; }

}  // extern "C"

ParkDriver::~ParkDriver(void) {}

NullDriver::~NullDriver(void) {}
