// Input bindings (0x801A75B4-0x801A9660): named controls ("Joystick Up", "Key_SPACE", ...) for the
// pads, keyboard and mouse, their current and previous-frame values, edge detection, reading and
// writing them as option strings and finding the control the player is pressing. Names are guesses
// apart from the strings.
#include "game/common.h"

extern "C" {
char* strncpy(char* d, const char* s, u32 n);
int strcasecmp(const char* a, const char* b);
void* memset(void* p, int c, u32 n);
}

class LocString {
public:
    LocString(const char* key);
    u8 pad[0xC];
};

struct JoyInfo {
    u32 hold;  // offset 0x0
    u32 bits;  // offset 0x4
    s32 a;     // offset 0x8
    s32 b;     // offset 0xC
};

struct JoyStatus {
    f32 ax0, ax1, ax2, ax3, ax4, ax5, ax6, ax7;  // offset 0x0: separate members (struct copies use lfs/stfs; an array copies as words)
    u8 buttons[32];  // offset 0x20
    u32 dir;         // offset 0x40
    JoyInfo info;    // offset 0x44
    s32 f54;         // offset 0x54
    s32 f58;         // offset 0x58
    u32 f5C;         // offset 0x5C
    u8 connected;    // offset 0x60
};

struct Mouse {
    f32 x;  // offset 0x0
    f32 y;  // offset 0x4
    u8 b0;  // offset 0x8
    u8 b1;  // offset 0x9
    u8 b2;  // offset 0xA
};

// One control: kind (1 key, 2 stick axis, 3 button, 4 direction bit, 5 mouse axis, 6 mouse button), the
// pad it reads and the control index.
struct Binding {
    s32 type;  // offset 0x0
    s32 pad;   // offset 0x4
    union {
        s32 code;
        u8 key;
    };           // offset 0x8
    u8 latched;  // offset 0xC
};

struct BindEntry {
    const char* name;   // offset 0x0
    const char* label;  // offset 0x4
    s32 type;           // offset 0x8
    s32 code;           // offset 0xC
    u8 key;             // offset 0x10
    u8 pad11[0x14 - 0x11];
    u8 used;  // offset 0x14
    u8 pad15[0x18 - 0x15];
};

struct NamePair {
    const char* name;
    const char* text;
};

extern "C" {
u8 fn_802E47F8(int key);
int fn_802E4A1C(int key);
void fn_80127A84(s32* x, s32* y, s32* buttons);
int fn_80129AE8(JoyStatus* out, int chan);
void fn_801334F4(int section, const char* key, char* out, int len);
void fn_8013358C(int section, const char* key, const char* s);
void fn_802E317C(const char* fmt, ...);
extern s32 lbl_8055CFF0;  // screen width
extern s32 lbl_8055CFF4;  // screen height
}

