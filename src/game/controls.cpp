// Track B2 full-body draft from original assembly; data qualification incomplete. External declarations identify
// unresolved code and storage; this TU is not registered or linked into the game.
#include "game/common.h"

int IdentGetCurrentSlot(void);
const char* IdentGetSlotName(int);

extern "C" {
struct ControlActionInfoFields { const char* displayName; const char* key; };
struct ControlActionDescriptorFields { const char* key; const char* defaultValue; };
struct ControlKeyDefaultFields { const char* key; const char* value; };

const char* lbl_8055BBA0 = "(nothing)";

extern const ControlActionInfoFields lbl_802FA0A8[29] = {
    {"Airlift", "airlift"},
    {"Alternate Reverse", "alt_reverse"},
    {"Braking 2", "braking"},
    {"Braking 1", "braking2"},
    {"Car Info", "car_info"},
    {"Change Camera", "view"},
    {"Digital Braking", "dig_braking"},
    {"Digital Throttle", "dig_throttle"},
    {"Downshift", "downshift"},
    {"Emergency Brake", "e_brake"},
    {"Hold/Special", "horn"},
    {"Look-Back Control", "look_back"},
    {"Look Left", "look_left"},
    {"Look Right", "look_right"},
    {"Mirror Control", "mirror_control"},
    {"Pitch Down", "pitch_down"},
    {"Pitch Up", "pitch_up"},
    {"Race Info", "race_info"},
    {"Reverse", "reverse"},
    {"Roll Left", "roll_left"},
    {"Roll Right", "roll_right"},
    {"Steer Left", "steer_left"},
    {"Steer Left", "steer2_left"},
    {"Steer Right", "steer_right"},
    {"Steer Right", "steer2_right"},
    {"Throttle 2", "throttle"},
    {"Throttle 1", "throttle2"},
    {"Upshift", "upshift"},
    {"Help", "help"},
};
extern const ControlActionDescriptorFields lbl_802FA190[17] = {
    {"steer_left", "Joy Left"},
    {"steer_right", "Joy Right"},
    {"throttle", "Button 1"},
    {"braking", "Button 2"},
    {"throttle2", ""},
    {"braking2", ""},
    {"reverse", ""},
    {"e_brake", ""},
    {"airlift", ""},
    {"pitch_up", "Joy Down"},
    {"pitch_down", "Joy Up"},
    {"roll_left", "JB Left"},
    {"roll_right", "JB Right"},
    {"horn", "Button 5"},
    {"look_back", ""},
    {"view", "Button 4"},
    {"help", "Button 5"},
};
extern const ControlKeyDefaultFields lbl_802FA218[8] = {
    {"digital_throttle", "false"},
    {"digital_braking", "false"},
    {"rumble", "true"},
    {"steer_dead_spot", "0.05"},
    {"nonlinear_steering", "1.0"},
    {"steer_dampening", "0.0"},
    {"brake_overshoot", "0.1"},
    {"throttle_overshoot", "0.35"},
};

typedef int ControlValueTableHandle;
extern int lbl_8055F18C;
extern int lbl_8055BC54;
extern int lbl_804987D0[4];
extern int lbl_8055F19C;
extern int lbl_8055F194;
struct ControlsTableFields {
    u8 pad0[4]; int columnCount, rowCount; u8 padC[0x24];
    int controller; void* inputConfig;
};
struct ControlInputFields { int id, controller; u32 modifiers; u8 flag; };
extern const int lbl_80563748[2];
extern const char* lbl_8055BBA0;
void __ct__7UITableFiiPCi(ControlsTableFields*, int, int, const int*);
const char* Get__7UITableFiiPi(ControlsTableFields*, int, int, int*);
void Set__7UITableFiiPCcc(ControlsTableFields*, int, int, const char*, char);
void fn_80137C98(int);
void* fn_8018108C(void);
void fn_801A7E54(char*, int, const ControlInputFields*);
extern ControlsTableFields* lbl_8055F190;
extern const char* lbl_80341C40[];

int fn_80137C74(void);
void fn_80137C78(int);
int fn_80137CFC(int);
void fn_80137C84(int, int);
void fn_80133460(ControlValueTableHandle, const char*, int);
void fn_801333E8(ControlValueTableHandle, const char*, u8*);
void fn_8023DA64(int, int, int);
void fn_8023E1FC(ControlsTableFields*);
void fn_8023E16C(ControlsTableFields*, int);
void fn_8023E0DC(ControlsTableFields*, int);
void fn_8023E33C(void*);
int fn_801A902C(void*);
void fn_801A9050(ControlInputFields*, int);

int fn_8023D6FC(void);
void fn_8023D704(int value);

void fn_8023DA2C(ControlValueTableHandle object, const char* key, int value, int write);

// Snapshot/restore controller assignments, bounded by the saved controller count.
void fn_8023DB68(void);
void fn_8023DBCC(void);
void fn_8023DD4C(void);
int fn_8023DFB0(void);

int fn_8023DFD4(void);
int fn_8023E028(void);

class ControlsChoiceBase {
public:
    ControlsChoiceBase(void) : state0(0),header4(-1),state8(0) {}
    u8 state0; u8 pad1[3]; int header4;
    u8 state8; u8 pad9[3];
    // Imported callback declarations only: no held base implementation.
    virtual void Base08(void);
    virtual void Base0C(void);
    virtual void Base10(void);
    virtual void Base14(void);
};
class ControlsChoiceFields : public ControlsChoiceBase {
public:
    ControlsChoiceFields(const ControlInputFields* value);
    virtual ~ControlsChoiceFields(void) {}
    virtual char* Label(void);
    virtual void Next(void);
    virtual void Previous(void);
    virtual void Base28(void);
    virtual void Base2C(void);
    int selected;
    char label[20];
};
typedef char CheckChoiceSize[sizeof(ControlsChoiceFields)==0x28?1:-1];
void fn_8023E3D8(ControlsChoiceFields* self, int direction);
void fn_8023E504(ControlsChoiceFields* self, ControlInputFields* out);
const char* fn_8023E52C(int index);
}

