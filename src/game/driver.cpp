// Drivers and vehicles (0x80016354-0x8001786C): the DriverInfoList / VehicleInfoList built from
// dirt.val, the four player slots, and the per-profile vehicle body colours registered with the
// ident system as 'Drvr'. Names are guesses except where the messages give them.
#include <string.h>
#include <ctype.h>
#include "game/common.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
int sscanf(const char* s, const char* fmt, ...);
char* strrchr(const char* s, int c);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);  // snprintf
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* table);
const char* fn_80131234(const char* key);  // localize
void fn_8013C804(void);
void fn_8013C8B8(void);
s32 fn_8013C8E8(const char* chassis);
}

void* operator new[](size_t size, const char* tag);
typedef void (*IdentFieldClear)(void);
void IdentRegisterField(s32 a, s32 b, void** ptr, u32 size, IdentFieldClear clear);

#define MAX_DRIVERS 50
#define MAX_VEHICLES 50
#define MAX_PLAYERS 4
#define MAX_BODY_COLORS 12
#define PLAYER_ID -1000
#define DEFAULT_COLOR 0xFF008040

#define VEHICLE_TRAILER 1
#define VEHICLE_EXTREME 2
#define VEHICLE_MAGICAL 4
#define VEHICLE_NO_AI 8

struct DriverInfo {
    s32 index;        // offset 0x0
    s32 id;           // offset 0x4, "number"
    char name[0x40];  // offset 0x8
    s32 rank;         // offset 0x48
    s32 vehicle;      // offset 0x4C, vehicle id, then its index
    u32 color;        // offset 0x50
    s32 unk54;        // offset 0x54
    ValueNode* node;  // offset 0x58
};

struct VehicleInfo {
    s32 index;                     // offset 0x0
    s32 id;                        // offset 0x4
    s32 base;                      // offset 0x8, id of the non-extreme version
    char display_name[0x20];       // offset 0xC
    char chassis[0x20];            // offset 0x2C
    char texture[0x10];            // offset 0x4C
    char number[8];                // offset 0x5C
    u32 colors[MAX_BODY_COLORS];   // offset 0x64
    s32 default_color;             // offset 0x94
    s32 num_colors;                // offset 0x98
    s32 unk9C;                     // offset 0x9C
    u32 flags;                     // offset 0xA0
    u32 groups;                    // offset 0xA4
};

struct VehicleColor {
    s32 body;  // offset 0x0
    s32 trim;  // offset 0x4
    u8 unk8;   // offset 0x8
};

struct LookVec {
    f32 x;
    f32 y;
    f32 z;

    LookVec& operator=(const LookVec& o) {
        x = o.x;
        y = o.y;
        z = o.z;
        return *this;
    }
};

struct PlayerLook {
    u8 unk0;        // offset 0x0
    LookVec unk4;   // offset 0x4
    LookVec unk10;  // offset 0x10
    LookVec unk1C;  // offset 0x1C
    LookVec unk28;  // offset 0x28
    LookVec unk34;  // offset 0x34
    LookVec unk40;  // offset 0x40
    LookVec unk4C;  // offset 0x4C
    LookVec unk58;  // offset 0x58
};

struct PlayerLookSlot {
    u8 valid;         // offset 0x0
    PlayerLook look;  // offset 0x4
};

struct PlayerUnk8 {
    u8 valid;
    u8 pad[7];
};

extern "C" void fn_8027953C(PlayerLook* look);

#define COPY_STRING(dst_, src_, size_)                  \
    {                                                   \
        const char* src = (src_);                       \
        int n = (size_);                                \
        char* dst = (dst_);                             \
        while ((*dst++ = *src++) != '\0') {             \
            if (--n <= 0) {                             \
                dst[-1] = '\0';                         \
                break;                                  \
            }                                           \
        }                                               \
    }

DriverInfo lbl_803577F0[MAX_PLAYERS];
VehicleInfo lbl_80357960[MAX_PLAYERS];
PlayerUnk8 lbl_80357C00[MAX_PLAYERS];
PlayerLookSlot lbl_80357C20[MAX_PLAYERS];

