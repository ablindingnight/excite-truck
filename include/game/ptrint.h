#ifndef GAME_PTRINT_H
#define GAME_PTRINT_H

// PtrInt: an integer that holds a pointer. Retail code kept some pointers in s32 variables and converts them at each
// use; that conversion shapes MWCC's register choice, so retyping them as pointers changes the code. PtrInt keeps
// the Wii form (s32: code-identical) and is pointer-sized on the PC (a 64-bit pointer does not fit in an s32).
// Port hazard ptr2int, 2026-11-16 (ObstaclesVtable). Cast through PtrInt, never through s32/u32, when such a
// variable stores or compares a pointer.
#ifdef __MWERKS__
#include "types.h"
typedef s32 PtrInt;
typedef u32 UPtrInt;  // the unsigned form (pointer compares written as (u32)p == (u32)q in retail)
#else
#include <stdint.h>
typedef intptr_t PtrInt;
typedef uintptr_t UPtrInt;
#endif

#endif
