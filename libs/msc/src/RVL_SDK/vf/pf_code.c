#include <revolution/vf/pf_code.h>

// DUMP_DATA VFipf_valid_fn_char u8
const u8 VFipf_valid_fn_char[96] = {
#include "dumpdata/VFipf_valid_fn_char.inc"
};

s32 VFiPFCODE_Combine_Width(s16 oem_width, s16 uni_width) {
    return (oem_width << 16) + uni_width;
}

void VFiPFCODE_Divide_Width(s32 width, s16* oem_width, s16* uni_width) {
    *oem_width = (u32)width >> 16;
    *uni_width = width;
}
