// Track map (0x80019BBC-0x8001BA68): renders the track lines into a render texture once per player
// (trackmap.val settings), then draws it as a speed-zoomed, heading-up disc with the other trucks,
// placed markers, the player's truck and the start/finish marker on top. Names are guesses.
#include <math.h>
#include "game/common.h"
#include "game/pathset.h"
#include "game/gart.h"
#include "game/idealline.h"
#include "game/centerline.h"
#include "game/carbody.h"
#include "game/carinfo.h"
#include "game/raceopts.h"
#include "game/gamedb.h"
#include <revolution/MTX/vec.h>
#include "game/math_types.h"

template <class T>
static inline void SetXYZ(T* v, f32 x, f32 y, f32 z) { /* Helper name unproven */
    v->x = x;
    v->y = y;
    v->z = z;
}

void* operator new(size_t size, const char* tag);

// Vec2: the SDK's (revolution/MTX/mtxtypes.h, same x/y layout).

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

#include "game/mtx34.h"

#include "game/vertex.h"

// Saves the viewport and sets a new one; the destructor restores it (mrender.cpp).
#include "game/viewport.h"

// Saves the projection and sets a new one; the destructor restores it (mrender.cpp).

struct PlayerCtrl {
    u8 pad0[0x260C];
    u8 reversed;  // offset 0x260C
};

// Unit-local (port ODR: other units define a different PlayerObj).
namespace {
struct PlayerObj {
    u8 pad0[0x70];
    f32 dirX;  // offset 0x70
    u8 pad74[0x4];
    f32 dirY;  // offset 0x78
    u8 pad7C[0x140 - 0x7C];
    PlayerCtrl* ctrl;  // offset 0x140
};
}  // namespace

// Ideal-line walker (idealline.cpp).
struct CenterLine;
extern "C" {
double atan2(double y, double x);
double tan(double x);
double sqrt(double x);
void* memset(void* p, int c, size_t n);
void fn_802A1120(Vec2* pos, Vec2* dir);
int fn_8012C33C(const char* name);
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* table);
int fn_802CB128(int w, int h, int a, int b);
void fn_802CB228(int rt);
void fn_802CB274(int rt);
s32 fn_802CAC28(const char* name, int flags);
void gxForgetTexture(s32 tex);
void fn_802C8530(void);
void fn_802C855C(void);
int fn_801E866C(int player);
Mtx34* fn_801E8718(int player);
CarInfo* GetCarInfo(int index);
int GetHumanPlayerCount(void);
RaceCarRec* fn_8017960C(int index);
PlayerObj* fn_802678E4(int player);
int fn_80178C9C(void);
Vec3 fn_80136468(int path, f32 dist);  // idealline: returns the ILPos by value
u32 fn_80230B9C(int unk);
void fn_80231380(Vertex* v, u32 color, f32 x, f32 y, f32 z, f32 u, f32 w);
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
void fn_802C8DB8(Vertex* v, int nv, const u16* idx, int ni);
void fn_802C8DBC(int n);
void fn_802C8E00(f32 x, f32 y, f32 z, f32 u, f32 v, u32 color, int flag);
void fn_802C8E48(void);
PathSet* fn_80156A40(void);
void fn_802D3050(int pass);
void fn_802C9294(int rt, int unk);
void fn_802C88CC(u32 color);
int fn_802C2C00(int mode);
void fn_802C8638(PlainValMatrix* cam);
}


