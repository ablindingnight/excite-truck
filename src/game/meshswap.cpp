// Car mesh fix-up (0x801D1C68-0x801D2008): byte-swaps a loaded 'CMsh' resource's vertex, face,
// strip and bone tables and turns their file offsets into pointers. Names are guesses.
#include "game/common.h"
#include "game/byteswap.h"

struct MeshVert {
    void Swap(void) {
        SwapWord(&w[0]);
        SwapWord(&w[1]);
        SwapWord(&w[2]);
        SwapWord(&w[3]);
        SwapWord(&w[4]);
    }
    u32 w[5];
};

struct MeshFace {
    void Swap(void) {
        SwapWord(&w[0]);
        SwapWord(&w[1]);
        SwapWord(&w[2]);
        SwapWord(&w[3]);
    }
    u32 w[4];
};

struct MeshStrip {
    void Swap(void) {
        SwapWord(&w[0]);
        SwapWord(&w[1]);
        SwapWord(&vert);
    }
    u32 w[2];
    MeshVert* vert;  // offset 0x8
};

struct MeshBone {
    u32 w[7];
};

struct MeshHeader {
    MeshVert* verts;    // offset 0x0
    MeshFace* faces;    // offset 0x4
    MeshStrip* strips;  // offset 0x8
    MeshBone* bones;    // offset 0xC
    s32 numVerts;       // offset 0x10
    s32 numFaces;       // offset 0x14
    s32 numStrips;      // offset 0x18
    s32 numBones;       // offset 0x1C
};

// Turns a file offset into a pointer, keeping null as null.
static inline void Fix(void** p, u32 base) {
    if (*p) {
        *p = (void*)(base + (u32)*p);
    } else {
        *p = 0;
    }
}

extern "C" {

// PROVISIONAL: shared byte-swap scratch ownership and function lifetime policy.
// The ordinary compiled source matches retail; original author spelling is unproven.
#pragma push
#pragma opt_lifetimes off
void fn_801D1C68(MeshHeader* m, u32 base) {
    Fix((void**)&m->verts, base);
    Fix((void**)&m->faces, base);
    Fix((void**)&m->strips, base);
    Fix((void**)&m->bones, base);
    MeshVert* v;
    int iv;
    MeshFace* f;
    int jf;
    MeshStrip* s;
    int ks;
#if defined(__MWERKS__) || defined(GAME_FILE_DATA_NEEDS_SWAP)
    unsigned byteScratch;
#endif
    unsigned offset;
    MeshBone* b;
    int lb;
    for (iv = 0; iv < m->numVerts; iv++) {
        v = &m->verts[iv];
        v->Swap();
    }
    for (jf = 0; jf < m->numFaces; jf++) {
        f = &m->faces[jf];
        f->Swap();
    }
    for (ks = 0; ks < m->numStrips; ks++) {
        s = &m->strips[ks];
        s->Swap();
        m->strips[ks].vert = &m->verts[(u32)m->strips[ks].vert];
    }
    for (lb = 0, offset = 0; lb < m->numBones; lb++, offset += sizeof(*m->bones)) {
        b = (MeshBone*)((u8*)m->bones + offset);
#if defined(__MWERKS__) || defined(GAME_FILE_DATA_NEEDS_SWAP)
        byteScratch = ((u8*)b)[0];
        ((u8*)b)[0] = ((u8*)b)[3];
        ((u8*)b)[3] = byteScratch;
        byteScratch = ((u8*)b)[1];
        ((u8*)b)[1] = ((u8*)b)[2];
        ((u8*)b)[2] = byteScratch;
        byteScratch = ((u8*)b)[4];
        ((u8*)b)[4] = ((u8*)b)[7];
        ((u8*)b)[7] = byteScratch;
        byteScratch = ((u8*)b)[5];
        ((u8*)b)[5] = ((u8*)b)[6];
        ((u8*)b)[6] = byteScratch;
        byteScratch = ((u8*)b)[8];
        ((u8*)b)[8] = ((u8*)b)[11];
        ((u8*)b)[11] = byteScratch;
        byteScratch = ((u8*)b)[9];
        ((u8*)b)[9] = ((u8*)b)[10];
        ((u8*)b)[10] = byteScratch;
        byteScratch = ((u8*)b)[12];
        ((u8*)b)[12] = ((u8*)b)[15];
        ((u8*)b)[15] = byteScratch;
        byteScratch = ((u8*)b)[13];
        ((u8*)b)[13] = ((u8*)b)[14];
        ((u8*)b)[14] = byteScratch;
        byteScratch = ((u8*)b)[16];
        ((u8*)b)[16] = ((u8*)b)[19];
        ((u8*)b)[19] = byteScratch;
        byteScratch = ((u8*)b)[17];
        ((u8*)b)[17] = ((u8*)b)[18];
        ((u8*)b)[18] = byteScratch;
        byteScratch = ((u8*)b)[20];
        ((u8*)b)[20] = ((u8*)b)[23];
        ((u8*)b)[23] = byteScratch;
        byteScratch = ((u8*)b)[21];
        ((u8*)b)[21] = ((u8*)b)[22];
        ((u8*)b)[22] = byteScratch;
        byteScratch = ((u8*)b)[24];
        ((u8*)b)[24] = ((u8*)b)[27];
        ((u8*)b)[27] = byteScratch;
        byteScratch = ((u8*)b)[25];
        ((u8*)b)[25] = ((u8*)b)[26];
        ((u8*)b)[26] = byteScratch;
#endif
    }
}
#pragma pop


}  // extern "C"
