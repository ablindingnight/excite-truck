// Event ("eve") registry (0x800126CC-0x80013B90): loads eves.val / demo_eves.val for the current game
// mode, one entry per track with its contests (event type, laps, cars, terrain, purse...), plus the
// "make your own" eves. The "Eve:" messages give the name; the rest are guesses.
#include <string.h>
#include <stdlib.h>
#include "game/common.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
char* strstr(const char* s, const char* sub);
char* strncpy(char* dst, const char* src, size_t n);
int sprintf(char* buf, const char* fmt, ...);
void qsort(void* base, size_t num, size_t size, int (*cmp)(const void*, const void*));
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);  // snprintf
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* table);
int fn_8012C33C(const char* name);  // resource exists
void fn_8013C804(void);
void fn_8013C8B8(void);
s32 fn_8013C8E8(const char* chassis);
s32 fn_8013C9A4(const char* car_class);
void fn_8022D0D8(char* buf, int size);
void fn_8022D0E8(char* buf, int size, const char* s);
}

struct League {
    u8 unk0[0x8];
    char name[0x20];  // offset 0x8
};

extern "C" League* fn_80137C54(int i);

BOOL OptDirectToTrack(void);
BOOL OptPaTest(void);
int FindTrack(const char* name);
u32 FindEventType(const char* name);
const char* GetEventTypeName(u32 type);
const char* GetEventTypeDisplayName(u32 type);

#define MAX_TRACKS 35
#define MAX_EVENTS 250
#define MAX_MYO 71
#define MAX_VAL_FILES 32
#define MAX_CONTESTS 64
#define MAX_CHASSIS_ALLOWED 7
#define MAX_CARS 8

#define MODE_ARCADE 0x12F
#define MODE_CAREER 0x130
#define MODE_MULTI 0x131
#define MODE_DEMO 0x132
#define MODE_CHALLENGE 0x133
#define MODE_NONE 0x134

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

struct ModeInfo {
    s32 mode;          // offset 0x0
    const char* name;  // offset 0x4
    const char* file;  // offset 0x8
    s32 unkC;          // offset 0xC
    u8 default_allow;  // offset 0x10
    u8 check_names;    // offset 0x11
};

struct Track {
    s32 index;              // offset 0x0
    s32 league;             // offset 0x4
    char track[0x20];       // offset 0x8
    char name[0x40];        // offset 0x28
    s32 sort_major;         // offset 0x68
    s32 sort_minor;         // offset 0x6C
    s32 num_contests;       // offset 0x70
    s32 car_class;          // offset 0x74
    f32 unk78;              // offset 0x78
    ValueNode* chassis;     // offset 0x7C
    ValueNode* node;        // offset 0x80
};

struct MyoTrack {
    Track track;   // offset 0x0
    s32 unk84;     // offset 0x84
};

struct Event {
    s32 index;        // offset 0x0
    s32 track;        // offset 0x4
    u32 type;         // offset 0x8
    s32 laps;         // offset 0xC
    s32 drivers;      // offset 0x10
    u32 terrain;      // offset 0x14
    s32 rounds;       // offset 0x18
    s32 purse;        // offset 0x1C
    ValueNode* node;  // offset 0x20
    u8 enabled;       // offset 0x24
};

struct ValFiles {
    ValueTable* tables[MAX_VAL_FILES];  // offset 0x0
    s32 count;                          // offset 0x80
};

class EveStatic {
public:
    EveStatic(void) {
        unk4 = 123;
        unk0 = 0;
        unk8 = 0;
        unkC = 0;
        unk10 = 0;
    }
    u32 unk0;
    u32 unk4;
    u32 unk8;
    u32 unkC;
    u32 unk10;
};

s32 lbl_80558AD8 = MODE_NONE;

ModeInfo lbl_802FC638[] = {
    {MODE_ARCADE, "arcade", "eves.val", 0, 1, 0},
    {MODE_CAREER, "career", "eves.val", 0, 0, 1},
    {MODE_MULTI, "multi", "eves.val", 0, 0, 0},
    {MODE_DEMO, "demo", "demo_eves.val", 0, 0, 0},
    {MODE_CHALLENGE, "challenge", "eves.val", 0, 0, 0},
};

