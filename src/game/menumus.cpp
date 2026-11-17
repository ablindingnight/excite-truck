// Menu music (0x8020C140-0x8020CA28): the track's music, the pre-race and post-race songs streamed as
// three sounds, which one plays depending on loading and race state, the goal-line sting after a race,
// and the master volume fade. Names are guesses apart from the file names.
#include "game/common.h"
#include "game/sound.h"
#include "game/mathutil.h"

BOOL IsLoadingActive(void);
BOOL IsLoadingFading(void);

extern "C" {
int fn_801345D8(char* buf, int size, const char* fmt, ...);  // snprintf
f32 fn_8020A5F0(void);                                       // music volume
void fn_80210510(f32 volume);
void fn_8021037C(void);
void fn_802103BC(void);
void fn_80210444(void);
u8 fn_802104B8(void);
void fn_80210688(int a);
}  // extern "C"
u8 fn_8000D75C(void);
extern "C" {
}  // extern "C"
int fn_80009C84(void);
extern "C" {
int fn_801B85CC(void);   // race over
int fn_801E8838(void);
int fn_8012E938(const char* name);  // resource ready
void fn_8012E81C(const char* name);
u8 fn_80128690(void);
void fn_80128684(void);
void fn_80128678(void);
void fn_802064D0(const char* name);
void fn_8012E498(const char* name);
void fn_8012DDA8(const char* name);
s32 fn_802E2C20(void);  // milliseconds
}

extern "C" Sound2D* fn_80207848(const char* name, int flags);

// Unit-local (port ODR: other units define a different MusicTrack).
namespace {
struct MusicTrack {
    char name[0x20];  // offset 0x0
    Sound2D* snd;     // offset 0x20
};
}  // namespace

static inline f32 Ramp(f32 x, f32 lo, f32 range) { return (x - lo) / range; }

ModuleTag lbl_8055EE30;
Sound2D* lbl_8055EE38;  // goal line sting
s32 lbl_8055EE3C;       // race end time
u8 lbl_8055EE40;        // paused
u8 lbl_8055EE41;        // track song loaded
u8 lbl_8055EE42;        // pre-race song loaded
u8 lbl_8055EE43;        // post-race song loaded
u8 lbl_8055EE44;        // race over
u8 lbl_8055EE45;        // loading seen
u8 lbl_8055EE46;
f32 lbl_8055EE48;  // music volume
f32 lbl_8055EE4C;  // fade
f32 lbl_8055EE50;  // fade target
f32 lbl_8055EE54;  // post-race duck
u8 lbl_8055EE58;   // muted
MusicTrack lbl_80489BE0[3];
char lbl_80489C50[0x20];  // track song
const char* lbl_8055B5D0 = "mus_prerace.sng";
const char* lbl_8055B5D4 = "mus_postrace.sng";

