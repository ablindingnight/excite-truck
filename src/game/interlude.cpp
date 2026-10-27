// Interludes (0x801C2F58-0x801CB2D8): scripted race intros/outros from "intrlude.val" ("action_list"
// of timed events: camera moves, fades, sounds, car teleports, AI think orders, waits and text), plus the
// in-game interlude editor (menus driven by the debug keyboard, an input line, an event list that can be
// re-timed and reordered, and export to "intrlude.txt"). Every function is written; the player state at
// lbl_80437D88 is still addressed by offsets in places. Names are guesses apart from the strings.
#include "game/common.h"

struct IVec3 {
    f32 x;
    f32 y;
    f32 z;
};

// A ground-plane (x, z) vector.
struct V2 {
    V2(void) {}
    V2(f32 x_, f32 y_) : x(x_), y(y_) {}
    V2(const V2& o) : x(o.x), y(o.y) {}
    f32 x;
    f32 y;
};

static inline V2 XZ(const IVec3& v) { return V2(v.x, v.z); }

static inline V2 operator-(const V2& a, const V2& b) { return V2(a.x - b.x, a.y - b.y); }

struct IMtx {
    u32 w[12];
};

struct Mat3 {
    f32 m[9];
};

// One timed event: 0x30C bytes, kept sorted by (time, order).
struct Event {
    f32 time;   // offset 0x0: < -1 = deleted
    s32 order;  // offset 0x4: tie-break between events at the same time
    s32 type;   // offset 0x8
    u8 body[0x300];
};

struct SoundEvent {
    f32 time;
    s32 order;
    s32 type;
    s32 sample;   // offset 0xC: -1 = random
    s32 soundType;  // offset 0x10: speech, music
    u8 overlap;     // offset 0x14
    u8 imm;         // offset 0x15: immediate
};

struct TextEvent {
    f32 time;
    s32 order;
    s32 type;
    char title[0x100];  // offset 0xC
    char text[0x200];   // offset 0x10C
};

struct WaitEvent {
    f32 time;
    s32 order;
    s32 type;
    s32 kind;           // offset 0xC
    u32 mask;           // offset 0x10
    char who[0x100];    // offset 0x14
    char except[0x100]; // offset 0x114
    IVec3 pt;           // offset 0x214
};

struct ThinkEvent {
    f32 time;
    s32 order;
    s32 type;
    u32 mask;           // offset 0xC
    char who[0x100];    // offset 0x10
    char except[0x100]; // offset 0x110
    s32 kind;           // offset 0x210
    s32 param;          // offset 0x214
    f32 speed;          // offset 0x218
    f32 x;              // offset 0x21C
    f32 z;              // offset 0x220
};

struct TeleportEvent {
    f32 time;
    s32 order;
    s32 type;
    u32 mask;           // offset 0xC
    char who[0x100];    // offset 0x10
    char except[0x100]; // offset 0x110
    s32 where;          // offset 0x210
    f32 speed;          // offset 0x214
    V2 pt;           // offset 0x218: world point (x, z)
    f32 yaw;            // offset 0x220
    V2 offset;       // offset 0x224: second car, from pt
};

struct DebugEvent {
    f32 time;
    s32 order;
    s32 type;
    char msg[0x100];  // offset 0xC
};

struct ShowEvent {
    f32 time;
    s32 order;
    s32 type;
    char title[0x100];  // offset 0xC
    char text[0x200];   // offset 0x10C
};

struct CamEvent {
    f32 time;
    s32 order;
    s32 type;
    IVec3 origin;  // offset 0xC
    Mat3 rot;      // offset 0x18
    IVec3 v;       // offset 0x3C: per second, in the start frame
    IVec3 av;      // offset 0x48: per second (axis * angle)
    f32 fov;       // offset 0x54
    f32 end;       // offset 0x58
    u8 accel;      // offset 0x5C
    s32 target;    // offset 0x60: car the camera follows, -1 = none
    s32 camType;   // offset 0x64: world, start, car
    s32 interpType;  // offset 0x68: linear, hermite
    f32 interpTime;  // offset 0x6C
};

// An editor camera: rotation rows, then position.
struct CamPose {
    Mat3 rot;   // offset 0x0
    IVec3 pos;  // offset 0x24
};

// The camera move being played (the player state starts with it).
struct CamPlay {
    f32 start;       // offset 0x0
    f32 end;         // offset 0x4
    IVec3 origin;    // offset 0x8
    Mat3 rot;        // offset 0x14
    IVec3 v;         // offset 0x38
    IVec3 av;        // offset 0x44
    f32 fov;         // offset 0x50
    u8 accel;        // offset 0x54
    s32 target;      // offset 0x58
    s32 camType;     // offset 0x5C
    s32 interpType;  // offset 0x60
    f32 interpTime;  // offset 0x64
};

// The on-screen message an event shows.
struct Message {
    const char* title;  // offset 0x0
    const char* text;   // offset 0x4
    f32 time;           // offset 0x8: < 0 = none
};

struct MenuItem {
    const char* label;     // offset 0x0
    void (*fn)(void);      // offset 0x4
    s32 next;              // offset 0x8: menu to open, 0x22 = back
    bool* check;           // offset 0xC: shown as a check box when set
};

struct Menu {
    MenuItem* items;    // offset 0x0
    const char* title;  // offset 0x4
    s32 count;          // offset 0x8
    s32 padC;
};

struct TrackNode {
    u8 pad0[0x14];
    TrackNode* next;  // offset 0x14
    u8 pad18[0x1C - 0x18];
    f32 height;       // offset 0x1C
    u8 pad20[0x34 - 0x20];
    u8 skip;          // offset 0x34
};

struct TrackList {
    u8 pad0[0x20];
    TrackNode* first;  // offset 0x20
};

struct Track {
    u8 pad0[0x2C];
    TrackList* list;  // offset 0x2C
};

struct RaceSlot {
    s32 event;  // offset 0x0
    u32 type;   // offset 0x4
};

class ValueNodeV {
public:
    virtual const char* GetName(void);
    virtual int Has(const char* key);
    virtual int GetPair(int index, const char** key, const char** value);
    virtual const char* GetValue(const char* key);
};

struct EventTable {
    u8 pad0[0x20];
    ValueNodeV* node;  // offset 0x20
};

EventTable* GetEvent(int i);
const char* GetEventTypeName(u32 type);
void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);
void* operator new[](size_t size, const char* name);
void operator delete[](void* p);

extern "C" {
void* memset(void* dst, int c, size_t n);
char* strcpy(char* dst, const char* src);
char* strncpy(char* dst, const char* src, size_t n);
size_t strlen(const char* s);
int strcmp(const char* a, const char* b);
void fn_802E30C8(const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);
double atan2(double y, double x);
int sprintf(char* dst, const char* fmt, ...);
double atof(const char* s);
int atoi(const char* s);
void qsort(void* base, size_t n, size_t size, int (*cmp)(const void*, const void*));
double cos(double x);
double sin(double x);
double acos(double x);
void* fn_801E8718(int view);
void fn_802C8638(void* cam);
void fn_801EA478(int cam);
void fn_801E9DC4(s16* hint);
f32 fn_801E9DDC(s16* hint, f32 x, f32 z);
void fn_801E3FC8(const IVec3* p, f32 size, u32 color);
double cos(double x);
int fn_8012C33C(const char* name);
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* t);
f32 PSVECMag(const ValVec3* v);
void fn_80144CD0(IVec3* out, const IVec3* p, const f32* m);
f32 PSVECSquareDistance(const void* a, const void* b);
void fn_8000FC8C(const char* s);
void fn_8000FDF0(const char* s);
void fn_80015A20(Mat3* out, const IVec3* euler);
void fn_80128678(void);
void fn_80128684(void);
void fn_8012BEF0(const char* name);
void fn_8012BF24(const char* name);
int fn_8013181C(const char* txt, const char* val, const char* qrs);
void fn_80176610(void);
void fn_80176780(int flags, int a);
u32 fn_801769A8(void);
RaceSlot* fn_801769CC(void);
void fn_801769E0(void);
void fn_801769EC(void);
Track* fn_80178C9C(void);
u8* fn_8017960C(int car);
int fn_80179630(int a);
void fn_801B7C8C(void* a, void* b, const f32* dir);
void fn_801B8054(Mat3* m, Track* t, f32 a, f32 h);
void fn_8020C8F8(void);
void fn_8020C8FC(void);
void fn_8023895C(void);
void fn_802389E4(void);
int fn_80238A20(void);
void fn_80238A74(int x, int y, const char* s, int color);
void fn_80238B44(int x0, int y0, int x1, int y1, u32 color);  // filled box
int fn_802E367C(const char* name);
void fn_802E3734(int* fd);
void fn_802E3AD0(int fd, const char* fmt, ...);
}

extern f32 lbl_8055E3C8;    // time
extern f32 lbl_80558DC4[];  // FLT_EPSILON
extern f32 lbl_8055F47C;
extern IMtx lbl_80499050;   // current camera

extern UnkStatic7F lbl_8055E7DC;  // box colour
extern bool lbl_8055E7E0;     // hidden
extern u8 lbl_8055E7E1;
extern u8 lbl_8055E7E2;     // active
extern u8 lbl_8055E7E3;
extern f32 lbl_8055E7E4;    // start offset
extern u8 lbl_8055E7E8;
extern s8 lbl_8055E818;
extern s8 lbl_8055E819;
extern s32 lbl_8055E7EC;  // fade frame
extern u8 lbl_8055E7E9;
extern s32 lbl_8055E7F0;    // events
extern s32 lbl_8055E7F4;
extern Event* lbl_8055E7F8; // the event list
extern u8 lbl_8055E7FC;
extern s32 lbl_8055E800;
extern s32 lbl_8055E804;    // cars
extern s32 lbl_8055E808;
extern bool lbl_8055E80C;
extern u8 lbl_8055E80D;     // global event time on
extern f32 lbl_8055E810;    // global event time
extern s32 lbl_8055E814;
extern s32 lbl_8055E81C;    // current menu
extern s32 lbl_8055E820;    // current item
extern s32 lbl_8055E824;    // editor key slot
extern s32 lbl_8055E828;    // selected event
extern s32 lbl_8055E82C;    // first listed event
extern f32 lbl_8055E830;  // editor camera transition time
extern f32 lbl_8055E834;
extern u8 lbl_8055E838;     // editing
extern u8 lbl_8055E839;     // listing
extern u8 lbl_8055E83A;
extern u8 lbl_8055E83B;
extern u8 lbl_8055E83C;
extern u8 lbl_8055E83D;
extern bool lbl_8055E83E;
extern bool lbl_8055E83F;
extern bool lbl_8055E840;
extern bool lbl_8055E841;
extern s32 lbl_8055E844;
extern s32 lbl_8055E848;
extern f32 lbl_8055E84C;
extern s32 lbl_8055E850;
extern s32 lbl_8055E854;
extern f32 lbl_8055E858;
extern f32 lbl_8055E85C;
extern f32 lbl_8055E860;
extern s32 lbl_8055E864;
extern void (*lbl_8055E868)(void);
extern Menu* lbl_8055E86C;
extern s32 lbl_8055E870;
extern f32 lbl_8055A958;    // duration

extern u8 lbl_80437D88[];        // player state; the editor statics follow it
extern Message lbl_80437E24;
extern char lbl_80437FC0[0x130];  // status line
extern char lbl_804380F0[0x100];  // input line
extern IMtx lbl_804381F0;
extern IMtx lbl_80438220;
extern IVec3 lbl_80438250;
extern IVec3 lbl_80438268;        // cursor
extern char lbl_80438280[0x100];  // wait: who
extern char lbl_80438380[0x100];  // wait: except
extern IVec3 lbl_8043848C;
extern IVec3 lbl_80438498;
extern char lbl_804384A8[0x100];  // think: who
extern char lbl_804385A8[0x100];  // think: except
extern u8 lbl_80337474[];         // debug key table

static const char* const lbl_80562950 = "debug\0camera\0effect\0sound\0teleport\0think\0wait\0text\0\0\0\0";
static const char* const lbl_80562954 = "world_pt\0entry\0start";
static const char* const lbl_80562958 = "park\0line\0target\0follow\0line_follow\0donut";
static const char* const lbl_8056295C = "cross_pt";
static const char* const lbl_80562960 = "speech\0music";
static const char* const lbl_80562964 = "world\0start\0car\0\0\0\0";
static const char* const lbl_80562968 = "linear\0hermite\0\0\0\0";

// A camera frame: axes (rows) and position, as in camframe.
struct IFrame {
    IVec3 axes[3];  // offset 0x0
    IVec3 pos;      // offset 0x24
};

struct IQuat {
    f32 x, y, z, w;
};

extern "C" double sqrt(double x);
extern "C" void fn_8012F57C(IQuat* out, IQuat* a, IQuat* b, f32 t);  // slerp

static inline void FrameToQuatAxis(f32 (*m)[3], IQuat* q) {
    f32* v = &q->x;
    int i = 0;
    if (m[1][1] > m[0][0]) {
        i = 1;
    }
    if (m[2][2] > m[i][i]) {
        i = 2;
    }
    int j = (i + 1) % 3;
    int k = (j + 1) % 3;
    f32 s = sqrt(m[i][i] - (m[j][j] + m[k][k]) + 1.0f);
    v[i] = 0.5f * s;
    s = 0.5f / s;
    q->w = (m[j][k] - m[k][j]) * s;
    v[j] = (m[i][j] + m[j][i]) * s;
    v[k] = (m[i][k] + m[k][i]) * s;
}

static inline void RotToQuat(const Mat3* r, IQuat* q) {
    const f32(*m)[3] = (const f32(*)[3])r;
    f32 tr = r->m[0] + r->m[4] + r->m[8];
    if (tr > 0.0f) {
        f32 s = sqrt(tr + 1.0f);
        q->w = 0.5f * s;
        s = 0.5f / s;
        q->x = (m[1][2] - m[2][1]) * s;
        q->y = (m[2][0] - m[0][2]) * s;
        q->z = (m[0][1] - m[1][0]) * s;
    } else {
        FrameToQuatAxis((f32(*)[3])m, q);
    }
}

static inline void FrameToQuat(const IFrame* f, IQuat* q) { RotToQuat((const Mat3*)f, q); }

