#include <revolution/vf/pf_filelock.h>

static int VFipf_sys_set = 0;

#ifdef GAME_REXE01
void VFiPF_InitCritical(struct PF_CRITICAL* p_critical) {
    struct PF_ET_SEMAPHORE_PARAM param;

    if (p_critical->initialized == 0) {
        param.initial_count = 1;
        param.max_count = 1;
        param.p_buf = p_critical->sem_buf;
        p_critical->sem_id = VFiPF_ET_CreateSemaphore(&param);
        p_critical->count = 0;
        p_critical->owner = 0;
        p_critical->initialized = 1;
    }
}

void VFiPF_DeleteCritical(struct PF_CRITICAL* p_critical) {
    if ((p_critical->initialized & 1) != 0) {
        VFiPF_ET_DeleteSemaphore(p_critical->sem_id);
        p_critical->initialized = 0;
    }
}

// Recursive: the owning task may enter again without waiting.
void VFiPF_LockCritical(struct PF_CRITICAL* p_critical) {
    u32 task_id;

    if (p_critical->count == 0) {
        VFiPF_ET_WaitSemaphore(p_critical->sem_id);
        VFiPF_ET_GetTaskID(&p_critical->owner);
    } else {
        VFiPF_ET_GetTaskID(&task_id);
        if (p_critical->owner != task_id) {
            VFiPF_ET_WaitSemaphore(p_critical->sem_id);
            p_critical->owner = task_id;
        }
    }
    p_critical->count++;
}

void VFiPF_UnlockCritical(struct PF_CRITICAL* p_critical) {
    if (--p_critical->count == 0) {
        p_critical->owner = 0;
        VFiPF_ET_SignalSemaphore(p_critical->sem_id);
    }
}
#endif

void VFiPF_InitLockFile(void) {
    VFipf_sys_set = 0;
}

s32 VFiPF_UnLockFile(struct PF_FILE* p_file) {
#ifdef GAME_REXE01
    VFiPF_ET_SignalSemaphore(p_file->p_sfd->lock.resource);
    if (p_file->p_sfd->lock.count == 0 && p_file->p_sfd->lock.wcount == 0 && VFipf_sys_set != 0) {
        VFiPF_ET_DeleteSemaphore(p_file->p_sfd->lock.resource);
        p_file->p_sfd->lock.resource = 0;
        VFipf_sys_set--;
    }
    return 0;
#else
    return -1;
#endif
}
