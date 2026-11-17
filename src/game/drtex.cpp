// GX texture driver (0x802D7564-0x802D98F8, "drgc/tex"): the 450-entry GX texture table behind
// texture.cpp - creating GXTexObjs (and alpha companions) from tt_texture descriptors, converting
// palettes to RGB565/IA8 TLUTs, swizzling raw 4/8-bit images into the GX tiled layouts, binding
// textures to TEXMAP0/1 (base), 2/4 (envmap) and 3 (lightmap), render-target copies, and texel
// peek/poke. Names are guesses apart from the strings.
#include "game/common.h"
#include "game/gxtex.h"
#include <revolution/GX/GXTypes.h>
#include <revolution/GX/GXFrameBuf.h>
#include <revolution/GX/GXPixel.h>
#include <revolution/GX/GXTexture.h>
#include <revolution/OS/OSCache.h>
#include "game/byteswap.h"
#include "game/strcopy.h"

void* operator new[](size_t size, const char* tag);

// GXTexObj: the SDK's.

// GXTlutObj: the SDK's.

extern "C" {
void* memset(void* p, int c, u32 n);
void* memcpy(void* d, const void* s, u32 n);
int strcasecmp(const char* a, const char* b);
// GXCITexFmt: the SDK's.
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

// Unit-local (port ODR: other units define a different TexImage).
namespace {
struct TexImage {
    u32* pal;    // offset 0x0
    u8* tex;     // offset 0x4
    u8* alpha;   // offset 0x8
};
}  // namespace

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
extern const u16 lbl_8055FE12 = TexShort(0x405);
u16 lbl_8055FE14 = TexShort(0x401);
GXTex* lbl_8055FE18;  // bound base texture
GXTex* lbl_8055FE1C;  // bound envmap
GXTex* lbl_8055FE20;  // bound lightmap
GXTex* lbl_8055FE24;  // render target
GXTex* lbl_8055FE28;  // texture table


static inline u8 MipMapped(s32 n) { return n > 2; }

extern "C" {

void fn_802D7564(void) {
    fn_802E30C8(" === DUMPING TEXTURES ===");
    for (int i = 0; i < 450; i++) {
        GXTex* t = &lbl_8055FE28[i];
        if (t->used) {
            fn_802E30C8("%3d: %-32s (%d x %d)", i, t, GXGetTexObjWidth((const GXTexObj*)&t->obj), GXGetTexObjHeight((const GXTexObj*)&t->obj));
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
    int tt = (d->wrap & 2) >> 1 != 0;
    if (!(d->mips & 2)) {
        GXInitTexObjFilter((GXTexObj*)&t->obj, (GXTexFilter)0, (GXTexFilter)0);
        if (t->hasAlpha) {
            GXInitTexObjFilter((GXTexObj*)&t->obj2, (GXTexFilter)0, (GXTexFilter)0);
        }
    }
    GXInitTexObjWrapMode((GXTexObj*)&t->obj, (GXTexWrapMode)s, (GXTexWrapMode)tt);
    if (t->hasAlpha) {
        GXInitTexObjWrapMode((GXTexObj*)&t->obj2, (GXTexWrapMode)s, (GXTexWrapMode)tt);
    }
    t->zhint = d->zhint;
    t->alphaRef = d->unk1C;
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

static inline u16 To565(const u8* c) { return (c[0] << 8 & 0xF800) | (c[1] << 3 & 0x7E0) | ((c[2] & 0xF8) >> 3); }

void fn_802D78D4(u16* rgb, u16* alpha, const u32* pal, int n) {
    for (int i = 0; i < n; i++) {
        union {
            u32 word;
            u8 b[4];
        } c;
        c.word = pal[i];
        rgb[i] = To565(c.b);
        alpha[i] = c.b[3] << 8;
    }
}

GXTex* fn_802D7B5C(const GXTexDesc* d, const char* name) {
    int ci4 = 1;
    if (d->format != 0 && d->format != 0x40) {
        ci4 = 0;
    }
    GXTex* t = d->user ? d->user : fn_802D7600(name);
    SafeStrCopy(t->name, name, 32);
    int n = ci4 ? 16 : 256;
    u32* pal = d->data->pal;
    fn_801282EC(0x20);
    t->lut = new ("lut_data") u16[n];
    fn_801282EC(0x20);
    t->lut2 = new ("alpha_lut_data") u16[n];
    t->hasAlpha = 1;
    t->srcPal = pal;
    t->lutSize = n * 2;
    fn_802D78D4(t->lut, t->lut2, pal, n);
    DCFlushRange((const void*)t->lut, (u32)t->lutSize);
    GXInitTlutObj((GXTlutObj*)&t->tlut, (void*)t->lut, (GXTlutFmt)1, (u16)n);
    t->lut2Size = n * 2;
    DCFlushRange((const void*)t->lut2, (u32)(n * 2));
    GXInitTlutObj((GXTlutObj*)&t->tlut2, (void*)t->lut2, (GXTlutFmt)0, (u16)n);
    s32 w = d->w;
    s32 h = d->h;
    GXGetTexBufferSize((u16)w, (u16)h, (u32)(ci4 ? GX_TF_C4 : GX_TF_C8), (GXBool)0, (u8)1);
    TexImage* img = d->data;
    t->data = img->tex;
    t->dataSize = w * h / ((ci4 != 0) + 1);
    DCFlushRange((const void*)t->data, (u32)t->dataSize);
    GXInitTexObjCI((GXTexObj*)&t->obj, (void*)img->tex, (u16)w, (u16)h, (GXTexFmt)(ci4 ? GX_TF_C4 : GX_TF_C8), (GXTexWrapMode)1, (GXTexWrapMode)1, (GXBool)0, (u32)0);
    GXInitTexObjCI((GXTexObj*)&t->obj2, (void*)img->tex, (u16)w, (u16)h, (GXTexFmt)(ci4 ? GX_TF_C4 : GX_TF_C8), (GXTexWrapMode)1, (GXTexWrapMode)1, (GXBool)0, (u32)1);
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
    SafeStrCopy(t->name, name, 32);
    s32 w = d->w;
    s32 h = d->h;
    t->lut = 0;
    t->lut2 = 0;
    t->hasAlpha = 0;
    u32 size = GXGetTexBufferSize((u16)w, (u16)h, (u32)fmt, (GXBool)0, (u8)1);
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
    DCFlushRange((const void*)t->data, (u32)t->dataSize);
    int mip = MipMapped(d->lods);
    GXInitTexObj((GXTexObj*)&t->obj, (void*)data, (u16)w, (u16)h, (GXTexFmt)fmt, (GXTexWrapMode)1, (GXTexWrapMode)1, (GXBool)mip);
    if (mip) {
        GXInitTexObjLOD((GXTexObj*)&t->obj, (GXTexFilter)5, (GXTexFilter)1, 0.0f, (f32)(d->lods - 1), -0.7f, (GXBool)1, (GXBool)1, (GXAnisotropy)0);
    }
    GXInvalidateTexAll();
    fn_802D7780(t, d);
    return t;
}

GXTex* create_generic_texture(const GXTexDesc* d, const char* name) {
    int fmt = -1;
    int bits = -1;
    switch (d->format) {
    case 0x48:  // I4
        bits = 4;
        fmt = 0;
        break;
    case 0x49:  // I8
        bits = 8;
        fmt = 1;
        break;
    case 0x4A:  // IA4
        bits = 8;
        fmt = 2;
        break;
    case 0x4B:  // IA8
        bits = 16;
        fmt = 3;
        break;
    case 0x45:  // RGB565
        bits = 16;
        fmt = 4;
        break;
    case 0x46:  // RGB5A3
        bits = 16;
        fmt = 5;
        break;
    default:
        fn_802E317C("create_generic_texture: unhandled type %x", d->format);
        break;
    }
    GXTex* t = d->user ? d->user : fn_802D7600(name);
    SafeStrCopy(t->name, name, 32);
    s32 w = d->w;
    s32 h = d->h;
    t->lut = 0;
    t->lut2 = 0;
    t->hasAlpha = 0;
    void* data = d->data->tex;
    t->data = data;
    t->dataSize = (w * h * 8) / bits;
    DCFlushRange((const void*)data, (u32)t->dataSize);
    int mip = MipMapped(d->lods);
    GXInitTexObj((GXTexObj*)&t->obj, (void*)data, (u16)w, (u16)h, (GXTexFmt)fmt, (GXTexWrapMode)1, (GXTexWrapMode)1, (GXBool)mip);
    s32 lods = d->lods;
    if (mip && strcasecmp(t->name, "gen_waterd.tex") == 0) {
        fn_802E30C8("tex: hacking mips for %s", t);
        lods = 4;
    }
    if (mip) {
        GXInitTexObjLOD((GXTexObj*)&t->obj, (GXTexFilter)5, (GXTexFilter)1, 0.0f, (f32)(lods - 1), -0.7f, (GXBool)1, (GXBool)1, (GXAnisotropy)0);
    }
    GXInvalidateTexAll();
    fn_802D7780(t, d);
    return t;
}

void fn_802D81C4(GXTexDesc* d) {
    const u8* wa;
    s32 w = d->w;
    u8* tex;
    s32 h = d->h;
    TexImage* img = d->data;
    tex = img->tex;
    int i;
    u8* p = tex;
    for (i = 0; i < w * h / 2; i++) {
        *p = *p >> 4 | *p << 4;
        p++;
    }
    u32* pal = img->pal;
    for (i = 0; i < 16; i++) {
        SwapWordV(&pal[i]);
    }
    int row = w / 2;
    fn_801282EC(0x20);
    u8* tmp = new ("tmp_tx8") u8[row * h];
    memcpy(tmp, tex, row * h);
    int tw = w / 2 / 4;
    const u8* src = tmp;
    int y;
    for (y = 0; y < h; y++) {
        wa = src;
        for (int x = 0; x < w / 2; wa++, x++) {
            tex[((y / 8) * tw + x / 4) * 32 + (y % 8) * 4 + x % 4] = *wa;
        }
        src += row;
    }
    delete[] tmp;
}

void fn_802D86BC(GXTexDesc* d) {
    const u8* wa;
    TexImage* img = d->data;
    s32 w = d->w;
    s32 h = d->h;
    u8* tex = img->tex;
    u32* pal = img->pal;
    for (int i = 0; i < 256; i++) {
        SwapWordV(&pal[i]);
    }
    fn_801282EC(0x20);
    u8* tmp = new ("pal8_tmp") u8[w * h];
    memcpy(tmp, tex, w * h);
    int tw = w / 8;
    int off = 0;
    for (int y = 0; y < h; y++) {
        wa = tmp + off;
        for (int x = 0; x < w; wa++, x++) {
            tex[((y / 4) * tw + x / 8) * 32 + (y % 4) * 8 + x % 8] = *wa;
        }
        off += w;
    }
    delete[] tmp;
}

GXTex* fn_802D8A84(const GXTexDesc* d, const char* name) {
    GXTex* t = d->user ? d->user : fn_802D7600(name);
    SafeStrCopy(t->name, name, 32);
    s32 w = d->w;
    s32 h = d->h;
    t->lut = 0;
    t->lut2 = 0;
    GXGetTexBufferSize((u16)w, (u16)h, (u32)14, (GXBool)0, (u8)1);
    void* data = d->data->tex;
    t->data = data;
    t->dataSize = w * h / 2;
    DCFlushRange((const void*)data, (u32)t->dataSize);
    int mip = MipMapped(d->lods);
    GXInitTexObj((GXTexObj*)&t->obj, (void*)data, (u16)w, (u16)h, (GXTexFmt)14, (GXTexWrapMode)1, (GXTexWrapMode)1, (GXBool)mip);
    if (mip) {
        GXInitTexObjLOD((GXTexObj*)&t->obj, (GXTexFilter)5, (GXTexFilter)1, 0.0f, (f32)(d->lods - 1), -0.7f, (GXBool)1, (GXBool)1, (GXAnisotropy)0);
    }
    GXInvalidateTexAll();
    fn_802D7780(t, d);
    return t;
}

GXTex* fn_802D8C00(const GXTexDesc* d, const char* name) {
    GXTex* t = fn_802D8A84(d, name);
    s32 w = d->w;
    s32 h = d->h;
    t->hasAlpha = 1;
    GXGetTexBufferSize((u16)w, (u16)h, (u32)14, (GXBool)0, (u8)1);
    u8* alpha = d->data->alpha;
    DCFlushRange((const void*)alpha, (u32)(w * h / 2));
    int mip = MipMapped(d->lods);
    GXInitTexObj((GXTexObj*)&t->obj2, (void*)alpha, (u16)w, (u16)h, (GXTexFmt)0, (GXTexWrapMode)1, (GXTexWrapMode)1, (GXBool)mip);
    if (mip) {
        GXInitTexObjLOD((GXTexObj*)&t->obj2, (GXTexFilter)5, (GXTexFilter)1, 0.0f, (f32)(d->lods - 1), -0.7f, (GXBool)0, (GXBool)1, (GXAnisotropy)0);
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
    SwapWordV(p);
    if (*p) {
        *p = (u8*)img + (u32)*p;
    }
}

/* Helper name unproven: turns one texture image's offsets into pointers. */
static inline void FixTexImage(TexImage* img) {
    Fix((u8**)&img->pal, img);
    Fix(&img->tex, img);
    Fix(&img->alpha, img);
}

void fn_802D8DCC(GXTexDesc* d, const char*) {
    FixTexImage(d->data);
    u32 fmt = d->format;
    if (fmt == 0x40 || fmt == 0) {
        fn_802D81C4(d);
    } else if (fmt == 0x41 || fmt == 1) {
        fn_802D86BC(d);
    }
}

void fn_802D9340(GXTex* t);

GXTex* drCreateTexture(const GXTexDesc* d, const char* name) {
    GXTex* t = 0;
    switch (d->format) {
    case 0x45:
    case 0x46:
    case 0x48:
    case 0x49:
    case 0x4A:
    case 0x4B:
        t = create_generic_texture(d, name);
        break;
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

void fn_802D9160(GXTex* t, int) {
    fn_802D6C68();
    lbl_8055FE18 = t;
    fn_802CFF5C(t->flags | (lbl_8055FD1C & ~0x14E));
    fn_802CFFA4(t->zhint);
    lbl_8055FD24 = t->alphaRef;
    if (t->lut) {
        GXLoadTlut((GXTlutObj*)&t->tlut, (u32)0);
    }
    GXLoadTexObj((const GXTexObj*)&t->obj, (GXTexMapID)0);
    if (t->hasAlpha) {
        if (t->lut2) {
            GXLoadTlut((GXTlutObj*)&t->tlut2, (u32)1);
        }
        GXLoadTexObj((const GXTexObj*)&t->obj2, (GXTexMapID)1);
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
    GXLoadTexObj((const GXTexObj*)&t->obj, (GXTexMapID)2);
    if (t->hasAlpha) {
        GXLoadTexObj((const GXTexObj*)&t->obj2, (GXTexMapID)4);
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
    GXInitTexObjWrapMode((GXTexObj*)&t->obj, (GXTexWrapMode)0, (GXTexWrapMode)0);
    GXLoadTexObj((const GXTexObj*)&t->obj, (GXTexMapID)3);
}

void fn_802D9340(GXTex* t) {
    if (t->data && t->dataSize) {
        DCFlushRange((const void*)t->data, (u32)t->dataSize);
    }
    if (t->lut && t->lutSize) {
        fn_802D78D4(t->lut, t->lut2, t->srcPal, GXGetTlutObjNumEntries((const GXTlutObj*)&t->tlut));
        DCFlushRange((const void*)t->lut, (u32)t->lutSize);
    }
    if (t->lut2) {
        DCFlushRange((const void*)t->lut2, (u32)t->lut2Size);
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
    int w = GXGetTexObjWidth((const GXTexObj*)o);
    int h = GXGetTexObjHeight((const GXTexObj*)o);
    GXSetTexCopySrc((u16)0, (u16)0, (u16)w, (u16)h);
    GXSetTexCopyDst((u16)w, (u16)h, (GXTexFmt)(GXGetTexObjFmt(o)), (GXBool)0);
    GXSetZMode((GXBool)1, (GXCompare)7, (GXBool)1);
    GXSetColorUpdate((GXBool)1);
    GXCopyTex((void*)(GXGetTexObjData((const GXTexObj*)o)), (GXBool)1);
    lbl_8055FE24 = 0;
    fn_802CFF50();
}

GXTex* fn_802D94DC(void) { return lbl_8055FE18; }

void fn_802D94E4(GXTex* dst, GXTex* a, GXTex* b, f32 t) {
    int n = GXGetTexObjHeight((const GXTexObj*)&dst->obj) * GXGetTexObjWidth((const GXTexObj*)&dst->obj) * 16 / 32;
    u32* pd = (u32*)((u8*)GXGetTexObjData((const GXTexObj*)&dst->obj) - 0x40000000);
    u32* pa = (u32*)((u8*)GXGetTexObjData((const GXTexObj*)&a->obj) - 0x40000000);
    u32* pb = (u32*)((u8*)GXGetTexObjData((const GXTexObj*)&b->obj) - 0x40000000);
    u32 kb = __cvt_fp2unsigned(256.0f * t);
    u32 ka = 0x100 - kb;
    for (int i = 0; i < n; i++) {
        u32 cb;
        u32 ca = *pa++;
        cb = *pb++;
        u32 b0 = cb & 0xFF;
        u32 a0 = ca & 0xFF;
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
    int fmt = GXGetTexObjFmt((const GXTexObj*)&t->obj);
    w = GXGetTexObjWidth((const GXTexObj*)&t->obj);
    GXGetTexObjHeight((const GXTexObj*)&t->obj);
    u8* data = (u8*)GXGetTexObjData((const GXTexObj*)&t->obj) - 0x40000000;
    if (fmt == 0) {
        u8* p = data + (x / 8 + (y / 8) * (w / 8)) * 32 + ((y % 8) * 8 + x % 8) * 4 / 8;
        u8 v = color;
        if (x & 1) {
            *p = (*p & 0xF0) | (v >> 4);
        } else {
            *p = (*p & 0xF) | (v & 0xF0);
        }
    }
}

u32 fn_802D972C(GXTex* t, int x, int y) {
    int fmt = GXGetTexObjFmt((const GXTexObj*)&t->obj);
    int w = GXGetTexObjWidth((const GXTexObj*)&t->obj);
    GXGetTexObjHeight((const GXTexObj*)&t->obj);
    u8* data = (u8*)GXGetTexObjData((const GXTexObj*)&t->obj) - 0x40000000;
    if (fmt == 0) {
        u8* p = data + (x / 8 + (y / 8) * (w / 8)) * 32 + ((y % 8) * 8 + x % 8) * 4 / 8;
        u32 v = (x & 1) == 0 ? *p >> 4 & 0xF : *p & 0xF;
        u32 c = v | v << 4;
        return c | c << 8 | c << 16 | c << 24;
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
