// Coin/star pickups (0x80167874-0x8016A0B0): the event's "coin_file" table is read by four makers: coin
// strips along a bound (rows of coins, the rainbow strip cycles colours), coin boxes (a grid filling a four
// point bound, its colour pattern by ring), single coins/stars at bound points and "poop" lists of loose
// coins. Pickups are kept in groups (one per table, with a bounding circle) and drawn spinning; a car
// collects a pickup when it falls in the car's footprint. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
int strncasecmp(const char* a, const char* b, u32 n);
u32 strlen(const char* s);
int sscanf(const char* s, const char* fmt, ...);
double sqrt(double x);
double cos(double x);
double sin(double x);
void* memset(void* dst, int c, u32 n);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* t);
void fn_80128658(void* p);  // MemFree
s32 fn_802C6FC0(const char* name, int a, int b);
void fn_802C6CF8(s32 model);
void fn_802C77F0(s32 model, f32* box);
void fn_801E9E30(s16* h, f32* pos);
void fn_801EA1E0(f32 x, f32 z, f32* y, int a);
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

struct V2 {
    f32 x;
    f32 z;
};

typedef V2 BoundPt;

struct Bound {
    s32 n;           // offset 0x0
    s32 cap;         // offset 0x4
    BoundPt pts[1];  // offset 0x8
};

struct CarDims {
    u8 pad0[0x24];
    f32 wid;  // offset 0x24
    u8 pad28[0x4];
    f32 len;  // offset 0x2C
};

struct CarModel {
    u8 pad0[0x17C];
    CarDims* dims;  // offset 0x17C
};

struct RaceCarRec {
    u8 pad0[0x14];
    CarModel* model;  // offset 0x14
    u8 pad18[0x24 - 0x18];
    s32 hasModel;  // offset 0x24
    u8 pad28[0x3C - 0x28];
    f32 rot[9];  // offset 0x3C
    f32 pos[3];  // offset 0x60
};

struct Camera {
    u8 pad0[0x24];
    f32 x;  // offset 0x24
    f32 y;  // offset 0x28
    f32 z;  // offset 0x2C
};

extern "C" {
Bound* fn_801399FC(ValueNode* n);
V2 fn_80139C5C(Bound* b);
RaceCarRec* fn_8017960C(int id);
Camera* fn_802C86DC(void);
int fn_802C9994(u32 c);
u32 fn_802C9950(u32 c);
void fn_802C9900(f32 a);
void fn_802C9904(void);
}

struct SpinXform {
    SpinXform(void) {}
    f32 rot[9];  // offset 0x0
    f32 pos[3];  // offset 0x24
};

extern "C" void fn_802C744C(s32 model, const SpinXform* m);

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
    T& At(int i) { return data[i]; }
    T& Back(void) { return At(count - 1); }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

template <class T, int N>
struct FixedArr : ArrayRef<T> {
    FixedArr(void) : ArrayRef<T>(storage, N) {}
    T storage[N];
};

struct PickupType {
    s32 type;           // offset 0x0
    const char* model;  // offset 0x4
    u32 color;          // offset 0x8
    u32 color2;         // offset 0xC
    s32 handle;         // offset 0x10
    f32 radius;         // offset 0x14
    f32 yoff;           // offset 0x18
    s32 count;          // offset 0x1C
};

PickupType lbl_80330020[] = {
    {0, "growcoin.mod", 0xFFFFFF00, 0x5FFFFFFF},
    {1, "growcoin.mod", 0xFFFFFF00, 0x5FFFFFFF},
    {2, "growcoin.mod", 0xFFFF0000, 0x5FFFFFFF},
    {3, "growcoin.mod", 0xFF0000FF, 0x5FFFFFFF},
    {4, "growstar.mod", 0xFFFFFF00, 0x5FFFFFFF},
    {6},
};

struct Pickup {
    Pickup(void) { type = 0; }
    ~Pickup(void) { type = 0; }
    int Live(void) { return type && !taken; }
    PickupType* type;  // offset 0x0
    f32 pos[3];        // offset 0x4
    f32 hover;         // offset 0x10
    u8 taken;          // offset 0x14
};

