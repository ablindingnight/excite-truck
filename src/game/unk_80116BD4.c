// Code 0x80116BD4-0x80119214 (the rest of Bink is game/unk_80119214.c, game/unk_8011AE18.c and game/unk_8011C3A8.c; split 2026-11-16: retail addresses the
// plane decoder's tables from lbl_802F65C0 = the start of another object's .rodata). Code 0x80116BD4-0x801274C0 was imported verbatim from the upstream project (github.com/orangedude27/excite-truck,
// CC0; file src/auto/text_801166C8_.c), where its functions match exactly. Unit name is provisional.
// CLEANUP: automated-decompiler source with raw offsets and placeholder types; rewrite with real types
// before the PC port. Tricks are tagged MATCHING-AID.
#include <types.h>
#include <revolution/AX/AXPB.h>
extern f64 cos(f64);
extern f64 sin(f64);
/* .data tables owned by this unit (0x80329B50-0x80329DE8), in address order; the switch jump tables of */
/* ExpandPlane follow them. */
extern const u8 lbl_802F65C0[];
extern const u8 lbl_802F65D0[];
extern const u8 lbl_802F65F0[];
extern const u8 lbl_802F6610[];
extern const u8 lbl_802F6630[];
extern const u8 lbl_802F6650[];
extern const u8 lbl_802F6670[];
extern const u8 lbl_802F6690[];
extern const u8 lbl_802F66D0[];
extern const u8 lbl_802F6710[];
extern const u8 lbl_802F6750[];
extern const u8 lbl_802F6790[];
extern const u8 lbl_802F67D0[];
extern const u8 lbl_802F6810[];
extern const u8 lbl_802F6890[];
extern const u8 lbl_802F6910[];


extern void read_rle_samples(f32 *, s32, void *, f32 *, s32);
typedef struct {
    u32 value;
    u32 *next;
    u32 count;
    u32 *start;
} BitReader8011;

// A decoder bundle: a buffer of values read from the bitstream on demand.
typedef struct Bundle8011 {
    u8 *cur;           // 0x00 next value to use
    u8 *end;           // 0x04
    u8 pad8[0x10 - 0x8];
    u8 symbols[16];    // 0x10 Huffman symbol map
    u32 maxBits;       // 0x20 Huffman code length limit
    u8 *lookup;        // 0x24 code -> (length << 4 | symbol index)
    u32 countBits;     // 0x28 width of the value count
    u8 *data;          // 0x2C
} Bundle8011;

// Reads one bit from the reader state held in value/avail/next.
#define BINK_GETBIT(bit)        \
    if (avail == 0) {           \
        bit = *next;            \
        avail = 31;             \
        next++;                 \
        value = bit >> 1;       \
    } else {                    \
        bit = value;            \
        value >>= 1;            \
        avail--;                \
    }

// Reads the escape bit; when it is clear, also reads `width` value bits into v.
#define BINK_GETESC(flag, v, width)            \
    if (avail == 0) {                          \
        bit = *next;                           \
        next++;                                \
        if (!(bit & 1)) {                      \
            v = bit >> 1;                      \
            value = bit >> ((width) + 1);      \
            avail = 31 - (width);              \
            flag = 0;                          \
        } else {                               \
            value = bit >> 1;                  \
            avail = 31;                        \
            flag = 1;                          \
        }                                      \
    } else {                                   \
        avail--;                               \
        if (!(value & 1)) {                    \
            if (avail < (width)) {             \
                bit = *next;                   \
                next++;                        \
                value >>= 1;                   \
                v = value | (bit << avail);    \
                value = bit >> ((width) - avail);\
                avail = avail + 32 - (width);  \
                flag = 0;                      \
            } else {                           \
                v = value >> 1;                \
                value >>= (width) + 1;         \
                avail -= (width);              \
                flag = 0;                      \
            }                                  \
        } else {                               \
            value >>= 1;                       \
            flag = 1;                          \
        }                                      \
    }

// Reads `width` bits into v from the reader state held in value/avail/next.
#define BINK_GETBITS(v, width)                                    \
    if (avail < (width)) {                                        \
        u32 word = *next;                                         \
        next++;                                                   \
        v = (value | (word << avail)) & (0xFFFFFFFFU >> (32 - (width))); \
        value = word >> ((width) - avail);                        \
        avail = avail + 32 - (width);                             \
    } else {                                                      \
        v = value & (0xFFFFFFFFU >> (32 - (width)));              \
        value >>= (width);                                        \
        avail -= (width);                                         \
    }


#include <revolution/ARC/arc.h>
#include <revolution/OS/OSAlarm.h>
#include <revolution/OS/OSContext.h>
#include <revolution/DVD/dvd.h>

// One decoded stream feeding a voice: callbacks plus a ring buffer of decoded samples (0x17C bytes).
typedef struct StreamTrack {
    s32 (*IsActive)(struct StreamTrack *t);                    // 0x00
    s32 (*GetBuffer)(struct StreamTrack *t, u8 **dst, u32 *len); // 0x04
    void (*Commit)(struct StreamTrack *t, u32 len);            // 0x08
    void (*SetVolume)(struct StreamTrack *t, s32 volume);     // 0x0C
    void (*SetPan)(struct StreamTrack *t, s32 pan);           // 0x10
    void (*SetPaused)(struct StreamTrack *t, s32 paused);      // 0x14
    s32 (*Run)(struct StreamTrack *t, s32 run);                // 0x18
    void (*Close)(struct StreamTrack *t);                      // 0x1C
    u8 pad20[0x28 - 0x20];
    u32 ringSize;    // 0x28
    u8 *ringStart;   // 0x2C
    u8 *ringEnd;     // 0x30
    u8 *writePtr;    // 0x34
    u8 *readPtr;     // 0x38
    s32 handle;      // 0x3C
    u32 available;   // 0x40
    s32 eightBit;    // 0x44  (16-bit data delivered as 8-bit)
    s32 deadline;    // 0x48
    u32 fillTarget;  // 0x4C  bytes to keep buffered
    u8 pad50[0x54 - 0x50];
    u32 minimum;     // 0x54
    s32 blockMask;   // 0x58
    s32 dirty;       // 0x5C  set when the voices ran dry (silence was written)
    s32 unk60;       // 0x60
    s32 playing;     // 0x64
    s32 unk68;       // 0x68
    s32 gain;        // 0x6C  0x10000 = unity
    u32 rate;        // 0x70  Hz
    s32 bits;        // 0x74
    s32 channels;    // 0x78  1 = one voice; otherwise left/right voices fed from interleaved data
    void *voices[2]; // 0x7C  AX voices (left, right)
    s16 *bufL;       // 0x84  voice buffers (bufL is also the allocation freed for both)
    s16 *bufR;       // 0x88
    u32 physL;       // 0x8C  physical addresses of the voice buffers
    u32 physR;       // 0x90
    u32 bufSize;     // 0x94  bytes of interleaved data the voice buffers hold
    u32 voiceSize;   // 0x98  bytes per voice buffer
    u32 chunk;       // 0x9C
    u32 curRate;     // 0xA0
    s32 volume;      // 0xA4
    s32 panL;        // 0xA8
    s32 panR;        // 0xAC
    s32 ending;      // 0xB0
    s32 stopping;    // 0xB4
    u32 writePos;    // 0xB8  interleaved byte position in the voice buffers
    u32 pending;     // 0xBC  bytes the voices still need
    u32 prefill;     // 0xC0
    u32 lastPos;     // 0xC4  last voice play position
    u32 threshold;   // 0xC8
    u32 lastTime;    // 0xCC
    u32 interval;    // 0xD0
    u8 padD4[0x17C - 0xD4];
} StreamTrack;
#define STREAM_TRACKS(p) (*(StreamTrack **)((u8 *)(p) + 0x2F0))

// A streamed music/voice player (0x388 bytes). Only the fields named here are understood.
// Per-track audio description in the Bink header.
typedef struct BinkAudioFlags8011 {
    u32 valid : 1;
    u32 is16 : 1;
    u32 stereo : 1;
    u32 dct : 1;   // DCT codec (else RDFT)
    u32 pad : 12;
    u32 rate : 16;
} BinkAudioFlags8011;

