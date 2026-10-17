#ifdef GAME_REXE01
// Excite Truck's SD drive (0x800B9BB4-0x800BAA8C): a PDM driver over the ISD library (IOS
// /dev/sdio). The pfd_* names come from its messages; the ISD_* names are guesses beside the
// names its own messages give.
#include <revolution/vf.h>
#include <revolution/os.h>

typedef s32 (*ISD_INTR_HANDLER)(s32 event);

s32 ISD_Init(void);
s32 ISD_DetectCard(s32 slot);
s32 ISD_Open(s32 slot, s32* p_handle);
s32 ISD_Close(s32 handle);
s32 ISD_RegisterDeviceIntrHandler(s32 handle, ISD_INTR_HANDLER handler, u32* p_event);
s32 ISD_ResetDevice(s32 handle);
s32 ISD_ReadCardRegister(s32 handle, u32 reg, void* buf, u32 size);
s32 ISD_GetDeviceStatus(s32 handle, u32* p_status);
s32 ISD_ReadBlock(s32 handle, u32 offset, void* buf, u32 size);
s32 ISD_WriteBlock(s32 handle, u32 offset, const void* buf, u32 size);
s32 ISD_ReadMultiBlock(s32 handle, u32 offset, void* buf, u32 size);
s32 ISD_WriteMultiBlock(s32 handle, u32 offset, const void* buf, u32 size);

typedef struct SDDRV_STATE {
    // total size: 0x18
    u32 flags;                // offset 0x0 (1 initialized, 2 mounted, 4 opened)
    u32 bps;                  // offset 0x4
    s32 handle;               // offset 0x8
    struct PDM_DISK* p_disk;  // offset 0xC
    s32 inserted;             // offset 0x10
    s8 drive;                 // offset 0x14, drive letter of the card's volume
} SDDRV_STATE;

static volatile SDDRV_STATE sddrv;
static u8 sddrv_work[0x28] ATTRIBUTE_ALIGN(32);
static u8 sddrv_buf[0x200] ATTRIBUTE_ALIGN(32);

static u32 sddrv_event ATTRIBUTE_ALIGN(32);
static VF_ET_MEDIA_CALLBACK_S sddrv_eject_callback;
static VF_ET_MEDIA_CALLBACK_S sddrv_insert_callback;

extern const struct PDM_FUNCTBL l_sd_func;

s32 pfd_st_removal_callback(s32 event);
static s32 pfd_sddrv_physical_read(u32 num_blocks, u8* buf, u32 block, u32 bps, u32* p_num_success);
static s32 pfd_sddrv_physical_write(u32 num_blocks, const u8* buf, u32 block, u32 bps, u32* p_num_success);
s32 pfd_sddrv_get_total_sectors(u32* p_total_sectors, u16* p_bps);
static u8 pfd_sddrv_get_drive_chars(struct PDM_DISK* p_disk, s8* p_drive, u8 num);

static inline void pfd_sddrv_update_drive(void) {
    s8 drive;

    if (sddrv.drive == 0) {
        if (pfd_sddrv_get_drive_chars(sddrv.p_disk, &drive, 1) == 1) {
            sddrv.drive = drive;
        }
    }
}

s32 pfd_st_inter_callback(s32 event) {
    if ((event & 1) != 1) {
        return 0;
    }

    if (sddrv.handle != 0) {
        sddrv_event = 2;
        if (ISD_RegisterDeviceIntrHandler(sddrv.handle, pfd_st_removal_callback, &sddrv_event) != 0) {
            OSReport("ERR:Failed to regist intr handler. pfd_st_inter_callback()\n");
        }
    }

    if (sddrv.p_disk != NULL) {
        pfd_sddrv_update_drive();
        VFipdm_disk_notify_media_insert(sddrv.p_disk);
        if (sddrv.drive != 0 && sddrv_insert_callback != NULL) {
            sddrv_insert_callback((s8)sddrv.drive);
        }
    }

    sddrv.inserted = 1;
    return 0;
}