ModuleTag lbl_8055D4F8;
u8 lbl_8055D500;           // loaded
s32 lbl_8055D504;          // tracks
s32 lbl_8055D508;          // events
s32 lbl_8055D50C;
s32 lbl_8055D510;
ValueNode* lbl_8055D514;   // videotest_e3_vehicle_map
s32 lbl_8055D518;          // make-your-own tracks
s32 lbl_8055D51C;

EveStatic lbl_80351828;
ValFiles lbl_8035183C;
Track lbl_803518C0[MAX_TRACKS];
Event lbl_80352AD0[MAX_EVENTS];
MyoTrack lbl_803551E0[MAX_MYO];

void LoadEves(int mode);
void LoadEveFile(const char* name);
void SortEves(void);
void FixupMyoEves(void);

ModeInfo* GetModeInfo(int mode) {
    int i;

    for (i = 0; i < 5; i++) {
        if (mode == lbl_802FC638[i].mode) {
            break;
        }
    }
    if (i >= 5) {
        fn_802E317C("get_mode_info: can't find mode %d", mode);
    }
    return &lbl_802FC638[i];
}

void LoadArcadeEves(void) {
    LoadEves(MODE_ARCADE);
}

void LoadCareerEves(void) {
    LoadEves(MODE_CAREER);
}

void LoadDemoEves(void) {
    LoadEves(MODE_DEMO);
}

void LoadModeEves(int mode) {
    LoadEves(mode);
}

void UnloadEves(void) {
    int i;

    lbl_80558AD8 = MODE_NONE;
    for (i = 0; i < lbl_8035183C.count; i++) {
        fn_80131818(lbl_8035183C.tables[i]);
    }
    lbl_8035183C.count = 0;
    lbl_8055D500 = 0;
    lbl_8055D514 = NULL;
}

int GetEveMode(void) {
    return lbl_80558AD8;
}

int GetNumTracks(void) {
    return lbl_8055D504;
}

Track* GetTrack(int i) {
    if (i >= 0) {
        return &lbl_803518C0[i];
    }
    return &lbl_803551E0[-1 - i].track;
}

int GetNumEvents(void) {
    return lbl_8055D508;
}

Event* GetEvent(int i) {
    return &lbl_80352AD0[i];
}

// Stripped from ET; only its strings remain.
Event* FindReserveEvent(int track, int n) {
    int k = 0;
    int i;

    for (i = 0; i < lbl_8055D508; i++) {
        if (track == lbl_80352AD0[i].track) {
            if (k == n) {
                return &lbl_80352AD0[i];
            }
            k++;
        }
    }
    fn_802E317C("Eve: Victor sucks.  Tried to find eve %d, reserve contest %d.", track, n);
    return NULL;
}

Event* FindTrackEvent(int track, int n) {
    int k = 0;
    int i;

    for (i = 0; i < lbl_8055D508; i++) {
        if (track == lbl_80352AD0[i].track && lbl_80352AD0[i].enabled) {
            if (k == n) {
                return &lbl_80352AD0[i];
            }
            k++;
        }
    }
    fn_802E317C("Eve: Chris sucks.  Tried to find eve %d, contest %d.", track, n);
    return NULL;
}

const char* GetEventDisplayName(int i) {
    return GetEventTypeDisplayName(GetEvent(i)->type);
}

int GetTrackSizeClass(f32 size) {
    if (size > 16.0f) {
        return 2;
    }
    return 1;
}

int GetEventSizeClass(int i) {
    return GetTrackSizeClass(GetTrack(GetEvent(i)->track)->unk78);
}

