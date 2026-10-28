// Ring challenge master (0x8016B964-0x80171698): the "ringchalmaster" game mode. It loads the Freestyle
// arena's object sets ("object_file_%d", dynos and teleports), counts the ring sets in the ring file, runs the
// timed ring challenge with its pre-race and results overlays ("ResultsOverlay:*", "PreRaceOverlay:*") and
// scores rings by size and difficulty. Names are guesses apart from the strings.
#include "game/common.h"

// Art animation player (artanim.cpp).
class ArtPlayer {
public:
    ArtPlayer(int n);
    ~ArtPlayer(void);
    int Add(const char* name, int bindings);
    void Start(int i);
    void Pause(int i);
    void Resume(int i);
    void StopAll(void);
    void Update(void);
    void Draw(int flags);

    void* list;  // offset 0x0
    f32 t;       // offset 0x4
};

class RSound;
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
void* fn_8023512C(const char* name);
void fn_80235380(void* art);
s32 fn_80204CF0(void);
s32 fn_80204EA0(const char* key);
void fn_802C6CF8(s32 model);
s32 fn_802C6FC0(const char* name, int a, int b);
void* fn_802BAA68(void* xlo, const ValMatrix* m);
void fn_8016A0B0(void);
void fn_802ADE60(void* layout, int flag);
void fn_8016A15C(const char* name);  // activate ring set
void fn_8016A240(const char* name);  // deactivate ring set
void fn_8016A0D4(void);
void fn_8016A68C(void);
void fn_8001B450(s32 h);
s32 fn_802BAF98(void* obj, int on);
void fn_80144C54(RSound* s);
s32 fn_80179650(void);
s32 fn_80145E70(const char* name);
void fn_802ADDD8(void* p, int a);
}

struct V2 {
    f32 x;
    f32 y;
};

union ElemColor {
    u32 all;
    struct {
        u32 a : 8;
        u32 rgb : 24;
    } ch;
};

struct TextElem {
    TextElem(void) {}
    TextElem(const TextElem& o);  // fn_8014C920
    const char* name;     // offset 0x0
    V2 pos;               // offset 0x4
    s32 elemJust;         // offset 0xC
    f32 scale;            // offset 0x10
    u32 just;             // offset 0x14
    ElemColor color;      // offset 0x18
    const char* defText;  // offset 0x1C
};

struct Box {
    Box(void) {}
    Box& operator=(const Box& o);
    const char* name;  // offset 0x0
    V2 pos;            // offset 0x4
    s32 c;             // offset 0xC
    f32 w;             // offset 0x10
    f32 h;             // offset 0x14
    ElemColor color;   // offset 0x18
};

class ILayout {
public:
    virtual ~ILayout(void) {}
    virtual TextElem* Find(const char* name) = 0;
    virtual void* FindArt(const char* name) = 0;
    virtual Box* FindBox(const char* name) = 0;
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
}  // extern "C"
void UpdateContinueOverlay(void);
extern "C" {
void fn_802AE118(void* layout);
void fn_801B0C78(int car);
}

class CarCtl {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual u32 vf14(void);
};

struct RaceCarRec {
    u8 pad0[0x10];
    u32 model;        // offset 0x10
    u8 pad14[0x54 - 0x14];
    ValVec3 fwd;      // offset 0x54
    ValVec3 pos;      // offset 0x60
    u8 pad6C[0x1EC - 0x6C];
    void* rings;      // offset 0x1EC
    u8 pad1F0[0x2690 - 0x1F0];
    f32 speed;        // offset 0x2690
    u8 pad2694[0x270C - 0x2694];
    CarCtl* ctl;      // offset 0x270C
    void* Rings(void);
};

extern "C" RaceCarRec* fn_8017960C(int id);
extern "C" f32 PSVECDotProduct(const void* a, const void* b);
extern f32 lbl_80558DC4[];  // FLT_EPSILON