extern "C" {
}

extern "C" {
}

extern "C" {

ControlsTableFields* fn_8023E078(ControlsTableFields* self, int controller);
void fn_8023E0DC(ControlsTableFields* self, int selectedRow);
void fn_8023E16C(ControlsTableFields* self, int selectedRow);
void fn_8023E33C(void* pointer);


extern const ControlActionDescriptorFields lbl_802FA190[17];
ControlActionInfoFields* fn_8023DC38(const char*);
void Clear__7UITableFv(ControlsTableFields*);
void fn_801A7974(ControlInputFields*, const char*, ControlValueTableHandle);
typedef ControlValueTableHandle (*ControlConfigMapGetter)(void*);

class ControlsConfigCallView {public:
virtual void Slot08();
virtual void Slot0C();
virtual void Slot10();
virtual void Slot14();
virtual void Slot18();
virtual void Slot1C();
virtual void Slot20();
virtual void Slot24();
virtual void Slot28();
virtual void Slot2C();
virtual void Slot30();
virtual void Slot34();
virtual void Slot38();
virtual void Slot3C();
virtual void Slot40();
virtual void Slot44();
virtual void Slot48();
virtual void Slot4C();
virtual void Slot50();
virtual void Slot54();
virtual void Slot58();
virtual void Slot5C();
virtual void Slot60();
virtual void Slot64();
virtual void Slot68();
virtual void Slot6C();
virtual int GetMap();
};
void fn_8023E1FC(ControlsTableFields* self);


// Definitions are deliberately ordered by the original section layout.
// Zero-storage offsets are recovered; string/descriptor placement still needs
// whole-unit qualification. The choice method table remains external.
static ModuleTag controlsModuleTag;       // .sbss +0, original 8055F180
static UnkStatic123 controlsStatic123;    // .bss +0, original 804987B8
static UnkStatic7F controlsStatic7F;      // .sbss +8, original 8055F188
int lbl_804987D0[4];                     // .bss +0x18
int lbl_8055F18C;
ControlsTableFields* lbl_8055F190;
int lbl_8055F194;
void* lbl_8055F198;
int lbl_8055F19C;


extern const ControlActionInfoFields lbl_802FA0A8[29];
extern const ControlKeyDefaultFields lbl_802FA218[8];
int strcasecmp(const char*, const char*);
void fn_802E317C(const char*, ...);
void fn_8013358C(ControlValueTableHandle, const char*, const char*);
ControlActionDescriptorFields* fn_8023DCCC(const char*);

// Reading a missing boolean creates it from the supplied byte default. The five
// returned bytes are intentionally unused here; materialization is the effect.
void fn_8023D70C(void);

void fn_8023DA64(int controller, int reset, int write);

ControlActionInfoFields* fn_8023DC38(const char* key);
ControlActionDescriptorFields* fn_8023DCCC(const char* key);



void __dl__FPv(void*);
}

