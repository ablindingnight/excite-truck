// HOME button menu (0x802EC4BC-0x802ECCE4): loads the HOME menu files for the console language, opens the
// menu on request (stopping rumble), runs and draws it each frame, acts on its result (return to menu or
// reset) and blocks the game loop while it is up. Names are guesses apart from the strings.
#include "game/common.h"
#include <revolution/GX/GXAttr.h>
#include <revolution/GX/GXLight.h>
#include <revolution/GX/GXPixel.h>
#include <revolution/GX/GXTev.h>
#include <revolution/GX/GXTransform.h>
#include <revolution/MTX/mtx44.h>

extern "C" {
void* memset(void* dst, int c, size_t n);
int fn_801345D8(char* dst, int n, const char* fmt, ...);
void fn_802E30C8(const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);  // fatal
int fn_802E9A0C(void);                   // aspect
int LoadFileWithName(const char* path);       // open
u32 GetFileSize(int fd);                 // size
void ReadFileBuffer(int fd, void* buf, u32 n);
void fn_802E3734(int* fd);
void fn_801282EC(int align);
void fn_801282F4(void);
u8 SCGetLanguage(void);
int fn_802D9BF0(void);
s32 fn_802E2C20(void);
void* fn_802E8668(int i);  // KPAD status
void UpdateWpadSettings(void);
void fn_802E7C40(void);
void fn_802E5438(void);
void fn_802E2E38(int mode);
void WPADControlMotor(s32 chan, u32 cmd);
void* fn_802D2D14(void);  // render mode
void fn_802D3360(void);
void fn_802CFCA0(void);
void fn_802D02F8(u32 color, int a);
void fn_802D68C4(int a, int b, int c, int d);
void HandleFrameSync(void);
void ResetGraphicsState(void);
void fn_80127464(void);

void HBMCreate(void* info);
void HBMDelete(void);
void HBMInit(void);
int HBMCalc(void* con);
void HBMDraw(void);
int HBMGetSelectBtnNum(void);
int HBMIsReassignedControllers(void);
void HBMDeleteSound(void);
void HBMUpdateSound(void);

}

void HBMCreateSound(const char* file, void* buf, u32 size);
void HBMSetAdjustFlag(bool on);
int LoadingFrame(void);

void* operator new[](size_t size, const char* name);
void operator delete[](void* p);

struct HBMDataInfo {
    void* layoutBuf;    // offset 0x0
    void* spkSeBuf;     // offset 0x4
    void* msgBuf;       // offset 0x8
    void* configBuf;    // offset 0xC
    void* mem;          // offset 0x10
    void* soundCallback;  // offset 0x14
    s32 backFlag;       // offset 0x18
    s32 region;         // offset 0x1C
    s32 cursor;         // offset 0x20
    u32 memSize;        // offset 0x24
    f32 frameDelta;     // offset 0x28
    f32 adjustX;        // offset 0x2C
    f32 adjustY;        // offset 0x30
    void* allocator;    // offset 0x34
};

struct HBMController {
    void* kpad;      // offset 0x0
    f32 x;           // offset 0x4
    f32 y;           // offset 0x8
    u32 devType;     // offset 0xC
};

struct RenderMode {
    u32 tvMode;      // offset 0x0
    u16 fbWidth;     // offset 0x4
    u16 efbHeight;   // offset 0x6
};

ModuleTag lbl_805600D8;
static u8* lbl_805600E0;    // sound heap
static void* lbl_805600E4;  // sound archive
static u8 lbl_805600E8;     // menu showing
static u8 lbl_805600E9;     // kernel idle
static u8 lbl_805600EA;     // requested
static u8 lbl_805600EB;     // modal
static u8 lbl_805600EC;     // ready
static u8 lbl_805600ED;     // in modal loop
static HBMDataInfo lbl_80558860;
static HBMController lbl_80558898[4];
static void* lbl_805588D8[8];  // callbacks