// The rotation of a unit quaternion.
static inline void QuatToFrame(IFrame* f, const IQuat* q) {
    f32(*m)[3] = (f32(*)[3])f;
    f32 x2 = q->x + q->x;
    f32 y2 = q->y + q->y;
    f32 z2 = q->z + q->z;
    f32 xx = q->x * x2;
    f32 xy = q->x * y2;
    f32 xz = q->x * z2;
    f32 yy = q->y * y2;
    f32 yz = q->y * z2;
    f32 zz = q->z * z2;
    f32 wx = q->w * x2;
    f32 wy = q->w * y2;
    f32 wz = q->w * z2;
    m[0][0] = 1.0f - (yy + zz);
    m[1][0] = xy - wz;
    m[2][0] = xz + wy;
    m[0][1] = xy + wz;
    m[1][1] = 1.0f - (xx + zz);
    m[2][1] = yz - wx;
    m[0][2] = xz - wy;
    m[1][2] = yz + wx;
    m[2][2] = 1.0f - (xx + yy);
}

extern "C" {

int fn_801C3B7C(const void* a, const void* b);
f32 fn_801C3450(f32 t);
u32 fn_801C3524(const char* who);
void fn_801C3D88(const char* name);
void fn_801C7254(Mat3* m);
void fn_801C855C(void);
void fn_801C957C(int on);
void fn_801C9920(const Event* e, int fd);
const char* fn_801C98F4(const char* s, int n);
void fn_801C94B8(int x, int y, const char* s, int color);
void fn_801CA1C4(const Event* e, int x, int y, int h);

f32 fn_801C2F58(int n, f32 x) {
    f32 r = 1.0f;
    for (int i = 0; i < n; i++) {
        r *= x;
    }
    return r;
}

// Blends two camera frames: orientation through quaternion slerp, position linearly.
void fn_801C2FF4(IFrame* out, const IFrame* a, const IFrame* b, f32 t) {
    IQuat qa;
    IQuat qb;
    IQuat q;
    IVec3 pos;
    FrameToQuat(a, &qa);
    FrameToQuat(b, &qb);
    fn_8012F57C(&q, &qa, &qb, t);
    QuatToFrame(out, &q);
    pos.y = a->pos.y + t * (b->pos.y - a->pos.y);
    pos.z = a->pos.z + t * (b->pos.z - a->pos.z);
    pos.x = a->pos.x + t * (b->pos.x - a->pos.x);
    out->pos = pos;
}

// Rounds to hundredths.
f32 fn_801C3450(f32 t) {
    f32 d = fn_801C2F58(2, 10.0f);
    return (f32)(int)(t * fn_801C2F58(2, 10.0f)) / d;
}

int fn_801C34CC(f32 a, f32 b) { return (f32)__fabs(a - b) <= lbl_80558DC4[0]; }

f32 fn_801C34F4(f32 x) { return 180.0f * x / 3.1415927f; }

int fn_801C3508(u32 mask, int i) { return (mask & (1 << i)) != 0; }

// A "who" name to a car mask: nobody, all, last, first, player, the eve leader, even or odd lineup
// slots, or a lineup index (counted from the end when negative).
u32 fn_801C3524(const char* who) {
    if (strcmp(who, "nobody") == 0) {
        return 0;
    }
    if (strcmp(who, "all") == 0) {
        return 0xFFFFFFFF;
    }
    if (strcmp(who, "last") == 0) {
        return 1 << (lbl_8055E804 - 1);
    }
    if (strcmp(who, "0") == 0 || strcmp(who, "first") == 0) {
        return 1;
    }
    if (strcmp(who, "player") == 0) {
        return 1 << fn_80179630(0);
    }
    if (strcmp(who, "eve_leader") == 0) {
        return 1 << lbl_8055E808;
    }
    if (strcmp(who, "even") == 0) {
        u32 mask = 0;
        for (int i = 0; i < lbl_8055E804; i += 2) {
            mask |= 1 << i;
        }
        return mask;
    }
    if (strcmp(who, "odd") == 0) {
        u32 mask = 0;
        for (int i = 1; i < lbl_8055E804; i += 2) {
            mask |= 1 << i;
        }
        return mask;
    }
    int n = atoi(who);
    if (n > 0) {
        return 1 << n;
    }
    if (n < 0) {
        return 1 << (lbl_8055E804 - n);
    }
    fn_802E30C8("interlude: bad who %s", who);
    return 0;
}

u32 fn_801C3954(const char* list) {
    char buf[256];
    strcpy(buf, list);
    char* start = buf;
    char* p = start;
    u32 mask = 0;
    for (;;) {
        int done = 0;
        while (*p != ' ' && *p != '\0') {
            p++;
        }
        if (*p == '\0') {
            done = 1;
        }
        *p = '\0';
        mask |= fn_801C3524(start);
        if (done) {
            break;
        }
        start = ++p;
    }
    return mask;
}

int fn_801C3A00(u32 mask) {
    for (int i = 0; i < lbl_8055E804; i++) {
        if (mask == (1 << i)) {
            return i;
        }
    }
    return -1;
}

void fn_801C3A3C(Mat3* m, f32 h, f32 yaw) {
    Track* t = fn_80178C9C();
    TrackNode* n;
    for (n = t->list->first; n && n->skip; n = n->next) {
    }
    f32 y = n ? n->height : 0.0f;
    fn_801B8054(m, t, 0.0f, y + h);
    IVec3 e;
    e.x = 0.0f;
    e.y = yaw;
    e.z = 0.0f;
    Mat3 r;
    fn_80015A20(&r, &e);
    Mat3Mul(m, &r, m);
}

void fn_801C3AF8(void* a, void* b, f32 ang) {
    f32 dir[2];
    f32 c = cos(ang);
    dir[0] = sin(ang);
    dir[1] = c;
    fn_801B7C8C(a, b, dir);
}

f32 fn_801C3B6C(void) {
    f32 t = lbl_8055E3C8;
    return lbl_8055E7E4 + t;
}

int fn_801C3B7C(const void* pa, const void* pb) {
    const Event* a = (const Event*)pa;
    const Event* b = (const Event*)pb;
    if (b->time < -1.0f) {
        return -1;
    }
    if (a->time < -1.0f) {
        return 1;
    }
    if (fn_801C34CC(a->time, b->time)) {
        return a->order - b->order;
    }
    if (a->time < b->time) {
        return -1;
    }
    return 1;
}

void fn_801C3C14(int i) {
    memset(&lbl_8055E7F8[i], 0, sizeof(Event));
    lbl_8055E7F8[i].time = -999.0f;
    qsort(lbl_8055E7F8, lbl_8055E7F0, sizeof(Event), fn_801C3B7C);
    lbl_8055E7F0--;
}

Event* fn_801C3C80(void) {
    Event* e = &lbl_8055E7F8[lbl_8055E7F0++];
    if (lbl_8055E80D) {
        e->time = lbl_8055E810;
    } else {
        e->time = fn_801C3450(fn_801C3B6C());
    }
    int order = 0;
    for (int i = 0; i < lbl_8055E7F0 - 1; i++) {
        if (fn_801C34CC(e->time, lbl_8055E7F8[i].time) && lbl_8055E7F8[i].order > order) {
            order = lbl_8055E7F8[i].order;
        }
    }
    e->order = order + 1;
    return e;
}

ValueNodeV* fn_801C3D5C(void) { return GetEvent(fn_801769CC()->event)->node; }

// An enum key, or def when the node does not have it.
static inline int GetEnumOr(ValueNode* n, const char* key, const char* names, int def) {
    return n->Has(key) ? n->GetEnum(key, names) : def;
}

static inline ValVec3 ZeroVec3(void) {
    ValVec3 v;
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = 0.0f;
    return v;
}

static inline ValPoint ZeroPoint(void) {
    ValPoint p;
    p.x = 0.0f;
    p.y = 0.0f;
    return p;
}

// The rotation of angle |v| about v.
static inline void AxisAngleToRot(Mat3* m, const ValVec3& v) {
    f32 ang = PSVECMag(&v);
    if (ang > 0.0001f) {
        f32 inv = 1.0f / ang;
        f32 x = inv * v.x;
        f32 y = inv * v.y;
        f32 z = inv * v.z;
        f32 s = sin(ang);
        f32 c = cos(ang);
        f32 t = 1.0f - c;
        m->m[0] = c + t * (x * x);
        m->m[1] = y * (t * x) + s * z;
        m->m[2] = z * (t * x) - s * y;
        m->m[3] = y * (t * x) - s * z;
        m->m[4] = c + t * (y * y);
        m->m[5] = z * (t * y) + s * x;
        m->m[6] = z * (t * x) + s * y;
        m->m[7] = z * (t * y) - s * x;
        m->m[8] = c + t * (z * z);
    } else {
        m->m[0] = 1.0f;
        m->m[1] = 0.0f;
        m->m[2] = 0.0f;
        m->m[3] = 0.0f;
        m->m[4] = 1.0f;
        m->m[5] = 0.0f;
        m->m[6] = 0.0f;
        m->m[7] = 0.0f;
        m->m[8] = 1.0f;
    }
}

// Loads the interlude for this event (or the override name) from the .val file: its duration and its
// "action" tables.
void fn_801C3D88(const char* name) {
    memset(lbl_8055E7F8, 0, 64 * sizeof(Event));
    for (int i = 0; i < 64; i++) {
        lbl_8055E7F8[i].time = -999.0f;
    }
    f32 last = 0.0f;
    lbl_8055E808 = 0;
    lbl_8055E7F0 = 0;
    if (!fn_8012C33C(name)) {
        fn_802E30C8("interlude: can't find %s", name);
        fn_802E317C("Interlude: Couldn't find interlude file %s!!!", name);
        return;
    }
    ValueTable* vt = fn_80131808(name);
    const char* table = lbl_8055E814 ? (const char*)lbl_8055E814
                        : fn_801C3D5C()->Has("interlude_name") ? fn_801C3D5C()->GetValue("interlude_name")
                                                               : GetEventTypeName(fn_801769CC()->type);
    if (!vt->HasChild(table)) {
        fn_802E30C8("interlude: can't find %s", table);
        table = "default";
    }
    if (!vt->HasChild(table)) {
        fn_802E30C8("interlude: can't find %s", table);
        fn_80131818(vt);
        fn_802E317C("Interlude: Couldn't find interlude %s in val file %s!!!", table, name);
        return;
    }
    ValueNode* root = vt->FindChild(table);
    lbl_8055A958 = root->GetFloat("duration");
    int order = 0;
    for (int i = 0;; i++) {
        ValueNode* n = root->GetChildAt(i);
        if (!n) {
            break;
        }
        if (strcmp(n->GetName(), "action") != 0) {
            continue;
        }
        if (lbl_8055E7F0 >= 64) {
            fn_802E317C("intrlude: too many events!");
        }
        Event* e = &lbl_8055E7F8[lbl_8055E7F0++];
        int type = n->GetEnum("type", lbl_80562950);
        e->type = type;
        e->time = fn_801C3450(n->GetFloat("t", last));
        e->time = fn_801C3450(e->time + n->GetFloat("dt", 0.0f));
        if (!fn_801C34CC(e->time, last)) {
            order = 0;
        }
        order = e->order = n->GetInt("order", order + 1);
        last = e->time;
        switch (type) {
        case 0:
            strncpy(((DebugEvent*)e)->msg, n->GetValue("msg"), 0x17);
            break;
        case 1: {
            CamEvent* c = (CamEvent*)e;
            c->fov = 0.017453292f * n->GetFloat("fov", 90.0f);
            const ValVec3& o = n->GetVec3("origin");
            c->origin.x = o.x;
            c->origin.y = o.y;
            c->origin.z = o.z;
            AxisAngleToRot(&c->rot, n->GetVec3("dir"));
            const ValVec3& v = n->GetVec3Or("v", ZeroVec3());
            c->v.x = v.x;
            c->v.y = v.y;
            c->v.z = v.z;
            const ValVec3& av = n->GetVec3Or("av", ZeroVec3());
            c->av.x = av.x;
            c->av.y = av.y;
            c->av.z = av.z;
            c->end = fn_801C3450(c->time + n->GetFloat("duration", 9999.0f));
            c->accel = n->GetBool("accel", 1);
            c->target = fn_801C3A00(fn_801C3954(n->GetString("who", "nobody")));
            c->camType = GetEnumOr(n, "camera_type", lbl_80562964, 0);
            c->interpType = GetEnumOr(n, "interp_type", lbl_80562968, 1);
            c->interpTime = n->GetFloat("interp_time", 0.0f);
            break;
        }
        case 3: {
            SoundEvent* s = (SoundEvent*)e;
            s->sample = n->GetInt("sample", -1);
            s->soundType = GetEnumOr(n, "sound_type", lbl_80562960, 0);
            s->overlap = n->GetBool("overlap", 1);
            s->imm = n->GetBool("immediate", 0);
            break;
        }
        case 4: {
            TeleportEvent* t = (TeleportEvent*)e;
            t->speed = n->GetFloat("speed", 0.0f);
            strcpy(t->who, n->GetString("who", "all"));
            t->mask = fn_801C3954(t->who);
            strcpy(t->except, n->GetString("except", "nobody"));
            t->mask &= ~fn_801C3954(t->except);
            t->where = n->GetEnum("where", lbl_80562954);
            switch (t->where) {
            case 1:
                t->pt.x = n->GetFloat("offset", 0.0f);
                t->pt.y = n->GetFloat("spacing", 0.0f);
                t->yaw = n->GetFloat("yaw", 0.0f);
                break;
            case 0: {
                const ValPoint& p = n->GetPoint("pt");
                t->pt.x = p.x;
                t->pt.y = p.y;
                t->yaw = n->GetFloat("yaw");
                const ValPoint& d = n->GetPointOr("offset", ZeroPoint());
                t->offset.x = d.x;
                t->offset.y = d.y;
                break;
            }
            case 2: {
                t->yaw = n->GetFloat("yaw", 0.0f);
                const ValPoint& d = n->GetPointOr("offset", ZeroPoint());
                t->pt.x = d.x;
                t->pt.y = d.y;
                break;
            }
            default:
                fn_802E317C("interlude: unhandled teleport where type %d", t->where);
                break;
            }
            break;
        }
        case 5: {
            ThinkEvent* t = (ThinkEvent*)e;
            strcpy(t->who, n->GetString("who", "all"));
            t->mask = fn_801C3954(t->who);
            strcpy(t->except, n->GetString("except", "nobody"));
            t->mask &= ~fn_801C3954(t->except);
            t->kind = n->GetEnum("how", lbl_80562958);
            t->speed = n->GetFloat("speed", 0.0f);
            t->x = n->GetFloat("lat", 0.0f);
            t->z = n->GetFloat("lon", 0.0f);
            t->param = n->GetInt("param", 0);
            if (t->kind == 2) {
                const ValPoint& p = n->GetPoint("pt");
                t->x = p.x;
                t->z = p.y;
            }
            break;
        }
        case 6: {
            WaitEvent* w = (WaitEvent*)e;
            w->kind = n->GetEnum("what", lbl_8056295C);
            strcpy(w->who, n->GetString("who", "all"));
            w->mask = fn_801C3954(w->who);
            strcpy(w->except, n->GetString("except", "nobody"));
            w->mask &= ~fn_801C3954(w->except);
            if (w->kind != 0) {
                fn_802E317C("interlude: unhandled wait type %d", w->kind);
            } else {
                const ValVec3& p = n->GetVec3("pt");
                w->pt.x = p.x;
                w->pt.y = p.y;
                w->pt.z = p.z;
            }
            break;
        }
        case 7: {
            TextEvent* t = (TextEvent*)e;
            strncpy(t->text, n->GetValue("msg"), 0x1FF);
            strncpy(t->title, n->GetString("title", ""), 0xFF);
            break;
        }
        default:
            fn_802E317C("interlude: unhandled event type %d", type);
            break;
        }
    }
    fn_802E30C8("intrlude: %d events", lbl_8055E7F0);
    fn_80131818(vt);
}

void fn_801C4AD0(void) {
    lbl_8055E7E9 = 0;
    lbl_8055E7E8 = 0;
    lbl_8055E7E2 = 0;
    lbl_8055E7E3 = 0;
    lbl_8055E7F0 = 0;
    lbl_8055E804 = 0;
    lbl_8055E808 = 0;
    lbl_8055E7E4 = 0.0f;
    lbl_80437E24.time = -999.0f;
    lbl_80437E24.title = 0;
    void fn_801C9834(void);
    fn_801C9834();
}

void fn_801C4B10(void) {
    for (lbl_8055E804 = 0; fn_8017960C(lbl_8055E804); lbl_8055E804++) {
    }
    lbl_8055E7F8 = new ("action_list") Event[64];
    fn_801C3D88("intrlude.val");
}

void fn_801C4B74(void) {
    void fn_801C98F0(void);
    fn_801C98F0();
    if (lbl_8055E7F8) {
        delete[] lbl_8055E7F8;
        lbl_8055E7F8 = 0;
    }
}

void fn_801C4BAC(void) {
    lbl_8055E800 = 0;
    lbl_8055E7FC = 1;
    lbl_8055E7E8 = 0;
    lbl_8055E7F4 = 0;
    lbl_8055E7E3 = 0;
    lbl_8055E7E4 = 0.0f;
    u8* s = lbl_80437D88;
    memset(s, 0, 0x9C);
    f32* m = (f32*)(s + 0x14);
    m[0] = 1.0f;
    m[1] = 0.0f;
    m[2] = 0.0f;
    m[3] = 0.0f;
    m[4] = 1.0f;
    m[5] = 0.0f;
    m[6] = 0.0f;
    m[7] = 0.0f;
    m[8] = 1.0f;
    *(f32*)(s + 0x50) = 1.5707964f;
    m = (f32*)(s + 0x68);
    m[0] = 1.0f;
    m[1] = 0.0f;
    m[2] = 0.0f;
    m[3] = 0.0f;
    m[4] = 1.0f;
    m[5] = 0.0f;
    m[6] = 0.0f;
    m[7] = 0.0f;
    m[8] = 1.0f;
    m[9] = 0.0f;
    m[10] = 0.0f;
    m[11] = 0.0f;
    *(f32*)(s + 0x98) = 1.5707964f;
    Message* msg = (Message*)(s + 0x9C);
    msg->time = -999.0f;
    msg->title = 0;
    if (lbl_8055E7F0 == 0) {
        lbl_8055E7E2 = 0;
    } else {
        if (lbl_8055E7E9) {
            fn_8020C8FC();
            fn_8020C8F8();
            lbl_8055E7E9 = 0;
        }
        if ((lbl_8055E7E2 = (fn_801769A8() >> 14) & 1) != 0) {
            memset(s + 0xA8, 0, 0xA0);
            fn_8017960C(fn_80179630(0));
        }
    }
}

// Where a teleported car is put: its frame, velocity and line node (-1 = find one).
struct CarPlace {
    CarPlace(void) : node(-1) {
        frame.rot.m[0] = 1.0f;
        frame.rot.m[1] = 0.0f;
        frame.rot.m[2] = 0.0f;
        frame.rot.m[3] = 0.0f;
        frame.rot.m[4] = 1.0f;
        frame.rot.m[5] = 0.0f;
        frame.rot.m[6] = 0.0f;
        frame.rot.m[7] = 0.0f;
        frame.rot.m[8] = 1.0f;
        frame.pos.x = 0.0f;
        frame.pos.y = 0.0f;
        frame.pos.z = 0.0f;
    }
    CamPose frame;  // offset 0x0
    IVec3 vel;      // offset 0x30
    f32 unk3C;
    s16 node;       // offset 0x40
};

// A car's start pose (the place plus more).
struct CarPose {
    CarPlace place;
    u8 pad44[0xC];
};


// The parts of a car body the AI orders read.
struct ICar {
    u8 pad0[0x3C];
    IVec3 side;  // offset 0x3C
    u8 pad48[0xC];
    IVec3 fwd;   // offset 0x54
    IVec3 pos;   // offset 0x60
    s16 id;       // offset 0x6C
    u8 pad6E[2];
    CamPose frame;  // offset 0x70
    u8 padA0[0x118 - 0xA0];
    IVec3 heading;  // offset 0x118
    u8 pad124[0x128 - 0x124];
    f32 yawRate;  // offset 0x128
    u8 pad12C[0x1F0 - 0x12C];
    s32 player;   // offset 0x1F0
    u8 pad1F4[0x274 - 0x1F4];
    CarPose pose;  // offset 0x274
    u8 pad2C4[0x25D4 - 0x2C4];
    f32 speed;  // offset 0x25D4
};

// One car's standing AI order, set by think events: 0 = none, 1 = follow the line, 2 = drive to a point,
// 3 = follow a car, 4 = follow a car along the line, 5 = drive a donut.
struct AIOrder {
    s32 kind;   // offset 0x0
    s32 param;  // offset 0x4: cars ahead to follow
    f32 speed;  // offset 0x8
    f32 x;      // offset 0xC
    f32 z;      // offset 0x10
};

// Motion targets for the gas/brake controller (catchup).
struct IMotion {
    f32 p1, v1, a1;
    f32 p2, v2, a2;
};

class LineFollower {
public:
    void Update(V2* p);
};

class IRacer {
public:
    virtual void v00(void);
    virtual void v04(void);
    virtual void v08(void);
    virtual void v0C(void);
    virtual LineFollower* Follower(void);  // 0x18
};

extern AIOrder lbl_80437E30[20];

extern "C" {
IRacer* fn_801B2ED0(int i);
void fn_8018ADAC(ICar* car, f32 v);  // steer
void fn_8018ADB4(ICar* car, f32 v);  // gas
void fn_8018ADBC(ICar* car, f32 v);  // brake
void fn_8018ADC4(ICar* car, f32 v);
void fn_8018ADCC(ICar* car, f32 v);
void fn_8018ADEC(ICar* car);
void fn_80135B9C(LineFollower* f, f32 d, V2* outPos, V2* outDir, f32* outSpeed, void* outIl);
f32 fn_80139030(IMotion* m);
f32 PSVECDotProduct(const IVec3* a, const IVec3* b);
}

static inline f32 Min(f32 a, f32 b) { return a < b ? a : b; }
static inline f32 Max(f32 a, f32 b) { return a > b ? a : b; }
static inline f32 Clamp(f32 v, f32 lo, f32 hi) { return Max(Min(hi, v), lo); }
static inline f32 ClampSigned(f32 v) { return Clamp(v, -1.0f, 1.0f); }
static inline f32 Clamp01(f32 v) { return Clamp(v, 0.0f, 1.0f); }

// The car object behind fn_8017960C, for its virtual placement.
class CarObj {
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
    virtual void Place(const CarPlace* p);  // 0x54
};

struct RaceLen {
    u8 pad0[4];
    f32 len;  // offset 0x4
};

extern "C" {
void fn_80176A00(int on);
int fn_801E8838(void);
int fn_80202B00(void);
void fn_80179758(const char* fmt, ...);
s32 fn_80179318(void);
void fn_80176858(void);
void fn_80129360(u32 sample, int a);
void fn_8020C8F4(u32 sample);
int fn_802794AC(s32 player, int a);
void fn_801B8100(CarPose* pose, int car, s32 player);
f32 fn_80139048(void);
RaceLen* fn_80137C54(int i);
void fn_801B8048(CarPlace* p, LineFollower* line, f32 lat, f32 along);
void fn_801B8E28(int car);
}

// The interlude player, once per frame: runs every event whose time has come, in order. A wait (cars to
// cross a point) or a fade still running holds the queue and pauses the clock.
void fn_801C4CE8(void) {
    if (lbl_8055E7E1) {
        fn_801769E0();
        lbl_8055E7E1 = 0;
    }
    if (!lbl_8055E7E2) {
        lbl_8055E7E3 = 0;
        return;
    }
    f32 t = fn_801C3B6C();
    if (t > lbl_8055A958) {
        if (!lbl_8055E7E3) {
            lbl_8055E7E3 = 1;
            fn_802E30C8("====== INTERLUDE: DONE at %1.1f! =======", t);
            fn_80176780(0x8102, 0);
        }
        return;
    }
    lbl_8055E7E3 = 0;
    fn_80176A00(1);
    if (lbl_8055E7E8) {
        lbl_8055E7E8 = fn_801E8838() != 0;
    }
    int blocked = 0;
    if (lbl_8055E7FC && lbl_8055E800 > 4 && !fn_80202B00()) {
        fn_802E30C8("interlude: reset completed");
        lbl_8055E7FC = 0;
    }
    if (lbl_8055E7FC) {
        lbl_8055E7E4 = -lbl_8055E3C8;
    }
    u8* s = lbl_80437D88;
    Message* msg = (Message*)(s + 0x9C);
    while (!blocked && lbl_8055E7F4 < lbl_8055E7F0) {
        Event* e = &lbl_8055E7F8[lbl_8055E7F4];
        if (!(t >= e->time)) {
            break;
        }
        lbl_8055E7F4++;
        switch (e->type) {
        case 6: {
            WaitEvent* w = (WaitEvent*)e;
            switch (w->kind) {
            case 0:
                blocked = 1;
                for (int i = 0; i < lbl_8055E804; i++) {
                    if (fn_801C3508(w->mask, i)) {
                        ICar* car = (ICar*)fn_8017960C(i);
                        if (car) {
                            IVec3 d;
                            d.x = car->frame.pos.x - w->pt.x;
                            d.y = car->frame.pos.y - w->pt.y;
                            d.z = car->frame.pos.z - w->pt.z;
                            if (PSVECDotProduct(&d, &car->heading) > 0.1f) {
                                blocked = 0;
                            }
                        }
                    }
                }
                break;
            default:
                fn_802E317C("intrlude: unhandled wait type %d", w->kind);
                break;
            }
            break;
        }
        case 0:
            fn_802E30C8("debug_event: %s", ((DebugEvent*)e)->msg);
            fn_80179758("%s", ((DebugEvent*)e)->msg);
            break;
        case 1: {
            CamEvent* c = (CamEvent*)e;
            CamPlay* p = (CamPlay*)s;
            p->start = c->time;
            p->end = c->end;
            p->origin.x = c->origin.x;
            p->origin.y = c->origin.y;
            p->origin.z = c->origin.z;
            p->rot = c->rot;
            p->v.x = c->v.x;
            p->v.y = c->v.y;
            p->v.z = c->v.z;
            p->av.x = c->av.x;
            p->av.y = c->av.y;
            p->av.z = c->av.z;
            p->fov = c->fov;
            p->accel = c->accel;
            p->target = c->target;
            p->camType = c->camType;
            p->interpType = c->interpType;
            p->interpTime = c->interpTime;
            break;
        }
        case 2:
            blocked = 1;
            if (!lbl_8055E7E8) {
                lbl_8055E7E8 = 1;
                lbl_8055E7EC = fn_80179318();
                fn_80176858();
            } else if (lbl_8055E7EC != fn_80179318()) {
                blocked = 0;
            }
            break;
        case 3: {
            SoundEvent* snd = (SoundEvent*)e;
            switch (snd->soundType) {
            case 0:
                fn_80129360(snd->sample, 0);
                break;
            case 1:
                fn_802E30C8("interlude: song %u (I:%d, O:%d)", snd->sample, snd->imm, snd->overlap);
                if (snd->imm) {
                    fn_8020C8FC();
                }
                if (snd->sample == 0xFFFFFFFF) {
                    fn_8020C8F8();
                } else {
                    fn_8020C8F4(snd->sample);
                }
                if (!snd->overlap) {
                    lbl_8055E7E9 = 1;
                } else {
                    lbl_8055E7E9 = 0;
                }
                break;
            }
            break;
        }
        case 4: {
            TeleportEvent* tp = (TeleportEvent*)e;
            int k = 0;
            for (int i = 0; i < lbl_8055E804; i++) {
                if (!fn_801C3508(tp->mask, i) || !fn_802794AC(((ICar*)fn_8017960C(i))->player, 0)) {
                    continue;
                }
                ICar* car = (ICar*)fn_8017960C(i);
                if (!car) {
                    break;
                }
                CarPlace place;
                switch (tp->where) {
                case 1:
                    fn_801C3A3C(&place.frame.rot, tp->pt.x + (f32)k * tp->pt.y, tp->yaw);
                    break;
                case 0: {
                    V2 pt;
                    pt.x = tp->pt.x + tp->offset.x * (f32)k;
                    pt.y = tp->pt.y + tp->offset.y * (f32)k;
                    fn_801C3AF8(&place, &pt, tp->yaw);
                    break;
                }
                case 2: {
                    V2 off = tp->pt;
                    if (fn_801769CC()->type & 0x100) {
                        CarPose pose = car->pose;
                        fn_801B8100(&pose, i, car->player);
                        place = pose.place;
                    } else if (fn_801769CC()->type & 0x100) {
                        f32 w = fn_80139048();
                        f32 spacing = 0.61f + (fn_80137C54(-1)->len + w);
                        fn_801B8048(&place, fn_801B2ED0(i)->Follower(), 0.0f, off.y + -spacing * (f32)k);
                    } else {
                        CamPose f = car->pose.place.frame;
                        IVec3 euler;
                        euler.x = 0.0f;
                        euler.y = tp->yaw;
                        euler.z = 0.0f;
                        Mat3 r;
                        fn_80015A20(&r, &euler);
                        Mat3Mul(&f.rot, &r, &f.rot);
                        f.pos.x += f.rot.m[0] * off.x;
                        f.pos.y += f.rot.m[1] * off.x;
                        f.pos.z += f.rot.m[2] * off.x;
                        f.pos.x += f.rot.m[6] * off.y;
                        f.pos.y += f.rot.m[7] * off.y;
                        f.pos.z += f.rot.m[8] * off.y;
                        f32 yaw = atan2(f.rot.m[6], f.rot.m[8]);
                        V2 pt;
                        pt.y = f.pos.z;
                        pt.x = f.pos.x;
                        fn_801C3AF8(&place, &pt, yaw);
                    }
                    break;
                }
                default:
                    fn_802E317C("interlude: unhandled teleport where %d", tp->where);
                    break;
                }
                place.vel.x = place.frame.rot.m[6] * tp->speed;
                place.vel.y = place.frame.rot.m[7] * tp->speed;
                place.vel.z = place.frame.rot.m[8] * tp->speed;
                ((CarObj*)car)->Place(&place);
                if (fn_802794AC(car->player, 0) && (fn_801769CC()->type & 0x1400)) {
                    fn_801B8E28(i);
                }
                k++;
            }
            break;
        }
        case 5: {
            ThinkEvent* th = (ThinkEvent*)e;
            AIOrder* o = lbl_80437E30;
            for (int i = 0; i < lbl_8055E804; o++, i++) {
                if (fn_801C3508(th->mask, i)) {
                    o->kind = th->kind;
                    o->param = th->param;
                    o->speed = th->speed;
                    o->x = th->x;
                    o->z = th->z;
                }
            }
            break;
        }
        case 7: {
            TextEvent* tx = (TextEvent*)e;
            msg->title = tx->text;
            msg->text = tx->title;
            msg->time = lbl_8055E3C8;
            break;
        }
        default:
            fn_802E30C8("interlude: unhandled event type %d", e->type);
            break;
        }
        if (blocked) {
            lbl_8055E7E4 -= t - e->time;
            lbl_8055E7F4--;
            return;
        }
    }
}

// Drives the AI cars by their standing orders while an interlude runs.
void fn_801C56AC(void) {
    if (lbl_8055E7E2) {
        AIOrder* o = lbl_80437E30;
        for (int i = 0; i < 8; i++, o++) {
            ICar* car = (ICar*)fn_8017960C(i);
            if (!car) {
                return;
            }
            f32 speed = o->speed;
            fn_8018ADCC(car, 0.0f);
            fn_8018ADC4(car, 1.0f);
            fn_8018ADEC(car);
            fn_8018ADAC(car, 0.0f);
            LineFollower* line = fn_801B2ED0(i)->Follower();
            V2 pos;
            pos.x = car->pos.x;
            pos.y = car->pos.z;
            line->Update(&pos);
            if (o->kind != 5 && o->kind != 0) {
                int ahead = 1;
                V2 tgt(0.0f, 0.0f);
                if (o->param > 1) {
                    ahead = o->param;
                }
                ICar* lead = (ICar*)fn_8017960C((i + lbl_8055E804 - ahead) % lbl_8055E804);
                if (o->kind == 2) {
                    tgt.x = o->x;
                    tgt.y = o->z;
                } else if (o->kind == 1 || o->kind == 4) {
                    f32 d = o->z;
                    if (5.0f > d) {
                        d = 5.0f;
                    }
                    V2 dir;
                    fn_80135B9C(line, d, &tgt, &dir, 0, 0);
                    f32 k = o->x;
                    tgt.x += dir.x * k;
                    tgt.y += dir.y * k;
                } else {
                    tgt.x = lead->pos.x;
                    tgt.y = lead->pos.z;
                    V2 side(car->side.x, car->side.z);
                    f32 k = o->x;
                    tgt.x += side.x * k;
                    tgt.y += side.y * k;
                }
                tgt.x -= pos.x;
                tgt.y -= pos.y;
                IVec3 to;
                to.x = tgt.x;
                to.y = 0.0f;
                to.z = tgt.y;
                fn_8018ADAC(car, ClampSigned(0.1f * PSVECDotProduct(&to, &car->side) + 0.9f * -car->yawRate));
                if (o->kind == 3 || o->kind == 4) {
                    IVec3 rel;
                    rel.x = lead->pos.x - car->pos.x;
                    rel.y = lead->pos.y - car->pos.y;
                    rel.z = lead->pos.z - car->pos.z;
                    f32 gap = PSVECDotProduct(&rel, &car->fwd) - o->z;
                    f32 a = 0.5f + (o->speed - car->speed);
                    IMotion m;
                    m.p2 = 0.0f;
                    m.v2 = lead->speed;
                    m.a2 = 0.0f;
                    m.p1 = -gap;
                    m.v1 = car->speed;
                    m.a1 = a;
                    f32 r = fn_80139030(&m);
                    fn_8018ADB4(car, Clamp01(10.0f * r));
                    fn_8018ADBC(car, Clamp01(-r));
                    f32 base = lead->speed + 0.5f * (o->speed - lead->speed);
                    if (gap > 0.0f) {
                        speed = base + 0.5f * (gap * o->speed);
                    } else {
                        speed = base + 2.0f * (gap * o->speed);
                    }
                }
            }
            if (o->kind == 5) {
                fn_8018ADB4(car, 1.0f);
                fn_8018ADBC(car, 0.0f);
                fn_8018ADAC(car, ClampSigned(o->x));
            } else if (o->kind != 3 && o->kind != 4) {
                f32 cur = car->speed;
                if (!((f32)__fabs(speed) <= lbl_80558DC4[0])) {
                    fn_8018ADB4(car, Clamp01(0.5f * (speed - cur)));
                    fn_8018ADBC(car, Clamp01(0.5f * (cur - speed)));
                } else {
                    fn_8018ADB4(car, 0.0f);
                    fn_8018ADBC(car, 1.0f);
                }
            }
            fn_8018ADCC(car, 0.0f);
            fn_8018ADC4(car, 1.0f);
            fn_8018ADEC(car);
        }
    }
}

u8 fn_801C5B60(void) { return lbl_8055E7E2; }

// A camera view handed to the camera manager (0xE0 bytes).
struct CamView {
    Mat3 rot;        // offset 0x0
    IVec3 pos;       // offset 0x24
    f32 fov;         // offset 0x30
    u8 pad34[0x58 - 0x34];
    s32 modeB;       // offset 0x58
    s32 modeA;       // offset 0x5C
    s32 car;         // offset 0x60
    s16 carId;       // offset 0x64
    u8 pad66[2];
    u32 flags;       // offset 0x68
    u8 pad6C[0xE0 - 0x6C];
};

// What the listener hears from: the car and its saved view and heading.
struct CamListen {
    s32 car;
    CamPose* pose;
    IVec3* heading;
};

extern "C" {
f32 Dot(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz);
void fn_801BFD34(IVec3* out, const IVec3* p, const CamPose* m);
void fn_8020655C(CamListen* l, int a);
}

// Smooth step on [-1, 1]: 3t^2 - 2t^3, mirrored for negative t.
static inline f32 Ease(f32 t) {
    if (t < -1.0f) {
        return -1.0f;
    }
    if (t > 1.0f) {
        return 1.0f;
    }
    if (t > 0.0f) {
        return t * (-2.0f * t * t) + 3.0f * t * t;
    }
    return t * (-2.0f * t * t) - 3.0f * t * t;
}

static inline void MulM(Mat3* out, const Mat3* a, const Mat3* b) {
    if (out == a || out == b) {
        f32 m0 = Dot(b->m[0], a->m[0], b->m[3], a->m[1], b->m[6], a->m[2]);
        f32 m3 = Dot(b->m[0], a->m[3], b->m[3], a->m[4], b->m[6], a->m[5]);
        f32 m6 = Dot(b->m[0], a->m[6], b->m[3], a->m[7], b->m[6], a->m[8]);
        f32 m1 = Dot(b->m[1], a->m[0], b->m[4], a->m[1], b->m[7], a->m[2]);
        f32 m4 = Dot(b->m[1], a->m[3], b->m[4], a->m[4], b->m[7], a->m[5]);
        f32 m7 = Dot(b->m[1], a->m[6], b->m[4], a->m[7], b->m[7], a->m[8]);
        f32 m2 = Dot(b->m[2], a->m[0], b->m[5], a->m[1], b->m[8], a->m[2]);
        f32 m5 = Dot(b->m[2], a->m[3], b->m[5], a->m[4], b->m[8], a->m[5]);
        f32 m8 = Dot(b->m[2], a->m[6], b->m[5], a->m[7], b->m[8], a->m[8]);
        out->m[0] = m0;
        out->m[3] = m3;
        out->m[6] = m6;
        out->m[1] = m1;
        out->m[4] = m4;
        out->m[7] = m7;
        out->m[2] = m2;
        out->m[5] = m5;
        out->m[8] = m8;
    } else {
        out->m[0] = Dot(b->m[0], a->m[0], b->m[3], a->m[1], b->m[6], a->m[2]);
        out->m[3] = Dot(b->m[0], a->m[3], b->m[3], a->m[4], b->m[6], a->m[5]);
        out->m[6] = Dot(b->m[0], a->m[6], b->m[3], a->m[7], b->m[6], a->m[8]);
        out->m[1] = Dot(b->m[1], a->m[0], b->m[4], a->m[1], b->m[7], a->m[2]);
        out->m[4] = Dot(b->m[1], a->m[3], b->m[4], a->m[4], b->m[7], a->m[5]);
        out->m[7] = Dot(b->m[1], a->m[6], b->m[4], a->m[7], b->m[7], a->m[8]);
        out->m[2] = Dot(b->m[2], a->m[0], b->m[5], a->m[1], b->m[8], a->m[2]);
        out->m[5] = Dot(b->m[2], a->m[3], b->m[5], a->m[4], b->m[8], a->m[5]);
        out->m[8] = Dot(b->m[2], a->m[6], b->m[5], a->m[7], b->m[8], a->m[8]);
    }
}

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void MulVec(IVec3* out, const IVec3* in, const Mat3* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m->m[0], in->y, m->m[3], in->z, m->m[6]);
        f32 y = Dot3(in->x, m->m[1], in->y, m->m[4], in->z, m->m[7]);
        f32 z = Dot3(in->x, m->m[2], in->y, m->m[5], in->z, m->m[8]);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m->m[0], in->y, m->m[3], in->z, m->m[6]);
        out->y = Dot3(in->x, m->m[1], in->y, m->m[4], in->z, m->m[7]);
        out->z = Dot3(in->x, m->m[2], in->y, m->m[5], in->z, m->m[8]);
    }
}

