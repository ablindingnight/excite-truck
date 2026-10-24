// Crush challenge master (0x80171D38-0x80176460): the "crushmaster" game mode. The player hunts AI trucks
// in an arena before the clock runs out; kills are graded by distance and speed ("cr_kill_ok"/"cr_awesome"),
// a radar points at the remaining trucks and crushed trucks respawn away from the forbidden zones
// ("crush_nogo.val"). Tuning comes from the config's "crush_table". It shares the results overlay with the
// ring challenge. Names are guesses apart from the strings.
#include "game/common.h"
#include <stdarg.h>

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
u32 strlen(const char* s);
double sqrt(double x);
double cos(double x);
double sin(double x);
double fmod(double x, double y);
f32 PSVECMag(const void* v);
f32 PSVECDistance(const void* a, const void* b);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void fn_801345D8(char* buf, int size, const char* fmt, ...);
void fn_801308F4(char* buf, int size, int value);         // number with separators
void fn_80130980(char* buf, int size, int digits, f32 v);  // fixed-point number
int fn_801609C4(int score);                                // grade for a score
void fn_802ADE60(void* layout, int flag);
void fn_802ADDD8(void* p, int a);
s32 fn_80145E70(const char* name);
s32 fn_80204CF0(void);
s32 fn_80204EA0(const char* key);
int fn_801EA994(void);  // number of cars
f32 fn_80129DC4(int n, f32 lo, f32 hi);  // random float
}

struct JoyStatus {
    f32 x;           // offset 0x0
    f32 y;           // offset 0x4
    u8 pad8[0x20 - 0x8];
    u8 buttons[32];  // offset 0x20
    u8 pad40[0x70 - 0x40];
};

extern "C" {
int fn_801298E8(JoyStatus* out, int chan);
s32* fn_801297F4(void);  // button mapping
}

struct V2 {
    f32 x;
    f32 y;
};

struct BoundSet;

extern "C" {
BoundSet* fn_80139D4C(const char* name);
void fn_80139E18(BoundSet* s);
int fn_80139EC4(BoundSet* s, f32 x, f32 y);
int fn_802B04D4(const ValVec3* p);
s32 fn_802CAC28(const char* name, int a);  // load texture
void fn_802CAC54(s32 tex);
}

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


const char* GetGradeDisplayName(int grade);
void* operator new(size_t size, const char* tag);

struct ConfigRoot {
    u8 pad0[0x20];
    ValueNode* vals;  // offset 0x20
};

ConfigRoot* fn_8000AC38(void);

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

