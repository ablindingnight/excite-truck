// GX texture driver (0x802D7564-0x802D98F8, "drgc/tex"): the 450-entry GX texture table behind
// texture.cpp - creating GXTexObjs (and alpha companions) from tt_texture descriptors, converting
// palettes to RGB565/IA8 TLUTs, swizzling raw 4/8-bit images into the GX tiled layouts, binding
// textures to TEXMAP0/1 (base), 2/4 (envmap) and 3 (lightmap), render-target copies, and texel
// peek/poke. Names are guesses apart from the strings.
#include "game/common.h"

void* operator new[](size_t size, const char* tag);

struct GXTexObj {
    u32 dummy[8];
};

struct GXTlutObj {
    u32 dummy[3];
};

extern "C" {
void* memset(void* p, int c, u32 n);
void* memcpy(void* d, const void* s, u32 n);
int strcasecmp(const char* a, const char* b);
void DCFlushRange(void* p, u32 n);
void GXInitTexObj(GXTexObj* obj, void* image, u16 w, u16 h, int fmt, int wrapS, int wrapT, u8 mip);
void GXInitTexObjCI(GXTexObj* obj, void* image, u16 w, u16 h, int fmt, int wrapS, int wrapT, u8 mip, u32 tlut);
void GXInitTexObjLOD(GXTexObj* obj, int minf, int magf, f32 minLod, f32 maxLod, f32 bias, u8 clamp, u8 edge, int aniso);
void GXInitTexObjWrapMode(GXTexObj* obj, int s, int t);
void GXInitTexObjFilter(GXTexObj* obj, int minf, int magf);
u16 GXGetTexObjWidth(const GXTexObj* obj);
u16 GXGetTexObjHeight(const GXTexObj* obj);
int GXGetTexObjFmt(const GXTexObj* obj);
void* GXGetTexObjData(const GXTexObj* obj);
void GXLoadTexObj(const GXTexObj* obj, int map);
void GXInitTlutObj(GXTlutObj* obj, void* lut, int fmt, u16 n);
u16 GXGetTlutObjNumEntries(const GXTlutObj* obj);
void GXLoadTlut(const GXTlutObj* obj, u32 name);
void GXInvalidateTexAll(void);
u32 GXGetTexBufferSize(u16 w, u16 h, u32 fmt, u8 mip, u8 maxLod);
void GXSetTexCopySrc(u16 l, u16 t, u16 w, u16 h);
void GXSetTexCopyDst(u16 w, u16 h, int fmt, u8 mip);
void GXCopyTex(void* dst, u8 clear);
void GXSetZMode(u8 enable, int func, u8 update);
void GXSetColorUpdate(u8 enable);
u32 __cvt_fp2unsigned(double v);
void fn_801282EC(int align);
void fn_801282F4(void);
void* fn_801284E8(const char* tag, u32 size);
void fn_80128658(void* p);
void fn_802CFEEC(void);
void fn_802CFF50(void);
void fn_802CFF5C(u32 flags);
void fn_802CFFA4(u32 zhint);
void fn_802D2614(void);
void fn_802D6C68(void);
void fn_802E30C8(const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);
}

struct TexImage {
    u32* pal;    // offset 0x0
    u8* tex;     // offset 0x4
    u8* alpha;   // offset 0x8
};

// What texture.cpp hands over (0x34).
struct GXTexDesc {
    u32 format;      // offset 0x0
    u32 mode;        // offset 0x4
    u32 flags;       // offset 0x8
    u32 zhint;       // offset 0xC
    u32 wrap;        // offset 0x10
    u32 wrapT;       // offset 0x14
    u32 mips;        // offset 0x18
    u8 unk1C;        // offset 0x1C
    s32 lods;        // offset 0x20
    s32 w;           // offset 0x24
    s32 h;           // offset 0x28
    TexImage* data;  // offset 0x2C
    struct GXTex* user;  // offset 0x30
};