// The interlude camera for view idx while an interlude plays (and the editor is not flying it): the
// current camera move at the current time, relative to the world, the start line or a car, blended in
// from the previous camera over the interpolation time. Returns 0 when not active.
int fn_801C5B68(CamView* v, int idx) {
    if (idx == 0) {
        lbl_8055E800++;
    }
    u8* s = lbl_80437D88;
    CamPlay* p = (CamPlay*)s;
    if (fn_801C5B60() && !lbl_8055E80C) {
        CamPose* cam = (CamPose*)&lbl_80499050;
        cam->pos.x = p->origin.x;
        cam->pos.y = p->origin.y;
        cam->pos.z = p->origin.z;
        cam->rot = p->rot;
        lbl_8055F47C = p->fov;
    }
    if (!fn_801C5B60() || lbl_8055E80C) {
        return 0;
    }
    if (!lbl_8055E818) {
        lbl_8055E818 = 1;
    }
    if (!lbl_8055E819) {
        lbl_8055E819 = 1;
    }
    memset(v, 0, sizeof(CamView));
    int car = fn_80179630(idx);
    if (p->target >= 0) {
        car = p->target;
    }
    ICar* c = (ICar*)fn_8017960C(car);
    v->flags |= 5;
    f32 dt;
    if (fn_801C3B6C() > p->end) {
        dt = p->end - p->start;
    } else {
        dt = fn_801C3B6C() - p->start;
    }
    f32 len = p->end - p->start;
    if (len > 0.1f) {
        f32 e = Ease(dt / len);
        if (p->accel) {
            dt = e * len;
        }
    }
    v->pos.x = p->origin.x;
    v->pos.y = p->origin.y;
    v->pos.z = p->origin.z;
    v->rot = p->rot;
    IVec3 move;
    move.x = p->v.x * dt;
    move.y = p->v.y * dt;
    move.z = p->v.z * dt;
    IVec3 d;
    MulVec(&d, &move, &v->rot);
    v->pos.x += d.x;
    v->pos.y += d.y;
    v->pos.z += d.z;
    ValVec3 turn;
    turn.x = p->av.x * dt;
    turn.y = p->av.y * dt;
    turn.z = p->av.z * dt;
    Mat3 r;
    AxisAngleToRot(&r, turn);
    MulM(&v->rot, &r, &v->rot);
    if (p->target >= 0) {
        CamPose rel;
        rel.rot = v->rot;
        rel.pos = v->pos;
        CamPose f;
        switch (p->camType) {
        case 0:
            f.rot.m[0] = 1.0f;
            f.rot.m[1] = 0.0f;
            f.rot.m[2] = 0.0f;
            f.rot.m[3] = 0.0f;
            f.rot.m[4] = 1.0f;
            f.rot.m[5] = 0.0f;
            f.rot.m[6] = 0.0f;
            f.rot.m[7] = 0.0f;
            f.rot.m[8] = 1.0f;
            f.pos.x = 0.0f;
            f.pos.y = 0.0f;
            f.pos.z = 0.0f;
            break;
        case 1:
            f = c->pose.place.frame;
            break;
        case 2:
            f = c->frame;
            break;
        default:
            fn_802E317C("intrlude: unknown camtype %d", p->camType);
            break;
        }
        if (p->camType != 1) {
            fn_801BFD34(&v->pos, &rel.pos, &f);
            Mat3Mul(&v->rot, &rel.rot, &f.rot);
        } else {
            v->pos = f.pos;
            MulM(&v->rot, &rel.rot, &f.rot);
            IVec3 o;
            MulVec(&o, &rel.pos, &v->rot);
            v->pos.x += o.x;
            v->pos.y += o.y;
            v->pos.z += o.z;
        }
    }
    CamPose* prev = (CamPose*)(s + 0x68);
    f32* prevFov = (f32*)(s + 0x98);
    if (!((f32)__fabs(p->interpTime) <= lbl_80558DC4[0]) && dt < p->interpTime) {
        f32 u = dt / p->interpTime;
        if (p->interpType == 1) {
            u = Ease(u);
        }
        fn_801C2FF4((IFrame*)v, (IFrame*)prev, (IFrame*)v, u);
        p->fov = *prevFov + u * (p->fov - *prevFov);
    } else {
        prev->rot.m[0] = v->rot.m[0];
        prev->rot.m[1] = v->rot.m[1];
        prev->rot.m[2] = v->rot.m[2];
        prev->rot.m[3] = v->rot.m[3];
        prev->rot.m[4] = v->rot.m[4];
        prev->rot.m[5] = v->rot.m[5];
        prev->rot.m[6] = v->rot.m[6];
        prev->rot.m[7] = v->rot.m[7];
        prev->rot.m[8] = v->rot.m[8];
        prev->pos.x = v->pos.x;
        prev->pos.y = v->pos.y;
        prev->pos.z = v->pos.z;
        *prevFov = p->fov;
    }
    v->fov = p->fov;
    CamPose* saved = (CamPose*)(s + 0x148) + idx;
    saved->rot = v->rot;
    saved->pos = v->pos;
    IVec3* heading = (IVec3*)(s + 0x208) + idx;
    *heading = c->heading;
    v->car = car;
    v->modeA = 0xB;
    v->modeB = 0xB;
    v->carId = c->id;
    CamListen l;
    l.car = car;
    l.pose = saved;
    l.heading = heading;
    fn_8020655C(&l, 0);
    return 1;
}

