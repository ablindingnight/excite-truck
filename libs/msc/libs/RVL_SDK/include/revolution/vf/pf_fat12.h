#ifndef RVL_SDK_VF_PF_FAT12_H
#define RVL_SDK_VF_PF_FAT12_H
#include <revolution/types.h>
#ifdef __cplusplus
extern "C" {
#endif

s32 VFiPFFAT12_ReadFATEntry(struct PF_VOLUME* p_vol, u16 cluster, u32* p_value);
#ifdef GAME_REXE01
s32 VFiPFFAT12_ReadFATEntryPage(struct PF_VOLUME* p_vol, u16 cluster, u32* p_value, struct PF_CACHE_PAGE* p_page);
#else
s32 VFiPFFAT12_ReadFATEntryPage(struct PF_VOLUME* p_vol, u16 cluster, u32* p_value, struct PF_CACHE_PAGE** pp_page);
#endif
s32 VFiPFFAT12_WriteFATEntry(struct PF_VOLUME* p_vol, u16 cluster, u16 value);
#ifdef GAME_REXE01
s32 VFiPFFAT12_WriteFATEntryPage(struct PF_VOLUME* p_vol, u16 cluster, u16 value, struct PF_CACHE_PAGE* p_page);
#else
s32 VFiPFFAT12_WriteFATEntryPage(struct PF_VOLUME* p_vol, u16 cluster, u16 value, struct PF_CACHE_PAGE** pp_page);
#endif

#ifdef __cplusplus
}
#endif
#endif