s32 pfd_st_removal_callback(s32 event) {
    if ((event & 2) != 2) {
        return 0;
    }

    sddrv.inserted = 0;
    if (sddrv.handle != 0) {
        sddrv_event = 1;
        if (ISD_RegisterDeviceIntrHandler(sddrv.handle, pfd_st_inter_callback, &sddrv_event) != 0) {
            OSReport("ERR:Failed to regist intr handler. pfd_st_removal_callback()\n");
        }
    }

    if (sddrv.p_disk != NULL) {
        pfd_sddrv_update_drive();
        VFipdm_disk_notify_media_eject(sddrv.p_disk);
        if (sddrv.drive != 0 && sddrv_eject_callback != NULL) {
            sddrv_eject_callback((s8)sddrv.drive);
        }
    }

    return 0;
}

s32 pfd_sddrv_init(struct PDM_DISK* p_disk) {
    s32 handle;
    s32 err;

    if (p_disk == (struct PDM_DISK*)NULL) {
        return 1;
    }

    if (sddrv.flags & 1) {
        OSReport("INFO SD Card driver is already initialize. pfd_sddrv_init()\n");
        if (p_disk != sddrv.p_disk) {
            return 0xB;
        }
        return 0;
    }

    if (!(sddrv.flags & 4)) {
        sddrv.bps = 0x200;
        VFipf_memset(sddrv_work, 0, sizeof(sddrv_work));
        if (ISD_Init() != 0) {
            OSReport("ERR:Failed to init SD Card Driver in pfd_sddrv_init()\n");
            return 0x15;
        }

        if (ISD_DetectCard(0) == 0) {
            sddrv.inserted = 1;
        }

        handle = (s32)sddrv_work;
        err = ISD_Open(0, &handle);
        if (err != 0) {
            OSReport("ERR SD card can not mount [ret = 0x%x]. pfd_sddrv_init()\n", err);
            return 0x15;
        }

        if (sddrv.inserted != 0) {
            sddrv_event = 2;
            err = ISD_RegisterDeviceIntrHandler(handle, pfd_st_removal_callback, &sddrv_event);
            if (err != 0) {
                OSReport("ERR:Failed to regist intr handler1 [ret = 0x%x] pfd_sddrv_init()\n", err);
                ISD_Close(handle);
                return 0x15;
            }
        } else {
            sddrv_event = 1;
            err = ISD_RegisterDeviceIntrHandler(handle, pfd_st_inter_callback, &sddrv_event);
            if (err != 0) {
                OSReport("ERR:Failed to regist intr handler1 [ret = 0x%x] pfd_sddrv_init()\n", err);
                ISD_Close(handle);
                return 0x15;
            }
        }

        sddrv.handle = handle;
        sddrv.flags |= 4;
    }

    sddrv.p_disk = p_disk;
    sddrv.flags |= 1;
    return 0;
}

s32 pfd_sddrv_mount(struct PDM_DISK* p_disk) {
    u8 cid[0x10];
    u32 ocr;
    u16 rca;
    u32 i;
    s32 err;

    if (p_disk == NULL) {
        return 1;
    }
    if (sddrv.inserted == 0) {
        return 6;
    }
    if (sddrv.flags & 2) {
        OSReport("INFO sdcard alrady mounted. pfd_sddrv_mount()\n");
        return 0;
    }

    for (i = 0; i < 5; i++) {
        err = ISD_ResetDevice(sddrv.handle);
        if (err == 0) {
            break;
        }
        OSReport("ERROR Failed to SD Card Reset [ret = 0x%x]. pfd_sddrv_mount()\n", err);
    }
    if (i == 5) {
        return 0x15;
    }

    ocr = 0;
    if (ISD_ReadCardRegister(sddrv.handle, 0x29, &ocr, sizeof(ocr)) != 0) {
        OSReport("ERR Failed to read OCR reg. pfd_sddrv_mount()\n");
        return 0x15;
    }

    VFipf_memset(cid, 0, sizeof(cid));
    if (ISD_ReadCardRegister(sddrv.handle, 0xA, cid, sizeof(cid)) != 0) {
        OSReport("ERR Failed to read CID reg. pfd_sddrv_mount()\n");
        return 0x15;
    }

    rca = 0;
    if (ISD_ReadCardRegister(sddrv.handle, 3, &rca, sizeof(rca)) != 0) {
        OSReport("ERR Failed to read RCA reg. pfd_sddrv_mount()\n");
        return 0x15;
    }

    sddrv.flags |= 2;
    return 0;
}

