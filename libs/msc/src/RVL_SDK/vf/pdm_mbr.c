#include <revolution/vf.h>

#ifdef GAME_REXE01
// ET's MBR check works on the sector buffer and the disk size; it has no table helpers.
s32 VFipdm_mbr_check_master_boot_record(u8* buf, u32 total_sectors, u32* is_master_boot) {
    u32 start_sector[4];
    u32 num_sectors[4];
    s16 part_cnt;

    *is_master_boot = 0;
    if (buf[0x1FE] != 0x55 || buf[0x1FF] != 0xAA) {
        return 2;
    }

    for (part_cnt = 0; part_cnt < 4; part_cnt++, buf += 16) {
        start_sector[part_cnt] = 0;
        start_sector[part_cnt] = buf[0x1C6] + (buf[0x1C7] << 8) + (buf[0x1C8] << 16) + (buf[0x1C9] << 24);
        num_sectors[part_cnt] = buf[0x1CA] + (buf[0x1CB] << 8) + (buf[0x1CC] << 16) + (buf[0x1CD] << 24);
        if (start_sector[part_cnt] != 0 && num_sectors[part_cnt] != 0) {
            if (part_cnt == 0) {
                *is_master_boot = 1;
            }
        } else if (part_cnt == 0) {
            return 2;
        }
    }

    for (part_cnt = 0; part_cnt < 4; part_cnt++) {
        if (start_sector[part_cnt] + num_sectors[part_cnt] > total_sectors) {
            if (part_cnt == 0) {
                *is_master_boot = 0;
            }
            return 2;
        }
    }
    return 0;
}

u32 VFipdm_ET_IsBootSector(u8* buf) {
    u32 is_boot;
    u16 byte_per_sector;
    u8 sector_per_cluster;
    u8 media;

    is_boot = 1;

    if (buf[0] != 0xEB || buf[2] != 0x90) {
        if (buf[0] != 0xE9) {
            is_boot = 0;
        }
    }

    if (buf[0x1FE] != 0x55 || buf[0x1FF] != 0xAA) {
        is_boot = 0;
    }

    byte_per_sector = PDM_LE16(buf, 0xB);
    if (byte_per_sector != 512 && byte_per_sector != 1024 && byte_per_sector != 2048 && byte_per_sector != 4096) {
        is_boot = 0;
    }

    sector_per_cluster = buf[0xD];
    if (sector_per_cluster != 1 && sector_per_cluster != 2 && sector_per_cluster != 4 && sector_per_cluster != 8 && sector_per_cluster != 16 && sector_per_cluster != 32 && sector_per_cluster != 64 && sector_per_cluster != 128) {
        is_boot = 0;
    }

    media = buf[0x15];
    if (media != 0xF0 && media != 0xF8 && media != 0xF9 && media != 0xFA && media != 0xFB && media != 0xFC && media != 0xFD && media != 0xFE && media != 0xFF) {
        is_boot = 0;
    }

    return is_boot;
}
#else
s32 VFipdm_mbr_get_table(u8* buf, u32 sector, struct PDM_MBR* p_mbr_tbl) {
    u8* p_buf;
    u16 cs_val;
    u32 i;

    if (!buf || !p_mbr_tbl) {
        return 1;
    }
    if (!sector) {
        p_mbr_tbl->epbr_base_sector = 0;
        p_mbr_tbl->current_sector = 0;
    } else {
        if (!p_mbr_tbl->epbr_base_sector) {
            p_mbr_tbl->epbr_base_sector = sector;
        }
        p_mbr_tbl->current_sector = sector;
    }

    p_buf = buf + 446;
    for (i = 0; i < 4; ++i) {
        p_mbr_tbl->partition_table[i].boot_flag = p_buf[0];
        p_mbr_tbl->partition_table[i].partition_type = p_buf[4];
        p_mbr_tbl->partition_table[i].s_head = p_buf[1];

        cs_val = p_buf[2] | (p_buf[3] << 8);
        p_mbr_tbl->partition_table[i].s_cylinder = ((cs_val << 2) & 0x300) + (cs_val >> 8);
        p_mbr_tbl->partition_table[i].s_sector = cs_val & 0x3F;

        p_mbr_tbl->partition_table[i].e_head = ((p_buf[6] << 8) & 0xFF00) | (p_buf[5] & 0xFFFF00FF);

        cs_val = p_buf[6] | (p_buf[7] << 8);
        p_mbr_tbl->partition_table[i].e_cylinder = ((cs_val << 2) & 0x300) + (cs_val >> 8);
        p_mbr_tbl->partition_table[i].e_sector = cs_val & 0x3F;

        p_mbr_tbl->partition_table[i].lba_start_sector = p_buf[8] | (p_buf[9] << 8) | (p_buf[10] << 16) | (p_buf[11] << 24);
        p_mbr_tbl->partition_table[i].lba_num_sectors = p_buf[12] | (p_buf[13] << 8) | (p_buf[14] << 16) | (p_buf[15] << 24);

        p_buf += 16;
    }

    return 0;
}