void LoadEves(int mode) {
    ModeInfo* info;

    fn_8013C804();
    info = GetModeInfo(mode);
    lbl_8055D514 = NULL;
    lbl_80558AD8 = info->mode;
    lbl_8055D508 = 0;
    lbl_8055D51C = 0;
    lbl_8055D504 = 0;
    lbl_8055D518 = 0;
    if (info->file != NULL) {
        LoadEveFile(info->file);
    }
    if (OptDirectToTrack()) {
        LoadEveFile("drct2trk.val");
    }
    if (OptPaTest()) {
        LoadEveFile("pa_test.val");
    }
    SortEves();
    lbl_8055D50C = lbl_8055D504;
    lbl_8055D510 = lbl_8055D508;
    if (info->check_names) {
        int i;

        for (i = 0; i < lbl_8055D510; i++) {
            char buf[0x80];
            fn_801345D8(buf, sizeof(buf), "%s %s: %s contest has no name value", info->name,
                        lbl_803518C0[lbl_80352AD0[i].track].track, GetEventTypeName(lbl_80352AD0[i].type));
        }
    }
    FixupMyoEves();
    fn_8013C8B8();
    lbl_8055D500 = 1;
}

BOOL IsEveAllowed(ValueNode* node) {
    ModeInfo* info;
    const char* name;
    BOOL allowed;
    const char* modes;

    if (OptDirectToTrack()) {
        return 1;
    }
    info = GetModeInfo(lbl_80558AD8);
    name = info->name;
    allowed = info->default_allow;
    modes = node->GetString("mode", NULL);
    if (modes != NULL) {
        allowed = 0;
        if (strstr(modes, name) != NULL) {
            allowed = 1;
        }
    }
    return allowed;
}

void CheckNoField(const char* key, ValueNode* node, const char* what);

