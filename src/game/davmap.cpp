// Driver/vehicle map (0x8001786C-0x8001880C): decides which drivers race in which vehicles. The
// players come first, then randomly shuffled AI drivers, vehicles and body colours, optionally
// overridden by the track's "force_chassis" or an event's explicit "vehicle_map". The "DAVMap" /
// "davmap:" messages give the name; the rest are guesses.
#include <string.h>
#include <stdlib.h>
#include "game/common.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
void fn_802E30C8(const char* fmt, ...);  // debug print
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);  // snprintf
s32 fn_8013C8E8(const char* chassis);
void fn_8013C804(void);
void fn_8013C8B8(void);
int fn_80129D18(int stream, int n);  // random
int atoi(const char* s);
}

#define MAX_RACERS 8
#define MAX_LIST 50

struct DriverInfo {
    s32 index;        // offset 0x0
    s32 id;           // offset 0x4
    char name[0x40];  // offset 0x8
    s32 rank;         // offset 0x48
    s32 vehicle;      // offset 0x4C
    u32 color;        // offset 0x50
};

struct VehicleInfo {
    s32 index;                // offset 0x0
    s32 id;                   // offset 0x4
    s32 base;                 // offset 0x8
    char display_name[0x20];  // offset 0xC
    char chassis[0x20];       // offset 0x2C
    char texture[0x10];       // offset 0x4C
    char number[8];           // offset 0x5C
    u32 colors[12];           // offset 0x64
    s32 default_color;        // offset 0x94
    s32 num_colors;           // offset 0x98
    s32 unk9C;                // offset 0x9C
    u32 flags;                // offset 0xA0
};

struct Track {
    u8 unk0[0x70];
    s32 num_eves;     // offset 0x70
    u8 unk74[0xC];
    ValueNode* node;  // offset 0x80
};

struct Eve {
    u8 unk0[0x10];
    s32 num_racers;  // offset 0x10
};

struct EventInfo;

extern "C" {
Track* fn_80012820(s32 track);
Eve* fn_80012870(s32 track, int eve);
Eve* fn_8001285C(s32 event);
}

VehicleInfo* FindVehicle(s32 id);
int GetNumPlayers(void);
DriverInfo* GetPlayerDriver(int slot);
VehicleInfo* GetVehicle(int i);
void SetDriverColor(s32 id, u32 color);
DriverInfo* GetDriver(int i);
int GetNumDrivers(void);
int GetNumVehicles(void);
DriverInfo* FindDriver(s32 id);
void SetListDriverColor(s32 id, u32 color);
VehicleInfo* FindVehicleNamed(const char* chassis, int n);

template <class T, int N>
class FixedArray {
public:
    FixedArray(void) {
        data = storage;
        max = N;
        count = 0;
    }
    ~FixedArray(void) {
        data = NULL;
        count = 0;
        max = 0;
    }
    void Add(T x) { *Alloc() = x; }
    T* Alloc(void) { return &data[count++]; }
    int Find(T x) {
        int i;

        for (i = 0; i < count; i++) {
            if (x == data[i]) {
                return i;
            }
        }
        return -1;
    }
    void RemoveAt(int i) {
        if (i >= 0 && i < count) {
            if (i != count - 1) {
                data[i] = data[count - 1];
            }
            count--;
        }
    }

    T* data;           // offset 0x0
    s32 count;         // offset 0x4
    s32 max;           // offset 0x8
    T storage[N];      // offset 0xC
};

template <class T>
inline void Swap(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}

typedef FixedArray<s32, MAX_LIST> IdList;
typedef FixedArray<u32, MAX_LIST> ColorList;

void ShuffleIds(IdList* list);
void ShuffleColors(ColorList* list);

class DAVMap {
public:
    void Add(s32 driver, s32 vehicle);
    void Append(DAVMap* other);

    s32 num_drivers;              // offset 0x0
    s32 drivers[MAX_RACERS];      // offset 0x4
    s32 num_vehicles;             // offset 0x24
    s32 vehicles[MAX_RACERS];     // offset 0x28
};

ModuleTag lbl_8055D558;
s32 lbl_8055D560;           // forced racer count
u8 lbl_8055D564;            // -default_colors
char lbl_80357DC0[0x40];    // forced chassis

void CheckVehicleClass(s32 vehicle) {
    const char* chassis = FindVehicle(vehicle)->chassis;

    if (fn_8013C8E8(chassis) == 6) {
        fn_802E30C8("DAVMap: WARNING! Player is driving the %s, an unclassified car!", chassis);
    }
}

void DAVMap::Add(s32 driver, s32 vehicle) {
    drivers[num_drivers++] = driver;
    vehicles[num_vehicles++] = vehicle;
}

