// Sound manager (0x80207960-0x80208A18, soundmgr.cpp): every live sound sorted by sound class, the
// listeners, the voice budget per class for the current quality setting, the one-shot sounds that
// are freed when they stop, and creating device sounds through the mixer. Names are guesses apart
// from the file name and messages.
#include "game/common.h"
#include "game/sound.h"
#include "game/math_types.h"
#include "macros.h"

extern "C" {
void* memset(void* dst, int c, size_t n);
void* memmove(void* dst, const void* src, size_t n);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
s32 fn_802E2C20(void);                   // milliseconds
s32 fn_802090B4(void);                   // sound quality
int fn_8026A344(int a);                  // local player
const char* fn_80206570(int cls);        // sound class name
}

void* operator new[](size_t size, const char* tag);

class BgObject {
public:
    virtual ~BgObject(void) {}
    virtual void Update(void) = 0;
};

extern "C" {
void fn_80127CF4(BgObject* obj, const char* name);
void fn_80127E28(BgObject* obj);
}

struct MixerCaps {
    u8 noPositional;  // offset 0x0
    s32 has3D;        // offset 0x4
    s32 hasPan;       // offset 0x8
};

class Mixer {
public:
    virtual ~Mixer(void);
    virtual const char* Name(void) = 0;
    virtual MixerCaps* Caps(void) = 0;
    virtual int vf14(void);
    virtual int Usable(void) = 0;
    virtual Sound* CreateSound(void* desc, int mode, int loop) = 0;
    virtual void SetListeners(void* listeners) = 0;
    virtual void Shutdown(void) = 0;
    virtual void vf28(void);
    virtual void vf2C(int a);
    virtual void vf30(void);
    virtual void vf34(int a);
    virtual void vf38(void);
    virtual void vf3C(void);
    virtual void vf40(void);
    virtual void vf44(void);
};

extern "C" {
void fn_802067C8(GameSound* s, void* listeners, s32 n);
void fn_80206824(GameSound* s, void* listeners, s32 n);
}

struct SoundList {
    SoundList(void);
    GameSound** first;  // offset 0x0
    s32 count;          // offset 0x4
};

struct Listener {
    s32 id;      // offset 0x0
    ValMatrix* mtx;  // offset 0x4
    ValVec3* vel;  // offset 0x8
};

// Voices per sound class for one quality setting.
struct VoiceBudget {
    s32 total;     // offset 0x0
    s32 cls[9];    // offset 0x4
    s32 unk28;     // offset 0x28
};

// DUMP_DATA lbl_802F9DC8 {s32,s32[9],s32}
const VoiceBudget lbl_802F9DC8[3] = {
#include "dumpdata/lbl_802F9DC8.inc"
};

ModuleTag lbl_8055ED98;
ValMatrix lbl_804891F8;
ValVec3 lbl_80489228;

// Identity pose (retail-proven cast, user 2026-11-16: genuine, spelling unproven). Retail keeps one resolved base register for
// all twelve stores; a plain m->basis[0].x store takes a split @ha/@l address for offset 0. Aids audit 2026-11-16:
// the base must come through a pointer CAST: eleven cast-free spellings (direct members, m->basis decay,
// &m->basis[0], row setter inline, accessor returning basis, reference parameter, Mat3::SetScale through Mat())
// all score 96.24, every cast spelling ((ValVec3*)m rows, (f32*)m, a same-type (ValMatrix*)m, Mat() bound to a
// reference) is exact. So retail's source had a cast here; which one is unknown. This spelling reuses the
// documented ValMatrix::Mat() view instead of a cast of its own.
static inline void Ident(ValMatrix* m) {
    Mat3& r = m->Mat();
    r.m[0][0] = 1.0f;
    r.m[0][1] = 0.0f;
    r.m[0][2] = 0.0f;
    r.m[1][0] = 0.0f;
    r.m[1][1] = 1.0f;
    r.m[1][2] = 0.0f;
    r.m[2][0] = 0.0f;
    r.m[2][1] = 0.0f;
    r.m[2][2] = 1.0f;
    m->origin.x = 0.0f;
    m->origin.y = 0.0f;
    m->origin.z = 0.0f;
}

static inline void Zero(ValVec3* v) {
    v->x = 0.0f;
    v->y = 0.0f;
    v->z = 0.0f;
}

class SoundManager : public BgObject {
public:
    SoundManager(Mixer* mixer);
    virtual ~SoundManager(void);
    virtual void Update(void);

    int Throttle(void);
    void Process(void);
    void ClearOneShots(void);
    void ReapOneShots(void);
    void Allocate(void);
    void CheckPlaying(void);
    void SortClass(SoundList* l);
    void SortClasses(void);
    void Check(void);
    void Add(GameSound* s);
    void Add2(GameSound* s);
    void Add3(GameSound* s);
    int IsPositional(int cls);
    void Remove(GameSound* s);
    Sound* Create(void* desc, int cls);
    Sound* Create2(void* desc, int cls);
    Sound* Create3(void* desc, int cls);
    void ReleaseSound(Sound* s);
    void SetListener(const Listener& l, int i);
    void SetListenerCount(s32 n);