extern "C" {

bool fn_802EC4BC(void) { return fn_802E9A0C() == 1; }

void* LoadHomeButtonFile(const char* name) {
    char path[0x40];
    fn_801345D8(path, sizeof(path), "homeButton2/%s", name);
    int fd = LoadFileWithName(path);
    if (fd == -1) {
        fn_802E317C("homebtn: can't open %s", path);
    }
    u32 size = GetFileSize(fd);
    fn_801282EC(0x20);
    u8* buf = new ("homeFile") u8[size];
    ReadFileBuffer(fd, buf, size);
    fn_802E3734(&fd);
    return buf;
}

const char* GetHomeButtonArchiveName(void) {
    switch (lbl_80558860.region) {
    default:
        lbl_80558860.region = 0;
    case 0:
        return "homeBtn.arc";
    case 1:
        return "homeBtn_ENG.arc";
    case 2:
        return "homeBtn_GER.arc";
    case 3:
        return "homeBtn_FRA.arc";
    case 4:
        return "homeBtn_SPA.arc";
    case 5:
        return "homeBtn_ITA.arc";
    case 6:
        return "homeBtn_NED.arc";
    }
}

// Only their strings survive in the game.
void HomeSoundCallback(int ev) { fn_802E30C8("homebtn: sound_callback(%d)\n", ev); }

void HomeEventCallback(int a, int b) { fn_802E30C8("homebtn: event_callback(%d, %d)\n", a, b); }

static inline void InitFastCast(void) {
#ifdef __MWERKS__
    asm {
        li r3, 4
        oris r3, r3, 4
        mtspr 0x392, r3
        li r3, 5
        oris r3, r3, 5
        mtspr 0x393, r3
        li r3, 6
        oris r3, r3, 6
        mtspr 0x394, r3
        li r3, 7
        oris r3, r3, 7
        mtspr 0x395, r3
    }
#endif
}

void InitializeHomeButtonMenu(void) {
    lbl_805600E9 = 0;
    lbl_805600EA = 0;
    InitFastCast();
    for (int i = 0; i < 4; i++) {
        lbl_80558898[i].x = 0.0f;
        lbl_80558898[i].y = 0.0f;
        lbl_80558898[i].devType = 0;
    }
    fn_801282EC(0x20);
    lbl_80558860.mem = new ("homemem") u8[0x80000];
    memset(lbl_80558860.mem, 0, 0x80000);
    lbl_80558860.memSize = 0x80000;
    lbl_80558860.region = SCGetLanguage();
    lbl_80558860.layoutBuf = LoadHomeButtonFile(GetHomeButtonArchiveName());
    lbl_80558860.spkSeBuf = LoadHomeButtonFile("SpeakerSe.arc");
    lbl_80558860.msgBuf = LoadHomeButtonFile("home.csv");
    lbl_80558860.configBuf = LoadHomeButtonFile("config.tx_");
    lbl_80558860.soundCallback = 0;
    lbl_80558860.backFlag = 0;
    lbl_80558860.cursor = 0;
    lbl_80558860.adjustX = 1.3684211f;
    lbl_80558860.adjustY = 1.0f;
    lbl_80558860.frameDelta = 2.0f;
    fn_801282EC(0x20);
    fn_801282F4();
    lbl_805600E0 = new ("home_snd") u8[0x19000];
    memset(lbl_805600E0, 0, 0x19000);
    lbl_805600E4 = LoadHomeButtonFile("HomeButtonSe.brsar");
    HBMCreate(&lbl_80558860);
    lbl_805600EC = 1;
}

void CleanupHomeButtonResources(void) {
    lbl_805600EC = 0;
    HBMDelete();
    for (int i = 0; i < 8; i++) {
        if (lbl_805588D8[i]) {
            fn_802E317C("homebtn: callback %p never removed", lbl_805588D8[i]);
        }
    }
    delete[] (u8*)lbl_80558860.mem;
    delete[] (u8*)lbl_80558860.layoutBuf;
    if (lbl_80558860.spkSeBuf) {
        delete[] (u8*)lbl_80558860.spkSeBuf;
    }
    delete[] (u8*)lbl_80558860.msgBuf;
    delete[] (u8*)lbl_80558860.configBuf;
    if (lbl_805600E0) {
        delete[] lbl_805600E0;
    }
    if (lbl_805600E4) {
        delete[] (u8*)lbl_805600E4;
    }
}

void HandleHomeButtonActivation(void) {
    if (!lbl_805600EC) {
        return;
    }
    if (lbl_805600EA) {
        lbl_805600EA = 0;
        if (!fn_802D9BF0()) {
            bool modal = !lbl_805600E9;
            lbl_805600EB = modal;
            fn_802E30C8("homebtn: activating home button in %s mode", modal ? "kernel-idle" : "normal");
            lbl_805600E8 = 1;
            for (int i = 0; i < 4; i++) {
                WPADControlMotor(i, 0);
            }
            HBMCreateSound((const char*)lbl_805600E4, lbl_805600E0, 0x19000);
            HBMInit();
            HBMSetAdjustFlag(fn_802EC4BC());
        }
    }
    fn_802E2C20();
    if (lbl_805600E8) {
        HBMController* c = lbl_80558898;
        for (int i = 0; i < 4; i++, c++) {
            c->kpad = fn_802E8668(i);
        }
        if (HBMCalc(lbl_80558898) >= 0) {
            UpdateWpadSettings();
            fn_802E7C40();
            fn_802E5438();
            fn_802E30C8("homebtn: leaving home menu");
            fn_802E30C8("Select Btn:%d", HBMGetSelectBtnNum());
            fn_802E30C8("Reassigned:%d", HBMIsReassignedControllers());
            switch (HBMGetSelectBtnNum()) {
            case 1:
                fn_802E30C8("homebtn: return to console menu");
                fn_802E2E38(1);
                break;
            case 2:
                fn_802E30C8("homebtn: resetting");
                fn_802E2E38(0);
                break;
            }
            HBMDeleteSound();
            lbl_805600E8 = 0;
        }
        if (lbl_805600E8) {
            HBMUpdateSound();
        }
    }
}

void Setup2DRenderState(void) {
    if (!lbl_805600E8) {
        return;
    }
    GXClearVtxDesc();
    GXSetVtxAttrFmt((GXVtxFmt)4, (GXAttr)9, (GXCompCnt)0, (GXCompType)4, (u8)0);
    GXSetVtxAttrFmt((GXVtxFmt)4, (GXAttr)11, (GXCompCnt)0, (GXCompType)1, (u8)0);
    GXSetVtxDesc((GXAttr)9, (GXAttrType)1);
    GXSetVtxDesc((GXAttr)11, (GXAttrType)1);
    RenderMode* rm = (RenderMode*)fn_802D2D14();
    GXSetViewport(0.0f, 0.0f, rm->fbWidth, rm->efbHeight, 0.0f, 1.0f);
    GXSetScissor((u32)0, (u32)0, (u32)rm->fbWidth, (u32)rm->efbHeight);
    GXSetNumChans((u8)1);
    GXSetNumTexGens((u8)0);
    GXSetNumTevStages((u8)1);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)0xFF, (GXTexMapID)0xFF, (GXChannelID)4);
    GXSetTevOp((GXTevStageID)0, (GXTevMode)4);
    GXSetBlendMode((GXBlendMode)0, (GXBlendFactor)0, (GXBlendFactor)0, (GXLogicOp)0);
    GXSetZMode((GXBool)1, (GXCompare)3, (GXBool)1);
    GXSetCurrentMtx((u32)3);
    f32 m[4][4];
    if (fn_802EC4BC()) {
        C_MTXOrtho(m, 228.0f, -228.0f, -416.0f, 416.0f, 0.0f, 500.0f);
    } else {
        C_MTXOrtho(m, 228.0f, -228.0f, -304.0f, 304.0f, 0.0f, 500.0f);
    }
    GXSetProjection(m, (GXProjectionType)1);
    if (lbl_805600E8) {
        HBMDraw();
    }
    fn_802D3360();
}

void fn_802ECBF8(void) {
    if (lbl_805600E8) {
        return;
    }
    lbl_805600EA = 1;
}

int fn_802ECC10(void) { return lbl_805600E8; }

void fn_802ECC18(u8 idle) { lbl_805600E9 = idle; }

void fn_802ECC20(void) {
    if (!LoadingFrame()) {
        fn_802CFCA0();
        fn_802D02F8(0xFF000000, 1);
        fn_802D68C4(0, 0, 0, 0);
        HandleFrameSync();
        ResetGraphicsState();
    }
}

void fn_802ECC74(void) {
    if (lbl_805600ED) {
        return;
    }
    lbl_805600ED = 1;
    if (lbl_805600EB) {
        while (lbl_805600E8) {
            fn_802ECC20();
            fn_80127464();
        }
        fn_802ECC20();
        lbl_805600EB = 0;
    }
    lbl_805600ED = 0;
}

}  // extern "C"
