#include <revolution/AX/AXPB.h>

// Code 0x802E4D84-0x802E6B48, imported verbatim from the upstream project (github.com/orangedude27/excite-truck,
// CC0; file src/auto/text_802E4D84_.c), where its functions match exactly. Unit name is provisional.
// CLEANUP: automated-decompiler source with raw offsets and placeholder types; rewrite with real types
// before the PC port. Tricks are tagged MATCHING-AID.
// This is the game's Spu sound layer. It was C++ (it uses delete[], and bool results are stored unmasked), so it is
// compiled as C++ with C names until the unit is renamed.
#pragma cplusplus on
extern "C" {

__declspec(section ".sdata") extern unsigned char lbl_8055FF94[];  // holds the voice table pointer
#define SPU_VOICES (*(unsigned char **)lbl_8055FF94)
extern unsigned char *lbl_8055FF90;                                  // sample table (256 x 0xB0)

// DSP ADPCM header, as written by the SDK's DSPADPCM tool.
typedef struct DSPADPCMHeader {
    unsigned int numSamples;
    unsigned int numNibbles;
    unsigned int sampleRate;
    unsigned short loopFlag;
    unsigned short format;
    unsigned int loopStart;
    unsigned int loopEnd;
    unsigned int currentAddr;
    unsigned short coef[16];
    unsigned short gain;
    unsigned short predScale;
    unsigned short yn1;
    unsigned short yn2;
    unsigned short loopPredScale;
    unsigned short loopYn1;
    unsigned short loopYn2;
    unsigned short pad[11];
} DSPADPCMHeader;

// A cached sample (the table holds 256). Field names follow their use here and in the voice code.
typedef struct SpuSample {
    char name[0x20];
    int locked;           // 0x20
    bool adpcm;           // 0x24
    bool loop;            // 0x25
    DSPADPCMHeader adpcmInfo; // 0x28
    int (*fill)(void *dst, int size, int arg); // 0x88 streaming samples: refills the buffer
    int fillArg;          // 0x8C
    unsigned char *data;  // 0x90 streaming samples: the buffer
    int refs;             // 0x94
    int users;            // 0x98
    unsigned int addr;    // 0x9C (sample data address in ARAM/MEM)
    unsigned int size;    // 0xA0 (bytes, rounded up to 64)
    float length;         // 0xA4 (seconds)
    float rate;           // 0xA8 (relative to 22050 Hz)
    struct SpuSampleRes *res; // 0xAC
} SpuSample;

// Header of an SFX0 sample resource.
typedef struct SpuSampleRes {
    unsigned int size;    // bytes of sample data
    unsigned char *data;  // offset in the file, a pointer once loaded
    unsigned char loop;
    unsigned char format; // 1 = DSP ADPCM
    unsigned short padA;
    unsigned short magic; // 0xCFFE
    unsigned short unkE;
    unsigned int rate;    // Hz
    unsigned int unk14;
    unsigned short unk18;
    unsigned short bits;  // 0x1A
    unsigned short unk1C;
    unsigned short pad1E;
    DSPADPCMHeader adpcmInfo;
} SpuSampleRes;

#define SPU_SAMPLES ((SpuSample *)lbl_8055FF90)

// One of the Spu's 32 voices.
typedef struct SpuVoice {
    int unk0;
    void *vpb;            // 0x04 AXVPB
    float volume;         // 0x08
    float pan;            // 0x0C -1 (left) .. 1 (right)
    float pitch;          // 0x10
    float depth;          // 0x14 -1 (back) .. 1 (front)
    unsigned int remotes; // 0x18 Wii Remote speakers to play on (bit per remote)
    SpuSample *sample;    // 0x1C
    unsigned char active; // 0x20
    unsigned char start;  // 0x21 start requested
    unsigned char startAck; // 0x22
    unsigned char play;   // 0x23 should be playing
    unsigned char wasPlaying; // 0x24 play as of the last update
    unsigned char loop;   // 0x25
    char pad26[0x28 - 0x26];
    unsigned int lastPos; // 0x28 playback position at the last refill check
} SpuVoice;

#define SPU_VOICE_TABLE ((SpuVoice *)SPU_VOICES)

extern char lbl_8055FFB5;
extern int lbl_8055FF8C;  // output mode: 0/1 stereo, 2 surround
extern void fn_802E5264(int, int);
extern float lbl_804FC680[];
extern int lbl_8055D1EC;
extern int lbl_8055D1F0;
extern void fn_802E7770(void);
extern int fn_802E7774(void (*arg0)(void));
extern void fn_802E2E0C(void *arg0, void *arg1);
extern int TRKReadBuffer_ui32(void);
extern float fn_802E9A2C(void);
extern void fn_80060CC0(int, int, void (*)());
extern void fn_802E30C8(const char *fmt, ...);  // Spu report
extern void fn_802E5248(int, int);
void fn_802E51D0(int chan, int result);
__declspec(section ".sdata") extern char lbl_8055D1D8[];
extern char lbl_8034D39C[];
extern char lbl_8034D3C0[];
extern unsigned char lbl_804FC800[];
// Per-remote speaker state (4 remotes).
typedef struct SpuRemote {
    unsigned char ready;
    unsigned char wenc[0x20];  // 0x01 WENC encoder state
    char pad21[3];
    int restart;               // 0x24 restart the encoder with the next block
    int unk28;
} SpuRemote;
#define SPU_REMOTES ((SpuRemote *)lbl_804FC800)
extern char lbl_8034D384[];
extern char lbl_804FC558[][0x20];  // names of the loaded sound packages
extern int lbl_8055FF78;
extern int lbl_8055FFA0;
extern struct SpuSample **lbl_8055FF9C;
struct SpuSample *fn_802E5AE8(char *name);
extern void fn_8012BEF0(const char *name);
extern void fn_8012C41C(void (*)(void), int, const char *name);
extern void fn_802E589C(char *name);
extern int fn_802E582C(char *name);
__declspec(section ".sdata") extern char lbl_8055D1E4[];
extern int sprintf(char *, const char *, ...);
extern char lbl_8034D490[];
extern char lbl_8034D4A0[];
extern char lbl_8034D5B0[];
extern void fn_802E317C(const char *fmt, ...);  // Spu warning
extern int lbl_8055FF98;
extern char lbl_8034D584[];
extern char lbl_8034D528[];
extern void fn_80128670(int);
extern void fn_8012C4D8(void *res);
void fn_802E5D5C(void *arg0);
extern void fn_8012BF24(void *);
extern void *memcpy(void *, const void *, unsigned int);
extern void DCFlushRange(void *, unsigned int);
extern void DCInvalidateRange(void *, unsigned int);
extern void __dla__FPv(void *);
extern char lbl_804FC690[];  // the Spu alarm
extern void *lbl_8055FFB0;
extern unsigned int lbl_8055FF84;  // ARAM buffer size
extern char lbl_8034D210[];  // the Spu's message strings
extern void fn_801282EC(int align);
extern void fn_801282F4(void);
extern void *__nwa__FUlPCc(unsigned long size, const char *tag);  // operator new[](size, tag)
extern int OSDisableInterrupts(void);
// Copies `size` bytes to "ARAM" (main memory here) and flushes them.
void fn_802E4D84(void *dst, void *src, int size, int dir) {
    memcpy(dst, src, size);
    DCFlushRange(dst, size);
}

extern unsigned char lbl_8055FFA5;
extern int OSEnableInterrupts(void);
extern int OSRestoreInterrupts(int);
extern void *memset(void *, int, unsigned int);
extern char lbl_8034D554[];
extern char lbl_8034D578[];

// Copies at most n - 1 characters of src to dst and terminates it.
static inline void StrCopy(char *d, const char *s, int n) {
    char *p = d;
    while ((*p++ = *s++) != 0) {
        if (--n <= 0) {
            p[-1] = 0;
            break;
        }
    }
}

// Byte swaps for resources saved with the other endianness.
static inline void Swap32(void *p) {
    unsigned char *b = (unsigned char *)p;
    unsigned char t = b[0];
    b[0] = b[3];
    b[3] = t;
    t = b[1];
    b[1] = b[2];
    b[2] = t;
}

static inline void Swap16(void *p) {
    unsigned char *b = (unsigned char *)p;
    unsigned char t = b[0];
    b[0] = b[1];
    b[1] = t;
}

extern void *fn_8012C370(const char *name, unsigned int type, unsigned int *version, unsigned int *size,
                         unsigned char *first, unsigned char *swapped);
extern char lbl_8034D280[];

// Looks up the SFX0 resource `name` (version 3); the first time a byte-swapped copy is seen its header and
// 16-bit PCM data are swapped, its data offset becomes a pointer and the data is flushed for the DSP.
SpuSampleRes *fn_802E4DC8(char *name) {
    unsigned int version;
    unsigned char first;
    unsigned char swapped;
    SpuSampleRes *res;
    unsigned short *p;
    int i;

    res = (SpuSampleRes *)fn_8012C370(name, 'SFX0', &version, 0, &first, &swapped);
    if (res != 0) {
        if (version != 3) {
            fn_802E317C(lbl_8034D280, name, version, 3);
        }
        if (swapped) {
            Swap32(&res->size);
            Swap32(&res->data);
            Swap16(&res->magic);
            Swap16(&res->unkE);
            Swap32(&res->rate);
            Swap32(&res->unk14);
            Swap16(&res->unk18);
            Swap16(&res->bits);
            Swap16(&res->unk1C);
            res->data = (unsigned char *)res + (unsigned int)res->data;
            if (res->bits == 16 && res->format != 1) {
                p = (unsigned short *)res->data;
                for (i = 0; i < (int)res->size / 2; i++) {
                    Swap16(p);
                    p++;
                }
            }
            DCFlushRange(res->data, res->size);
        }
        return res;
    }
    return 0;
}
typedef struct SpuAdpcmLoop {
    unsigned short predScale;
    unsigned short yn1;
    unsigned short yn2;
} SpuAdpcmLoop;

extern unsigned char lbl_8055FFA4;
extern unsigned char lbl_8055FFA6;
extern unsigned char lbl_8055FFB4;
extern int fn_802D9BF0(void);
extern void AXSetVoiceAdpcmLoop(void *vpb, SpuAdpcmLoop *loop);

// Spu alarm: for every voice playing a streaming sample, refills the half of its buffer that playback has just
// left (with silence while muted) and flushes it for the DSP.
void fn_802E4F98(void) {
    unsigned int pos;
    int level;
    int muted;
    int i;
    SpuVoice *v;
    SpuSample *s;
    unsigned int half;
    unsigned int mid;
    SpuAdpcmLoop loop;

    if (lbl_8055FFA5 && !lbl_8055FFA6) {
        level = OSEnableInterrupts();
        muted = fn_802D9BF0();
        lbl_8055FFB4 = 1;
        for (i = 0; i < 0x20; i++) {
            v = &SPU_VOICE_TABLE[i];
            s = v->sample;
            if (v->active && s != 0 && s->locked == 1 && !v->start) {
                pos = *(unsigned int *)((char *)v->vpb + 0x1BC) << 1;  // CLEANUP: AXVPB current address
                if (s->adpcm) {
                    pos = *(unsigned int *)((char *)v->vpb + 0x1BC) >> 1;
                }
                half = s->size >> 1;
                mid = s->addr + half;
                if (pos < v->lastPos) {
                    if (!muted) {
                        s->fill(s->data + half, half, s->fillArg);
                    } else {
                        memset(s->data + half, 0, half);
                    }
                    DCFlushRange(s->data + half, half);
                }
                if (pos >= mid && v->lastPos < mid) {
                    if (!muted) {
                        s->fill(s->data, half, s->fillArg);
                    } else {
                        memset(s->data, 0, half);
                    }
                    if (s->adpcm) {
                        loop.predScale = *s->data;
                        loop.yn1 = 0;
                        loop.yn2 = 0;
                        AXSetVoiceAdpcmLoop(v->vpb, &loop);
                    }
                    DCFlushRange(s->data, half);
                }
                v->lastPos = pos;
            }
        }
        lbl_8055FFA4 = 1;
        lbl_8055FFB4 = 0;
        OSRestoreInterrupts(level);
    }
}

// Converts a 0..1 volume to a remote-speaker mix level (0..0x8000), rounding to nearest.
unsigned short fn_802E5168(float volume) {
    float value = 32768.0f * volume;
    int converted = (int)(value < 0.0f ? value - 0.5f : value + 0.5f);
    int result = converted & ((-converted & ~converted) >> 31);  // negative -> 0
    return result < 0x8000 ? result : 0x8000;
}

// Wii Remote speaker setup, last step: the speaker is playing, so mark the remote ready and reset its encoder.
void fn_802E51D0(int chan, int result) {
    if (result == 0) {
        SpuRemote *r = &SPU_REMOTES[chan];
        r->ready = 1;
        r->restart = 1;
        r->unk28 = 0;
        memset(r->wenc, 0, sizeof(r->wenc));
        fn_802E30C8(lbl_8034D384, chan);
    }
}

void fn_802E5248(int arg0, int arg1) {
    if (arg1 == 0) fn_80060CC0(arg0, 4, (void (*)())fn_802E51D0);
}

void fn_802E5264(int arg0, int arg1) {
    fn_802E30C8(lbl_8034D39C, arg0,
                 arg1 < 0 ? lbl_8034D3C0 : lbl_8055D1D8);
    lbl_804FC800[arg0 * 0x2C] = 0;
    if (arg1 >= 0) fn_80060CC0(arg0, 1, (void (*)())fn_802E5248);
}

extern int OSRestoreInterrupts(int);
extern int WPADCanSendStreamData(int);
extern unsigned char fn_802E86B4(int);
extern int AXRmtGetSamples(int, short *, int);
extern void WENCGetEncodeData(void *, int, short *, int, void *);
extern void fn_80062278(int, void *, int);
extern void AXRmtAdvancePtr(int);
extern unsigned char lbl_804FC6C0[];
extern int lbl_8055FFA8;
void fn_802E52EC(void) {
    unsigned char *entry = lbl_804FC800;
    short *samples = (short *)lbl_804FC6C0;
    int any = 0;
    int i = 0;
    int zero = 0;
    int one = 1;
    do {
        int state = OSDisableInterrupts();
        if (WPADCanSendStreamData(i) && fn_802E86B4(i) > 0x55 &&
            AXRmtGetSamples(i, samples, 0x28) == 0x28) {
            any = 1;
            if (entry[0] && (lbl_8055FFA8 & (one << i))) {
                int active = *(int *)(entry + 0x24);
                int encode = active == 0;
                if (active) *(int *)(entry + 0x24) = zero;
                WENCGetEncodeData(entry + 1, encode, samples, 0x28, &zero);
                fn_80062278(i, &zero, 0x14);
            } else {
                *(int *)(entry + 0x24) = one;
            }
        }
        OSRestoreInterrupts(state);
        i++;
        entry += 0x2C;
        samples += 0x28;
    } while (i < 4);
    if (any) AXRmtAdvancePtr(0x28);
}

int fn_802E540C(int arg0) {
    if (arg0 == 1) TRKReadBuffer_ui32();
    return 0;
}

void fn_802E6248(int arg0, float arg1);

void fn_802E5438(void) {
    int i;
    for (i = 0; i < 4; i++) {
        fn_802E6248(i, fn_802E9A2C());
    }
}

extern void *lbl_8055FF7C;
extern void *lbl_8055FF80;
extern int lbl_8055FF88;  // ARAM address of the zero area
__declspec(section ".sdata") extern char lbl_8055D1E0[];
extern void *fn_801284E8(const char *tag, unsigned int size);
extern void DCInvalidateRange(void *, unsigned int);
extern void AIInit(void *);
extern void AXInit(void);
extern void AXSetMode(int);
extern void *AXAcquireVoice(int priority, void *callback, int context);
extern void AXSetVoiceState(void *vpb, int state);
extern void AXSetVoiceType(void *vpb, int type);
extern void AXRegisterCallback(void (*)(void));
extern void fn_802E6B04(void);
extern void OSCreateAlarm(void *);
extern long long OSGetTime(void);
extern void OSSetPeriodicAlarm(void *alarm, long long start, long long period, void (*handler)());
extern void fn_802E4F98(void);
extern void fn_802E52EC(void);

#define OS_BUS_CLOCK (*(unsigned int *)0x800000F8)
#define OS_TIMER_CLOCK (OS_BUS_CLOCK / 4)
#define OSNanosecondsToTicks(nsec) (((nsec) * (OS_TIMER_CLOCK / 125000)) / 8000)

// Starts the Spu: takes an ARAM buffer, starts AI/AX, allocates the sample, voice and cache tables, acquires the
// 32 voices, clears the zero area at the end of ARAM, and starts the refill callback and the 150 Hz alarm.
void fn_802E5478(void) {
    char *msg = lbl_8034D210;  // MATCHING-AID: stands in for the pooled message literals until this unit owns its .data
    int i;
    void *vpb;
    void *zero;

    fn_802E30C8(msg + 0x1BC);
    lbl_8055FF8C = 1;
    lbl_8055FF84 = 0x20000;
    fn_801282F4();
    lbl_8055FF80 = lbl_8055FF7C = lbl_8055FFB0 = fn_801284E8(msg + 0x1C8, lbl_8055FF84);
    DCInvalidateRange(lbl_8055FF80, lbl_8055FF84);
    fn_802E30C8(msg + 0x1D4, lbl_8055FF80, lbl_8055FF84, lbl_8055FF84 >> 10);
    AIInit(0);
    AXInit();
    AXSetMode(0);
    lbl_8055FF90 = (unsigned char *)__nwa__FUlPCc(0x100 * 0xB0, msg + 0x1F4);
    SPU_VOICES = (unsigned char *)__nwa__FUlPCc(0x20 * 0x2C, msg + 0x200);
    lbl_8055FF9C = (SpuSample **)__nwa__FUlPCc(0x100 * 4, msg + 0x20C);
    lbl_8055FFA0 = 0;
    memset(lbl_8055FF90, 0, 0x100 * 0xB0);
    memset(SPU_VOICES, 0, 0x20 * 0x2C);
    for (i = 0; i < 0x20; i++) {
        vpb = AXAcquireVoice(0x1F, 0, 0);
        if (vpb == 0) {
            fn_802E317C(msg + 0x218, i);
        }
        AXSetVoiceState(vpb, 0);
        AXSetVoiceType(vpb, 0);
        SPU_VOICE_TABLE[i].unk0 = i;
        SPU_VOICE_TABLE[i].vpb = vpb;
    }
    lbl_8055FF88 = (int)lbl_8055FF7C + (lbl_8055FF84 - 0x20000);
    fn_801282EC(0x20);
    zero = __nwa__FUlPCc(0x20000, msg + 0x234);
    memset(zero, 0, 0x20000);
    fn_802E4D84((void *)lbl_8055FF88, zero, 0x20000, 1);
    __dla__FPv(zero);
    AXRegisterCallback(fn_802E4F98);
    fn_802E6B04();
    OSCreateAlarm(lbl_804FC690);
    OSSetPeriodicAlarm(lbl_804FC690, OSGetTime(), OSNanosecondsToTicks(6666667), fn_802E52EC);
    for (i = 0; i < 4; i++) {
        lbl_804FC680[i] = 1.0f;
    }
    fn_802E2E0C(fn_802E540C, lbl_8055D1E0);
    lbl_8055FFA5 = 1;
    fn_802E5438();
}

extern unsigned char lbl_8055FFA5;
extern char lbl_8034D450[];
extern char lbl_8034D470[];
extern void OSCancelAlarm(void *);
extern void AXFreeVoice(void *);
extern void AXQuit(void);
extern void AIReset(void);
extern void fn_802E6B34(void);
extern void fn_80128658(void *);
// Shuts the Spu down: stops its alarm, frees the 32 voices (reporting any still playing), reports samples still
// referenced, frees the tables and quits AX.
void fn_802E56F0(void) {
    int i;
    int j;
    OSCancelAlarm(lbl_804FC690);
    lbl_8055FFA5 = 0;
    fn_802E6B34();
    for (i = 0; i < 0x20; i++) {
        if (SPU_VOICES[i * 0x2C + 0x20] != 0) {
            fn_802E317C(lbl_8034D450, *(void **)(SPU_VOICES + i * 0x2C + 0x1C));
        }
        AXFreeVoice(*(void **)(SPU_VOICES + i * 0x2C + 4));
        *(void **)(SPU_VOICES + i * 0x2C + 4) = 0;
    }
    for (j = 0; j < 0x100; j++) {
        if (*(int *)(lbl_8055FF90 + j * 0xB0 + 0x94) > 0) {
            fn_802E317C(lbl_8034D470, lbl_8055FF90 + j * 0xB0);
        }
    }
    __dla__FPv(SPU_VOICES);
    SPU_VOICES = 0;
    __dla__FPv(lbl_8055FF90);
    lbl_8055FF90 = 0;
    __dla__FPv(lbl_8055FF9C);
    lbl_8055FF9C = 0;
    AXQuit();
    AIReset();
    lbl_8055FF84 = 0;
    if (lbl_8055FFB0 != 0) {
        fn_80128658(lbl_8055FFB0);
        lbl_8055FFB0 = 0;
    }
}

void fn_802E5824(void) {
}

void fn_802E5828(void) {
}

// Loads sample `name` into the next cache slot (256 at most); returns whether it loaded.
int fn_802E582C(char *name) {
    if (lbl_8055FFA0 < 0x100) {
        lbl_8055FF9C[lbl_8055FFA0] = fn_802E5AE8(name);
        if (lbl_8055FF9C[lbl_8055FFA0] == 0) {
            return 0;
        }
        lbl_8055FFA0++;
        return 1;
    }
    return 0;
}

// Resource callback: cache the sample named `name`.
void fn_802E589C(char *name) {
    fn_802E582C(name);
}

extern char lbl_804FC558[][0x20];  // names of the loaded sound packages
extern int lbl_8055FF78;
extern int lbl_8055FFA0;
extern struct SpuSample **lbl_8055FF9C;
struct SpuSample *fn_802E5AE8(char *name);
extern void fn_8012BEF0(const char *name);
extern void fn_8012C41C(void (*)(void), int, const char *name);
extern int fn_802E582C(char *name);
int fn_802E58A0(void *arg0, int arg1) {
    if (arg1 == 0x53465830) return fn_802E582C((char *)arg0);
    return 1;
}

void fn_802E58B8(char *name) {
    StrCopy(lbl_804FC558[lbl_8055FF78++], name, 0x20);
    fn_8012BEF0(name);
    fn_8012C41C((void (*)())fn_802E589C, 0, name);
}

// Caches the sample named by format `lbl_8055D1E4` with (arg0, arg1).
void fn_802E5950(int arg0, int arg1) {
    char name[0x100];
    sprintf(name, lbl_8055D1E4, arg0, arg1);
    fn_802E30C8(lbl_8034D490, name);
    if (fn_802E582C(name) == 0) {
        fn_802E30C8(lbl_8034D4A0, name);
    }
}

void fn_802E59BC(char *name) {
    StrCopy(lbl_804FC558[lbl_8055FF78++], name, 0x20);
    fn_8012BEF0(name);
    fn_8012C41C((void (*)())fn_802E58A0, 0, name);
}

// Releases every cached sample and unloads the sound packages.
void fn_802E5A54(void) {
    int i;
    for (i = 0; i < lbl_8055FFA0; i++) {
        fn_802E5D5C(lbl_8055FF9C[i]);
    }
    lbl_8055FFA0 = 0;
    for (i = 0; i < lbl_8055FF78; i++) {
        fn_8012BF24(lbl_804FC558[i]);
    }
    lbl_8055FF78 = 0;
}

extern char lbl_8034D210[];  // the Spu's message strings
extern char *strrchr(const char *, int);
extern int strcasecmp(const char *, const char *);
extern char *strcpy(char *, const char *);
extern void *memset(void *, int, unsigned int);
extern SpuSampleRes *fn_802E4DC8(char *name);

// Returns the cached sample for `name` (its directory stripped), adding a reference; loads it into a free slot
// when it is not cached yet.
SpuSample *fn_802E5AE8(char *name) {
    char *msg = lbl_8034D210;  // MATCHING-AID: stands in for the pooled message literals until this unit owns its .data
    char *base;
    char *slash;
    int i;
    int free;
    SpuSampleRes *res;
    SpuSample *s;
    float length;
    float rate;
    unsigned int addr;
    unsigned int size;
    unsigned short magic;

    slash = strrchr(name, '/');
    if (slash != 0) {
        base = slash + 1;
    } else {
        base = name;
    }
    free = -1;
    for (i = 0; i < 0x100; i++) {
        if (SPU_SAMPLES[i].refs != 0) {
            if (strcasecmp(base, SPU_SAMPLES[i].name) == 0 && SPU_SAMPLES[i].locked == 0) {
                SPU_SAMPLES[i].refs++;
                return &SPU_SAMPLES[i];
            }
        } else if (free < 0) {
            free = i;
        }
    }
    if (free < 0) {
        fn_802E317C(msg + 0x2A8, name);
    }
    res = fn_802E4DC8(name);
    if (res == 0) {
        return 0;
    }
    if ((magic = res->magic = 0xCFFE) != 0xCFFE) {
        fn_802E317C(msg + 0x2C4, name, magic, 0xCFFE);
    }
    size = res->size;
    length = (float)(size >> 1) / 22050.0f;
    size = (size + 0x3F) & ~0x3F;
    if (res->format == 1) {
        length *= 3.488817f;
    }
    rate = (float)res->rate / 22050.0f;
    s = &SPU_SAMPLES[free];
    memset(s, 0, sizeof(SpuSample));
    addr = (unsigned int)res->data;
    if (addr & 0x1F) {
        fn_802E317C(msg + 0x2FC, base, addr);
    }
    s->size = size;
    s->refs = 1;
    s->locked = 0;
    s->adpcm = res->format == 1;
    s->loop = res->loop;
    if (s->adpcm) {
        memcpy(&s->adpcmInfo, &res->adpcmInfo, sizeof(s->adpcmInfo));
    } else {
        memset(&s->adpcmInfo, 0, sizeof(s->adpcmInfo));
    }
    s->users = 0;
    s->rate = rate;
    s->length = length / rate;
    strcpy(s->name, base);
    s->addr = addr;
    s->res = res;
    return s;
}


extern void fn_801282EC(int align);
extern void fn_801282F4(void);
extern void *__nwa__FUlPCc(unsigned long size, const char *tag);  // operator new[](size, tag)

// Creates a streaming sample: a zeroed `size`-byte buffer in a free slot that `fill(arg)` keeps topped up.
SpuSample *fn_802E5DD8(char *name, unsigned int size, int format, int (*fill)(void *, int, int), int arg) {
    int free = -1;
    int i;
    SpuSample *s;

    for (i = 0; i < 0x100; i++) {
        if (SPU_SAMPLES[i].refs == 0) {
            free = i;
            break;
        }
    }
    if (free < 0) {
        fn_802E317C(lbl_8034D554, name);
    }
    s = &SPU_SAMPLES[free];
    memset(s, 0, sizeof(SpuSample));
    fn_801282EC(0x80);
    fn_801282F4();
    s->data = (unsigned char *)__nwa__FUlPCc(size, lbl_8034D578);
    memset(s->data, 0, size);
    s->loop = 1;
    s->addr = (unsigned int)s->data - 0x80000000;
    s->size = size;
    s->refs = 1;
    s->locked = 1;
    s->adpcm = format == 1;
    s->fill = fill;
    s->fillArg = arg;
    s->users = 0;
    s->rate = 1.0f;
    s->length = 1000.0f;
    StrCopy(s->name, name, 0x20);
    return s;
}

// Free a stream's buffer (warns if channels still use it; the stream's name is at its start).
void fn_802E5FE4(void *arg0) {
    if (*(int *)((char *)arg0 + 0x98) > 0) {
        fn_802E317C(lbl_8034D584, arg0, *(int *)((char *)arg0 + 0x98));
    }
    delete[] *(unsigned char **)((char *)arg0 + 0x90);
    *(int *)((char *)arg0 + 0x90) = 0;
    *(int *)((char *)arg0 + 0x94) = 0;
}

// Drops a reference to a cached sample, freeing its data with the last one.
void fn_802E5D5C(void *arg0) {
    if (*(int *)((char *)arg0 + 0x94) == 1 && *(int *)((char *)arg0 + 0x98) > 0) {
        fn_802E317C(lbl_8034D528, arg0, *(int *)((char *)arg0 + 0x98));
    }
    if (--*(int *)((char *)arg0 + 0x94) == 0 && *(void **)((char *)arg0 + 0xAC) != 0) {
        fn_8012C4D8(*(void **)((char *)arg0 + 0xAC));
        *(void **)((char *)arg0 + 0xAC) = 0;
    }
}

float fn_802E6040(void *arg0) {
    if (arg0 != 0) return *(float *)((char *)arg0 + 0xA4);
    return 0.0f;
}

extern unsigned char lbl_8055FF94[];
extern int OSDisableInterrupts(void);
__declspec(section ".sdata") extern unsigned char lbl_8055FF94[];
void *fn_802E6058(void *arg0) {
    unsigned char *entry;
    int i;
    i = 0;
    for (;;) {
        entry = lbl_8055FF94 + ((((i << 27) - (i >> 31)) << 5) + (i >> 31)) * 0x2C;
        if (entry[0x20] == 0) {
            OSDisableInterrupts();
            entry[0x20] = 1;
            *(void **)(entry + 0x1C) = arg0;
            *(float *)(entry + 8) = 1.0f;
            *(float *)(entry + 0xC) = 0.0f;
            *(float *)(entry + 0x14) = 1.0f;
            *(int *)(entry + 0x18) = 0;
            *(float *)(entry + 0x10) = *(float *)((char *)arg0 + 0xA8);
            entry[0x23] = 0;
            entry[0x25] = 0;
            *(int *)((char *)arg0 + 0x98) += 1;
            entry[0x21] = 1;
            entry[0x22] = 0;
            *(int *)(entry + 0x28) = 0;
            OSEnableInterrupts();
            ++lbl_8055FF98;
            return entry;
        }
        if (++i == 0x20) break;
    }
    return 0;
}

void fn_802E6138(void *arg0) {
    if (((unsigned char *)arg0)[0x23] != 0) {
        fn_802E317C(lbl_8034D5B0, *(int *)arg0, *(void **)((char *)arg0 + 0x1C));
    }
    {
        void *temp_r4 = *(void **)((char *)arg0 + 0x1C);
        *(int *)((char *)temp_r4 + 0x98) -= 1;
    }
    ((unsigned char *)arg0)[0x20] = 0;
    lbl_8055FF98 -= 1;
}

void fn_802E61A8(void *arg0) {
    ((unsigned char *)arg0)[0x25] = 0;
    ((unsigned char *)arg0)[0x23] = 1;
}

void fn_802E61BC(void *arg0) {
    if (((unsigned char *)arg0)[0x23] == 0) {
        ((unsigned char *)arg0)[0x25] = 1;
        ((unsigned char *)arg0)[0x23] = 1;
    }
}

void fn_802E61D8(void *arg0) {
    ((unsigned char *)arg0)[0x23] = 0;
}

void fn_802E61E4(void *arg0, float farg0) {
    *(float *)((char *)arg0 + 8) = farg0;
}

void fn_802E61EC(void *arg0, float farg0) {
    if (lbl_8055FF8C != 0) {
        *(float *)((char *)arg0 + 0xC) = farg0;
        return;
    }
    *(float *)((char *)arg0 + 0xC) = 0.0f;
}

void fn_802E620C(void *arg0, float farg0) {
    if (lbl_8055FF8C == 2) {
        *(float *)((char *)arg0 + 0x14) = farg0;
        return;
    }
    *(float *)((char *)arg0 + 0x14) = 1.0f;
}

void fn_802E622C(void *arg0, float farg0) { *(float *)((char *)arg0 + 0x10) = farg0; }

void fn_802E6234(void) {}

void fn_802E6238(void) {}

void fn_802E623C(void) {}

void fn_802E6240(void* arg0, int arg1) {
    *(int*)((char*)arg0 + 0x18) = arg1;
}

void fn_802E6248(int arg0, float arg1) {
    lbl_804FC680[arg0] = arg1;
}

void fn_802E625C(int mode) {
    lbl_8055FF8C = mode;
}

extern int lbl_8055D1D0;  // output mode last applied
extern int lbl_8055FFAC;
extern unsigned int lbl_804FC658[10];  // remotes used in each of the last 10 updates
extern int fn_802ECC10(void);
extern void AXSetVoiceVe(void *vpb, AXPBVE *ve);
extern void AXSetVoiceAddr(void *vpb, AXPBADDR *addr);
extern void AXSetVoiceAdpcm(void *vpb, AXPBADPCM *adpcm);
extern void AXSetVoiceSrcType(void *vpb, int type);
extern void AXSetVoiceSrc(void *vpb, AXPBSRC *src);
extern void AXSetVoiceSrcRatio(void *vpb, float ratio);
extern void AXSetVoiceMix(void *vpb, AXPBMIX *mix);
extern void AXSetVoiceRmtOn(void *vpb, int on);
extern void AXSetVoiceRmtMix(void *vpb, AXPBRMTMIX *mix);

// Per-frame voice update (from the AX callback): applies output-mode changes, stops and starts voices (setting up
// their addresses for in-memory or streaming PCM16/ADPCM samples), and sets pitch, volume, pan and remote-speaker
// mixes for playing voices; records which remotes were used over the last 10 frames.
void fn_802E6264(void) {
    char *msg = lbl_8034D210;  // MATCHING-AID: stands in for the pooled message literals until this unit owns its .data
    int muted;  // silence all voices this update
    int level;
    int i;
    SpuVoice *v;
    int wasPlaying;
    SpuSample *s;
    int starting;
    unsigned int used;
    unsigned int remotes;
    unsigned int start;
    unsigned int loop;
    unsigned int end;

    if (lbl_8055FFA4) {
        lbl_8055FFA4 = 0;
        lbl_8055FFA6 = 1;
        used = 0;
        if (lbl_8055FF8C != lbl_8055D1D0) {
            lbl_8055D1D0 = lbl_8055FF8C;
            fn_802E30C8(msg + 0x3C4);
            switch (lbl_8055FF8C) {
            case 0:
            case 1:
                AXSetMode(0);
                break;
            case 2:
                AXSetMode(2);
                break;
            default:
                fn_802E317C(msg + 0x3E0);
                break;
            }
        }
        muted = 1;
        if (!fn_802ECC10() && !fn_802D9BF0()) {
            muted = 0;
        }
        level = OSDisableInterrupts();
        for (i = 0; i < 0x20; i++) {
            v = &SPU_VOICE_TABLE[i];
            wasPlaying = v->wasPlaying;
            starting = v->start;
            v->wasPlaying = v->play;
            if ((wasPlaying && !v->play) || (starting && !v->startAck)) {
                AXPBVE ve;
                AXSetVoiceState(v->vpb, 0);
                wasPlaying = 0;
                memset(&ve, 0, sizeof(ve));
                AXSetVoiceVe(v->vpb, &ve);
            }
            if (v->active) {
                if (v->start && !v->startAck) {
                    v->startAck = 1;
                    continue;
                }
                if (v->start && v->play) {
                    v->start = 0;
                    wasPlaying = 0;
                    s = v->sample;
                    if (s->locked == 0 && !s->adpcm) {
                        AXPBADDR addr;
                        AXSetVoiceType(v->vpb, 0);
                        start = (int)s->addr / 2;
                        memset(&addr, 0, sizeof(addr));
                        addr.loopFlag = v->loop;
                        addr.format = 10;
                        if (v->loop) {
                            addr.loopAddressHi = start >> 16;
                            addr.loopAddressLo = start;
                        } else {
                            loop = lbl_8055FF88 / 2;
                            addr.loopAddressHi = loop >> 16;
                            addr.loopAddressLo = loop;
                        }
                        end = start + (int)s->size / 2 - 1;
                        addr.endAddressHi = end >> 16;
                        addr.endAddressLo = end;
                        addr.currentAddressHi = start >> 16;
                        addr.currentAddressLo = start;
                        AXSetVoiceAddr(v->vpb, &addr);
                    } else if (s->locked == 0 && s->adpcm) {
                        AXPBADDR addr;
                        AXPBADPCM adpcm;
                        AXSetVoiceType(v->vpb, 0);
                        start = s->addr * 2 + 2;
                        memset(&addr, 0, sizeof(addr));
                        addr.loopFlag = v->loop;
                        addr.format = 0;
                        if (v->loop) {
                            loop = start + s->adpcmInfo.loopStart;
                            addr.loopAddressHi = loop >> 16;
                            addr.loopAddressLo = loop;
                        } else {
                            loop = lbl_8055FF88 * 2 + 2;
                            addr.loopAddressHi = loop >> 16;
                            addr.loopAddressLo = loop;
                        }
                        end = start + s->adpcmInfo.loopEnd - 2;
                        addr.endAddressHi = end >> 16;
                        addr.endAddressLo = end;
                        addr.currentAddressHi = start >> 16;
                        addr.currentAddressLo = start;
                        memset(&adpcm, 0, sizeof(adpcm));
                        memcpy(adpcm.a, s->adpcmInfo.coef, sizeof(adpcm.a));
                        adpcm.gain = s->adpcmInfo.gain;
                        adpcm.pred_scale = s->adpcmInfo.predScale;
                        adpcm.yn1 = s->adpcmInfo.yn1;
                        adpcm.yn2 = s->adpcmInfo.yn2;
                        AXSetVoiceAddr(v->vpb, &addr);
                        AXSetVoiceAdpcm(v->vpb, &adpcm);
                    } else if (s->locked == 1 && !s->adpcm) {
                        AXPBADDR addr;
                        AXSetVoiceType(v->vpb, 0);
                        start = (int)s->addr / 2;
                        memset(&addr, 0, sizeof(addr));
                        addr.loopFlag = 1;
                        addr.format = 10;
                        addr.loopAddressHi = start >> 16;
                        addr.loopAddressLo = start;
                        end = start + (s->size >> 1) - 1;
                        addr.endAddressHi = end >> 16;
                        addr.endAddressLo = end;
                        addr.currentAddressHi = start >> 16;
                        addr.currentAddressLo = start;
                        AXSetVoiceAddr(v->vpb, &addr);
                        s->rate = 2.0f;
                    } else if (s->locked == 1) {
                        AXPBADDR addr;
                        DSPADPCMHeader hdr;
                        AXPBADPCM adpcm;
                        AXSetVoiceType(v->vpb, 1);
                        start = s->addr * 2 + 2;
                        memset(&addr, 0, sizeof(addr));
                        addr.loopFlag = 1;
                        addr.format = 0;
                        addr.loopAddressHi = start >> 16;
                        addr.loopAddressLo = start;
                        end = start + s->size * 2 - 3;
                        addr.endAddressHi = end >> 16;
                        addr.endAddressLo = end;
                        addr.currentAddressHi = start >> 16;
                        addr.currentAddressLo = start;
                        memset(&hdr, 0, sizeof(hdr));
                        s->fill(&hdr, sizeof(hdr), s->fillArg);
                        s->rate = (float)hdr.sampleRate / 22050.0f;
                        s->fill(s->data, s->size, s->fillArg);
                        DCFlushRange(s->data, s->size);
                        AXSetVoiceAddr(v->vpb, &addr);
                        memset(&adpcm, 0, sizeof(adpcm));
                        memcpy(adpcm.a, hdr.coef, sizeof(adpcm.a));
                        adpcm.gain = hdr.gain;
                        adpcm.pred_scale = hdr.predScale;
                        adpcm.yn1 = hdr.yn1;
                        adpcm.yn2 = hdr.yn2;
                        AXSetVoiceAdpcm(v->vpb, &adpcm);
                    } else {
                        fn_802E317C(msg + 0x3F8);
                    }
                    {
                        AXPBSRC src;
                        memset(&src, 0, sizeof(src));
                        src.ratioHi = 1;
                        src.ratioLo = 0;
                        src.currentAddressFrac = 0;
                        AXSetVoiceSrcType(v->vpb, 1);
                        AXSetVoiceSrc(v->vpb, &src);
                    }
                }
                if (v->play) {
                    AXPBVE ve;
                    AXPBMIX mix;
                    AXPBRMTMIX rmt;
                    float vol;
                    float left;
                    float right;
                    float front;
                    float back;
                    AXSetVoiceSrcRatio(v->vpb, muted ? 0.0f : v->sample->rate * ((22050.0f / 32000.0f) * v->pitch));
                    memset(&ve, 0, sizeof(ve));
                    vol = v->volume;
                    if (1.0f < vol) {
                        vol = 1.0f;
                    }
                    ve.currentVolume = 0.75f * (32767.0f * vol);
                    ve.currentDelta = 0;
                    if (muted) {
                        ve.currentVolume = 0;
                    }
                    AXSetVoiceVe(v->vpb, &ve);
                    memset(&mix, 0, sizeof(mix));
                    left = 1.0f - v->pan;
                    left = left < 1.0f ? left : 1.0f;
                    right = 1.0f + v->pan;
                    right = right < 1.0f ? right : 1.0f;
                    front = 1.0f + v->depth;
                    front = front < 1.0f ? front : 1.0f;
                    back = 1.0f - v->depth;
                    back = back < 1.0f ? back : 1.0f;
                    mix.vL = 32767.0f * (left * front);
                    mix.vR = 32767.0f * (right * front);
                    mix.vS = 32767.0f * (left * back);
                    mix.vAuxCL = 32767.0f * (right * back);
                    AXSetVoiceMix(v->vpb, &mix);
                    remotes = v->remotes;
                    used |= remotes;
                    if (remotes) {
                        AXSetVoiceRmtOn(v->vpb, 1);
                    } else {
                        AXSetVoiceRmtOn(v->vpb, 0);
                    }
                    memset(&rmt, 0, sizeof(rmt));
                    if (remotes & 1) {
                        rmt.vAux0 = rmt.vMain0 = fn_802E5168(lbl_804FC680[0] * v->volume);
                    }
                    if (remotes & 2) {
                        rmt.vAux1 = rmt.vMain1 = fn_802E5168(lbl_804FC680[1] * v->volume);
                    }
                    AXSetVoiceRmtMix(v->vpb, &rmt);
                }
                if (v->play && !wasPlaying) {
                    AXSetVoiceState(v->vpb, 1);
                }
            }
        }
        OSRestoreInterrupts(level);
        lbl_804FC658[lbl_8055FFAC++] = used;
        if (lbl_8055FFAC >= 10) {
            lbl_8055FFAC = 0;
        }
        for (i = 0; i < 10; i++) {
            used |= lbl_804FC658[i];
        }
        lbl_8055FFA8 = used;
        lbl_8055FFA6 = 0;
    }
}

int* fn_802E6ABC(void) {
    return &lbl_8055D1EC;
}

void fn_802E6AC4(void) {
    lbl_8055FFB5 = 1;
}

int fn_802E6AD0(int arg0) {
    if (arg0 == 1) {
        fn_802E7774(fn_802E6AC4);
    }
    return 0;
}

// MATCHING-AID: compiler pragma from upstream; revisit.
#pragma scheduling invalid_target
void fn_802E6B04(void) {
    fn_802E7770();
    fn_802E2E0C(fn_802E6AD0, &lbl_8055D1F0);
}
#pragma scheduling on
void fn_802E6B34(void) {
}

void fn_802E6B38(void* arg0, signed char* arg1) {
    *arg1 = 0;
}

// MATCHING-AID: compiler pragma from upstream; revisit.
#pragma dont_inline on
void fn_802E6B44(int arg0, int arg1) {
    fn_802E5264(arg0, arg1);
}
#pragma dont_inline reset
}
#pragma cplusplus reset
