// Radar (0x801390B0-0x801398D8): draws the HUD radar - the background art, then a blip for every other
// car, positioned by its offset from the camera rotated into the camera's heading, scaled and faded by
// distance. Names are guesses apart from the art names.
#include "game/common.h"
#include "game/carbody.h"
#include "game/textelem.h"
#include "game/ilayout.h"
#include "game/v2.h"
#include <revolution/MTX/vec.h>
#include "game/plainvec.h"
#include "game/mathutil.h"
#include "game/clamp01.h"
#include "game/makeilpos.h"

extern "C" {
double sqrt(double x);
void fn_80148134(f32* ox, f32* oy, f32 x, f32 y, int mode);  // layout -> screen
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
int fn_802C8DBC(int n);
void fn_802C8E00(f32 x, f32 y, f32 z, f32 u, f32 v, u32 color, int flag);
void fn_802C8E48(void);
u8* fn_8017960C(int i);
int fn_801B8570(int i);
}

extern f32 __float_epsilon[];  // FLT_EPSILON (MSL)
extern s32 lbl_8055E4D0;    // car count

extern "C" void fn_801476D8(const ArtElem* art, u32 color, s32 tex);

// Camera: common.h's PlainValMatrix.
extern "C" PlainValMatrix* fn_801E8718(int);

// Frame: common.h's PlainValMatrix.
ModuleTag lbl_8055E118;

extern const u32 lbl_8055E120 = MakeILPos(0xFFFF, 0);
static UnkStatic123 lbl_8042F7A8;
u32 lbl_8055E124 = MakeILPos(0x7F00, 0);
extern const u32 lbl_8055E128 = MakeILPos(0xFFFF, 0);


static inline int NearZero(f32 x) { return (f32)__fabs(x) <= __float_epsilon[0]; }


// Stripped by the linker; its constants come first in the pool.
f32 RadarClampStub(f32 t) { return Clamp01(t); }

/* Helper name unproven */
static inline f32 Remap(f32 x, f32 inLo, f32 inHi, f32 outLo, f32 outHi) {
    f32 hi = outHi;
    f32 lo = outLo;
    f32 t = Clamp01((x - inLo) / (inHi - inLo));
    return lo + t * (hi - lo);
}

extern "C" {

void fn_801390B0(ILayout* l, V2* pos, V2* dir, int focus, f32 scale, f32 alpha) {
    ArtElem* art = l->FindArt(focus ? "focus_blip" : "other_blip");
    f32 h = scale * art->h / 2;
    f32 w = scale * art->w / 2;
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
        fn_802C8E00(x, y, 0.0f, art->u0, art->v0, color, 0);
        fn_80148134(&x, &y, fwd.x + (pos->x + side.x), fwd.y + (pos->y + side.y), 0);
        fn_802C8E00(x, y, 0.0f, art->u1, art->v0, color, 0);
        fn_80148134(&x, &y, pos->x - side.x - fwd.x, pos->y - side.y - fwd.y, 0);
        fn_802C8E00(x, y, 0.0f, art->u0, art->v1, color, 1);
        fn_80148134(&x, &y, pos->x + side.x - fwd.x, pos->y + side.y - fwd.y, 0);
        fn_802C8E00(x, y, 0.0f, art->u1, art->v1, color, 1);
    }
}

int fn_801392F8(u8* car) {
    if (((CarBody*)car)->f1F0 == 0x50) {
        return car[0x260] & 3;
    }
    return 0;
}

}  // extern "C"

static inline void MulVec(const PlainValMatrix& m, const V3& src, V3* dst) {
    if (dst == &src) {
        f32 x = Dot3(src.x, m.basis[0].x, src.y, m.basis[0].y, src.z, m.basis[0].z);
        f32 y = Dot3(src.x, m.basis[1].x, src.y, m.basis[1].y, src.z, m.basis[1].z);
        f32 z = Dot3(src.x, m.basis[2].x, src.y, m.basis[2].y, src.z, m.basis[2].z);
        dst->x = x;
        dst->y = y;
        dst->z = z;
    } else {
        dst->x = Dot3(src.x, m.basis[0].x, src.y, m.basis[0].y, src.z, m.basis[0].z);
        dst->y = Dot3(src.x, m.basis[1].x, src.y, m.basis[1].y, src.z, m.basis[1].z);
        dst->z = Dot3(src.x, m.basis[2].x, src.y, m.basis[2].y, src.z, m.basis[2].z);
    }
}

extern "C" {


void fn_80139318(ILayout* l, s32 tex, int self, f32 range) {
    PlainValMatrix m;
    V3 v;
    V3 p;
    ArtElem* bg;
    PlainValMatrix* cam;
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
    f32 sizeY = bg->h;
    size.x = bg->w;
    size.y = sizeY;
    corner.x = origin.x + size.x;
    corner.y = origin.y + size.y;
    cam = fn_801E8718(fn_801392F8(fn_8017960C(self)));
    if (FAbs(cam->basis[2].x) <= __float_epsilon[0] && FAbs(cam->basis[2].z) <= __float_epsilon[0]) {
        return;
    }
    h.x = cam->basis[2].x;
    h.y = cam->basis[2].z;
    len = 1.0f / (f32)sqrt(h.x * h.x + h.y * h.y);
    h.x *= len;
    h.y *= len;
    m.basis[2].y = 0.0f;
    m.basis[0].y = 0.0f;
    m.basis[2].x = h.x;
    m.basis[2].z = h.y;
    {
        f32 t = h.x;
        h.x = h.y;
        h.y = -t;
    }
    m.basis[0].x = h.x;
    m.basis[0].z = h.y;
    m.basis[1].x = 0.0f;
    m.basis[1].y = 1.0f;
    m.basis[1].z = 0.0f;
    m.origin = cam->origin;
    fn_802C8BD0(tex);
    if (fn_802C8DBC(0x20)) {
        for (i = lbl_8055E4D0 - 1; i >= 0; i--) {
            car = fn_8017960C(i);
            if (!fn_801B8570(i)) {
                continue;
            }
            MulVec(m, *(V3*)(car + 0x54), &v);
            d.x = v.x;
            d.y = v.z;
            d.y *= -1.0f;
            len = sqrt(d.x * d.x + d.y * d.y);
            if (0.01f > len) {
                len = 0.01f;
            }
            len = 1.0f / len;
            d.x *= len;
            d.y *= len;
            p.x = ((CarBody*)car)->mtx.pos.x - m.origin.x;
            p.y = ((CarBody*)car)->mtx.pos.y - m.origin.y;
            p.z = ((CarBody*)car)->mtx.pos.z - m.origin.z;
            MulVec(m, p, &p);
            dist = PSVECMag((const Vec*)&p);
            scale = Lerp(1.0f, 0.6f, Clamp01((dist - 0.7f * range) / (2.0f * range - 0.7f * range)));
            alpha = Lerp(1.0f, 0.3f, Clamp01((dist - 2.0f * range) / (6.0f * range - 2.0f * range)));
            if (dist > range) {
                f32 mag = PSVECMag((const Vec*)&p);
                if (!((f32)__fabs(mag) <= __float_epsilon[0])) {
                    f32 k = range / mag;
                    p.x *= k;
                    p.y *= k;
                    p.z *= k;
                }
            }
            screen.x = Remap(p.x, -range, range, origin.x, corner.x);
            screen.y = Remap(p.z, range, -range, origin.y, corner.y);
            fn_801390B0(l, &screen, &d, i == self, scale, alpha);
        }
        fn_802C8E48();
    }
    fn_802C8D74();
}

}  // extern "C"