typedef struct StreamPlayer {
    u32 width;                          // 0x00
    u32 height;                         // 0x04
    u32 numSets;                        // 0x08
    s32 curSet;                         // 0x0C
    s32 prevSet;                        // 0x10
    u32 fpsNum;                         // 0x14
    u32 fpsDen;                         // 0x18
    s32 done;                           // 0x1C
    u32 flags;                          // 0x20
    s32 videoFlags;                     // 0x24
    u32 fileSize;                       // 0x28
    u8 pad2C[0xB4 - 0x2C];
    s32 unkB4;                          // 0xB4
    void *video;                        // 0xB8  video decoder (0 = audio only)
    u8 *frameBuffer;                    // 0xBC
    u32 blocksWide;                     // 0xC0  width in 16-pixel blocks
    u32 frameBufferSize;                // 0xC4
    u32 largestFrame;                   // 0xC8
    u32 numIndex;                       // 0xCC  frame index entries - 1
    int numAudio;                       // 0xD0  audio tracks in the file
    u32 maxFrameSize;                   // 0xD4
    u32 unkD8;                          // 0xD8
    s32 paused;                         // 0xDC
    u8 padE0[0xE4 - 0xE0];
    s32 setData;                        // 0xE4  start of the current set's data
    void *memoryData;                   // 0xE8  set when playing from memory
    s32 *setIndex;                      // 0xEC  file offset of each set (low bit is a flag)
    // 0xF0: the file reader (BinkFileOpen for DVD files); its state follows its functions.
    s32 (*readerRead)(void *reader, s32 pos, void *dst, u32 size);             // 0xF0
    s32 (*readerSeek)(void *reader, s32 set, s32 offset, s32 dst, s32 size);  // 0xF4
    u32 (*readerBufSize)(void *reader, u32 size);                              // 0xF8
    void (*readerStart)(void *reader, void *buf, u32 bufSize, u32 total, s32 arg);  // 0xFC
    s32 (*readerFill)(void *reader);    // 0x100
    void (*readerClose)(void *reader);  // 0x104
    u8 pad108[0x10C - 0x108];
    struct StreamPlayer *self;          // 0x10C
    s32 unk110;                         // 0x110
    s32 unk114;                         // 0x114
    u8 pad118[0x11C - 0x118];
    volatile s32 busy;                  // 0x11C  also read by the decode callbacks
    u8 pad120[0x124 - 0x120];
    s32 readTime;                       // 0x124
    s32 unk128;                         // 0x128
    s32 unk12C;                         // 0x12C
    u8 pad130[0x138 - 0x130];
    volatile u32 readerCapacity;        // 0x138  the reader's current buffer size (volatile in RAD's BINKIO)
    volatile u32 readerFilled;          // 0x13C  bytes buffered
    u8 pad140[0x1F4 - 0x140];
    u8 request[0x234 - 0x1F4];          // 0x1F4
    void *readBuffer;                   // 0x234
    u32 readBufSize;                    // 0x238
    u32 width2;                         // 0x23C
    u32 height2;                        // 0x240
    s32 *trackIds;                      // 0x244  audio track number of each track
    u32 *audioMaxSize;                  // 0x248  per audio track
    struct BinkAudioFlags8011 *audioFlags;  // 0x24C
    s32 *audioIds;                      // 0x250
    u8 pad254[0x258 - 0x254];
    s32 unk258;                         // 0x258  frames decoded
    s32 unk25C;                         // 0x25C
    s32 frameStart;                     // 0x260
    s32 frameEnd;                       // 0x264
    s32 startTime;                      // 0x268
    u8 pad26C[0x270 - 0x26C];
    s32 unk270;                         // 0x270
    s32 unk274;                         // 0x274
    u8 pad278[0x288 - 0x278];
    s32 running;                        // 0x288
    s32 unk28C;                         // 0x28C
    u32 memUsed;                        // 0x290
    s32 decodeTime;                     // 0x294
    s32 idleTime;                       // 0x298
    s32 unk29C;                         // 0x29C
    s32 openTime;                       // 0x2A0
    u32 rateNum;                        // 0x2A4
    u32 rateDen;                        // 0x2A8
    s32 histSize;                       // 0x2AC
    s32 histPos;                        // 0x2B0
    s32 *histFrameStart;                // 0x2B4
    s32 *histIdle;                      // 0x2B8
    s32 *histDecode;                    // 0x2BC
    s32 *hist29C;                       // 0x2C0
    s32 *histRead;                      // 0x2C4
    s32 *hist128;                       // 0x2C8
    s32 *hist12C;                       // 0x2CC
    u8 pad2D0[0x2D4 - 0x2D0];
    u32 unk2D4;                         // 0x2D4
    s32 firstTime;                      // 0x2D8  time the first set last started
    s32 stalled;                        // 0x2DC
    u32 videoResult;                    // 0x2E0
    u32 frameWords;                     // 0x2E4
    int numTracks;                      // 0x2E8
    s32 resyncs;                        // 0x2EC
    StreamTrack *tracks;                // 0x2F0
    s32 unk2F4;                         // 0x2F4
    s32 unk2F8;                         // 0x2F8
    u8 pad2FC[0x300 - 0x2FC];
    u32 planeSizes[9];                  // 0x300  video plane buffer sizes
    u8 pad324[0x328 - 0x324];
    s32 unk328;                         // 0x328
    u8 pad32C[0x330 - 0x32C];
    s32 pauseTime;                      // 0x330
    u8 pad334[0x33C - 0x334];
    s32 tick;                           // 0x33C
    u8 queue[0x380 - 0x340];            // 0x340  request queue entry
    s32 unk380;                         // 0x380
    void *extra;                        // 0x384
} StreamPlayer;

// The Bink audio decoder's state.
typedef struct BinkAudio8011 {
    u32 frameSize;     // 0x00 samples per transform
    f32 scale;         // 0x04 2 / sqrt(frameSize)
    u32 frameBytes;    // 0x08 output bytes per frame
    u32 overlapBytes;  // 0x0C
    u32 channels;      // 0x10
    s16 *output;       // 0x14
    s32 first;         // 0x18 no previous frame to overlap yet
    u32 numBands;      // 0x1C
    u32 *bands;        // 0x20 band edges in coefficients
    s32 *quant;        // 0x24
    void *transform;   // 0x28 DCT or RDFT work area
    s16 *previous;     // 0x2C overlap from the last frame
    u32 flags;         // 0x30 bit 0: DCT (per channel) instead of RDFT (interleaved)
} BinkAudio8011;
void BinkAudioDecompress(BinkAudio8011 *a, s16 **out, u32 *outLen, u8 *src, u8 **next);
u32 ExpandBink(void *arg0, s32 arg1, s32 arg2, s32 *arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7);
void inittimer(void *arg0);
void checksound(StreamPlayer *p);

extern void *lbl_8055DED4;
u32 RADCB_try_to_suspend_callback(void *arg0, void *arg1);
void RADCB_resume_callback(void *arg0, void *arg1);
s8 *conv16to8(s8 *dst, s16 *src, u32 bytes);
void checksound(StreamPlayer *p);
void BinkNextFrame(StreamPlayer *p);
void BinkClose(StreamPlayer *p);
s32 BinkPause(StreamPlayer *p, s32 arg1);
s32 BinkSetSoundOnOff(StreamPlayer *p, s32 arg1);
void timeframe(void *arg0, s32 arg1);
void endframe(void *arg0, s32 arg1);
void GotoFrame(struct StreamPlayer *p, s32 next);
u32 dosilence(struct StreamTrack *t);
s32 RADTimerRead(void);
void fn_8011BC48(s32 arg0);
s32 RADCB_unregister_2_callbacks(s32 arg0, s32 arg1, void *arg2, s32 arg3, s32 arg4);
void RADCB_suspend_callback(void *arg0, void *arg1);
void radfree(void *arg0);
extern void memset(void *, int, unsigned int);
extern void *memcpy(void *, const void *, unsigned int);
extern void *memmove(void *, const void *, unsigned int);

