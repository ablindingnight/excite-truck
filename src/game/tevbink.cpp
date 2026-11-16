// TEV presets and Bink movies (0x802DEAC4-0x802E1728): the fixed texture-environment setups (and their
// cleanups) that the GX state code picks per surface mode - envmaps, lightmaps, specular, indirect
// warps, the ground layers - followed by the Bink movie player on its own SimpleHeap: open/close,
// per-frame decode, and drawing the frame as a YUV quad. Names are guesses apart from the strings.
#include "game/common.h"
#include <revolution/GX/GXTypes.h>
#include <revolution/GX/GXAttr.h>
#include <revolution/GX/GXBump.h>
#include <revolution/GX/GXPixel.h>
#include <revolution/GX/GXTev.h>
#include <revolution/GX/GXTexture.h>

// GXTexObj: the SDK's.

// GXColor: the SDK's.

// GXColorS10: the SDK's.

extern "C" {
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
void* BinkOpen(const char* name, u32 flags);  // BinkOpen
void BinkGetFrameBuffersInfo(void* bink, void* info);
void BinkRegisterFrameBuffers(void* bink, void* info);
void BinkClose(void* bink);  // BinkClose
void BinkDoFrame(void* bink);  // BinkDoFrame
void BinkNextFrame(void* bink);  // BinkNextFrame
int fn_802ECC10(void);
void fn_80119214(void* (*alloc)(u32), void (*free)(void*));
void BinkSetSoundSystem(void* fn, void* arg);
struct StreamAllocator;
void* BinkOpenAX(StreamAllocator *allocator);
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
    GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)0, (u32)33, (GXBool)0, (u32)125);
    GXSetTexCoordGen2((GXTexCoordID)1, (GXTexGenType)1, (GXTexGenSrc)0, (u32)36, (GXBool)0, (u32)125);
    GXSetTexCoordGen2((GXTexCoordID)2, (GXTexGenType)1, (GXTexGenSrc)0, (u32)39, (GXBool)0, (u32)125);
    GXSetTexCoordGen2((GXTexCoordID)3, (GXTexGenType)1, (GXTexGenSrc)0, (u32)30, (GXBool)0, (u32)125);
    GXSetNumTexGens((u8)4);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)3, (GXTexMapID)1, (GXChannelID)255);
    GXSetTevOrder((GXTevStageID)1, (GXTexCoordID)0, (GXTexMapID)2, (GXChannelID)255);
    GXSetTevOrder((GXTevStageID)2, (GXTexCoordID)0, (GXTexMapID)3, (GXChannelID)255);
    GXSetTevOrder((GXTevStageID)3, (GXTexCoordID)2, (GXTexMapID)0, (GXChannelID)255);
    GXSetTevOrder((GXTevStageID)4, (GXTexCoordID)1, (GXTexMapID)4, (GXChannelID)255);
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)2, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)3, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)4, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)8, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)1, (GXTevColorArg)8, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)1);
    GXSetTevAlphaIn((GXTevStageID)1, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)0);
    GXSetTevAlphaOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)2, (GXTevColorArg)0, (GXTevColorArg)2, (GXTevColorArg)9, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)2, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)2, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)0);
    GXSetTevAlphaOp((GXTevStageID)2, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)3, (GXTevColorArg)15, (GXTevColorArg)0, (GXTevColorArg)8, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)3, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)3, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)0);
    GXSetTevAlphaOp((GXTevStageID)3, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)4, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)0);
    GXSetTevColorOp((GXTevStageID)4, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)4, (GXTevAlphaArg)4, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)4, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetNumTevStages((u8)5);
}

void fn_802DEE1C(void) {
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)2, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)3, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)4, (GXTevSwapSel)0, (GXTevSwapSel)0);
}

void fn_802DEE88(void) {
    GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)4, (u32)60, (GXBool)0, (u32)125);
    GXSetNumTexGens((u8)1);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)0, (GXTexMapID)0, (GXChannelID)4);
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)8, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)5, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetNumTevStages((u8)1);
    GXSetBlendMode((GXBlendMode)1, (GXBlendFactor)4, (GXBlendFactor)5, (GXLogicOp)0);
}