// This unit's data, in the target's order (names from the symbol map).
extern "C" {
char lbl_80333D18[] = "Joytick Up";
char lbl_80333D24[] = "Joy Down";
char lbl_80333D30[] = "Joystick Down";
char lbl_80333D40[] = "Joy Left";
char lbl_80333D4C[] = "Joystick Left";
char lbl_80333D5C[] = "Joy Right";
char lbl_80333D68[] = "Joystick Right";
char lbl_80333D78[] = "C-Stick Up";
char lbl_80333D84[] = "C-Stick Down";
char lbl_80333D94[] = "C-Stick Left";
char lbl_80333DA4[] = "JB Right";
char lbl_80333DB0[] = "C-Stick Right";
char lbl_80333DC0[] = "Left Trigger";
char lbl_80333DD0[] = "Right Trigger";
char lbl_80333DE0[] = "Button 1";
char lbl_80333DEC[] = "A Button";
char lbl_80333DF8[] = "Button 2";
char lbl_80333E04[] = "B Button";
char lbl_80333E10[] = "Button 3";
char lbl_80333E1C[] = "X Button";
char lbl_80333E28[] = "Button 4";
char lbl_80333E34[] = "Y Button";
char lbl_80333E40[] = "Button 5";
char lbl_80333E4C[] = "Z Button";
char lbl_80333E58[] = "Button 6";
char lbl_80333E64[] = "Left Trigger Click";
char lbl_80333E78[] = "Button 7";
char lbl_80333E84[] = "Right Trigger Click";
char lbl_80333E98[] = "POV Front";
char lbl_80333EA4[] = "D-Pad Up";
char lbl_80333EB0[] = "POV Back";
char lbl_80333EBC[] = "D-Pad Down";
char lbl_80333EC8[] = "POV Left";
char lbl_80333ED4[] = "D-Pad Left";
char lbl_80333EE0[] = "POV Right";
char lbl_80333EEC[] = "D-Pad Right";
char lbl_80333EF8[] = "JoyAXPlus";
char lbl_80333F04[] = "JoyAYPlus";
char lbl_80333F10[] = "JoyBXPlus";
char lbl_80333F1C[] = "JoyBYPlus";
char lbl_80333F28[] = "JoyAXMinus";
char lbl_80333F34[] = "JoyAYMinus";
char lbl_80333F40[] = "JoyBXMinus";
char lbl_80333F4C[] = "JoyBYMinus";
char lbl_80333F58[] = "JoyALeft";
char lbl_80333F64[] = "JoyARight";
char lbl_80333F70[] = "JoyADown";
char lbl_80333F7C[] = "JoyBLeft";
char lbl_80333F88[] = "JoyBRight";
char lbl_80333F94[] = "JoyBDown";
char lbl_80333FA0[] = "JoyCLeft";
char lbl_80333FAC[] = "JC Right";
char lbl_80333FB8[] = "JoyCRight";
char lbl_80333FC4[] = "JoyCDown";
char lbl_80333FD0[] = "JoyDLeft";
char lbl_80333FDC[] = "JD Right";
char lbl_80333FE8[] = "JoyDRight";
char lbl_80333FF4[] = "JoyDDown";
char lbl_80334000[] = "Button 8";
char lbl_8033400C[] = "Button 9";
char lbl_80334018[] = "Button 10";
char lbl_80334024[] = "Button10";
char lbl_80334030[] = "Button 11";
char lbl_8033403C[] = "Button11";
char lbl_80334048[] = "Button 12";
char lbl_80334054[] = "Button12";
char lbl_80334060[] = "Button 13";
char lbl_8033406C[] = "Button13";
char lbl_80334078[] = "Button 14";
char lbl_80334084[] = "Button14";
char lbl_80334090[] = "Button 15";
char lbl_8033409C[] = "Button15";
char lbl_803340A8[] = "Button 16";
char lbl_803340B4[] = "Button16";
char lbl_803340C0[] = "Button 17";
char lbl_803340CC[] = "Button 18";
char lbl_803340D8[] = "Button 19";
char lbl_803340E4[] = "Button 20";
char lbl_803340F0[] = "Button 21";
char lbl_803340FC[] = "Button 22";
char lbl_80334108[] = "Button 23";
char lbl_80334114[] = "Button 24";
char lbl_80334120[] = "Button 25";
char lbl_8033412C[] = "Button 26";
char lbl_80334138[] = "Button 27";
char lbl_80334144[] = "Button 28";
char lbl_80334150[] = "Button 29";
char lbl_8033415C[] = "Button 30";
char lbl_80334168[] = "Button 31";
char lbl_80334174[] = "Button 32";
char lbl_80334180[] = "POVFront";
char lbl_8033418C[] = "POVRight";
char lbl_80334198[] = "MouseDown";
char lbl_803341A4[] = "MouseLeft";
char lbl_803341B0[] = "MouseRight";
char lbl_803341BC[] = "Key_LBRACKET";
char lbl_803341CC[] = "Key_RBRACKET";
char lbl_803341DC[] = "Key_SEMICOLON";
char lbl_803341EC[] = "Key_QUOTE";
char lbl_803341F8[] = "Key_COMMA";
char lbl_80334204[] = "Key_PERIOD";
char lbl_80334210[] = "Key_SLASH";
char lbl_8033421C[] = "Key_MINUS";
char lbl_80334228[] = "Key_EQUAL";
char lbl_80334234[] = "Key_SHIFT";
char lbl_80334240[] = "KeySHIFT";
char lbl_8033424C[] = "Key_CTRL";
char lbl_80334258[] = "Key_RSHIFT";
char lbl_80334264[] = "KeyRSHIFT";
char lbl_80334270[] = "Key_RCTRL";
char lbl_8033427C[] = "KeyRCTRL";
char lbl_80334288[] = "Key_SPACE";
char lbl_80334294[] = "KeySPACE";
char lbl_803342A0[] = "Key_DOWN";
char lbl_803342AC[] = "Key_LEFT";
char lbl_803342B8[] = "Key_RIGHT";
char lbl_803342C4[] = "KeyRIGHT";
char lbl_8055A520[] = "Joy Up";
char lbl_8055A528[] = "JB Up";
char lbl_8055A530[] = "JB Down";
char lbl_8055A538[] = "JB Left";
char lbl_8055A540[] = "Joy DX-";
char lbl_8055A548[] = "Joy DY-";
char lbl_8055A550[] = "Joy AX+";
char lbl_8055A558[] = "Joy AY+";
char lbl_8055A560[] = "Joy BX+";
char lbl_8055A568[] = "Joy BY+";
char lbl_8055A570[] = "Joy AX-";
char lbl_8055A578[] = "Joy AY-";
char lbl_8055A580[] = "Joy BX-";
char lbl_8055A588[] = "Joy BY-";
char lbl_8055A590[] = "JoyAUp";
char lbl_8055A598[] = "JoyBUp";
char lbl_8055A5A0[] = "JC Left";
char lbl_8055A5A8[] = "JC Up";
char lbl_8055A5B0[] = "JoyCUp";
char lbl_8055A5B8[] = "JC Down";
char lbl_8055A5C0[] = "Joy CX+";
char lbl_8055A5C8[] = "Joy CX-";
char lbl_8055A5D0[] = "Joy CY+";
char lbl_8055A5D8[] = "Joy CY-";
char lbl_8055A5E0[] = "JD Left";
char lbl_8055A5E8[] = "JD Up";
char lbl_8055A5F0[] = "JoyDUp";
char lbl_8055A5F8[] = "JD Down";
char lbl_8055A600[] = "Joy DX+";
char lbl_8055A608[] = "Joy DY+";
char lbl_8055A610[] = "Button1";
char lbl_8055A618[] = "Button2";
char lbl_8055A620[] = "Button3";
char lbl_8055A628[] = "Button4";
char lbl_8055A630[] = "Button5";
char lbl_8055A638[] = "Button6";
char lbl_8055A640[] = "Button7";
char lbl_8055A648[] = "Button8";
char lbl_8055A650[] = "Button9";
char lbl_8055A658[] = "Btn17";
char lbl_8055A660[] = "Btn18";
char lbl_8055A668[] = "Btn19";
char lbl_8055A670[] = "Btn20";
char lbl_8055A678[] = "Btn21";
char lbl_8055A680[] = "Btn22";
char lbl_8055A688[] = "Btn23";
char lbl_8055A690[] = "Btn24";
char lbl_8055A698[] = "Btn26";
char lbl_8055A6A0[] = "Btn27";
char lbl_8055A6A8[] = "Btn28";
char lbl_8055A6B0[] = "Btn29";
char lbl_8055A6B8[] = "Btn30";
char lbl_8055A6C0[] = "Btn31";
char lbl_8055A6C8[] = "Btn32";
char lbl_8055A6D0[] = "BtnXX";
char lbl_8055A6D8[] = "POVLeft";
char lbl_8055A6E0[] = "POVBack";
char lbl_8055A6E8[] = "POV Any";
char lbl_8055A6F0[] = "POVAny";
char lbl_8055A6F8[] = "M Up";
char lbl_8055A700[] = "MouseUp";
char lbl_8055A708[] = "M Down";
char lbl_8055A710[] = "M Left";
char lbl_8055A718[] = "M Right";
char lbl_8055A720[] = "Key_A";
char lbl_8055A728[] = "Key_B";
char lbl_8055A730[] = "Key_C";
char lbl_8055A738[] = "Key_D";
char lbl_8055A740[] = "Key_E";
char lbl_8055A748[] = "Key_F";
char lbl_8055A750[] = "Key_G";
char lbl_8055A758[] = "Key_H";
char lbl_8055A760[] = "Key_I";
char lbl_8055A768[] = "Key_J";
char lbl_8055A770[] = "Key_K";
char lbl_8055A778[] = "Key_L";
char lbl_8055A780[] = "Key_M";
char lbl_8055A788[] = "Key_N";
char lbl_8055A790[] = "Key_O";
char lbl_8055A798[] = "Key_Q";
char lbl_8055A7A0[] = "Key_R";
char lbl_8055A7A8[] = "Key_S";
char lbl_8055A7B0[] = "Key_U";
char lbl_8055A7B8[] = "Key_V";
char lbl_8055A7C0[] = "Key_W";
char lbl_8055A7C8[] = "Key_X";
char lbl_8055A7D0[] = "Key_Y";
char lbl_8055A7D8[] = "Key_Z";
char lbl_8055A7E0[] = "Key_6";
char lbl_8055A7E8[] = "Key_7";
char lbl_8055A7F0[] = "Key_8";
char lbl_8055A7F8[] = "Key_9";
char lbl_8055A800[] = "Key_0";
char lbl_8055A808[] = "KeyCTRL";
char lbl_8055A810[] = "Key_INS";
char lbl_8055A818[] = "KeyINS";
char lbl_8055A820[] = "Key_DEL";
char lbl_8055A828[] = "KeyDEL";
char lbl_8055A830[] = "Key_UP";
char lbl_8055A838[] = "KeyUP";
char lbl_8055A840[] = "KeyDOWN";
char lbl_8055A848[] = "KeyLEFT";
char lbl_8055A850[] = "Unknown";

extern const NamePair lbl_802F9588[21] = {
    {lbl_8055A520, lbl_80333D18},
    {lbl_80333D24, lbl_80333D30},
    {lbl_80333D40, lbl_80333D4C},
    {lbl_80333D5C, lbl_80333D68},
    {lbl_8055A528, lbl_80333D78},
    {lbl_8055A530, lbl_80333D84},
    {lbl_8055A538, lbl_80333D94},
    {lbl_80333DA4, lbl_80333DB0},
    {lbl_8055A540, lbl_80333DC0},
    {lbl_8055A548, lbl_80333DD0},
    {lbl_80333DE0, lbl_80333DEC},
    {lbl_80333DF8, lbl_80333E04},
    {lbl_80333E10, lbl_80333E1C},
    {lbl_80333E28, lbl_80333E34},
    {lbl_80333E40, lbl_80333E4C},
    {lbl_80333E58, lbl_80333E64},
    {lbl_80333E78, lbl_80333E84},
    {lbl_80333E98, lbl_80333EA4},
    {lbl_80333EB0, lbl_80333EBC},
    {lbl_80333EC8, lbl_80333ED4},
    {lbl_80333EE0, lbl_80333EEC},
};

BindEntry lbl_803342D0[0x7A] = {
    {lbl_8055A550, lbl_80333EF8, 2, 0x10, 0x0, {0}, 0},
    {lbl_8055A558, lbl_80333F04, 2, 0x11, 0x0, {0}, 0},
    {lbl_8055A560, lbl_80333F10, 2, 0x12, 0x0, {0}, 0},
    {lbl_8055A568, lbl_80333F1C, 2, 0x13, 0x0, {0}, 0},
    {lbl_8055A570, lbl_80333F28, 2, 0x18, 0x0, {0}, 0},
    {lbl_8055A578, lbl_80333F34, 2, 0x19, 0x0, {0}, 0},
    {lbl_8055A580, lbl_80333F40, 2, 0x1A, 0x0, {0}, 0},
    {lbl_8055A588, lbl_80333F4C, 2, 0x1B, 0x0, {0}, 0},
    {lbl_80333D40, lbl_80333F58, 2, 0x0, 0x0, {0}, 0},
    {lbl_80333D5C, lbl_80333F64, 2, 0x1, 0x0, {0}, 0},
    {lbl_8055A520, lbl_8055A590, 2, 0x2, 0x0, {0}, 0},
    {lbl_80333D24, lbl_80333F70, 2, 0x3, 0x0, {0}, 0},
    {lbl_8055A538, lbl_80333F7C, 2, 0x4, 0x0, {0}, 0},
    {lbl_80333DA4, lbl_80333F88, 2, 0x5, 0x0, {0}, 0},
    {lbl_8055A528, lbl_8055A598, 2, 0x6, 0x0, {0}, 0},
    {lbl_8055A530, lbl_80333F94, 2, 0x7, 0x0, {0}, 0},
    {lbl_8055A5A0, lbl_80333FA0, 2, 0x8, 0x0, {0}, 0},
    {lbl_80333FAC, lbl_80333FB8, 2, 0x9, 0x0, {0}, 0},
    {lbl_8055A5A8, lbl_8055A5B0, 2, 0xA, 0x0, {0}, 0},
    {lbl_8055A5B8, lbl_80333FC4, 2, 0xB, 0x0, {0}, 0},
    {lbl_8055A5C0, lbl_8055A5C0, 2, 0x14, 0x0, {0}, 0},
    {lbl_8055A5C8, lbl_8055A5C8, 2, 0x1C, 0x0, {0}, 0},
    {lbl_8055A5D0, lbl_8055A5D0, 2, 0x15, 0x0, {0}, 0},
    {lbl_8055A5D8, lbl_8055A5D8, 2, 0x1D, 0x0, {0}, 0},
    {lbl_8055A5E0, lbl_80333FD0, 2, 0xC, 0x0, {0}, 0},
    {lbl_80333FDC, lbl_80333FE8, 2, 0xD, 0x0, {0}, 0},
    {lbl_8055A5E8, lbl_8055A5F0, 2, 0xE, 0x0, {0}, 0},
    {lbl_8055A5F8, lbl_80333FF4, 2, 0xF, 0x0, {0}, 0},
    {lbl_8055A600, lbl_8055A600, 2, 0x16, 0x0, {0}, 0},
    {lbl_8055A540, lbl_8055A540, 2, 0x1E, 0x0, {0}, 0},
    {lbl_8055A608, lbl_8055A608, 2, 0x17, 0x0, {0}, 0},
    {lbl_8055A548, lbl_8055A548, 2, 0x1F, 0x0, {0}, 0},
    {lbl_80333DE0, lbl_8055A610, 3, 0x0, 0x0, {0}, 0},
    {lbl_80333DF8, lbl_8055A618, 3, 0x1, 0x0, {0}, 0},
    {lbl_80333E10, lbl_8055A620, 3, 0x2, 0x0, {0}, 0},
    {lbl_80333E28, lbl_8055A628, 3, 0x3, 0x0, {0}, 0},
    {lbl_80333E40, lbl_8055A630, 3, 0x4, 0x0, {0}, 0},
    {lbl_80333E58, lbl_8055A638, 3, 0x5, 0x0, {0}, 0},
    {lbl_80333E78, lbl_8055A640, 3, 0x6, 0x0, {0}, 0},
    {lbl_80334000, lbl_8055A648, 3, 0x7, 0x0, {0}, 0},
    {lbl_8033400C, lbl_8055A650, 3, 0x8, 0x0, {0}, 0},
    {lbl_80334018, lbl_80334024, 3, 0x9, 0x0, {0}, 0},
    {lbl_80334030, lbl_8033403C, 3, 0xA, 0x0, {0}, 0},
    {lbl_80334048, lbl_80334054, 3, 0xB, 0x0, {0}, 0},
    {lbl_80334060, lbl_8033406C, 3, 0xC, 0x0, {0}, 0},
    {lbl_80334078, lbl_80334084, 3, 0xD, 0x0, {0}, 0},
    {lbl_80334090, lbl_8033409C, 3, 0xE, 0x0, {0}, 0},
    {lbl_803340A8, lbl_803340B4, 3, 0xF, 0x0, {0}, 0},
    {lbl_803340C0, lbl_8055A658, 3, 0x10, 0x0, {0}, 0},
    {lbl_803340CC, lbl_8055A660, 3, 0x11, 0x0, {0}, 0},
    {lbl_803340D8, lbl_8055A668, 3, 0x12, 0x0, {0}, 0},
    {lbl_803340E4, lbl_8055A670, 3, 0x13, 0x0, {0}, 0},
    {lbl_803340F0, lbl_8055A678, 3, 0x14, 0x0, {0}, 0},
    {lbl_803340FC, lbl_8055A680, 3, 0x15, 0x0, {0}, 0},
    {lbl_80334108, lbl_8055A688, 3, 0x16, 0x0, {0}, 0},
    {lbl_80334114, lbl_8055A690, 3, 0x17, 0x0, {0}, 0},
    {lbl_80334120, lbl_8055A698, 3, 0x18, 0x0, {0}, 0},
    {lbl_8033412C, lbl_8055A6A0, 3, 0x19, 0x0, {0}, 0},
    {lbl_80334138, lbl_8055A6A8, 3, 0x1A, 0x0, {0}, 0},
    {lbl_80334144, lbl_8055A6B0, 3, 0x1B, 0x0, {0}, 0},
    {lbl_80334150, lbl_8055A6B8, 3, 0x1C, 0x0, {0}, 0},
    {lbl_8033415C, lbl_8055A6C0, 3, 0x1D, 0x0, {0}, 0},
    {lbl_80334168, lbl_8055A6C8, 3, 0x1E, 0x0, {0}, 0},
    {lbl_80334174, lbl_8055A6D0, 3, 0x1F, 0x0, {0}, 0},
    {lbl_80333EC8, lbl_8055A6D8, 4, 0x8, 0x0, {0}, 0},
    {lbl_80333E98, lbl_80334180, 4, 0x1, 0x0, {0}, 0},
    {lbl_80333EE0, lbl_8033418C, 4, 0x2, 0x0, {0}, 0},
    {lbl_80333EB0, lbl_8055A6E0, 4, 0x4, 0x0, {0}, 0},
    {lbl_8055A6E8, lbl_8055A6F0, 4, 0xF, 0x0, {0}, 0},
    {lbl_8055A6F8, lbl_8055A700, 5, 0x2, 0x0, {0}, 0},
    {lbl_8055A708, lbl_80334198, 5, 0x3, 0x0, {0}, 0},
    {lbl_8055A710, lbl_803341A4, 5, 0x0, 0x0, {0}, 0},
    {lbl_8055A718, lbl_803341B0, 5, 0x1, 0x0, {0}, 0},
    {lbl_8055A720, 0, 1, 0x1E, 0x0, {0}, 0},
    {lbl_8055A728, 0, 1, 0x30, 0x0, {0}, 0},
    {lbl_8055A730, 0, 1, 0x2E, 0x0, {0}, 0},
    {lbl_8055A738, 0, 1, 0x20, 0x0, {0}, 0},
    {lbl_8055A740, 0, 1, 0x12, 0x0, {0}, 0},
    {lbl_8055A748, 0, 1, 0x21, 0x0, {0}, 0},
    {lbl_8055A750, 0, 1, 0x22, 0x0, {0}, 0},
    {lbl_8055A758, 0, 1, 0x23, 0x0, {0}, 0},
    {lbl_8055A760, 0, 1, 0x17, 0x0, {0}, 0},
    {lbl_8055A768, 0, 1, 0x24, 0x0, {0}, 0},
    {lbl_8055A770, 0, 1, 0x25, 0x0, {0}, 0},
    {lbl_8055A778, 0, 1, 0x26, 0x0, {0}, 0},
    {lbl_8055A780, 0, 1, 0x32, 0x0, {0}, 0},
    {lbl_8055A788, 0, 1, 0x31, 0x0, {0}, 0},
    {lbl_8055A790, 0, 1, 0x18, 0x0, {0}, 0},
    {lbl_8055A798, 0, 1, 0x10, 0x0, {0}, 0},
    {lbl_8055A7A0, 0, 1, 0x13, 0x0, {0}, 0},
    {lbl_8055A7A8, 0, 1, 0x1F, 0x0, {0}, 0},
    {lbl_8055A7B0, 0, 1, 0x16, 0x0, {0}, 0},
    {lbl_8055A7B8, 0, 1, 0x2F, 0x0, {0}, 0},
    {lbl_8055A7C0, 0, 1, 0x11, 0x0, {0}, 0},
    {lbl_8055A7C8, 0, 1, 0x2D, 0x0, {0}, 0},
    {lbl_8055A7D0, 0, 1, 0x15, 0x0, {0}, 0},
    {lbl_8055A7D8, 0, 1, 0x2C, 0x0, {0}, 0},
    {lbl_8055A7E0, 0, 1, 0x7, 0x0, {0}, 0},
    {lbl_8055A7E8, 0, 1, 0x8, 0x0, {0}, 0},
    {lbl_8055A7F0, 0, 1, 0x9, 0x0, {0}, 0},
    {lbl_8055A7F8, 0, 1, 0xA, 0x0, {0}, 0},
    {lbl_8055A800, 0, 1, 0xB, 0x0, {0}, 0},
    {lbl_803341BC, 0, 1, 0x1A, 0x0, {0}, 0},
    {lbl_803341CC, 0, 1, 0x1B, 0x0, {0}, 0},
    {lbl_803341DC, 0, 1, 0x27, 0x0, {0}, 0},
    {lbl_803341EC, 0, 1, 0x28, 0x0, {0}, 0},
    {lbl_803341F8, 0, 1, 0x33, 0x0, {0}, 0},
    {lbl_80334204, 0, 1, 0x34, 0x0, {0}, 0},
    {lbl_80334210, 0, 1, 0x35, 0x0, {0}, 0},
    {lbl_8033421C, 0, 1, 0xC, 0x0, {0}, 0},
    {lbl_80334228, 0, 1, 0xD, 0x0, {0}, 0},
    {lbl_80334234, lbl_80334240, 1, 0xFD, 0x0, {0}, 0},
    {lbl_8033424C, lbl_8055A808, 1, 0xFE, 0x0, {0}, 0},
    {lbl_80334258, lbl_80334264, 1, 0x36, 0x0, {0}, 0},
    {lbl_80334270, lbl_8033427C, 1, 0x9D, 0x0, {0}, 0},
    {lbl_8055A810, lbl_8055A818, 1, 0xD2, 0x0, {0}, 0},
    {lbl_8055A820, lbl_8055A828, 1, 0xD3, 0x0, {0}, 0},
    {lbl_80334288, lbl_80334294, 1, 0x39, 0x0, {0}, 1},
    {lbl_8055A830, lbl_8055A838, 1, 0xC8, 0x0, {0}, 0},
    {lbl_803342A0, lbl_8055A840, 1, 0xD0, 0x0, {0}, 0},
    {lbl_803342AC, lbl_8055A848, 1, 0xCB, 0x0, {0}, 0},
    {lbl_803342B8, lbl_803342C4, 1, 0xCD, 0x0, {0}, 0},
};
}

