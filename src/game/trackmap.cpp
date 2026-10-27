// Track map (0x80019BBC-0x8001BA68): renders the track lines into a render texture once per player
// (trackmap.val settings), then draws it as a speed-zoomed, heading-up disc with the other trucks,
// placed markers, the player's truck and the start/finish marker on top. Names are guesses.
#include <math.h>
#include "game/common.h"

void* operator new(size_t size, const char* tag);

struct Vec2 {
    f32 x;
    f32 y;
};

struct Vec3 {
    f32 x;
    f32 y;
    f32 z;
};

struct CirclePoint : public Vec3 {
    CirclePoint(void) {}
};

// Rows: right, forward, up, position.
struct Basis {
    Vec3 right;  // offset 0x0
    Vec3 fwd;    // offset 0xC
    Vec3 up;     // offset 0x18
    Vec3 pos;    // offset 0x24
};

struct Mtx34 {
    f32 m[3][4];
};

struct Vertex {
    f32 data[6];
};

struct Viewport {
    u8 data[0x10];
};

struct Projection {
    u8 data[0x10];
};

struct CameraPose {
    f32 m[3][3];  // offset 0x0
    Vec3 pos;     // offset 0x24
};

struct TruckState {
    u8 pad0[0x18];
    f32 dirX;  // offset 0x18
    u8 pad1C[0x20 - 0x1C];
    f32 dirZ;  // offset 0x20
    u8 pad24[0x4C - 0x24];
    f32 speed;  // offset 0x4C
    u8 pad50[0x2C8 - 0x50];
    f32 lapDist;  // offset 0x2C8
};

struct Racer {
    u8 pad0[0x8];
    TruckState* truck;  // offset 0x8
    u8 padC[0x1C - 0xC];
    s32 lap;  // offset 0x1C
    u8 pad20[0xAC - 0x20];
    s32 driver;  // offset 0xAC
};

struct Truck {
    u8 pad0[0x60];
    Vec3 pos;  // offset 0x60
};

struct DriverInfo {
    u8 pad0[0x50];
    s32 mapIcon;  // offset 0x50
};

struct PlayerCtrl {
    u8 pad0[0x260C];
    u8 reversed;  // offset 0x260C
};

struct PlayerObj {
    u8 pad0[0x70];
    f32 dirX;  // offset 0x70
    u8 pad74[0x4];
    f32 dirY;  // offset 0x78
    u8 pad7C[0x140 - 0x7C];
    PlayerCtrl* ctrl;  // offset 0x140
};

struct EventInfo {
    u8 pad0[0x10];
    u32 flags;  // offset 0x10
};

struct PathNode {
    Vec2 pos;   // offset 0x0
    Vec2 dir;   // offset 0x8
    u8 pad10[0x4];
    PathNode* next;  // offset 0x14
    u8 pad18[0x4];
    f32 dist;  // offset 0x1C
};

struct Path {
    u8 pad0[0x8];
    f32 length;  // offset 0x8
    u8 padC[0x1C - 0xC];
    PathNode* first;  // offset 0x1C
};

struct PathIter {
    u8 pad0[0x2C];
    Path* path;  // offset 0x2C
    u8 pad30[0x38 - 0x30];
};

class PathSet {
public:
    virtual void vf08(void);
    virtual int Has(int index);  // 0xC
};

