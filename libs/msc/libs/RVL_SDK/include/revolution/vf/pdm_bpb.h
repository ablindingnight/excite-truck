#ifndef RVL_SDK_VF_PDM_BPB_H
#define RVL_SDK_VF_PDM_BPB_H
#include <revolution/types.h>
#ifdef __cplusplus
extern "C" {
#endif

#ifdef GAME_REXE01
void VFipdm_bpb_calculate_common_bpb_fields(struct PF_BPB* p_bpb);
#else
void VFipdm_bpb_calculate_common_bpb_fields(struct PDM_BPB* p_bpb);
#endif
s32 VFipdm_bpb_get_bpb_information(u8* buf, struct PDM_BPB* p_bpb);
s32 VFipdm_bpb_get_fsinfo_information(u8* buf, struct PDM_FSINFO* p_fsinfo);
s32 VFipdm_bpb_set_fsinfo_information(struct PDM_FSINFO* p_fsinfo, u8* buf);
s32 VFipdm_bpb_check_boot_sector(u8* buf, u32* is_boot);
s32 VFipdm_bpb_check_fsinfo_sector(u8* buf, u32* is_fsinfo);

#ifdef GAME_REXE01
// ET's pdm_bpb fills the volume directly (names guessed).
// 16-bit fields are read bytewise when
// unaligned; otherwise through a naturally aligned view of the boot sector, so an odd field
// offset reads the halfword at the next even offset.
#define PDM_LE16(buf, off) (((u32)((buf) + (off)) & 1) != 0 ? (u16)((buf)[(off)] | ((buf)[(off) + 1] << 8)) : (u16)SWAP16(*(u16*)((buf) + (((off) + 1) & ~1))))
#define PDM_LE32(p) (u32)(SWAP32(*(u32*)(p)))

s32 VFipdm_bpb_get_volume_bpb(struct PF_VOLUME* p_vol, u8* buf);
s32 VFipdm_bpb_get_volume_fsinfo(struct PF_VOLUME* p_vol, u8* buf, u32* p_free_count, u32* p_next_free);
// Name unknown (0x800A6A40): nonzero when buf holds a valid boot sector.
u32 VFipdm_ET_IsBootSector(u8* buf);
#endif

#ifdef __cplusplus
}
#endif
#endif