// Takuya Ooura's General Purpose FFT Package (https://www.kurims.kyoto-u.ac.jp/~ooura/fft.html), Copyright
// Takuya OOURA 1996-2001, used under its licence. This is fftsg.c of the Oct. 1999 release (fft.tgz as
// archived on 2001-04-19; the routines cftrec1/2, cftexp1/2 and cftfx42 were dropped in Sep. 2001), in a
// float build: double -> f32, 0.5 -> 0.5f, and cos/sin -> MSL's cosf/sinf with atan(1.0) folded to
// the float constant pi/4 (makewt, makect). Every routine below is otherwise Ooura's text.
void rdft(int n, int isgn, f32 *a, int *ip, f32 *w);  // rdft
void ddct(int n, int isgn, f32 *a, int *ip, f32 *w);  // ddct
void makewt(int nw, int *ip, f32 *w);                     // makewt
void makect(int nc, int *ip, f32 *c);                     // makect
void fn_80120CA4(int n, f32 *a, int *ip, int nw, f32 *w);    // cftfsub
void fn_80120E14(int n, f32 *a, int *ip, int nw, f32 *w);    // cftbsub
void bitrv2(int n, int *ip, f32 *a);                    // bitrv2
void bitrv2conj(int n, int *ip, f32 *a);                    // bitrv2conj
void bitrv216(f32 *a);                                    // bitrv216
void bitrv216neg(f32 *a);                                    // bitrv216neg
void bitrv208(f32 *a);                                    // bitrv208
void bitrv208neg(f32 *a);                                    // bitrv208neg
void cftf1st(int n, f32 *a, f32 *w);                     // cftf1st
void cftb1st(int n, f32 *a, f32 *w);                     // cftb1st
void cftrec1(int n, f32 *a, int nw, f32 *w);             // cftrec1
void cftrec2(int n, f32 *a, int nw, f32 *w);             // cftrec2
void cftexp1(int n, f32 *a, int nw, f32 *w);             // cftexp1
void cftexp2(int n, f32 *a, int nw, f32 *w);             // cftexp2
void cftmdl1(int n, f32 *a, f32 *w);                     // cftmdl1
void cftmdl2(int n, f32 *a, f32 *w);                     // cftmdl2
void cftfx41(int n, f32 *a, int nw, f32 *w);             // cftfx41
void cftfx42(int n, f32 *a, int nw, f32 *w);             // cftfx42
void cftf161(f32 *a, f32 *w);                            // cftf161
void cftf162(f32 *a, f32 *w);                            // cftf162
void cftf081(f32 *a, f32 *w);                            // cftf081
void cftf082(f32 *a, f32 *w);                            // cftf082
void cftf040(f32 *a);                                    // cftf040
void cftb040(f32 *a);                                    // cftb040
void cftx020(f32 *a);                                    // cftx020
void rftfsub(int n, f32 *a, int nc, f32 *c);             // rftfsub
void rftbsub(int n, f32 *a, int nc, f32 *c);             // rftbsub
void dctsub(int n, f32 *a, int nc, f32 *c);             // dctsub

#define M2C_FIELD(base, type, offset) (*(type)((u8 *)(base) + (offset)))

extern void fn_802ECC74(void);
extern void *radmalloc(s32);
extern f32 lbl_80329D48[];
extern void CancelReadRequests(void *);
void fn_8011FFB8(void *arg0, void *arg1);
extern void *lbl_8055DEF8;
extern void *lbl_8055DEFC;
extern void ***lbl_80559948;
extern u32 *lbl_8055994C;
extern void fn_80127F70(void);
extern void fn_80127F94(void);
extern s32 BinkFileIdle(DVDFileInfo *);
extern s32 OSDisableInterrupts(void);
extern s32 OSEnableInterrupts(void);
extern s32 OSRestoreInterrupts(s32);
extern void OSYieldThread(void);
extern int DVDReadAsyncPrio(DVDFileInfo *, void *, s32, s32, DVDAsyncCallback, s32);
extern int DVDClose(DVDFileInfo *);
extern s32 DVDConvertPathToEntrynum(const char *);
extern int DVDFastOpen(s32, DVDFileInfo *);
void fn_80127440(void);
extern void fn_80127398(s32);
extern void fn_802E300C(s32);
typedef struct CallbackList {
    s32 count;
    void (*fns[4])(void);
} CallbackList;
extern CallbackList lbl_8041D728[];
extern const s32 lbl_802F6A40[];
extern u8 lbl_8041D508[];
extern u32 lbl_80559950;

// Returns a * b / d with a 64-bit intermediate (the quotient must fit in 32 bits): shifts for a power-of-two d,
// divides directly when the product fits, else uses two-digit long division (Hacker's Delight divlu).
#pragma push
#pragma opt_lifetimes off
u32 mult64anddiv(u32 a, u32 b, u32 d) {
    u32 p;
    u32 rhat;
    u32 q1;
    u32 vn1;
    u32 vn0;
    u32 un1;
    u32 r;
    u32 q0;
    u32 un21;
    u32 p2;
    u32 r2;
    u32 rhat2;
    u32 hi = __mulhwu(a, b);
    u32 lo = a * b;
    u32 s;
    u32 v;
    /* MATCHING-AID: remainder slot written through a cast; the store emits no code but is removed after numbering,
       which keeps rhat/r2 whole as in retail. It mirrors textbook divlu's *r out-parameter; a real u32* rem
       parameter scores far worse. The original form is unproven. */
    u32 rem;
    if ((d & (d - 1)) == 0) {
        s = __cntlzw(d);
#ifdef __MWERKS__
        return (hi << (s + 1)) | (lo >> (31 - s));
#else
        /* PPC slw/srw give 0 for shift counts 32-63; x86 masks the count to 5 bits. d == 1 (s = 31) shifts hi by
           32 and d == 0 (s = 32) shifts by 33 and by -1: keep the console's results. */
        return (s + 1 < 32 ? hi << (s + 1) : 0) | (31 - s < 32 ? lo >> (31 - s) : 0);
#endif
    }
    if (hi == 0) {
        return lo / d;
    }
    s = __cntlzw(d);
    v = d << s;
    hi = (hi << s) + (lo >> (32 - s));
    lo <<= s;
    vn1 = v >> 16;
    vn0 = v & 0xFFFF;
    un1 = lo >> 16;
    q1 = hi / vn1;
    rhat = hi - (q1 * vn1);
    r = (rhat << 16) | un1;
    *(u32 *)&rem = rhat;
    p = q1 * vn0;
    if (r < p) {
        r += v;
        q1--;
        if ((r >= v) && (r < p)) {
            r += v;
            q1--;
        }
    }
    un21 = r - p;
    q0 = un21 / vn1;
    rhat2 = un21 - (q0 * vn1);
    r2 = (rhat2 << 16) | (lo & 0xFFFF);
    p2 = q0 * vn0;
    if (r2 < p2) {
        r2 += v;
        q0--;
        if ((r2 >= v) && (r2 < p2)) {
            q0--;
        }
    }
    *(u32 *)&rem = r2;
    return (q1 << 16) | q0;
}
#pragma pop

// Fills `size` bytes at dst with the 16-bit value v.
void radmemset16(u32 *dst, u16 v, u32 size) {
    u32 n = size >> 1;
    u32 val = v | (v << 16);
    while (n--) {
        *dst++ = val;
    }
    if (size & 1) {
        *(u16 *)dst = v;
    }
}

extern s64 OSGetTime(void);
extern s64 lbl_8055DEC0;  // time of the first call
// Milliseconds since the first call (60750 timer ticks per ms), divided without a 64-bit division.
s32 RADTimerRead(void) {
    s64 now = OSGetTime();
    u64 delta;
    u64 ms;
    if (!lbl_8055DEC0) {
        lbl_8055DEC0 = now;
    }
    delta = now - lbl_8055DEC0;
    ms = (u32)(delta >> 32) * 70699;
    delta -= ms * 60750;
    delta = (delta * 0x8A15866B) >> 47;
    return delta + ms;
}

extern u32 fn_8011FF24(void);
extern void *popmalloc(s32);
// Allocates `size` bytes for player p, counting them (plus allocator overhead) in its memory total.
void *bpopmalloc(void *p, s32 size) {
    u32 overhead = fn_8011FF24();
    s32 *fields = (s32 *)p;
    fields[0x290 / 4] += size + overhead;
    return popmalloc(size);
}

void BinkSetError(const u8 *arg0) {
    const u8 *src = arg0;
    u8 *dst = lbl_8041D508;
    u8 c;
    do {
        c = *src;
        *dst = c;
        if ((s8)c == 0)
            return;
        dst++;
        src++;
    } while (1);
}

