// Car pick screen (0x8025E034-0x8025FE8C): the car icon grid ("caricona.art", "caricons0.tex", ...)
// with the chassis/uv fields. Written against raw field offsets; the large screen routines are still
// stubs. Names are guesses apart from the strings.
#include "game/common.h"

#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

extern "C" {
void fn_80235380(u8* p);
void fn_80214A9C(s32 s);
u8* fn_8023512C(const char* name);
}

void operator delete(void* p);

extern void* lbl_80345A30[];

ModuleTag lbl_8055F3C8;
UnkStatic123s lbl_80498EE8;
UnkStatic7F lbl_8055F3D0;

extern "C" {

void fn_8025E034(u8* p) {}  // stub (376)

u8* fn_8025E614(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80345A30;
        fn_80235380(PTR(p, 0x2C));
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_8025E678(u8* p) {}  // stub (401)

void fn_8025ECBC(u8* p) {}  // stub (366)

}  // extern "C"

// A car choice in one of the pick lists.
class CarChoice {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual void v20(void);
    virtual int Unlocked(void);  // 0x24
    virtual void v28(void);
    virtual int Hidden(void);    // 0x2C
};

class CarChoiceList {
public:
    virtual void v08(void);
    virtual int Count(void);            // 0x0C
    virtual CarChoice* Get(int i);      // 0x10
};

struct CarPickScreen {
    CarChoiceList* all;       // offset 0x0
    CarChoiceList* lists[3];  // offset 0x4
    u8 pad10[0x144 - 0x10];
    s32 sel;                  // offset 0x144, then every 0x58 bytes per column
};

extern CarPickScreen* lbl_8055F3D4;

static inline int AnyShown(int i) {
    for (int j = 0; j < 3; j++) {
        if (i < lbl_8055F3D4->lists[j]->Count()) {
            CarChoice* c = lbl_8055F3D4->lists[j]->Get(i);
            if (c->Unlocked() || !c->Hidden()) {
                return 1;
            }
        }
    }
    return 0;
}

static inline int AnyUnlocked(int i) {
    for (int j = 0; j < 3; j++) {
        if (i < lbl_8055F3D4->lists[j]->Count()) {
            if (lbl_8055F3D4->lists[j]->Get(i)->Unlocked()) {
                return 1;
            }
        }
    }
    return 0;
}

extern "C" {
void fn_80214AF0(s32 s);

// Greys out the grid buttons of cars nobody can pick and highlights each column's selection.
void fn_8025F274(u8* p) {
    for (int i = 0; i < lbl_8055F3D4->all->Count(); i++) {
        if (!AnyShown(i)) {
            fn_80214A9C(i + 0x10);
        }
        if (!AnyUnlocked(i)) {
            fn_80214A9C(i + 0x1A);
        }
    }
    CarPickScreen* s = lbl_8055F3D4;
    for (int k = 0; k < 5; k++) {
        for (int j = 0; j < 3; j++) {
            if (j == S32(s, 0x144 + k * 0x58)) {
                fn_80214AF0(k + 1 + j * 10);
            } else {
                fn_80214A9C(k + 1 + j * 10);
            }
        }
    }
}

void fn_8025F46C(u8* p) {}  // stub (339)

void fn_8025F9B8(u8* p, u32 v) { U32(p, 0x18) = v; }

u8* fn_8025F9C0(u8* p) {
    U32(p, 0x14) = 0;
    *(void**)p = lbl_80345A30;
    U32(p, 0x18) = 0;
    U8(p, 0x25) = 0;
    PTR(p, 0x2C) = fn_8023512C("caricona.art");
    return p;
}

}  // extern "C"

// One car icon: its chassis name ("_e" suffix for the second, "e" texture set) and texture rectangle.
struct CarIcon {
    char name[32];  // offset 0x0
    u8* tex;        // offset 0x20
    u8* overlay;    // offset 0x24
    f32 u1;         // offset 0x28
    f32 v1;         // offset 0x2C
    f32 u2;         // offset 0x30
    f32 v2;         // offset 0x34
};

static inline void CopyName(char* d, int n, const char* s) {
    char* p = d;
    while ((*p++ = *s++) != 0) {
        if (--n <= 0) {
            p[-1] = 0;
            break;
        }
    }
}

static inline void AppendName(const char* s, int n, char* d) {
    while (*d) {
        n--;
        d++;
    }
    if (n > 0) {
        CopyName(d, n, s);
    }
}

void* operator new(size_t n, const char* tag);
void* operator new[](size_t n, const char* tag);
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* t);
extern "C" u8* fn_802CAC28(const char* name, int flags);
extern "C" int strcmp(const char* a, const char* b);