struct GXTex {
    char name[32];      // offset 0x0
    u8 used;            // offset 0x20
    GXTexObj obj;       // offset 0x24
    u8 pad44[0x20];
    GXTlutObj tlut;     // offset 0x64
    u16* lut;           // offset 0x70
    u32 lutSize;        // offset 0x74
    GXTexObj obj2;      // offset 0x78
    u8 pad98[0x20];
    GXTlutObj tlut2;    // offset 0xB8
    u16* lut2;          // offset 0xC4
    u32 lut2Size;       // offset 0xC8
    u8 hasAlpha;        // offset 0xCC
    void* data;         // offset 0xD0
    u32 dataSize;       // offset 0xD4
    u32* srcPal;        // offset 0xD8
    u32 zhint;          // offset 0xDC
    u32 flags;          // offset 0xE0
    u8 unkE4;           // offset 0xE4
    void* heap;         // offset 0xE8
    u32 lastFrame;      // offset 0xEC
    u32 uses;           // offset 0xF0
};

struct TexBind {
    u8 pad[8];
    u8 base2;   // offset 0x8
    u8 pad9[2];
    u8 env2;    // offset 0xB
};

extern u32 lbl_8055FD1C;  // render flags
extern u8 lbl_8055FD24;
extern u32 lbl_8055FD2C;  // frame
extern u32 lbl_8055FD30;
extern TexBind lbl_804C5700;

static inline u16 TexShort(int v) { return v; }

ModuleTag lbl_8055FE08;
u16 lbl_8055FE10 = TexShort(0x100);
u16 lbl_8055FE12 = TexShort(0x405);
u16 lbl_8055FE14 = TexShort(0x401);
GXTex* lbl_8055FE18;  // bound base texture
GXTex* lbl_8055FE1C;  // bound envmap
GXTex* lbl_8055FE20;  // bound lightmap
GXTex* lbl_8055FE24;  // render target
GXTex* lbl_8055FE28;  // texture table

static inline void StrCopy(char* d, const char* s, int n) {
    char* p = d;
    while ((*p++ = *s++) != 0) {
        if (--n <= 0) {
            p[-1] = 0;
            break;
        }
    }
}

static inline void Swap32(void* p) {
    u8* b = (u8*)p;
    u8 t = b[0];
    b[0] = b[3];
    b[3] = t;
    t = b[1];
    b[1] = b[2];
    b[2] = t;
}

static inline u8 MipMapped(s32 n) { return n >= 2; }