struct PickupGroup {
    s32 id;       // offset 0x0
    u8 enabled;   // offset 0x4
    V2 c;         // offset 0x8
    f32 radius;   // offset 0x10
    s32 first;    // offset 0x14
    s32 count;    // offset 0x18
    u8 hasBound;  // offset 0x1C
    u8 done;      // offset 0x1D
    void On(int i) {
        if (i == id) {
            enabled = 1;
        }
    }
    void Off(int i) {
        if (i == id) {
            enabled = 0;
        }
    }
};

// Car footprint test history: the position the car had last frame.
struct PickupTrack {
    PickupTrack(void) {}
    s32 frame;   // offset 0x0
    f32 pos[3];  // offset 0x4
};

struct PickupCounts {
    s32 n[5];  // offset 0x0
    s32 done;  // offset 0x14
};

extern "C" PickupType* fn_80167874(int type);

struct PickupObjMan {
    PickupObjMan(void);
    ~PickupObjMan(void);
    void BeginGroup(Bound* b, int id);
    void EnableGroups(int id) {
        int i;
        for (i = 0; i < groups.count; i++) {
            groups.data[i].On(id);
        }
    }
    void Add(int type, const V2& pos, f32 hover);
    FixedArr<Pickup, 5000> objs;       // offset 0x0
    FixedArr<PickupGroup, 100> groups;  // offset 0x1D4CC
    u8 grouping;                        // offset 0x1E158
};

inline void PickupObjMan::BeginGroup(Bound* b, int id) {
    PickupGroup* g = &groups.data[groups.count++];
    if (b) {
        int i;
        g->hasBound = 1;
        g->c = fn_80139C5C(b);
        g->radius = 0.0f;
        for (i = 0; i < b->n; i++) {
            V2 d;
            d.z = g->c.z - b->pts[i].z;
            d.x = g->c.x - b->pts[i].x;
            g->radius = MAX(g->radius, d.x * d.x + d.z * d.z);
        }
        g->radius = 1.1f * (f32)sqrt(g->radius);
    } else {
        g->hasBound = 0;
        g->c.x = 0.0f;
        g->c.z = 0.0f;
        g->radius = 0.0f;
    }
    g->first = objs.count;
    g->count = 0;
    g->done = 0;
    g->id = id;
    g->enabled = 1;
    grouping = 1;
}

inline void PickupObjMan::Add(int type, const V2& pos, f32 hover) {
    Pickup* p = &objs.data[objs.count++];
    p->type = 0;
    p->type = fn_80167874(type);
    p->pos[0] = pos.x;
    p->pos[1] = 0.0f;
    p->pos[2] = pos.z;
    p->hover = hover;
    fn_801E9E30(0, p->pos);
    p->pos[1] += p->type->yoff;
    p->pos[1] += p->hover;
    p->taken = 0;
    fn_801EA1E0(p->pos[0], p->pos[2], &p->pos[1], 1);
    fn_80167874(type)->count++;
    if (grouping) {
        groups.Back().count++;
    } else {
        if (groups.count == 0 || groups.Back().hasBound) {
            PickupGroup* g = &groups.data[groups.count++];
            g->hasBound = 0;
            g->first = objs.count - 1;
            g->count = 0;
            g->done = 0;
        }
        groups.Back().count++;
    }
}

namespace {

class PickupMaker {
public:
    virtual ~PickupMaker(void) {}
    virtual int Match(const char* name) = 0;
    virtual int Make(ValueNode* n, PickupObjMan* m, const f32* params) = 0;
};

class CoinStripPickupMaker : public PickupMaker {
public:
    virtual ~CoinStripPickupMaker(void) {}
    virtual int Match(const char* name);
    virtual int Make(ValueNode* n, PickupObjMan* m, const f32* params);
};

class CoinBoxPickupMaker : public PickupMaker {
public:
    virtual ~CoinBoxPickupMaker(void) {}
    virtual int Match(const char* name);
    virtual int Make(ValueNode* n, PickupObjMan* m, const f32* params);
};

class SinglesPickupMaker : public PickupMaker {
public:
    virtual ~SinglesPickupMaker(void) {}
    virtual int Match(const char* name);
    virtual int Make(ValueNode* n, PickupObjMan* m, const f32* params);
};

class PoopPickupMaker : public PickupMaker {
public:
    virtual ~PoopPickupMaker(void) {}
    virtual int Match(const char* name);
    virtual int Make(ValueNode* n, PickupObjMan* m, const f32* params);
};

}  // namespace

