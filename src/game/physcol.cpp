// Physics collision (0x801E1F8C-0x801E2C1C): pairs the physics objects of the frame against each other
// (through the PhobList grid) and against the two static collision trees (the track's track.sol
// and one supplied by the game), and casts rays against the same ("PhysCollideCheckRay"), falling
// back to a slow test of every object for near-horizontal rays. Names are guesses apart from the
// message and the file name.
#include "game/common.h"

struct PVec {
    f32 x;
    f32 y;
    f32 z;
};

class Phob;

// A collision primitive owned by an object.
class Shape {
public:
    virtual void vf08(void);
    virtual void Reset(void);                                                // 0x0C
    virtual void Collide(Shape* other);                                      // 0x10
    virtual void vf14(void);
    virtual int Ray(const PVec* from, const PVec* dir, f32* t, PVec* normal);  // 0x18

    u8 pad4[0x1C];
    s32 id;  // offset 0x20: surface/owner id
};

class Phob {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void vf1C(void);
    virtual void vf20(void);
    virtual void vf24(void);
    virtual void vf28(void);
    virtual void vf2C(void);
    virtual void vf30(void);
    virtual void vf34(void);
    virtual void vf38(void);
    virtual void vf3C(void);
    virtual void vf40(void);
    virtual int Solid(void);  // 0x44

    Shape* Get(int i) {
        if (i >= 0 && i < count) {
            return shapes[i];
        }
        return 0;
    }

    u8 pad4[0x10];
    Shape* shapes[4];  // offset 0x14
    s32 count;         // offset 0x24
    u8 pad28[0x38];
    PVec pos;          // offset 0x60
};

struct PhobNode {
    Phob* phob;      // offset 0x0
    PhobNode* next;  // offset 0x4
};

struct PhobCell {
    PhobNode* head;  // offset 0x0
};

struct QuadNode {
    u16 first;  // offset 0x0
    u16 end;    // offset 0x2
    u16 child;  // offset 0x4
    u16 pad6;
};

struct GroupItem {
    u8 pad0[0x3C];
    Shape* shape;  // offset 0x3C
    u8 active;     // offset 0x40
};

struct Group {
    s32 count;              // offset 0x0
    GroupItem** items;      // offset 0x4
    u32 unk8;               // offset 0x8
    u16* index;             // offset 0xC
    QuadNode* nodes;        // offset 0x10
    void (*release)(Group* g);  // offset 0x14
};

typedef int (*RayFilter)(s32 id);

struct RayQuery {
    PVec from;         // offset 0x0
    PVec dir;          // offset 0xC
    f32* t;            // offset 0x18
    u8 hit;            // offset 0x1C
    RayFilter filter;  // offset 0x20
};

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
f32 PSVECMag(const PVec* v);
f32 PSVECDotProduct(const PVec* a, const PVec* b);
void fn_801B619C(PhobCell** out, int* n, const PVec* pos);
Group* fn_801A4548(const char* name);
void fn_801A47FC(Group* g);
void fn_801A47C8(Group* g);
void fn_8015F670(Group** groups, int n);
int fn_801A4E88(const PVec* from, const PVec* dir, f32* t, PVec* normal, s32* surf);  // terrain
extern s32 lbl_8055E3C4;  // frame counter
}

ModuleTag lbl_8055EA28;
UnkStaticFFFF lbl_8055EA30;
UnkStaticFFFF lbl_8055EA34;
u8 lbl_8055EA38;      // collision active
Group* lbl_8055EA3C;  // track.sol
Group* lbl_8055EA40;  // game-supplied