#define NearZeroF(x) ((f32)__fabs(x) <= lbl_80558DC4[0])

struct PathPt {
    f32 x;    // offset 0x0
    f32 z;    // offset 0x4
    f32 dx;   // offset 0x8
    f32 dz;   // offset 0xC
    u8 pad10[0x4];
    PathPt* next;  // offset 0x14
};

// A position on a centre line (idealline): segment start and fraction.
struct ILPos {
    PathPt* pt;  // offset 0x0
    f32 t;       // offset 0x4
    s32 f8;      // offset 0x8
};

struct CenterLine;

// Follows a centre line (idealline.cpp).
class LineFollower {
public:
    LineFollower(void);
    ~LineFollower(void);
    void SetLine(CenterLine* line);
    int Locate(V2* p, V2* dir, ILPos* out, int n);
    u8 pad0[0x38];
};

class PathSet {
public:
    virtual void vf08(void);
    virtual CenterLine* Get(int index);  // 0xC
};

extern "C" {
PathSet* fn_80156A40(void);
void fn_801B7C8C(struct Respawn* r, V2* pos, V2* dir);
}

extern "C" {
f32 fn_801B306C(void);
}  // extern "C"
int fn_80009C84(void);
extern "C" {
s32 fn_8016A30C(void* rings);
s32 fn_8016A780(const char* name, void* rings);
s32 fn_8016A878(const char* name);
s32 fn_8015A404(RaceCarRec* car);
int fn_80141FA0(RSound* s);
void fn_80142B60(RSound* s);
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
extern "C" void fn_801B8060(struct Respawn* r, struct ObjTeleport* t);

const char* GetGradeDisplayName(int grade);
int IdentGetCurrentSlot(void);
void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

struct ConfigRoot {
    u8 pad0[0x20];
    ValueNode* vals;  // offset 0x20
};

ConfigRoot* fn_8000AC38(void);

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
    void Stop(void) {
        stop = 1;
        f4C = 0;
    }
};

extern "C" RSound* fn_802076A0(const char* name, int a, int b, int c);
extern "C" RSound* fn_80207848(const char* name, int a);

static inline void Play(RSound* s) {
    if (s) {
        s->Play();
    }
}

static inline void Stop(RSound* s) {
    if (s) {
        s->Stop();
    }
}