ModuleTag lbl_8055E348;
UnkStaticFFFF lbl_8055E350;
PickupObjMan* lbl_8055E354;

SpinXform lbl_8042FF60[16];
PickupTrack lbl_80430260[8];

extern s32 lbl_8055E3C4;  // frame
extern f32 lbl_8055E460;

inline PickupObjMan::PickupObjMan(void) {
    PickupType* t = lbl_80330020;
    int i;
    for (i = 0; i < 5; i++) {
        t->handle = fn_802C6FC0(t->model, 0, 0);
        t->count = 0;
        if (t->handle != -1) {
            f32 box[6];
            f32 m;
            f32 w;
            fn_802C77F0(t->handle, box);
            m = box[5] - box[4];
            w = box[3] - box[2];
            m = w > m ? w : m;
            w = box[1] - box[0];
            m = w > m ? w : m;
            t->radius = 0.5f * m;
            t->yoff = -box[2];
        }
        t++;
    }
    grouping = 0;
}

extern "C" {

PickupType* fn_80167874(int type) {
    PickupType* t;
    for (t = lbl_80330020; t->type != 6; t++) {
        if (t->type == type) {
            return t;
        }
    }
    fn_802E317C("get_pickup_type_info: invalid type %d passed!", type);
    return lbl_80330020;
}

}  // extern "C"

struct StripName {
    const char* prefix;  // offset 0x0
    const char* fmt;     // offset 0x4
    s32 type;            // offset 0x8
};

struct StripTable {
    StripName e[6];
};

const StripTable lbl_802F94B8 = {{
    {"coin_strip_blue", "coin_strip_blue_%d_%d", 3},
    {"coin_strip_red", "coin_strip_red_%d_%d", 2},
    {"coin_strip_yellow", "coin_strip_yellow_%d_%d", 1},
    {"coin_strip_star", "coin_strip_star_%d_%d", 4},
    {"coin_strip", "coin_strip_%d_%d", 1},
    {0, 0, 6},
}};

struct RainbowTable {
    s32 t[5];
};

const RainbowTable lbl_802F9500 = {{1, 2, 3, 2, 1}};

extern "C" {

void fn_801678D8(void) {
    f32 c;
    SpinXform* m = lbl_8042FF60;
    int i;
    f32 t = 0.016683351f * ((f32)lbl_8055E3C4 + lbl_8055E460);
    for (i = 0; i < 16; i++) {
        c = cos(6.2831855f * (t + (f32)i / 16.0f));
        f32 s = sin(6.2831855f * (t + (f32)i / 16.0f));
        m->rot[0] = c;
        m->rot[1] = 0.0f;
        m->rot[2] = -s;
        m->rot[3] = 0.0f;
        m->rot[4] = 1.0f;
        m->rot[5] = 0.0f;
        m->rot[6] = s;
        m->rot[7] = 0.0f;
        m->rot[8] = c;
        m++;
    }
}

void fn_80168020(void);

void fn_801679F0(void) {
    ValueNode* vals = GetEvent(fn_801769CC()->event)->vals;
    CoinStripPickupMaker strip;
    CoinBoxPickupMaker box;
    SinglesPickupMaker singles;
    PoopPickupMaker poop;
    PickupMaker* makers[] = {&poop, &strip, &box, &singles, 0};
    lbl_8055E354 = new ("PickupObjMan") PickupObjMan;
    if (vals->Has("coin_file")) {
        f32 params[3] = {4.0f, 30.0f, 10.0f};
        ValueTable* t;
        bool has = vals->FindChild("coin_params") != 0;
        if (has) {
            ValueNode* p = vals->FindChild("coin_params");
            params[0] = p->GetFloat("box_step_size", params[0]);
            params[1] = p->GetFloat("strip_step_size", params[1]);
            params[2] = p->GetFloat("strip_wid_step_size", params[2]);
        }
        t = fn_80131810(vals->GetValue("coin_file"));
        if (t) {
            ValueNode* n;
            int i;
            for (i = 0; (n = t->GetChild(i)) != 0; i++) {
                PickupMaker** m;
                for (m = makers; *m; m++) {
                    if ((*m)->Match(n->GetName())) {
                        (*m)->Make(n, lbl_8055E354, params);
                        break;
                    }
                }
            }
            fn_80131818(t);
        }
    }
    fn_80168020();
}

}  // extern "C"

