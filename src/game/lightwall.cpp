// Light walls (0x802A96E8-0x802A9F30): scrolling camera-facing light strips standing on a list of points,
// fading out with distance from the camera, plus a single fading textured gate between two points. Names
// are guesses.
#include "game/common.h"

extern "C" {
double fmod(double x, double y);
f32 PSVECDistance(const void* a, const void* b);
f32 PSVECSquareDistance(const void* a, const void* b);
void PSVECNormalize(const void* in, void* out);
f32 fn_801E7E18(void);  // time
void fn_802C8BD0(s32 tex);
int fn_802C8DBC(int n);
void fn_802C8E00(u32 color, int idx, f32 x, f32 y, f32 z, f32 u, f32 v);
void fn_802C8E48(void);
void fn_802C8D74(void);
void fn_801E4F20(int n);
int fn_801E4F3C(u32 color, int back, f32 x, f32 y, f32 z, f32 u, f32 v);
void fn_801E4FF4(void);
}

struct LVec {
    f32 x;
    f32 y;
    f32 z;
};

struct LCam {
    LVec right;  // offset 0x0
    LVec up;     // offset 0xC
    LVec fwd;    // offset 0x18
    LVec pos;    // offset 0x24
};

extern "C" LCam* fn_802C86DC(void);

struct LSeg {
    LSeg(void) {}
    LSeg(const LVec* p, const LVec* q) : a(*p), b(*q) {}
    void Set(const LVec* p, const LVec* q) {
        a = *p;
        b = *q;
    }
    LVec a;
    LVec b;
};

static inline f32 LRamp(f32 x, f32 lo, f32 hi) { return (x - lo) / (hi - lo); }
static inline f32 LMin(f32 a, f32 b) { return b < a ? b : a; }
static inline f32 LMax(f32 a, f32 b) { return a > b ? a : b; }
static inline f32 LLerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

static inline void LAddScaled(LVec* v, const LVec* d, f32 k) {
    v->x += d->x * k;
    v->y += d->y * k;
    v->z += d->z * k;
}
static inline void LAddScaled2(LVec* v, const LVec* d, f32 k) {
    v->x = v->x + k * d->x;
    v->y = v->y + k * d->y;
    v->z = v->z + k * d->z;
}
static inline void LAddScaled3(LVec* v, const LVec* d, f32 a, f32 b) {
    f32 k = a * b;
    v->x += d->x * k;
    v->y += d->y * k;
    v->z += d->z * k;
}

static inline f32 LScaleH(int n, f32 k) { return n * k / 72.0f; }

ModuleTag lbl_8055F910;