static inline void Stop(RSound* s) {
    if (s) {
        s->Stop();
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
static inline f32 Remap(f32 x, f32 x0, f32 x1, f32 y0, f32 y1) { return Lerp(y0, y1, Unlerp(x0, x1, x)); }

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

struct Respawn {
    ValMatrix m;  // offset 0x0
    ValVec3 vel;  // offset 0x30
    s32 flag;     // offset 0x3C
    s16 path;     // offset 0x40
};

// A race start slot: the spawn transform plus how the car starts.
struct StartPos : Respawn {
    s32 f44;  // offset 0x44
    s32 f48;  // offset 0x48
    s32 f4C;  // offset 0x4C
};

struct ZoneResults {
    struct Entry {
        s32 car;    // offset 0x0
        s32 place;  // offset 0x4
        s32 score;  // offset 0x8
        u8 padC[0x10 - 0xC];
        bool b10;   // offset 0x10
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

class RaceCarRec {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual void v28(void);
    virtual void v2C(void);
    virtual void v30(void);
    virtual void v34(void);
    virtual void v38(void);
    virtual void v3C(void);
    virtual void v40(void);
    virtual void v44(void);
    virtual void v48(void);
    virtual void v4C(void);
    virtual void v50(void);
    virtual void Respawn(struct Respawn* r);  // 0x54
    u8 pad4[0x10 - 0x4];
    u32 model;        // offset 0x10
    u8 pad14[0x3C - 0x14];
    ValMatrix mtx;    // offset 0x3C
    u8 pad6C[0xD4 - 0x6C];
    f32 fD4;          // offset 0xD4
    u8 padD8[0x118 - 0xD8];
    ValVec3 center;   // offset 0x118
    u8 pad124[0x25A0 - 0x124];
    f32 turbo;        // offset 0x25A0
    u8 pad25A4[0x25D4 - 0x25A4];
    f32 speed;        // offset 0x25D4
};

extern "C" RaceCarRec* fn_8017960C(int id);

struct Camera {
    u8 pad0[0x24];
    ValVec3 pos;  // offset 0x24
};

extern "C" {
Camera* fn_802C86DC(void);
void fn_8018F76C(RaceCarRec* car, int wheel);
void fn_8001DAE0(struct Timer* t, f32 len);
}

extern "C" f32 fn_8001DAE8(struct Timer* t);  // time
extern "C" f32 fn_8001DAF0(struct Timer* t);  // progress

struct Timer {
    Timer(f32 len) { fn_8001DAE0(this, len); }
    int Running(void) {
        f32 t = fn_8001DAE8(this) - start;
        return t >= 0.0f && t < len;
    }
    void Start(void) { start = fn_8001DAE8(this); }
    void Clear(void) { start = -999.0f; }
    f32 Progress(void) { return fn_8001DAF0(this); }
    f32 len;    // offset 0x0
    f32 start;  // offset 0x4
};

// Arrow/marker vertex.
struct MarkVtx {
    ValVec3 pos;  // offset 0x0
    u32 color;    // offset 0xC
    f32 u;        // offset 0x10
    f32 v;        // offset 0x14
};

extern "C" {
void fn_802C8BD0(s32 tex);
void fn_802C8DB8(MarkVtx* v, int nv, const u16* idx, int ni);
void fn_802C8D74(void);
}

extern f32 lbl_80558DC4[];  // FLT_EPSILON

static inline void SetLen(ValVec3* v, f32 len) {
    f32 m = PSVECMag(v);
    if (!((f32)__fabs(m) <= lbl_80558DC4[0])) {
        f32 k = len / m;
        v->x *= k;
        v->y *= k;
        v->z *= k;
    }
}

static inline void AddScaled(ValVec3* out, const ValVec3* a, const ValVec3* b, f32 k) {
    out->x = a->x + b->x * k;
    out->y = a->y + b->y * k;
    out->z = a->z + b->z * k;
}

// "crush_table" settings.
struct CrushTuning {
    f32 speedA;         // offset 0x0
    f32 speedB;         // offset 0x4
    f32 speedATime;     // offset 0x8
    f32 speedBTime;     // offset 0xC
    u8 alwaysRadar;     // offset 0x10
    u8 infTurbo;        // offset 0x11
    u8 pad12[2];
    s32 pointsPerKill;  // offset 0x14
    u8 timeInSeconds;   // offset 0x18
    u8 pad19[3];
    f32 killOk;         // offset 0x1C
    f32 awesome;        // offset 0x20
    f32 killDist;       // offset 0x24
    f32 killSpeed;      // offset 0x28
};

class CrushResults {
public:
    CrushResults(void) {
        layout = 0;
        sBonus = 0;
        sNoBonus = 0;
        s1C = 0;
        s24 = 0;
        s28 = 0;
        s2C = 0;
        memset(killScore, '0', 3);
        killScore[3] = 0;
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
    ~CrushResults(void) { Unload(); }
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
    void Update(int score, int kills, int bonus);
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
    char killScore[0x14];   // offset 0x30
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

union ElemColor {
    u32 all;
    struct {
        u32 a : 8;
        u32 rgb : 24;
    } ch;
};

struct TextElem {
    TextElem(void) {}
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

// Owns the mode's HUD layout.
struct LayoutRef {
    void Free(void);
    ILayout* p;  // offset 0x0
};

struct ArtAnimManager {
    ArtAnimManager(int a);
    u8 pad0[8];
};

extern "C" int fn_80171EC0(int id);


class GameMode {
public:
    GameMode(void) {}
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
    virtual int vfA0(StartPos* out, int idx);
    virtual void vfA4(void);
    virtual void vfA8(int car);
    virtual void vfAC(int car);
    virtual int vfB0(int car, Respawn* r);
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

class CSubTaskMgr {
public:
    virtual ~CSubTaskMgr(void) {}
    virtual int vf0C(V2* pos, f32* speed, int car, int d, u32* flags);
    virtual int vf10(V2* pos, f32* speed, int car, int d, u32* flags);
    virtual int vf14(void);
    virtual void vf18(int a, int b);
    virtual int vf1C(void);
    virtual void vf20(void);
    virtual void vf24(void);
    virtual int vf28(void);
};

class CrushMaster;

// Where an AI truck is headed.
struct CrushTarget {
    u8 b0;        // offset 0x0
    u8 pad1[0xF];
    s32 f10;      // offset 0x10
    f32 x;        // offset 0x14
    f32 y;        // offset 0x18
    f32 speed;    // offset 0x1C
    f32 until;    // offset 0x20
};

class CrushSubTaskMgr : public CSubTaskMgr {
public:
    CrushSubTaskMgr(CrushMaster* m) {
        nCars = fn_801EA994();
        master = m;
    }
    virtual ~CrushSubTaskMgr(void);
    virtual int vf0C(V2* pos, f32* speed, int car, int d, u32* flags);
    virtual int vf10(V2* pos, f32* speed, int car, int d, u32* flags);
    virtual int vf14(void);
    virtual void vf18(int a, int b);
    virtual int vf1C(void);
    virtual void vf20(void);
    virtual void vf24(void);
    CrushTarget targets[8];  // offset 0x4
    s32 nCars;               // offset 0x124
    CrushMaster* master;     // offset 0x128
};

// Per-car crush state.
struct CrushCar {
    s32 Score(void);
    void ClearHist(void) { memset(f10, 0, 8); }
    u8 active;    // offset 0x0
    u8 human;     // offset 0x1
    u8 pad2[2];
    s32 bonus;    // offset 0x4
    s32 f8;       // offset 0x8
    s32 fC;       // offset 0xC
    bool f10[8];  // offset 0x10
    f32 f18;      // offset 0x18
};

struct SpawnList : ArrayRef<V2> {
    SpawnList(void) : ArrayRef<V2>(storage, 16) {}
    V2 storage[16];  // offset 0xC
};

class CrushMaster : public GameMode {
public:
    CrushMaster(void) : t174(0.0f), t17C(2.0f), t184(1.5f), t18C(1.5f) {}
    virtual ~CrushMaster(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual int vf20(int car);
    virtual int vf24(void);
    virtual int vf28(void);
    virtual f32 vf38(void);
    virtual int vf3C(int view);
    virtual void vf44(void);
    virtual void vf4C(int p);
    virtual void vf50(int p);
    virtual int vf54(void);
    virtual int vf5C(int p);
    virtual int vf64(void);
    virtual int vf68(void);
    virtual int vf8C(int p, int key);
    virtual int vfA0(StartPos* out, int idx);
    virtual void vfA8(int car);
    virtual void vfB8(ZoneResults* r);
    virtual void* vfE8(void);
    virtual void vfF8(void);
    virtual void vfFC(void);
    int AliveAI(void) {
        int n = 0;
        int i;
        for (i = 0; i < nCars; i++) {
            if (!cars[i].human && cars[i].active) {
                n++;
            }
        }
        return n;
    }
    void Finish(f32 now);
    SpawnList spawns;          // offset 0x4
    CrushCar cars[8];          // offset 0x90
    s32 nCars;                 // offset 0x170
    Timer t174;                // offset 0x174
    Timer t17C;                // offset 0x17C
    Timer t184;                // offset 0x184
    Timer t18C;                // offset 0x18C
    u8 b194;                   // offset 0x194
    u8 pad195[3];
    s32 radarTex;              // offset 0x198
    f32 endTime;               // offset 0x19C
    f32 finishTime;            // offset 0x1A0
    f32 timeout;               // offset 0x1A4
    LayoutRef layout;          // offset 0x1A8
    CrushResults results;    // offset 0x1AC
    RSound* sPreRace;          // offset 0x2B4
    u8 b2B8;                   // offset 0x2B8
};

ModuleTag lbl_8055E390;
static UnkStatic123s lbl_80430378;
UnkStaticFFFF lbl_8055E398;
UnkStaticFFFF lbl_8055E39C;
UnkStatic7F lbl_8055E3A0;
static CrushTuning lbl_8043038C;
BoundSet* lbl_8055E3A4;
s32 lbl_8055E3A8;

extern "C" {

static inline void Rotate(V2* v, f32 a) {
    f32 cs;
    f32 sn;
    f32 tx = v->x;
    cs = cos(a);
    sn = sin(a);
    v->x = v->x * cs - v->y * sn;
    v->y = tx * sn + v->y * cs;
}

int fn_80171D38(int chan) {
    JoyStatus s;
    int r = 0;
    if (fn_801298E8(&s, chan) && s.buttons[fn_801297F4()[12]]) {
        r = 1;
    }
    return r;
}

void fn_80171D94(int id) {
    RaceCarRec* car = fn_8017960C(id);
    int i;
    for (i = 0; i < 4; i++) {
        fn_8018F76C(car, i);
    }
}

int fn_80171DE4(const V2* p) {
    int r = 0;
    if (lbl_8055E3A4 && fn_80139EC4(lbl_8055E3A4, p->x, p->y)) {
        r = 1;
    }
    return r;
}

int fn_80171E3C(const V2& p) {
    ValVec3 v;
    f32 x;
    f32 z;
    z = p.y;
    x = p.x;
    v.x = x;
    v.y = 10.0f;
    v.z = z;
    if (fn_802B04D4(&v)) {
        return 0;
    }
    if (lbl_8055E3A4 && fn_80139EC4(lbl_8055E3A4, p.x, p.y)) {
        return 0;
    }
    return 1;
}

static inline V2 LerpV(const V2* a, const V2* b, f32 t) {
    V2 r;
    f32 dy = b->y - a->y;
    f32 dx = b->x - a->x;
    r.y = a->y + t * dy;
    r.x = a->x + t * dx;
    return r;
}

static inline f32 DistV(const V2* a, const V2* b) {
    V2 d;
    d.y = a->y - b->y;
    d.x = a->x - b->x;
    return sqrt(d.x * d.x + d.y * d.y);
}

int fn_80171EF8(const V2* a, const V2* b) {
    f32 len = DistV(a, b);
    f32 step = 3.0f / len;
    f32 t;
    for (t = step; t < 1.0f - step; t += step) {
        if (!fn_80171E3C(LerpV(a, b, t))) {
            return 0;
        }
    }
    return 1;
}

V2 fn_80171FF0(const V2* c, int tries, f32 r0, f32 dr) {
    V2 p;
    f32 r = r0;
    while (tries-- > 0) {
        V2 o;
        o.x = r;
        o.y = 0.0f;
        Rotate(&o, fn_80129DC4(3, 0.0f, 6.2831855f));
        p.x = c->x + o.x;
        p.y = c->y + o.y;
        if (fn_80171E3C(p)) {
            break;
        }
        r += dr;
    }
    return p;
}

u32 fn_801720E0(u32 c) { return ((c >> 1) & 0x7F7F7F) | (c & 0xFF000000); }

void fn_801720FC(const ValVec3* at, u32 color, f32 size) {
    ValVec3 up;
    ValVec3 fwd;
    ValVec3 side;
    MarkVtx v[5];
    f32 w = 0.66f * size;
    u32 dark;
    int i;
    up.x = 0.0f;
    up.y = -1.0f;
    up.z = 0.0f;
    side.x = 1.0f;
    side.y = 0.0f;
    side.z = 0.0f;
    fwd.x = 0.0f;
    fwd.y = 0.0f;
    fwd.z = 1.0f;
    SetLen(&up, -size);
    SetLen(&side, w * 0.5f);
    SetLen(&fwd, w * 0.5f);
    v[0].pos.x = 0.0f;
    v[0].pos.y = 0.0f;
    v[0].pos.z = 0.0f;
    AddScaled(&v[1].pos, &up, &side, -1.0f);
    AddScaled(&v[2].pos, &up, &side, 1.0f);
    AddScaled(&v[3].pos, &up, &fwd, -1.0f);
    AddScaled(&v[4].pos, &up, &fwd, 1.0f);
    dark = fn_801720E0(color);
    for (i = 0; i < 5; i++) {
        v[i].color = color;
        v[i].u = 0.1f * i;
        v[i].v = 0.1f * i;
        v[i].pos.x += at->x;
        v[i].pos.y += at->y;
        v[i].pos.z += at->z;
    }
    v[0].color = dark;
    {
        u16 idx[6] = {0, 1, 2, 0, 4, 3};
        fn_802C8BD0(lbl_8055E3A8);
        fn_802C8DB8(v, 5, idx, 6);
        fn_802C8D74();
        v[0].color &= 0x3FFFFFFF;
        v[1].color &= 0x3FFFFFFF;
        v[2].color &= 0x3FFFFFFF;
        v[3].color &= 0x3FFFFFFF;
        v[4].color &= 0x3FFFFFFF;
        fn_802C8BD0(-1);
        fn_802C8DB8(v, 5, idx, 6);
        fn_802C8D74();
    }
}


static inline f32 Parab(f32 t, f32 w) {
    f32 x = t - w;
    return 1.0f - x * x / (w * w);
}

static inline ValVec3* CamPos(void) { return &fn_802C86DC()->pos; }

void fn_8017248C(int id) {
    RaceCarRec* car = fn_8017960C(id);
    f32 size = Remap(PSVECDistance(&car->mtx.origin, CamPos()), 200.0f, 700.0f, 14.0f, 28.0f);
    ValVec3 p;
    p.x = car->mtx.origin.x;
    p.y = car->mtx.origin.y;
    p.z = car->mtx.origin.z;
    {
        f32 lo = 4.0f;
        f32 h = 40.0f;
        p.y += lo + h * Parab(fmod(3.742f * (2.0f * id) + lbl_8055E3C8, 2.0), 1.0f);
    }
    fn_801720FC(&p, 0xFFFFFF00, size);
}

f32 fn_801725C8(f32 t) {
    return Remap(t, lbl_8043038C.speedATime, lbl_8043038C.speedBTime, lbl_8043038C.speedA, lbl_8043038C.speedB);
}

int fn_80172624(int killer, int victim) {
    RaceCarRec* a = fn_8017960C(killer);
    RaceCarRec* b = fn_8017960C(victim);
    f32 d = PSVECDistance(&a->center, &b->center);
    f32 sp = a->speed;
    f32 f = lbl_8043038C.killDist * d + lbl_8043038C.killSpeed * sp;
    int r = 0;
    if (f >= lbl_8043038C.awesome) {
        r = 2;
    } else if (f >= lbl_8043038C.killOk) {
        r = 1;
    }
    return r;
}

}  // extern "C"

void* CrushMaster::vfE8(void) {
    return new ("subtaskmgr") CrushSubTaskMgr(this);
}

void CrushResults::Update(int score, int kills, int bonus) {
    if (layout) {
        int shown;
        int n;
        fn_801308F4(killScore, 0x14, kills);
        shown = bonus;
        n = 0;
        if (bonus && start > 0.0f) {
            f32 t = Unlerp(0.0f, 1.0f, lbl_8055E3C8 - start);
            n = Lerp(0.0f, 0.5f + bonus, t);
            shown = bonus - n;
        }
        fn_801345D8(timeLeft, 0x14, "%d", n);
        if (noBonus) {
            StrCopyN(timeBonus, "", 0x14);
            StrCopyN(bonusTitle, "", 0x14);
        } else {
            static LocString s("ResultsOverlay:TIME_BONUS_CAPS");
            fn_80130980(timeBonus, 0x14, 2, shown);
            StrCopyN(bonusTitle, s.Get(), 0x14);
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

extern "C" CrushMaster* fn_80172C28(void) {
    CrushMaster* m = new ("crushmaster") CrushMaster;
    if (m) {
        lbl_8055E3A4 = fn_80139D4C("crush_nogo.val");
        lbl_8055E3A8 = fn_802CAC28("white_zr.tex", 0);
        m->radarTex = fn_802CAC28("ovradar.tex", 0);
        if (lbl_8055E3A4) {
            fn_802E30C8("crush: Loaded forbidden zones from %s", "crush_nogo.val");
        }
        m->sPreRace = fn_80207848("chal_prerace.sfx", 1);
        m->b2B8 = 0;
    }
    return m;
}

CrushSubTaskMgr::~CrushSubTaskMgr(void) {}

CrushMaster::~CrushMaster(void) {
    if (sPreRace) {
        delete sPreRace;
    }
    if (lbl_8055E3A4) {
        fn_80139E18(lbl_8055E3A4);
        lbl_8055E3A4 = 0;
    }
    fn_802CAC54(lbl_8055E3A8);
    fn_802CAC54(radarTex);
}

void CrushMaster::vfF8(void) {
    int i;
    ValueNode* t;
    layout.p = (ILayout*)fn_80145E70("crushlyt.val");
    nCars = fn_801EA994();
    for (i = 0; i < nCars; i++) {
        cars[i].human = fn_80171EC0(i);
        cars[i].active = 1;
        cars[i].f18 = -1.0f;
        cars[i].fC = 0;
        cars[i].f8 = 0;
        cars[i].bonus = 0;
        cars[i].ClearHist();
    }
    t = fn_8000AC38()->vals->FindChild((const char*)fn_80204EA0("crush_table"));
    timeout = t->GetFloat("timeout", 180.0f);
    lbl_8043038C.speedA = 0.44444445f * t->GetFloat("ai_speed_a", 160.0f);
    lbl_8043038C.speedB = 0.44444445f * t->GetFloat("ai_speed_b", 100.0f);
    lbl_8043038C.speedATime = t->GetFloat("ai_speed_a_time", 60.0f);
    lbl_8043038C.speedBTime = t->GetFloat("ai_speed_b_time", 120.0f);
    lbl_8043038C.alwaysRadar = t->GetBool("always_show_radar", 0);
    lbl_8043038C.infTurbo = t->GetBool("infinite_turbo", 0);
    lbl_8043038C.pointsPerKill = t->GetInt("points_per_kill", 10);
    lbl_8043038C.timeInSeconds = t->GetBool("show_time_in_seconds", 0);
    lbl_8043038C.killOk = t->GetFloat("cr_kill_ok", 10.0f);
    lbl_8043038C.awesome = t->GetFloat("cr_awesome", 30.0f);
    lbl_8043038C.killDist = t->GetFloat("killfactor_dist", 1.0f);
    lbl_8043038C.killSpeed = t->GetFloat("killfactor_speed", 0.2f);
    spawns.count = 0;
    {
        ValueNode* sp = fn_8000AC38()->vals->FindChild("spawns");
        if (sp) {
            int k;
            for (k = 0;; k++) {
                char name[0x20];
                fn_801345D8(name, 0x20, "point%d", k);
                if (!sp->Has(name) || spawns.count + 1 > spawns.cap) {
                    break;
                }
                {
                    ValPoint pt = sp->GetPoint(name);
                    spawns.data[spawns.count++] = *(V2*)&pt;
                }
            }
        }
    }
    results.layout = new ("ArtAnimManager") ArtAnimManager(1);
    results.Load();
    results.Reset();
    fn_802E30C8("crush: timeout %.0f (%.0f->%.0f at (%.0f->%.0f)", timeout, lbl_8043038C.speedA / 0.44444445f,
                lbl_8043038C.speedB / 0.44444445f, lbl_8043038C.speedATime, lbl_8043038C.speedBTime);
}

inline void CrushResults::Reset(void) {
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

void CrushResults::Load(void) {
    Binding b[16] = {
        Bind(3, "time_left", &fF8),
        Bind(3, "time_bonus", &fFC),
        Bind(3, "grade", &f100),
        Bind(3, "final_score", &f104),
        Bind(1, "kill_score", killScore),
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
    fF4 = ((ArtPlayer*)(layout))->Add("a_crush_finish.val", (int)(b));
    sBonus = fn_80207848("pr_crushchal1.sfx", 1);
    sNoBonus = fn_80207848("pr_crushchal1_nobonus.sfx", 1);
    s1C = fn_80207848("pr_crushchal2.sfx", 1);
    s24 = fn_80207848("pr_transition.sfx", 1);
    s28 = fn_80207848("levelup.sfx", 1);
    s2C = fn_80207848("timeout.sfx", 1);
}

void CrushMaster::vfFC(void) {
    results.Unload();
    layout.Free();
}

void LayoutRef::Free(void) {
    if (p) {
        delete p;
        p = 0;
    }
}

int CrushMaster::vf28(void) { return AliveAI() != 0; }

int IsLoadingActive(void);

struct RaceSetup {
    u8 pad0[0x10];
    f32 time;  // offset 0x10
};

extern "C" {
RaceSetup* fn_801E7E3C(void);
void fn_8000FBE4(void);
void fn_801479A8(const TextElem* a, const TextElem* b, const char* fmt, f32 t, ...);
f32 fn_80147204(TextElem* e, const char* text);
s32 fn_80160998(s32 a);
s32 fn_80160A74(void);
void fn_802AE118(void* layout);
}  // extern "C"
void UpdateContinueOverlay(void);
extern "C" {
void fn_80139318(ILayout* l, s32 tex, int car, f32 size);  // radar
void fn_80147940(ILayout* l, Box* b, const char* art, int part);
void fn_80147230(TextElem* e, const char* fmt, ...);
void fn_801476C4(TextElem* e);
void fn_80147CEC(TextElem* out, const TextElem* a, const TextElem* b, f32 t);
void fn_801475D0(ILayout* l, const char* name, const char* fmt, ...);
void fn_8014784C(ILayout* l, const char* box, const char* set, int part);
void fn_80130978(char* buf, int size, f32 v);
f32 fn_801E7E18(void);
}  // extern "C"
void UpdateScoreOverlays(int id, int score);
extern "C" {
int vsprintf(char* buf, const char* fmt, va_list args);
}

extern "C" {
int fn_801E866C(int view);
void fn_8018AF04(RaceCarRec* car);
void fn_801E9DC4(s16* h);
f32 fn_801DCF48(int a, f32 x, f32 z);  // ground height
void fn_801B7C8C(Respawn* r, V2* pos, V2* dir);
void PSVECNormalize(const void* src, void* dst);
f32 fn_801B306C(void);  // race time
int fn_80161C38(void);
int fn_80179650(void);
int fn_80009C84(void);
void fn_801CB5DC(int id, s32 total, s32 grade);
void fn_801CB648(int id, f32 t, int done);
int fn_80179630(int view);
const char* fn_80131234(const char* key);  // localize
}

inline s32 CrushCar::Score(void) { return bonus + lbl_8043038C.pointsPerKill * (fC + f8); }

inline void CrushMaster::Finish(f32 now) {
    int i;
    for (i = 0; i < nCars; i++) {
        CrushCar* c = &cars[i];
        if (c->active) {
            c->active = 0;
            c->f18 = now;
        }
        if (c->human) {
            s32 g = fn_801609C4(c->Score());
            fn_801CB5DC(i, c->Score(), g);
            fn_801CB648(i, c->bonus, 1);
        }
    }
    finishTime = now;
}

void CrushMaster::vf0C(void) {
    f32 now = lbl_8055E3C8;
    int i;
    if (!b2B8 && fn_801B306C() < 6.75f) {
        b2B8 = 1;
        if (sPreRace && !fn_80161C38() && fn_80179650() == 1 && !fn_80009C84()) {
            sPreRace->Play();
        }
    }
    if (fn_801B306C() >= 0.0f) {
        endTime = now + timeout;
    }
    for (i = 0; i < nCars; i++) {
        if (cars[i].human && lbl_8043038C.infTurbo) {
            fn_8017960C(i)->turbo = 0.0f;
        }
        if (!cars[i].human && !cars[i].active) {
            fn_8017960C(i)->turbo = 0.9f;
        }
    }
    if (!vf24()) {
        if (AliveAI() == 0 && !t174.Running()) {
            if (b194) {
                Finish(now);
            } else {
                t174.Start();
            }
            b194 = 1;
            StrCopyN(results.bB2, fn_80131234("ResultsOverlay:CHALLENGE_COMPLETE_1"), 0x14);
            StrCopyN(results.bC6, fn_80131234("ResultsOverlay:CHALLENGE_COMPLETE_2"), 0x14);
            StrCopyN(results.complete, "", 0x14);
        }
        {
            f32 left = endTime - now;
            left = 0.0f > left ? 0.0f : left;
            if ((f32)__fabs(left) <= lbl_80558DC4[0] && AliveAI() > 0) {
                StrCopyN(results.complete, fn_80131234("ResultsOverlay:TIMED_OUT"), 0x14);
                StrCopyN(results.bB2, "", 0x14);
                StrCopyN(results.bC6, "", 0x14);
                Finish(now);
            }
        }
    } else if (!t17C.Running()) {
        CrushCar* c = &cars[fn_80179630(0)];
        results.Update(c->Score(), lbl_8043038C.pointsPerKill * (c->f8 + c->fC), c->bonus);
    }
}

void CrushMaster::vf10(void) {
    int i;
    b194 = 0;
    endTime = timeout;
    finishTime = -1.0f;
    for (i = 0; i < nCars; i++) {
        cars[i].active = 1;
        cars[i].f18 = -1.0f;
        cars[i].fC = 0;
        cars[i].f8 = 0;
        cars[i].bonus = 0;
        cars[i].ClearHist();
    }
    t17C.Clear();
    t184.Clear();
    t18C.Clear();
    t174.Clear();
    results.Reset();
    Stop(sPreRace);
    b2B8 = 0;
}

static inline int NearZero(f32 x) { return (f32)__fabs(x) <= lbl_80558DC4[0]; }
static inline f32 Max0(f32 x) { return 0.0f > x ? 0.0f : x; }

int CrushMaster::vf20(int car) {
    CrushCar* c = &cars[car];
    int r = c->active;
    if (r) {
        f32 now = lbl_8055E3C8;
        r = !NearZero(Max0(endTime - now));
    }
    return r;
}

int CrushMaster::vf24(void) { return finishTime > 0.0f; }

int CrushMaster::vf54(void) { return 8; }

static inline V2 FindSpot(const ValVec3* p) {
    V2 c;
    c.x = p->x;
    c.y = p->z;
    return fn_80171FF0(&c, 99, 15.0f, 5.0f);
}

static inline void NormV2(V2* d) {
    f32 len = sqrt(d->x * d->x + d->y * d->y);
    len = 0.01f > len ? 0.01f : len;
    d->x *= 1.0f / len;
    d->y *= 1.0f / len;
}

void CrushMaster::vfA8(int car) {
    if (cars[car].active) {
        Respawn r;
        ValVec3 dst;
        ValVec3 v;
        V2 at;
        V2 to;
        V2 d;
        V2 fwd;
        ValVec3* pos;
        RaceCarRec* rec = fn_8017960C(car);
        fn_8018AF04(rec);
        r.path = -1;
        fn_801E9DC4(&r.path);
        r.flag = 0;
        pos = &rec->mtx.origin;
        r.m = rec->mtx;
        r.vel.x = 0.0f;
        r.vel.y = 0.0f;
        r.vel.z = 0.0f;
        at.x = rec->mtx.origin.x;
        at.y = rec->mtx.origin.z;
        if (fn_802B04D4(pos) || !fn_80171E3C(at)) {
            int jump;
            {
                V2 np = FindSpot(pos);
                dst.x = np.x;
                dst.y = 0.0f;
                dst.z = np.y;
                dst.y = fn_801DCF48(0, np.x, np.y);
            }
            jump = 0;
            if (fn_802B04D4(pos) && !fn_80171DE4(&at)) {
                jump = 1;
            }
            if (jump) {
                f32 t = 4.0f;
                f32 k = 1.0f / t;
                v.x = (dst.x - pos->x) * k;
                v.y = (dst.y - pos->y) * k + 9.81f * t;
                v.z = (dst.z - pos->z) * k;
                r.m.origin.x = rec->mtx.origin.x;
                r.m.origin.y = rec->mtx.origin.y;
                r.m.origin.z = rec->mtx.origin.z;
                r.m.basis[1].x = 0.0f;
                r.m.basis[1].y = 1.0f;
                r.m.basis[1].z = 0.0f;
                r.m.basis[2].x = v.x;
                r.m.basis[2].y = 0.0f;
                r.m.basis[2].z = v.z;
                PSVECNormalize(&r.m.basis[2], &r.m.basis[2]);
                r.m.basis[0].x = r.m.basis[2].z;
                r.m.basis[0].y = 0.0f;
                r.m.basis[0].z = -r.m.basis[2].x;
                r.vel = v;
            } else {
                to.x = dst.x;
                to.y = dst.z;
                d.x = dst.x - at.x;
                d.y = dst.z - at.y;
                NormV2(&d);
                fn_801B7C8C(&r, &to, &d);
                r.m.origin.y += 3.0f;
                r.vel.x = r.m.basis[2].x * 8.888889f;
                r.vel.y = r.m.basis[2].y * 8.888889f;
                r.vel.z = r.m.basis[2].z * 8.888889f;
            }
        } else if (rec->mtx.basis[1].y < 0.2f) {
            fwd.x = rec->mtx.basis[2].x;
            fwd.y = rec->mtx.basis[2].z;
            NormV2(&fwd);
            fn_801B7C8C(&r, &at, &fwd);
        }
        rec->Respawn(&r);
    }
}

void CrushMaster::vfB8(ZoneResults* r) {
    ZoneResults::Entry* e;
    int i;
    r->nCars = nCars;
    r->nHumans = 1;
    r->done = vf24();
    r->bC = 0;
    for (i = 0, e = r->entries; i < r->nCars; e++, i++) {
        e->b10 = fn_80171EC0(i) == 0;
        e->place = i;
        e->score = cars[i].Score();
    }
}

void CrushMaster::vf44(void) {
    if (fn_801B306C() < 0.0f && !IsLoadingActive()) {
        int n = fn_801EA994();
        int i;
        for (i = 0; i < n; i++) {
            if (i != fn_801E866C(-1) && vf20(i)) {
                fn_8017248C(i);
            }
        }
    }
}

int CrushMaster::vfA0(StartPos* out, int idx) {
    if (idx < spawns.count) {
        V2* p = &spawns.data[idx];
        V2 d;
        f32 k = -1.0f;
        d.x = p->x * k;
        d.y = p->y * k;
        NormV2(&d);
        fn_801B7C8C(out, p, &d);
        out->f44 = 2;
        out->f48 = 0;
        out->f4C = 0;
        return 1;
    }
    return 0;
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

extern "C" void fn_80175080(ILayout* l, const char* from, const char* to, f32 t, const char* fmt, ...);

void CrushMaster::vf4C(int view) {
    int car = fn_801E866C(view);
    int radar = 1;
    if (!lbl_8043038C.alwaysRadar && !fn_80171D38(view)) {
        radar = 0;
    }
    if (layout.p) {
        CrushCar* cc;
        int n;
        Box* icon;
        Box* hit;
        TextElem* col;
        int i;
        if (radar && vf20(car)) {
            fn_80139318(layout.p, radarTex, car, 140.0f);
        }
        cc = &cars[car];
        n = nCars - 1;
        icon = layout.p->FindBox("ring_set_icon");
        hit = layout.p->FindBox("ring_set_hit");
        if (!hit) {
            hit = icon;
        }
        col = layout.p->Find("score_column");
        for (i = 0; i < n; i++) {
            Box b = *icon;
            int killed = i < cc->f8;
            int pts;
            b.pos.y += i * hit->h;
            if (!killed) {
                b.color.all = 0x66FFFFFF;
            }
            fn_80147940(layout.p, &b, "star_art", 0);
            if (cc->f10[i]) {
                V2 save = b.pos;
                b.pos.x += 5.0f;
                b.pos.y += 4.0f;
                fn_80147940(layout.p, &b, "star_art", 0);
                b.pos = save;
            }
            pts = killed ? lbl_8043038C.pointsPerKill * (cc->f10[i] + 1) : 0;
            if (pts) {
                TextElem t = *col;
                t.pos.x = b.pos.x + b.w * 0.5f;
                t.pos.y = b.pos.y + b.h * 0.5f;
                fn_80147230(&t, "%d", pts);
            }
        }
        {
            char buf[0x20];
            if (lbl_8043038C.timeInSeconds) {
                f32 left = endTime - fn_801E7E18();
                left = 0.0f > left ? 0.0f : left;
                fn_801345D8(buf, 0x20, "%.0f", left);
            } else {
                f32 left = endTime - fn_801E7E18();
                left = 0.0f > left ? 0.0f : left;
                fn_80130978(buf, 0x20, left);
            }
            if (AliveAI() > 0) {
                fn_801475D0(layout.p, "timer", "%s", buf);
                fn_8014784C(layout.p, "stop_watch_art", "stop_watch_artset", 0);
            }
        }
        if (t184.Running()) {
            fn_80175080(layout.p, "lame_kill_msg_0", "lame_kill_msg", Ease(t184.Progress()), 0);
        }
        if (t17C.Running()) {
            fn_80175080(layout.p, "kill_msg_0", "kill_msg", Ease(t17C.Progress()), 0);
        }
        if (t18C.Running()) {
            fn_80175080(layout.p, "awesome_kill_msg_0", "awesome_kill_msg", Ease(t18C.Progress()), 0);
        }
        if (t174.Running()) {
            fn_80175080(layout.p, "player_wins_msg_0", "player_wins_msg", Ease(t174.Progress()), 0);
        }
    }
    UpdateScoreOverlays(fn_801E866C(view), lbl_8043038C.pointsPerKill * (cars[car].fC + cars[car].f8));
}

extern "C" void fn_80175080(ILayout* l, const char* from, const char* to, f32 t, const char* fmt, ...) {
    TextElem* a = l->Find(from);
    TextElem* b = l->Find(to);
    TextElem e;
    if (a) {
        fn_80147CEC(&e, a, b, t);
    } else {
        e = *b;
    }
    if (fmt) {
        va_list args;
        char buf[0x80];
        va_start(args, fmt);
        vsprintf(buf, fmt, args);
        va_end(args);
        if (strlen(buf) >= 0x80) {
            fn_802E317C(" oversized text elem (len %d, fmt \"%s\")", strlen(buf), fmt);
        }
        fn_80147230(&e, "%s", buf);
    } else {
        fn_801476C4(&e);
    }
}

void CrushMaster::vf50(int p) { vf4C(p); }

f32 CrushMaster::vf38(void) { return 7.75f; }

int CrushMaster::vf64(void) { return 1; }

int CrushMaster::vf5C(int p) {
    fn_801E866C(p);
    if (t17C.Running()) {
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

int CrushMaster::vf68(void) { return results.state == 2; }

int CrushMaster::vf8C(int p, int key) {
    int r = 0;
    if (vf68() && key == 'X') {
        if (results.state == 2) {
            results.b9 = 1;
        }
        r = 1;
    } else if (results.state != 0) {
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
        r = 1;
    }
    return r;
}

static inline f32 Clamp01(f32 x) {
    f32 r = 1.0f < x ? 1.0f : x;
    return r > 0.0f ? r : 0.0f;
}

static inline void Blend(const TextElem* a, const TextElem* b, const char* text, f32 t) { fn_801479A8(a, b, text, t); }

static inline f32 Width(TextElem* e, const char* text) { return fn_80147204(e, text); }

int CrushMaster::vf3C(int view) {
    f32 t = fn_801E7E3C()->time;
    f32 end0 = 6.75f;
    f32 in0 = end0 - 0.8f;
    f32 full0 = in0 - 0.3f;
    f32 end1 = full0 - 0.15f;
    f32 in1 = end1 - 0.8f;
    f32 full1 = in1 - 0.3f;
    TextElem* h0 = layout.p->Find("intro_head_0");
    TextElem* h1 = layout.p->Find("intro_head_1");
    if (t < full1) {
        fn_8000FBE4();
    }
    if (!fn_80161C38()) {
        if (t < end0) {
            f32 k = Lerp(0.0f, 1.0f, Clamp01((t - in0) / (full0 - in0)));
            TextElem* body = layout.p->Find("star_body_0");
            TextElem* denom = layout.p->Find("star_body_1");
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
            fn_8014784C(layout.p, "stop_watch_art", "stop_watch_artset", 0);
            k = Lerp(0.0f, 1.0f, Clamp01((t - in1) / (full1 - in1)));
            body = layout.p->Find("time_body_0");
            timer = layout.p->Find("timer");
            if (h0 && h1 && body && timer) {
                char buf[0x14];
                Blend(h0, h1, fn_80131234("PreRaceOverlay:TIME_LIMIT"), Ease(k));
                {
                    f32 left = endTime - fn_801E7E18();
                    left = 0.0f > left ? 0.0f : left;
                    fn_80130978(buf, sizeof(buf), left);
                }
                {
                    TextElem e = *body;
                    e.pos.x += 0.5f * Width(&e, buf);
                    Blend(&e, timer, buf, Ease(k));
                }
            }
        }
    }
    return 1;
}

class CarBrain {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual void v28(void);
    virtual void v2C(void);
    virtual void Crushed(int by);  // 0x30
};

extern "C" {
CarBrain* fn_801B2EC0(int car);
u8 fn_801B3050(int car);
void fn_8017B3A8(RaceCarRec* car, const ValVec3* imp, const ValVec3* at);
void fn_8018F9B4(RaceCarRec* car);
}

static inline int Round(f32 v) { return v < 0.0f ? v - 0.5f : 0.5f + v; }

static inline void PickTarget(CrushTarget* t, int car, f32 a0, f32 a1) {
    f32 speed = fn_801725C8(lbl_8055E3C8);
    f32 dur = 10.0f;
    f32 dist = 220.0f;
    V2 tgt;
    int tries = 19;
    do {
        RaceCarRec* rec = fn_8017960C(car);
        V2 pos;
        V2 dir;
        pos.x = rec->mtx.origin.x;
        pos.y = rec->mtx.origin.z;
        dir.x = rec->mtx.basis[2].x;
        dir.y = rec->mtx.basis[2].z;
        {
            f32 len = sqrt(dir.x * dir.x + dir.y * dir.y);
            if (!((f32)__fabs(len) <= lbl_80558DC4[0])) {
                f32 k = dist / len;
                dir.x *= k;
                dir.y *= k;
            }
        }
        Rotate(&dir, fn_80129DC4(3, 0.017453292f * a0, 0.017453292f * a1));
        tgt.x = rec->mtx.origin.x;
        tgt.y = rec->mtx.origin.z;
        tgt.x += dir.x;
        tgt.y += dir.y;
        if (fn_80171EF8(&pos, &tgt)) {
            break;
        }
        dur = 0.7f * dur;
        a0 -= 10.0f;
        a1 += 10.0f;
        dur = MAX(dur, 2.0f);
        speed = 0.7f * speed;
        speed = MAX(speed, 17.777779f);
        dist = 0.9f * dist;
        dist = MAX(dist, 40.0f);
    } while (tries-- > 0);
    t->x = tgt.x;
    t->y = tgt.y;
    t->speed = speed;
    t->until = dur + lbl_8055E3C8;
}

int CrushSubTaskMgr::vf0C(V2* pos, f32* speed, int car, int d, u32* flags) {
    CrushTarget* t = &targets[car];
    *flags = 0x21F;
    pos->x = t->x;
    pos->y = t->y;
    *speed = t->speed;
    return 1;
}

extern const f32 lbl_80561AF0;  // 50.0f
int CrushSubTaskMgr::vf10(V2* pos, f32* speed, int car, int d, u32* flags) {
    RaceCarRec* rec = fn_8017960C(car);
    ValVec3 p;
    p.x = rec->mtx.origin.x + rec->mtx.basis[2].x * lbl_80561AF0;
    p.y = rec->mtx.origin.y + rec->mtx.basis[2].y * lbl_80561AF0;
    p.z = rec->mtx.origin.z + rec->mtx.basis[2].z * lbl_80561AF0;
    pos->x = p.x;
    pos->y = p.z;
    *flags = 0x21F;
    *speed = 9.777778f;
    return 1;
}

void CrushSubTaskMgr::vf24(void) {
    int i;
    for (i = 0; i < nCars; i++) {
        f32 a0;
        f32 a1;
        if (fn_801B3050(i) & 1) {
            a0 = -20.0f;
            a1 = 0.0f;
        } else {
            a0 = 0.0f;
            a1 = 20.0f;
        }
        PickTarget(&targets[i], i, a0, a1);
        targets[i].b0 = 0;
        targets[i].f10 = -1;
    }
}

void CrushSubTaskMgr::vf20(void) {
    int i;
    for (i = 0; i < nCars; i++) {
        CrushTarget* t = &targets[i];
        if (master->vf20(i)) {
            RaceCarRec* rec = fn_8017960C(i);
            V2 pos;
            V2 d;
            f32 r = 75.0f;
            pos.x = rec->mtx.origin.x;
            pos.y = rec->mtx.origin.z;
            d.x = pos.x - t->x;
            d.y = pos.y - t->y;
            if (d.x * d.x + d.y * d.y < r * r || lbl_8055E3C8 > t->until) {
                V2 here;
                rec = fn_8017960C(i);
                here.x = rec->mtx.origin.x;
                here.y = rec->mtx.origin.z;
                if (fn_80171E3C(here)) {
                    PickTarget(t, i, -20.0f, 20.0f);
                } else {
                    V2 p = fn_80171FF0(&here, 10, 5.0f, 5.0f);
                    t->x = p.x;
                    t->y = p.y;
                    t->speed = 17.777779f;
                    t->until = 5.0f + lbl_8055E3C8;
                }
            }
        }
    }
}

int CrushSubTaskMgr::vf14(void) { return 0; }

int CrushSubTaskMgr::vf1C(void) { return 0; }

void CrushSubTaskMgr::vf18(int a, int b) {
    if (master->vf20(a) && master->vf20(b)) {
        int victim = -1;
        if (fn_80171EC0(a)) {
            victim = b;
        } else if (fn_80171EC0(b)) {
            victim = a;
        }
        if (victim != -1) {
            int grade;
            CrushMaster* m;
            if (a == victim) {
                a = b;
            }
            grade = fn_80172624(a, victim);
            m = master;
            m->t17C.Clear();
            m->t18C.Clear();
            if (grade >= 1) {
                f32 now;
                m->t184.Clear();
                fn_801B2EC0(victim)->Crushed(a);
                fn_80171D94(victim);
                now = lbl_8055E3C8;
                m->cars[victim].active = 0;
                m->cars[victim].f18 = now;
                m->t17C.Start();
                m->cars[a].f10[m->cars[a].f8++] = grade == 2;
                if (grade == 2) {
                    m->cars[a].fC++;
                }
                if (m->AliveAI() == 0) {
                    f32 left = m->endTime - now;
                    left = 0.0f > left ? 0.0f : left;
                    m->cars[a].bonus = Round(0.5f + left);
                }
                if (grade == 2) {
                    ValVec3 at;
                    ValVec3 imp;
                    ValVec3 tgt;
                    RaceCarRec* ka;
                    RaceCarRec* v;
                    f32 len;
                    m->t18C.Start();
                    ka = fn_8017960C(a);
                    tgt.x = ka->mtx.origin.x + ka->mtx.basis[2].x * 50.0f;
                    tgt.y = ka->mtx.origin.y + ka->mtx.basis[2].y * 50.0f;
                    tgt.z = ka->mtx.origin.z + ka->mtx.basis[2].z * 50.0f;
                    v = fn_8017960C(victim);
                    imp.x = tgt.x - v->mtx.origin.x;
                    imp.y = tgt.y - v->mtx.origin.y;
                    imp.z = tgt.z - v->mtx.origin.z;
                    len = PSVECMag(&imp);
                    len = 0.01f > len ? 0.01f : len;
                    {
                        f32 inv = 1.0f / len;
                        imp.x *= inv;
                        imp.y *= inv;
                        imp.z *= inv;
                    }
                    {
                        f32 kk = 40.0f * v->fD4 / 0.0166833512f;
                        imp.x *= kk;
                        imp.y *= kk;
                        imp.z *= kk;
                    }
                    imp.y = 17.777779f * v->fD4 / 0.0166833512f;
                    at = v->mtx.origin;
                    at.y += 0.5f;
                    fn_8017B3A8(v, &imp, &at);
                    fn_8018F9B4(v);
                }
            } else if (!m->t184.Running()) {
                m->t184.Start();
            }
        }
    }
}
