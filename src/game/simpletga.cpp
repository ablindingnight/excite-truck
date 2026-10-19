// Simple uncompressed TGA loading and saving (0x802B4FB0-0x802B539C). Names are guesses.
#include "game/common.h"

extern "C" {
void* memset(void* p, int c, size_t n);
void fn_802E30C8(const char* fmt, ...);  // debug print
void* fn_801284E8(const char* tag, s32 size);  // MemAlloc
void fn_80128658(void* p);                     // MemFree
s32 fn_802E3D04(const char* name);             // open for reading
s32 fn_802E3D68(const char* name);             // create
int fn_802E39D0(s32 fd, void* buf, s32 n);     // read, checked
int fn_802E3A64(s32 fd, const void* buf, s32 n);  // write
void fn_802E3734(s32* fd);                     // close
}

struct TGAHeader {
    u8 idLength;      // offset 0x0
    u8 colorMapType;  // offset 0x1
    u8 imageType;     // offset 0x2
    u8 colorMap[5];   // offset 0x3
    u16 xOrigin;      // offset 0x8
    u16 yOrigin;      // offset 0xA
    u16 width;        // offset 0xC
    u16 height;       // offset 0xE
    u8 bpp;           // offset 0x10
    u8 descriptor;    // offset 0x11
};

struct SimpleTGA {
    s32 width;    // offset 0x0
    s32 height;   // offset 0x4
    u32* pixels;  // offset 0x8
};

ModuleTag lbl_8055F9D8;

extern "C" {

int fn_802B4FB0(u32 bpp) {
    int ok = 0;
    if (bpp == 32 || bpp == 24) {
        ok = 1;
    }
    return ok;
}

int fn_802B4FD0(const char* name, s32* width, s32* height) {
    int ok = 0;
    s32 fd = fn_802E3D04(name);
    if (fd != -1) {
        TGAHeader hdr;
        if (fn_802E39D0(fd, &hdr, sizeof(hdr)) && hdr.imageType == 2 && fn_802B4FB0(hdr.bpp)) {
            ok = 1;
            *width = hdr.width;
            *height = hdr.height;
        }
        fn_802E3734(&fd);
    }
    return ok;
}

int fn_802B5070(const TGAHeader* hdr, s32 count) {
    int ok = 0;
    if (hdr->imageType == 2 && fn_802B4FB0(hdr->bpp) && count == hdr->width * hdr->height) {
        ok = 1;
    }
    return ok;
}

int fn_802B50E8(const char* name, u32* pixels, s32 count) {
    int ok = 0;
    s32 fd = fn_802E3D04(name);
    if (fd != -1) {
        TGAHeader hdr;
        if (fn_802E39D0(fd, &hdr, sizeof(hdr)) && fn_802B5070(&hdr, count)) {
            if (hdr.bpp == 32) {
                ok = fn_802E39D0(fd, pixels, count * 4);
            } else {
                int i;
                ok = 1;
                for (i = 0; ok && i < count; pixels++, i++) {
                    u32 px;
                    if (fn_802E39D0(fd, &px, 3)) {
                        *pixels = px;
                    } else {
                        ok = 0;
                    }
                }
            }
        }
        fn_802E3734(&fd);
    }
    return ok;
}

SimpleTGA* fn_802B51E4(s32 width, s32 height) {
    SimpleTGA* img = (SimpleTGA*)fn_801284E8("SimpleTGA", width * height * 4 + sizeof(SimpleTGA));
    img->width = width;
    img->height = height;
    img->pixels = (u32*)(img + 1);
    return img;
}

SimpleTGA* fn_802B5240(const char* name) {
    SimpleTGA* img = 0;
    s32 width;
    s32 height;
    if (fn_802B4FD0(name, &width, &height)) {
        img = fn_802B51E4(width, height);
        fn_802B50E8(name, img->pixels, img->width * img->height);
    }
    return img;
}

void fn_802B52B4(SimpleTGA* img) {
    img->pixels = 0;
    fn_80128658(img);
}

int fn_802B52C0(const char* name, const SimpleTGA* img) {
    int ok = 0;
    TGAHeader hdr;
    s32 fd;
    memset(&hdr, 0, sizeof(hdr));
    hdr.imageType = 2;
    hdr.width = img->width;
    hdr.height = img->height;
    hdr.bpp = 32;
    fd = fn_802E3D68(name);
    if (fd != -1) {
        fn_802E3A64(fd, &hdr, sizeof(hdr));
        fn_802E3A64(fd, img->pixels, img->width * 4 * img->height);
        fn_802E3734(&fd);
        ok = 1;
    } else {
        fn_802E30C8("save_simple_tga: Can't create \"%s\"", name);
    }
    return ok;
}

}  // extern "C"
