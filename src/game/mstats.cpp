// Records screen (0x80257284-0x80257C10): browses the profile's best results by track and event
// in two lists, with the selected event's statistics beside them. Names are guesses apart from
// the layout and element names.
#include "game/common.h"

extern "C" int strcasecmp(const char* a, const char* b);

struct Track {
    s32 index;          // offset 0x0
    s32 league;         // offset 0x4
    char track[0x20];   // offset 0x8
    u8 pad28[0x48];
    s32 num_contests;   // offset 0x70
};

struct Event {
    s32 index;        // offset 0x0
    s32 track;        // offset 0x4
    u32 type;         // offset 0x8
    u8 padC[0x14];
    ValueNode* node;  // offset 0x20
};

class Statistic {
public:
    Statistic(int identity, u32 type, s32 event, int slot);
    BOOL HasValue(void);
    char* Format(char* buf, int size);

    u8 pad0[0x24];
};

void UnloadEves(void);
void LoadCareerEves(void);
void LoadArcadeEves(void);
int GetNumTracks(void);
Track* GetTrack(int i);
Event* FindTrackEvent(int track, int eve);
int GetNumEvents(void);
Event* GetEvent(int i);
const char* GetTrackEnviron(const char* name);
const char* GetTrackFriendlyName(const char* name);
const char* GetEventTypeDisplayName(u32 type);
const char* GetEventTypeName(u32 type);
int IdentGetCurrentSlot(void);
const char* IdentGetSlotName(int slot);
u32 StatsGetTypeFromIndex(u32 index);
const char* StatsGetLabel(u32 type);

extern "C" int fn_80247624(void);  // career mode active

// A text grid shown by a menu list element.
class MenuTable {
public:
    MenuTable(int rows, int cols, const int* widths);
    ~MenuTable(void);
    void Clear(void);
    const char* GetCell(int row, int col, int unk);
    void SetCell(int row, int col, const char* text, int style);
    int Rows(void) { return rows; }

    u8 pad0[0x8];
    int rows;  // offset 0x8
    u8 padC[0x24];
};

class StatsTable : public MenuTable {
public:
    StatsTable(void);
    ~StatsTable(void) {}
    void Refresh(void);
    void Show(s32 event);

    s32 event;  // offset 0x30
};

// A menu state; most slots are empty defaults.
class MenuState {
public:
    MenuState(void) { unk14 = 0; }
    virtual ~MenuState(void) {}
    virtual void Enter(void);   // 0x0C
    virtual void Exit(void);    // 0x10
    virtual void Unk14(void);
    virtual void Unk18(void);
    virtual void Update(void);  // 0x1C
    virtual void Unk20(void);
    virtual void Unk24(void);
    virtual void Draw(void);    // 0x28
    virtual void Unk2C(void);
    virtual void Unk30(void);
    virtual void Unk34(void);
    virtual void Unk38(void);
    virtual void Unk3C(void);
    virtual void Unk40(void);
    virtual void Unk44(void);
    virtual void Unk48(void);
    virtual void Unk4C(void);
    virtual void Unk50(void);
    virtual void Unk54(void);
    virtual int HandleKey(u32 key);  // 0x58
    virtual void Unk5C(void);

    u8 pad4[0x10];
    int unk14;
};

class RecordsState : public MenuState {
public:
    RecordsState(void);
    virtual ~RecordsState(void);
    virtual void Enter(void);
    virtual void Update(void);
    virtual int HandleKey(u32 key);

    int focus;            // offset 0x18: 0 = tracks, 1 = events
    MenuTable tracks;     // offset 0x1C
    int trackSel;         // offset 0x4C
    u8 tracksDirty;       // offset 0x50
    MenuTable events;     // offset 0x54
    int eventSel;         // offset 0x84
    u8 eventsDirty;       // offset 0x88
    s32 eventIds[0x40];   // offset 0x8C
    StatsTable stats;     // offset 0x18C
};

struct MenuItem {
    MenuItem(const char* n, int t, const void* d, int a0, int a1) {
        type = t;
        name = n;
        data = d;
        arg0 = a0;
        arg1 = a1;
    }
    const char* name;
    int type;
    const void* data;
    int arg0;
    int arg1;
};

struct MenuDesc {
    const char* layout;
    MenuItem* items;
    int (*poll)(int* result);
    int unkC;
    int unk10;
};

extern "C" {
int fn_802146BC(MenuDesc* desc, int x, int y, int a);
void fn_80236914(void);
extern char lbl_8055BAB8[4];
extern char lbl_8055B860[8];
}

static const int lbl_80563AA8[2] = {32, 32};
static const int lbl_80563AB0[2] = {32, 20};
static const int lbl_80563AB8[2] = {20, 20};

ModuleTag lbl_8055F348;
static UnkStatic123 lbl_80498E38;
UnkStatic7F lbl_8055F350;