extern "C" {

void fn_802D7564(void) {
    fn_802E30C8(" === DUMPING TEXTURES ===");
    for (int i = 0; i < 450; i++) {
        GXTex* t = &lbl_8055FE28[i];
        if (t->used) {
            fn_802E30C8("%3d: %-32s (%d x %d)", i, t, GXGetTexObjWidth(&t->obj), GXGetTexObjHeight(&t->obj));
        }
    }
}

GXTex* fn_802D7600(const char* name) {
    GXTex* t = lbl_8055FE28;
    for (int i = 0; i < 450; i++, t++) {
        if (!t->used) {
            memset(t, 0, sizeof(GXTex));
            t->used = 1;
            return t;
        }
    }
    fn_802D7564();
    fn_802E317C("dr: out of textures for %s", name);
    return 0;
}

void fn_802D768C(GXTex* t) {
    t->used = 0;
    if (t->heap) {
        fn_80128658(t->heap);
        t->heap = 0;
    }
}

int fn_802D76DC(u32 fmt) {
    switch (fmt) {
    case 0x3:
    case 0x40:
    case 0x41:
    case 0x43:
    case 0x46:
    case 0x47:
    case 0x4A:
    case 0x4B:
        return 1;
    case 0x2:
    case 0x42:
    case 0x44:
    case 0x45:
    case 0x48:
    case 0x49:
    case 0x4C:
        return 0;
    }
    fn_802E317C("drgc/tex: 0x%x is unrecognized tex format", fmt);
    return 1;
}

void fn_802D7780(GXTex* t, const GXTexDesc* d) {
    int s = d->wrap & 1;
    int tt = (d->wrap >> 1 & 1) != 0;
    if (!(d->mips & 2)) {
        GXInitTexObjFilter(&t->obj, 0, 0);
        if (t->hasAlpha) {
            GXInitTexObjFilter(&t->obj2, 0, 0);
        }
    }
    GXInitTexObjWrapMode(&t->obj, s, tt);
    if (t->hasAlpha) {
        GXInitTexObjWrapMode(&t->obj2, s, tt);
    }
    t->zhint = d->zhint;
    t->unkE4 = d->unk1C;
    t->flags = 0;
    if (d->wrapT & 1) {
        t->flags |= 4;
    }
    if (d->wrapT & 2) {
        t->flags |= 2;
    }
    if (d->wrapT == 7) {
        t->flags |= 8;
    }
    if (d->mips & 4) {
        t->flags |= 0x40;
    }
    if (fn_802D76DC((u8)d->format)) {
        t->flags |= 0x100;
    }
}

static inline u16 To565(const u8* c) { return (c[0] << 8 & 0xF800) | (c[1] << 3 & 0x7E0) | (c[2] >> 3 & 0x1F); }

void fn_802D78D4(u16* rgb, u16* alpha, const u32* pal, int n) {
    for (int i = 0; i < n; i++) {
        u32 c = pal[i];
        u8* b = (u8*)&c;
        rgb[i] = To565(b);
        alpha[i] = b[3] << 8;
    }
}

GXTex* fn_802D7B5C(const GXTexDesc* d, const char* name) {
    int ci4 = 1;
    if (d->format != 0 && d->format != 0x40) {
        ci4 = 0;
    }
    GXTex* t = d->user ? d->user : fn_802D7600(name);
    StrCopy(t->name, name, 32);
    int n = ci4 ? 16 : 256;
    u32* pal = d->data->pal;
    fn_801282EC(0x20);
    int bytes = n * 2;
    t->lut = new ("lut_data") u16[n];
    fn_801282EC(0x20);
    t->lut2 = new ("alpha_lut_data") u16[n];
    t->hasAlpha = 1;
    t->srcPal = pal;
    t->lutSize = bytes;
    fn_802D78D4(t->lut, t->lut2, pal, n);
    DCFlushRange(t->lut, t->lutSize);
    GXInitTlutObj(&t->tlut, t->lut, 1, n);
    t->lut2Size = bytes;
    DCFlushRange(t->lut2, bytes);
    GXInitTlutObj(&t->tlut2, t->lut2, 0, n);
    s32 w = d->w;
    s32 h = d->h;
    GXGetTexBufferSize(w, h, ci4 ? 8 : 9, 0, 1);
    TexImage* img = d->data;
    t->data = img->tex;
    t->dataSize = w * h / ((ci4 != 0) + 1);
    DCFlushRange(t->data, t->dataSize);
    GXInitTexObjCI(&t->obj, img->tex, w, h, ci4 ? 8 : 9, 1, 1, 0, 0);
    GXInitTexObjCI(&t->obj2, img->tex, w, h, ci4 ? 8 : 9, 1, 1, 0, 1);
    GXInvalidateTexAll();
    fn_802D7780(t, d);
    return t;
}

GXTex* fn_802D7D98(const GXTexDesc* d, const char* name) {
    int fmt = -1;
    if (d->format == 0x47 || d->format == 3) {
        fmt = 6;
    } else if (d->format == 0x45 || d->format == 2) {
        fmt = 4;
    } else {
        fn_802E317C("gx: can't create raw texture %s in format 0x%x", name, d->format);
    }
    GXTex* t = d->user ? d->user : fn_802D7600(name);
    StrCopy(t->name, name, 32);
    s32 h = d->h;
    s32 w = d->w;
    t->lut = 0;
    t->lut2 = 0;
    t->hasAlpha = 0;
    u32 size = GXGetTexBufferSize(w, h, fmt, 0, 1);
    void* data;
    if (d->mode == 3) {
        fn_801282F4();
        fn_801282EC(0x80);
        data = fn_801284E8("gc:rendertarget", size);
        t->heap = data;
    } else {
        data = d->data->tex;
    }
    t->data = data;
    t->dataSize = (fmt == 6 ? 4 : 2) * (w * h);
    DCFlushRange(t->data, t->dataSize);
    int mip = MipMapped(d->lods);
    GXInitTexObj(&t->obj, data, w, h, fmt, 1, 1, mip);
    if (mip) {
        GXInitTexObjLOD(&t->obj, 5, 1, 0.0f, (f32)(d->lods - 1), 0.0f, 1, 1, 0);
    }
    GXInvalidateTexAll();
    fn_802D7780(t, d);
    return t;
}

GXTex* fn_802D7FA0(const GXTexDesc* d, const char* name) {
    int bits = -1;
    int fmt = -1;
    switch (d->format) {
    case 0x45:
        bits = 4;
        fmt = 0;
        break;
    case 0x46:
        bits = 8;
        fmt = 1;
        break;
    case 0x47:
        bits = 8;
        fmt = 2;
        break;
    case 0x48:
        bits = 16;
        fmt = 3;
        break;
    case 0x49:
        bits = 16;
        fmt = 4;
        break;
    case 0x4A:
        bits = 16;
        fmt = 5;
        break;
    default:
        fn_802E317C("create_generic_texture: unhandled type %x", d->format);
        break;
    }
    GXTex* t = d->user ? d->user : fn_802D7600(name);
    StrCopy(t->name, name, 32);
    s32 w = d->w;
    s32 h = d->h;
    t->lut = 0;
    t->lut2 = 0;
    t->hasAlpha = 0;
    void* data = d->data->tex;
    t->data = data;
    t->dataSize = (w * h * 8) / bits;
    DCFlushRange(data, t->dataSize);
    int mip = MipMapped(d->lods);
    GXInitTexObj(&t->obj, data, w, h, fmt, 1, 1, mip);
    s32 lods = d->lods;
    if (mip && strcasecmp(t->name, "gen_waterd.tex") == 0) {
        fn_802E30C8("tex: hacking mips for %s", t);
        lods = 4;
    }
    if (mip) {
        GXInitTexObjLOD(&t->obj, 5, 1, 0.0f, (f32)(lods - 1), 0.0f, 1, 1, 0);
    }
    GXInvalidateTexAll();
    fn_802D7780(t, d);
    return t;
}

void fn_802D81C4(GXTexDesc* d) {
    s32 w = d->w;
    s32 h = d->h;
    TexImage* img = d->data;
    u8* tex = img->tex;
    int i;
    u8* p = tex;
    int n = w * h / 2;
    for (i = 0; i < n; i++) {
        *p = *p >> 4 | *p << 4;
        p++;
    }
    u32* pal = img->pal;
    for (i = 0; i < 16; i++) {
        Swap32(&pal[i]);
    }
    int row = w / 2;
    fn_801282EC(0x20);
    u8* tmp = new ("tmp_tx8") u8[row * h];
    memcpy(tmp, tex, row * h);
    int tw = row / 4;
    u8* src = tmp;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < row; x++) {
            tex[((y / 8) * tw + x / 4) * 32 + (y % 8) * 4 + x % 4] = src[x];
        }
        src += row;
    }
    delete[] tmp;
}

