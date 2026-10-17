#include <revolution/vf.h>

struct PF_VOLUME_SET VFipf_vol_set;

#ifdef GAME_REXE01
// ET's volume layer: PrFILE2 configured for PF_ET_NUM_CONTEXT tasks, volume flags laid out
// differently (1 attached, 2 write protected, 4 inserted, 8 mounted, 0x10 insert detected,
// 0x20 eject detected, 0x40 format on mount, 0x80 keep FAT on format), and critical sections
// instead of argument checks.
#define PF_VOL_ATTACHED 0x1
#define PF_VOL_MOUNTED 0x8
#define PF_VOL_FORMAT 0x40
#define PF_VOL_NO_FAT_INIT 0x80

static u32 VFiPFVOL_CheckContextRegistered(s32 context_id) {
    u32 i;
    for (i = 1; i < PF_ET_NUM_CONTEXT; i++) {
        VFipf_vol_set.context[i].stat &= 1;
        if (VFipf_vol_set.context[i].stat != 0 && VFipf_vol_set.context[i].context_id == context_id) {
            return 1;
        }
    }
    return 0;
}

static s32 VFiPFVOL_SetUpVolumeForMount(struct PF_VOLUME* p_vol) {
    p_vol->last_error = 0;
    p_vol->last_driver_error = 0;
    return 0;
}

static s32 VFiPFVOL_InitCurrentDir(struct PF_VOLUME* p_vol) {
    s32 err;
    s32 context_id;
    u32 i;

    if ((p_vol->flags & PF_VOL_MOUNTED) == 0) {
        return 9;
    }

    VFiPFSYS_GetCurrentContextID(&context_id);
    p_vol->current_dir[0].stat |= 1;

    if (VFiPFVOL_CheckContextRegistered(context_id)) {
        p_vol->current_dir[1].stat |= 1;
        p_vol->current_dir[1].context_id = context_id;
    }

    for (i = 0; i < PF_ET_NUM_CONTEXT; i++) {
        err = VFiPFENT_GetRootDir(p_vol, &p_vol->current_dir[i].directory);
        if (err) {
            return err;
        }
    }

    return 0;
}

static s32 VFiPFVOL_FinalizeCurrentDir(struct PF_VOLUME* p_vol) {
    u32 i;

    if ((p_vol->flags & PF_VOL_MOUNTED) == 0) {
        return 9;
    }
    for (i = 0; i < PF_ET_NUM_CONTEXT; i++) {
        p_vol->current_dir[i].stat = 0;
    }

    return 0;
}

s32 VFiPFVOL_DoMountVolume(struct PF_VOLUME* p_vol) {
    s32 err;

    VFiPFVOL_SetUpVolumeForMount(p_vol);
    err = VFiPFDRV_mount(p_vol);
    if (err) {
        return err;
    }

    if (!p_vol->bpb.bytes_per_sector || (p_vol->bpb.bytes_per_sector & 0x1FF) != 0) {
        return 15;
    }

    // The cache pages are 512 bytes: a sector must fit in the FAT cache and twice in the data cache.
    if ((p_vol->bpb.bytes_per_sector >> 9) > p_vol->cache.num_fat_pages || (p_vol->bpb.bytes_per_sector >> 9) * 2 > p_vol->cache.num_data_pages) {
        return 30;
    }

    err = VFiPFCACHE_InitCaches(p_vol);
    if (err) {
        return err;
    }

    p_vol->flags |= PF_VOL_MOUNTED;
    err = VFiPFVOL_InitCurrentDir(p_vol);
    if (!err && (p_vol->flags & PF_VOL_FORMAT) != 0) {
        err = VFiPFDRV_format(p_vol, p_vol->format_param);
        if (!err && (p_vol->flags & PF_VOL_NO_FAT_INIT) == 0) {
            err = VFiPFFAT_InitFATRegion(p_vol);
            if (!err) {
                err = VFiPFENT_MakeRootDir(p_vol);
            }
        }
    }
    if (err) {
        p_vol->flags &= ~PF_VOL_MOUNTED;
    }
    return err;
}

static s32 VFiPFVOL_DoUnmountVolume(struct PF_VOLUME* p_vol, u32 mode) {
    s32 err;

    err = VFiPFDRV_unmount(p_vol, mode);
    if (err) {
        return err;
    }

    VFiPFVOL_FinalizeCurrentDir(p_vol);
    p_vol->flags &= ~PF_VOL_MOUNTED;
    return 0;
}

static void VFiPFVOL_UnmountVolumeByEject(struct PF_VOLUME* p_vol) {
    VFiPFFILE_FinalizeAllFiles(p_vol);
    VFiPFDIR_FinalizeAllDirs(p_vol);
    VFiPFVOL_DoUnmountVolume(p_vol, 1);
    VFiPFDRV_ClearEjected(p_vol);
    VFipf_vol_set.num_mounted_volumes--;
}