extern u8 lbl_8041D508[];
extern s32 (*lbl_8055DEC8)(s32);
extern s32 lbl_8055DECC;
extern s32 lbl_8055DED0;
s32 BinkSetSoundSystem(s32 (*arg0)(s32), s32 arg1) {
    s32 (*func)(s32);
    s32 result;
    if (arg0 == 0)
        return 0;
    func = lbl_8055DEC8;
    if (func == 0) {
        func = arg0;
        lbl_8055DEC8 = arg0;
    }
    if ((u32)func != (u32)arg0) {
        if (lbl_8055DED0 == 0) {
            lbl_8055DEC8 = arg0;
        } else {
            return 0;
        }
    }
    result = lbl_8055DEC8(arg1);
    if (result != 0)
        lbl_8055DECC = result;
    return lbl_8055DECC != 0;
}

// Converts `bytes` of 16-bit samples to 8-bit ones (high byte), returning the end of the output.
s8 *conv16to8(s8 *dst, s16 *src, u32 bytes) {
    u32 n;
    for (n = bytes >> 1; n != 0; n--) {
        *dst++ = *src++ >> 8;
    }
    return dst;
}

// Tops a track's buffered data up to its fill target by inserting silence before the read position; returns the
// milliseconds of silence added.
u32 dosilence(StreamTrack *t) {
    u32 need;
    u32 ms;
    u32 wrap;
    u8 *start;
    u8 *ring;
    if (t->available < t->fillTarget) {
        need = t->fillTarget - t->available;
        ms = need * 1000 / ((t->channels * (t->rate * t->bits)) >> 3);
        start = t->readPtr -= need;
        ring = t->ringStart;
        if (start < ring) {
            wrap = ring - start;
            t->readPtr = start + t->ringSize;
            memset(ring, 0, need - wrap);
            memset(t->readPtr, 0, wrap);
        } else {
            memset(start, 0, need);
        }
        t->available += need;
        return ms;
    }
    return 0;
}
extern s32 RADTimerRead(void);
// Feeds every stream track: while a track is playing and has enough data buffered (or its deadline passed),
// copies the next block out of its ring buffer into the buffer it hands out.
// Feeds every stream track: while a track is playing and has enough data buffered (or its deadline passed),
// copies the next block out of its ring buffer into the buffer it hands out.
void checksound(StreamPlayer *p) {
    u32 i;
    u8 *dst;
    u32 len;
    u32 first;

    if (p->tracks[p->numTracks - 1].IsActive != 0 && RADCB_try_to_suspend_callback(lbl_8055DED4, p->queue) != 0) {
        for (i = 0; i < p->numTracks; i++) {
            while (p->tracks[i].IsActive(&p->tracks[i]) &&
                   (p->tracks[i].available > p->tracks[i].minimum ||
                    (p->curSet > p->tracks[i].deadline && p->tracks[i].available != 0))) {
                if (p->tracks[i].GetBuffer(&p->tracks[i], &dst, &len) == 0) {
                    break;
                }
                if (p->tracks[i].eightBit) {
                    len *= 2;
                }
                if (len > p->tracks[i].available) {
                    len = p->tracks[i].available;
                }
                if (p->curSet <= p->tracks[i].deadline) {
                    len &= p->tracks[i].blockMask << p->tracks[i].eightBit;
                }
                p->tracks[i].available -= len;
                first = p->tracks[i].ringEnd - p->tracks[i].readPtr;
                if (first < len) {
                    if (first != 0) {
                        if (p->tracks[i].eightBit) {
                            dst = (u8 *)conv16to8((s8 *)dst, (s16 *)p->tracks[i].readPtr, first);
                        } else {
                            memcpy(dst, p->tracks[i].readPtr, first);
                            dst += first;
                        }
                    }
                    if (p->tracks[i].eightBit) {
                        conv16to8((s8 *)dst, (s16 *)p->tracks[i].ringStart, len - first);
                    } else {
                        memcpy(dst, p->tracks[i].ringStart, len - first);
                    }
                    p->tracks[i].readPtr = p->tracks[i].ringStart + (len - first);
                } else {
                    if (p->tracks[i].eightBit) {
                        conv16to8((s8 *)dst, (s16 *)p->tracks[i].readPtr, len);
                    } else {
                        memcpy(dst, p->tracks[i].readPtr, len);
                    }
                    p->tracks[i].readPtr += len;
                }
                if (p->tracks[i].eightBit) {
                    len >>= 1;
                }
                p->tracks[i].Commit(&p->tracks[i], len);
            }
        }
        RADCB_resume_callback(lbl_8055DED4, p->queue);
    }
}

void inittimer(void* arg0) {
 if(*(s32*)((u8*)arg0+0x268)==0) {
  s32 value=RADTimerRead();
  s32 count=*(s32*)((u8*)arg0+0x258);
  *(s32*)((u8*)arg0+0x268)=value;
  *(s32*)((u8*)arg0+0x26c)=count-1;
  *(s32*)((u8*)arg0+0x330)=0;
 }
}

// Moves a stream player to set `next` (1-based; 0 restarts the first): feeds the tracks, notes when the first set
// starts (flagging a stall after 300000 ticks), and points the data at the set, in memory or through the reader.
void GotoFrame(StreamPlayer *p, s32 next) {
    int set;
    s32 now;
    s32 base;
    set = next;
    if (next != 0) set = next - 1;
    p->unk2F8 = 0;
    if (p->numTracks != 0) checksound(p);
    p->prevSet = p->curSet;
    if (set == 0) {
        now = RADTimerRead();
        if (p->firstTime == 0) p->firstTime = now;
        if ((u32)(now - p->firstTime) > 300000) {
            p->firstTime = now;
            p->stalled = 1;
        }
    }
    base = (s32)p->memoryData;
    if (base != 0) {
        p->setData = base + ((p->setIndex[set] & ~1) - (p->setIndex[0] & ~1));
    } else {
        s32 a = p->setIndex[set] & ~1;
        s32 b = p->setIndex[set + 1] & ~1;
        p->readerSeek(&p->readerRead, set, a, p->setData, b - a);
    }
    if (p->numTracks != 0) checksound(p);
    p->curSet = set + 1;
}

u32 mult64anddiv(u32 a, u32 b, u32 d);
u32 high1secrate(s32 count, u32 *values, s32 offset, s32 *outIndex, s32 *outFlag) {
    s32 i;
    u32 maxDiff = 0;
    s32 maxIndex = 0;
    s32 allOdd = 1;
    for (i = 0; i < count - offset; i++) {
        u32 *ahead = values + offset;
        u32 diff = ahead[(u32)i] - values[i];
        if (diff > maxDiff) {
            maxDiff = diff;
            maxIndex = i;
        }
    }
    if (maxDiff == 0) {
        maxDiff = mult64anddiv(values[count] - values[0], offset, count);
    }
    for (i = 0; i < count; i++) {
        if ((values[i] & 1) == 0) {
            allOdd = 0;
            break;
        }
    }
    *outFlag = allOdd;
    *outIndex = maxIndex;
    return maxDiff;
}

s32 bink_get_priority_sound(s32 arg0, s32 arg1) {
    StreamPlayer *p = (StreamPlayer *)((u8 *)arg0 - ((u8 *)((StreamPlayer *)arg0)->queue - (u8 *)arg0));
    s32 v = p->tick;
    return v != arg1 ? -1 - v : 0;
}

// Queue callback: `arg0` is the player's queue entry; feeds the player's tracks and stores the tick.
void bink_sound_callback(s32 arg0, s32 arg1) {
    // container-of: the player that owns this queue entry
    StreamPlayer *p = (StreamPlayer *)((u8 *)arg0 - ((u8 *)((StreamPlayer *)arg0)->queue - (u8 *)arg0));
    if (p->numTracks) checksound(p);
    p->tick = arg1;
}

void BinkGetFrameBuffersInfo(void *arg0, void *arg1) {
    s32 *out = (s32 *)arg1;
    s32 temp_r5;
    u32 half;

    out[5] = 0;
    out[1] = (*(s32 *)((u8 *)arg0 + 0x23C) + 7) & ~7;
    out[2] = (*(s32 *)((u8 *)arg0 + 0x240) + 7) & ~7;
    half = (u32)(*(s32 *)((u8 *)arg0 + 0x23C) + 1);
    out[3] = ((half >> 1) + 7) & ~7;
    half = (u32)(*(s32 *)((u8 *)arg0 + 0x240) + 1);
    out[6] = 1;
    temp_r5 = ((half >> 1) + 7) & ~7;
    out[9] = 1;
    out[4] = temp_r5;
    out[12] = 1;
    temp_r5 = (*(u32 *)((u8 *)arg0 + 0x20) >> 20) & 1;
    out[7] = 0;
    out[15] = temp_r5;
    out[10] = 0;
    out[13] = 0;
    out[16] = 0;
    out[8] = 0;
    out[11] = 0;
    out[14] = 0;
    out[17] = 0;
    if (*(s32 *)((u8 *)arg0 + 0x380) != 0) { out[0] = 1; return; }
    out[18] = 1; out[0] = 2;
    out[19] = 0; out[20] = 0; out[21] = 1; out[22] = 0;
    out[23] = 0; out[24] = 1; out[25] = 0; out[26] = 0;
    out[27] = temp_r5; out[28] = 0; out[29] = 0;
}