BOOL UseVehicleColors(void) {
    return 0;
}

void SetDriverColorFromVehicle(s32 driver, s32 vehicle) {
    VehicleInfo* v = FindVehicle(vehicle);

    SetDriverColor(driver, v->colors[v->default_color]);
}

void AddPlayers(DAVMap* map) {
    int n = GetNumPlayers();
    int i;

    map->num_drivers = map->num_vehicles = 0;
    for (i = 0; i < n; i++) {
        DriverInfo* d = GetPlayerDriver(i);
        s32 driver = d->id;
        s32 vehicle = GetVehicle(d->vehicle)->id;

        map->Add(driver, vehicle);
        CheckVehicleClass(vehicle);
        if (UseVehicleColors()) {
            SetDriverColorFromVehicle(driver, vehicle);
        }
    }
}

BOOL ListContains(s32 id, s32* list, int n) {
    int i;

    for (i = 0; i < n; i++) {
        if (id == list[i]) {
            return 1;
        }
    }
    return 0;
}

BOOL MapHasVehicle(s32 id, DAVMap* map) {
    return ListContains(id, map->vehicles, map->num_vehicles);
}

BOOL MapHasDriver(s32 id, DAVMap* map) {
    return ListContains(id, map->drivers, map->num_drivers);
}

void CollectDrivers(IdList* list, DAVMap* map) {
    int i;

    for (i = 0; i < GetNumDrivers(); i++) {
        s32 id = GetDriver(i)->id;
        if (!MapHasDriver(id, map)) {
            list->Add(id);
        }
    }
    ShuffleIds(list);
}

void ShuffleIds(IdList* list) {
    int i;

    for (i = 0; i < list->count; i++) {
        int j = fn_80129D18(1, list->count);
        Swap(list->data[i], list->data[j]);
    }
}

void CollectVehicles(IdList* list, DAVMap* map) {
    int i;

    for (i = 0; i < GetNumVehicles(); i++) {
        VehicleInfo* v = GetVehicle(i);
        s32 id = v->id;
        s32 base = v->base;
        bool extreme = (v->flags & 2) != 0;
        bool magical = (v->flags & 4) != 0;
        bool no_ai = (v->flags & 8) != 0;
        BOOL skip = extreme || magical || no_ai;

        if (!skip && !MapHasVehicle(id, map) && !MapHasVehicle(base, map)) {
            list->Add(id);
        }
    }
    ShuffleIds(list);
}

void CollectColors(ColorList* list, DAVMap* map) {
    VehicleInfo* v = GetVehicle(0);
    int i;

    for (i = 0; i < v->num_colors; i++) {
        *list->Alloc() = v->colors[i];
    }
    for (i = 0; i < map->num_drivers; i++) {
        int k = list->Find(FindDriver(map->drivers[i])->color);
        if (k != -1) {
            list->RemoveAt(k);
        }
    }
    ShuffleColors(list);
}

void ShuffleColors(ColorList* list) {
    int i;

    for (i = 0; i < list->count; i++) {
        int j = fn_80129D18(1, list->count);
        Swap(list->data[i], list->data[j]);
    }
}

void BuildAIMap(DAVMap* map, int n, DAVMap* players) {
    map->num_drivers = map->num_vehicles = 0;
    if (n > 0) {
        int i;
        IdList drivers;
        CollectDrivers(&drivers, players);
        IdList vehicles;
        CollectVehicles(&vehicles, players);
        ColorList colors;
        CollectColors(&colors, players);
        for (i = 0; i < n; i++) {
            s32 driver = drivers.data[i % drivers.count];
            s32 vehicle = vehicles.data[i % vehicles.count];
            u32 color = colors.data[i % colors.count];

            map->Add(driver, vehicle);
            if (UseVehicleColors()) {
                SetDriverColorFromVehicle(driver, vehicle);
            } else {
                SetListDriverColor(driver, color);
            }
        }
    }
}

BOOL IsSinglePlayer(void) {
    return GetNumPlayers() == 1;
}

int GetNumRacers(s32 track) {
    if (lbl_8055D560 != 0) {
        int n = GetNumPlayers();
        return n > lbl_8055D560 ? n : lbl_8055D560;
    }
    if (!IsSinglePlayer()) {
        return GetNumPlayers();
    } else {
        int best = 0;
        Track* t = fn_80012820(track);
        int i;

        for (i = 0; i < t->num_eves; i++) {
            Eve* e = fn_80012870(track, i);
            if (e->num_racers > best) {
                best = e->num_racers;
            }
        }
        return best;
    }
}

