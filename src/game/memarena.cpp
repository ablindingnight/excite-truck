// Memory arenas (0x802E3DF8-0x802E3EB0): the usable MEM1/MEM2 range, start aligned to 128 bytes.
// Names are guesses apart from the message.
#include "game/common.h"
#include <revolution/OS/OSArena.h>

extern "C" {
void fn_802E317C(const char* fmt, ...);  // panic
}

ModuleTag lbl_8055FF28;

extern "C" void fn_802E3DF8(int id, u32* start, u32* size) {
    u32 hi = 0;
    u32 lo = 0;
    if (id == 0) {
        lo = (u32)OSGetMEM1ArenaLo();
        hi = (u32)OSGetMEM1ArenaHi();
    } else if (id == 1) {
        lo = (u32)OSGetMEM2ArenaLo();
        hi = (u32)OSGetMEM2ArenaHi();
    } else {
        fn_802E317C("mem: bad memory id %d", id);
    }
    lo = (lo + 0x7F) & ~0x7F;
    *start = lo;
    *size = hi - lo;
}
