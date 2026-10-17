#include <revolution/vf/vf_struct.h>

#include <revolution/vf/pf_dir.h>
#include <revolution/vf/pf_fat.h>
#ifdef GAME_REXE01
#include <revolution/vf.h>
#endif

static void VFiPFDIR_FinalizeSDD(struct PF_SDD* p_sdd) {
    p_sdd->stat = 0;
    VFiPFFAT_FinalizeFFD(&p_sdd->ffd);
}

static void VFiPFDIR_FinalizeUDD(struct PF_DIR* p_dir) {
    p_dir->stat &= ~1u;
}

#ifdef GAME_REXE01
static s32 VFiPFDIR_p_fsnext(struct PF_DTA* p_dta) {
    s32 err;
    struct PF_FFD ffd;
    struct PF_FAT_HINT fat_hint;
    struct PF_DIR_ENT ent;
    u32 lpos = 0;
    u32 ppos = 0;
    struct PF_STR pattern;
    u32 code_mode;
    s32 last_file_error;
    s32 i;

    i = 0;
    last_file_error = 0;
    if ((p_dta->status & 1) == 1) {
        code_mode = 1;
        err = VFiPFSTR_InitStr(&pattern, p_dta->reg_exp, code_mode);
    } else {
        code_mode = 2;
        err = VFiPFSTR_InitStr(&pattern, (s8*)((struct PF_DTAW*)p_dta)->reg_expW, code_mode);
    }
    if (err != 0) {
        return err;
    }
    VFiPFSTR_SetLocalStr(&pattern, p_dta->reg_exp);

    while (ppos < 0xF423F) {
        if (p_dta->parent_pos >= 0xF423F) {
            return 10;
        }
        ent.start_cluster = p_dta->parent_start_cluster;
        VFiPFFAT_InitFFD(&ffd, &fat_hint, p_dta->p_vol, &ent.start_cluster);
        last_file_error = VFiPFENT_findEntryPos(&ffd, &ent, p_dta->parent_pos, &pattern, p_dta->attr, 0, &lpos, &ppos);
        if (last_file_error != 0) {
            return last_file_error;
        }
        p_dta->parent_pos = ppos + 1;
        if (ppos == 0xF423F) {
            return 3;
        }
        break;
    }

    VFipf_strcpy(p_dta->FileName, ent.short_name);
    if ((p_dta->status & 2) == 2) {
        VFiPFPATH_transformInUnicode(((struct PF_DTAW*)p_dta)->FileNameW, p_dta->FileName);
    }
    if (ent.long_name[0] != 0) {
        if ((VFipf_vol_set.setting & 2) == 2) {
            VFipf_vol_set.setting &= ~3;
            VFipf_vol_set.setting |= 1;
            VFiPFPATH_transformFromUnicodeToNormal(p_dta->LongName, ent.long_name);
            VFipf_vol_set.setting &= ~3;
            VFipf_vol_set.setting |= 2;
        } else {
            VFiPFPATH_transformFromUnicodeToNormal(p_dta->LongName, ent.long_name);
        }
        if ((p_dta->status & 2) == 2) {
            VFipf_w_strcpy(((struct PF_DTAW*)p_dta)->LongNameW, ent.long_name);
        }
    } else {
        p_dta->LongName[0] = 0;
        if ((p_dta->status & 2) == 2) {
            ((struct PF_DTAW*)p_dta)->LongNameW[0] = 0;
        }
    }

    // an open file's entry is more current than the one on the medium
    if (p_dta->p_vol->num_opened_files != 0) {
        for (i = 0; i < 5; i++) {
            if ((p_dta->p_vol->sfds[i].stat & 1) && (p_dta->p_vol->sfds[i].stat & 2) &&
                ent.p_vol == p_dta->p_vol->sfds[i].dir_entry.p_vol &&
                p_dta->p_vol->sfds[i].dir_entry.entry_sector == ent.entry_sector &&
                p_dta->p_vol->sfds[i].dir_entry.entry_offset == ent.entry_offset) {
                p_dta->FileSize = p_dta->p_vol->sfds[i].dir_entry.file_size;
                p_dta->Time = p_dta->p_vol->sfds[i].dir_entry.modify_time;
                p_dta->Date = p_dta->p_vol->sfds[i].dir_entry.modify_date;
                p_dta->Attribute = p_dta->p_vol->sfds[i].dir_entry.attr;
                p_dta->num_entry_LFNs = p_dta->p_vol->sfds[i].dir_entry.num_entry_LFNs;
                p_dta->ordinal = p_dta->p_vol->sfds[i].dir_entry.ordinal;
                p_dta->check_sum = p_dta->p_vol->sfds[i].dir_entry.check_sum;
                break;
            }
        }
    }
    if (p_dta->p_vol->num_opened_files == 0 || i == 5) {
        p_dta->FileSize = ent.file_size;
        p_dta->Time = ent.modify_time;
        p_dta->Date = ent.modify_date;
        p_dta->Attribute = ent.attr;
        p_dta->num_entry_LFNs = ent.num_entry_LFNs;
        p_dta->ordinal = ent.ordinal;
        p_dta->check_sum = ent.check_sum;
    }

    return 0;
}