static s32 VFiPFVOL_p_attach(struct PF_VOLUME* p_vol, struct PF_DRV_TBL* p_drv, s16 vol_idx) {
    s32 err;

    VFipf_memset(p_vol, 0, sizeof(struct PF_VOLUME));
    p_vol->p_part = p_drv->p_part;
    p_vol->drv_char = vol_idx + 'A';
    VFiPF_InitCritical(&p_vol->critical);
    p_vol->tail_entry.tracker_size = 1;
    p_vol->tail_entry.tracker_bits = p_vol->tail_entry.tracker_buff;

    err = VFiPFDRV_init(p_vol);
    if (err != 0) {
        return err;
    }

    VFiPFCACHE_SetCache(p_vol, p_drv->cache->pages, p_drv->cache->buffers, p_drv->cache->num_fat_pages, p_drv->cache->num_data_pages);
    VFiPFCACHE_SetFATBufferSize(p_vol, p_drv->cache->num_fat_buf_size);
    VFiPFCACHE_SetDataBufferSize(p_vol, p_drv->cache->num_data_buf_size);
    return 0;
}

static s32 VFiPFVOL_p_detach(struct PF_VOLUME* p_vol) {
    s32 err;
    err = VFiPFDRV_finalize(p_vol);
    if (err) {
        return err;
    }
    VFiPF_DeleteCritical(&p_vol->critical);
    return 0;
}

static s32 VFiPFVOL_p_mount(struct PF_VOLUME* p_vol) {
    s32 err;

    if ((p_vol->flags & PF_VOL_MOUNTED) == 0) {
        err = VFiPFVOL_DoMountVolume(p_vol);
        if (err != 0) {
            return err;
        }

        p_vol->fsi_flag &= ~7;
        VFiPFDRV_ClearEjected(p_vol);
        VFipf_vol_set.num_mounted_volumes++;
    }
    VFiPFDRV_ClearDetected(p_vol);
    return 0;
}

s32 VFiPFVOL_p_unmount(struct PF_VOLUME* p_vol, u32 mode) {
    s32 err;
    s32 err2;

    err = 0;

    if ((p_vol->flags & PF_VOL_MOUNTED) == 0) {
        return 9;
    }

    VFiPFFILE_FinalizeAllFiles(p_vol);
    VFiPFDIR_FinalizeAllDirs(p_vol);
    err2 = VFiPFCACHE_FlushAllCaches(p_vol);
    if (err2 != 0) {
        err = err2;
    }

    if (err2 == 0 || (mode & 1) != 0) {
        VFiPFCACHE_FreeAllCaches(p_vol);
        err2 = VFiPFVOL_DoUnmountVolume(p_vol, mode);
        if (err2 != 0 && err == 0) {
            err = err2;
        }
    }

    if (err == 0 || (mode & 1) != 0) {
        VFiPFDRV_ClearDetected(p_vol);
        VFiPFDRV_ClearEjected(p_vol);
        VFipf_vol_set.num_mounted_volumes--;
    }

    return err;
}

static s32 VFiPFVOL_CheckMediaInsert(struct PF_VOLUME* p_vol) {
    s32 err;

    if (VFiPFDRV_IsInserted(p_vol) != 0) {
        if (VFiPFDRV_IsEjected(p_vol) != 0 && (p_vol->flags & PF_VOL_MOUNTED) == 0) {
            VFiPFVOL_UnmountVolumeByEject(p_vol);
        }
        if (VFiPFDRV_IsDetected(p_vol) != 0) {
            err = VFiPFVOL_p_mount(p_vol);
            if (err != 0) {
                return err;
            }
        }
    } else {
        if (VFiPFDRV_IsEjected(p_vol) != 0 && (p_vol->flags & PF_VOL_MOUNTED) == 0) {
            VFiPFVOL_UnmountVolumeByEject(p_vol);
        }
        if (VFiPFDRV_IsDetected(p_vol) != 0) {
            VFiPFDRV_ClearDetected(p_vol);
        }
    }
    return 0;
}

s32 VFiPFVOL_InitModule(u32 config, void* param) {
    s32 vol_idx;
    u32 i;

    if ((config & 0x10000) != 0) {
        VFipf_vol_set.config |= 0x10000;
    } else {
        VFipf_vol_set.config &= ~0x10000;
    }

    for (i = 0; i < PF_ET_NUM_CONTEXT; i++) {
        VFipf_vol_set.current_vol[i].p_vol = &VFipf_vol_set.volumes[0];
    }
    VFipf_vol_set.current_vol[0].stat |= 1;
    VFipf_vol_set.num_attached_drives = 0;
    VFipf_vol_set.num_mounted_volumes = 0;

    if ((config & 0x10000) != 0) {
        VFipf_vol_set.config |= 0x10000;
    } else {
        VFipf_vol_set.config &= ~0x10000;
    }

    VFipf_vol_set.param = param;
    VFipf_vol_set.last_error = 0;
    VFipf_vol_set.last_driver_error = 0;
    VFipf_vol_set.setting = 1;

    VFipf_vol_set.codeset.oem2unicode = &VFiPFCODE_CP932_OEM2Unicode;
    VFipf_vol_set.codeset.unicode2oem = &VFiPFCODE_CP932_Unicode2OEM;
    VFipf_vol_set.codeset.oem_char_width = &VFiPFCODE_CP932_OEMCharWidth;
    VFipf_vol_set.codeset.is_oem_mb_char = &VFiPFCODE_CP932_isOEMMBchar;
    VFipf_vol_set.codeset.unicode_char_width = &VFiPFCODE_CP932_UnicodeCharWidth;
    VFipf_vol_set.codeset.is_unicode_mb_char = &VFiPFCODE_CP932_isUnicodeMBchar;

    for (vol_idx = 0; vol_idx < 26; vol_idx++) {
        VFipf_memset(&VFipf_vol_set.volumes[vol_idx], 0, sizeof(struct PF_VOLUME));
    }

    VFiPF_InitCritical(&VFipf_vol_set.critical);
    VFiPF_InitLockFile();

    return 0;
}