void BinkRegisterFrameBuffers(void* arg0,s32 arg1){if(arg0)*(s32*)((u8*)arg0+0xb8)=arg1;}

struct BinkIO8011;
s32 BinkFileOpen(struct BinkIO8011 *reader, const char *name, u32 flags);
BinkAudio8011 *BinkAudioDecompressOpen(u32 rate, u32 channels, u32 flags);
void pushmalloc(void **slot, u32 amount);
void ExpandBundleSizes(void *arg0, u32 arg1);
void *RADCB_allocate_handler(s32 arg0);
void RADCB_resume_handler(void *arg0);
void RADCB_register_callback(void *arg0, void *arg1, s32 arg2, s32 arg3);
void radfree(void *p);

// Bink file header.
typedef struct BinkHeader8011 {
    u32 magic;         // 'BIKf'..'BIKi' (stored little-endian)
    u32 fileSize;      // 0x04 bytes after the first 8
    u32 frames;        // 0x08
    u32 largestFrame;  // 0x0C
    u32 numIndex;      // 0x10
    u32 width;         // 0x14
    u32 height;        // 0x18
    u32 fpsNum;        // 0x1C
    u32 fpsDen;        // 0x20
    u32 flags;         // 0x24
    u32 numAudio;      // 0x28
} BinkHeader8011;

// One video plane buffer request.
typedef struct BinkPlane8011 {
    s32 wanted;
    void *buffer;
    u32 pitch;
} BinkPlane8011;

// Frame buffer layout filled in by BinkGetFrameBuffersInfo.
typedef struct BinkFrameBuffers8011 {
    s32 count;      // frames (1 or 2)
    u32 width;      // 0x04 luma width/height
    u32 height;     // 0x08
    u32 cWidth;     // 0x0C chroma width/height
    u32 cHeight;    // 0x10
    u32 unk14;
    struct {
        BinkPlane8011 y;
        BinkPlane8011 cr;
        BinkPlane8011 cb;
        BinkPlane8011 a;
    } frames[2];    // 0x18
} BinkFrameBuffers8011;

extern const char lbl_802F6528[];
extern s32 lbl_80559938;     // frame rate override (-1 = none)
extern s32 lbl_8055DEDC;
extern s32 lbl_8055993C;     // read buffer size override (-1 = none)
extern s32 lbl_80559940;     // reader start argument override (-1 = none)
extern s32 lbl_80559944;     // number of requested sound tracks
extern s32 lbl_8041D608[];   // requested sound track ids
extern s32 (*lbl_8055DED8)(void *reader, void *name, u32 flags);  // one-shot reader override
typedef s32 (*BinkSoundOpen8011)(StreamTrack *t, u32 rate, s32 bits, s32 chans, u32 flags, struct StreamPlayer *p);

