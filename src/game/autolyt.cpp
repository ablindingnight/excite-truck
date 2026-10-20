// Layout files (0x8022E0EC-0x8022E9E0): loads .lyt layouts (byte-swapping them when needed) and
// builds a plain fallback layout from a menu description. Names are guesses apart from the
// messages.
#include "game/common.h"

extern "C" {
int sprintf(char* buf, const char* fmt, ...);
void* memset(void* dst, int c, size_t n);
char* strcpy(char* dst, const char* src);
int strcmp(const char* a, const char* b);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
const char* fn_80131234(const char* key);  // localize
void* fn_8012C370(const char* name, u32 tag, u32* version, u32* size, int flags, u8* swapped);
void fn_8012C4D8(void* res);
}

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

// One element of a layout.
struct LytElem {
    int type;        // offset 0x0
    int unk4;        // offset 0x4
    int x;           // offset 0x8
    int y;           // offset 0xC
    int unk10;       // offset 0x10
    int w;           // offset 0x14
    int h;           // offset 0x18
    f32 size;        // offset 0x1C
    char res[0x20];  // offset 0x20
    char text[0x40]; // offset 0x40
    int unk80;       // offset 0x80
    int unk84;       // offset 0x84
    int unk88;       // offset 0x88
    int unk8C;       // offset 0x8C
    int unk90;       // offset 0x90
    int unk94;       // offset 0x94
    int unk98;       // offset 0x98
    u32 color;       // offset 0x9C
    int unkA0;       // offset 0xA0
    int unkA4;       // offset 0xA4
    int unkA8;       // offset 0xA8
    int unkAC;       // offset 0xAC
    int unkB0;       // offset 0xB0
    u8 padB4[0x80];
};

struct LytFile {
    u32 unk0;
    u8 pad4[0x60];
    LytElem* elems;  // offset 0x64
};

struct MenuItem {
    const char* name;
    int type;
    const void* data;
    int arg0;
    int arg1;
};

struct MenuDesc {
    const char* layout;
    MenuItem* items;
};

static inline LytElem* NextElem(LytFile* f, int& n) { return &f->elems[n++]; }

static inline void Swap32(void* p) {
    u8* b = (u8*)p;
    u8 t = b[0];
    b[0] = b[3];
    b[3] = t;
    t = b[1];
    b[1] = b[2];
    b[2] = t;
}

ModuleTag lbl_8055EFE0;
static UnkStatic123 lbl_80497E10;