s32 VFiPFVOL_CheckForRead(struct PF_VOLUME* p_vol) {
    s32 err;

    err = VFiPFVOL_CheckMediaInsert(p_vol);
    if (err != 0) {
        return err;
    }

    if (!(p_vol->flags & PF_VOL_MOUNTED)) {
        return 9;
    }

    return 0;
}

s32 VFiPFVOL_CheckForWrite(struct PF_VOLUME* p_vol) {
    s32 err;

    err = VFiPFVOL_CheckMediaInsert(p_vol);
    if (err != 0) {
        return err;
    }

    if ((p_vol->flags & PF_VOL_MOUNTED) == 0) {
        return 9;
    }

    if (p_vol->flags & 2) {
        return 11;
    }

    return 0;
}

s32 VFiPFVOL_GetCurrentDir(struct PF_VOLUME* p_vol, struct PF_DIR_ENT* p_current_dir) {
    s32 context_id;
    u32 i;

    if ((p_vol->flags & PF_VOL_MOUNTED) == 0) {
        return 9;
    }

    VFiPF_LockCritical(&VFipf_vol_set.critical);
    VFiPFSYS_GetCurrentContextID(&context_id);

    for (i = 1; i < PF_ET_NUM_CONTEXT; i++) {
        if ((p_vol->current_dir[i].stat & 1) != 0 && p_vol->current_dir[i].context_id == context_id) {
            *p_current_dir = p_vol->current_dir[i].directory;
            break;
        }
    }

    if (i == PF_ET_NUM_CONTEXT) {
        if (VFiPFVOL_CheckContextRegistered(context_id)) {
            for (i = 1; i < PF_ET_NUM_CONTEXT; i++) {
                if ((p_vol->current_dir[i].stat & 1) == 0) {
                    p_vol->current_dir[i].stat |= 1;
                    p_vol->current_dir[i].context_id = context_id;
                    *p_current_dir = p_vol->current_dir[i].directory;
                    break;
                }
            }
        } else {
            *p_current_dir = p_vol->current_dir[0].directory;
        }
    }

    VFiPF_UnlockCritical(&VFipf_vol_set.critical);
    return 0;
}

struct PF_VOLUME* VFiPFVOL_GetCurrentVolume() {
    struct PF_VOLUME* p_vol;
    s32 context_id;
    u32 i;

    VFiPF_LockCritical(&VFipf_vol_set.critical);
    p_vol = NULL;
    VFiPFSYS_GetCurrentContextID(&context_id);

    for (i = 1; i < PF_ET_NUM_CONTEXT; i++) {
        if ((VFipf_vol_set.current_vol[i].stat & 1) != 0 && VFipf_vol_set.current_vol[i].context_id == context_id) {
            p_vol = VFipf_vol_set.current_vol[i].p_vol;
            break;
        }
    }

    if (i == PF_ET_NUM_CONTEXT) {
        if (VFiPFVOL_CheckContextRegistered(context_id)) {
            for (i = 1; i < PF_ET_NUM_CONTEXT; i++) {
                if ((VFipf_vol_set.current_vol[i].stat & 1) == 0) {
                    VFipf_vol_set.current_vol[i].stat |= 1;
                    VFipf_vol_set.current_vol[i].context_id = context_id;
                    p_vol = VFipf_vol_set.current_vol[i].p_vol;
                    break;
                }
            }
        } else {
            p_vol = VFipf_vol_set.current_vol[0].p_vol;
        }
    }
    VFiPF_UnlockCritical(&VFipf_vol_set.critical);
    return p_vol;
}

struct PF_VOLUME* VFiPFVOL_GetVolumeFromDrvChar(s8 drv_char) {
    s16 vol_idx;

    vol_idx = VFipf_toupper(drv_char) - 'A';
    VFiPF_LockCritical(&VFipf_vol_set.critical);
    if (vol_idx < 0 || vol_idx >= 26) {
        VFiPF_UnlockCritical(&VFipf_vol_set.critical);
        return NULL;
    }

    VFiPF_UnlockCritical(&VFipf_vol_set.critical);
    return &VFipf_vol_set.volumes[vol_idx];
}