void fn_802DEF70(void) {
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    fn_802D3144();
}

void fn_802DEFA0(void) {
    GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)0, (GXTexGenSrc)1, (u32)42, (GXBool)1, (u32)76);
    GXSetTexCoordGen2((GXTexCoordID)1, (GXTexGenType)1, (GXTexGenSrc)1, (u32)33, (GXBool)0, (u32)125);
    GXSetNumTexGens((u8)2);
    GXSetIndTexOrder((GXIndTexStageID)0, (GXTexCoordID)0, (GXTexMapID)5);
    GXSetNumIndStages((u8)1);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)1, (GXTexMapID)7, (GXChannelID)4);
    GXSetTevOrder((GXTevStageID)1, (GXTexCoordID)255, (GXTexMapID)6, (GXChannelID)255);
    GXSetTevSwapModeTable((GXTevSwapSel)1, (GXTevColorChan)2, (GXTevColorChan)2, (GXTevColorChan)2, (GXTevColorChan)0);
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)1);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)15, (GXTevColorArg)10, (GXTevColorArg)6, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)4, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)1, (GXTevColorArg)0, (GXTevColorArg)8, (GXTevColorArg)9, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)1, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)0);
    GXSetTevAlphaOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevIndirect((GXTevStageID)1, (GXIndTexStageID)0, (GXIndTexFormat)0, (GXIndTexBiasSel)0, (GXIndTexMtxID)1, (GXIndTexWrap)6, (GXIndTexWrap)6, (GXBool)0, (GXBool)0, (GXIndTexAlphaSel)0);
    GXSetNumTevStages((u8)2);
    GXSetBlendMode((GXBlendMode)1, (GXBlendFactor)4, (GXBlendFactor)0, (GXLogicOp)0);
    GXLoadTexObj((const GXTexObj*)(fn_802D312C()), (GXTexMapID)5);
    GXLoadTexObj((const GXTexObj*)(fn_802D30E4()), (GXTexMapID)7);
    GXLoadTexObj((const GXTexObj*)(fn_802D30F0()), (GXTexMapID)6);
}

void fn_802DF1B4(void) {
    GXSetTevSwapModeTable((GXTevSwapSel)1, (GXTevColorChan)0, (GXTevColorChan)0, (GXTevColorChan)0, (GXTevColorChan)3);
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
    fn_802D3144();
    GXSetNumIndStages((u8)0);
    GXSetTevDirect((GXTevStageID)1);
}

void fn_802DF21C(void) {
    GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)1, (u32)42, (GXBool)0, (u32)125);
    GXSetNumTexGens((u8)1);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)0, (GXTexMapID)0, (GXChannelID)4);
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)15, (GXTevColorArg)10, (GXTevColorArg)8, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)1, (GXTevScale)1, (GXBool)1, (GXTevRegID)0);
    GXSetNumTevStages((u8)1);
}

void fn_802DF2F0(void) {
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
}

void fn_802DF300(void) {
    GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)0, (u32)45, (GXBool)0, (u32)125);
    GXSetNumTexGens((u8)1);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)255, (GXTexMapID)255, (GXChannelID)4);
    GXSetTevOrder((GXTevStageID)1, (GXTexCoordID)0, (GXTexMapID)7, (GXChannelID)255);
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)15, (GXTevColorArg)10, (GXTevColorArg)6, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)1, (GXTevScale)1, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)1, (GXTevColorArg)0, (GXTevColorArg)8, (GXTevColorArg)9, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)1, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)0);
    GXSetTevAlphaOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetNumTevStages((u8)2);
    GXLoadTexObj((const GXTexObj*)(fn_802D3138()), (GXTexMapID)7);
}

void fn_802DF46C(void) {
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
}

