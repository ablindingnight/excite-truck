// Name entry keyboard (0x80243478-0x802445FC): an on-screen keyboard menu state. The keys are a
// 5x16 table of labels; two-letter labels hold the lower and upper case. The edited string ends in a
// '_' cursor while the keyboard is open. Names are guesses apart from the strings.
#include "game/common.h"
#include "game/menustate.h"
#include <string.h>
#include "game/strcopy.h"

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
const char* fn_80131234(const char* key);  // localize
s32 fn_80232578(const char* name);         // load font
void fn_802328D4(s32 font);
void* gxGetImage(const char* name);  // load image
void fn_802C519C(void* img);
f32 CalculateTextWidth(s32 font, const char* text, f32 scale);
void fn_80230C30(f32* x, f32* y, int lx, int ly);
void fn_80230924(u32 color, f32 x0, f32 y0, f32 x1, f32 y1, f32 z);
void fn_80230F78(u8* flash, f32* t, u32* color, int a);
void DrawColoredText(const char* text, s32 font, f32 x, f32 y, u32 color, int align, f32 scale, f32 a, f32 b);
void fn_80230E90(int a);
f32 CalculateTimeDelta(void);  // seconds
void fn_80221E7C(void* state);
}

extern "C" double fmod(double x, double y);



struct KeyDef {
    const char* label;
    int width;
    int type;  // 1 character, 2 space, 3 caps, 4 backspace, 5 done
};

// DUMP_DATA lbl_803428B0 {cstr,s32,s32} group=16
KeyDef lbl_803428B0[5][16] = {
#include "dumpdata/lbl_803428B0.inc"
};

int lbl_8055BE0C = -1;  // the column remembered when moving between rows
int lbl_8055BE10 = -1;  // the column it was moved to

#include "game/namechooser.h"
#include "game/makeilpos.h"
#include "game/strcat.h"

ModuleTag lbl_8055F208;

static UnkStatic123 lbl_80498848;
extern const u32 lbl_8055F210 = MakeILPos(0x7F00, 0);
extern const u32 lbl_8055F214 = MakeILPos(0xFFFF, 0);



extern "C" {

int CountKeyboardEntries(int row) {
    KeyDef* k = lbl_803428B0[row];
    int n = 0;
    for (int i = 0; i < 16; i++) {
        if (k[i].label == NULL) {
            return n;
        }
        n++;
    }
    return n;
}

int CalculateKeyXPosition(int col, int row) {
    KeyDef* k = lbl_803428B0[row];
    int x = 0;
    for (int i = 0; i < col; i++) {
        x += k[i].width;
    }
    return x;
}

int FindKeyIndexFromXPosition(int x, int row) {
    KeyDef* k = lbl_803428B0[row];
    int start = 0;
    int last = 0;
    for (int i = 0; i < 16; i++) {
        int end = start + k[i].width;
        if (k[i].width > 0) {
            last = i;
        }
        if (x >= start && x < end) {
            return i;
        }
        start = end;
    }
    return last;
}

int fn_80243710(int col, int row) {
    int ok = 0;
    if (col >= 0 && col <= CountKeyboardEntries(row)) {
        ok = 1;
    }
    return ok;
}

int UpdateKeyboardSelection(int* col, int* row, int dx, int dy) {
    int oldCol = *col;
    int oldRow = *row;
    *row = oldRow + dy;
    if (*row < 0) {
        *row = 4;
    } else if (*row > 4) {
        *row = 0;
    }
    if (*row != oldRow) {
        int pos = CalculateKeyXPosition(*col, oldRow);
        pos = FindKeyIndexFromXPosition(pos, *row);
        if (CountKeyboardEntries(oldRow) != CountKeyboardEntries(*row)) {
            if (fn_80243710(lbl_8055BE0C, *row) && *col == lbl_8055BE10) {
                pos = lbl_8055BE0C;
                lbl_8055BE0C = lbl_8055BE10;
                lbl_8055BE10 = pos;
            } else {
                int c = *col;
                lbl_8055BE10 = pos;
                lbl_8055BE0C = c;
            }
        } else {
            lbl_8055BE10 = -1;
            lbl_8055BE0C = -1;
        }
        *col = pos;
    }
    *col += dx;
    if (*col < 0) {
        *col = CountKeyboardEntries(*row) - 1;
    } else if (*col > CountKeyboardEntries(*row) - 1) {
        *col = 0;
    }
    if (*col != lbl_8055BE10) {
        lbl_8055BE0C = -1;
        lbl_8055BE10 = -1;
    }
    if (oldCol != *col || oldRow != *row) {
        fn_80230E90(0);
        return 1;
    }
    return 0;
}

void RemoveTrailingUnderscore(char* s) {
    int n = strlen(s);
    if (n > 0 && s[n - 1] == '_') {
        s[n - 1] = 0;
    }
}

void AppendUnderscoreIfNeeded(char* s, int max) {
    int n = strlen(s);
    if ((n == 0 || s[n - 1] != '_') && n + 1 < max) {
        StrNCat(s, "_", max);
    }
}

}  // extern "C"

