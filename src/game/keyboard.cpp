// Name entry keyboard (0x80243478-0x802445FC): an on-screen keyboard menu state. The keys are a
// 5x16 table of labels; two-letter labels hold the lower and upper case. The edited string ends in a
// '_' cursor while the keyboard is open. Names are guesses apart from the strings.
#include "game/common.h"
#include <string.h>

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
const char* fn_80131234(const char* key);  // localize
s32 fn_80232578(const char* name);         // load font
void fn_802328D4(s32 font);
void* fn_802C5130(const char* name);  // load image
void fn_802C519C(void* img);
f32 fn_8023293C(s32 font, const char* text, f32 scale);
void fn_80230C30(f32* x, f32* y, int lx, int ly);
void fn_80230924(u32 color, f32 x0, f32 y0, f32 x1, f32 y1, f32 z);
void fn_80230F78(u8* flash, f32* t, u32* color, int a);
void fn_80232BA4(const char* text, s32 font, u32 color, int align, f32 x, f32 y, f32 scale, f32 a, f32 b);
void fn_80230E90(int a);
f32 fn_8023659C(void);  // seconds
void fn_80221E7C(void* state);
}

double fmod(double x, double y);

class MenuState {
public:
    MenuState(void) { unk14 = 0; }
    virtual ~MenuState(void);
    virtual void Enter(void);   // 0x0C
    virtual void Exit(void);    // 0x10
    virtual void Unk14(void);
    virtual void Unk18(void);
    virtual void Update(void);  // 0x1C
    virtual void Unk20(void);
    virtual void Unk24(void);
    virtual void Draw(void);    // 0x28
    virtual void Unk2C(void);
    virtual void Unk30(void);
    virtual void Unk34(void);
    virtual void Unk38(void);
    virtual void Unk3C(void);
    virtual void Unk40(void);
    virtual void Unk44(void);
    virtual void Unk48(void);
    virtual void Unk4C(void);
    virtual void Unk50(void);
    virtual void Unk54(void);
    virtual int Key(int key);   // 0x58
    virtual void Unk5C(void);

    s32 x;      // offset 0x4
    s32 y;      // offset 0x8
    s32 w;      // offset 0xC
    s32 h;      // offset 0x10
    int unk14;  // offset 0x14
};

inline MenuState::~MenuState(void) {}

struct KeyDef {
    const char* label;
    int width;
    int type;  // 1 character, 2 space, 3 caps, 4 backspace, 5 done
};

KeyDef lbl_803428B0[5][16] = {
    {{"1", 1, 1}, {"2", 1, 1}, {"3", 1, 1}, {"4", 1, 1}, {"5", 1, 1}, {"6", 1, 1}, {"7", 1, 1}, {"8", 1, 1},
     {"9", 1, 1}, {"0", 1, 1}},
    {{"aA", 1, 1}, {"bB", 1, 1}, {"cC", 1, 1}, {"dD", 1, 1}, {"eE", 1, 1}, {"fF", 1, 1}, {"gG", 1, 1}, {"hH", 1, 1},
     {"iI", 1, 1}, {"jJ", 1, 1}},
    {{"kK", 1, 1}, {"lL", 1, 1}, {"mM", 1, 1}, {"nN", 1, 1}, {"oO", 1, 1}, {"pP", 1, 1}, {"qQ", 1, 1}, {"rR", 1, 1},
     {"sS", 1, 1}, {"tT", 1, 1}},
    {{"uU", 1, 1}, {"vV", 1, 1}, {"wW", 1, 1}, {"xX", 1, 1}, {"yY", 1, 1}, {"zZ", 1, 1}, {",", 1, 1}, {".", 1, 1},
     {"!", 1, 1}, {"?", 1, 1}},
    {{"", 2, 3}, {"", 4, 2}, {"", 4, 5}},
};

int lbl_8055BE0C = -1;  // the column remembered when moving between rows
int lbl_8055BE10 = -1;  // the column it was moved to

class NameChooser : public MenuState {
public:
    NameChooser(char* buf, int max);
    virtual ~NameChooser(void);
    virtual void Enter(void);
    virtual void Exit(void);
    virtual void Unk18(void);
    virtual void Draw(void);
    virtual int Key(int key);
    virtual void Accept(void);     // 0x60
    virtual void Backspace(void);  // 0x64
    virtual void Caps(void);       // 0x68
    int IsDone(void);
    int Length(void);

