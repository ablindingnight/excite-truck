// Single events (0x8023B764-0x8023C2B4): sets up and runs one event, head-to-head sessions and
// direct-to-track starts from the command line. Names are guesses apart from the messages.
#include "game/common.h"

extern "C" {
int atoi(const char* s);
int fn_80128314(int heap);
int fn_80265D58(void);
}

// The drivers/vehicles of one race.
struct DAVMap {
    s32 num_drivers;   // offset 0x0
    s32 drivers[8];    // offset 0x4
    s32 num_vehicles;  // offset 0x24
    s32 vehicles[8];   // offset 0x28
};

struct RaceResultEntry {
    u8 pad0[0x10];
    s32 value;  // offset 0x10
};

struct RaceResult {
    s32 count;   // offset 0x0
    s32 unk4;    // offset 0x4
    s32 races;   // offset 0x8
    u8 unkC;     // offset 0xC
    u8 finished; // offset 0xD
    u8 aborted;  // offset 0xE
    RaceResultEntry entries[8];  // offset 0x10
};

struct RaceSetup {
    char track[0x20];     // offset 0x0
    u8 pad20[0x3C4];
    s32 event;            // offset 0x3E4
    u8 pad3E8[4];
    s32 difficulty;       // offset 0x3EC
    s32 unk3F0;           // offset 0x3F0
    u32 flags;            // offset 0x3F4
    DAVMap dav;           // offset 0x3F8
};

class GameListener {
public:
    virtual void Begin(void);              // 0x8
    virtual void End(RaceResult* result);  // 0xC
    virtual void Finish(void);             // 0x10
};

// Keeps the result of one race.
class ResultListener : public GameListener {
public:
    virtual void Begin(void) {}
    virtual void End(RaceResult* r) { result = *r; }
    virtual void Finish(void) {}
    virtual ~ResultListener(void) {}

    RaceResult result;  // offset 0x4
};

// A screen run between head-to-head races.
class SessionScreen {
public:
    virtual ~SessionScreen(void) {}
};

// Heap use at the start of an event, for leak checks.
class HeapMark {
public:
    HeapMark(const char* name) {
        this->name = name;
        heap0 = fn_80128314(0);
        heap1 = fn_80128314(1);
        other = fn_80265D58();
    }
    ~HeapMark(void) {}

    const char* name;
    int heap0;
    int heap1;
    int other;
};

struct Track {
    s32 index;          // offset 0x0
    s32 league;         // offset 0x4
    char track[0x20];   // offset 0x8
    char name[0x40];    // offset 0x28
    s32 sort_major;     // offset 0x68
    s32 sort_minor;     // offset 0x6C
    s32 num_contests;   // offset 0x70
};

struct Event {
    s32 index;        // offset 0x0
    s32 track;        // offset 0x4
    u32 type;         // offset 0x8
    u8 padC[0x14];
    ValueNode* node;  // offset 0x20
};

struct DriverInfo {
    s32 index;        // offset 0x0
    s32 id;           // offset 0x4
    char name[0x40];  // offset 0x8
    s32 rank;         // offset 0x48
    s32 vehicle;      // offset 0x4C
    u32 color;        // offset 0x50
};

struct VehicleInfo {
    s32 index;  // offset 0x0
    s32 id;     // offset 0x4
};

BOOL OptDirectToTrack(void);
void BuildDAVMap(DAVMap* out, s32 track);
void BuildReversedMap(DAVMap* out, s32 event, const DAVMap* src);
void SetNumPlayers(int n);
DriverInfo* GetDriver(int i);
VehicleInfo* GetVehicle(int i);
void ClearPlayer(int slot);
void SetPlayer(int slot, s32 id, s32 vehicle, u32 color);
void SetPlayerName(const char* name, int slot);
void AchievementsClearSession(void);
int IdentGetCurrentSlot(void);
const char* IdentGetSlotName(int slot);
void UnloadEves(void);
void LoadModeEves(int mode);
void LoadArcadeEves(void);
Event* GetEvent(int i);
Track* GetTrack(int i);
Event* FindTrackEvent(int track, int eve);
const char* GetEventTypeDisplayName(u32 type);

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
int fn_802054EC(const char* key, int track, int eve);
int fn_802C9A58(void);
void fn_802CED50(int x, int y, int w, int h);
void fn_802ECC18(int a);
void fn_802445FC(int track, int eve, int difficulty);
void fn_8020A634(void);
void fn_8027B77C(void);
void fn_80211F4C(void);
void fn_800126B4(RaceSetup* rs, DAVMap* dav, int a, int b);
void fn_800126C0(RaceSetup* rs, DAVMap* dav);
void fn_80244738(void);
void fn_80211F44(void);
void fn_8027B690(void);
void fn_8020A600(void);
int fn_802ECEEC(void);
void fn_802ECEC0(void);
void fn_802ECEC4(void);
int fn_80128314(int heap);
int fn_80265D58(void);
int fn_80204024(void);
void fn_8012BF28(void);
void fn_801E952C(RaceSetup* rs);
void fn_80129AEC(int a);
int fn_802411CC(void);
void fn_802411C8(void);
void fn_802411D0(int i, s32* vehicle, u32* color, int* pad);
void fn_80137C84(int pad, int slot);
int fn_8023FF1C(int a);
int fn_8024A4CC(int* track, int* eve, int* difficulty);
int fn_8024A4C4(int* track, int* eve, int* difficulty);
SessionScreen* fn_8027DB40(int a);
int fn_8024DAE0(s32* vehicle, u32* color, int a, SessionScreen* screen, int b);
void fn_80137C78(int n);
int fn_802E4A10(void);
void fn_80137C80(int slot);
int fn_80137C74(void);
void fn_80137C7C(s32 id, int slot);
void fn_8023B5E0(void);
void fn_8000824C(s32* event, s32 index);
extern u8 lbl_8055F291;
}

