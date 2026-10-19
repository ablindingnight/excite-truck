// Ring challenge master (0x8016B964-0x80171698): the "ringchalmaster" game mode. It loads the Freestyle
// arena's object sets ("object_file_%d", dynos and teleports), counts the ring sets in the ring file, runs the
// timed ring challenge with its pre-race and results overlays ("ResultsOverlay:*", "PreRaceOverlay:*") and
// scores rings by size and difficulty. Names are guesses apart from the strings.
#include "game/common.h"

class RSound;

extern "C" {
void* memset(void* dst, int c, u32 n);
int strcmp(const char* a, const char* b);
int strcasecmp(const char* a, const char* b);
char* strstr(const char* s, const char* t);
u32 strlen(const char* s);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // warning print
void fn_801345D8(char* buf, int size, const char* fmt, ...);
void fn_801308F4(char* buf, int size, int value);         // number with separators
void fn_80130980(char* buf, int size, int digits, f32 v);  // fixed-point number
int fn_801609C4(int score);                                // grade for a score
void fn_802ADFF4(void* layout, s32 a);
void fn_802AE02C(void* layout, s32 a);
void fn_802AE0B8(void* layout, s32 a);
void fn_802AE168(void* layout);
ValueTable* fn_80131808(const char* name);  // open .val table
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* t);  // close .val table
void* fn_8023512C(const char* name);
void fn_80235380(void* art);
s32 fn_80204CF0(void);
s32 fn_80204EA0(const char* key);
void fn_802C2914(void* p);
void fn_802C2930(void* p, int flag);
void fn_802C6CF8(s32 model);
s32 fn_802C6FC0(const char* name, int a, int b);
void* fn_802BAA68(void* xlo, const ValMatrix* m);
void fn_8016A0B0(void);
void fn_80144504(void* p);
void fn_802ADE60(void* layout, int flag);
void fn_8016A15C(const char* name);  // activate ring set
void fn_8016A240(const char* name);  // deactivate ring set
void fn_8016A0D4(void);
void fn_8016A68C(void);
void fn_8001B450(s32 h);
s32 fn_802BAF98(void* obj, int on);
void fn_80144C54(RSound* s);
void fn_802AE0E0(void* layout);
s32 fn_80179650(void);
s32 fn_80145E70(const char* name);
void fn_802ADDD8(void* p, int a);
}

struct V2 {
    f32 x;
    f32 y;
};

struct TextElem {
    TextElem(void) {}
    const char* name;     // offset 0x0
    V2 pos;               // offset 0x4
    s32 elemJust;         // offset 0xC
    f32 scale;            // offset 0x10
    u32 just;             // offset 0x14
    u32 color;            // offset 0x18
    const char* defText;  // offset 0x1C
};

class ILayout {
public:
    virtual ~ILayout(void) {}
    virtual TextElem* Find(const char* name) = 0;
};

struct RaceSetup {
    u8 pad0[0x10];
    f32 time;  // offset 0x10
};

struct Binding {
    s32 type;          // offset 0x0
    const char* name;  // offset 0x4
    void* ptr;         // offset 0x8
};

static inline Binding Bind(s32 t, const char* n, void* p) {
    Binding b;
    b.type = t;
    b.name = n;
    b.ptr = p;
    return b;
}

extern const Binding lbl_802FAA40;

extern "C" {
RaceSetup* fn_801E7E3C(void);
void fn_8000FBE4(void);
int fn_80161C38(void);
void fn_8001DF08(int id, s32 score);
int fn_801E866C(int view);
const char* fn_80131234(const char* key);
void fn_801479A8(const TextElem* a, const TextElem* b, const char* fmt, f32 t, ...);
f32 fn_80147204(TextElem* e, const char* text);
s32 fn_80160998(s32 a);
s32 fn_80160A74(void);
void fn_8014784C(ILayout* l, const char* box, const char* set, int part);
void fn_80130978(char* buf, int size, f32 v);
s32 fn_802ADF34(void* layout, const char* name, Binding* b);
}

struct Phob {
    u8 pad0[0x60];
    ValVec3 pos;  // offset 0x60
};

struct Respawn {
    ValVec3 basis[3];  // offset 0x0
    ValVec3 pos;       // offset 0x24
    ValVec3 vel;       // offset 0x30
};

extern "C" {
double sqrt(double x);
void fn_802C744C(s32 model, const ValMatrix* m);
void fn_802C9900(f32 alpha);
void fn_802C9904(void);
void fn_8016A438(void);
int fn_801B8514(void);
void fn_802AE220(void* layout, int a);
void fn_8001DFCC(void);
void fn_802AE118(void* layout);
void fn_801B0C78(int car);
}

