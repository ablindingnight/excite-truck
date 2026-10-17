#ifndef GAME_COMMON_H
#define GAME_COMMON_H
// Things every Excite Truck / Monster Games engine unit seems to share. Each unit's .sbss starts with
// an 8-byte object whose inline constructor links it into a global list, which
// suggests a header-level static; the real names are unknown.
#include <types.h>

class ModuleTag;
void RegisterModuleTag(ModuleTag* tag);

class ModuleTag {
public:
    ModuleTag(void) { RegisterModuleTag(this); }

    u8 flag;          // offset 0x0, set to '1'
    ModuleTag* next;  // offset 0x4
};

// Static objects some units define next to their tag, built by inline constructors.
class UnkStaticFFFF {
public:
    UnkStaticFFFF(void) { value = 0xFFFF0000; }
    u32 value;
};

class UnkStatic7F {
public:
    UnkStatic7F(void) { value = 0x7F000000; }
    u32 value;
};

#endif