ModuleTag lbl_8055D530;
ValueTable* lbl_8055D538;
ValueTable* lbl_8055D53C;
DriverInfo* lbl_8055D540;
VehicleInfo* lbl_8055D544;
VehicleColor* lbl_8055D548;
s32 lbl_8055D54C;
s32 lbl_8055D550;
s32 lbl_8055D554;

void LoadDriverLists(void);
void ResetPlayer(int slot);
void ResetVehicleColors(void);
void CheckDuplicateNumbers(void);
void LoadDriverList(ValueTable* table);
void LoadDriver(ValueNode* node);
void LoadVehicle(ValueNode* node);
void PickVehicleColor(VehicleInfo* v, int slot, VehicleColor* out);

// Stripped from ET; only its strings remain.
void GetDriverResPath(char* buf, u32 size) {
    fn_801345D8(buf, size, "*ct.res/");
}

void DriverInit(void) {
    int i;

    lbl_8055D538 = fn_80131808("dirt.val");
    lbl_8055D540 = new ("DriverInfoList") DriverInfo[MAX_DRIVERS];
    lbl_8055D544 = new ("VehicleInfoList") VehicleInfo[MAX_VEHICLES];
    LoadDriverLists();
    lbl_8055D554 = 0;
    for (i = 0; i < MAX_PLAYERS; i++) {
        ResetPlayer(i);
    }
    IdentRegisterField('Drvr', 1, (void**)&lbl_8055D548, sizeof(VehicleColor) * MAX_PLAYERS * MAX_VEHICLES,
                       ResetVehicleColors);
}

void DriverShutdown(void) {
    lbl_8055D550 = 0;
    delete[] lbl_8055D544;
    lbl_8055D544 = NULL;
    lbl_8055D54C = 0;
    delete[] lbl_8055D540;
    lbl_8055D540 = NULL;
    if (lbl_8055D538 != NULL) {
        fn_80131818(lbl_8055D538);
        lbl_8055D538 = NULL;
    }
    if (lbl_8055D53C != NULL) {
        fn_80131818(lbl_8055D53C);
        lbl_8055D53C = NULL;
    }
}

DriverInfo* GetDriver(int i) {
    return &lbl_8055D540[i];
}

DriverInfo* FindDriver(s32 id) {
    int i;

    for (i = 0; i < lbl_8055D554; i++) {
        if (id == lbl_803577F0[i].id) {
            return &lbl_803577F0[i];
        }
    }
    for (i = 0; i < lbl_8055D54C; i++) {
        if (id == lbl_8055D540[i].id) {
            return &lbl_8055D540[i];
        }
    }
    return &lbl_803577F0[0];
}

int GetNumDrivers(void) {
    return lbl_8055D54C;
}

VehicleInfo* GetVehicle(int i) {
    if (i < lbl_8055D550) {
        return &lbl_8055D544[i];
    }
    return &lbl_80357960[i - lbl_8055D550];
}

VehicleInfo* FindVehicle(s32 id) {
    int i;

    for (i = 0; i < lbl_8055D554; i++) {
        if (id == lbl_80357960[i].id) {
            return &lbl_80357960[i];
        }
    }
    for (i = 0; i < lbl_8055D550; i++) {
        if (id == lbl_8055D544[i].id) {
            return &lbl_8055D544[i];
        }
    }
    return &lbl_8055D544[0];
}

int GetNumVehicles(void) {
    return lbl_8055D550;
}

VehicleInfo* GetDriverVehicle(s32 id) {
    return GetVehicle(FindDriver(id)->vehicle);
}

int CountVehiclesNamed(const char* chassis) {
    int count = 0;
    int i;

    for (i = 0; i < lbl_8055D550; i++) {
        if (strcasecmp(lbl_8055D544[i].chassis, chassis) == 0) {
            count++;
        }
    }
    return count;
}

VehicleInfo* FindVehicleNamed(const char* chassis, int n) {
    int k = 0;
    int i;

    for (i = 0; i < lbl_8055D550; i++) {
        if (strcasecmp(lbl_8055D544[i].chassis, chassis) == 0) {
            if (k == n) {
                return &lbl_8055D544[i];
            }
            k++;
        }
    }
    return &lbl_8055D544[0];
}

