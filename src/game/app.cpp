// Excite Truck's application layer (0x800049C0-0x800053CC): startup and shutdown of every
// subsystem, the attract movie, command-line switches and "direct to track" launching. The file
// name is a guess (it holds AppMain); the engine's own names are unknown, so callees keep their
// addresses.
#include <types.h>
#include <string.h>
#include <stdio.h>

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
void fn_80133024(void);
void fn_802C2AA8(int);
void fn_80127464(void);
void fn_802062E8(void);
void fn_80230090(void);
void fn_801303A8(void);
void fn_80245118(void);
void fn_802E712C(void);
void fn_80012478(void);
void fn_800124DC(const char* name, int, int, float);
void fn_80232230(void);
void fn_80137B20(void);
void fn_80016354(void);
void fn_800125C4(void);
void fn_801E3C04(void);
void fn_80007F40(void);
void fn_8000D8C8(void);
void fn_8001BA78(void);
void fn_8001EF38(void);
void fn_8000D7D8(void);
void fn_80141454(void);
void fn_8027DAF4(void);
void fn_8021031C(void);
void fn_801AED54(void);
void fn_80019864(void);
void fn_80018F40(void);
void fn_802789A8(void);
void fn_8020CA30(void);
void fn_800218F4(void);
void fn_80211F44(void);
void fn_802E986C(void (*)(void));
void fn_802E9870(void (*)(void));
void fn_802E9874(void (*)(void));
void fn_802E9878(void (*)(void));
void fn_802E987C(void (*)(void));
void fn_802E9880(void (*)(void));
void fn_802E9884(void (*)(void));
void fn_802E9888(void (*)(void));
void fn_8023E7A8(void);
void fn_8023E5B4(void);
void fn_8023F0E8(void);
void fn_8023F4B8(void);
void fn_8023F694(void);
void fn_8023EEF8(void);
void fn_8023F2C4(void);
void fn_8023ED30(void);
void fn_802789AC(void);
void fn_802E47E8(void);

void fn_8020A634(void);
void fn_80211F4C(void);
void fn_800218F8(void);
void fn_8020CA34(void);
void fn_802789B0(void);
void fn_80018F7C(void);
void fn_80019884(void);
void fn_801AED58(void);
void fn_80141458(void);
void fn_8000D7E4(void);
void fn_8001EFCC(void);
void fn_8001BA84(void);
void fn_8000D94C(void);
void fn_80007F68(void);
void fn_801E3C88(void);
void fn_800163F0(void);
void fn_80137BE4(void);
void fn_8023243C(void);
void fn_80012628(void);
void fn_80130518(void);
void fn_80245124(void);
void fn_80230114(void);
void fn_802063D8(void);
void fn_802C2B18(void);
void fn_80133144(void);
void fn_8000D878(void);

void fn_800199B8(void);

void fn_80265D48(void);
void fn_80129BD4(void);
int fn_80019B6C(void);
int fn_80019B48(void);
int fn_80019B54(void);
int fn_80019B3C(void);
void fn_801E78A0(void);
void fn_8024BE04(void);
void fn_8023B6B0(void);
void fn_80129BF8(void);
void fn_80265D4C(void);

void fn_80127434(void);
void fn_8023D7CC(void);
void fn_8023D70C(void);
void fn_8020A600(void);
void fn_8023BFBC(int eve, int track);
}

class ValueTable {
public:
    virtual void vf0(void);
    virtual void vf1(void);
    virtual void vf2(void);
    virtual void vf3(void);
    virtual const char* GetString(const char* key, const char* def);
};

struct Eve {
    u8 unk0[0x20];
    ValueTable* vals;  // offset 0x20
};

struct Track {
    u8 unk0[0x8];
    char name[0x20];   // offset 0x8
    char name2[0x48];  // offset 0x28
    s32 num_eves;      // offset 0x70
};

extern "C" {
int fn_80012818(void);
Track* fn_80012820(int track);
Eve* fn_80012870(int track, int eve);
}

class Checker {
public:
    virtual void vf0(void);
    virtual int IsValid(void);
    virtual void vf2(void);
    virtual int Get(int key);
};

class Unk_8000D838 {
public:
    Unk_8000D838(void);
    u8 unk0[0x8];
};

// Values set up by the static initializer.
class Unk_D328 {
public:
    Unk_D328(void) { value = 0xFFFF0000; }
    u32 value;
};

class Unk_D32C {
public:
    Unk_D32C(void) { value = 0x7F000000; }
    u32 value;
};

