// Physics camera (0x80270AE8-0x80273B4C): the camera definitions file ("camera3.val") with its
// cam/radius/group/event/fov/shakey/rumble fields, camera region lookup ("find_best_region"), pit-mode
// checks and the angle/range helpers. Written against raw field offsets; the large routines and the
// static-array initialiser are still stubs. Names are guesses apart from the strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

// A position on a camera path: the segment and how far along it.
struct PathPos {
    u8* seg;  // offset 0x0
    f32 t;    // offset 0x4
    u32 w8;   // offset 0x8
};

extern "C" {
u8* fn_801E87A8(void);
int strcasecmp(const char* a, const char* b);
u8* fn_80178C9C(void);
void fn_80136468(PathPos* out, u8* src, f32 t);
void fn_80270EC4(u8* c, u32 id);
void fn_802712F8(const char* path);
void fn_80271624(int mode, u8* p);
}

void operator delete(void* p);

// One selectable camera view.
struct CamView {
    s32 type;       // offset 0x0
    u8 pad4[0x4];
    f32 off[3];     // offset 0x8, mount offset in the car frame
    f32 rot[3];     // offset 0x14, rotation vector (degrees)
    f32 fov;        // offset 0x20 (degrees)
    u8 b24;         // offset 0x24
    u8 b25;         // offset 0x25
    u8 pad26[0x2];
};

const char* lbl_80346B24[3] = {"TV Set 1", "TV Set 2", "TV Set 3"};

// Camera type names, indexed by type (see fn_80271FAC).
const char* lbl_80346B54[19] = {
    "FIXED", "PAN", "ZOOM", "CLOSEUP", "CHASE", "IN_CAR", "TWIRL", "WHEEL", "REV_WHEEL", "BUTT",
    "HELI", "MAX", "R_FIXED", "S_PAN", "S_ZOOM", "S_CLOSEUP", "HOOD", "FLYCAM", "REPORT_ME",
};

CamView lbl_80346BA0[8] = {
    {34, {0}, {0.7f, 2.0f, 0.0f}, {0.0f, -90.0f, 0.0f}, 60.0f, 1, 1},
    {34, {0}, {-0.7f, 2.0f, 0.0f}, {0.0f, 90.0f, 0.0f}, 60.0f, 1, 1},
    {34, {0}, {0.0f, 2.0f, 1.5239999f}, {0.0f, 0.0f, 0.0f}, 80.0f, 1, 1},
    {4},
    {5},
    {34, {0}, {0.0f, 0.7f, -1.426f}, {1.2f, 180.0f, 1.6f}, 60.0f, 1, 1},
    {7},
    {34, {0}, {0.0f, 1.4f, 2.3000002f}, {0.3f, 178.5f, -2.2f}, 71.85f, 0, 0},
};

f32 lbl_8055C598 = -1.0f;  // focus distance override (< 0: use the target)

// One camera region record (its vptr follows the data).
class CamRegion {
public:
    u8* cam;         // offset 0x0, null when unused
    f32 lo1;         // offset 0x4, first heading window
    f32 hi1;         // offset 0x8
    f32 lo2;         // offset 0xC, second heading window
    f32 hi2;         // offset 0x10
    u8 dual;         // offset 0x14, second window in use
    u8 pad15[0x3];

    CamRegion(void) { cam = 0; }
    virtual ~CamRegion(void);  // vptr at 0x18
    virtual int Weight(u8* car, f32* weight, int type);
    virtual void Prepare(u8* car);
};

// A "region_sphere" table from the camera file (0x60); camera regions point at these.
class RegionSphere {
public:
    RegionSphere(void) { Reset(); }
    void Reset(void);
    int Load(ValueNode* t);
    f32 pos[3];       // offset 0x0, location_x/y/z
    f32 radius;       // offset 0xC
    u32 events[2];    // offset 0x10, event types the region applies to (bit per type)
    s32 cameras[16];  // offset 0x18, TV camera indices
    s32 cameraCount;  // offset 0x58
    s32 group;        // offset 0x5C
};

// A TV camera: one "cam" table from the camera file (0x60).
class TvCamera {
public:
    TvCamera(void) {}
    ~TvCamera(void) {}
    int Load(ValueNode* t);
    f32 pos[3];      // offset 0x0, x/y/z
    f32 ex[3];       // offset 0xC, ex_1..ex_3
    f32 fov;         // offset 0x18
    u8 moves;        // offset 0x1C, has an "_a" end pose
    u8 flags;        // offset 0x1D, 1 funky_fov, 2 shakey, 4 rumble, 8 focus_blur
    u8 pad1E[2];
    f32 from[3];     // offset 0x20
    f32 to[3];       // offset 0x2C, x_a/y_a/z_a
    f32 exFrom[3];   // offset 0x38
    f32 exTo[3];     // offset 0x44, ex_1_a..ex_3_a
    f32 fovFrom;     // offset 0x50
    f32 fovTo;       // offset 0x54, new_fov_a / fov_a
    s32 type;        // offset 0x58
    u8 pad5C[4];
};
extern s32 lbl_8055F54C;
extern s32 lbl_8055F554;
extern u8 lbl_8055F558;

// A camera cycle: how many views it has and which one is current.
struct CamCycle {
    u8 pad0[0x4];   // offset 0x0
    s32 first;      // offset 0x4, index of the first view in lbl_80346BA0
    s32 count;      // offset 0x8
    s32 cur;        // offset 0xC
    u8 pad10[0x4];  // offset 0x10
};

extern const f32 lbl_80563F9C;
extern const f32 lbl_80563FA0;
extern const f32 lbl_80563FA4;

ModuleTag lbl_8055F538;
f32 lbl_8055F540 = lbl_80563FA0;
f32 lbl_8055F544 = lbl_80563FA4;
static CamCycle lbl_8049A258[5];
static TvCamera lbl_8049A2C8[200];
static RegionSphere lbl_8049EDC8[32];
static CamRegion lbl_8049F9D8[32];
f32 lbl_8055F560 = lbl_80563F9C;

extern "C" int fn_80273494(int x);

