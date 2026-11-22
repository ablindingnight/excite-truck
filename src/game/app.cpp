// Excite Truck's application layer (0x800049C0-0x800053CC): startup and shutdown of every
// subsystem, the attract movie, command-line switches and "direct to track" launching. The file
// name is a guess (it holds AppMain); the engine's own names are unknown, so callees keep their
// addresses.
#include <types.h>
#include <string.h>
#include <stdio.h>
#include "game/common.h"
#include "game/gamedb.h"

void FlagsInit(void);
void FlagsShutdown(void);
void RepInit(void);
void RepShutdown(void);
void DriverInit(void);
void DriverShutdown(void);
void StatsInit(void);
void StatsShutdown(void);
void LoadingInitHook(void);
void LoadingShutdownHook(void);
void AchievementsShutdown(void);
void AchievementsInit(void);
void IdentShutdown(void);
void IdentInit(void);
void UnlocksShutdown(void);
void UnlocksInit(void);
void ParseOptions(const char* cmdline);
BOOL OptDirectToTrack(void);
BOOL OptPaTest(void);
BOOL OptModelTest(void);
BOOL OptGxTest(void);
void RaceShutdown(void);
void RaceInit(void);
void LoadTimerShutdown(void);
void LoadTimerProgress(void);
void LoadTimerBegin(const char* name, int, int, f32 duration);
void LoadTimerInit(void);

extern "C" {
int strcasecmp(const char* a, const char* b);

void fn_802E30C8(const char* fmt, ...);  // debug print

void fn_802E2CFC(void* time);
void fn_80129C80(int id, u32 seed_hi, u32 seed_lo);
int fn_80129D18(int id, int range);
int fn_80278A1C(const char* movie);

const char* fn_80130390(void);
void fn_8012DDA8(const char* res);
void fn_8012E498(const char* res);
void fn_8012E81C(const char* res);

void fn_802E2C20(void);
void LoadOptionsFile(void);
void fn_802C2AA8(int);
void fn_80127464(void);
void fn_802062E8(void);
void fn_80230090(void);
void InitializeLanguageSettings(void);
void fn_80245118(void);
void fn_802E712C(void);
void LoadEscapeSequenceData(void);
void InitializeLeagueData(void);
void LoadDebugGeometry(void);
void fn_80141454(void);
void RegisterCareerCallbacks(void);
void fn_8021031C(void);
void fn_801AED54(void);
void fn_802789A8(void);
void fn_8020CA30(void);
void fn_80211F44(void);
void fn_802E986C(void (*)(int));
void fn_802E9870(void (*)(struct CardTask*));
void fn_802E9874(void (*)(void));
void fn_802E9878(void (*)(void));
void fn_802E987C(void (*)(void));
void fn_802E9880(void (*)(void));
void fn_802E9884(void (*)(void));
void fn_802E9888(void (*)(void));
void fn_8023E7A8(int status);
struct CardTask;
void InitializeSaveMenu(CardTask* task);
void fn_8023F0E8(void);
void fn_8023F4B8(void);
void fn_8023F694(void);
void fn_8023EEF8(void);
void fn_8023F2C4(void);
void fn_8023ED30(void);
void fn_802789AC(void);
void ClearInputBuffer(void);

void fn_8020A634(void);
void fn_80211F4C(void);
void fn_8020CA34(void);
void fn_802789B0(void);
void fn_801AED58(void);
void fn_80141458(void);
void CleanupGraphicsResources(void);
void fn_80137BE4(void);
void fn_8023243C(void);
void ResetLanguageData(void);
void fn_80245124(void);
void fn_80230114(void);
void fn_802063D8(void);
void fn_802C2B18(void);
void CheckForLeakedCallbacks(void);
void fn_8000D878(void);


void fn_80265D48(void);
void fn_80129BD4(void);
void fn_801E78A0(void);
void RunModelTextureViewer(void);
void fn_8023B6B0(void);
void fn_80129BF8(void);
void fn_80265D4C(void);

void fn_80127434(void);
void fn_8023D7CC(void);
void fn_8023D70C(void);
void PlayMusicTrack(void);
void GameDoEve(int eve, int track);
}

extern "C" {
}
int GetNumTracks(void);

extern u32 lbl_8055D430;
extern u8 lbl_8055F63C;

struct GameTime {
    u32 unk0;
    u16 a;  // offset 0x4
    u16 b;  // offset 0x6
    u16 c;  // offset 0x8
    u16 d;  // offset 0xA
    u8 unkC[0xC];
};