extern "C" {
}

extern "C" {


void fn_80137D10(void);
void fn_802E8664(int);
void fn_802E30C8(const char*, ...);
void fn_8023D7CC(void);


// Same five-word records as the known menu units. Reading the shared end tag's
// fields explicitly avoids type-punning the UnkStatic123 object as a MenuItem.
struct CaptionContext { int value; CaptionContext(int v=0):value(v){} };
struct MenuItem {
MenuItem(const char* n,const char* t,CaptionContext context=CaptionContext()){type=3;name=n;data=t;arg0=(int)context.value;arg1=0;}

    MenuItem(const char* n, int t, const void* d, int a0, int a1) {
        type = t; name = n; data = d; arg0 = a0; arg1 = a1;
    }
    MenuItem(const UnkStatic123& end) {
        name = (const char*)end.unk0;
        type = end.unk4;
        data = (const void*)end.unk8;
        arg0 = end.unkC;
        arg1 = end.unk10;
    }
    const char* name;
    int type;
    const void* data;
    int arg0, arg1;
};
struct MenuDesc {
    const char* layout;
    MenuItem* items;
    int (*poll)(int*);
    int unkC, unk10;
};
extern char lbl_8055B860[8];
int fn_802146BC(MenuDesc*, int, int, int);
void fn_8017EBE0(void);
void fn_8017EC30(void);
const char* fn_802E85B8(int);
void __dt__7UITableFv(ControlsTableFields*, int);
void fn_801A7A08(ControlInputFields*, const char*, ControlValueTableHandle);
void fn_801808DC(void);
int fn_8023DD80(void);

void fn_8023D4B8(int controller);

int fn_8023DD80(void);


// Original pointer-table contents recovered from DOL; not a placement claim.

extern const int lbl_80563748[2] = {20, 20};


}

int lbl_8055BC54 = -1;

// Definitions ordered by original owning identities, not ordinal pairing.
extern "C" void fn_8023D4B8(int controller) {
    lbl_8055F19C = controller;
    fn_8023DB68();
    fn_80137C78(1);
    fn_80137C84(controller, 0);
    fn_8017EBE0();
    fn_80137C98(controller);
    lbl_8055F198 = fn_8018108C();
    ControlsTableFields table;
    fn_8023E078(&table, controller);
    lbl_8055F190 = &table;
    MenuItem items[] = {
        MenuItem("control", 13, &table, 0, 0),
        MenuItem("input", 13, &table, 1, 0),
        MenuItem(lbl_8055B860, 6, (const void*)fn_8023E028, 0, 0x128),
        MenuItem(lbl_8055B860, 6, (const void*)fn_8023DFD4, 0, 0x126),
        MenuItem(lbl_8055B860, 6, (const void*)fn_8023DD80, 0, 13),
        MenuItem(lbl_8055B860, 6, 0, -1, 27),
        MenuItem(lbl_8055B860, 6, (const void*)fn_8023DFB0, 0, 0x6F),
        MenuItem("#controller_name", fn_802E85B8(controller)),
        MenuItem("#profile_name", IdentGetSlotName(IdentGetCurrentSlot())),
        MenuItem(controlsStatic123)
    };
    MenuDesc desc = {"controltab.lyt", 0, 0, 0, 0};
    desc.items = items;
    lbl_8055F194 = 0;
    fn_8023E0DC(lbl_8055F190, 0);
    fn_802146BC(&desc, -999, -999, 1);
    fn_8017EC30();
    fn_8023DBCC();
    __dt__7UITableFv(&table, 0);
}

extern "C" int fn_8023D6FC(void) { return lbl_8055F18C; }

extern "C" void fn_8023D704(int value) { lbl_8055F18C = value; }