extern "C" {

void fn_80270AE8(f32* p) {
    f32 z = 0.0f;
    p[0] = z;
    p[1] = z;
    p[3] = z;
    p[2] = z;
}

void fn_80270B00(const char* fmt, ...) {}

u32 fn_80270B50(void) { return U32(fn_801E87A8(), 0x4); }

void fn_80270B74(void) {
    for (int i = 0; i < 32; i++) {
        lbl_8049F9D8[i].cam = 0;
    }
    lbl_8055F554 = 0;
}

extern s32 lbl_8055F550;
extern u8 lbl_8055F564;

// Collects the camera regions usable for this event; falls back to region 0.
void fn_80270C04(u8* car, u32 flags) {
    fn_80270B74();
    if (lbl_8055F550) {
        u8* cam;
        u8 check = (flags >> 8) & 1;
        lbl_8055F564 = check;
        fn_80270B00("Preparing regions (%d)", lbl_8055F550);
        cam = (u8*)lbl_8049EDC8;
        for (int i = 0; i < lbl_8055F550; i++) {
            fn_80270B00("  Region %d:", i);
            fn_80270B00("  ...is ok for event type");
            CamRegion* r = &lbl_8049F9D8[lbl_8055F554];
            r->cam = cam;
            if (check) {
                r->CamRegion::Prepare(car);
            }
            lbl_8055F554++;
            cam += 0x60;
        }
        if (!lbl_8055F554) {
            fn_80270B00("Found no regions!  Using only region 0");
            lbl_8049F9D8[0].cam = (u8*)lbl_8049EDC8;
            if (check) {
                lbl_8049F9D8[0].Prepare(car);
            }
            lbl_8055F554 = 1;
        }
    }
}

int fn_80270D1C(int x) {
    switch (x) {
    case 0:
        return 1;
    case 1:
        return 2;
    default:
        return 0;
    }
}

s32 fn_80270D4C(u8* a, const u8* b) {
    f32* pb;
    f32* pa;
    if (U8(b, 0x4)) {
        pb = (f32*)(PTR(b, 0x0) + 0xC);
    } else {
        pb = (f32*)(PTR(b, 0x0) + 0x4);
    }
    if (U8(a, 0x4)) {
        pa = (f32*)(PTR(a, 0x0) + 0xC);
    } else {
        pa = (f32*)(PTR(a, 0x0) + 0x4);
    }
    return (s32)(*pa - *pb);
}

// A view into a camera track's angle window; `alt` selects the second window.
struct AngleRef {
    u8* track;  // offset 0x0
    u8 alt;     // offset 0x4
};

static inline f32* AngleLo(AngleRef* r) { return (f32*)(r->alt ? r->track + 0xC : r->track + 0x4); }
static inline f32* AngleHi(AngleRef* r) { return (f32*)(r->alt ? r->track + 0x10 : r->track + 0x8); }

extern const f32 lbl_80563F40;  // 0.5

// Meets two adjacent windows halfway: a's end and b's start become their midpoint (mod wrap).
void fn_80270DB0(AngleRef* a, AngleRef* b, f32 wrap) {
    f32 hi = *AngleHi(a);
    f32 lo = *AngleLo(b);
    if (hi < *AngleLo(a)) {
        hi += wrap;
    }
    if (lo < *AngleLo(a)) {
        lo += wrap;
    }
    f32 v = *AngleLo(a);
    f32 mid = lbl_80563F40 * (hi + lo);
    v = v > mid ? v : mid;
    if (v >= wrap) {
        v -= wrap;
    }
    *AngleHi(a) = v;
    *AngleLo(b) = v;
}

void qsort(void* base, u32 n, u32 size, s32 (*cmp)(u8*, const u8*));

// Collects the usable regions for `car`, then (when id bit 8 is set) makes each camera group's
// heading windows tile the full circle: sorted by start, neighbours meet halfway.
void fn_80270EC4(u8* car, u32 id) {
    AngleRef refs[64];
    fn_80270C04(car, id);
    if (id & 0x100) {
        for (int g = 0; g < 3; g++) {
            int n = 0;
            CamRegion* r = lbl_8049F9D8;
            for (int i = 0; i < lbl_8055F554; i++) {
                if (g == fn_80270D1C(S32(r->cam, 0x5C))) {
                    refs[n].track = (u8*)r;
                    refs[n].alt = 0;
                    if (r->dual) {
                        n++;
                        refs[n].track = refs[n - 1].track;
                        refs[n].alt = 1;
                    }
                    n++;
                }
                r++;
            }
            if (n > 0) {
                qsort(refs, n, sizeof(AngleRef), fn_80270D4C);
                f32 wrap = *(f32*)(PTR(car, 0x2C) + g * 4 + 8);
                int last = n - 1;
                for (int i = 0; i < last; i++) {
                    fn_80270DB0(&refs[i], &refs[i + 1], wrap);
                }
                fn_80270DB0(&refs[last], &refs[0], wrap);
            }
        }
        fn_80270B00("Region longitudes: (%d)", lbl_8055F554);
        CamRegion* r = lbl_8049F9D8;
        for (int i = 0; i < lbl_8055F554; i++) {
            fn_80270B00("  %2d: %.3f  %.3f", i, r->lo1, r->hi1);
            if (r->dual) {
                fn_80270B00(" +%2d: %.3f  %.3f", i, r->lo2, r->hi2);
            }
            r++;
        }
    }
}

void fn_802E30C8(const char* fmt, ...);  // debug print
int fn_801B526C(void);
void fn_80136414(PathPos* out, u8* track, s32 node);
s32 fn_801363A0(u8* track, u8* cur);
extern f32 lbl_8055F568;  // car heading
extern const f32 lbl_80563F44;
extern const f32 lbl_80563F48;  // 0.0

// The car's track controller (slot 0x18 returns its track).
class CamCarCtl {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual u8* Track(void);
};

// find_best_region: the camera record (index into lbl_8049EDC8) whose region suits `car` best for
// camera group `type` (-1 = any); regions that do not apply still win over nothing if their camera
// is enabled.
int fn_802710A0(u8* car, int type) {
    f32 best = lbl_80563F44;
    f32 altBest = best;
    CamRegion* r;
    int found = 0;
    int foundAlt = 0;
    int bestIdx = -1;
    int altIdx = -1;
    if (lbl_8055F564) {
        u8* track = ((CamCarCtl*)PTR(car, 0x270))->Track();
        f32* heading = &lbl_8055F568;
        if (fn_801B526C()) {
            PathPos pos;
            fn_80136414(&pos, track, S32(car, 0x2678));
            *heading = F32(pos.seg, 0x1C) + F32(pos.seg, 0x18) * pos.t;
        } else {
            PathPos pos;
            fn_80136414(&pos, track, fn_801363A0(track, track + 4));
            *heading = F32(pos.seg, 0x1C) + F32(pos.seg, 0x18) * pos.t;
        }
    }
    r = lbl_8049F9D8;
    for (int i = 0; i < lbl_8055F554; i++) {
        f32 w = lbl_80563F48;
        if (r->CamRegion::Weight(car, &w, type)) {
            if (!found || best < w) {
                best = w;
                bestIdx = i;
                found = 1;
            }
        } else if (!found) {
            if ((!foundAlt || altBest < w) && S32(r->cam, 0x58)) {
                altBest = w;
                altIdx = i;
                foundAlt = 1;
            }
        }
        r++;
    }
    if (!found) {
        found = foundAlt;
        bestIdx = altIdx;
    }
    if (!found) {
        if (type != -1) {
            static bool warnedGroups;
            if (!warnedGroups) {
                warnedGroups = true;
                fn_802E30C8("%s", "find_best_region: didn't find any, trying all groups...");
            }
            return fn_802710A0(car, -1);
        }
        static bool warnedAny;
        if (!warnedAny) {
            warnedAny = true;
            fn_802E30C8("%s", "find_best_region: didn't find any with CG_ANY?!?");
        }
        return -1;
    }
    return (u32)(lbl_8049F9D8[bestIdx].cam - (u8*)lbl_8049EDC8) / sizeof(RegionSphere);
}

extern s32 lbl_8055F548;  // TV camera count
int fn_8012C33C(const char* name);  // file exists
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* t);
void fn_80277740(void);
int fn_80277754(ValueNode* t);  // named camera table (namedcam.cpp)
void fn_80277828(void);

