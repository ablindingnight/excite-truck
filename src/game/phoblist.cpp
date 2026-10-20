// PhobList (0x801B5F40-0x801B64B4): a 100x100 grid of physics-object lists over the track (20-unit
// cells), with a separate list for large objects, built from a fixed node pool. Names are guesses
// apart from the messages and allocation tags.
#include "game/common.h"

extern "C" void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

class Phob;

struct PhobNode {
    PhobNode(void) {
        phob = 0;
        next = 0;
    }
    Phob* phob;      // offset 0x0
    PhobNode* next;  // offset 0x4
};

struct PhobCell {
    PhobCell(void) { head = 0; }
    PhobNode* head;  // offset 0x0
};

struct PhobPos {
    f32 x, y, z;
};

static inline int CellIndex(f32 v) {
    int i;
    v += 1000.0f;
    i = v / 20.0f;
    if (i < 0) {
        i = 0;
    } else if (i >= 100) {
        i = 99;
    }
    return i;
}

s32 lbl_8055A8D0 = 512;  // node count

ModuleTag lbl_8055E6F8;
UnkStaticFFFF lbl_8055E700;
UnkStaticFFFF lbl_8055E704;
PhobNode* lbl_8055E708;  // node pool
PhobCell* lbl_8055E70C;  // grid
PhobNode* lbl_8055E710;  // free nodes
PhobCell* lbl_8055E714;  // large objects

extern "C" {

PhobNode* fn_801B5F40(void) {
    if (lbl_8055E710) {
        PhobNode* n = lbl_8055E710;
        lbl_8055E710 = n->next;
        n->next = 0;
        return n;
    }
    fn_802E317C("PhobList: out of nodes");
    return 0;
}

void fn_801B5F90(PhobNode* n) {
    n->next = lbl_8055E710;
    lbl_8055E710 = n;
}

void fn_801B5FA0(void) {
    int i;
    lbl_8055E708 = new ("phobnode_pool") PhobNode[lbl_8055A8D0];
    lbl_8055E70C = new ("phoblist") PhobCell[100 * 100];
    lbl_8055E710 = 0;
    lbl_8055E714 = new ("bigphoblist") PhobCell;
    for (i = 0; i < lbl_8055A8D0; i++) {
        PhobNode* n = &lbl_8055E708[i];
        n->next = lbl_8055E710;
        lbl_8055E710 = n;
    }
}

void fn_801B60A8(void) {
    delete lbl_8055E714;
    delete[] lbl_8055E708;
    delete[] lbl_8055E70C;
}

PhobCell* fn_801B60F4(const PhobPos* pos, f32 radius) {
    int x;
    int z;
    if (radius > 20.0f) {
        return lbl_8055E714;
    }
    x = CellIndex(pos->x);
    z = CellIndex(pos->z);
    return &lbl_8055E70C[z * 100 + x];
}

void fn_801B619C(PhobCell** out, int* n, const PhobPos* pos) {
    int x0 = CellIndex(pos->x - 9.5999994f);
    int x1 = CellIndex(9.5999994f + pos->x);
    int z0 = CellIndex(pos->z - 9.5999994f);
    int z1 = CellIndex(9.5999994f + pos->z);
    if (x0 != x1) {
        if (z0 != z1) {
            *n = 4;
            out[0] = &lbl_8055E70C[z0 * 100 + x0];
            out[1] = &lbl_8055E70C[z0 * 100 + x1];
            out[2] = &lbl_8055E70C[z1 * 100 + x0];
            out[3] = &lbl_8055E70C[z1 * 100 + x1];
        } else {
            *n = 2;
            out[0] = &lbl_8055E70C[z0 * 100 + x0];
            out[1] = &lbl_8055E70C[z0 * 100 + x1];
        }
    } else if (z0 != z1) {
        *n = 2;
        out[0] = &lbl_8055E70C[z0 * 100 + x0];
        out[1] = &lbl_8055E70C[z1 * 100 + x0];
    } else {
        *n = 1;
        out[0] = &lbl_8055E70C[z0 * 100 + x0];
    }
    out[(*n)++] = lbl_8055E714;
}

void fn_801B63E0(PhobCell* c, Phob* p) {
    PhobNode* n = fn_801B5F40();
    n->phob = p;
    n->next = c->head;
    c->head = n;
}

void fn_801B6428(PhobCell* c, Phob* p) {
    PhobNode* prev = c->head;
    PhobNode* n;
    if (prev->phob == p) {
        c->head = prev->next;
        fn_801B5F90(prev);
        return;
    }
    while ((n = prev->next) != 0) {
        if (n->phob == p) {
            prev->next = n->next;
            fn_801B5F90(n);
            return;
        }
        prev = n;
    }
    fn_802E317C("PhobList: removing unknown phob");
}

}  // extern "C"
