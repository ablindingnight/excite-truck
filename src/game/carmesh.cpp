// Car mesh resource loading (0x801D2008-0x801D21C0): loads a 'CMsh' resource, checks its version and
// byte-swaps its header. Names are guesses apart from the messages.
#include "game/common.h"

extern "C" {
void* fn_8012C370(const char* name, u32 type, u32* info, s32* size, bool* first, bool* swap);
int fn_8012C4D8(void* p);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void fn_801D1C68(void* mesh, void* base);
}

static inline void Swap4(void* v) {
    u8* p = (u8*)v;
    u8 t = p[0];
    p[0] = p[3];
    p[3] = t;
    t = p[1];
    p[1] = p[2];
    p[2] = t;
}

struct MeshPart {
    void Swap(void) {
        Swap4(&w[0]);
        Swap4(&w[1]);
        Swap4(&w[2]);
        Swap4(&w[3]);
        Swap4(&w[4]);
        Swap4(&w[5]);
        Swap4(&w[6]);
    }
    u32 w[7];
};

ModuleTag lbl_8055E920;

extern "C" {

void* fn_801D2008(const char* name) {
    u32 version = 0;
    bool swap = true;
    u32* mesh = (u32*)fn_8012C370(name, 'CMsh', &version, 0, 0, &swap);
    if (!mesh) {
        fn_802E317C("could not load car mesh %s", name);
    }
    if (version) {
        fn_802E317C("car mesh version mismatch: %d != %d", version, 0);
    }
    if (swap) {
        int i;
        for (i = 0; i < 8; i++) {
            ((MeshPart*)mesh)[i].Swap();
        }
        fn_801D1C68(mesh, mesh);
    }
    return mesh;
}

void fn_801D21B4(void* mesh) { fn_8012C4D8(mesh); }

}  // extern "C"