NameChooser::NameChooser(char* buf, int max) {
    this->buf = buf;
    this->max = max;
    this->max = (this->max > 11) ? 11 : this->max;
    unk34 = 0;
    col = 0;
    row = 0;
    caps = 1;
    done = 0;
}

void NameChooser::Enter(void) {
    font = fn_80232578("uifnt_o.gtx");
    box = gxGetImage("namebox.img");
    if (box == NULL) {
        fn_802E30C8("mname: Error getting selector box");
    }
    AppendUnderscoreIfNeeded(buf, max);
    for (int r = 0; r < 5; r++) {
        for (int i = 0; i < CountKeyboardEntries(r); i++) {
            KeyDef* k = &lbl_803428B0[r][i];
            switch (k->type) {
            case 3:
                if (k->label == NULL || strlen(k->label) < 1) {
                    k->label = fn_80131234("NameEntry:CAPS");
                }
                break;
            case 2:
                if (k->label == NULL || strlen(k->label) < 1) {
                    k->label = fn_80131234("NameEntry:SPACE");
                }
                break;
            case 5:
                if (k->label == NULL || strlen(k->label) < 1) {
                    k->label = fn_80131234("NameEntry:DONE");
                }
                break;
            }
        }
    }
}

void NameChooser::Exit(void) {
    RemoveTrailingUnderscore(buf);
    fn_802C519C(box);
    box = NULL;
    fn_802328D4(font);
    font = 0;
}

NameChooser::~NameChooser(void) {}

void NameChooser::Changed(int key, void* value) { fn_80221E7C(this); }

void NameChooser::Draw(void) {
    u32 sel;
    KeyDef* rowp;
    KeyDef* k;
    const char* label;
    int pass;
    int r;
    int c;
    f32 px;
    f32 py;
    fn_80230C30(&px, &py, x, y);
    for (pass = 0; pass < 2; pass++) {
        rowp = lbl_803428B0[0];
        for (r = 0; r < 5; r++, rowp += 16) {
            k = rowp;
            f32 ky;
            f32 kx;
            kx = px;
            ky = py - 38.0f * r;
            for (c = 0; c < 16; c++, k++) {
                sel = 0;
                if (r == row && c == col) {
                    sel = 1;
                }
                label = k->label;
                if (label != NULL) {
                    f32 scale = (k->type == 1) ? 0.6f : 0.45f;
                    u32 color = sel ? 0xE00000FF : 0xE0FFFFFF;
                    char text[32];
                    if (k->type == 1 && strlen(label) == 2) {
                        text[0] = label[caps];
                        text[1] = 0;
                    } else {
                        SafeStrCopy(text, label, 32);
                    }
                    u8 flash = 0;
                    f32 t = 0.0f;
                    u32 box;
                    if (sel) {
                        u32 c2 = 0;
                        fn_80230F78(&flash, &t, &c2, 0);
                        color = c2;
                        box = 0xFFC34C05;
                    } else if (k->type == 1) {
                        box = 0x50000000;
                    } else if ((k->type == 5 || k->type == 2) && Length() == 0) {
                        box = 0x78000000;
                        color = 0x28000000;
                    } else {
                        box = 0x50000078;
                    }
                    if (pass == 0 || (sel && flash)) {
                        f32 gx = 0.0f;
                        f32 gy = 0.0f;
                        if (pass == 1) {
                            gx = 32.0f * t;
                            gy = 38.0f * t;
                            box = (flash << 24) | (((box >> 16) & 0xFF) << 16) | (((box >> 8) & 0xFF) << 8) | (box & 0xFF);
                        }
                        fn_80230924(box, (1.0f + kx) - gx, gy + (ky - 1.0f), gx + ((kx + 32.0f * k->width) - 2.0f),
                                    (2.0f + (ky - 38.0f)) - gy, 0.0f);
                    }
                    if (sel == (pass == 1)) {
                        DrawColoredText(text, font, -3.0f + (kx + 0.5f * (32.0f * k->width)), ky - 19.0f, color, 0x14, scale, 0.0f, 1.0f);
                    }
                }
                kx += 32.0f * k->width;
            }
        }
    }
    u32 color;
    int n;
    int last;
    bool blink;
    int i;
    f32 nx = 50.0f + px;
    f32 ny = 37.0f + py;
    n = max - 1;
    int len = Length();
    last = len < n - 1 ? len : n - 1;
    blink = (f32)fmod(CalculateTimeDelta(), 0.6f) > 0.3f;
    for (i = 0; i < n; i++) {
        int cursor = 0;
        if (blink && i == last) {
            cursor = 1;
        }
        color = cursor ? 0xFF000000 : 0xFFFFFFFF;
        char ch[2];
        *(u16*)ch = 0;
        ch[0] = buf[i];
        if (cursor) {
            fn_80230924(0x80FFFF00, 2.0f + (nx + 23.0f * i), (17.0f + ny) - 2.0f, 4.0f + ((nx + 23.0f * (i + 1)) - 3.0f),
                        ny - 17.0f, 0.0f);
        }
        DrawColoredText(i < Length() ? ch : "_", font, nx + 23.0f * (0.5f + i), ny, color, 0x14, 0.57f, 0.0f, 1.0f);
    }
}

