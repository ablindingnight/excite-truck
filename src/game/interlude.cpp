// Interludes (0x801C2F58-0x801CB2D8): scripted race intros/outros from "intrlude.val" ("action_list"
// of timed events: camera moves, fades, sounds, car teleports, AI think orders, waits and text), plus the
// in-game interlude editor (menus driven by the debug keyboard, an input line, an event list that can be
// re-timed and reordered, and export to "intrlude.txt"). The loader, the per-frame player, the event
// dispatcher, the editor's draw/menu set-up and the exporter's per-event writer are placeholders.
// Names are guesses apart from the strings.
#include "game/common.h"

struct IVec3 {
    f32 x;
    f32 y;
    f32 z;
};

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
    s32 imm;      // offset 0x10
    u8 overlap;   // offset 0x14
    u8 b15;       // offset 0x15
};

struct TextEvent {
    f32 time;
    s32 order;
    s32 type;
    u8 pad0C[0x10C - 0xC];
    char text[0x200];  // offset 0x10C
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
    u8 pad0C[0x58 - 0xC];
    f32 end;  // offset 0x58
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
    s32 pad;
};

struct Menu {
    MenuItem* items;  // offset 0x0
    s32 pad4;
    s32 count;        // offset 0x8
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
int sprintf(char* dst, const char* fmt, ...);
double atof(const char* s);
int atoi(const char* s);
void qsort(void* base, size_t n, size_t size, int (*cmp)(const void*, const void*));
double cos(double x);
double sin(double x);
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
void fn_80238B44(int x, int y, const char* s, int color);
int fn_802E367C(const char* name);
void fn_802E3734(int* fd);
void fn_802E3AD0(int fd, const char* fmt, ...);
}

extern f32 lbl_8055E3C8;    // time
extern f32 lbl_80558DC4[];  // FLT_EPSILON
extern f32 lbl_8055F47C;
extern IMtx lbl_80499050;   // current camera

extern bool lbl_8055E7E0;     // hidden
extern u8 lbl_8055E7E1;
extern u8 lbl_8055E7E2;     // active
extern u8 lbl_8055E7E3;
extern f32 lbl_8055E7E4;    // start offset
extern u8 lbl_8055E7E8;
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
extern "C" void IL_Use(int k, ...);  // placeholder sink: keeps the original strings in the pool

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

static inline void FrameToQuat(const IFrame* f, IQuat* q) {
    const f32(*m)[3] = (const f32(*)[3])f;
    f32 tr = f->axes[0].x + f->axes[1].y + f->axes[2].z;
    if (tr > 0.0f) {
        f32 s = sqrt(tr + 1.0f);
        q->w = 0.5f * s;
        s = 0.5f / s;
        q->x = (m[1][2] - m[2][1]) * s;
        q->y = (m[2][0] - m[0][2]) * s;
        q->z = (m[0][1] - m[1][0]) * s;
    } else {
        FrameToQuatAxis((f32(*)[3])f, q);
    }
}

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
    pos.x = a->pos.x + t * (b->pos.x - a->pos.x);
    pos.y = a->pos.y + t * (b->pos.y - a->pos.y);
    pos.z = a->pos.z + t * (b->pos.z - a->pos.z);
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