// Loads the camera file: TV cameras ("cam"), region spheres ("region_sphere") and named cameras.
// Without it, falls back to the old camera2.val (TV cameras only).
void fn_802712F8(const char* path) {
    lbl_8055F550 = 0;
    lbl_8055F548 = 0;
    fn_80277740();
    if (fn_8012C33C(path)) {
        ValueTable* t = fn_80131808(path);
        ValueNode* n;
        for (int i = 0; (n = t->GetChild(i)) != 0; i++) {
            if (!strcasecmp(n->GetName(), "cam")) {
                if (lbl_8049A2C8[lbl_8055F548].Load(n)) {
                    lbl_8055F548++;
                }
            } else if (!strcasecmp(n->GetName(), "region_sphere")) {
                if (lbl_8049EDC8[lbl_8055F550].Load(n)) {
                    lbl_8055F550++;
                }
            } else if (!fn_80277754(n)) {
                fn_802E30C8("unrecognized table '%s' in camera file", n->GetName());
            }
        }
        fn_80131818(t);
    } else if (fn_8012C33C("camera2.val")) {
        ValueTable* t = fn_80131808("camera2.val");
        ValueNode* n;
        for (int i = 0; (n = t->GetChild(i)) != 0; i++) {
            if (!strcasecmp(n->GetName(), "cam")) {
                if (lbl_8055F548 < 200) {
                    if (lbl_8049A2C8[lbl_8055F548].Load(n)) {
                        lbl_8055F548++;
                    } else {
                        fn_802E30C8("physcam: failed to load tv camera (index %d)", i);
                    }
                } else {
                    static bool warnedTooMany;
                    if (!warnedTooMany) {
                        warnedTooMany = true;
                        fn_802E30C8("%s", "physcam: too many TV cameras!");
                    }
                }
            }
        }
        fn_80131818(t);
    }
    fn_80277828();
}

void fn_8027156C(void) { fn_802712F8("camera3.val"); }

void fn_80271578(void) {
    u32 keep;
    u8* c = fn_80178C9C();
    keep = U32(c, 0xC);
    fn_80270EC4(c, fn_80270B50());
    U32(c, 0xC) = keep;
}

extern const f32 lbl_80563F4C;  // degrees to radians

static inline f32 DegToRad(f32 d) { return lbl_80563F4C * d; }

static inline void VecToRad(f32* o, const f32* v) {
    f32 z = v[2];
    f32 y = v[1];
    f32 x = v[0];
    o[2] = DegToRad(z);
    o[0] = DegToRad(x);
    o[1] = DegToRad(y);
}

void fn_802715C4(f32* out, u8* p) { VecToRad(out, (const f32*)(p + 0xC)); }

f32 fn_802715F0(u8* p) {
    f32 a = F32(p, 0x18);
    f32 v = lbl_80563F4C * a;
    f32 x;
    if (lbl_8055F544 < v) {
        x = lbl_8055F544;
    } else {
        x = v;
    }
    if (x > lbl_8055F540) {
        return x;
    }
    return lbl_8055F540;
}

}  // extern "C"
struct Mat3 {
    f32 m[9];
};

