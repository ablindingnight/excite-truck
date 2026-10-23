// Chainlink fences (0x802A0940-0x802A0D60): fence posts arrive as world events and are drawn as
// textured strips between the posts. Names are guesses apart from the allocation tag and texture.
#include "game/common.h"

void* operator new[](size_t size, const char* tag);

extern "C" {
s32 fn_802CAC28(const char* name, int flags);  // load texture
void fn_802CAC54(s32 tex);
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
}

struct FencePost {
    FencePost(void) {}
    f32 x, y, z;
};

struct FenceVtx {
    ValVec3 pos;  // offset 0x0
    u32 color;    // offset 0xC
    f32 u;        // offset 0x10
    f32 v;        // offset 0x14
};

extern "C" void fn_802C8DB8(FenceVtx* v, int nv, const u16* idx, int ni);

struct Fence {
    s32 type;            // offset 0x0
    s32 id;              // offset 0x4
    s32 count;           // offset 0x8
    FencePost posts[6];  // offset 0xC
};

typedef int (*EventHandler)(const Fence* e);

extern "C" {
void fn_801EEFDC(EventHandler h, u8 type);
void fn_801EF024(EventHandler h);
}

s32 lbl_8055CB68 = -2;  // fence texture

ModuleTag lbl_8055F828;
UnkStaticFFFF lbl_8055F830;
Fence* lbl_8055F834;

extern "C" {

void fn_802A0940(void) {}

int fn_802A0944(const Fence* e) {
    int i;
    if (e->type != 1) {
        i = e->id;
    } else {
        i = 63 - e->id;
    }
    lbl_8055F834[i] = *e;
    return sizeof(Fence);
}

void fn_802A09B0(void) {
    int i;
    lbl_8055F834 = new ("chainlink") Fence[64];
    for (i = 0; i < 64; i++) {
        lbl_8055F834[i].count = 0;
    }
    lbl_8055CB68 = fn_802CAC28("fencepol.tex", 0);
    fn_801EEFDC(fn_802A0944, 6);
}

void fn_802A0AFC(void) {
    fn_801EF024(fn_802A0944);
    fn_802CAC54(lbl_8055CB68);
    lbl_8055CB68 = -2;
    delete[] lbl_8055F834;
    lbl_8055F834 = 0;
}

void fn_802A0B50(void) {}

// NonMatching: the hoisted float constants get different callee-saved registers (1.0 lands in f27
// in the target).
void fn_802A0B54(void) {
    int i;
    fn_802C8BD0(lbl_8055CB68);
    for (i = 0; i < 64; i++) {
        Fence* f = &lbl_8055F834[i];
        if (f->count > 1) {
            u16 idx[32];
            FenceVtx v[12];
            f32 v0 = 0.25f * f->type;
            f32 len = 1.0f + (f->type * 6);
            int j;
            for (j = 0; j < f->count; j++) {
                FenceVtx* p = &v[j * 2];
                p[0].pos.x = f->posts[j].x;
                p[0].pos.y = f->posts[j].y;
                p[0].pos.z = f->posts[j].z;
                p[0].u = (j / 5.0f) * len;
                p[0].v = v0;
                p[0].color = 0xFFFFFFFF;
                p[1] = p[0];
                p[1].pos.y += 0.15f;
                p[1].v = (0.25f + v0);
                if (j > 0) {
                    u16* q = &idx[(j - 1) * 6];
                    u16 b = (j - 1) * 2;
                    q[0] = b;
                    q[1] = b + 2;
                    q[2] = b + 1;
                    q[3] = b + 1;
                    q[4] = b + 2;
                    q[5] = b + 3;
                }
            }
            fn_802C8DB8(v, f->count * 2, idx, (f->count - 1) * 6);
        }
    }
    fn_802C8D74();
}

}  // extern "C"
