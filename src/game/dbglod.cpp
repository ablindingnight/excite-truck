// Debug LOD page (0x801E45CC-0x801E4A2C): a debug-menu page showing the LOD factor, the selected car
// and wheel, the wireframe and cache flags, with key handlers for them. Names are guesses.
#include "game/common.h"

extern "C" {
int sprintf(char* buf, const char* fmt, ...);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_8023895C(void);
void fn_802389E4(void);
void fn_80238A74(int x, int y, const char* s, u32 color);
void fn_80238B44(int x, int y, int w, int h, u32 color);
int fn_802CC744(void);
int fn_802CC74C(void);
int fn_801769F8(void);
void fn_801EC1F8(void);
void fn_801EC204(void);
void fn_802E4CDC(void);
}

class UnkStaticS16 {
public:
    UnkStaticS16(s16 v) { val = v; }
    s16 val;
};

inline s16 MakeS16(s16 v) { return v; }

struct DbgLog {
    char text[0x80];
    int count;  // offset 0x80
};

static u8 lbl_80339930[] = {
    0x1A, 0x1B, 0x28, 0x26, 0x21, 0x11, 0x14, 0xC8, 0xD0, 0x1F, 0x20, 0x12, 0xD2,
    0xD3, 0xC7, 0xCF, 0xC9, 0xD1, 0x15, 0x23, 0x16, 0x24, 0x17, 0x25, 0x18, 0x4F,
    0x50, 0x51, 0x4B, 0x4C, 0x4D, 0x47, 0x48, 0x49, 0x0F, 0x39, 0x2E, 0x31, 0xFF,
};

int lbl_8055AEC0 = 0x41;
f32 lbl_8055AEC4 = 1.0f;  // LOD factor
int lbl_8055AEC8 = -1;    // selected car
int lbl_8055AECC = -1;    // selected wheel
const char* lbl_8055AED4 = "";
const char* lbl_8055AED8 = "";
bool lbl_8055AEDC = true;

ModuleTag lbl_8055EA70;
UnkStaticFFFF lbl_8055EA78;
s16 lbl_8055EA7C = MakeS16(0x100);
s16 lbl_8055EA7E = MakeS16(0x405);
s16 lbl_8055EA80 = MakeS16(0x401);
UnkStatic7F lbl_8055EA84;
const char* lbl_8055EA88;
bool lbl_8055EA8C;
bool lbl_8055EA8D;
bool lbl_8055EA8E;
bool lbl_8055EA8F;  // show log
bool lbl_8055EA90;  // wireframe
DbgLog lbl_804874F0;

extern "C" {

void fn_801E45CC(void) {
    char buf[80];
    static const char* unused[] = {"test %d of %d from lon %8.1f / %.1f"};  // from a stripped DMA test
    fn_8023895C();
    fn_80238B44(10, 10, 230, 84, 0x80000000);
    sprintf(buf, "LOD Factor:  %4.1f", lbl_8055AEC4);
    fn_80238A74(15, 15, buf, -1);
    fn_80238A74(15, 27, "[=down ]=up '=1.0", -1);
    sprintf(buf, "Car:%2d:%s", lbl_8055AEC8, lbl_8055AED4);
    fn_80238A74(10, 39, buf, -1);
    sprintf(buf, "Wheel:%2d:%s", lbl_8055AECC, lbl_8055AED8);
    fn_80238A74(10, 51, buf, -1);
    fn_80238A74(10, 63, "WIRE", lbl_8055EA90 ? 0xFF2DC82D : 0xFF202020);
    if (!fn_802CC744()) {
        fn_80238A74(50, 63, "NO CACHE", 0xFF2D2DC8);
    } else if (!fn_802CC74C()) {
        fn_80238A74(50, 63, "CACHING", 0xFF2DC82D);
    }
    if (lbl_8055EA88) {
        fn_80238A74(150, 63, lbl_8055EA88, 0xFF2DC82D);
    }
    if (lbl_8055EA8F) {
        int y = 100;
        fn_80238B44(20, 100, 320, 112, 0x60000000);
        for (int i = 0; i < lbl_804874F0.count; i++) {
            fn_80238A74(20, y, lbl_804874F0.text, -1);
            y -= 12;
        }
    }
    fn_802389E4();
}

int fn_801E47BC(int key) {
    int handled = 1;
    switch (key) {
    case 't':
        if (fn_801769F8()) {
            if (lbl_8055AEC0 == 0x41) {
                lbl_8055AEC0 = 0;
            } else {
                lbl_8055AEC0 = 0x41;
            }
        } else {
            fn_802E30C8("physics must be paused to perform dma test.");
        }
        break;
    case 'f':
        if (!fn_801769F8()) {
            fn_802E30C8("physics must be paused to perform dma test.");
        }
        break;
    case '[':
        lbl_8055AEC4 -= 0.2f;
        break;
    case '\'':
        lbl_8055AEC4 = 1.0f;
        break;
    case ']':
        lbl_8055AEC4 += 0.2f;
        break;
    case 'n':
        lbl_8055EA8C = !lbl_8055EA8C;
        break;
    case 'c':
        lbl_8055AEDC = !lbl_8055AEDC;
        break;
    case 'm':
        lbl_8055EA8E = true;
        break;
    case 'l':
        lbl_8055EA8F = !lbl_8055EA8F;
        break;
    case 'w':
        lbl_8055EA90 = !lbl_8055EA90;
        break;
    case 'E':
        lbl_8055EA8D = !lbl_8055EA8D;
        break;
    case 11:
        fn_801EC1F8();
        fn_802E4CDC();
        break;
    case 4:
        fn_801EC204();
        fn_802E4CDC();
        break;
    case 9:
        break;
    default:
        handled = 0;
        break;
    }
    if (lbl_8055AEC4 > 20.0f) {
        lbl_8055AEC4 = 20.0f;
    }
    if (lbl_8055AEC4 < 0.2f) {
        lbl_8055AEC4 = 0.2f;
    }
    return handled;
}

}  // extern "C"

// NonMatching: the sinit is one of the epilogue-scheduled kind.