extern "C" {

#pragma push
#pragma ppc_unroll_instructions_limit 40
void fn_8020C140(void) {
    f32 v = lbl_8055EE48 * lbl_8055EE4C;
    if (lbl_8055EE58) {
        if (lbl_8055EE40) {
            v = 0.2f * v;
        }
        fn_80210510(v);
        for (int i = 0; i < 3; i++) {
            if (lbl_80489BE0[i].snd) {
                lbl_80489BE0[i].snd->SetVolume(0.0f);
            }
        }
    } else {
        f32 music = v * lbl_8055EE54;
        if (lbl_8055EE46) {
            music *= (255 - fn_801E8838()) / 255.0f;
        }
        fn_80210510(0.0f);
        for (int i = 0; i < 3; i++) {
            if (lbl_80489BE0[i].snd) {
                lbl_80489BE0[i].snd->SetVolume(lbl_8055EE40 ? 0.2f * music : music);
            }
        }
    }
}
#pragma pop

/* Helper name unproven */
struct HeapGuard {
    u8 open;
    HeapGuard() {
        open = fn_80128690();
        if (!open) {
            fn_80128684();
        }
    }
    ~HeapGuard() {
        if (!open) {
            fn_80128678();
        }
    }
};

void fn_8020C2AC(void) {
    lbl_8055EE40 = 0;
    lbl_8055EE48 = fn_8020A5F0();
    lbl_8055EE4C = 1.0f;
    lbl_8055EE50 = 1.0f;
    lbl_8055EE54 = 1.0f;
    MusicTrack* t = lbl_80489BE0;
    for (int i = 0; i < 3; i++, t++) {
        const char* song = NULL;
        u8* loaded = NULL;
        switch (i) {
        case 0:
            song = lbl_80489C50;
            loaded = &lbl_8055EE41;
            break;
        case 1:
            song = lbl_8055B5D0;
            loaded = &lbl_8055EE42;
            break;
        case 2:
            song = lbl_8055B5D4;
            loaded = &lbl_8055EE43;
            break;
        }
        if (!*loaded && fn_8012E938(song)) {
            fn_8012E81C(song);
            HeapGuard guard;
            fn_802064D0(lbl_80489C50);
            for (int part = 0; part < 1; part++) {
                if (t->name[0]) {
                    t->snd = fn_80207848(t->name, 0);
                }
                if (t->snd) {
                    t->snd->SetPan(0.0f);
                    t->snd->SetDepth(0.0f);
                }
            }
            *loaded = 1;
        }
    }
    fn_8020C140();
}

void fn_8020C41C(void) {
    fn_8021037C();
    lbl_8055EE38 = fn_80207848("goalline.sfx", 1);
    lbl_8055EE45 = 0;
    lbl_8055EE46 = 0;
}

void fn_8020C45C(void) {
    if (lbl_8055EE38) {
        delete lbl_8055EE38;
    }
    lbl_8055EE38 = NULL;
    fn_802103BC();
    MusicTrack* t = lbl_80489BE0;
    for (int i = 0; i < 3; i++, t++) {
        if (t->snd) {
            t->snd->Stop();
            delete t->snd;
            t->snd = NULL;
        }
    }
    fn_8012E498(lbl_80489C50);
    fn_8012E498(lbl_8055B5D4);
    fn_8012E498(lbl_8055B5D0);
    lbl_8055EE43 = 0;
    lbl_8055EE42 = 0;
    lbl_8055EE41 = 0;
}

void fn_8020C54C(void) {
    fn_80210444();
    lbl_8055EE4C = 1.0f;
    lbl_8055EE50 = 1.0f;
    lbl_8055EE54 = 1.0f;
    lbl_8055EE48 = fn_8020A5F0();
    lbl_8055EE40 = 0;
    lbl_8055EE44 = 0;
    lbl_8055EE3C = 0;
    lbl_8055EE45 = 0;
    lbl_8055EE46 = 0;
}

void fn_8020C59C(void) {
    if (!lbl_8055EE41 || !lbl_8055EE43 || !lbl_8055EE42) {
        fn_8020C2AC();
    }
    lbl_8055EE58 = fn_802104B8();
    lbl_8055EE40 = fn_8000D75C();
    int loading;
    int over;
    MusicTrack* t;
    int pre;
    int track;
    loading = 0;
    if (IsLoadingActive() && !fn_80009C84()) {
        loading = 1;
    }
    pre = loading;
    over = 1;
    if (!fn_801B85CC() && !fn_80009C84()) {
        over = 0;
    }
    if (!lbl_8055EE45) {
        lbl_8055EE45 = loading;
    } else {
        lbl_8055EE46 = 1;
        pre = lbl_8055EE45;
    }
    track = 0;
    if (pre) {
        track = 1;
    } else if (over) {
        track = 2;
    }
    if (loading && IsLoadingFading()) {
        track = -1;
    }
    lbl_8055EE54 = 1.0f;
    if (over && !lbl_8055EE44) {
        lbl_8055EE44 = 1;
        if (!fn_80009C84()) {
            lbl_8055EE3C = fn_802E2C20();
            if (lbl_8055EE38) {
                lbl_8055EE38->Play();
            }
        }
    } else if (!over) {
        if (lbl_8055EE38) {
            lbl_8055EE38->Stop();
        }
    } else if (!fn_80009C84()) {
        s32 now = fn_802E2C20();
        if (now >= lbl_8055EE3C && now < lbl_8055EE3C + 4000) {
            track = 0;
            f32 k = Ramp(now - lbl_8055EE3C, 0.0f, 1000.0f);
            k = MaxF(MinFRev(k, 1.0f), 0.0f);
            lbl_8055EE54 = Lerp(1.0f, 0.0f, k);
        }
    }
    f32 d = MaxF(MinFRev(lbl_8055EE50 - lbl_8055EE4C, 0.2f), -0.2f);
    lbl_8055EE4C = MaxF(MinFRev(lbl_8055EE4C + d, 1.0f), 0.0f);
    fn_8020C140();
    t = lbl_80489BE0;
    for (int i = 0; i < 3; i++, t++) {
        if (i == track) {
            int sting = 0;
            if (lbl_8055EE44 && track == 2 && !fn_80009C84()) {
                s32 now = fn_802E2C20();
                if (now >= lbl_8055EE3C && now < lbl_8055EE3C + 4000) {
                    sting = 1;
                }
            }
            if (t->snd && !sting) {
                t->snd->PlayLoop();
                t->snd->SetPitch(1.0f);
            }
        } else if (t->snd) {
            t->snd->Stop();
        }
    }
    fn_80210688(0);
}

void fn_8020C8F4(u32) {}

void fn_8020C8F8(void) {}

void fn_8020C8FC(void) {}

void fn_8020C900(u8 paused) { lbl_8055EE40 = paused; }

void fn_8020C908(f32 volume) {
    lbl_8055EE48 = volume;
    fn_8020C140();
}

void fn_8020C910(f32 target) { lbl_8055EE50 = target; }

void fn_8020C918(int) {}

void fn_8020C91C(int) {}

void fn_8020C920(const char* track) {
    MusicTrack* t;
    const char* name = track;
    int i;
    int alt;
    fn_801345D8(lbl_80489C50, 0x20, "mus_%s.sng", track);
    fn_8012DDA8(lbl_80489C50);
    fn_8012DDA8(lbl_8055B5D0);
    fn_8012DDA8(lbl_8055B5D4);
    t = lbl_80489BE0;
    for (i = 0; i < 3; i++, t++) {
        for (alt = 0; alt < 1; alt++) {
            switch (i) {
            case 0:
                fn_801345D8(t->name, 0x20, "mus_%s%s.sfx", name, alt == 1 ? ".2" : "");
                break;
            case 1:
                fn_801345D8(t->name, 0x20, "mus_prerace.sfx");
                break;
            case 2:
                fn_801345D8(t->name, 0x20, "mus_postrace.sfx");
                break;
            }
        }
    }
}

}  // extern "C"

// NonMatching: fn_8020C2AC still issues the hoisted constant 1 one store later than the target; case 2
// there loads song before the loaded flag (swapping it here fixes that row but lowers objdiff's score until
// the constant also moves). fn_8020C140 is now exact.