// The car icon table (caricons.val): two icons per Car entry, plain and "_e".
struct CarIconSet {
    CarIconSet(void) {
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
                CopyName(c->name, 32, n->GetValue("chassis"));
                if (k) {
                    AppendName("_e", 32, c->name);
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
    CarIcon* icons;  // offset 0x0
    s32 count;       // offset 0x4
};

extern CarIconSet* lbl_8055F2E4;
extern s32 lbl_8055F2E8;

extern "C" {

// Loads the car icon table on first use; reference counted.
CarIconSet* fn_8025FA18(void) {
    if (lbl_8055F2E8 == 0) {
        lbl_8055F2E4 = new ("cimgr") CarIconSet;
    }
    lbl_8055F2E8++;
    return lbl_8055F2E4;
}

}  // extern "C"

// One car icon's texture rectangles.
struct IconEntry {
    u8 pad0[0x20];  // offset 0x0
    u8* tex;        // offset 0x20
    u8* overlay;    // offset 0x24
    f32 u0;         // offset 0x28
    f32 v0;         // offset 0x2C
    f32 u1;         // offset 0x30
    f32 v1;         // offset 0x34
};

// Scales each colour channel by 255*255/65536, alpha forced opaque.
static inline u32 DimColor(u32 c) {
    int b = (int)(c & 0xFF) * 255 * 255 / 256 / 256;
    int g = (int)((c >> 8) & 0xFF) * 255 * 255 / 256 / 256;
    int r = (int)((c >> 16) & 0xFF) * 255 * 255 / 256 / 256;
    return 0xFF000000 | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}

extern "C" {
void fn_80230C30(f32* x, f32* y, s32 lx, s32 ly);
void fn_802354A0(u8* art, int part, f32 x, f32 y, f32 z, f32 w, f32 h, u32 color);
void fn_80231220(u8* tex, f32 x, f32 y, f32 w, f32 h, f32 u0, f32 v0, f32 u1, f32 v1, u32 color);

// The chosen car icon in a frame, the overlay tinted with the player's colour.
void fn_8025FC8C(u8* p) {
    if (!U8(p, 0x25)) {
        return;
    }
    u32 c;
    IconEntry* e;
    f32 sx;
    f32 sy;
    f32 x;
    f32 y;
    f32 v1;
    f32 u1;
    f32 v0;
    f32 u0;
    f32 fw;
    f32 fh;
    fn_80230C30(&sx, &sy, S32(p, 0x4), S32(p, 0x8));
    fw = (f32)S32(p, 0xC);
    fh = (f32)S32(p, 0x10);
    fn_802354A0(PTR(p, 0x2C), U8(p, 0x24), sx - 3.0f, 3.0f + sy, 0.0f, 6.0f + fw, 6.0f + fh, -1);
    e = &((IconEntry*)*(u8**)PTR(p, 0x18))[S32(p, 0x1C)];
    y = sy;
    x = sx;
    u0 = e->u0;
    v0 = e->v0;
    u1 = e->u1;
    v1 = e->v1;
    c = U32(p, 0x28);
    fn_80231220(e->tex, x, y, fw, fh, u0, v0, u1, v1, -1);
    fn_80231220(e->overlay, x, y, fw, fh, u0, v0, u1, v1, DimColor(c));
}


}  // extern "C"
