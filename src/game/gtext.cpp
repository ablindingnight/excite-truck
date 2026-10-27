// gText (0x80231504-0x80234D44): bitmap fonts ("GTEX" files, byte-swapped when built big-endian), a
// font tracker with reference counts, UTF-8 string drawing and measuring, escape sequences that draw
// gArt pictures inline ("esc_seq.val"), outlined text, word-wrapped and vertical text, and a buffer of
// text drawn later in-game. Names are guesses apart from the strings.
#include "game/common.h"
#include <stdarg.h>
#include <string.h>

struct gArtSet;

struct Glyph {
    f32 pre;    // offset 0x0
    f32 width;  // offset 0x4
    f32 post;   // offset 0x8
    f32 u0;     // offset 0xC
    f32 v0;     // offset 0x10
    f32 u1;     // offset 0x14
    f32 v1;     // offset 0x18
};

struct Font {
    s32 tex;         // offset 0x0
    u8 height;       // offset 0x4
    u8 b5;           // offset 0x5
    u8 b6;           // offset 0x6
    Font* outline;   // offset 0x8
    u16 count;       // offset 0xC
    Glyph* glyphs;   // offset 0x10
    u32 unk14;       // offset 0x14
    char* texName;   // offset 0x18
};

struct FontEntry {
    Font* font;  // offset 0x0
    char* name;  // offset 0x4
    int refs;    // offset 0x8
};

struct FontTracker {
    FontEntry e[30];  // offset 0x0
    int count;        // offset 0x168
};

struct EscSeq {
    const char* name;  // offset 0x0
    f32 scale;         // offset 0x4
    s32 tex;           // offset 0x8
    gArtSet* art;      // offset 0xC
    int artIdx;        // offset 0x10
    u8 takeColor;      // offset 0x14
    f32 postspace;     // offset 0x18
};

struct ArtItem {
    int esc;    // offset 0x0
    f32 x;      // offset 0x4
    f32 y;      // offset 0x8
    f32 z;      // offset 0xC
    f32 h;      // offset 0x10
    f32 w;      // offset 0x14
    u32 color;  // offset 0x18
};

struct TextItem {
    char text[0x40];  // offset 0x0
    f32 x;            // offset 0x40
    f32 y;            // offset 0x44
    u32 color;        // offset 0x48
    u32 masked;       // offset 0x4C
    f32 scale;        // offset 0x50
    f32 xs;           // offset 0x54
};

struct IngameText {
    Font* font;  // offset 0x0
    int count;   // offset 0x4
};

struct VecP {
    f32 x, y, z;
};

struct UIXform {
    f32 m[3][3];  // offset 0x0
    f32 pos[3];   // offset 0x24
};

class UIContext {
public:
    UIXform xf;  // offset 0x0
    f32 alpha;   // offset 0x30
    f32 unk34;   // offset 0x34
    f32 scale;   // offset 0x38
};

struct UIVert {
    f32 data[6];
};

template <class T>
class ArrayBase {
public:
    ~ArrayBase(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    T* data;    // offset 0x0
    int count;  // offset 0x4
    int cap;    // offset 0x8
};

template <class T, int N>
class FixedArray : public ArrayBase<T> {
public:
    FixedArray(void) {
        this->data = buf;
        this->cap = N;
        this->count = 0;
    }
    T buf[N];  // offset 0xC
};

typedef void (*XformFn)(f32 x, f32 y, f32* ox, f32* oy);
typedef void (*DrawFn)(const char* s, Font* f, f32 x, f32 y, f32 z, f32 h, f32 scale, f32 xs, f32 spacing);

extern "C" {
int vsprintf(char* buf, const char* fmt, va_list args);
int sprintf(char* buf, const char* fmt, ...);
int strcasecmp(const char* a, const char* b);
int strncasecmp(const char* a, const char* b, size_t n);
void* __nw__FUlPCc(u32 size, const char* tag);
void* __nwa__FUlPCc(u32 size, const char* tag);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal
int fn_8012C33C(const char* name);       // file exists
void* fn_8012C370(const char* name, u32 type, u32* version, u32* size, u8* first, u8* swapped);
void fn_8012C4D8(void* p);
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* t);
s32 fn_802CAC28(const char* name, s32 key);
s32 fn_802CAC34(const char* name, const char* data);
void fn_802CAC54(s32 tex);
void fn_802CB228(s32 tex);
UIContext* fn_8022EDC4(void);
u32 fn_80230B9C(u32 color);
void fn_80231380(UIVert* v, u32 color, f32 x, f32 y, f32 z, f32 u, f32 w);
gArtSet* fn_8023512C(const char* name);
void fn_80235380(gArtSet* s);
void fn_802354A0(gArtSet* s, int idx, f32 x, f32 y, f32 z, f32 w, f32 h, u32 color);
f32 fn_80235EF0(gArtSet* s, int idx);  // width
f32 fn_80235F64(gArtSet* s, int idx);  // height
void fn_802C8B98(s32 tex, const void* m);
void fn_802C8BD0(s32 tex);
void fn_802C8C04(s32 tex);
void fn_802C8D74(void);
void fn_802C8DB8(UIVert* v, int nv, const u16* idx, int ni);
int fn_802C8DBC(int n);
void fn_802C8E00(u32 color, int row, f32 x, f32 y, f32 z, f32 u, f32 v);
void fn_802C8E48(void);
}