void NameChooser::Accept(void) {
    CalculateTextWidth(font, "WWWWWWWW", 1.0f);
    KeyDef* k = &lbl_803428B0[row][col];
    switch (k->type) {
    case 4:
        Backspace();
        break;
    case 1:
    case 2:
        if (Length() >= max - 1) {
            Backspace();
        }
        if (Length() + 1 < max) {
            RemoveTrailingUnderscore(buf);
            char text[32] = {0};
            if (k->type == 2) {
                if (buf[0]) {
                    SafeStrCopy(text, " ", 32);
                } else {
                    text[0] = 0;
                }
            } else {
                const char* label = k->label;
                if (strlen(label) == 2) {
                    text[1] = 0;
                    text[0] = label[caps];
                } else {
                    SafeStrCopy(text, label, 32);
                }
            }
            StrNCat(buf, text, max);
            AppendUnderscoreIfNeeded(buf, max);
            if (max == Length() + 1) {
                for (int r = 0; r < 5; r++) {
                    for (int c = 0; c < 16; c++) {
                        if (lbl_803428B0[r][c].type == 5) {
                            row = r;
                            col = c;
                            break;
                        }
                    }
                }
            }
        }
        break;
    case 3:
        Caps();
        break;
    case 5:
        if (strlen(buf) > 1) {
            done = 1;
            RemoveTrailingUnderscore(buf);
        }
        break;
    }
}

void NameChooser::Backspace(void) {
    RemoveTrailingUnderscore(buf);
    char* s = buf;
    if (strlen(s) != 0) {
        int n = strlen(s);
        s[n - 1] = 0;
    }
    AppendUnderscoreIfNeeded(buf, max);
}

void NameChooser::Caps(void) { caps = !caps; }

int NameChooser::Key(int key) {
    switch (key) {
    case 0x125:
        UpdateKeyboardSelection(&col, &row, -1, 0);
        break;
    case 0x127:
        UpdateKeyboardSelection(&col, &row, 1, 0);
        break;
    case 0x126:
        UpdateKeyboardSelection(&col, &row, 0, -1);
        break;
    case 0x128:
        UpdateKeyboardSelection(&col, &row, 0, 1);
        break;
    case 0xD:
        Accept();
        break;
    case 0x1B:
        if (Length() > 0) {
            Backspace();
            break;
        }
        return 0;
    default:
        return 0;
    }
    return 1;
}

int NameChooser::IsDone(void) { return done; }

int NameChooser::Length(void) {
    int n = strlen(buf);
    if (n > 0 && buf[n - 1] == '_') {
        n--;
    }
    return n;
}