// BinkOpen: opens `name` (or a file image in memory with flag 0x04000000) and sets up the player, its frame
// buffers, frame index, read buffer and sound tracks. Returns 0 on failure (the reason is in lbl_8041D508).
StreamPlayer *BinkOpen(void *name, u32 flags) {
    BinkHeader8011 hdr;
    BinkFrameBuffers8011 fb;
    StreamPlayer local;
    s32 fromMemory;
    StreamPlayer *p;
    s32 (*open)(void *reader, void *name, u32 flags);
    s32 startArg;
    u32 i;
    u32 track;
    u32 n;
    s32 any;
    s32 f;
    void *buf;

    open = (s32 (*)(void *, void *, u32))BinkFileOpen;
    if (!(flags & 0x4000)) {
        lbl_80559944 = 1;
        lbl_8041D608[0] = 0;
    }
    memset(&local, 0, sizeof(local));
    local.openTime = RADTimerRead();
    lbl_8041D508[0] = 0;
    fromMemory = flags & 0x04000000;
    if (fromMemory) {
        memcpy(&hdr, name, sizeof(hdr));
    } else {
        if ((flags & 0x02000000) && lbl_8055DED8 != 0) {
            open = lbl_8055DED8;
        }
        lbl_8055DED8 = 0;
        if (!open(&local.readerRead, name, flags)) {
            if ((s8)lbl_8041D508[0] == 0) {
                BinkSetError((u8 *)lbl_802F6528);
            }
            return 0;
        }
        local.readerRead(&local.readerRead, 0, &hdr, sizeof(hdr));
    }
    if ((hdr.magic != 'fKIB' && hdr.magic != 'gKIB' && hdr.magic != 'hKIB' && hdr.magic != 'iKIB') ||
        local.unk110 != 0) {
        BinkSetError((u8 *)lbl_802F6528 + 0x14);
    close:
        if (!(flags & 0x04000000)) {
            local.readerClose(&local.readerRead);
        }
        return 0;
    }
    if (hdr.frames == 0) {
        BinkSetError((u8 *)lbl_802F6528 + 0x25);
        goto close;
    }
    local.width = hdr.width;
    local.height = hdr.height;
    local.width2 = hdr.width;
    local.height2 = hdr.height;
    local.videoFlags = hdr.flags;
    local.blocksWide = (hdr.width + 15) >> 4;
    local.flags = (flags & ~0x70000000) | (hdr.flags & 0x20000);
    if (!(hdr.flags & 0x100000)) {
        local.flags &= ~0x100000;
    }
    if ((flags & 0x70000000) != 0x70000000) {
        if (flags & 0x70000000) {
            local.flags |= flags & 0x70000000;
        } else {
            local.flags |= hdr.flags & 0x70000000;
        }
        switch (local.flags & 0x70000000) {
        case 0x10000000:
        case 0x20000000:
            local.height *= 2;
            break;
        case 0x30000000:
            local.width *= 2;
            break;
        case 0x40000000:
        case 0x50000000:
            local.width *= 2;
            local.height *= 2;
            break;
        }
    }
    if (hdr.magic == 'fKIB' || hdr.magic == 'gKIB') {
        local.flags |= 0x18000;
    } else if (hdr.magic == 'hKIB') {
        local.flags |= 0x8000;
    }
    local.numSets = hdr.frames;
    local.numIndex = hdr.numIndex;
    if ((flags & 0x1000) && lbl_80559938 != -1) {
        local.fpsNum = lbl_80559938;
        local.fpsDen = lbl_8055DEDC;
        lbl_80559938 = -1;
    } else {
        local.fpsNum = hdr.fpsNum;
        local.fpsDen = hdr.fpsDen;
    }
    local.rateNum = hdr.fpsNum;
    local.rateDen = hdr.fpsDen;
    local.fileSize = hdr.fileSize;
    local.numAudio = hdr.numAudio;
    local.largestFrame = hdr.largestFrame;
    local.frameBufferSize = local.blocksWide * ((local.height + 15) >> 4);
    local.frameWords = local.frameBufferSize >> 2;
    local.histSize = (hdr.fpsNum + (hdr.fpsDen >> 1)) / hdr.fpsDen;
    if (local.histSize == 0) {
        local.histSize = 1;
    }
    pushmalloc((void **)&local.frameBuffer, local.frameBufferSize + 16);
    pushmalloc((void **)&local.histFrameStart, local.histSize * 4);
    pushmalloc((void **)&local.histIdle, local.histSize * 4);
    pushmalloc((void **)&local.histDecode, local.histSize * 4);
    pushmalloc((void **)&local.hist29C, local.histSize * 4);
    pushmalloc((void **)&local.histRead, local.histSize * 4);
    pushmalloc((void **)&local.hist128, local.histSize * 4);
    pushmalloc((void **)&local.hist12C, local.histSize * 4);
    pushmalloc((void **)&local.tracks, (lbl_80559944 != 0 ? lbl_80559944 : 1) * sizeof(StreamTrack));
    pushmalloc((void **)&local.trackIds, lbl_80559944 * 4);
    ExpandBundleSizes(local.planeSizes, (local.width + 15) & ~15);
    pushmalloc((void **)&local.planeSizes[0], local.planeSizes[0]);
    pushmalloc((void **)&local.planeSizes[1], local.planeSizes[1]);
    pushmalloc((void **)&local.planeSizes[2], local.planeSizes[2]);
    pushmalloc((void **)&local.planeSizes[3], local.planeSizes[3]);
    pushmalloc((void **)&local.planeSizes[4], local.planeSizes[4]);
    pushmalloc((void **)&local.planeSizes[5], local.planeSizes[5]);
    pushmalloc((void **)&local.planeSizes[6], local.planeSizes[6]);
    pushmalloc((void **)&local.planeSizes[7], local.planeSizes[7]);
    pushmalloc((void **)&local.planeSizes[8], local.planeSizes[8]);
    if (!fromMemory) {
        pushmalloc((void **)&local.setIndex, (local.numIndex + 1) * 4);
        pushmalloc((void **)&local.audioMaxSize, local.numAudio * 4);
        pushmalloc((void **)&local.audioFlags, local.numAudio * 4);
        pushmalloc((void **)&local.audioIds, local.numAudio * 4);
    }
    p = bpopmalloc(&local, sizeof(StreamPlayer));
    if (p == 0) {
    nomem:
        BinkSetError((u8 *)lbl_802F6528 + 0x59);
        goto close;
    }
    memcpy(p, &local, sizeof(StreamPlayer));
    p->self = p;
    *p->histIdle = 0;
    *p->histDecode = 0;
    *p->hist29C = 0;
    *p->histRead = 0;
    *p->hist128 = 0;
    *p->hist12C = 0;
    if (fromMemory) {
        p->audioMaxSize = (u32 *)((u8 *)name + 0x2C);
        p->audioFlags = (BinkAudioFlags8011 *)((u8 *)name + p->numAudio * 4 + 0x2C);
        p->audioIds = (s32 *)((u8 *)name + p->numAudio * 8 + 0x2C);
        p->setIndex = (s32 *)((u8 *)name + p->numAudio * 12 + 0x2C);
    } else {
        p->readerRead(&p->readerRead, -1, p->audioMaxSize, p->numAudio * 4);
        p->readerRead(&p->readerRead, -1, p->audioFlags, p->numAudio * 4);
        p->readerRead(&p->readerRead, -1, p->audioIds, p->numAudio * 4);
        p->readerRead(&p->readerRead, -1, p->setIndex, (p->numIndex + 1) * 4);
        if (p->unk110) {
            BinkSetError((u8 *)lbl_802F6528 + 0x68);
            goto close;
        }
    }
    p->maxFrameSize = high1secrate(p->numSets, (u32 *)p->setIndex, p->histSize, (s32 *)&p->unkD8, &p->unk380);
    if (!(flags & 0x400)) {
        BinkGetFrameBuffersInfo(p, &fb);
        any = 0;
        for (f = 0; f < fb.count; f++) {
            u32 cPitch = (fb.cWidth + 15) & ~15;
            if (fb.frames[f].y.wanted) {
                pushmalloc(&fb.frames[f].y.buffer, fb.width * fb.height);
                any = 1;
                fb.frames[f].y.pitch = fb.width;
            }
            if (fb.frames[f].cr.wanted) {
                pushmalloc(&fb.frames[f].cr.buffer, cPitch * fb.cHeight);
                fb.frames[f].cr.pitch = cPitch;
                any = 1;
            }
            if (fb.frames[f].cb.wanted) {
                pushmalloc(&fb.frames[f].cb.buffer, cPitch * fb.cHeight);
                fb.frames[f].cb.pitch = cPitch;
                any = 1;
            }
            if (fb.frames[f].a.wanted) {
                pushmalloc(&fb.frames[f].a.buffer, fb.width * fb.height);
                any = 1;
                fb.frames[f].a.pitch = fb.width;
            }
        }
        for (; f < 2; f++) {
            fb.frames[f].cr.buffer = 0;
            fb.frames[f].cb.buffer = 0;
            fb.frames[f].y.buffer = 0;
        }
        if (any) {
            p->extra = buf = bpopmalloc(p, sizeof(fb));
            if (buf == 0) {
            freeplayer:
                radfree(p);
                goto nomem;
            }
            p->video = buf;
            memcpy(buf, &fb, sizeof(fb));
        }
    }
    if ((flags & 0x01000000) && lbl_8055993C != -1) {
        p->readBufSize = lbl_8055993C;
        lbl_8055993C = -1;
    } else {
        p->readBufSize = p->maxFrameSize;
    }
    if ((flags & 0x00400000) && (startArg = lbl_80559940) != -1) {
        lbl_80559940 = -1;
    } else {
        startArg = 0;
    }
    if (fromMemory) {
        p->memoryData = (u8 *)name + (p->setIndex[0] & ~1);
    } else {
        p->readBufSize = p->readerBufSize(&p->readerRead, p->readBufSize);
        if (p->readBufSize >= p->fileSize * 9 / 10) {
            flags |= 0x2000;
            p->flags |= 0x2000;
        }
        if (flags & 0x2000) {
            n = p->fileSize + 8 - (p->setIndex[0] & ~1);
            p->memoryData = bpopmalloc(p, n);
            if (p->memoryData == 0) {
                if (p->video != 0) {
                    radfree(p->video);
                }
                goto freeplayer;
            }
            p->readerStart(&p->readerRead, 0, 0, p->fileSize + 8, startArg);
            p->readerSeek(&p->readerRead, 0, p->setIndex[0] & ~1, (s32)p->memoryData, n);
            p->readerClose(&p->readerRead);
            p->readTime = 0;
        } else {
            pushmalloc((void **)&p->setData, p->largestFrame);
            p->readBuffer = bpopmalloc(p, p->readBufSize);
            if (p->readBuffer == 0) {
                p->readBufSize = 0;
            }
            p->readerStart(&p->readerRead, p->readBuffer, p->readBufSize, p->fileSize + 8, startArg);
        }
    }
    p->frameBuffer[p->frameBufferSize] = 0;
    p->curSet = -1;
    if (p->fpsNum != 0) {
        p->unk270 = mult64anddiv(2000, p->fpsDen, p->fpsNum);
    } else {
        p->unk270 = 2000;
    }
    p->unk28C = 1;
    GotoFrame(p, 1);
    p->openTime = RADTimerRead() - p->openTime;
    p->tracks->gain = 0x10000;
    p->numTracks = 0;
    if (p->numAudio != 0 && lbl_80559944 != 0) {
        for (i = 0; i < lbl_80559944; i++) {
            s32 j;
            for (j = 0; j < p->numAudio; j++) {
                if (p->audioIds[j] == lbl_8041D608[i]) {
                    p->trackIds[p->numTracks++] = j;
                    break;
                }
            }
        }
    }
    lbl_80559944 = 1;
    lbl_8041D608[0] = 0;
    for (track = 0; track < p->numTracks;) {
        StreamTrack *t;
        BinkAudioFlags8011 af;
        /* MATCHING-AID: at the two places that copy a whole flags word, the track id is written through a cast of
           its own address. That emits no code but makes the id a real value numbered below the audioFlags
           load, as in retail (trackIds in r4, audioFlags in r3). The original form is unproven. */
        s32 idx;
        p->tracks[track].ringStart = 0;
#define AF (p->audioFlags[p->trackIds[track]])
        *(s32 *)&idx = p->trackIds[track];
        af = p->audioFlags[idx];
        if ((af.valid || af.dct) && lbl_8055DECC != 0) {
            u32 rate = af.rate;
            if (local.fpsNum != 0 && local.fpsDen != 0) {
                rate = (s32)((f64)(s32)local.rateDen * ((f64)(s32)rate * (f64)(s32)local.fpsNum) /
                             ((f64)(s32)local.fpsDen * (f64)(s32)local.rateNum));
            }
            if (((BinkSoundOpen8011)lbl_8055DECC)(&p->tracks[track], rate, af.is16 * 8 + 8, af.stereo + 1, p->flags, p)) {
                s32 latency;
                if (p->tracks[track].blockMask == 0) {
                    p->tracks[track].blockMask = -1;
                }
                p->tracks[track].ringSize = (p->audioMaxSize[p->trackIds[track]] + 0xFF) & ~0xFF;
                p->tracks[track].ringStart = bpopmalloc(p, p->tracks[track].ringSize);
                t = &p->tracks[track];
                if (t->ringStart == 0) {
                    t->Close(t);
                } else {
                    latency = 750 - t->unk68;
                    if (latency < 0) {
                        latency = 0;
                    }
                    t->eightBit = AF.is16 * 8 + 8 == 8;
                    p->tracks[track].ringEnd = p->tracks[track].ringStart + p->tracks[track].ringSize;
                    p->tracks[track].writePtr = p->tracks[track].ringStart;
                    p->tracks[track].readPtr = p->tracks[track].ringStart;
                    p->tracks[track].fillTarget = mult64anddiv(rate * (AF.stereo + 1) * 2, latency, 1000) & ~3;
                    if (p->tracks[track].fillTarget > p->tracks[track].ringSize) {
                        p->tracks[track].fillTarget = p->tracks[track].ringSize;
                    }
                    {
                        BinkAudioFlags8011 f;
                        int dct = 0;
                        *(s32 *)&idx = p->trackIds[track];
                        f = p->audioFlags[idx];
                        if (!f.valid && f.dct) {
                            dct = 1;
                        }
                        p->tracks[track].handle = (s32)BinkAudioDecompressOpen(f.rate, f.stereo + 1, dct);
                    }
                    if (p->tracks[track].handle == 0) {
                        p->tracks[track].Close(&p->tracks[track]);
                        p->tracks[track].ringStart = 0;
                    }
                    lbl_8055DED0++;
                    p->tracks[track].deadline = p->numSets - (p->rateNum * 3) / (p->rateDen * 4);
                    p->tracks[track].available = 0;
                }
            }
        }
#undef AF
        if (p->tracks[track].gain == 0) {
            p->tracks[track].gain = 0x10000;
        }
        if (p->tracks[track].ringStart == 0) {
            p->numTracks--;
            memcpy(&p->trackIds[track], &p->trackIds[track + 1], (p->numTracks - track) * 4);
        } else {
            track++;
        }
    }
    if (p->numTracks != 0) {
        p->running = 1;
        if (p->tracks[0].unk60 == 0) {
            if (lbl_8055DED4 == 0) {
                lbl_8055DED4 = RADCB_allocate_handler(-13);
                if (lbl_8055DED4 != 0) {
                    RADCB_resume_handler(lbl_8055DED4);
                }
            }
            if (lbl_8055DED4 != 0) {
                RADCB_register_callback(lbl_8055DED4, p->queue, (s32)bink_get_priority_sound, (s32)bink_sound_callback);
            }
        }
    }
    if (p->memoryData == 0 && !(flags & 0x200000)) {
        while (p->readerFill(&p->readerRead)) {
        }
    }
    return p;
}