// gameloop.cpp / raceload.cpp (C++ linkage)
void fn_8000AB48(RaceSetup* rs, DAVMap* dav);
void fn_8000ABE0(RaceSetup* rs);
void fn_8000BF20(RaceSetup* rs, GameListener* listener);
void fn_8000C0E8(RaceSetup* rs);

static inline int Abs(int v) {
    int m = v >> 31;
    return (m ^ v) - m;
}

ModuleTag lbl_8055F130;
static UnkStatic123 lbl_80498770;
int lbl_8055BB30 = 1;  // players for direct-to-track starts

extern "C" {

int fn_8023B764(int track, int eve) { return fn_802054EC("mirror_mode", track, eve); }

void fn_8023B77C(void) {
    int left = 297;
    int right = 397;
    if (!fn_802C9A58()) {
        left = 310;
        right = 444;
    }
    fn_802CED50(left * 640 / 512, 392, (right - left) * 640 / 512, 31);
}

void fn_8023C0EC(RaceSetup* rs, int track, int eve);

void fn_8023B7EC(RaceSetup* rs, int track, int eve, int difficulty) {
    fn_802ECC18(0);
    fn_802445FC(track, eve, difficulty);
    fn_8023B77C();
    fn_8020A634();
    fn_8027B77C();
    fn_80211F4C();
    fn_8023C0EC(rs, track, eve);
    BuildDAVMap(&rs->dav, track);
    fn_800126B4(rs, &rs->dav, 0, 1);
    DAVMap rev;
    BuildReversedMap(&rev, rs->event, &rs->dav);
    fn_8000AB48(rs, &rev);
}

void fn_8023B8AC(RaceSetup* rs) {
    fn_8000ABE0(rs);
    fn_800126C0(rs, &rs->dav);
    fn_80244738();
    fn_8023B77C();
    fn_80211F44();
    if (!OptDirectToTrack()) {
        fn_8027B690();
    }
    fn_8020A600();
}

void fn_8023B904(RaceSetup* rs, RaceResult* out, int skip) {
    if (!skip) {
        ResultListener listener;
        fn_8000BF20(rs, &listener);
        if (out) {
            *out = listener.result;
        }
    } else {
        fn_8000C0E8(rs);
    }
}

void fn_8023B9EC(int track, int eve, RaceResult* out, int difficulty, int skip) {
    int paused = fn_802ECEEC();
    if (paused) {
        fn_802ECEC0();
    }
    do {
        HeapMark mark("msingle:do_event()");
        if (fn_80204024()) {
            fn_8012BF28();
        }
        RaceSetup rs;
        fn_801E952C(&rs);
        fn_8023B7EC(&rs, track, eve, difficulty);
        rs.unk3F0 = 0;
        rs.difficulty = difficulty;
        if (fn_8023B764(difficulty, rs.event)) {
            fn_802E30C8("Enabling Mirror Mode!!");
            rs.flags |= 2;
        }
        fn_8023B904(&rs, out, skip);
        fn_8023B8AC(&rs);
    } while (fn_80204024());
    fn_80129AEC(0);
    if (paused) {
        fn_802ECEC4();
    }
}

void fn_8023BB0C(void) {
    int n = fn_802411CC();
    SetNumPlayers(n);
    for (int i = 0; i < n; i++) {
        s32 id = GetDriver(i)->id;
        s32 vehicle;
        int pad;
        u32 color;
        fn_802411D0(i, &vehicle, &color, &pad);
        fn_80137C84(pad, i);
        ClearPlayer(i);
        SetPlayer(i, id, vehicle, color);
    }
}

void fn_8023BBA8(void) {
    int track = 0;
    int eve = 0;
    int difficulty = 0;
    while (fn_8024A4CC(&track, &eve, &difficulty)) {
        fn_802411C8();
        if (fn_8023FF1C(1) != 1) {
            fn_8023BB0C();
            fn_8023B9EC(track, eve, NULL, difficulty, 0);
            AchievementsClearSession();
        }
    }
}

int fn_8023BC24(s32* id, s32* vehicle, u32* color, int unused) {
    SessionScreen* screen = fn_8027DB40(1);
    int r = fn_8024DAE0(vehicle, color, 0, screen, 0);
    delete screen;
    return r;
}

void fn_8023BCEC(void) {
    fn_80137C78(1);
    fn_80137C84(fn_802E4A10(), 0);
    s32 id = GetDriver(0)->id;
    s32 vehicle = GetVehicle(GetDriver(0)->vehicle)->id;
    u32 color = GetDriver(0)->color;
    int track = 0;
    int eve = 0;
    int difficulty = 0;
    while (fn_8024A4C4(&track, &eve, &difficulty)) {
        if (fn_8023BC24(&id, &vehicle, &color, 0)) {
            u8 skip = lbl_8055F291;
            SetPlayerName(IdentGetSlotName(IdentGetCurrentSlot()), 0);
            SetPlayer(0, id, vehicle, color);
            RaceResult result;
            fn_8023B9EC(track, eve, &result, difficulty, skip);
            fn_80137C80(0);
        }
    }
}

void fn_8023BDEC(void) {
    for (int i = 0; i < fn_80137C74(); i++) {
        fn_80137C80(i);
    }
    fn_80137C78(1);
}

void fn_8023BE38(const char* name, int headToHead) {
    UnloadEves();
    LoadModeEves(0x130);
    fn_802411C8();
    if (headToHead) {
        fn_8023BBA8();
    } else {
        while (fn_8023FF1C(headToHead) != 1) {
            fn_8023BB0C();
            fn_8023B5E0();
        }
    }
    fn_8023BDEC();
    UnloadEves();
    LoadArcadeEves();
}

void fn_8023BEA8(void) { fn_8023BE38("GameDoHeadToHead", 1); }

void fn_8023BEB8(int track, int eve, RaceResult* out) { fn_8023B9EC(track, eve, out, 0, 0); }

void fn_8023BEC4(int index, RaceResult* out) {
    s32 track = GetEvent(index)->track;
    int eve = -1;
    int i = GetTrack(track)->num_contests;
    while (i-- > 0) {
        if (index == FindTrackEvent(track, i)->index) {
            eve = i;
        }
    }
    fn_8023BEB8(track, eve, out);
}

struct EventStart {
    int track;
    int eve;
    int difficulty;
    RaceResult* out;
};

void fn_8023BF4C(EventStart* s) { fn_8023B9EC(s->track, s->eve, s->out, s->difficulty, 0); }

void fn_8023BF68(const char* arg) {
    int n = atoi(arg);
    lbl_8055BB30 = n;
    if (n == 0 || __abs(n) > 4) {
        fn_802E317C("Can't set %d players.  That's crazy.", n);
    }
}

void fn_8023BFBC(int track, int eve) {
    Track* t = GetTrack(track);
    Event* e = FindTrackEvent(track, eve);
    fn_802E30C8("GameDoEve: eve [%s] contest [%s] at %s", t->name,
                e->node->GetString("name", GetEventTypeDisplayName(e->type)), t->track);
    int i;
    int n = Abs(lbl_8055BB30);
    fn_80137C78(n);
    for (i = 0; i < n; i++) {
        if (lbl_8055BB30 >= 0) {
            fn_80137C84(fn_802E4A10(), i);
        } else {
            fn_80137C84(i, i);
        }
        fn_80137C7C(GetDriver(i)->id, i);
    }
    fn_8023BEB8(track, eve, NULL);
    for (i = 0; i < n; i++) {
        fn_80137C80(i);
    }
}

void fn_8023C0EC(RaceSetup* rs, int track, int eve) {
    Track* t = GetTrack(track);
    Event* e = FindTrackEvent(track, eve);
    if (e->node->Has("track")) {
        const char* s = e->node->GetValue("track");
        int n = sizeof(rs->track);
        char* d = rs->track;
        while ((*d++ = *s++) != '\0') {
            if (--n <= 0) {
                d[-1] = '\0';
                break;
            }
        }
    } else {
        const char* s = t->track;
        int n = sizeof(rs->track);
        char* d = rs->track;
        while ((*d++ = *s++) != '\0') {
            if (--n <= 0) {
                d[-1] = '\0';
                break;
            }
        }
    }
    fn_8000824C(&rs->event, e->index);
}

}  // extern "C"
