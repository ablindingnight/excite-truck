#ifndef RVL_SDK_VF_VF_ET_H
#define RVL_SDK_VF_VF_ET_H
// Excite Truck's VF device layer (0x800B7968-~0x800BA49C): public front end, OS glue, a NAND-file
// drive and the SD drive. No reference exists; names are guesses.
#include <revolution/vf/vf_struct.h>
#include <revolution/vf/pf_dir.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef void (*VF_ET_MEDIA_CALLBACK)(u8 slot);

// Front end (vf_et_api.c)
s32 VFAttachDrive(u8 type, const s8* dev_name, u32 dev_param, struct PF_DRV_TBL* p_drv);
s32 VFDetachDrive(s8 drive);
s32 VFCloseFile(struct PF_FILE* p_file);
s32 VFGetFileInfo(struct PF_FILE* p_file, struct PF_INFO* p_info);
struct PF_FILE* VFOpenFile(const s8* path, const s8* mode);
u32 VFReadFile(void* p_buf, u32 size, u32 count, struct PF_FILE* p_file);
s32 VFSeekFile(struct PF_FILE* p_file, s32 offset, s32 origin);
s32 VFFindFirst(const s8* path, u8 attr, struct PF_DTA* p_dta);
s32 VFFindNext(struct PF_DTA* p_dta);
s32 VFInit(s32 config);
s32 VFSetMediaCallback(s32 type, VF_ET_MEDIA_CALLBACK insert, VF_ET_MEDIA_CALLBACK eject);
s32 VFUnmountDrive(s8 drive, u32 mode);

// Drives
s32 VFi_nanddrv_init_drv_tbl(struct PDM_DISK_TBL* p_disk_tbl, u32 ui_ext);  // name from msc nand_drv.h
s32 VFi_nanddrv_register_image(const s8* path, u32 size);
s32 VFi_sddrv_init_drv_tbl(struct PDM_DISK_TBL* p_disk_tbl, u32 ui_ext);
s32 VFiSD_SetCallback(void (*insert)(s32), void (*eject)(s32));
s32 VFiSD_IsInserted(void);

#ifdef __cplusplus
}
#endif
#endif