void fn_802D86BC(GXTexDesc* d) {
    TexImage* img = d->data;
    s32 w = d->w;
    s32 h = d->h;
    u8* tex = img->tex;
    u32* pal = img->pal;
    for (int i = 0; i < 256; i++) {
        Swap32(&pal[i]);
    }
    fn_801282EC(0x20);
    u8* tmp = new ("pal8_tmp") u8[w * h];
    memcpy(tmp, tex, w * h);
    int tw = w / 8;
    u8* src = tmp;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            tex[((y / 4) * tw + x / 8) * 32 + (y % 4) * 8 + x % 8] = src[x];
        }
        src += w;
    }
    delete[] tmp;
}

GXTex* fn_802D8A84(const GXTexDesc* d, const char* name) {
    GXTex* t = d->user ? d->user : fn_802D7600(name);
    StrCopy(t->name, name, 32);
    s32 w = d->w;
    s32 h = d->h;
    t->lut = 0;
    t->lut2 = 0;
    t->hasAlpha = 0;
    GXGetTexBufferSize(w, h, 14, 0, 1);
    void* data = d->data->tex;
    t->data = data;
    t->dataSize = w * h / 2;
    DCFlushRange(data, t->dataSize);
    int mip = MipMapped(d->lods);
    GXInitTexObj(&t->obj, data, w, h, 14, 1, 1, mip);
    if (mip) {
        GXInitTexObjLOD(&t->obj, 5, 1, 0.0f, (f32)(d->lods - 1), 0.0f, 1, 1, 0);
    }
    GXInvalidateTexAll();
    fn_802D7780(t, d);
    return t;
}

