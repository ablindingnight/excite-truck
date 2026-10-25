// Sound dashboard (0x8020BCE4-0x8020C140): a debug overlay listing sample volumes, with keys to pick
// and adjust them and write sounds.txt. Names are guesses apart from the strings.
#include "game/common.h"

struct SoundSample {
    char name[0x20];  // offset 0x0
    f32 volume;       // offset 0x20
};

extern "C" {
int sprintf(char* s, const char* fmt, ...);
void fn_802E30C8(const char* fmt, ...);  // debug print
s32 fn_802E3D68(const char* name);       // create
void fn_802E3AD0(s32 fd, const char* fmt, ...);  // file printf
void fn_802E3734(s32* fd);                       // close
int fn_8020A76C(void);
SoundSample* fn_8020A774(int i);
void fn_802E6B38(int i, char* buf);
void fn_8023895C(void);
void fn_802389E4(void);
void fn_80238A74(int x, int y, const char* text, u32 color);
void fn_80238B44(int x0, int y0, int x1, int y1, u32 color);
}

static inline void StrCopyN(char* dst, const char* src, int len) {
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

static inline int Half(int x) { return x / 2; }

ModuleTag lbl_8055EE18;
UnkStatic7F lbl_8055EE20;
s32 lbl_8055EE24;  // page
s32 lbl_8055EE28;  // selection

extern "C" {

void fn_8020BCE4(void) {
    s32 fd = fn_802E3D68("sounds.txt");
    if (fd != -1) {
        int i;
        fn_802E3AD0(fd, "tabular samples\r\n");
        fn_802E3AD0(fd, "\tname\t\t\tvolume\r\n");
        fn_802E3AD0(fd, ";\t----\t\t\t------\r\n");
        for (i = 0; i < fn_8020A76C(); i++) {
            SoundSample* s = fn_8020A774(i);
            fn_802E3AD0(fd, "\t%s\t\t%1.2f\r\n", s->name, s->volume);
        }
        fn_802E3AD0(fd, "end\r\n\r\nend\r\n");
        fn_802E3734(&fd);
    } else {
        fn_802E30C8("can't write to sounds.txt");
    }
}

int fn_8020BDB8(int key) {
    switch (key) {
    case 'm':
        lbl_8055EE24 = !lbl_8055EE24;
        return 1;
    case 0x128:
        if (!lbl_8055EE24) {
            int last = fn_8020A76C() - 1;
            lbl_8055EE28 = last < lbl_8055EE28 + 1 ? last : lbl_8055EE28 + 1;
            return 1;
        }
        break;
    case 0x126:
        if (!lbl_8055EE24) {
            int n = lbl_8055EE28 - 1;
            lbl_8055EE28 = n < 0 ? 0 : n;
            return 1;
        }
        break;
    case 0x125:
        if (!lbl_8055EE24) {
            int sel = lbl_8055EE28;
            if (sel < fn_8020A76C()) {
                SoundSample* s = fn_8020A774(sel);
                f32 v = s->volume - 0.01f;
                s->volume = v > 0.0f ? v : 0.0f;
                return 1;
            }
        }
        break;
    case 0x127:
        if (!lbl_8055EE24) {
            int sel = lbl_8055EE28;
            if (sel < fn_8020A76C()) {
                SoundSample* s = fn_8020A774(sel);
                f32 v = 0.01f + s->volume;
                s->volume = v < 2.0f ? v : 2.0f;
                return 1;
            }
        }
        break;
    case 'S':
        if (!lbl_8055EE24) {
            fn_802E30C8("writing sounds.txt...");
            fn_8020BCE4();
            return 1;
        }
        break;
    }
    return 0;
}

// NonMatching: the target recomputes `i / 2` for the row after the sample call (our build CSEs it).
void fn_8020BF58(void) {
    fn_8023895C();
    fn_80238A74(12, 12, "SOUND DASH", 0xFF2D2DC8);
    if (lbl_8055EE24 == 1) {
        int i;
        fn_80238B44(7, 7, 350, 448, 0x8C000000);
        for (i = 0; i < 32; i++) {
            char buf[0x110];
            int y;
            int x;
            fn_802E6B38(i, buf);
            {
                int col = i / 16;
                int row = i % 16;
                fn_80238A74(col ? 290 : 20, row * 14 + 26, buf, 0xFFFFFF00);
            }
        }
    } else if (lbl_8055EE24 == 0) {
        int i;
        int n;
        fn_80238B44(7, 7, 300, 448, 0x8C000000);
        n = fn_8020A76C() * 2;
        for (i = 0; i < n; i++) {
            char buf[64];
            SoundSample* s = fn_8020A774(i / 2);
            int col = i % 2;
            int row = i / 2;
            u32 color;
            if (col == 0) {
                StrCopyN(buf, s->name, sizeof(buf));
            } else {
                sprintf(buf, "%1.2f", s->volume);
            }
            color = 0xFFFFFF00;
            if (row == lbl_8055EE28) {
                color = 0xFF2D2DC8;
            }
            fn_80238A74(col ? 240 : 20, row * 14 + 26, buf, color);
        }
    }
    fn_802389E4();
}

}  // extern "C"
