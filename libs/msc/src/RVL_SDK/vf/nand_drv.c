#ifdef GAME_REXE01
// Excite Truck's NAND drive: a FAT image stored in a NAND file (0x800B808C-0x800B9BB4). ET's
// version predates the later VF_HANDLE design; images are registered by path and size, then
// bound to a PDM disk on mount. Names follow msc's nand_drv.h where it has them.
#include <revolution/vf.h>
#include <revolution/nand.h>

// Little-endian stores into a boot sector buffer: bytewise when the field is unaligned,
// otherwise one store at the (off + 1) & ~1 position (ET's aligned view of the sector).
#define NANDDRV_ST16(buf, off, v)                                            \
    if (((u32)((buf) + (off)) & 1) != 0) {                                   \
        (buf)[(off)] = (u8)(v);                                              \
        (buf)[(off) + 1] = (u8)((v) >> 8);                                   \
    } else {                                                                 \
        *(u16*)((buf) + (((off) + 1) & ~1)) = (u16)(SWAP16((u16)(v)));       \
    }
#define NANDDRV_ST32(buf, off, aoff, v)                                      \
    if (((u32)((buf) + (off)) & 3) != 0) {                                   \
        (buf)[(off)] = (u8)(v);                                              \
        (buf)[(off) + 1] = (u8)((v) >> 8);                                   \
        (buf)[(off) + 2] = (u8)((v) >> 16);                                  \
        (buf)[(off) + 3] = (u8)((v) >> 24);                                  \
    } else {                                                                 \
        *(u32*)((buf) + (aoff)) = (u32)(SWAP32((u32)(v)));                   \
    }

#define NANDDRV_NUM_DEV 23

typedef struct NANDDRV_DEV {
    // total size: 0xF0
    struct PDM_DISK* p_disk;   // offset 0x0, disk the image is mounted on
    s8 path[0x50];             // offset 0x4
    u32 size;                  // offset 0x54, image size in bytes
    NANDFileInfo info;         // offset 0x58
    u32 flags;                 // offset 0xE4 (2 mounted, 4 formatting)
    u32 reserved;              // offset 0xE8
    s32 last_error;            // offset 0xEC
} NANDDRV_DEV;

extern const struct PDM_FUNCTBL l_nand_func;

static s8 nanddrv_home_dir[0x40];
static NANDDRV_DEV nanddrv_dev[NANDDRV_NUM_DEV];
static struct PF_ET_SEMAPHORE_PARAM nanddrv_sem_param;
static u8 nanddrv_sem_buf[0xC];

static s32 nanddrv_sem;
static s32 nanddrv_init_count;
static s32 nanddrv_register_count;

static s32 nanddrv_create_image(NANDDRV_DEV* p_dev);
static s32 nanddrv_create_path(NANDDRV_DEV* p_dev);
static s32 nanddrv_get_fat_type(u8* buf, u32* p_num_data_sectors);
static s32 nanddrv_BuildUpFSInfoSector(u8* buf);
static s32 nanddrv_BuildUpBootSector(NANDDRV_DEV* p_dev, u8* buf, s32* p_type, u32 num_sectors);
static s32 nanddrv_physical_read(u32 num_blocks, u8* buf, u32 block, u32 bps, u32* p_num_success, struct PDM_DISK* p_disk);
static s32 nanddrv_physical_write(u32 num_blocks, const u8* buf, u32 block, u32 bps, u32* p_num_success, struct PDM_DISK* p_disk);

static s32 nanddrv_open_image(struct PDM_DISK* p_disk, NANDDRV_DEV* p_dev, s32 create) {
    s32 err;

    err = NANDChangeDir((char*)nanddrv_home_dir);
    if (err != 0) {
        return err;
    }

    if ((p_dev->flags & 4) != 0) {
        err = 0;
    } else {
        err = NANDOpen((char*)p_dev->path, &p_dev->info, 3);
    }
    if (create != 0) {
        err = -12;
    }

    switch (err) {
        case -12:
            err = nanddrv_create_image(p_dev);
            if (err == 0) {
                p_dev->p_disk = p_disk;
            }
            break;
        case 0:
            p_dev->p_disk = p_disk;
            return 0;
    }
    return err;
}