extern "C" void fn_8023D70C(void) {
    for (int i = 0; i < 5; i++) {
        ControlValueTableHandle table = i < 4 ? i + 12 : 16;
        u8 autoShifting = 1, stabilityHelp = 1, brakingHelp = 0, hardcore = 0, draftMeter = 1;
        fn_801333E8(table, "auto_shifting", &autoShifting);
        fn_801333E8(table, "arc_stability_help", &stabilityHelp);
        fn_801333E8(table, "arcade_braking_help", &brakingHelp);
        fn_801333E8(table, "hardcore_mode", &hardcore);
        fn_801333E8(table, "draft_meter", &draftMeter);
    }
}

extern "C" void fn_8023D7CC(void) {
    fn_80137D10();
    for (int controller = 0; controller < 4; controller++) {
        fn_8023DA64(controller, 1, 0);
        switch (fn_8023D6FC()) {
        case 0:
            fn_802E8664(1);
            fn_8013358C(controller + 8, "horn", "POV Any");
            fn_8013358C(controller + 8, "horn2", "");
            fn_8013358C(controller + 8, "roll_left", "");
            fn_8013358C(controller + 8, "roll_right", "");
            break;
        case 2:
            fn_802E30C8("MCM_NES_GRIP_DPAD_STEER is an obsolete controller configuration");
            fn_802E8664(1);
            fn_8013358C(controller + 8, "steer_left", "POV Left");
            fn_8013358C(controller + 8, "steer_right", "POV Right");
            fn_8013358C(controller + 8, "roll_left", "Joy Left");
            fn_8013358C(controller + 8, "roll_right", "Joy Right");
            fn_8013358C(controller + 8, "horn", "Button 2");
            fn_8013358C(controller + 8, "braking", "Button 3");
            break;
        case 3:
            fn_802E30C8("MCM_REMOTE_GRIP_DPD_STEER is an obsolete controller configuration");
            fn_802E8664(0);
            fn_8013358C(controller + 8, "braking", "POV Back");
            fn_8013358C(controller + 8, "horn", "Button 2");
            fn_8013358C(controller + 8, "view", "Button 5");
            fn_8013358C(controller + 8, "pitch_up", "JB Up");
            fn_8013358C(controller + 8, "pitch_down", "JB Down");
            break;
        case 1:
            fn_802E8664(0);
            fn_8013358C(controller + 8, "steer_left", "JB Left");
            fn_8013358C(controller + 8, "steer_right", "JB Right");
            fn_8013358C(controller + 8, "pitch_up", "JB Up");
            fn_8013358C(controller + 8, "pitch_down", "JB Down");
            fn_8013358C(controller + 8, "roll_left", "");
            fn_8013358C(controller + 8, "roll_right", "");
            fn_8013358C(controller + 8, "braking", "POV Back");
            fn_8013358C(controller + 8, "horn", "Button 2");
            fn_8013358C(controller + 8, "view", "Button 5");
            break;
        default:
            fn_802E317C("mcontroller: unhandled controller mode %d", fn_8023D6FC());
            break;
        }
    }
}

// Explicit retention scaffolding; no historical body identity asserted.
#pragma force_active on
extern "C" __declspec(section ".data") const char ControlsObservedDiagnostic[] = "MenuDoAdvanceCareerControls: Shouldn't this function do something?";
#pragma force_active off

extern "C" void fn_8023DA2C(ControlValueTableHandle object, const char* key, int value, int write) {
    u8 byteDefault = value;
    if (write) fn_80133460(object, key, value);
    else fn_801333E8(object, key, &byteDefault);
}

extern "C" void fn_8023DA64(int controller, int reset, int write) {
const ControlKeyDefaultFields* value;
ControlInputFields input;
const ControlActionInfoFields* info;
ControlValueTableHandle table;
input.controller=controller;
input.flag=0;
table=controller+8;
info=lbl_802FA0A8;
    for (int i = 0; i < 29; i++, info++) {
        const ControlActionDescriptorFields* defaults = fn_8023DCCC(info->key);
        if (defaults) {
            input.id = 0;
            if (!reset) fn_801A7974(&input, info->key, table);
            if (input.id == 0) fn_8013358C(table, info->key, defaults->defaultValue);
        } else fn_8013358C(table, info->key, "");
    }
    value = lbl_802FA218;
    for (int i = 0; i < 8; i++, value++) fn_8013358C(table, value->key, value->value);
    fn_8023DA2C(table, "force_feedback", 1, write);
}

