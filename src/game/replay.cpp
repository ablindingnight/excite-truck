// Replay viewer (0x80008390-0x80009DC8): the playback loop with its key/action handling, the
// on-screen prompt legend (replaylyt.val), camera switching and the "debug replay" A/B-point and
// movie tool. Names come from the "Replay:" / "replay:" strings; function names are guesses.
#include <stdio.h>
#include "game/common.h"
#include "macros.h"

template <class T, int N>
class FixedArray {
public:
    FixedArray(void) {
        data = storage;
        max = N;
        count = 0;
    }
    ~FixedArray(void) {
        data = NULL;
        count = 0;
        max = 0;
    }
    void Clear(void) { count = 0; }
    void Add(T x) { *Alloc() = x; }
    T* Alloc(void) { return &data[count++]; }
    int Has(T x) {
        int i;

        for (i = 0; i < count; i++) {
            if (x == data[i]) {
                return 1;
            }
        }
        return 0;
    }

    T* data;     // offset 0x0
    s32 count;   // offset 0x4
    s32 max;     // offset 0x8
    T storage[N];  // offset 0xC
};

class LocString {
public:
    LocString(const char* key);
    const char* Get(void);

    const char* key;   // offset 0x0
    const char* text;  // offset 0x4
    u32 version;       // offset 0x8
};

class Layout {
public:
    virtual ~Layout(void);
};

struct EventInfo {
    u8 pad0[0x10];
    u32 flags;  // offset 0x10
};

struct PadState {
    u8 pad0[0x40];
    u32 dir;  // offset 0x40
    u8 pad44[0x70 - 0x44];
};

struct Camera {
    u8 data[0x30];
};

struct DrawOffset {
    u8 data[0x8];
};

extern "C" {
extern u32 lbl_8055E044;  // localization version
extern s32 lbl_8055CFF0;  // screen width
extern s32 lbl_8055CFF4;  // screen height
void fn_80131318(LocString* s);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);
const char* fn_80131234(const char* key);
void fn_801473F8(Layout* layout, const char* name, const char* fmt, ...);
void fn_8014819C(DrawOffset* off, f32 x, f32 y);
void fn_801481C8(DrawOffset* off, int unk);
u32 fn_8026E20C(void);
void fn_8026E204(u32 flags);
EventInfo* fn_801E87A8(void);
int fn_801769F8(void);
void fn_80011A2C(void);
int fn_802ECC10(void);
int fn_801298E8(PadState* pad, int unk);
void fn_80176A00(int unk);
void fn_80278894(int unk);
void fn_801769E0(void);
void fn_801769EC(void);
void fn_80176734(void);
void fn_802782C4(void);
void fn_80206454(void);
void fn_80206444(void);
void fn_801E7E48(void);
void fn_802C8530(void);
void fn_802C855C(void);
void fn_802C87B4(int x, int y, int w, int h);
void fn_801E81B8(int a, int b);
void fn_80230B5C(Camera* cam);
void fn_802C8638(Camera* cam);
void fn_802C974C(f32 a, f32 b, f32 c);
void fn_80278924(f32* a, f32* b);
f32 fn_801E7E18(void);
int fn_802787F4(void);
void fn_80010188(u32 color, int flags, const char* fmt, f32 scale, f32 x, f32 y, ...);
int fn_80278358(void);
void fn_80211F64(void);
void fn_80129AFC(void);
void fn_802C2B48(void);
int fn_802E47B8(void);
u32 fn_802E46F0(void);
void fn_8027889C(f32 t);
void fn_802788E0(f32 t);
void fn_802787E4(void);
void fn_80278318(void);
void fn_80278778(const char* name);
void fn_8026A1F0(int cam, int a, int b);
void fn_8027888C(int unk);
void fn_801E85CC(int unk);
void fn_801E85BC(int unk);
void fn_8026DDD8(int cam);
int fn_8026DDD0(void);
void fn_801CEFD4(int unk);
int fn_80137C74(void);
int fn_80137CFC(int player);
void fn_80129AEC(int pad);
int fn_801E86F8(int player);
void fn_801E86E8(int index, int unk);
Layout* fn_80145E70(const char* name);
int fn_802E2C20(void);
void fn_80179588(void);
void fn_801795B0(void);
int fn_801CB8FC(void);
void fn_801E7E04(int a, int b);
int fn_8018FD38(void);
int fn_802E4A10(void);
int fn_80137C98(int unk);
int fn_801E8838(void);
void fn_80230E90(int unk);
void fn_8017691C(void);
void fn_801E85DC(int unk);
void fn_801E882C(void);
void fn_80012158(void);
void fn_802C9A70(int unk);
void fn_802C942C(u32 color);
}
int GetNumPlayers(void);
void DrawFade2(f32 t);
void fn_80007210(void);
void fn_800073D8(int selection);
void fn_80007318(int unk);