static s32 nanddrv_create_image(NANDDRV_DEV* p_dev) {
    u8 zero_buf[0x200] ATTRIBUTE_ALIGN(32);
    u8 boot_buf[0x200] ATTRIBUTE_ALIGN(32);
    u8 fsinfo_buf[0x200] ATTRIBUTE_ALIGN(32);
    s32 type;
    u32 num_sectors;
    s32 err;
    u32 rest;

    err = nanddrv_create_path(p_dev);
    if (err != 0) {
        return err;
    }

    VFipf_memset(boot_buf, 0, 0x200);
    num_sectors = 0;
    nanddrv_BuildUpBootSector(p_dev, boot_buf, &type, 0);
    if (type != nanddrv_get_fat_type(boot_buf, &num_sectors)) {
        nanddrv_BuildUpBootSector(p_dev, boot_buf, &type, num_sectors);
    }

    err = NANDWrite(&p_dev->info, boot_buf, 0x200);
    if (err < 0) {
        NANDClose(&p_dev->info);
        NANDDelete((char*)p_dev->path);
        return err;
    }

    rest = p_dev->size - 0x200;
    if (type == FAT_32) {
        nanddrv_BuildUpFSInfoSector(fsinfo_buf);
        err = NANDWrite(&p_dev->info, fsinfo_buf, 0x200);
        if (err < 0) {
            NANDClose(&p_dev->info);
            NANDDelete((char*)p_dev->path);
            return err;
        }
        rest -= 0x200;
    }

    VFipf_memset(zero_buf, 0, 0x200);
    while (rest >= 0x200) {
        err = NANDWrite(&p_dev->info, zero_buf, 0x200);
        if (err < 0) {
            NANDClose(&p_dev->info);
            NANDDelete((char*)p_dev->path);
            return err;
        }
        rest -= 0x200;
    }
    if (rest != 0) {
        err = NANDWrite(&p_dev->info, zero_buf, rest);
        if (err < 0) {
            NANDClose(&p_dev->info);
            NANDDelete((char*)p_dev->path);
            return err;
        }
    }
    return 0;
}

// Creates the directories of p_dev->path below the NAND root and the image file itself.
static s32 nanddrv_create_path(NANDDRV_DEV* p_dev) {
    s8 name[16];
    u16 len;
    s8* p_src;
    u16 pos;
    s8* p_dst;
    s32 err;

    len = 0;
    pos = 0;
    if ((p_dev->flags & 4) != 0) {
        return 0;
    }

    p_src = p_dev->path;
    VFipf_memset(name, 0, sizeof(name));
    p_dst = name;
    while (1) {
        switch (*p_src) {
            case '/':
                if (len == 0) {
                    if (pos == 0) {
                        err = NANDChangeDir("/");
                        if (err != 0) {
                            return err;
                        }
                    }
                    p_src++;
                } else if (p_src[1] == '\0') {
                    if (len > 12) {
                        return -20;
                    }
                    err = NANDCreate((char*)name, 0x30, 0);
                    if (err != 0 && err != -6) {
                        goto exit;
                    }
                    NANDOpen((char*)name, &p_dev->info, 3);
                    return NANDChangeDir((char*)nanddrv_home_dir);
                } else {
                    if (len > 11) {
                        return -20;
                    }
                    if (VFipf_strcmp(name, (const s8*)"..") != 0) {
                        err = NANDCreateDir((char*)name, 0x30, 0);
                    } else {
                        err = 0;
                    }
                    if (err != 0 && err != -6 && err != -1) {
                        goto exit;
                    }
                    err = NANDChangeDir((char*)name);
                    if (err != 0) {
                        return err;
                    // (ET's error exit sits here; the jumps to it are threaded away, leaving a dead branch.)
                    exit:
                        return err;
                    }
                    p_src++;
                    VFipf_memset(name, 0, sizeof(name));
                    p_dst = name;
                    len = 0;
                }
                break;
            case '\0':
                if (len > 12) {
                    return -20;
                }
                err = NANDCreate((char*)name, 0x30, 0);
                if (err != 0 && err != -6) {
                    goto exit;
                }
                NANDOpen((char*)name, &p_dev->info, 3);
                return NANDChangeDir((char*)nanddrv_home_dir);
        }
        len++;
        pos++;
        *p_dst++ = *p_src++;
    }
}

