// Menu music (0x80209FFC-0x8020A6AC): the two streamed menu songs (mus_ui and mus_credits), the
// music volume setting, and the crossfade when the song changes. Names are guesses apart from the
// file names, keys and messages.
#include "game/common.h"

class MusicVoice {
public:
    void SetVolume(f32 v) {
        if (v != volume) {
            volumeChanged = 1;
            volume = v;
        }
    }
    void SetUnk64(f32 v) {
        if (v != unk64) {
            unk64Changed = 1;
            unk64 = v;
        }
    }
    void SetUnk68(f32 v) {
        if (v != unk68) {
            unk68Changed = 1;
            unk68 = v;
        }
    }
    void Play(void) {
        unk4A = 0;
        unk4B = 1;
        unk49 = 1;
    }
    void Stop(void) {
        unk4A = 1;
        unk4C = 0;
    }

    u8 pad0[0x4];
    f32 pitch;   // offset 0x4
    f32 volume;  // offset 0x8
    u8 padC[0x3D];
    u8 unk49;          // offset 0x49
    u8 unk4A;          // offset 0x4A
    u8 unk4B;          // offset 0x4B
    u8 unk4C;          // offset 0x4C
    u8 volumeChanged;  // offset 0x4D
    u8 pitchChanged;   // offset 0x4E
    u8 pad4F[0x15];
    f32 unk64;         // offset 0x64
    f32 unk68;         // offset 0x68
    u8 unk64Changed;   // offset 0x6C
    u8 unk68Changed;   // offset 0x6D
};

void* operator new[](size_t size, const char* tag);
void IdentRegisterCallback(void (*cb)(int, int));

extern "C" {
int sprintf(char* buf, const char* fmt, ...);
void fn_802E30C8(const char* fmt, ...);  // debug print
s32 fn_802E2C20(void);                   // milliseconds
void fn_801331C4(int owner, const char* key, f32* value);
void fn_8013323C(int owner, const char* key, f32 value);
int fn_8013365C(const char* name, void (*cb)(void));
void fn_801336E0(int handle, void (*cb)(void));
void fn_80127C90(void (*fn)(void), const char* name);
void fn_80127D58(void (*fn)(void));
int fn_8012E938(const char* name);
void fn_8012E81C(const char* name);
void fn_8012DDA8(const char* name);
void fn_8012E498(const char* name);
MusicVoice* fn_802078CC(const char* name, int flags, u8* buf, int size);
void fn_80207924(MusicVoice* v);
}


static inline f32 Max(f32 a, f32 b) { return (a > b) ? a : b; }
static inline f32 Ramp(f32 x, f32 a, f32 b) { return (x - a) / (b - a); }
static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

struct MusicTrack {
    MusicVoice* voice;  // offset 0x0
    u8* buf;            // offset 0x4
};

ModuleTag lbl_8055EDC8;
int lbl_8055B550 = -1;          // settings callback handle
u8 lbl_8055EDD0;                // active
u8 lbl_8055EDD4[2];             // song loaded
f32 lbl_8055EDD8;               // volume
u8 lbl_8055EDDC;                // playing
int lbl_8055EDE0;               // current song
volatile int lbl_8055EDE4;      // requested song
s32 lbl_8055EDE8;               // time of the request
const char* lbl_8055B560[2] = {"mus_ui", "mus_credits"};
MusicTrack lbl_804892C0[2];

