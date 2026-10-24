// Race resource loading (0x80009DC8-0x8000AC98): fills the racer slots from the chosen drivers and
// vehicles (plus "Trailer" slots), builds the track/resource/weather .trk names for the event and
// loads/unloads the track, car and overlay resources. Names are guesses.
#include "game/common.h"

struct DriverInfo {
    u8 pad0[0x8];
    char name[1];  // offset 0x8
};

struct VehicleInfo {
    u8 pad0[0x4];
    s32 id;  // offset 0x4
    u8 pad8[0x2C - 0x8];
    char name[0x78];  // offset 0x2C
    s32 unkA4;        // offset 0xA4
};

struct RacerSlot {
    s32 type;              // offset 0x0
    char driverName[64];   // offset 0x4
    char vehicleName[32];  // offset 0x44
    struct { f32 v; } unk64;  // offset 0x64
    s32 driver;            // offset 0x68
    s32 vehicle;           // offset 0x6C
    u8 flags;              // offset 0x70 (low two bits: player)
    s32 unk74;             // offset 0x74
};

struct RacerList {
    RacerSlot slots[8];  // offset 0x0
    s32 count;           // offset 0x3C0
};

struct RaceSetup {
    char track[0x20];  // offset 0x0
    RacerList racers;  // offset 0x20
    s32 event;         // offset 0x3E4
    u32 flags;         // offset 0x3E8
};

struct DAVMap {
    s32 num_drivers;   // offset 0x0
    s32 drivers[8];    // offset 0x4
    s32 num_vehicles;  // offset 0x24
    s32 vehicles[8];   // offset 0x28
};

struct Event {
    u8 pad0[0x20];
    ValueNode* values;  // offset 0x20
};

extern "C" {
int strcasecmp(const char* a, const char* b);
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);
int fn_80137C74(void);
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* table);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void fn_8012DDA8(const char* name);
void fn_8012E498(const char* name);
void fn_8012E81C(const char* name);
void fn_801DCDA0(void);
void fn_801DCDA4(void);
void fn_802E3D00(void);
void fn_80206484(const char* name);
void* fn_801292CC(const char* name);
void fn_80129324(void* p);
void fn_80206518(void);
void fn_8020CA38(void);
void fn_8020C920(RaceSetup* rs);
void fn_802103B8(RaceSetup* rs);
}
DriverInfo* FindDriver(long id);
VehicleInfo* FindVehicle(long id);
VehicleInfo* FindVehicleByFlags(u32 flags, int n);
int IsPlayerDriver(long id, int player);
Event* GetEvent(int id);
void LoadTimerBegin(const char* name, int a, int b, f32 total);
void LoadTimerProgress(void);