extern "C" {

LytFile* fn_8022E0EC(const char* name) {
    u32 version;
    u32 size;
    u8 swapped;
    LytFile* f = (LytFile*)fn_8012C370(name, 'LAYT', &version, &size, 0, &swapped);
    if (f) {
        if (version != 5) {
            fn_802E317C("Wrong layout version in %s: %x != %x", name, version, 5);
        }
        if (swapped) {
            LytElem* e = f->elems = (LytElem*)(f + 1);
            Swap32(&f->unk0);
            for (;;) {
                Swap32(&e->type);
                Swap32(&e->unk4);
                Swap32(&e->x);
                Swap32(&e->y);
                Swap32(&e->unk10);
                Swap32(&e->w);
                Swap32(&e->h);
                Swap32(&e->size);
                Swap32(&e->unk80);
                Swap32(&e->unk84);
                Swap32(&e->unk88);
                Swap32(&e->unk8C);
                Swap32(&e->unk90);
                Swap32(&e->unk94);
                Swap32(&e->unk98);
                Swap32(&e->color);
                Swap32(&e->unkA0);
                Swap32(&e->unkA4);
                Swap32(&e->unkA8);
                Swap32(&e->unkAC);
                Swap32(&e->unkB0);
                if (e->type == 0) {
                    break;
                }
                if (e->text[0] == '|') {
                    const char* s = fn_80131234(e->text + 1);
                    int n = sizeof(e->text);
                    char* d = e->text;
                    while ((*d++ = *s++) != '\0') {
                        if (--n <= 0) {
                            d[-1] = '\0';
                            break;
                        }
                    }
                }
                e++;
            }
        }
    } else {
        fn_802E317C("Can't get layout %s", name);
    }
    return f;
}

void fn_8022E4BC(LytFile* f) { fn_8012C4D8(f); }

// Builds a plain column of labels and widgets for a menu that has no layout file.
LytFile* fn_8022E4C0(MenuDesc* desc) {
    // From layout functions the linker stripped.
    static const char* unused[] = {"Can't write %s", "Corrupt layout %s"};
    LytFile* f = new ("autolyt") LytFile;
    memset(f, 0, sizeof(LytFile));
    f->elems = new ("autoitem") LytElem[0x100];
    memset(f->elems, 0, sizeof(LytElem) * 0x100);
    int n = 0;
    LytElem* e = NextElem(f, n);
    e->type = 0x21;
    e->unk4 = 6;
    e->x = 0;
    e->y = 0;
    e->w = 640;
    e->h = 480;
    strcpy(e->text, "savebkg1.img");
    const char* title = desc->layout;
    e = NextElem(f, n);
    e->type = 0x1C;
    strcpy(e->text, title);
    e->x = 320;
    e->y = 30;
    e->w = 32;
    e->h = 32;
    e->unk4 = 12;
    e->size = 40.0f;
    strcpy(e->res, "uifnt.gtx");
    e->unk90 = -1;
    e->color = 0xFF000000;
    e->unkB0 = 26;
    e->unk84 = 2;
    int y = 90;
    for (MenuItem* it = desc->items; it->type != 0x7B; it++) {
        int dy = 30;
        int label = 1;
        int t = it->type;
        if (t == 6) {
            continue;
        }
        if (t == 9) {
            e = NextElem(f, n);
            e->type = 0x32;
            e->x = 270;
            e->y = y;
            e->w = 138;
            e->h = 14;
            strcpy(e->res, it->name);
        } else if (t == 3) {
            e = NextElem(f, n);
            e->type = 0x1C;
            sprintf(e->text, "#%s", it->name);
            e->x = 300;
            e->y = y;
            e->w = 32;
            e->h = 32;
            e->unk4 = 12;
            e->size = 20.0f;
            strcpy(e->res, "uifnt.gtx");
            e->unk90 = -1;
            e->color = 0xFF000000;
            e->unkB0 = 26;
            e->unk84 = 2;
        } else if (t == 16 || t == 7) {
            e = NextElem(f, n);
            e->type = (t == 16) ? 0x31 : 10;
            e->x = 300;
            e->y = y;
            e->w = 114;
            e->h = 35;
            strcpy(e->res, it->name);
            strcpy(e->text, "Multi");
        } else if (t == 0) {
            label = 0;
            e = NextElem(f, n);
            e->type = 2;
            e->unk4 = 25;
            e->x = 200;
            e->y = y;
            e->w = 114;
            e->h = 35;
            strcpy(e->res, it->name);
            strcpy(e->text, it->name);
        } else if (t == 2) {
            if (it != desc->items && it[-1].type == 9 && strcmp(it->name, it[-1].name) == 0) {
                continue;
            }
            e = NextElem(f, n);
            e->type = 7;
            e->x = 200;
            e->y = y;
            e->w = 114;
            e->h = 35;
            strcpy(e->res, it->name);
            strcpy(e->text, "%d");
        } else if (t == 12) {
            e = NextElem(f, n);
            e->type = 9;
            e->x = 200;
            e->y = y;
            e->w = 114;
            e->h = 35;
            strcpy(e->res, it->name);
            strcpy(e->text, "text");
        } else if (t == 10) {
            e = NextElem(f, n);
            e->type = 0x17;
            e->x = 200;
            e->y = y;
            e->w = 200;
            e->h = 80;
            strcpy(e->res, it->name);
            dy = e->h + 5;
        } else if (t == 5) {
            if (it->name) {
                e = NextElem(f, n);
                e->type = 26;
                e->x = 200;
                e->y = y;
                e->w = 200;
                e->h = 50;
                strcpy(e->res, it->name);
                dy = 50;
            }
        }
        if (label && it->name) {
            e = NextElem(f, n);
            e->type = 0x1C;
            strcpy(e->text, it->name);
            e->x = 100;
            e->y = y;
            e->w = 32;
            e->h = 32;
            e->unk4 = 12;
            e->size = 20.0f;
            strcpy(e->res, "uifnt.gtx");
            e->unk90 = -1;
            e->color = 0xFF000000;
            e->unkB0 = 26;
            e->unk84 = 2;
        }
        y += dy;
    }
    return f;
}

void fn_8022E960(LytFile* f) {
    delete[] f->elems;
    delete f;
}

}  // extern "C"

// NonMatching: fn_8022E4C0 schedules the desc->items load later than the target (8 diffs).