ModuleTag lbl_8055E5C0;
u8 lbl_8055E5C8;          // keyboard disabled
BindEntry* lbl_8055E5CC;  // the bindable controls
s32 lbl_8055E5D0;         // how many
s8 lbl_8055E5D4;          // key codes filled in

static JoyStatus lbl_80435050[4];  // current pads
static JoyStatus lbl_804351E0[4];  // last frame's
static Mouse lbl_80435370;  // current mouse
static Mouse lbl_8043537C;  // last frame's

static LocString lbl_80435388(lbl_80333D18 + 0x126C);
static LocString lbl_80435394(lbl_80333D18 + 0x1288);
static LocString lbl_804353A0(lbl_80333D18 + 0x12A4);
static LocString lbl_804353AC(lbl_80333D18 + 0x12C0);
static LocString lbl_804353B8(lbl_80333D18 + 0x12DC);
static LocString lbl_804353C4(lbl_80333D18 + 0x12F8);
static LocString lbl_804353D0(lbl_80333D18 + 0x1314);
static LocString lbl_804353DC(lbl_80333D18 + 0x1330);
static LocString lbl_804353E8(lbl_80333D18 + 0x1350);
static LocString lbl_804353F4(lbl_80333D18 + 0x1368);
static LocString lbl_80435400(lbl_80333D18 + 0x1384);
static LocString lbl_8043540C(lbl_80333D18 + 0x13A0);
static LocString lbl_80435418(lbl_80333D18 + 0x13B8);
static LocString lbl_80435424(lbl_80333D18 + 0x13D0);
static LocString lbl_80435430(lbl_80333D18 + 0x13EC);
static LocString lbl_8043543C(lbl_80333D18 + 0x1408);
static LocString lbl_80435448(lbl_80333D18 + 0x1420);
static LocString lbl_80435454(lbl_80333D18 + 0x1438);
static LocString lbl_80435460(lbl_80333D18 + 0x1450);
static LocString lbl_8043546C(lbl_80333D18 + 0x1468);
static LocString lbl_80435478(lbl_80333D18 + 0x1480);
static LocString lbl_80435484(lbl_80333D18 + 0x1498);
static LocString lbl_80435490(lbl_80333D18 + 0x14B0);
static LocString lbl_8043549C(lbl_80333D18 + 0x14C8);
static LocString lbl_804354A8(lbl_80333D18 + 0x14E0);
static LocString lbl_804354B4(lbl_80333D18 + 0x14F8);
static LocString lbl_804354C0(lbl_80333D18 + 0x1510);
static LocString lbl_804354CC(lbl_80333D18 + 0x1528);
static LocString lbl_804354D8(lbl_80333D18 + 0x1540);
static LocString lbl_804354E4(lbl_80333D18 + 0x1558);
static LocString lbl_804354F0(lbl_80333D18 + 0x1570);
static LocString lbl_804354FC(lbl_80333D18 + 0x1588);
static LocString lbl_80435508(lbl_80333D18 + 0x15A0);
static LocString lbl_80435514(lbl_80333D18 + 0x15B8);
static LocString lbl_80435520(lbl_80333D18 + 0x15D0);
static LocString lbl_8043552C(lbl_80333D18 + 0x15E8);
static LocString lbl_80435538(lbl_80333D18 + 0x1600);
static LocString lbl_80435544(lbl_80333D18 + 0x1618);
static LocString lbl_80435550(lbl_80333D18 + 0x1630);
static LocString lbl_8043555C(lbl_80333D18 + 0x164C);
static LocString lbl_80435568(lbl_80333D18 + 0x1668);
static LocString lbl_80435574(lbl_80333D18 + 0x1684);
static LocString lbl_80435580(lbl_80333D18 + 0x16A4);
static LocString lbl_8043558C(lbl_80333D18 + 0x16C8);
static LocString lbl_80435598(lbl_80333D18 + 0x16E8);
static LocString lbl_804355A4(lbl_80333D18 + 0x1700);
static LocString lbl_804355B0(lbl_80333D18 + 0x1718);
static LocString lbl_804355BC(lbl_80333D18 + 0x1738);
static LocString lbl_804355C8(lbl_80333D18 + 0x1750);
static LocString lbl_804355D4(lbl_80333D18 + 0x1768);
static LocString lbl_804355E0(lbl_80333D18 + 0x1780);
static LocString lbl_804355EC(lbl_80333D18 + 0x1798);
static LocString lbl_804355F8(lbl_80333D18 + 0x17B0);
static LocString lbl_80435604(lbl_80333D18 + 0x17C8);
static LocString lbl_80435610(lbl_80333D18 + 0x17E0);
static LocString lbl_8043561C(lbl_80333D18 + 0x17F8);