extern "C" {
double atan2(double y, double x);
double tan(double x);
double sqrt(double x);
void* memset(void* p, int c, size_t n);
int fn_8023512C(const char* name);
void fn_80235380(int art);
f32 fn_80235EF0(int art, int part);
void fn_802354A0(int art, int part, u32 color, f32 x, f32 y, f32 z, f32 w, f32 h);
void fn_80235AFC(int art, int part, u32 color, f32 x, f32 y, f32 z, f32 w, f32 h, f32 angle);
void fn_802A1120(Vec2* pos, Vec2* dir);
int fn_8012C33C(const char* name);
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* table);
int fn_802CB128(int w, int h, int a, int b);
void fn_802CB228(int rt);
void fn_802CB274(int rt);
s32 fn_802CAC28(const char* name, int flags);
void fn_802CAC54(s32 tex);
void fn_802C8530(void);
void fn_802C855C(void);
int fn_801E866C(int player);
Mtx34* fn_801E8718(int player);
Racer* fn_801EA9A0(int index);
int fn_801EA994(void);
Truck* fn_8017960C(int index);
PlayerObj* fn_802678E4(int player);
EventInfo* fn_801E87A8(void);
int fn_80178C9C(void);
void fn_80136468(Vec3* out, int path, f32 dist);
void PSVECNormalize(const Vec3* src, Vec3* dst);
void PSVECCrossProduct(const Vec3* a, const Vec3* b, Vec3* out);
f32 PSVECDotProduct(const Vec3* a, const Vec3* b);
u32 fn_80230B9C(int unk);
void fn_80231380(Vertex* v, u32 color, f32 x, f32 y, f32 z, f32 u, f32 w);
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
void fn_802C8DB8(Vertex* v, int nv, const u16* idx, int ni);
void fn_801352B8(PathIter* it);
int fn_80135D60(PathIter* it, int index);
void fn_8013530C(PathIter* it, int unk);
void fn_802C8DBC(int n);
void fn_802C8E00(u32 color, int flag, f32 x, f32 y, f32 z, f32 u, f32 v);
void fn_802C8E48(void);
PathSet* fn_80156A40(void);
void fn_802D3050(int pass);
void fn_802C9294(int rt, int unk);
void fn_802C9694(Projection* p, f32 nearZ, f32 farZ, f32 fov, f32 tanHalf);
void fn_802C96DC(Projection* p, int unk);
void fn_802C86F0(Viewport* vp, int x, int y, int w, int h);
void fn_802C8750(Viewport* vp, int unk);
void fn_802C88CC(u32 color);
int fn_802C2C00(int mode);
void fn_802C8638(CameraPose* cam);
}

DriverInfo* FindDriver(long id);

// Start/finish marker shared by all maps.
class SFMarker {
public:
    SFMarker(void) {
        Vec2 dir;
        f32 t;
        art = fn_8023512C("mapsf.art");
        fn_802A1120(&pos, &dir);
        t = dir.x;
        dir.x = dir.y;
        dir.y = -t;
        lap = 0;
        unk10 = 0.0f;
        angle = atan2(dir.y, dir.x);
    }
    ~SFMarker(void) { fn_80235380(art); }

    int art;     // offset 0x0
    s32 lap;     // offset 0x4
    Vec2 pos;    // offset 0x8
    f32 unk10;   // offset 0x10
    f32 angle;   // offset 0x14
};

// Truck icons of one map.
class TruckDraw {
public:
    TruckDraw(void) {
        art = fn_8023512C("mapgems.art");
        memset(icons, 0, sizeof(icons));
        count = 0;
        self = -1;
    }
    ~TruckDraw(void) { fn_80235380(art); }

    int art;  // offset 0x0
    struct {
        s32 part;
        f32 angle;
    } icons[8];  // offset 0x4
    s32 count;   // offset 0x44
    s32 self;    // offset 0x48
};

class MapMarker {
public:
    MapMarker(void) { Reset(); }
    ~MapMarker(void) {}

    void Reset(void) {
        active = 0;
        art = 0;
        part = -1;
    }
    int Set(const Vec2* p, int a, int pt) {
        if (!active) {
            art = a;
            part = pt;
            pos.x = p->x;
            pos.y = p->y;
            active = 1;
            return 1;
        }
        return 0;
    }

    int art;     // offset 0x0
    int part;    // offset 0x4
    Vec2 pos;    // offset 0x8
    u8 active;   // offset 0x10
};

class TrackMap {
public:
    TrackMap(void) {
        tex = -2;
        target = -2;
        line2Tex = -2;
        lineTex = -2;
        backTex = -2;
        frontTex = -2;
    }

    s32 tex;              // offset 0x0
    s32 target;           // offset 0x4
    s32 line2Tex;         // offset 0x8
    s32 lineTex;          // offset 0xC
    TruckDraw* trucks;    // offset 0x10
    s32 backTex;          // offset 0x14
    s32 frontTex;         // offset 0x18
    f32 texSize;          // offset 0x1C
    Vec2 origin;          // offset 0x20
    f32 width;            // offset 0x28
    f32 minSpeed;         // offset 0x2C
    f32 maxSpeed;         // offset 0x30
    f32 minWidth;         // offset 0x34
    f32 maxWidth;         // offset 0x38
    f32 lineWidth;        // offset 0x3C
    f32 iconScale;        // offset 0x40
    s32 player;           // offset 0x44
};