extern "C" void fn_80257284(void) {
    if (fn_80247624()) {
        UnloadEves();
        LoadCareerEves();
    }
    RecordsState state;
    MenuItem items[] = {
        MenuItem(NULL, 5, &state, 0, 0),
        MenuItem("track", 13, &state.tracks, 0, 0),
        MenuItem("event", 13, &state.events, 0, 0),
        MenuItem("record", 13, &state.stats, 0, 0),
        MenuItem("value", 13, &state.stats, 1, 0),
        MenuItem("#profile_name", 3, IdentGetSlotName(IdentGetCurrentSlot()), 0, 0),
        MenuItem(lbl_8055B860, 6, NULL, -1, 27),
        MenuItem(lbl_8055BAB8, 8, (void*)fn_80236914, 2, 0),
        *(MenuItem*)&lbl_80498E38,
    };
    MenuDesc desc = {"mstats.lyt", items, NULL, 0, 0};
    while (fn_802146BC(&desc, -999, -999, 1) != -1) {
    }
    if (fn_80247624()) {
        UnloadEves();
        LoadArcadeEves();
    }
}

RecordsState::RecordsState(void) : tracks(40, 2, lbl_80563AA8), events(64, 2, lbl_80563AB0) {}

RecordsState::~RecordsState(void) {}

void RecordsState::Enter(void) {
    int row;
    Track* track;
    int found;
    int t;
    int e;
    const char* env;
    int listed;
    int i;
    focus = 0;
    for (t = 0; t < GetNumTracks(); t++) {
        track = GetTrack(t);
        found = 0;
        for (e = 0; !found && e < track->num_contests; e++) {
            if (FindTrackEvent(t, e)->node->Has("name")) {
                found = 1;
            }
        }
        if (found) {
            env = GetTrackEnviron(track->track);
            listed = 0;
            for (i = 0; !listed && i < tracks.rows; i++) {
                if (strcasecmp(env, tracks.GetCell(i, 1, 0)) == 0) {
                    listed = 1;
                }
            }
            if (!listed) {
                row = tracks.rows;
                tracks.SetCell(row, 1, env, 0);
                tracks.SetCell(row, 0, GetTrackFriendlyName(track->track), 0);
            }
        }
    }
    trackSel = 0;
    tracks.SetCell(trackSel, 0, tracks.GetCell(trackSel, 0, 0), 1);
    tracksDirty = 1;
}

void RecordsState::Update(void) {
    Event* ev;
    int row;
    const char* env;
    int i;
    if (tracksDirty) {
        env = tracks.GetCell(trackSel, 1, 0);
        events.Clear();
        for (i = 0; i < GetNumEvents(); i++) {
            ev = GetEvent(i);
            if (ev->node->Has("name")) {
                if (strcasecmp(GetTrackEnviron(ev->node->GetString("track", GetTrack(ev->track)->track)), env) == 0) {
                    row = events.rows;
                    events.SetCell(row, 0, GetEventTypeDisplayName(ev->type), 0);
                    events.SetCell(row, 1, GetEventTypeName(ev->type), 0);
                    eventIds[row] = ev->index;
                }
            }
        }
        eventSel = 0;
        eventsDirty = 1;
        tracksDirty = 0;
    }
    if (eventsDirty) {
        if (events.rows > 0) {
            stats.Show(eventIds[eventSel]);
        } else {
            stats.Clear();
        }
        eventsDirty = 0;
    }
    tracks.SetCell(trackSel, 0, tracks.GetCell(trackSel, 0, 0), focus == 0 ? 1 : 2);
    if (events.rows > 0) {
        events.SetCell(eventSel, 0, events.GetCell(eventSel, 0, 0), focus == 1 ? 1 : 2);
    }
}

static void Wrap(int* v, int delta, int n);

int RecordsState::HandleKey(u32 key) {
    switch (key) {
    case 0x125:
        if (focus == 1) {
            focus = 0;
        }
        break;
    case 0x127:
        if (focus == 0 && events.rows > 0) {
            focus = 1;
        }
        break;
    case 0x126:
    case 0x128:
        switch (focus) {
        case 0:
            tracks.SetCell(trackSel, 0, tracks.GetCell(trackSel, 0, 0), 0);
            Wrap(&trackSel, key == 0x128 ? 1 : -1, tracks.Rows());
            tracksDirty = 1;
            break;
        case 1:
            events.SetCell(eventSel, 0, events.GetCell(eventSel, 0, 0), 0);
            Wrap(&eventSel, key == 0x128 ? 1 : -1, events.Rows());
            eventsDirty = 1;
            break;
        }
        break;
    default:
        return 0;
    }
    return 1;
}

static void Wrap(int* v, int delta, int n) { *v = (*v + (n + delta)) % n; }

void StatsTable::Refresh(void) {
    Clear();
    for (int i = 0; i < 7; i++) {
        u32 type = StatsGetTypeFromIndex(i + 1);
        SetCell(i, 0, StatsGetLabel(type), 0);
        Statistic stat(IdentGetCurrentSlot(), type, event, 0);
        if (stat.HasValue()) {
            char buf[20];
            SetCell(i, 1, stat.Format(buf, 20), 0);
        } else {
            SetCell(i, 1, "Not set!", 0);
        }
    }
}

StatsTable::StatsTable(void) : MenuTable(7, 2, lbl_80563AB8) {}

void StatsTable::Show(s32 event) {
    this->event = event;
    Refresh();
}

// NonMatching: needs the MenuState base methods' symbols to link. Also, fn_80257284 keeps a second
// zero register (li r30, 0) for the profile item that no form tried reproduces, and Update gives
// its highlight temp r30 instead of r29.
