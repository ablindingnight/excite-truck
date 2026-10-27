// Radar (0x801390B0-0x801398D8): draws the HUD radar - the background art, then a blip for every other
// car, positioned by its offset from the camera rotated into the camera's heading, scaled and faded by
// distance. Names are guesses apart from the art names.
#include "game/common.h"

extern "C" {
double sqrt(double x);
f32 PSVECMag(const void* v);
void fn_80148134(f32* ox, f32* oy, f32 x, f32 y, int mode);  // layout -> screen
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
int fn_802C8DBC(int n);
void fn_802C8E00(u32 color, int flag, f32 x, f32 y, f32 z, f32 u, f32 v);
void fn_802C8E48(void);
u8* fn_8017960C(int i);
int fn_801B8570(int i);
}

extern f32 lbl_80558DC4[];  // FLT_EPSILON (MSL)
extern s32 lbl_8055E4D0;    // car count

struct V2 {
    f32 x;
    f32 y;
};

struct V3 {
    f32 x;
    f32 y;
    f32 z;
};

struct LayoutArt {
    u8 pad0[4];
    V2 pos;   // offset 0x4
    u8 padC[4];
    V2 size;  // offset 0x10
    f32 u0;   // offset 0x18
    f32 v0;   // offset 0x1C
    f32 u1;   // offset 0x20
    f32 v1;   // offset 0x24
};

class Layout {
public:
    virtual ~Layout(void);
    virtual void* Find(const char* name);          // 0xC
    virtual LayoutArt* FindArt(const char* name);  // 0x10
};

extern "C" void fn_801476D8(LayoutArt* art, u32 color, s32 tex);

struct Camera {
    u8 pad0[0x18];
    V3 dir;  // offset 0x18
    V3 pos;  // offset 0x24
};

extern "C" Camera* fn_801E8718(void);

struct Frame {
    V3 rows[3];  // offset 0x0
    V3 pos;      // offset 0x24
};

ModuleTag lbl_8055E118;
UnkStaticFFFF lbl_8055E120;
static UnkStatic123 lbl_8042F7A8;
UnkStatic7F lbl_8055E124;
UnkStaticFFFF lbl_8055E128;

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline f32 LerpF(f32 a, f32 b, f32 t) { return a + t * (b - a); }
static inline int NearZero(f32 x) { return (f32)__fabs(x) <= lbl_80558DC4[0]; }

static inline f32 Clamp01(f32 t) {
    if (1.0f < t) {
        t = 1.0f;
    }
    t = t <= 0.0f ? 0.0f : t;
    return t;
}

// Stripped by the linker; its constants come first in the pool.
f32 RadarClampStub(f32 t) { return Clamp01(t); }

extern "C" {

void fn_801390B0(Layout* l, V2* pos, V2* dir, int focus, f32 scale, f32 alpha) {
    LayoutArt* art = l->FindArt(focus ? "focus_blip" : "other_blip");
    f32 h = scale * art->size.y / 2;
    f32 w = scale * art->size.x / 2;
    V2 fwd;
    V2 side;
    fwd.x = dir->x * h;
    fwd.y = dir->y * h;
    side.x = dir->y * w;
    side.y = -(dir->x * w);
    if (art) {
        f32 x;
        f32 y;
        u32 color = ((s32)(255.0f * alpha) << 24) | 0xFFFFFF;
        fn_80148134(&x, &y, fwd.x + (pos->x - side.x), fwd.y + (pos->y - side.y), 0);
        fn_802C8E00(color, 0, x, y, 0.0f, art->u0, art->v0);
        fn_80148134(&x, &y, fwd.x + (pos->x + side.x), fwd.y + (pos->y + side.y), 0);
        fn_802C8E00(color, 0, x, y, 0.0f, art->u1, art->v0);
        fn_80148134(&x, &y, pos->x - side.x - fwd.x, pos->y - side.y - fwd.y, 0);
        fn_802C8E00(color, 1, x, y, 0.0f, art->u0, art->v1);
        fn_80148134(&x, &y, pos->x + side.x - fwd.x, pos->y + side.y - fwd.y, 0);
        fn_802C8E00(color, 1, x, y, 0.0f, art->u1, art->v1);
    }
}

int fn_801392F8(u8* car) {
    if (*(s32*)(car + 0x1F0) == 0x50) {
        return car[0x260] & 3;
    }
    return 0;
}

}  // extern "C"