void fn_801C6AD0(int* alpha) {
    if (lbl_8055E7E2) {
        f32 t = fn_801C3B6C();
        if (t < 1.0f) {
            f32 v = 255.0f * (1.0f - t);
            int a = *alpha;
            int x = (int)(v < 0.0f ? v - 0.5f : 0.5f + v);
            if (x > a) {
                a = x;
            }
            *alpha = a >= 255 ? 255 : a;
        }
    }
}

void fn_801C6B70(void) {
    Message* msg = &lbl_80437E24;
    if (msg->time >= 0.0f) {
        if (lbl_8055E3C8 < msg->time || lbl_8055E3C8 - msg->time > 10.0f) {
            lbl_80437E24.time = -999.0f;
            lbl_80437E24.title = 0;
            lbl_80437E24.text = 0;
        } else {
            const char* s = msg->text;
            if (strlen(s)) {
                fn_8000FC8C(s);
            }
            s = lbl_80437E24.title;
            if (strlen(s)) {
                fn_8000FDF0(s);
            }
        }
    }
}

int fn_801C6C28(const IVec3* p) {
    int found = -1;
    for (int i = 0; i < lbl_8055E804; i++) {
        if (PSVECSquareDistance(fn_8017960C(i) + 0x60, p) < 2.25f) {
            found = i;
            break;
        }
    }
    return found;
}

