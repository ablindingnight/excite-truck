// Joystick input (0x80129400-0x80129BD4): polls the four pads as the "JoyUpdate" background task and
// turns button presses and stick directions into key events through two key maps, with key repeat.
// Names are guesses.
#include "game/common.h"

struct JoyStatus {
    f32 x;            // offset 0x0
    f32 y;            // offset 0x4
    f32 unk8;
    f32 unkC;
    f32 unk10;
    f32 unk14;
    f32 unk18;
    f32 unk1C;
    u8 buttons[32];   // offset 0x20
    u32 dir;          // offset 0x40
    s32 vals[6];      // offset 0x44
    u32 unk5C;        // offset 0x5C
    u8 connected;     // offset 0x60
};

struct JoyPad {
    u32 lastTime;       // offset 0x0
    s32 repeat;         // offset 0x4
    u8 changed;         // offset 0x8
    u8 prev[14];        // offset 0x9
    u32 prevDir;        // offset 0x18
    JoyStatus status;   // offset 0x1C
};

struct KeyMap {
    u8 enabled;     // offset 0x0
    u8 stickDirs;   // offset 0x1
    u8 ownerOnly;   // offset 0x2
    u16 keys[14];   // offset 0x4
};

extern "C" {
void* memset(void* p, int c, size_t n);
void fn_802E7FD0(void);
void fn_802E8364(JoyStatus* status, int chan);
int fn_802E8504(int chan);
void fn_802E4800(int key);
u32 fn_802E2C20(void);
void fn_802E7C64(void);
void fn_802E7D0C(void);
void fn_802E830C(s32* map);
void fn_80127C90(void (*fn)(void), const char* name);
void fn_80127D58(void (*fn)(void));
}

ModuleTag lbl_8055DFC8;
JoyPad lbl_80422180[4];
KeyMap lbl_80422380;  // menu keys
KeyMap lbl_804223A0;  // game keys
s32 lbl_804223C0[14];  // key slot -> button index
u8 lbl_8055DFD0;      // stick arrows enabled
s32 lbl_8055DFD4;     // owning pad (-1: any)

extern "C" {

void fn_80129400(void) {
    int i;
    fn_802E7FD0();
    for (i = 0; i < 4; i++) {
        fn_802E8364(&lbl_80422180[i].status, i);
    }
}

void fn_80129458(void) {
    lbl_80422380.enabled = 1;
    lbl_804223A0.enabled = 1;
    lbl_80422380.keys[1] = 'X';
    lbl_80422380.keys[0] = 'T';
    lbl_80422380.ownerOnly = 0;
    lbl_80422380.keys[4] = 0x1B;
    lbl_80422380.keys[2] = 'C';
    lbl_80422380.keys[3] = 'S';
    lbl_804223A0.ownerOnly = 1;
    lbl_804223A0.stickDirs = 0;
    lbl_804223A0.keys[1] = 0xD;
    lbl_804223A0.keys[4] = 0x113;
    lbl_804223A0.keys[0] = ' ';
    lbl_804223A0.keys[2] = 'o';
    lbl_804223A0.keys[3] = 0x1B;
    lbl_804223A0.keys[12] = 0x121;
    lbl_804223A0.keys[5] = 0;
    lbl_804223A0.keys[7] = 0;
    lbl_804223A0.keys[6] = 0;
    lbl_804223A0.keys[8] = 0;
}

void fn_801294FC(int key, int chan) { fn_802E4800(key | (chan & 0xF) << 12); }

void fn_80129508(JoyPad* p, int chan, KeyMap* m) {
    int key = 0;
    if (m->enabled) {
        u32 dir = p->status.dir;
        if (dir == 0 && m->stickDirs && lbl_8055DFD0) {
            if (p->status.x < -0.5f) {
                dir |= 8;
            } else if (p->status.x > 0.5f) {
                dir |= 2;
            }
            if (p->status.y < -0.5f) {
                dir |= 1;
            } else if (p->status.y > 0.5f) {
                dir |= 4;
            }
        }
        switch (dir & (dir ^ p->prevDir)) {
        case 1:
            key = 0x126;
            break;
        case 4:
            key = 0x128;
            break;
        case 8:
            key = 0x125;
            break;
        case 2:
            key = 0x127;
            break;
        }
        p->prevDir = dir;
        if (dir) {
            p->changed = 1;
        }
    }
    if (key) {
        fn_801294FC(key, chan);
    }
}

void fn_8012960C(JoyPad* p) {
    u32 now = fn_802E2C20();
    if (p->changed) {
        s32 delay = p->repeat ? 125 : 250;
        if ((s32)(now - p->lastTime) > delay) {
            p->lastTime = now;
            p->prevDir = 0;
            p->repeat++;
        }
    } else {
        p->lastTime = now;
        p->repeat = 0;
    }
}

void fn_80129690(JoyPad* p, int chan, KeyMap* m) {
    int active = 0;
    s32 idx;
    u16 key;
    u8 b;
    int i;
    if (!m->ownerOnly || lbl_8055DFD4 == -1 || lbl_8055DFD4 == chan) {
        active = 1;
    }
    p->changed = 0;
    if (active) {
        fn_80129508(p, chan, m);
    }
    for (i = 0; i < 14; i++) {
        idx = lbl_804223C0[i];
        if (idx >= 0 && (key = m->keys[i]) != 0 && active) {
            b = p->status.buttons[idx];
            if (b) {
                p->changed = 1;
                if (!p->prev[i]) {
                    fn_801294FC(key, chan);
                }
            }
            p->prev[i] = b;
        }
    }
    fn_8012960C(p);
}

void fn_80129790(void) {
    fn_802E7C64();
    fn_80127C90(fn_80129400, "JoyUpdate");
    fn_80129458();
    lbl_8055DFD0 = 1;
    memset(lbl_804223C0, -1, sizeof(lbl_804223C0));
    fn_802E830C(lbl_804223C0);
}

s32* fn_801297F4(void) { return lbl_804223C0; }

int fn_801298E8(JoyStatus* out, int chan);

u8 fn_80129800(int chan, int button) {
    JoyStatus s;
    if (fn_801298E8(&s, chan) && lbl_804223C0[button] >= 0) {
        return s.buttons[lbl_804223C0[button]];
    }
    return 0;
}

void fn_80129864(void) {
    fn_80127D58(fn_80129400);
    fn_802E7D0C();
}

int fn_80129890(void) {
    int n = 0;
    int i;
    for (i = 0; i < 4; i++) {
        if (fn_802E8504(i)) {
            n++;
        }
    }
    return n;
}

int fn_801298E8(JoyStatus* out, int chan) {
    int ok = fn_802E8504(chan);
    if (ok) {
        *out = lbl_80422180[chan].status;
    } else {
        memset(out, 0, sizeof(JoyStatus));
    }
    return ok;
}

int fn_80129AE8(JoyStatus* out, int chan) { return fn_801298E8(out, chan); }

void fn_80129AEC(s32 chan) { lbl_8055DFD4 = chan; }

s32 fn_80129AF4(void) { return lbl_8055DFD4; }

void fn_80129AFC(void) {
    int i;
    for (i = 0; i < 4; i++) {
        fn_80129690(&lbl_80422180[i], i, &lbl_804223A0);
    }
}

void fn_80129B60(void) {
    int i;
    for (i = 0; i < 4; i++) {
        fn_80129690(&lbl_80422180[i], i, &lbl_80422380);
    }
}

int fn_80129BC4(void) { return 1; }

}  // extern "C"
