// GenericDrawList (0x801E76A8-0x801E78A0): a growable triangle-strip vertex/index buffer with a
// C-style API. Names are guesses apart from the allocation tags.
#include "game/common.h"

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

struct DrawVtx {
    f32 x, y, z;  // offset 0x0
    u32 color;    // offset 0xC
    f32 u, v;     // offset 0x10
};

extern "C" void fn_802D68C4(DrawVtx* verts, s32 nverts, u16* idx, s32 nidx);

class GenericDrawList {
public:
    GenericDrawList(s32 max);
    ~GenericDrawList(void);
    void Add(f32 x, f32 y, f32 z, u32 color, f32 u, f32 v, int strip);
    void Draw(void);

    DrawVtx* verts;  // offset 0x0
    s32 count;       // offset 0x4
    s32 max;         // offset 0x8
    u16* idx;        // offset 0xC
    s32 nidx;        // offset 0x10
    s32 maxIdx;      // offset 0x14
};

ModuleTag lbl_8055EAD8;

GenericDrawList::GenericDrawList(s32 n) {
    verts = new ("DrawList verts") DrawVtx[n];
    max = n;
    count = 0;
    s32 m = (n - 2) * 3;
    idx = new ("DrawList idxs") u16[m];
    nidx = 0;
    maxIdx = m;
}

GenericDrawList::~GenericDrawList(void) {
    delete[] idx;
    delete[] verts;
}

void GenericDrawList::Add(f32 x, f32 y, f32 z, u32 color, f32 u, f32 v, int strip) {
    DrawVtx* p = &verts[count];
    p->x = x;
    p->y = y;
    p->z = z;
    p->color = color;
    p->u = u;
    p->v = v;
    count++;
    if (strip && count > 1) {
        idx[nidx++] = count - 3;
        idx[nidx++] = count - 2;
        idx[nidx++] = count - 1;
    }
}

void GenericDrawList::Draw(void) { fn_802D68C4(verts, count, idx, nidx); }

extern "C" {

GenericDrawList* fn_801E7840(s32 max) { return new ("GenericDrawList") GenericDrawList(max); }

void fn_801E7888(GenericDrawList* list) { delete list; }

void fn_801E7890(GenericDrawList* list, f32 x, f32 y, f32 z, u32 color, f32 u, f32 v, int strip) {
    list->Add(x, y, z, color, u, v, strip);
}

void fn_801E7894(GenericDrawList* list) { list->Draw(); }

}  // extern "C"
