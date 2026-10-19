// Tech demo trigger master (0x80171698-0x80171D38): the event's "actzone" entries are bounds that, once a car
// on at least the given lap drives into them, fire their "act_seq" sequence at the midpoint of the bound's
// longest edge. Registered as a race listener. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
int strcmp(const char* a, const char* b);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_80128658(void* p);              // MemFree
s32 fn_801D4888(const char* name);      // sequence by name
void fn_801D488C(s32 seq, int a, int b, const f32* pos);
int fn_801B2F24(void);                  // car count
int fn_801B2F54(int car);               // lap
void fn_801E9DC4(s16* h);
void fn_801E9E30(s16* h, f32* pos);
void fn_8000FF00(void);
}

void* operator new(size_t size, const char* tag);

struct EventDesc {
    s32 event;  // offset 0x0
};

struct Event {
    u8 pad0[0x20];
    ValueNode* vals;  // offset 0x20
};

extern "C" EventDesc* fn_801769CC(void);
Event* GetEvent(int id);

struct BoundPt {
    f32 x;
    f32 z;
};

struct Bound {
    s32 n;           // offset 0x0
    s32 cap;         // offset 0x4
    BoundPt pts[1];  // offset 0x8
};

struct V2 {
    f32 x;
    f32 z;
};

struct RaceCarRec;

extern "C" {
Bound* fn_801399FC(ValueNode* n);
int fn_80139B5C(RaceCarRec* car, Bound* b);  // inside bound
RaceCarRec* fn_8017960C(int id);
}

class IRaceListener {
public:
    virtual void a0(void) = 0;
    virtual void a1(void) = 0;
    virtual void a2(void) = 0;
    virtual void a3(void) = 0;
    virtual void a4(void) = 0;
};

extern "C" {
void fn_801777CC(IRaceListener* l);
void fn_801777D0(IRaceListener* l);
}

class TriggerMaster {
public:
    virtual ~TriggerMaster(void) {}
    virtual void Draw(void) = 0;
};

struct ZoneDraw {
    ZoneDraw(void);
    ~ZoneDraw(void);
    void Draw(void* zone, int mode, void (*fn)(void));
    u8 pad0[0x8];
};

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

template <class T, int N>
struct FixedArr : ArrayRef<T> {
    FixedArr(void) : ArrayRef<T>(storage, N) {}
    T storage[N];
};

struct Vec3 {
    Vec3(void) {}
    f32 x;
    f32 y;
    f32 z;
};

struct ActZone {
    void Load(ValueNode* n);
    void Check(int c);
    void Free(void) {
        fn_80128658(bound);
        bound = 0;
    }
    Vec3 pt[2];    // offset 0x0
    Bound* bound;  // offset 0x18
    s32 seq;       // offset 0x1C
    s32 lap;       // offset 0x20
    u8 done;       // offset 0x24
    u8 draw;       // offset 0x25
};

class TechDemoMaster : public TriggerMaster, public IRaceListener {
public:
    TechDemoMaster(void);
    virtual ~TechDemoMaster(void);
    virtual void Draw(void);
    virtual void a0(void) {}
    virtual void a4(void);
    virtual void a1(void);
    virtual void a2(void);
    virtual void a3(void);
    FixedArr<ActZone, 64> zones;  // offset 0x8
    ZoneDraw zd;                  // offset 0xA14
};

ModuleTag lbl_8055E380;
UnkStaticFFFF lbl_8055E388;

inline TechDemoMaster::TechDemoMaster(void) { fn_801777CC(this); }

extern "C" TriggerMaster* fn_80171698(void) { return new ("TriggerMaster") TechDemoMaster; }

inline TechDemoMaster::~TechDemoMaster(void) {
    int i;
    fn_801777D0(this);
    for (i = 0; i < zones.count; i++) {
        zones.data[i].Free();
    }
}

inline void TechDemoMaster::Draw(void) {
    int i;
    for (i = 0; i < zones.count; i++) {
        ActZone* z = &zones.data[i];
        if (z->draw && !z->done && z->bound->n > 1) {
            zd.Draw(z, 2, fn_8000FF00);
        }
    }
}

inline void TechDemoMaster::a4(void) {}

inline void ActZone::Check(int c) {
    if (!done && fn_801B2F54(c) >= lap && fn_80139B5C(fn_8017960C(c), bound)) {
        f32 m[3];
        m[0] = pt[0].x + pt[1].x;
        m[1] = pt[0].y + pt[1].y;
        m[2] = pt[0].z + pt[1].z;
        m[0] *= 0.5f;
        m[1] *= 0.5f;
        m[2] *= 0.5f;
        fn_801D488C(seq, 1, 1, m);
        done = 1;
    }
}

inline void TechDemoMaster::a3(void) {
    int n = fn_801B2F24();
    int c;
    for (c = 0; c < n; c++) {
        int i;
        for (i = 0; i < zones.count; i++) {
            zones.data[i].Check(c);
        }
    }
}

inline void TechDemoMaster::a2(void) {
    int i;
    for (i = 0; i < zones.count; i++) {
        zones.data[i].done = 0;
    }
}

inline void TechDemoMaster::a1(void) {
    ValueNode* vals = GetEvent(fn_801769CC()->event)->vals;
    int i;
    ValueNode* n;
    for (i = 0; (n = vals->GetChildAt(i)) != 0; i++) {
        if (strcmp(n->GetName(), "actzone") == 0) {
            zones.data[zones.count++].Load(n);
        }
    }
    fn_802E30C8("TechDemoMaster: Loaded %d actzones", zones.count);
}

inline void ActZone::Load(ValueNode* n) {
    Bound* b;
    int start;
    f32 best;
    V2 d;
    s16 h[4];
    int k;
    f32* p;
    seq = fn_801D4888(n->GetValue("act_seq"));
    draw = n->GetBool("should_draw", 1);
    lap = n->GetInt("lap", 0);
    b = bound = fn_801399FC(n->FindChild("bound"));
    start = 2;
    done = 0;
    {
        V2 e;
        e.x = b->pts[0].x - b->pts[2].x;
        e.z = b->pts[0].z - b->pts[2].z;
        best = e.x * e.x + e.z * e.z;
    }
    d.x = b->pts[0].x - b->pts[1].x;
    d.z = b->pts[0].z - b->pts[1].z;
    if (d.x * d.x + d.z * d.z > best) {
        best = d.x * d.x + d.z * d.z;
        start = 0;
    }
    d.z = b->pts[1].z - b->pts[2].z;
    d.x = b->pts[1].x - b->pts[2].x;
    if (d.x * d.x + d.z * d.z > best) {
        start = 1;
    }
    fn_801E9DC4(h);
    p = &pt[0].x;
    for (k = 0; k < 2; k++) {
        BoundPt* q = &bound->pts[(start + k) % 3];
        p[0] = q->x;
        p[1] = 0.0f;
        p[2] = q->z;
        fn_801E9E30(h, p);
        p += 3;
    }
}
