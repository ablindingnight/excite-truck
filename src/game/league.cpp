// AI leagues (0x80137950-0x80137D58): the four AI driver leagues (Dirt, Modified, Truck, Cup), each
// loading "<name>.val" for its car width/length, the current league selection, and the player-to-driver
// table. Names are guesses.
#include "game/common.h"

extern "C" {
int sprintf(char* s, const char* fmt, ...);
char* strcpy(char* d, const char* s);
int strcasecmp(const char* a, const char* b);
void* memset(void* p, int c, size_t n);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* t);
}

void* operator new(size_t size, const char* tag);

int GetNumPlayers(void);
void SetNumPlayers(int n);
void SetPlayerDriver(s32 player, int driver);
void ClearPlayer(int player);

struct League {
    f32 carWid;     // offset 0x0
    f32 carLen;     // offset 0x4
    char name[32];  // offset 0x8
};

ModuleTag lbl_8055E0D8;
u8 lbl_8055E0E0;
s32 lbl_80559C00 = -1;
League* lbl_8042F788[4];
s32 lbl_8042F798[4];

extern "C" {

const char* fn_80137950(int i) {
    switch (i) {
    case 3:
        return "Cup";
    case 1:
        return "Modified";
    case 2:
        return "Truck";
    case 0:
        return "Dirt";
    default:
        fn_802E317C("driver: bad league %d", i);
        return 0;
    }
}

League* fn_801379CC(void) {
    if (lbl_80559C00 == -1) {
        fn_802E317C("AIDriver: league not set!");
    }
    return lbl_8042F788[lbl_80559C00];
}

void fn_80137A18(void) {
    char buf[0x100];
    League* l = fn_801379CC();
    ValueTable* t;
    int i;
    ValueNode* n;
    sprintf(buf, "%s.val", l->name);
    t = fn_80131808(buf);
    i = 0;
    for (;;) {
        n = t->GetChild(i++);
        if (!n) {
            break;
        }
        if (!strcasecmp(n->GetName(), "Info")) {
            l->carWid = n->GetFloat("car_wid");
            l->carLen = n->GetFloat("car_len");
        }
    }
    fn_80131818(t);
}

void fn_80137B0C(s32 i) { lbl_80559C00 = i; }

void fn_80137B14(void) { lbl_80559C00 = -1; }

void fn_80137B20(void) {
    int i;
    lbl_8055E0E0 = 1;
    lbl_80559C00 = -1;
    for (i = 0; i < 4; i++) {
        int j;
        lbl_8042F788[i] = (League*)operator new(sizeof(League), "league");
        memset(lbl_8042F788[i], 0, sizeof(League));
        strcpy(lbl_8042F788[i]->name, fn_80137950(i));
        fn_80137B0C(i);
        fn_801379CC();
        for (j = 0; j < 4; j++) {
            lbl_8042F798[j] = 4;
        }
        fn_80137A18();
        fn_80137B14();
    }
    fn_80137B0C(0);
}

void fn_80137BE4(void) {
    int i;
    for (i = 0; i < 4; i++) {
        delete lbl_8042F788[i];
        lbl_8042F788[i] = 0;
    }
    lbl_8055E0E0 = 0;
}

s32 fn_80137C4C(void) { return lbl_80559C00; }

League* fn_80137C54(s32 i) {
    if (i == -1) {
        i = lbl_80559C00;
    }
    return lbl_8042F788[i];
}

int fn_80137C74(void) { return GetNumPlayers(); }

void fn_80137C78(int n) { SetNumPlayers(n); }

void fn_80137C7C(s32 player, int driver) { SetPlayerDriver(player, driver); }

void fn_80137C80(int player) { ClearPlayer(player); }

void fn_80137C84(int player, int driver) { lbl_8042F798[driver] = driver; }

int fn_80137C98(int driver) {
    int i;
    int n = GetNumPlayers();
    for (i = 0; i < n; i++) {
        if (driver == lbl_8042F798[i]) {
            return i;
        }
    }
    return -1;
}

s32 fn_80137CFC(int player) { return lbl_8042F798[player]; }

void fn_80137D10(void) {
    int i;
    for (i = 0; i < 4; i++) {
        fn_80137C84(i, i);
    }
}

}  // extern "C"
