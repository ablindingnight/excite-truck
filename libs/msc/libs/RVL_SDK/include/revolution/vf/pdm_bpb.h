#ifndef RVL_SDK_VF_PDM_BPB_H
#define RVL_SDK_VF_PDM_BPB_H
#include <revolution/types.h>
#ifdef __cplusplus
extern "C" {
#endif

void VFipdm_bpb_calculate_common_bpb_fields(struct PDM_BPB* p_bpb);
s32 VFipdm_bpb_get_bpb_information(u8* buf, struct PDM_BPB* p_bpb);
s32 VFipdm_bpb_get_fsinfo_information(u8* buf, struct PDM_FSINFO* p_fsinfo);
s32 VFipdm_bpb_set_fsinfo_information(struct PDM_FSINFO* p_fsinfo, u8* buf);
s32 VFipdm_bpb_check_boot_sector(u8* buf, u32* is_boot);
s32 VFipdm_bpb_check_fsinfo_sector(u8* buf, u32* is_fsinfo);

#ifdef GAME_REXE01
// ET's pdm_bpb fills the volume directly (names guessed).
s32 VFipdm_bpb_get_volume_bpb(struct PF_VOLUME* p_vol, u8* buf);
s32 VFipdm_bpb_get_volume_fsinfo(struct PF_VOLUME* p_vol, u8* buf, u32* p_free_count, u32* p_next_free);
#endif

#ifdef __cplusplus
}
#endif
#endif
