// Tech demo trigger master (0x80171698-0x80171D38): the event's "actzone" entries are bounds that, once a car
// on at least the given lap drives into them, fire their "act_seq" sequence at the midpoint of the bound's
// longest edge. Registered as a race listener. Names are guesses apart from the strings.
#include "game/common.h"
#include "game/glow.h"
#include "game/v2.h"
#include "game/arrayref.h"
#include "game/raceopts.h"
#include "game/gamedb.h"

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
}

void* operator new(size_t size, const char* tag);

#include "game/bound.h"

struct RaceCarRec;

extern "C" {
Bound* fn_801399FC(ValueNode* n);
int fn_80139B5C(RaceCarRec* car, Bound* b);  // inside bound
RaceCarRec* fn_8017960C(int id);
}

// MATCHING-AID (linker-strip stand-in, as COLLSHAPE_BASE_DECL): retail keeps IRaceListener tables only in other units;
// techdemo's weak copy is a duplicate the linker strips (this .data is not addressed from the section start).
class __declspec(novtable) IRaceListener {
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

// MATCHING-AID (novtable, plausible original): no TriggerMaster table exists in any retail unit (its vptr stores are dead).
class __declspec(novtable) TriggerMaster {
public:
    virtual ~TriggerMaster(void) {}
    virtual void Draw(void) = 0;
};

template <class T, int N>
struct FixedArr : ArrayRef<T> {
    FixedArr(void) : ArrayRef<T>(storage, N) {}
    T storage[N];
};

struct ActZone {
    void Load(ValueNode* n);
    void Check(int c);
    void Free(void) {
        fn_80128658(bound);
        bound = 0;
    }
    ZoneVec3 pt[2];    // offset 0x0
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
    Glow glow;                    // offset 0xA14
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
            glow.Draw(z->pt, 2, 0x8000FF00);
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

static char ActZoneTag[] = "actzone";
static char SequenceKey[] = "act_seq";
static char LapKey[] = "lap";
static char BoundsTag[] = "bound";

inline void TechDemoMaster::a1(void) {
    f32 dy;
    f32 dx;
    f32 best;
    int k;
    int start;
    ActZone* zone;
    f32* p;
    ValueNode* vals = GetEvent(fn_801769CC()->event)->node;
    int i;
    ValueNode* n;
    for (i = 0; (n = vals->GetChildAt(i)) != 0; i++) {
        if (strcmp(n->GetName(), ActZoneTag) == 0) {
            zone = &zones.data[zones.count++];
            Bound* b;
            zone->Load(n);
            b = zone->bound;
            start = 2;
            zone->done = 0;
            {
                V2C e(b->pts[0].x - b->pts[2].x, b->pts[0].y - b->pts[2].y);
                best = e.x * e.x + e.y * e.y;
            }
            V2 d;
            s16 h[4];
            for (int edge = 0; edge < 2; edge++) {
                dy = b->pts[edge].y - b->pts[edge + 1].y;
                dx = b->pts[edge].x - b->pts[edge + 1].x;
                d.x = dx;
                d.y = dy;
                f32 len = d.x * d.x + d.y * d.y;
                if (len > best) {
                    best = len;
                    start = edge;
                }
            }
            fn_801E9DC4(h);
            p = &zone->pt[0].x;
            for (k = 0; k < 2; k++) {
                BoundPt* q = &zone->bound->pts[(start + k) % 3];
                p[0] = q->x;
                p[1] = 0.0f;
                p[2] = q->y;
                fn_801E9E30(h, p);
                p += 3;
            }
        }
    }
    fn_802E30C8("TechDemoMaster: Loaded %d actzones", zones.count);
}

inline void ActZone::Load(ValueNode* n) {
    seq = fn_801D4888(n->GetValue(SequenceKey));
    draw = n->GetBool("should_draw", 1);
    lap = n->GetInt(LapKey, 0);
    bound = fn_801399FC(n->FindChild(BoundsTag));
}
