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
    virtual void vf9(void);                                           // 0x24
    virtual void vf10(void);                                          // 0x28
    virtual void vf11(void);                                          // 0x2C
    virtual int GetBool(const char* key, int def);                    // 0x30
    virtual void vf13(void);                                          // 0x34
    virtual void vf14(void);                                          // 0x38
    virtual void vf15(void);                                          // 0x3C
    virtual void vf16(void);                                          // 0x40
    virtual void vf17(void);                                          // 0x44
    virtual void vf18(void);                                          // 0x48
    virtual void vf19(void);                                          // 0x4C
    virtual void vf20(void);                                          // 0x50
    virtual void vf21(void);                                          // 0x54
    virtual void vf22(void);                                          // 0x58
    virtual void vf23(void);                                          // 0x5C
    virtual ValueNode* GetChildAt(int index);                         // 0x60
};

#endif