ModuleTag lbl_8055D5A8;
UnkStatic7F lbl_8055D5B0;
CirclePoint lbl_803582C0[21];
MapMarker lbl_803583C8[16];
TrackMap lbl_80358508[4];
SFMarker* lbl_8055D5B4;
s32 lbl_8055D5B8;

extern "C" {

void fn_8001B7BC(TrackMap* map);

void fn_80019BBC(void) {
    f32 a = 0.0f;
    int i;
    f32 z = a;
    f32 step = 0.31415927f;
    for (i = 0; i < 20; i++) {
        lbl_803582C0[i].x = cos(a);
        lbl_803582C0[i].y = sin(a);
        a += step;
        lbl_803582C0[i].z = z;
    }
    lbl_803582C0[20].x = 0.0f;
    lbl_803582C0[20].y = 0.0f;
    lbl_803582C0[20].z = 0.0f;
}

// Stripped from ET; only its constants remain.
void GetMapDefaults(f32* half, f32* scale) {
    *half = 0.5f;
    *scale = 1.0f;
}

TrackMap* fn_80019C68(int player) {
    const char* name;
    ValueTable* table;
    TrackMap* map;
    ValueNode* node;

    fn_80019BBC();
    if (lbl_8055D5B8 == 0) {
        lbl_8055D5B4 = new ("sfmarker") SFMarker;
        lbl_8055D5B8++;
    } else {
        lbl_8055D5B8++;
    }
    name = "trackmap.val";
    if (fn_8012C33C(name)) {
        map = &lbl_80358508[player];
        map->tex = -2;
        map->target = -2;
        map->line2Tex = -2;
        map->lineTex = -2;
        map->backTex = -2;
        map->frontTex = -2;
        table = fn_80131808(name);
        node = table->FindChild("TrackMap");
        if (node) {
            ValPoint origin;
            map->width = node->GetFloat("width");
            map->texSize = node->GetFloat("tex_size");
            origin = node->GetPoint("origin");
            map->origin.x = origin.x;
            map->origin.x = origin.x - map->texSize / 2;
            map->origin.y = origin.y - map->texSize / 2;
            map->minSpeed = node->GetFloat("min_speed", 70.0f);
            map->maxSpeed = node->GetFloat("max_speed", 120.0f);
            map->minWidth = node->GetFloat("min_wid", map->width / 4.5f);
            map->maxWidth = node->GetFloat("max_wid", map->width / 3.5f);
            map->lineWidth = node->GetFloat("line_wid", 50.0f);
            map->iconScale = node->GetFloat("icon_scale", 1.0f);
        }
        fn_80131818(table);
        map->target = fn_802CB128(map->texSize, map->texSize, 0, 0x21);
        map->lineTex = fn_802CAC28("mapline.tex", 0);
        map->line2Tex = fn_802CAC28("mapline2.tex", 0);
        fn_802C8530();
        fn_8001B7BC(map);
        fn_802C855C();
        map->trucks = new ("truckdraw") TruckDraw;
        map->backTex = fn_802CAC28("mapback.tex", 0);
        map->frontTex = fn_802CAC28("mapfront.tex", 0);
        map->player = player;
        return map;
    }
    return 0;
}

void fn_8001A04C(TrackMap* map) {
    int i;
    if (--lbl_8055D5B8 == 0) {
        delete lbl_8055D5B4;
        lbl_8055D5B4 = 0;
    }
    for (i = 0; i < sizeof(lbl_803583C8) / sizeof(lbl_803583C8[0]); i++) {
        lbl_803583C8[i].Reset();
    }
    if (map) {
        if (map->frontTex != -2) {
            fn_802CAC54(map->frontTex);
            map->frontTex = -2;
        }
        if (map->backTex != -2) {
            fn_802CAC54(map->backTex);
            map->backTex = -2;
        }
        if (map->tex != -2) {
            fn_802CAC54(map->tex);
            map->tex = -2;
        }
        if (map->line2Tex != -2) {
            fn_802CAC54(map->line2Tex);
            map->line2Tex = -2;
        }
        if (map->lineTex != -2) {
            fn_802CAC54(map->lineTex);
            map->lineTex = -2;
        }
        if (map->target != -2) {
            fn_802CB228(map->target);
            map->target = -2;
        }
        if (map->trucks) {
            delete map->trucks;
            map->trucks = 0;
        }
    }
}

void fn_8001A1F0(void) {}

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void MulVec(Vec3* out, const Vec3* in, const Basis* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m->right.x, in->y, m->fwd.x, in->z, m->up.x);
        f32 y = Dot3(in->x, m->right.y, in->y, m->fwd.y, in->z, m->up.y);
        f32 z = Dot3(in->x, m->right.z, in->y, m->fwd.z, in->z, m->up.z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m->right.x, in->y, m->fwd.x, in->z, m->up.x);
        out->y = Dot3(in->x, m->right.y, in->y, m->fwd.y, in->z, m->up.y);
        out->z = Dot3(in->x, m->right.z, in->y, m->fwd.z, in->z, m->up.z);
    }
}

