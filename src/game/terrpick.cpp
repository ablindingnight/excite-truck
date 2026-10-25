// Terrain picking (0x80203FE0-0x80204330): a set of stub hooks, plus the screen-to-terrain pick that
// walks a ray from the camera until it lands on the ground. Names are guesses apart from the message.
#include "game/common.h"

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
void PSVECNormalize(const void* src, void* dst);
void fn_801E9DC4(s16* h);
int fn_801E9E78(s16* h, ValVec3* pos, ValVec3* normal, void* info);
extern s32 lbl_8055CFF0;  // screen width
extern s32 lbl_8055CFF4;  // screen height
extern f32 lbl_8055FC88;  // projection scale
}

struct PickCam {
    ValVec3 basis[3];
    ValVec3 pos;  // offset 0x24
};

ModuleTag lbl_8055ED20;

static inline u32 MakeILPos(u32 seg, u32 frac) { return (seg << 16) | frac; }

u32 lbl_8055ED28 = MakeILPos(0xFFFF, 0);
u32 lbl_8055ED2C = MakeILPos(0x7F00, 0);

extern "C" {

f32 fn_80203FE0(void) { return 16.0f; }
void fn_80203FE8(void) {}
void fn_80203FEC(void) {}
f32 fn_80203FF0(void) { return 0.0f; }
void fn_80203FF8(void) {}
void fn_80203FFC(void) {}
void fn_80204000(void) {}
void fn_80204004(void) {}
void fn_80204008(void) {}
int fn_8020400C(void) { return 0; }
void fn_80204014(void) {}
void fn_80204018(void) {}
void fn_8020401C(void) {}
void fn_80204020(void) {}
int fn_80204024(void) { return 0; }
int fn_8020402C(void) { return 0; }
void fn_80204034(void) {}
void fn_80204038(void) {}
int fn_8020403C(void) { return 1; }
void fn_80204044(void) {}
void fn_80204048(void) {}
void fn_8020404C(void) {}
void fn_80204050(void) {}

// Finds the terrain under screen point (sx, sy); returns 0 when there is none.
int fn_80204054(ValVec3* out, ValVec3* outNormal, s16* hint, const PickCam* cam, int sx, int sy) {
    ValVec3 dir;
    ValVec3 hit;
    ValVec3 normal;
    ValVec3 p;
    u32 info;
    s16 localHint[2];
    int halfW = lbl_8055CFF0 / 2;
    f32 depth = halfW / lbl_8055FC88;
    f32 ax = cam->basis[2].x * depth;
    f32 az = cam->basis[2].z * depth;
    f32 ay = cam->basis[2].y * depth;
    f32 dx = sx - halfW;
    f32 bx = cam->basis[0].x * dx;
    f32 by = cam->basis[0].y * dx;
    f32 bz = cam->basis[0].z * dx;
    f32 dy = -(f32)(sy - lbl_8055CFF4 / 2);
    dir.x = ax + bx + cam->basis[1].x * dy;
    dir.y = ay + by + cam->basis[1].y * dy;
    dir.z = az + bz + cam->basis[1].z * dy;
    PSVECNormalize(&dir, &dir);
    if (hint == NULL) {
        hint = localHint;
    }
    f32 step = 10.0f;
    f32 dist = step;
    int fail;
    int halved = 0;
    do {
        p.x = cam->pos.x + dir.x * dist;
        p.y = cam->pos.y + dir.y * dist;
        p.z = cam->pos.z + dir.z * dist;
        hit = p;
        int back = 0;
        fn_801E9DC4(hint);
        fail = !fn_801E9E78(hint, &hit, &normal, &info);
        if (!fail) {
            if (hit.y > p.y) {
                back = 1;
            } else if ((f32)__fabs(hit.y - p.y) > 0.0254f) {
                if (halved) {
                    step *= 0.5f;
                }
                dist += step;
            } else {
                break;
            }
        } else {
            back = 1;
        }
        if (back) {
            step *= 0.5f;
            halved = 1;
            dist -= step;
        }
        if (dist > 1e7f) {
            fail = 1;
            fn_802E30C8("TERRAIN SYSTEM IS LYING: no terrain is really at %.2f, %.2f", p.x, p.z);
            break;
        }
    } while (step > 0.01f);
    if (!fail) {
        *out = hit;
        if (outNormal) {
            *outNormal = normal;
        }
    }
    return !fail;
}

}  // extern "C"

// NonMatching: the sinit is one of the epilogue-scheduled kind.
