// Ring challenge master (0x8016B964-0x80171698): the "ringchalmaster" game mode. It loads the Freestyle
// arena's object sets ("object_file_%d", dynos and teleports), counts the ring sets in the ring file, runs the
// timed ring challenge with its pre-race and results overlays ("ResultsOverlay:*", "PreRaceOverlay:*") and
// scores rings by size and difficulty. Names are guesses apart from the strings.
#include "game/common.h"
#include "game/mathutil.h"
#include "game/phob.h"
#include "game/pathset.h"
#include "game/valuebinding.h"
#include "game/gart.h"
#include "game/idealline.h"
#include "game/centerline.h"
#include "game/gamemode.h"
#include "game/ilayout.h"
#include "game/v2.h"
#include "game/arrayref.h"
#include "game/locstring.h"
#include "game/raceprog.h"
#include "game/respawn.h"
#include <revolution/MTX/vec.h>
#include "game/springcar.h"
#include "game/sound.h"

// Art animation player (artanim.cpp).
struct ArtXform;  // artanim.cpp
#include "game/artplayer.h"

extern f32 lbl_8055E3C8;  // time

extern "C" {
void* memset(void* dst, int c, u32 n);
int strcmp(const char* a, const char* b);
int strcasecmp(const char* a, const char* b);
char* strstr(const char* s, const char* t);
u32 strlen(const char* s);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void fn_801345D8(char* buf, int size, const char* fmt, ...);
void fn_801308F4(char* buf, int size, int value);         // number with separators
void fn_80130980(char* buf, int size, int digits, f32 v);  // fixed-point number
int fn_801609C4(int score);                                // grade for a score
ValueTable* fn_80131808(const char* name);  // open .val table
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* t);  // close .val table
s32 fn_80204CF0(void);
s32 fn_80204EA0(const char* key);
void mrModelUnload(s32 model);
s32 fn_802C6FC0(const char* name, int a, int b);
void* fn_802BAA68(void* xlo, const ValMatrix* m);
void fn_8016A0B0(void);
void fn_8016A15C(const char* name);  // activate ring set
void fn_8016A240(const char* name);  // deactivate ring set
void fn_8016A0D4(void);
void fn_8016A68C(void);
void fn_8001B450(s32 h);
s32 fn_802BAF98(void* obj, int on);
void fn_80144C54(GameSound* s);
s32 fn_80179650(void);
s32 fn_80145E70(const char* name);
void fn_802ADDD8(void* p, int a);
}

union ElemColor {
    u32 all;  // 0xAARRGGBB (alpha in the top byte)
};

struct TextElem {
    TextElem(void) {}
    TextElem(const TextElem& o);  // fn_8014C920 (weak; defined at the end, after its uses)
    const char* name;     // offset 0x0
    V2 pos;               // offset 0x4
    s32 elemJust;         // offset 0xC
    f32 scale;            // offset 0x10
    u32 just;             // offset 0x14
    ElemColor color;      // offset 0x18
    const char* defText;  // offset 0x1C
};

struct Box {
    const char* name;  // offset 0x0
    V2 pos;            // offset 0x4
    s32 c;             // offset 0xC
    f32 w;             // offset 0x10
    f32 h;             // offset 0x14
    ElemColor color;   // offset 0x18
};

static inline ValueBinding Bind(s32 t, const char* n, void* p) {
    ValueBinding b;
    b.type = t;
    b.name = n;
    b.ptr = p;
    return b;
}

extern const ValueBinding lbl_802FAA40;

extern "C" {
void fn_8000FBE4(void);
int fn_80161C38(void);
}  // extern "C"
void UpdateScoreOverlays(int, int);
extern "C" {
int fn_801E866C(int view);
const char* fn_80131234(const char* key);
void fn_801479A8(const TextElem* a, const TextElem* b, const char* fmt, f32 t, ...);
f32 fn_80147204(TextElem* e, const char* text);
s32 fn_80160998(s32 a);
s32 fn_80160A74(void);
void fn_8014784C(ILayout* l, const char* box, const char* set, int part);
void fn_80130978(char* buf, int size, f32 v);
}  // extern "C"
f32 Remap(float, float, float, float, float);
extern "C" {
f32 fn_80144C80(f32 a, f32 b, f32 t);
f32 fn_80144BD8(f32 t);
void fn_80147940(ILayout* l, Box* b, const char* art, int part);
void fn_80147230(TextElem* e, const char* fmt, ...);
void fn_801476C4(TextElem* e);
int fn_802C9A58(void);
}  // extern "C"
void DrawText(ILayout*, const char*);
extern "C" {
void fn_801475D0(ILayout* l, const char* name, const char* fmt, ...);
}

// Respawn, CarPlace: game/respawn.h.
extern "C" {
double sqrt(double x);
void fn_802C744C(s32 model, const ValMatrix* m);
void fn_802C9900(f32 alpha);
void fn_802C9904(void);
void fn_8016A438(void);
int fn_801B8514(void);
}  // extern "C"
void UpdateContinueOverlay(void);
extern "C" {
void fn_801B0C78(int car);
}

extern "C" RaceCarRec* fn_8017960C(int id);
extern f32 __float_epsilon[];  // FLT_EPSILON

#define NearZeroF(x) ((f32)__fabs(x) <= __float_epsilon[0])

struct CenterLine;

extern "C" {
PathSet* fn_80156A40(void);
void fn_801B7C8C(CarPlace* r, V2* pos, V2* dir);
}

extern "C" {
f32 fn_801B306C(void);
}  // extern "C"
int fn_80009C84(void);
extern "C" {
s32 fn_8016A30C(void* rings);
s32 fn_8016A780(const char* name, void* rings);
s32 fn_8016A878(const char* name);
// Retail calls the out-of-line forms of GameSound::Playing and GameSound::Play here.
#ifdef __MWERKS__
int Playing__9GameSoundFv(GameSound* s);
void Play__9GameSoundFv(GameSound* s);
#else
static inline int Playing__9GameSoundFv(GameSound* s) { return s->Playing(); }
static inline void Play__9GameSoundFv(GameSound* s) { s->Play(); }
#endif
void fn_801CB5DC(int car, s32 score, s32 grade);
void fn_801CB648(int car, int a, f32 t);
}

static inline f32 Time(void);

struct GemRef {
    s32 a;
    s32 b;
};

extern "C" GemRef fn_8016A6B0(const char* name, int n);
extern "C" s32 fn_8001B308(GemRef* g, void* art, int a);
extern "C" void fn_801B8060(CarPlace* r, struct ObjTeleport* t);

const char* GetGradeDisplayName(int grade);
int IdentGetCurrentSlot(void);
void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

#include "game/configroot.h"

ConfigRoot* fn_8000AC38(void);

extern f32 lbl_8055E3C8;  // time

extern "C" GameSound* fn_802076A0(const char* name, int a, int b, int c);
extern "C" GameSound* fn_80207848(const char* name, int a);

static inline void Play(GameSound* s) {
    if (s) {
        s->Play();
    }
}

static inline void Stop(GameSound* s) {
    if (s) {
        s->Stop();
    }
}

static inline void PlayIdle(GameSound* s) {
    if (s && !s->playing) {
        s->Play();
    }
}