void fn_802DF4A8(void) {
    GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)4, (u32)60, (GXBool)0, (u32)125);
    GXSetTexCoordGen2((GXTexCoordID)1, (GXTexGenType)0, (GXTexGenSrc)1, (u32)42, (GXBool)1, (u32)76);
    GXSetNumTexGens((u8)2);
    GXSetIndTexOrder((GXIndTexStageID)0, (GXTexCoordID)1, (GXTexMapID)5);
    GXSetNumIndStages((u8)1);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)0, (GXTexMapID)0, (GXChannelID)4);
    GXSetTevOrder((GXTevStageID)1, (GXTexCoordID)255, (GXTexMapID)6, (GXChannelID)255);
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)15, (GXTevColorArg)10, (GXTevColorArg)8, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)1, (GXTevScale)1, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)1, (GXTevColorArg)0, (GXTevColorArg)8, (GXTevColorArg)9, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)1, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)0);
    GXSetTevAlphaOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevIndirect((GXTevStageID)1, (GXIndTexStageID)0, (GXIndTexFormat)0, (GXIndTexBiasSel)0, (GXIndTexMtxID)1, (GXIndTexWrap)6, (GXIndTexWrap)6, (GXBool)0, (GXBool)0, (GXIndTexAlphaSel)0);
    GXSetNumTevStages((u8)2);
    GXLoadTexObj((const GXTexObj*)(fn_802D3130()), (GXTexMapID)5);
    GXLoadTexObj((const GXTexObj*)(fn_802D3100()), (GXTexMapID)6);
}

void fn_802DF684(void) {
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetNumIndStages((u8)0);
    GXSetTevDirect((GXTevStageID)1);
}

void fn_802DF6D0(void) {
    GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)4, (u32)60, (GXBool)0, (u32)125);
    GXSetTexCoordGen2((GXTexCoordID)1, (GXTexGenType)0, (GXTexGenSrc)1, (u32)42, (GXBool)1, (u32)76);
    GXSetNumTexGens((u8)2);
    GXSetIndTexOrder((GXIndTexStageID)0, (GXTexCoordID)1, (GXTexMapID)5);
    GXSetNumIndStages((u8)1);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)0, (GXTexMapID)0, (GXChannelID)4);
    GXSetTevOrder((GXTevStageID)1, (GXTexCoordID)255, (GXTexMapID)6, (GXChannelID)255);
    GXSetTevOrder((GXTevStageID)2, (GXTexCoordID)0, (GXTexMapID)1, (GXChannelID)255);
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)2, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)15, (GXTevColorArg)8, (GXTevColorArg)10, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)1, (GXTevColorArg)0, (GXTevColorArg)8, (GXTevColorArg)9, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)1, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)0);
    GXSetTevAlphaOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevIndirect((GXTevStageID)1, (GXIndTexStageID)0, (GXIndTexFormat)0, (GXIndTexBiasSel)0, (GXIndTexMtxID)1, (GXIndTexWrap)6, (GXIndTexWrap)6, (GXBool)0, (GXBool)0, (GXIndTexAlphaSel)0);
    GXSetTevColorIn((GXTevStageID)2, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)0);
    GXSetTevColorOp((GXTevStageID)2, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)2, (GXTevAlphaArg)4, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)2, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetNumTevStages((u8)3);
    GXLoadTexObj((const GXTexObj*)(fn_802D3134()), (GXTexMapID)5);
    GXLoadTexObj((const GXTexObj*)(fn_802D30FC()), (GXTexMapID)6);
}

void fn_802DF938(void) {
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)2, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetNumIndStages((u8)0);
    GXSetTevDirect((GXTevStageID)1);
}

void fn_802DF994(void) {
    GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)4, (u32)60, (GXBool)0, (u32)125);
    GXSetNumTexGens((u8)1);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)0, (GXTexMapID)0, (GXChannelID)4);
    GXSetTevOrder((GXTevStageID)1, (GXTexCoordID)255, (GXTexMapID)255, (GXChannelID)5);
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)15, (GXTevColorArg)8, (GXTevColorArg)10, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)1, (GXTevScale)1, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)1, (GXTevColorArg)15, (GXTevColorArg)10, (GXTevColorArg)5, (GXTevColorArg)0);
    GXSetTevColorOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)1, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)0);
    GXSetTevAlphaOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetNumTevStages((u8)2);
}