s32 VFipdm_mbr_get_mbr_part_table(struct PDM_DISK* p_disk, struct PDM_MBR* p_mbr_tbl) {
    s32 err;
    u8 buf[512];
    u32 num_success;
    u32 is_master_boot;

    if (!p_disk || !p_mbr_tbl) {
        return 1;
    }
    err = VFipdm_disk_check_disk_handle(p_disk);
    if (err) {
        return err;
    }

    err = VFipdm_disk_physical_read(p_disk, buf, 0, 1, 512, &num_success);
    if (err) {
        return err;
    }

    VFipdm_mbr_check_master_boot_record(p_disk, buf, &is_master_boot);
    if (is_master_boot) {
        VFipdm_mbr_get_table(buf, 0, p_mbr_tbl);
    } else {
        return 5;
    }

    return 0;
}

s32 VFipdm_mbr_get_epbr_part_table(struct PDM_DISK* p_disk, struct PDM_MBR* p_mbr_tbl) {
    s32 err;
    u8 buf[512];
    u32 extend_start_sector;
    u16 tbl_index;
    u32 num_success;
    u32 is_master_boot;
    struct PDM_DISK_INFO disk_info;
    u32 extend_start_block;

    if (!p_disk || !p_mbr_tbl) {
        return 1;
    }
    err = VFipdm_disk_check_disk_handle(p_disk);
    if (err) {
        return err;
    }
    extend_start_sector = 0;
    for (tbl_index = 0; tbl_index < 4u; ++tbl_index) {
        if (p_mbr_tbl->partition_table[tbl_index].partition_type == 5 || p_mbr_tbl->partition_table[tbl_index].partition_type == 15) {
            extend_start_sector = p_mbr_tbl->epbr_base_sector + p_mbr_tbl->partition_table[tbl_index].lba_start_sector;
            break;
        }
    }
    if (tbl_index == 4) {
        return 7;
    } else {
        err = VFipdm_disk_get_media_information(p_disk, &disk_info);
        if (err) {
            return err;
        }
        extend_start_block = extend_start_sector * (disk_info.bytes_per_sector >> 9);
        err = VFipdm_disk_physical_read(p_disk, buf, extend_start_block, 1, 512, &num_success);
        if (err) {
            return err;
        }
        VFipdm_mbr_check_master_boot_record(p_disk, buf, &is_master_boot);
        if (is_master_boot) {
            VFipdm_mbr_get_table(buf, extend_start_sector, p_mbr_tbl);
        } else {
            return 6;
        }
        return 0;
    }
}

s32 VFipdm_mbr_check_master_boot_record(struct PDM_DISK* p_disk, u8* buf, u32* is_master_boot) {
    s16 part_cnt;
    struct PDM_MBR_SEC* p_mbr_sec;
    struct PDM_MBR mbr_tbl;
    struct PDM_DISK* lp_disk;
    u32 is_boot;
    s32 err;

    if (!p_disk || !buf || !is_master_boot) {
        return 1;
    }
    err = VFipdm_disk_check_disk_handle(p_disk);
    if (err) {
        return err;
    }
    *is_master_boot = 0;
    p_mbr_sec = (struct PDM_MBR_SEC*)buf;
    if (p_mbr_sec->signature1 != 0x55 || p_mbr_sec->signature2 != 0xaa) {
        return 0;
    }
    lp_disk = VFipdm_disk_set.disk;
    VFipdm_mbr_get_table(buf, 0, &mbr_tbl);
    for (part_cnt = 0; part_cnt < 4; ++part_cnt) {
        if (mbr_tbl.partition_table[part_cnt].lba_num_sectors > (u32)(-1) - mbr_tbl.partition_table[part_cnt].lba_start_sector || mbr_tbl.partition_table[part_cnt].lba_start_sector + mbr_tbl.partition_table[part_cnt].lba_num_sectors > lp_disk[(u8)(u32)p_disk].disk_info.total_sectors) {
            *is_master_boot = 0;
            break;
        }
        if (mbr_tbl.partition_table[part_cnt].lba_start_sector) {
            *is_master_boot = 1;
        }
    }
    if (!*is_master_boot && !mbr_tbl.partition_table[0].lba_start_sector && !mbr_tbl.partition_table[1].lba_start_sector && !mbr_tbl.partition_table[2].lba_start_sector && !mbr_tbl.partition_table[3].lba_start_sector) {
        VFipdm_bpb_check_boot_sector(buf, &is_boot);
        if (!is_boot) {
            *is_master_boot = 1;
        } else {
            *is_master_boot = 0;
        }
    }
    return 0;
}
#endif