void ApplyForceChassis(DAVMap* map, s32 track) {
    const char* chassis = fn_80012820(track)->node->GetString("force_chassis", lbl_80357DC0);

    if (chassis != NULL && *chassis != '\0') {
        int i;

        fn_802E30C8("davmap: using force_chassis \"%s\"", chassis);
        for (i = 0; i < map->num_vehicles; i++) {
            map->vehicles[i] = FindVehicleNamed(chassis, 0)->id;
        }
    }
}

void ApplyVehicleMap(DAVMap* map, s32 track) {
    ValueNode* vm = fn_80012820(track)->node->FindChild("vehicle_map");

    if (vm != NULL) {
        const char* def;
        const char* player;
        int i;

        fn_802E30C8("davmap: eve contains explicit vehicle map");
        def = vm->GetString("default_vehicle", NULL);
        player = vm->GetString("player_vehicle", NULL);
        for (i = 0; i < map->num_vehicles; i++) {
            BOOL is_player = 0;
            int j;

            for (j = 0; j < GetNumPlayers(); j++) {
                if (map->drivers[i] == GetPlayerDriver(j)->id) {
                    is_player = 1;
                    break;
                }
            }
            if (player != NULL && is_player) {
                if (strcasecmp(player, "*") != 0) {
                    map->vehicles[i] = FindVehicleNamed(player, 0)->id;
                }
            } else {
                char key[0x20];
                const char* name;

                fn_801345D8(key, sizeof(key), "car%d", i + 1);
                name = vm->GetString(key, def);
                if (strcasecmp(name, "*") != 0) {
                    map->vehicles[i] = FindVehicleNamed(name, 0)->id;
                }
            }
        }
    }
}

// Stripped from ET; only its strings remain.
void DumpDAVMap(DAVMap* map) {
    int i;

    fn_802E30C8("davmap: map has %d drivers and %d vehicles", map->num_drivers, map->num_vehicles);
    for (i = 0; i < map->num_drivers; i++) {
        fn_802E30C8("\tdriver %d) %d (%s)", i, map->drivers[i], FindDriver(map->drivers[i])->name);
    }
    for (i = 0; i < map->num_vehicles; i++) {
        fn_802E30C8("\tvehicle %d) %d (%s)", i, map->vehicles[i], FindVehicle(map->vehicles[i])->chassis);
    }
}

void DAVMap::Append(DAVMap* other) {
    int i;

    for (i = 0; i < other->num_drivers; i++) {
        drivers[num_drivers++] = other->drivers[i];
    }
    for (i = 0; i < other->num_vehicles; i++) {
        vehicles[num_vehicles++] = other->vehicles[i];
    }
}

void BuildDAVMap(DAVMap* out, s32 track) {
    DAVMap players;
    DAVMap ai;
    int n;

    fn_8013C804();
    AddPlayers(&players);
    n = GetNumRacers(track) - players.num_drivers;
    BuildAIMap(&ai, n < 0 ? 0 : n, &players);
    memset(out, 0, sizeof(DAVMap));
    out->Append(&ai);
    out->Append(&players);
    ApplyForceChassis(out, track);
    ApplyVehicleMap(out, track);
    fn_8013C8B8();
}

int GetNumRacersForEve(Eve* eve) {
    if (lbl_8055D560 != 0) {
        int n = GetNumPlayers();
        return n > lbl_8055D560 ? n : lbl_8055D560;
    }
    {
        int result;

        if (IsSinglePlayer()) {
            int n = GetNumPlayers();
            result = n > eve->num_racers ? n : eve->num_racers;
        } else {
            result = GetNumPlayers();
        }
        return result;
    }
}

void BuildReversedMap(DAVMap* out, s32 event, const DAVMap* src) {
    int n = GetNumRacersForEve(fn_8001285C(event));
    int i;

    memset(out, 0, sizeof(DAVMap));
    for (i = 0; i < n; i++) {
        out->drivers[i] = src->drivers[src->num_drivers - 1 - i];
        out->num_drivers++;
        out->vehicles[i] = src->vehicles[src->num_vehicles - 1 - i];
        out->num_vehicles++;
    }
}

void SetForcedRacers(const char* arg) {
    lbl_8055D560 = atoi(arg);
}

#define COPY_STRING(dst_, src_, size_)                  \
    {                                                   \
        const char* src = (src_);                       \
        int n = (size_);                                \
        char* dst = (dst_);                             \
        while ((*dst++ = *src++) != '\0') {             \
            if (--n <= 0) {                             \
                dst[-1] = '\0';                         \
                break;                                  \
            }                                           \
        }                                               \
    }

void SetForceChassis(const char* arg) {
    COPY_STRING(lbl_80357DC0, arg, sizeof(lbl_80357DC0));
}

void SetDefaultColors(const char* arg) {
    lbl_8055D564 = 1;
}