static inline f32 MaxZero(f32 v) { return 0.0f > v ? 0.0f : v; }

static inline void StrNCopy(char* dst, const char* src, int len) {
    char* d = dst;
    int n = len;
    const char* s = src;
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

extern "C" {

void fn_801A7628(void);
f32 fn_801A7F98(const Binding* b);
f32 fn_801A85E8(const Binding* b);
int fn_801A8A10(const Binding* b);
int fn_801A8E38(const Binding* b);
int fn_801A8EE4(Binding* b);
int ControlExists(const Binding* b);
void fn_801A90A4(char* out, int len, Binding* b);
void fn_801A9110(Binding* b, const char* name);
void fn_801A91B0(Binding* b, BindEntry* e);
BindEntry* fn_801A9234(Binding* b);

const NamePair* fn_801A75B4(const char* name) {
    const NamePair* p = lbl_802F9588;
    int i;
    for (i = 0; i < 21; i++, p++) {
        if (!strcasecmp(p->name, name)) {
            return p;
        }
    }
    return 0;
}

void fn_801A7628(void) {
    if (!lbl_8055E5D4) {
        lbl_803342D0[73].key = fn_802E47F8(0x1E);
        lbl_803342D0[74].key = fn_802E47F8(0x30);
        lbl_803342D0[75].key = fn_802E47F8(0x2E);
        lbl_803342D0[76].key = fn_802E47F8(0x20);
        lbl_803342D0[77].key = fn_802E47F8(0x12);
        lbl_803342D0[78].key = fn_802E47F8(0x21);
        lbl_803342D0[79].key = fn_802E47F8(0x22);
        lbl_803342D0[80].key = fn_802E47F8(0x23);
        lbl_803342D0[81].key = fn_802E47F8(0x17);
        lbl_803342D0[82].key = fn_802E47F8(0x24);
        lbl_803342D0[83].key = fn_802E47F8(0x25);
        lbl_803342D0[84].key = fn_802E47F8(0x26);
        lbl_803342D0[85].key = fn_802E47F8(0x32);
        lbl_803342D0[86].key = fn_802E47F8(0x31);
        lbl_803342D0[87].key = fn_802E47F8(0x18);
        lbl_803342D0[88].key = fn_802E47F8(0x10);
        lbl_803342D0[89].key = fn_802E47F8(0x13);
        lbl_803342D0[90].key = fn_802E47F8(0x1F);
        lbl_803342D0[91].key = fn_802E47F8(0x16);
        lbl_803342D0[92].key = fn_802E47F8(0x2F);
        lbl_803342D0[93].key = fn_802E47F8(0x11);
        lbl_803342D0[94].key = fn_802E47F8(0x2D);
        lbl_803342D0[95].key = fn_802E47F8(0x15);
        lbl_803342D0[96].key = fn_802E47F8(0x2C);
        lbl_803342D0[97].key = fn_802E47F8(0x7);
        lbl_803342D0[98].key = fn_802E47F8(0x8);
        lbl_803342D0[99].key = fn_802E47F8(0x9);
        lbl_803342D0[100].key = fn_802E47F8(0xA);
        lbl_803342D0[101].key = fn_802E47F8(0xB);
        lbl_803342D0[102].key = fn_802E47F8(0x1A);
        lbl_803342D0[103].key = fn_802E47F8(0x1B);
        lbl_803342D0[104].key = fn_802E47F8(0x27);
        lbl_803342D0[105].key = fn_802E47F8(0x28);
        lbl_803342D0[106].key = fn_802E47F8(0x33);
        lbl_803342D0[107].key = fn_802E47F8(0x34);
        lbl_803342D0[108].key = fn_802E47F8(0x35);
        lbl_803342D0[109].key = fn_802E47F8(0xC);
        lbl_803342D0[110].key = fn_802E47F8(0xD);
        lbl_8055E5D4 = 1;
    }
    lbl_8055E5CC = lbl_803342D0;
    lbl_8055E5D0 = 0x7A;
}

void fn_801A7844(int reset) {
    s32 x;
    s32 y;
    s32 buttons;
    if (reset) {
        JoyStatus* p = lbl_80435050;
        int i;
        for (i = 0; i < 4; i++, p++) {
            memset(p, 0, sizeof(*p));
            fn_80129AE8(p, i);
        }
    }
    fn_80127A84(&x, &y, &buttons);
    {
        s32 w = lbl_8055CFF0 / 2;
        lbl_80435370.x = (f32)(x - w) / (f32)w;
        lbl_80435370.y = (f32)(y - (lbl_8055CFF4 / 2)) / (f32)(lbl_8055CFF4 / 2);
        lbl_80435370.b0 = buttons & 1;
        lbl_80435370.b1 = (buttons >> 1) & 1;
        lbl_80435370.b2 = (buttons >> 2) & 1;
    }
}

void fn_801A7974(Binding* b, const char* key, int section) {
    char buf[32] = {0};
    fn_801A90A4(buf, sizeof(buf), b);
    fn_801334F4(section, key, buf, sizeof(buf));
    fn_801A9110(b, buf);
}

void fn_801A7A08(Binding* b, const char* key, int section) {
    char buf[32];
    fn_801A90A4(buf, sizeof(buf), b);
    fn_8013358C(section, key, buf);
}

void fn_801A7A5C(void) {
    int i;
    lbl_80435370.x = 0.0f;
    lbl_80435370.y = 0.0f;
    lbl_80435370.b2 = 0;
    lbl_80435370.b1 = 0;
    lbl_80435370.b0 = 0;
    fn_801A7844(1);
    fn_801A7844(1);
    for (i = 0; i < 4; i++) {
        lbl_804351E0[i] = lbl_80435050[i];
    }
    lbl_8043537C = lbl_80435370;
}

int fn_801A7C8C(Binding* out) {
    int i;
    fn_801A7628();
    for (i = 0; i < lbl_8055E5D0; i++) {
        if (!lbl_8055E5CC[i].used) {
            Binding b;
            b.pad = 0;
            b.latched = 0;
            fn_801A91B0(&b, &lbl_8055E5CC[i]);
            if (b.type != 5 && b.type != 6) {
                if (fn_801A8EE4(&b)) {
                    if (fn_801A8E38(&b) && !fn_801A8A10(&b)) {
                        *out = b;
                        return 1;
                    }
                } else {
                    f32 v = fn_801A7F98(&b);
                    if ((f32)__fabs(fn_801A85E8(&b)) < 0.25 && v > 0.5f) {
                        *out = b;
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

void fn_801A7DFC(const char* key, const char* name, int section) {
    Binding b;
    b.pad = 0;
    b.latched = 0;
    fn_801A9110(&b, name);
    fn_801A7A08(&b, key, section);
}

void fn_801A7E54(char* out, int len, Binding* b) {
    BindEntry* e = fn_801A9234(b);
    if (e) {
        const NamePair* p = fn_801A75B4(e->name);
        if (p) {
            StrNCopy(out, p->text, len);
        } else {
            StrNCopy(out, e->label ? e->label : (const char*)&e->key, len);
        }
    } else {
        StrNCopy(out, lbl_8055A850, len);
    }
    out[len - 1] = 0;
}

void fn_801A7F8C(void) { lbl_8055E5C8 = 0; }

f32 fn_801A7F98(const Binding* b) {
    JoyStatus* p;
    if (!ControlExists(b)) {
        return 0.0f;
    }
    p = &lbl_80435050[b->pad];
    switch (b->type) {
    case 2:
        switch (b->code) {
        case 0:
            return MaxZero(-p->ax0);
        case 1:
            return MaxZero(p->ax0);
        case 2:
            return MaxZero(-p->ax1);
        case 3:
            return MaxZero(p->ax1);
        case 4:
            return MaxZero(-p->ax3);
        case 5:
            return MaxZero(p->ax3);
        case 6:
            return MaxZero(-p->ax2);
        case 7:
            return MaxZero(p->ax2);
        case 8:
            return MaxZero(-p->ax4);
        case 9:
            return MaxZero(p->ax4);
        case 10:
            return MaxZero(-p->ax5);
        case 11:
            return MaxZero(p->ax5);
        case 12:
            return MaxZero(-p->ax6);
        case 13:
            return MaxZero(p->ax6);
        case 14:
            return MaxZero(-p->ax7);
        case 15:
            return MaxZero(p->ax7);
        case 16:
            return 0.5f * (1.0f - p->ax0);
        case 17:
            return 0.5f * (1.0f - p->ax1);
        case 18:
            return 0.5f * (1.0f - p->ax3);
        case 19:
            return 0.5f * (1.0f - p->ax2);
        case 20:
            return 0.5f * (1.0f - p->ax4);
        case 21:
            return 0.5f * (1.0f - p->ax5);
        case 22:
            return 0.5f * (1.0f - p->ax6);
        case 23:
            return 0.5f * (1.0f - p->ax7);
        case 24:
            return 0.5f * (1.0f + p->ax0);
        case 25:
            return 0.5f * (1.0f + p->ax1);
        case 26:
            return 0.5f * (1.0f + p->ax3);
        case 27:
            return 0.5f * (1.0f + p->ax2);
        case 28:
            return 0.5f * (1.0f + p->ax4);
        case 29:
            return 0.5f * (1.0f + p->ax5);
        case 30:
            return 0.5f * (1.0f + p->ax6);
        case 31:
            return 0.5f * (1.0f + p->ax7);
        }
        break;
    case 3:
        if (b->code >= 0 && (u32)b->code < 32) {
            return p->buttons[b->code] ? 1.0f : 0.0f;
        }
        break;
    case 4:
        return (p->dir & b->code) ? 1.0f : 0.0f;
    case 5:
        switch (b->code) {
        case 0:
            return MaxZero(-lbl_80435370.x);
        case 1:
            return MaxZero(lbl_80435370.x);
        case 2:
            return MaxZero(-lbl_80435370.y);
        case 3:
            return MaxZero(lbl_80435370.y);
        case 4:
            return 0.5f * (1.0f - lbl_80435370.x);
        case 6:
            return 0.5f * (1.0f - lbl_80435370.y);
        case 5:
            return 0.5f * (1.0f + lbl_80435370.x);
        case 7:
            return 0.5f * (1.0f + lbl_80435370.y);
        }
        break;
    case 6:
        switch (b->code) {
        case 0:
            return lbl_80435370.b0 ? 1.0f : 0.0f;
        case 2:
            return lbl_80435370.b1 ? 1.0f : 0.0f;
        case 1:
            return lbl_80435370.b2 ? 1.0f : 0.0f;
        }
        break;
    case 1:
        if (!lbl_8055E5C8) {
            return fn_802E4A1C(b->key) ? 1.0f : 0.0f;
        }
        break;
    }
    return 0.0f;
}

f32 fn_801A85E8(const Binding* b) {
    if (b->type == 1) {
        return 0.0f;
    } else {
        JoyStatus* cur = &lbl_80435050[b->pad];
        JoyStatus saved = *cur;
        Mouse m = lbl_80435370;
        f32 v;
        *cur = lbl_804351E0[b->pad];
        lbl_80435370 = lbl_8043537C;
        v = fn_801A7F98(b);
        *cur = saved;
        lbl_80435370 = m;
        return v;
    }
}

int fn_801A8A10(const Binding* b) {
    if (b->type == 1) {
        return 0;
    } else {
        JoyStatus* cur = &lbl_80435050[b->pad];
        JoyStatus saved = *cur;
        Mouse m = lbl_80435370;
        int v;
        *cur = lbl_804351E0[b->pad];
        lbl_80435370 = lbl_8043537C;
        v = fn_801A8E38(b);
        *cur = saved;
        lbl_80435370 = m;
        return v;
    }
}

int fn_801A8E38(const Binding* b) { return fn_801A7F98(b) > 0.2f; }

int fn_801A8E68(Binding* b) {
    switch (b->type) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        if (fn_801A8E38(b)) {
            if (!b->latched) {
                b->latched = 1;
                return 1;
            }
        } else {
            b->latched = 0;
            return 0;
        }
        break;
    }
    return 0;
}

int fn_801A8EE4(Binding* b) {
    s32 t = b->type;
    return t == 1 || t == 3 || t == 4 || t == 6;
}

int ControlExists(const Binding* b) {
    s32 type = b->type;
    JoyInfo info = lbl_80435050[b->pad].info;
    if (type == 4) {
        return (info.hold >> 15) & 1;
    }
    if (type == 3) {
        return (info.bits & (1 << b->code)) != 0;
    }
    if (type == 2) {
        switch (b->code) {
    case 0:
    case 1:
    case 16:
    case 24:
        return (info.hold >> 0) & 1;
    case 2:
    case 3:
    case 17:
    case 25:
        return (info.hold >> 1) & 1;
    case 4:
    case 5:
    case 18:
    case 26:
        return (info.hold >> 3) & 1;
    case 6:
    case 7:
    case 19:
    case 27:
        return (info.hold >> 2) & 1;
    case 8:
    case 9:
    case 20:
    case 28:
        return (info.hold >> 4) & 1;
    case 10:
    case 11:
    case 21:
    case 29:
        return (info.hold >> 5) & 1;
    case 12:
    case 13:
    case 22:
    case 30:
        return (info.hold >> 6) & 1;
    case 14:
    case 15:
    case 23:
    case 31:
        return (info.hold >> 7) & 1;
        default:
            fn_802E317C("ControlExists: unhandled axis %d", b->code);
            return 0;
        }
    }
    return type != 0;
}

int fn_801A902C(void) {
    fn_801A7628();
    return 21;
}

void fn_801A9050(Binding* b, int i) {
    char unused[32] = {0};
    fn_801A9110(b, lbl_802F9588[i].name);
}

void fn_801A90A4(char* out, int len, Binding* b) {
    BindEntry* e = fn_801A9234(b);
    if (e) {
        strncpy(out, e->name, len);
    } else {
        out[0] = 0;
    }
    out[len - 1] = 0;
}

void fn_801A9110(Binding* b, const char* name) {
    int i;
    fn_801A7628();
    for (i = 0; i < lbl_8055E5D0; i++) {
        if (!strcasecmp(lbl_8055E5CC[i].name, name)) {
            fn_801A91B0(b, &lbl_8055E5CC[i]);
            return;
        }
    }
    b->type = 0;
}

void fn_801A91B0(Binding* b, BindEntry* e) {
    b->type = e->type;
    switch (b->type) {
    case 2:
        b->code = e->code;
        break;
    case 3:
        b->code = e->code;
        break;
    case 5:
        b->code = e->code;
        break;
    case 6:
        b->code = e->code;
        break;
    case 4:
        b->code = e->code;
        break;
    case 1:
        b->key = e->code;
        break;
    }
}

BindEntry* fn_801A9234(Binding* b) {
    BindEntry* e;
    int i;
    int n;
    fn_801A7628();
    n = lbl_8055E5D0;
    for (i = 0; i < n; i++) {
        e = &lbl_8055E5CC[i];
        if (b->type == e->type) {
            switch (b->type) {
            case 2:
                if (b->code == e->code) {
                    return &lbl_8055E5CC[i];
                }
                break;
            case 4:
                if (b->code == e->code) {
                    return &lbl_8055E5CC[i];
                }
                break;
            case 3:
                if (b->code == e->code) {
                    return &lbl_8055E5CC[i];
                }
                break;
            case 5:
                if (b->code == e->code) {
                    return &lbl_8055E5CC[i];
                }
                break;
            case 6:
                if (b->code == e->code) {
                    return &lbl_8055E5CC[i];
                }
                break;
            case 1:
                if (b->key == (u8)e->code) {
                    return &lbl_8055E5CC[i];
                }
                break;
            }
        }
    }
    return 0;
}
}

extern "C" {
char lbl_80334F84[0x5A4] = "ControlStrings:JoyAXPlus\0\0\0\0ControlStrings:JoyAYPlus\0\0\0\0ControlStrings:JoyBXPlus\0\0\0\0ControlStrings:JoyBYPlus\0\0\0\0ControlStrings:JoyAXMinus\0\0\0ControlStrings:JoyAYMinus\0\0\0ControlStrings:JoyBXMinus\0\0\0ControlStrings:JoyBYMinus\0\0\0\0\0\0\0ControlStrings:JoyALeft\0ControlStrings:JoyARight\0\0\0\0ControlStrings:JoyAUp\0\0\0\0\0\0\0ControlStrings:JoyADown\0ControlStrings:JoyBLeft\0ControlStrings:JoyBRight\0\0\0\0ControlStrings:JoyBUp\0\0\0\0\0\0\0ControlStrings:JoyBDown\0ControlStrings:Button1\0\0ControlStrings:Button2\0\0ControlStrings:Button3\0\0ControlStrings:Button4\0\0ControlStrings:Button5\0\0ControlStrings:Button6\0\0ControlStrings:Button7\0\0ControlStrings:Button8\0\0ControlStrings:Button9\0\0ControlStrings:Button10\0ControlStrings:Button11\0ControlStrings:Button12\0ControlStrings:Button13\0ControlStrings:Button14\0ControlStrings:Button15\0ControlStrings:Button16\0ControlStrings:POVLeft\0\0ControlStrings:POVRight\0ControlStrings:POVFront\0ControlStrings:POVBack\0\0ControlStrings:POVAny\0\0\0ControlStrings:MouseUp\0\0ControlStrings:MouseDown\0\0\0\0ControlStrings:MouseLeft\0\0\0\0ControlStrings:MouseRight\0\0\0ControlStrings:MouseButtonLeft\0\0ControlStrings:MouseButtonMiddle\0\0\0\0ControlStrings:MouseButtonRight\0ControlStrings:KeySHIFT\0ControlStrings:KeyCTRL\0\0ControlStrings:KeyRSHIFT\0\0\0\0\0\0\0\0ControlStrings:KeyRCTRL\0ControlStrings:KeyINS\0\0\0ControlStrings:KeyDEL\0\0\0ControlStrings:KeySPACE\0ControlStrings:KeyUP\0\0\0\0ControlStrings:KeyDOWN\0\0ControlStrings:KeyLEFT\0\0ControlStrings:KeyRIGHT\0ControlStrings:Unknown";
}