void fn_801C6CB0(void) {}

void fn_801C6CB4(void) { fn_801C957C(1); }

void fn_801C6CBC(void) { sprintf(lbl_804380F0, "%s %d", lbl_804380F0, fn_801C6C28(&lbl_80438268)); }

void fn_801C6D00(void) {
    lbl_804381F0 = lbl_80499050;
    lbl_8055E834 = lbl_8055F47C;
}

void fn_801C6D94(void) {
    lbl_80438220 = lbl_80499050;
    fn_801C957C(1);
}

// A rotation as axis * angle.
static inline void RotToAxisAngle(const Mat3* m, IVec3* out) {
    IQuat q;
    RotToQuat(m, &q);
    f32 ang = acos(q.w);
    f32 sn = sin(ang);
    if ((f32)__fabs(sn) > lbl_80558DC4[0]) {
        f32 k = 2.0f * ang / sn;
        out->x = q.x * k;
        out->y = q.y * k;
        out->z = q.z * k;
    } else {
        out->x = 0.0f;
        out->y = 0.0f;
        out->z = 0.0f;
    }
}

// Add camera: a move from the current editor camera to the destination one over the transition time,
// stored as linear and angular velocities in the current camera's frame.
void fn_801C6E40(void) {
    u8* s = lbl_80437D88;
    lbl_8055E830 = atof((char*)(s + 0x368));
    CamEvent* e = (CamEvent*)fn_801C3C80();
    e->type = 1;
    CamPose* cur = (CamPose*)(s + 0x468);
    e->fov = lbl_8055E834;
    e->origin.x = cur->pos.x;
    e->origin.y = cur->pos.y;
    e->origin.z = cur->pos.z;
    e->rot = cur->rot;
    e->end = e->time + lbl_8055E830;
    e->accel = 1;
    if (lbl_8055E830 > 0.01f) {
        IVec3 dp;
        Mat3 d;
        Mat3 inv;
        CamPose* dst = (CamPose*)(s + 0x498);
        inv = cur->rot;
        fn_801C7254(&inv);
        Mat3Mul(&d, &dst->rot, &inv);
        fn_80144CD0(&dp, &dst->pos, (const f32*)cur);
        RotToAxisAngle(&d, &e->av);
        f32 inv1 = 1.0f / lbl_8055E830;
        e->av.x *= inv1;
        e->av.y *= inv1;
        e->av.z *= inv1;
        f32 inv2 = 1.0f / lbl_8055E830;
        e->v.x = dp.x * inv2;
        e->v.y = dp.y * inv2;
        e->v.z = dp.z * inv2;
    } else {
        e->av.x = 0.0f;
        e->av.y = 0.0f;
        e->av.z = 0.0f;
        e->v.x = 0.0f;
        e->v.y = 0.0f;
        e->v.z = 0.0f;
    }
    sprintf((char*)(s + 0x238), "Camera added at %.2f seconds", e->time);
    qsort(lbl_8055E7F8, lbl_8055E7F0, sizeof(Event), fn_801C3B7C);
    fn_801C957C(0);
}

void fn_801C7254(Mat3* m) {
    f32 s;
    s = m->m[1];
    m->m[1] = m->m[3];
    m->m[3] = s;
    s = m->m[2];
    m->m[2] = m->m[6];
    m->m[6] = s;
    s = m->m[5];
    m->m[5] = m->m[7];
    m->m[7] = s;
}

void fn_801C7288(void) { fn_801C957C(0); }

void fn_801C7290(void) {
    Event* e = fn_801C3C80();
    e->type = 2;
    sprintf(lbl_80437FC0, "Fadeout added at %.2f seconds", e->time);
    qsort(lbl_8055E7F8, lbl_8055E7F0, sizeof(Event), fn_801C3B7C);
}

void fn_801C72F0(void) {
    TextEvent* e = (TextEvent*)fn_801C3C80();
    e->type = 7;
    strncpy(e->text, lbl_804380F0, 0x1FF);
    sprintf(lbl_80437FC0, "Text message added at %.2f seconds", e->time);
    qsort(lbl_8055E7F8, lbl_8055E7F0, sizeof(Event), fn_801C3B7C);
    fn_801C957C(0);
}

void fn_801C7374(void) { fn_801C957C(0); }

void fn_801C737C(void) { lbl_8055E839 = 1; }

void fn_801C7388(void) { lbl_8055E839 = 0; }

void fn_801C7394(void) {
    if (lbl_8055E7F0 > 0) {
        fn_801C3C14(lbl_8055E828);
        if (lbl_8055E828 > lbl_8055E7F0 - 1) {
            lbl_8055E828 = lbl_8055E7F0 - 1;
        }
    }
}

void fn_801C73DC(void) {
    int sel = lbl_8055E828;
    int other = sel - 1;
    int n = lbl_8055E7F0;
    if (n != 0 && other >= 0 && other < n) {
        if (fn_801C34CC(lbl_8055E7F8[sel].time, lbl_8055E7F8[other].time)) {
            Event* a = &lbl_8055E7F8[sel];
            Event* b = &lbl_8055E7F8[other];
            int t = a->order;
            a->order = b->order;
            b->order = t;
            qsort(lbl_8055E7F8, lbl_8055E7F0, sizeof(Event), fn_801C3B7C);
        }
    }
}

void fn_801C7480(void) {
    int sel = lbl_8055E828;
    int other = sel + 1;
    int n = lbl_8055E7F0;
    if (n != 0 && other >= 0 && other < n) {
        if (fn_801C34CC(lbl_8055E7F8[sel].time, lbl_8055E7F8[other].time)) {
            Event* a = &lbl_8055E7F8[sel];
            Event* b = &lbl_8055E7F8[other];
            int t = a->order;
            a->order = b->order;
            b->order = t;
            qsort(lbl_8055E7F8, lbl_8055E7F0, sizeof(Event), fn_801C3B7C);
        }
    }
}

void fn_801C7524(void) {
    fn_801C957C(1);
    sprintf(lbl_804380F0, "%.2f", lbl_8055E7F8[lbl_8055E828].time);
}

void fn_801C756C(void) {
    if (lbl_8055E7F0 > 0) {
        f32 t = atof(lbl_804380F0);
        int order = 0;
        for (int i = 0; i < lbl_8055E7F0; i++) {
            if (fn_801C34CC(lbl_8055E7F8[i].time, t) && lbl_8055E7F8[i].order > order) {
                order = lbl_8055E7F8[i].order;
            }
        }
        CamEvent* e = (CamEvent*)&lbl_8055E7F8[lbl_8055E828];
        if (e->type == 1) {
            e->end = t + (e->end - e->time);
        }
        lbl_8055E7F8[lbl_8055E828].time = fn_801C3450(t);
        lbl_8055E7F8[lbl_8055E828].order = order + 1;
        qsort(lbl_8055E7F8, lbl_8055E7F0, sizeof(Event), fn_801C3B7C);
    }
    fn_801C957C(0);
}

void fn_801C7698(void) {
    if (lbl_8055E7F0 > 0) {
        f32 d = atof(lbl_804380F0);
        int i = lbl_8055E828;
        if (d + lbl_8055E7F8[i].time < 0.0f) {
            sprintf(lbl_80437FC0, "Invalid time change!!!");
        } else {
            for (; i < lbl_8055E7F0; i++) {
                lbl_8055E7F8[i].time = fn_801C3450(d + lbl_8055E7F8[i].time);
            }
            qsort(lbl_8055E7F8, lbl_8055E7F0, sizeof(Event), fn_801C3B7C);
        }
    }
    fn_801C957C(0);
}

void fn_801C7770(void) { lbl_8055E83E = !lbl_8055E83E; }

void fn_801C7784(void) { lbl_8055E83F = !lbl_8055E83F; }

void fn_801C7798(void) { lbl_8055E840 = !lbl_8055E840; }

void fn_801C77AC(void) { lbl_8055E841 = !lbl_8055E841; }

void fn_801C77C0(void) {
    lbl_8055E844 = atoi(lbl_804380F0);
    lbl_8055E841 = 0;
    fn_801C957C(0);
}

void fn_801C77FC(void) {
    char buf[16];
    SoundEvent* e = (SoundEvent*)fn_801C3C80();
    e->type = 3;
    e->imm = lbl_8055E83F;
    e->overlap = lbl_8055E83E;
    e->soundType = lbl_8055E840 != 0;
    if (lbl_8055E841) {
        e->sample = -1;
    } else {
        e->sample = lbl_8055E844;
    }
    if (lbl_8055E841) {
        strcpy(buf, "RANDOM");
    } else {
        sprintf(buf, "%u", lbl_8055E844);
    }
    sprintf(lbl_80437FC0, "Sound sample %s added at %.2f seconds", buf, e->time);
    qsort(lbl_8055E7F8, lbl_8055E7F0, sizeof(Event), fn_801C3B7C);
}

void fn_801C78D8(void) {
    lbl_8055E83C = 1;
    fn_801C957C(1);
    lbl_8055E83A = 1;
    lbl_8055E868 = fn_801C6CBC;
}

void fn_801C791C(void) {
    fn_801C78D8();
    lbl_8055E848 = 2;
}

void fn_801C7944(void) {
    lbl_8055E83A = 1;
    lbl_8055E868 = fn_801C6CB0;
    lbl_8055E848 = 0;
}

void fn_801C7964(void) { lbl_80438250 = lbl_80438268; }

void fn_801C7990(void) {
    u8* s = lbl_80437D88;
    IVec3* c = (IVec3*)(s + 0x4E0);
    *(IVec3*)(s + 0x6F8) = *c;
    lbl_8055E83B = 1;
    *(IVec3*)(s + 0x4D4) = *c;
    lbl_8055E868 = fn_801C7964;
}

void fn_801C79E0(void) {
    lbl_8043848C = lbl_80438268;
    lbl_8055E83B = 0;
    lbl_8055E868 = fn_801C6CB0;
    lbl_8055E83D = 1;
}

void fn_801C7A28(void) {
    lbl_80438498 = lbl_80438268;
    lbl_8055E83A = 0;
    lbl_8055E868 = fn_801C6CB0;
    lbl_8055E83D = 0;
    fn_801C78D8();
}

void fn_801C7A6C(void) {
    f32 z = atof(lbl_804380F0);
    lbl_80438498.x = 0.0f;
    lbl_80438498.y = 0.0f;
    lbl_80438498.z = z;
    strcpy(lbl_804380F0, "");
}

void fn_801C7AC4(void) {
    strcpy(lbl_80438280, lbl_804380F0);
    strcpy(lbl_804380F0, "");
    if (!lbl_80438280[0]) {
        strcpy(lbl_80438280, "nobody");
    }
}

void fn_801C7B28(void) {
    strcpy(lbl_80438380, lbl_804380F0);
    strcpy(lbl_804380F0, "");
    if (!lbl_80438380[0]) {
        strcpy(lbl_80438380, "nobody");
    }
    lbl_8055E868 = fn_801C6CB0;
    lbl_8055E83A = 0;
}

void fn_801C7BA0(void) {
    lbl_8055E83D = 0;
    lbl_8055E83C = 0;
    lbl_8055E83B = 0;
    lbl_8055E83A = 0;
    lbl_8055E868 = fn_801C6CB0;
    strcpy(lbl_80438280, "");
    strcpy(lbl_80438380, "");
    fn_801C957C(0);
}