void ParseTrack(Track* t, int index, ValueNode* node, const char* file) {
    char leagues[0x100];
    char title[0xC0];
    const char* chassis;
    const char* value;
    u32 terrain;
    ValueNode* c;
    int i;
    int j;

    memset(t, 0, sizeof(Track));
    t->index = index;
    fn_8022D0D8(leagues, sizeof(leagues));
    for (i = 0; i < 4; i++) {
        const char* name = fn_80137C54(i)->name;
        fn_8022D0E8(leagues, sizeof(leagues), name);
    }
    {
        const char* key = "league";
        t->league = node->Has(key) ? node->GetEnum(key, leagues) : 0;
    }
    COPY_STRING(t->track, node->GetValue("track"), sizeof(t->track));
    if (FindTrack(t->track) == -1) {
        fn_802E317C("Eve(%s):  I don't know track '%s'", file, t->track);
    }
    t->sort_minor = 1;
    t->sort_major = 1;
    strncpy(t->name, node->Has("name2") ? node->GetValue("name2") : node->GetValue("name"), sizeof(t->name));
    t->name[sizeof(t->name) - 1] = '\0';
    if (node->Has("car_class")) {
        t->car_class = fn_8013C9A4(node->GetValue("car_class"));
        if (t->car_class == 6) {
            fn_802E317C("Eve(%s): Invalid car_class name for eve %s", file, t->name);
        }
    } else {
        t->car_class = 1;
    }
    t->unk78 = 12.0f;
    t->chassis = node->FindChild("chassis_allowed");
    if (t->chassis != NULL) {
        for (i = 0; t->chassis->GetPair(i, &chassis, &value); i++) {
            if (fn_8013C8E8(chassis) == 6) {
                fn_802E317C("Eve: %s chassis allowed has invalid chassis %s", t->name, chassis);
            }
        }
        if (i > MAX_CHASSIS_ALLOWED) {
            fn_802E317C("Eve: %s has too many chassis allowed (%d requested, %d max)", t->name, i,
                        MAX_CHASSIS_ALLOWED);
        }
    }
    t->node = node;
    CheckNoField("event_type", node, t->name);
    CheckNoField("laps", node, t->name);
    CheckNoField("drivers", node, t->name);
    CheckNoField("opt_terrain", node, t->name);
    CheckNoField("max_cars", node, t->name);
    CheckNoField("purse", node, t->name);
    if (node->Has("speed_factor") || node->Has("skill_range")) {
        if (!node->Has("speed_factor") || !node->Has("skill_range")) {
            fn_802E317C("eve: Eve %s must have both speed_factor and skill_range or neither", t->name);
        }
    }
    terrain = node->GetInt("terrain_mask", 0);
    for (j = 0; (c = node->GetChildAt(j)) != NULL; j++) {
        if (strcasecmp(c->GetName(), "contest") == 0) {
            Event* e;
            s32 opt;

            if (lbl_8055D508 >= MAX_EVENTS) {
                fn_802E317C("Eve: too many overall contests (%d), ask a programmer to increase", lbl_8055D508);
            }
            if (t->num_contests >= MAX_CONTESTS) {
                fn_802E317C("Eve: %s has too many contests (%d), ask a programmer to increase", t->name,
                            t->num_contests);
            }
            e = &lbl_80352AD0[lbl_8055D508];
            e->index = lbl_8055D508;
            e->track = t->index;
            e->enabled = 1;
            if (c->Has("event_type")) {
                e->type = FindEventType(c->GetValue("event_type"));
                if (e->type == 0) {
                    fn_802E317C("Eve %s, contest %d has bad event_type", t->name, lbl_8055D508 + 1);
                }
            } else {
                fn_802E317C("Eve %s, contest %d has no event_type", t->name, lbl_8055D508 + 1);
            }
            sprintf(title, "%s: %s", t->name, GetEventTypeDisplayName(e->type));
            e->laps = c->GetInt("laps", 0);
            e->drivers = c->GetInt("max_cars", MAX_CARS);
            if (e->drivers > MAX_CARS) {
                fn_802E317C("Eve: %s has %d cars, %d is max", title, e->drivers, MAX_CARS);
            }
            e->terrain = c->GetInt("terrain_mask", -1);
            if (e->terrain == 0xFFFFFFFF) {
                opt = c->GetInt("opt_terrain", 0);
                if (opt != 0) {
                    e->terrain = 1 << (opt - 1);
                } else {
                    e->terrain = 0;
                }
            }
            e->terrain |= terrain;
            e->terrain &= ~c->GetInt("terrain_disable_mask", 0);
            e->rounds = c->GetInt("rounds", 1);
            e->purse = c->GetInt("purse", 1000);
            e->node = c;
            CheckNoField("league", c, title);
            CheckNoField("date", c, title);
            CheckNoField("time", c, title);
            if (c->Has("speed_factor") || c->Has("skill_range")) {
                if (!c->Has("speed_factor") || !c->Has("skill_range")) {
                    fn_802E317C("Eve: %s must have both speed_factor and skill_range or neither", title);
                }
            }
            if ((e->type & 0x400) && e->drivers * 2 > MAX_CARS) {
                fn_802E317C("Eve: %s has too many vehicles (%d cars + %d trailers > %d max)", title, e->drivers,
                            e->drivers, MAX_CARS);
            }
            lbl_8055D508++;
            t->num_contests++;
        }
    }
    if (t->num_contests <= 0) {
        fn_802E317C("Eve: eve %s has no contests!", t->name);
    }
}

s32 GetIntOr(ValueNode* node, const char* key, s32 def) {
    if (node->Has(key)) {
        return node->GetIntValue(key);
    }
    return def;
}

void ParseEveTable(ValueTable* table, const char* file) {
    ValueNode* c;
    int i;

    if (table != NULL) {
        for (i = 0; (c = table->GetChild(i)) != NULL; i++) {
            if (strcasecmp(c->GetName(), "include") == 0) {
                if (!fn_8012C33C(c->GetValue("name"))) {
                    fn_802E317C("eve: Can't include \"%s\", no such resource", c->GetValue("name"));
                }
                LoadEveFile(c->GetString("name", NULL));
            } else if (strcasecmp(c->GetName(), "eve") == 0) {
                if (lbl_8055D504 >= MAX_TRACKS) {
                    fn_802E317C("Too many eves (more than %d) increase MAX_EVES", lbl_8055D504 - 1);
                }
                if (IsEveAllowed(c)) {
                    ParseTrack(&lbl_803518C0[lbl_8055D504], lbl_8055D504, c, file);
                    lbl_8055D504++;
                }
            } else if (strcasecmp(c->GetName(), "myo_eve") == 0) {
                if (lbl_8055D518 >= MAX_TRACKS) {
                    fn_802E317C("Too many make-your-own eves (more than %d) - increase MAX_EVES", lbl_8055D518 - 1);
                }
                if (IsEveAllowed(c)) {
                    ParseTrack(&lbl_803551E0[lbl_8055D518].track, -1 - lbl_8055D518, c, file);
                    lbl_803551E0[lbl_8055D518].unk84 = 0;
                    lbl_8055D518++;
                }
            } else if (strcasecmp(c->GetName(), "videotest_e3_vehicle_map") == 0) {
                if (lbl_8055D514 == NULL) {
                    lbl_8055D514 = c;
                }
            }
        }
    }
}

