#ifndef RVL_SDK_VF_PF_FILELOCK_H
#define RVL_SDK_VF_PF_FILELOCK_H
#include <revolution/types.h>
#include <revolution/vf/vf_struct.h>
#ifdef __cplusplus
extern "C" {
#endif

void VFiPF_InitLockFile(void);
s32 VFiPF_UnLockFile(struct PF_FILE* p_file);

#ifdef GAME_REXE01
// Excite Truck's older PrFILE2 guards volumes with recursive critical sections (in pf_filelock.c).
// The original names of these functions are not known.
void VFiPF_InitCritical(struct PF_CRITICAL* p_critical);
void VFiPF_DeleteCritical(struct PF_CRITICAL* p_critical);
void VFiPF_LockCritical(struct PF_CRITICAL* p_critical);
void VFiPF_UnlockCritical(struct PF_CRITICAL* p_critical);
#endif

#ifdef __cplusplus
}
#endif
#endif