s32 pfd_sddrv_format(struct PDM_DISK* p_disk, const u8* param) {
    if (p_disk == NULL) {
        return 1;
    }
    return sddrv.inserted != 0 ? 0 : 6;
}

s32 pfd_sddrv_pread(struct PDM_DISK* p_disk, u8* p_buf, u32 block, u32 num_blocks, u32* p_num_success) {
    if (p_disk == NULL || p_buf == NULL || p_num_success == NULL) {
        return 1;
    }
    return pfd_sddrv_physical_read(num_blocks, p_buf, block, sddrv.bps, p_num_success);
}

s32 pfd_sddrv_pwrite(struct PDM_DISK* p_disk, const u8* p_buf, u32 block, u32 num_blocks, u32* p_num_success) {
    if (p_disk == NULL || p_buf == NULL || p_num_success == NULL) {
        return 1;
    }
    return pfd_sddrv_physical_write(num_blocks, p_buf, block, sddrv.bps, p_num_success);
}

s32 pfd_sddrv_unmount(struct PDM_DISK* p_disk) {
    if (p_disk == NULL) {
        return 1;
    }
    if (sddrv.flags & 2) {
        sddrv.flags &= ~2;
    }
    return 0;
}

s32 pfd_sddrv_finalize(struct PDM_DISK* p_disk) {
    if (p_disk == NULL) {
        return 1;
    }
    if (sddrv.flags & 2) {
        sddrv.flags &= ~2;
    }
    if (sddrv.flags & 1) {
    }
    sddrv.flags &= ~1;
    sddrv.p_disk = NULL;
    sddrv.drive = 0;
    return 0;
}

#pragma push
#pragma dont_inline on
s32 pfd_sddrv_get_disk_info(struct PDM_DISK* p_disk, struct PDM_DISK_INFO* p_disk_info) {
    u32 status;
    s32 err;

    if (p_disk == NULL || p_disk_info == NULL) {
        return 1;
    }
    if (!(sddrv.flags & 2)) {
        return 0xF;
    }

    err = ISD_GetDeviceStatus(sddrv.handle, &status);
    if (err != 0) {
        OSReport("ERR Failed to get sd card status. [ret = 0x%x]\n", err);
        return 0x15;
    }
    if (!(status & 1)) {
        OSReport("INFO Card not inserted! in pfd_sddrv_get_disk_info()\n");
        return 6;
    }
    if (status & 2) {
        OSReport("INFO Card removed! in pfd_sddrv_get_disk_info()\n");
        return 6;
    }
    if (!(status & 0x10000)) {
        OSReport("INFO SD card type is not memory. in pfd_sddrv_get_disk_info()\n");
        return 0x15;
    }

    p_disk_info->media_attr = 4;
    if (status & 4) {
        p_disk_info->media_attr |= 1;
    }
    if (pfd_sddrv_get_total_sectors(&p_disk_info->total_sectors, &p_disk_info->bytes_per_sector) != 0) {
        OSReport("ERR Failed to read master boot sector. pfd_sddrv_get_disk_info()\n");
        return 0x15;
    }
    p_disk_info->cylinders = 0;
    p_disk_info->heads = 0;
    p_disk_info->sectors_per_track = 0;
    p_disk_info->format_param = NULL;
    return 0;
}
#pragma pop

s32 VFi_sddrv_init_drv_tbl(struct PDM_DISK_TBL* p_disk_tbl, u32 ui_ext) {
    p_disk_tbl->ui_ext = ui_ext;
    p_disk_tbl->p_func = (struct PDM_FUNCTBL*)&l_sd_func;
    return 0;
}

