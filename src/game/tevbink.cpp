// TEV presets and Bink movies (0x802DEAC4-0x802E1728): the fixed texture-environment setups (and their
// cleanups) that the GX state code picks per surface mode - envmaps, lightmaps, specular, indirect
// warps, the ground layers - followed by the Bink movie player on its own SimpleHeap: open/close,
// per-frame decode, and drawing the frame as a YUV quad. Names are guesses apart from the strings.
#include "game/common.h"

struct GXTexObj {
    u32 dummy[8];
};

struct GXColor {
    u8 r, g, b, a;
};

struct GXColorS10 {
    s16 r, g, b, a;
};

extern "C" {
void GXSetTexCoordGen2(int dst, int type, int src, u32 mtx, u8 norm, u32 pt);
void GXSetNumTexGens(u8 n);
void GXSetNumTevStages(u8 n);
void GXSetNumIndStages(u8 n);
void GXSetTevOrder(int stage, int coord, int map, int color);
void GXSetTevSwapMode(int stage, int ras, int tex);
void GXSetTevSwapModeTable(int table, int r, int g, int b, int a);
void GXSetTevColorIn(int stage, int a, int b, int c, int d);
void GXSetTevColorOp(int stage, int op, int bias, int scale, u8 clamp, int reg);
void GXSetTevAlphaIn(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp(int stage, int op, int bias, int scale, u8 clamp, int reg);
void GXSetTevKColorSel(int stage, int sel);
void GXSetTevKAlphaSel(int stage, int sel);
void GXSetTevKColor(int reg, GXColor c);
void GXSetTevColorS10(int reg, GXColorS10 c);
void GXSetTevDirect(int stage);
void GXSetTevIndirect(int stage, int ind, int fmt, int bias, int mtx, int ws, int wt, u8 addPrev, u8 utcLod,
                      int alpha);
void GXSetIndTexOrder(int stage, int coord, int map);
void GXSetBlendMode(int type, int src, int dst, int op);
void GXSetZMode(u8 test, int func, u8 update);
void GXSetColorUpdate(u8 on);
void GXSetAlphaCompare(int c0, u8 r0, int op, int c1, u8 r1);
void GXLoadTexObj(const GXTexObj* obj, int map);
GXTexObj* fn_802D30E4(void);
GXTexObj* fn_802D30F0(void);
GXTexObj* fn_802D30FC(void);
GXTexObj* fn_802D3100(void);
GXTexObj* fn_802D312C(void);
GXTexObj* fn_802D3130(void);
GXTexObj* fn_802D3134(void);
GXTexObj* fn_802D3138(void);
void fn_802D3144(void);
void fn_802D3360(void);
void fn_802D007C(f32 a);
void fn_802C8C04(int a);
void fn_802C8DB8(void* verts, int nverts, const u16* idx, int n);
void fn_802C8D74(void);
void fn_802E317C(const char* fmt, ...);  // fatal
void fn_802E30C8(const char* fmt, ...);  // warning print
int fn_802E2C20(void);                   // milliseconds
int fn_801345D8(char* dst, int n, const char* fmt, ...);
char* strrchr(const char* s, int c);
void fn_801282EC(int align);
void fn_801282F4(void);
int fn_802E36D8(const char* path);  // open
void fn_802E3804(int fd, void* buf, u32 n);
void fn_802E3734(int* fd);
void* fn_8011770C(const char* name, u32 flags);  // BinkOpen
void fn_80117618(void* bink, void* info);
void fn_801176FC(void* bink, void* info);
void fn_80118DC0(void* bink);  // BinkClose
void fn_801186B8(void* bink);  // BinkDoFrame
void fn_80118C44(void* bink);  // BinkNextFrame
int fn_802ECC10(void);
void fn_80119214(void* (*alloc)(u32), void (*free)(void*));
void fn_80116E54(void* fn, void* arg);
void fn_8011A168(void);
void fn_802E1728(void* tex);
int fn_802E1BB4(void* tex);
void fn_802E27B8(void* tex);
void fn_802E27A0(void* tex);
void fn_802E1DA4(void* tex, u32 w, u32 h, f32 x, f32 y, f32 sx, f32 sy, f32 a);
}

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);
void operator delete(void* p);
void operator delete[](void* p);

