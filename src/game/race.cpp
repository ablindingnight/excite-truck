// Race/track registry (0x80007D94-0x80008390): reads the track list from tracks.val, maps event
// type ids to names, and holds the camera-name strings. The "race:" prefix of its messages
// suggests race.cpp; the function names are guesses.
#include <string.h>
#include <stdlib.h>
#include "game/common.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
void qsort(void* base, size_t num, size_t size, int (*cmp)(const void*, const void*));
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* table);
void fn_80279464(void);
void fn_80279488(void);
}
void LoadArcadeEves(void);
void UnloadEves(void);

struct Unk_8001285C {
    s32 unk0;
    u32 unk4;
    s32 unk8;
    s32 unkC;
};

Unk_8001285C* GetEvent(int id);

class LocString {
public:
    LocString(const char* key);
    u8 unk0[0xC];
};

struct EventType {
    u32 id;
    const char* name;
    const char* display_name;
};

struct TrackEntry {
    const char* name;
    const char* friendly_name;
    const char* environ;
};

struct TrackList {
    TrackEntry entries[40];  // offset 0x0
    u32 count;               // offset 0x1E0
    ValueTable* table;       // offset 0x1E4
};

struct Unk_8000822C {
    s32 id;     // offset 0x0
    s32 unk4;   // offset 0x4
    s32 unk8;   // offset 0x8
    s32 unkC;   // offset 0xC
    u32 flags;  // offset 0x10
};

ModuleTag lbl_8055D358;
static UnkStatic123 lbl_8034EF20;
TrackList lbl_8034EF38;

EventType lbl_802FB70C[] = {
    {0x180201, "zone", "Zone"},
    {0x180302, "excite_beta_race", "Excite"},
    {0x180303, "freestyle", "Freestyle"},
    {0x180304, "crush", "Crush"},
    {0, NULL, NULL},
};

int CompareTracks(const void* a, const void* b) {
    return strcasecmp(((const TrackEntry*)a)->name, ((const TrackEntry*)b)->name);
}

void LoadTracks(void) {
    int i;
    ValueNode* track;

    lbl_8034EF38.table = fn_80131808("tracks.val");
    lbl_8034EF38.count = 0;
    i = 0;
    for (;;) {
        track = lbl_8034EF38.table->GetChild(i++);
        if (track == NULL) {
            break;
        }
        if (strcasecmp(track->GetName(), "track") == 0) {
            lbl_8034EF38.entries[lbl_8034EF38.count].name = track->GetValue("name");
            lbl_8034EF38.entries[lbl_8034EF38.count].friendly_name = track->GetString("friendly_name", "");
            lbl_8034EF38.entries[lbl_8034EF38.count].environ = track->GetString("environ", "");
            if (++lbl_8034EF38.count >= 40) {
                fn_802E317C("race: Too many tracks!");
                break;
            }
        }
    }
    qsort(lbl_8034EF38.entries, lbl_8034EF38.count, sizeof(TrackEntry), CompareTracks);
}

void FreeTracks(void) {
    fn_80131818(lbl_8034EF38.table);
    lbl_8034EF38.table = NULL;
}

void RaceInit(void) {
    LoadTracks();
    fn_80279464();
    LoadArcadeEves();
}

void RaceShutdown(void) {
    UnloadEves();
    fn_80279488();
    FreeTracks();
}

const char* GetEventTypeName(u32 type) {
    EventType* e;

    for (e = lbl_802FB70C; e->id != 0; e++) {
        if (e->id == type) {
            return e->name;
        }
    }
    fn_802E317C("race: can't find event type for 0x%x", type);
    return "?";
}

const char* GetEventTypeDisplayName(u32 type) {
    EventType* e;

    for (e = lbl_802FB70C; e->id != 0; e++) {
        if (e->id == type) {
            return e->display_name;
        }
    }
    fn_802E317C("race: can't find event type for 0x%x", type);
    return "unknown";
}

u32 FindEventType(const char* name) {
    u32 id;
    EventType* e;

    for (e = lbl_802FB70C; (id = e->id) != 0; e++) {
        if (strcmp(e->name, name) == 0) {
            return id;
        }
    }
    fn_802E30C8("race: No such event identifier: \"%s\"", name);
    return 0;
}

// Stripped from ET; only its strings remain.
const char* GetSkillName(int skill) {
    switch (skill) {
    case 0:
        return "1 Rookie";
    case 1:
        return "3 Veteran";
    default:
        return "5 Legend";
    }
}

int GetTrackCount(void) {
    return lbl_8034EF38.count;
}

const char* GetTrackName(int idx);

int FindTrack(const char* name) {
    int i;

    for (i = 0; i < GetTrackCount(); i++) {
        if (strcasecmp(name, GetTrackName(i)) == 0) {
            return i;
        }
    }
    return -1;
}

const char* GetTrackName(int idx) {
    if (idx >= 0 && idx < (int)lbl_8034EF38.count) {
        return lbl_8034EF38.entries[idx].name;
    }
    return "invalid";
}

const char* GetTrackFriendlyName(const char* name) {
    return lbl_8034EF38.entries[FindTrack(name)].friendly_name;
}

const char* GetTrackEnviron(const char* name) {
    return lbl_8034EF38.entries[FindTrack(name)].environ;
}

extern "C" s32 fn_800081F0(Unk_8000822C* obj) {
    if ((obj->flags & 1) == 0) {
        return GetEvent(obj->id)->unkC;
    } else {
        return 200;
    }
}

extern "C" void fn_8000822C(Unk_8000822C* obj) {
    obj->id = -1;
    obj->unk4 = 0;
    obj->flags = 0;
    obj->unkC = 0;
    obj->unk8 = 0;
}

extern "C" void fn_8000824C(Unk_8000822C* obj, s32 id) {
    Unk_8001285C* src;

    fn_8000822C(obj);
    src = GetEvent(id);
    obj->id = src->unk0;
    obj->unk4 = src->unk8;
}

// Twelve separate statics, not an array: MWCC then pools their .bss addresses off one base
// register in the static initializer, as in the original.
static LocString s_cameraCockpit("CameraNames:Cockpit");
static LocString s_cameraBumper("CameraNames:Bumper");
static LocString s_cameraChassis("CameraNames:Chassis");
static LocString s_cameraRear("CameraNames:Rear");
static LocString s_cameraNearChase("CameraNames:NearChase");
static LocString s_cameraChase("CameraNames:Chase");
static LocString s_cameraFarChase("CameraNames:FarChase");
static LocString s_cameraRearChase("CameraNames:RearChase");
static LocString s_cameraOverhead("CameraNames:Overhead");
static LocString s_cameraAerial("CameraNames:Aerial");
static LocString s_cameraBlimp("CameraNames:Blimp");
static LocString s_cameraTV("CameraNames:TV");