s32 VFiSD_SetCallback(VF_ET_MEDIA_CALLBACK_S insert, VF_ET_MEDIA_CALLBACK_S eject) {
    sddrv_insert_callback = insert;
    sddrv_eject_callback = eject;
    return 0;
}

s32 VFiSD_IsInserted(void) {
    if (sddrv.flags & 1) {
        pfd_sddrv_update_drive();
        if (sddrv.inserted != 0) {
            VFipdm_disk_notify_media_insert(sddrv.p_disk);
            return 1;
        }
        return 0;
    }
    return 0;
}

static s32 pfd_sddrv_physical_read(u32 num_blocks, u8* buf, u32 block, u32 bps, u32* p_num_success) {
    u32 size;
    u32 offset;
    u32 i;
    s32 err;

    if (!(sddrv.flags & 2)) {
        *p_num_success = 0;
        return 0xF;
    }

    size = num_blocks * bps;
    if (size == 0) {
        *p_num_success = 0;
        return 1;
    }

    offset = block * bps;
    *p_num_success = (u32)-1;
    if (((u32)buf & 0x1F) == 0) {
        if (sddrv.inserted == 0) {
            *p_num_success = 0;
            return 6;
        }
        if (num_blocks == 1) {
            err = ISD_ReadBlock(sddrv.handle, offset, buf, size);
            if (err != 0) {
                OSReport("INFO Failed to read SD card1 [ret = 0x%x] pfd_sddrv_physical_read()\n", err);
                *p_num_success = 0;
                return 0x15;
            }
        } else {
            err = ISD_ReadMultiBlock(sddrv.handle, offset, buf, size);
            if (err != 0) {
                OSReport("INFO Failed to read SD card2 [ret = 0x%x] pfd_sddrv_physical_read()\n", err);
                *p_num_success = 0;
                return 0x15;
            }
        }
    } else {
        for (i = 0; i < num_blocks; i++) {
            if (sddrv.inserted == 0) {
                if (i != 0) {
                    *p_num_success = i;
                    return 0;
                }
                *p_num_success = 0;
                return 6;
            }
            err = ISD_ReadBlock(sddrv.handle, offset, sddrv_buf, bps);
            if (err != 0) {
                OSReport("INFO Failed to read SD card3 [ret = 0x%x] pfd_sddrv_physical_read()\n", err);
                if (i != 0) {
                    *p_num_success = i;
                    return 0;
                }
                *p_num_success = 0;
                return 0x15;
            }
            VFipf_memcpy(buf, sddrv_buf, bps);
            buf += bps;
            offset += bps;
        }
    }

    *p_num_success = num_blocks;
    return 0;
}

static s32 pfd_sddrv_physical_write(u32 num_blocks, const u8* buf, u32 block, u32 bps, u32* p_num_success) {
    u32 size;
    u32 offset;
    u32 i;
    s32 err;

    if (!(sddrv.flags & 2)) {
        *p_num_success = 0;
        return 0xF;
    }

    size = num_blocks * bps;
    if (size == 0) {
        *p_num_success = 0;
        return 1;
    }

    offset = block * bps;
    *p_num_success = (u32)-1;
    if (((u32)buf & 0x1F) == 0) {
        if (sddrv.inserted == 0) {
            *p_num_success = 0;
            return 6;
        }
        if (num_blocks == 1) {
            err = ISD_WriteBlock(sddrv.handle, offset, buf, size);
            if (err != 0) {
                OSReport("INFO Failed to write SD card1 [ret = 0x%x] pfd_sddrv_physical_write()\n", err);
                *p_num_success = 0;
                return 0x15;
            }
        } else {
            err = ISD_WriteMultiBlock(sddrv.handle, offset, buf, size);
            if (err != 0) {
                OSReport("INFO Failed to write SD card2 [ret = 0x%x] pfd_sddrv_physical_write()\n", err);
                *p_num_success = 0;
                return 0x15;
            }
        }
    } else {
        for (i = 0; i < num_blocks; i++) {
            if (sddrv.inserted == 0) {
                if (i != 0) {
                    *p_num_success = i;
                    return 0;
                }
                *p_num_success = 0;
                return 6;
            }
            VFipf_memcpy(sddrv_buf, (void*)buf, bps);
            err = ISD_WriteBlock(sddrv.handle, offset, sddrv_buf, bps);
            if (err != 0) {
                OSReport("INFO Failed to write SD card3 [ret = 0x%x] pfd_sddrv_physical_write()\n", err);
                if (i != 0) {
                    *p_num_success = i;
                    return 0;
                }
                *p_num_success = 0;
                return 0x15;
            }
            buf += bps;
            offset += bps;
        }
    }

    *p_num_success = num_blocks;
    return 0;
}

