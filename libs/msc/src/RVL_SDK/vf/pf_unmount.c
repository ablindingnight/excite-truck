#include <revolution/vf/vf_struct.h>
#include <revolution/vf/pf_api_util.h>
#include <revolution/vf/pf_volume.h>

int VFipf2_unmount(s8 drive, u32 mode) {
    s32 err;
    err = VFiPFAPI_convertReturnValue4unmount(VFiPFVOL_unmount(drive, mode));
    return err;
}