static inline void StrCopyN(char* dst, const char* src, int len) {
    char* d = dst;
    int n = len;
    const char* s = src;
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

#define CLAMP01(x) ((x) = MIN(1.0f, (x)), (x) = MAX((x), 0.0f))

/* Helper name unproven */
static inline f32 UnlerpClamped(f32 a, f32 b, f32 x) {
    return MaxF(MinF(1.0f, (x - a) / (b - a)), 0.0f);
}



template <class T>
struct RArray : ArrayRef<T> {
    RArray(int n, const char* tag);
    ~RArray(void) { delete[] buf; }
    T* Add(void);
    void Push(const T& v);
    int Count(void);
    T* At(int i);
    T* buf;     // offset 0xC
};

template <class T>
struct IArray : ArrayRef<T> {
    IArray(int n, const char* tag) : ArrayRef<T>(new (tag) T[n], n) { buf = this->data; }
    ~IArray(void) { delete[] buf; }
    T* Add(void) { return &this->data[this->count++]; }
    T* buf;     // offset 0xC
};

struct ObjInfo {
    ObjInfo(void) {
        loaded = 0;
        model = -1;
        obj = 0;
        xlo = 0;
        type = 3;
    }
    ~ObjInfo(void) {
        if (loaded) {
            loaded = 0;
            if (model != -1) {
                mrModelUnload(model);
                model = -1;
            }
            obj = 0;
            xlo = 0;
        }
    }
    s32 model;      // offset 0x0
    void* obj;      // offset 0x4
    ValueNode* xlo;  // offset 0x8
    s32 type;       // offset 0xC
    u8 loaded;      // offset 0x10
    u8 pad11[3];
    ValMatrix mtx;  // offset 0x14
};

#include "game/objteleport.h"

struct Race;

namespace {
struct ObjSet {
    ObjSet(void) {
        loaded = 0;
        infos = 0;
        teles = 0;
        f10 = 0;
    }
    ~ObjSet(void) {
        if (loaded) {
            loaded = 0;
            delete infos;
            infos = 0;
            delete teles;
            teles = 0;
        }
    }
    u8 Load(ValueNode* set, ValueTable** dynos);
    void CheckTeleport(struct Phob* car);
    ObjTeleport* Next(void) const {
        int i = f10;
        if (i + 1 < teles->count) {
            return &teles->data[i + 1];
        }
        return &teles->data[i];
    }
    IArray<ObjInfo>* infos;      // offset 0x0
    IArray<ObjTeleport>* teles;  // offset 0x4
    void Reset(void);
    u8 loaded;                   // offset 0x8
    u8 active;                   // offset 0x9
    u8 padA[2];
    f32 time;                    // offset 0xC
    s32 f10;  // offset 0x10
};

struct SetStat {
    u8 done;      // offset 0x0
    s32 best;     // offset 0x4
    f32 time;     // offset 0x8
    f32 score;    // offset 0xC
};

struct ArenaTimer {
    void Set(f32 t);
    void Reset(void);
    f32 Get(void);
    void Draw(class ILayout* l) const;
    int Expired(void) { return t <= 0.0f; }
    f32 t;      // offset 0x0
    f32 limit;  // offset 0x4
};

}  // namespace

// A value table this unit owns and releases (fn_80131818) when it goes; not tableref.h's TableRef. Name unproven.
struct OwnedTable {
    ~OwnedTable(void) { fn_80131818(t); }
    ValueTable* t;
};

// Every arena is a preloader (preload.cpp): it counts and spawns its dynos.
#include "game/preload.h"

namespace {
struct PlayerData;
class Arena : public Preloader {
public:
    int CountDynosIn(int n, int& i);
public:
public:
    Arena(void);
    virtual s32 Size(void);        // 0x8: counts the arena's dynos
    virtual void Load(void);       // 0xC: spawns them
    virtual ~Arena(void);          // 0x10
    void Init(void);
    void Setup(void);
    int CountFiles(ValueNode* cfg);
    int CountRingSets(void);
    void Step(PlayerData* pd);
    void Tally(void);
    int PointValue(void);
    void SetName(char* buf, int n);
    void ShowSetGems(void);
    void ActivateRingSet(void);
    void ActivateObjSet(void);
    void ClearGems(void);
    void Deactivate(int i);
    ArenaTimer timer;              // offset 0x8
    s32 f10;                       // offset 0x10
    OwnedTable dynos;                // offset 0x14
    RArray<ObjSet>* sets;          // offset 0x18
    RArray<SetStat>* stats;        // offset 0x1C
    gArtSet* art;                     // offset 0x20
    s32 f24;                       // offset 0x24
    f32 f28;                       // offset 0x28
    s32 curSet;                    // offset 0x2C
    s32 f30;                       // offset 0x30
    s32 f34;                       // offset 0x34
    s32 nSets;                     // offset 0x38
    s32 nFiles;                    // offset 0x3C
    s32 f40;                       // offset 0x40
    s32 difficulty;                // offset 0x44
    u8 b48;                        // offset 0x48
    u8 b49;                        // offset 0x49
    u8 b4A;                        // offset 0x4A
    u8 pad4B;
    GameSound* sRingHit;              // offset 0x4C
    GameSound* sCheerHit;             // offset 0x50
    GameSound* sLongCheer;            // offset 0x54
    GameSound* sTally;                // offset 0x58
    GameSound* sMissFlag;             // offset 0x5C
    GameSound* sLevelUp;              // offset 0x60
    GameSound* sFail;                 // offset 0x64
};

struct PlayerData {
    PlayerData(int i);
    ~PlayerData(void) { IdentGetCurrentSlot(); }
    void Reset(void) {
        score = 0;
        b8 = 0;
        fC = 0.0f;
    }
    int Stuck(void);
    struct RaceCarRec* Car(void);
    s32 score;  // offset 0x0
    s32 index;  // offset 0x4
    u8 b8;      // offset 0x8
    u8 pad9[3];
    f32 fC;     // offset 0xC
};

struct PlayerList : ArrayRef<PlayerData*> {
    PlayerList(void);
    void Clear(void);
    PlayerData** Push(PlayerData* const& p);
    PlayerData* storage[8];   // offset 0xC
};

class RingResults {
public:
    void Open(void);
    RingResults(void);
    ~RingResults(void) { Unload(); }
    void Unload(void) {
        if (layout) {
            delete layout;
            layout = 0;
        }
        if (sBonus) {
            delete sBonus;
            sBonus = 0;
        }
        if (sNoBonus) {
            delete sNoBonus;
            sNoBonus = 0;
        }
        if (s1C) {
            delete s1C;
            s1C = 0;
        }
        if (s24) {
            delete s24;
            s24 = 0;
        }
        if (s28) {
            delete s28;
            s28 = 0;
        }
        if (s2C) {
            delete s2C;
            s2C = 0;
        }
    }
    void Update(int score, int rings, int bonus);
    void Reset(void);
    void Load(void);
    // Accessor name unproven. The key handler reads the state through it once and directly afterwards,
    // which is why retail loads the field twice (this used to be forced with a volatile field).
    int Active(void) const { return state != 0; }
    s32 state;  // offset 0x0
    f32 start;         // offset 0x4
    u8 b8;             // offset 0x8
    u8 b9;             // offset 0x9
    u8 padA[2];
    ArtPlayer* layout; // offset 0xC
    u8 noBonus;        // offset 0x10
    u8 pad11[3];
    GameSound* sBonus;    // offset 0x14
    GameSound* sNoBonus;  // offset 0x18
    GameSound* s1C;       // offset 0x1C
    u8 b20;            // offset 0x20
    u8 pad21[3];
    GameSound* s24;       // offset 0x24
    GameSound* s28;       // offset 0x28
    GameSound* s2C;       // offset 0x2C
    char ringScore[0x14];   // offset 0x30
    char timeLeft[0x14];    // offset 0x44
    char bonusTitle[0x14];  // offset 0x58
    char finalScore[0x14];  // offset 0x6C
    char grade[0xA];        // offset 0x80
    char noBonusScore[0x14];  // offset 0x8A
    char timeBonus[0x14];   // offset 0x9E
    char bB2[0x14];         // offset 0xB2
    char bC6[0x14];         // offset 0xC6
    char complete[0x14];    // offset 0xDA
    u8 padEE[2];
    s32 fF0;   // offset 0xF0
    s32 fF4;   // offset 0xF4
    s32 fF8;   // offset 0xF8
    s32 fFC;   // offset 0xFC
    s32 f100;  // offset 0x100
    s32 f104;  // offset 0x104
};

}  // namespace

#include "game/zoneresults.h"

namespace {
class RingChalMaster : public GameMode {
public:
    RingChalMaster(Race* race);
    virtual ~RingChalMaster(void);
    virtual void vf10(void);
    void ResetGame(void);
    virtual int vf20(int car) const;
    virtual int vf3C(int view);
    virtual f32 vf38(void);
    virtual void vfB8(ZoneResults* r);
    virtual void vfF8(void);
    virtual void vfFC(void);
    virtual void vf4C(int p) const;
    virtual void vf50(int p);
    virtual void vf40(void);
    virtual void vf44(void);
    virtual int vf54(int p);
    virtual int vfA0(struct Respawn* out, int idx, int c);
    virtual void vf0C(void);
    virtual int vf24(void) const;
    virtual int vf68(void) const;
    virtual int vf64(void);
    virtual int vf5C(int p) const;
    virtual int vf8C(int p, int key);
    virtual int Started1C(int car);
    virtual void vfA8(int car, int b);
    virtual int vfB0(int car, struct Respawn* r, int c);
    virtual int vfE0(int car, u8* out);
    void DrawRings(void) const;
    PlayerList players;       // offset 0x4
    Arena arena;              // offset 0x30
    ILayout* layout;          // offset 0x98
    u8 b9C;                   // offset 0x9C
    u8 pad9D[3];
    f32 fA0;                  // offset 0xA0
    GameSound* sPreRace;         // offset 0xA4
    u8 bA8;                   // offset 0xA8
    u8 padA9[3];
    RingResults results;   // offset 0xAC
};

}  // namespace

struct Race {
    u8 pad0[0x3E0];
    s32 nCars;  // offset 0x3E0
};

ModuleTag lbl_8055E368;
UnkStaticFFFF lbl_8055E370;
static UnkStatic123s lbl_80430358;
extern RingChalMaster* lbl_8055E378;

void RingResults::Update(int score, int rings, int bonus) {
    if (layout) {
        int shown;
        int n;
        fn_801308F4(ringScore, 0x14, rings);
        shown = bonus;
        n = 0;
        if (bonus && start > 0.0f) {
            f32 now = lbl_8055E3C8;
            f32 t = UnlerpClamped(0.0f, 1.0f, now - start);
            n = Lerp(0.0f, 0.5f + bonus, t);
            shown = bonus - n;
        }
        fn_801345D8(timeLeft, 0x14, "%d", n);
        {
            static LocString s("ResultsOverlay:TIME_BONUS_CAPS");
            if (noBonus) {
                StrCopyN(timeBonus, "", 0x14);
                StrCopyN(bonusTitle, "", 0x14);
            } else {
                fn_80130980(timeBonus, 0x14, 2, shown);
                StrCopyN(bonusTitle, s.Get(), 0x14);
            }
        }
        fn_801308F4(finalScore, 0x14, score);
        fn_801308F4(noBonusScore, 0x14, score - bonus);
        {
            int g = fn_801609C4(score);
            if (g != -1) {
                StrCopyN(grade, GetGradeDisplayName(g), 0xA);
            } else {
                StrCopyN(grade, "-", 0xA);
            }
        }
        if (fF8) {
            b8 = 0;
        }
        switch (state) {
        case 0:
            ((ArtPlayer*)(layout))->Start(fF4);
            if (strlen(complete)) {
                PlayIdle(s2C);
            } else {
                PlayIdle(s28);
            }
            if (bonus > 0) {
                Play(sBonus);
            } else {
                Play(sNoBonus);
            }
            state = 1;
            break;
        case 1:
            if (fFC && start < 0.0f) {
                start = lbl_8055E3C8;
            }
            if (fFC == 2 && !b20) {
                b20 = 1;
                Play(s1C);
            }
            if (f100) {
                ((ArtPlayer*)(layout))->Pause(fF4);
                state = 2;
            }
            break;
        case 2:
            if (b9) {
                ((ArtPlayer*)(layout))->Resume(fF4);
                Play(s24);
                state = 3;
            }
            break;
        case 3:
            if (f104) {
                ((ArtPlayer*)(layout))->Pause(fF4);
                state = 4;
            }
            break;
        case 4:
            break;
        }
        ((ArtPlayer*)(layout))->Update();
    }
}

RingChalMaster* lbl_8055E378;

inline RingChalMaster::RingChalMaster(Race* race) {
    int i;
    lbl_8055E378 = this;
    players.Clear();
    for (i = 0; i < race->nCars; i++) {
        PlayerData* p = new ("playerdata") PlayerData(i);
        players.Push(p);
    }
    fn_8016A0B0();
    arena.Init();
    layout = 0;
    b9C = 1;
    fA0 = -100.0f;
    sPreRace = fn_80207848("chal_prerace.sfx", 1);
    bA8 = 0;
}

extern "C" RingChalMaster* fn_8016BE60(Race* race) { return new ("ringchalmaster") RingChalMaster(race); }


inline Arena::~Arena(void) {
    fn_80235380(art);
    if (sRingHit) {
        delete sRingHit;
    }
    if (sLongCheer) {
        delete sLongCheer;
    }
    if (sTally) {
        delete sTally;
    }
    if (sCheerHit) {
        delete sCheerHit;
    }
    if (sMissFlag) {
        delete sMissFlag;
    }
    if (sLevelUp) {
        delete sLevelUp;
    }
    if (sFail) {
        delete sFail;
    }
    delete sets;
    delete stats;
}

template <class T>
RArray<T>::RArray(int n, const char* tag) : ArrayRef<T>(new (tag) T[n], n) { buf = this->data; }

template <class T>
void RArray<T>::Push(const T& v) {
    this->data[this->count++] = v;
}

template <class T>
inline int RArray<T>::Count(void) {
    return this->count;
}

template <class T>
T* RArray<T>::Add(void) {
    return &this->data[this->count++];
}

// Reads the time limit, loads every object file, counts the ring sets and builds the per-set stats.
/* Helper name unproven: Arena::Init is this one inline body. Retail numbers its locals as an expanded inline's
   objects (declared at first use), with the new-expression's pointer temporary between cfg and t; as Init's own
   locals they are numbered ahead of every temporary (8 rows, FACTS 2026-11-17 13:20). */
inline void Arena::Setup(void) {
    ValueNode* cfg = fn_8000AC38()->vals;
    char name[0x1E];
    if (cfg->Has("time_limit")) {
        timer.Set(cfg->GetFloat("time_limit"));
    } else {
        timer.Set(120.0f);
    }
    fn_801345D8(name, 0x1E, "object_file_0");
    sets = new ("rc_object_sets") RArray<ObjSet>(CountFiles(cfg), "rc_object_files");
    while (cfg->Has(name)) {
        ValueTable* t = fn_80131810(cfg->GetValue(name));
        if (t) {
            ValueNode* n;
            for (int i = 0; (n = t->GetChild(i)) != 0; i++) {
                sets->Add()->Load(n, &dynos.t);
            }
            fn_80131818(t);
        } else {
            fn_802E30C8("FreestyleArenaMaster: can't load object file '%s'. Object load ceased.", name);
            return;
        }
        fn_801345D8(name, 0x1E, "object_file_%d", ++nFiles);
    }
    nFiles = sets->Count();
    nSets = CountRingSets();
    stats = new ("rc_ringsetstats") RArray<SetStat>(nSets, "rc_ringsetstats");
    for (int i = 0; i < nSets; i++) {
        SetStat st;
        st.done = 0;
        st.best = -1;
        st.time = -100.0f;
        st.score = -100.0f;
        stats->Push(st);
    }
}

void Arena::Init(void) { Setup(); }

int Arena::CountRingSets(void) {
    ValueNode* cfg = fn_8000AC38()->vals;
    const char* key = (const char*)fn_80204EA0("ring_file_key");
    int found;
    int n = 0;
    if (cfg->Has(key)) {
        ValueTable* t = fn_80131810(cfg->GetValue(key));
        if (t) {
            do {
                char prefix[0x1E];
                int i;
                found = 0;
                fn_801345D8(prefix, 0x1E, "set_%i_", n);
                for (i = 0; t->GetChild(i) != 0; i++) {
                    if (!strcmp(t->GetChild(i)->GetName(), "ring_set") &&
                        strstr(t->GetChild(i)->GetValue("name"), prefix)) {
                        found = 1;
                        n++;
                        break;
                    }
                }
            } while (found);
            fn_80131818(t);
        }
    }
    return n;
}

static inline int CountInfos(ValueNode* set) {
    ValueNode* c;
    int n = 0;
    int i;
    for (i = 0; (c = set->GetChildAt(i)) != 0; i++) {
        if (!strcasecmp(c->GetName(), "object") && strcasecmp(c->GetValue("dyno"), "teleport")) {
            n++;
        }
    }
    return n;
}

static inline int CountTeles(ValueNode* set) {
    ValueNode* c;
    int n = 0;
    int i;
    for (i = 0; (c = set->GetChildAt(i)) != 0; i++) {
        if (!strcasecmp(c->GetName(), "object") && !strcasecmp(c->GetValue("dyno"), "teleport")) {
            n++;
        }
    }
    return n;
}

static inline bool HasChild(ValueNode* n, const char* name) { return n->FindChild(name) != 0; }

u8 ObjSet::Load(ValueNode* set, ValueTable** dynos) {
    ValueNode* c;
    u8 ok;
    int i;
    infos = new ("ObjectInfo") IArray<ObjInfo>(CountInfos(set), "rc_object_info");
    teles = new ("ObjectTeleport") IArray<ObjTeleport>(CountTeles(set), "rc_object_teleport");
    ok = 1;
    {
        for (i = 0; (c = set->GetChildAt(i)) != 0; i++) {
            if (strcmp(c->GetName(), "object")) {
                continue;
            }
            if (strcmp(c->GetValue("dyno"), "teleport")) {
                ObjInfo* o = infos->Add();
                const char* dyno = c->GetValue("dyno");
                int j;
                ValueNode* d;
                for (j = 0; (d = (*dynos)->GetChild(j)) != 0; j++) {
                    if (!strcmp(d->GetName(), "dyno") && !strcmp(d->GetValue("name"), dyno)) {
                        break;
                    }
                }
                if (!d) {
                    fn_802E317C("freestyle: unknown dyno type %s", dyno);
                }
                if (HasChild(d, "xlo_info")) {
                    o->type = 0;
                    o->xlo = d->FindChild("xlo_info");
                } else if (!strcmp(dyno, "teleport")) {
                    o->type = 2;
                } else {
                    o->type = 1;
                    o->model = fn_802C6FC0(d->GetValue("model"), 0, 0);
                }
                o->mtx = c->GetMatrix("unfloat_frame64");
                o->loaded = 1;
                if (!o->loaded) {
                    ok = 0;
                }
            } else if (!strcmp(c->GetValue("dyno"), "teleport")) {
                ObjTeleport* t = teles->Add();
                if (strcmp(c->GetValue("dyno"), "teleport")) {
                    fn_802E317C("Trying to load non-teleport object as teleport");
                }
                t->mtx = c->GetMatrix("unfloat_frame64");
                t->radius = c->GetFloat("radius", 80.0f);
                t->valid = 1;
                if (!t->valid) {
                    ok = 0;
                }
            }
        }
    }
    loaded = ok;
    return ok;
}

int Arena::CountFiles(ValueNode* cfg) {
    char name[0x1E];
    int n = 0;
    fn_801345D8(name, 0x1E, "object_file_0");
    while (cfg->Has(name)) {
        n++;
        fn_801345D8(name, 0x1E, "object_file_%i", n);
    }
    return n;
}

void ArenaTimer::Set(f32 t) { limit = t; }

PlayerData** PlayerList::Push(PlayerData* const& p) {
    PlayerData** s = &data[count++];
    *s = p;
    return s;
}

PlayerData::PlayerData(int i) {
    index = i;
    IdentGetCurrentSlot();
    fC = 0.0f;
}

void PlayerList::Clear(void) { count = 0; }

RingResults::RingResults(void) {
    layout = 0;
    sBonus = 0;
    sNoBonus = 0;
    s1C = 0;
    s24 = 0;
    s28 = 0;
    s2C = 0;
    memset(ringScore, '0', 3);
    ringScore[3] = 0;
    memset(timeLeft, '0', 3);
    timeLeft[3] = 0;
    memset(bonusTitle, '0', 3);
    bonusTitle[3] = 0;
    memset(finalScore, '0', 3);
    finalScore[3] = 0;
    memset(grade, '0', 3);
    grade[3] = 0;
    memset(noBonusScore, '0', 3);
    noBonusScore[3] = 0;
    memset(timeBonus, '0', 3);
    timeBonus[3] = 0;
    memset(bB2, '0', 3);
    bB2[3] = 0;
    memset(bC6, '0', 3);
    bC6[3] = 0;
    memset(complete, '0', 3);
    complete[3] = 0;
}

Arena::Arena(void) {
    dynos.t = fn_80131808("race.res/obstacle.val");
    art = fn_8023512C("mapgems.art");
    sRingHit = fn_802076A0("ringhit.sfx", 1, 0, 0);
    sCheerHit = fn_802076A0("cheerhit.sfx", 1, 0, 0);
    sTally = fn_802076A0("rctally.sfx", 1, 0, 0);
    sLongCheer = fn_802076A0("longcheer.sfx", 1, 0, 0);
    sMissFlag = fn_802076A0("missflag.sfx", 1, 0, 0);
    sLevelUp = fn_802076A0("levelup.sfx", 1, 0, 0);
    sFail = fn_802076A0("rcfail.sfx", 1, 0, 0);
    f24 = -1;
    b48 = 0;
    b49 = 0;
    b4A = 1;
    f28 = 0.0f;
    nSets = 0;
    nFiles = 0;
    if (sRingHit) {
        sRingHit->SetVolume(1.0f);
    }
    if (sCheerHit) {
        sCheerHit->SetVolume(1.0f);
    }
    if (sTally) {
        sTally->SetVolume(1.0f);
    }
    if (sLongCheer) {
        sLongCheer->SetVolume(1.0f);
    }
    if (sMissFlag) {
        sMissFlag->SetVolume(1.0f);
    }
    if (sLevelUp) {
        sLevelUp->SetVolume(1.0f);
    }
    if (sFail) {
        sFail->SetVolume(1.0f);
    }
    f40 = fn_80204CF0();
}

PlayerList::PlayerList(void) : ArrayRef<PlayerData*>(storage, 8) {}

/* Helper name unproven: the counting loop with the caller's counter by reference; the boundary is needed (in place: Size 100 -> 96.21) */
inline int Arena::CountDynosIn(int n, int& i) {
    const int count = sets->count;
    for (i = 0; i < count; i++) {
        int c = 0;
        const ObjSet& set = sets->data[i];
        int j;
        for (j = 0; j < set.infos->count; j++) {
            if (sets->data[i].infos->data[j].type == 0) {
                c++;
            }
        }
        n += c;
    }
    return n;
}

s32 Arena::Size(void) {
    int n = 0;
    int i;
    return CountDynosIn(n, i);
}


void Arena::Load(void) {
    struct { ObjInfo* o; int j; } sp_;  /* Struct unproven: fields of a small local (MWCC numbers struct fields as temps; structpair.py) */
    ObjSet* d;
    int i;
    for (i = 0; i < sets->count; i++) {
        d = sets->data;
        for (sp_.j = 0; sp_.j < d[i].infos->count; sp_.j++) {
            sp_.o = &d[i].infos->data[sp_.j];
            if (sp_.o->type == 0) {
                sp_.o->obj = fn_802BAA68(sp_.o->xlo, &sp_.o->mtx);
            }
        }
    }
}

#define SET_NAME(a, buf, n)                                               \
    switch ((a)->difficulty) {                                            \
    case 0:                                                               \
        fn_801345D8(buf, n, "set_%i_5", (a)->curSet);                     \
        break;                                                            \
    case 1:                                                               \
        fn_801345D8(buf, n, "set_%i_10", (a)->curSet);                    \
        break;                                                            \
    case 2:                                                               \
        fn_801345D8(buf, n, "set_%i_15", (a)->curSet);                    \
        break;                                                            \
    default:                                                              \
        fn_802E317C("Unknown difficulty specification in Ring Challenge mode"); \
        break;                                                            \
    }






/* Helper name unproven: shows every object of a set. */
static inline void ShowSetObjs(ObjSet* os) {
    int i;
    for (i = 0; i < os->infos->count; i++) {
        if (os->infos->data[i].obj) {
            fn_802BAF98(os->infos->data[i].obj, 1);
        }
    }
}

/* Helper name unproven: the arena set in use. */
static inline ObjSet* CurrentSet(Arena* ar) {
    return &ar->sets->data[ar->curSet];
}

static inline void LoadGemArt(Arena* ar) {
    {
        char name[0x1E];
        if (ar->f24 != -1) {
            if (ar->f24 != -1) {
                fn_8001B450(ar->f24);
                ar->f24 = -1;
            }
        }
        SET_NAME(ar, name, 0x1E);
        switch (ar->difficulty) {
        case 0: {
            GemRef g = fn_8016A6B0(name, 12);
            ar->f24 = fn_8001B308(&g, ar->art, 2);
            break;
        }
        case 1: {
            GemRef g = fn_8016A6B0(name, 9);
            ar->f24 = fn_8001B308(&g, ar->art, 2);
            break;
        }
        case 2: {
            GemRef g = fn_8016A6B0(name, 6);
            ar->f24 = fn_8001B308(&g, ar->art, 2);
            break;
        }
        default:
            fn_802E317C("Unknown difficulty specification in Ring Challenge mode");
            break;
        }
    }
}

extern "C" void fn_8016D294(int a, int b, int set) {
    RingChalMaster* m = lbl_8055E378;
    if (!m || b != 0) {
        return;
    }
    Arena* ar = &m->arena;
    ar->curSet = set;
    if (ar->curSet < ar->nFiles) {
        ObjSet* os = CurrentSet(ar);
        int i;
        os->active = 1;
        os->time = lbl_8055E3C8;
        os->f10 = 0;
        ShowSetObjs(os);
    }
    {
        char name[0x1E];
        SET_NAME(ar, name, 0x1E);
        fn_8016A15C(name);
    }
    LoadGemArt(ar);
}

inline RingChalMaster::~RingChalMaster(void) {
    int i;
    if (sPreRace) {
        delete sPreRace;
    }
    for (i = 0; i < players.count; i++) {
        delete players.data[i];
    }
    lbl_8055E378 = 0;
    fn_8016A68C();
}





// Helper name unproven: one player's ring-set logic for a frame. An inlined body, so the Arena methods defined
// after it stay calls while vf0C itself inlines StrCopyN (no dont_inline pragma needed).
inline void Arena::Step(PlayerData* pd) {
    char name[0x1E];
    SetName(name, 0x1E);
    if (!pd->Stuck() && !b49) {
        RaceCarRec* car = pd->Car();
        int hit;
        int left;
        stats->At(curSet)->score = timer.Get();
        hit = fn_8016A30C(car->Rings());
        left = fn_8016A780(name, car->Rings());
        if (left == 0 && !b4A) {
            b4A = 1;
            ActivateRingSet();
        }
        if (b4A && f24 == -1) {
            ShowSetGems();
        }
        if ((hit > 0 || left == fn_8016A878(name) || (left > 0 && car->Air() > 1)) && b4A) {
            stats->At(curSet)->time = Time();
            Deactivate(curSet);
            if (hit > 0) {
                stats->At(curSet)->done = 1;
                f34 = f30;
                {
                    int pts = stats->At(curSet)->best;
                    f30 += pts;
                    difficulty++;
                }
                if (difficulty > 2) {
                    difficulty = 2;
                }
                if (sRingHit) {
                    if (Playing__9GameSoundFv(sRingHit)) {
                        fn_80144C54(sRingHit);
                    }
                    Play__9GameSoundFv(sRingHit);
                }
                if (sCheerHit) {
                    if (Playing__9GameSoundFv(sCheerHit)) {
                        fn_80144C54(sCheerHit);
                    }
                    Play__9GameSoundFv(sCheerHit);
                }
                if (stats->At(curSet)->best == 10 || stats->At(curSet)->best == 15) {
                    if (sLongCheer) {
                        Play__9GameSoundFv(sLongCheer);
                    }
                }
            } else {
                if (--difficulty < 0) {
                    difficulty = 0;
                }
            }
            if (curSet < nSets - 1) {
                b4A = 0;
                curSet++;
                stats->At(curSet)->best = PointValue();
                ActivateObjSet();
            } else {
                b48 = 1;
                b49 = 1;
            }
        } else {
            sets->At(curSet)->CheckTeleport((Phob*)car);
        }
    } else if (f24 != -1) {
        ClearGems();
    }
}

// Helper name unproven: the countdown, score tally and cheer fade.
inline void Arena::Tally(void) {
    if (!b49) {
        f28 += 0.0166833512f;
        if (f28 > 1.75f) {
            f28 = 0.0f;
        }
        int expired = timer.t <= 0.0f;
        if (expired) {
            b49 = 1;
        } else if (fn_801B306C() <= 0.0f) {
            timer.t -= 0.0166833512f;
        }
    }
    if (curSet > 0) {
        int k = curSet - 1;
        if (b48) {
            k = curSet;
        }
        if (f34 < f30 && stats->data[k].done) {
            SetStat* st = &stats->data[k];
            f32 lo = 0.3f + (0.2f + 0.05f * st->best) + 0.05f * (st->best - (f30 - f34));
            int n = Lerp(0.0f, 1.0f, UnlerpClamped(lo, 0.25f + lo, Time() - st->time));
            if (n) {
                f34 += n;
                GameSound* tally = sTally;
                if (tally && !tally->playing && f34 != f30) {
                    tally->stop = 0;
                    tally->loop = 1;
                    tally->start = 1;
                }
                if (f34 == f30 && (tally = sTally) && tally->playing) {
                    tally->stop = 1;
                    tally->restart = 0;
                }
            }
        }
        if (sLongCheer && stats->data[k].done) {
            f32 a = 0.0f;
            f32 b = 0.0f;
            int lvl;
            switch (stats->data[k].best) {
            case 5:
                lvl = 0;
                break;
            case 10:
                lvl = 1;
                break;
            case 15:
                lvl = 2;
                break;
            default:
                fn_802E317C("Freestyle::get_ring_point_value_from_difficulty - Unknown point value mapping");
                lvl = 0;
                break;
            }
            switch (lvl) {
            case 1:
                a = 0.8f;
                b = 1.0f;
                break;
            case 2:
                a = 1.0f;
                b = 2.5f;
                break;
            }
            if (!NearZeroF(a)) {
                f32 t = UnlerpClamped(0.0f, b, lbl_8055E3C8 - stats->data[k].time);
                f32 v = Lerp(a, 0.0f, t);
                GameSound* cheer = sLongCheer;
                if (v != cheer->volume) {
                    cheer->volumeDirty = 1;
                    cheer->volume = v;
                }
            }
        }
    }
}

/* Helper name unproven */
inline void RingChalMaster::ResetGame(void) {
    int i;
    u32 playerOffset;
    i = 0;
    playerOffset = 0;
    for (; i < players.count; i++, playerOffset += sizeof(PlayerData*)) {
        PlayerData* pd = *reinterpret_cast<PlayerData**>(reinterpret_cast<u8*>(players.data) + playerOffset);
        pd->score = 0;
        pd->b8 = 0;
        pd->fC = 0.0f;
    }
    fn_8016A0D4();
    if (arena.f40 == 0) {
        arena.difficulty = 0;
    } else {
        arena.difficulty = 2;
    }
    arena.curSet = 0;
    arena.f34 = 0;
    arena.f30 = 0;
    arena.b48 = 0;
    arena.b49 = 0;
    arena.b4A = 1;
    arena.f28 = 0.0f;
    if (arena.nFiles < arena.nSets) {
        fn_802E30C8("WARNING: There are less Freestyle object sets than Freestyle ring sets");
    }
    arena.timer.Reset();
    for (i = 0; i < arena.nSets; i++) {
        arena.Deactivate(i);
    }
    for (i = 0; i < arena.nFiles; i++) {
        arena.sets->At(i)->Reset();
    }
    arena.ClearGems();
    if (arena.curSet >= 0 && arena.curSet < arena.nSets) {
        arena.ActivateObjSet();
        arena.ActivateRingSet();
        arena.ShowSetGems();
    }
    for (i = 0; i < arena.stats->Count(); i++) {
        arena.stats->At(i)->done = 0;
        arena.stats->At(i)->best = -1;
        arena.stats->At(i)->time = -100.0f;
        arena.stats->At(i)->score = -100.0f;
    }
    arena.stats->At(arena.curSet)->best = arena.PointValue();
    if (arena.sRingHit) {
        fn_80144C54(arena.sRingHit);
    }
    if (arena.sLongCheer) {
        fn_80144C54(arena.sLongCheer);
    }
    if (arena.sTally) {
        fn_80144C54(arena.sTally);
    }
    if (arena.sCheerHit) {
        fn_80144C54(arena.sCheerHit);
    }
    if (arena.sMissFlag) {
        fn_80144C54(arena.sMissFlag);
    }
    if (arena.sLevelUp) {
        fn_80144C54(arena.sLevelUp);
    }
    if (arena.sFail) {
        fn_80144C54(arena.sFail);
    }
}

inline void RingChalMaster::vf10(void) {
    ResetGame();
    b9C = 1;
    fA0 = -100.0f;
    results.Reset();
    Stop(sPreRace);
    bA8 = 0;
}


inline int Arena::PointValue(void) {
    switch (difficulty) {
    case 0:
        return 5;
    case 1:
        return 10;
    case 2:
        return 15;
    default:
        fn_802E317C("Unknown point value specification in Ring Challenge mode");
        return 0;
    }
}

template <class T>
inline T* RArray<T>::At(int i) {
    return &this->data[i];
}

inline void Arena::ShowSetGems(void) {
    char name[0x1E];
    if (f24 != -1) {
        if (f24 != -1) {
            fn_8001B450(f24);
            f24 = -1;
        }
    }
    SET_NAME(this, name, 0x1E);
    switch (difficulty) {
    case 0: {
        GemRef g = fn_8016A6B0(name, 12);
        f24 = fn_8001B308(&g, art, 2);
        break;
    }
    case 1: {
        GemRef g = fn_8016A6B0(name, 9);
        f24 = fn_8001B308(&g, art, 2);
        break;
    }
    case 2: {
        GemRef g = fn_8016A6B0(name, 6);
        f24 = fn_8001B308(&g, art, 2);
        break;
    }
    default:
        fn_802E317C("Unknown difficulty specification in Ring Challenge mode");
        break;
    }
}

inline void Arena::ActivateRingSet(void) {
    char name[0x1E];
    SET_NAME(this, name, 0x1E);
    fn_8016A15C(name);
}

inline void Arena::ActivateObjSet(void) {
    if (curSet < nFiles) {
        int i;
        ObjSet* s = &sets->data[curSet];
        u32 off;
        s->active = 1;
        s->time = lbl_8055E3C8;
        s->f10 = 0;
        // The element offset is stepped by hand next to the index (numfit: a named walker, not the compiler's).
        for (i = 0, off = 0; i < s->infos->count; off += sizeof(ObjInfo), i++) {
            ObjInfo* info = (ObjInfo*)((u8*)s->infos->data + off);
            if (info->obj) {
                fn_802BAF98(info->obj, 1);
            }
        }
    }
}

inline void Arena::ClearGems(void) {
    if (f24 != -1) {
        fn_8001B450(f24);
        f24 = -1;
    }
}

inline void ObjSet::Reset(void) {
    int i;
    active = 0;
    time = -100.0f;
    f10 = 0;
    for (i = 0; i < teles->count; i++) {
        teles->data[i].b30 = 0;
    }
}

inline void Arena::Deactivate(int i) {
    char name[0x1E];
    SET_NAME(this, name, 0x1E);
    fn_8016A240(name);
    if (i < nFiles) {
        int j;
        ObjSet* s = &sets->data[i];
        int off;  // MATCHING-AID: byte offset walker, needed for the loop registers
        s->active = 0;
        j = 0;
        off = 0;
        s->time = lbl_8055E3C8;
        for (; j < s->infos->count; off += sizeof(s->infos->data[0]), j++) {
            void* h = ((ObjInfo*)((u8*)s->infos->data + off))->obj;
            if (h) {
                fn_802BAF98(h, 0);
            }
        }
    }
    if (f24 != -1) {
        fn_8001B450(f24);
        f24 = -1;
    }
}

inline void ArenaTimer::Reset(void) { t = limit; }

inline int RingChalMaster::vf20(int car) const { return b9C; }

inline void RingChalMaster::vfB8(ZoneResults* r) {
    int i;
    r->nCars = players.count;
    r->nHumans = fn_80179650();
    r->done = 1;
    r->bE = 0;
    for (i = 0; i < players.count; i++) {
        r->entries[i].car = i;
        r->entries[i].b10 = 0;
        r->entries[i].score = players.data[i]->score;
        r->entries[i].place = 1;
        r->bC = 1;
    }
}

// Helper name unproven. One inline body: with the ArtPlayer constructor, the loader and Reset inside it, it is over
// MWCC's nested-inline budget (512 units, FACTS 2026-11-17 11:16), so those three stay calls with no pragma. The
// same member shape exists in zone (ZoneHud) and ringchal (RingResults).
inline void RingResults::Open(void) {
    layout = new ("ArtAnimManager") ArtPlayer(1);
    Load();
    Reset();
}

inline void RingChalMaster::vfF8(void) {
    layout = (ILayout*)fn_80145E70("freestyle.val");
    results.Open();
}

inline void RingResults::Reset(void) {
    state = 0;
    start = -1.0f;
    b8 = 1;
    b9 = 0;
    noBonus = 0;
    fF8 = 0;
    fFC = 0;
    f100 = 0;
    f104 = 0;
    if (layout) {
        ((ArtPlayer*)(layout))->StopAll();
    }
    Stop(sBonus);
    Stop(sNoBonus);
    Stop(s1C);
    b20 = 0;
    Stop(s24);
    Stop(s28);
    Stop(s2C);
}

static inline f32 Clamp01(f32 x) {
    f32 r = 1.0f < x ? 1.0f : x;
    return r > 0.0f ? r : 0.0f;
}

static inline f32 Ease(f32 t) {
    if (t < -1.0f) {
        return -1.0f;
    } else if (t > 1.0f) {
        return 1.0f;
    } else if (t > 0.0f) {
        return t * (-2.0f * t * t) + 3.0f * t * t;
    } else {
        return t * (-2.0f * t * t) - 3.0f * t * t;
    }
}

static inline void Blend(const TextElem* a, const TextElem* b, const char* text, f32 t) { fn_801479A8(a, b, text, t); }

static inline f32 Width(TextElem* e, const char* text) { return fn_80147204(e, text); }

inline int RingChalMaster::vf3C(int view) {
    f32 t = fn_801E7E3C()->time;
    f32 end0 = 6.75f;
    f32 in0 = end0 - 0.8f;
    f32 full0 = in0 - 0.3f;
    f32 end1 = full0 - 0.15f;
    f32 in1 = end1 - 0.8f;
    f32 full1 = in1 - 0.3f;
    TextElem* h0 = layout->Find("intro_head_0");
    TextElem* h1 = layout->Find("intro_head_1");
    if (t < full1) {
        fn_8000FBE4();
    }
    if (!fn_80161C38()) {
        if (t < end0) {
            f32 k = Lerp(0.0f, 1.0f, Clamp01((t - in0) / (full0 - in0)));
            TextElem* body = layout->Find("star_body_0");
            TextElem* denom = layout->Find("star_body_1");
            UpdateScoreOverlays(fn_801E866C(view), 0);
            if (h0 && h1 && body && denom) {
                char buf[0x14];
                Blend(h0, h1, fn_80131234("PreRaceOverlay:STARS_NEEDED"), Ease(k));
                fn_801308F4(buf, sizeof(buf), fn_80160998(fn_80160A74()));
                {
                    TextElem e;
                    e = *body;
                    e.pos.x -= 0.5f * Width(&e, buf);
                    Blend(&e, denom, buf, Ease(k));
                }
            }
        }
        if (t < end1) {
            f32 k;
            TextElem* timer;
            fn_8014784C(layout, "stop_watch_art", "stop_watch_artset", 0);
            k = Lerp(0.0f, 1.0f, Clamp01((t - in1) / (full1 - in1)));
            TextElem* body = layout->Find("time_body_0");
            timer = layout->Find("timer");
            if (h0 && h1 && body && timer) {
                char buf[0x14];
                Blend(h0, h1, fn_80131234("PreRaceOverlay:TIME_LIMIT"), Ease(k));
                fn_80130978(buf, sizeof(buf), arena.timer.t);
                {
                    TextElem e;
                    e = *body;
                    e.pos.x += 0.5f * Width(&e, buf);
                    Blend(&e, timer, buf, Ease(k));
                }
            }
        }
    }
    return 1;
}

inline f32 RingChalMaster::vf38(void) { return 7.75f; }

inline void RingResults::Load(void) {
    ValueBinding b[16] = {
        Bind(3, "time_left", &fF8),
        Bind(3, "time_bonus", &fFC),
        Bind(3, "grade", &f100),
        Bind(3, "final_score", &f104),
        Bind(1, "ring_score", ringScore),
        Bind(1, "time_bonus", timeLeft),
        Bind(1, "time_bonus_title_caps", bonusTitle),
        Bind(1, "final_score", finalScore),
        Bind(1, "grade", grade),
        Bind(1, "final_score_no_time_bonus", noBonusScore),
        Bind(1, "time_left", timeBonus),
        Bind(1, "challenge_complete_1", bB2),
        Bind(1, "challenge_complete_2", bC6),
        Bind(1, "out_of_time_message", complete),
        Bind(2, "difficulty_level", &fF0),
        lbl_802FAA40,
    };
    StrCopyN(noBonusScore, "100", 0x14);
    StrCopyN(timeLeft, "50", 0x14);
    StrCopyN(finalScore, "150", 0x14);
    fn_80130980(timeBonus, 0x14, 2, 75.0f);
    fF0 = fn_80204CF0() + 1;
    fF4 = ((ArtPlayer*)(layout))->Add("a_ring_finish.val", (int)(b));
    sBonus = fn_80207848("pr_ringchal1.sfx", 1);
    sNoBonus = fn_80207848("pr_ringchal1_nobonus.sfx", 1);
    s1C = fn_80207848("pr_ringchal2.sfx", 1);
    s24 = fn_80207848("pr_transition.sfx", 1);
    s28 = fn_80207848("levelup.sfx", 1);
    s2C = fn_80207848("timeout.sfx", 1);
}

inline void RingChalMaster::vf40(void) {
    int i;
    int j;
    ObjSet* s;
    int wo;
    int off;
    for (i = 0, wo = 0; i < arena.nFiles; wo += sizeof(arena.sets->data[0]), i++) {
        s = (ObjSet*)((u8*)arena.sets->data + wo);
        if (s->active) {
            for (j = 0, off = 0; j < s->infos->count; off += sizeof(ObjInfo), j++) {
                ObjInfo* o = (ObjInfo*)((u8*)s->infos->data + off);
                if (o->type == 1) {
                    fn_802C744C(o->model, &o->mtx);
                }
            }
        } else if (!s->active) {
            f32 t = lbl_8055E3C8 - s->time;
            if (t < 0.25f) {
                f32 a = Lerp(1.0f, 0.0f, UnlerpClamped(0.0f, 0.25f, t));
                if (a > 0.01f) {
                    int off;
                    int j;
                    fn_802C9900(a);
                    for (j = 0, off = 0; j < s->infos->count; off += sizeof(s->infos->data[0]), j++) {
                        ObjInfo* o = (ObjInfo*)((u8*)s->infos->data + off);
                        if (o->type == 1) {
                            fn_802C744C(o->model, &o->mtx);
                        }
                    }
                    fn_802C9904();
                }
            }
        }
    }
}

inline void RingChalMaster::vf44(void) {
    int any = 0;
    int i;
    for (i = 0; i < players.count; i++) {
        if (players.data[i]->b8) {
            any = 1;
            break;
        }
    }
    if (!any) {
        fn_8016A438();
    }
}

inline void RingChalMaster::vf50(int p) { vf4C(p); }

inline int RingChalMaster::vf54(int p) { return 0x1408; }


inline int RingChalMaster::vf24(void) const {
    int r = 0;
    if (fn_801B8514() && arena.b49 && !b9C) {
        r = 1;
    }
    return r;
}

inline int RingChalMaster::vf68(void) const {
    if (vf24()) {
        return results.state == 2;
    }
    return 0;
}

inline int RingChalMaster::vf64(void) { return 1; }

inline int RingChalMaster::vf5C(int p) const {
    if (lbl_8055E3C8 - fA0 < 2.25f) {
        vf4C(p);
    } else {
        if (results.b8) {
            vf4C(p);
        }
        if (results.layout) {
            ((ArtPlayer*)(results.layout))->Draw(0);
        }
    }
    if (vf68()) {
        UpdateContinueOverlay();
    }
    return results.state != 4;
}

inline int RingChalMaster::vf8C(int p, int key) {
    if (vf68() && key == 'X') {
        if (results.state == 2) {
            results.b9 = 1;
        }
        return 1;
    }
    if (results.Active()) {
        f32 now = lbl_8055E3C8;
        if (results.state == 1 && !results.noBonus && results.start > 0.0f && now - results.start < 1.0f) {
            results.noBonus = 1;
            if (results.layout) {
                results.layout->Rewind();
            }
            Stop(results.sBonus);
            Stop(results.sNoBonus);
            Stop(results.s28);
            Stop(results.s2C);
            results.start = lbl_8055E3C8 - 1.0f;
            results.b8 = 0;
        }
        return 1;
    }
    return 0;
}

inline int RingChalMaster::Started1C(int car) { return !arena.b49; }

inline void RingChalMaster::vfA8(int car, int b) { fn_801B0C78(car); }

static inline void ScaleV(ValVec3* o, const ValVec3* a, f32 k) {
    o->x = a->x * k;
    o->y = a->y * k;
    o->z = a->z * k;
}

inline int RingChalMaster::vfB0(int car, Respawn* r, int c) {
    PlayerData* p = players.data[car];
    p->b8 = 0;
    p->fC = 0.0f;
    if (arena.curSet < arena.nFiles) {
        ObjSet* s = &arena.sets->data[arena.curSet];
        if (s->teles && s->teles->count > 0 && s->teles->data[s->f10].b30) {
            ObjTeleport* t = s->Next();
            fn_801B8060(r, t);
            ScaleV(&r->vel, &t->mtx.basis[2], 26.666666f);
            r->place.origin.y += 7.625f;
            return 1;
        }
        return 0;
    }
    return 0;
}

inline void RingChalMaster::vfFC(void) {
    results.Unload();
    delete layout;
}

static inline int Stopped(f32 speed) { return NearZeroF(speed); }

// The teleport test: 1 when the car is at or facing away from the current teleport (out = held long enough),
// 0 otherwise; reaching the last teleport clears it. Its 'return 0' paths share one block in retail.
/* Helper name unproven */
static inline int TeleCheck(Arena& arena, PlayerData* p, u8* out) {
    ObjSet* s;
    if (arena.curSet < arena.nFiles) {
        s = &arena.sets->data[arena.curSet];
        if (s->teles && s->teles->count > 0 && s->teles->data[s->f10].b30) {
            RaceCarRec* c = fn_8017960C(p->index);
            ObjTeleport* t = &s->teles->data[s->f10];
            V2 d;
            V2 a;
            V2 b;
            bool outside;
            int behind;
            f32 dz;
            f32 dx;
            a.x = t->mtx.origin.x;
            a.y = t->mtx.origin.z;
            b.x = c->mtx.pos.x;
            b.y = c->mtx.pos.z;
            dx = a.x - b.x;
            dz = a.y - b.y;
            d.x = dx;
            d.y = dz;
            f64 wide = sqrt(d.x * d.x + d.y * d.y);
            f32 radius = t->radius;
            f32 distance = (f32)wide;
            outside = distance > radius;
            behind = PSVECDotProduct((const Vec*)&t->mtx.basis[2], (const Vec*)((const ValVec3*)&c->mtx.rot.z)) < 0.0f;
            if (s->f10 == s->teles->count - 1 && outside) {
                s->teles->data[s->f10].b30 = 0;
            } else {
                if (outside || behind) {
                    if (!p->b8) {
                        p->b8 = 1;
                        p->fC = 0.0f;
                    }
                    if (p->fC > 2.0f) {
                        *out = 1;
                    } else {
                        *out = 0;
                    }
                } else {
                    p->b8 = 0;
                    p->fC = 0.0f;
                    *out = 0;
                }
                return 1;
            }
        }
        return 0;
    }
    return 0;
}


inline int RingChalMaster::vfE0(int car, u8* out) {
    int r = 0;
    f32 now = lbl_8055E3C8;
    if (now > vf38() && !arena.b48) {
        if (Stopped(fn_8017960C(players.data[car]->index)->f2690)) {
            r = TeleCheck(arena, players.data[car], out);
            if (!r) {
                if ((u32)fn_8017960C(players.data[car]->index)->type == 'PCAR') {
                    RaceCarRec* c = fn_8017960C(players.data[car]->index);
                    PlayerData* q;
                    if ((((SpringCar*)c)->ctl->vf14() & 0x10) && !(q = players.data[car])->b8) {
                        q->b8 = 1;
                        q->fC = 0.0f;
                    } else if (!(((SpringCar*)c)->ctl->vf14() & 0x10) && (q = players.data[car])->b8) {
                        q->b8 = 0;
                        q->fC = 0.0f;
                    }
                }
            }
        }
    }
    return r;
}

static inline void Sub(V2& r, const V2& a, const V2& b) {
    r.x = a.x - b.x;
    r.y = a.y - b.y;
}

static inline f32 LenSq(const V2& v) { return v.x * v.x + v.y * v.y; }

static inline V2 MidPoint(const V2& a, const V2& b) {
    V2 m;
    m.y = a.y + (b.y - a.y) / 2.0f;
    m.x = a.x + 0.5f * (b.x - a.x);
    return m;
}

static inline void Normalize(V2& v) {
    f32 inv = 1.0f / (f32)sqrt(LenSq(v));
    v.x *= inv;
    v.y *= inv;
}

static inline f32 PathLerp(f32 a, f32 b, f32 k) { return a + k * (b - a); }

#pragma push
#pragma opt_common_subs off  /* Pragma unproven: helps but not exact (PRAGSCAN_2026-11-15) */
/* Helper name unproven */
static inline void ReadBound(ValueNode* n, int k, V2* w) {
    char b[0x10];
    fn_801345D8(b, 0x10, "bound%d", k);
    ValPoint p = n->GetPoint(b);
    w->x = p.x;
    w->y = p.y;
}

inline int RingChalMaster::vfA0(Respawn* out, int idx, int c) {
    int i;
    int k;
    V2* w;
    ValueNode* cfg = fn_8000AC38()->vals;
    if (cfg) {
        char name[0x10];
        ValueNode* sp;
        fn_801345D8(name, 0x10, "start_pos_%i", idx);
        sp = cfg->FindChild(name);
        if (sp) {
            V2 pts[3];
            V2 mid;
            V2 dir;
            f32 best;
            f32 l;
            mid.x = 0.0f;
            mid.y = 0.0f;
            dir.x = 0.0f;
            dir.y = 0.0f;
            w = pts;
            for (i = 0; i < 3; i++, w++) {
                ReadBound(sp, i, w);
            }
            k = 2;
            {
                V2 d;
                Sub(d, pts[2], pts[0]);
                best = LenSq(d);
            }
            {
                V2 e;
                Sub(e, pts[0], pts[1]);
                l = LenSq(e);
                if (l > best) {
                    best = l;
                    k = 0;
                }
                Sub(e, pts[1], pts[2]);
                if (LenSq(e) > best) {
                    k = 1;
                }
            }
            mid = MidPoint(pts[k], pts[(k + 1) % 3]);
            Sub(dir, pts[(k + 2) % 3], mid);
            Normalize(dir);
            if (sp->GetBool("on_racingline", 1)) {
                LineFollower lf;
                lf.SetLine((CenterLine*)fn_80156A40()->GetRacingLine(0));
                ILPos pos;
                pos.seg = 0;
                pos.mode = 10000;
                pos.t = 0.0f;
                lf.Locate(&mid, 0, &pos, 0);
                mid.x = PathLerp(pos.seg->pos.x, pos.seg->next->pos.x, pos.t);
                mid.y = PathLerp(pos.seg->pos.y, pos.seg->next->pos.y, pos.t);
                dir.x = PathLerp(pos.seg->dir.x, pos.seg->next->dir.x, pos.t);
                dir.y = PathLerp(pos.seg->dir.y, pos.seg->next->dir.y, pos.t);
                Normalize(dir);
            }
            fn_801B7C8C(out, &mid, &dir);
            {
                f32 s = (4.0f / 9.0f) * sp->GetFloat("speed", 0.0f);
                out->vel.x = out->place.basis[2].x * s;
                out->vel.y = out->place.basis[2].y * s;
                out->vel.z = out->place.basis[2].z * s;
            }
            return 1;
        }
    }
    return 0;
}
#pragma pop

#define ALPHA(c, v) ((c).all = ((int)(255.0f * (v)) << 24) | ((c).all & 0xFFFFFF))

// The set list and the ring graphics. Everything but the score overlay; the stack puts its locals in rising order.
inline void RingChalMaster::DrawRings(void) const {
    TextElem* setScore;
    Box* setIcon;
    Box* arrowIcon;
    Box* starScore;
    Box* target;
    Box* next;
    int s;
    int i;
    int n;
    int j;
    TextElem* nr;
    ILayout* l;
    l = layout;
    arena.timer.Draw(l);
    setScore = l->Find("ring_set_score");
    setIcon = (Box*)l->FindBox("ring_set_icon");
    arrowIcon = (Box*)l->FindBox("arrow_icon");
    starScore = (Box*)l->FindBox("star_score");
    for (i = 0; i < arena.nSets; i++) {
        Box icon;
        icon = *setIcon;
        TextElem txt = *setScore;
        f32 dt;
        dt = Time() - arena.stats->At(i)->time;
        icon.pos.y += i * 40;
        txt.pos.y += i * 40;
        if (i > arena.curSet) {
            icon.color.all = 0x66FFFFFF;
        }
        fn_80147940(l, &icon, "star_art", 0);
        if (i < arena.curSet || arena.b48) {
            n = 0;
            if (arena.stats->At(i)->done) {
                f32 a;
                f32 b;
                n = dt / 0.05f;
                if (n > arena.stats->At(i)->best) {
                    n = arena.stats->At(i)->best;
                }
                a = 0.2f + 0.05f * arena.stats->At(i)->best;
                b = 0.25f + 0.05f * arena.stats->At(i)->best;
                if (dt < b + (0.3f + a)) {
                    f32 sx = icon.pos.x;
                    f32 sy = icon.pos.y;
                    f32 x0 = starScore->pos.x - icon.w * (arena.stats->At(i)->best / 2.0f);
                    f32 y0 = starScore->pos.y;
                    f32 cx = l->Find("score_counter")->pos.x;
                    f32 cy = l->Find("score_counter")->pos.y;
                    for (j = 0; j < arena.stats->At(i)->best; j++) {
                        Box star;
                        star = *starScore;
                        if (dt < a) {
                            f32 k = Remap(0.05f * j, 0.2f + 0.05f * j, 0.0f, 1.0f, dt);
                            star.pos.x = fn_80144C80(sx, x0 + j * star.w, fn_80144BD8(k));
                            star.pos.y = fn_80144C80(sy, y0, fn_80144BD8(k));
                            ALPHA(star.color, k);
                        } else if (dt > 0.3f + a + 0.05f * j && dt - (0.3f + a) < b) {
                            f32 u = dt - a - 0.3f;
                            if (u > 0.05f * j) {
                                f32 k = Remap(0.0f, 0.25f, 0.0f, 1.0f, u - 0.05f * j);
                                f32 k2 = Remap(0.0f, 0.2f, 0.0f, 1.0f, u - 0.05f * j);
                                star.pos.x = fn_80144C80(x0 + j * star.w, cx, k);
                                star.pos.y = fn_80144C80(y0, cy, k);
                                ALPHA(star.color, 1.0f - k2);
                            }
                        } else {
                            star.color.all = 0xFFFFFFFF;
                            star.pos.y = y0;
                            star.pos.x = x0 + j * star.w;
                        }
                        fn_80147940(l, &star, "star_art", 0);
                    }
                }
                if (dt < 0.2f) {
                    f32 k = Remap(0.0f, 0.2f, 0.0f, 1.0f, dt);
                    txt.scale = fn_80144C80(4.0f, txt.scale, k);
                    ALPHA(txt.color, k);
                }
            }
            fn_80147230(&txt, "%i", n);
        } else if (i == arena.curSet && !arena.b49) {
            Box hl;
            f32 k;
            hl = icon;
            k = Remap(0.0f, 0.75f, 0.0f, 1.0f, arena.f28);
            hl.w = fn_80144C80(icon.w, 2.5f * icon.w, k);
            hl.h = fn_80144C80(icon.h, 2.5f * icon.h, k);
            hl.pos.x = fn_80144C80(icon.pos.x, icon.pos.x - (2.5f * icon.w - icon.w) / 2.0f, k);
            hl.pos.y = fn_80144C80(icon.pos.y, icon.pos.y - (2.5f * icon.h - icon.h) / 2.0f, k);
            ALPHA(hl.color, 1.0f - k);
            fn_80147940(l, &hl, "star_art", 0);
        }
        if (dt < 3.4f) {
            nr = l->Find("next_ring");
            TextElem* msg;
            if (arena.stats->At(i)->done) {
                msg = l->Find("ring_hit");
            } else {
                msg = l->Find("all_rings_missed");
            }
            if (msg) {
                TextElem m = *msg;
                if (dt < 0.1f) {
                    f32 k;
                    if (!Playing__9GameSoundFv(arena.sMissFlag)) {
                        Play__9GameSoundFv(arena.sMissFlag);
                    }
                    k = Remap(0.0f, 0.1f, 0.0f, 1.0f, dt);
                    m.scale = fn_80144C80(10.0f, m.scale, k);
                    ALPHA(m.color, k);
                } else if (dt > 1.6f) {
                    f32 k = Remap(1.5f, 1.6f, 0.0f, 1.0f, dt);
                    ALPHA(m.color, 1.0f - k);
                }
                fn_801476C4(&m);
            }
            if (dt > 1.7f && nr && !arena.b49) {
                TextElem n = *nr;
                f32 d2 = dt - 1.7f;
                if (d2 < 0.1f) {
                    f32 k;
                    if (!Playing__9GameSoundFv(arena.sMissFlag)) {
                        Play__9GameSoundFv(arena.sMissFlag);
                    }
                    k = Remap(0.0f, 0.1f, 0.0f, 1.0f, d2);
                    n.scale = fn_80144C80(10.0f, n.scale, k);
                    ALPHA(n.color, k);
                } else if (d2 > 1.6f) {
                    f32 k = Remap(1.6f, 1.7f, 0.0f, 1.0f, d2);
                    n.scale = fn_80144C80(n.scale, 10.0f, k);
                    ALPHA(n.color, 1.0f - k);
                }
                fn_801476C4(&n);
            }
        }
    }
    if (!arena.b49) {
        Box arrow;
        arrow = *arrowIcon;
        if (arena.curSet > 0) {
            f32 k = Remap(0.0f, 0.1f, 0.0f, 1.0f, Time() - arena.stats->At(arena.curSet - 1)->time);
            int s = arena.curSet;
            arrow.pos.y = fn_80144C80(arrowIcon->pos.y + (s - 1) * 40, arrowIcon->pos.y + s * 40, fn_80144BD8(k));
        }
        fn_80147940(l, &arrow, "arrow_art", 0);
    }
    {
        Box tr;
        if (fn_802C9A58()) {
            target = (Box*)l->FindBox("target_ring_ws");
        } else {
            target = (Box*)l->FindBox("target_ring");
        }
        next = (Box*)l->FindBox("next_ring");
        tr = *target;
        s = arena.curSet;
        if (arena.curSet > 0 && !arena.b49) {
            f32 k = 1.7f + 0.2f;
            f32 a = Time() - arena.stats->At(arena.curSet - 1)->time;
            f32 d = a - k;
            if (Time() - arena.stats->At(arena.curSet - 1)->time > 0.0f && d <= 2.2f) {
                s = arena.curSet - 1;
            }
            if (d >= 0.0f) {
                Box nx;
                nx = *next;
                if (d <= 0.15f) {
                    f32 k;
                    if (arena.stats->At(arena.curSet - 1)->done) {
                        if (!Playing__9GameSoundFv(arena.sLevelUp)) {
                            Play__9GameSoundFv(arena.sLevelUp);
                        }
                    } else {
                        if (!Playing__9GameSoundFv(arena.sFail)) {
                            Play__9GameSoundFv(arena.sFail);
                        }
                    }
                    k = Remap(0.0f, 0.15f, 0.0f, 1.0f, d);
                    ALPHA(nx.color, k);
                    nx.w = fn_80144C80(10.0f * next->w, next->w, k);
                    nx.h = fn_80144C80(10.0f * next->h, next->h, k);
                    nx.pos.x = fn_80144C80(next->pos.x - 10.0f * next->w / 2.0f, next->pos.x, k);
                    nx.pos.y = fn_80144C80(next->pos.y - 10.0f * next->h / 2.0f, next->pos.y, k);
                }
                if (d > 1.35f) {
                    f32 k = Remap(1.35f, 1.7f + 0.2f, 0.0f, 1.0f, d);
                    nx.pos.x = fn_80144C80(next->pos.x, target->pos.x, fn_80144BD8(k));
                    nx.pos.y = fn_80144C80(next->pos.y, target->pos.y, fn_80144BD8(k));
                    ALPHA(tr.color, 1.0f - Remap(1.5f, 1.65f, 0.0f, 1.0f, d));
                }
                switch (arena.stats->At(arena.curSet)->best) {
                case 15:
                    fn_80147940(l, &nx, "ring_15_art", 0);
                    break;
                case 10:
                    fn_80147940(l, &nx, "ring_10_art", 0);
                    break;
                case 5:
                    fn_80147940(l, &nx, "ring_5_art", 0);
                    break;
                }
            }
        }
        if (!arena.b49) {
            switch (arena.stats->At(s)->best) {
            case 15:
                fn_80147940(l, &tr, "ring_15_art", 0);
                break;
            case 10:
                fn_80147940(l, &tr, "ring_10_art", 0);
                break;
            case 5:
                fn_80147940(l, &tr, "ring_5_art", 0);
                break;
            }
            DrawText(l, "ring_label");
        }
    }
}

inline void RingChalMaster::vf4C(int view) const {
    {
        PlayerData* p = players.data[fn_801E866C(view)];
        if (p) {
            UpdateScoreOverlays(p->index, p->score);
        }
    }
    DrawRings();
}

inline void ArenaTimer::Draw(ILayout* l) const {
    char buf[0x20];
    f32 shown = MaxF(0.0f, t);
    fn_80130978(buf, 0x20, shown);
    fn_8014784C(l, "stop_watch_art", "stop_watch_artset", 0);
    fn_801475D0(l, "timer", "%s", buf);
}

inline void RingChalMaster::vf0C(void) {
    f32 rt = fn_801B306C();
    if (!bA8 && rt < 6.75f) {
        bA8 = 1;
        if (sPreRace && !fn_80161C38() && fn_80179650() == 1 && !fn_80009C84()) {
            GameSound* pre = sPreRace;
            pre->start = 1;
            pre->loop = 0;
            pre->stop = 0;
        }
    }
    f32 now = lbl_8055E3C8;
    if (now > vf38() && arena.nSets > 0) {
        int i;
        for (i = 0; i < players.count; i++) {
            if (!vf24()) {
                if (players.data[i]->b8) {
                    players.data[i]->fC += 0.0166833512f;
                }
                arena.Step(players.data[i]);
            }
            s32 playerScore = players.data[i]->score;
            s32 arenaScore = arena.f34;
            if (playerScore != arenaScore) {
                players.data[i]->score = arena.f34;
            }
            if (b9C && arena.b49) {
                if (fA0 < 0.0f) {
                    fA0 = lbl_8055E3C8;
                }
                if (arena.b48) {
                    StrCopyN(results.bB2, fn_80131234("ResultsOverlay:CHALLENGE_COMPLETE_1"), 0x14);
                    StrCopyN(results.bC6, fn_80131234("ResultsOverlay:CHALLENGE_COMPLETE_2"), 0x14);
                    StrCopyN(results.complete, "", 0x14);
                } else {
                    StrCopyN(results.complete, fn_80131234("ResultsOverlay:TIMED_OUT"), 0x14);
                    StrCopyN(results.bB2, "", 0x14);
                    StrCopyN(results.bC6, "", 0x14);
                }
                {
                    // Two casts of the time, the grade's first: every cast owns its own conversion slot, handed out in
                    // source order, and retail reads the grade's from the lower one (FACTS 2026-11-17 12:50).
                    int gradeSecs = (int)arena.timer.t;
                    int secs = (int)arena.timer.t;
                    int total = arena.f30;
                    fn_801CB5DC(i, total + secs, fn_801609C4(total + gradeSecs));
                }
                fn_801CB648(i, 1, arena.timer.t);
                b9C = 0;
            }
        }
        arena.Tally();
    }
    if (vf24() && !(lbl_8055E3C8 - fA0 < 2.25f)) {
        int secs = (int)arena.timer.t;
        results.Update(arena.f30 + secs, arena.f30, secs);
    }
}

inline void ObjSet::CheckTeleport(Phob* car) {
    ObjTeleport* data;
    ObjTeleport* t;
    int cur = f10;
    int next = cur + 1;
    if (next < teles->count) {
        data = teles->data;
        t = &data[next];
        V2 a;
        V2 b;
        V2 d;
        f32 dx;
        f32 dz;
        a.x = t->mtx.origin.x;
        a.y = t->mtx.origin.z;
        b.x = car->mtx.origin.x;
        b.y = car->mtx.origin.z;
        dx = a.x - b.x;
        dz = a.y - b.y;
        d.x = dx;
        d.y = dz;
        f64 wide = sqrt(d.x * d.x + d.y * d.y);
        f32 distance = (f32)wide;
        f32 radius = t->radius;
        if (distance < radius) {
            data[cur].b30 = 0;
            f10 = next;
            teles->data[f10].b30 = 1;
        }
    }
}

inline void* RaceCarRec::Rings(void) { return (void*)index; }  // the ring record at 0x1EC

inline f32 ArenaTimer::Get(void) { return t; }

inline RaceCarRec* PlayerData::Car(void) { return fn_8017960C(index); }

inline int PlayerData::Stuck(void) { return b8; }

inline void Arena::SetName(char* buf, int n) { SET_NAME(this, buf, n); }



static inline f32 Time(void) { return lbl_8055E3C8; }

// TextElem's member-wise copy, defined after its uses, so this unit calls the out-of-line weak copy (retail's kept
// copy is betarace's).
inline TextElem::TextElem(const TextElem& o)
    : name(o.name), pos(o.pos), elemJust(o.elemJust), scale(o.scale), just(o.just), color(o.color), defText(o.defText) {}

// GameMode's constructor after its use: RingChalMaster's constructor calls the out-of-line copy.
#include "game/gamemodector.h"
