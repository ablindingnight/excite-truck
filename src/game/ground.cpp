// Ground queries (0x801E9C80-0x801EA1FC): height, surface point and normal lookups on the track
// collision mesh, with an optional cached triangle hint. Names are guesses.
#include "game/common.h"

struct V3 {
    void Set(f32 x_, f32 y_, f32 z_) {
        x = x_;
        y = y_;
        z = z_;
    }
    f32 x, y, z;
};

extern "C" {
f32 PSVECDotProduct(const void* a, const void* b);
int fn_801DCE14(s16* hint, const V3* pos, V3* outPos, V3* outNormal, int flags);
void fn_801DCEF4(void);
void fn_801DCF00(void);
void fn_801DD3BC(int a, int on);
void fn_801DD808(void);
}

extern const s16 lbl_80562E28 = -2;  // shared "no hint" value

ModuleTag lbl_8055EB48;

extern "C" {

void fn_801E9C80(void) {}

int fn_801E9C84(const V3* from, const V3* at, V3* outPos, V3* outNormal, s16* hint, int flags, f32 maxDist) {
    V3 p = *from;
    V3 n;
    V3 d;
    f32 dist;
    if (!fn_801DCE14(hint, from, &p, &n, flags)) {
        return 0;
    }
    d.x = at->x - p.x;
    d.y = at->y - p.y;
    d.z = at->z - p.z;
    dist = PSVECDotProduct(&n, &d);
    if ((f32)__fabs(dist) < maxDist) {
        f32 t = -dist;
        outPos->x = at->x + n.x * t;
        outPos->y = at->y + n.y * t;
        outPos->z = at->z + n.z * t;
        outNormal->x = n.x;
        outNormal->y = n.y;
        outNormal->z = n.z;
        return 1;
    }
    return 0;
}

void fn_801E9DC4(s16* hint) { *hint = -1; }

f32 fn_801E9DDC(s16* hint, f32 x, f32 z);

f32 fn_801E9DD0(s16* hint, f32 x, f32 z) { return fn_801E9DDC(hint, x, z); }

void fn_801E9DD4(void) {}

void fn_801E9DD8(void) {}

f32 fn_801E9DDC(s16* hint, f32 x, f32 z) {
    V3 p;
    V3 out;
    p.x = x;
    p.y = 0.0f;
    p.z = z;
    if (fn_801DCE14(hint, &p, &out, 0, 0)) {
        return out.y;
    }
    return 0.0f;
}

int fn_801E9E30(s16* hint, V3* pos) {
    V3 p = *pos;
    return fn_801DCE14(hint, &p, pos, 0, 0);
}

int fn_801E9E78(s16* hint, V3* pos, V3* normal, int flags) {
    V3 p = *pos;
    return fn_801DCE14(hint, &p, pos, normal, flags);
}

int fn_801E9EC4(s16* hint, V3* pos, V3* normal, int flags, f32 x, f32 z) {
    V3 p;
    p.Set(x, pos->y, z);
    return fn_801DCE14(hint, &p, pos, normal, flags);
}

int fn_801E9F08(s16* hint, const V3* at, V3* outPos, V3* outNormal, int flags, f32 maxDist) {
    V3 p;
    f32 z;
    f32 y;
    f32 x;
    x = at->x;
    y = at->y;
    z = at->z;
    p.Set(x, y - (0.5f + maxDist), z);
    if (fn_801E9C84(&p, at, outPos, outNormal, hint, 0, maxDist)) {
        p.x = outPos->x;
        p.y = outPos->y;
        p.z = outPos->z;
        if (fn_801E9C84(&p, at, outPos, outNormal, hint, flags, maxDist)) {
            return 1;
        }
    }
    return 0;
}

f32 fn_801E9FDC(const s16* hint, f32 x, f32 z) {
    s16 h = *hint;
    V3 p;
    V3 out;
    p.x = x;
    p.y = 0.0f;
    p.z = z;
    if (fn_801DCE14(&h, &p, &out, 0, 0)) {
        return out.y;
    }
    return 0.0f;
}

int fn_801EA03C(const s16* hint, V3* pos) {
    s16 h = *hint;
    V3 p = *pos;
    pos->y = 0.0f;
    return fn_801DCE14(&h, &p, pos, 0, 0);
}

int fn_801EA098(const s16* hint, V3* pos, V3* normal, int flags, f32 x, f32 z) {
    s16 h = *hint;
    V3 p;
    p.Set(x, pos->y, z);
    return fn_801DCE14(&h, &p, pos, normal, flags);
}

f32 fn_801EA0E8(f32 x, f32 z) {
    V3 p;
    V3 out;
    p.x = x;
    p.y = 0.0f;
    p.z = z;
    if (fn_801DCE14(0, &p, &out, 0, 0)) {
        return out.y;
    }
    return 0.0f;
}

int fn_801EA140(V3* pos) {
    V3 p = *pos;
    return fn_801DCE14(0, &p, pos, 0, 0);
}

int fn_801EA18C(V3* pos, V3* normal, int flags) {
    V3 p = *pos;
    return fn_801DCE14(0, &p, pos, normal, flags);
}

void fn_801EA1D8(void) { fn_801DCEF4(); }

void fn_801EA1DC(void) { fn_801DCF00(); }

void fn_801EA1E0(int a, int on) { fn_801DD3BC(a, on != 0); }

void fn_801EA1F0(void) { fn_801DD808(); }

}  // extern "C"