// Interlude loader (placeholder; strings and switch keep the pool layout).
void fn_801C3D88(const char* name) {
    int k = (int)name;
    IL_Use(0, "interlude: can't find %s", "Interlude: Couldn't find interlude file %s!!!", "interlude_name", "Interlude: Couldn't find interlude %s in val file %s!!!", "duration", "intrlude: too many events!", "camera_type", "interp_type", "interp_time", "sound_type", "immediate", "interlude: unhandled teleport where type %d", "interlude: unhandled wait type %d", "interlude: unhandled event type %d", "intrlude: %d events");
    switch (k) {
    case 0:
        IL_Use(0);
        break;
    case 1:
        IL_Use(1);
        break;
    case 2:
        IL_Use(2);
        break;
    case 3:
        IL_Use(3);
        break;
    case 4:
        IL_Use(4);
        break;
    case 5:
        IL_Use(5);
        break;
    case 6:
        IL_Use(6);
        break;
    case 7:
        IL_Use(7);
        break;
    }
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

// Event dispatch, player update and the per-event start (placeholders).
void fn_801C4CE8(int k) {
    IL_Use(0, "====== INTERLUDE: DONE at %1.1f! =======", "interlude: reset completed", "intrlude: unhandled wait type %d", "debug_event: %s", "interlude: song %u (I:%d, O:%d)", "interlude: unhandled teleport where %d");
    switch (k) {
    case 0:
        IL_Use(0);
        break;
    case 1:
        IL_Use(1);
        break;
    case 2:
        IL_Use(2);
        break;
    case 3:
        IL_Use(3);
        break;
    case 4:
        IL_Use(4);
        break;
    case 5:
        IL_Use(5);
        break;
    case 6:
        IL_Use(6);
        break;
    case 7:
        IL_Use(7);
        break;
    }
}

void fn_801C56AC(void) {}

u8 fn_801C5B60(void) { return lbl_8055E7E2; }

void fn_801C5B68(void) {
    IL_Use(0, "intrlude: unknown camtype %d");
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

// Camera event start/update (placeholder).
void fn_801C6E40(void) {
    IL_Use(0, "Camera added at %.2f seconds");
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
    e->b15 = lbl_8055E83F;
    e->overlap = lbl_8055E83E;
    e->imm = lbl_8055E840 != 0;
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

// Add teleport (placeholder).
void fn_801C7C04(void) {
    IL_Use(0, "Interlude Editor: I'm not set up to handle where type %d", "Added teleport to (%.2f, %.2f) at %.2f");
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

// Editor menu set-up (placeholder).
void fn_801C855C(void) {
    IL_Use(0, "INTERLUDE EDITOR", "Add text message...", "Set camera here...", "Add effect...", "Add sound...", "Teleport cars...", "Set AI thinking...", "Wait until...", "List events...", "Set duration", "Reload Interlude", "Global event time...", "GLOBAL EVENT TIME", "Turn Off", "SET GLOBAL TIME", "REALLY RELOAD?", "Yes, Reload", "SET DURATION", "Set Duration", "ENTER MSG TO DISPLAY", "Set Message", "Add Fadeout", "MOVE CAM TO DEST", "Ok-I'm at the destination", "TRANSITION TIME? (SECS)", "Set Camera", "TYPE OF SOUND", "Immediate", "Set Sample...", "Set Sound", "SET SAMPLE ID", "Set Sample", "TELEPORT TYPE", "Starting positions", "World point", "Continue", "TELEPORT WHO?", "EXCEPT WHO?", "TELEPORT WHERE?", "FACING WHERE?", "WHERE IS 2ND CAR?", "CAR SPEED?", "DO WHAT?", "Move to point", "Use Line", "Follow with Line", "TO WHERE?", "FOLLOW WHO?", "WAIT TYPE", "Cross Point", "WAIT FOR WHOM?", "EXCEPT FOR WHOM?", "WHAT POINT?", "Set wait point", "EVENT LIST", "Move down", "Change time", "Insert/Del time", "REALLY DELETE?", "SET TIME", "Set time", "Add time");
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

void fn_801C94BC(int x, int y, const char* s, int color) { fn_80238B44(x, y, s, color); }

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

// Per-event export writer (placeholder).
void fn_801C9920(const Event* e, int fd) {
    int k = e->type;
    IL_Use(0, "\ttable action\r\n", "\t\tt\t%.2f\r\n", "\t\ttype\tdebug\r\n", "\t\tmsg\t%s\r\n", "\t\ttype\tcamera\r\n", "\t\tfov\t%.2f\r\n", "\t\torigin\t%.2f, %.2f, %.2f\r\n", "\t\tdir\t%.2f, %.2f, %.2f\r\n", "\t\tv\t%.2f, %.2f, %.2f\r\n", "\t\tav\t%.2f, %.2f, %.2f\r\n", "\t\tduration\t%.2f\r\n", "\t\taccel\t%s\r\n", "\t\ttype\teffect\r\n", "\t\ttype\tsound\r\n", "\t\tsound_type\t%s\r\n", "\t\toverlap\t%s\r\n", "\t\timmediate\t%s\r\n", "\t\tsample\t%u\r\n", "\t\ttype\tteleport\r\n", "\t\tspeed\t%.2f\r\n", "\t\twho\t%s\r\n", "\t\texcept\t%s\r\n", "\t\twhere\t%s\r\n", "\t\toffset\t%.2f\r\n", "\t\tspacing\t%.2f\r\n", "\t\tyaw\t%.2f\r\n", "\t\tpt\t%.2f, %.2f\r\n", "\t\toffset\t%.2f, %.2f\r\n", "interlude: unhandled teleport where type %d in export", "\t\ttype\tthink\r\n", "\t\thow\t%s\r\n", "\t\tlat\t%.2f\r\n", "\t\tlon\t%.2f\r\n", "\t\tparam\t%d\r\n", "\t\ttype\twait\r\n", "\t\twhat\t%s\r\n", "\t\tpt\t%.2f, %.2f, %.2f\r\n", "interlude: unhandled wait type %d in export", "\t\ttype\ttext\r\n", "\t\ttitle\t%s\r\n", "interlude: unhandled event type %d in export");
    switch (k) {
    case 0:
        IL_Use(0);
        break;
    case 1:
        IL_Use(1);
        break;
    case 2:
        IL_Use(2);
        break;
    case 3:
        IL_Use(3);
        break;
    case 4:
        IL_Use(4);
        break;
    case 5:
        IL_Use(5);
        break;
    case 6:
        IL_Use(6);
        break;
    case 7:
        IL_Use(7);
        break;
    }
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
        sprintf(buf, "Sound type: %s", fn_801C98F4(lbl_80562960, ((const SoundEvent*)e)->imm));
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

// Editor overlay draw (placeholder).
void fn_801CA67C(void) {
    IL_Use(0, "HIDDEN ('H')", "INTERLUDE", "[ACTIVE]", "[INACTIVE]", "T: %1.1f (%1.1f)", "E: %d/%d", "C: %5.1f, %5.1f, %5.1f", "R: %5.2f, %5.2f, %5.2f", "F: %3.0f", "VALID ENTRIES", "(and combos of):", "## (Car index in lineup)", "%c - Un/Pause", "%c - Restart", "%c - Change Cam", "%c - +0.1 Secs", "%c - Save", "%c - Hide", "[ and ] Scroll", "%5.2f   %4d   %s", "InterludeSkipToEnd: interlude not active!");
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