void fn_802DFAF4(void) {
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
}

void fn_802DFB30(void) {
    GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)0, (u32)30, (GXBool)0, (u32)125);
    GXSetTexCoordGen2((GXTexCoordID)1, (GXTexGenType)1, (GXTexGenSrc)0, (u32)30, (GXBool)0, (u32)125);
    GXSetNumTexGens((u8)2);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)1, (GXTexMapID)1, (GXChannelID)255);
    GXSetTevOrder((GXTevStageID)1, (GXTexCoordID)0, (GXTexMapID)0, (GXChannelID)255);
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)8, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)1, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)1, (GXTevColorArg)15, (GXTevColorArg)0, (GXTevColorArg)8, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)1, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)0);
    GXSetTevAlphaOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetNumTevStages((u8)2);
    GXSetBlendMode((GXBlendMode)1, (GXBlendFactor)4, (GXBlendFactor)5, (GXLogicOp)0);
}

void fn_802DFCC0(void) {
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
    fn_802D3144();
}

void fn_802DFD00(void) {
    GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)0, (u32)30, (GXBool)0, (u32)125);
    GXSetTexCoordGen2((GXTexCoordID)1, (GXTexGenType)1, (GXTexGenSrc)4, (u32)60, (GXBool)0, (u32)125);
    GXSetNumTexGens((u8)2);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)1, (GXTexMapID)1, (GXChannelID)255);
    GXSetTevOrder((GXTevStageID)1, (GXTexCoordID)0, (GXTexMapID)0, (GXChannelID)255);
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)8, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevKAlphaSel((GXTevStageID)0, (GXTevKAlphaSel)29);
    GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)7, (GXTevAlphaArg)4, (GXTevAlphaArg)6, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)1, (GXTevColorArg)15, (GXTevColorArg)0, (GXTevColorArg)8, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)1, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)0);
    GXSetTevAlphaOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetNumTevStages((u8)2);
}

void fn_802DFE88(void) {
    GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)0, (u32)30, (GXBool)0, (u32)125);
    GXSetTexCoordGen2((GXTexCoordID)1, (GXTexGenType)1, (GXTexGenSrc)4, (u32)60, (GXBool)0, (u32)125);
    GXSetNumTexGens((u8)2);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)1, (GXTexMapID)1, (GXChannelID)255);
    GXSetTevOrder((GXTevStageID)1, (GXTexCoordID)0, (GXTexMapID)0, (GXChannelID)255);
    GXSetTevOrder((GXTevStageID)2, (GXTexCoordID)1, (GXTexMapID)2, (GXChannelID)255);
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)2, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)8, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)1, (GXTevColorArg)15, (GXTevColorArg)0, (GXTevColorArg)8, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)1, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)0);
    GXSetTevAlphaOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)2, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)0);
    GXSetTevColorOp((GXTevStageID)2, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevKAlphaSel((GXTevStageID)2, (GXTevKAlphaSel)29);
    GXSetTevAlphaIn((GXTevStageID)2, (GXTevAlphaArg)7, (GXTevAlphaArg)4, (GXTevAlphaArg)6, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)2, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetNumTevStages((u8)3);
}

void fn_802E009C(void) {
    GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)5, (u32)60, (GXBool)0, (u32)125);
    GXSetTexCoordGen2((GXTexCoordID)1, (GXTexGenType)1, (GXTexGenSrc)4, (u32)60, (GXBool)0, (u32)125);
    GXSetNumTexGens((u8)2);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)1, (GXTexMapID)1, (GXChannelID)4);
    GXSetTevOrder((GXTevStageID)1, (GXTexCoordID)0, (GXTexMapID)0, (GXChannelID)255);
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)8, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)7, (GXTevAlphaArg)4, (GXTevAlphaArg)5, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)1, (GXTevColorArg)15, (GXTevColorArg)0, (GXTevColorArg)8, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)1, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)0);
    GXSetTevAlphaOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetNumTevStages((u8)2);
}