struct GemRef {
    s32 a;
    s32 b;
};

extern "C" GemRef fn_8016A6B0(const char* name, int n);
extern "C" s32 fn_8001B308(GemRef* g, void* art, int a);
extern "C" void fn_801B8060(struct Respawn* r, struct ObjTeleport* t);

const char* GetGradeDisplayName(int grade);
int IdentGetCurrentSlot(void);
void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

struct ConfigRoot {
    u8 pad0[0x20];
    ValueNode* vals;  // offset 0x20
};

extern "C" ConfigRoot* fn_8000AC38(void);

extern f32 lbl_8055E3C8;  // time
extern s32 lbl_8055E044;  // language

class RSound {
public:
    virtual ~RSound(void);
    u8 pad4[0x4];
    f32 vol;  // offset 0x8
    u8 padC[0x19 - 0xC];
    u8 busy;  // offset 0x19
    u8 pad1A[0x49 - 0x1A];
    u8 playing;  // offset 0x49
    u8 stop;     // offset 0x4A
    u8 loop;     // offset 0x4B
    u8 f4C;      // offset 0x4C
    u8 dirty;    // offset 0x4D
    void SetVol(f32 v) {
        if (v != vol) {
            dirty = 1;
            vol = v;
        }
    }
    void Play(void) {
        playing = 1;
        loop = 0;
        stop = 0;
    }
};

extern "C" RSound* fn_802076A0(const char* name, int a, int b, int c);
extern "C" RSound* fn_80207848(const char* name, int a);

static inline void Play(RSound* s) {
    if (s) {
        s->Play();
    }
}

static inline void PlayIdle(RSound* s) {
    if (s && !s->busy) {
        s->Play();
    }
}

static inline void StrCopyN(char* dst, const char* src, int n) {
    while ((*dst++ = *src++) != '\0') {
        if (--n <= 0) {
            dst[-1] = '\0';
            break;
        }
    }
}

#define CLAMP01(x) ((x) = MIN(1.0f, (x)), (x) = MAX((x), 0.0f))

static inline f32 Unlerp(f32 a, f32 b, f32 x) {
    f32 t = (x - a) / (b - a);
    CLAMP01(t);
    return t;
}

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

class LocString {
public:
    LocString(const char* key);
    void Refresh(void);  // fn_80131318
    const char* Get(void) {
        if (lang != lbl_8055E044) {
            Refresh();
        }
        return text;
    }
    const char* key;   // offset 0x0
    const char* text;  // offset 0x4
    s32 lang;          // offset 0x8
};

extern "C" void fn_80131318(LocString* s);

inline void LocString::Refresh(void) { fn_80131318(this); }

template <class T>
struct RArray {
    RArray(int n, const char* tag);
    ~RArray(void) {
        delete[] buf;
        data = 0;
        count = 0;
        cap = 0;
    }
    T* Add(void);
    void Push(const T& v);
    int Count(void);
    T* At(int i);
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
    T* buf;     // offset 0xC
};

template <class T>
struct IArray {
    IArray(int n, const char* tag) {
        data = buf = new (tag) T[n];
        cap = n;
        count = 0;
    }
    ~IArray(void) {
        delete[] buf;
        data = 0;
        count = 0;
        cap = 0;
    }
    T* Add(void) { return &data[count++]; }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
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
                fn_802C6CF8(model);
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

struct ObjTeleport {
    ObjTeleport(void) {
        b30 = 0;
        valid = 0;
        radius = 80.0f;
    }
    ~ObjTeleport(void) {}
    ValMatrix mtx;  // offset 0x0
    u8 b30;         // offset 0x30
    u8 valid;       // offset 0x31
    u8 pad32[2];
    f32 radius;     // offset 0x34
};

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
    f32 t;      // offset 0x0
    f32 limit;  // offset 0x4
};

struct TableRef {
    ~TableRef(void) { fn_80131818(t); }
    ValueTable* t;
};

class ArenaBase {
public:
    ArenaBase(void) { fn_802C2914(this); }
    ~ArenaBase(void) { fn_802C2930(this, 0); }
    s32 f0;  // offset 0x0
};