// Start/finish marker shared by all maps.
static inline f32 Atan2F(f32 y, f32 x) { return (f32)atan2(y, x); }

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
        angle = Atan2F(dir.y, dir.x);
    }
    ~SFMarker(void) { fn_80235380(art); }

    gArtSet* art;     // offset 0x0
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

    gArtSet* art;  // offset 0x0
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
    int Set(const Vec2* p, gArtSet* a, int pt) {
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

    gArtSet* art;     // offset 0x0
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
    f32 step;
    f32 z;
    f32 a;
    a = 0.0f;
    int i;
    z = a;
    step = 0.31415927f;
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
            map->width = node->GetFloat("width");
            map->texSize = node->GetFloat("tex_size");
            ValPoint origin = node->GetPoint("origin");
            map->origin.x = origin.x;
            map->origin.y = origin.y;
            map->origin.x = map->origin.x - map->texSize / 2;
            map->origin.y = map->origin.y - map->texSize / 2;
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
            gxForgetTexture(map->frontTex);
            map->frontTex = -2;
        }
        if (map->backTex != -2) {
            gxForgetTexture(map->backTex);
            map->backTex = -2;
        }
        if (map->tex != -2) {
            gxForgetTexture(map->tex);
            map->tex = -2;
        }
        if (map->line2Tex != -2) {
            gxForgetTexture(map->line2Tex);
            map->line2Tex = -2;
        }
        if (map->lineTex != -2) {
            gxForgetTexture(map->lineTex);
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



static inline u32 ARGB(u8 a, u8 r, u8 g, u8 b) { return a << 24 | r << 16 | g << 8 | b; }
// Name unproven: inverse lerp; parameter order unproven (lo, hi, v evaluated right to left gives retail load order).
static inline f32 Unlerp(f32 lo, f32 hi, f32 v) { return (v - lo) / (hi - lo); }
// Name unproven: map texture U projection.
static inline f32 MapTextureU(f32 half, f32 scale, f32 value, f32 origin, f32 texSize) {
    return half + half * scale * value + origin / texSize;
}
// Name unproven: consumed map texture projection.
static inline f32 MapTextureV(f32 one, f32 half, f32 scale, f32 value, f32 origin, f32 texSize) {
    return one - (half + half * scale * value) + origin / texSize;
}

// Name unproven: initialize the real right row via its first float member.
static inline void SetMapRightRow(f32* first, f32 x, f32 y, f32 z) {
    Vec3* row = (Vec3*)first;
    row->x = x;
    row->y = y;
    row->z = z;
}

// Name unproven: prepare the actual truck-icon output, including relative heading.
static inline void PrepareTruckIcons(TruckDraw* draw, int me, int view) {
    int i;
    memset(draw->icons, 0, sizeof(draw->icons));
    draw->count = GetHumanPlayerCount();
    for (i = 0; i < draw->count; i++) {
        CarInfo* r = GetCarInfo(i);
        draw->icons[i].part = FindDriver(r->setup.driver)->color;  // the driver's colour picks its map icon
        if (r->draw) {
            draw->icons[i].angle = -Atan2F(r->draw->pose.rot.m[6], r->draw->pose.rot.m[8]);
        }
    }
    {
        Mtx34 m = *fn_801E8718(me);
        f32 rot = -Atan2F(m.m[1][2], m.m[2][0]);
        for (int heading = 0; heading < draw->count; heading++) {
            draw->icons[heading].angle -= rot;
        }
    }
    draw->self = view;
}

// Name unproven: render the actual placed-marker operation.
static inline void DrawPlacedMarkers(Basis* basis, f32 x, f32 y, f32 w, f32 h, f32 radius, s32& j) {
    for (j = 0; j < sizeof(lbl_803583C8) / sizeof(lbl_803583C8[0]); j++) {
        Vec2 c;
        c.x = x + w / 2.0f;
        c.y = y - h / 2.0f;
        if (lbl_803583C8[j].active) {
            Vec3 rel;
            f32 size;
            rel.y = lbl_803583C8[j].pos.y - basis->pos.y;
            rel.x = lbl_803583C8[j].pos.x - basis->pos.x;
            rel.z = 0.0f;
            rel.z -= basis->pos.z;
            MulVecT(&rel, &rel, basis);
            {
                f32 s = w / 2.0f / radius;
                rel.x *= s;
                rel.y *= s;
                rel.z *= s;
            }
            if (PSVECDotProduct((const Vec*)&rel, (const Vec*)&rel) > (w / 2.0f) * (w / 2.0f)) {
                PSVECNormalize((const Vec*)&rel, (Vec*)&rel);
                rel.x *= w / 2.0f;
                rel.y *= w / 2.0f;
                rel.z *= w / 2.0f;
            }
            size = fn_80235EF0(lbl_803583C8[j].art, lbl_803583C8[j].part);
            fn_802354A0(lbl_803583C8[j].art, lbl_803583C8[j].part, c.x + rel.x - size / 2.0f, size * 0.5f + (c.y + rel.y), 0.0f, size, size, -1);
        }
    }
}

// Name unproven: project and draw the actual final start/finish marker.
static inline void DrawFinalMarker(SFMarker* m, Basis* basis, const Vec2& tp, f32 centerX, f32 centerY, f32 w, f32 radius) {
    Vec2 delta;
    Vec2 c;
    Vec3 rel;
    c.x = centerX;
    c.y = centerY;
    SetXYZ(&rel, m->pos.x - basis->pos.x, m->pos.y - basis->pos.y, 0.0f);
    rel.z -= basis->pos.z;
    MulVecT(&rel, &rel, basis);
    {
        f32 s = w / 2.0f / radius;
        rel.x *= s;
        rel.y *= s;
        rel.z *= s;
    }
    if (PSVECDotProduct((const Vec*)&rel, (const Vec*)&rel) < (w / 2.0f) * (w / 2.0f)) {
        f32 size;
        f32 fade;
        f32 a;
        delta.x = tp.x - m->pos.x;
        delta.y = tp.y - m->pos.y;
        size = fn_80235EF0(m->art, 0);
        fade = Ease(1.0f - PSVECDotProduct((const Vec*)&rel, (const Vec*)&rel) / ((w / 2.0f) * (w / 2.0f)));
        a = 255.0f * fade;
        fn_80235AFC(m->art, 0, ARGB((int)(a < 0.0f ? a - 0.5f : 0.5f + a), 0xFF, 0xFF, 0xFF),
                    c.x + rel.x, c.y + rel.y, 0.0f, size, size, m->angle + m->unk10);
    }
}

void fn_8001A1F4(TrackMap* map, f32 x, f32 y, f32 w, f32 h) {
    SFMarker* m;
    Vertex verts[41] __attribute__((aligned(16)));
    u16 idx[180];
    Basis basis;
    Vec2 dir;
    Vec2 tpos;
    SFMarker* sf;
    TruckDraw* draw;
    int view;
    int i;
    f32 scale;
    u32 color;
    s32 j;  // placed-marker counter owned here, used by DrawPlacedMarkers by reference (ownership unproven)

    if (!map) {
        return;
    }
    view = fn_801E866C(map->player);
    int cameraPlayer = map->player;
    sf = lbl_8055D5B4;
    {
        Mtx34 m = *fn_801E8718(cameraPlayer);
        f32 y = m.m[1][2];
        f32 x = m.m[2][0];
        sf->unk10 = Atan2F(y, x);
    }
    sf->lap = GetCarInfo(view)->params.lap;
    draw = map->trucks;
    PrepareTruckIcons(draw, map->player, view);
    // radius is a named local declared here (after dist), not at the top and not an inline result:
    // retail colours it first (f31), ahead of x/y/w/h, which needs its number below dist's.
    f32 dist = GetCarInfo(view)->draw->lapDist;
    f32 t;
    scale = 1.0f / (map->width / 2.0f);
    t = Unlerp(map->minSpeed, map->maxSpeed, GetCarInfo(view)->draw->speedXZ / 0.44444445f);
    f32 lo;
    f32 hi;
    hi = map->maxWidth;
    lo = map->minWidth;
    f32 k = 1.0f;
    k = Min(k, t);
    k = Max(k, 0.0f);
    f32 radius = lo + k * (hi - lo);
    const Vec3& path = fn_80136468(fn_80178C9C(), dist);  // unused
    {
        RaceCarRec* tr = fn_8017960C(view);
        PlayerObj* obj = fn_802678E4(map->player);
        dir.x = obj->dirX;
        dir.y = obj->dirY;
        if (obj->ctrl->reversed) {
            dir.x *= -1.0f;
            dir.y *= -1.0f;
        }
        tpos.x = tr->mtx.pos.x;
        tpos.y = tr->mtx.pos.z;
        SetMapRightRow(&basis.right.x, 1.0f, 0.0f, 0.0f);
        basis.up.x = 0.0f;
        basis.up.y = 0.0f;
        basis.up.z = 1.0f;
        basis.pos.x = tpos.x;
        basis.pos.y = tpos.y;
        basis.pos.z = 0.0f;
        SetXYZ(&basis.fwd, dir.x, dir.y, 0.0f);
    }
    PSVECNormalize((const Vec*)&basis.fwd, (Vec*)&basis.fwd);
    basis.up.x = 0.0f;
    basis.up.y = 0.0f;
    basis.up.z = 1.0f;
    Vec3* right = &basis.right;
    PSVECCrossProduct((const Vec*)&basis.fwd, (const Vec*)&basis.up, (Vec*)right);
    if (fn_801E87A8()->flags & 2) {
        basis.right.x *= -1.0f;
        basis.right.y *= -1.0f;
        basis.right.z *= -1.0f;
    }
    color = fn_80230B9C(-1);
    f32 zero = 0.0f;
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
        f32 tu = MapTextureU(0.5f, scale, v.x, map->origin.x, map->texSize);
        fn_80231380(&verts[i], color, x + w * (1.0f + 0.5f * c.x) * 0.5f, y + h * (0.5f * c.y - 1.0f) * 0.5f, zero, tu, MapTextureV(1.0f, 0.5f, scale, v.y, map->origin.y, map->texSize));
        if (i < 20) {
            v.x = c.x * radius;
            v.y = c.y * radius;
            v.z = c.z * radius;
            MulVec(&v, &v, &basis);
            v.x += basis.pos.x;
            v.y += basis.pos.y;
            v.z += basis.pos.z;
            f32 tu = MapTextureU(0.5f, scale, v.x, map->origin.x, map->texSize);
            fn_80231380(&verts[i + 21], color & 0xFFFFFF, x + w * (1.0f + c.x) * 0.5f, y + h * (c.y - 1.0f) * 0.5f, zero, tu, MapTextureV(1.0f, 0.5f, scale, v.y, map->origin.y, map->texSize));
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
    GetCarInfo(view);
    for (int truck = 0; truck < GetHumanPlayerCount(); truck++) {
        f32 fade;
        int part;
        TruckDraw* d;
        bool self = truck == fn_801E866C(map->player);
        if (self) {
            continue;
        }
        GetCarInfo(truck);
        Vec3 rel = *(const Vec3*)&fn_8017960C(truck)->mtx.pos;
        rel.x = rel.x - basis.pos.x;
        f32 yv = basis.pos.y;
        rel.y = rel.z - yv;
        rel.z = 0.0f;
        rel.z -= basis.pos.z;
        MulVecT(&rel, &rel, &basis);
        {
            f32 s = w / 2.0f / radius;
            rel.x *= s;
            rel.y *= s;
            rel.z *= s;
        }
        if (!(PSVECDotProduct((const Vec*)&rel, (const Vec*)&rel) < (w / 2.0f) * (w / 2.0f))) {
            continue;
        }
        fade = Ease(1.0f - PSVECDotProduct((const Vec*)&rel, (const Vec*)&rel) / ((w / 2.0f) * (w / 2.0f)));
        d = map->trucks;
        {
            f32 px;
            f32 py = rel.y + (y - h / 2.0f);
            px = rel.x + (x + w / 2.0f);
            part = truck == d->self ? 1 : 3;
            f32 size = fade * fn_80235EF0(d->art, part);
            fn_80235AFC(d->art, part, -1, px, py, 0.0f, size, size, d->icons[truck].angle);
        }
    }
    Vec2 tp;  // Actual final truck-position workspace; writes remain at their original points.
    DrawPlacedMarkers(&basis, x, y, w, h, radius, j);
    {
        TruckDraw* d = map->trucks;
        int part = view == d->self ? 1 : 3;
        f32 size = fn_80235EF0(d->art, part);
        f32 one = 1.0f;
        size = one * size;  // retail keeps the 1.0f multiply (fmuls after the call)
        fn_80235AFC(d->art, part, -1, x + w / 2.0f, y - h / 2.0f, 0.0f, size, size, d->icons[view].angle);
    }
    {
        RaceCarRec* tr = fn_8017960C(fn_801E866C(map->player));
        m = lbl_8055D5B4;
        f32 centerX = x + w / 2.0f;
        f32 centerY = y - h / 2.0f;
        Vec3 p = *(const Vec3*)&tr->mtx.pos;  // a plain Vec3 copy (RVec3 has a constructor)
        tp.x = p.x;
        tp.y = p.z;
        DrawFinalMarker(m, &basis, tp, centerX, centerY, w, radius);
    }
}

int fn_8001B308(const Vec2* pos, gArtSet* art, int part) {
    int i;
    for (i = 0; i < 16; i++) {
        if (lbl_803583C8[i].Set(pos, art, part)) {
            return i;
        }
    }
    return -1;
}

void fn_8001B450(int index) { lbl_803583C8[index].Reset(); }

static inline f32 NLerpT(f32 t, f32 a, f32 b) { return a + t * (b - a); }

// Point and heading on the path, t of the way from node n to the next (name unproven; retail has the same
// block in aicar fn_80195F6C and LineFollower::Locate).
// Squared length (name unproven): retail evaluates the squares inside an inline, which is what orders them
// against the caller's other float work.
static inline f32 MagSq(const Vec2& v) { return v.x * v.x + v.y * v.y; }
static inline void NodeAt(const IdealPoint* n, f32 t, Vec2* p, Vec2* d) {
    p->x = NLerpT(t, n->pos.x, n->next->pos.x);
    p->y = NLerpT(t, n->pos.y, n->next->pos.y);
    d->x = NLerpT(t, n->dir.x, n->next->dir.x);
    d->y = NLerpT(t, n->dir.y, n->next->dir.y);
}

void fn_8001B478(int index, f32 width) {
    LineFollower it;
    Vec2 p;
    Vec2 d;
    Vec2 v;
    CenterLine* path;
    f32 len;
    f32 n;
    int segs;
    f32 vscale;
    IdealPoint* node;
    int count;
    IdealPoint* q;
    int flag;
    f32 prev;
    int i;

    if (!it.SetLine((CenterLine*)index)) {
        return;
    }
    path = it.line;
    len = path->len[0];
    n = len / width;
    segs = n;
    if (n - segs > 0.5f) {
        segs++;
    }
    node = path->points;
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
            NodeAt(node, 0.0f, &p, &d);
            {
                f32 inv = 1.0f / (f32)sqrt(MagSq(d));
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
            fn_802C8E00(v.x, 0.0f, v.y, 0.0f, t, -1, flag);
            v.x = p.x + d.x * (width / 2);
            v.y = p.y + d.y * (width / 2);
            fn_802C8E00(v.x, 0.0f, v.y, 1.0f, t, -1, flag);
            flag = 1;
        }
    }
    fn_802C8E48();
}

void fn_8001B754(f32 width) {
    int i;
    int id;
    for (i = 0; (id = (int)fn_80156A40()->GetRacingLine(i)) != 0; i++) {
        fn_8001B478(id, width);
    }
}

void fn_8001B7B8(int pass) { fn_802D3050(pass); }

void fn_8001B7BC(TrackMap* map) {
    PlainValMatrix cam;
    f32 fov;
    f32 dist;
    int old;

    fn_802CB274(map->target);
    fn_802C9294(map->target, 0);
    fov = 0.34906584f;
    f32 t = tan(fov / 2);
    dist = 0.5f * map->width / t;
    Perspective proj(dist - 1.0f, 1.0f + dist, fov);
    Viewport vp(0, 0, map->texSize, map->texSize);
    fn_802C88CC(0xFFFFFF);
    old = fn_802C2C00(0);
    cam.origin.x = 0.0f;
    cam.origin.y = dist;
    cam.origin.z = 0.0f;
    cam.basis[0].x = 1.0f;
    cam.basis[0].y = 0.0f;
    cam.basis[0].z = 0.0f;
    cam.basis[1].x = 0.0f;
    cam.basis[1].y = 0.0f;
    cam.basis[1].z = 1.0f;
    cam.basis[2].x = 0.0f;
    cam.basis[2].y = -1.0f;
    cam.basis[2].z = 0.0f;
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
}

}  // extern "C"