static inline void MulVecT(Vec3* out, const Vec3* in, const Basis* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m->right.x, in->y, m->right.y, in->z, m->right.z);
        f32 y = Dot3(in->x, m->fwd.x, in->y, m->fwd.y, in->z, m->fwd.z);
        f32 z = Dot3(in->x, m->up.x, in->y, m->up.y, in->z, m->up.z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m->right.x, in->y, m->right.y, in->z, m->right.z);
        out->y = Dot3(in->x, m->fwd.x, in->y, m->fwd.y, in->z, m->fwd.z);
        out->z = Dot3(in->x, m->up.x, in->y, m->up.y, in->z, m->up.z);
    }
}

static inline f32 Min(f32 a, f32 b) { return a < b ? a : b; }
static inline f32 Max(f32 a, f32 b) { return a > b ? a : b; }

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

void fn_8001A1F4(TrackMap* map, f32 x, f32 y, f32 w, f32 h) {
    Vertex verts[41] __attribute__((aligned(16)));
    u16 idx[180];
    Basis basis;
    Vec3 path;
    Vec2 dir;
    Vec2 tpos;
    int view;
    SFMarker* sf;
    TruckDraw* draw;
    int me;
    int i;
    f32 scale;
    f32 radius;
    u32 color;

    if (!map) {
        return;
    }
    view = fn_801E866C(map->player);
    sf = lbl_8055D5B4;
    {
        Mtx34 m = *fn_801E8718(map->player);
        sf->unk10 = atan2(m.m[1][2], m.m[2][0]);
    }
    sf->lap = fn_801EA9A0(view)->lap;
    draw = map->trucks;
    me = map->player;
    memset(draw->icons, 0, sizeof(draw->icons));
    draw->count = fn_801EA994();
    for (i = 0; i < draw->count; i++) {
        Racer* r = fn_801EA9A0(i);
        draw->icons[i].part = FindDriver(r->driver)->mapIcon;
        if (r->truck) {
            draw->icons[i].angle = -(f32)atan2(r->truck->dirX, r->truck->dirZ);
        }
    }
    {
        Mtx34 m = *fn_801E8718(me);
        f32 rot = -(f32)atan2(m.m[1][2], m.m[2][0]);
        for (i = 0; i < draw->count; i++) {
            draw->icons[i].angle -= rot;
        }
    }
    draw->self = view;
    {
        f32 dist = fn_801EA9A0(view)->truck->lapDist;
        f32 t;
        scale = 1.0f / (map->width / 2.0f);
        t = (fn_801EA9A0(view)->truck->speed / 0.44444445f - map->minSpeed) /
            (map->maxSpeed - map->minSpeed);
        t = Max(Min(1.0f, t), 0.0f);
        radius = map->minWidth + t * (map->maxWidth - map->minWidth);
        fn_80136468(&path, fn_80178C9C(), dist);
    }
    {
        Truck* tr = fn_8017960C(view);
        PlayerObj* obj = fn_802678E4(map->player);
        dir.x = obj->dirX;
        dir.y = obj->dirY;
        if (obj->ctrl->reversed) {
            dir.x *= -1.0f;
            dir.y *= -1.0f;
        }
        tpos.x = tr->pos.x;
        tpos.y = tr->pos.z;
        basis.right.x = 1.0f;
        basis.right.y = 0.0f;
        basis.right.z = 0.0f;
        basis.up.x = 0.0f;
        basis.up.y = 0.0f;
        basis.up.z = 1.0f;
        basis.pos.x = tpos.x;
        basis.pos.y = tpos.y;
        basis.pos.z = 0.0f;
        basis.fwd.x = dir.x;
        basis.fwd.y = dir.y;
        basis.fwd.z = 0.0f;
    }
    PSVECNormalize(&basis.fwd, &basis.fwd);
    basis.up.x = 0.0f;
    basis.up.y = 0.0f;
    basis.up.z = 1.0f;
    PSVECCrossProduct(&basis.fwd, &basis.up, &basis.right);
    if (fn_801E87A8()->flags & 2) {
        basis.right.x *= -1.0f;
        basis.right.y *= -1.0f;
        basis.right.z *= -1.0f;
    }
    color = fn_80230B9C(-1);
    for (i = 0; i < 21; i++) {
        Vec3 c = lbl_803582C0[i];
        Vec3 v;
        v.x = c.x * (0.5f * radius);
        v.y = c.y * (0.5f * radius);
        v.z = c.z * (0.5f * radius);
        MulVec(&v, &v, &basis);
        v.x += basis.pos.x;
        v.y += basis.pos.y;
        v.z += basis.pos.z;
        fn_80231380(&verts[i], color, x + w * (1.0f + 0.5f * c.x) * 0.5f, y + h * (0.5f * c.y - 1.0f) * 0.5f,
                    0.0f, 0.5f + 0.5f * scale * v.x + map->origin.x / map->texSize,
                    1.0f - (0.5f + 0.5f * scale * v.y) + map->origin.y / map->texSize);
        if (i < 20) {
            v.x = c.x * radius;
            v.y = c.y * radius;
            v.z = c.z * radius;
            MulVec(&v, &v, &basis);
            v.x += basis.pos.x;
            v.y += basis.pos.y;
            v.z += basis.pos.z;
            fn_80231380(&verts[i + 21], color & 0xFFFFFF, x + w * (1.0f + c.x) * 0.5f,
                        y + h * (c.y - 1.0f) * 0.5f, 0.0f,
                        0.5f + 0.5f * scale * v.x + map->origin.x / map->texSize,
                        1.0f - (0.5f + 0.5f * scale * v.y) + map->origin.y / map->texSize);
        }
    }
    {
        int k = 0;
        for (i = 0; i < 20; i++) {
            idx[k++] = 20;
            idx[k++] = i;
            idx[k++] = (i + 1) % 20;
            idx[k++] = i;
            idx[k++] = (i + 1) % 20;
            idx[k++] = i + 21;
            idx[k++] = (i + 1) % 20;
            idx[k++] = i + 21;
            idx[k++] = (i + 1) % 20 + 21;
        }
    }
    if (map->target != -2) {
        fn_802C8BD0(map->target);
    } else {
        fn_802C8BD0(map->tex);
    }
    fn_802C8DB8(verts, 41, idx, 180);
    fn_802C8D74();
    fn_801EA9A0(view);
    for (i = 0; i < fn_801EA994(); i++) {
        Vec3 p;
        Vec3 rel;
        f32 fade;
        int part;
        TruckDraw* d;
        if (i == fn_801E866C(map->player)) {
            continue;
        }
        fn_801EA9A0(i);
        p = fn_8017960C(i)->pos;
        rel.x = p.x - basis.pos.x;
        rel.y = p.z - basis.pos.y;
        rel.z = 0.0f - basis.pos.z;
        MulVecT(&rel, &rel, &basis);
        {
            f32 s = w * 0.5f / radius;
            rel.x *= s;
            rel.y *= s;
            rel.z *= s;
        }
        if (PSVECDotProduct(&rel, &rel) >= (w * 0.5f) * (w * 0.5f)) {
            continue;
        }
        fade = Ease(1.0f - PSVECDotProduct(&rel, &rel) / ((w * 0.5f) * (w * 0.5f)));
        d = map->trucks;
        part = i == d->self ? 1 : 3;
        {
            f32 px = rel.x + (x + w * 0.5f);
            f32 py = rel.y + (y - h * 0.5f);
            f32 size = fade * fn_80235EF0(d->art, part);
            fn_80235AFC(d->art, part, -1, px, py, 0.0f, size, size, d->icons[i].angle);
        }
    }
    {
        f32 cx = x + w * 0.5f;
        f32 cy = y - h * 0.5f;
        unsigned int j;
        for (j = 0; j < sizeof(lbl_803583C8) / sizeof(lbl_803583C8[0]); j++) {
            MapMarker* m = &lbl_803583C8[j];
            Vec2 c;
            c.x = cx;
            c.y = cy;
            if (m->active) {
                Vec3 rel;
                f32 size;
                rel.x = m->pos.x - basis.pos.x;
                rel.y = m->pos.y - basis.pos.y;
                rel.z = 0.0f - basis.pos.z;
                MulVecT(&rel, &rel, &basis);
                {
                    f32 s = w * 0.5f / radius;
                    rel.x *= s;
                    rel.y *= s;
                    rel.z *= s;
                }
                if (PSVECDotProduct(&rel, &rel) > (w * 0.5f) * (w * 0.5f)) {
                    PSVECNormalize(&rel, &rel);
                    rel.x *= w * 0.5f;
                    rel.y *= w * 0.5f;
                    rel.z *= w * 0.5f;
                }
                size = fn_80235EF0(m->art, m->part);
                fn_802354A0(m->art, m->part, -1, c.x + rel.x - size * 0.5f, size * 0.5f + (c.y + rel.y), 0.0f,
                            size, size);
            }
        }
    }
    {
        TruckDraw* d = map->trucks;
        int part = view == d->self ? 1 : 3;
        f32 size = 1.0f * fn_80235EF0(d->art, part);
        fn_80235AFC(d->art, part, -1, x + w * 0.5f, y - h * 0.5f, 0.0f, size, size, d->icons[view].angle);
    }
    {
        Truck* tr = fn_8017960C(fn_801E866C(map->player));
        SFMarker* m = lbl_8055D5B4;
        Vec2 c;
        Vec2 tp;
        Vec3 p;
        Vec3 rel;
        c.x = x + w * 0.5f;
        c.y = y - h * 0.5f;
        p = tr->pos;
        tp.x = p.x;
        tp.y = p.z;
        rel.x = m->pos.x - basis.pos.x;
        rel.y = m->pos.y - basis.pos.y;
        rel.z = 0.0f - basis.pos.z;
        MulVecT(&rel, &rel, &basis);
        {
            f32 s = w * 0.5f / radius;
            rel.x *= s;
            rel.y *= s;
            rel.z *= s;
        }
        if (PSVECDotProduct(&rel, &rel) < (w * 0.5f) * (w * 0.5f)) {
            Vec2 delta;
            f32 size;
            f32 fade;
            f32 a;
            delta.x = tp.x - m->pos.x;
            delta.y = tp.y - m->pos.y;
            size = fn_80235EF0(m->art, 0);
            fade = Ease(1.0f - PSVECDotProduct(&rel, &rel) / ((w * 0.5f) * (w * 0.5f)));
            a = 255.0f * fade;
            fn_80235AFC(m->art, 0, ((int)(a < 0.0f ? a - 0.5f : 0.5f + a) << 24) | 0xFF | 0xFFFF00,
                        c.x + rel.x, c.y + rel.y, 0.0f, size, size, m->angle + m->unk10);
        }
    }
}

