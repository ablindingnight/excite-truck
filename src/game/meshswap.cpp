// Car mesh fix-up (0x801D1C68-0x801D2008): byte-swaps a loaded 'CMsh' resource's vertex, face,
// strip and bone tables and turns their file offsets into pointers. Names are guesses.
#include "game/common.h"

static inline void Swap4(void* v) {
    u8* p = (u8*)v;
    s8 t = p[0];
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
    MeshVert* v;
    int iv;
    MeshFace* f;
    int jf;
    MeshStrip* s;
    int ks;
    int lb;
    unsigned offset;
    MeshBone* b;
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
        b->Swap();
    }
}

}  // extern "C"
