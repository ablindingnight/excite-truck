#include <revolution/vf.h>

s32 VFiPFFAT16_ReadFATEntry(struct PF_VOLUME* p_vol, u32 cluster, u32* p_value) {
    u32 fat_offset;
    u32 fat_sector;
    u16 offset_in_sector;
    u8 buf[2];
    s32 err;
    u32 current_fat;
    s32 result;

#ifndef GAME_REXE01
    if (!p_vol) {
        *p_value = -1;
        return 10;
    }
#endif
#ifndef GAME_REXE01
    if (!((cluster >= 2) && (cluster < p_vol->bpb.num_clusters + 2)) && (cluster != 0) && (cluster != 1)) {
        *p_value = -1;
        return 14;
    }
#endif

    fat_offset = (s64)(cluster << 1);

    fat_sector = (u16)(p_vol->bpb.active_FAT_sector + (fat_offset >> p_vol->bpb.log2_bytes_per_sector));
    offset_in_sector = fat_offset & (p_vol->bpb.bytes_per_sector - 1);

    if ((p_vol->bpb.ext_flags & 0x80) == 0) {
        current_fat = 1;
    } else {
        current_fat = (p_vol->bpb.ext_flags) & 7;
    }

    do {
        err = VFiPFSEC_ReadFAT(p_vol, buf, fat_sector, offset_in_sector, 2);

        if (err != 0x1000 || p_vol->p_callback == 0) {
            goto check;
        }

        result = ((s32 (*)(s32))p_vol->p_callback)(p_vol->last_driver_error);
        if (result != 0) {
            if (result == 1 && p_vol->bpb.num_active_FATs >= 2 && current_fat < (u32)p_vol->bpb.num_active_FATs) {
                current_fat++;
                fat_sector += p_vol->bpb.sectors_per_FAT;
            } else {
            check:;
                if (err != 0) {
                    *p_value = -1;
                    return err;
                }
            }
        }
    } while (err != 0);

    *p_value = SWAP16(*(u16*)buf);
    return 0;
}

#ifndef GAME_REXE01
s32 VFiPFFAT16_ReadFATEntryPage(struct PF_VOLUME* p_vol, u32 cluster, u32* p_value, struct PF_CACHE_PAGE** pp_page) {
    s32 err;
    u32 offset;
    u32 sector;
    u32 current_fat;
    s32 result;

    err = 0;
#ifndef GAME_REXE01
    if (!p_vol) {
        return 10;
    }
#endif

#ifndef GAME_REXE01
    if (!((cluster >= 2) && (cluster < p_vol->bpb.num_clusters + 2)) && (cluster != 0) && (cluster != 1)) {
        return 14;
    }
#endif

    offset = cluster << 1;
    sector = (u16)(p_vol->bpb.active_FAT_sector + (offset >> p_vol->bpb.log2_bytes_per_sector));

    if ((p_vol->bpb.ext_flags & 0x80) == 0) {
        current_fat = 1;
    } else {
        current_fat = (p_vol->bpb.ext_flags) & 7;
    }

    if ((*pp_page)->sector > sector || (*pp_page)->sector + p_vol->cache.fat_buff_size <= sector) {
        if ((*pp_page)->option == 1) {
            err = VFiPFFAT_UpdateFATEntry(p_vol, *pp_page);
            if (err != 0) {
                return err;
            }
        }

        do {
            err = VFiPFCACHE_ReadFATPage(p_vol, sector, pp_page);
            if (err != 0x1000 || p_vol->p_callback == 0) {
                goto check;
            }

            result = ((s32 (*)(s32))p_vol->p_callback)(p_vol->last_driver_error);
            if (result != 0) {
                if (result == 1 && p_vol->bpb.num_active_FATs >= 2 && current_fat < p_vol->bpb.num_active_FATs) {
                    current_fat++;
                    sector += p_vol->bpb.sectors_per_FAT;
                } else {
                check:;
                    if (err != 0) {
                        return err;
                    }
                }
            }
        } while (err != 0);

    } else {
        if (sector != (*pp_page)->sector + (((*pp_page)->p_buf - (*pp_page)->buffer) >> p_vol->bpb.log2_bytes_per_sector)) {
            (*pp_page)->p_buf += p_vol->bpb.bytes_per_sector;
        }
    }

    offset &= (p_vol->bpb.bytes_per_sector - 1);
    *p_value = SWAP16(*((u16*)((*pp_page)->p_buf + offset)));
    return err;
}
#endif

