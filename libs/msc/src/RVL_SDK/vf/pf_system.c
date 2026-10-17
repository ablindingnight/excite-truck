#include <revolution/vf/vf_struct.h>
#include <revolution/vf/pf_system.h>
#include <revolution/vf/d_time.h>
#ifdef GAME_REXE01
#include <revolution/os.h>
#include <revolution/vf/pf_filelock.h>
#endif

void VFiPFSYS_initializeSYS() {
    return;
}

#ifdef GAME_REXE01
// ET uses the system layer's task id as the context id.
s32 VFiPFSYS_GetCurrentContextID(s32* context_id) {
    return VFiPF_ET_GetTaskID((u32*)context_id);
}
#else
s32 VFiPFSYS_GetCurrentContextID(s32* context_id) {
    s32 err;

    err = 0;
    *context_id = 0;
    return err;
}
#endif

void VFiPFSYS_TimeStamp(struct PF_SYS_DATE* sdate, struct PF_SYS_TIME* stime) {
#ifdef GAME_REXE01
    OSCalendarTime td;

    OSTicksToCalendarTime(OSGetTime(), &td);
    sdate->sys_year = td.year;
    sdate->sys_month = td.month + 1;
    sdate->sys_day = td.mday;
    stime->sys_hour = td.hour;
    stime->sys_min = td.min;
    stime->sys_sec = td.sec;
#else
    sdate->sys_year = 2000;
    sdate->sys_month = 4;
    sdate->sys_day = 1;
    stime->sys_hour = 0;
    stime->sys_min = 0;
    stime->sys_sec = 0;
    stime->sys_ms = 1;
    dTM_GetNowTime(sdate, stime);
    stime->sys_ms = 1;
#endif
}
