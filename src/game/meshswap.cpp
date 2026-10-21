// Car mesh fix-up (0x801D1C68-0x801D2008): byte-swaps a loaded 'CMsh' resource's vertex, face,
// strip and bone tables and turns their file offsets into pointers. Names are guesses.
#include "game/common.h"

static inline void Swap4(void* v) {
    u8* p = (u8*)v;
    u8 t = p[0];
    p[0] = p[3];
    p[3] = t;
    t = p[1];
    p[1] = p[2];
    p[2] = t;
}

struct MeshVert {
    void Swap(void) {
        Swap4(&w[0]);
        Swap4(&w[1]);
        Swap4(&w[2]);
        Swap4(&w[3]);
        Swap4(&w[4]);
    }
    u32 w[5];
};

struct MeshFace {
    void Swap(void) {
        Swap4(&w[0]);
        Swap4(&w[1]);
        Swap4(&w[2]);
        Swap4(&w[3]);
    }
    u32 w[4];
};

struct MeshStrip {
    void Swap(void) {
        Swap4(&w[0]);
        Swap4(&w[1]);
        Swap4(&vert);
    }
    u32 w[2];
    MeshVert* vert;  // offset 0x8
};

struct MeshBone {
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

void fn_801D1C68(MeshHeader* m, u32 base) {
    Fix((void**)&m->verts, base);
    Fix((void**)&m->faces, base);
    Fix((void**)&m->strips, base);
    Fix((void**)&m->bones, base);
    int i;
    for (i = 0; i < m->numVerts; i++) {
        m->verts[i].Swap();
    }
    for (i = 0; i < m->numFaces; i++) {
        m->faces[i].Swap();
    }
    for (i = 0; i < m->numStrips; i++) {
        m->strips[i].Swap();
        m->strips[i].vert = &m->verts[(u32)m->strips[i].vert];
    }
    for (i = 0; i < m->numBones; i++) {
        m->bones[i].Swap();
    }
}

}  // extern "C"