const char* GetBaseChassisName(const char* chassis) {
    VehicleInfo* v = FindVehicleNamed(chassis, 0);

    if (v->flags & VEHICLE_EXTREME) {
        v = FindVehicle(v->base);
    }
    return v->chassis;
}

VehicleInfo* FindVehicleByFlags(u32 mask, int n) {
    int k = 0;
    int i;

    for (i = 0; i < lbl_8055D550; i++) {
        if (mask == (mask & lbl_8055D544[i].flags)) {
            if (k == n) {
                return &lbl_8055D544[i];
            }
            k++;
        }
    }
    return &lbl_8055D544[0];
}

DriverInfo* GetPlayerDriver(int slot) {
    return &lbl_803577F0[slot];
}

VehicleInfo* GetPlayerVehicle(int slot) {
    return GetVehicle(GetPlayerDriver(slot)->vehicle);
}

int GetNumPlayers(void) {
    return lbl_8055D554;
}

void SetNumPlayers(int n) {
    lbl_8055D554 = n;
}

int IsPlayerDriver(s32 id, int slot) {
    if (slot < lbl_8055D554 && id == lbl_803577F0[slot].id) {
        return 1;
    }
    return 0;
}

void SetPlayerDriver(s32 id, int slot) {
    DriverInfo* src = FindDriver(id);
    DriverInfo* p = &lbl_803577F0[slot];

    p->id = id;
    p->vehicle = src->vehicle;
    p->color = src->color;
    lbl_80357960[slot].id = PLAYER_ID;
}

void SetDriverColor(s32 id, u32 color) {
    int i;

    for (i = 0; i < lbl_8055D554; i++) {
        if (id == lbl_803577F0[i].id) {
            lbl_803577F0[i].color = color;
        }
    }
    for (i = 0; i < lbl_8055D54C; i++) {
        if (id == lbl_8055D540[i].id) {
            lbl_8055D540[i].color = color;
            return;
        }
    }
}

void SetListDriverColor(s32 id, u32 color) {
    int i;

    for (i = 0; i < lbl_8055D54C; i++) {
        if (id == lbl_8055D540[i].id) {
            lbl_8055D540[i].color = color;
            return;
        }
    }
}

void SetPlayer(int slot, s32 id, s32 vehicle, u32 color) {
    lbl_803577F0[slot].id = id;
    lbl_803577F0[slot].vehicle = FindVehicle(vehicle)->index;
    lbl_803577F0[slot].color = color;
    lbl_80357960[slot].id = PLAYER_ID;
}

void SetPlayerName(const char* name, int slot) {
    COPY_STRING(lbl_803577F0[slot].name, name, sizeof(lbl_803577F0[slot].name));
}

void ClearPlayer(int slot) {
    ResetPlayer(slot);
}

void GetPlayerLook(PlayerLook* out, s32 vehicle) {
    int found = -1;
    int i;

    for (i = 0; i < lbl_8055D554 && found == -1; i++) {
        if (vehicle == GetVehicle(GetPlayerDriver(i)->vehicle)->id) {
            found = i;
        }
    }
    if (found != -1 && lbl_80357C20[found].valid) {
        *out = lbl_80357C20[found].look;
    } else {
        fn_8027953C(out);
    }
}

VehicleColor* GetVehicleColor(VehicleInfo* v, int slot) {
    if (v->flags & VEHICLE_EXTREME) {
        v = FindVehicle(v->base);
    }
    return &lbl_8055D548[v->index + slot * MAX_VEHICLES];
}

// Stripped from ET; only its strings remain.
void GetDriverTexName(char* buf, u32 size, const char* name, int n) {
    fn_801345D8(buf, size, "%d", n);
    fn_801345D8(buf, size, "%s%d.tex", name, n);
}