// The signed angle from a to b (both normalised first unless already unit length).
static inline f32 Angle(const V2& b, const V2& a) {
    const V2* pa = &a;
    V2 na;
    f32 la = a.x * a.x + a.y * a.y;
    if (!((f32)__fabs(la - 1.0f) <= lbl_80558DC4[0])) {
        na = a;
        f32 inv = 1.0f / (f32)sqrt(la);
        na.x *= inv;
        na.y *= inv;
        pa = &na;
    }
    const V2* pb = &b;
    V2 nb;
    f32 lb = b.x * b.x + b.y * b.y;
    if (!((f32)__fabs(lb - 1.0f) <= lbl_80558DC4[0])) {
        nb = b;
        f32 inv = 1.0f / (f32)sqrt(lb);
        nb.x *= inv;
        nb.y *= inv;
        pb = &nb;
    }
    return atan2(pa->x * pb->y - pb->x * pa->y, pa->x * pb->x + pa->y * pb->y);
}

// Add teleport: who/except, where (0 = world point facing a direction, 2 = starting positions) and speed.
void fn_801C7C04(void) {
    u8* s = lbl_80437D88;
    lbl_8055E84C = 0.44444445f * (f32)atof((char*)(s + 0x368));
    fn_801C957C(0);
    TeleportEvent* e = (TeleportEvent*)fn_801C3C80();
    e->type = 4;
    e->where = lbl_8055E848;
    strcpy(e->who, (char*)(s + 0x4F8));
    strcpy(e->except, (char*)(s + 0x5F8));
    e->mask = fn_801C3954(e->who);
    e->mask &= ~fn_801C3954(e->except);
    e->speed = lbl_8055E84C;
    switch (e->where) {
    case 2:
        e->yaw = 0.0f;
        e->pt = XZ(*(IVec3*)(s + 0x710));
        break;
    case 0:
        e->pt = XZ(*(IVec3*)(s + 0x6F8));
        e->offset = XZ(*(IVec3*)(s + 0x710)) - e->pt;
        e->yaw = Angle(V2(0.0f, 1.0f), XZ(*(IVec3*)(s + 0x704)) - XZ(*(IVec3*)(s + 0x6F8)));
        break;
    case 1:
    default:
        fn_802E317C("Interlude Editor: I'm not set up to handle where type %d", e->where);
        break;
    }
    IVec3* p = (IVec3*)(s + 0x6F8);
    sprintf((char*)(s + 0x238), "Added teleport to (%.2f, %.2f) at %.2f", p->x, p->z, e->time);
    fn_801C7BA0();
    qsort(lbl_8055E7F8, lbl_8055E7F0, sizeof(Event), fn_801C3B7C);
}

void fn_801C7EF4(void) {
    lbl_8055E850 = 0;
    fn_801C78D8();
}

void fn_801C7F00(void) {
    strcpy(lbl_80438280, lbl_804380F0);
    strcpy(lbl_804380F0, "");
    if (!lbl_80438280[0]) {
        strcpy(lbl_80438280, "all");
    }
}

void fn_801C7F64(void) {
    strcpy(lbl_80438380, lbl_804380F0);
    strcpy(lbl_804380F0, "");
    if (!lbl_80438380[0]) {
        strcpy(lbl_80438380, "nobody");
    }
    fn_801C957C(0);
    lbl_8055E868 = fn_801C6CB0;
    lbl_8055E83A = 1;
}

void fn_801C7FE4(void) {
    u8* s = lbl_80437D88;
    lbl_8055E83A = 0;
    WaitEvent* e = (WaitEvent*)fn_801C3C80();
    e->type = 6;
    e->kind = lbl_8055E850;
    strcpy(e->who, (char*)(s + 0x4F8));
    strcpy(e->except, (char*)(s + 0x5F8));
    e->mask = fn_801C3954(e->who);
    e->mask &= ~fn_801C3954(e->except);
    IVec3* c = (IVec3*)(s + 0x4E0);
    e->pt.x = c->x;
    e->pt.y = c->y;
    e->pt.z = c->z;
    sprintf((char*)(s + 0x238), "Added wait at %.2f", e->time);
    qsort(lbl_8055E7F8, lbl_8055E7F0, sizeof(Event), fn_801C3B7C);
}

void fn_801C80BC(void) {
    lbl_8055E854 = 0;
    fn_801C78D8();
}

void fn_801C80C8(void) {
    lbl_8055E83A = 1;
    lbl_8055E868 = fn_801C6CB0;
    lbl_8055E854 = 2;
}

void fn_801C80E8(void) {
    lbl_8055E854 = 3;
    lbl_8055E860 = 5.0f;
    lbl_8055E85C = 0.0f;
    lbl_8055E83A = 1;
    lbl_8055E868 = fn_801C6CBC;
    fn_801C957C(1);
}

void fn_801C811C(void) {
    lbl_8055E854 = 4;
    lbl_8055E860 = 5.0f;
    lbl_8055E85C = 0.0f;
    lbl_8055E83A = 1;
    lbl_8055E868 = fn_801C6CBC;
    fn_801C957C(1);
}

void fn_801C8150(void) {
    lbl_8055E864 = atoi(lbl_804380F0);
    strcpy(lbl_804380F0, "");
}

void fn_801C8190(void) {
    lbl_8055E854 = 1;
    fn_801C78D8();
}

void fn_801C819C(void) {
    lbl_8055E85C = lbl_80438268.x;
    lbl_8055E860 = lbl_80438268.z;
    fn_801C78D8();
}

void fn_801C81B8(void) {
    strcpy(lbl_804384A8, lbl_804380F0);
    strcpy(lbl_804380F0, "");
    if (!lbl_804384A8[0]) {
        strcpy(lbl_804384A8, "all");
    }
}

void fn_801C821C(void) {
    strcpy(lbl_804385A8, lbl_804380F0);
    strcpy(lbl_804380F0, "");
    if (!lbl_804385A8[0]) {
        strcpy(lbl_804385A8, "nobody");
    }
    lbl_8055E83A = 0;
    lbl_8055E868 = fn_801C6CB0;
    lbl_8055E83C = 0;
}

void fn_801C8298(void) {
    strcpy(lbl_804384A8, "");
    strcpy(lbl_804385A8, "");
    lbl_8055E864 = 0;
    fn_801C7BA0();
}

void fn_801C82E0(void) {
    u8* s = lbl_80437D88;
    lbl_8055E858 = 0.44444445f * (f32)atof((char*)(s + 0x368));
    ThinkEvent* e = (ThinkEvent*)fn_801C3C80();
    e->type = 5;
    e->kind = lbl_8055E854;
    strcpy(e->who, (char*)(s + 0x720));
    strcpy(e->except, (char*)(s + 0x820));
    e->mask = fn_801C3954(e->who);
    e->mask &= ~fn_801C3954(e->except);
    e->x = lbl_8055E85C;
    e->z = lbl_8055E860;
    e->speed = lbl_8055E858;
    e->param = lbl_8055E864;
    sprintf((char*)(s + 0x238), "Added AI Think at %.2f", e->time);
    qsort(lbl_8055E7F8, lbl_8055E7F0, sizeof(Event), fn_801C3B7C);
    fn_801C8298();
}

void fn_801C83D0(void) {
    f32 d = atof(lbl_804380F0);
    lbl_8055A958 = d;
    sprintf(lbl_80437FC0, "Set interlude duration to %.2f", d);
    fn_801C957C(0);
}

void fn_801C8420(void) {
    char path[256];
    const char* qrs = "qintrlud.qrs";
    const char* val = "qintrlud.val";
    if (fn_8013181C("intrlude.txt", val, qrs)) {
        sprintf(path, "%s/%s", qrs, val);
        fn_80128684();
        fn_8012BEF0(qrs);
        fn_801C3D88(path);
        fn_8012BF24(qrs);
        fn_80128678();
        sprintf(lbl_80437FC0, "Interlude Reloaded!!");
    } else {
        sprintf(lbl_80437FC0, "Error trying to reload!!!");
    }
}

void fn_801C84E0(void) {
    lbl_8055E80D = 0;
    sprintf(lbl_80437FC0, "Global event time disabled");
}

void fn_801C8500(void) {
    lbl_8055E810 = fn_801C3450(atof(lbl_804380F0));
    lbl_8055E80D = 1;
    sprintf(lbl_80437FC0, "All new events will be set at %.2f", lbl_8055E810);
    fn_801C7374();
}

static inline void SetItem(MenuItem* it, const char* label, void (*fn)(void), s32 next, bool* check) {
    it->label = label;
    it->fn = fn;
    it->next = next;
    it->check = check;
}

// Builds the editor menus (33 of them; item "next" = menu to open, 0x22 = back).
void fn_801C855C(void) {
    Menu* m;
    lbl_8055E86C = new ("menu") Menu[33];
    memset(lbl_8055E86C, 0, 528);
    m = &lbl_8055E86C[0];
    m->title = "INTERLUDE EDITOR";
    m->count = 11;
    m->items = new ("mi") MenuItem[11];
    SetItem(&m->items[0], "Add text message...", fn_801C6CB4, 1, 0);
    SetItem(&m->items[1], "Set camera here...", fn_801C6D00, 2, 0);
    SetItem(&m->items[2], "Add effect...", fn_801C6CB0, 3, 0);
    SetItem(&m->items[3], "Add sound...", fn_801C6CB0, 4, 0);
    SetItem(&m->items[4], "Teleport cars...", fn_801C6CB0, 5, 0);
    SetItem(&m->items[5], "Set AI thinking...", fn_801C6CB0, 6, 0);
    SetItem(&m->items[6], "Wait until...", fn_801C6CB0, 7, 0);
    SetItem(&m->items[7], "List events...", fn_801C737C, 11, 0);
    SetItem(&m->items[8], "Set duration", fn_801C6CB4, 12, 0);
    SetItem(&m->items[9], "Reload Interlude", fn_801C6CB0, 8, 0);
    SetItem(&m->items[10], "Global event time...", fn_801C6CB0, 9, 0);
    m = &lbl_8055E86C[9];
    m->title = "GLOBAL EVENT TIME";
    m->count = 3;
    m->items = new ("mi") MenuItem[3];
    SetItem(&m->items[0], "Set", fn_801C6CB4, 10, 0);
    SetItem(&m->items[1], "Turn Off", fn_801C84E0, 0, 0);
    SetItem(&m->items[2], "Cancel", fn_801C6CB0, 0, 0);
    m = &lbl_8055E86C[10];
    m->title = "SET GLOBAL TIME";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Set", fn_801C8500, 0, 0);
    SetItem(&m->items[1], "Cancel", fn_801C7374, 0, 0);
    m = &lbl_8055E86C[8];
    m->title = "REALLY RELOAD?";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "No", fn_801C6CB0, 0, 0);
    SetItem(&m->items[1], "Yes, Reload", fn_801C8420, 0, 0);
    m = &lbl_8055E86C[12];
    m->title = "SET DURATION";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Set Duration", fn_801C83D0, 0, 0);
    SetItem(&m->items[1], "Cancel", fn_801C7374, 0, 0);
    m = &lbl_8055E86C[1];
    m->title = "ENTER MSG TO DISPLAY";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Set Message", fn_801C72F0, 0, 0);
    SetItem(&m->items[1], "Cancel", fn_801C7374, 0, 0);
    m = &lbl_8055E86C[3];
    m->title = "EFFECTS";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Add Fadeout", fn_801C7290, 0, 0);
    SetItem(&m->items[1], "Cancel", fn_801C6CB0, 0, 0);
    m = &lbl_8055E86C[2];
    m->title = "MOVE CAM TO DEST";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Ok-I'm at the destination", fn_801C6D94, 32, 0);
    SetItem(&m->items[1], "Cancel", fn_801C7288, 0, 0);
    m = &lbl_8055E86C[32];
    m->title = "TRANSITION TIME? (SECS)";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Set Camera", fn_801C6E40, 0, 0);
    SetItem(&m->items[1], "Cancel", fn_801C7288, 0, 0);
    m = &lbl_8055E86C[4];
    m->title = "TYPE OF SOUND";
    m->count = 7;
    m->items = new ("mi") MenuItem[7];
    SetItem(&m->items[0], "Music", fn_801C7798, 4, &lbl_8055E840);
    SetItem(&m->items[1], "Random", fn_801C77AC, 4, &lbl_8055E841);
    SetItem(&m->items[2], "Overlap", fn_801C7770, 4, &lbl_8055E83E);
    SetItem(&m->items[3], "Immediate", fn_801C7784, 4, &lbl_8055E83F);
    SetItem(&m->items[4], "Set Sample...", fn_801C6CB4, 31, 0);
    SetItem(&m->items[5], "Set Sound", fn_801C77FC, 0, 0);
    SetItem(&m->items[6], "Cancel", fn_801C6CB0, 0, 0);
    m = &lbl_8055E86C[31];
    m->title = "SET SAMPLE ID";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Set Sample", fn_801C77C0, 4, 0);
    SetItem(&m->items[1], "Cancel", fn_801C7374, 4, 0);
    m = &lbl_8055E86C[5];
    m->title = "TELEPORT TYPE";
    m->count = 3;
    m->items = new ("mi") MenuItem[3];
    SetItem(&m->items[0], "Starting positions", fn_801C791C, 13, 0);
    SetItem(&m->items[1], "World point", fn_801C7944, 16, 0);
    SetItem(&m->items[2], "Cancel", fn_801C7BA0, 0, 0);
    m = &lbl_8055E86C[13];
    m->title = "OFFSET";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Continue", fn_801C7A6C, 14, 0);
    SetItem(&m->items[1], "Cancel", fn_801C7BA0, 0, 0);
    m = &lbl_8055E86C[14];
    m->title = "TELEPORT WHO?";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Continue", fn_801C7AC4, 15, 0);
    SetItem(&m->items[1], "Cancel", fn_801C7BA0, 0, 0);
    m = &lbl_8055E86C[15];
    m->title = "EXCEPT WHO?";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Continue", fn_801C7B28, 19, 0);
    SetItem(&m->items[1], "Cancel", fn_801C7BA0, 0, 0);
    m = &lbl_8055E86C[16];
    m->title = "TELEPORT WHERE?";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Continue", fn_801C7990, 17, 0);
    SetItem(&m->items[1], "Cancel", fn_801C7BA0, 0, 0);
    m = &lbl_8055E86C[17];
    m->title = "FACING WHERE?";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Continue", fn_801C79E0, 18, 0);
    SetItem(&m->items[1], "Cancel", fn_801C7BA0, 0, 0);
    m = &lbl_8055E86C[18];
    m->title = "WHERE IS 2ND CAR?";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Continue", fn_801C7A28, 14, 0);
    SetItem(&m->items[1], "Cancel", fn_801C7BA0, 0, 0);
    m = &lbl_8055E86C[19];
    m->title = "CAR SPEED?";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Continue", fn_801C7C04, 0, 0);
    SetItem(&m->items[1], "Cancel", fn_801C7BA0, 0, 0);
    m = &lbl_8055E86C[6];
    m->title = "DO WHAT?";
    m->count = 6;
    m->items = new ("mi") MenuItem[6];
    SetItem(&m->items[0], "Move to point", fn_801C80C8, 23, 0);
    SetItem(&m->items[1], "Park", fn_801C80BC, 24, 0);
    SetItem(&m->items[2], "Use Line", fn_801C8190, 24, 0);
    SetItem(&m->items[3], "Follow", fn_801C80E8, 27, 0);
    SetItem(&m->items[4], "Follow with Line", fn_801C811C, 27, 0);
    SetItem(&m->items[5], "Cancel", fn_801C6CB0, 0, 0);
    m = &lbl_8055E86C[23];
    m->title = "TO WHERE?";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Continue", fn_801C819C, 24, 0);
    SetItem(&m->items[1], "Cancel", fn_801C8298, 0, 0);
    m = &lbl_8055E86C[27];
    m->title = "FOLLOW WHO?";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Continue", fn_801C8150, 24, 0);
    SetItem(&m->items[1], "Cancel", fn_801C8298, 0, 0);
    m = &lbl_8055E86C[24];
    m->title = "WHO?";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Continue", fn_801C81B8, 25, 0);
    SetItem(&m->items[1], "Cancel", fn_801C8298, 0, 0);
    m = &lbl_8055E86C[25];
    m->title = "EXCEPT?";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Continue", fn_801C821C, 26, 0);
    SetItem(&m->items[1], "Cancel", fn_801C8298, 0, 0);
    m = &lbl_8055E86C[26];
    m->title = "SPEED";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Continue", fn_801C82E0, 0, 0);
    SetItem(&m->items[1], "Cancel", fn_801C8298, 0, 0);
    m = &lbl_8055E86C[7];
    m->title = "WAIT TYPE";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Cross Point", fn_801C7EF4, 20, 0);
    SetItem(&m->items[1], "Cancel", fn_801C6CB0, 0, 0);
    m = &lbl_8055E86C[20];
    m->title = "WAIT FOR WHOM?";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Continue", fn_801C7F00, 21, 0);
    SetItem(&m->items[1], "Cancel", fn_801C7BA0, 0, 0);
    m = &lbl_8055E86C[21];
    m->title = "EXCEPT FOR WHOM?";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Continue", fn_801C7F64, 22, 0);
    SetItem(&m->items[1], "Cancel", fn_801C7BA0, 0, 0);
    m = &lbl_8055E86C[22];
    m->title = "WHAT POINT?";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Set wait point", fn_801C7FE4, 0, 0);
    SetItem(&m->items[1], "Cancel", fn_801C7BA0, 0, 0);
    m = &lbl_8055E86C[11];
    m->title = "EVENT LIST";
    m->count = 6;
    m->items = new ("mi") MenuItem[6];
    SetItem(&m->items[0], "Move up", fn_801C73DC, 11, 0);
    SetItem(&m->items[1], "Move down", fn_801C7480, 11, 0);
    SetItem(&m->items[2], "Change time", fn_801C7524, 29, 0);
    SetItem(&m->items[3], "Insert/Del time", fn_801C6CB4, 30, 0);
    SetItem(&m->items[4], "Delete", fn_801C6CB0, 28, 0);
    SetItem(&m->items[5], "Cancel", fn_801C7388, 0, 0);
    m = &lbl_8055E86C[28];
    m->title = "REALLY DELETE?";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Cancel", fn_801C6CB0, 11, 0);
    SetItem(&m->items[1], "Delete", fn_801C7394, 11, 0);
    m = &lbl_8055E86C[29];
    m->title = "SET TIME";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Set time", fn_801C756C, 11, 0);
    SetItem(&m->items[1], "Cancel", fn_801C7374, 11, 0);
    m = &lbl_8055E86C[30];
    m->title = "SET TIME";
    m->count = 2;
    m->items = new ("mi") MenuItem[2];
    SetItem(&m->items[0], "Add time", fn_801C7698, 11, 0);
    SetItem(&m->items[1], "Cancel", fn_801C7374, 11, 0);
    lbl_8055E81C = 0;
    lbl_8055E820 = 0;
}

