// PrFILE2's kernel glue on Revolution OS threads and semaphores (ET only; the file name is a guess,
// the function names come from its messages).
#include <revolution/vf.h>
#include <revolution/os.h>
#include <revolution/os/OSSemaphore.h>

s32 pfk_get_task_id(u32* p_task_id) {
    OSThread* thread;

    if (p_task_id == NULL) {
        OSReport("prfile2:ERR pfk_get_task_id() invalid param\n");
        return -2;
    }

    thread = OSGetCurrentThread();
    if (thread == NULL) {
        OSReport("INFO failed to OSGetCurrentThread\n");
        *p_task_id = 0;
        return -3;
    }

    *p_task_id = (u32)thread;
    return 0;
}

s32 pfk_create_semaphore(struct PF_ET_SEMAPHORE_PARAM* p_param) {
    s32 num_wait;

    if (p_param == NULL || p_param->p_buf == NULL || p_param->initial_count > p_param->max_count) {
        OSReport("prfile2:ERR pfk_create_semaphore() Invalid Param\n");
        return 0;
    }

    OSInitSemaphore((OSSemaphore*)p_param->p_buf, p_param->max_count);
    for (num_wait = p_param->max_count - p_param->initial_count; num_wait != 0; num_wait--) {
        OSWaitSemaphore((OSSemaphore*)p_param->p_buf);
    }
    return (s32)p_param->p_buf;
}

s32 pfk_delete_semaphore(s32 sem_id) {
    if (sem_id == 0) {
        OSReport("prfile2:ERR pfk_delete_semaphore() Invalid Param\n");
        return -2;
    }
    return 0;
}

s32 pfk_get_semaphore(s32 sem_id) {
    if (sem_id == 0) {
        OSReport("prfile2:ERR pfk_get_semaphore() Invalid Param\n");
        return -2;
    }
    OSWaitSemaphore((OSSemaphore*)sem_id);
    return 0;
}

s32 pfk_release_semaphore(s32 sem_id) {
    if (sem_id == 0) {
        OSReport("prfile2:ERR pfk_release_semaphore() Invalid Param\n");
        return -2;
    }
    OSSignalSemaphore((OSSemaphore*)sem_id);
    return 0;
}