class Unk_EC40 {
public:
    Unk_EC40(void) {
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
    u32 unk14;
};

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

Unk_8000D838 lbl_8055D320;
Unk_D328 lbl_8055D328;
int lbl_80558900 = 2;
const char* lbl_8055890C = "winky";
Unk_EC40 lbl_8034EC40;
Unk_D32C lbl_8055D32C;
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
    fn_80133024();
    fn_802C2AA8(lbl_80558900);
    fn_80127464();
    fn_80127464();
    fn_802062E8();
    fn_80230090();
    fn_801303A8();
    fn_80245118();
    fn_802E712C();
    fn_80012478();
    fn_800124DC("app_begin", 1, 1, 12538.0f);
    fn_80127464();
    fn_8012E81C("common.res");
    fn_80232230();
    fn_80137B20();
    fn_80016354();
    fn_800125C4();
    fn_80127464();
    fn_800125C4();
    fn_80127464();
    fn_801E3C04();
    fn_80007F40();
    fn_8000D8C8();
    fn_8001BA78();
    fn_8001EF38();
    fn_8000D7D8();
    fn_80141454();
    fn_8027DAF4();
    fn_8021031C();
    fn_801AED54();
    fn_80019864();
    fn_80018F40();
    fn_802789A8();
    fn_8020CA30();
    fn_80127464();
    fn_800218F4();
    fn_80211F44();
    fn_802E986C(fn_8023E7A8);
    fn_802E9870(fn_8023E5B4);
    fn_802E9874(fn_8023F0E8);
    fn_802E9878(fn_8023F4B8);
    fn_802E987C(fn_8023F694);
    fn_802E9884(fn_8023EEF8);
    fn_802E9880(fn_8023F2C4);
    fn_802E9888(fn_8023ED30);
    fn_802789AC();
    fn_80127464();
    fn_802E47E8();
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
    fn_800218F8();
    fn_8020CA34();
    fn_802789B0();
    fn_80018F7C();
    fn_80019884();
    fn_801AED58();
    fn_80141458();
    fn_8000D7E4();
    fn_8001EFCC();
    fn_8001BA84();
    fn_8000D94C();
    fn_80007F68();
    fn_801E3C88();
    fn_800163F0();
    fn_80137BE4();
    fn_8023243C();
    fn_80012628();
    fn_80130518();
    fn_80245124();
    fn_80230114();
    fn_802063D8();
    fn_802C2B18();
    fn_80133144();
    FreeResidentRes();
    fn_8000D878();
}

int ParseCommandLine(const char* args) {
    if (args != NULL) {
        fn_800199B8();
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
    if (fn_80019B6C()) {
        fn_801E78A0();
    } else if (fn_80019B48()) {
        fn_8024BE04();
    } else if (fn_80019B54()) {
        void PlayEves(void);
        PlayEves();
    } else if (fn_80019B3C()) {
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
    Track* track;
    int found;
    int track_idx;
    int eve_idx;

    found = 0;
    for (track_idx = 0; track_idx < fn_80012818(); track_idx++) {
        track = fn_80012820(track_idx);
        for (eve_idx = 0; eve_idx < track->num_eves; eve_idx++) {
            if (strcasecmp(fn_80012870(track_idx, eve_idx)->vals->GetString("name", ""), name) == 0) {
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

int CheckerGet(Checker* checker, int key, int def) {
    if (checker->IsValid()) {
        return checker->Get(key);
    }
    return def;
}

int FindTrack(int* out_track, int* out_eve, const char* name) {
    int found;
    int track_idx;

    found = 0;
    for (track_idx = 0; track_idx < fn_80012818(); track_idx++) {
        if (strcasecmp(name, fn_80012820(track_idx)->name) == 0) {
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
    for (track_idx = 0; track_idx < fn_80012818(); track_idx++) {
        if (strcasecmp(fn_80012820(track_idx)->name2, name) == 0) {
            *out_track = track_idx;
            found = 1;
            *out_eve = 0;
            break;
        }
    }
    return found;
}

class FlagGuard {
public:
    FlagGuard(void) {
        saved = lbl_8055D430 & 1;
        lbl_8055D430 |= 1;
    }
    ~FlagGuard(void) { lbl_8055D430 = (lbl_8055D430 & ~1) | saved; }
    u32 saved;
};

int DirectToTrack(const char* name) {
    int found;

    fn_80127434();
    fn_8023D7CC();
    fn_8023D70C();
    {
        FlagGuard guard;
        int eve = 0;
        int track = fn_80012818();

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
            fn_8020A600();
            fn_8023BFBC(track, eve);
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