s32 VFiPFVOL_attach(struct PF_DRV_TBL* p_drv, const s8* dev_name, u32 dev_param) {
    s32 vol_idx;
    struct PF_VOLUME* p_vol;
    s32 err;

    if (p_drv->cache->num_fat_buf_size == 0) {
        p_drv->cache->num_fat_buf_size = 1;
    }
    if (p_drv->cache->num_data_buf_size == 0) {
        p_drv->cache->num_data_buf_size = 1;
    }

    p_drv->stat = 0;
    p_drv->drive = 0;

    VFiPF_LockCritical(&VFipf_vol_set.critical);
    if (dev_name != NULL && VFi_nanddrv_register_image(dev_name, dev_param) != 0) {
        VFiPF_UnlockCritical(&VFipf_vol_set.critical);
        VFipf_vol_set.last_error = 17;
        return 17;
    }

    for (vol_idx = 0; vol_idx < 26; vol_idx++) {
        p_vol = &VFipf_vol_set.volumes[vol_idx];
        if ((p_vol->flags & PF_VOL_ATTACHED) == 0) {
            break;
        }
    }

    if (vol_idx < 0 || vol_idx >= 26 || VFipf_vol_set.num_attached_drives < 0 || VFipf_vol_set.num_attached_drives >= 26) {
        VFiPF_UnlockCritical(&VFipf_vol_set.critical);
        VFipf_vol_set.last_error = 4;
        return 4;
    }

    p_vol->num_free_clusters = -1;
    p_vol->last_free_cluster = -1;

    err = VFiPFVOL_p_attach(p_vol, p_drv, vol_idx);
    if (err != 0) {
        VFiPF_UnlockCritical(&VFipf_vol_set.critical);
        VFipf_vol_set.last_error = err;
        return err;
    }

    p_vol->flags |= PF_VOL_ATTACHED;
    p_drv->stat |= 1;
    p_vol->drv_char = vol_idx + 'A';
    p_drv->drive = vol_idx + 'A';

    VFipf_vol_set.num_attached_drives++;

    if (VFiPFDRV_IsInserted(p_vol) != 0) {
        p_drv->stat |= 0x10;
        err = VFiPFVOL_p_mount(p_vol);
        if (err != 0) {
            VFipf_vol_set.last_error = err;
            p_vol->last_error = err;
            VFiPF_UnlockCritical(&VFipf_vol_set.critical);
            return 0;
        }
        p_drv->stat |= 2;
    }

    VFiPF_UnlockCritical(&VFipf_vol_set.critical);
    return 0;
}

s32 VFiPFVOL_detach(s8 drv_char) {
    struct PF_VOLUME* p_vol;
    s32 err;

    p_vol = VFiPFVOL_GetVolumeFromDrvChar(drv_char);

    if ((p_vol->flags & PF_VOL_MOUNTED) != 0) {
        VFipf_vol_set.last_error = 10;
        p_vol->last_error = 10;
        return 10;
    }

    if ((p_vol->flags & PF_VOL_ATTACHED) == 0) {
        VFipf_vol_set.last_error = 10;
        p_vol->last_error = 10;
        return 10;
    }

    VFiPF_LockCritical(&VFipf_vol_set.critical);
    err = VFiPFVOL_p_detach(p_vol);
    if (err) {
        VFiPF_UnlockCritical(&VFipf_vol_set.critical);
        VFipf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }

    p_vol->flags &= ~PF_VOL_ATTACHED;
    VFipf_vol_set.num_attached_drives--;
    VFiPF_UnlockCritical(&VFipf_vol_set.critical);

    return 0;
}

s32 VFiPFVOL_unmount(s8 drv_char, u32 mode) {
    struct PF_VOLUME* p_vol;
    s32 err;
    s32 err2;

    err2 = 0;
    p_vol = VFiPFVOL_GetVolumeFromDrvChar(drv_char);
    if ((p_vol->flags & PF_VOL_ATTACHED) == 0) {
        VFipf_vol_set.last_error = 10;
        return 10;
    }

    VFiPF_LockCritical(&p_vol->critical);
    err = VFiPFVOL_CheckMediaInsert(p_vol);
    if (err != 0) {
        VFiPF_UnlockCritical(&p_vol->critical);
        VFipf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }

    if ((p_vol->flags & PF_VOL_MOUNTED) == 0) {
        VFiPF_UnlockCritical(&p_vol->critical);
        return 0;
    }

    if (p_vol->num_opened_files != 0 || p_vol->num_opened_directories != 0) {
        err2 = 1;
        VFipf_vol_set.last_error = 19;
        p_vol->last_error = 19;
        if ((mode & 1) == 0) {
            VFiPF_UnlockCritical(&p_vol->critical);
            return 19;
        }
    }

    err = VFiPFVOL_p_unmount(p_vol, mode);
    if (err != 0 && err2 == 0) {
        VFipf_vol_set.last_error = err;
        p_vol->last_error = err;
        err2 = 1;
        if ((mode & 1) == 0) {
            err2 = err;
        }
    }

    VFiPF_UnlockCritical(&p_vol->critical);
    return err2;
}
#else
static u32 VFiPFVOL_CheckContextRegistered(s32 context_id) {
    u32 i;
    for (i = 1; i < 1; i++) {
        VFipf_vol_set.context[i].stat &= 1;
        if (VFipf_vol_set.context[i].stat != 0 && VFipf_vol_set.context[i].context_id == context_id) {
            return 1;
        }
    }
    return 0;
}