static s32 nanddrv_get_fat_type(u8* buf, u32* p_num_data_sectors) {
    u8 log2_bps;
    u8 log2_spc;
    u16 temp;
    u32 root_size;
    u16 num_root_sectors;
    u16 sectors_per_FAT16;
    u32 sectors_per_FAT;
    u32 first_data_sector;
    u32 total_sectors;

    log2_bps = 0;
    temp = 0x200;
    while ((temp = (temp >> 1) & 0x7FFF) != 0) {
        log2_bps++;
    }
    log2_spc = 0;
    temp = 1;
    while ((temp = (temp >> 1) & 0x7FFF) != 0) {
        log2_spc++;
    }

    root_size = PDM_LE16(buf, 0x11) * 32;
    num_root_sectors = (s32)(root_size + 0x1FF) >> log2_bps;
    sectors_per_FAT16 = PDM_LE16(buf, 0x16);
    sectors_per_FAT = (sectors_per_FAT16 == 0) ? PDM_LE32(buf + 0x24) : sectors_per_FAT16;
    first_data_sector = num_root_sectors + sectors_per_FAT * 2 + 1;
    total_sectors = PDM_LE16(buf, 0x13);
    if (total_sectors == 0) {
        total_sectors = PDM_LE32(buf + 0x20);
    }

    *p_num_data_sectors = total_sectors - first_data_sector;
    return (((total_sectors - first_data_sector) >> log2_spc) < 0xFF5) ? FAT_12 : ((((total_sectors - first_data_sector) >> log2_spc) < 0xFFF5) ? FAT_16 : FAT_32);
}

static s32 nanddrv_BuildUpFSInfoSector(u8* buf) {
    VFipf_memset(buf, 0, 0x200);
    NANDDRV_ST32(buf, 0x0, 0x0, 0x41615252);
    NANDDRV_ST32(buf, 0x1E4, 0x1E4, 0x61417272);
    NANDDRV_ST32(buf, 0x1E8, 0x1E8, 0xFFFFFFFF);
    NANDDRV_ST32(buf, 0x1EC, 0x1EC, 0xFFFFFFFF);
    NANDDRV_ST32(buf, 0x1FC, 0x1FC, 0xAA550000);
    return 0;
}

static s32 nanddrv_BuildUpBootSector(NANDDRV_DEV* p_dev, u8* buf, s32* p_type, u32 num_sectors) {
    u32 total_sectors;
    u32 sectors_per_FAT;
    s32 type;

    if (num_sectors != 0) {
        total_sectors = num_sectors;
    } else {
        total_sectors = p_dev->size >> 9;
    }

    if (total_sectors < 0xFF5) {
        type = FAT_12;
        sectors_per_FAT = ((total_sectors * 3) / 2 + 0x1FF) >> 9;
    } else if (total_sectors < 0xFFF5) {
        type = FAT_16;
        sectors_per_FAT = (total_sectors * 2 + 0x1FF) >> 9;
    } else {
        type = FAT_32;
        sectors_per_FAT = (total_sectors * 4 + 0x1FF) >> 9;
    }

    VFipf_memset(buf, 0, 0x200);
    buf[0x0] = 0xEB;
    buf[0x2] = 0x90;
    buf[0x1FE] = 0x55;
    buf[0x1FF] = 0xAA;
    NANDDRV_ST16(buf, 0x18, 0x3F);
    NANDDRV_ST16(buf, 0x1A, 0xFF);
    NANDDRV_ST16(buf, 0xB, 0x200);
    buf[0xD] = 1;
    NANDDRV_ST16(buf, 0xE, 1);
    buf[0x10] = 2;
    NANDDRV_ST16(buf, 0x11, 0x80);
    buf[0x15] = 0xF0;
    if (total_sectors < 0x10000) {
        NANDDRV_ST16(buf, 0x13, total_sectors);
        NANDDRV_ST32(buf, 0x20, 0x20, 0);
    } else {
        NANDDRV_ST16(buf, 0x13, 0);
        NANDDRV_ST32(buf, 0x20, 0x20, total_sectors);
    }

    if (type == FAT_32) {
        NANDDRV_ST16(buf, 0x11, 0);
        NANDDRV_ST32(buf, 0x24, 0x24, sectors_per_FAT);
        NANDDRV_ST16(buf, 0x28, 0);
        NANDDRV_ST16(buf, 0x2A, 0);
        NANDDRV_ST32(buf, 0x2C, 0x2C, 2);
        NANDDRV_ST16(buf, 0x30, 1);
        buf[0x41] = 0;
        buf[0x42] = 0x29;
        NANDDRV_ST32(buf, 0x43, 0x4C, 0x1234);
    } else {
        NANDDRV_ST16(buf, 0x11, 0x80);
        NANDDRV_ST16(buf, 0x16, sectors_per_FAT);
        buf[0x25] = 0;
        buf[0x26] = 0x29;
        NANDDRV_ST32(buf, 0x27, 0x30, 0x1234);
    }

    *p_type = type;
    return 0;
}