ModuleTag lbl_8055D320;
UnkStaticFFFF lbl_8055D328;
int lbl_80558900 = 2;
const char* lbl_8055890C = "winky";
UnkStatic123 lbl_8034EC40;
UnkStatic7F lbl_8055D32C;
char lbl_8034EC58[0x20];

// Plays the logos, then one of the attract movies picked from the clock.
void PlayAttractMovie(void) {
    const char* movies[10] = {
        "backsiz.pss", "boatsiz.pss", "bussiz.pss",   "campsiz.pss", "chainsiz.pss",
        "compsiz.pss", "derbbasz.pss", "derbstsz.pss", "flagsiz.pss", "soccsiz.pss",
    };
    GameTime t;
    int idx;

    fn_802E2CFC(&t);
    fn_80129C80(6, (t.c << 8) & (t.d ^ t.b * 3600), (t.d << 8) & (t.c * 60 ^ t.a * 43200));
    idx = fn_80129D18(6, 10);
    if (fn_80278A1C("logos.pss")) {
        fn_80278A1C(movies[idx]);
    }
}

const char* ResolveResName(const char* name) {
    if (name == lbl_8055890C) {
        return fn_80130390();
    }
    return name;
}

const char* lbl_802FAFA0[] = {
    "genesis.res", lbl_8055890C, "common.res", "race.res", "uibig.res",
    "ghostcar.res", "hobbyset.res", "part.res", "mcbanner.res", NULL,
};

void LoadResidentRes(void) {
    const char** p;

    for (p = lbl_802FAFA0; *p != NULL; p++) {
        fn_8012DDA8(ResolveResName(*p));
    }
}

void FreeResidentRes(void) {
    const char** p;

    for (p = lbl_802FAFA0; *p != NULL; p++) {
        fn_8012E498(ResolveResName(*p));
    }
}

void AppInit(void) {
    fn_802E2C20();
    LoadResidentRes();
    fn_8012E81C("genesis.res");
    LoadOptionsFile();
    fn_802C2AA8(lbl_80558900);
    fn_80127464();
    fn_80127464();
    fn_802062E8();
    fn_80230090();
    InitializeLanguageSettings();
    fn_80245118();
    fn_802E712C();
    LoadTimerInit();
    LoadTimerBegin("app_begin", 1, 1, 12538.0f);
    fn_80127464();
    fn_8012E81C("common.res");
    LoadEscapeSequenceData();
    InitializeLeagueData();
    DriverInit();
    LoadTimerProgress();
    fn_80127464();
    LoadTimerProgress();
    fn_80127464();
    LoadDebugGeometry();
    RaceInit();
    IdentInit();
    StatsInit();
    AchievementsInit();
    FlagsInit();
    fn_80141454();
    RegisterCareerCallbacks();
    fn_8021031C();
    fn_801AED54();
    RepInit();
    UnlocksInit();
    fn_802789A8();
    fn_8020CA30();
    fn_80127464();
    LoadingInitHook();
    fn_80211F44();
    fn_802E986C(fn_8023E7A8);
    fn_802E9870(InitializeSaveMenu);
    fn_802E9874(fn_8023F0E8);
    fn_802E9878(fn_8023F4B8);
    fn_802E987C(fn_8023F694);
    fn_802E9884(fn_8023EEF8);
    fn_802E9880(fn_8023F2C4);
    fn_802E9888(fn_8023ED30);
    fn_802789AC();
    fn_80127464();
    ClearInputBuffer();
    PlayAttractMovie();
}

void AppShutdown(void) {
    fn_8020A634();
    fn_802E9880(NULL);
    fn_802E9884(NULL);
    fn_802E9878(NULL);
    fn_802E9874(NULL);
    fn_802E986C(NULL);
    fn_80211F4C();
    LoadingShutdownHook();
    fn_8020CA34();
    fn_802789B0();
    UnlocksShutdown();
    RepShutdown();
    fn_801AED58();
    fn_80141458();
    FlagsShutdown();
    AchievementsShutdown();
    StatsShutdown();
    IdentShutdown();
    RaceShutdown();
    CleanupGraphicsResources();
    DriverShutdown();
    fn_80137BE4();
    fn_8023243C();
    LoadTimerShutdown();
    ResetLanguageData();
    fn_80245124();
    fn_80230114();
    fn_802063D8();
    fn_802C2B18();
    CheckForLeakedCallbacks();
    FreeResidentRes();
    fn_8000D878();
}