// A camera frame copied as 12 words (rotation rows + position).
struct Words12 {
    u32 w[12];
};
void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);
extern "C" {
void fn_802704D4(int to, u8* cam);
f32 PSVECDotProduct(const f32* a, const f32* b);
void fn_801BFD34(u8* out, const f32* v, const u8* frame);
extern s32 lbl_8055F548;
extern u8 lbl_8055F480;
extern const f32 lbl_80563F44;  // -1.0
extern const f32 lbl_80563F50;  // 1.0
extern const f32 lbl_80563F54;  // -2.0
extern const f32 lbl_80563F58;  // 3.0
extern const f32 lbl_80563F5C;
extern const f32 lbl_80563F60;
extern const f32 lbl_80563F64;
extern const f32 lbl_80563F68;  // minimum focus distance
extern const f32 lbl_80563F6C;  // overhead camera height
extern const f32 lbl_80563F70;  // fallback grid size
extern const f32 lbl_80563F74;
extern const f32 lbl_80563F78;  // fallback height above ground
extern f32 lbl_80499050[12];    // last camera frame
int fn_80129D90(int seed, int lo, int hi);  // random int
int fn_801769F8(void);
u32 fn_802E2C20(void);
void fn_802E317C(const char* fmt, ...);  // fatal
void fn_801E9DC4(u8* h);
int fn_801E9E30(u8* h, f32* pos);
f32 fn_801E9DDC(u8* h, f32 x, f32 z);
void fn_80265EC4(u8* shake, u32 t, int a);
void fn_80266070(u8* shake, f32* a, f32* b, f32* c);
void fn_80266170(u8* funky, u32 t, int a, int fresh, f32 fov);
void fn_802662D8(u8* funky, f32* a, f32* b, f32* c, f32* d);
void fn_80266388(u8* rumble, int a, f32* frame, u8* tv, u8* car);
void fn_80266A0C(u8* p, f32* w, u8* tv, f32 fov);
void fn_80274730(u8* p, f32 fov, f32 dist, f32 zoom, f32 stiff);
void fn_802749FC(u8* p, f32* move);
void fn_80274E94(u8* p);
void fn_80274EA8(u8* p);
void fn_80274F74(u8* p, const f32* w, f32 t);
void fn_802750C4(u8* p, f32 k, f32 dist);
void fn_802751AC(u8* p);
void fn_80275410(u8* p);
void fn_802756A4(u8* p);
void fn_80275938(u8* p);
void fn_80275B9C(u8* p, f32 radius, f32 height, f32 speed);
void fn_80275D0C(u8* p);

// The track's pit/route mode (3 = none).
static inline int PitMode(u8* track) {
    u8* r = PTR(track, 0x4);
    return r ? U8(r, 0x34) : 3;
}

// Smoothstep-style ease of a -1..1 fraction.
static inline f32 CamEase(f32 x) {
    if (x < lbl_80563F44) {
        return lbl_80563F44;
    }
    if (x > lbl_80563F50) {
        return lbl_80563F50;
    }
    if (x > lbl_80563F48) {
        return -2.0f * x * x * x + 3.0f * x * x;
    }
    return -2.0f * x * x * x - 3.0f * x * x;
}

// Per-frame TV (replay) camera: picks the best region's camera on a region change, eases moving cameras
// along the car's progress through the region, applies shake/funky fov/rumble/focus, then runs the
// camera's type. Without TV cameras it uses a grid-snapped default; with no region, an overhead view.
static inline f32 TvLerp(f32 a, f32 b, f32 k) { return a + k * (b - a); }

void fn_80271624(int mode, u8* p) {
    u8* track = ((CamCarCtl*)PTR(PTR(p, 0x140), 0x270))->Track();
    int group;
    switch (PitMode(track)) {
    case 1:
        group = 0;
        break;
    case 2:
        group = 1;
        break;
    default:
        static bool warnedPit;
        if (!warnedPit) {
            warnedPit = true;
            fn_802E30C8("%s", "PhysCam warning: car pit mode is invalid");
        }
    case 0:
        group = lbl_8049A258[0].cur + 2;
        break;
    }
    fn_80270B50();
    if (lbl_8055F548 > 1) {
        int fresh = 0;
        int idx;
        if (mode == 11) {
            idx = fn_802710A0(PTR(p, 0x140), group);
            if (idx >= 0) {
                S32(p, 0x14C) = idx;
                if (idx != S32(p, 0x150)) {
                    S32(p, 0x150) = idx;
                    RegionSphere* rs = &lbl_8049EDC8[idx];
                    fresh = 1;
                    S32(p, 0x148) = rs->cameras[fn_80129D90(6, 0, rs->cameraCount)];
                }
            }
        } else {
            idx = -1;
        }
        if (idx >= 0) {
            TvCamera* tv = &lbl_8049A2C8[S32(p, 0x148)];
            if (tv->moves) {
                if (lbl_8055F564) {
                    CamRegion* r = &lbl_8049F9D8[idx];
                    f32 f = lbl_80563F48;
                    f32 h = lbl_8055F568;
                    f32 lo = r->lo1;
                    f32 hi = r->hi1;
                    if (lo >= hi) {
                        if (h >= lo) {
                            hi += *(f32*)(PTR(track, 0x2C) + PitMode(track) * 4 + 8);
                        } else {
                            lo -= *(f32*)(PTR(track, 0x2C) + PitMode(track) * 4 + 8);
                        }
                    }
                    if (lo <= h && h <= hi) {
                        f = (h - lo) / (hi - lo);
                    } else {
                        lo = r->lo2;
                        hi = r->hi2;
                        if (lo >= hi) {
                            if (h >= lo) {
                                hi += *(f32*)(PTR(track, 0x2C) + PitMode(track) * 4 + 8);
                            } else {
                                lo -= *(f32*)(PTR(track, 0x2C) + PitMode(track) * 4 + 8);
                            }
                        }
                        if (lo <= h && h <= hi) {
                            f = (h - lo) / (hi - lo);
                        }
                    }
                    f32 e = CamEase(f);
                    f32 v[3];
                    v[0] = TvLerp(tv->from[0], tv->to[0], e);
                    v[1] = TvLerp(tv->from[1], tv->to[1], e);
                    v[2] = TvLerp(tv->from[2], tv->to[2], e);
                    tv->pos[0] = v[0];
                    tv->pos[1] = v[1];
                    tv->pos[2] = v[2];
                    f32 x[3];
                    x[0] = TvLerp(tv->exFrom[0], tv->exTo[0], e);
                    x[1] = TvLerp(tv->exFrom[1], tv->exTo[1], e);
                    x[2] = TvLerp(tv->exFrom[2], tv->exTo[2], e);
                    tv->ex[0] = x[0];
                    tv->ex[1] = x[1];
                    tv->ex[2] = x[2];
                    tv->fov = TvLerp(tv->fovFrom, tv->fovTo, e);
                } else {
                    tv->pos[0] = tv->from[0];
                    tv->pos[1] = tv->from[1];
                    tv->pos[2] = tv->from[2];
                    tv->ex[0] = tv->exFrom[0];
                    tv->ex[1] = tv->exFrom[1];
                    tv->ex[2] = tv->exFrom[2];
                    tv->fov = tv->fovFrom;
                }
            }
            F32(p, 0x7C) = tv->pos[0];
            F32(p, 0x80) = tv->pos[1];
            F32(p, 0x84) = tv->pos[2];
            F32(p, 0x120) = lbl_80563F48;
            F32(p, 0x124) = lbl_80563F48;
            F32(p, 0x128) = lbl_80563F48;
            if (lbl_8055F54C != tv->type) {
                fn_801E9DC4(p + 0x1EC);
            }
            if (tv->flags & 2) {
                int a = fn_801769F8();
                fn_80265EC4(p + 0x160, fn_802E2C20(), a);
                fn_80266070(p + 0x160, (f32*)(p + 0x4C), (f32*)(p + 0xE4), (f32*)(p + 0x12C));
            } else {
                F32(p, 0x4C) = F32(p, 0xE4);
                F32(p, 0x50) = F32(p, 0xE8);
                F32(p, 0x54) = F32(p, 0xEC);
            }
            if (tv->flags & 1) {
                int a = fn_801769F8();
                fn_80266170(p + 0x178, fn_802E2C20(), a, fresh, F32(p, 0xBC));
                f32 fov = F32(p, 0x180) + F32(p, 0x184) * (F32(p, 0xBC) - F32(p, 0x180));
                F32(p, 0x194) = fov;
                F32(p, 0xBC) = fov;
            }
            switch (tv->type) {
            case 0: {
                f32 fov = fn_802715F0((u8*)tv);
                f32 w[3];
                fn_80274F74(p, w, fov);
                fn_802715C4(w, (u8*)tv);
                break;
            }
            case 1:
                fn_80274730(p, lbl_80563F4C * tv->fov, lbl_80563F44, lbl_80563F44, lbl_80563F48);
                break;
            case 2:
                fn_80274E94(p);
                break;
            case 3:
                fn_80274EA8(p);
                break;
            case 4:
                fn_802704D4(4, p);
                break;
            case 5: {
                f32 move[3];
                move[0] = lbl_80563F5C;
                move[1] = lbl_80563F50;
                move[2] = lbl_80563F48;
                lbl_8055F480 = 1;
                fn_802749FC(p, move);
                break;
            }
            case 6:
                fn_80275B9C(p, lbl_80563F60, lbl_80563F64, lbl_80563F50);
                break;
            case 7:
                fn_80275410(p);
                break;
            case 8:
                fn_802756A4(p);
                break;
            case 9:
                fn_80275938(p);
                break;
            case 10:
                fn_80275D0C(p);
                break;
            case 12: {
                f32 fov = fn_802715F0((u8*)tv);
                f32 w[3];
                fn_802715C4(w, (u8*)tv);
                fn_80266A0C(p, w, (u8*)tv, fov);
                break;
            }
            case 16:
                fn_802751AC(p);
                break;
            case 17:
                fn_802750C4(p, tv->ex[0], tv->ex[1]);
                break;
            default:
                fn_802E317C("Unknown camera %d", tv->type);
                break;
            }
            lbl_8055F54C = tv->type;
            if (tv->flags & 1) {
                fn_802662D8(p + 0x178, (f32*)(p + 0x7C), (f32*)(p + 0x7C), (f32*)(p + 0x70), (f32*)(p + 0x4C));
            }
            if (tv->flags & 4) {
                fn_80266388(p + 0x188, fn_801769F8(), (f32*)(p + 0x58), (u8*)tv, PTR(p, 0x140));
            }
            if (tv->flags & 8) {
                f32 d[3];
                d[0] = F32(p, 0xE4) - F32(p, 0x7C);
                d[1] = F32(p, 0xE8) - F32(p, 0x80);
                d[2] = F32(p, 0xEC) - F32(p, 0x84);
                f32 focus = PSVECDotProduct(d, (f32*)(p + 0x70));
                focus = lbl_8055C598 >= lbl_80563F48 ? lbl_8055C598 : focus;
                F32(p, 0x1A0) = focus > lbl_80563F68 ? focus : lbl_80563F68;
            }
        } else if (idx != -10) {
            f32 off[3];
            off[0] = lbl_80563F48;
            off[1] = lbl_80563F6C;
            off[2] = lbl_80563F48;
            Mat3 down;
            down.m[0] = lbl_80563F50;
            down.m[1] = lbl_80563F48;
            down.m[2] = lbl_80563F48;
            down.m[3] = lbl_80563F48;
            down.m[4] = lbl_80563F48;
            down.m[5] = lbl_80563F50;
            down.m[6] = lbl_80563F48;
            down.m[7] = lbl_80563F44;
            down.m[8] = lbl_80563F48;
            fn_801BFD34(p + 0x7C, off, p + 0xC0);
            Mat3Mul((Mat3*)(p + 0x58), &down, (Mat3*)(p + 0xC0));
            *(Words12*)lbl_80499050 = *(Words12*)(p + 0x58);
        }
    } else {
        F32(p, 0x7C) = lbl_80563F70 * ((f32)(int)(lbl_80563F40 + (lbl_80563F74 + F32(p, 0xE4) / lbl_80563F70)) - lbl_80563F74);
        F32(p, 0x84) = lbl_80563F70 * ((f32)(int)(lbl_80563F40 + (lbl_80563F74 + F32(p, 0xEC) / lbl_80563F70)) - lbl_80563F74);
        fn_801E9DC4(p + 0x1EC);
        if (!fn_801E9E30(p + 0x1EC, (f32*)(p + 0x7C))) {
            F32(p, 0x80) = fn_801E9DDC(p + 0x1EC, F32(p, 0xE4), F32(p, 0xEC));
        }
        F32(p, 0x80) = F32(p, 0x80) + lbl_80563F78;
        fn_80274E94(p);
        *(Words12*)lbl_80499050 = *(Words12*)(p + 0x58);
    }
}

int fn_80271FAC(const char* name) {
    for (int i = 0; i < 18; i++) {
        if (i != 11 && !strcasecmp(name, lbl_80346B54[i])) {
            return i;
        }
    }
    return 11;
}

}  // extern "C"
u32 FindEventType(const char* name);
extern "C" {
int sprintf(char* buf, const char* fmt, ...);
int strncmp(const char* a, const char* b, u32 n);
}