GXTex* fn_802D8C00(const GXTexDesc* d, const char* name) {
    GXTex* t = fn_802D8A84(d, name);
    s32 h = d->h;
    s32 w = d->w;
    t->hasAlpha = 1;
    GXGetTexBufferSize(w, h, 14, 0, 1);
    u8* alpha = d->data->alpha;
    DCFlushRange(alpha, w * h / 2);
    int mip = MipMapped(d->lods);
    GXInitTexObj(&t->obj2, alpha, w, h, 0, 1, 1, mip);
    if (mip) {
        GXInitTexObjLOD(&t->obj2, 5, 1, 0.0f, (f32)(d->lods - 1), 0.0f, 0, 1, 0);
    }
    fn_802D7780(t, d);
    return t;
}

void fn_802D8D1C(void) {
    lbl_8055FE24 = 0;
    lbl_8055FE20 = 0;
    lbl_8055FE1C = 0;
    lbl_8055FE18 = 0;
    lbl_8055FE28 = (GXTex*)new ("texture_list") u8[450 * sizeof(GXTex)];
    for (int i = 0; i < 450; i++) {
        memset(&lbl_8055FE28[i], 0, sizeof(GXTex));
    }
}

void fn_802D8DA0(void) {
    delete[] (u8*)lbl_8055FE28;
    lbl_8055FE28 = 0;
}

static inline void Fix(u8** p, TexImage* img) {
    Swap32(p);
    if (*p) {
        *p = (u8*)img + (u32)*p;
    }
}

void fn_802D8DCC(GXTexDesc* d) {
    TexImage* img = d->data;
    Fix((u8**)&img->pal, img);
    Fix(&img->tex, img);
    Fix(&img->alpha, img);
    switch (d->format) {
    case 0x40:
    case 0:
        fn_802D81C4(d);
        return;
    case 0x41:
    case 1:
        fn_802D86BC(d);
        return;
    }
}

void fn_802D9340(GXTex* t);

GXTex* fn_802D8E9C(const GXTexDesc* d, const char* name) {
    GXTex* t = 0;
    switch (d->format) {
    case 0:
    case 1:
    case 0x40:
    case 0x41:
        t = fn_802D7B5C(d, name);
        break;
    case 0x10:
    case 0x17:
    case 0x42:
    case 0x43:
        t = fn_802D8A84(d, name);
        break;
    case 0x11:
    case 0x44:
        t = fn_802D8C00(d, name);
        break;
    case 2:
    case 3:
    case 0x47:
        t = fn_802D7D98(d, name);
        break;
    case 0x45:
    case 0x46:
    case 0x48:
    case 0x49:
    case 0x4A:
    case 0x4B:
        t = fn_802D7FA0(d, name);
        break;
    default:
        fn_802E317C("drCreateTexture: unknown type 0x%x (%s)", d->format, name);
        break;
    }
    fn_802D9340(t);
    return t;
}

