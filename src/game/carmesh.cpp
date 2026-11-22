// Car mesh resource loading (0x801D2008-0x801D21C0): loads a 'CMsh' resource, checks its version and
// byte-swaps its header. Names are guesses apart from the messages.
#include "game/common.h"
#include "game/byteswap.h"

extern "C" {
void* fn_8012C370(const char* name, u32 type, u32* info, s32* size, bool* first, bool* swap);
int fn_8012C4D8(void* p);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void FixMeshHeaders(void* mesh, void* base);
}

struct MeshPart {
    void Swap(void) {
        SwapWord(&w[0]);
        SwapWord(&w[1]);
        SwapWord(&w[2]);
        SwapWord(&w[3]);
        SwapWord(&w[4]);
        SwapWord(&w[5]);
        SwapWord(&w[6]);
    }
    u32 w[7];
};

ModuleTag lbl_8055E920;

extern "C" {

void* LoadCarMesh(const char* name) {
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
        FixMeshHeaders(mesh, mesh);
    }
    return mesh;
}

void fn_801D21B4(void* mesh) { fn_8012C4D8(mesh); }

}  // extern "C"