static s32 VFiPFVOL_SetUpVolumeForMount(struct PF_VOLUME* p_vol) {
    return 0;
}

static s32 VFiPFVOL_InitCurrentDir(struct PF_VOLUME* p_vol) {
    s32 err;
    s32 context_id;
    u32 i;

    if ((p_vol->flags & 2) == 0) {
        return 9;
    }

    VFiPFSYS_GetCurrentContextID(&context_id);
    p_vol->current_dir[0].stat |= 1;

    if (VFiPFVOL_CheckContextRegistered(context_id)) {
        p_vol->current_dir[1].stat |= 1;
        p_vol->current_dir[1].context_id = context_id;
    }

    for (i = 0; i < 1; i++) {
        err = VFiPFENT_GetRootDir(p_vol, &p_vol->current_dir[i].directory);
        if (err) {
            return err;
        }
    }

    return 0;
}

static s32 VFiPFVOL_FinalizeCurrentDir(struct PF_VOLUME* p_vol) {
    u32 i;

    if ((p_vol->flags & 2) == 0) {
        return 9;
    }
    for (i = 0; i < 1; i++) {
        p_vol->current_dir[i].stat = 0;
    }

    return 0;
}

s32 VFiPFVOL_DoMountVolume(struct PF_VOLUME* p_vol) {
    s32 err;

    VFiPFVOL_SetUpVolumeForMount(p_vol);
    err = VFiPFDRV_mount(p_vol);
    if (err) {
        return err;
    }

    if (!p_vol->bpb.bytes_per_sector || (p_vol->bpb.bytes_per_sector & 0x1FF) != 0) {
        return 15;
    }

    err = VFiPFCACHE_InitCaches(p_vol);
    if (err) {
        return err;
    }

    p_vol->flags |= 2u;
    err = VFiPFVOL_InitCurrentDir(p_vol);
    if (!err && (p_vol->flags & 0x10) != 0) {
        err = VFiPFDRV_format(p_vol, p_vol->format_param);
        if (!err && (p_vol->flags & 0x20) == 0) {
            err = VFiPFFAT_InitFATRegion(p_vol);
            if (!err) {
                err = VFiPFENT_MakeRootDir(p_vol);
            }
        }
    }
    if (err) {
        p_vol->flags &= ~2u;
    }
    return err;
}

static s32 VFiPFVOL_DoUnmountVolume(struct PF_VOLUME* p_vol, u32 mode) {
    s32 err;

    err = VFiPFDRV_unmount(p_vol, mode);
    if (err) {
        return err;
    }

    VFiPFVOL_FinalizeCurrentDir(p_vol);
    p_vol->flags &= ~2u;
    return 0;
}

static void VFiPFVOL_UnmountVolumeByEject(struct PF_VOLUME* p_vol) {
    VFiPFFILE_FinalizeAllFiles(p_vol);
    VFiPFDIR_FinalizeAllDirs(p_vol);
    VFiPFCACHE_FreeAllCaches(p_vol);
    VFiPFVOL_DoUnmountVolume(p_vol, 1);
    VFipf_vol_set.num_mounted_volumes--;
}

static s32 VFiPFVOL_p_attach(struct PF_VOLUME* p_vol, struct PF_DRV_TBL* p_drv, s16 vol_idx) {
    s32 err;

    VFipf_memset(p_vol, 0, sizeof(struct PF_VOLUME));
    p_vol->p_part = p_drv->p_part;
    p_vol->drv_char = vol_idx + 'A';
    p_vol->tail_entry.tracker_size = 1;
    p_vol->tail_entry.tracker_bits = p_vol->tail_entry.tracker_buff;

    err = VFiPFDRV_init(p_vol);
    if (err != 0) {
        return err;
    }

    VFiPFCACHE_SetCache(p_vol, p_drv->cache->pages, p_drv->cache->buffers, p_drv->cache->num_fat_pages, p_drv->cache->num_data_pages);
    VFiPFCACHE_SetFATBufferSize(p_vol, p_drv->cache->num_fat_buf_size);
    VFiPFCACHE_SetDataBufferSize(p_vol, p_drv->cache->num_data_buf_size);
    return 0;
}

static s32 VFiPFVOL_p_detach(struct PF_VOLUME* p_vol) {
    s32 err;
    err = VFiPFDRV_finalize(p_vol);
    if (err) {
        return err;
    }
    return 0;
}

static s32 VFiPFVOL_p_mount(struct PF_VOLUME* p_vol) {
    s32 err;

    if ((p_vol->flags & 2) == 0) {
        err = VFiPFVOL_DoMountVolume(p_vol);
        if (err != 0) {
            return err;
        }

        p_vol->fsi_flag &= ~7;
        VFipf_vol_set.num_mounted_volumes++;
    }
    return 0;
}

