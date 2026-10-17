#ifndef RVL_SDK_VF_PF_ATTACH_H
#define RVL_SDK_VF_PF_ATTACH_H
#include <revolution/vf/vf_struct.h>
#ifdef __cplusplus
extern "C" {
#endif

#ifdef GAME_REXE01
// ET passes a device description to every attached drive (layout from VFipf2_attach's use).
typedef struct PF_ET_DEV_CONFIG {
    s8 name[0x50];
    u32 param;
} PF_ET_DEV_CONFIG;

int VFipf2_attach(struct PF_DRV_TBL** drv_tbl, struct PF_ET_DEV_CONFIG* p_config);
#else
int VFipf2_attach(struct PF_DRV_TBL** drv_tbl);
#endif

#ifdef __cplusplus
}
#endif
#endif