static s32 nanddrv_search_dev(struct PDM_DISK* p_disk, NANDDRV_DEV** pp_dev) {
    u16 i;

    for (i = 0; i < NANDDRV_NUM_DEV; i++) {
        if (nanddrv_dev[i].p_disk == p_disk) {
            *pp_dev = &nanddrv_dev[i];
            break;
        }
    }
    return (i == NANDDRV_NUM_DEV) ? -22 : 0;
}

static void nanddrv_set_last_error(struct PDM_DISK* p_disk, s32 err) {
    u16 i;

    for (i = 0; i < NANDDRV_NUM_DEV; i++) {
        if (nanddrv_dev[i].p_disk == p_disk) {
            nanddrv_dev[i].last_error = err;
            break;
        }
    }
}

static s32 nanddrv_clear_last_error(struct PDM_DISK* p_disk) {
    u16 i;

    for (i = 0; i < NANDDRV_NUM_DEV; i++) {
        if (nanddrv_dev[i].p_disk == p_disk) {
            nanddrv_dev[i].last_error = 0;
            break;
        }
    }
    return (i == NANDDRV_NUM_DEV) ? -22 : 0;
}

// The device to mount on p_disk: one already bound to it, else the first registered free one.
static inline NANDDRV_DEV* nanddrv_get_mount_dev(struct PDM_DISK* p_disk) {
    NANDDRV_DEV* p_dev;
    u16 i;

    for (i = 0; i < NANDDRV_NUM_DEV; i++) {
        p_dev = &nanddrv_dev[i];
        if (p_dev->p_disk == p_disk && (p_dev->flags & 2) == 0) {
            return p_dev;
        }
    }
    for (i = 0; i < NANDDRV_NUM_DEV; i++) {
        p_dev = &nanddrv_dev[i];
        if (p_dev->p_disk == NULL && p_dev->path[0] != '\0') {
            return p_dev;
        }
    }
    return NULL;
}

s32 nanddrv_init(struct PDM_DISK* p_disk) {
    if (p_disk == NULL) {
        return -20;
    }
    if (nanddrv_init_count == 0) {
        if (NANDInit() != 0) {
            return -21;
        }
        if (NANDGetCurrentDir((char*)nanddrv_home_dir) != 0) {
            return -21;
        }
        nanddrv_init_count++;
    }
    VFipdm_disk_notify_media_insert(p_disk);
    return 0;
}

s32 nanddrv_mount(struct PDM_DISK* p_disk) {
    NANDDRV_DEV* p_dev;
    NANDDRV_DEV* p_mount_dev;
    s32 err;

    pfk_get_semaphore(nanddrv_sem);
    if (p_disk == NULL) {
        pfk_release_semaphore(nanddrv_sem);
        return -20;
    }

    if (nanddrv_search_dev(p_disk, &p_dev) == 0 && (p_dev->flags & 2) != 0) {
        pfk_release_semaphore(nanddrv_sem);
        return -21;
    }

    p_mount_dev = nanddrv_get_mount_dev(p_disk);
    if (p_mount_dev == NULL) {
        pfk_release_semaphore(nanddrv_sem);
        return -22;
    }

    if (p_mount_dev->p_disk != NULL) {
        err = 0;
    } else {
        err = nanddrv_open_image(p_disk, p_mount_dev, 0);
    }
    if (err == 0) {
        p_mount_dev->flags |= 2;
    }
    pfk_release_semaphore(nanddrv_sem);
    return err;
}