u8 lbl_80337474[0x38] = {
    0xC8, 0xD0, 0x1C, 0x2E, 0x13, 0x26, 0x23, 0x17, 0x1A, 0x1B, 0xFF, 0x1E, 0x30, 0x2E, 0x20, 0x12, 0x21, 0x22, 0x23,
    0x17, 0x24, 0x25, 0x26, 0x32, 0x31, 0x18, 0x19, 0x10, 0x13, 0x1F, 0x14, 0x16, 0x2F, 0x11, 0x2D, 0x15, 0x2C, 0x02,
    0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0xD3, 0x0E, 0x92, 0x34, 0x39, 0x0C, 0x4A, 0xFF,
};

void fn_801C9444(void) {
    for (int i = 0; i < 0x21; i++) {
        delete[] lbl_8055E86C[i].items;
        lbl_8055E86C[i].items = 0;
    }
    delete[] lbl_8055E86C;
    lbl_8055E86C = 0;
}

void fn_801C94B8(int x, int y, const char* s, int color) { fn_80238A74(x, y, s, color); }

void fn_801C94BC(int x0, int y0, int x1, int y1, u32 color) { fn_80238B44(x0, y0, x1, y1, color); }

void fn_801C94C0(int x, int y) {
    fn_8023895C();
    fn_801C94B8(x, y, lbl_80437FC0, -1);
    fn_802389E4();
}

void fn_801C9514(void) {
    f32 end;
    f32 t0 = lbl_8055E3C8;
    fn_801769EC();
    end = 0.1f + t0;
    while (lbl_8055E3C8 < end && lbl_8055E3C8 >= t0) {
        fn_80176610();
    }
    fn_801769E0();
}

void fn_801C957C(int on) {
    if (on) {
        strcpy(lbl_804380F0, "");
        lbl_80337474[lbl_8055E824] = 0x1E;
    } else {
        lbl_80337474[lbl_8055E824] = 0xFF;
    }
    lbl_8055E838 = on;
}

void fn_801C95EC(void) {
    strcpy(lbl_80437FC0, "");
    lbl_8055E86C[lbl_8055E81C].items[lbl_8055E820].fn();
    int m = lbl_8055E81C;
    int i = lbl_8055E820;
    int n = lbl_8055E86C[m].items[i].next;
    if (n == 0x22) {
        lbl_8055E81C = lbl_8055E870;
    } else {
        lbl_8055E870 = m;
        lbl_8055E81C = n;
        if (m != n) {
            lbl_8055E820 = 0;
        }
    }
}

void fn_801C968C(int d) {
    int i = lbl_8055E820 + d;
    lbl_8055E820 = i;
    if (i < 0) {
        lbl_8055E820 = lbl_8055E86C[lbl_8055E81C].count - 1;
    } else if (i >= lbl_8055E86C[lbl_8055E81C].count) {
        lbl_8055E820 = 0;
    }
}

void fn_801C96E4(void) { lbl_8055E82C = lbl_8055E828 / 10; }

int fn_801C9708(int key) {
    switch (key) {
    case 0xD:
        fn_801C95EC();
        break;
    case 0x126:
        fn_801C968C(-1);
        break;
    case 0x128:
        fn_801C968C(1);
        break;
    case 0x8:
    case 0x12E:
        if (strlen(lbl_804380F0)) {
            lbl_804380F0[strlen(lbl_804380F0) - 1] = 0;
        }
        break;
    default:
        if ((key >= 'A' && key <= 'z') || (key >= '0' && key <= '9') || key == ':' || key == '.' ||
            key == ' ' || key == '-') {
            size_t n = strlen(lbl_804380F0);
            if (n < 0xFF) {
                lbl_804380F0[n] = key;
                int end = n + 1;
                lbl_804380F0[end] = 0;
            }
        }
        break;
    }
    return 1;
}

void fn_801C9830(void) {}

void fn_801C9834(void) {
    fn_801C855C();
    for (int i = 0; i < 55; i++) {
        if (lbl_80337474[i] == 0xFF) {
            lbl_8055E824 = i;
            break;
        }
    }
    lbl_8055E838 = 0;
}

void fn_801C98F0(void) { fn_801C9444(); }

const char* fn_801C98F4(const char* s, int n) {
    for (int i = 0; i < n; i++) {
        while (*s) {
            s++;
        }
        s++;
    }
    return s;
}

// Writes one event as an "action" table of the interlude .val file.
void fn_801C9920(const Event* e, int fd) {
    fn_802E3AD0(fd, "\ttable action\r\n");
    fn_802E3AD0(fd, "\t\tt\t%.2f\r\n", e->time);
    switch (e->type) {
    case 0: {
        const DebugEvent* d = (const DebugEvent*)e;
        fn_802E3AD0(fd, "\t\ttype\tdebug\r\n");
        fn_802E3AD0(fd, "\t\tmsg\t%s\r\n", d->msg);
        break;
    }
    case 1: {
        const CamEvent* c = (const CamEvent*)e;
        fn_802E3AD0(fd, "\t\ttype\tcamera\r\n");
        fn_802E3AD0(fd, "\t\tfov\t%.2f\r\n", fn_801C34F4(c->fov));
        fn_802E3AD0(fd, "\t\torigin\t%.2f, %.2f, %.2f\r\n", c->origin.x, c->origin.y, c->origin.z);
        IVec3 dir;
        RotToAxisAngle(&c->rot, &dir);
        fn_802E3AD0(fd, "\t\tdir\t%.2f, %.2f, %.2f\r\n", dir.x, dir.y, dir.z);
        fn_802E3AD0(fd, "\t\tv\t%.2f, %.2f, %.2f\r\n", c->v.x, c->v.y, c->v.z);
        fn_802E3AD0(fd, "\t\tav\t%.2f, %.2f, %.2f\r\n", c->av.x, c->av.y, c->av.z);
        fn_802E3AD0(fd, "\t\tduration\t%.2f\r\n", c->end - c->time);
        fn_802E3AD0(fd, "\t\taccel\t%s\r\n", c->accel ? "yes" : "no");
        break;
    }
    case 2:
        fn_802E3AD0(fd, "\t\ttype\teffect\r\n");
        break;
    case 3: {
        const SoundEvent* s = (const SoundEvent*)e;
        fn_802E3AD0(fd, "\t\ttype\tsound\r\n");
        fn_802E3AD0(fd, "\t\tsound_type\t%s\r\n", fn_801C98F4(lbl_80562960, s->soundType));
        fn_802E3AD0(fd, "\t\toverlap\t%s\r\n", s->overlap ? "yes" : "no");
        fn_802E3AD0(fd, "\t\timmediate\t%s\r\n", s->imm ? "yes" : "no");
        fn_802E3AD0(fd, "\t\tsample\t%u\r\n", s->sample);
        break;
    }
    case 4: {
        const TeleportEvent* t = (const TeleportEvent*)e;
        fn_802E3AD0(fd, "\t\ttype\tteleport\r\n");
        fn_802E3AD0(fd, "\t\tspeed\t%.2f\r\n", t->speed);
        fn_802E3AD0(fd, "\t\twho\t%s\r\n", t->who);
        fn_802E3AD0(fd, "\t\texcept\t%s\r\n", t->except);
        fn_802E3AD0(fd, "\t\twhere\t%s\r\n", fn_801C98F4(lbl_80562954, t->where));
        switch (t->where) {
        case 1:
            fn_802E3AD0(fd, "\t\toffset\t%.2f\r\n", t->pt.x);
            fn_802E3AD0(fd, "\t\tspacing\t%.2f\r\n", t->pt.y);
            fn_802E3AD0(fd, "\t\tyaw\t%.2f\r\n", t->yaw);
            break;
        case 0:
            fn_802E3AD0(fd, "\t\tpt\t%.2f, %.2f\r\n", t->pt.x, t->pt.y);
            fn_802E3AD0(fd, "\t\tyaw\t%.2f\r\n", t->yaw);
            fn_802E3AD0(fd, "\t\toffset\t%.2f, %.2f\r\n", t->offset.x, t->offset.y);
            break;
        case 2:
            fn_802E3AD0(fd, "\t\tyaw\t%.2f\r\n", t->yaw);
            fn_802E3AD0(fd, "\t\toffset\t%.2f, %.2f\r\n", t->pt.x, t->pt.y);
            break;
        default:
            fn_802E317C("interlude: unhandled teleport where type %d in export", t->where);
            break;
        }
        break;
    }
    case 5: {
        const ThinkEvent* t = (const ThinkEvent*)e;
        fn_802E3AD0(fd, "\t\ttype\tthink\r\n");
        fn_802E3AD0(fd, "\t\twho\t%s\r\n", t->who);
        fn_802E3AD0(fd, "\t\texcept\t%s\r\n", t->except);
        fn_802E3AD0(fd, "\t\thow\t%s\r\n", fn_801C98F4(lbl_80562958, t->kind));
        fn_802E3AD0(fd, "\t\tspeed\t%.2f\r\n", t->speed);
        fn_802E3AD0(fd, "\t\tlat\t%.2f\r\n", t->x);
        fn_802E3AD0(fd, "\t\tlon\t%.2f\r\n", t->z);
        fn_802E3AD0(fd, "\t\tparam\t%d\r\n", t->param);
        if (t->kind == 2) {
            fn_802E3AD0(fd, "\t\tpt\t%.2f, %.2f\r\n", t->x, t->z);
        }
        break;
    }
    case 6: {
        const WaitEvent* w = (const WaitEvent*)e;
        fn_802E3AD0(fd, "\t\ttype\twait\r\n");
        fn_802E3AD0(fd, "\t\twhat\t%s\r\n", fn_801C98F4(lbl_8056295C, w->kind));
        fn_802E3AD0(fd, "\t\twho\t%s\r\n", w->who);
        fn_802E3AD0(fd, "\t\texcept\t%s\r\n", w->except);
        if (w->kind == 0) {
            fn_802E3AD0(fd, "\t\tpt\t%.2f, %.2f, %.2f\r\n", w->pt.x, w->pt.y, w->pt.z);
        } else {
            fn_802E317C("interlude: unhandled wait type %d in export", w->kind);
        }
        break;
    }
    case 7: {
        const TextEvent* t = (const TextEvent*)e;
        fn_802E3AD0(fd, "\t\ttype\ttext\r\n");
        fn_802E3AD0(fd, "\t\tmsg\t%s\r\n", t->text);
        fn_802E3AD0(fd, "\t\ttitle\t%s\r\n", t->title);
        break;
    }
    default:
        fn_802E317C("interlude: unhandled event type %d in export", e->type);
        break;
    }
    fn_802E3AD0(fd, "\tend\r\n");
}

