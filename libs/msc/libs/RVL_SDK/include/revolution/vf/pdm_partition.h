#ifndef RVL_SDK_VF_PDM_PARTITION_H
#define RVL_SDK_VF_PDM_PARTITION_H
#include <revolution/types.h>
#ifdef __cplusplus
extern "C" {
#endif

s32 VFipdm_part_get_start_sector(struct PDM_PARTITION* p_part);
u32 VFipdm_part_chg_ltop(struct PDM_PARTITION* p_part, u32 lsector, u16 lbps);
s32 VFipdm_part_get_handle(struct PDM_DISK* p_disk, u16 part_id, struct PDM_PARTITION** pp_part, u16* p_part_no, u16* p_handle_no);
#ifdef GAME_REXE01
s32 VFipdm_part_open_partition(struct PDM_DISK* p_disk, s32 part_id, struct PDM_PARTITION** pp_part);
#else
s32 VFipdm_part_open_partition(struct PDM_DISK* p_disk, u16 part_id, struct PDM_PARTITION** pp_part);
#endif
s32 VFipdm_part_close_partition(struct PDM_PARTITION* p_part);
s32 VFipdm_part_get_permission(struct PDM_PARTITION* p_part);
s32 VFipdm_part_release_permission(struct PDM_PARTITION* p_part, u32 mode);
#ifdef GAME_REXE01
// ET's older pdm layer (0x800A6048-0x800A7774). Names not taken from a reference are guessed.
typedef struct PDM_MEDIA_CALLBACK {
    void (*insert)(void* p_part);
    void (*eject)(void* p_part);
} PDM_MEDIA_CALLBACK;

s32 VFipdm_part_format(struct PDM_PARTITION* p_part);
s32 VFipdm_part_set_media_callback(struct PDM_PARTITION* p_part, struct PDM_MEDIA_CALLBACK* p_callback);
s32 VFipdm_part_clear_media_callback(struct PDM_PARTITION* p_part);
s32 VFipdm_part_open(struct PDM_PARTITION* p_part);
s32 VFipdm_part_close(struct PDM_PARTITION* p_part);
#else
s32 VFipdm_part_format(struct PDM_PARTITION* p_part, const u8* param);
#endif
s32 VFipdm_part_logical_read(struct PDM_PARTITION* p_part, u8* buf, u32 lsector, u32 num_sector, u16 bps, u32* p_num_success);
s32 VFipdm_part_logical_write(struct PDM_PARTITION* p_part, const u8* buf, u32 lsector, u32 num_sector, u16 bps, u32* p_num_success);
s32 VFipdm_part_get_media_information(struct PDM_PARTITION* p_part, struct PDM_DISK_INFO* p_disk_info);
s32 VFipdm_part_check_media_write_protect(struct PDM_PARTITION* p_part, u32* is_wprotect);
s32 VFipdm_part_check_media_insert(struct PDM_PARTITION* p_part, u32* is_insert);
s32 VFipdm_part_check_media_detect(struct PDM_PARTITION* p_part, u32* is_detect);
void VFipdm_part_set_change_media_state(struct PDM_DISK* p_disk, u32 event);
void VFipdm_part_set_driver_error_code(struct PDM_PARTITION* p_part, s32 error_code);
s32 VFipdm_part_get_driver_error_code(struct PDM_PARTITION* p_part);

#ifdef __cplusplus
}
#endif
#endif