s32 nanddrv_format(struct PDM_DISK* p_disk, const u8* param) {
    NANDDRV_DEV* p_dev;
    s32 err;

    p_dev = NULL;
    pfk_get_semaphore(nanddrv_sem);
    if (p_disk == NULL) {
        pfk_release_semaphore(nanddrv_sem);
        return -20;
    }
    if (nanddrv_search_dev(p_disk, &p_dev) != 0) {
        pfk_release_semaphore(nanddrv_sem);
        return -20;
    }

    p_dev->flags |= 4;
    err = nanddrv_open_image(p_disk, p_dev, 1);
    p_dev->flags &= ~4;
    pfk_release_semaphore(nanddrv_sem);
    return err;
}

s32 nanddrv_pread(struct PDM_DISK* p_disk, u8* p_buf, u32 block, u32 num_blocks, u32* p_num_success) {
    s32 err;

    pfk_get_semaphore(nanddrv_sem);
    *p_num_success = 0;
    if (p_disk == NULL) {
        pfk_release_semaphore(nanddrv_sem);
        return -20;
    }
    if (p_buf == NULL) {
        pfk_release_semaphore(nanddrv_sem);
        return -20;
    }
    err = nanddrv_physical_read(num_blocks, p_buf, block, 0x200, p_num_success, p_disk);
    if (err != 0) {
        pfk_release_semaphore(nanddrv_sem);
        return err;
    }
    pfk_release_semaphore(nanddrv_sem);
    return 0;
}

s32 nanddrv_pwrite(struct PDM_DISK* p_disk, const u8* p_buf, u32 block, u32 num_blocks, u32* p_num_success) {
    s32 err;

    pfk_get_semaphore(nanddrv_sem);
    *p_num_success = 0;
    if (p_disk == NULL) {
        pfk_release_semaphore(nanddrv_sem);
        return -20;
    }
    if (p_buf == NULL) {
        pfk_release_semaphore(nanddrv_sem);
        return -20;
    }
    err = nanddrv_physical_write(num_blocks, p_buf, block, 0x200, p_num_success, p_disk);
    if (err != 0) {
        pfk_release_semaphore(nanddrv_sem);
        return err;
    }
    pfk_release_semaphore(nanddrv_sem);
    return 0;
}

// Reopens the image so NAND commits what was written, keeping the file position.
s32 nanddrv_unmount(struct PDM_DISK* p_disk) {
    NANDDRV_DEV* p_dev;
    s32 err;
    s32 result;
    u32 pos;

    p_dev = NULL;
    pfk_get_semaphore(nanddrv_sem);
    if (p_disk == NULL) {
        pfk_release_semaphore(nanddrv_sem);
        return -20;
    }

    err = nanddrv_clear_last_error(p_disk);
    if (err == 0) {
        if (nanddrv_search_dev(p_disk, &p_dev) != 0) {
            pfk_release_semaphore(nanddrv_sem);
            return -20;
        }

        p_dev->flags &= ~2;
        if ((result = NANDGetPosition(&p_dev->info, &pos)) != 0) {
            err = result;
        } else if ((result = NANDClose(&p_dev->info)) < 0) {
            err = result;
        } else if ((result = NANDOpen((char*)p_dev->path, &p_dev->info, 3)) < 0) {
            err = result;
        } else {
            result = NANDSeek(&p_dev->info, pos, 0);
            err = (result < 0) ? result : 0;
        }

        if (err < 0) {
            nanddrv_set_last_error(p_disk, err);
            pfk_release_semaphore(nanddrv_sem);
            return err;
        }
    }
    pfk_release_semaphore(nanddrv_sem);
    return err;
}