void fn_801CA070(const char* name) {
    int fd = fn_802E367C(name);
    if (fd != -1) {
        const char* table;
        if (lbl_8055E814) {
            table = (const char*)lbl_8055E814;  // name override
        } else if (fn_801C3D5C()->Has("interlude_name")) {
            table = fn_801C3D5C()->GetValue("interlude_name");
        } else {
            table = GetEventTypeName(fn_801769CC()->type);
        }
        fn_802E3AD0(fd, "table %s\r\n", table);
        fn_802E3AD0(fd, "\tduration\t%.2f\r\n", lbl_8055A958);
        for (int i = 0; i < lbl_8055E7F0; i++) {
            fn_801C9920(&lbl_8055E7F8[i], fd);
        }
        fn_802E3AD0(fd, "end\r\nend\r\n");
        fn_802E3734(&fd);
        sprintf(lbl_80437FC0, "Saved interlude to %s", name);
    } else {
        sprintf(lbl_80437FC0, "Error: couldn't save interlude to %s!", name);
    }
}

void fn_801CA1C4(const Event* e, int x, int y, int h) {
    char buf[256];
    if (lbl_8055E7F0 == 0 || lbl_8055E828 >= lbl_8055E7F0) {
        return;
    }
    switch (e->type) {
    case 0:
        sprintf(buf, "Msg: %s", ((const DebugEvent*)e)->msg);
        fn_801C94B8(x, y, buf, -1);
        break;
    case 1:
        break;
    case 2:
        sprintf(buf, "Effect type: Fade");
        fn_801C94B8(x, y, buf, -1);
        break;
    case 3:
        sprintf(buf, "Sound type: %s", fn_801C98F4(lbl_80562960, ((const SoundEvent*)e)->soundType));
        fn_801C94B8(x, y, buf, -1);
        break;
    case 4: {
        const TeleportEvent* t = (const TeleportEvent*)e;
        sprintf(buf, "Teleport type: %s", fn_801C98F4(lbl_80562954, t->where));
        fn_801C94B8(x, y, buf, -1);
        y += h;
        sprintf(buf, "Who: %s", t->who);
        fn_801C94B8(x, y, buf, -1);
        y += h;
        sprintf(buf, "Except: %s", t->except);
        fn_801C94B8(x, y, buf, -1);
        y += h;
        sprintf(buf, "Speed: %.2f", t->speed / 0.44444445f);
        fn_801C94B8(x, y, buf, -1);
        break;
    }
    case 5: {
        const ThinkEvent* t = (const ThinkEvent*)e;
        sprintf(buf, "Think type: %s", fn_801C98F4(lbl_80562958, t->kind));
        fn_801C94B8(x, y, buf, -1);
        y += h;
        sprintf(buf, "Who: %s", t->who);
        fn_801C94B8(x, y, buf, -1);
        y += h;
        sprintf(buf, "Except: %s", t->except);
        fn_801C94B8(x, y, buf, -1);
        y += h;
        sprintf(buf, "Speed: %.2f", t->speed / 0.44444445f);
        fn_801C94B8(x, y, buf, -1);
        break;
    }
    case 6: {
        const WaitEvent* w = (const WaitEvent*)e;
        sprintf(buf, "Wait type: %s", fn_801C98F4(lbl_8056295C, w->kind));
        fn_801C94B8(x, y, buf, -1);
        y += h;
        sprintf(buf, "Who: %s", w->who);
        fn_801C94B8(x, y, buf, -1);
        y += h;
        sprintf(buf, "Except: %s", w->except);
        fn_801C94B8(x, y, buf, -1);
        break;
    }
    case 7: {
        const ShowEvent* t = (const ShowEvent*)e;
        char line[50] = {0};
        strncpy(line, t->title, 0x31);
        sprintf(buf, "Title: %s", line);
        fn_801C94B8(x, y, buf, -1);
        y += h;
        strncpy(line, t->text, 0x31);
        sprintf(buf, "Text: %s", line);
        fn_801C94B8(x, y, buf, -1);
        break;
    }
    default:
        void fn_802E317C(const char* fmt, ...);
        fn_802E317C("Unhandled event type %d in display_action()", e->type);
        break;
    }
}

void fn_801CA5A8(void) {
    char buf[256];
    if (lbl_8055E7F0 != 0) {
        int h = fn_80238A20();
        Event* e = &lbl_8055E7F8[lbl_8055E828];
        sprintf(buf, "type: %s", fn_801C98F4(lbl_80562950, e->type));
        fn_801C94B8(400, 180, buf, -1);
        int y = 180 + h;
        sprintf(buf, "t: %.2f", e->time);
        fn_801C94B8(400, y, buf, -1);
        fn_801CA1C4(&lbl_8055E7F8[lbl_8055E828], 400, y + h, h);
    }
}

// Editor overlay draw: the status box (time, events, camera position/rotation/fov, a time line of the
// events), the current menu, the input line or the key help, the event list, and markers in the world for
// the selected event.
void fn_801CA67C(void) {
    int h = fn_80238A20();
    int bottom = (434 / h) * h;
    int lineH = h + 6;
    IVec3 rot;
    RotToAxisAngle((const Mat3*)&lbl_80499050, &rot);
    fn_8023895C();
    if (lbl_8055E7E0) {
        fn_80238B44(5, 5, 135, h + 15, lbl_8055E7DC.value);
        fn_80238A74(10, 10, "HIDDEN ('H')", -1);
        fn_802389E4();
        return;
    }
    u32 bg = lbl_8055E7DC.value;
    fn_80238B44(5, 5, 265, 110, bg);
    fn_80238A74(10, 10, "INTERLUDE", 0xFFFFFF00);
    fn_80238A74(110, 10, fn_801C5B60() ? "[ACTIVE]" : "[INACTIVE]", -1);
    f32 t = fn_801C3B6C();
    char buf[0x100];
    sprintf(buf, "T: %1.1f (%1.1f)", t, lbl_8055E7E4);
    fn_80238A74(10, 25, buf, -1);
    sprintf(buf, "E: %d/%d", lbl_8055E7F4, lbl_8055E7F0);
    fn_80238A74(110, 25, buf, -1);
    const CamPose* cam = (const CamPose*)&lbl_80499050;
    sprintf(buf, "C: %5.1f, %5.1f, %5.1f", cam->pos.x, cam->pos.y, cam->pos.z);
    fn_80238A74(10, 50, buf, -1);
    sprintf(buf, "R: %5.2f, %5.2f, %5.2f", rot.x, rot.y, rot.z);
    fn_80238A74(10, 65, buf, 0xFFC0C0C0);
    sprintf(buf, "F: %3.0f", lbl_8055F47C / 0.017453292f);
    fn_80238A74(200, 65, buf, 0xFFFFFF00);
    if (lbl_8055E7F0) {
        fn_80238B44(10, 90, 260, 105, 0x40000000);
        f32 dur = lbl_8055A958 > 0.01f ? lbl_8055A958 : 0.01f;
        f32 scale = 250.0f / dur;
        for (int i = 0; i < lbl_8055E7F0; i++) {
            int color = fn_801C34CC(lbl_8055E7F8[i].time, lbl_8055E7F8[lbl_8055E828].time) ? -1 : 0xFF000000;
            int x = (int)(0.5f + lbl_8055E7F8[i].time * scale) + 10;
            x = x <= 260 ? x : 260;
            fn_80238B44(x, 90, x + 1, 105, color);
        }
        int x = (int)(0.5f + t * scale) + 10;
        if (x > 260) {
            x = 260;
        }
        fn_80238B44(x, 90, x + 1, 105, 0xFFFFFF00);
    }
    Menu* m = &lbl_8055E86C[lbl_8055E81C];
    fn_801C94BC(398, 8, 600, lineH * (m->count + 1) + 10, bg);
    fn_801C94BC(398, 8, 600, h + 10, 0x32000000);
    fn_801C94B8(400, 10, m->title, -1);
    int y = lineH + 10;
    for (int i = 0; i < m->count; i++) {
        MenuItem* it = &m->items[i];
        char line[0x100];
        if (it->check) {
            sprintf(line, "[%s] %s", *it->check ? "X" : "  ", it->label);
        } else {
            strcpy(line, it->label);
        }
        fn_801C94B8(400, y, line, lbl_8055E820 == i ? 0xFFFFFF00 : -1);
        y += lineH;
    }
    if (lbl_8055E838) {
        fn_801C94BC(200, 200, 400, h + 212, bg);
        sprintf(buf, "%s_", lbl_804380F0);
        fn_801C94B8(206, 206, buf, -1);
        if (lbl_8055E83C) {
            fn_801C94B8(270, 5, "VALID ENTRIES", -1);
            int yy = h + 5;
            fn_801C94B8(270, yy, "(and combos of):", -1);
            yy += h;
            fn_801C94B8(270, yy, "all", -1);
            yy += h;
            fn_801C94B8(270, yy, "even", -1);
            yy += h;
            fn_801C94B8(270, yy, "odd", -1);
            yy += h;
            fn_801C94B8(270, yy, "first", -1);
            yy += h;
            fn_801C94B8(270, yy, "last", -1);
            fn_801C94B8(270, yy + h, "## (Car index in lineup)", -1);
        }
    } else {
        sprintf(buf, "%c - Un/Pause", 'p');
        fn_801C94B8(270, 5, buf, -1);
        int yy = h + 5;
        sprintf(buf, "%c - Restart", 'r');
        fn_801C94B8(270, yy, buf, -1);
        yy += h;
        sprintf(buf, "%c - Change Cam", 'c');
        fn_801C94B8(270, yy, buf, -1);
        yy += h;
        sprintf(buf, "%c - +0.1 Secs", 'i');
        fn_801C94B8(270, yy, buf, -1);
        yy += h;
        sprintf(buf, "%c - Save", 'l');
        fn_801C94B8(270, yy, buf, -1);
        sprintf(buf, "%c - Hide", 'h');
        fn_801C94B8(270, yy + h, buf, -1);
    }
    if (lbl_8055E839) {
        fn_801C94BC(10, 125, 260, h * 14 + 125, bg);
        fn_801C94B8(10, 125, "[ and ] Scroll", 0xE6969696);
        int yy = h * 2 + 125;
        int first = lbl_8055E82C * 10;
        for (int i = 0; i < 10; i++) {
            int k = first + i;
            if (k >= lbl_8055E7F0) {
                break;
            }
            Event* e = &lbl_8055E7F8[k];
            sprintf(buf, "%5.2f   %4d   %s", e->time, e->order, fn_801C98F4(lbl_80562950, e->type));
            fn_801C94B8(10, yy, buf, k == lbl_8055E828 ? 0xFFFFFF00 : -1);
            yy += h;
        }
        if (first < lbl_8055E7F0 - 10) {
            fn_801C94B8(10, yy, "  v", -1);
            fn_801C94B8(10, yy + h - 10, "  v", -1);
        }
        fn_801CA5A8();
    }
    fn_802389E4();
    if (lbl_8055E83A) {
        fn_801C9830();
    }
    if (lbl_8055E7F0 > 0 && lbl_8055E828 < lbl_8055E7F0) {
        fn_802C8638(fn_801E8718(0));
        fn_801EA478(0);
        Event ev = lbl_8055E7F8[lbl_8055E828];
        switch (ev.type) {
        case 6:
            fn_801E3FC8(&((WaitEvent*)&ev)->pt, 2.0f, 0xFFFFFF00);
            break;
        case 4: {
            TeleportEvent* tp = (TeleportEvent*)&ev;
            if (tp->where == 0) {
                s16 hint;
                fn_801E9DC4(&hint);
                IVec3 p;
                p.x = tp->pt.x;
                p.y = 0.0f;
                p.z = tp->pt.y;
                p.y = fn_801E9DDC(&hint, tp->pt.x, tp->pt.y);
                fn_801E3FC8(&p, 2.0f, 0xFF0000FF);
            }
            break;
        }
        case 5: {
            ThinkEvent* th = (ThinkEvent*)&ev;
            if (th->kind == 2) {
                s16 hint;
                fn_801E9DC4(&hint);
                IVec3 p;
                p.x = th->x;
                p.y = 0.0f;
                p.z = th->z;
                p.y = fn_801E9DDC(&hint, th->x, th->z);
                fn_801E3FC8(&p, 2.0f, 0xFF00FF00);
            }
            break;
        }
        }
    }
    fn_801C94C0(10, bottom);
}

// Dead-stripped from the game: only its message is left in the string pool (after the overlay's).
// The body is reconstructed from the message.
void InterludeSkipToEnd(void) {
    if (!lbl_8055E7E2) {
        fn_802E30C8("InterludeSkipToEnd: interlude not active!");
        return;
    }
    lbl_8055E7E4 = lbl_8055A958;
}

int fn_801CB114(int key) {
    if (lbl_8055E838) {
        return fn_801C9708(key);
    }
    if (key == 'h') {
        lbl_8055E7E0 = !lbl_8055E7E0;
    }
    if (lbl_8055E7E0) {
        return 0;
    }
    switch (key) {
    case 'c':
        lbl_8055E80C = !lbl_8055E80C;
        break;
    case 'r':
        fn_80176780(0x4200, 0);
        lbl_8055E7E1 = 1;
        break;
    case 'l':
        fn_801CA070("intrlude.txt");
        break;
    case 'i':
        fn_801C9514();
        break;
    case 0x126:
        fn_801C968C(-1);
        break;
    case 0x128:
        fn_801C968C(1);
        break;
    case '[':
        if (lbl_8055E839) {
            if (lbl_8055E828 > 0) {
                lbl_8055E828--;
            }
            fn_801C96E4();
        }
        break;
    case ']':
        if (lbl_8055E839) {
            if (lbl_8055E828 < lbl_8055E7F0 - 1) {
                lbl_8055E828++;
            }
            fn_801C96E4();
        }
        break;
    case 0xD:
        fn_801C95EC();
        break;
    }
    return 0;
}

}  // extern "C"

ModuleTag lbl_8055E7D0;
UnkStaticFFFF lbl_8055E7D8;
UnkStatic7F lbl_8055E7DC;