static inline u16 DecodeUTF8(const char* s, const char** next) {
    u8 b = s[0];
    if ((b & 0xF0) == 0xE0) {
        *next = s + 3;
        return ((b & 0xF) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
    }
    if ((b & 0xE0) == 0xC0) {
        *next = s + 2;
        return ((b & 0x1F) << 6) | ((u8)s[1] & ~0xC0);
    }
    *next = s + 1;
    return b & 0x7F;
}

static inline f32 Mul(f32 a, f32 b) { return a * b; }

static inline void Swap16(void* p) {
    u8* b = (u8*)p;
    u8 t = b[0];
    b[0] = b[1];
    b[1] = t;
}

static inline void Swap32(void* p) {
    u8 t;
    u8* b = (u8*)p;
    t = b[0];
    b[0] = b[3];
    b[3] = t;
    t = b[1];
    b[1] = b[2];
    b[2] = t;
}

class UnkStaticFF00 {
public:
    UnkStaticFF00(void) { value = 0xFF000000; }
    u32 value;
};

static inline void SwapUV(Glyph* g) {
    u8* b = (u8*)&g->u0;
    u8 t;
    t = b[0];
    b[0] = b[3];
    b[3] = t;
    t = b[1];
    b[1] = b[2];
    b[2] = t;
    t = b[4];
    b[4] = b[7];
    b[7] = t;
    t = b[5];
    b[5] = b[6];
    b[6] = t;
    t = b[8];
    b[8] = b[11];
    b[11] = t;
    t = b[9];
    b[9] = b[10];
    b[10] = t;
    t = b[12];
    b[12] = b[15];
    b[15] = t;
    t = b[13];
    b[13] = b[14];
    b[14] = t;
}

ModuleTag lbl_8055F038;

struct TextVersion {
    TextVersion(u16 v) { value = v; }
    u16 value;
};

TextVersion lbl_8055F040(0x100);
TextVersion lbl_8055F042(0x405);
TextVersion lbl_8055F044(0x401);
UnkStatic7F lbl_8055F048;
UnkStaticFF00 lbl_8055F04C;
u32 lbl_8055F050;          // the current text colour
IngameText lbl_8055F058;   // the in-game text font and count
FixedArray<EscSeq, 48> lbl_80497F14;
FixedArray<ArtItem, 16> lbl_8049846C;
UnkStaticFF00 lbl_8055F060;  // the outline colour mask
u8 lbl_8055F064;
FontTracker* lbl_8055F068;  // the loaded fonts
TextItem* lbl_8055F06C;     // the in-game text buffer
XformFn lbl_8055F070;       // the screen transform for 3D text
bool lbl_8055F074;          // drawing black text

extern "C" {

void fn_80231AF0(const char* s, Font* f, f32 x, f32 y, f32 z, f32 h, f32 scale, f32 xs, f32 spacing);
void fn_80231D84(const char* s, Font* f, f32 x, f32 y, f32 z, f32 h, f32 scale, f32 xs, f32 spacing);
DrawFn lbl_8055B9A8 = fn_80231D84;

Font* fn_80232578(const char* name);
void fn_80233AA4(Font* f, u16 c, f32 x0, f32 y0, f32 x1, f32 y1, f32 z);
void fn_80233BF0(const char* s, Font* f, f32* x, f32* y, int align, f32 scale, f32 xs);
void fn_80233DD8(Font* f, u32 color);
void fn_80233E34(Font* f, const UIXform* m, u32 color);
void fn_80233E80(Font* f, u32 color);
void fn_80233EBC(void);
void fn_80233EC0(Font* f, u16 c, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1);
void fn_80232BA4(const char* text, Font* f, f32 x, f32 y, u32 color, int align, f32 scale, f32 z, f32 xs);
void fn_802344CC(const char* s, u32 color, int align, int wide, f32 x, f32 y, f32 scale);
void fn_802346C8(void);

int fn_80231504(void) { return 0; }

int fn_8023150C(const char* name) {
    for (int i = 0; i < lbl_8055F068->count; i++) {
        if (strcasecmp(lbl_8055F068->e[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void fn_8023158C(Font* f, const char* name) {
    FontEntry* e = &lbl_8055F068->e[lbl_8055F068->count++];
    e->font = f;
    e->refs = 1;
    e->name = (char*)__nwa__FUlPCc(0x40, "font name");
    strcpy(e->name, name);
}

int fn_802315FC(int idx) {
    FontEntry* e = &lbl_8055F068->e[idx];
    if (--e->refs < 1) {
        fn_802CAC54(e->font->tex);
        fn_8012C4D8(e->font);
        operator delete[](e->name);
        lbl_8055F068->e[idx] = lbl_8055F068->e[--lbl_8055F068->count];
        return 1;
    }
    return 0;
}

int fn_802316A4(Font* f) {
    FontEntry* e = lbl_8055F068->e;
    for (int i = 0; i < lbl_8055F068->count; e++, i++) {
        if (e->font == f) {
            return fn_802315FC(i);
        }
    }
    fn_802E30C8("gText: I don't remember the font %p I was asked to forget", f);
    return 0;
}

void fn_80231714(u32* color) {}

u32 fn_80231718(u32 c) {
    u32 m = lbl_8055F060.value;
    return (c & 0xFF000000) | (((m >> 16) & 0xFF) << 16) | (((m >> 8) & 0xFF) << 8) | (m & 0xFF);
}

Font* fn_80231730(const char* name) {
    char buf[24];
    sprintf(buf, "_ol_%s", name);
    if (fn_8012C33C(buf)) {
        return fn_80232578(buf);
    }
    return NULL;
}

static inline void XformVec(const UIXform* m, const VecP* in, VecP* out) {
    if (in == out) {
        VecP t;
        t.x = in->z * m->m[2][0] + (in->x * m->m[0][0] + in->y * m->m[1][0]);
        t.y = in->z * m->m[2][1] + (in->x * m->m[0][1] + in->y * m->m[1][1]);
        t.z = in->z * m->m[2][2] + (in->x * m->m[0][2] + in->y * m->m[1][2]);
        *out = t;
    } else {
        out->x = in->z * m->m[2][0] + (in->x * m->m[0][0] + in->y * m->m[1][0]);
        out->y = in->z * m->m[2][1] + (in->x * m->m[0][1] + in->y * m->m[1][1]);
        out->z = in->z * m->m[2][2] + (in->x * m->m[0][2] + in->y * m->m[1][2]);
    }
}

void fn_80231780(const UIXform* m) {
    for (int i = 0; i < lbl_8049846C.count; i++) {
        ArtItem* it = &lbl_8049846C.data[i];
        EscSeq* e = &lbl_80497F14.data[it->esc];
        u32 color = e->takeColor ? it->color : fn_80230B9C((it->color & 0xFF000000) | 0xFFFFFF);
        if (e->art) {
            VecP p;
            p.x = it->x;
            p.y = it->y;
            p.z = it->z;
            if (m) {
                XformVec(m, &p, &p);
                p.x += m->pos[0];
                p.y += m->pos[1];
                p.z += m->pos[2];
            }
            f32 s = fn_8022EDC4()->scale;
            s = (s > 0.001f) ? s : 0.001f;
            f32 k = 1.0f / s;
            fn_802354A0(e->art, e->artIdx, p.x - 0.5f * (k * it->w - it->w), p.y + 0.5f * (k * it->h - it->h), p.z,
                        k * it->w, k * it->h, color);
        } else {
            if (m) {
                fn_802C8B98(lbl_80497F14.data[it->esc].tex, m);
            } else {
                fn_802C8B98(lbl_80497F14.data[it->esc].tex, fn_8022EDC4());
            }
            fn_802C8DBC(4);
            f32 x0 = it->x;
            f32 y0 = it->y;
            f32 x1 = x0 + it->w;
            f32 y1 = y0 - it->h;
            fn_802C8E00(color, 0, x0, y0, it->z, 0.0f, 0.0f);
            fn_802C8E00(color, 0, x1, y0, it->z, 1.0f, 0.0f);
            fn_802C8E00(color, 1, x0, y1, it->z, 0.0f, 1.0f);
            fn_802C8E00(color, 1, x1, y1, it->z, 1.0f, 1.0f);
            fn_802C8E48();
            fn_802C8D74();
        }
    }
    lbl_8049846C.count = 0;
}

void fn_80231ADC(void) { lbl_8049846C.count = 0; }

void fn_80231AF0(const char* s, Font* f, f32 x, f32 y, f32 z, f32 h, f32 scale, f32 xs, f32 spacing) {
    strlen(s);
    while (*s) {
        f32 adv = h;
        int skip;
        if (*s == '\\') {
            for (int i = 0; i < lbl_80497F14.count; i++) {
                const char* name = lbl_80497F14.data[i].name;
                if (strncasecmp(s + 1, name, strlen(name)) == 0) {
                    EscSeq* e = &lbl_80497F14.data[i];
                    if (e->art) {
                        f32 k = h / fn_80235F64(e->art, e->artIdx);
                        adv = e->scale * (k * fn_80235EF0(e->art, e->artIdx)) + e->scale * (e->postspace * k);
                    } else {
                        adv = h * e->scale;
                    }
                    skip = strlen(lbl_80497F14.data[i].name) + 1;
                    goto found;
                }
            }
        }
        skip = 0;
    found:
        if (skip > 0) {
            s += skip;
            x += adv;
            continue;
        }
        const char* next;
        u16 c = DecodeUTF8(s, &next);
        Glyph* g = &f->glyphs[c];
        x += xs * (scale * g->pre);
        s += next - s;
        f32 tx, ty;
        if (lbl_8055F074) {
            lbl_8055F070(x - 2.0f, 2.0f + y, &tx, &ty);
            tx += 2.0f;
            ty -= 2.0f;
        } else {
            lbl_8055F070(x, y, &tx, &ty);
        }
        fn_80233AA4(f, c, tx, ty + h, tx + scale * f->glyphs[c].width, ty, z);
        x += scale * f->glyphs[c].width;
        x += spacing + xs * (scale * f->glyphs[c].post);
    }
}

void fn_80231D84(const char* s, Font* f, f32 x, f32 y, f32 z, f32 h, f32 scale, f32 xs, f32 spacing) {
    strlen(s);
    while (*s) {
        f32 adv;
        f32* advp = &adv;
        adv = h;
        u32 color = lbl_8055F050;
        int skip;
        if (*s == '\\') {
            for (int i = 0; i < lbl_80497F14.count; i++) {
                EscSeq* e = &lbl_80497F14.data[i];
                if (strncasecmp(s + 1, e->name, strlen(e->name)) == 0) {
                    ArtItem* it = &lbl_8049846C.data[lbl_8049846C.count++];
                    f32 post = 0.0f;
                    it->esc = i;
                    if (e->art) {
                        f32 k = h / fn_80235F64(e->art, e->artIdx);
                        it->h = h * e->scale;
                        it->w = (k * e->scale) * fn_80235EF0(e->art, e->artIdx);
                        post = e->scale * (e->postspace * k);
                    } else {
                        it->w = h * e->scale;
                        it->h = h * e->scale;
                    }
                    it->x = x;
                    it->y = (y + h) + 0.5f * (it->h - h);
                    it->z = z;
                    it->color = color;
                    if (advp) {
                        *advp = it->w + post;
                    }
                    skip = strlen(e->name) + 1;
                    goto found;
                }
            }
        }
        skip = 0;
    found:
        if (skip > 0) {
            s += skip;
            x += adv;
            continue;
        }
        const char* next;
        u16 c = DecodeUTF8(s, &next);
        Glyph* g = &f->glyphs[c];
        x += xs * (scale * g->pre);
        fn_80233AA4(f, c, x, y + h, x + scale * g->width, y, z);
        s += next - s;
        x += scale * f->glyphs[c].width;
        x += spacing + xs * (scale * f->glyphs[c].post);
    }
}

void fn_80232024(const char** s, u32* color) {
    const char* p = *s;
    if ((u8)*p == 1) {
        *s = p + 1;
    }
}

void fn_80232040(const char* s, Font* f, u32 color, int align, const UIXform* m, f32 x, f32 y, f32 scale, f32 z,
                 f32 xs, f32 spacing) {
    u32 c;
    f32 py;
    f32 px;
    const char* str;
    int outline;
    int passes;
    str = s;
    px = x;
    py = y;
    c = color;
    lbl_8055F074 = color == 0xFF000000;
    fn_80232024(&str, &c);
    passes = 1;
    if (f->outline) {
        passes = 2;
    }
    f32 lh = f->height * scale * fn_8022EDC4()->scale;
    fn_80233BF0(str, f, &px, &py, align, scale, xs);
    while (passes > 0) {
        outline = passes == 2;
        Font* ff = f;
        u32 cc = c;
        if (outline) {
            ff = f->outline;
            cc = fn_80231718(cc);
        }
        if (m) {
            fn_80233E34(ff, m, cc);
        } else {
            fn_80233DD8(ff, cc);
        }
        lbl_8055B9A8(str, ff, px, py, z, lh, scale * fn_8022EDC4()->scale, xs, spacing);
        fn_80233EBC();
        if (outline) {
            fn_80231ADC();
        } else {
            fn_80231780(m);
        }
        passes--;
    }
}

void fn_802321D8(void) {
    if (lbl_8055F068) {
        fn_802E317C("Font system already loaded! (gTextBegin() called twice)");
    }
    lbl_8055F068 = (FontTracker*)__nw__FUlPCc(sizeof(FontTracker), "gtext:fontTracker");
    memset(lbl_8055F068, 0, sizeof(FontTracker));
}

void fn_80232230(void) {
    EscSeq* e;
    ValueTable* t;
    ValueNode* n;
    int i;
    lbl_80497F14.count = 0;
    t = fn_80131810("esc_seq.val");
    if (t) {
        for (i = 0; (n = t->GetChild(i)) != NULL; i++) {
            if (strcasecmp(n->GetName(), "esc_seq") == 0) {
                e = &lbl_80497F14.data[lbl_80497F14.count++];
                e->name = n->GetValue("seq");
                if (n->Has("art") && n->Has("art_idx")) {
                    e->art = fn_8023512C(n->GetValue("art"));
                    e->artIdx = n->GetIntValue("art_idx");
                    e->postspace = n->GetFloat("art_postspace", 0.0f);
                    e->tex = -1;
                } else {
                    e->art = NULL;
                    e->artIdx = 0;
                    e->postspace = 0.0f;
                    e->tex = fn_802CAC28(n->GetValue("tex"), 0);
                }
                e->scale = n->GetFloat("scale", 1.0f);
                e->takeColor = n->GetBool("take_color", 0);
            }
        }
        fn_80131818(t);
    }
}

void fn_8023243C(void) {
    for (int i = 0; i < lbl_80497F14.count; i++) {
        if (lbl_80497F14.data[i].art) {
            fn_80235380(lbl_80497F14.data[i].art);
        }
        if (lbl_80497F14.data[i].tex != -1) {
            fn_802CAC54(lbl_80497F14.data[i].tex);
        }
    }
    lbl_80497F14.count = 0;
}

void fn_802324D8(void) {
    for (int i = 0; i < lbl_8055F068->count; i++) {
        FontEntry* e = &lbl_8055F068->e[i];
        fn_802E30C8("gText: font %s gone but not forgotten", e->name);
        fn_802CB228(e->font->tex);
        fn_8012C4D8(e->font);
        operator delete[](e->name);
    }
    operator delete(lbl_8055F068);
    lbl_8055F068 = NULL;
}

Font* fn_80232578(const char* name) {
    int idx = fn_8023150C(name);
    if (idx >= 0) {
        FontEntry* e = &lbl_8055F068->e[idx];
        e->refs++;
        e->font->outline = fn_80231730(name);
        return e->font;
    }
    u8 ok = 0;
    u8 swapped = 0;
    u32 size = 0;
    u32 version;
    Font* f = (Font*)fn_8012C370(name, 'GTEX', &version, &size, &ok, &swapped);
    if (f != NULL) {
        if (version != 8) {
            fn_802E317C("gText: Font \"%s\" has bad version %d", name, version);
        }
        if (!ok) {
            fn_802E317C("Duplicate fonts allocated (%s).  Why? How? Arrrrrrggggg.", name);
        }
    } else {
        fn_802E30C8("gText: Null font %s!", name);
        return NULL;
    }
    if (swapped) {
        Swap16(&f->count);
        Swap32(&f->unk14);
        Swap32(&f->glyphs);
        Swap32(&f->texName);
        f->glyphs = (Glyph*)((u8*)f->glyphs + (u32)f);
        f->texName = (char*)((u8*)f->texName + (u32)f);
        for (int i = 0; i < f->count; i++) {
            SwapUV(&f->glyphs[i]);
            Swap32(&f->glyphs[i].pre);
            Swap32(&f->glyphs[i].post);
            Swap32(&f->glyphs[i].width);
        }
    }
    char tex[24];
    strncpy(tex, name, 11);
    tex[11] = 0;
    strcat(tex, ".tex");
    if (strlen(tex) > 16) {
        fn_802E317C("gText: Texture name %s is longer than %d, it's %d", tex, 16, strlen(tex));
    }
    s32 t = fn_802CAC34(tex, f->texName);
    if (t == -1) {
        fn_802E30C8("gText: Failed to extract texture with font- returning NULL font.");
        return NULL;
    }
    f->tex = t;
    f->outline = fn_80231730(name);
    fn_8023158C(f, name);
    return f;
}

void fn_802328D4(Font* f) {
    if (f->outline && fn_802316A4(f->outline)) {
        f->outline = NULL;
    }
    fn_802316A4(f);
}

void fn_80232928(u32 mask) { lbl_8055F060.value = mask; }

void fn_80232930(void) { lbl_8055F060.value = lbl_8055F04C.value; }

f32 fn_8023293C(Font* f, const char* s, f32 xs) {
    if (s == NULL) {
        return 0.0f;
    }
    f32 gaps = 0.0f;
    f32 w = 0.0f;
    int i = 0;
    strlen(s);
    while (s[i]) {
        f32 adv = f->height;
        f32 h = f->height;
        int skip;
        if (s[i] == '\\') {
            const char* p = s + i + 1;
            for (int j = 0; j < lbl_80497F14.count; j++) {
                const char* name = lbl_80497F14.data[j].name;
                if (strncasecmp(p, name, strlen(name)) == 0) {
                    EscSeq* e = &lbl_80497F14.data[j];
                    if (e->art) {
                        h /= (fn_80235F64(e->art, e->artIdx));
                        adv = e->scale * (h * fn_80235EF0(e->art, e->artIdx)) + e->scale * (e->postspace * h);
                    } else {
                        adv = h * e->scale;
                    }
                    skip = strlen(lbl_80497F14.data[j].name) + 1;
                    goto found;
                }
            }
        }
        skip = 0;
    found:
        if (skip) {
            i += skip;
            w += adv;
            continue;
        }
        const char* next;
        u16 c = DecodeUTF8(s + i, &next);
        Glyph* g = &f->glyphs[c];
        i += next - (s + i);
        w += g->width;
        gaps += g->pre + g->post;
    }
    gaps *= xs;
    return (w + gaps) * fn_8022EDC4()->scale;
}

int fn_80232B70(Font* f) { return f->height; }

u8 fn_80232B78(Font* f) { return f->b5; }

void fn_80232B80(XformFn fn) {
    lbl_8055F070 = fn;
    lbl_8055B9A8 = (fn == NULL) ? fn_80231D84 : fn_80231AF0;
}

void fn_80232BA4(const char* text, Font* f, f32 x, f32 y, u32 color, int align, f32 scale, f32 z, f32 xs) {
    u32 c = color;
    fn_80231714(&c);
    if (*text) {
        fn_80232040(text, f, c, align, NULL, x, y, scale, z, xs, 0.0f);
    }
}

void fn_80232C50(const char* text, Font* f, u32 color, int align, f32 x, f32 y, f32 scale, f32 width, f32 z,
                 f32 xs) {
    u32 c = color;
    f32 spacing = width - scale * fn_8023293C(f, text, xs);
    spacing /= strlen(text);
    fn_80231714(&c);
    if (*text) {
        fn_80232040(text, f, c, align, NULL, x, y, scale, z, xs, spacing);
    }
}

void fn_80232D3C(const char* text, Font* f, u32 color, int align, const UIXform* m, f32 x, f32 y, f32 scale,
                 f32 z, f32 xs) {
    u32 c = color;
    fn_80231714(&c);
    if (*text) {
        fn_80232040(text, f, c, align, m, x, y, scale, z, xs, 0.0f);
    }
}

void fn_80232DF4(const char* text, Font* f, u32 color, int align, f32 x, f32 y, f32 scale, f32 z, f32 xs) {
    f32 px = x;
    f32 py = y;
    u32 c = color;
    fn_80231714(&c);
    if (*text) {
        f32 lh = f->height * scale * fn_8022EDC4()->unk34;
        fn_80233BF0(text, f, &px, &py, align, scale, xs);
        f32 x0 = px;
        strlen(text);
        for (int passes = (f->outline != NULL) + 1; passes > 0; passes--) {
            px = x0;
            int i = 0;
            Font* ff = f;
            u32 cc = c;
            if (passes == 2) {
                ff = f->outline;
                cc = fn_80231718(cc);
            }
            fn_80233E80(ff, cc);
            while (text[i]) {
                const char* next;
                u16 ch = DecodeUTF8(text + i, &next);
                Glyph* g = &ff->glyphs[ch];
                px = px + xs * (scale * g->pre);
                fn_80233AA4(ff, ch, px, py - lh, px + scale * ff->glyphs[ch].width, py, z);
                px = px + scale * ff->glyphs[ch].width;
                i += next - (text + i);
                px = px + xs * (scale * ff->glyphs[ch].post);
            }
            fn_80233EBC();
        }
    }
}

// Returns the start of the next UTF-8 character (1-3 byte sequences).
static inline const char* NextGlyph(const char* p) {
    const unsigned char* u = (const unsigned char*)p;
    if ((*u & 0xF0) == 0xE0) return p + 3;
    if ((*u & 0xE0) == 0xC0) return p + 2;
    return p + 1;
}

void fn_80233010(const char* text, Font* f, u32 color, int align, int ingame, f32 x, f32 y, f32 maxw, f32 scale,
                 f32 z, f32 xs) {
    const char* p = text;
    int line = 0;
    strlen(p);
    char buf[0x200];
    while (*p) {
        const char* start = p;
        memset(buf, 0, sizeof(buf));
        int n = 0;
        while (scale * fn_8023293C(f, buf, xs) < maxw && *p && *p != '\n' && *p != '|') {
            const char* next = NextGlyph(p);
            int len = next - p;
            for (int j = 0; j < len; j++) {
                buf[n++] = *p++;
            }
        }
        const char* save = p;
        int saven = n;
        if (*p) {
            while (*p != ' ' && p != start && *p != '\n' && *p != '|') {
                p--;
                n--;
            }
        }
        if (p == start) {
            p = save;
            n = saven;
        }
        buf[n] = 0;
        if (*p == ' ' || *p == '\n' || *p == '|') {
            p++;
        }
        for (int k = 0; k < n; k++) {
            if (buf[k] == '^') {
                buf[k] = ' ';
            }
        }
        f32 yy = y - line++ * (scale * fn_80232B70(f));
        if (!ingame) {
            fn_80232BA4(buf, f, x, yy, color, align, scale, z, xs);
        } else {
            fn_802344CC(buf, color, align, 0, x, yy, scale);
        }
    }
}

void fn_80233354(const char* text, Font* f, u32 color, int align, f32 x, f32 y, f32 scale, f32 z) {
    u32 c = color;
    fn_80231714(&c);
    f32 y0 = y;
    strlen(text);
    if (*text) {
        f32 lh = f->height * scale;
        const char* next;
        DecodeUTF8(text, &next);
        if (align & 1) {
        } else if (align & 2) {
            fn_8023293C(f, text, 1.0f);
        } else if (align & 4) {
            fn_8023293C(f, text, 1.0f);
        }
        if (align & 8) {
            x += f->height * scale;
        } else if (align & 0x20) {
            x += f->b6 * scale;
        } else if (align & 0x10) {
            x += Mul(f->height * scale, 0.5f);
        }
        for (int passes = (f->outline != NULL) + 1; passes > 0; passes--) {
            y = y0;
            int i = 0;
            Font* ff = f;
            u32 cc = c;
            if (passes == 2) {
                ff = f->outline;
                cc = fn_80231718(cc);
            }
            fn_80233DD8(ff, cc);
            while (text[i]) {
                const char* nx;
                u16 ch = DecodeUTF8(text + i, &nx);
                Glyph* g = &ff->glyphs[ch];
                y -= scale * g->pre;
                fn_80233EC0(ff, ch, x, y, z, x + lh, y - scale * g->width, z);
                y -= scale * (ff->glyphs[ch].width + ff->glyphs[ch].post);
                i += nx - (text + i);
            }
            fn_80233EBC();
        }
    }
}

void fn_80233600(const char* text, Font* f, u32 color, int align, f32 x, f32 y, f32 z);

void fn_802335F0(const char* text, Font* f, f32 x, f32 y) { fn_80233600(text, f, -1, 9, x, y, 0.0f); }

void fn_80233600(const char* text, Font* f, u32 color, int align, f32 x, f32 y, f32 z) {
    u32 c = color;
    fn_80231714(&c);
    if (*text) {
        fn_80232040(text, f, c, align, NULL, x, y, 1.0f, z, 1.0f, 0.0f);
    }
}

void fn_802336AC(Font* f, u32 color, int align, const char* fmt, f32 x, f32 y, f32 z, ...) {
    char buf[128];
    va_list ap;
    va_start(ap, z);
    vsprintf(buf, fmt, ap);
    fn_80233600(buf, f, color, align, x, y, z);
}

void fn_802337A0(Font* f, u32 color, int align, const char* fmt, f32 x, f32 y, f32 z, f32 scale, f32 xs, ...) {
    char buf[128];
    va_list ap;
    va_start(ap, xs);
    vsprintf(buf, fmt, ap);
    fn_80232BA4(buf, f, x, y, color, align, scale, z, xs);
}

void fn_8023389C(Font* f, u32 color, int align, const UIXform* m, const char* fmt, f32 x, f32 y, f32 z,
                 f32 scale, f32 xs, ...) {
    char buf[128];
    va_list ap;
    va_start(ap, xs);
    vsprintf(buf, fmt, ap);
    fn_80232D3C(buf, f, color, align, m, x, y, scale, z, xs);
}

void fn_802339A8(Font* f, u32 color, int align, const char* fmt, f32 x, f32 y, f32 z, f32 scale, f32 xs, ...) {
    char buf[128];
    va_list ap;
    va_start(ap, xs);
    vsprintf(buf, fmt, ap);
    fn_80232DF4(buf, f, color, align, x, y, scale, z, xs);
}

void fn_80233AA4(Font* f, u16 c, f32 x0, f32 y0, f32 x1, f32 y1, f32 z) {
    if (c != ' ') {
        u32 color = lbl_8055F050;
        UIVert v[4];
        fn_80231380(&v[0], color, x0, y0, z, f->glyphs[c].u0, f->glyphs[c].v0);
        fn_80231380(&v[1], color, x1, y0, z, f->glyphs[c].u1, f->glyphs[c].v0);
        fn_80231380(&v[2], color, x0, y1, z, f->glyphs[c].u0, f->glyphs[c].v1);
        fn_80231380(&v[3], color, x1, y1, z, f->glyphs[c].u1, f->glyphs[c].v1);
        u16 idx[6] = {0, 1, 3, 0, 3, 2};
        fn_802C8DB8(v, 4, idx, 6);
    }
}

void fn_80233BF0(const char* s, Font* f, f32* x, f32* y, int align, f32 scale, f32 xs) {
    if (align & 1) {
        const char* next;
        u16 c = DecodeUTF8(s, &next);
        *x -= f->glyphs[c].pre;
    } else if (align & 2) {
        *x -= scale * fn_8023293C(f, s, xs);
    } else if (align & 4) {
        *x -= Mul(scale * fn_8023293C(f, s, xs), 0.5f);
    }
    f32 cs = fn_8022EDC4()->scale;
    if (align & 8) {
        *y -= cs * (f->height * scale);
    } else if (align & 0x20) {
        *y -= cs * (f->b6 * scale);
    } else if (align & 0x10) {
        *y -= Mul(cs * (f->height * scale), 0.5f);
    }
}

void fn_80233DD8(Font* f, u32 color) {
    UIContext* ctx = fn_8022EDC4();
    lbl_8055F050 = fn_80230B9C(color);
    fn_802C8B98(f->tex, ctx);
}

void fn_80233E34(Font* f, const UIXform* m, u32 color) {
    lbl_8055F050 = fn_80230B9C(color);
    fn_802C8B98(f->tex, m);
}

void fn_80233E80(Font* f, u32 color) {
    lbl_8055F050 = fn_80230B9C(color);
    fn_802C8C04(f->tex);
}

void fn_80233EBC(void) { fn_802C8D74(); }

void fn_80233EC0(Font* f, u16 c, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1) {
    if (c != ' ') {
        u32 color = lbl_8055F050;
        UIVert v[4];
        fn_80231380(&v[0], color, x0, y0, z0, f->glyphs[c].u0, f->glyphs[c].v1);
        fn_80231380(&v[1], color, x1, y0, z0, f->glyphs[c].u0, f->glyphs[c].v0);
        fn_80231380(&v[2], color, x0, y1, z1, f->glyphs[c].u1, f->glyphs[c].v1);
        fn_80231380(&v[3], color, x1, y1, z1, f->glyphs[c].u1, f->glyphs[c].v0);
        u16 idx[6] = {0, 1, 3, 0, 3, 2};
        fn_802C8DB8(v, 4, idx, 6);
    }
}

f32 fn_8023400C(int idx) {
    f32 w = 0.0f;
    f32 gaps = 0.0f;
    Font* f = lbl_8055F058.font;
    const char* s = lbl_8055F06C[idx].text;
    int i = 0;
    strlen(s);
    while (s[i]) {
        f32 adv = f->height;
        f32 h = f->height;
        int skip;
        if (s[i] == '\\') {
            const char* p = s + i + 1;
            for (int j = 0; j < lbl_80497F14.count; j++) {
                const char* name = lbl_80497F14.data[j].name;
                if (strncasecmp(p, name, strlen(name)) == 0) {
                    EscSeq* e = &lbl_80497F14.data[j];
                    if (e->art) {
                        h = h / fn_80235F64(e->art, e->artIdx);
                        adv = e->scale * (h * fn_80235EF0(e->art, e->artIdx)) + e->scale * (e->postspace * h);
                    } else {
                        adv = h * e->scale;
                    }
                    skip = strlen(lbl_80497F14.data[j].name) + 1;
                    goto found;
                }
            }
        }
        skip = 0;
    found:
        if (skip) {
            i += skip;
            w += adv;
            continue;
        }
        const char* next;
        u16 c = DecodeUTF8(s + i, &next);
        Glyph* g = &f->glyphs[c];
        i += next - (s + i);
        w += g->width;
        gaps += g->pre + g->post;
    }
    w *= lbl_8055F06C[idx].scale;
    gaps *= lbl_8055F06C[idx].xs;
    return w + gaps;
}

void fn_8023423C(int idx, int align) {
    Font* f = lbl_8055F058.font;
    f32 scale = lbl_8055F06C[idx].scale;
    f32 xs = lbl_8055F06C[idx].xs;
    if (align & 1) {
        const char* next;
        u16 c = DecodeUTF8(lbl_8055F06C[idx].text, &next);
        lbl_8055F06C[idx].x -= xs * f->glyphs[c].pre;
    } else if (align & 2) {
        lbl_8055F06C[idx].x -= fn_8023400C(idx);
    } else if (align & 4) {
        lbl_8055F06C[idx].x -= Mul(fn_8023400C(idx), 0.5f);
    }
    if (align & 8) {
        lbl_8055F06C[idx].y -= f->height * scale;
    } else if (align & 0x20) {
        lbl_8055F06C[idx].y -= f->b6 * scale;
    } else if (align & 0x10) {
        lbl_8055F06C[idx].y -= f->height * scale / 2.0f;
    }
}

void fn_8023441C(const char* font) {
    lbl_8055F06C = (TextItem*)__nwa__FUlPCc(0x60 * sizeof(TextItem), "gText ingame buffer");
    lbl_8055F058.font = fn_80232578(font);
    lbl_8055F058.count = 0;
}

void fn_80234470(void) {
    fn_802328D4(lbl_8055F058.font);
    lbl_8055F058.font = NULL;
    operator delete[](lbl_8055F06C);
    lbl_8055F06C = NULL;
}

int fn_802344B0(void) { return lbl_8055F06C != NULL; }

void fn_802344C4(u8 v) { lbl_8055F064 = v; }

void fn_802344CC(const char* s, u32 color, int align, int wide, f32 x, f32 y, f32 scale) {
    u32 c = color;
    const char* str = s;
    fn_80232024(&str, &c);
    if (lbl_8055F058.count >= 0x60) {
        fn_802346C8();
    }
    strncpy(lbl_8055F06C[lbl_8055F058.count].text, str, 0x40);
    lbl_8055F06C[lbl_8055F058.count].text[0x3F] = 0;
    lbl_8055F06C[lbl_8055F058.count].color = c;
    lbl_8055F06C[lbl_8055F058.count].masked = fn_80231718(c);
    lbl_8055F06C[lbl_8055F058.count].x = x;
    lbl_8055F06C[lbl_8055F058.count].y = y;
    lbl_8055F06C[lbl_8055F058.count].scale = scale;
    if (wide) {
        lbl_8055F06C[lbl_8055F058.count].xs = 0.45f * scale;
    } else {
        lbl_8055F06C[lbl_8055F058.count].xs = scale;
    }
    fn_8023423C(lbl_8055F058.count, align);
    lbl_8055F058.count++;
}

int fn_80234654(void) {
    TextItem* it = lbl_8055F06C;
    int n = lbl_8055F058.count;
    int total = 0;
    for (int i = 0; i < n; it++, i++) {
        total += strlen(it->text);
    }
    return total;
}

void fn_802346C8(void) {
    for (int passes = (lbl_8055F058.font->outline != NULL) + 1; passes > 0; passes--) {
        Font* f;
        if (passes == 2) {
            f = lbl_8055F058.font->outline;
        } else {
            f = lbl_8055F058.font;
        }
        fn_802C8BD0(f->tex);
        int n = fn_80234654() * 4;
        if (n > 0 && fn_802C8DBC(n)) {
            for (int i = 0; i < lbl_8055F058.count; i++) {
                TextItem* it = &lbl_8055F06C[i];
                f32 h = f->height * it->scale;
                f32 y0 = it->y;
                f32 y1 = h + y0;
                u32* cp;
                if (passes == 2) {
                    cp = &it->masked;
                } else {
                    cp = &it->color;
                }
                f32 x = it->x;
                strlen(lbl_8055F06C[i].text);
                int j = 0;
                while (lbl_8055F06C[i].text[j]) {
                    const char* s = lbl_8055F06C[i].text;
                    f32 adv;
                    f32* advp = &adv;
                    adv = h;
                    u32 color = *cp;
                    int skip;
                    if (s[j] == '\\') {
                        for (int k = 0; k < lbl_80497F14.count; k++) {
                            EscSeq* e = &lbl_80497F14.data[k];
                            if (strncasecmp(s + j + 1, e->name, strlen(e->name)) == 0) {
                                ArtItem* a = &lbl_8049846C.data[lbl_8049846C.count++];
                                f32 post = 0.0f;
                                a->esc = k;
                                if (e->art) {
                                    f32 q = h / fn_80235F64(e->art, e->artIdx);
                                    a->h = h * e->scale;
                                    a->w = (q * e->scale) * fn_80235EF0(e->art, e->artIdx);
                                    post = e->scale * (e->postspace * q);
                                } else {
                                    a->w = h * e->scale;
                                    a->h = h * e->scale;
                                }
                                a->x = x;
                                a->y = y1 + 0.5f * (a->h - h);
                                a->z = 0.0f;
                                a->color = color;
                                if (advp) {
                                    *advp = a->w + post;
                                }
                                skip = strlen(e->name) + 1;
                                goto found;
                            }
                        }
                    }
                    skip = 0;
                found:
                    if (skip > 0) {
                        j += skip;
                        x += adv;
                        continue;
                    }
                    const char* next;
                    u16 c = DecodeUTF8(s + j, &next);
                    Glyph* g = &f->glyphs[c];
                    x += it->xs * g->pre;
                    f32 x1 = x + it->scale * g->width;
                    if (c != ' ') {
                        fn_802C8E00(*cp, 0, x, y1, 0.0f, g->u0, g->v0);
                        fn_802C8E00(*cp, 0, x1, y1, 0.0f, f->glyphs[c].u1, f->glyphs[c].v0);
                        fn_802C8E00(*cp, 1, x, y0, 0.0f, f->glyphs[c].u0, f->glyphs[c].v1);
                        fn_802C8E00(*cp, 1, x1, y0, 0.0f, f->glyphs[c].u1, f->glyphs[c].v1);
                    }
                    j += next - (s + j);
                    x += it->scale * f->glyphs[c].width;
                    x += it->xs * f->glyphs[c].post;
                }
            }
            fn_802C8E48();
        }
        fn_802C8D74();
        if (passes == 2) {
            fn_80231ADC();
        } else {
            fn_80231780(NULL);
        }
    }
    lbl_8055F058.count = 0;
}

void fn_80234AEC(void) { lbl_8055F058.count = 0; }

struct TextStyle {
    Font* font;  // offset 0x0
    u32 color;   // offset 0x4
    int align;   // offset 0x8
    f32 scale;   // offset 0xC
    f32 xs;      // offset 0x10
};

void fn_80234AFC(TextStyle* st, const char* fmt, f32 x, f32 y, ...) {
    char buf[128];
    va_list ap;
    va_start(ap, y);
    vsprintf(buf, fmt, ap);
    fn_80232BA4(buf, st->font, x, y, st->color, st->align, st->scale, 1.0f, st->xs);
}

}  // extern "C"
