#include <revolution/vf/vf_struct.h>
#include <revolution/vf/pf_fsnext.h>
#include <revolution/vf/pf_api_util.h>
#include <revolution/vf/pf_dir.h>

int VFipf2_fsnext(struct PF_DTA* p_dta) {
    s32 err;
    err = VFiPFAPI_convertReturnValue(VFiPFDIR_fsnext(p_dta));
    return err;
}