inline PickupObjMan::~PickupObjMan(void) {
    int i;
    for (i = 0; i < 5; i++) {
        lbl_80330020[i].count = 0;
    }
    {
        int n = objs.count;
        while (n-- > 0) {
            objs.data[n].type = 0;
        }
    }
    objs.count = 0;
    groups.count = 0;
    grouping = 0;
    for (i = 0; i < 5; i++) {
        if (lbl_80330020[i].handle != -1) {
            fn_802C6CF8(lbl_80330020[i].handle);
            lbl_80330020[i].handle = -1;
        }
    }
}

extern "C" {

void fn_80167EEC(void) { delete lbl_8055E354; }

void fn_80168020(void) {
    int i;
    PickupObjMan* m = lbl_8055E354;
    for (i = 0; i < m->objs.count; i++) {
        Pickup* p = &m->objs.data[i];
        fn_801E9E30(0, p->pos);
        p->pos[1] = p->hover + (p->pos[1] + p->type->yoff);
        p->taken = 0;
    }
    for (i = 0; i < m->groups.count; i++) {
        m->groups.data[i].done = 0;
    }
    for (i = 0; i < 8; i++) {
        lbl_80430260[i].frame = -1;
    }
}

void fn_80168104(void) {
    Camera* cam = fn_802C86DC();
    V2 c;
    int i;
    c.x = cam->x;
    c.z = cam->z;
    for (i = 0; i < lbl_8055E354->groups.count; i++) {
        PickupGroup* g = &lbl_8055E354->groups.data[i];
        if (g->enabled) {
            if (g->hasBound) {
                V2 d;
                d.z = c.z - g->c.z;
                d.x = c.x - g->c.x;
                if (!((f32)sqrt(d.x * d.x + d.z * d.z) - g->radius < 600.0f)) {
                    continue;
                }
            }
            {
                int j;
                for (j = 0; j < lbl_8055E354->groups.data[i].count; j++) {
                    Pickup* p = &lbl_8055E354->objs.data[lbl_8055E354->groups.data[i].first + j];
                    if (p->type && p->type->handle && !p->taken) {
                        u32 col = p->type ? p->type->color : 0xFFFFFFFF;
                        int o1 = fn_802C9994(col);
                        u32 o2 = fn_802C9950(((col >> 1) & 0x7F000000) | ((col >> 1) & 0x7F0000) |
                                             ((col >> 1) & 0x7F00) | ((col >> 1) & 0x7F));
                        SpinXform* x;
                        fn_802C9900((f32)(col >> 24) / 255.0f);
                        x = &lbl_8042FF60[((s32)p->type / 12) & 15];
                        x->pos[0] = p->pos[0];
                        x->pos[1] = p->pos[1];
                        x->pos[2] = p->pos[2];
                        fn_802C744C(p->type->handle, x);
                        fn_802C9994(o1);
                        fn_802C9950(o2);
                        fn_802C9904();
                    }
                }
            }
        }
    }
}

void fn_80168328(void) {
    PickupObjMan* m = lbl_8055E354;
    int i;
    fn_801678D8();
    for (i = 0; i < m->groups.count; i++) {
        PickupGroup* g = &m->groups.data[i];
        if (!g->done) {
            int j;
            g->done = 1;
            for (j = 0; j < g->count; j++) {
                if (m->objs.data[g->first + j].Live()) {
                    g->done = 0;
                    break;
                }
            }
        }
    }
}

void fn_801683F4(PickupCounts* out, int player) {
    PickupCounts res;
    RaceCarRec* rec;
    memset(&res, 0, sizeof(res));
    rec = fn_8017960C(player);
    if (lbl_8055E3C4 - 1 == lbl_80430260[player].frame) {
        int done = 0;
        f32 r = 0.0f;
        V2 pos;
        V2 lo;
        V2 hi;
        CarModel* car;
        int i;
        pos.x = rec->pos[0];
        pos.z = rec->pos[2];
        lo.x = 0.0f;
        lo.z = 0.0f;
        hi.x = 0.0f;
        hi.z = 0.0f;
        car = rec->hasModel > 0 ? rec->model : 0;
        if (car) {
            f32 l[3];
            f32 dz = lbl_80430260[player].pos[2] - rec->pos[2];
            f32 dx = lbl_80430260[player].pos[0] - rec->pos[0];
            f32 dy = lbl_80430260[player].pos[1] - rec->pos[1];
            CarDims* dims;
            l[0] = dx * rec->rot[0] + dy * rec->rot[1] + dz * rec->rot[2];
            l[1] = dx * rec->rot[3] + dy * rec->rot[4] + dz * rec->rot[5];
            l[2] = dx * rec->rot[6] + dy * rec->rot[7] + dz * rec->rot[8];
            dims = car->dims;
            lo.x = MIN(l[0], 0.0f) - 1.1f * dims->wid;
            lo.z = MIN(l[2], 0.0f) - 1.1f * dims->len;
            hi.x = MAX(l[0], 0.0f) + 1.1f * dims->wid;
            hi.z = MAX(l[2], 0.0f) + 1.1f * dims->len;
            r = sqrt(dims->wid * dims->wid + dims->len * dims->len);
        }
        for (i = 0; i < lbl_8055E354->groups.count; i++) {
            PickupGroup* g = &lbl_8055E354->groups.data[i];
            if (g->done) {
                done++;
            } else if (g->enabled) {
                int j;
                if (g->hasBound) {
                    V2 d;
                    d.z = pos.z - g->c.z;
                    d.x = pos.x - g->c.x;
                    if (!((f32)sqrt(d.x * d.x + d.z * d.z) < r + g->radius)) {
                        continue;
                    }
                }
                for (j = 0; j < g->count; j++) {
                    Pickup* p = &lbl_8055E354->objs.data[lbl_8055E354->groups.data[i].first + j];
                    if (p->Live()) {
                        f32 l[3];
                        f32 dx = p->pos[0] - rec->pos[0];
                        f32 dy = p->pos[1] - rec->pos[1];
                        f32 dz = p->pos[2] - rec->pos[2];
                        f32 rad;
                        l[0] = dx * rec->rot[0] + dy * rec->rot[1] + dz * rec->rot[2];
                        l[1] = dx * rec->rot[3] + dy * rec->rot[4] + dz * rec->rot[5];
                        l[2] = dx * rec->rot[6] + dy * rec->rot[7] + dz * rec->rot[8];
                        rad = p->type ? p->type->radius : 0.0f;
                        if (l[0] + rad > lo.x && l[0] - rad < hi.x && l[2] + rad > lo.z && l[2] - rad < hi.z) {
                            p->taken = 1;
                            res.n[p->type ? p->type->type : 6]++;
                            break;
                        }
                    }
                }
            }
        }
        res.done = done;
    }
    lbl_80430260[player].pos[0] = rec->pos[0];
    lbl_80430260[player].pos[1] = rec->pos[1];
    lbl_80430260[player].pos[2] = rec->pos[2];
    lbl_80430260[player].frame = lbl_8055E3C4;
    *out = res;
}

void fn_801688B4(int id) { lbl_8055E354->EnableGroups(id); }

void fn_801688FC(int id) {
    PickupObjMan* m = lbl_8055E354;
    int i;
    for (i = 0; i < m->groups.count; i++) {
        m->groups.data[i].Off(id);
    }
}

}  // extern "C"