s32 VFiPFVOL_p_unmount(struct PF_VOLUME* p_vol, u32 mode) {
    s32 err;
    s32 err2;

    err = 0;

    if ((p_vol->flags & 2) == 0) {
        return 9;
    }

    VFiPFFILE_FinalizeAllFiles(p_vol);
    VFiPFDIR_FinalizeAllDirs(p_vol);
    err2 = VFiPFCACHE_FlushAllCaches(p_vol);
    if (err2 != 0) {
        err = err2;
    }

    if (err2 == 0 || (mode & 1) != 0) {
        VFiPFCACHE_FreeAllCaches(p_vol);
        err2 = VFiPFVOL_DoUnmountVolume(p_vol, mode);
        if (err2 != 0 && err == 0) {
            err = err2;
        }
    }

    if (err == 0 || (mode & 1) != 0) {
        VFipf_vol_set.num_mounted_volumes--;
    }

    return err;
}

static s32 VFiPFVOL_CheckMediaInsert(struct PF_VOLUME* p_vol) {
    s32 err;

    if (VFiPFDRV_IsInserted(p_vol) != 0) {
        if (VFiPFDRV_IsDetected(p_vol) != 0) {
            if ((p_vol->flags & 2) != 0) {
                VFiPFVOL_UnmountVolumeByEject(p_vol);
            }
            err = VFiPFVOL_p_mount(p_vol);
            if (err != 0) {
                return err;
            }
        }
    } else {
        if ((p_vol->flags & 2) != 0) {
            if (VFiPFDRV_IsDetected(p_vol) != 0) {
                VFiPFVOL_UnmountVolumeByEject(p_vol);
            }
            p_vol->flags &= ~2;
        }
    }
    return 0;
}

s32 VFiPFVOL_InitModule(u32 config, void* param) {
    s32 vol_idx;
    struct PF_CHARCODE codeset;  // Present in DWARF but unused here.
    u32 i;

    if ((config & 0xFFFCFFFF) != 0) {
        VFipf_vol_set.last_error = 10;
        return 10;
    }
    if ((config & 0x30000) == 0x30000) {
        VFipf_vol_set.last_error = 10;
        return 10;
    }

    if ((config & 0x10000) != 0) {
        VFipf_vol_set.config |= 0x10000;
    } else {
        VFipf_vol_set.config &= ~0x10000;
    }

    VFipf_vol_set.current_vol[0].p_vol = &VFipf_vol_set.volumes[0];
    VFipf_vol_set.current_vol[0].stat |= 1;
    VFipf_vol_set.num_attached_drives = 0;
    VFipf_vol_set.num_mounted_volumes = 0;

    if ((config & 0x10000) != 0) {
        VFipf_vol_set.config |= 0x10000;
    } else {
        VFipf_vol_set.config &= ~0x10000;
    }

    VFipf_vol_set.param = param;
    VFipf_vol_set.last_error = 0;
    VFipf_vol_set.last_driver_error = 0;
    VFipf_vol_set.setting = 1;

    VFipf_vol_set.codeset.oem2unicode = &VFiPFCODE_CP932_OEM2Unicode;
    VFipf_vol_set.codeset.unicode2oem = &VFiPFCODE_CP932_Unicode2OEM;
    VFipf_vol_set.codeset.oem_char_width = &VFiPFCODE_CP932_OEMCharWidth;
    VFipf_vol_set.codeset.is_oem_mb_char = &VFiPFCODE_CP932_isOEMMBchar;
    VFipf_vol_set.codeset.unicode_char_width = &VFiPFCODE_CP932_UnicodeCharWidth;
    VFipf_vol_set.codeset.is_unicode_mb_char = &VFiPFCODE_CP932_isUnicodeMBchar;

    for (vol_idx = 0; vol_idx < 26; vol_idx++) {
        VFipf_memset(&VFipf_vol_set.volumes[vol_idx], 0, sizeof(struct PF_VOLUME));
    }

    VFiPF_InitLockFile();

    return 0;
}

s32 VFiPFVOL_CheckForRead(struct PF_VOLUME* p_vol) {
    s32 err;

    if (p_vol == NULL) {
        return 10;
    }

    err = VFiPFVOL_CheckMediaInsert(p_vol);
    if (err != 0) {
        return err;
    }

    if (!(p_vol->flags & 2)) {
        return 9;
    }

    return 0;
}

s32 VFiPFVOL_CheckForWrite(struct PF_VOLUME* p_vol) {
    s32 err;

    if (p_vol == NULL) {
        return 10;
    }

    err = VFiPFVOL_CheckMediaInsert(p_vol);
    if (err != 0) {
        return err;
    }

    if ((p_vol->flags & 2) == 0) {
        return 9;
    }

    if (VFiPFDRV_IsWProtected(p_vol)) {
        return 11;
    }

    return 0;
}

