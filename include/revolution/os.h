#ifndef RVL_SDK_OS_H
#define RVL_SDK_OS_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int OSHeapHandle;

#define OSRoundUp32B(x) (((u32)(x) + 32 - 1) & ~(32 - 1))
#define OSRoundDown32B(x) (((u32)(x)) & ~(32 - 1))

void OSReport(const char* msg, ...);

void* OSGetArenaHi(void);
void* OSGetArenaLo(void);
void OSSetArenaHi(void* newHi);
void OSSetArenaLo(void* newLo);

extern volatile OSHeapHandle __OSCurrHeap;

void* OSInitAlloc(void* arenaStart, void* arenaEnd, int maxHeaps);
OSHeapHandle OSCreateHeap(void* start, void* end);
OSHeapHandle OSSetCurrentHeap(OSHeapHandle heap);
void* OSAllocFromHeap(OSHeapHandle heap, u32 size);
void OSFreeToHeap(OSHeapHandle heap, void* ptr);

#ifdef __cplusplus
}
#endif

#endif