#define REGION_MISSING "can't load region sphere - value '%s' is missing!"

int RegionSphere::Load(ValueNode* t) {
    int j;
    int i;
    Reset();
    if (!t->Has("location_x")) {
        fn_80270B00(REGION_MISSING, "location_x");
        return 0;
    }
    pos[0] = t->GetFloat("location_x");
    if (!t->Has("location_y")) {
        fn_80270B00(REGION_MISSING, "location_y");
        return 0;
    }
    pos[1] = t->GetFloat("location_y");
    if (!t->Has("location_z")) {
        fn_80270B00(REGION_MISSING, "location_z");
        return 0;
    }
    pos[2] = t->GetFloat("location_z");
    if (!t->Has("radius")) {
        fn_80270B00(REGION_MISSING, "radius");
        return 0;
    }
    radius = t->GetFloat("radius");
    group = t->GetInt("group", 2);
    const char* key;
    const char* value;
    for (i = 0; t->GetPair(i, &key, &value); i++) {
        if (!strncmp(key, "event", 5) && key[5] != '_') {
            int type = (u8)FindEventType(value);
            if (type < 32) {
                events[0] |= 1 << type;
            } else {
                events[1] |= 1 << (type - 32);
            }
        }
    }
    if (!t->Has("camera_count")) {
        fn_80270B00(REGION_MISSING, "camera_count");
        return 0;
    }
    cameraCount = t->GetIntValue("camera_count");
    for (j = 0; j < cameraCount; j++) {
        char name[16];
        sprintf(name, "camera%d", j);
        if (!t->Has(name)) {
            fn_80270B00(REGION_MISSING, name);
            return 0;
        }
        cameras[j] = t->GetIntValue(name);
    }
    return 1;
}
extern "C" {

}  // extern "C"
void RegionSphere::Reset(void) {
    u8* p = (u8*)this;
    F32(p, 0x0) = 0.0f;
    F32(p, 0x4) = 0.0f;
    F32(p, 0x8) = 0.0f;
    F32(p, 0xC) = 10.0f;
    memset(p + 0x10, 0, 8);
    S32(p, 0x58) = 0;
    S32(p, 0x5C) = 2;
}
extern "C" {

void fn_802723B4(u8* src, f32* out, f32 at) {
    PathPos pos;
    fn_80136468(&pos, src, at);
    f32 t = pos.t;
    u8* q = pos.seg;
    f32 x = F32(q, 0x0);
    out[0] = x + t * (F32(PTR(q, 0x14), 0x0) - x);
    f32 y = F32(q, 0x4);
    out[1] = y + t * (F32(PTR(q, 0x14), 0x4) - y);
}

// A 2D point on the track plane.
struct TrackPt {
    f32 x, y;
};

static inline f32 AbsF(f32 v) { return (f32)__fabs(v); }
static inline double AbsD(double v) { return __fabs(v); }

// Finds where the looping path `track` crosses the circle of squared radius `r2` around `c`, between
// path positions a and b (b may have wrapped past the loop end), by bisection.
f32 fn_80272428(u8* track, const f32* c, f32 a, f32 b, f32 r2) {
    PathPos pa;
    PathPos pb;
    TrackPt qa;
    TrackPt qb;
    TrackPt da;
    TrackPt db;
    f32 wrap = *(f32*)(PTR(track, 0x2C) + 8);
    if (a > b) {
        f32 span = b - (a - wrap);
        if (span * span < lbl_8055F560) {
            return a;
        }
    } else {
        f32 span = b - a;
        if (span * span < lbl_8055F560) {
            return a;
        }
    }
    fn_80136468(&pa, track, a);
    f32 paT = pa.t;
    qa.x = F32(pa.seg, 0x0) + paT * (F32(PTR(pa.seg, 0x14), 0x0) - F32(pa.seg, 0x0));
    qa.y = F32(pa.seg, 0x4) + pa.t * (F32(PTR(pa.seg, 0x14), 0x4) - F32(pa.seg, 0x4));
    da.y = qa.y - c[1];
    da.x = qa.x - c[0];
    f32 ea = (f32)AbsD(r2 - (da.x * da.x + da.y * da.y));
    if (ea < lbl_8055F560) {
        return a;
    }
    fn_80136468(&pb, track, b);
    f32 pbT = pb.t;
    qb.x = F32(pb.seg, 0x0) + pbT * (F32(PTR(pb.seg, 0x14), 0x0) - F32(pb.seg, 0x0));
    qb.y = F32(pb.seg, 0x4) + pb.t * (F32(PTR(pb.seg, 0x14), 0x4) - F32(pb.seg, 0x4));
    db.y = qb.y - c[1];
    db.x = qb.x - c[0];
    f32 eb = (f32)AbsD(r2 - (db.x * db.x + db.y * db.y));
    if (eb < lbl_8055F560) {
        return b;
    }
    f32 mid;
    if (a > b) {
        mid = (b + (a - wrap)) * lbl_80563F40;
        if (mid < lbl_80563F48) {
            mid += wrap;
        }
    } else {
        mid = (a + b) * lbl_80563F40;
    }
    if (ea < eb) {
        return fn_80272428(track, c, a, mid, r2);
    }
    return fn_80272428(track, c, mid, b, r2);
}

void fn_8027264C(const char* name, u8* src, f32 x) {
    f32 v[2];
    fn_802723B4(src, v, x);
    fn_80270B00("  %s: %.3f  (%.3f, %.3f)", name, x, v[0], v[1]);
}

int fn_802726AC(f32 x, f32 a, f32 b) {
    if (a < b) {
        return a < x && x < b;
    }
    return a > x && x > b;
}

int fn_802726EC(f32 x, f32 a, f32 b) {
    int r = 0;
    if (x == a || x == b || fn_802726AC(x, a, b)) {
        r = 1;
    }
    return r;
}

}  // extern "C"
struct V2 {
    f32 x, y;
};