s32 VFiPFVOL_GetCurrentDir(struct PF_VOLUME* p_vol, struct PF_DIR_ENT* p_current_dir) {
    s32 err;
    s32 context_id;
    u32 i;

    if (p_vol == NULL) {
        err = 10;
        return err;
    }
    if ((p_vol->flags & 2) == 0) {
        err = 9;
        return err;
    }

    VFiPFSYS_GetCurrentContextID(&context_id);

    for (i = 1; i < 1; i++) {
        if ((p_vol->current_dir[i].stat & 1) != 0 && p_vol->current_dir[i].context_id == context_id) {
            *p_current_dir = p_vol->current_dir[i].directory;
            break;
        }
    }

    if (i == 1) {
        if (VFiPFVOL_CheckContextRegistered(context_id)) {
            for (i = 1; i < 1; i++) {
                if ((p_vol->current_dir[i].stat & 1) == 0) {
                    p_vol->current_dir[i].stat |= 1;
                    p_vol->current_dir[i].context_id = context_id;
                    *p_current_dir = p_vol->current_dir[i].directory;
                    break;
                }
            }
        } else {
            *p_current_dir = p_vol->current_dir[0].directory;
        }
    }

    return 0;
}

void VFiPFVOL_SetCurrentVolume(struct PF_VOLUME* p_vol) {
    s32 context_id;
    u32 i;

    VFiPFSYS_GetCurrentContextID(&context_id);
    for (i = 1; i < 1; i++) {
        if ((VFipf_vol_set.current_vol[i].stat & 1) != 0 && VFipf_vol_set.current_vol[i].context_id == context_id) {
            VFipf_vol_set.current_vol[i].p_vol = p_vol;
            break;
        }
    }

    if (i == 1) {
        if (VFiPFVOL_CheckContextRegistered(context_id)) {
            for (i = 1; i < 1; i++) {
                if ((VFipf_vol_set.current_vol[i].stat & 1) == 0) {
                    VFipf_vol_set.current_vol[i].stat |= 1;
                    VFipf_vol_set.current_vol[i].context_id = context_id;
                    VFipf_vol_set.current_vol[i].p_vol = p_vol;
                    break;
                }
            }
        }
    }
    VFipf_vol_set.current_vol[0].p_vol = p_vol;
}

struct PF_VOLUME* VFiPFVOL_GetCurrentVolume() {
    struct PF_VOLUME* p_vol;
    s32 context_id;
    u32 i;

    p_vol = NULL;
    VFiPFSYS_GetCurrentContextID(&context_id);

    for (i = 1; i < 1; i++) {
        if ((VFipf_vol_set.current_vol[i].stat & 1) != 0 && VFipf_vol_set.current_vol[i].context_id == context_id) {
            p_vol = VFipf_vol_set.current_vol[i].p_vol;
            break;
        }
    }

    if (i == 1) {
        if (VFiPFVOL_CheckContextRegistered(context_id)) {
            for (i = 1; i < 1; i++) {
                if ((VFipf_vol_set.current_vol[i].stat & 1) == 0) {
                    VFipf_vol_set.current_vol[i].stat |= 1;
                    VFipf_vol_set.current_vol[i].context_id = context_id;
                    p_vol = VFipf_vol_set.current_vol[i].p_vol;
                    break;
                }
            }
        } else {
            p_vol = VFipf_vol_set.current_vol[0].p_vol;
        }
    }
    return p_vol;
}

struct PF_VOLUME* VFiPFVOL_GetVolumeFromDrvChar(s8 drv_char) {
    struct PF_VOLUME* p_vol;  // Present in DWARF but unused here.
    s16 vol_idx;

    vol_idx = VFipf_toupper(drv_char) - 'A';
    if (vol_idx < 0 || vol_idx >= 26) {
        return NULL;
    }

    return &VFipf_vol_set.volumes[vol_idx];
}