void fn_802E0218(void) {
    GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)5, (u32)60, (GXBool)0, (u32)125);
    GXSetTexCoordGen2((GXTexCoordID)1, (GXTexGenType)1, (GXTexGenSrc)4, (u32)60, (GXBool)0, (u32)125);
    GXSetNumTexGens((u8)2);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)1, (GXTexMapID)1, (GXChannelID)255);
    GXSetTevOrder((GXTevStageID)1, (GXTexCoordID)0, (GXTexMapID)0, (GXChannelID)255);
    GXSetTevOrder((GXTevStageID)2, (GXTexCoordID)1, (GXTexMapID)2, (GXChannelID)4);
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)2, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)8, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)1, (GXTevColorArg)15, (GXTevColorArg)0, (GXTevColorArg)8, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)1, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)0);
    GXSetTevAlphaOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)2, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)0);
    GXSetTevColorOp((GXTevStageID)2, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)2, (GXTevAlphaArg)7, (GXTevAlphaArg)4, (GXTevAlphaArg)5, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)2, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetNumTevStages((u8)3);
}

void fn_802E0420(void) {
    GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)4, (u32)60, (GXBool)0, (u32)125);
    GXSetNumTexGens((u8)1);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)0, (GXTexMapID)1, (GXChannelID)255);
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevKColorSel((GXTevStageID)0, (GXTevKColorSel)13);
    GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)15, (GXTevColorArg)8, (GXTevColorArg)14, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevKAlphaSel((GXTevStageID)0, (GXTevKAlphaSel)29);
    GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)7, (GXTevAlphaArg)4, (GXTevAlphaArg)6, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetNumTevStages((u8)1);
}

void fn_802E050C(void) {
    GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)4, (u32)60, (GXBool)0, (u32)125);
    GXSetNumTexGens((u8)1);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)0, (GXTexMapID)1, (GXChannelID)255);
    GXSetTevOrder((GXTevStageID)1, (GXTexCoordID)0, (GXTexMapID)2, (GXChannelID)255);
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevKColorSel((GXTevStageID)0, (GXTevKColorSel)13);
    GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)15, (GXTevColorArg)8, (GXTevColorArg)14, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)1, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)0);
    GXSetTevColorOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevKAlphaSel((GXTevStageID)1, (GXTevKAlphaSel)29);
    GXSetTevAlphaIn((GXTevStageID)1, (GXTevAlphaArg)7, (GXTevAlphaArg)4, (GXTevAlphaArg)6, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetNumTevStages((u8)2);
}

void fn_802E0684(void) {
    GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)0, (u32)33, (GXBool)0, (u32)125);
    GXSetTexCoordGen2((GXTexCoordID)1, (GXTexGenType)1, (GXTexGenSrc)0, (u32)30, (GXBool)0, (u32)125);
    GXSetTexCoordGen2((GXTexCoordID)2, (GXTexGenType)1, (GXTexGenSrc)0, (u32)39, (GXBool)0, (u32)125);
    GXSetNumTexGens((u8)3);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)1, (GXTexMapID)1, (GXChannelID)255);
    GXSetTevOrder((GXTevStageID)1, (GXTexCoordID)0, (GXTexMapID)3, (GXChannelID)255);
    GXSetTevOrder((GXTevStageID)2, (GXTexCoordID)0, (GXTexMapID)4, (GXChannelID)255);
    GXSetTevOrder((GXTevStageID)3, (GXTexCoordID)2, (GXTexMapID)0, (GXChannelID)255);
    GXSetTevOrder((GXTevStageID)4, (GXTexCoordID)1, (GXTexMapID)2, (GXChannelID)255);
    GXSetTevSwapModeTable((GXTevSwapSel)1, (GXTevColorChan)3, (GXTevColorChan)3, (GXTevColorChan)3, (GXTevColorChan)3);
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)2, (GXTevSwapSel)0, (GXTevSwapSel)1);
    GXSetTevSwapMode((GXTevStageID)3, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)4, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)8, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)1, (GXTevColorArg)8, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)1);
    GXSetTevAlphaIn((GXTevStageID)1, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)0);
    GXSetTevAlphaOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)2, (GXTevColorArg)0, (GXTevColorArg)2, (GXTevColorArg)8, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)2, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)2, (GXTevAlphaArg)4, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)2, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)1);
    GXSetTevColorIn((GXTevStageID)3, (GXTevColorArg)15, (GXTevColorArg)0, (GXTevColorArg)8, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)3, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)3, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)0);
    GXSetTevAlphaOp((GXTevStageID)3, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)4, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)15, (GXTevColorArg)0);
    GXSetTevColorOp((GXTevStageID)4, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)4, (GXTevAlphaArg)7, (GXTevAlphaArg)4, (GXTevAlphaArg)1, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)4, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetNumTevStages((u8)5);
    GXSetBlendMode((GXBlendMode)1, (GXBlendFactor)4, (GXBlendFactor)5, (GXLogicOp)0);
}