int ParseCommandLine(const char* args) {
    if (args != NULL) {
        ParseOptions(args);
        while (*args != '\0') {
            switch (*args) {
            case '-':
            case '/':
                if (strncmp(args + 1, "interlude", 9) == 0) {
                    lbl_8055F63C = 1;
                } else if (strncmp(args + 1, "res:", 4) == 0) {
                }
                break;
            case ' ':
                break;
            }
            args++;
        }
    }
    return 1;
}

void AppMain(void) {
    fn_80265D48();
    fn_80129BD4();
    AppInit();
    fn_802E30C8("AppMain is 0x%x", AppMain);
    if (OptGxTest()) {
        fn_801E78A0();
    } else if (OptModelTest()) {
        RunModelTextureViewer();
    } else if (OptPaTest()) {
        void PlayEves(void);
        PlayEves();
    } else if (OptDirectToTrack()) {
        void DirectToTrackDefault(void);
        DirectToTrackDefault();
    } else {
        fn_8023B6B0();
    }
    AppShutdown();
    fn_80129BF8();
    fn_80265D4C();
}

// Name of the contest/eve/track to jump straight into ("$name"/"@name" picks a contest).
void SetDirectToTrackName(const char* name) {
    int n = 0x20;
    char* dst = lbl_8034EC58;

    while ((*dst++ = *name++) != '\0') {
        if (--n <= 0) {
            dst[-1] = '\0';
            return;
        }
    }
}

int FindContest(int* out_track, int* out_eve, const char* name) {
    TrackInfo* track;
    int found;
    int track_idx;
    int eve_idx;

    found = 0;
    for (track_idx = 0; track_idx < GetNumTracks(); track_idx++) {
        track = GetTrack(track_idx);
        for (eve_idx = 0; eve_idx < track->events; eve_idx++) {
            if (strcasecmp(FindTrackEvent(track_idx, eve_idx)->node->GetString("name", ""), name) == 0) {
                *out_track = track_idx;
                found = 1;
                *out_eve = eve_idx;
                break;
            }
        }
        if (found) {
            break;
        }
    }
    return found;
}

const char* ValueNode::GetString(const char* key, const char* def) {
    if (Has(key)) {
        return GetValue(key);
    }
    return def;
}

int FindTrack(int* out_track, int* out_eve, const char* name) {
    int found;
    int track_idx;

    found = 0;
    for (track_idx = 0; track_idx < GetNumTracks(); track_idx++) {
        if (strcasecmp(name, GetTrack(track_idx)->name) == 0) {
            *out_track = track_idx;
            found = 1;
            *out_eve = 0;
            break;
        }
    }
    return found;
}

int FindEve(int* out_track, int* out_eve, const char* name) {
    int found;
    int track_idx;

    found = 0;
    for (track_idx = 0; track_idx < GetNumTracks(); track_idx++) {
        if (strcasecmp(GetTrack(track_idx)->title, name) == 0) {
            *out_track = track_idx;
            found = 1;
            *out_eve = 0;
            break;
        }
    }
    return found;
}

#include "game/flagguard.h"

int DirectToTrack(const char* name) {
    int found;

    fn_80127434();
    fn_8023D7CC();
    fn_8023D70C();
    {
        FlagGuard guard;
        int eve = 0;
        int track = GetNumTracks();

        if (lbl_8034EC58[0] == '$' || lbl_8034EC58[0] == '@') {
            found = FindContest(&track, &eve, lbl_8034EC58 + 1);
            if (!found) {
                fn_802E30C8("direct_to_track: Can't find contest named \"%s\"", lbl_8034EC58 + 1);
            }
        } else if (lbl_8034EC58[0] != '\0') {
            found = FindTrack(&track, &eve, lbl_8034EC58);
            if (!found) {
                fn_802E30C8("direct_to_track: Can't find eve at track \"%s\"", lbl_8034EC58);
            }
        } else {
            found = FindEve(&track, &eve, name);
            if (!found) {
                fn_802E30C8("direct_to_track: Can't find eve named \"%s\"", name);
            }
        }

        if (found) {
            PlayMusicTrack();
            GameDoEve(track, eve);
        } else if (lbl_8034EC58[0] != '\0') {
            fn_802E30C8("no eves for \"%s\"", lbl_8034EC58);
        } else {
            fn_802E30C8("no eves are called \"%s\"", name);
        }
    }
    return 0;
}

void DirectToTrackDefault(void) {
    DirectToTrack("DirectToTrack");
}

void PlayEves(void) {
    char name[0x40];
    int i;

    for (i = 0;; i++) {
        sprintf(name, "PA_Eve%d", i);
        if (!DirectToTrack(name)) {
            break;
        }
    }
}
