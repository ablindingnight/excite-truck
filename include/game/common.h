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

// The 20-byte form of the (0, 123, 0, 0, 0) static (no trailing word), e.g. menu item terminators.
class UnkStatic123s {
public:
    UnkStatic123s(void) {
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
};

class ValueNode;

// A parsed .val file (root): its entries are ValueNodes. Slot names are guesses.
class ValueTable {
public:
    virtual ValueNode* FindChild(const char* name);  // 0x8
    virtual ValueNode* GetChild(int index);          // 0xC
    virtual int HasChild(const char* name);          // 0x10
};

struct ValVec3 {
    f32 x, y, z;
};

struct ValPoint {
    f32 x, y;
};

struct ValMatrix {
    ValMatrix(void) {}
    ValVec3 basis[3];  // basis_x, basis_y, basis_z
    ValVec3 origin;
};

// A node inside a .val table (an entry's key/value pairs).
class ValueNode {
public:
    virtual const char* GetName(void);                                // 0x8
    virtual int Has(const char* key);                                 // 0xC
    virtual int GetPair(int index, const char** key, const char** value);  // 0x10
    virtual const char* GetValue(const char* key);                    // 0x14
    virtual const char* GetString(const char* key, const char* def);  // 0x18
    virtual s32 GetIntValue(const char* key);                         // 0x1C
    virtual s32 GetInt(const char* key, s32 def);                     // 0x20
    virtual f32 GetFloat(const char* key);                            // 0x24
    virtual f32 GetFloat(const char* key, f32 def) { return Has(key) ? GetFloat(key) : def; }  // 0x28
    virtual s32 GetIntValue2(const char* key);                        // 0x2C (bool: "yes"/"true")
    virtual int GetBool(const char* key, int def);                    // 0x30
    virtual ValVec3 GetVec3(const char* key);                         // 0x34
    virtual ValVec3 GetVec3Or(const char* key, const ValVec3& def);     // 0x38
    virtual ValPoint GetPoint(const char* key);                       // 0x3C (returned in r3:r4)
    virtual ValPoint GetPointOr(const char* key, const ValPoint& def);  // 0x40
    virtual ValMatrix GetMatrix(const char* key);                     // 0x44
    virtual ValMatrix GetMatrixOr(const char* key, const ValMatrix& def);  // 0x48
    virtual f32 GetReal(const char* key);                             // 0x4C (base64 float)
    virtual f32 GetRealOr(const char* key, f32 def);                    // 0x50
    virtual int GetEnum(const char* key, const char* names);          // 0x54
    virtual f32 GetScale(const char* key, f32 t);                     // 0x58
    virtual ValueNode* FindChild(const char* name);                   // 0x5C
    virtual ValueNode* GetChildAt(int index);                         // 0x60
    virtual u32 Checksum(void) { return 0; }                          // 0x64
};

#endif