s32 nanddrv_finalize(struct PDM_DISK* p_disk) {
    s32 err;
    u16 i;

    err = 0;
    pfk_get_semaphore(nanddrv_sem);
    if (p_disk == NULL) {
        pfk_release_semaphore(nanddrv_sem);
        return -20;
    }

    for (i = 0; i < NANDDRV_NUM_DEV; i++) {
        if (nanddrv_dev[i].p_disk == p_disk) {
            err = NANDClose(&nanddrv_dev[i].info);
            VFipf_memset(&nanddrv_dev[i], 0, sizeof(NANDDRV_DEV));
            break;
        }
    }
    pfk_release_semaphore(nanddrv_sem);
    if (err != 0) {
        return -20;
    }
    return (i == NANDDRV_NUM_DEV) ? -22 : 0;
}

s32 nanddrv_get_disk_info(struct PDM_DISK* p_disk, struct PDM_DISK_INFO* p_disk_info) {
    NANDDRV_DEV* p_dev;
    u32 num_sectors;

    p_dev = NULL;
    pfk_get_semaphore(nanddrv_sem);
    if (p_disk == NULL || p_disk_info == NULL) {
        pfk_release_semaphore(nanddrv_sem);
        return -20;
    }
    if (nanddrv_search_dev(p_disk, &p_dev) != 0) {
        pfk_release_semaphore(nanddrv_sem);
        return -20;
    }

    num_sectors = p_dev->size >> 9;
    p_disk_info->total_sectors = num_sectors;
    p_disk_info->cylinders = num_sectors / 255 / 63 / 512;
    p_disk_info->heads = 0xFF;
    p_disk_info->sectors_per_track = 0x3F;
    p_disk_info->bytes_per_sector = 0x200;
    p_disk_info->media_attr = 0;
    p_disk_info->format_param = NULL;
    pfk_release_semaphore(nanddrv_sem);
    return 0;
}

s32 VFi_nanddrv_init_drv_tbl(struct PDM_DISK_TBL* p_disk_tbl, u32 ui_ext) {
    p_disk_tbl->ui_ext = ui_ext;
    p_disk_tbl->p_func = (struct PDM_FUNCTBL*)&l_nand_func;
    return 0;
}

// Registers a NAND image (path below the NAND root, size in bytes) for the next mount.
s32 VFi_nanddrv_register_image(const s8* path, u32 size) {
    u16 i;

    if (VFipf_strlen(path) >= 0x4C) {
        return -20;
    }

    if (nanddrv_register_count == 0) {
        VFipf_memset(nanddrv_dev, 0, sizeof(nanddrv_dev));
        nanddrv_register_count++;
        nanddrv_sem_param.initial_count = 1;
        nanddrv_sem_param.max_count = 1;
        nanddrv_sem_param.p_buf = nanddrv_sem_buf;
        nanddrv_sem = pfk_create_semaphore(&nanddrv_sem_param);
        if (nanddrv_sem == 0) {
            return -21;
        }
    }

    for (i = 0; i < NANDDRV_NUM_DEV; i++) {
        if (VFipf_strcmp(nanddrv_dev[i].path, path) == 0) {
            return 0;
        }
        if (nanddrv_dev[i].p_disk == NULL) {
            VFipf_strcpy(nanddrv_dev[i].path, path);
            nanddrv_dev[i].size = size;
            break;
        }
    }
    return (i == NANDDRV_NUM_DEV) ? -22 : 0;
}

