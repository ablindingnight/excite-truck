// Model test screen (0x8024BDDC-0x8024C070): the debug model viewer ("ModelTest!"), preceded by the
// multiplayer event menu's adjustor thunks. Names are guesses
// apart from the strings.
#include "game/common.h"

extern "C" {
void fn_8024BB98(u8* p);
void fn_8024A110(u8* p);
void fn_8024BBF4(u8* p);
void fn_8024BCC0(u8* p);
void fn_8024A1AC(u8* p);
s32 fn_802CAC28(const char* name, int flags);  // load texture
void fn_802CAC54(s32 tex);
void fn_802E30C8(const char* fmt, ...);
void fn_802C8530(void);
void fn_802C87B4(int x, int y, int w, int h);
void fn_802C86E8(u32 color);
void fn_802C88BC(int mode);
void fn_802C974C(f32 near, f32 far, f32 fov);
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
void fn_802C855C(void);
void fn_802C2B48(void);
void fn_80129AFC(void);
int fn_802E47B8(void);
int fn_802E46F0(void);
extern s32 lbl_8055CFF0;
extern s32 lbl_8055CFF4;
extern const f32 lbl_80563910;  // 10.0f: view distance
}

struct MTVec {
    MTVec(void) {}
    MTVec(f32 ax, f32 ay, f32 az) {
        x = ax;
        y = ay;
        z = az;
    }
    f32 x, y, z;
};

struct MTVtx {
    MTVec pos;   // offset 0x0
    u32 color;   // offset 0xC
    f32 u, v;    // offset 0x10
};

struct MTMtx {
    void SetView(f32 dist) {
        basis[0].x = 1.0f;
        basis[0].y = 0.0f;
        basis[0].z = 0.0f;
        basis[1].x = 0.0f;
        basis[1].y = 1.0f;
        basis[1].z = 0.0f;
        basis[2].x = 0.0f;
        basis[2].y = 0.0f;
        basis[2].z = 1.0f;
        origin.x = 0.0f;
        origin.y = 0.0f;
        origin.z = 0.0f - dist;
    }
    MTVec basis[3];
    MTVec origin;
};

extern "C" {
void fn_802C8638(MTMtx* m);
void fn_802C8DB8(MTVtx* v, int nv, const u16* idx, int ni);
}

ModuleTag lbl_8055F2A0;

extern "C" {

void fn_8024BDDC(u8* p) { fn_8024BB98(p - 0x34); }

void fn_8024BDE4(u8* p) { fn_8024A110(p - 0x34); }

void fn_8024BDEC(u8* p) { fn_8024BBF4(p - 0x18); }

void fn_8024BDF4(u8* p) { fn_8024BCC0(p - 0x18); }

void fn_8024BDFC(u8* p) { fn_8024A1AC(p - 0x18); }

// The model/texture test viewer: draws leather.tex on a unit quad until ESC is pressed.
// NonMatching: the target builds v[1..3] by copying earlier vertices (lfd/stfd) and keeps four position
// temporaries on the stack; matrix and calls match.
void fn_8024BE04(u8* p) {
    fn_802E30C8("ModelTest!");
    s32 tex = fn_802CAC28("leather.tex", 0);
    MTMtx view;
    MTMtx* vp = &view;
    for (u32 frame = 0;; frame++) {
        vp->SetView(lbl_80563910);
        fn_802C8530();
        fn_802C87B4(0, 0, lbl_8055CFF0, lbl_8055CFF4);
        fn_802C86E8(0xFF7F7F7F);
        fn_802C88BC(3);
        fn_802C974C(0.2f, 1000.0f, 1.5707964f);
        fn_802C8638(vp);
        MTVtx v[4];
        const u16 idx[6] = {0, 1, 2, 1, 3, 2};
        v[0].pos = MTVec(0.0f, 0.0f, 0.0f);
        v[0].color = 0xFFFFFFFF;
        v[0].u = 0.0f;
        v[0].v = 0.0f;
        v[1].pos = MTVec(1.0f, 0.0f, 0.0f);
        v[1].color = 0xFFFFFFFF;
        v[1].u = 1.0f;
        v[1].v = 0.0f;
        v[2].pos = MTVec(0.0f, 1.0f, 0.0f);
        v[2].color = 0xFFFFFFFF;
        v[2].u = 0.0f;
        v[2].v = 1.0f;
        v[3].pos = MTVec(1.0f, 1.0f, 0.0f);
        v[3].color = 0xFFFFFFFF;
        v[3].u = 1.0f;
        v[3].v = 1.0f;
        fn_802C8BD0(tex);
        fn_802C8DB8(v, 4, idx, 6);
        fn_802C8D74();
        fn_802C855C();
        fn_802C2B48();
        if ((frame & 0x3F) == 0) {
            fn_802E30C8("frame %08x", frame);
        }
        fn_80129AFC();
        if (fn_802E47B8() && (u16)fn_802E46F0() == 0x1B) {
            break;
        }
    }
    fn_802CAC54(tex);
    fn_802E30C8("-ModelTest");
}

}  // extern "C"