    u8 enabled;              // offset 0x4
    u8 primaryOnly;          // offset 0x5
    s32 lastUpdate;          // offset 0x8
    SoundList all;           // offset 0xC
    SoundList classes[9];    // offset 0x14
    Listener listeners[4];   // offset 0x5C
    s32 numListeners;        // offset 0x8C
    Mixer* mixer;       // offset 0x90
    GameSound* oneShots[0x20];  // offset 0x94
    s32 numOneShots;         // offset 0x114
};

extern "C" {

void fn_80207960(void) {}

int fn_80207964(int cls) {
    switch (cls) {
    case 0:
        return 1;
    }
    return 0;
}

}  // extern "C"

SoundList::SoundList(void) {
    first = NULL;
    count = 0;
}

SoundManager::SoundManager(Mixer* mixer) {
    fn_80207960();
    all.first = new ("SoundManager::snd_tab") GameSound*[0xF0];
    all.count = 0;
    this->mixer = mixer;
    memset(listeners, 0, sizeof(listeners));
    primaryOnly = 0;
    Ident(&lbl_804891F8);
    Zero(&lbl_80489228);
    numListeners = 1;
    Listener l = {-100, &lbl_804891F8, &lbl_80489228};
    for (int i = 0; i < 4; i++) {
        SetListener(l, i);
    }
    memset(oneShots, 0, sizeof(oneShots) + sizeof(numOneShots));
    for (int c = 0; c < 9; c++) {
        classes[c].first = all.first;
    }
    lastUpdate = 0;
    fn_80127CF4(this, "SoundManager::Update");
    enabled = mixer->Usable();
}

SoundManager::~SoundManager(void) {
    ClearOneShots();
    for (int i = 0; i < all.count; i++) {
        fn_802E30C8("undestroyed sound: %s", all.first[i]->name);
    }
    if (all.first) {
        delete[] all.first;
        all.first = NULL;
    }
    all.count = 0;
    if (mixer) {
        mixer->Shutdown();
    }
    fn_80127E28(this);
    enabled = 0;
}

void Mixer::vf40(void) {}

void Mixer::vf44(void) {}

void SoundManager::SetListener(const Listener& l, int i) { listeners[i] = l; }

void SoundManager::SetListenerCount(s32 n) { numListeners = n; }

void Mixer::vf28(void) {}

void SoundManager::Process(void) {
    int i;
    for (i = 0; i < all.count; i++) {
        fn_802067C8(all.first[i], listeners, numListeners);
    }
    SortClasses();
    Allocate();
    for (i = 0; i < all.count; i++) {
        fn_80206824(all.first[i], listeners, numListeners);
    }
    CheckPlaying();
    ReapOneShots();
}

int SoundManager::Throttle(void) {
    s32 now = fn_802E2C20();
    if (now - lastUpdate <= 16) {
        return 0;
    }
    lastUpdate = now;
    return 1;
}

void SoundManager::Update(void) {
    if (Throttle()) {
        Process();
        mixer->SetListeners(listeners);
    }
}

void SoundManager::ClearOneShots(void) {
    for (int i = 0; i < numOneShots; i++) {
        delete oneShots[i];
        oneShots[i] = NULL;
    }
    numOneShots = 0;
}

void SoundManager::ReapOneShots(void) {
    for (int i = 0; i < numOneShots;) {
        if (oneShots[i]->dev->State() == 3) {
            delete oneShots[i];
            numOneShots--;
            memmove(oneShots + i, oneShots + i + 1, (numOneShots - i) * sizeof(GameSound*));
        } else {
            i++;
        }
    }
}

void SoundManager::Allocate(void) {
    int q = fn_802090B4();
    int avail = lbl_802F9DC8[q].total;
    int want[9];
    int got[9];
    int done;
    int k;
    memset(want, 0, sizeof(want));
    if (avail > 0) {
        for (int c = 0; c < 9; c++) {
            want[c] = 0;
            if (!primaryOnly || c == 0) {
                int n = classes[c].count;
                for (k = 0; k < n; k++) {
                    if (classes[c].first[k]->active) {
                        want[c]++;
                    }
                }
            }
        }
    }
    memset(got, 0, sizeof(got));
    do {
        done = 1;
        for (int c = 0; avail > 0 && c < 9; c++) {
            if (want[c]) {
                int lim = (lbl_802F9DC8[q].cls[c] < avail) ? lbl_802F9DC8[q].cls[c] : avail;
                if (want[c] > lim) {
                    avail -= lim;
                    done = 0;
                    got[c] += lim;
                    want[c] -= got[c];
                } else {
                    got[c] += want[c];
                    avail -= want[c];
                    want[c] = 0;
                }
            }
        }
    } while (!done && avail > 0);
    for (int c = 0; c < 9; c++) {
        SoundList* l = &classes[c];
        int n = l->count;
        for (k = 0; k < n; k++) {
            GameSound* s = l->first[k];
            if (s->active) {
                if (got[c] == 0) {
                    s->paused = 1;
                } else {
                    got[c]--;
                    s->paused = 0;
                }
            } else {
                s->paused = 1;
            }
        }
    }
}

