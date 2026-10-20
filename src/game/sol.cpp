// Static object lookup ("sol", 0x801B53E4-0x801B5F40): the track's solid objects are sorted into a
// quadtree (cells split until they are under 50 units, objects go into every cell their bounds touch),
// then flattened into one "SOL" block of u16 tables - per cell the first/last entry and the first child
// - that the collision code walks. Touching box ends are flagged first so joined boxes collide smoothly.
// Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
void* memset(void* p, int c, u32 n);
void fn_802E317C(const char* fmt, ...);               // fatal: print, then exit() -> PPCHalt (never returns)
void* fn_801284E8(const char* tag, s32 size);          // MemAlloc
void fn_80128658(void* p);                             // MemFree
f32 PSVECSquareDistance(const void* a, const void* b);
}

void* operator new[](size_t size, const char* tag);
void operator delete[](void* p);

struct V3 {
    f32 x;
    f32 y;
    f32 z;
};

struct SolMtx {
    u8 pad[0x18];
    V3 axis;  // offset 0x18
    V3 pos;   // offset 0x24
};

class SolObj {
public:
    virtual ~SolObj(void);
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
    virtual void GetBounds(V3* min, V3* max);

    u8 pad4[0x8];
    SolMtx* mtx;    // offset 0xC
    u8 pad10[0x8];
    u32 type;       // offset 0x18
    u8 pad1C[0x4];
    s32 kind;       // offset 0x20
    u8 pad24[0x8];
    f32 halfLen;    // offset 0x2C
    u8 pad30[0x4];
    u32 flags;      // offset 0x34
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
    virtual ~Phob(void);

    u8 pad4[0x38];
    SolObj* sol;  // offset 0x3C
    u8 pad40[0x4];
    Phob* next;   // offset 0x44
};

extern "C" void fn_801A49FC(Phob* p);

// An object in a quadtree cell (0xC).
struct SolEntry {
    Phob* phob;      // offset 0x0
    s32 index;       // offset 0x4
    SolEntry* next;  // offset 0x8
};

extern "C" void fn_801B591C(void);
extern "C" void fn_801B5984(void);
ModuleTag lbl_8055E6E0;
s32 lbl_8055E6E8;  // cells made
s32 lbl_8055E6EC;  // entries made

class QuadTree {
public:
    QuadTree(void) {
        count = 0;
        children = 0;
        head = 0;
        lbl_8055E6E8++;
    }
    ~QuadTree(void) {
        if (children) {
            delete[] children;
        }
    }

    SolEntry* head;      // offset 0x0
    s32 count;           // offset 0x4
    QuadTree* children;  // offset 0x8
};

struct SolRect {
    f32 x0;  // offset 0x0
    f32 z0;  // offset 0x4
    f32 x1;  // offset 0x8
    f32 z1;  // offset 0xC
};

// The flattened tree, as allocated in one block.
struct SolTable {
    s32 count;                  // offset 0x0
    Phob** phobs;               // offset 0x4
    s32 entryCount;             // offset 0x8
    u16* entries;               // offset 0xC
    u16* cells;                 // offset 0x10
    void (*free)(SolTable* t);  // offset 0x14
};

struct SolFlatten {
    s32 base;        // offset 0x0
    s32 cellCount;   // offset 0x4
    s32 cellsMade;   // offset 0x8
    u16* cells;      // offset 0xC
    s32 entryCount;  // offset 0x10
    s32 entryMax;    // offset 0x14
    u16* entries;    // offset 0x18
};

s32 lbl_8055E6F0;
SolEntry* lbl_8055E6F4;
SolRect lbl_80435EB8;  // bounds of the object being added
SolFlatten lbl_80435EC8;