static s32 nanddrv_physical_read(u32 num_blocks, u8* buf, u32 block, u32 bps, u32* p_num_success, struct PDM_DISK* p_disk) {
    u8 tmp_buf[0x200] ATTRIBUTE_ALIGN(32);
    u32 pos;
    u32 file_size;
    u32 offset;
    u32 size;
    u32 done;
    s32 err;
    NANDDRV_DEV* p_dev;

    p_dev = NULL;
    if (nanddrv_search_dev(p_disk, &p_dev) != 0) {
        return -20;
    }

    size = num_blocks * bps;
    file_size = p_dev->size;
    offset = block * bps;
    if (NANDGetPosition(&p_dev->info, &pos) != 0 || pos != offset) {
        err = NANDSeek(&p_dev->info, offset, 0);
    } else {
        err = 0;
    }
    if (err >= 0) {
        if (offset + size > file_size) {
            size = file_size - offset;
            done = 0;
            while (size >= 0x200) {
                err = NANDRead(&p_dev->info, tmp_buf, 0x200);
                if (err < 0) {
                    return -22;
                }
                VFipf_memcpy(buf + done, tmp_buf, err);
                done += err;
                size -= err;
            }
            if (size != 0) {
                err = NANDRead(&p_dev->info, tmp_buf, size);
                if (err < 0) {
                    return -22;
                }
                VFipf_memcpy(buf + done, tmp_buf, err);
            }
            return -22;
        }

        done = 0;
        while (size >= 0x200) {
            err = NANDRead(&p_dev->info, tmp_buf, 0x200);
            if (err < 0) {
                nanddrv_set_last_error(p_disk, err);
                return err;
            }
            VFipf_memcpy(buf + done, tmp_buf, err);
            done += err;
            size -= err;
        }
        if (size != 0) {
            err = NANDRead(&p_dev->info, tmp_buf, size);
            if (err < 0) {
                nanddrv_set_last_error(p_disk, err);
                return err;
            }
            VFipf_memcpy(buf + done, tmp_buf, err);
        }
        *p_num_success = num_blocks;
        return 0;
    }
    nanddrv_set_last_error(p_disk, err);
    return err;
}

static s32 nanddrv_physical_write(u32 num_blocks, const u8* buf, u32 block, u32 bps, u32* p_num_success, struct PDM_DISK* p_disk) {
    u8 tmp_buf[0x200] ATTRIBUTE_ALIGN(32);
    u32 pos;
    u32 file_size;
    u32 offset;
    u32 size;
    u32 done;
    s32 err;
    NANDDRV_DEV* p_dev;

    p_dev = NULL;
    if (nanddrv_search_dev(p_disk, &p_dev) != 0) {
        return -20;
    }

    size = num_blocks * bps;
    file_size = p_dev->size;
    offset = block * bps;
    if (NANDGetPosition(&p_dev->info, &pos) != 0 || pos != offset) {
        err = NANDSeek(&p_dev->info, offset, 0);
    } else {
        err = 0;
    }
    if (err >= 0) {
        if (offset + size > file_size) {
            size = file_size - offset;
            done = 0;
            while (size >= 0x200) {
                VFipf_memcpy(tmp_buf, (void*)(buf + done), 0x200);
                err = NANDWrite(&p_dev->info, tmp_buf, 0x200);
                if (err < 0) {
                    return -22;
                }
                size -= err;
                done += err;
            }
            if (size != 0) {
                VFipf_memcpy(tmp_buf, (void*)(buf + done), size);
                NANDWrite(&p_dev->info, tmp_buf, size);
            }
            return -22;
        }

        done = 0;
        while (size >= 0x200) {
            VFipf_memcpy(tmp_buf, (void*)(buf + done), 0x200);
            err = NANDWrite(&p_dev->info, tmp_buf, 0x200);
            if (err < 0) {
                nanddrv_set_last_error(p_disk, err);
                return err;
            }
            size -= err;
            done += err;
        }
        if (size != 0) {
            VFipf_memcpy(tmp_buf, (void*)(buf + done), size);
            err = NANDWrite(&p_dev->info, tmp_buf, size);
            if (err < 0) {
                nanddrv_set_last_error(p_disk, err);
                return err;
            }
        }
        *p_num_success = num_blocks;
        return 0;
    }
    nanddrv_set_last_error(p_disk, err);
    return err;
}

const struct PDM_FUNCTBL l_nand_func = {
    nanddrv_init,
    nanddrv_finalize,
    nanddrv_mount,
    nanddrv_unmount,
    (long (*)(struct PDM_DISK*, unsigned char*))nanddrv_format,
    (long (*)(struct PDM_DISK*, unsigned char*, unsigned long, unsigned long, unsigned long*))nanddrv_pread,
    (long (*)(struct PDM_DISK*, unsigned char*, unsigned long, unsigned long, unsigned long*))nanddrv_pwrite,
    nanddrv_get_disk_info,
};
#else
#include <revolution/nand.h>
#include <revolution/vf/vf_struct.h>
#include <revolution/vf/d_common.h>
#include <revolution/vf/d_vf_sys.h>
#include <revolution/vf/nand_drv.h>

