// Camera script recording and playback (0x80278214-0x802789A8): records camera frames per tick,
// replays them by interpolation, and drives screenshot "movie" capture over a tick range. Names
// are guesses apart from the messages and allocation tag.
#include "game/common.h"
#include "macros.h"

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);  // snprintf
void fn_802D08B4(int a, int b, int c);
void fn_802D07E4(const char* name, int a);
}

void* operator new[](size_t size, const char* tag);

struct CamFrame {
    ValMatrix mtx;  // offset 0x0
};

struct ScriptNode {
    ScriptNode(void) {}
    CamFrame frame;  // offset 0x0
    f32 fov;         // offset 0x30
    s32 tick;        // offset 0x34
};

struct ScriptCam {
    u8 pad0[0x58];
    CamFrame frame;  // offset 0x58
    u8 pad88[0xBC - 0x88];
    f32 fov;  // offset 0xBC
};

struct ScriptPlayer {
    u8 pad0[0x10];
    s32 mode;  // offset 0x10
};

extern "C" {
void fn_8026E22C(CamFrame* out, const ScriptNode* a, const ScriptNode* b, f32 t);
ScriptPlayer* fn_802678E4(int player);
}

static inline void StrCopyN(char* dst, const char* src, int len) {
    char* d = dst;
    int n = len;
    const char* s = src;
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

#define ROUND(x) ((x) < 0.0f ? (x) - 0.5f : (x) + 0.5f)

extern s32 lbl_8055E3C4;  // frame tick

ModuleTag lbl_8055F5A8;
ScriptNode* lbl_8055F5B0;  // nodes
s32 lbl_8055F5B4;          // node count
u8 lbl_8055F5B8;           // recording
u8 lbl_8055F5B9;           // movie armed
u8 lbl_8055F5BA;           // movie running
u8 lbl_8055F5BB;
s32 lbl_8055F5BC;  // movie end tick
s32 lbl_8055F5C0;  // movie start tick
s32 lbl_8055F5C4;  // last captured tick
u8 lbl_8055F5C8;
u8 lbl_8055F5C9;          // out-of-nodes warned
char lbl_804A15D0[0x100];  // movie name

static inline ScriptNode* NodeAt(int i) { return &lbl_8055F5B0[i]; }

static inline ScriptNode* NodeAt(int i);

extern "C" {

void fn_80278214(void) {
    lbl_8055F5B4 = 0;
    lbl_8055F5C8 = 0;
    lbl_8055F5BB = 0;
    lbl_8055F5BA = 0;
    lbl_8055F5B9 = 0;
    lbl_8055F5BC = -1;
    lbl_8055F5C4 = -1;
    lbl_8055F5B8 = 0;
    lbl_8055F5B0 = new ("cscript_node") ScriptNode[2];
}

void fn_80278288(void) {
    delete[] lbl_8055F5B0;
    lbl_8055F5B0 = 0;
    lbl_8055F5B4 = 0;
}

void fn_802782C4(void) {
    lbl_8055F5B8 = 1;
    lbl_8055F5B4 = 0;
    lbl_8055F5BA = 0;
    lbl_8055F5B9 = 0;
    fn_802E30C8("script: begin recording on tick %d", lbl_8055E3C4);
    lbl_8055F5BC = -1;
    lbl_8055F5C0 = -1;
}

void fn_80278318(void) {
    fn_802E30C8("script: end recording on tick %d", lbl_8055E3C4);
    lbl_8055F5B8 = 0;
    lbl_8055F5BA = 0;
    lbl_8055F5B9 = 0;
}

u8 fn_80278358(void) { return lbl_8055F5B8; }

void fn_80278360(const ScriptCam* cam) {
    if (lbl_8055F5B8 && lbl_8055F5B4 == 0 && lbl_8055E3C4 > 10) {
        return;
    }
    if (lbl_8055F5B8 && lbl_8055F5B4 < 2) {
        ScriptNode* n;
        if (lbl_8055F5B4 != 0 && lbl_8055E3C4 - NodeAt(lbl_8055F5B4 - 1)->tick < 2) {
            return;
        }
        n = &lbl_8055F5B0[lbl_8055F5B4++];
        n->tick = lbl_8055E3C4;
        n->frame = cam->frame;
        n->fov = cam->fov;
    } else if (!lbl_8055F5C9) {
        lbl_8055F5C9 = 1;
        fn_802E30C8("%s", "script: out of nodes!");
    }
}

void fn_8027847C(ScriptCam* cam) {
    int i;
    for (i = 0; i + 2 < lbl_8055F5B4; i++) {
        if (lbl_8055F5B0[i + 1].tick > lbl_8055E3C4) {
            break;
        }
    }
    if (i >= 0 && i + 1 < lbl_8055F5B4) {
        ScriptNode* a = &lbl_8055F5B0[i];
        ScriptNode* b = &lbl_8055F5B0[i + 1];
        f32 span = MAX((f32)(b->tick - a->tick), 0.001f);
        f32 t = (f32)(lbl_8055E3C4 - a->tick) / span;
        t = MIN(1.0f, t);
        t = MAX(t, 0.0f);
        fn_8026E22C(&cam->frame, a, b, t);
        cam->fov = a->fov + t * (b->fov - a->fov);
        if (lbl_8055F5B9 && !lbl_8055F5BA && lbl_8055E3C4 >= lbl_8055F5C0 && lbl_8055E3C4 < lbl_8055F5C0 + 10) {
            lbl_8055F5B9 = 0;
            lbl_8055F5BA = 1;
            lbl_8055F5C4 = -1;
            fn_802E30C8("script: started movie (%d - %d)", lbl_8055F5C0, lbl_8055F5BC);
        } else if (lbl_8055F5BA && lbl_8055E3C4 > NodeAt(lbl_8055F5B4 - 1)->tick) {
            lbl_8055F5BA = 0;
            fn_802E30C8("script: stopped movie");
        }
        if (lbl_8055F5BA && lbl_8055E3C4 != lbl_8055F5C4) {
            int frame;
            lbl_8055F5C4 = lbl_8055E3C4;
            frame = lbl_8055E3C4 - lbl_8055F5C0;
            if (frame >= 0 && frame <= lbl_8055F5BC - lbl_8055F5C0 && lbl_8055F5C0 <= lbl_8055F5BC && frame % 2 == 0) {
                char name[0x100];
                // The original literal is followed by four extra NUL bytes in .data (cause unknown).
                fn_802E30C8("script: screenshot on tick %d / %d\0\0\0\0", frame, lbl_8055F5BC - lbl_8055F5C0);
                fn_801345D8(name, sizeof(name), "sd:\\%s_%04d.baz", lbl_804A15D0, frame / 2);
                fn_802D08B4(2, 2, 1);
                fn_802D07E4(name, 1);
            }
        }
    } else {
        cam->frame.mtx.basis[0].x = 1.0f;
        cam->frame.mtx.basis[0].y = 0.0f;
        cam->frame.mtx.basis[0].z = 0.0f;
        cam->frame.mtx.basis[1].x = 0.0f;
        cam->frame.mtx.basis[1].y = 1.0f;
        cam->frame.mtx.basis[1].z = 0.0f;
        cam->frame.mtx.basis[2].x = 0.0f;
        cam->frame.mtx.basis[2].y = 0.0f;
        cam->frame.mtx.basis[2].z = 1.0f;
        cam->frame.mtx.origin.x = 0.0f;
        cam->frame.mtx.origin.y = 0.0f;
        cam->frame.mtx.origin.z = 0.0f;
        cam->fov = 1.5707964f;
    }
}

void fn_80278778(const char* name) {
    if (lbl_8055F5B8) {
        fn_802E30C8("script: can't make a movie while recording!");
        return;
    }
    StrCopyN(lbl_804A15D0, name, sizeof(lbl_804A15D0));
    lbl_8055F5B9 = 1;
    lbl_8055F5BA = 0;
}

void fn_802787E4(void) {
    lbl_8055F5BA = 0;
    lbl_8055F5B9 = 0;
}

u8 fn_802787F4(void) { return lbl_8055F5BA; }

int fn_802787FC(void) {
    int r = 0;
    if (lbl_8055F5BA || lbl_8055F5BB) {
        r = 1;
    }
    return r;
}

int fn_80278820(void) {
    if (fn_802678E4(0)->mode == 25 && lbl_8055F5B9 && lbl_8055E3C4 < lbl_8055F5C0) {
        return 1;
    }
    return lbl_8055F5BA ? 0 : lbl_8055F5C8;
}

void fn_8027888C(u8 v) { lbl_8055F5C8 = v; }

void fn_80278894(u8 v) { lbl_8055F5BB = v; }

void fn_8027889C(f32 sec) {
    f32 t = sec / 0.016683351f;
    lbl_8055F5C0 = ROUND(t);
}

void fn_802788E0(f32 sec) {
    f32 t = sec / 0.016683351f;
    lbl_8055F5BC = ROUND(t);
}

int fn_80278924(f32* start, f32* end) {
    int ok = 0;
    *start = 0.016683351f * lbl_8055F5C0;
    *end = 0.016683351f * lbl_8055F5BC;
    if (lbl_8055F5C0 >= 0 && lbl_8055F5BC >= lbl_8055F5C0) {
        ok = 1;
    }
    return ok;
}

}  // extern "C"