extern "C" void fn_8023DB68(void) {
    lbl_8055BC54 = fn_80137C74();
    int* out = lbl_804987D0;
    for (int i = 0; i < lbl_8055BC54; out++, i++) *out = fn_80137CFC(i);
}

extern "C" void fn_8023DBCC(void) {
    fn_80137C78(lbl_8055BC54);
    int* in = lbl_804987D0;
    for (int i = 0; i < lbl_8055BC54; in++, i++) fn_80137C84(*in, i);
    lbl_8055BC54 = -1;
}

extern "C" ControlActionInfoFields* fn_8023DC38(const char* key) {
    const ControlActionInfoFields* info = lbl_802FA0A8;
    for (int i = 0; i < 29; i++, info++) {
        if (!strcasecmp(info->key, key)) return (ControlActionInfoFields*)&lbl_802FA0A8[i];
    }
    fn_802E317C("get_control_info: %s was not found in all_controls.", key);
    return 0;
}

extern "C" ControlActionDescriptorFields* fn_8023DCCC(const char* key) {
    const ControlActionDescriptorFields* info = lbl_802FA190;
    for (int i = 0; i < 17; i++, info++) {
        if (!strcasecmp(info->key, key)) return (ControlActionDescriptorFields*)&lbl_802FA190[i];
    }
    return 0;
}

extern "C" void fn_8023DD4C(void) {
    fn_8023DA64(lbl_8055F19C, 1, 1);
    fn_8023E1FC(lbl_8055F190);
}

extern "C" int fn_8023DD80(void) {
    if (lbl_8055F194 >= 17) { fn_8023DD4C(); return 0; }
    ControlInputFields initial;
    initial.controller = lbl_8055F19C;
    initial.flag = 0;
    ControlValueTableHandle table = ((ControlsConfigCallView*)lbl_8055F198)->GetMap();
    fn_801A7974(&initial, lbl_802FA190[lbl_8055F194].key, table);
    ControlsChoiceFields choice(&initial);
    MenuItem items[] = {
        MenuItem("#title", 3, Get__7UITableFiiPi(lbl_8055F190, lbl_8055F194, 0, 0), 0, 0),
        MenuItem("#input", 16, 0, (int)&choice, 0),
        MenuItem(lbl_8055B860, 6, 0, -1, 27),
        MenuItem(lbl_8055B860, 6, 0, -2, 13),
        MenuItem(controlsStatic123)
    };
    MenuDesc desc = {"controlpop.lyt", 0, 0, 0, 0};
    desc.items = items;
    if (fn_802146BC(&desc, -999, -999, 1) != -1) {
        ControlInputFields selected;
        selected.controller = lbl_8055F19C;
        selected.flag = 0;
        fn_8023E504(&choice, &selected);
        table = ((ControlsConfigCallView*)lbl_8055F198)->GetMap();
        fn_801A7A08(&selected, lbl_802FA190[lbl_8055F194].key, table);
        fn_8023E1FC(lbl_8055F190);
        fn_801808DC();
    }
    return 0;
}

extern "C" int fn_8023DFB0(void) { fn_8023DD4C(); return 0; }

extern "C" int fn_8023DFD4(void) {
    fn_8023E16C(lbl_8055F190, lbl_8055F194);
    int columns = lbl_8055F190->rowCount;
    lbl_8055F194 = (columns + lbl_8055F194 - 1) % columns;
    fn_8023E0DC(lbl_8055F190, lbl_8055F194);
    return 0;
}

extern "C" int fn_8023E028(void) {
    fn_8023E16C(lbl_8055F190, lbl_8055F194);
    lbl_8055F194 = (lbl_8055F194 + 1) % lbl_8055F190->rowCount;
    fn_8023E0DC(lbl_8055F190, lbl_8055F194);
    return 0;
}

extern "C" ControlsTableFields* fn_8023E078(ControlsTableFields* self, int controller) {
    __ct__7UITableFiiPCi(self, 18, 2, lbl_80563748);
    self->controller = controller;
    fn_80137C98(controller);
    self->inputConfig = fn_8018108C();
    fn_8023E1FC(self);
    return self;
}