/* Helper name unproven */
static inline StreamTrack* SelectStreamTrack(StreamTrack* tracks, s32 index) {
    return tracks + index;
}

// Decodes the current frame of a stream player: updates the timing history, copies each audio track's decoded
// packets into its ring (dropping the oldest data on overflow), then decodes the video. Returns 1 when playback is
// done, otherwise 0.
s32 BinkDoFrame(StreamPlayer *p) {
    s32 i;
    s32 old;
    u8 *data;
    s32 locked = 0;
    u32 size;
    u32 remaining;
    s32 j;
    u32 room;
    u32 over;
    u8 *src;
    s16 *out;
    u32 len;
    s32 now;
    s32 level;
    StreamTrack *t;

    if (p == 0) {
        return 0;
    }
    if (p->unk2D4 == p->curSet) {
        return 0;
    }
    if (p->stalled) {
        p->stalled = 0;
        if (p->running) {
            BinkSetSoundOnOff(p, 0);
            BinkSetSoundOnOff(p, 1);
        }
        p->startTime = 0;
        p->pauseTime = 0;
        p->unk328 = 0;
    }
    if (p->paused) {
        p->startTime = 0;
        p->pauseTime = 0;
        p->unk328 = 0;
    }
    p->unk2F8 = 0;
    p->busy = 1;
    p->frameStart = RADTimerRead();
    old = p->histPos;
    if (--p->histPos < 0) {
        p->histPos = p->histSize - 1;
    }
    p->histFrameStart[p->histPos] = p->frameStart;
    p->histDecode[p->histPos] = p->decodeTime;
    p->histIdle[p->histPos] = p->idleTime;
    p->hist29C[p->histPos] = p->unk29C;
    p->histRead[p->histPos] = p->readTime;
    p->hist128[p->histPos] = p->unk128;
    p->hist12C[p->histPos] = p->unk12C;
    if (p->unk25C == 0) {
        p->histFrameStart[old] = p->frameStart - mult64anddiv(1000, p->rateDen, p->rateNum);
        p->unk25C = p->frameStart;
        p->unk12C = 0;
        p->unk128 = 0;
    } else if (p->numTracks != 0) {
        checksound(p);
    }
    data = (u8 *)p->setData;
    p->unk274 = p->frameStart;
    if (p->unk110) {
        p->done = 1;
    }
    if (p->done) {
        return 1;
    }
    for (i = 0; i < p->numAudio; i++) {
        size = *(u32 *)data;
        data += 4;
        for (j = 0; j < p->numTracks; j++) {
            if (i == p->trackIds[j]) {
                goto found;
            }
        }
        j = -1;
    found:
        if (j != -1 && size != 0) {
            src = data;
            remaining = *(u32 *)src;
            src += 4;
            if (!locked) {
                locked = 1;
                RADCB_suspend_callback(lbl_8055DED4, p->queue);
            }
            while (remaining != 0) {
                uintptr_t writeOffset;
                uintptr_t decodeOffset = (uintptr_t)j * sizeof(StreamTrack);
                t = (StreamTrack *)(decodeOffset + (uintptr_t)p->tracks);
                room = t->ringSize - t->available;
                BinkAudioDecompress((BinkAudio8011 *)t->handle, &out, &len, src, &src);
                if (len > remaining) {
                    len = remaining;
                }
                remaining -= len;
                if (len > room) {
                    uintptr_t readOffset;
                    over = len - room;
                    p->tracks[j].readPtr += over;
                    level = OSDisableInterrupts();
                    SelectStreamTrack(p->tracks, j)->available -= over;
                    OSRestoreInterrupts(level);
                    readOffset = (uintptr_t)j * sizeof(StreamTrack);
                    t = (StreamTrack *)(readOffset + (uintptr_t)p->tracks);
                    if (t->readPtr >= t->ringEnd) {
                        t->readPtr -= t->ringSize;
                    }
                }
                level = OSDisableInterrupts();
                SelectStreamTrack(p->tracks, j)->available += len;
                OSRestoreInterrupts(level);
                writeOffset = (uintptr_t)j * sizeof(StreamTrack);
                t = (StreamTrack *)(writeOffset + (uintptr_t)p->tracks);
                room = t->ringEnd - t->writePtr;
                if (room < len) {
                    if (room != 0) {
                        memcpy(t->writePtr, out, room);
                        len -= room;
                        out = (s16 *)((u8 *)out + room);
                    }
                    memcpy(p->tracks[j].ringStart, out, len);
                    p->tracks[j].writePtr = p->tracks[j].ringStart + len;
                } else {
                    memcpy(t->writePtr, out, len);
                    p->tracks[j].writePtr += len;
                }
            }
        }
        data += size;
    }
    if (locked) {
        RADCB_resume_callback(lbl_8055DED4, p->queue);
    }
    now = RADTimerRead();
    if (p->unk28C && p->video) {
        if (p->unk2F4 == 0) {
            memset(p->frameBuffer, 0, p->frameBufferSize);
        }
        if (p->memoryData == 0 && p->unk114 == 0) {
            u32 n = p->maxFrameSize < p->readerCapacity ? p->maxFrameSize : p->readerCapacity;
            if (p->readerFilled < (n * 4 - n) >> 2) {
                p->readerFill(&p->readerRead);
            }
        }
        {
            u32 set = p->curSet - 1;
            p->videoResult = ExpandBink(p->video, (s32)p->frameBuffer, (s32)p->blocksWide, (s32 *)data,
                                         p->setIndex[set] & 1, (s32)p->planeSizes, p->flags, p->videoFlags);
        }
    }
    if (p->numTracks != 0) {
        checksound(p);
    }
    p->unkB4 = -1;
    p->unk258++;
    inittimer(p);
    p->frameEnd = RADTimerRead();
    p->idleTime += now - p->frameStart;
    p->decodeTime += p->frameEnd - now;
    p->unk2D4 = p->curSet;
    p->prevSet = p->curSet;
    p->busy = 0;
    return 0;
}

