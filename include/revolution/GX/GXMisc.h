#ifndef RVL_SDK_GX_MISC_H
#define RVL_SDK_GX_MISC_H
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef void (*GXDrawDoneCallback)(void);
typedef void (*GXDrawSyncCallback)(u16 token);

void GXSetMisc(UNKWORD token, UNKWORD val);
void GXFlush(void);
void GXResetWriteGatherPipe(void);

void GXAbortFrame(void);

void GXSetDrawSync(u16 token);

void GXDrawDone(void);
void GXPixModeSync(void);

GXDrawSyncCallback GXSetDrawSyncCallback(GXDrawSyncCallback);
GXDrawDoneCallback GXSetDrawDoneCallback(GXDrawDoneCallback);

void GXSetDrawDone(void);
void GXPeekZ(u16 x, u16 y, u32* z);
void GXReadXfRasMetric(u32* xfWaitIn, u32* xfWaitOut, u32* rasBusy, u32* clocks);

#ifdef __cplusplus
}
#endif
#endif