void LoadDriverLists(void) {
    int i;

    fn_8013C804();
    lbl_8055D54C = 0;
    lbl_8055D550 = 0;
    if (lbl_8055D538 != NULL) {
        LoadDriverList(lbl_8055D538);
    }
    if (lbl_8055D53C != NULL) {
        LoadDriverList(lbl_8055D53C);
    }
    if (lbl_8055D550 == 0) {
        fn_802E317C("driver: No vehicles.");
    }
    for (i = 0; i < lbl_8055D54C; i++) {
        DriverInfo* d = &lbl_8055D540[i];
        int found = 0;
        int j;

        for (j = 0; j < lbl_8055D550; j++) {
            VehicleInfo* v = &lbl_8055D544[j];
            if (v->id == d->vehicle) {
                d->vehicle = j;
                found = 1;
                break;
            }
        }
        if (!found) {
            d->vehicle = i % lbl_8055D550;
        }
    }
    CheckDuplicateNumbers();
    fn_8013C8B8();
}

void LoadDriverList(ValueTable* table) {
    ValueNode* node;
    int i;

    for (i = 0; (node = table->GetChild(i)) != NULL; i++) {
        if (strcasecmp(node->GetName(), "driver") == 0) {
            LoadDriver(node);
        } else if (strcasecmp(node->GetName(), "vehicle") == 0) {
            LoadVehicle(node);
        }
    }
}

u32 ParseColor(const char* s) {
    u32 color = DEFAULT_COLOR;

    if (s[0] == '0' && tolower(s[1]) == 'x') {
        u32 value;
        if (sscanf(s + 2, "%x", &value) == 1) {
            color = value;
        }
    } else if (s[0] >= '0' && s[0] <= '9') {
        int r;
        int g;
        int b;
        if (sscanf(s, "%d, %d, %d", &r, &g, &b) == 3) {
            color = 0xFF000000 | ((u32)(u8)r << 16) | ((u32)(u8)g << 8) | (u32)(u8)b;
        }
    }
    return color;
}

void LoadDriver(ValueNode* node) {
    DriverInfo* d = &lbl_8055D540[lbl_8055D54C];
    s32 id;
    const char* name;

    d->node = node;
    d->index = lbl_8055D54C;
    id = node->GetInt("number", lbl_8055D54C);
    name = node->GetString("name", NULL);
    if (name != NULL) {
        COPY_STRING(d->name, name, sizeof(d->name));
    } else {
        fn_801345D8(d->name, sizeof(d->name), "defname-%d", d->index);
    }
    d->id = id;
    d->unk54 = -1;
    d->rank = node->GetInt("rank", id);
    d->vehicle = -1;
    d->color = DEFAULT_COLOR;
    lbl_8055D54C++;
}

void LoadVehicle(ValueNode* node) {
    VehicleInfo* v = &lbl_8055D544[lbl_8055D550];
    int extreme;
    int magical;
    int no_ai;
    const char* str;
    int i;
    int j;
    const char* base_name;

    v->index = lbl_8055D550;
    v->id = lbl_8055D550 | 0x40000000;
    COPY_STRING(v->chassis, node->GetValue("chassis"), sizeof(v->chassis));
    v->chassis[0] = toupper(v->chassis[0]);
    extreme = node->GetBool("is_extreme_car", 0);
    magical = node->GetBool("is_magical_car", 0);
    no_ai = node->GetBool("no_ai", 0);
    base_name = NULL;
    if (extreme) {
        char base[0x40];
        char* p;

        v->base = 0;
        COPY_STRING(base, v->chassis, sizeof(base));
        p = strrchr(base, '_');
        if (p != NULL) {
            *p = '\0';
            for (i = 0; i < lbl_8055D550; i++) {
                if (strcasecmp(base, lbl_8055D544[i].chassis) == 0) {
                    v->base = lbl_8055D544[i].id;
                    lbl_8055D544[i].base = v->id;
                    base_name = lbl_8055D544[i].chassis;
                    break;
                }
            }
            if (v->base == 0) {
                fn_802E317C("driver: Can't find non-extreme version of %s", v->chassis);
            }
        } else {
            fn_802E317C("driver: 'Extreme' chassis does not contain '_e'? (%s)", v->chassis);
        }
    } else {
        v->base = v->id;
    }
    COPY_STRING(v->display_name, node->GetString("display_name", base_name != NULL ? base_name : v->chassis),
                sizeof(v->display_name));
    COPY_STRING(v->texture, "invalid.tex", sizeof(v->texture));
    {
        char key[0x40];
        fn_801345D8(key, sizeof(key), "CarName:%s", v->display_name);
        COPY_STRING(v->display_name, fn_80131234(key), sizeof(v->display_name));
    }
    for (v->num_colors = 0; v->num_colors < MAX_BODY_COLORS; v->num_colors++) {
        char key[0x20];
        fn_801345D8(key, sizeof(key), "body_color%d", v->num_colors + 1);
        str = node->GetString(key, NULL);
        if (str != NULL) {
            v->colors[v->num_colors] = ParseColor(str);
        } else {
            if (v->num_colors == 0) {
                v->colors[v->num_colors++] = DEFAULT_COLOR;
            }
            break;
        }
    }
    v->default_color = node->GetInt("default_body_color_idx", 0) - 1;
    v->unk9C = fn_8013C8E8(v->chassis);
    v->flags = 0;
    if (node->GetBool("is_trailer", 0)) {
        v->flags |= VEHICLE_TRAILER;
    }
    if (extreme) {
        v->flags |= VEHICLE_EXTREME;
    }
    if (magical) {
        v->flags |= VEHICLE_MAGICAL;
    }
    if (no_ai) {
        v->flags |= VEHICLE_NO_AI;
    }
    v->groups = 0x0FF807FF;
    for (j = 0;; j++) {
        char key[0x20];
        int group;
        fn_801345D8(key, sizeof(key), "group_%d", j + 1);
        group = node->GetInt(key, 0);
        if (group == 0) {
            break;
        }
        if (group >= 0) {
            v->groups |= 1 << group;
        } else {
            v->groups &= ~(1 << -group);
        }
    }
    fn_801345D8(v->number, sizeof(v->number), "%d", v->id & ~0x40000000);
    lbl_8055D550++;
}

