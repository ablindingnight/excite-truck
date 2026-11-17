// Records screen (0x80257284-0x80257C10): browses the profile's best results by track and event
// in two lists, with the selected event's statistics beside them. Names are guesses apart from
// the layout and element names.
#include "game/common.h"
#include "game/menu.h"
#include "game/gamedb.h"

extern "C" int strcasecmp(const char* a, const char* b);

#include "game/statistic.h"

void UnloadEves(void);
void LoadCareerEves(void);
void LoadArcadeEves(void);
int GetNumTracks(void);
int GetNumEvents(void);
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
#include "game/uitable.h"

// Retail callers pass the style in a full register (Set stores its low byte): they call retail's symbols by
// name with an int style (PC: the members).
#ifdef __MWERKS__
extern "C" {
const char* Get__7UITableFiiPi(UITable*, int, int, int*);
void Set__7UITableFiiPCcc(UITable*, int, int, const char*, int);
}
#endif
static inline const char* GetCell(UITable* t, int row, int col, int* flag) {
#ifdef __MWERKS__
    return Get__7UITableFiiPi(t, row, col, flag);
#else
    return t->Get(row, col, flag);
#endif
}
static inline void SetCell(UITable* t, int row, int col, const char* text, int style) {
#ifdef __MWERKS__
    Set__7UITableFiiPCcc(t, row, col, text, style);
#else
    t->Set(row, col, text, (char)style);
#endif
}
static inline int Rows(UITable* t) { return t->used; }  // an inline getter in retail (read early)

#include "game/statstable.h"

// A menu state; most slots are empty defaults.
class MenuState {
public:
    MenuState(void) { unk14 = 0; }
    virtual ~MenuState(void) {}
    virtual void Enter(void);   // 0x0C
    virtual void Exit(void);    // 0x10
    virtual void Unk14(void);
    virtual void Changed(int key, void* value);
    virtual void Update(void);  // 0x1C
    virtual void Unk20(void);
    virtual void Unk24(struct gxCanvas*);
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
public:
    RecordsState(void);
    virtual ~RecordsState(void);
    virtual void Enter(void);
    virtual void Update(void);
    virtual int HandleKey(u32 key);

    int focus;            // offset 0x18: 0 = tracks, 1 = events
    UITable tracks;     // offset 0x1C
    int trackSel;         // offset 0x4C
    u8 tracksDirty;       // offset 0x50
    UITable events;     // offset 0x54
    int eventSel;         // offset 0x84
    u8 eventsDirty;       // offset 0x88
    s32 eventIds[0x40];   // offset 0x8C
    StatsTable stats;     // offset 0x18C
};

extern "C" {
int fn_802146BC(MenuDesc* desc, int x, int y, int a);
void fn_80236914(int which, f32* out, f32 t);
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
        MenuItem("#profile_name", IdentGetSlotName(IdentGetCurrentSlot())),
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
    TrackInfo* track;
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
        for (e = 0; !found && e < track->events; e++) {
            if (FindTrackEvent(t, e)->node->Has("name")) {
                found = 1;
            }
        }
        if (found) {
            env = GetTrackEnviron(track->name);
            listed = 0;
            for (i = 0; !listed && i < tracks.used; i++) {
                if (strcasecmp(env, GetCell(&tracks, i, 1, 0)) == 0) {
                    listed = 1;
                }
            }
            if (!listed) {
                row = tracks.used;
                SetCell(&tracks, row, 1, env, 0);
                SetCell(&tracks, row, 0, GetTrackFriendlyName(track->name), 0);
            }
        }
    }
    trackSel = 0;
    SetCell(&tracks, trackSel, 0, GetCell(&tracks, trackSel, 0, 0), 1);
    tracksDirty = 1;
}

static inline void AddEventRow(RecordsState* s, EventInfo* ev, const char* env, int row) {
    if (strcasecmp(GetTrackEnviron(ev->node->GetString("track", GetTrack(ev->track)->name)), env) == 0) {
        row = s->events.used;
        SetCell(&s->events, row, 0, GetEventTypeDisplayName(ev->type), 0);
        SetCell(&s->events, row, 1, GetEventTypeName(ev->type), 0);
        s->eventIds[row] = ev->id;
    }
}

void RecordsState::Update(void) {
    EventInfo* ev;
    int row;
    const char* env;
    int i;
    if (tracksDirty) {
        env = GetCell(&tracks, trackSel, 1, 0);
        events.Clear();
        for (i = 0; i < GetNumEvents(); i++) {
            ev = GetEvent(i);
            if (ev->node->Has("name")) {
                AddEventRow(this, ev, env, row);
            }
        }
        eventSel = 0;
        eventsDirty = 1;
        tracksDirty = 0;
    }
    if (eventsDirty) {
        if (events.used > 0) {
            stats.Show(eventIds[eventSel]);
        } else {
            stats.Clear();
        }
        eventsDirty = 0;
    }
    SetCell(&tracks, trackSel, 0, GetCell(&tracks, trackSel, 0, 0), focus == 0 ? 1 : 2);
    if (events.used > 0) {
        SetCell(&events, eventSel, 0, GetCell(&events, eventSel, 0, 0), focus == 1 ? 1 : 2);
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
        if (focus == 0 && events.used > 0) {
            focus = 1;
        }
        break;
    case 0x126:
    case 0x128:
        switch (focus) {
        case 0:
            SetCell(&tracks, trackSel, 0, GetCell(&tracks, trackSel, 0, 0), 0);
            Wrap(&trackSel, key == 0x128 ? 1 : -1, Rows(&tracks));
            tracksDirty = 1;
            break;
        case 1:
            SetCell(&events, eventSel, 0, GetCell(&events, eventSel, 0, 0), 0);
            Wrap(&eventSel, key == 0x128 ? 1 : -1, Rows(&events));
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
        SetCell(this, i, 0, StatsGetLabel(type), 0);
        Statistic stat(IdentGetCurrentSlot(), type, event, 0);
        if (stat.HasValue()) {
            char buf[20];
            SetCell(this, i, 1, stat.Format(buf, 20), 0);
        } else {
            SetCell(this, i, 1, "Not set!", 0);
        }
    }
}

StatsTable::StatsTable(void) : UITable(7, 2, lbl_80563AB8) {}

void StatsTable::Show(s32 event) {
    this->event = event;
    Refresh();
}

// NonMatching: needs the MenuState base methods' symbols to link. Also, fn_80257284 keeps a second
// zero register (li r30, 0) for the profile item that no form tried reproduces, and Update gives
// its highlight temp r30 instead of r29.