    char* buf;    // offset 0x18
    int max;      // offset 0x1C
    s32 font;     // offset 0x20
    void* box;    // offset 0x24
    u8 done;      // offset 0x28
    bool caps;    // offset 0x29
    int row;      // offset 0x2C
    int col;      // offset 0x30
    int unk34;    // offset 0x34
};

ModuleTag lbl_8055F208;
static UnkStatic123 lbl_80498848;
UnkStatic7F lbl_8055F210;
UnkStaticFFFF lbl_8055F214;

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

static inline void StrNCat(char* d, const char* s, int n) {
    while (*d) {
        n--;
        d++;
    }
    if (n > 0) {
        StrNCopy(d, s, n);
    }
}

extern "C" {

int fn_80243478(int row) {
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

int fn_80243528(int col, int row) {
    KeyDef* k = lbl_803428B0[row];
    int x = 0;
    for (int i = 0; i < col; i++) {
        x += k[i].width;
    }
    return x;
}

int fn_8024360C(int x, int row) {
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
    if (col >= 0 && col <= fn_80243478(row)) {
        ok = 1;
    }
    return ok;
}

int fn_80243764(int* col, int* row, int dx, int dy) {
    int oldCol = *col;
    int oldRow = *row;
    *row = oldRow + dy;
    if (*row < 0) {
        *row = 4;
    } else if (*row > 4) {
        *row = 0;
    }
    if (*row != oldRow) {
        int pos = fn_80243528(*col, oldRow);
        pos = fn_8024360C(pos, *row);
        if (fn_80243478(oldRow) != fn_80243478(*row)) {
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
        *col = fn_80243478(*row) - 1;
    } else if (*col > fn_80243478(*row) - 1) {
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

void fn_802438EC(char* s) {
    int n = strlen(s);
    if (n > 0 && s[n - 1] == '_') {
        s[n - 1] = 0;
    }
}

void fn_80243938(char* s, int max) {
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
    box = fn_802C5130("namebox.img");
    if (box == NULL) {
        fn_802E30C8("mname: Error getting selector box");
    }
    fn_80243938(buf, max);
    for (int r = 0; r < 5; r++) {
        for (int i = 0; i < fn_80243478(r); i++) {
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
    fn_802438EC(buf);
    fn_802C519C(box);
    box = NULL;
    fn_802328D4(font);
    font = 0;
}

NameChooser::~NameChooser(void) {}

void NameChooser::Unk18(void) { fn_80221E7C(this); }

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
                        StrNCopy(text, label, 32);
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
                        fn_80232BA4(text, font, color, 0x14, -3.0f + (kx + 0.5f * (32.0f * k->width)), ky - 19.0f,
                                    scale, 0.0f, 1.0f);
                    }
                }
                kx += 32.0f * k->width;
            }
        }
    }
    u32 color;
    int n;
    int last;
    int blink;
    int i;
    f32 nx = 50.0f + px;
    f32 ny = 37.0f + py;
    n = max - 1;
    int len = Length();
    last = n - 1;
    if (len < last) {
        last = len;
    }
    blink = (f32)fmod(fn_8023659C(), 0.6f) > 0.3f;
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
        fn_80232BA4(i < Length() ? ch : "_", font, color, 0x14, nx + 23.0f * (0.5f + i), ny, 0.57f, 0.0f, 1.0f);
    }
}

void NameChooser::Accept(void) {
    fn_8023293C(font, "WWWWWWWW", 1.0f);
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
            fn_802438EC(buf);
            char text[32] = {0};
            if (k->type == 2) {
                if (buf[0]) {
                    StrNCopy(text, " ", 32);
                } else {
                    text[0] = 0;
                }
            } else {
                const char* label = k->label;
                if (strlen(label) == 2) {
                    text[1] = 0;
                    text[0] = label[caps];
                } else {
                    StrNCopy(text, label, 32);
                }
            }
            StrNCat(buf, text, max);
            fn_80243938(buf, max);
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
            fn_802438EC(buf);
        }
        break;
    }
}

void NameChooser::Backspace(void) {
    fn_802438EC(buf);
    char* s = buf;
    if (strlen(s) != 0) {
        int n = strlen(s);
        s[n - 1] = 0;
    }
    fn_80243938(buf, max);
}

void NameChooser::Caps(void) { caps = !caps; }

int NameChooser::Key(int key) {
    switch (key) {
    case 0x125:
        fn_80243764(&col, &row, -1, 0);
        break;
    case 0x127:
        fn_80243764(&col, &row, 1, 0);
        break;
    case 0x126:
        fn_80243764(&col, &row, 0, -1);
        break;
    case 0x128:
        fn_80243764(&col, &row, 0, 1);
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