void SoundManager::CheckPlaying(void) {
    int max = lbl_802F9DC8[fn_802090B4()].total;
    int playing = 0;
    for (int i = 0; i < all.count; i++) {
        if (all.first[i]->dev->State() == 1) {
            playing++;
        }
    }
    if (playing > max) {
        fn_802E317C("Hey I suck!  playing > max_sounds (%d > %d)", playing, max);
    }
}

static int Louder(GameSound* a, GameSound* b) {
    f32 pb = ((Sound3D*)b)->gain;
    f32 pa = ((Sound3D*)a)->gain;
    int gt = pa > pb;
    if (a->cls == b->cls && (u32)(a->cls - 3) <= 1) {
        int p = fn_8026A344(0);
        int bp = p == b->player;
        return p == a->player || (!bp && gt);
    }
    return gt;
}

void SoundManager::SortClass(SoundList* l) {
    int n;
    GameSound** a;
    a = l->first;
    n = l->count;
    for (int i = 1; i < n; i++) {
        GameSound* x = a[i];
        int j = i;
        while (j > 0 && Louder(x, a[j - 1])) {
            a[j] = a[j - 1];
            j--;
        }
        a[j] = x;
    }
}

void SoundManager::SortClasses(void) {
    for (int c = 0; c < 9; c++) {
        switch (c) {
        case 0:
        case 1:
        case 2:
        case 7:
            break;
        case 3:
        case 4:
        case 5:
        case 6:
        case 8:
            SortClass(&classes[c]);
            break;
        default:
            fn_802E317C("%s/%d: Invalid soundclass! (%s)", "soundmgr.cpp", 613, fn_80206570(c));
            break;
        }
    }
}

void SoundManager::Check(void) {}

void SoundManager::Add(GameSound* s) {
    const struct { int index; } cls = { s->cls };
    memmove(classes[cls.index].first + 1, classes[cls.index].first,
            (all.count - (classes[cls.index].first - all.first)) * sizeof(GameSound*));
    for (int c = cls.index + 1; c < 9; c++) {
        classes[c].first++;
    }
    *classes[cls.index].first = s;
    classes[cls.index].count++;
    all.count++;
    Check();
}

void SoundManager::Add2(GameSound* s) { Add(s); }

void SoundManager::Add3(GameSound* s) { Add(s); }

int SoundManager::IsPositional(int cls) {
    switch (cls) {
    case 3:
    case 4:
    case 5:
    case 6:
    case 8:
        return 1;
    }
    return 0;
}

void SoundManager::Remove(GameSound* s) {
    const struct { int index; } cls = { s->cls };
    GameSound** p = NULL;
    for (int i = 0; i < classes[cls.index].count; i++) {
        if (classes[cls.index].first[i] == s) {
            p = &classes[cls.index].first[i];
            break;
        }
    }
    if (p) {
        all.count--;
        classes[cls.index].count--;
        memmove(p, p + 1, (all.count - (p - all.first)) * sizeof(GameSound*));
        for (int c = cls.index + 1; c < 9; c++) {
            classes[c].first--;
        }
    } else {
        fn_802E317C("SoundManager::Remove(): Can't find sound (%s).", s->name);
    }
}

Sound* SoundManager::Create(void* desc, int cls) {
    int mode = IsPositional(cls);
    int loop = fn_80207964(cls);
    MixerCaps* caps = mixer->Caps();
    Sound* d;
    if (!caps->noPositional || mode) {
        if (caps->has3D && mode) {
            mode = 0;
        } else {
            mode = 2;
            if (caps->hasPan) {
                mode = 1;
            }
        }
        d = mixer->CreateSound(desc, mode, loop);
        if (mode == 0 && d == NULL) {
            fn_802E30C8("Creating 3d sound failed.  Supposed to be impossible.");
            d = mixer->CreateSound(desc, 1, loop);
        }
    } else {
        d = mixer->CreateSound(desc, 2, loop);
    }
    return d;
}

Sound* SoundManager::Create2(void* desc, int cls) { return Create(desc, cls); }

Sound* SoundManager::Create3(void* desc, int cls) { return Create(desc, cls); }

void SoundManager::ReleaseSound(Sound* s) { s->vf0C(); }

// NonMatching: The constructor schedules its float stores differently.
