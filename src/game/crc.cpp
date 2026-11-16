// CRC-32 (0x80132E7C-0x80133010): MSB-first, polynomial 0x04C11DB7, with the first four bytes taken
// inverted as the initial value (no augmentation). Used for resource and save-data checks.
#include "game/common.h"

ModuleTag lbl_8055E068;

// DUMP_DATA lbl_802F8F08 u32
static const u32 lbl_802F8F08[256] = {
#include "dumpdata/lbl_802F8F08.inc"
};

extern "C" u32 fn_80132E7C(const void* data, s32 len) {
    const u8* p = (const u8*)data;
    u32 crc;
    int i;
    if (len >= 4) {
        crc = p[0] << 24;
        crc |= p[1] << 16;
        crc |= p[2] << 8;
        crc |= p[3];
        crc = ~crc;
        p += 4;
        len -= 4;
    } else {
        crc = lbl_802F8F08[128];
    }
    for (i = 0; i < len; i++) {
        crc = (crc << 8 | *p++) ^ lbl_802F8F08[crc >> 24];
    }
    return ~crc;
}