void fn_802D8FB4(GXTex* t) {
    if (t->uses && lbl_8055FD2C - t->lastFrame >= (u32)-0x100) {
        fn_802CFEEC();
    }
    if (t->lut) {
        delete[] t->lut;
    }
    if (t->lut2) {
        delete[] t->lut2;
    }
    fn_802D768C(t);
    if (lbl_8055FE18 == t) {
        lbl_8055FE18 = 0;
    }
    if (lbl_8055FE1C == t) {
        lbl_8055FE1C = 0;
    }
    if (lbl_8055FE20 == t) {
        lbl_8055FE20 = 0;
    }
}

int fn_802D9068(void) {
    for (int i = 0; i < 450; i++) {
        if (!lbl_8055FE28[i].used) {
            return 1;
        }
    }
    return 0;
}

GXTex* fn_802D9154(void) { return fn_802D7600("handle"); }

void fn_802D915C(GXTex* t) { fn_802D768C(t); }

void fn_802D9160(GXTex* t) {
    fn_802D6C68();
    lbl_8055FE18 = t;
    fn_802CFF5C(t->flags | (lbl_8055FD1C & ~0x14E));
    fn_802CFFA4(t->zhint);
    lbl_8055FD24 = t->unkE4;
    if (t->lut) {
        GXLoadTlut(&t->tlut, 0);
    }
    GXLoadTexObj(&t->obj, 0);
    if (t->hasAlpha) {
        if (t->lut2) {
            GXLoadTlut(&t->tlut2, 1);
        }
        GXLoadTexObj(&t->obj2, 1);
    }
    t->lastFrame = lbl_8055FD30;
    t->uses++;
    lbl_804C5700.base2 = t->hasAlpha;
}

void fn_802D9230(GXTex* t) {
    lbl_8055FE1C = t;
    if (t->lut) {
        fn_802E317C("dr: envmap with lut data disallowed (%s)", t);
    }
    GXLoadTexObj(&t->obj, 2);
    if (t->hasAlpha) {
        GXLoadTexObj(&t->obj2, 4);
    }
    lbl_804C5700.env2 = t->hasAlpha;
}

void fn_802D92B0(void) { fn_802D9230(lbl_8055FE1C); }

void fn_802D92B8(GXTex* t) {
    lbl_8055FE20 = t;
    if (t->hasAlpha) {
        fn_802E317C("dr: lightmap with alpha channel disallowed (%s)", t);
    }
    if (t->lut) {
        fn_802E317C("dr: lightmap with lut data disallowed (%s)", t);
    }
    GXInitTexObjWrapMode(&t->obj, 0, 0);
    GXLoadTexObj(&t->obj, 3);
}

void fn_802D9340(GXTex* t) {
    if (t->data && t->dataSize) {
        DCFlushRange(t->data, t->dataSize);
    }
    if (t->lut && t->lutSize) {
        fn_802D78D4(t->lut, t->lut2, t->srcPal, GXGetTlutObjNumEntries(&t->tlut));
        DCFlushRange(t->lut, t->lutSize);
    }
    if (t->lut2) {
        DCFlushRange(t->lut2, t->lut2Size);
    }
    GXInvalidateTexAll();
}

f32 fn_802D93E0(void) { return 0.0f; }

void fn_802D93E8(GXTex* t) {
    fn_802D2614();
    lbl_8055FE24 = t;
}