class Arena : public ArenaBase {
public:
    Arena(void);
    virtual int CountDynos(void);  // 0x8
    virtual void Spawn(void);      // 0xC
    virtual ~Arena(void);          // 0x10
    void Init(void);
    int CountFiles(ValueNode* cfg);
    int CountRingSets(void);
    void Reset(void);
    int PointValue(void);
    void SetName(char* buf);
    void ShowSetGems(void);
    void ActivateRingSet(void);
    void ActivateObjSet(void);
    void ClearGems(void);
    void Deactivate(int i);
    ArenaTimer timer;              // offset 0x8
    s32 f10;                       // offset 0x10
    TableRef dynos;                // offset 0x14
    RArray<ObjSet>* sets;          // offset 0x18
    RArray<SetStat>* stats;        // offset 0x1C
    void* art;                     // offset 0x20
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
    RSound* sRingHit;              // offset 0x4C
    RSound* sCheerHit;             // offset 0x50
    RSound* sLongCheer;            // offset 0x54
    RSound* sTally;                // offset 0x58
    RSound* sMissFlag;             // offset 0x5C
    RSound* sLevelUp;              // offset 0x60
    RSound* sFail;                 // offset 0x64
};

struct PlayerData {
    PlayerData(int i);
    ~PlayerData(void) { IdentGetCurrentSlot(); }
    void Reset(void) {
        score = 0;
        b8 = 0;
        fC = 0.0f;
    }
    s32 score;  // offset 0x0
    s32 index;  // offset 0x4
    u8 b8;      // offset 0x8
    u8 pad9[3];
    f32 fC;     // offset 0xC
};