static inline V2 Lerp2(const V2& a, const V2& b, f32 t) {
    V2 r;
    r.x = a.x + t * (b.x - a.x);
    r.z = a.z + t * (b.z - a.z);
    return r;
}

static inline f32 Dist(const BoundPt* a, const BoundPt* b) {
    V2 d;
    d.z = b->z - a->z;
    d.x = b->x - a->x;
    return sqrt(d.x * d.x + d.z * d.z);
}

inline int CoinStripPickupMaker::Match(const char* name) {
    const char* p = "coin_strip";
    return strncasecmp(name, p, strlen(p)) == 0;
}

inline int CoinStripPickupMaker::Make(ValueNode* n, PickupObjMan* m, const f32* params) {
    Bound* b;
    int rainbow;
    int count;
    int id;
    int type;
    f32 len;
    int steps;
    f32 spacing;
    int k;
    if (!Match(n->GetName())) {
        fn_802E30C8("CoinStripPickupMaker: table name \"%s\" is not a match", n->GetName());
        return 0;
    }
    b = fn_801399FC(n);
    if (b->n < 2) {
        fn_802E30C8("CoinStripPickupMaker: bound for table \"%s\" has less than two points", n->GetName());
        fn_80128658(b);
        return 0;
    }
    rainbow = strcasecmp(n->GetName(), "coin_strip_rainbow") == 0;
    count = 1;
    id = -1;
    type = 6;
    if (rainbow) {
        count = 5;
    } else {
        StripTable names = lbl_802F94B8;
        StripName* s;
        for (s = names.e; s->type != 6; s++) {
            const char* pre = s->prefix;
            if (strncasecmp(n->GetName(), pre, strlen(pre)) == 0) {
                break;
            }
        }
        if (!s) {
            fn_802E30C8("CoinStrip: table name \"%s\" doesn't make much sense to me!", n->GetName());
            type = 1;
        } else {
            sscanf(n->GetName(), s->fmt, &count, &id);
            type = s->type;
        }
    }
    len = 0.0f;
    for (k = 1; k < b->n; k++) {
        len += Dist(&b->pts[k - 1], &b->pts[k]);
    }
    steps = 1;
    {
        int s = (int)(len / params[1]);
        if (s > 1) {
            steps = s;
        }
    }
    spacing = len / (f32)steps;
    m->BeginGroup(b, id);
    {
        RainbowTable rainbowTypes = lbl_802F9500;
        f32 along = 0.0f;
        BoundPt* seg = b->pts;
        int si = 1;
        int i;
        for (i = 0; i <= steps; i++) {
            int last = b->n - 1;
            f32 t;
            V2 dir;
            f32 px;
            f32 pz;
            V2 base;
            f32 inv;
            int c;
            s32* rt;
            f32 d;
            while (si < last && (d = Dist(&seg[0], &seg[1])) < along) {
                along -= d;
                seg++;
                si++;
            }
            t = along / Dist(&seg[0], &seg[1]);
            if (1.0f < t) {
                t = 1.0f;
            }
            dir.x = seg[1].x - seg[0].x;
            dir.z = seg[1].z - seg[0].z;
            {
                V2 perp = dir;
                f32 x = perp.x;
                perp.x = perp.z;
                perp.z = -x;
                inv = 1.0f / (f32)sqrt(perp.x * perp.x + perp.z * perp.z);
                base.x = seg[0].x + dir.x * t;
                base.z = seg[0].z + dir.z * t;
                perp.x *= inv;
                perp.z *= inv;
                px = perp.x;
                pz = perp.z;
            }
            rt = rainbowTypes.t;
            for (c = 0; c < count; c++) {
                f32 off = params[2] * (0.5f + ((f32)c - 0.5f * (f32)count));
                V2 p;
                int ty;
                p.x = base.x + px * off;
                p.z = base.z + pz * off;
                if (rainbow) {
                    ty = *rt;
                } else {
                    ty = type;
                }
                m->Add(ty, p, 0.0f);
                rt++;
            }
            along += spacing;
        }
    }
    m->grouping = 0;
    fn_80128658(b);
    return 1;
}