extern "C" {

int fn_801B53E4(SolObj* o) {
    int r = 0;
    if (o->type == 'BOX ' && o->kind == 0x6E) {
        r = 1;
    }
    return r;
}

void fn_801B5410(SolObj* o, V3* a, V3* b) {
    SolMtx* m = o->mtx;
    f32 h = o->halfLen;
    a->x = m->pos.x + m->axis.x * h;
    a->y = m->pos.y + m->axis.y * h;
    a->z = m->pos.z + m->axis.z * h;
    h = -o->halfLen;
    b->x = m->pos.x + m->axis.x * h;
    b->y = m->pos.y + m->axis.y * h;
    b->z = m->pos.z + m->axis.z * h;
    b->y = 0.0f;
    a->y = 0.0f;
}

SolObj* fn_801B54A8(Phob* p) { return p->sol; }

void fn_801B54B0(Phob* list) {
    for (Phob* p = list; p; p = p->next) {
        if (fn_801B53E4(p->sol)) {
            SolObj* s = fn_801B54A8(p);
            V3 a0;
            V3 b0;
            fn_801B5410(p->sol, &a0, &b0);
            for (Phob* q = list; q; q = q->next) {
                if (fn_801B53E4(q->sol)) {
                    SolObj* t = fn_801B54A8(q);
                    V3 a1;
                    V3 b1;
                    fn_801B5410(q->sol, &a1, &b1);
                    if (PSVECSquareDistance(&a0, &a1) < 0.1f * 0.1f) {
                        s->flags |= 1;
                        t->flags |= 1;
                    }
                    if (PSVECSquareDistance(&a0, &b1) < 0.1f * 0.1f) {
                        s->flags |= 1;
                        t->flags |= 2;
                    }
                    if (PSVECSquareDistance(&b0, &a1) < 0.1f * 0.1f) {
                        s->flags |= 2;
                        t->flags |= 1;
                    }
                    if (PSVECSquareDistance(&b0, &b1) < 0.1f * 0.1f) {
                        s->flags |= 2;
                        t->flags |= 2;
                    }
                }
            }
        }
    }
}

void fn_801B561C(Phob* phob, s32 index, QuadTree* t, int depth, f32 x0, f32 z0, f32 x1, f32 z1) {
    f32 cx = 0.5f * (x0 + x1);
    f32 cz = 0.5f * (z0 + z1);
    SolRect rects[4] = {{x0, z0, cx, cz}, {cx, z0, x1, cz}, {x0, cz, cx, z1}, {cx, cz, x1, z1}};
    int n = 0;
    u8 hit[4] = {0, 0, 0, 0};
    int last = -1;
    for (int i = 0; i < 4; i++) {
        if (lbl_80435EB8.x0 < rects[i].x1 && lbl_80435EB8.x1 > rects[i].x0 && lbl_80435EB8.z0 < rects[i].z1 &&
            lbl_80435EB8.z1 > rects[i].z0) {
            hit[i] = 1;
            last = i;
            n++;
        }
    }
    if (n == 0) {
        fn_802E317C("sol:add_object(): Object not in any square");
    } else if (n == 1) {
        if (!t->children) {
            t->children = new ("QuadTree::children") QuadTree[4];
        }
        fn_801B561C(phob, index, &t->children[last], depth + 1, rects[last].x0, rects[last].z0, rects[last].x1,
                    rects[last].z1);
    } else {
        f32 dx = x1 - x0;
        f32 dz = z1 - z0;
        if ((dx > dz ? dx : dz) < 50.0f) {
            SolEntry* e = &lbl_8055E6F4[lbl_8055E6F0++];
            e->phob = phob;
            e->index = index;
            e->next = t->head;
            t->head = e;
            t->count++;
            lbl_8055E6EC++;
        } else {
            if (!t->children) {
                t->children = new ("QuadTree::children") QuadTree[4];
            }
            for (int i = 0; i < 4; i++) {
                if (hit[i]) {
                    fn_801B561C(phob, index, &t->children[i], depth + 1, rects[i].x0, rects[i].z0, rects[i].x1,
                                rects[i].z1);
                }
            }
        }
    }
}

void fn_801B59A4(QuadTree* t, int depth, int i) {
    if (i >= lbl_80435EC8.cellCount) {
        lbl_80435EC8.cellCount = i + 1;
    }
    u16* c = &lbl_80435EC8.cells[i * 4];
    c[0] = lbl_80435EC8.base;
    c[1] = lbl_80435EC8.base + (u16)t->count;
    c[3] = 0;
    if (t->children) {
        c[2] = lbl_80435EC8.cellCount;
    } else {
        c[2] = 0;
    }
    for (SolEntry* e = t->head; e; e = e->next) {
        lbl_80435EC8.entries[lbl_80435EC8.entryCount++] = e->index;
    }
    lbl_80435EC8.base += t->count;
    if (t->children) {
        int child = c[2];
        lbl_80435EC8.cellCount += 4;
        for (int j = 0; j < 4; j++) {
            fn_801B59A4(&t->children[j], depth + 1, child++);
        }
    }
}

void fn_801B5AE4(void) {
    lbl_8055E6E8 = 0;
    lbl_8055E6EC = 0;
    memset(&lbl_80435EC8, 0, sizeof(lbl_80435EC8));
}

void fn_801B5B04(QuadTree* t, Phob** list, int n) {
    for (int i = 0; i < n; i++) {
        V3 min;
        V3 max;
        list[i]->sol->GetBounds(&min, &max);
        lbl_80435EB8.x0 = min.x - 4.0f;
        lbl_80435EB8.z0 = min.z - 4.0f;
        lbl_80435EB8.x1 = max.x + 4.0f;
        lbl_80435EB8.z1 = max.z + 4.0f;
        fn_801B561C(list[i], i, t, 0, -4000000.0f, -4000000.0f, 4000000.0f, 4000000.0f);
    }
}

void fn_801B5BDC(Phob** out, int n, Phob* p) {
    for (; n > 0; n--) {
        *out++ = p;
        p = p->next;
    }
}

void fn_801B5C58(SolTable* t) {
    for (int i = 0; i < t->count; i++) {
        fn_801A49FC(t->phobs[i]);
        delete t->phobs[i]->sol;
        delete t->phobs[i];
    }
    fn_80128658(t);
}

int fn_801B5D10(Phob* p) {
    int n = 0;
    while (p) {
        p = p->next;
        n++;
    }
    return n;
}

SolTable* fn_801B5D30(Phob** head) {
    fn_801B5AE4();
    fn_801B54B0(*head);
    int n = fn_801B5D10(*head);
    Phob** list = new ("sobjtable") Phob*[n];
    fn_801B5BDC(list, n, *head);
    {
        QuadTree root;
        lbl_8055E6F0 = 0;
        lbl_8055E6F4 = (SolEntry*)new ("sobjlnode") u8[0x8000 * sizeof(SolEntry)];
        fn_801B5B04(&root, list, n);
        delete[] list;
        delete[] lbl_8055E6F4;
        lbl_8055E6F4 = 0;
    }
    SolTable* t = (SolTable*)fn_801284E8("SOL", n * sizeof(Phob*) + lbl_8055E6EC * sizeof(u16) +
                                                    lbl_8055E6E8 * 4 * sizeof(u16) + sizeof(SolTable));
    t->count = n;
    t->entryCount = lbl_8055E6EC;
    t->phobs = (Phob**)(t + 1);
    t->entries = (u16*)(t->phobs + n);
    t->cells = t->entries + t->entryCount;
    t->free = fn_801B5C58;
    fn_801B5BDC(t->phobs, n, *head);
    {
        QuadTree root;
        lbl_8055E6F0 = 0;
        lbl_8055E6F4 = (SolEntry*)new ("sobjlnode") u8[0x8000 * sizeof(SolEntry)];
        fn_801B5B04(&root, t->phobs, t->count);
        memset(&lbl_80435EC8, 0, sizeof(lbl_80435EC8));
        lbl_80435EC8.base = 0;
        lbl_80435EC8.cellsMade = lbl_8055E6E8;
        lbl_80435EC8.cells = t->cells;
        lbl_80435EC8.entryMax = t->entryCount;
        lbl_80435EC8.entries = t->entries;
        fn_801B59A4(&root, 0, 0);
        delete[] lbl_8055E6F4;
        lbl_8055E6F4 = 0;
        *head = 0;
    }
    return t;
}

}  // extern "C"
