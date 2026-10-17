#ifndef RVL_SDK_AX_H
#define RVL_SDK_AX_H
#include <revolution/types.h>
#ifdef __cplusplus
extern "C" {
#endif

void AXInit(void);
#ifdef GAME_REXE01
void AXQuit(void);
#endif
BOOL AXIsInit(void);
void AXInitEx(u32 mode);

#ifdef __cplusplus
}
#endif
#endif
