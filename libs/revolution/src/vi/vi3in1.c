#include <revolution/os.h>
#include <revolution/vi.h>

static VIVideo Vdac_Flag_Region;
volatile u32 Vdac_Flag_Changed = 0;
static u32 __current_3in1_video_mode = 0;
static VIACPType __type;
#if SDK_SEP2006
static u32 __tvType;  // defined after __cc4 to match this SDK's data layout
#else
static u32 __tvType = 0xFF;
#endif

// DUMP_DATA gammaSet {u16[6],u8[7],u16[7]}
static VIGammaObj gammaSet[] = {
#include "dumpdata/gammaSet.inc"
};

VIMacroVisionObj VINtscACPType1 = {0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B,
                                   0x1B, 0x24, 0x07, 0xF8, 0x00, 0x00, 0x0F, 0x0F, 0x60,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

VIMacroVisionObj VINtscACPType2 = {0x3E, 0x1D, 0x11, 0x25, 0x11, 0x01, 0x07, 0x00, 0x1B,
                                   0x1B, 0x24, 0x07, 0xF8, 0x00, 0x00, 0x0F, 0x0F, 0x60,
                                   0x01, 0x0A, 0x00, 0x05, 0x04, 0x03, 0xFF, 0x00};

VIMacroVisionObj VINtscACPType3 = {0x3E, 0x17, 0x15, 0x21, 0x15, 0x05, 0x05, 0x02, 0x1B,
                                   0x1B, 0x24, 0x07, 0xF8, 0x00, 0x00, 0x0F, 0x0F, 0x60,
                                   0x01, 0x0A, 0x00, 0x05, 0x04, 0x03, 0xFF, 0x00};

VIMacroVisionObj VIPalACPType1 = {0x36, 0x1A, 0x22, 0x2A, 0x22, 0x05, 0x02, 0x00, 0x1C,
                                  0x3D, 0x14, 0x03, 0xFE, 0x01, 0x54, 0xFE, 0x7E, 0x60,
                                  0x00, 0x08, 0x00, 0x04, 0x07, 0x01, 0x55, 0x01};

VIMacroVisionObj VIPalACPType2 = {0x36, 0x1A, 0x22, 0x2A, 0x22, 0x05, 0x02, 0x00, 0x1C,
                                  0x3D, 0x14, 0x03, 0xFE, 0x01, 0x54, 0xFE, 0x7E, 0x60,
                                  0x00, 0x08, 0x00, 0x04, 0x07, 0x01, 0x55, 0x01};

VIMacroVisionObj VIPalACPType3 = {0x36, 0x1A, 0x22, 0x2A, 0x22, 0x05, 0x02, 0x00, 0x1C,
                                  0x3D, 0x14, 0x03, 0xFE, 0x01, 0x54, 0xFE, 0x7E, 0x60,
                                  0x00, 0x08, 0x00, 0x04, 0x07, 0x01, 0x55, 0x01};

VIMacroVisionObj VIEurgb60ACPType1 = {0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B,
                                      0x1B, 0x24, 0x07, 0xF8, 0x00, 0x00, 0x1E, 0x1E, 0x60,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

VIMacroVisionObj VIEurgb60ACPType2 = {0x36, 0x1D, 0x11, 0x25, 0x11, 0x01, 0x07, 0x00, 0x1B,
                                      0x1B, 0x24, 0x07, 0xF8, 0x00, 0x00, 0x1E, 0x1E, 0x60,
                                      0x01, 0x0A, 0x00, 0x05, 0x04, 0x03, 0xFF, 0x01};

VIMacroVisionObj VIEurgb60ACPType3 = {0x36, 0x17, 0x15, 0x21, 0x15, 0x05, 0x05, 0x02, 0x1B,
                                      0x1B, 0x24, 0x07, 0xF8, 0x00, 0x00, 0x1E, 0x1E, 0x60,
                                      0x01, 0x0A, 0x00, 0x05, 0x04, 0x03, 0xFF, 0x01};

VIMacroVisionObj VIMpalACPType1 = {0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B,
                                   0x1B, 0x24, 0x07, 0xF8, 0x00, 0x00, 0x0F, 0x0F, 0x60,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

VIMacroVisionObj VIMpalACPType2 = {0x36, 0x1D, 0x11, 0x25, 0x11, 0x01, 0x07, 0x00, 0x1B,
                                   0x1B, 0x24, 0x07, 0xF8, 0x00, 0x00, 0x0F, 0x0F, 0x60,
                                   0x01, 0x0A, 0x00, 0x05, 0x04, 0x03, 0xFF, 0x00};

VIMacroVisionObj VIMpalACPType3 = {0x36, 0x17, 0x15, 0x21, 0x15, 0x05, 0x05, 0x02, 0x1B,
                                   0x1B, 0x24, 0x07, 0xF8, 0x00, 0x00, 0x0F, 0x0F, 0x60,
                                   0x01, 0x0A, 0x00, 0x05, 0x04, 0x03, 0xFF, 0x00};

VIMacroVisionObj VIProgressiveACPType = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

VIMacroVisionObj VIZeroACPType = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

extern s32 __VISendI2CData(u8, u8*, s32);
extern void WaitMicroTime(s32);

static void __VISetVideoMode(VIVideo vmode, VIBool outsel) {
    u8 buffer[2];
    Vdac_Flag_Region = vmode;
    buffer[0] = 1;
    buffer[1] = (u8)((outsel << 5) | Vdac_Flag_Region);
    __VISendI2CData(0xE0, buffer, 2);
    WaitMicroTime(2);
}

static void __VISetCCSEL(VIBool ccsel) {
    u8 buffer[2];

    buffer[0] = 0x6A;
    buffer[1] = VI_ENABLE;
    __VISendI2CData((u8)0xE0, buffer, 2);
    WaitMicroTime(2);
}

static void __VISetOverSampling(VIBool os) {
    u8 buffer[2];
    buffer[0] = 0x65;
    buffer[1] = VI_ENABLE;
    __VISendI2CData((u8)0xE0, buffer, 2);
    WaitMicroTime(2);
}

void __VISetVolume(u8 volumeL, u8 volumeR) {
    u8 buffer[3];
    buffer[0] = 0x71;
    buffer[1] = volumeL;
    buffer[2] = volumeR;
    __VISendI2CData((u8)0xE0, buffer, 3);
    WaitMicroTime(2);
}

void __VISetYUVSEL(VIBool outsel) {
    u8 buffer[2];
    u32 tv;

    tv = *(u32*)OSPhysicalToCached(0xCC);

    switch (tv) {
    case VI_PAL:
    case VI_EURGB60:
        Vdac_Flag_Region = VI_VMODE_PAL;
        break;
    case VI_MPAL:
        Vdac_Flag_Region = VI_VMODE_MPAL;
        break;
    case VI_NTSC:
        Vdac_Flag_Region = VI_VMODE_NTSC;
        break;
    default:
        ASSERTMSGLINE(919, FALSE, "Unkwon TV mode!! TV mode is set to NTSC mode.\n");
        Vdac_Flag_Region = VI_VMODE_NTSC;
        break;
    }

    buffer[0] = 0x1;
    buffer[1] = (u8)((outsel << 5) | Vdac_Flag_Region);
    __VISendI2CData((u8)0xE0, buffer, 2);
    WaitMicroTime(2);
}

void __VISetTiming(VITiming timing) {
    u8 buffer[2];

    buffer[0] = 0x0;
    buffer[1] = timing;
    __VISendI2CData((u8)0xE0, buffer, 2);
    WaitMicroTime(2);
}

void __VISetFilter4EURGB60(VIBool enable) {
    u8 buffer[2];
    buffer[0] = 0x6E;
    buffer[1] = (u8)enable;
    __VISendI2CData((u8)0xE0, buffer, 2);
    WaitMicroTime(2);
}

void __VISetVBICtrl(VIBool cgms, VIBool wss, VIBool cc) {
    u8 buffer[2];

    buffer[0] = 0x2;
    buffer[1] = (u8)(((~wss & 0x1) << 2) | ((~cgms & 0x1) << 1) | (~cc & 0x1));
    __VISendI2CData((u8)0xE0, buffer, 2);
    WaitMicroTime(2);
}

static u8 __wd0 = 0xFF;
static u8 __wd1 = 0xFF;
static u8 __wd2 = 0xFF;

void __VISetCGMS(void) {
    u8 buffer[3];
    buffer[0] = 5;
    buffer[1] = (u8)(((__wd1 & 0xF) << 2) | (__wd0 & 3));
    buffer[2] = __wd2;
    __VISendI2CData((u8)0xE0, buffer, 3);
    WaitMicroTime(2);
}

void __VISetCGMSClear(void) {
    __wd0 = 0;
    __wd1 = 0;
    __wd2 = 0;
    __VISetCGMS();
}

#if !defined(GAME_REXE01)  // Excite Truck's VI data has no such string
// NOTE: function doesn't exist in TP debug, needed for string data
void VISetCGMS(void) {
    OSReport("VISetCGMS(): Tried to set APS Trigger bit(WORD2[3:2]) \nwhen the scan is progressive in DTV mode, which is forbidden\n");
}
#endif

static u8 __gp1 = 0xFF;
static u8 __gp2 = 0xFF;
static u8 __gp3 = 0xFF;
static u8 __gp4 = 0xFF;

void __VISetWSS(void) {
    u8 buffer[3];
    buffer[0] = 0x8;
    buffer[1] = (u8)(((__gp2 & 0xf) << 4) | (__gp1 & 0xf));
    buffer[2] = (u8)(((__gp4 & 0x7) << 3) | (__gp3 & 0x7));
    __VISendI2CData((u8)0xE0, buffer, 3);
    WaitMicroTime(2);
}

void VISetWSS(u8 gp1, u8 gp2, u8 gp3, u8 gp4) {
    if ((__gp1 == gp1) && (__gp2 == gp2) && (__gp3 == gp3) && (__gp4 == gp4)) {
        return;
    }

    __gp1 = gp1;
    __gp2 = gp2;
    __gp3 = gp3;
    __gp4 = gp4;

    Vdac_Flag_Changed |= 2;
}

static u8 __cc1 = 0xFF;
static u8 __cc2 = 0xFF;
static u8 __cc3 = 0xFF;
static u8 __cc4 = 0xFF;
#if SDK_SEP2006
static u32 __tvType = 0xFF;
#endif

void __VISetClosedCaption(void) {
    u8 buffer[5];
    buffer[0] = 0x7A;
    buffer[1] = (u8)(__cc1 & 0x7F);
    buffer[2] = (u8)(__cc2 & 0x7F);
    buffer[3] = (u8)(__cc3 & 0x7F);
    buffer[4] = (u8)(__cc4 & 0x7F);
    __VISendI2CData((u8)0xE0, buffer, 5);
    WaitMicroTime(2);
}

void VISetClosedCaption(u8 cc1, u8 cc2, u8 cc3, u8 cc4) {
    if ((__cc1 == cc1) && (__cc2 == cc2) && (__cc3 == cc3) && (__cc4 == cc4)) {
        return;
    }

    __cc1 = cc1;
    __cc2 = cc2;
    __cc3 = cc3;
    __cc4 = cc4;

    Vdac_Flag_Changed |= 4;
}

void __VISetMacrovisionImm(VIMacroVisionObj* mparam) {
    u8 i;
    u8 buffer[27];

    buffer[0] = 0x40;

    for (i = 1; i < 27; i++) {
        buffer[i] = mparam->m[i - 1];
    }

    __VISendI2CData((u8)0xE0, buffer, 27);
    WaitMicroTime(2);
}

void __VISetMacrovision(void) {
    switch (__type) {
    case 2:
        switch (__tvType) {
        case VI_NTSC:
            __VISetMacrovisionImm(&VINtscACPType1);
            break;
        case VI_PAL:
            __VISetMacrovisionImm(&VIPalACPType1);
            break;
        case VI_MPAL:
            __VISetMacrovisionImm(&VIMpalACPType1);
            break;
        case VI_EURGB60:
            __VISetMacrovisionImm(&VIEurgb60ACPType1);
            break;
        }
        break;
    case 3:
        switch (__tvType) {
        case VI_NTSC:
            __VISetMacrovisionImm(&VINtscACPType2);
            break;
        case VI_PAL:
            __VISetMacrovisionImm(&VIPalACPType2);
            break;
        case VI_MPAL:
            __VISetMacrovisionImm(&VIMpalACPType2);
            break;
        case VI_EURGB60:
            __VISetMacrovisionImm(&VIEurgb60ACPType2);
            break;
        }
        break;
    case 4:
        switch (__tvType) {
        case VI_NTSC:
            __VISetMacrovisionImm(&VINtscACPType3);
            break;
        case VI_PAL:
            __VISetMacrovisionImm(&VIPalACPType3);
            break;
        case VI_MPAL:
            __VISetMacrovisionImm(&VIMpalACPType3);
            break;
        case VI_EURGB60:
            __VISetMacrovisionImm(&VIEurgb60ACPType3);
            break;
        }
        break;
    case 1:
        __VISetMacrovisionImm(&VIZeroACPType);
        break;
    }
}

void __VISetGammaImm(VIGammaObj* gamma) {
    u8 i, j;
    u8 buffer[34];

    buffer[0] = 0x10;
    i = 1;
    for (j = 0; j < 6; j++) {
        buffer[i] = (u8)((gamma->a[j] >> 8) & 0xFF);
        buffer[i + 1] = (u8)(gamma->a[j] & 0xFF);
        i += 2;
    }

    for (j = 0; j < 7; j++) {
        buffer[i] = gamma->yin[j];
        i++;
    }

    for (j = 0; j < 7; j++) {
        buffer[i] = (u8)((gamma->yout[j] >> 8) & 0xFF);
        buffer[i + 1] = (u8)(gamma->yout[j] & 0xC0);
        i += 2;
    }

    __VISendI2CData((u8)0xE0, buffer, 34);
    WaitMicroTime(2);
}

void __VISetGamma1_0(void) {
    __VISetGammaImm(&gammaSet[10]);
}

static VIGamma __gamma;
void __VISetGamma(void) {
    __VISetGammaImm(&gammaSet[__gamma]);
}

void VISetGamma(VIGamma gamma) {
    if (__gamma != gamma) {
        __gamma = gamma;
        Vdac_Flag_Changed |= 0x10;
    }
}

static VIBool __filter = 0xFF;

void __VISetTrapFilterImm(VIBool filter) {
    u8 buffer[2];
    buffer[0] = 0x3;

    if (filter == VI_TRUE) {
        buffer[1] = VI_FALSE;
    } else {
        buffer[1] = VI_TRUE;
    }
    __VISendI2CData((u8)0xE0, buffer, 2);
    WaitMicroTime(2);
}

void __VISetTrapFilter(void) {
    u8 buffer[2];

    buffer[0] = 0x3;

    if (__filter == VI_TRUE) {
        buffer[1] = VI_FALSE;
    } else {
        buffer[1] = VI_TRUE;
    }

    __VISendI2CData((u8)0xE0, buffer, 2);
    WaitMicroTime(2);
}

void VISetTrapFilter(VIBool filter) {
    if (__filter == filter) {
        return;
    }

    __filter = filter;
    Vdac_Flag_Changed |= 0x20;
}

static VIOverDrive __level;

void __VISetRGBOverDrive(void) {
    u8 buffer[2];

    if (Vdac_Flag_Region == VI_VMODE_RGB) {
        buffer[0] = 0xA;
        buffer[1] = (u8)((__level << 1) | VI_ENABLE);
        __VISendI2CData((u8)0xE0, buffer, 2);
        WaitMicroTime(2);
    } else {
        buffer[0] = 0xA;
        buffer[1] = VI_DISABLE;
        __VISendI2CData((u8)0xE0, buffer, 2);
        WaitMicroTime(2);
    }
}

void VISetRGBOverDrive(VIOverDrive level) {
    if (__level == level) {
        return;
    }
    __level = level;
    Vdac_Flag_Changed |= 0x40;
}

void VISetRGBModeImm(void) {
    Vdac_Flag_Changed |= 0x80;
}

void __VISetRGBModeImm(void) {
    __VISetVideoMode(VI_VMODE_RGB, VI_DISABLE);
}

void __VISetRevolutionModeSimple(void) {
    u32 dtv;

    __VISetCCSEL(VI_TRUE);
    __VISetOverSampling(VI_TRUE);
    dtv = VIGetDTVStatus();
    __VISetYUVSEL((VIBool)dtv);
    __VISetTiming(VI_TMG_GAME);
    __VISetVolume(0x8E, 0x8E);
    __VISetVBICtrl(VI_DISABLE, VI_DISABLE, VI_DISABLE);
    __VISetCGMSClear();
    VISetWSS(0, 0, 0, 0);
    __VISetWSS();
    VISetClosedCaption(0, 0, 0, 0);
    __VISetClosedCaption();
    __VISetMacrovisionImm(&VIZeroACPType);
#if SDK_SEP2006
    VISetRGBOverDrive(VI_ODV_L2);
#else
    VISetRGBOverDrive(VI_ODV_L1);
#endif
    __VISetRGBOverDrive();
    __VISetTrapFilterImm(VI_DISABLE);
    __VISetGammaImm(&gammaSet[VI_GM_1_0]);
#if !SDK_SEP2006
    __current_3in1_video_mode = 0;
#endif
}