s32 VFiPFFAT16_WriteFATEntry(struct PF_VOLUME* p_vol, u32 cluster, u32 value) {
    s32 err;
    u32 fat_offset;
    u16 fat_sector;
    u16 offset_in_sector;
    u8 buf[2];
#ifdef GAME_REXE01
    u32 i;
    s32 first_err;
#endif

#ifndef GAME_REXE01
    if (!p_vol) {
        return 10;
    }
#endif

#ifndef GAME_REXE01
    if (!((cluster >= 2) && (cluster < p_vol->bpb.num_clusters + 2)) && (cluster != 0) && (cluster != 1)) {
        return 14;
    }
#endif

    fat_offset = cluster << 1;
    fat_sector = (u16)(p_vol->bpb.active_FAT_sector + (fat_offset >> p_vol->bpb.log2_bytes_per_sector));
    offset_in_sector = fat_offset & (p_vol->bpb.bytes_per_sector - 1);

    *(u16*)buf = SWAP16(value);

#ifdef GAME_REXE01
    // Excite Truck's older PrFILE2 writes every active FAT copy here
    first_err = 0;
    for (i = 0; i < p_vol->bpb.num_active_FATs; i++) {
        err = VFiPFSEC_WriteFAT(p_vol, buf, fat_sector, offset_in_sector, 2);
        if (err != 0 && first_err == 0) {
            first_err = err;
        }
        fat_sector += p_vol->bpb.sectors_per_FAT;
    }

    return first_err;
#else
    err = VFiPFSEC_WriteFAT(p_vol, buf, fat_sector, offset_in_sector, 2);

    return err;
#endif
}

#ifndef GAME_REXE01
s32 VFiPFFAT16_WriteFATEntryPage(struct PF_VOLUME* p_vol, u32 cluster, u32 value, struct PF_CACHE_PAGE** pp_page) {
    s32 err;
    u32 fat_offset;
    u32 fat_sector;
    u32 offset_in_sector;
    u32 current_fat;
    s32 result;

    err = 0;
#ifndef GAME_REXE01
    if (!p_vol) {
        return 10;
    }
#endif
#ifndef GAME_REXE01
    if (!((cluster >= 2) && (cluster < p_vol->bpb.num_clusters + 2)) && (cluster != 0) && (cluster != 1)) {
        return 14;
    }
#endif

    fat_offset = (s64)(cluster << 1);
    fat_sector = (u16)(p_vol->bpb.active_FAT_sector + (fat_offset >> p_vol->bpb.log2_bytes_per_sector));
    offset_in_sector = (unsigned short)(fat_offset & (p_vol->bpb.bytes_per_sector - 1));

    if ((*pp_page)->sector > fat_sector || (*pp_page)->sector + p_vol->cache.fat_buff_size <= fat_sector) {
        err = VFiPFFAT_UpdateFATEntry(p_vol, *pp_page);
        if (err != 0) {
            return err;
        }

        if ((p_vol->bpb.ext_flags & 0x80) == 0) {
            current_fat = 1;
        } else {
            current_fat = (p_vol->bpb.ext_flags) & 7;
        }

        do {
            err = VFiPFCACHE_ReadFATPage(p_vol, fat_sector, pp_page);
            if (err != 0x1000 || p_vol->p_callback == 0) {
                goto check;
            }

            result = ((s32 (*)(s32))p_vol->p_callback)(p_vol->last_driver_error);
            if (result != 0) {
                if (result == 1 && p_vol->bpb.num_active_FATs >= 2 && current_fat < p_vol->bpb.num_active_FATs) {
                    current_fat++;
                    fat_sector += p_vol->bpb.sectors_per_FAT;
                } else {
                check:;
                    if (err != 0) {
                        return err;
                    }
                }
            }
        } while (err != 0);

    } else {
        if (fat_sector != (*pp_page)->sector + (((*pp_page)->p_buf - (*pp_page)->buffer) >> p_vol->bpb.log2_bytes_per_sector)) {
            (*pp_page)->p_buf += p_vol->bpb.bytes_per_sector;
        }
    }

    (*pp_page)->p_buf[offset_in_sector] = (u8)value;
    (*pp_page)->p_buf[offset_in_sector + 1] = (u8)(value >> 8);

    VFiPFCACHE_UpdateModifiedSector((*pp_page));

    return err;
}
#endif

