#include <revolution/vf.h>

s32 VFiPFFAT32_ReadFATEntry(struct PF_VOLUME* p_vol, u32 cluster, u32* p_value) {
    u32 fat_offset;
    u32 fat_sector;
    u16 offset_in_sector;
    u8 buf[4];
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

    fat_offset = cluster << 2;
    current_fat = 1;

    fat_sector = (u16)(p_vol->bpb.active_FAT_sector + (fat_offset >> p_vol->bpb.log2_bytes_per_sector));
    offset_in_sector = fat_offset & (p_vol->bpb.bytes_per_sector - 1);

    if ((p_vol->bpb.ext_flags & 0x80) == 0) {
        current_fat = 1;
    } else {
        current_fat = p_vol->bpb.ext_flags & 7;
    }

    do {
        err = VFiPFSEC_ReadFAT(p_vol, buf, fat_sector, offset_in_sector, 4);

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
                    *p_value = -1;
                    return err;
                }
            }
        }
    } while (err != 0);

    err = 0;
    *p_value = (u32)(SWAP32(*(u32*)buf)) & 0x0FFFFFFF;

    return err;
}

#ifndef GAME_REXE01
s32 VFiPFFAT32_ReadFATEntryPage(struct PF_VOLUME* p_vol, u32 cluster, u32* p_value, struct PF_CACHE_PAGE** pp_page) {
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

    offset = cluster << 2;
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
    *p_value = (u32)(SWAP32(*((u32*)((*pp_page)->p_buf + offset)))) & 0x0FFFFFFF;

    return err;
}
#endif

s32 VFiPFFAT32_WriteFATEntry(struct PF_VOLUME* p_vol, u32 cluster, u32 value) {
    s32 err;
    u32 fat_offset;
    u32 fat_sector;
    u16 offset_in_sector;
#ifdef GAME_REXE01
    u8 buf[4];
    u32 dword;
    u16 i;
    s32 first_err;
#else
    u8 buf[4];
    u32 dword;
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

    fat_offset = cluster << 2;
    fat_sector = (u16)(p_vol->bpb.active_FAT_sector + (fat_offset >> p_vol->bpb.log2_bytes_per_sector));
    offset_in_sector = fat_offset & (p_vol->bpb.bytes_per_sector - 1);

#ifdef GAME_REXE01
    err = VFiPFSEC_ReadFAT(p_vol, buf, fat_sector, offset_in_sector, 4);
    if (err != 0) {
        return err;
    }

    // Excite Truck's older PrFILE2 writes every active FAT copy here
    *(u32*)buf = SWAP32(((value & 0xFFFFFFF) | ((SWAP32(*(u32*)buf)) & 0xF0000000)));
    first_err = 0;
    for (i = 0; i < p_vol->bpb.num_active_FATs; i++) {
        err = VFiPFSEC_WriteFAT(p_vol, buf, fat_sector, offset_in_sector, 4);
        if (err != 0 && first_err == 0) {
            first_err = err;
        }
        fat_sector += p_vol->bpb.sectors_per_FAT;
    }

    return first_err;
#else
    err = VFiPFSEC_ReadFAT(p_vol, buf, fat_sector, offset_in_sector, 4);
    switch (err) {
        case 0: {
            *(u32*)buf = SWAP32(((value & 0xFFFFFFF) | ((SWAP32(*(u32*)buf)) & 0xF0000000)));
            err = VFiPFSEC_WriteFAT(p_vol, buf, fat_sector, offset_in_sector, 4);
            break;
        }
        default:
            break;
    }

    return err;
#endif
}

#ifndef GAME_REXE01
s32 VFiPFFAT32_WriteFATEntryPage(struct PF_VOLUME* p_vol, u32 cluster, u32 value, struct PF_CACHE_PAGE** pp_page) {
    s32 err;
    u32 fat_offset;
    u32 fat_sector;
    u32 offset_in_sector;  // Present in DWARF but unused here.
    u32 dword;             // Present in DWARF but unused here.
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

    fat_offset = cluster << 2;
    fat_sector = (u16)(p_vol->bpb.active_FAT_sector + (fat_offset >> p_vol->bpb.log2_bytes_per_sector));
    fat_offset = (u16)(fat_offset & (p_vol->bpb.bytes_per_sector - 1));

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

    *(u32*)((*pp_page)->p_buf + fat_offset) = SWAP32((value & 0xFFFFFFF) | ((SWAP32(*(u32*)((*pp_page)->p_buf + fat_offset))) & 0xF0000000));
    VFiPFCACHE_UpdateModifiedSector((*pp_page));

    return err;
}
#endif

#ifdef GAME_REXE01
s32 VFiPFFAT32_WriteFATEntryPage(struct PF_VOLUME* p_vol, u32 cluster, u32 value, struct PF_CACHE_PAGE* p_page) {
    s32 err;
    u16 fat_sector;
    u32 fat_offset;
    u16 offset_in_sector;
    s32 first_err;
    u16 i;

    fat_offset = cluster << 2;
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
        err = VFiPFSEC_ReadFAT(p_vol, p_page->p_buf, fat_sector, 0, p_vol->bpb.bytes_per_sector);
        if (err != 0) {
            return err;
        }
        p_page->sector = fat_sector;
    }

    *(u32*)(p_page->p_buf + offset_in_sector) = SWAP32((value & 0xFFFFFFF) | ((SWAP32(*(u32*)(p_page->p_buf + offset_in_sector))) & 0xF0000000));
    if (err != 0) {
        first_err = err;
    }
    return first_err;
}

s32 VFiPFFAT32_ReadFATEntryPage(struct PF_VOLUME* p_vol, u32 cluster, u32* p_value, struct PF_CACHE_PAGE* p_page) {
    u32 offset;
    u32 sector;
    u32 current_fat;
    s32 err;
    s32 result;
    u16 i;

    offset = cluster << 2;
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
    *p_value = (u32)(SWAP32(*((u32*)(p_page->p_buf + offset)))) & 0x0FFFFFFF;
    return 0;
}
#endif