inline int CoinBoxPickupMaker::Match(const char* name) {
    const char* p = "coin_box";
    return strncasecmp(name, p, strlen(p)) == 0;
}

extern "C" static inline int fn_801698B8(int j, int i, int ny, int nx) {
    int lo;
    int hi;
    int v;
    if (j > ny - j) {
        j = ny - j;
    }
    if (i > nx - i) {
        i = nx - i;
    }
    lo = i < j ? i : j;
    hi = i > j ? i : j;
    v = hi + lo / 2;
    if (v < 3) {
        return 3;
    } else if (v >= 5) {
        return 1;
    } else {
        return 2;
    }
}

extern "C" static inline int fn_80169928(int j, int i, int ny, int nx) {
    if (i > nx - i) {
        i = nx - i;
    }
    if (j > ny - j) {
        j = ny - j;
    }
    if (i < j) {
        j = i;
    }
    if (j > 2) {
        return 3;
    } else if (j > 1) {
        return 2;
    } else {
        return 1;
    }
}


inline int CoinBoxPickupMaker::Make(ValueNode* n, PickupObjMan* m, const f32* params) {
    Bound* b;
    int np;
    f32 e[4];
    int a;
    int i0;
    int i2;
    f32 w;
    int nx;
    f32 rest;
    int (*pick)(int, int, int, int);
    BoundPt* p0;
    BoundPt* p1;
    BoundPt* p2;
    BoundPt* p3;
    int i;
    if (!Match(n->GetName())) {
        fn_802E30C8("CoinBoxPickupMaker: table name \"%s\" is not a match", n->GetName());
        return 0;
    }
    b = fn_801399FC(n);
    np = b->n;
    if (np < 4 || np > 4) {
        fn_802E30C8("CoinBoxPickupMaker: bound %s has %d points, should have 4", n->GetName(), np);
        fn_80128658(b);
        return 0;
    }
    e[0] = Dist(&b->pts[1], &b->pts[0]);
    e[1] = Dist(&b->pts[1], &b->pts[2]);
    e[2] = Dist(&b->pts[3], &b->pts[2]);
    e[3] = Dist(&b->pts[3], &b->pts[0]);
    a = !(e[0] + e[2] > e[1] + e[3]);
    i2 = (a + 2) % 4;
    i0 = a % 4;
    w = 0.5f * (e[i0] + e[i2]);
    nx = 1;
    {
        int s = (int)(w / params[0]);
        if (s > 1) {
            nx = s;
        }
    }
    rest = w - (f32)nx * params[0];
    pick = fn_80169928;
    if (strcasecmp(n->GetName(), "coin_box_2") == 0) {
        pick = fn_801698B8;
    }
    m->BeginGroup(b, -1);
    p1 = &b->pts[(a + 1) % 4];
    p0 = &b->pts[i0];
    p2 = &b->pts[i2];
    p3 = &b->pts[(a + 3) % 4];
    for (i = 0; i <= nx; i++) {
        f32 t = (0.5f * rest + (f32)i * params[0]) / w;
        f32 l;
        int ny;
        f32 off;
        int j;
        V2 pa = Lerp2(*p0, *p1, t);
        V2 pb = Lerp2(*p3, *p2, t);
        l = Dist(&pb, &pa);
        ny = 1;
        {
            int s = (int)(l / params[0]);
            if (s > 1) {
                ny = s;
            }
        }
        off = 0.5f * (l - (f32)ny * params[0]);
        for (j = 0; j <= ny; j++) {
            f32 s = (off + (f32)j * params[0]) / l;
            V2 pt = Lerp2(pa, pb, s);
            m->Add(pick(j, i, ny, nx), pt, 0.0f);
        }
    }
    m->grouping = 0;
    fn_80128658(b);
    return 1;
}

