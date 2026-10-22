// Model test screen (0x8024BDDC-0x8024C070): the debug model viewer ("ModelTest!"), preceded by the
// multiplayer event menu's adjustor thunks. The viewer itself is still a stub. Names are guesses
// apart from the strings.
#include "game/common.h"

extern "C" {
void fn_8024BB98(u8* p);
void fn_8024A110(u8* p);
void fn_8024BBF4(u8* p);
void fn_8024BCC0(u8* p);
void fn_8024A1AC(u8* p);
}

ModuleTag lbl_8055F2A0;

extern "C" {

void fn_8024BDDC(u8* p) { fn_8024BB98(p - 0x34); }

void fn_8024BDE4(u8* p) { fn_8024A110(p - 0x34); }

void fn_8024BDEC(u8* p) { fn_8024BBF4(p - 0x18); }

void fn_8024BDF4(u8* p) { fn_8024BCC0(p - 0x18); }

void fn_8024BDFC(u8* p) { fn_8024A1AC(p - 0x18); }

void fn_8024BE04(u8* p) {}  // stub (153)

}  // extern "C"