static s32 VFiPFDIR_p_fsfirst(struct PF_VOLUME* p_vol, struct PF_STR* p_path_str, u8 attr, struct PF_DTA* p_dta) {
    struct PF_STR sDir;
    struct PF_STR sPattern;
    struct PF_DIR_ENT ent;
    struct PF_ENT_ITER iter;
    s32 err;

    err = VFiPFPATH_SplitPathPattern(p_path_str, &sDir, &sPattern);
    if (err != 0) {
        return err;
    }
    err = VFiPFENT_ITER_GetEntryOfPattern(&iter, &ent, p_vol, &sDir);
    if (err != 0) {
        return err;
    }
    if ((ent.attr & 0x10) == 0) {
        return 0x14;
    }
    p_dta->parent_start_cluster = ent.start_cluster;
    p_dta->parent_pos = 0;
    p_dta->p_vol = ent.p_vol;
    p_dta->attr = attr;
    p_dta->status = 0;
    if (VFiPFSTR_GetCodeMode(&sPattern) == 1) {
        VFiPFPATH_SetSearchPattern(p_dta->reg_exp, NULL, &sPattern);
        p_dta->status |= 1;
    } else {
        VFiPFPATH_SetSearchPattern(p_dta->reg_exp, ((struct PF_DTAW*)p_dta)->reg_expW, &sPattern);
        p_dta->status |= 2;
    }

    return VFiPFDIR_p_fsnext(p_dta);
}
#endif

void VFiPFDIR_FinalizeAllDirs(struct PF_VOLUME* p_vol) {
    u16 i;

    for (i = 0; i < 3u; ++i) {
        VFiPFDIR_FinalizeSDD(&p_vol->sdds[i]);
    }
    for (i = 0; i < 3u; ++i) {
        VFiPFDIR_FinalizeUDD(&p_vol->udds[i]);
    }
    p_vol->num_opened_directories = 0;
}

#ifdef GAME_REXE01
s32 VFiPFDIR_fsfirst(struct PF_STR* p_path_str, u8 attr, struct PF_DTA* p_dta) {
    struct PF_VOLUME* p_vol;
    s32 err;

    p_vol = VFiPFPATH_GetVolumeFromPath(p_path_str);
    VFiPF_LockCritical(&p_vol->critical);

    err = VFiPFVOL_CheckForRead(p_vol);
    if (err != 0) {
        VFiPF_UnlockCritical(&p_vol->critical);
        VFipf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }

    err = VFiPFDIR_p_fsfirst(p_vol, p_path_str, attr, p_dta);
    if (err != 0) {
        p_dta->p_vol = NULL;
        VFipf_vol_set.last_error = err;
        p_vol->last_error = err;
    }

    VFiPF_UnlockCritical(&p_vol->critical);
    return err;
}

s32 VFiPFDIR_fsnext(struct PF_DTA* p_dta) {
    struct PF_VOLUME* p_vol;
    s32 err;

    p_vol = p_dta->p_vol;
    VFiPF_LockCritical(&p_vol->critical);

    err = VFiPFVOL_CheckForRead(p_vol);
    if (err != 0) {
        VFiPF_UnlockCritical(&p_vol->critical);
        VFipf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }

    err = VFiPFDIR_p_fsnext(p_dta);
    if (err != 0) {
        VFipf_vol_set.last_error = err;
        p_vol->last_error = err;
    }

    VFiPF_UnlockCritical(&p_vol->critical);
    return err;
}
#endif
