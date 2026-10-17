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

// Static object built by an inline constructor storing (0, 123, 0, 0, 0).
class UnkStatic123 {
public:
    UnkStatic123(void) {
        unk4 = 123;
        unk0 = 0;
        unk8 = 0;
        unkC = 0;
        unk10 = 0;
    }
    u32 unk0;
    u32 unk4;
    u32 unk8;
    u32 unkC;
    u32 unk10;
    u32 unk14;
};

class ValueNode;

// A parsed .val file (root): its entries are ValueNodes. Slot names are guesses.
class ValueTable {
public:
    virtual ValueNode* FindChild(const char* name);  // 0x8
    virtual ValueNode* GetChild(int index);          // 0xC
    virtual int HasChild(const char* name);          // 0x10
};

// A node inside a .val table (an entry's key/value pairs).
class ValueNode {
public:
    virtual const char* GetName(void);                                // 0x8
    virtual int Has(const char* key);                                 // 0xC
    virtual void vf2(void);                                           // 0x10
    virtual const char* GetValue(const char* key);                    // 0x14
    virtual const char* GetString(const char* key, const char* def);  // 0x18
    virtual s32 GetIntValue(const char* key);                         // 0x1C
    virtual s32 GetInt(const char* key, s32 def);                     // 0x20
};

#endif
