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

// PrFILE2 kernel layer (pfk.c, 0x800B7EAC-0x800B808C); names from its error messages.
typedef struct PF_ET_SEMAPHORE_PARAM {
    u32 initial_count;
    u32 max_count;
    void* p_buf;
} PF_ET_SEMAPHORE_PARAM;

s32 pfk_get_task_id(u32* p_task_id);
s32 pfk_create_semaphore(struct PF_ET_SEMAPHORE_PARAM* p_param);
s32 pfk_delete_semaphore(s32 sem_id);
s32 pfk_get_semaphore(s32 sem_id);
s32 pfk_release_semaphore(s32 sem_id);
#endif

#ifdef __cplusplus
}
#endif
#endif
