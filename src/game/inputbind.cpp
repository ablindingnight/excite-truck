// Input bindings (0x801A75B4-0x801A9660): named controls ("Joystick Up", "Key_SPACE", ...) for the
// pads, keyboard and mouse, their current and previous-frame values, edge detection, reading and
// writing them as option strings and finding the control the player is pressing. Names are guesses
// apart from the strings.
#include "game/common.h"
#include "game/binding.h"
#include "game/locstring.h"
#include "game/joy.h"
#include "game/strcopy.h"

extern "C" {
char* strncpy(char* d, const char* s, u32 n);
int strcasecmp(const char* a, const char* b);
void* memset(void* p, int c, u32 n);
}

// JoyInfo: game/joy.h.
// JoyStatus: game/joy.h.
struct Mouse {
    f32 x;  // offset 0x0
    f32 y;  // offset 0x4
    u8 b0;  // offset 0x8
    u8 b1;  // offset 0x9
    u8 b2;  // offset 0xA
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
void LoadOptionString(int section, const char* key, char* out, int len);
void SetOptionString(int section, const char* key, const char* s);
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

// DUMP_DATA lbl_802F9588 {cstrptr,cstrptr}
extern const NamePair lbl_802F9588[21] = {
#include "dumpdata/lbl_802F9588.inc"
};

// DUMP_DATA lbl_803342D0 {cstrptr,cstrptr,s32,s32,u8,u8[3],u8,u8[3]}
BindEntry lbl_803342D0[0x7A] = {
#include "dumpdata/lbl_803342D0.inc"
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

static LocString lbl_80435388("ControlStrings:JoyAXPlus");
static LocString lbl_80435394("ControlStrings:JoyAYPlus");
static LocString lbl_804353A0("ControlStrings:JoyBXPlus");
static LocString lbl_804353AC("ControlStrings:JoyBYPlus");
static LocString lbl_804353B8("ControlStrings:JoyAXMinus");
static LocString lbl_804353C4("ControlStrings:JoyAYMinus");
static LocString lbl_804353D0("ControlStrings:JoyBXMinus");
static LocString lbl_804353DC("ControlStrings:JoyBYMinus");
static LocString lbl_804353E8("ControlStrings:JoyALeft");
static LocString lbl_804353F4("ControlStrings:JoyARight");
static LocString lbl_80435400("ControlStrings:JoyAUp");
static LocString lbl_8043540C("ControlStrings:JoyADown");
static LocString lbl_80435418("ControlStrings:JoyBLeft");
static LocString lbl_80435424("ControlStrings:JoyBRight");
static LocString lbl_80435430("ControlStrings:JoyBUp");
static LocString lbl_8043543C("ControlStrings:JoyBDown");
static LocString lbl_80435448("ControlStrings:Button1");
static LocString lbl_80435454("ControlStrings:Button2");
static LocString lbl_80435460("ControlStrings:Button3");
static LocString lbl_8043546C("ControlStrings:Button4");
static LocString lbl_80435478("ControlStrings:Button5");
static LocString lbl_80435484("ControlStrings:Button6");
static LocString lbl_80435490("ControlStrings:Button7");
static LocString lbl_8043549C("ControlStrings:Button8");
static LocString lbl_804354A8("ControlStrings:Button9");
static LocString lbl_804354B4("ControlStrings:Button10");
static LocString lbl_804354C0("ControlStrings:Button11");
static LocString lbl_804354CC("ControlStrings:Button12");
static LocString lbl_804354D8("ControlStrings:Button13");
static LocString lbl_804354E4("ControlStrings:Button14");
static LocString lbl_804354F0("ControlStrings:Button15");
static LocString lbl_804354FC("ControlStrings:Button16");
static LocString lbl_80435508("ControlStrings:POVLeft");
static LocString lbl_80435514("ControlStrings:POVRight");
static LocString lbl_80435520("ControlStrings:POVFront");
static LocString lbl_8043552C("ControlStrings:POVBack");
static LocString lbl_80435538("ControlStrings:POVAny");
static LocString lbl_80435544("ControlStrings:MouseUp");
static LocString lbl_80435550("ControlStrings:MouseDown");
static LocString lbl_8043555C("ControlStrings:MouseLeft");
static LocString lbl_80435568("ControlStrings:MouseRight");
static LocString lbl_80435574("ControlStrings:MouseButtonLeft");
static LocString lbl_80435580("ControlStrings:MouseButtonMiddle");
static LocString lbl_8043558C("ControlStrings:MouseButtonRight");
static LocString lbl_80435598("ControlStrings:KeySHIFT");
static LocString lbl_804355A4("ControlStrings:KeyCTRL");
static LocString lbl_804355B0("ControlStrings:KeyRSHIFT");
static LocString lbl_804355BC("ControlStrings:KeyRCTRL");
static LocString lbl_804355C8("ControlStrings:KeyINS");
static LocString lbl_804355D4("ControlStrings:KeyDEL");
static LocString lbl_804355E0("ControlStrings:KeySPACE");
static LocString lbl_804355EC("ControlStrings:KeyUP");
static LocString lbl_804355F8("ControlStrings:KeyDOWN");
static LocString lbl_80435604("ControlStrings:KeyLEFT");
static LocString lbl_80435610("ControlStrings:KeyRIGHT");
static LocString lbl_8043561C("ControlStrings:Unknown\0");

static inline f32 MaxZero(f32 v) { return 0.0f > v ? 0.0f : v; }


extern "C" {

void InitializeInputBindings(void);
f32 GetJoystickAxisValue(const Binding* b);
f32 fn_801A85E8(const Binding* b);
int fn_801A8A10(const Binding* b);
int fn_801A8E38(const Binding* b);
int fn_801A8EE4(Binding* b);
int ControlExists(const Binding* b);
void fn_801A90A4(char* out, int len, Binding* b);
void BindInputToDevice(Binding* b, const char* name);
void CopyBindingEntry(Binding* b, BindEntry* e);
BindEntry* FindMatchingBindingEntry(Binding* b);

const NamePair* FindInputBindingByName(const char* name) {
    const NamePair* p = lbl_802F9588;
    int i;
    for (i = 0; i < 21; i++, p++) {
        if (!strcasecmp(p->name, name)) {
            return p;
        }
    }
    return 0;
}

void InitializeInputBindings(void) {
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

void InitializeJoyStatus(int reset) {
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

void SaveInputBinding(Binding* b, const char* key, int section) {
    char buf[32] = {0};
    fn_801A90A4(buf, sizeof(buf), b);
    LoadOptionString(section, key, buf, sizeof(buf));
    BindInputToDevice(b, buf);
}

void fn_801A7A08(Binding* b, const char* key, int section) {
    char buf[32];
    fn_801A90A4(buf, sizeof(buf), b);
    SetOptionString(section, key, buf);
}

void fn_801A7A5C(void) {
    int i;
    lbl_80435370.x = 0.0f;
    lbl_80435370.y = 0.0f;
    lbl_80435370.b2 = 0;
    lbl_80435370.b1 = 0;
    lbl_80435370.b0 = 0;
    InitializeJoyStatus(1);
    InitializeJoyStatus(1);
    for (i = 0; i < 4; i++) {
        lbl_804351E0[i] = lbl_80435050[i];
    }
    lbl_8043537C = lbl_80435370;
}

int FindValidInputBinding(Binding* out) {
    int i;
    InitializeInputBindings();
    for (i = 0; i < lbl_8055E5D0; i++) {
        if (!lbl_8055E5CC[i].used) {
            Binding b;
            b.pad = 0;
            b.latched = 0;
            CopyBindingEntry(&b, &lbl_8055E5CC[i]);
            if (b.type != 5 && b.type != 6) {
                if (fn_801A8EE4(&b)) {
                    if (fn_801A8E38(&b) && !fn_801A8A10(&b)) {
                        *out = b;
                        return 1;
                    }
                } else {
                    f32 v = GetJoystickAxisValue(&b);
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

void BindInputKey(const char* key, const char* name, int section) {
    Binding b;
    b.pad = 0;
    b.latched = 0;
    BindInputToDevice(&b, name);
    fn_801A7A08(&b, key, section);
}

void GetBindingText(char* out, int len, Binding* b) {
    BindEntry* e = FindMatchingBindingEntry(b);
    if (e) {
        const NamePair* p = FindInputBindingByName(e->name);
        if (p) {
            SafeStrCopy(out, p->text, len);
        } else {
            SafeStrCopy(out, e->label ? e->label : (const char*)&e->key, len);
        }
    } else {
        SafeStrCopy(out, lbl_8055A850, len);
    }
    out[len - 1] = 0;
}

void fn_801A7F8C(void) { lbl_8055E5C8 = 0; }

f32 GetJoystickAxisValue(const Binding* b) {
    JoyStatus* p;
    if (!ControlExists(b)) {
        return 0.0f;
    }
    p = &lbl_80435050[b->pad];
    switch (b->type) {
    case 2:
        switch (b->code) {
        case 0:
            return MaxZero(-p->x);
        case 1:
            return MaxZero(p->x);
        case 2:
            return MaxZero(-p->y);
        case 3:
            return MaxZero(p->y);
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
            return 0.5f * (1.0f - p->x);
        case 17:
            return 0.5f * (1.0f - p->y);
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
            return 0.5f * (1.0f + p->x);
        case 25:
            return 0.5f * (1.0f + p->y);
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
    }

    int pad = b->pad;
    JoyStatus saved = lbl_80435050[pad];
    Mouse m = lbl_80435370;
    lbl_80435050[pad] = lbl_804351E0[pad];
    lbl_80435370 = lbl_8043537C;
    f32 result = GetJoystickAxisValue(b);
    lbl_80435050[pad] = saved;
    lbl_80435370 = m;
    return result;
}

int fn_801A8A10(const Binding* b) {
    if (b->type == 1) {
        return 0;
    }

    int pad = b->pad;
    JoyStatus saved = lbl_80435050[pad];
    Mouse m = lbl_80435370;
    lbl_80435050[pad] = lbl_804351E0[pad];
    lbl_80435370 = lbl_8043537C;
    int result = fn_801A8E38(b);
    lbl_80435050[pad] = saved;
    lbl_80435370 = m;
    return result;
}

int fn_801A8E38(const Binding* b) { return GetJoystickAxisValue(b) > 0.2f; }

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

int fn_801A902C(void*) {
    InitializeInputBindings();
    return 21;
}

void fn_801A9050(Binding* b, int i) {
    char unused[32] = {0};
    BindInputToDevice(b, lbl_802F9588[i].name);
}

void fn_801A90A4(char* out, int len, Binding* b) {
    BindEntry* e = FindMatchingBindingEntry(b);
    if (e) {
        strncpy(out, e->name, len);
    } else {
        out[0] = 0;
    }
    out[len - 1] = 0;
}

void BindInputToDevice(Binding* b, const char* name) {
    int i;
    InitializeInputBindings();
    for (i = 0; i < lbl_8055E5D0; i++) {
        if (!strcasecmp(lbl_8055E5CC[i].name, name)) {
            CopyBindingEntry(b, &lbl_8055E5CC[i]);
            return;
        }
    }
    b->type = 0;
}

void CopyBindingEntry(Binding* b, BindEntry* e) {
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

BindEntry* FindMatchingBindingEntry(Binding* b) {
    BindEntry* e;
    int i;
    int n;
    InitializeInputBindings();
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