s32 VF_nand_sleep_msec;
s32 VF_nand_retry_max;

static struct {
    s32 (*create)(const char*, u8, u8);
    s32 (*open)(const char*, struct NANDFileInfo*, u8);
    s32 (*createDir)(const char*, u8, u8);
    s32 (*delete)(const char*);
} l_nandFunc[26];

static inline void _SleepAfewMiliSec(void) {
    OSSleepTicks(
        (s64)VF_nand_sleep_msec * ((*(u32*)0x800000F8 / 4) / 1000));
}

s32 VFi_NandClose(struct NANDFileInfo* info) {
    s32 challenge;
    s32 error;

    challenge = VF_nand_retry_max;
    error = 0;
    while (challenge-- > 0) {
        error = NANDClose(info);
        if (error != NAND_RESULT_BUSY) {
            return error;
        } else {
            _SleepAfewMiliSec();
        }
    }
    return error;
}

s32 VFi_NandOpen(const char* path, struct NANDFileInfo* info, u8 accType) {
    s32 challenge;
    s32 error;

    challenge = VF_nand_retry_max;
    error = 0;
    while (challenge-- > 0) {
        error = NANDOpen(path, info, accType);
        if (error != NAND_RESULT_BUSY) {
            return error;
        } else {
            _SleepAfewMiliSec();
        }
    }
    return error;
}

s32 VFi_NandPrivateOpen(
    const char* path, struct NANDFileInfo* info, u8 accType) {
    s32 challenge;
    s32 error;

    challenge = VF_nand_retry_max;
    error = 0;
    while (challenge-- > 0) {
        error = NANDPrivateOpen(path, info, accType);
        if (error != NAND_RESULT_BUSY) {
            return error;
        } else {
            _SleepAfewMiliSec();
        }
    }
    return error;
}

s32 VFi_NandFlushNANDFromHandleIdx(s32 i_handleIdx, int i_setLastDeviceError) {
    s32 NANDError;
    struct VF_HANDLE_DRIVE* drive_p;
    struct VF_HANDLE_TYPE* handle_p;
    const char* sys_name_p;
    struct NANDFileInfo* fileInfo_p;

    drive_p = (struct VF_HANDLE_DRIVE*)VFSysGetDriveP(i_handleIdx);
    handle_p = (struct VF_HANDLE_TYPE*)VFSysGetHandleP(i_handleIdx);

    if (handle_p != NULL && handle_p->device_p != NULL && handle_p->device_p->sync_mode == 1) {
        return 0;
    }

    if (drive_p != NULL) {
        sys_name_p = (const char*)drive_p->pf_filename;
        fileInfo_p = drive_p->file_p;
        NANDError = VFi_NandClose(fileInfo_p);

        if (NANDError < 0) {
            if (i_setLastDeviceError != 0) {
                dCommon_setLastDeviceErrorToDisk2(i_handleIdx, NANDError);
            }
            return NANDError;
        }

        if ((u32)i_handleIdx < 26) {
            s32 (*tmpOpen)(const char*, struct NANDFileInfo*, u8) = l_nandFunc[i_handleIdx].open;
            if (tmpOpen != NULL) {
                NANDError = tmpOpen(sys_name_p, fileInfo_p, 3);
            } else {
                NANDError = VFi_NandOpen(sys_name_p, fileInfo_p, 3);
            }
        } else if (i_handleIdx == 0xFFFFFFF6) {
            NANDError = VFi_NandOpen(sys_name_p, fileInfo_p, 3);
        } else {
            NANDError = VFi_NandPrivateOpen(sys_name_p, fileInfo_p, 3);
        }

        if (NANDError < 0) {
            if (i_setLastDeviceError != 0) {
                dCommon_setLastDeviceErrorToDisk2(i_handleIdx, NANDError);
            }
            return NANDError;
        }
    }
    return 0;
}
#endif