// Six-word static built by an inline constructor (first word stored first).
class RaceLoadStatic {
public:
    RaceLoadStatic(void) {
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

ModuleTag lbl_8055D3A0;
static RaceLoadStatic lbl_8034F4C8;
UnkStaticFFFF lbl_8055D3A8;
UnkStatic7F lbl_8055D3AC;

static char lbl_8034F4E0[64];  // track
char lbl_8034F520[64];         // track resource
static char lbl_8034F560[64];  // track weather
void* lbl_8055D3B0;            // race sounds
u8 lbl_8055D3B4;               // resource track used
u8 lbl_8055D3B5;               // weather track used
Event* lbl_8055D3B8;

static inline void CopyString(const char* src, int size, char* dst) {
    char* d = dst;
    int n = size;
    const char* s = src;
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

void fn_80009DC8(RaceSetup* rs) {
    s32 vehicle;
    RacerSlot* s;
    int p;
    RacerSlot tmp;
    int i;
    DriverInfo* d;
    int n = rs->racers.count;
    RacerList* list = &rs->racers;
    VehicleInfo* v;

    for (i = 0, s = list->slots; i < n; s++, i++) {
        d = FindDriver(s->driver);
        v = FindVehicle(s->vehicle);
        s->flags = 0;
        s->unk74 = v->unkA4;
        s->unk64.v = 0.0f;
        CopyString(v->name, 32, s->vehicleName);
        s->type = 'Q';
        for (p = 0; p < fn_80137C74(); p++) {
            if (IsPlayerDriver(s->driver, p)) {
                s->type = 'P';
                s->flags = (s->flags & ~3) | (p & 3);
            }
        }
        CopyString(d->name, 64, s->driverName);
    }
    if (rs->flags & 0x1000) {
        for (i = list->count / 2; i < list->count; i++) {
            list->slots[i].type = 'T';
        }
    }
    if (rs->flags & 0x400) {
        tmp.type = 'T';
        CopyString("Trailer", 64, tmp.driverName);
        CopyString("", 32, tmp.vehicleName);
        tmp.driver = -31;
        tmp.vehicle = -31;
        tmp.flags = 0;
        tmp.unk74 = 0;
        for (i = 0; i < n; i++) {
            s = &list->slots[list->count++];
            vehicle = FindVehicleByFlags(1, i)->id;
            *s = tmp;
            s->vehicle = vehicle;
            CopyString(FindVehicle(vehicle)->name, 32, s->vehicleName);
        }
    }
}

void fn_8000A644(RaceSetup* rs, DAVMap* map) {
    RacerList* list = &rs->racers;
    RacerSlot* s = list->slots;
    int i;
    rs->racers.count = map->num_vehicles;
    for (i = 0; i < list->count; i++, s++) {
        s->driver = map->drivers[i];
        s->vehicle = map->vehicles[i];
    }
    fn_80009DC8(rs);
}

void fn_8000A688(void (*fn)(const char* name), const char* car) {
    char name[32];
    fn_801345D8(name, sizeof(name), "%s.car", car);
    fn(name);
}

void fn_8000A6D8(void) {
    const char* contest = lbl_8055D3B8->values->GetValue("weather");
    const char* res = "none";
    ValueTable* table = fn_80131810("weather.val");
    if (table) {
        res = table->FindChild("weather")->GetValue("res");
        fn_80131818(table);
    }
    if (strcasecmp(res, contest)) {
        fn_802E317C("game: weather mismatch (contest=%s,resource=%s)", contest, res);
    }
}

void fn_8000A7A8(void) {
    if (lbl_8055D3B4) {
        fn_8012DDA8(lbl_8034F520);
    }
    fn_8012DDA8(lbl_8034F4E0);
    if (lbl_8055D3B5) {
        fn_8012DDA8(lbl_8034F560);
    }
    fn_801DCDA0();
}

void fn_8000A808(void) {
    fn_801DCDA4();
    fn_8012E498(lbl_8034F4E0);
    if (lbl_8055D3B4) {
        fn_8012E498(lbl_8034F520);
    }
    if (lbl_8055D3B5) {
        fn_8012E498(lbl_8034F560);
    }
}

void fn_8000A868(void) {
    fn_8012E81C(lbl_8034F4E0);
    if (lbl_8055D3B4) {
        fn_8012E81C(lbl_8034F520);
    }
    if (lbl_8055D3B5) {
        fn_8012E81C(lbl_8034F560);
    }
}

void fn_8000A8C4(RaceSetup* rs) {
    const char* res;
    const char* weather;

    fn_802E3D00();
    fn_801345D8(lbl_8034F4E0, sizeof(lbl_8034F4E0), "%s.trk", rs->track);
    lbl_8055D3B8 = GetEvent(rs->event);
    res = lbl_8055D3B8->values->GetString("resource", NULL);
    if (res) {
        lbl_8055D3B4 = 1;
        fn_801345D8(lbl_8034F520, sizeof(lbl_8034F520), "%s_%s.trk", rs->track, res);
    } else {
        lbl_8055D3B4 = 0;
        fn_801345D8(lbl_8034F520, sizeof(lbl_8034F520), "%s.trk", rs->track);
    }
    weather = lbl_8055D3B8->values->GetValue("weather");
    if ((lbl_8055D3B5 = strcasecmp(weather, "none") != 0)) {
        fn_801345D8(lbl_8034F560, sizeof(lbl_8034F560), "%s_%s.trk", rs->track, weather);
    } else {
        lbl_8034F560[0] = 0;
    }
    LoadTimerBegin("preload_track", 1, 1, 45345.0f);
    fn_80206484("racesnd.res");
    lbl_8055D3B0 = fn_801292CC("speech");
    LoadTimerProgress();
}

void fn_8000AA38(RaceSetup* rs) {
    fn_80129324(lbl_8055D3B0);
    lbl_8055D3B0 = 0;
    fn_80206518();
    fn_8020CA38();
    lbl_8055D3B8 = 0;
    lbl_8055D3B4 = 0;
    lbl_8055D3B5 = 0;
    lbl_8034F4E0[0] = 0;
    lbl_8034F560[0] = 0;
    lbl_8034F520[0] = 0;
}

void fn_8000AA9C(const char* car) {
    if (*car) {
        fn_8000A688(fn_8012DDA8, car);
    }
}

void fn_8000AABC(const char* car) {
    if (*car) {
        fn_8000A688(fn_8012E498, car);
    }
}

void fn_8000AADC(RaceSetup* rs) { fn_8000A7A8(); }

void fn_8000AAE0(RaceSetup* rs) { fn_8000A808(); }

void fn_8000AAE4(RaceSetup* rs) {
    int i;
    for (i = 0; i < rs->racers.count; i++) {
        fn_8000AABC(rs->racers.slots[i].vehicleName);
    }
}

void fn_8000AB44(RaceSetup* rs) { fn_8000A6D8(); }

void fn_8000AB48(RaceSetup* rs, DAVMap* map) {
    int i;
    fn_8000A8C4(rs);
    fn_8000AADC(rs);
    fn_8000A644(rs, map);
    for (i = 0; i < rs->racers.count; i++) {
        fn_8000AA9C(rs->racers.slots[i].vehicleName);
    }
    fn_8012DDA8("overlays.res");
    fn_8020C920(rs);
    fn_802103B8(rs);
}

void fn_8000ABE0(RaceSetup* rs) {
    fn_8012E498("overlays.res");
    fn_8000AAE4(rs);
    fn_8000AAE0(rs);
    fn_8000AA38(rs);
}

char* fn_8000AC2C(void) { return lbl_8034F520; }

Event* fn_8000AC38(void) { return lbl_8055D3B8; }
