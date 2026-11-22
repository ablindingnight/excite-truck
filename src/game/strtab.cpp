// String tables (0x8012F150-0x8012F31C): 'STAB' resources holding fixed-stride rows of strings with one
// column per language, byte-swapped on first load. Names are guesses.
#include "game/common.h"
#include "game/byteswap.h"

struct StringTable {
    u32 count;        // offset 0x0
    u32 unk4;
    u32 column[16];   // offset 0x8 (byte offset of each language within a row)
    u32 stride;       // offset 0x48
    char data[4];     // offset 0x4C
};

extern "C" {
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void* fn_8012C370(const char* name, u32 type, u32* info, s32* size, bool* first, bool* got);
int fn_8012C4D8(void* p);
}

ModuleTag lbl_8055E018;

extern "C" {

StringTable* LoadStringTable(const char* name) {
    bool swap;
    u32 version;
    StringTable* t = (StringTable*)fn_8012C370(name, 'STAB', &version, 0, 0, &swap);
    if (!t) {
        fn_802E317C("Can't find string table %s", name);
    }
    if (version != 0) {
        fn_802E317C("Wrong string table version %x (expecting %x) in %s", version, 0, name);
    }
    if (swap) {
        int i;
        SwapWordB((u8*)&t->count);
        SwapWordB((u8*)&t->unk4);
        for (i = 0; i < 16; i++) {
            SwapWordB((u8*)&t->column[i]);
        }
        SwapWordB((u8*)&t->stride);
    }
    return t;
}

int fn_8012F2E4(StringTable* t) { return fn_8012C4D8(t); }

u32 fn_8012F2E8(StringTable* t) { return t->count; }

const char* fn_8012F2F0(StringTable* t, int row, int lang) { return t->data + t->stride * row + t->column[lang]; }

}  // extern "C"