static inline void MulVec(const Frame& m, const V3& src, V3* dst) {
    if (dst == &src) {
        f32 x = Dot3(src.x, m.rows[0].x, src.y, m.rows[0].y, src.z, m.rows[0].z);
        f32 y = Dot3(src.x, m.rows[1].x, src.y, m.rows[1].y, src.z, m.rows[1].z);
        f32 z = Dot3(src.x, m.rows[2].x, src.y, m.rows[2].y, src.z, m.rows[2].z);
        dst->x = x;
        dst->y = y;
        dst->z = z;
    } else {
        dst->x = Dot3(src.x, m.rows[0].x, src.y, m.rows[0].y, src.z, m.rows[0].z);
        dst->y = Dot3(src.x, m.rows[1].x, src.y, m.rows[1].y, src.z, m.rows[1].z);
        dst->z = Dot3(src.x, m.rows[2].x, src.y, m.rows[2].y, src.z, m.rows[2].z);
    }
}

extern "C" {

void fn_80139318(Layout* l, s32 tex, int self, f32 range) {
    Frame m;
    V3 v;
    V3 p;
    LayoutArt* bg;
    Camera* cam;
    int i;
    u8* car;
    f32 len;
    f32 dist;
    f32 scale;
    f32 alpha;
    fn_801476D8(l->FindArt("radar_bg_art"), -1, tex);
    bg = l->FindArt("radar_rect");
    V2 origin = bg->pos;
    V2 corner;
    V2 h;
    V2 d;
    V2 screen;
    V2 size;
    size.x = bg->size.x;
    size.y = bg->size.y;
    corner.x = origin.x + size.x;
    corner.y = origin.y + size.y;
    fn_801392F8(fn_8017960C(self));
    cam = fn_801E8718();
    if ((f32)__fabs(cam->dir.x) <= lbl_80558DC4[0] && (f32)__fabs(cam->dir.z) <= lbl_80558DC4[0]) {
        return;
    }
    h.x = cam->dir.x;
    h.y = cam->dir.z;
    len = 1.0f / (f32)sqrt(h.x * h.x + h.y * h.y);
    h.x *= len;
    h.y *= len;
    m.rows[2].y = 0.0f;
    m.rows[0].y = 0.0f;
    m.rows[2].x = h.x;
    m.rows[2].z = h.y;
    {
        f32 t = h.x;
        h.x = h.y;
        h.y = -t;
    }
    m.rows[0].x = h.x;
    m.rows[0].z = h.y;
    m.rows[1].x = 0.0f;
    m.rows[1].y = 1.0f;
    m.rows[1].z = 0.0f;
    m.pos = cam->pos;
    fn_802C8BD0(tex);
    if (fn_802C8DBC(0x20)) {
        for (i = lbl_8055E4D0 - 1; i >= 0; i--) {
            car = fn_8017960C(i);
            if (!fn_801B8570(i)) {
                continue;
            }
            MulVec(m, *(V3*)(car + 0x54), &v);
            d.x = v.x;
            d.y = v.z * -1.0f;
            len = sqrt(d.x * d.x + d.y * d.y);
            if (0.01f > len) {
                len = 0.01f;
            }
            len = 1.0f / len;
            d.x *= len;
            d.y *= len;
            p.x = *(f32*)(car + 0x60) - m.pos.x;
            p.y = *(f32*)(car + 0x64) - m.pos.y;
            p.z = *(f32*)(car + 0x68) - m.pos.z;
            MulVec(m, p, &p);
            dist = PSVECMag(&p);
            scale = LerpF(1.0f, 0.6f, Clamp01((dist - 0.7f * range) / (2.0f * range - 0.7f * range)));
            alpha = LerpF(1.0f, 0.3f, Clamp01((dist - 2.0f * range) / (6.0f * range - 2.0f * range)));
            if (dist > range) {
                f32 mag = PSVECMag(&p);
                if (!((f32)__fabs(mag) <= lbl_80558DC4[0])) {
                    f32 k = range / mag;
                    p.x *= k;
                    p.y *= k;
                    p.z *= k;
                }
            }
            screen.x = origin.x + Clamp01((p.x - -range) / (range - -range)) * (corner.x - origin.x);
            screen.y = origin.y + Clamp01((p.z - range) / (-range - range)) * (corner.y - origin.y);
            fn_801390B0(l, &screen, &d, i == self, scale, alpha);
        }
        fn_802C8E48();
    }
    fn_802C8D74();
}

}  // extern "C"