int fn_8001B308(const Vec2* pos, int art, int part) {
    int i;
    for (i = 0; i < 16; i++) {
        if (lbl_803583C8[i].Set(pos, art, part)) {
            return i;
        }
    }
    return -1;
}

void fn_8001B450(int index) { lbl_803583C8[index].Reset(); }

static inline void LerpVec2(Vec2* out, const Vec2* a, const Vec2* b, f32 t) {
    out->x = a->x + t * (b->x - a->x);
    out->y = a->y + t * (b->y - a->y);
}

void fn_8001B478(int index, f32 width) {
    PathIter it;
    Vec2 p;
    Vec2 d;
    Vec2 v;
    Path* path;
    f32 len;
    f32 n;
    int segs;
    f32 vscale;
    PathNode* node;
    int count;
    PathNode* q;
    int flag;
    f32 prev;
    int i;

    fn_801352B8(&it);
    if (!fn_80135D60(&it, index)) {
        fn_8013530C(&it, -1);
        return;
    }
    path = it.path;
    len = path->length;
    n = len / width;
    segs = n;
    if (n - segs > 0.5f) {
        segs++;
    }
    node = path->first;
    count = 1;
    vscale = segs / len;
    for (q = node->next; q != node; q = q->next) {
        count++;
    }
    if (count % 4 > 0) {
        count += 4 - count % 4;
    }
    fn_802C8DBC(count / 4 * 2 + 2);
    count++;
    prev = -1.0f;
    flag = 0;
    for (i = 0; i < count; i++, node = node->next) {
        if (i % 4 == 0) {
            f32 t;
            LerpVec2(&d, &node->dir, &node->next->dir, 0.0f);
            LerpVec2(&p, &node->pos, &node->next->pos, 0.0f);
            {
                f32 inv = 1.0f / (f32)sqrt(d.x * d.x + d.y * d.y);
                f32 dx;
                d.x *= inv;
                d.y *= inv;
                dx = d.x;
                d.x = d.y;
                d.y = -dx;
            }
            if (node->dist < prev) {
                t = vscale * (node->dist + len);
            } else {
                t = node->dist * vscale;
            }
            prev = node->dist;
            v.x = p.x + d.x * (-width / 2);
            v.y = p.y + d.y * (-width / 2);
            fn_802C8E00(-1, flag, v.x, 0.0f, v.y, 0.0f, t);
            v.x = p.x + d.x * (width / 2);
            v.y = p.y + d.y * (width / 2);
            fn_802C8E00(-1, flag, v.x, 0.0f, v.y, 1.0f, t);
            flag = 1;
        }
    }
    fn_802C8E48();
    fn_8013530C(&it, -1);
}

