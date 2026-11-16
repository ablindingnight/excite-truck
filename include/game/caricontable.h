#ifndef GAME_CARICONTABLE_H
#define GAME_CARICONTABLE_H
// The car icon table (caricons.val): two icons per "Car" entry, plain and "_e", each a texture, an overlay texture
// and a UV rectangle. Shared, reference-counted, by the car menus (garage, carpick, frname, recscreen). One declaration
// for every unit (port ODR work, 2026-11-16). Class and helper names unproven; field names follow the .val keys.
#include "game/common.h"

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

extern "C" {
ValueTable* fn_80131808(const char* name);  // load a .val table
void fn_80131818(ValueTable* t);            // release it
u8* fn_802CAC28(const char* name, int flags);  // load a texture
int strcmp(const char* a, const char* b);
void gxForgetTexture(u8* tex);
}

// A fixed-size name with a bounded append.
template <int N> struct IconName {
    char text[N];
    operator char*() { return text; }
    operator const char*() const { return text; }
    static void CopyTail(const char* s, int n, char* d) {
        while ((*d++ = *s++) != 0) {
            if (--n <= 0) { d[-1] = 0; break; }
        }
    }
    void Append(const char* s) {
        char* d = text;
        int n = N;
        while (*d) { n--; d++; }
        if (n > 0) { CopyTail(s, n, d); }
    }
};

static inline void CopyIconName(char* d, int n, const char* s) {
    char* p = d;
    while ((*p++ = *s++) != 0) {
        if (--n <= 0) {
            p[-1] = 0;
            break;
        }
    }
}

// One icon (0x38).
struct CarIcon {
    IconName<32> name;  // offset 0x0 (the "chassis" key, "_e" appended for the second icon)
    u8* tex;            // offset 0x20
    u8* overlay;        // offset 0x24
    f32 u1;             // offset 0x28
    f32 v1;             // offset 0x2C
    f32 u2;             // offset 0x30
    f32 v2;             // offset 0x34
};

class CarIconTable;
extern CarIconTable* lbl_8055F2E4;  // the shared table
extern s32 lbl_8055F2E8;            // its reference count

struct CarIconTable {
    CarIconTable(void) {
        ValueTable* t = fn_80131808("caricons.val");
        count = 0;
        icons = new ("cicon") CarIcon[50];
        ValueNode* n;
        for (int i = 0; (n = t->GetChild(i)) != 0; i++) {
            if (strcmp(n->GetName(), "Car")) {
                continue;
            }
            for (int k = 0; k < 2; k++) {
                CarIcon* c = &icons[count++];
                CopyIconName(c->name, 32, n->GetValue("chassis"));
                if (k) {
                    c->name.Append("_e");
                }
                c->tex = fn_802CAC28(k ? "caricons0e.tex" : "caricons0.tex", 0);
                c->overlay = fn_802CAC28(k ? "caricons1e.tex" : "caricons1.tex", 0);
                c->u1 = n->GetFloat("u1");
                c->v1 = n->GetFloat("v1");
                c->u2 = n->GetFloat("u2");
                c->v2 = n->GetFloat("v2");
            }
        }
        fn_80131818(t);
    }
    ~CarIconTable(void) {
        for (int i = 0; i < count; i++) {
            CarIcon* c = &icons[i];
            gxForgetTexture(c->tex);
            gxForgetTexture(c->overlay);
        }
        delete[] icons;
    }
    // Reference-counted share (released by the menus when the count drops to zero). A static member defined in the
    // class body AFTER the ctor and dtor: "cimgr" is then its own weak string after the constructor's, as in retail
    // garage's .sdata (Bink session, 2026-11-16).
    static CarIconTable* Acquire(void) {
        if (lbl_8055F2E8 == 0) {
            lbl_8055F2E4 = new ("cimgr") CarIconTable;
        }
        lbl_8055F2E8++;
        return lbl_8055F2E4;
    }

    CarIcon* icons;  // offset 0x0
    s32 count;       // offset 0x4
};

#endif