extern "C" void fn_8023E0DC(ControlsTableFields* self, int selectedRow) {
    if (selectedRow >= 0 && selectedRow < self->rowCount) {
        int column = self->columnCount;
        while (column-- > 0) {
            const char* text = Get__7UITableFiiPi(self, selectedRow, column, 0);
            Set__7UITableFiiPCcc(self, selectedRow, column, text, 1);
        }
    }
}

extern "C" void fn_8023E16C(ControlsTableFields* self, int selectedRow) {
    if (selectedRow >= 0 && selectedRow < self->rowCount) {
        int column = self->columnCount;
        while (column-- > 0) {
            const char* text = Get__7UITableFiiPi(self, selectedRow, column, 0);
            Set__7UITableFiiPCcc(self, selectedRow, column, text, 0);
        }
    }
}

extern "C" void fn_8023E1FC(ControlsTableFields* self) {
    Clear__7UITableFv(self);
    ControlInputFields input;
    input.controller = self->controller;
    input.flag = 0;
    const ControlActionDescriptorFields* action = lbl_802FA190;
    for (int row = 0; row < 17; row++, action++) {
        const char* title = fn_8023DC38(action->key)->displayName;
        Set__7UITableFiiPCcc(self, row, 0, title, 0);
        // The original calls the configuration object's virtual slot +0x70.
        // Its class name and the map's concrete type are deliberately unknown.
        ControlValueTableHandle map = ((ControlsConfigCallView*)self->inputConfig)->GetMap();
        fn_801A7974(&input, action->key, map);
        char text[20];
        if (input.id == 0) {
            const char* src = lbl_8055BBA0;
            int n = 20;
            char* dst = text;
            while ((*dst++ = *src++) != 0) {
                if (--n <= 0) { dst[-1] = 0; break; }
            }
        } else fn_801A7E54(text, 20, &input);
        Set__7UITableFiiPCcc(self, row, 1, text, 0);
    }
    Set__7UITableFiiPCcc(self, 17, 0, "Restore Defaults", 0);
}

extern "C" void fn_8023E33C(void* pointer) {
    ControlsChoiceFields* self = (ControlsChoiceFields*)pointer;
    if (self->selected == -1) {
        const char* src = lbl_8055BBA0;
            int n = 20;
            char* dst = self->label;
        while ((*dst++ = *src++) != 0) {
            if (--n <= 0) { dst[-1] = 0; break; }
        }
    } else {
        ControlInputFields input;
        input.controller = 0;
        input.flag = 0;
        fn_801A9050(&input, self->selected);
        fn_801A7E54(self->label, 20, &input);
    }
}

extern "C" void fn_8023E3D8(ControlsChoiceFields* self, int direction){int count=fn_801A902C(self)+1;int next=count+direction;++self->selected;self->selected=(self->selected+next)%count-1;fn_8023E33C(self);}

ControlsChoiceFields::ControlsChoiceFields(const ControlInputFields* value) {
    ControlsChoiceFields* self=this;
    self->selected = -1;
    int i = fn_801A902C(self);
    while (i-- > 0) {
        ControlInputFields candidate;
        candidate.controller = 0;
        candidate.flag = 0;
        fn_801A9050(&candidate, i);
        if (candidate.id == value->id && candidate.modifiers == value->modifiers) self->selected = i;
    }
    fn_8023E33C(self);
}

extern "C" void fn_8023E504(ControlsChoiceFields* self, ControlInputFields* out) {
    if (self->selected == -1) { out->id = 0; return; }
    fn_801A9050(out, self->selected);
}

extern "C" const char* fn_8023E52C(int index) { return lbl_80341C40[index]; }

char* ControlsChoiceFields::Label(void) { return label; }

void ControlsChoiceFields::Previous(void) { fn_8023E3D8(this, 1); }

void ControlsChoiceFields::Next(void) { fn_8023E3D8(this, -1); }

extern "C" {
const char* lbl_80341C40[] = {"NES Grip", "Remote Grip", "NES Grip (DPAD Steer)", "Remote Grip (DPD Steer)"};
}
