// Physics collision (0x801E1F8C-0x801E2C1C): pairs the physics objects of the frame against each other
// (through the PhobList grid) and against the two static collision trees (the track's track.sol
// and one supplied by the game), and casts rays against the same ("PhysCollideCheckRay"), falling
// back to a slow test of every object for near-horizontal rays. Names are guesses apart from the
// message and the file name.
#include "game/common.h"
#include "game/solgroup.h"
#include <revolution/MTX/vec.h>
#include "game/collshape.h"
#include "game/collvol.h"
#include "game/mathutil.h"



struct PhobNode {
    CollVolume* phob;      // offset 0x0
    PhobNode* next;  // offset 0x4
};

struct PhobCell {
    PhobNode* head;  // offset 0x0
};

// Walk over the objects of a set of grid cells: the object, its list node and the cell.
struct PhobIter {  /* Helper name unproven */
    CollVolume* p;
    PhobNode* nd;
    PhobCell** w;

    PhobIter(PhobCell** cells) : p(0), nd(0), w(cells) {}
};

typedef int (*RayFilter)(s32 id);

struct RayQuery {
    RVec3 from;         // offset 0x0
    RVec3 dir;          // offset 0xC
    f32* t;            // offset 0x18
    u8 hit;            // offset 0x1C
    RayFilter filter;  // offset 0x20
};

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_801B619C(PhobCell** out, int* n, const RVec3* pos);
SolGroup* fn_801A4548(const char* name);
void fn_801A47FC(SolGroup* g);
void fn_801A47C8(SolGroup* g);
void fn_8015F670(SolGroup** groups, int n);
int fn_801A4E88(const RVec3* from, const RVec3* dir, f32* t, RVec3* normal, s32* surf);  // terrain
extern s32 lbl_8055E3C4;  // frame counter
}

ModuleTag lbl_8055EA28;
UnkStaticFFFF lbl_8055EA30;
UnkStaticFFFF lbl_8055EA34;
u8 lbl_8055EA38;      // collision active
SolGroup* lbl_8055EA3C;  // track.sol
SolGroup* lbl_8055EA40;  // game-supplied

