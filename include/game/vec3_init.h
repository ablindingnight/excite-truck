#ifndef GAME_VEC3_INIT_H
#define GAME_VEC3_INIT_H

// The retail vector initializer leaves all three components uninitialized.
void DefaultConstructVec3(void* storage);  /* Helper name unproven: 0x8001BA68, a 4-byte blr shared by BVec/WPt/TVec ctors */

#endif