inline const char* LocString::Get(void) {
    if (version != lbl_8055E044) {
        fn_80131318(this);
    }
    return text;
}

struct ActionPrompt {
    s32 action;         // offset 0x0
    const char* glyph;  // offset 0x4
    const char* label;  // offset 0x8
};

ActionPrompt lbl_802FB8F0[7] = {
    {0x1, "\\dpad", NULL},
    {0xF, "\\minus", NULL},
    {0x15, "\\plus", NULL},
    {0x14, "\\A", NULL},
    {0x13, "\\2", NULL},
    {0x10, "\\1", NULL},
    {0x11, "\\1", NULL},
};

// Five-word static built by an inline constructor (first word stored first).
class ReplayStatic {
public:
    ReplayStatic(void) {
        unk0 = 0;
        unk4 = 123;
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

struct PromptTimes {
    u8 visible;  // offset 0x0
    s32 shown;   // offset 0x4
    s32 touched; // offset 0x8
};

struct ReplayClock {
    s32 last;  // offset 0x0
    s32 time;  // offset 0x4
};

ModuleTag lbl_8055D360;
static ReplayStatic lbl_8034F3B0;
UnkStatic7F lbl_8055D368;
// The action list lives in a wrapper object; its implicit dtor is the one registered.
struct ActionList {
    FixedArray<s32, 28> list;
};
ActionList lbl_8034F3D0;
PromptTimes lbl_8034F44C;

u8 lbl_8055D36C;
u8 lbl_8055D36D;  // replay running
s32 lbl_8055D370;
Layout* lbl_8055D374;
u8 lbl_8055D378[2];  // restart / replay requested
u8 lbl_8055D37A;     // results menu up
ReplayClock lbl_8055D380;
f32 lbl_8055D394;
s32 lbl_8055D398;
u8 lbl_8055D39C;
u8 lbl_8055D39D;

u8 lbl_805589B0 = 1;  // advanced overlay
s32 lbl_805589EC = 4;  // camera mode

void fn_80008390(void) {
    static const s32 multi[] = {0xF};
    static const s32 basic[] = {0x1, 0x15, 0x14, 0x13};
    static const s32 advanced[] = {0x1, 0x15, 0x14, 0x13, 0x10};
    static const s32 dpad[] = {0x2, 0x3, 0x4, 0x5};
    u32 i;
    int j;

    lbl_8034F3D0.list.Clear();
    if (GetNumPlayers() > 1) {
        lbl_8034F3D0.list.Add(multi[0]);
    }
    if (!lbl_8055D370) {
        for (i = 0; i < sizeof(basic) / sizeof(basic[0]); i++) {
            lbl_8034F3D0.list.Add(basic[i]);
        }
    } else {
        for (i = 0; i < sizeof(advanced) / sizeof(advanced[0]); i++) {
            lbl_8034F3D0.list.Add(advanced[i]);
        }
    }
    for (j = 0; j < lbl_8034F3D0.list.count; j++) {
        if (lbl_8034F3D0.list.data[j] == 1) {
            for (i = 0; i < sizeof(dpad) / sizeof(dpad[0]); i++) {
                lbl_8034F3D0.list.Add(dpad[i]);
            }
            break;
        }
    }
}

int fn_800085D8(s32 action) { return lbl_8034F3D0.list.Has(action); }

void fn_80008618(u32 bit) {
    u32 flags = fn_8026E20C();
    fn_8026E204(flags & bit ? flags & ~bit : flags | bit);
}

int fn_8000865C(void) { return (fn_801E87A8()->flags >> 1) & 1; }

static inline f32 Clamp01(f32 x) {
    f32 v = MIN(1.0f, x);
    return MAX(v, 0.0f);
}

void fn_80008684(int camera) {
    ActionPrompt* prompt;
    int dt;
    DrawOffset off;
    f32 t;
    f32 x;
    int now;
    s32 action;
    int n;
    u32 i;
    int j;
    char name[64];
    f32 y;
    char label[64];

    if (lbl_8055D374) {
        now = lbl_8055D380.time;
        dt = now - lbl_8034F44C.shown;
        if (dt < 600) {
            t = dt / 600.0f;
            t = Clamp01(t);
        } else {
            t = 1.0f;
        }
        if (now - lbl_8034F44C.touched < 3000) {
            t = 1.0f - t;
        }
        x = 400.0f * t;
        y = 0.0f;
        static LocString s_cameras("Replay:CAMERAS");
        static LocString s_changeCar("Replay:CHANGE_CAR");
        static LocString s_pause("Replay:PAUSE");
        static LocString s_restart("Replay:RESTART");
        static LocString s_exit("Replay:EXIT");
        static LocString s_basic("Replay:BASIC_OVERLAY");
        static LocString s_advanced("Replay:ADVANCED_OVERLAY");
        static LocString s_pauseTitle("Replay:PAUSE_TITLE");
        static LocString s_playTitle("Replay:PLAY_TITLE");
        static struct LabelInit {
            LabelInit(void) {
                lbl_802FB8F0[0].label = s_cameras.Get();
                lbl_802FB8F0[1].label = s_changeCar.Get();
                lbl_802FB8F0[2].label = s_pause.Get();
                lbl_802FB8F0[3].label = s_restart.Get();
                lbl_802FB8F0[4].label = s_exit.Get();
                lbl_802FB8F0[5].label = s_basic.Get();
                lbl_802FB8F0[6].label = s_advanced.Get();
            }
        } s_init;
        fn_8014819C(&off, x, y);
        n = 0;
        for (i = 0, prompt = lbl_802FB8F0; i < 7; i++, prompt++) {
            action = prompt->action;
            for (j = 0; j < lbl_8034F3D0.list.count; j++) {
                if (action == lbl_8034F3D0.list.data[j]) {
                    fn_801345D8(name, sizeof(name), "prompt%d", n);
                    fn_801345D8(label, sizeof(label), "label%d", n);
                    if (lbl_8055D374) {
                        fn_801473F8(lbl_8055D374, name, "%s", prompt->glyph);
                        fn_801473F8(lbl_8055D374, label, "%s", prompt->label);
                    }
                    n++;
                }
            }
        }
        fn_801481C8(&off, -1);
        if (fn_801769F8()) {
            if (lbl_8055D380.time % 1000 < 500) {
                fn_801473F8(lbl_8055D374, "title", "%s", s_pauseTitle.Get());
            }
        } else {
            fn_801473F8(lbl_8055D374, "title", "%s", s_playTitle.Get());
        }
        if (camera != 4) {
            if (camera == 13 && fn_8000865C()) {
                camera = 12;
            } else if (camera == 12 && fn_8000865C()) {
                camera = 13;
            }
            switch (camera) {
            case 13:
                fn_801473F8(lbl_8055D374, "camera_type", "%s", fn_80131234("Replay:CAMERA_RIGHT"));
                break;
            case 12:
                fn_801473F8(lbl_8055D374, "camera_type", "%s", fn_80131234("Replay:CAMERA_LEFT"));
                break;
            case 14:
                fn_801473F8(lbl_8055D374, "camera_type", "%s", fn_80131234("Replay:CAMERA_UP"));
                break;
            case 7:
                fn_801473F8(lbl_8055D374, "camera_type", "%s", fn_80131234("Replay:CAMERA_DOWN"));
                break;
            default:
                fn_801473F8(lbl_8055D374, "camera_type", "%s", fn_80131234("Replay:CAMERA_DEFAULT"));
                break;
            }
        }
        fn_80011A2C();
    }
}

enum { CAM_LEFT = 12, CAM_RIGHT = 13 };

int fn_80008C5C(void) {
    int prev = lbl_805589EC;
    PadState pad;
    u32 dir;
    int any;

    if (!lbl_8055D37A && !fn_802ECC10() && fn_801298E8(&pad, 0)) {
        dir = pad.dir;
        if (dir == 8) {
            lbl_805589EC = fn_8000865C() ? CAM_RIGHT : CAM_LEFT;
        } else if (dir == 2) {
            lbl_805589EC = fn_8000865C() ? CAM_LEFT : CAM_RIGHT;
        } else if (dir == 4) {
            lbl_805589EC = 7;
        } else if (dir == 1) {
            lbl_805589EC = 14;
        }
        any = 0;
        if (pad.dir - 1 <= 7 && ((1 << (pad.dir - 1)) & 0x8B)) {
            any = 1;
        }
        if (!any && lbl_8055D394 > 1.0f && lbl_805589EC != 4) {
            lbl_805589EC = 4;
        }
        if (lbl_805589EC != prev) {
            if (lbl_8055D394 > 1.0f || prev == 4) {
                lbl_8055D394 = 0.0f;
            } else {
                lbl_805589EC = prev;
            }
        }
        lbl_8055D394 += 0.016683351f;
    }
    return lbl_805589EC;
}

int fn_80008DBC(int key) {
    switch (key) {
    case 0x125:
        return 2;
    case 0x126:
        return 4;
    case 0x128:
        return 5;
    case 0x127:
        return 3;
    case '\r':
        return 0x13;
    case ' ':
        return 0xF;
    case 'o':
        return 0xE;
    case 0x113:
        return 0x15;
    case 0x121:
        return 0x14;
    case 0x1B:
        return 0x12;
    case 0x122:
        return 0x15;
    case 0x170:
        return 0;
    case 0x171:
        return 0;
    case 0x172:
        return 0;
    case 0x173:
        return 0;
    case 0x174:
        return 0;
    case 0x175:
        return 0;
    case 0x176:
        return 0;
    case '1':
        return 0x17;
    case '2':
        return 0x18;
    case '0':
        return 0x19;
    case '-':
        return 0x1A;
    case '+':
        return 0x1B;
    }
    return 0;
}

int fn_80008F68(int action) {
    switch (action) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 0x15:
        return 1;
    }
    return 0;
}

// Not referenced in the final game; kept for their strings.
static void DrawContinuePrompt(void) { fn_802E30C8("Press START to continue"); }

static void WarnDiverged(void) { fn_802E30C8("Replay has diverged!"); }

void fn_80008F98(void) {
    char name[80];
    Camera cam;
    f32 a;
    f32 b;
    int done;
    u32 paused;
    int resume;
    u32 key;
    f32 t;
    f32 x;
    f32 y;
    int inRange;
    u32 p;

    fn_802E30C8("debug replay!");
    fn_80176A00(1);
    fn_80278894(1);
    done = 0;
    fn_801769E0();
    fn_80176734();
    fn_802782C4();
    paused = fn_801769F8() == 0;
    resume = 0;
    while (!done) {
        p = fn_801769F8();
        if (p != paused) {
            paused = p;
            if (!p) {
                fn_80206454();
            } else {
                fn_80206444();
            }
        }
        fn_801E7E48();
        fn_802C8530();
        fn_802C87B4(0, 0, lbl_8055CFF0, lbl_8055CFF4);
        fn_801E81B8(1, 0);
        fn_802C87B4(0, 0, lbl_8055CFF0, lbl_8055CFF4);
        fn_80230B5C(&cam);
        fn_802C8638(&cam);
        fn_802C974C(1.0f, 1000.0f, 1.4398966f);
        a = 0.0f;
        b = 0.0f;
        fn_80278924(&a, &b);
        t = fn_801E7E18();
        inRange = 0;
        if (t >= a && t <= b) {
            inRange = 1;
        }
        if (!fn_802787F4() || !inRange) {
            x = 0.5f * lbl_8055CFF0 - 20.0f;
            y = 0.5f * lbl_8055CFF4 - 20.0f;
            fn_80010188(0xFFFFFFFF, 10, "%1.1f", 1.0f, x, y, t);
            y += -25.0f;
            if (fn_80278358()) {
                fn_80010188(0xFFFF0000, 10, "R", 1.0f, x, y);
            }
            y += -25.0f;
            if (!fn_80278358()) {
                fn_80010188(0xFF8040FF, 10, "A: %1.1f", 1.0f, x, y, a);
            }
            y += -25.0f;
            if (!fn_80278358()) {
                fn_80010188(0xFF4080FF, 10, "B: %1.1f", 1.0f, x, y, b);
            }
        }
        fn_802C855C();
        fn_80211F64();
        fn_80129AFC();
        fn_802C2B48();
        if (resume) {
            resume = 0;
            fn_801769E0();
        }
        if (fn_802E47B8()) {
            key = fn_802E46F0();
            switch ((u16)key) {
            case 0x1B:
                done = 1;
                break;
            case '1':
                fn_8027889C(t);
                fn_802E30C8("replay: set A point");
                break;
            case '2':
                fn_802788E0(t);
                fn_802E30C8("replay: set B point");
                break;
            case '+':
                fn_802E30C8("replay: aborting movie");
                fn_802787E4();
                break;
            case '-':
            case '0':
                fn_80278318();
                fn_801769E0();
                fn_80176734();
                if ((u16)key == '0') {
                    sprintf(name, "m%d_", lbl_8055D398++);
                    fn_80278778(name);
                    fn_802E30C8("replay: recording movie %sXXXX.tga", name);
                } else {
                    fn_802E30C8("replay: previewing movie");
                }
                fn_8026A1F0(0x19, 0, 1);
                break;
            case 'r':
                fn_801769E0();
                fn_80176734();
                fn_802782C4();
                break;
            case 'p':
            case 0x113:
                if (fn_801769F8()) {
                    fn_801769EC();
                } else {
                    fn_801769E0();
                }
                fn_8027888C(0);
                break;
            case 's':
                resume = 1;
                fn_8027888C(0);
                fn_801769EC();
                break;
            case 'F':
                fn_8027888C(1);
                break;
            case 'f':
                fn_8027888C(0);
                break;
            case 't':
                fn_80278894(0);
                break;
            case 'T':
                fn_80278894(1);
                break;
            case 0x121:
                fn_801E85CC(0);
                break;
            case 0x122:
                fn_801E85BC(0);
                break;
            case 0x170:
                fn_8026A1F0(4, 0, 0);
                break;
            case 0x171:
                fn_8026A1F0(0x11, 0, 0);
                fn_8026DDD8(0);
                break;
            case 0x172:
                fn_8026A1F0(0x11, 0, 0);
                fn_8026DDD8(1);
                break;
            case 0x173:
                fn_8026A1F0(0x11, 0, 0);
                fn_8026DDD8(2);
                break;
            case 0x174:
                fn_8026A1F0(0x11, 0, 0);
                fn_8026DDD8(3);
                break;
            case 0x175:
                fn_8026A1F0(0x11, 0, 0);
                fn_8026DDD8(4);
                break;
            }
        }
    }
    fn_80278894(0);
}

static inline void TouchPrompts(void) {
    int now = lbl_8055D380.time;
    if (now - lbl_8034F44C.touched >= 3000) {
        if (now - lbl_8034F44C.shown >= 600) {
            lbl_8034F44C.shown = now;
        } else {
            lbl_8034F44C.shown = 2 * now - lbl_8034F44C.shown - 600;
        }
    }
    lbl_8034F44C.touched = now;
}

static inline int PromptsVisible(void) {
    int now = lbl_8055D380.time;
    int visible = 0;
    if (now - lbl_8034F44C.touched < 3000 && now - lbl_8034F44C.shown >= 600) {
        visible = 1;
    }
    return visible;
}

void fn_80009534(void) {
    int player;
    u32 paused;
    int exiting;
    int frames;
    int newCam;
    int idle;
    int savedCam;
    int locked;
    int selection;
    u32 key;
    int handled;
    int camera;
    int now;
    int t;
    u32 q;
    Camera cam;

    lbl_8055D378[0] = 0;
    lbl_8055D378[1] = 0;
    fn_801CEFD4(0);
    fn_80137C74();
    fn_80129AEC(fn_80137CFC(0));
    player = fn_801E86F8(0);
    lbl_8055D374 = fn_80145E70("replaylyt.val");
    lbl_8055D36D = 1;
    lbl_8055D36C = 0;
    lbl_805589B0 = 1;
    lbl_8055D380.last = fn_802E2C20();
    lbl_8055D380.time = 0;
    lbl_8034F44C.shown = -600;
    lbl_8034F44C.touched = 0;
    lbl_8034F44C.visible = 1;
    lbl_8055D370 = 0;
    fn_80176A00(1);
    fn_80179588();
    fn_80176734();
    paused = 1;
    exiting = 0;
    frames = 0;
    newCam = 0;
    idle = fn_802E2C20() - 2000;
    savedCam = fn_8026DDD0();
    locked = 0;
    lbl_8055D37A = 0;
    selection = 0;
    if (!fn_801CB8FC()) {
        selection = 1;
    }
    fn_80007210();
    fn_801E7E04(0, 0);
    fn_802E2C20();
    while (!exiting || frames < 60) {
        fn_80008390();
        if (exiting) {
            frames++;
        } else if (fn_8018FD38() && fn_802E2C20() > idle + 2000) {
            fn_801769E0();
            fn_80176734();
            idle = fn_802E2C20();
        }
        key = fn_801769F8();
        if (key != paused) {
            paused = key;
            if (!key) {
                fn_80206454();
            } else {
                fn_80206444();
            }
        }
        q = fn_802ECC10();
        if (q != lbl_8055D39C) {
            lbl_8055D39C = q;
            if (q) {
                if (!fn_801769F8()) {
                    lbl_8055D39D = 1;
                    fn_801769E0();
                } else {
                    lbl_8055D39D = 0;
                }
            } else if (lbl_8055D39D) {
                fn_801769EC();
            }
        }
        if (fn_802E47B8()) {
            key = fn_802E46F0();
            if (fn_802ECC10() || fn_80137C98(fn_802E4A10()) < 0) {
                fn_802E30C8("IGNORE KEY");
                key = 0;
            }
            if ((u16)key == 'T') {
                fn_80008F98();
            }
            if (!exiting && lbl_8055D37A) {
                if ((u16)key) {
                    TouchPrompts();
                }
                switch ((u16)key) {
                case 0x126:
                    if (selection > 0) {
                        selection--;
                    }
                    fn_80230E90(1);
                    break;
                case 0x128:
                    if (selection < 2) {
                        selection++;
                    }
                    fn_80230E90(1);
                    break;
                case 0x1B:
                    lbl_8055D37A = 0;
                    break;
                case '\r':
                    if (selection == 2) {
                        lbl_8055D37A = 0;
                    } else {
                        fn_8017691C();
                        exiting = 1;
                    }
                    break;
                }
            } else {
                if (fn_801E8838()) {
                    key = 0;
                }
                key = fn_80008DBC((u16)key);
                if (key && !fn_80008F68(key)) {
                    TouchPrompts();
                    if (!PromptsVisible()) {
                        key = 0;
                    }
                }
                if (locked && key - 2 <= 3) {
                    key = 0;
                }
                if (key == 0x12) {
                    key = 0x10;
                    if (!lbl_8055D370) {
                        key = 0x11;
                    }
                }
                if (!fn_800085D8(key)) {
                    key = 0;
                }
                handled = key != 0;
                switch (key) {
                case 0:
                    break;
                case 2:
                case 3:
                case 4:
                case 5:
                    handled = 0;
                    break;
                case 6:
                    break;
                case 7:
                    locked = !locked;
                    newCam = 0;
                    break;
                case 8:
                    newCam = 1;
                    locked = 1;
                    fn_8026DDD8(0);
                    break;
                case 9:
                    newCam = 1;
                    locked = 1;
                    fn_8026DDD8(2);
                    break;
                case 0xA:
                    newCam = 1;
                    locked = 1;
                    fn_8026DDD8(1);
                    break;
                case 0xB:
                    fn_80008618(1);
                    break;
                case 0xC:
                    fn_80008618(2);
                    break;
                case 0xD:
                    fn_80008618(4);
                    break;
                case 0xE:
                    break;
                case 0xF:
                    if (GetNumPlayers() > 1) {
                        fn_801E85DC(0);
                    }
                    break;
                case 0x13:
                    lbl_8055D37A = 1;
                    break;
                case 0x14:
                    lbl_8055D36C = 0;
                    idle = fn_802E2C20();
                    fn_801769E0();
                    fn_80176734();
                    break;
                case 0x15:
                    if (fn_801769F8()) {
                        fn_801E882C();
                        fn_801769EC();
                    } else {
                        fn_801769E0();
                    }
                    break;
                case 0x12:
                    lbl_805589B0 = !lbl_805589B0;
                    break;
                case 0x10:
                    lbl_8055D370 = 0;
                    break;
                case 0x11:
                    lbl_8055D370 = 1;
                    break;
                case 0x16:
                case 0x17:
                case 0x18:
                case 0x19:
                case 0x1A:
                case 0x1B:
                    break;
                default:
                    fn_802E317C("control-stream replay - action %d undefined!", key);
                    break;
                }
                if (handled) {
                    fn_80012158();
                }
            }
        }
        camera = fn_80008C5C();
        fn_802C8530();
        fn_802C87B4(0, 0, lbl_8055CFF0, lbl_8055CFF4);
        fn_801E7E48();
        fn_8026A1F0(camera, 0, newCam);
        newCam = 0;
        fn_801E81B8(1, 0);
        fn_802E2C20();
        fn_802C87B4(0, 0, lbl_8055CFF0, lbl_8055CFF4);
        fn_80230B5C(&cam);
        fn_802C8638(&cam);
        fn_802C974C(1.0f, 1000.0f, 1.4398966f);
        if (exiting) {
            DrawFade2(frames / 59.0f);
            fn_80206444();
        }
        fn_802C9A70(0);
        now = fn_802E2C20();
        if (!fn_802ECC10()) {
            lbl_8055D380.time += now - lbl_8055D380.last;
        }
        lbl_8055D380.last = now;
        t = lbl_8055D380.time;
        if (t - lbl_8034F44C.shown >= 600) {
            int d = t - lbl_8034F44C.touched;
            if (d > 3000 && d < 3600) {
                lbl_8034F44C.shown = t;
            }
        }
        if (!fn_802787F4()) {
            if (lbl_8055D37A) {
                fn_800073D8(selection);
            } else {
                fn_80008684(camera);
            }
        }
        if (!lbl_8055D37A) {
            fn_80007318(0);
        }
        if (fn_801E8838()) {
            fn_802C942C(fn_801E8838() << 24);
        }
        fn_802C855C();
        fn_80211F64();
        fn_80129AFC();
        fn_802C2B48();
    }
    fn_80206444();
    fn_801795B0();
    fn_801E86E8(player, 0);
    fn_8026DDD8(savedCam);
    if (lbl_8055D374) {
        delete lbl_8055D374;
    }
    lbl_8055D374 = 0;
    lbl_8055D36D = 0;
    fn_801CEFD4(1);
    lbl_8055D378[0] = selection == 1;
    lbl_8055D378[1] = selection == 2;
}

void fn_80009C80(void) { fn_80009534(); }

u8 fn_80009C84(void) { return lbl_8055D36D; }

int fn_80009C8C(void) {
    int result = 0;
    if (!lbl_8055D36D && lbl_8055D378[0]) {
        result = 1;
    }
    lbl_8055D378[0] = 0;
    return result;
}

int fn_80009CB8(void) {
    int result = 0;
    if (!lbl_8055D36D && lbl_8055D378[1]) {
        result = 1;
    }
    lbl_8055D378[1] = 0;
    return result;
}