extern "C" {

static inline void CollideShapes2(CollVolume* a, CollShape* s) {
    int j;
    for (j = 0; j < a->nAdded; j++) {
        a->sub[j]->vf10((u8*)s);
    }
}

void fn_801E1F8C(CollVolume* a, CollVolume* b) {
    CollShape* s;
    int j;
    int i;
    if (a->nAdded && a->Shape(0)->surface == 0x72 && b->nAdded && b->Shape(0)->surface == 0x72) {
        return;
    }
    for (i = 0; i < b->nAdded; i++) {
        CollideShapes2(a, b->Shape(i));
    }
}

static inline void CollideShapes(CollVolume* p, CollShape* s) {
    for (int i = 0; i < p->nAdded; i++) {
        p->sub[i]->vf10((u8*)s);
    }
}

void fn_801E2084(CollShape* s, CollVolume* p) {
    CollideShapes(p, s);
}

void fn_801E2100(CollShape* s, RayQuery* q) {
    if (!q->filter || q->filter(s->surface)) {
        RVec3 n;
        if (s->vf18(&q->from, &q->dir, q->t, &n)) {
            q->hit = 1;
        }
    }
}

void fn_801E2188(void) { lbl_8055EA3C = fn_801A4548("track.sol"); }

void fn_801E21B4(SolGroup* g) { lbl_8055EA40 = g; }

void fn_801E21BC(void) {
    lbl_8055EA38 = 1;
    SolGroup* g[2];
    g[0] = lbl_8055EA3C;
    g[1] = lbl_8055EA40;
    for (u32 i = 0; i < 2; i++) {
        fn_801A47FC(g[i]);
    }
    fn_8015F670(g, 2);
}

void fn_801E222C(void) {
    lbl_8055EA38 = 0;
    if (lbl_8055EA3C) {
        lbl_8055EA3C->release(lbl_8055EA3C);
        lbl_8055EA3C = 0;
    }
    if (lbl_8055EA40) {
        lbl_8055EA40->release(lbl_8055EA40);
        lbl_8055EA40 = 0;
    }
}

void fn_801E2294(void) {
    if (lbl_8055EA3C) {
        fn_801A47C8(lbl_8055EA3C);
    }
    if (lbl_8055EA40) {
        fn_801A47C8(lbl_8055EA40);
    }
}


static inline void QuadCollide(SolGroup* g, CollVolume* p, f32 scale) {
    int x = (int)(scale * p->mtx.pos.x);
    int z = (int)(scale * p->mtx.pos.z);
    int minX = -40000000;
    int minZ = -40000000;
    int maxX = 40000000;
    int maxZ = 40000000;
    int node = 0;
    for (;;) {
        QuadNode* nd = &g->nodes[node];
        int end = nd->end;
        for (int k = nd->first; k < end; k++) {
            StaticCollObject* it = g->items[g->index[k]];
            if (it->b40) {
                fn_801E2084(it->shape, p);
            }
        }
        node = nd->child;
        if (!node) {
            break;
        }
        int mx = (minX + maxX) >> 1;
        int mz = (minZ + maxZ) >> 1;
        if (z <= mz) {
            maxZ = mz;
            if (x <= mx) {
                maxX = mx;
            } else {
                minX = mx;
                node += 1;
            }
        } else {
            minZ = mz;
            if (x <= mx) {
                maxX = mx;
                node += 2;
            } else {
                minX = mx;
                node += 3;
            }
        }
    }
}

void fn_801E22D0(CollVolume** list, int n) {
    for (int i = 0; i < n; i++) {
        CollVolume* p = list[i];
        for (int j = 0; j < p->nAdded; j++) {
            p->Shape(j)->vf0C();
        }
    }
    f32 scale = 10.0f;
    for (int i = 0; i < n; i++) {
        CollVolume* p = list[i];
        if (p->nAdded > 0) {
            int nc = 5;
            PhobCell* cells[5];
            fn_801B619C(cells, &nc, &p->mtx.pos);
            PhobIter it(cells);
            int k;
            for (k = 0; k < nc; it.w++, k++) {
                for (it.nd = (*it.w)->head; it.nd; it.nd = it.nd->next) {
                    it.p = it.nd->phob;
                    if (p < it.p || !it.p->vf44()) {
                        fn_801E1F8C(p, it.p);
                    }
                }
            }
            if (lbl_8055EA3C) {
                QuadCollide(lbl_8055EA3C, p, scale);
            }
            if (lbl_8055EA40) {
                QuadCollide(lbl_8055EA40, p, scale);
            }
        }
    }
}

static inline void QuadRay(SolGroup* g, const RVec3* from, RayQuery* q, f32 scale) {
    int x = (int)(scale * from->x);
    int z = (int)(scale * from->z);
    int minX = -40000000;
    int minZ = -40000000;
    int maxX = 40000000;
    int maxZ = 40000000;
    int node = 0;
    for (;;) {
        QuadNode* nd = &g->nodes[node];
        int end = nd->end;
        for (int k = nd->first; k < end; k++) {
            StaticCollObject* it = g->items[g->index[k]];
            if (it->b40) {
                fn_801E2100(it->shape, q);
            }
        }
        node = nd->child;
        if (!node) {
            break;
        }
        int mx = (minX + maxX) >> 1;
        int mz = (minZ + maxZ) >> 1;
        if (z <= mz) {
            maxZ = mz;
            if (x <= mx) {
                maxX = mx;
            } else {
                minX = mx;
                node += 1;
            }
        } else {
            minZ = mz;
            if (x <= mx) {
                maxX = mx;
                node += 2;
            } else {
                minX = mx;
                node += 3;
            }
        }
    }
}

int fn_801E2640(const RVec3* from, const RVec3* to, RVec3* outPos, RVec3* outNormal, s32* outSurf, CollVolume** outBody,
                CollVolume** list, int n, RayFilter filter) {
    RVec3 d;
    d.x = to->x - from->x;
    d.y = to->y - from->y;
    d.z = to->z - from->z;
    f32 len = PSVECMag((const Vec*)&d);
    f32 inv = 1.0f / MaxF(0.01f, len);
    RVec3 dir;
    dir.x = d.x * inv;
    dir.y = d.y * inv;
    dir.z = d.z * inv;
    f32 t = PSVECDotProduct((const Vec*)&dir, (const Vec*)&d);
    int hit = 0;
    *outBody = 0;
    if ((f32)__fabs(dir.y) > 0.9f) {
        int nc = 5;
        PhobCell* cells[5];
        fn_801B619C(cells, &nc, from);
        PhobIter it(cells);
        int k;
        for (k = 0; k < nc; it.w++, k++) {
            for (it.nd = (*it.w)->head; it.nd; it.nd = it.nd->next) {
                it.p = it.nd->phob;
                if (it.p && it.p->vf44()) {
                    for (int j = 0; j < it.p->nAdded; j++) {
                        CollShape* s = it.p->Shape(j);
                        s32 id = s->surface;
                        if (!filter || filter(id)) {
                            if (s->vf18(from, &dir, &t, outNormal)) {
                                *outSurf = id;
                                hit = 1;
                                *outBody = it.p;
                            }
                        }
                    }
                }
            }
        }
        SolGroup* g[2];
        g[0] = lbl_8055EA3C;
        g[1] = lbl_8055EA40;
        RayQuery q;
        q.from = *from;
        q.dir = dir;
        q.t = &t;
        q.hit = hit;
        q.filter = filter;
        f32 scale = 10.0f;
        for (u32 i = 0; i < 2; i++) {
            if (g[i]) {
                QuadRay(g[i], from, &q, scale);
            }
        }
        hit = q.hit;
    } else {
        fn_802E30C8("physcollide: using slow PhysCollideCheckRay path %08x", lbl_8055E3C4);
        for (int i = 0; i < n; i++) {
            CollVolume* p = list[i];
            if (p && p->vf44()) {
                for (int j = 0; j < p->nAdded; j++) {
                    CollShape* s = p->Shape(j);
                    s32 id = s->surface;
                    if (!filter || filter(id)) {
                        if (s->vf18(from, &dir, &t, outNormal)) {
                            *outSurf = id;
                            hit = 1;
                            *outBody = p;
                        }
                    }
                }
            }
        }
        SolGroup* g[2];
        g[0] = lbl_8055EA3C;
        g[1] = lbl_8055EA40;
        for (u32 i = 0; i < 2; i++) {
            if (g[i]) {
                for (int k = 0; k < g[i]->count; k++) {
                    StaticCollObject* it = g[i]->items[k];
                    if (it->b40) {
                        if (it->shape->vf18(from, &dir, &t, outNormal)) {
                            hit = 1;
                            *outSurf = it->shape->surface;
                            *outBody = 0;
                        }
                    }
                }
            }
        }
    }
    s32 surf;
    if (fn_801A4E88(from, &dir, &t, outNormal, &surf)) {
        *outSurf = surf;
        hit = 1;
        *outBody = 0;
    }
    if (hit) {
        f32 tt = t;
        outPos->x = from->x + dir.x * tt;
        outPos->y = from->y + dir.y * tt;
        outPos->z = from->z + dir.z * tt;
    }
    return hit;
}

}  // extern "C"