void CheckDuplicateNumbers(void) {
    int j;
    int i;

    for (i = 0; i < lbl_8055D54C; i++) {
        for (j = i + 1; j < lbl_8055D54C; j++) {
            if (lbl_8055D540[i].id == lbl_8055D540[j].id) {
                fn_802E317C("driver: Drivers %s and %s have the same number (%d)", lbl_8055D540[i].name,
                            lbl_8055D540[j].name, lbl_8055D540[j].id);
            }
        }
    }
    for (i = 0; i < lbl_8055D550; i++) {
        for (j = i + 1; j < lbl_8055D550; j++) {
            if (lbl_8055D544[i].id == lbl_8055D544[j].id) {
                fn_802E317C("driver: two vehicles (%s and %s) have the same number (%d)",
                            lbl_8055D544[i].chassis, lbl_8055D544[j].chassis, lbl_8055D544[i].id);
            }
        }
    }
}

void ResetPlayer(int slot) {
    memset(&lbl_803577F0[slot], 0, sizeof(DriverInfo));
    lbl_803577F0[slot].id = PLAYER_ID;
    fn_801345D8(lbl_803577F0[slot].name, sizeof(lbl_803577F0[slot].name), "Player-%d", slot + 1);
    lbl_803577F0[slot].rank = 1000;
    lbl_80357C00[slot].valid = 0;
    lbl_80357C20[slot].valid = 0;
}

// Only this function needs 604 scheduling (found with ChatGPT); file-wide it breaks the others.
#pragma scheduling 604
void PickVehicleColor(VehicleInfo* v, int slot, VehicleColor* out) {
    int c;

    memset(out, 0, sizeof(VehicleColor));
    c = v->default_color;
    out->unk8 = 0;
    if (c >= 0) {
        c = (c + slot) % v->num_colors;
        out->body = c;
        out->trim = c;
    } else {
        c = (v->id + slot * v->num_colors / 4) % v->num_colors;
        out->body = c;
        out->trim = c;
    }
}
#pragma scheduling reset


void ResetVehicleColors(void) {
    int i;

    memset(lbl_8055D548, 0, sizeof(VehicleColor) * MAX_PLAYERS * MAX_VEHICLES);
    for (i = 0; i < lbl_8055D550; i++) {
        VehicleInfo* v = &lbl_8055D544[i];
        int slot;

        if (!(v->flags & VEHICLE_EXTREME)) {
            for (slot = 0; slot < MAX_PLAYERS; slot++) {
                PickVehicleColor(v, slot, GetVehicleColor(v, slot));
            }
        }
    }
}