// A position on an ideal line (idealline.cpp).
struct ILPos {
    ILPos(void) {
        seg = 0;
        mode = 10000;
        t = lbl_80563F48;
    }
    u8* seg;   // offset 0x0
    f32 t;     // offset 0x4
    s32 mode;  // offset 0x8
};

class LineFollower {
public:
    void Locate(V2* pos, V2* dir, ILPos* out, int unused);
};

extern "C" {
double sqrt(double x);
int fn_802726EC(f32 x, f32 a, f32 b);
void fn_802723B4(u8* src, f32* out, f32 at);
void fn_8027264C(const char* name, u8* src, f32 x);
void* memmove(void* d, const void* s, u32 n);
extern f32 lbl_80558DC4[];  // epsilon
}
extern const f32 lbl_80563F50;  // 1.0
extern const f32 lbl_80563F8C;  // 2.0
extern const f32 lbl_80563F90;  // 4.0

// Works out this region's heading windows on `line`: where the sphere's circle crosses the line
// (two crossings = one window, four = two), falling back to the nearest line point +- radius.
void CamRegion::Prepare(u8* line) {
    V2 center;
    V2 s;
    V2 e;
    V2 dir;
    V2 rel;
    f32 sol[2];
    V2 pt;
    V2 d1;
    V2 d2;
    center.x = F32(cam, 0x0);
    center.y = F32(cam, 0x8);
    f32 rad = F32(cam, 0xC);
    f32 wrap = *(f32*)(PTR(line, 0x2C) + 8);
    f32 r2 = rad * rad;
    S32(line, 0xC) = fn_80270D1C(S32(cam, 0x5C));
    int k = 0;
    int done = 0;
    u8* first = PTR(PTR(line, 0x2C), 0x1C);
    u8* seg = first;
    do {
        int n = 0;
        s.x = F32(seg, 0x0) + lbl_80563F48 * (F32(PTR(seg, 0x14), 0x0) - F32(seg, 0x0));
        s.y = F32(seg, 0x4) + lbl_80563F48 * (F32(PTR(seg, 0x14), 0x4) - F32(seg, 0x4));
        e.x = F32(seg, 0x0) + lbl_80563F50 * (F32(PTR(seg, 0x14), 0x0) - F32(seg, 0x0));
        e.y = F32(seg, 0x4) + lbl_80563F50 * (F32(PTR(seg, 0x14), 0x4) - F32(seg, 0x4));
        dir.x = e.x - s.x;
        dir.y = e.y - s.y;
        rel.x = s.x - center.x;
        f32 a = dir.x * dir.x + dir.y * dir.y;
        rel.y = s.y - center.y;
        f32 b = lbl_80563F8C * (rel.x * dir.x + rel.y * dir.y);
        f32 c = rel.x * rel.x + rel.y * rel.y - r2;
        f32 disc = b * b - lbl_80563F90 * a * c;
        if (!(AbsF(a) <= lbl_80558DC4[0]) && disc > lbl_80563F48) {
            f32 q = (f32)sqrt(disc);
            n = 2;
            sol[0] = (-b - q) / (lbl_80563F8C * a);
            sol[1] = (-b + q) / (lbl_80563F8C * a);
        }
        for (int j = 0; j < n; j++) {
            if (fn_802726EC(sol[j], lbl_80563F48, lbl_80563F50)) {
                f32 v = F32(seg, 0x1C) + F32(seg, 0x18) * sol[j];
                if (k < 4) {
                    (&lo1)[k] = v;
                    fn_80270B00("  solution: %.3f", (&lo1)[k++]);
                } else {
                    fn_80270B00("    too many solutions, discarding %.3f", v);
                }
            }
        }
        u8* next = PTR(seg, 0x14);
        u32 idx = U32(seg, 0x10);
        seg = next;
        if (next == first || k >= 4 || U32(next, 0x10) == 0) {
            done = 1;
        } else if (U32(next, 0x10) < idx) {
            fn_80270B00("** PhysCam: lon_idx < old_lon_idx, but lon_idx != 0");
            done = 1;
        }
    } while (!done);
    if (k == 0) {
        fn_80270B00("failed to find intersection, using lame method");
        ILPos pos;
        ((LineFollower*)line)->Locate(&center, 0, &pos, 0);
        f32 mid = F32(pos.seg, 0x1C) + F32(pos.seg, 0x18) * pos.t;
        f32 lo = mid - rad;
        if (rad > mid) {
            lo += wrap;
        }
        f32 hi = mid + rad;
        if (wrap < rad - mid) {
            hi -= wrap;
        }
        (&lo1)[0] = fn_80272428(line, &center.x, lo, mid, r2);
        (&lo1)[1] = fn_80272428(line, &center.x, mid, hi, r2);
        k = 2;
    } else if (k == 1) {
        (&lo1)[1] = (&lo1)[0];
        k++;
    } else if (k == 3) {
        (&lo1)[3] = (&lo1)[2];
        k++;
    }
    if (k > 2) {
        fn_80270B00("  region is in the x!");
        dual = 1;
    } else {
        dual = 0;
    }
    fn_802723B4(line, &pt.x, lbl_80563F40 * ((&lo1)[0] + (&lo1)[1]));
    d1.y = pt.y - center.y;
    d1.x = pt.x - center.x;
    if (d1.x * d1.x + d1.y * d1.y > r2) {
        if (dual) {
            fn_802723B4(line, &pt.x, lbl_80563F40 * ((&lo1)[1] + (&lo1)[2]));
            d2.y = pt.y - center.y;
            d2.x = pt.x - center.x;
            if (d2.x * d2.x + d2.y * d2.y > r2) {
                dual = 0;
            } else {
                f32 first0 = (&lo1)[0];
                memmove(&(&lo1)[0], &(&lo1)[1], 12);
                (&lo1)[3] = first0;
            }
        }
        if (!dual) {
            f32 t0 = (&lo1)[0];
            (&lo1)[0] = (&lo1)[1];
            (&lo1)[1] = t0;
        }
    }
    fn_80270B00("  finding line points for (%.3f, %.3f)", center.x, center.y);
    fn_8027264C("start_long", line, (&lo1)[0]);
    fn_8027264C("end_long", line, (&lo1)[1]);
    if (dual) {
        fn_8027264C("start_long_2", line, (&lo1)[2]);
        fn_8027264C("end_long_2", line, (&lo1)[3]);
    }
}
extern "C" {

int fn_80272C24(f32 x, f32 a, f32 b, f32 m) {
    if (a == b) {
        return 1;
    }
    if (a > b) {
        if (a > x) {
            a -= m;
        } else {
            b += m;
        }
    }
    return x >= a && x <= b;
}

class CarCtl {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual u8* Track(void);  // 0x18
};

f32 PSVECDistance(const u8* a, const u8* b);
extern f32 lbl_8055F568;        // car heading
extern const f32 lbl_80563F48;  // 0.0
extern const f32 lbl_80563F50;  // 1.0

// How strongly `r` applies to `car` (0..1 into *weight); returns whether the region is usable.
}  // extern "C"
int CamRegion::Weight(u8* car, f32* weight, int type) {
    CamRegion* r = this;
    *weight = lbl_80563F48;
    if (!S32(r->cam, 0x58)) {
        return 0;
    }
    if (type != -1 && type != S32(r->cam, 0x5C)) {
        return 0;
    }
    int ok = 1;
    if (lbl_8055F564) {
        f32 wrap = F32(PTR(((CarCtl*)PTR(car, 0x270))->Track(), 0x2C), 0x8);
        if (fn_80272C24(lbl_8055F568, r->lo1, r->hi1, wrap) ||
            (r->dual && fn_80272C24(lbl_8055F568, r->lo2, r->hi2, wrap))) {
            *weight = lbl_80563F50;
            return 1;
        }
        ok = 0;
    }
    f32 d = PSVECDistance(car + 0x60, r->cam) - F32(r->cam, 0xC);
    if (d <= lbl_80563F48) {
        *weight = lbl_80563F50;
    } else {
        *weight = lbl_80563F50 / (lbl_80563F50 + d);
    }
    return ok;
}
extern "C" {

}  // extern "C"
extern const f32 lbl_80563F94;  // 0.75: old "fov" values to new_fov units
extern const f32 lbl_80563F98;  // 20.0