#ifdef GAME_REXE01
s32 VFiPFFAT16_WriteFATEntryPage(struct PF_VOLUME* p_vol, u32 cluster, u32 value, struct PF_CACHE_PAGE* p_page) {
    s32 err;
    u32 fat_offset;
    u16 fat_sector;
    u16 offset_in_sector;
    u32 i;
    s32 first_err;

    fat_offset = cluster << 1;
    fat_sector = (u16)(p_vol->bpb.active_FAT_sector + (fat_offset >> p_vol->bpb.log2_bytes_per_sector));
    offset_in_sector = fat_offset & (p_vol->bpb.bytes_per_sector - 1);

    err = 0;
    first_err = 0;
    if (p_page->sector != fat_sector) {
        for (i = 0; i < p_vol->bpb.num_active_FATs; i++) {
            err = VFiPFSEC_WriteFAT(p_vol, p_page->p_buf, p_page->sector + i * p_vol->bpb.sectors_per_FAT, 0, p_vol->bpb.bytes_per_sector);
            if (err != 0 && first_err == 0) {
                first_err = err;
            }
        }
        p_page->sector = fat_sector;
        err = VFiPFSEC_ReadFAT(p_vol, p_page->p_buf, fat_sector, 0, p_vol->bpb.bytes_per_sector);
    }

    p_page->p_buf[offset_in_sector] = (u8)value;
    p_page->p_buf[offset_in_sector + 1] = (u8)(value >> 8);
    if (err != 0) {
        first_err = err;
    }
    return first_err;
}

s32 VFiPFFAT16_ReadFATEntryPage(struct PF_VOLUME* p_vol, u32 cluster, u32* p_value, struct PF_CACHE_PAGE* p_page) {
    u32 offset;
    u32 sector;
    u32 current_fat;
    s32 err;
    s32 result;
    u16 i;

    offset = cluster << 1;
    sector = (u16)(p_vol->bpb.active_FAT_sector + (offset >> p_vol->bpb.log2_bytes_per_sector));

    if ((p_vol->bpb.ext_flags & 0x80) == 0) {
        current_fat = 1;
    } else {
        current_fat = (p_vol->bpb.ext_flags) & 7;
    }

    if (p_page->sector != sector) {
        if (p_page->option == 1) {
            for (i = 0; i < p_vol->bpb.num_active_FATs; i++) {
                VFiPFSEC_WriteFAT(p_vol, p_page->p_buf, p_page->sector + i * p_vol->bpb.sectors_per_FAT, 0, p_vol->bpb.bytes_per_sector);
            }
        }

        do {
            err = VFiPFSEC_ReadFAT(p_vol, p_page->p_buf, sector, 0, p_vol->bpb.bytes_per_sector);
            if (err != 0x1000 || p_vol->p_callback == 0) {
                goto check;
            }

            result = ((s32 (*)(s32))p_vol->p_callback)(p_vol->last_driver_error);
            if (result != 0) {
                if (result == 1 && p_vol->bpb.num_active_FATs >= 2 && current_fat < p_vol->bpb.num_active_FATs) {
                    current_fat++;
                    sector += p_vol->bpb.sectors_per_FAT;
                } else {
                check:;
                    if (err != 0) {
                        return err;
                    }
                    p_page->sector = sector;
                }
            }
        } while (err != 0);
    }

    offset &= (p_vol->bpb.bytes_per_sector - 1);
    *p_value = SWAP16(*((u16*)(p_page->p_buf + offset)));
    return 0;
}
#endif