#ifdef GAME_REXE01
void VFiPFVOL_LoadVolumeLabelFromBuf(struct PF_VOLUME* p_vol, const u8* buf) {
#else
void VFiPFVOL_LoadVolumeLabelFromBuf(const u8* buf, struct PF_VOLUME* p_vol) {
#endif
    VFipf_memcpy(p_vol->label, (void*)buf, 11);
    p_vol->label[11] = '\0';
}

s32 VFiPFVOL_errnum() {
    return VFipf_vol_set.last_error;
}

s32 VFiPFVOL_getdev(s8 drv_char, struct PF_DEV_INF* dev_inf) {
    struct PF_VOLUME* p_vol;
    s32 err;

    p_vol = VFiPFVOL_GetVolumeFromDrvChar(drv_char);
    if (p_vol == NULL) {
        VFipf_vol_set.last_error = 10;
        return 10;
    }

    if (dev_inf == NULL) {
        VFipf_vol_set.last_error = 10;
        p_vol->last_error = 10;
        return 10;
    }

    err = VFiPFVOL_CheckForRead(p_vol);
    if (err != 0) {
        VFipf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }

    dev_inf->bps = p_vol->bpb.bytes_per_sector;
    dev_inf->spc = p_vol->bpb.sectors_per_cluster;
    dev_inf->cls = p_vol->bpb.num_clusters;

    err = VFiPFFAT_CountFreeClusters(p_vol, &dev_inf->ecl);
    if (err != 0) {
        VFipf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }

    return 0;
}

s32 VFiPFVOL_attach(struct PF_DRV_TBL* p_drv) {
    s32 vol_idx;
    struct PF_VOLUME* p_vol;
    s32 err;

    if (p_drv == NULL || p_drv->cache->num_fat_pages < 1 || p_drv->cache->num_data_pages < 2 || p_drv->cache->pages == NULL || p_drv->cache->buffers == NULL || ((u32)p_drv->cache->pages & 3) != 0 || ((u32)p_drv->cache->buffers & 3) != 0) {
        VFipf_vol_set.last_error = 10;
        return 10;
    }

    if (p_drv->cache->num_fat_buf_size == 0) {
        p_drv->cache->num_fat_buf_size = 1;
    }
    if (p_drv->cache->num_data_buf_size == 0) {
        p_drv->cache->num_data_buf_size = 1;
    }

    if (p_drv->cache->num_fat_pages / p_drv->cache->num_fat_buf_size < 1) {
        VFipf_vol_set.last_error = 10;
        return 10;
    }

    if (p_drv->cache->num_data_pages / p_drv->cache->num_data_buf_size < 2) {
        VFipf_vol_set.last_error = 10;
        return 10;
    }

    p_drv->stat = 0;
    p_drv->drive = 0;

    for (vol_idx = 0; vol_idx < 26; vol_idx++) {
        p_vol = &VFipf_vol_set.volumes[vol_idx];
        if ((p_vol->flags & 1) == 0) {
            break;
        }
    }

    if (vol_idx < 0 || vol_idx >= 26 || VFipf_vol_set.num_attached_drives < 0 || VFipf_vol_set.num_attached_drives >= 26) {
        VFipf_vol_set.last_error = 4;
        return 4;
    }

    p_vol->num_free_clusters = -1;
    p_vol->last_free_cluster = -1;

    err = VFiPFVOL_p_attach(p_vol, p_drv, vol_idx);
    if (err != 0) {
        VFipf_vol_set.last_error = err;
        return err;
    }

    p_vol->flags |= 1;
    p_drv->stat |= 1;
    p_vol->drv_char = vol_idx + 'A';
    p_drv->drive = vol_idx + 'A';

    VFipf_vol_set.num_attached_drives++;

    err = VFiPFVOL_CheckMediaInsert(p_vol);
    if (err != 0) {
        VFipf_vol_set.last_error = err;
        p_vol->last_error = err;
        return 0;
    }

    err = VFiPFDRV_IsInserted(p_vol);
    if (err != 0) {
        err = VFiPFVOL_p_mount(p_vol);
        if (err != 0) {
            VFipf_vol_set.last_error = err;
            p_vol->last_error = err;
            return 0;
        }
        p_drv->stat |= 2;
    }

    return 0;
}

s32 VFiPFVOL_detach(s8 drv_char) {
    struct PF_VOLUME* p_vol;
    s32 err;

    p_vol = VFiPFVOL_GetVolumeFromDrvChar(drv_char);
    if (p_vol == NULL) {
        VFipf_vol_set.last_error = 10;
        return 10;
    }

    if ((p_vol->flags & 2) != 0) {
        VFipf_vol_set.last_error = 10;
        p_vol->last_error = 10;
        return 10;
    }

    if ((p_vol->flags & 1) == 0) {
        VFipf_vol_set.last_error = 10;
        p_vol->last_error = 10;
        return 10;
    }

    err = VFiPFVOL_p_detach(p_vol);
    if (err) {
        VFipf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }

    p_vol->flags &= ~1;
    VFipf_vol_set.num_attached_drives--;

    return 0;
}

s32 VFiPFVOL_unmount(s8 drv_char, u32 mode) {
    struct PF_VOLUME* p_vol;
    s32 err;
    s32 err2;

    err2 = 0;
    p_vol = VFiPFVOL_GetVolumeFromDrvChar(drv_char);
    if (p_vol == NULL) {
        VFipf_vol_set.last_error = 10;
        return 10;
    } else if ((p_vol->flags & 1) == 0) {
        VFipf_vol_set.last_error = 10;
        return 10;
    } else if (mode != 0 && (mode & 1) == 0) {
        VFipf_vol_set.last_error = 10;
        p_vol->last_error = 10;
        return 10;
    } else {
        if (((p_vol->flags >> 1) & 1) & VFiPFDRV_IsDetected(p_vol)) {
            VFiPFVOL_UnmountVolumeByEject(p_vol);
        }

        if ((p_vol->flags & 2) == 0) {
            return 0;
        } else {
            if (p_vol->num_opened_files != 0 || p_vol->num_opened_directories != 0) {
                err2 = 1;
                VFipf_vol_set.last_error = 19;
                p_vol->last_error = 19;
                if ((mode & 1) == 0) {
                    return 19;
                }
            }

            err = VFiPFVOL_p_unmount(p_vol, mode);
            if (err != 0 && err2 == 0) {
                VFipf_vol_set.last_error = err;
                p_vol->last_error = err;
                err2 = 1;
                if ((mode & 1) == 0) {
                    err2 = err;
                }
            }
        }
    }

    return err2;
}
#endif