// One "cam" table: position, extras, field of view ("new_fov", or old-style "fov" * 0.75), type and
// flags, plus an optional end pose ("x_a"...) the camera moves to.
int TvCamera::Load(ValueNode* t) {
    if (t->Has("x") && t->Has("y") && t->Has("z") && (t->Has("fov") || t->Has("new_fov")) && t->Has("type")) {
        pos[0] = t->GetFloat("x");
        pos[1] = t->GetFloat("y");
        pos[2] = t->GetFloat("z");
        if (t->Has("new_fov")) {
            fov = t->GetFloat("new_fov");
        } else {
            fov = lbl_80563F94 * t->GetFloat("fov");
        }
        type = fn_80271FAC(t->GetValue("type"));
        ex[0] = t->GetFloat("ex_1", lbl_80563F48);
        ex[1] = t->GetFloat("ex_2", lbl_80563F48);
        ex[2] = t->GetFloat("ex_3", lbl_80563F50);
        int funky = t->GetBool("funky_fov", 0);
        int shakey = t->GetBool("shakey", 0);
        int rumble = t->GetBool("rumble", 0);
        int blur = t->GetBool("focus_blur", 0);
        flags = funky | (rumble ? 4 : 0) | (shakey ? 2 : 0) | (blur ? 8 : 0);
        switch (type) {
        case 13:
            type = 1;
            flags |= 2;
            break;
        case 14:
            flags |= 2;
            type = 2;
            break;
        case 15:
            flags |= 2;
            type = 3;
            break;
        case 12:
            flags |= 4;
            type = 0;
            break;
        }
        if (t->Has("x_a") && t->Has("y_a") && t->Has("z_a")) {
            moves = 1;
            from[0] = pos[0];
            from[1] = pos[1];
            from[2] = pos[2];
            exFrom[0] = ex[0];
            exFrom[1] = ex[1];
            exFrom[2] = ex[2];
            fovFrom = fov;
            to[0] = t->GetFloat("x_a", pos[0]);
            to[1] = t->GetFloat("y_a", lbl_80563F98 + pos[1]);
            to[2] = t->GetFloat("z_a", pos[2]);
            if (t->Has("new_fov_a")) {
                fovTo = t->GetFloat("new_fov_a");
            } else if (t->Has("fov_a")) {
                fovTo = lbl_80563F94 * t->GetFloat("fov_a");
            } else {
                fovTo = fovFrom;
            }
            exTo[0] = t->GetFloat("ex_1_a", exFrom[0]);
            exTo[1] = t->GetFloat("ex_2_a", exFrom[1]);
            exTo[2] = t->GetFloat("ex_3_a", exFrom[2]);
        } else {
            moves = 0;
        }
        return 1;
    }
    return 0;
}
extern "C" {

void fn_80273354(void) {
    memset(lbl_8049A258, 0, 100);
    S32(lbl_8049A258, 0x18) = 0;
    S32(lbl_8049A258, 0x1C) = 1;
    S32(lbl_8049A258, 0x2C) = 1;
    S32(lbl_8049A258, 0x30) = 1;
    S32(lbl_8049A258, 0x40) = 2;
    S32(lbl_8049A258, 0x44) = 3;
    S32(lbl_8049A258, 0x54) = 5;
    S32(lbl_8049A258, 0x58) = 3;
    fn_8027156C();
}

void fn_802733C8(void) {
    fn_80271578();
    lbl_8055F54C = 0x12;
}

void fn_802733F0(u8* p) {}

void fn_802733F4(u8* p) {
    if (!U8(p, 0x138) && lbl_8055F558) {
        int t = fn_80273494(S32(p, 0x10));
        if (t != 6) {
            CamCycle* c = &((CamCycle*)lbl_8049A258)[t];
            if (++c->cur >= c->count) {
                c->cur = 0;
            }
            if (c->count > 1) {
                U8(p, 0x138) = 1;
            }
        }
    }
    lbl_8055F558 = 0;
}

void fn_80273490(int mode, u8* p) { fn_80271624(mode, p); }

int fn_80273494(int x) {
    switch (x) {
    case 0xB:
        return 0;
    case 0xC:
        return 1;
    case 0xD:
        return 2;
    case 0xE:
        return 3;
    case 0xF:
        return 4;
    default:
        return 6;
    }
}

extern u8 lbl_8055F480;

// HUD/feature flags for the camera used in `mode`.
u32 fn_802734F0(int mode) {
    int t = fn_80273494(mode);
    u32 f = 0;
    if (lbl_8055F480) {
        f |= 8;
    } else if (t != 6 && t != 0) {
        CamCycle* c = &((CamCycle*)lbl_8049A258)[t];
        CamView* v = &lbl_80346BA0[c->cur + c->first];
        if (!v->b24) {
            f |= 1;
        } else if (v->type == 2) {
            f |= 2;
        } else if (v->type == 0) {
            f |= 8;
        }
        if (!v->b25) {
            f |= 4;
        }
    } else if (mode != 1) {
        switch (mode) {
        case 2:
            f |= 6;
            break;
        case 0:
            f |= 8;
            break;
        default:
            f |= 5;
            break;
        }
    }
    return f;
}

void fn_802735F4(int a, u8 b) {
    if (a == 0) {
        lbl_8055F558 = b;
    }
}

f32 PSVECMag(const f32* v);
double sin(double x);
double cos(double x);
void fn_801BFD34(u8* out, const f32* v, const u8* frame);
void fn_80273BD0(u8* p);
void fn_80274268(u8* p);
void fn_802704D0(int to, int from, u8* cam);
void fn_802704D4(int to, u8* cam);
extern const f32 lbl_80563F4C;  // degrees to radians

// Rotation from the rotation vector w (axis * angle), Rodrigues' formula (as camstate's).
static inline void CamRotFromVec(Mat3* r, const f32* w) {
    f32 a = PSVECMag(w);
    if (a > lbl_80563F9C) {
        f32 x, s, y, z, c, k;
        f32 inv = lbl_80563F50 / a;
        x = inv * w[0];
        y = inv * w[1];
        z = inv * w[2];
        s = (f32)sin(a);
        c = (f32)cos(a);
        k = lbl_80563F50 - c;
        r->m[0] = c + k * (x * x);
        r->m[1] = y * (k * x) + s * z;
        r->m[2] = z * (k * x) - s * y;
        r->m[3] = y * (k * x) - s * z;
        r->m[4] = c + k * (y * y);
        r->m[5] = z * (k * y) + s * x;
        r->m[6] = z * (k * x) + s * y;
        r->m[7] = z * (k * y) - s * x;
        r->m[8] = c + k * (z * z);
    } else {
        r->m[1] = lbl_80563F48;
        r->m[0] = lbl_80563F50;
        r->m[2] = lbl_80563F48;
        r->m[3] = lbl_80563F48;
        r->m[4] = lbl_80563F50;
        r->m[5] = lbl_80563F48;
        r->m[6] = lbl_80563F48;
        r->m[7] = lbl_80563F48;
        r->m[8] = lbl_80563F50;
    }
}

// Applies the current view of camera cycle `ci` to camera `p`.
void fn_80273604(u8* p, int ci) {
    CamCycle* cyc = &lbl_8049A258[ci];
    CamView* v = &lbl_80346BA0[cyc->first + cyc->cur];
    switch (v->type) {
    case 34: {
        *(Words12*)(p + 0x58) = *(Words12*)(p + 0xC0);
        f32 w[3];
        w[0] = lbl_80563F4C * v->rot[0];
        w[1] = lbl_80563F4C * v->rot[1];
        w[2] = lbl_80563F4C * v->rot[2];
        f32 off[3];
        off[0] = v->off[0];
        off[1] = v->off[1];
        off[2] = v->off[2];
        Mat3 rot;
        CamRotFromVec(&rot, w);
        fn_801BFD34(p + 0x7C, off, p + 0xC0);
        Mat3Mul((Mat3*)(p + 0x58), &rot, (Mat3*)(p + 0xC0));
        F32(p, 0xBC) = lbl_80563F4C * v->fov;
        break;
    }
    case 0:
        fn_80273BD0(p);
        break;
    case 2:
        fn_80274268(p);
        break;
    case 4:
    case 5:
    case 7: {
        int t = fn_80273494(S32(p, 0x4));
        if (t == 6 || t == 0 || t == ci) {
            fn_802704D0(v->type, lbl_80346BA0[cyc->first + (cyc->count + cyc->cur - 1) % cyc->count].type, p);
        } else {
            CamCycle* o = &lbl_8049A258[t];
            fn_802704D0(v->type, lbl_80346BA0[o->first + o->cur].type, p);
        }
        break;
    }
    default:
        fn_802E30C8("camera_car: unexpected camera type %d", v->type);
        fn_802704D4(4, p);
        break;
    }
}

}  // extern "C"
CamRegion::~CamRegion(void) {}
extern "C" {

}  // extern "C"
