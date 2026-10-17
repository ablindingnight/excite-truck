#ifndef RVL_SDK_VF_PF_DIR_H
#define RVL_SDK_VF_PF_DIR_H
#include <revolution/types.h>
#ifdef __cplusplus
extern "C" {
#endif

void VFiPFDIR_FinalizeAllDirs(struct PF_VOLUME* p_vol);

#ifdef GAME_REXE01
typedef struct PF_DTA {
    // total size: 0x444
    struct PF_FILE* p_file;      // offset 0x0, size 0x4
    struct PF_DIR* p_dir;        // offset 0x4, size 0x4
    struct PF_VOLUME* p_vol;     // offset 0x8, size 0x4
    u32 parent_start_cluster;    // offset 0xC, size 0x4
    u32 parent_pos;              // offset 0x10, size 0x4
    u32 status;                  // offset 0x14, size 0x4
    u8 num_entry_LFNs;           // offset 0x18, size 0x1
    u8 ordinal;                  // offset 0x19, size 0x1
    u8 check_sum;                // offset 0x1A, size 0x1
    u8 attr;                     // offset 0x1B, size 0x1
    s8 reg_exp[520];             // offset 0x1C, size 0x208
    u16 Time;                    // offset 0x224, size 0x2
    u16 Date;                    // offset 0x226, size 0x2
    u32 FileSize;                // offset 0x228, size 0x4
    u8 Attribute;                // offset 0x22C, size 0x1
    s8 FileName[13];             // offset 0x22D, size 0xD
    s8 LongName[520];            // offset 0x23A, size 0x208
} PF_DTA;

typedef struct PF_DTAW {
    // total size: 0x866
    struct PF_FILE* p_file;      // offset 0x0, size 0x4
    struct PF_DIR* p_dir;        // offset 0x4, size 0x4
    struct PF_VOLUME* p_vol;     // offset 0x8, size 0x4
    u32 parent_start_cluster;    // offset 0xC, size 0x4
    u32 parent_pos;              // offset 0x10, size 0x4
    u32 status;                  // offset 0x14, size 0x4
    u8 num_entry_LFNs;           // offset 0x18, size 0x1
    u8 ordinal;                  // offset 0x19, size 0x1
    u8 check_sum;                // offset 0x1A, size 0x1
    u8 attr;                     // offset 0x1B, size 0x1
    s8 reg_exp[520];             // offset 0x1C, size 0x208
    u16 Time;                    // offset 0x224, size 0x2
    u16 Date;                    // offset 0x226, size 0x2
    u32 FileSize;                // offset 0x228, size 0x4
    u8 Attribute;                // offset 0x22C, size 0x1
    s8 FileName[13];             // offset 0x22D, size 0xD
    s8 LongName[522];            // offset 0x23A, size 0x20A
    u16 reg_expW[260];           // offset 0x444, size 0x208
    u16 FileNameW[13];           // offset 0x64C, size 0x1A
    u16 LongNameW[256];          // offset 0x666, size 0x200
} PF_DTAW;

s32 VFiPFDIR_fsfirst(struct PF_STR* p_path_str, u8 attr, struct PF_DTA* p_dta);
s32 VFiPFDIR_fsnext(struct PF_DTA* p_dta);
#endif

#ifdef __cplusplus
}
#endif
#endif