extern "C" {

void fn_802A96E8(const LVec* pts, int n, u32 color, s32 tex, s32 tex2) {
    const LVec* eye = &fn_802C86DC()->pos;
    const LVec* right = &fn_802C86DC()->right;
    LCam* cam = fn_802C86DC();
    fn_802C8BD0(tex);
    f32 t = fmod(fn_801E7E18(), 1.0);
    f32 v0 = 1.0f - t;
    f32 v1 = 1.75f + v0;
    if (fn_802C8DBC(n * 4)) {
        for (int i = 0; i < n; i++) {
            const LVec* p = &pts[i];
            f32 d = PSVECDistance(p, eye);
            if (d < 225.0f) {
                f32 k = 2.0f * d / 225.0f - 1.0f;
                int a = (int)(196.0f * (1.0f - k * k));
                int lo = 16;
                if (a > 16) {
                    lo = a;
                }
                int alpha = 196;
                if (lo < 196) {
                    alpha = lo;
                }
                f32 w = 5.0f * (d - 25.0f) / 200.0f;
                if (1.0f > w) {
                    w = 1.0f;
                }
                u32 c = (alpha << 24) | (color & 0xFFFFFF);
                LVec v;
                f32 s = 0.5f * -w;
                v.x = p->x + right->x * s;
                v.y = p->y + right->y * s;
                v.z = p->z + right->z * s;
                fn_802C8E00(c, 0, v.x, v.y, v.z, 0.0f, v0);
                v.x += right->x * w;
                v.y += right->y * w;
                v.z += right->z * w;
                fn_802C8E00(c, 0, v.x, v.y, v.z, 1.0f, v0);
                v.y += 105.0f;
                LAddScaled(&v, right, -w);
                fn_802C8E00(color & 0xFFFFFF, 1, v.x, v.y, v.z, 0.0f, v1);
                v.x += right->x * w;
                v.y += right->y * w;
                v.z += right->z * w;
                fn_802C8E00(color & 0xFFFFFF, 1, v.x, v.y, v.z, 1.0f, v1);
            }
        }
        fn_802C8E48();
    }
    fn_802C8D74();
    if (tex2) {
        fn_802C8BD0(tex2);
        fn_802C8DBC(n * 4);
        for (int i = 0; i < n; pts++, i++) {
            f32 d = PSVECDistance(pts, eye);
            if (d < 225.0f) {
                f32 k = 2.0f * d / 225.0f - 1.0f;
                int a = (int)(196.0f * (1.0f - k * k));
                int lo = 16;
                if (a > 16) {
                    lo = a;
                }
                int alpha = 196;
                if (lo < 196) {
                    alpha = lo;
                }
                f32 w = 5.0f * (d - 25.0f) / 200.0f;
                if (1.0f > w) {
                    w = 1.0f;
                }
                u32 c = (alpha << 24) | (color & 0xFFFFFF);
                LVec v;
                f32 s = 2.0f * -w;
                v.x = pts->x + right->x * s;
                v.y = pts->y + right->y * s;
                v.z = pts->z + right->z * s;
                LAddScaled(&v, &cam->fwd, 0.1f);
                fn_802C8E00(c, 0, v.x, v.y, v.z, 0.0f, v0);
                s = 4.0f * w;
                v.x += right->x * s;
                v.y += right->y * s;
                v.z += right->z * s;
                fn_802C8E00(c, 0, v.x, v.y, v.z, 1.0f, v0);
                v.y += 105.0f;
                LAddScaled(&v, right, 4.0f * -w);
                fn_802C8E00(color & 0xFFFFFF, 1, v.x, v.y, v.z, 0.0f, v1);
                s = 4.0f * w;
                v.x += right->x * s;
                v.y += right->y * s;
                v.z += right->z * s;
                fn_802C8E00(color & 0xFFFFFF, 1, v.x, v.y, v.z, 1.0f, v1);
            }
        }
        fn_802C8E48();
        fn_802C8D74();
    }
}

void fn_802A9BFC(const LVec* from, const LVec* to, s32 tex, f32 fade, int u0, int v0, int u1, int v1) {
    LSeg seg(from, to);
    f32 top = seg.a.y > seg.b.y ? seg.a.y : seg.b.y;
    seg.b.y = 10.0f + top;
    seg.a.y = seg.b.y;
    f32 len = 16.0f;
    if (PSVECSquareDistance(&seg.a, &seg.b) < len * len) {
        len -= PSVECDistance(&seg.a, &seg.b);
        LVec dir;
        dir.x = seg.a.x - seg.b.x;
        dir.y = seg.a.y - seg.b.y;
        dir.z = seg.a.z - seg.b.z;
        PSVECNormalize(&dir, &dir);
        LAddScaled(&seg.a, &dir, len / 2);
        LAddScaled(&seg.b, &dir, -len / 2);
    }
    f32 m = LMax(LMin(LRamp(fade, 0.0f, 0.5f), 1.0f), 0.0f);
    int alpha = (int)LLerp(0.0f, 200.0f, m);
    u32 c = (alpha << 24) | 0xFFFFFFF;
    f32 fu0 = u0 / 256.0f;
    f32 fu1 = u1 / 256.0f;
    f32 fv0 = v0 / 256.0f;
    f32 fv1 = v1 / 256.0f;
    f32 h = (v1 - v0) * (PSVECDistance(&seg.a, &seg.b) / 3.0f) / 72.0f;
    fn_802C8BD0(tex);
    fn_801E4F20(4);
    fn_801E4F3C(c, 0, seg.a.x, seg.a.y, seg.a.z, fu0, fv1);
    fn_801E4F3C(c, 0, seg.b.x, seg.b.y, seg.b.z, fu1, fv1);
    fn_801E4F3C(c, 1, seg.a.x, seg.a.y + h, seg.a.z, fu0, fv0);
    fn_801E4F3C(c, 1, seg.b.x, seg.b.y + h, seg.b.z, fu1, fv0);
    fn_801E4FF4();
    fn_802C8D74();
}

}  // extern "C"