void timeframe(void *arg0, s32 arg1) {
    s32 temp_r0 = *(s32 *)((u8 *)arg0 + 0x264);
    if (temp_r0 != 0) {
        s32 temp_r5 = *(s32 *)((u8 *)arg0 + 0x29c);
        *(s32 *)((u8 *)arg0 + 0x29c) = temp_r5 + (arg1 - temp_r0);
        *(s32 *)((u8 *)arg0 + 0x264) = 0;
    }
}

// Per-frame update of a stream player: catches up tracks whose dirty flag is set, refills buffers and steps the
// playback state.
void BinkNextFrame(StreamPlayer *p) {
    s32 resync = 0;
    u32 i;
    if (p != 0) {
        p->busy = 1;
        p->unk2F8 = 0;
        p->busy = 0;
        for (i = 0; i < p->numTracks; i++) {
            if (p->tracks[i].dirty != 0) {
                p->tracks[i].dirty = 0;
                if (p->curSet > 1U && p->curSet <= p->tracks[i].deadline) {
                    resync = 1;
                }
            }
        }
        if (resync) {
            p->resyncs++;
            RADCB_suspend_callback(lbl_8055DED4, p->queue);
            BinkSetSoundOnOff(p, 0);
            for (;;) {
                u32 lim = (p->readerCapacity * 29) >> 5;
                if (p->readerFilled >= lim || p->readerFill(&p->readerRead) == 0) {
                    break;
                }
            }
            p->startTime = 0;
            p->unk328 = 0;
            p->pauseTime = 0;
            BinkSetSoundOnOff(p, 1);
            RADCB_resume_callback(lbl_8055DED4, p->queue);
        }
        p->busy = 1;
        if (p->numTracks != 0) {
            checksound(p);
        }
        timeframe(p, RADTimerRead());
        GotoFrame(p, p->curSet < p->numSets ? p->curSet + 1 : 1);
        p->busy = 0;
    }
}

// Destroys a stream player: closes its tracks, releases its file and buffers, and frees it.
// Destroys a stream player: closes its tracks, releases its file and buffers, and frees it.
void BinkClose(StreamPlayer *p) {
    u32 i;
    if (p != 0) {
        BinkPause(p, 1);
        switch (RADCB_unregister_2_callbacks(0, (s32)p->request, p->numTracks != 0 ? lbl_8055DED4 : 0, (s32)p->queue, 1)) {
        case 2:
            lbl_8055DED4 = 0;
            break;
        case 3:
            lbl_8055DED4 = 0;
            break;
        }
        for (i = 0; i < p->numTracks; i++) {
            p->tracks[i].Close(&p->tracks[i]);
            fn_8011BC48(p->tracks[i].handle);
            if (p->tracks[i].ringStart != 0) {
                radfree(p->tracks[i].ringStart);
            }
        }
        if (p->memoryData != 0) {
            if (!(p->flags & 0x04000000)) {
                radfree(p->memoryData);
            }
        } else {
            p->readerClose(&p->readerRead);
            radfree(p->readBuffer);
        }
        if (p->extra != 0) {
            radfree(p->extra);
        }
        memset(p, 0, sizeof(StreamPlayer));
        radfree(p);
    }
}

void endframe(void *arg0, s32 arg1) {
    s32 temp_r0 = *(s32*)((u8*)arg0 + 0x274);
    if (temp_r0 != 0) {
        u32 temp_r5 = *(u32*)((u8*)arg0 + 0x278);
        u32 temp_r6 = arg1 - temp_r0;
        *(s32*)((u8*)arg0 + 0x274) = 0;
        if (temp_r6 > temp_r5) {
            *(u32*)((u8*)arg0 + 0x280) = temp_r5;
            *(s32*)((u8*)arg0 + 0x284) = *(s32*)((u8*)arg0 + 0x27C);
            *(u32*)((u8*)arg0 + 0x278) = temp_r6;
            *(s32*)((u8*)arg0 + 0x27C) = *(s32*)((u8*)arg0 + 0xC);
        } else if (temp_r6 > *(u32*)((u8*)arg0 + 0x280)) {
            *(u32*)((u8*)arg0 + 0x280) = temp_r6;
            *(s32*)((u8*)arg0 + 0x284) = *(s32*)((u8*)arg0 + 0xC);
        }
    }
}

// Pauses (arg1 != 0) or resumes a stream player and all its tracks; returns the new paused state.
// Pauses (arg1 != 0) or resumes a stream player and all its tracks; returns the new paused state.
s32 BinkPause(StreamPlayer *p, s32 arg1) {
    s32 now;
    u32 i;
    s32 resumed = 0;
    if (p == 0) {
        return 0;
    }
    now = RADTimerRead();
    timeframe(p, now);
    endframe(p, now);
    if (arg1 == 0) {
        if (p->paused != 0) {
            resumed = 1;
        }
    } else if (p->paused == 0) {
        if (p->startTime != 0) {
            p->pauseTime = now - p->startTime;
        } else {
            p->pauseTime = 0;
        }
    }
    p->paused = arg1;
    RADCB_suspend_callback(lbl_8055DED4, p->queue);
    for (i = 0; i < p->numTracks; i++) {
        p->tracks[i].SetPaused(&p->tracks[i], arg1);
    }
    RADCB_resume_callback(lbl_8055DED4, p->queue);
    if (p->unk258 != 0 && p->numTracks != 0) {
        checksound(p);
    }
    if (resumed) {
        p->startTime = RADTimerRead() - p->pauseTime;
    }
    return p->paused;
}

// Stops (arg1 == 0) or restarts every track; when a stop empties the tracks their rings are reset.
s32 BinkSetSoundOnOff(StreamPlayer *p, s32 arg1) {
    s32 any = 0;
    s32 locked;
    u32 i;
    u32 r;
    if (p != 0) {
        locked = 0;
        for (i = 0; i < p->numTracks; i++) {
            if (p->tracks[i].Run != 0) {
                if (!locked) {
                    locked = 1;
                    RADCB_suspend_callback(lbl_8055DED4, p->queue);
                }
                r = p->tracks[i].Run(&p->tracks[i], arg1 ? 1 : 0);
                if (r != 0) {
                    any = 1;
                }
                if (r == 0 && p->running != 0) {
                    p->tracks[i].writePtr = p->tracks[i].ringStart;
                    p->tracks[i].readPtr = p->tracks[i].ringStart;
                    p->tracks[i].available = 0;
                    if (p->curSet != 1U || p->unk2D4 == p->curSet) {
                        dosilence(&p->tracks[i]);
                    }
                    p->running = 0;
                } else if (r != 0 && p->running == 0) {
                    p->running = 1;
                    p->startTime = 0;
                    p->unk328 = 0;
                    p->pauseTime = 0;
                }
            }
        }
        if (locked) {
            RADCB_resume_callback(lbl_8055DED4, p->queue);
        }
    }
    return any;
}

/* .sdata owned by this unit (0x80559938-0x80559948), in address order. */
s32 lbl_80559938 = -1;
s32 lbl_8055993C = -1;
s32 lbl_80559940 = -1;
s32 lbl_80559944 = 1;

/* .rodata owned by this unit (0x802F6528-0x802F65C0): BinkOpen's error strings. */
// DUMP_DATA lbl_802F6528 char
const char lbl_802F6528[152] = {
#include "dumpdata/lbl_802F6528.inc"
};

/* .bss owned by this unit (0x8041D508-0x8041D628), in REVERSE address order: this C unit emits globals last-defined first. */
s32 lbl_8041D608[8];
u8 lbl_8041D508[0x100];

/* .sbss owned by this unit (0x8055DEC0-0x8055DEE0), in REVERSE address order: this C unit emits globals last-defined first. */
s32 lbl_8055DEDC;
s32 (*lbl_8055DED8)(void *reader, void *name, u32 flags);
void *lbl_8055DED4;
s32 lbl_8055DED0;
s32 lbl_8055DECC;
s32 (*lbl_8055DEC8)(s32);
s64 lbl_8055DEC0;