static inline void PlayIdle(RSound* s) {
    if (s && !s->busy) {
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
    u32 lang;          // offset 0x8
};

extern "C" void fn_80131318(LocString* s);

inline void LocString::Refresh(void) { fn_80131318(this); }

template <class T>
struct ArrayRef {
    ArrayRef(T* d, int n) {
        data = d;
        cap = n;
        count = 0;
    }
    ~ArrayRef(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

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
    IArray(int n, const char* tag) : ArrayRef<T>(buf = new (tag) T[n], n) {}
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
    f32 Get(void);
    void Draw(class ILayout* l);
    int Expired(void) { return t <= 0.0f; }
    f32 t;      // offset 0x0
    f32 limit;  // offset 0x4
};

struct TableRef {
    ~TableRef(void) { fn_80131818(t); }
    ValueTable* t;
};

// The preload list node every arena is (preload.cpp); only its first word is used here.
class Preloader {
public:
    Preloader(void);
    ~Preloader(void);
    s32 f0;  // offset 0x0
};

class Arena : public Preloader {
public:
    Arena(void);
    virtual int CountDynos(void);  // 0x8
    virtual void Spawn(void);      // 0xC
    virtual ~Arena(void);          // 0x10
    void Init(void);
    int CountFiles(ValueNode* cfg);
    int CountRingSets(void);
    int PointValue(void);
    void SetName(char* buf, int n);
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
    u8 Stuck(void);
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
    RingResults(void);
    ~RingResults(void) { Unload(); }
    void Unload(void) {
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
    volatile s32 state;  // offset 0x0
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
    GameMode(void);
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
    virtual int vfA0(struct Respawn* out, int idx);
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
    virtual int vfE0(int car, u8* out);
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
    virtual ~RingChalMaster(void);
    virtual void vf0C(void);
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
    virtual int vfA0(struct Respawn* out, int idx);
    virtual void vfA8(int car);
    virtual int vfB0(int car, struct Respawn* r);
    virtual int vfE0(int car, u8* out);
    virtual void vfB8(ZoneResults* r);
    virtual void vfF8(void);
    virtual void vfFC(void);
    PlayerList players;       // offset 0x4
    Arena arena;              // offset 0x30
    ILayout* layout;          // offset 0x98
    u8 b9C;                   // offset 0x9C
    u8 pad9D[3];
    f32 fA0;                  // offset 0xA0
    RSound* sPreRace;         // offset 0xA4
    u8 bA8;                   // offset 0xA8
    u8 padA9[3];
    RingResults results;   // offset 0xAC
};

struct Race {
    u8 pad0[0x3E0];
    s32 nCars;  // offset 0x3E0
};

ModuleTag lbl_8055E368;
UnkStaticFFFF lbl_8055E370;
static UnkStatic123s lbl_80430358;
RingChalMaster* lbl_8055E378;

void RingResults::Update(int score, int rings, int bonus) {
    if (layout) {
        int shown;
        int n;
        fn_801308F4(ringScore, 0x14, rings);
        shown = bonus;
        n = 0;
        if (bonus && start > 0.0f) {
            f32 now = lbl_8055E3C8;
            f32 t = Unlerp(0.0f, 1.0f, now - start);
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

extern "C" RingChalMaster* fn_8016BE60(Race* race) {
    RingChalMaster* m = new ("ringchalmaster") RingChalMaster;
    int i;
    lbl_8055E378 = m;
    m->players.Clear();
    for (i = 0; i < race->nCars; i++) {
        PlayerData* p = new ("playerdata") PlayerData(i);
        m->players.Push(p);
    }
    fn_8016A0B0();
    m->arena.Init();
    m->layout = 0;
    m->b9C = 1;
    m->fA0 = -100.0f;
    m->sPreRace = fn_80207848("chal_prerace.sfx", 1);
    m->bA8 = 0;
    return m;
}


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
int RArray<T>::Count(void) {
    return this->count;
}

template <class T>
T* RArray<T>::Add(void) {
    return &this->data[this->count++];
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
        ValueNode* c;
        int i;
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

PlayerList::PlayerList(void) : ArrayRef<PlayerData*>(storage, 8) {}

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






extern "C" void fn_8016D294(int a, int b, int set) {
    RingChalMaster* m = lbl_8055E378;
    if (!m || b != 0) {
        return;
    }
    Arena* ar = &m->arena;
    ar->curSet = set;
    if (ar->curSet < ar->nFiles) {
        ObjSet* os = &ar->sets->data[ar->curSet];
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
        SET_NAME(ar, name, 0x1E);
        fn_8016A15C(name);
    }
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

RingChalMaster::~RingChalMaster(void) {
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





void RingChalMaster::vf10(void) {
    int i;
    for (i = 0; i < players.count; i++) {
        players.data[i]->Reset();
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
        ((ArtPlayer*)(results.layout))->StopAll();
    }
    Stop(results.sBonus);
    Stop(results.sNoBonus);
    Stop(results.s1C);
    results.b20 = 0;
    Stop(results.s24);
    Stop(results.s28);
    Stop(results.s2C);
    Stop(sPreRace);
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
    return &this->data[i];
}

void Arena::ShowSetGems(void) {
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

void Arena::ActivateRingSet(void) {
    char name[0x1E];
    SET_NAME(this, name, 0x1E);
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
    SET_NAME(this, name, 0x1E);
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
    ZoneResults::Entry* e;
    int i;
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

void RingResults::Reset(void) {
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

int RingChalMaster::vf3C(int view) {
    f32 t = fn_801E7E3C()->time;
    f32 end0 = 6.75f;
    f32 in0 = end0 - 0.8f;
    f32 full0 = in0 - 0.3f;
    f32 end1 = full0 - 0.15f;
    f32 in1 = end1 - 0.8f;
    f32 full1 = in1 - 0.3f;
    TextElem* h1 = layout->Find("intro_head_1");
    TextElem* h0 = layout->Find("intro_head_0");
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

void RingResults::Load(void) {
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
    fF4 = ((ArtPlayer*)(layout))->Add("a_ring_finish.val", (int)(b));
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
            ((ArtPlayer*)(results.layout))->Draw(0);
        }
    }
    if (vf68()) {
        UpdateContinueOverlay();
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

void RingChalMaster::vfFC(void) {
    results.Unload();
    delete layout;
}

static inline int Stopped(f32 speed) { return NearZeroF(speed); }

int RingChalMaster::vfE0(int car, u8* out) {
    ObjSet* s;
    int r = 0;
    f32 now = lbl_8055E3C8;
    if (now > vf38() && !arena.b48) {
        if (Stopped(fn_8017960C(players.data[car]->index)->speed)) {
            PlayerData* p = players.data[car];
            if (arena.curSet < arena.nFiles) {
                s = &arena.sets->data[arena.curSet];
                if (s->teles && s->teles->count > 0 && s->teles->data[s->f10].b30) {
                    RaceCarRec* c = fn_8017960C(p->index);
                    ObjTeleport* t = &s->teles->data[s->f10];
                    V2 b;
                    V2 a;
                    V2 d;
                    int inside;
                    int behind;
                    a.x = t->mtx.origin.x;
                    a.y = t->mtx.origin.z;
                    b.x = c->pos.x;
                    d.x = a.x - b.x;
                    b.y = c->pos.z;
                    d.y = a.y - b.y;
                    inside = (f32)sqrt(d.x * d.x + d.y * d.y) < t->radius;
                    behind = PSVECDotProduct(&t->mtx.basis[2], &c->fwd) < 0.0f;
                    if (s->f10 == s->teles->count - 1 && inside) {
                        s->teles->data[s->f10].b30 = 0;
                        r = 0;
                    } else {
                        if (inside || behind) {
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
                        r = 1;
                    }
                } else {
                    r = 0;
                }
            } else {
                r = 0;
            }
            if (!r) {
                if (fn_8017960C(players.data[car]->index)->model == 'PCAR') {
                    RaceCarRec* c = fn_8017960C(players.data[car]->index);
                    PlayerData* q;
                    if ((c->ctl->vf14() & 0x10) && !(q = players.data[car])->b8) {
                        q->b8 = 1;
                        q->fC = 0.0f;
                    } else if (!(c->ctl->vf14() & 0x10) && (q = players.data[car])->b8) {
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
    m.y = a.y + 0.5f * (b.y - a.y);
    m.x = a.x + 0.5f * (b.x - a.x);
    return m;
}

static inline void Normalize(V2& v) {
    f32 inv = 1.0f / (f32)sqrt(LenSq(v));
    v.x *= inv;
    v.y *= inv;
}

static inline f32 PathLerp(f32 a, f32 b, f32 k) { return a + k * (b - a); }

int RingChalMaster::vfA0(Respawn* out, int idx) {
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
            int i;
            int k;
            f32 best;
            f32 l;
            mid.x = 0.0f;
            mid.y = 0.0f;
            dir.x = 0.0f;
            dir.y = 0.0f;
            for (i = 0; i < 3; i++) {
                char b[0x10];
                fn_801345D8(b, 0x10, "bound%d", i);
                ValPoint p = sp->GetPoint(b);
                pts[i].x = p.x;
                pts[i].y = p.y;
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
                lf.SetLine(fn_80156A40()->Get(0));
                ILPos pos;
                pos.pt = 0;
                pos.t = 0.0f;
                pos.f8 = 10000;
                lf.Locate(&mid, 0, &pos, 0);
                mid.x = PathLerp(pos.pt->x, pos.pt->next->x, pos.t);
                mid.y = PathLerp(pos.pt->z, pos.pt->next->z, pos.t);
                dir.x = PathLerp(pos.pt->dx, pos.pt->next->dx, pos.t);
                dir.y = PathLerp(pos.pt->dz, pos.pt->next->dz, pos.t);
                Normalize(dir);
            }
            fn_801B7C8C(out, &mid, &dir);
            {
                f32 s = 0.444444f * sp->GetFloat("speed", 0.0f);
                out->vel.x = out->basis[2].x * s;
                out->vel.y = out->basis[2].y * s;
                out->vel.z = out->basis[2].z * s;
            }
            return 1;
        }
    }
    return 0;
}

#define ALPHA(c, v) ((c).ch.a = (int)(255.0f * (v)))

void RingChalMaster::vf4C(int view) {
    ILayout* l;
    TextElem* setScore;
    Box* setIcon;
    Box* starScore;
    Box* arrowIcon;
    int i;
    {
        PlayerData* p = players.data[fn_801E866C(view)];
        if (p) {
            UpdateScoreOverlays(p->index, p->score);
        }
    }
    l = layout;
    arena.timer.Draw(l);
    setScore = l->Find("ring_set_score");
    setIcon = l->FindBox("ring_set_icon");
    arrowIcon = l->FindBox("arrow_icon");
    starScore = l->FindBox("star_score");
    for (i = 0; i < arena.nSets; i++) {
        Box icon;
        TextElem txt = *setScore;
        f32 dt;
        icon = *setIcon;
        dt = Time() - arena.stats->At(i)->time;
        icon.pos.y += i * 40;
        txt.pos.y += i * 40;
        if (i > arena.curSet) {
            icon.color.all = 0x66FFFFFF;
        }
        fn_80147940(l, &icon, "star_art", 0);
        if (i < arena.curSet || arena.b48) {
            int n = 0;
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
                    int j;
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
            hl.pos.x = fn_80144C80(icon.pos.x, icon.pos.x - (2.5f * icon.w - icon.w) * 0.5f, k);
            hl.pos.y = fn_80144C80(icon.pos.y, icon.pos.y - (2.5f * icon.h - icon.h) * 0.5f, k);
            ALPHA(hl.color, 1.0f - k);
            fn_80147940(l, &hl, "star_art", 0);
        }
        if (dt < 3.4f) {
            TextElem* nr = l->Find("next_ring");
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
                    if (!fn_80141FA0(arena.sMissFlag)) {
                        fn_80142B60(arena.sMissFlag);
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
                    if (!fn_80141FA0(arena.sMissFlag)) {
                        fn_80142B60(arena.sMissFlag);
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
        Box* target;
        Box tr;
        int s;
        Box* next;
        if (fn_802C9A58()) {
            target = l->FindBox("target_ring_ws");
        } else {
            target = l->FindBox("target_ring");
        }
        next = l->FindBox("next_ring");
        tr = *target;
        s = arena.curSet;
        if (s > 0 && !arena.b49) {
            f32 d = Time() - arena.stats->At(s - 1)->time - 1.9f;
            if (Time() - arena.stats->At(s - 1)->time > 0.0f && d <= 2.2f) {
                s = s - 1;
            }
            if (d >= 0.0f) {
                Box nx;
                nx = *next;
                if (d <= 0.15f) {
                    f32 k;
                    if (arena.stats->At(arena.curSet - 1)->done) {
                        if (!fn_80141FA0(arena.sLevelUp)) {
                            fn_80142B60(arena.sLevelUp);
                        }
                    } else {
                        if (!fn_80141FA0(arena.sFail)) {
                            fn_80142B60(arena.sFail);
                        }
                    }
                    k = Remap(0.0f, 0.15f, 0.0f, 1.0f, d);
                    ALPHA(nx.color, k);
                    nx.w = fn_80144C80(10.0f * next->w, next->w, k);
                    nx.h = fn_80144C80(10.0f * next->h, next->h, k);
                    nx.pos.x = fn_80144C80(next->pos.x - 10.0f * next->w / 2.0f, next->pos.x, k);
                    nx.pos.y = fn_80144C80(next->pos.y - 10.0f * next->h * 0.5f, next->pos.y, k);
                }
                if (d > 1.35f) {
                    f32 k = Remap(1.35f, 1.9f, 0.0f, 1.0f, d);
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

inline Box& Box::operator=(const Box& o) {
    name = o.name;
    pos = o.pos;
    c = o.c;
    w = o.w;
    h = o.h;
    color = o.color;
    return *this;
}

inline void ArenaTimer::Draw(ILayout* l) {
    char buf[0x20];
    fn_80130978(buf, 0x20, MAX(0.0f, t));
    fn_8014784C(l, "stop_watch_art", "stop_watch_artset", 0);
    fn_801475D0(l, "timer", "%s", buf);
}

void RingChalMaster::vf0C(void) {
    f32 rt = fn_801B306C();
    if (!bA8 && rt < 6.75f) {
        bA8 = 1;
        if (sPreRace && !fn_80161C38() && fn_80179650() == 1 && !fn_80009C84()) {
            sPreRace->Play();
        }
    }
    f32 now = lbl_8055E3C8;
    if (now > vf38() && arena.nSets > 0) {
        int i;
        for (i = 0; i < players.count; i++) {
            if (!vf24()) {
                char name[0x1E];
                PlayerData* pd;
                if (players.data[i]->b8) {
                    players.data[i]->fC += 0.0166833512f;
                }
                pd = players.data[i];
                arena.SetName(name, 0x1E);
                if (!pd->Stuck() && !arena.b49) {
                    RaceCarRec* car = pd->Car();
                    int hit;
                    int left;
                    arena.stats->At(arena.curSet)->score = arena.timer.Get();
                    hit = fn_8016A30C(car->Rings());
                    left = fn_8016A780(name, car->Rings());
                    if (left == 0 && !arena.b4A) {
                        arena.b4A = 1;
                        arena.ActivateRingSet();
                    }
                    if (arena.b4A && arena.f24 == -1) {
                        arena.ShowSetGems();
                    }
                    if ((hit > 0 || left == fn_8016A878(name) || (left > 0 && fn_8015A404(car) > 1)) && arena.b4A) {
                        arena.stats->At(arena.curSet)->time = Time();
                        arena.Deactivate(arena.curSet);
                        if (hit > 0) {
                            arena.stats->At(arena.curSet)->done = 1;
                            arena.f34 = arena.f30;
                            {
                                int pts = arena.stats->At(arena.curSet)->best;
                                arena.f30 += pts;
                                arena.difficulty++;
                            }
                            if (arena.difficulty > 2) {
                                arena.difficulty = 2;
                            }
                            if (arena.sRingHit) {
                                if (fn_80141FA0(arena.sRingHit)) {
                                    fn_80144C54(arena.sRingHit);
                                }
                                fn_80142B60(arena.sRingHit);
                            }
                            if (arena.sCheerHit) {
                                if (fn_80141FA0(arena.sCheerHit)) {
                                    fn_80144C54(arena.sCheerHit);
                                }
                                fn_80142B60(arena.sCheerHit);
                            }
                            if (arena.stats->At(arena.curSet)->best == 10 || arena.stats->At(arena.curSet)->best == 15) {
                                if (arena.sLongCheer) {
                                    fn_80142B60(arena.sLongCheer);
                                }
                            }
                        } else {
                            if (--arena.difficulty < 0) {
                                arena.difficulty = 0;
                            }
                        }
                        if (arena.curSet < arena.nSets - 1) {
                            arena.b4A = 0;
                            arena.curSet++;
                            arena.stats->At(arena.curSet)->best = arena.PointValue();
                            arena.ActivateObjSet();
                        } else {
                            arena.b48 = 1;
                            arena.b49 = 1;
                        }
                    } else {
                        arena.sets->At(arena.curSet)->CheckTeleport((Phob*)car);
                    }
                } else if (arena.f24 != -1) {
                    arena.ClearGems();
                }
            }
            if (players.data[i]->score != arena.f34) {
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
                    int tb = (int)arena.timer.t;
                    int total = arena.f30;
                    fn_801CB5DC(i, total + tb, fn_801609C4(total + (int)arena.timer.t));
                }
                fn_801CB648(i, 1, arena.timer.t);
                b9C = 0;
            }
        }
        if (!arena.b49) {
            arena.f28 += 0.0166833512f;
            if (arena.f28 > 1.75f) {
                arena.f28 = 0.0f;
            }
            if (arena.timer.Expired()) {
                arena.b49 = 1;
            } else if (fn_801B306C() <= 0.0f) {
                arena.timer.t -= 0.0166833512f;
            }
        }
        if (arena.curSet > 0) {
            int k = arena.curSet - 1;
            if (arena.b48) {
                k = arena.curSet;
            }
            if (arena.f34 < arena.f30 && arena.stats->data[k].done) {
                SetStat* st = &arena.stats->data[k];
                f32 lo = 0.3f + (0.2f + 0.05f * st->best) + 0.05f * (st->best - (arena.f30 - arena.f34));
                int n = Lerp(0.0f, 1.0f, Unlerp(lo, 0.25f + lo, lbl_8055E3C8 - st->time));
                if (n) {
                    arena.f34 += n;
                    if (arena.sTally && !arena.sTally->busy && arena.f34 != arena.f30) {
                        arena.sTally->stop = 0;
                        arena.sTally->loop = 1;
                        arena.sTally->playing = 1;
                    }
                    if (arena.f34 == arena.f30 && arena.sTally && arena.sTally->busy) {
                        arena.sTally->stop = 1;
                        arena.sTally->f4C = 0;
                    }
                }
            }
            if (arena.sLongCheer && arena.stats->data[k].done) {
                f32 b = 0.0f;
                f32 a = 0.0f;
                int lvl;
                switch (arena.stats->data[k].best) {
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
                    f32 t = Unlerp(0.0f, b, lbl_8055E3C8 - arena.stats->data[k].time);
                    arena.sLongCheer->SetVol(Lerp(a, 0.0f, t));
                }
            }
        }
    }
    if (vf24() && lbl_8055E3C8 - fA0 >= 2.25f) {
        results.Update(arena.f30 + (int)arena.timer.t, arena.f30, (int)arena.timer.t);
    }
}

inline void ObjSet::CheckTeleport(Phob* car) {
    ObjTeleport* t;
    int cur = f10;
    int next = cur + 1;
    if (next < teles->count) {
        ObjTeleport* data = teles->data;
        t = &data[next];
        V2 a;
        V2 b;
        V2 d;
        a.x = t->mtx.origin.x;
        a.y = t->mtx.origin.z;
        b.x = car->pos.x;
        d.x = a.x - b.x;
        b.y = car->pos.z;
        d.y = a.y - b.y;
        if ((f32)sqrt(d.x * d.x + d.y * d.y) < t->radius) {
            data[cur].b30 = 0;
            f10 = next;
            teles->data[next].b30 = 1;
        }
    }
}

inline void* RaceCarRec::Rings(void) { return rings; }

inline f32 ArenaTimer::Get(void) { return t; }

inline RaceCarRec* PlayerData::Car(void) { return fn_8017960C(index); }

inline u8 PlayerData::Stuck(void) { return b8; }

void Arena::SetName(char* buf, int n) { SET_NAME(this, buf, n); }



static inline f32 Time(void) { return lbl_8055E3C8; }