void fn_802E09EC(void) {
    GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)4, (u32)60, (GXBool)0, (u32)125);
    GXSetTexCoordGen2((GXTexCoordID)1, (GXTexGenType)1, (GXTexGenSrc)1, (u32)42, (GXBool)0, (u32)125);
    GXSetNumTexGens((u8)2);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)1, (GXTexMapID)2, (GXChannelID)255);
    GXSetTevOrder((GXTevStageID)1, (GXTexCoordID)0, (GXTexMapID)0, (GXChannelID)4);
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)15, (GXTevColorArg)8, (GXTevColorArg)5, (GXTevColorArg)15);
    GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
    GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)1, (GXTevScale)1, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)1, (GXTevColorArg)15, (GXTevColorArg)8, (GXTevColorArg)10, (GXTevColorArg)0);
    GXSetTevColorOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)1, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)0);
    GXSetTevAlphaOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetNumTevStages((u8)2);
}

void fn_802E0B68(void) {
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
}

}  // extern "C"

struct HeapBlock {
    u32 magic;        // offset 0x0
    s32 size;         // offset 0x4
    HeapBlock* next;  // offset 0x8
    void* heap;       // offset 0xC
};

struct SimpleHeap {
    ~SimpleHeap(void) {
        if (used) {
            Dump("~SimpleHeap");
            fn_802E317C("SimpleHeap Destroyed with pending allocations");
        }
    }
    void* Alloc(u32 size) {
        s32 n = (size + align) & ~align;
        HeapBlock* split;
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
            split = 0;
            HeapBlock* b = *best;
            bestSize = b->size - n;
            HeapBlock* next = b->next;
            s32 hdr = (align + 0x10) & ~align;
            if (bestSize > hdr + 0x20) {
                HeapBlock* s = (HeapBlock*)((u8*)b + align + (n + 1));
                s->magic = 0xABCFEED0;
                s->next = 0;
                s->size = bestSize - ((align + 0x10) & ~align);
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
    u8 active;        // offset 0x0
    u32 align;        // offset 0x4
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
    HeapBlock** l;
    int merged;
    HeapBlock** m;
    int adj;
    HeapBlock* c;
    HeapBlock* a;
    u32 al;
    HeapBlock** dst;
    HeapBlock** used;
    for (used = &h->used; *used; used = &(*used)->next) {
        if (*used == b) {
            *used = b->next;
            break;
        }
    }
    HeapBlock** seed = &h->free;
    b->next = *seed;
    *seed = b;
    l = seed;
    do {
        merged = 0;
        for (m = &h->free; (c = *m) != 0; m = &c->next) {
            a = *l;
            al = h->align;
            adj = 1;
            if ((u8*)a + al + 1 + a->size != (u8*)c && (u8*)c + al + 1 + c->size != (u8*)a) {
                adj = 0;
            }
            if (adj) {
                dst = l;
                if ((u8*)c + al + 1 + c->size == (u8*)a) {
                    dst = m;
                    m = l;
                }
                (*dst)->size = (*dst)->size + (*m)->size + ((al + 0x10) & ~al);
                *m = (*m)->next;
                l = dst;
                merged = 1;
                break;
            }
        }
    } while (merged);
}

void fn_802E0E60(BinkMovie* m) {
    if (m->bink) {
        fn_802E1728(m->tex);
        BinkClose(m->bink);
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
    m->bink = (u32*)BinkOpen(m->name, 0x100400);
    if (m->bink) {
        fn_802E30C8("bink: opened %s", m->name);
        BinkGetFrameBuffersInfo(m->bink, m->info);
        BinkGetFrameBuffersInfo(m->bink, m->info);
        if (!fn_802E1BB4(m->tex)) {
            fn_802E317C("bmovie: can't create bink texture set");
        }
        BinkRegisterFrameBuffers(m->bink, m->info);
        m->time = fn_802E2C20();
    } else {
        fn_802E30C8("bink: can't open %s", m->name);
    }
}

void* lbl_8055D150[2] = {(void*)fn_802E0C04, (void*)fn_802E0D44};

void fn_802E0F98(void) {
    fn_80119214(fn_802E0C04, fn_802E0D44);
    BinkSetSoundSystem((void*)BinkOpenAX, lbl_8055D150);
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

/* Bounded copy and append (n = buffer size), the same pair as carmodel's. */
static inline void StrCopy(char* d, const char* s, int n) {
    char* p = d;
    while ((*p++ = *s++) != 0) {
        if (--n <= 0) {
            p[-1] = 0;
            break;
        }
    }
}

static inline void StrCat(char* d, const char* s, int n) {
    while (*d) {
        n--;
        d++;
    }
    if (n > 0) {
        StrCopy(d, s, n);
    }
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
    StrCat(m->name, ".bik", 0x40);
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
        BinkDoFrame(m->bink);
        int now = fn_802E2C20();
        if (now - m->time > 0x96) {
            fn_802E30C8("bmovie: resyncing timer");
            m->time = now - 0x20;
        }
        if (now - m->time > 0x1E) {
            if (!fn_802ECC10()) {
                BinkNextFrame(m->bink);
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
        v[i].u = i & 1 ? 1.0f : 0.0f;
        v[i].v = i & 2 ? 1.0f : 0.0f;
        v[i].c = -1;
    }
    u16 ix[6] = {0, 1, 2, 2, 1, 3};
    fn_802C8DB8(v, 4, ix, 6);
    fn_802C8D74();
    u32* b = m->bink;
    x = 320.0f + x;
    y = 240.0f + -y;
    f32 sx = w / (f32)b[0];
    f32 sy = h / (f32)b[1];
    GXSetZMode((GXBool)0, (GXCompare)7, (GXBool)0);
    GXSetColorUpdate((GXBool)1);
    GXSetAlphaCompare((GXCompare)7, (u8)0, (GXAlphaOp)1, (GXCompare)0, (u8)0);
    fn_802E1DA4(m->tex, m->bink[0], m->bink[1], x, y, sx, sy, 1.0f);
    fn_802D007C(1.0f);
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)2, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevSwapMode((GXTevStageID)3, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevColorS10((GXTevRegID)1, (GXColorS10)lbl_80565238);
    GXColor k = {0xFF, 0xFF, 0xFF, 0xFF};
    GXSetTevKColor((GXTevKColorID)0, (GXColor)k);
    GXSetTevKColor((GXTevKColorID)1, (GXColor)k);
    GXSetTevKColor((GXTevKColorID)2, (GXColor)k);
    GXSetTevSwapModeTable((GXTevSwapSel)0, (GXTevColorChan)0, (GXTevColorChan)1, (GXTevColorChan)2, (GXTevColorChan)3);
    fn_802D3360();
    fn_802E2C20();
}

int fn_802E1718(s32) { return 0; }

}  // extern "C"