extern "C" {

f32 fn_8020A5F0(void);
void fn_8020A5F8(f32 v);
void fn_8020A634(void);

void fn_80209FFC(void) {
    f32 v = fn_8020A5F0();
    fn_801331C4(4, "music_volume", &v);
    fn_8020A5F8(v);
}

void fn_8020A03C(int ident, int event) {
    f32 v = 0.7f;
    switch (event) {
    case 3:
        fn_801331C4(ident + 0xC, "player_music_vol", &v);
        fn_8013323C(4, "music_volume", v);
        fn_8020A5F8(v);
        break;
    case 1:
    case 2:
        fn_801331C4(4, "music_volume", &v);
        fn_8013323C(ident + 0xC, "player_music_vol", v);
        break;
    case 0:
        fn_8013323C(4, "music_volume", v);
        fn_8020A5F8(v);
        break;
    }
}

void fn_8020A100(void) {
    if (lbl_8055EDD0) {
        f32 fade = 1.0f;
        s32 dt = fn_802E2C20() - lbl_8055EDE8;
        if (lbl_8055EDE0 != lbl_8055EDE4) {
            if (dt >= 400 || dt < 0 || lbl_8055EDE0 == -1) {
                lbl_8055EDE0 = lbl_8055EDE4;
            } else {
                f32 t = MIN(1.0f, Ramp((f32)dt / 400.0f, 0.0f, 0.7f));
                fade = Lerp(1.0f, 0.0f, Max(t, 0.0f));
            }
        }
        for (int i = 0; i < 2; i++) {
            MusicTrack* t = &lbl_804892C0[i];
            int part;
            char buf[0x40];
            if (!lbl_8055EDD4[i]) {
                sprintf(buf, "%s.sng", lbl_8055B560[i]);
                if (fn_8012E938(buf)) {
                    fn_8012E81C(buf);
                    for (part = 0; part < 1; part++) {
                        sprintf(buf, "%s%s.sfx", lbl_8055B560[i], part == 1 ? ".2" : "");
                        t->voice = fn_802078CC(buf, 0, t->buf, 0x70);
                        fn_802E30C8("music: track %d.%d (%s) is loaded (%p)", i, part, buf, t->voice);
                    }
                    lbl_8055EDD4[i] = 1;
                }
            }
            MusicVoice* v = t->voice;
            if (v) {
                v->SetVolume(lbl_8055EDD8 * fade);
                v->SetUnk64(0.0f);
                v->SetUnk68(0.0f);
                if (i == lbl_8055EDE0 && lbl_8055EDDC) {
                    v->Play();
                } else {
                    v->Stop();
                }
            }
        }
    }
}

void fn_8020A368(void) {
    for (int i = 0; i < 2; i++) {
        lbl_804892C0[i].buf = new ("mus_buf") u8[0x70];
    }
    lbl_8055EDDC = 0;
    lbl_8055EDD8 = 0.7f;
    fn_8013323C(4, "music_volume", 0.7f);
    lbl_8055B550 = fn_8013365C("Music", fn_80209FFC);
    IdentRegisterCallback(fn_8020A03C);
}

void fn_8020A400(void) {
    fn_8020A634();
    fn_801336E0(lbl_8055B550, fn_80209FFC);
    lbl_8055B550 = -1;
    for (int i = 0; i < 2; i++) {
        delete[] lbl_804892C0[i].buf;
        lbl_804892C0[i].buf = NULL;
    }
}

void fn_8020A47C(void) {
    fn_802E30C8("music: begin UI!");
    lbl_8055EDE0 = 0;
    lbl_8055EDD0 = 1;
    lbl_8055EDE4 = 0;
    lbl_8055EDE8 = 0;
    for (int i = 0; i < 2; i++) {
        char buf[0x40];
        sprintf(buf, "%s.sng", lbl_8055B560[i]);
        lbl_8055EDD4[i] = 0;
        fn_802E30C8("music: loading %s...", buf);
        fn_8012DDA8(buf);
    }
    fn_80127C90(fn_8020A100, "music");
    char buf[0x40];
    sprintf(buf, "%s.sng", lbl_8055B560[0]);
    fn_8012E81C(buf);
}

void fn_8020A550(void) {
    fn_80127D58(fn_8020A100);
    lbl_8055EDD0 = 0;
    for (int i = 0; i < 2; i++) {
        if (lbl_804892C0[i].voice) {
            fn_80207924(lbl_804892C0[i].voice);
            lbl_804892C0[i].voice = NULL;
        }
        char buf[0x40];
        sprintf(buf, "%s.sng", lbl_8055B560[i]);
        fn_8012E498(buf);
        lbl_8055EDD4[i] = 0;
    }
}

f32 fn_8020A5F0(void) { return lbl_8055EDD8; }

void fn_8020A5F8(f32 v) { lbl_8055EDD8 = v; }

void fn_8020A600(void) {
    fn_802E30C8("music: play!");
    lbl_8055EDDC = 1;
}

void fn_8020A634(void) { lbl_8055EDDC = 0; }

u8 fn_8020A640(void) { return lbl_8055EDDC; }

void fn_8020A648(int song) {
    if (song != lbl_8055EDE4) {
        fn_802E30C8("music: setting song idx to %d", song);
        lbl_8055EDE4 = song;
        lbl_8055EDE8 = fn_802E2C20();
    }
}

int fn_8020A69C(void) { return lbl_8055EDE0; }

}  // extern "C"

// NonMatching: fn_8020A100 gives the clamp's second int-to-float conversion its own stack temp and
// hoists the loop's 1/0 constants before the string addresses (7 diffs).