inline int SinglesPickupMaker::Match(const char* name) {
    int r = 0;
    if (strncasecmp(name, "star", 4) == 0 || strncasecmp(name, "coin", 4) == 0) {
        r = 1;
    }
    return r;
}

inline int SinglesPickupMaker::Make(ValueNode* n, PickupObjMan* m, const f32* params) {
    int type;
    int id;
    Bound* b;
    int i;
    if (!Match(n->GetName())) {
        fn_802E30C8("SinglesPickupMaker: table name \"%s\" is not a match!", n->GetName());
        return 0;
    }
    if (strncasecmp("star", n->GetName(), 4) == 0) {
        type = 4;
    } else if (strncasecmp("coin", n->GetName(), 4) == 0) {
        type = 1;
    } else {
        fn_802E30C8("SinglesPickupMaker: can't find CoinAreaPickupType for table %s", n->GetName());
        return 0;
    }
    id = -1;
    if (type == 1) {
        sscanf(n->GetName(), "coin%d", &id);
    } else if (type == 4) {
        sscanf(n->GetName(), "star%d", &id);
    }
    b = fn_801399FC(n);
    m->BeginGroup(0, id);
    for (i = 0; i < b->n; i++) {
        m->Add(type, b->pts[i], 0.0f);
    }
    m->grouping = 0;
    fn_80128658(b);
    return 1;
}

inline int PoopPickupMaker::Match(const char* name) {
    const char* p = "poop";
    return strncasecmp(name, p, strlen(p)) == 0;
}

inline int PoopPickupMaker::Make(ValueNode* n, PickupObjMan* m, const f32* params) {
    int id = -1;
    int i;
    ValueNode* c;
    sscanf(n->GetName(), "poop_%d", &id);
    m->BeginGroup(0, id);
    for (i = 0; (c = n->GetChildAt(i)) != 0; i++) {
        f32 hit = c->GetFloat("hit");
        ValPoint loc = c->GetPoint("loc");
        m->Add(1, *(V2*)&loc, hit);
    }
    m->grouping = 0;
    return 1;
}