void fn_8001B754(f32 width) {
    int i;
    int id;
    for (i = 0; (id = fn_80156A40()->Has(i)) != 0; i++) {
        fn_8001B478(id, width);
    }
}

void fn_8001B7B8(int pass) { fn_802D3050(pass); }

void fn_8001B7BC(TrackMap* map) {
    Projection proj;
    Viewport vp;
    CameraPose cam;
    f32 fov;
    f32 dist;
    int old;

    fn_802CB274(map->target);
    fn_802C9294(map->target, 0);
    fov = 0.34906584f;
    {
        f32 t = tan(fov / 2);
        dist = 0.5f * map->width / t;
        fn_802C9694(&proj, dist - 1.0f, 1.0f + dist, fov, t);
    }
    fn_802C86F0(&vp, 0, 0, map->texSize, map->texSize);
    fn_802C88CC(0xFFFFFF);
    old = fn_802C2C00(0);
    cam.pos.x = 0.0f;
    cam.pos.y = dist;
    cam.pos.z = 0.0f;
    cam.m[0][0] = 1.0f;
    cam.m[0][1] = 0.0f;
    cam.m[0][2] = 0.0f;
    cam.m[1][0] = 0.0f;
    cam.m[1][1] = 0.0f;
    cam.m[1][2] = 1.0f;
    cam.m[2][0] = 0.0f;
    cam.m[2][1] = -1.0f;
    cam.m[2][2] = 0.0f;
    fn_802C8638(&cam);
    fn_802C8BD0(map->lineTex);
    fn_8001B754(map->lineWidth);
    fn_802C8D74();
    fn_8001B7B8(0);
    fn_802C8BD0(map->line2Tex);
    fn_8001B754(map->lineWidth);
    fn_802C8D74();
    fn_8001B7B8(1);
    fn_802C2C00(old);
    fn_802C9294(-2, 0);
    fn_802C8750(&vp, -1);
    fn_802C96DC(&proj, -1);
}

}  // extern "C"