#define SDDRV_MAX(a, b) ((a) < (b) ? (b) : (a))
#define SDDRV_MIN(a, b) ((a) > (b) ? (b) : (a))

// Card capacity from the CSD register (v1.0 layout as IOS returns it).
s32 pfd_sddrv_get_total_sectors(u32* p_total_sectors, u16* p_bps) {
    u32 csd[4];
    u16 mult;
    u32 num_blocks;
    u16 read_bl_len;
    u32 c_size;

    VFipf_memset(csd, 0, sizeof(csd));
    if (ISD_ReadCardRegister(sddrv.handle, 9, csd, sizeof(csd)) != 0) {
        OSReport("ERR Failed to get CSD Info. pfd_sddrv_get_total_sectors()\n");
        return 0x15;
    }

    c_size = ((csd[2] & 3) << 10) | (csd[1] >> 22);
    mult = 1 << (((csd[1] >> 7) & 7) + 2);
    num_blocks = (c_size + 1) * mult;
    read_bl_len = SDDRV_MAX((csd[2] >> 8) & 0xF, 9);
    read_bl_len = SDDRV_MIN(read_bl_len, 11);
    read_bl_len = read_bl_len - 9;
    *p_total_sectors = num_blocks * (u16)(1 << read_bl_len);
    *p_bps = 0x200;
    return 0;
}

// Drive letters of the volumes mounted on p_disk's partitions; returns how many were stored.
static u8 pfd_sddrv_get_drive_chars(struct PDM_DISK* p_disk, s8* p_drive, u8 num) {
    struct PF_VOLUME_SET* p_vol_set;
    u32 i;
    u8 count;
    s8* p_dst;
    struct PDM_PARTITION* p_part;
    u16 j;
    struct PF_VOLUME* p_vol;

    if (p_drive == NULL || num == 0) {
        return 0;
    }
    p_dst = p_drive;
    p_part = VFipdm_disk_set.partition;
    p_vol_set = &VFipf_vol_set;
    count = 0;
    for (i = 0; i < 26; i++, p_part++) {
        if (VFipdm_part_is_open(p_part) && p_part->p_disk == p_disk) {
            p_vol = p_vol_set->volumes;
            for (j = 0; j < 26; j++, p_vol++) {
                if (p_vol->p_part == p_part) {
                    count++;
                    *p_dst++ = p_vol->drv_char;
                    if (count == num) {
                        break;
                    }
                }
            }
        }
        if (count == num) {
            break;
        }
    }
    return count;
}

const struct PDM_FUNCTBL l_sd_func = {
    pfd_sddrv_init,
    pfd_sddrv_finalize,
    pfd_sddrv_mount,
    pfd_sddrv_unmount,
    (long (*)(struct PDM_DISK*, unsigned char*))pfd_sddrv_format,
    pfd_sddrv_pread,
    (long (*)(struct PDM_DISK*, unsigned char*, unsigned long, unsigned long, unsigned long*))pfd_sddrv_pwrite,
    pfd_sddrv_get_disk_info,
};
#else
#include <revolution/vf/sd_drv.h>

void VFi_InitSDWrok(void) {
    return;
}
#endif