void CheckNoField(const char* key, ValueNode* node, const char* what) {
    if (node->Has(key)) {
        fn_802E317C("Eve: %s shouldn't have a %s field", what, key);
    }
}

void LoadEveFile(const char* name) {
    if (name != NULL && fn_8012C33C(name)) {
        ValueTable* table = fn_80131808(name);

        lbl_8035183C.tables[lbl_8035183C.count++] = table;
        ParseEveTable(table, name);
    }
}

int CompareDisplayOrder(const void* a, const void* b) {
    s32 oa = ((const Track*)a)->node->GetInt("display_order", 99);
    return oa - ((const Track*)b)->node->GetInt("display_order", 99);
}

// Stripped from ET; only its strings remain.
const char* GetTrackDisplayName(int i) {
    if (i < 0) {
        return "Make Your Own";
    }
    return GetTrack(i)->name;
}

int CompareSortKeys(const void* a, const void* b) {
    const Track* ta = (const Track*)a;
    const Track* tb = (const Track*)b;

    if (ta->sort_major == tb->sort_major) {
        return ta->sort_minor - tb->sort_minor;
    }
    return ta->sort_major - tb->sort_major;
}

void SortEves(void) {
    int i;

    if (lbl_80558AD8 == MODE_CHALLENGE) {
        qsort(lbl_803518C0, lbl_8055D504, sizeof(Track), CompareDisplayOrder);
        qsort(lbl_803551E0, lbl_8055D518, sizeof(MyoTrack), CompareDisplayOrder);
    } else if (lbl_80558AD8 == MODE_CAREER) {
        qsort(lbl_803518C0, lbl_8055D504, sizeof(Track), CompareSortKeys);
        qsort(lbl_803551E0, lbl_8055D518, sizeof(MyoTrack), CompareSortKeys);
    }
    for (i = 0; i < lbl_8055D508; i++) {
        BOOL found = 0;
        int j;
        int k;

        for (j = 0; j < lbl_8055D504; j++) {
            if (lbl_803518C0[j].index == lbl_80352AD0[i].track) {
                lbl_80352AD0[i].track = j;
                found = 1;
                break;
            }
        }
        if (!found) {
            for (k = 0; k < lbl_8055D518; k++) {
                if (lbl_803551E0[k].track.index == lbl_80352AD0[i].track) {
                    lbl_80352AD0[i].track = -1 - k;
                    break;
                }
            }
        }
    }
    for (i = 0; i < lbl_8055D504; i++) {
        lbl_803518C0[i].index = i;
    }
    for (i = 0; i < lbl_8055D518; i++) {
        lbl_803551E0[i].track.index = -1 - i;
    }
}

void FixupMyoEves(void) {
    int i;
    int j;

    {
        MyoTrack* t = lbl_803551E0;
        for (i = 0; i < lbl_8055D518; i++, t++) {
            t->unk84 += t->track.num_contests;
            t->track.num_contests = 0;
        }
    }
    for (i = 0; i < lbl_8055D508; i++) {
        for (j = 0; j < lbl_8055D518; j++) {
            if (lbl_80352AD0[i].track == lbl_803551E0[j].track.index) {
                lbl_80352AD0[i].enabled = 0;
            }
        }
    }
}