extern "C" {

void fn_801E1F8C(Phob* a, Phob* b) {
    Shape* s;
    int j;
    int i;
    if (a->count && a->Get(0)->id == 0x72 && b->count && b->Get(0)->id == 0x72) {
        return;
    }
    for (i = 0; i < b->count; i++) {
        s = b->Get(i);
        for (j = 0; j < a->count; j++) {
            a->shapes[j]->Collide(s);
        }
    }
}

void fn_801E2084(Shape* s, Phob* p) {
    for (int i = 0; i < p->count; i++) {
        p->shapes[i]->Collide(s);
    }
}

void fn_801E2100(Shape* s, RayQuery* q) {
    if (!q->filter || q->filter(s->id)) {
        PVec n;
        if (s->Ray(&q->from, &q->dir, q->t, &n)) {
            q->hit = 1;
        }
    }
}

void fn_801E2188(void) { lbl_8055EA3C = fn_801A4548("track.sol"); }

void fn_801E21B4(Group* g) { lbl_8055EA40 = g; }

void fn_801E21BC(void) {
    lbl_8055EA38 = 1;
    Group* g[2];
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

static inline f32 Max(f32 a, f32 b) { return (a > b) ? a : b; }

static inline void QuadCollide(Group* g, Phob* p, f32 scale) {
    int x = (int)(scale * p->pos.x);
    int z = (int)(scale * p->pos.z);
    int minX = -40000000;
    int minZ = -40000000;
    int maxX = 40000000;
    int maxZ = 40000000;
    int node = 0;
    for (;;) {
        QuadNode* nd = &g->nodes[node];
        int end = nd->end;
        for (int k = nd->first; k < end; k++) {
            GroupItem* it = g->items[g->index[k]];
            if (it->active) {
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

void fn_801E22D0(Phob** list, int n) {
    for (int i = 0; i < n; i++) {
        Phob* p = list[i];
        for (int j = 0; j < p->count; j++) {
            p->Get(j)->Reset();
        }
    }
    f32 scale = 10.0f;
    for (int i = 0; i < n; i++) {
        Phob* p = list[i];
        if (p->count > 0) {
            int nc = 5;
            PhobCell* cells[5];
            fn_801B619C(cells, &nc, &p->pos);
            PhobNode* nd;
            Phob* o;
            for (int k = 0; k < nc; k++) {
                for (nd = cells[k]->head; nd; nd = nd->next) {
                    o = nd->phob;
                    if (p < o || !o->Solid()) {
                        fn_801E1F8C(p, o);
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

static inline void QuadRay(Group* g, const PVec* from, RayQuery* q, f32 scale) {
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
            GroupItem* it = g->items[g->index[k]];
            if (it->active) {
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

int fn_801E2640(const PVec* from, const PVec* to, PVec* outPos, PVec* outNormal, s32* outSurf, Phob** outBody,
                Phob** list, int n, RayFilter filter) {
    PVec d;
    d.x = to->x - from->x;
    d.y = to->y - from->y;
    d.z = to->z - from->z;
    f32 len = PSVECMag(&d);
    f32 inv = 1.0f / Max(0.01f, len);
    PVec dir;
    dir.x = d.x * inv;
    dir.y = d.y * inv;
    dir.z = d.z * inv;
    f32 t = PSVECDotProduct(&dir, &d);
    int hit = 0;
    *outBody = 0;
    if ((f32)__fabs(dir.y) > 0.9f) {
        int nc = 5;
        PhobCell* cells[5];
        fn_801B619C(cells, &nc, from);
        PhobNode* nd;
        Phob* p;
        for (int k = 0; k < nc; k++) {
            for (nd = cells[k]->head; nd; nd = nd->next) {
                p = nd->phob;
                if (p && p->Solid()) {
                    for (int j = 0; j < p->count; j++) {
                        Shape* s = p->Get(j);
                        s32 id = s->id;
                        if (!filter || filter(id)) {
                            if (s->Ray(from, &dir, &t, outNormal)) {
                                *outSurf = id;
                                hit = 1;
                                *outBody = p;
                            }
                        }
                    }
                }
            }
        }
        Group* g[2];
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
            Phob* p = list[i];
            if (p && p->Solid()) {
                for (int j = 0; j < p->count; j++) {
                    Shape* s = p->Get(j);
                    s32 id = s->id;
                    if (!filter || filter(id)) {
                        if (s->Ray(from, &dir, &t, outNormal)) {
                            *outSurf = id;
                            hit = 1;
                            *outBody = p;
                        }
                    }
                }
            }
        }
        Group* g[2];
        g[0] = lbl_8055EA3C;
        g[1] = lbl_8055EA40;
        for (u32 i = 0; i < 2; i++) {
            if (g[i]) {
                for (int k = 0; k < g[i]->count; k++) {
                    GroupItem* it = g[i]->items[k];
                    if (it->active) {
                        if (it->shape->Ray(from, &dir, &t, outNormal)) {
                            hit = 1;
                            *outSurf = it->shape->id;
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
