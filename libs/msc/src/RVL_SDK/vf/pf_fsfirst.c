#include <revolution/vf/vf_struct.h>
#include <revolution/vf/pf_fsfirst.h>
#include <revolution/vf/pf_api_util.h>
#include <revolution/vf/pf_dir.h>
#include <revolution/vf/pf_str.h>
#include <revolution/vf/pf_volume.h>

int VFipf2_fsfirst(const s8* path, u8 attr, struct PF_DTA* p_dta) {
    struct PF_STR path_str;
    s32 err;

    err = VFiPFSTR_InitStr(&path_str, path, 1);
    if (err == 0) {
        err = VFiPFDIR_fsfirst(&path_str, attr, p_dta);
    } else {
        VFipf_vol_set.last_error = err;
    }
    return VFiPFAPI_convertReturnValue(err);
}