void fn_802D9418(void) {
    fn_802D2614();
    GXTexObj* o = &lbl_8055FE24->obj;
    int w = GXGetTexObjWidth(o);
    u16 h = GXGetTexObjHeight(o);
    GXSetTexCopySrc(0, 0, w, h);
    GXSetTexCopyDst(w, h, GXGetTexObjFmt(o), 0);
    GXSetZMode(1, 7, 1);
    GXSetColorUpdate(1);
    GXCopyTex(GXGetTexObjData(o), 1);
    lbl_8055FE24 = 0;
    fn_802CFF50();
}

GXTex* fn_802D94DC(void) { return lbl_8055FE18; }

void fn_802D94E4(GXTex* dst, GXTex* a, GXTex* b, f32 t) {
    int w = GXGetTexObjWidth(&dst->obj);
    int n = GXGetTexObjHeight(&dst->obj) * w * 16 / 32;
    u32* pd = (u32*)((u8*)GXGetTexObjData(&dst->obj) - 0x40000000);
    u32* pa = (u32*)((u8*)GXGetTexObjData(&a->obj) - 0x40000000);
    u32* pb = (u32*)((u8*)GXGetTexObjData(&b->obj) - 0x40000000);
    u32 kb = __cvt_fp2unsigned(256.0f * t);
    u32 ka = 0x100 - kb;
    for (int i = 0; i < n; i++) {
        u32 ca = *pa++;
        u32 cb = *pb++;
        u32 a0 = ca & 0xFF;
        u32 b0 = cb & 0xFF;
        ca >>= 8;
        u32 a1 = ca & 0xFF;
        ca >>= 8;
        cb >>= 8;
        u32 b1 = cb & 0xFF;
        cb >>= 8;
        u32 a2 = ca & 0xFF;
        u32 a3 = ca >> 8 & 0xFF;
        u32 b2 = cb & 0xFF;
        u32 b3 = cb >> 8 & 0xFF;
        u32 r0 = a0 * ka + b0 * kb;
        u32 r1 = a1 * ka + b1 * kb;
        u32 r2 = a2 * ka + b2 * kb;
        u32 r3 = a3 * ka + b3 * kb;
        *pd++ = r0 >> 8 | (r1 >> 8) << 8 | (r2 >> 8) << 16 | (r3 >> 8) << 24;
    }
}

void fn_802D9628(GXTex* t, int x, int y, u32 color) {
    int w;
    int fmt = GXGetTexObjFmt(&t->obj);
    w = GXGetTexObjWidth(&t->obj);
    GXGetTexObjHeight(&t->obj);
    u8* data = (u8*)GXGetTexObjData(&t->obj) - 0x40000000;
    if (fmt == 0) {
        u8* blk = data + (x / 8 + (y / 8) * (w / 8)) * 32;
        int off = (x % 8 + (y % 8) * 8) * 4 / 8;
        u8 v = color;
        if (x & 1) {
            blk[off] = (v >> 4) | (blk[off] & 0xF0);
        } else {
            blk[off] = (blk[off] & 0xF) | (v & 0xF0);
        }
    }
}

u32 fn_802D972C(GXTex* t, int x, int y) {
    int fmt = GXGetTexObjFmt(&t->obj);
    int w = GXGetTexObjWidth(&t->obj);
    GXGetTexObjHeight(&t->obj);
    u8* data = (u8*)GXGetTexObjData(&t->obj) - 0x40000000;
    if (fmt == 0) {
        u8* blk = data + (x / 8 + (y / 8) * (w / 8)) * 32;
        int off = ((y % 8) * 8 + x % 8) * 4 / 8;
        u32 v = x & 1 ? blk[off] & 0xF : blk[off] >> 4;
        v = v | v << 4;
        return v << 24 | (v << 16 | v) | v << 8;
    }
    if (fmt == 6) {
        u8* blk = data + (x / 4 + (y / 4) * (w / 4)) * 64;
        int off = ((y % 4) * 4 + x % 4) * 16 / 8;
        u16* p = (u16*)(blk + off);
        return p[0] << 16 | p[16];
    }
    return 0;
}

}  // extern "C"
