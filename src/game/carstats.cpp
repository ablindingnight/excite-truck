// Car stats (0x8001880C-0x80018B70): one entry per chassis from car_stats.val, each with up to five
// numbered stat name/value pairs. "CarStatsLookup" is the engine's own name (from its message); the
// others are guesses.
#include <string.h>
#include "game/common.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);  // snprintf
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* table);
}

void* operator new[](size_t size, const char* tag);

#define MAX_CAR_STATS 5

struct CarStat {
    const char* name;
    s32 value;
};

struct CarStats {
    const char* chassis;               // offset 0x0
    CarStat stats[MAX_CAR_STATS];      // offset 0x4
};

ModuleTag lbl_8055D568;
u8 lbl_8055D570;
ValueTable* lbl_8055D574;
CarStats* lbl_8055D578;
s32 lbl_8055D57C;

void CarStatsInit(void) {
    int i;
    ValueNode* entry;
    CarStats* cs;
    int n;

    lbl_8055D570 = 1;
    lbl_8055D574 = fn_80131808("car_stats.val");
    lbl_8055D57C = 0;
    for (i = 0; (entry = lbl_8055D574->GetChild(i)) != NULL; i++) {
        if (strcasecmp(entry->GetName(), "carstats") == 0 && entry->Has("chassis")) {
            lbl_8055D57C++;
        }
    }

    lbl_8055D578 = new ("car_stats") CarStats[lbl_8055D57C];
    n = 0;
    for (i = 0; (entry = lbl_8055D574->GetChild(i)) != NULL; i++) {
        if (strcasecmp(entry->GetName(), "carstats") == 0 && entry->Has("chassis")) {
            u32 k;

            cs = &lbl_8055D578[n++];
            memset(cs, 0, sizeof(CarStats));
            cs->chassis = entry->GetValue("chassis");
            for (k = 0;; k++) {
                char name_key[0x14];
                char value_key[0x14];

                fn_801345D8(name_key, sizeof(name_key), "stat_name_%d", k + 1);
                fn_801345D8(value_key, sizeof(value_key), "stat_value_%d", k + 1);
                if (!entry->Has(name_key) || !entry->Has(value_key)) {
                    break;
                }
                if (k >= MAX_CAR_STATS) {
                    fn_802E317C("stat table for %s has too many stats (limit: %d)", cs->chassis, MAX_CAR_STATS);
                }
                cs->stats[k].name = entry->GetValue(name_key);
                cs->stats[k].value = entry->GetIntValue(value_key);
            }
            if (k == 0) {
                fn_802E317C("stat table for %s has no stats! (maybe you didn't number from 1)", cs->chassis);
            }
        }
    }
}

CarStats* CarStatsLookup(const char* chassis) {
    int i;

    for (i = 0; i < lbl_8055D57C; i++) {
        if (strcasecmp(lbl_8055D578[i].chassis, chassis) == 0) {
            return &lbl_8055D578[i];
        }
    }
    fn_802E317C("CarStatsLookup: no such car as %s, AFAIK", chassis);
    return NULL;
}

void CarStatsShutdown(void) {
    lbl_8055D570 = 0;
    delete[] lbl_8055D578;
    lbl_8055D578 = NULL;
    lbl_8055D57C = 0;
    fn_80131818(lbl_8055D574);
}
