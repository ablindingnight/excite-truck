// Career path classes and the career runner (0x8027D304-0x8027E908): the concrete Path / Bracket / Contest
// classes behind the abstract event-path interfaces (their constructors, destructors and accessors; the
// loading code is in eventpath.cpp), the per-slot career points save block, the vehicle list offered for an
// event, and the loop that plays career contests and reports what a run unlocked. Names are guesses apart
// from the strings.
#include "game/pathcareer.h"

static inline int PMax(int a, int b) { return a > b ? a : b; }
static inline int PMin(int a, int b) { return a < b ? a : b; }

ModuleTag lbl_8055F660;
UnkStatic123s lbl_804B2FC0;
static CareerSave lbl_804B2FD8[4];
UnkStatic7F lbl_8055F668;
u8 lbl_8055F66C;  // running
s32 lbl_8055C790 = -1;

extern "C" {

void fn_8027D96C(int slot) {
    switch (slot) {
    case 1:
        break;
    case 0:
    case 2:
        memset(lbl_804B2FD8, 0, sizeof(lbl_804B2FD8));
        break;
    }
}

void fn_8027D9A4(int slot, int ok) {
    switch (ok) {
    case 0:
        fn_802E30C8("PathCareer: Identity called to say that %d was cleared", slot);
        memset(&lbl_804B2FD8[slot], 0, sizeof(CareerSave));
        break;
    }
}

void fn_8027DA08(void) {
    CareerSave* s = &lbl_804B2FD8[IdentGetCurrentSlot()];
    s->points = 0;
    for (int i = 0; i < 3; i++) {
        s32 total, max;
        fn_8027BCC0(i, &total, &max);
        s->points += total;
    }
    int got = 0;
    int all = 0;
    for (int i = 0; i < 3; i++) {
        s32 total, max;
        fn_8027BCC8(i, &total, &max);
        got += total;
        all += max;
    }
    s->percent = PMin(PMax(got * 100 / all, 0), 100);
}

void fn_8027DAF4(void) {
    fn_802E6BD8('PCRR', 1, lbl_804B2FD8, sizeof(lbl_804B2FD8), fn_8027D96C);
    IdentRegisterCallback(fn_8027D9A4);
}

int fn_8027DE68(int i);

VehicleList* fn_8027DB40(u32 flags) {
    int all = flags & 1;
    int unlocked;
    int showAll = (flags >> 1) & 1;
    int n;
    int limit;
    int lockedOut = (flags >> 2) & 1;
    if (IsUnlockAll()) {
        all = 1;
    }
    int maxPoints = GetMaxExcitePoints();
    limit = 0;
    if (!all && !fn_8027DE68(0)) {
        limit = 1;
    }
    n = 0;
    for (int i = 0; i < GetUnlockableCount(); i++) {
        Unlockable* u = GetUnlockable(i);
        if (limit && u->points > maxPoints) {
            continue;
        }
        if (u && !strcasecmp(u->type, "CAR") && CountVehiclesNamed(u->name) > 0) {
            n++;
        }
    }
    if (IsExtremeChassisUnlocked()) {
        n++;
    }
    if (IsCareerComplete()) {
        n++;
    }
    VehicleList* list = fn_8025007C(n);
    bool checkSpecial = !showAll;
    for (int i = 0; i < GetUnlockableCount(); i++) {
        Unlockable* u = GetUnlockable(i);
        if (limit && u->points > maxPoints) {
            continue;
        }
        if (u && !strcasecmp(u->type, "CAR") && CountVehiclesNamed(u->name) > 0) {
            VehicleInfo* v = FindVehicleNamed(u->name, 0);
            unlocked = 1;
            if (!all && !IsCarUnlocked(u->name)) {
                unlocked = 0;
            }
            int special = 0;
            if (!(v->flags & 2) && v->a8 != v->a4) {
                special = 1;
            }
            if (!unlocked && lockedOut) {
                continue;
            }
            int sp = special && (all || (checkSpecial && IsSpecialChassisUnlocked(u->name)));
            list->Add(u->name, unlocked, sp);
        }
    }
    if (IsExtremeChassisUnlocked()) {
        list->Add(GetAllExtremesChassis(), 1, 0);
    }
    if (IsCareerComplete()) {
        list->Add(GetCareerCompletedChassis(), 1, 0);
    }
    return list;
}

void fn_8027E0C8(Path* (*getPath)(int), int which);

void fn_8027DDCC(int which) {
    lbl_8055F66C = 1;
    int mode = GetEveMode();
    if (mode != 0x130) {
        if (mode != 0x134) {
            UnloadEves();
        }
        LoadModeEves(0x130);
    }
    fn_8027E0C8(fn_8027B834, which);
    lbl_8055F66C = 0;
    int cur = GetEveMode();
    if (mode != cur) {
        if (cur != 0x134) {
            UnloadEves();
        }
        if (mode != 0x134) {
            LoadModeEves(mode);
        }
    }
}

int fn_8027DE68(int i) { return fn_8027B94C(i); }

void fn_8027DE6C(void) {
    fn_8027B77C();
    fn_8027BF38();
    fn_8027B690();
    fn_8027DA08();
}

s32 fn_8027DE98(int slot) { return lbl_804B2FD8[slot].points; }

s32 fn_8027DEAC(void) { return fn_8027DE98(IdentGetCurrentSlot()); }

s32 fn_8027DED0(void) { return lbl_8055C790; }

void fn_8027DED8(void) {}

static inline bool HasChild(ValueNode* n, const char* name) { return n->FindChild(name) != 0; }

int fn_8027DEDC(s32* vehicle, s32* x, int track, int which) {
    SetNumPlayers(1);
    TrackInfo* t = GetTrack(track);
    const char* def = 0;
    if (t && t->node && HasChild(t->node, "vehicle_map")) {
        def = t->node->FindChild("vehicle_map")->GetString("default_vehicle", 0);
    }
    VehicleList* list;
    if (!def) {
        list = fn_8027DB40(0);
    } else {
        list = fn_8025007C(1);
        list->Add(def, 1, 0);
    }
    int r = fn_8024DAE0(vehicle, x, 0, list, which);
    delete list;
    return r;
}

void fn_8027E024(void* out, int event, int which) {
    CareerRun run;
    run.track = GetEvent(event)->track;
    run.index = -1;
    for (int i = GetTrack(run.track)->events; i-- > 0;) {
        if (event == FindTrackEvent(run.track, i)->event) {
            run.index = i;
        }
    }
    run.which = which;
    run.out = out;
    fn_8023BF4C(&run);
}

void fn_8027E0C8(Path* (*getPath)(int), int which) {
    s32 sel = -1;
    s32 driver = GetDriver(0)->id;
    s32 vehicle = GetVehicle(GetDriver(0)->vehicle)->a4;
    s32 x = GetDriver(0)->a50;
    IPath* path = getPath(which);
    fn_802840F4();
    CareerResult res;
    memset(&res, 0, sizeof(res));
    while (fn_802840FC(&sel, path, &res)) {
        memset(&res, 0, sizeof(res));
        if (!fn_8027DEDC(&vehicle, &x, GetEvent(sel)->track, which)) {
            continue;
        }
        int bi = -1;
        int ci = -1;
        int wasUnlocked = 0;
        int wasOpen = 0;
        int bracketUnlocked = 0;
        int bracketDone = 0;
        int prevDone = 1;
        for (int i = 0; i < path->Count(); i++) {
            IBracket* b = path->Get(i);
            int unl = b->Unlocked();
            int p = prevDone;
            prevDone = 0;
            if (p && unl) {
                prevDone = 1;
            }
            for (int k = 0; k < b->Count(); k++) {
                IContest* c = b->Get(k);
                if (sel == c->EventId()) {
                    bi = i;
                    ci = k;
                    wasUnlocked = c->Unlocked();
                    wasOpen = c->Done();
                    bracketUnlocked = unl;
                    bracketDone = b->AllDone();
                }
            }
        }
        int oldBest = path->Get(bi)->Get(ci)->Best();
        int oldPaths = fn_8027BAB8(path);
        VehicleInfo* v = FindVehicle(vehicle);
        int special = !(v->flags & 2);
        bool hadSpecial = special && IsSpecialChassisUnlocked(v->name);
        int hadExtreme = IsExtremeChassisUnlocked();
        int hadCareer = IsCareerComplete();
        fn_802840F8();
        SetPlayer(0, driver, vehicle, x);
        SetPlayerName(IdentGetSlotName(IdentGetCurrentSlot()), 0);
        fn_80137C84(fn_802E4A10(), 0);
        int lim = fn_8027DE68(0) == 0;
        AchSetScoreLimiter(lim);
        if (path->Get(bi)->Unlocked()) {
            lbl_8055C790 = 0;
        } else {
            lbl_8055C790 = path->Get(bi)->Get(ci)->Passing();
        }
        u8 out[0xB0];
        memset(out, 0, sizeof(out));
        fn_8027E024(out, sel, which);
        lbl_8055C790 = -1;
        ClearPlayer(0);
        AchSetScoreLimiter(0);
        path = getPath(which);
        fn_802840F4();
        if (*(s32*)(out + 8) > 0) {
            fn_8027DA08();
            int newBest = path->Get(bi)->Get(ci)->Best();
            int hasSpecial = 0;
            if (special && IsSpecialChassisUnlocked(v->name)) {
                hasSpecial = 1;
            }
            AchQuery q;
            q.a0 = 0;
            q.a4 = 0;
            q.a8 = 0;
            AchievementsQuery(&q);
            VehicleList* list = fn_8025007C(2);
            if (!hadSpecial && hasSpecial) {
                list->Add(GetSpecialChassis(v->name), 1, 0);
                const char* why = ShowUnlockReason(v->name);
                list->SetReason(list->Count() - 1, why);
            }
            if (!hadExtreme && AllExtremeCarsUnlocked2()) {
                list->Add(GetAllExtremesChassis(), 1, 0);
                const char* why = ShowExtremeUnlockReason();
                list->SetReason(list->Count() - 1, why);
            }
            fn_80284580(q.a4, q.a8, list, &x);
            delete list;
            fn_8025A204();
            AchievementsCommitSession();
            if (path->IsCareer()) {
                int nowUnlocked = path->Get(bi)->Get(ci)->Unlocked();
                int nowOpen = path->Get(bi)->Get(ci)->Done();
                int nowBracketUnlocked = path->Get(bi)->Unlocked();
                int nowBracketDone = path->Get(bi)->AllDone();
                int nowPaths = fn_8027BAB8(path);
                int nowPrev = 1;
                for (int i = 0; i < path->Count(); i++) {
                    int p = nowPrev;
                    nowPrev = p && path->Get(i)->Unlocked();
                }
                int nowCareer = IsCareerComplete();
                int d = newBest - oldBest;
                res.delta = d;
                res.improved = d > 0;
                res.event = sel;
                res.newly[0] = !wasUnlocked && nowUnlocked;
                res.newly[1] = !wasOpen && nowOpen;
                res.newly[2] = !bracketUnlocked && nowBracketUnlocked;
                res.newly[3] = !bracketDone && nowBracketDone;
                res.newly[4] = !prevDone && nowPrev;
                res.newly[5] = !oldPaths && nowPaths;
                res.newly[6] = !hadCareer && nowCareer;
            }
        }
        fn_802E7128();
    }
    fn_802840F8();
}

}  // extern "C"
