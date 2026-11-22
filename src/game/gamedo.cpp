// Single events (0x8023B764-0x8023C2B4): sets up and runs one event, head-to-head sessions and
// direct-to-track starts from the command line. Names are guesses apart from the messages.
#include "game/common.h"
#include "game/davmap.h"
#include "game/racesetup.h"
#include "game/gamedb.h"

extern "C" {
int atoi(const char* s);
int fn_80128314(int heap);
int fn_80265D58(void);
}

#include "game/raceresultentry.h"

// The setup gamedo runs an event with: the race setup, then its driver/vehicle map. Name unproven.
struct GameDoSetup : RaceSetup {
    DAVMap dav;  // offset 0x3F8
};

#include "game/gamelistener.h"

// Keeps the result of one race.
class ResultListener : public GameListener {
public:
    virtual void Begin(void) {}
    virtual void End(RaceStandings* r) { result = *r; }
    virtual void Finish(void) {}
    virtual ~ResultListener(void) {}

    RaceStandings result;  // offset 0x4
};

// A screen run between head-to-head races.
#include "game/sessionscreen.h"

#include "game/heapmark.h"

BOOL OptDirectToTrack(void);
void BuildDAVMap(DAVMap* out, s32 track);
void BuildReversedMap(DAVMap* out, s32 event, const DAVMap* src);
void SetNumPlayers(int n);
void ClearPlayer(int slot);
void SetPlayer(int slot, s32 id, s32 vehicle, u32 color);
void SetPlayerName(const char* name, int slot);
void AchievementsClearSession(void);
int IdentGetCurrentSlot(void);
const char* IdentGetSlotName(int slot);
void UnloadEves(void);
void LoadModeEves(int mode);
void LoadArcadeEves(void);
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
void InitializeCareerEvents(void);
void fn_8020A600(void);
int fn_802ECEEC(void);
void fn_802ECEC0(void);
void fn_802ECEC4(void);
int fn_80128314(int heap);
int fn_80265D58(void);
int fn_80204024(void);
void fn_8012BF28(void);
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

int GetMirrorModeSetting(int track, int eve) { return fn_802054EC("mirror_mode", track, eve); }

void fn_8023B77C(void) {
    int left = 297;
    int right = 397;
    if (!fn_802C9A58()) {
        left = 310;
        right = 444;
    }
    fn_802CED50(left * 640 / 512, 392, (right - left) * 640 / 512, 31);
}

void SetRaceTrackFromEvent(RaceSetup* rs, int track, int eve);

void SetupGameDo(GameDoSetup* rs, int track, int eve, int difficulty) {
    fn_802ECC18(0);
    fn_802445FC(track, eve, difficulty);
    fn_8023B77C();
    fn_8020A634();
    fn_8027B77C();
    fn_80211F4C();
    SetRaceTrackFromEvent(rs, track, eve);
    BuildDAVMap(&rs->dav, track);
    fn_800126B4(rs, &rs->dav, 0, 1);
    DAVMap rev;
    BuildReversedMap(&rev, rs->opts.event, &rs->dav);
    fn_8000AB48(rs, &rev);
}

void fn_8023B8AC(GameDoSetup* rs) {
    fn_8000ABE0(rs);
    fn_800126C0(rs, &rs->dav);
    fn_80244738();
    fn_8023B77C();
    fn_80211F44();
    if (!OptDirectToTrack()) {
        InitializeCareerEvents();
    }
    fn_8020A600();
}

void fn_8023B904(GameDoSetup* rs, RaceStandings* out, int skip) {
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

void RunSinglePlayerEvent(int track, int eve, RaceStandings* out, int difficulty, int skip) {
    int paused = fn_802ECEEC();
    if (paused) {
        fn_802ECEC0();
    }
    do {
        HeapMark mark("msingle:do_event()");
        if (fn_80204024()) {
            fn_8012BF28();
        }
        GameDoSetup rs;
        SetupGameDo(&rs, track, eve, difficulty);
        rs.opts.level = 0;
        rs.opts.difficulty = difficulty;
        if (GetMirrorModeSetting(difficulty, rs.opts.event)) {
            fn_802E30C8("Enabling Mirror Mode!!");
            rs.opts.flags |= 2;
        }
        fn_8023B904(&rs, out, skip);
        fn_8023B8AC(&rs);
    } while (fn_80204024());
    fn_80129AEC(0);
    if (paused) {
        fn_802ECEC4();
    }
}

void InitializePlayers(void) {
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
            InitializePlayers();
            RunSinglePlayerEvent(track, eve, NULL, difficulty, 0);
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

void RunPlayerRace(void) {
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
            RaceStandings result;
            RunSinglePlayerEvent(track, eve, &result, difficulty, skip);
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

void LoadRaceModeEves(const char* name, int headToHead) {
    UnloadEves();
    LoadModeEves(0x130);
    fn_802411C8();
    if (headToHead) {
        fn_8023BBA8();
    } else {
        while (fn_8023FF1C(headToHead) != 1) {
            InitializePlayers();
            fn_8023B5E0();
        }
    }
    fn_8023BDEC();
    UnloadEves();
    LoadArcadeEves();
}

void StartHeadToHeadMode(void) { LoadRaceModeEves("GameDoHeadToHead", 1); }

void fn_8023BEB8(int track, int eve, RaceStandings* out) { RunSinglePlayerEvent(track, eve, out, 0, 0); }

void fn_8023BEC4(int index, RaceStandings* out) {
    s32 track = GetEvent(index)->track;
    int eve = -1;
    int i = GetTrack(track)->events;
    while (i-- > 0) {
        if (index == FindTrackEvent(track, i)->id) {
            eve = i;
        }
    }
    fn_8023BEB8(track, eve, out);
}

struct EventStart {
    int track;
    int eve;
    int difficulty;
    RaceStandings* out;
};

void fn_8023BF4C(EventStart* s) { RunSinglePlayerEvent(s->track, s->eve, s->out, s->difficulty, 0); }

void ValidatePlayerCount(const char* arg) {
    int n = atoi(arg);
    lbl_8055BB30 = n;
    if (n == 0 || __abs(n) > 4) {
        fn_802E317C("Can't set %d players.  That's crazy.", n);
    }
}

void GameDoEve(int track, int eve) {
    TrackInfo* t = GetTrack(track);
    EventInfo* e = FindTrackEvent(track, eve);
    fn_802E30C8("GameDoEve: eve [%s] contest [%s] at %s", t->title,
                e->node->GetString("name", GetEventTypeDisplayName(e->type)), t->name);
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

void SetRaceTrackFromEvent(RaceSetup* rs, int track, int eve) {
    TrackInfo* t = GetTrack(track);
    EventInfo* e = FindTrackEvent(track, eve);
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
        const char* s = t->name;
        int n = sizeof(rs->track);
        char* d = rs->track;
        while ((*d++ = *s++) != '\0') {
            if (--n <= 0) {
                d[-1] = '\0';
                break;
            }
        }
    }
    fn_8000824C(&rs->opts.event, e->id);
}

}  // extern "C"
