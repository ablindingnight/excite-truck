// Excite Truck's VF front end (0x800B7968-0x800B7EAC). ET predates the NAND-only d_vf of later
// SDKs: drives are SD (type 0), a NAND-file disk (type 2) or a caller-provided partition (type 3).
// No reference exists for this layer; all names here are guesses.
#include <revolution/vf.h>
#include <revolution/vf/pf_attach.h>
#include <revolution/vf/pf_detach.h>
#include <revolution/vf/pf_fclose.h>
#include <revolution/vf/pf_finfo.h>
#include <revolution/vf/pf_fopen.h>
#include <revolution/vf/pf_fread.h>
#include <revolution/vf/pf_fseek.h>
#include <revolution/vf/pf_fsfirst.h>
#include <revolution/vf/pf_fsnext.h>
#include <revolution/vf/pf_init_prfile2.h>
#include <revolution/vf/pf_unmount.h>
#include <revolution/vf/vf_et.h>

#include <string.h>

typedef struct VF_ET_INIT_DISK_SET {
    struct PDM_INIT_DISK* p_tbl;
    u32 num;
} VF_ET_INIT_DISK_SET;

static struct PDM_INIT_DISK VFi_init_disk_tbl[3] = {
    {VFi_sddrv_init_drv_tbl, 0},
    {NULL, 0},
    {VFi_nanddrv_init_drv_tbl, 0},
};

static VF_ET_INIT_DISK_SET VFi_init_disk_set = {VFi_init_disk_tbl, 3};

static struct PDM_DISK* VFi_disk[26];
static struct PDM_PARTITION* VFi_part[26];
static VF_ET_MEDIA_CALLBACK VFi_media_callback[3][2];  // [drive type][insert, eject]; only SD sets one

static void VFi_InsertCallback(s32 slot);
static void VFi_EjectCallback(s32 slot);

static s32 VFi_pdm_initialized;
static s32 VFi_busy;
static s32 VFi_pf_initialized;

s32 VFAttachDrive(u8 type, const s8* dev_name, u32 dev_param, struct PF_DRV_TBL* p_drv) {
    struct PDM_DISK* p_disk;
    struct PF_DRV_TBL* drv_tbl[2];
    struct PF_ET_DEV_CONFIG config;
    s32 err;
    s32 idx;

    if (type >= 4) {
        return -2;
    }
    if (p_drv == NULL) {
        return -2;
    }
    if (type == 2 && dev_name == NULL) {
        return -2;
    }
    if (type == 3 && p_drv->p_part == NULL) {
        return -2;
    }

    if (type <= 2) {
        if (VFipdm_open_disk(&VFi_init_disk_set.p_tbl[type], &p_disk) != 0) {
            return -1;
        }
        if (VFipdm_open_partition(p_disk, 0, (struct PDM_PARTITION**)&p_drv->p_part) != 0) {
            return -1;
        }
    }

    drv_tbl[0] = p_drv;
    drv_tbl[1] = NULL;
    if (type == 2) {
        memcpy(config.name, dev_name, 0x4E);
        config.param = dev_param;
    }
    if (type == 2) {
        err = VFipf2_attach(drv_tbl, &config);
    } else {
        err = VFipf2_attach(drv_tbl, NULL);
    }
    if (err != 0) {
        return -1;
    }

    idx = p_drv->drive - 'A';
    if (type <= 2) {
        VFi_disk[idx] = p_disk;
        VFi_part[idx] = p_drv->p_part;
        if (type == 0 && VFiSD_IsInserted()) {
            drv_tbl[0]->stat |= 0x10;
        }
    } else {
        VFi_disk[idx] = NULL;
        VFi_part[idx] = NULL;
    }
    return 0;
}

s32 VFDetachDrive(s8 drive) {
    struct PDM_DISK* p_disk;
    struct PDM_PARTITION* p_part;

    p_disk = VFi_disk[drive - 'A'];
    p_part = VFi_part[drive - 'A'];
    if (VFipf2_detach(drive) != 0) {
        return -1;
    }
    if (p_disk != NULL && p_part != NULL) {
        if (VFipdm_close_partition(p_part) != 0) {
            return -1;
        }
        if (VFipdm_close_disk(p_disk) != 0) {
            return -1;
        }
    }
    return 0;
}

s32 VFCloseFile(struct PF_FILE* p_file) {
    if (VFipf2_fclose(p_file) != 0) {
        return -1;
    }
    return 0;
}

s32 VFGetFileInfo(struct PF_FILE* p_file, struct PF_INFO* p_info) {
    if (VFipf2_finfo(p_file, p_info) != 0) {
        return -1;
    }
    return 0;
}

struct PF_FILE* VFOpenFile(const s8* path, const s8* mode) {
    struct PF_FILE* p_file;

    p_file = VFipf2_fopen(path, mode);
    if (p_file == NULL) {
        return NULL;
    }
    return p_file;
}

u32 VFReadFile(void* p_buf, u32 size, u32 count, struct PF_FILE* p_file) {
    return VFipf2_fread(p_buf, size, count, p_file);
}

s32 VFSeekFile(struct PF_FILE* p_file, s32 offset, s32 origin) {
    if (VFipf2_fseek(p_file, offset, origin) != 0) {
        return -1;
    }
    return 0;
}

s32 VFFindFirst(const s8* path, u8 attr, struct PF_DTA* p_dta) {
    if (VFipf2_fsfirst(path, attr, p_dta) != 0) {
        return -1;
    }
    return 0;
}

s32 VFFindNext(struct PF_DTA* p_dta) {
    if (VFipf2_fsnext(p_dta) != 0) {
        return -1;
    }
    return 0;
}

s32 VFInit(s32 config) {
    s32 err;

    if (VFi_busy == 1) {
        return -1;
    }
    VFi_busy = 1;

    if (VFi_pdm_initialized == 0) {
        err = VFipdm_init_diskmanager(0, NULL);
        VFi_pdm_initialized = (err == 0);
        if (err != 0) {
            VFi_busy = 0;
            return -1;
        }
    }

    if (VFi_pf_initialized == 0) {
        err = VFipf2_init_prfile2(config, NULL);
        VFi_pf_initialized = (err == 0);
        if (err == -1) {
            VFi_busy = 0;
            return -1;
        }
    }

    VFi_busy = 0;
    return 0;
}

s32 VFSetMediaCallback(s32 type, VF_ET_MEDIA_CALLBACK insert, VF_ET_MEDIA_CALLBACK eject) {
    s32 err;

    switch (type) {
        case 0:
            VFi_media_callback[0][0] = insert;
            VFi_media_callback[0][1] = eject;
            err = VFiSD_SetCallback(VFi_InsertCallback, VFi_EjectCallback);
            break;
        default:
            return -2;
    }
    return err ? -1 : 0;
}

static void VFi_InsertCallback(s32 slot) {
    if (VFi_media_callback[0][0] != NULL) {
        VFi_media_callback[0][0](slot);
    }
}

static void VFi_EjectCallback(s32 slot) {
    if (VFi_media_callback[0][1] != NULL) {
        VFi_media_callback[0][1](slot);
    }
}

s32 VFUnmountDrive(s8 drive, u32 mode) {
    s32 err;

    err = VFipf2_unmount(drive, mode);
    if (err == -1) {
        return -1;
    }
    if (err == 1) {
        return 1;
    }
    if (err != 0) {
        return -1;
    }
    return 0;
}
