#include <revolution/vf/pf_attach.h>
#include <revolution/vf/pf_api_util.h>
#include <revolution/vf/pf_volume.h>

int VFipf2_attach(struct PF_DRV_TBL** drv_tbl, struct PF_ET_DEV_CONFIG* p_config) {
    s32 err;

    if (drv_tbl == NULL || *drv_tbl == NULL) {
        err = 10;
        VFipf_vol_set.last_error = 10;
    } else {
        while (*drv_tbl != NULL) {
            if (p_config != NULL) {
                err = VFiPFVOL_attach(*drv_tbl, p_config->name, p_config->param);
            } else {
                err = VFiPFVOL_attach(*drv_tbl, NULL, 0);
            }
            if (err != 0) {
                return VFiPFAPI_convertReturnValue(err);
            }
            drv_tbl++;
        }
    }
    return VFiPFAPI_convertReturnValue(err);
}