struct PlayerList {
    PlayerList(void);
    ~PlayerList(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    void Clear(void);
    void Push(PlayerData* const& p);
    PlayerData** data;        // offset 0x0
    s32 count;                // offset 0x4
    s32 cap;                  // offset 0x8
    PlayerData* storage[8];   // offset 0xC
};

class ResultsOverlay {
public:
    ResultsOverlay(void);
    ~ResultsOverlay(void) {
        if (layout) {
            fn_802ADE60(layout, 1);
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
    s32 state;         // offset 0x0
    f32 start;         // offset 0x4
    u8 b8;             // offset 0x8
    u8 b9;             // offset 0x9
    u8 padA[2];
    void* layout;      // offset 0xC
    u8 noBonus;        // offset 0x10
    u8 pad11[3];
    RSound* sBonus;    // offset 0x14
    RSound* sNoBonus;  // offset 0x18
    RSound* s1C;       // offset 0x1C
    u8 b20;            // offset 0x20
    u8 pad21[3];
    RSound* s24;       // offset 0x24
    RSound* s28;       // offset 0x28
    RSound* s2C;       // offset 0x2C
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

struct ZoneResults {
    struct Entry {
        s32 car;    // offset 0x0
        s32 place;  // offset 0x4
        s32 score;  // offset 0x8
        u8 padC[0x10 - 0xC];
        u8 b10;     // offset 0x10
        u8 pad11[3];
    };
    s32 nCars;    // offset 0x0
    s32 nHumans;  // offset 0x4
    u8 pad8[0x4];
    u8 bC;    // offset 0xC
    u8 done;  // offset 0xD
    u8 bE;    // offset 0xE
    u8 padF;
    Entry entries[8];  // offset 0x10
};

struct ArtAnimManager {
    ArtAnimManager(int a);
    u8 pad0[8];
};

class GameMode {
public:
    GameMode(void) { fn_80144504(this); }
    virtual ~GameMode(void) {}
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual int vf1C(int car);
    virtual int vf20(int car) = 0;
    virtual int vf24(void);
    virtual int vf28(void);
    virtual void vf2C(void);
    virtual int vf30(void);
    virtual int vf34(void);
    virtual f32 vf38(void);
    virtual int vf3C(int p);
    virtual void vf40(void);
    virtual void vf44(void);
    virtual void vf48(void);
    virtual void vf4C(int p);
    virtual void vf50(int p);
    virtual int vf54(void);
    virtual void vf58(void);
    virtual int vf5C(int p);
    virtual void vf60(void);
    virtual int vf64(void);
    virtual int vf68(void);
    virtual void vf6C(void);
    virtual void vf70(void);
    virtual void vf74(void);
    virtual void vf78(void);
    virtual int vf7C(void);
    virtual int vf80(void);
    virtual int vf84(void);
    virtual int vf88(void);
    virtual int vf8C(int p, int key);
    virtual void vf90(void);
    virtual void vf94(void);
    virtual void vf98(void);
    virtual void vf9C(void);
    virtual int vfA0(void* cam);
    virtual void vfA4(void);
    virtual void vfA8(int car);
    virtual void vfAC(int car);
    virtual int vfB0(int car, struct Respawn* r);
    virtual void vfB4(void);
    virtual void vfB8(ZoneResults* r);
    virtual int vfBC(ZoneResults* r);
    virtual void vfC0(void);
    virtual void vfC4(void);
    virtual void vfC8(void);
    virtual int vfCC(void);
    virtual int vfD0(void);
    virtual int vfD4(int car);
    virtual int vfD8(void);
    virtual int vfDC(void);
    virtual int vfE0(void);
    virtual int vfE4(void);
    virtual void* vfE8(void);
    virtual int vfEC(void);
    virtual int vfF0(void);
    virtual void vfF4(void);
    virtual void vfF8(void);
    virtual void vfFC(void);
};

class RingChalMaster : public GameMode {
public:
    RingChalMaster(struct Race* race);
    virtual ~RingChalMaster(void);
    virtual void vf10(void);
    virtual int vf20(int car);
    virtual int vf24(void);
    virtual int vf28(void);
    virtual f32 vf38(void);
    virtual int vf3C(int view);
    virtual void vf40(void);
    virtual void vf44(void);
    virtual void vf4C(int p);
    virtual void vf50(int p);
    virtual int vf54(void);
    virtual int vf5C(int p);
    virtual int vf64(void);
    virtual int vf68(void);
    virtual int vf8C(int p, int key);
    virtual void vfA8(int car);
    virtual int vfB0(int car, struct Respawn* r);
    virtual void vfB8(ZoneResults* r);
    virtual void vfF8(void);
    PlayerList players;       // offset 0x4
    Arena arena;              // offset 0x30
    ILayout* layout;          // offset 0x98
    u8 b9C;                   // offset 0x9C
    u8 pad9D[3];
    f32 fA0;                  // offset 0xA0
    RSound* sPreRace;         // offset 0xA4
    u8 bA8;                   // offset 0xA8
    u8 padA9[3];
    ResultsOverlay results;   // offset 0xAC
};

struct Race {
    u8 pad0[0x3E0];
    s32 nCars;  // offset 0x3E0
};

ModuleTag lbl_8055E368;
UnkStaticFFFF lbl_8055E370;
static UnkStatic123 lbl_80430358;
RingChalMaster* lbl_8055E378;

void ResultsOverlay::Update(int score, int rings, int bonus) {
    if (layout) {
        int shown;
        int n;
        fn_801308F4(ringScore, 0x14, rings);
        shown = bonus;
        n = 0;
        if (bonus && start > 0.0f) {
            f32 t = Unlerp(0.0f, 1.0f, lbl_8055E3C8 - start);
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
            fn_802ADFF4(layout, fF4);
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
                fn_802AE02C(layout, fF4);
                state = 2;
            }
            break;
        case 2:
            if (b9) {
                fn_802AE0B8(layout, fF4);
                Play(s24);
                state = 3;
            }
            break;
        case 3:
            if (f104) {
                fn_802AE02C(layout, fF4);
                state = 4;
            }
            break;
        case 4:
            break;
        }
        fn_802AE168(layout);
    }
}

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
    if (sets) {
        delete sets;
    }
    if (stats) {
        delete stats;
    }
}

template <class T>
RArray<T>::RArray(int n, const char* tag) {
    data = buf = new (tag) T[n];
    cap = n;
    count = 0;
}

template <class T>
void RArray<T>::Push(const T& v) {
    data[count++] = v;
}

template <class T>
int RArray<T>::Count(void) {
    return count;
}

template <class T>
T* RArray<T>::Add(void) {
    return &data[count++];
}

void Arena::Init(void) {
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
        if (!t) {
            fn_802E30C8("FreestyleArenaMaster: can't load object file '%s'. Object load ceased.", name);
            return;
        }
        {
            int i;
            ValueNode* n;
            for (i = 0; (n = t->GetChild(i)) != 0; i++) {
                sets->Add()->Load(n, &dynos.t);
            }
        }
        fn_80131818(t);
        fn_801345D8(name, 0x1E, "object_file_%d", ++nFiles);
    }
    nFiles = sets->Count();
    nSets = CountRingSets();
    stats = new ("rc_ringsetstats") RArray<SetStat>(nSets, "rc_ringsetstats");
    {
        int i;
        for (i = 0; i < nSets; i++) {
            SetStat st;
            st.done = 0;
            st.best = -1;
            st.time = -100.0f;
            st.score = -100.0f;
            stats->Push(st);
        }
    }
}

int Arena::CountRingSets(void) {
    ValueNode* cfg = fn_8000AC38()->vals;
    const char* key = (const char*)fn_80204EA0("ring_file_key");
    int n = 0;
    if (cfg->Has(key)) {
        ValueTable* t = fn_80131810(cfg->GetValue(key));
        if (t) {
            int found;
            do {
                char prefix[0x1E];
                int i;
                fn_801345D8(prefix, 0x1E, "set_%i_", n);
                found = 0;
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
    int n = 0;
    int i;
    ValueNode* c;
    for (i = 0; (c = set->GetChildAt(i)) != 0; i++) {
        if (!strcasecmp(c->GetName(), "object") && strcasecmp(c->GetValue("dyno"), "teleport")) {
            n++;
        }
    }
    return n;
}

static inline int CountTeles(ValueNode* set) {
    int n = 0;
    int i;
    ValueNode* c;
    for (i = 0; (c = set->GetChildAt(i)) != 0; i++) {
        if (!strcasecmp(c->GetName(), "object") && !strcasecmp(c->GetValue("dyno"), "teleport")) {
            n++;
        }
    }
    return n;
}

u8 ObjSet::Load(ValueNode* set, ValueTable** dynos) {
    u8 ok;
    infos = new ("ObjectInfo") IArray<ObjInfo>(CountInfos(set), "rc_object_info");
    teles = new ("ObjectTeleport") IArray<ObjTeleport>(CountTeles(set), "rc_object_teleport");
    ok = 1;
    {
        int i;
        ValueNode* c;
        for (i = 0; (c = set->GetChildAt(i)) != 0; i++) {
            if (strcmp(c->GetName(), "object")) {
                continue;
            }
            if (strcmp(c->GetValue("dyno"), "teleport")) {
                ObjInfo* o = infos->Add();
                const char* dyno = c->GetValue("dyno");
                ValueNode* d;
                int j;
                for (j = 0; (d = (*dynos)->GetChild(j)) != 0; j++) {
                    if (!strcmp(d->GetName(), "dyno") && !strcmp(d->GetValue("name"), dyno)) {
                        break;
                    }
                }
                if (!d) {
                    fn_802E317C("freestyle: unknown dyno type %s", dyno);
                }
                if (d->FindChild("xlo_info")) {
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

void PlayerList::Push(PlayerData* const& p) { data[count++] = p; }

PlayerData::PlayerData(int i) {
    index = i;
    IdentGetCurrentSlot();
    fC = 0.0f;
}

void PlayerList::Clear(void) { count = 0; }

ResultsOverlay::ResultsOverlay(void) {
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
        sRingHit->SetVol(1.0f);
    }
    if (sCheerHit) {
        sCheerHit->SetVol(1.0f);
    }
    if (sTally) {
        sTally->SetVol(1.0f);
    }
    if (sLongCheer) {
        sLongCheer->SetVol(1.0f);
    }
    if (sMissFlag) {
        sMissFlag->SetVol(1.0f);
    }
    if (sLevelUp) {
        sLevelUp->SetVol(1.0f);
    }
    if (sFail) {
        sFail->SetVol(1.0f);
    }
    f40 = fn_80204CF0();
}

PlayerList::PlayerList(void) {
    data = storage;
    cap = 8;
    count = 0;
}

int Arena::CountDynos(void) {
    int n = 0;
    int i;
    for (i = 0; i < sets->count; i++) {
        int c = 0;
        int j;
        for (j = 0; j < sets->data[i].infos->count; j++) {
            if (sets->data[i].infos->data[j].type == 0) {
                c++;
            }
        }
        n += c;
    }
    return n;
}

void Arena::Spawn(void) {
    int i;
    for (i = 0; i < sets->count; i++) {
        int j;
        for (j = 0; j < sets->data[i].infos->count; j++) {
            ObjInfo* o = &sets->data[i].infos->data[j];
            if (o->type == 0) {
                o->obj = fn_802BAA68(o->xlo, &o->mtx);
            }
        }
    }
}

#define STOP(s)          \
    if (s) {             \
        (s)->stop = 1;   \
        (s)->f4C = 0;    \
    }

inline void Arena::SetName(char* buf) {
    switch (difficulty) {
    case 0:
        fn_801345D8(buf, 0x1E, "set_%i_5", curSet);
        break;
    case 1:
        fn_801345D8(buf, 0x1E, "set_%i_10", curSet);
        break;
    case 2:
        fn_801345D8(buf, 0x1E, "set_%i_15", curSet);
        break;
    default:
        fn_802E317C("Unknown difficulty specification in Ring Challenge mode");
        break;
    }
}





extern "C" void fn_8016D294(int a, int b, int set) {
    RingChalMaster* m = lbl_8055E378;
    if (m) {
        if (b == 0) {
            Arena* a = &m->arena;
            a->curSet = set;
            if (a->curSet < a->nFiles) {
                ObjSet* os = &a->sets->data[a->curSet];
                int i;
                os->active = 1;
                os->time = lbl_8055E3C8;
                os->f10 = 0;
                for (i = 0; i < os->infos->count; i++) {
                    if (os->infos->data[i].obj) {
                        fn_802BAF98(os->infos->data[i].obj, 1);
                    }
                }
            }
            {
                char name[0x1E];
                a->SetName(name);
                fn_8016A15C(name);
            }
            {
                char name[0x1E];
                if (a->f24 != -1) {
                    if (a->f24 != -1) {
                        fn_8001B450(a->f24);
                        a->f24 = -1;
                    }
                }
                a->SetName(name);
                switch (a->difficulty) {
                case 0: {
                    GemRef g = fn_8016A6B0(name, 12);
                    a->f24 = fn_8001B308(&g, a->art, 2);
                    break;
                }
                case 1: {
                    GemRef g = fn_8016A6B0(name, 9);
                    a->f24 = fn_8001B308(&g, a->art, 2);
                    break;
                }
                case 2: {
                    GemRef g = fn_8016A6B0(name, 6);
                    a->f24 = fn_8001B308(&g, a->art, 2);
                    break;
                }
                default:
                    fn_802E317C("Unknown difficulty specification in Ring Challenge mode");
                    break;
                }
            }
        }
    }
}

RingChalMaster::~RingChalMaster(void) {
    int i;
    if (sPreRace) {
        delete sPreRace;
    }
    for (i = 0; i < players.count; i++) {
        PlayerData* p = players.data[i];
        if (p) {
            delete p;
        }
    }
    lbl_8055E378 = 0;
    fn_8016A68C();
}



inline void Arena::Reset(void) {
    int i;
    if (f40 == 0) {
        difficulty = 0;
    } else {
        difficulty = 2;
    }
    curSet = 0;
    f34 = 0;
    f30 = 0;
    b48 = 0;
    b49 = 0;
    b4A = 1;
    f28 = 0.0f;
    if (nFiles < nSets) {
        fn_802E30C8("WARNING: There are less Freestyle object sets than Freestyle ring sets");
    }
    timer.Reset();
    for (i = 0; i < nSets; i++) {
        Deactivate(i);
    }
    for (i = 0; i < nFiles; i++) {
        sets->At(i)->Reset();
    }
    ClearGems();
    if (curSet >= 0 && curSet < nSets) {
        ActivateObjSet();
        ActivateRingSet();
        ShowSetGems();
    }
    for (i = 0; i < stats->Count(); i++) {
        stats->At(i)->done = 0;
        stats->At(i)->best = -1;
        stats->At(i)->time = -100.0f;
        stats->At(i)->score = -100.0f;
    }
    stats->At(curSet)->best = PointValue();
    if (sRingHit) {
        fn_80144C54(sRingHit);
    }
    if (sLongCheer) {
        fn_80144C54(sLongCheer);
    }
    if (sTally) {
        fn_80144C54(sTally);
    }
    if (sCheerHit) {
        fn_80144C54(sCheerHit);
    }
    if (sMissFlag) {
        fn_80144C54(sMissFlag);
    }
    if (sLevelUp) {
        fn_80144C54(sLevelUp);
    }
    if (sFail) {
        fn_80144C54(sFail);
    }
}


void RingChalMaster::vf10(void) {
    int i;
    for (i = 0; i < players.count; i++) {
        players.data[i]->Reset();
    }
    fn_8016A0D4();
    arena.Reset();
    b9C = 1;
    fA0 = -100.0f;
    results.state = 0;
    results.start = -1.0f;
    results.b8 = 1;
    results.b9 = 0;
    results.noBonus = 0;
    results.fF8 = 0;
    results.fFC = 0;
    results.f100 = 0;
    results.f104 = 0;
    if (results.layout) {
        fn_802AE0E0(results.layout);
    }
    STOP(results.sBonus);
    STOP(results.sNoBonus);
    STOP(results.s1C);
    results.b20 = 0;
    STOP(results.s24);
    STOP(results.s28);
    STOP(results.s2C);
    STOP(sPreRace);
    bA8 = 0;
}


int Arena::PointValue(void) {
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
T* RArray<T>::At(int i) {
    return &data[i];
}

void Arena::ShowSetGems(void) {
    char name[0x1E];
    if (f24 != -1) {
        ClearGems();
    }
    SetName(name);
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

void Arena::ActivateRingSet(void) {
    char name[0x1E];
    SetName(name);
    fn_8016A15C(name);
}

void Arena::ActivateObjSet(void) {
    if (curSet < nFiles) {
        ObjSet* s = &sets->data[curSet];
        int i;
        s->active = 1;
        s->time = lbl_8055E3C8;
        s->f10 = 0;
        for (i = 0; i < s->infos->count; i++) {
            if (s->infos->data[i].obj) {
                fn_802BAF98(s->infos->data[i].obj, 1);
            }
        }
    }
}

void Arena::ClearGems(void) {
    if (f24 != -1) {
        fn_8001B450(f24);
        f24 = -1;
    }
}

void ObjSet::Reset(void) {
    int i;
    active = 0;
    time = -100.0f;
    f10 = 0;
    for (i = 0; i < teles->count; i++) {
        teles->data[i].b30 = 0;
    }
}

void Arena::Deactivate(int i) {
    char name[0x1E];
    SetName(name);
    fn_8016A240(name);
    if (i < nFiles) {
        ObjSet* s = &sets->data[i];
        int j;
        s->active = 0;
        s->time = lbl_8055E3C8;
        for (j = 0; j < s->infos->count; j++) {
            if (s->infos->data[j].obj) {
                fn_802BAF98(s->infos->data[j].obj, 0);
            }
        }
    }
    if (f24 != -1) {
        fn_8001B450(f24);
        f24 = -1;
    }
}

void ArenaTimer::Reset(void) { t = limit; }

int RingChalMaster::vf20(int car) { return b9C; }

void RingChalMaster::vfB8(ZoneResults* r) {
    int i;
    ZoneResults::Entry* e;
    r->nCars = players.count;
    r->nHumans = fn_80179650();
    r->done = 1;
    r->bE = 0;
    for (i = 0, e = r->entries; i < players.count; i++, e++) {
        e->car = i;
        e->b10 = 0;
        e->score = players.data[i]->score;
        e->place = 1;
        r->bC = 1;
    }
}

void RingChalMaster::vfF8(void) {
    layout = (ILayout*)fn_80145E70("freestyle.val");
    results.layout = new ("ArtAnimManager") ArtAnimManager(1);
    results.Load();
    results.Reset();
}

void ResultsOverlay::Reset(void) {
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
        fn_802AE0E0(layout);
    }
    STOP(sBonus);
    STOP(sNoBonus);
    STOP(s1C);
    b20 = 0;
    STOP(s24);
    STOP(s28);
    STOP(s2C);
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

int RingChalMaster::vf3C(int view) {
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
            fn_8001DF08(fn_801E866C(view), 0);
            if (h0 && h1 && body && denom) {
                char buf[0x14];
                Blend(h0, h1, fn_80131234("PreRaceOverlay:STARS_NEEDED"), Ease(k));
                fn_801308F4(buf, sizeof(buf), fn_80160998(fn_80160A74()));
                {
                    TextElem e = *body;
                    e.pos.x -= 0.5f * Width(&e, buf);
                    Blend(&e, denom, buf, Ease(k));
                }
            }
        }
        if (t < end1) {
            f32 k;
            TextElem* body;
            TextElem* timer;
            fn_8014784C(layout, "stop_watch_art", "stop_watch_artset", 0);
            k = Lerp(0.0f, 1.0f, Clamp01((t - in1) / (full1 - in1)));
            body = layout->Find("time_body_0");
            timer = layout->Find("timer");
            if (h0 && h1 && body && timer) {
                char buf[0x14];
                Blend(h0, h1, fn_80131234("PreRaceOverlay:TIME_LIMIT"), Ease(k));
                fn_80130978(buf, sizeof(buf), arena.timer.t);
                {
                    TextElem e = *body;
                    e.pos.x -= 0.5f * Width(&e, buf);
                    Blend(&e, timer, buf, Ease(k));
                }
            }
        }
    }
    return 1;
}

f32 RingChalMaster::vf38(void) { return 7.75f; }

void ResultsOverlay::Load(void) {
    Binding b[16] = {
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
    fF4 = fn_802ADF34(layout, "a_ring_finish.val", b);
    sBonus = fn_80207848("pr_ringchal1.sfx", 1);
    sNoBonus = fn_80207848("pr_ringchal1_nobonus.sfx", 1);
    s1C = fn_80207848("pr_ringchal2.sfx", 1);
    s24 = fn_80207848("pr_transition.sfx", 1);
    s28 = fn_80207848("levelup.sfx", 1);
    s2C = fn_80207848("timeout.sfx", 1);
}

void RingChalMaster::vf40(void) {
    int i;
    for (i = 0; i < arena.nFiles; i++) {
        ObjSet* s = &arena.sets->data[i];
        if (s->active) {
            int j;
            for (j = 0; j < s->infos->count; j++) {
                ObjInfo* o = &s->infos->data[j];
                if (o->type == 1) {
                    fn_802C744C(o->model, &o->mtx);
                }
            }
        } else if (!s->active) {
            f32 t = lbl_8055E3C8 - s->time;
            if (t < 0.25f) {
                f32 a = Lerp(1.0f, 0.0f, Unlerp(0.0f, 0.25f, t));
                if (a > 0.01f) {
                    int j;
                    fn_802C9900(a);
                    for (j = 0; j < s->infos->count; j++) {
                        ObjInfo* o = &s->infos->data[j];
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

void RingChalMaster::vf44(void) {
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

void RingChalMaster::vf50(int p) { vf4C(p); }

int RingChalMaster::vf54(void) { return 0x1408; }

inline void ObjSet::CheckTeleport(Phob* car) {
    int next = f10 + 1;
    if (next < teles->count) {
        ObjTeleport* t = &teles->data[next];
        V2 a;
        V2 b;
        V2 d;
        a.x = t->mtx.origin.x;
        a.y = t->mtx.origin.z;
        b.x = car->pos.x;
        b.y = car->pos.z;
        d.x = a.x - b.x;
        d.y = a.y - b.y;
        if ((f32)sqrt(d.x * d.x + d.y * d.y) < t->radius) {
            teles->data[f10].b30 = 0;
            f10 = next;
            teles->data[next].b30 = 1;
        }
    }
}

int RingChalMaster::vf24(void) {
    int r = 0;
    if (fn_801B8514() && arena.b49 && !b9C) {
        r = 1;
    }
    return r;
}

int RingChalMaster::vf68(void) {
    if (vf24()) {
        return results.state == 2;
    }
    return 0;
}

int RingChalMaster::vf64(void) { return 1; }

int RingChalMaster::vf5C(int p) {
    if (lbl_8055E3C8 - fA0 < 2.25f) {
        vf4C(p);
    } else {
        if (results.b8) {
            vf4C(p);
        }
        if (results.layout) {
            fn_802AE220(results.layout, 0);
        }
    }
    if (vf68()) {
        fn_8001DFCC();
    }
    return results.state != 4;
}

int RingChalMaster::vf8C(int p, int key) {
    if (vf68() && key == 'X') {
        if (results.state == 2) {
            results.b9 = 1;
        }
        return 1;
    }
    if (results.state != 0) {
        f32 now = lbl_8055E3C8;
        if (results.state == 1 && !results.noBonus && results.start > 0.0f && now - results.start < 1.0f) {
            results.noBonus = 1;
            if (results.layout) {
                fn_802AE118(results.layout);
            }
            STOP(results.sBonus);
            STOP(results.sNoBonus);
            STOP(results.s28);
            STOP(results.s2C);
            results.b8 = 0;
            results.start = lbl_8055E3C8 - 1.0f;
        }
        return 1;
    }
    return 0;
}

int RingChalMaster::vf28(void) { return !arena.b49; }

void RingChalMaster::vfA8(int car) { fn_801B0C78(car); }

int RingChalMaster::vfB0(int car, Respawn* r) {
    PlayerData* p = players.data[car];
    p->b8 = 0;
    p->fC = 0.0f;
    if (arena.curSet < arena.nFiles) {
        ObjSet* s = &arena.sets->data[arena.curSet];
        if (s->teles && s->teles->count > 0 && s->teles->data[s->f10].b30) {
            ObjTeleport* t;
            if (s->f10 + 1 < s->teles->count) {
                t = &s->teles->data[s->f10 + 1];
            } else {
                t = &s->teles->data[s->f10];
            }
            fn_801B8060(r, t);
            r->vel.x = t->mtx.basis[2].x * 26.666666f;
            r->pos.y += 7.625f;
            r->vel.y = t->mtx.basis[2].y * 26.666666f;
            r->vel.z = t->mtx.basis[2].z * 26.666666f;
            return 1;
        }
        return 0;
    }
    return 0;
}