extern "C" {

void fn_802DEAC4(void) {
    GXSetTexCoordGen2(0, 1, 0, 33, 0, 125);
    GXSetTexCoordGen2(1, 1, 0, 36, 0, 125);
    GXSetTexCoordGen2(2, 1, 0, 39, 0, 125);
    GXSetTexCoordGen2(3, 1, 0, 30, 0, 125);
    GXSetNumTexGens(4);
    GXSetTevOrder(0, 3, 1, 255);
    GXSetTevOrder(1, 0, 2, 255);
    GXSetTevOrder(2, 0, 3, 255);
    GXSetTevOrder(3, 2, 0, 255);
    GXSetTevOrder(4, 1, 4, 255);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevSwapMode(2, 0, 0);
    GXSetTevSwapMode(3, 0, 0);
    GXSetTevSwapMode(4, 0, 0);
    GXSetTevColorIn(0, 8, 15, 15, 15);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 7, 7, 7);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(1, 8, 15, 15, 15);
    GXSetTevColorOp(1, 0, 0, 0, 1, 1);
    GXSetTevAlphaIn(1, 7, 7, 7, 0);
    GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
    GXSetTevColorIn(2, 0, 2, 9, 15);
    GXSetTevColorOp(2, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(2, 7, 7, 7, 0);
    GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
    GXSetTevColorIn(3, 15, 0, 8, 15);
    GXSetTevColorOp(3, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(3, 7, 7, 7, 0);
    GXSetTevAlphaOp(3, 0, 0, 0, 1, 0);
    GXSetTevColorIn(4, 15, 15, 15, 0);
    GXSetTevColorOp(4, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(4, 4, 7, 7, 7);
    GXSetTevAlphaOp(4, 0, 0, 0, 1, 0);
    GXSetNumTevStages(5);
}

void fn_802DEE1C(void) {
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevSwapMode(2, 0, 0);
    GXSetTevSwapMode(3, 0, 0);
    GXSetTevSwapMode(4, 0, 0);
}

void fn_802DEE88(void) {
    GXSetTexCoordGen2(0, 1, 4, 60, 0, 125);
    GXSetNumTexGens(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevColorIn(0, 8, 15, 15, 15);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 5, 7, 7, 7);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetNumTevStages(1);
    GXSetBlendMode(1, 4, 5, 0);
}

void fn_802DEF70(void) {
    GXSetTevSwapMode(0, 0, 0);
    fn_802D3144();
}

void fn_802DEFA0(void) {
    GXSetTexCoordGen2(0, 0, 1, 42, 1, 76);
    GXSetTexCoordGen2(1, 1, 1, 33, 0, 125);
    GXSetNumTexGens(2);
    GXSetIndTexOrder(0, 0, 5);
    GXSetNumIndStages(1);
    GXSetTevOrder(0, 1, 7, 4);
    GXSetTevOrder(1, 255, 6, 255);
    GXSetTevSwapModeTable(1, 2, 2, 2, 0);
    GXSetTevSwapMode(0, 0, 1);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevColorIn(0, 15, 10, 6, 15);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 4, 7, 7, 7);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(1, 0, 8, 9, 15);
    GXSetTevColorOp(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(1, 7, 7, 7, 0);
    GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
    GXSetTevIndirect(1, 0, 0, 0, 1, 6, 6, 0, 0, 0);
    GXSetNumTevStages(2);
    GXSetBlendMode(1, 4, 0, 0);
    GXLoadTexObj(fn_802D312C(), 5);
    GXLoadTexObj(fn_802D30E4(), 7);
    GXLoadTexObj(fn_802D30F0(), 6);
}

void fn_802DF1B4(void) {
    GXSetTevSwapModeTable(1, 0, 0, 0, 3);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetNumIndStages(0);
    GXSetTevDirect(1);
}

void fn_802DF21C(void) {
    GXSetTexCoordGen2(0, 1, 1, 42, 0, 125);
    GXSetNumTexGens(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevColorIn(0, 15, 10, 8, 15);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 7, 7, 7);
    GXSetTevAlphaOp(0, 0, 1, 1, 1, 0);
    GXSetNumTevStages(1);
}

void fn_802DF2F0(void) {
    GXSetTevSwapMode(0, 0, 0);
}

void fn_802DF300(void) {
    GXSetTexCoordGen2(0, 1, 0, 45, 0, 125);
    GXSetNumTexGens(1);
    GXSetTevOrder(0, 255, 255, 4);
    GXSetTevOrder(1, 0, 7, 255);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevColorIn(0, 15, 10, 6, 15);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 7, 7, 7);
    GXSetTevAlphaOp(0, 0, 1, 1, 1, 0);
    GXSetTevColorIn(1, 0, 8, 9, 15);
    GXSetTevColorOp(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(1, 7, 7, 7, 0);
    GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
    GXSetNumTevStages(2);
    GXLoadTexObj(fn_802D3138(), 7);
}

void fn_802DF46C(void) {
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
}

void fn_802DF4A8(void) {
    GXSetTexCoordGen2(0, 1, 4, 60, 0, 125);
    GXSetTexCoordGen2(1, 0, 1, 42, 1, 76);
    GXSetNumTexGens(2);
    GXSetIndTexOrder(0, 1, 5);
    GXSetNumIndStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevOrder(1, 255, 6, 255);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevColorIn(0, 15, 10, 8, 15);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 7, 7, 7);
    GXSetTevAlphaOp(0, 0, 1, 1, 1, 0);
    GXSetTevColorIn(1, 0, 8, 9, 15);
    GXSetTevColorOp(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(1, 7, 7, 7, 0);
    GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
    GXSetTevIndirect(1, 0, 0, 0, 1, 6, 6, 0, 0, 0);
    GXSetNumTevStages(2);
    GXLoadTexObj(fn_802D3130(), 5);
    GXLoadTexObj(fn_802D3100(), 6);
}

void fn_802DF684(void) {
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetNumIndStages(0);
    GXSetTevDirect(1);
}

void fn_802DF6D0(void) {
    GXSetTexCoordGen2(0, 1, 4, 60, 0, 125);
    GXSetTexCoordGen2(1, 0, 1, 42, 1, 76);
    GXSetNumTexGens(2);
    GXSetIndTexOrder(0, 1, 5);
    GXSetNumIndStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevOrder(1, 255, 6, 255);
    GXSetTevOrder(2, 0, 1, 255);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevSwapMode(2, 0, 0);
    GXSetTevColorIn(0, 15, 8, 10, 15);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 7, 7, 7);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(1, 0, 8, 9, 15);
    GXSetTevColorOp(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(1, 7, 7, 7, 0);
    GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
    GXSetTevIndirect(1, 0, 0, 0, 1, 6, 6, 0, 0, 0);
    GXSetTevColorIn(2, 15, 15, 15, 0);
    GXSetTevColorOp(2, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(2, 4, 7, 7, 7);
    GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
    GXSetNumTevStages(3);
    GXLoadTexObj(fn_802D3134(), 5);
    GXLoadTexObj(fn_802D30FC(), 6);
}

void fn_802DF938(void) {
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevSwapMode(2, 0, 0);
    GXSetNumIndStages(0);
    GXSetTevDirect(1);
}

void fn_802DF994(void) {
    GXSetTexCoordGen2(0, 1, 4, 60, 0, 125);
    GXSetNumTexGens(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevOrder(1, 255, 255, 5);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevColorIn(0, 15, 8, 10, 15);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 7, 7, 7);
    GXSetTevAlphaOp(0, 0, 1, 1, 1, 0);
    GXSetTevColorIn(1, 15, 10, 5, 0);
    GXSetTevColorOp(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(1, 7, 7, 7, 0);
    GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
    GXSetNumTevStages(2);
}

void fn_802DFAF4(void) {
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
}

void fn_802DFB30(void) {
    GXSetTexCoordGen2(0, 1, 0, 30, 0, 125);
    GXSetTexCoordGen2(1, 1, 0, 30, 0, 125);
    GXSetNumTexGens(2);
    GXSetTevOrder(0, 1, 1, 255);
    GXSetTevOrder(1, 0, 0, 255);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevColorIn(0, 8, 15, 15, 15);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 1, 7, 7, 7);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(1, 15, 0, 8, 15);
    GXSetTevColorOp(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(1, 7, 7, 7, 0);
    GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
    GXSetNumTevStages(2);
    GXSetBlendMode(1, 4, 5, 0);
}

void fn_802DFCC0(void) {
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    fn_802D3144();
}

void fn_802DFD00(void) {
    GXSetTexCoordGen2(0, 1, 0, 30, 0, 125);
    GXSetTexCoordGen2(1, 1, 4, 60, 0, 125);
    GXSetNumTexGens(2);
    GXSetTevOrder(0, 1, 1, 255);
    GXSetTevOrder(1, 0, 0, 255);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevColorIn(0, 8, 15, 15, 15);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevKAlphaSel(0, 29);
    GXSetTevAlphaIn(0, 7, 4, 6, 7);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(1, 15, 0, 8, 15);
    GXSetTevColorOp(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(1, 7, 7, 7, 0);
    GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
    GXSetNumTevStages(2);
}

void fn_802DFE88(void) {
    GXSetTexCoordGen2(0, 1, 0, 30, 0, 125);
    GXSetTexCoordGen2(1, 1, 4, 60, 0, 125);
    GXSetNumTexGens(2);
    GXSetTevOrder(0, 1, 1, 255);
    GXSetTevOrder(1, 0, 0, 255);
    GXSetTevOrder(2, 1, 2, 255);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevSwapMode(2, 0, 0);
    GXSetTevColorIn(0, 8, 15, 15, 15);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 7, 7, 7);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(1, 15, 0, 8, 15);
    GXSetTevColorOp(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(1, 7, 7, 7, 0);
    GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
    GXSetTevColorIn(2, 15, 15, 15, 0);
    GXSetTevColorOp(2, 0, 0, 0, 1, 0);
    GXSetTevKAlphaSel(2, 29);
    GXSetTevAlphaIn(2, 7, 4, 6, 7);
    GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
    GXSetNumTevStages(3);
}

void fn_802E009C(void) {
    GXSetTexCoordGen2(0, 1, 5, 60, 0, 125);
    GXSetTexCoordGen2(1, 1, 4, 60, 0, 125);
    GXSetNumTexGens(2);
    GXSetTevOrder(0, 1, 1, 4);
    GXSetTevOrder(1, 0, 0, 255);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevColorIn(0, 8, 15, 15, 15);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 4, 5, 7);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(1, 15, 0, 8, 15);
    GXSetTevColorOp(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(1, 7, 7, 7, 0);
    GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
    GXSetNumTevStages(2);
}

void fn_802E0218(void) {
    GXSetTexCoordGen2(0, 1, 5, 60, 0, 125);
    GXSetTexCoordGen2(1, 1, 4, 60, 0, 125);
    GXSetNumTexGens(2);
    GXSetTevOrder(0, 1, 1, 255);
    GXSetTevOrder(1, 0, 0, 255);
    GXSetTevOrder(2, 1, 2, 4);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevSwapMode(2, 0, 0);
    GXSetTevColorIn(0, 8, 15, 15, 15);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 7, 7, 7);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(1, 15, 0, 8, 15);
    GXSetTevColorOp(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(1, 7, 7, 7, 0);
    GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
    GXSetTevColorIn(2, 15, 15, 15, 0);
    GXSetTevColorOp(2, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(2, 7, 4, 5, 7);
    GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
    GXSetNumTevStages(3);
}

void fn_802E0420(void) {
    GXSetTexCoordGen2(0, 1, 4, 60, 0, 125);
    GXSetNumTexGens(1);
    GXSetTevOrder(0, 0, 1, 255);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevKColorSel(0, 13);
    GXSetTevColorIn(0, 15, 8, 14, 15);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevKAlphaSel(0, 29);
    GXSetTevAlphaIn(0, 7, 4, 6, 7);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetNumTevStages(1);
}

void fn_802E050C(void) {
    GXSetTexCoordGen2(0, 1, 4, 60, 0, 125);
    GXSetNumTexGens(1);
    GXSetTevOrder(0, 0, 1, 255);
    GXSetTevOrder(1, 0, 2, 255);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevKColorSel(0, 13);
    GXSetTevColorIn(0, 15, 8, 14, 15);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 7, 7, 7);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(1, 15, 15, 15, 0);
    GXSetTevColorOp(1, 0, 0, 0, 1, 0);
    GXSetTevKAlphaSel(1, 29);
    GXSetTevAlphaIn(1, 7, 4, 6, 7);
    GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
    GXSetNumTevStages(2);
}

void fn_802E0684(void) {
    GXSetTexCoordGen2(0, 1, 0, 33, 0, 125);
    GXSetTexCoordGen2(1, 1, 0, 30, 0, 125);
    GXSetTexCoordGen2(2, 1, 0, 39, 0, 125);
    GXSetNumTexGens(3);
    GXSetTevOrder(0, 1, 1, 255);
    GXSetTevOrder(1, 0, 3, 255);
    GXSetTevOrder(2, 0, 4, 255);
    GXSetTevOrder(3, 2, 0, 255);
    GXSetTevOrder(4, 1, 2, 255);
    GXSetTevSwapModeTable(1, 3, 3, 3, 3);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevSwapMode(2, 0, 1);
    GXSetTevSwapMode(3, 0, 0);
    GXSetTevSwapMode(4, 0, 0);
    GXSetTevColorIn(0, 8, 15, 15, 15);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 7, 7, 7);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(1, 8, 15, 15, 15);
    GXSetTevColorOp(1, 0, 0, 0, 1, 1);
    GXSetTevAlphaIn(1, 7, 7, 7, 0);
    GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
    GXSetTevColorIn(2, 0, 2, 8, 15);
    GXSetTevColorOp(2, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(2, 4, 7, 7, 7);
    GXSetTevAlphaOp(2, 0, 0, 0, 1, 1);
    GXSetTevColorIn(3, 15, 0, 8, 15);
    GXSetTevColorOp(3, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(3, 7, 7, 7, 0);
    GXSetTevAlphaOp(3, 0, 0, 0, 1, 0);
    GXSetTevColorIn(4, 15, 15, 15, 0);
    GXSetTevColorOp(4, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(4, 7, 4, 1, 7);
    GXSetTevAlphaOp(4, 0, 0, 0, 1, 0);
    GXSetNumTevStages(5);
    GXSetBlendMode(1, 4, 5, 0);
}

void fn_802E09EC(void) {
    GXSetTexCoordGen2(0, 1, 4, 60, 0, 125);
    GXSetTexCoordGen2(1, 1, 1, 42, 0, 125);
    GXSetNumTexGens(2);
    GXSetTevOrder(0, 1, 2, 255);
    GXSetTevOrder(1, 0, 0, 4);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevColorIn(0, 15, 8, 5, 15);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 7, 7, 7);
    GXSetTevAlphaOp(0, 0, 1, 1, 1, 0);
    GXSetTevColorIn(1, 15, 8, 10, 0);
    GXSetTevColorOp(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(1, 7, 7, 7, 0);
    GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
    GXSetNumTevStages(2);
}

void fn_802E0B68(void) {
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
}

}  // extern "C"

struct HeapBlock {
    u32 magic;        // offset 0x0
    s32 size;         // offset 0x4
    HeapBlock* next;  // offset 0x8
    void* heap;       // offset 0xC
};

struct SimpleHeap {
    void Dump(const char* who) {
        fn_802E30C8("SimpleHeap dump: %s", who);
        fn_802E30C8("Allocated:");
        u32 total = 0;
        for (HeapBlock* b = used; b; b = b->next) {
            fn_802E30C8("	user ptr %8.8p: %d", b + 1, b->size);
            total += b->size;
        }
        fn_802E30C8("	%dK total", total >> 10);
        u32 nfree = 0;
        fn_802E30C8("Free:");
        for (HeapBlock* b = free; b; b = b->next) {
            fn_802E30C8("	user ptr %8.8p: %d", b + 1, b->size);
            nfree += b->size;
        }
        fn_802E30C8("	%dK total", nfree >> 10);
    }
    void* Alloc(u32 size) {
        s32 n = (size + align) & ~align;
        HeapBlock** best = 0;
        s32 bestSize = 0x7FFFFFFF;
        void* res = 0;
        for (HeapBlock** l = &free; *l; l = &(*l)->next) {
            HeapBlock* b = *l;
            if (b->size >= n && bestSize > b->size) {
                best = l;
                bestSize = b->size;
            }
        }
        if (best) {
            HeapBlock* b = *best;
            HeapBlock* split = 0;
            HeapBlock* next = b->next;
            s32 rest = b->size - n;
            if (rest > ((align + 0x10) & ~align) + 0x20) {
                HeapBlock* s = (HeapBlock*)((u8*)b + align + n + 1);
                s->magic = 0xABCFEED0;
                s->next = 0;
                s->size = rest - ((align + 0x10) & ~align);
                s->heap = this;
                b->size = n;
                split = s;
            }
            if (split) {
                split->next = next;
                *best = split;
            } else {
                *best = next;
            }
            b->next = used;
            used = b;
            res = (u8*)b + align + 1;
        }
        if (!res && active) {
            fn_802E317C("SimpleHeap: out of memory for %d byte allocation.", n);
        }
        return res;
    }
    ~SimpleHeap(void) {
        if (used) {
            Dump("~SimpleHeap");
            fn_802E317C("SimpleHeap Destroyed with pending allocations");
        }
    }
    u8 active;        // offset 0x0
    s32 align;        // offset 0x4
    HeapBlock* free;  // offset 0x8
    HeapBlock* used;  // offset 0xC
};

struct BinkMovie {
    u32 magic;        // offset 0x0
    u32* bink;        // offset 0x4
    char name[0x40];  // offset 0x8
    u8 loop;          // offset 0x48
    u8 pad49[3];
    s32 time;         // offset 0x4C
    u8 tex[0x100];    // offset 0x50
    u8 info[0xD8];    // offset 0x150
};

ModuleTag lbl_8055FEE0;
SimpleHeap* lbl_8055FEE8;
u8* lbl_8055FEEC;

extern "C" {

BinkMovie* fn_802E0BA4(BinkMovie* m) { return m; }

BinkMovie* fn_802E0BA8(void) {
    BinkMovie* m = (BinkMovie*)operator new(0x228, "BinkMovie");
    m->magic = 0x10011001;
    return m;
}

void fn_802E0BE0(BinkMovie* m) { operator delete(fn_802E0BA4(m)); }

void* fn_802E0C04(u32 size) { return lbl_8055FEE8->Alloc(size); }

void fn_802E0D44(void* p) {
    SimpleHeap* h = lbl_8055FEE8;
    HeapBlock* b = (HeapBlock*)((u8*)p - (h->align + 1));
    for (HeapBlock** l = &h->used; *l; l = &(*l)->next) {
        if (*l == b) {
            *l = b->next;
            break;
        }
    }
    b->next = h->free;
    h->free = b;
    int merged;
    do {
        merged = 0;
        for (HeapBlock** l = &h->free; *l; l = &(*l)->next) {
            HeapBlock* a = *l;
            for (HeapBlock** m = &h->free; *m; m = &(*m)->next) {
                HeapBlock* c = *m;
                if ((u8*)a + h->align + a->size + 1 == (u8*)c) {
                    a->size += c->size + ((h->align + 0x10) & ~h->align);
                    *m = c->next;
                    merged = 1;
                    break;
                }
            }
            if (merged) {
                break;
            }
        }
    } while (merged);
}

void fn_802E0E60(BinkMovie* m) {
    if (m->bink) {
        fn_802E1728(m->tex);
        fn_80118DC0(m->bink);
        m->bink = 0;
    }
}

void fn_802E0EAC(BinkMovie* m) {
    fn_802E0E60(m);
    int fd = fn_802E36D8(m->name);
    if (fd != -1) {
        u8 hdr[0x20];
        fn_802E3804(fd, hdr, 0x20);
        fn_802E3734(&fd);
    }
    m->bink = (u32*)fn_8011770C(m->name, 0x100400);
    if (m->bink) {
        fn_802E30C8("bink: opened %s", m->name);
        fn_80117618(m->bink, m->info);
        fn_80117618(m->bink, m->info);
        if (!fn_802E1BB4(m->tex)) {
            fn_802E317C("bmovie: can't create bink texture set");
        }
        fn_801176FC(m->bink, m->info);
        m->time = fn_802E2C20();
    } else {
        fn_802E30C8("bink: can't open %s", m->name);
    }
}

void* lbl_8055D150[2] = {(void*)fn_802E0C04, (void*)fn_802E0D44};

void fn_802E0F98(void) {
    fn_80119214(fn_802E0C04, fn_802E0D44);
    fn_80116E54((void*)fn_8011A168, lbl_8055D150);
    fn_801282EC(0x80);
    fn_801282F4();
    lbl_8055FEEC = new ("bmemory") u8[0x600000];
    SimpleHeap* h = (SimpleHeap*)operator new(0x10, "bheap");
    if (h) {
        u8* mem = lbl_8055FEEC;
        h->used = 0;
        h->align = 0x1F;
        HeapBlock* b = (HeapBlock*)mem;
        b->magic = 0xABCFEED0;
        b->next = 0;
        b->size = 0x600000 - ((h->align + 0x10) & ~h->align);
        b->heap = h;
        h->free = b;
        h->active = 1;
    }
    lbl_8055FEE8 = h;
}

void fn_802E1058(void) {
    delete lbl_8055FEE8;
    lbl_8055FEE8 = 0;
    delete[] lbl_8055FEEC;
    lbl_8055FEEC = 0;
}

BinkMovie* fn_802E11A4(const char* name, u8 loop) {
    BinkMovie* r = fn_802E0BA8();
    BinkMovie* m = fn_802E0BA4(r);
    m->bink = 0;
    m->loop = loop;
    fn_801345D8(m->name, 0x40, "video/%s", name);
    char* dot = strrchr(m->name, '.');
    if (dot) {
        *dot = 0;
    }
    char* d = m->name;
    int n = 0x40;
    while (*d) {
        n--;
        d++;
    }
    if (n > 0) {
        const char* s = ".bik";
        while ((*d++ = *s++) != 0) {
            if (--n <= 0) {
                d[-1] = 0;
                break;
            }
        }
    }
    fn_802E0EAC(m);
    return r;
}

void fn_802E1290(BinkMovie* m) {
    fn_802E30C8("drMovieUnload");
    fn_802E0E60(fn_802E0BA4(m));
    fn_802E0BE0(m);
}

void fn_802E12DC(BinkMovie* r) {
    BinkMovie* m = fn_802E0BA4(r);
    if (m->bink && m->bink[7]) {
        fn_802E30C8("bmovie: movie has error");
        fn_802E0E60(m);
    }
    if (!m->bink) {
        fn_802E0EAC(m);
    }
    if (m->bink) {
        fn_802E27B8(m->tex);
        fn_801186B8(m->bink);
        int now = fn_802E2C20();
        if (now - m->time > 0x96) {
            fn_802E30C8("bmovie: resyncing timer");
            m->time = now - 0x20;
        }
        if (now - m->time > 0x1E) {
            if (!fn_802ECC10()) {
                fn_80118C44(m->bink);
            }
            m->time += 0x20;
        }
        fn_802E27A0(m->tex);
    }
}

struct MVtx {
    f32 x, y, z;
    u32 c;
    f32 u, v;
};

const GXColorS10 lbl_80565238 = {0, 0, 0, 0};

void fn_802E13D8(BinkMovie* r, f32 x, f32 y, f32 w, f32 h) {
    BinkMovie* m = fn_802E0BA4(r);
    fn_802D3360();
    if (!m->bink) {
        return;
    }
    fn_802C8C04(-1);
    MVtx v[4];
    for (int i = 0; i < 4; i++) {
        v[i].x = i & 1 ? 60.0f : 0.0f;
        v[i].y = i & 2 ? 40.0f : 0.0f;
        v[i].z = 0.0f;
        v[i].c = -1;
        v[i].u = i & 1 ? 1.0f : 0.0f;
        v[i].v = i & 2 ? 1.0f : 0.0f;
    }
    static const u16 idx[6] = {0, 1, 2, 2, 1, 3};
    u16 ix[6];
    for (int i = 0; i < 6; i++) {
        ix[i] = idx[i];
    }
    fn_802C8DB8(v, 4, ix, 6);
    fn_802C8D74();
    u32* b = m->bink;
    x = 320.0f + x;
    y = 240.0f + -y;
    w = w / (f32)b[0];
    h = h / (f32)b[1];
    GXSetZMode(0, 7, 0);
    GXSetColorUpdate(1);
    GXSetAlphaCompare(7, 0, 1, 0, 0);
    fn_802E1DA4(m->tex, m->bink[0], m->bink[1], x, y, w, h, 1.0f);
    fn_802D007C(1.0f);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevSwapMode(2, 0, 0);
    GXSetTevSwapMode(3, 0, 0);
    GXSetTevColorS10(1, lbl_80565238);
    GXColor k;
    k.r = 0xFF;
    k.g = 0xFF;
    k.b = 0xFF;
    k.a = 0xFF;
    GXSetTevKColor(0, k);
    GXSetTevKColor(1, k);
    GXSetTevKColor(2, k);
    GXSetTevSwapModeTable(0, 0, 1, 2, 3);
    fn_802D3360();
    fn_802E2C20();
}

int fn_802E1718(void) { return 0; }

}  // extern "C"
