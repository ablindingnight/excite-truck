// Declarations shared by the Bink units (game/unk_80116BD4.c, game/unk_8011C3A8.c): types, bit-reader macros, externs.
// Generated from the declarations of unk_80116BD4.c when the unit was split (2026-11-16, Bink session); that file
// still carries its own copy. CLEANUP: make unk_80116BD4.c include this header and drop its copy.
#ifndef GAME_BINK_INTERNAL_H
#define GAME_BINK_INTERNAL_H
// Code 0x80116BD4-0x801274C0, imported verbatim from the upstream project (github.com/orangedude27/excite-truck,
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
#ifndef __cplusplus  /* PORT: C++ units get the C library's void* memset */
extern void memset(void *, int, unsigned int);
#endif
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

extern u8 lbl_802FE850[];

extern s64 OSGetTime(void);
extern s64 lbl_8055DEC0;  // time of the first call

extern u32 fn_8011FF24(void);
extern void *popmalloc(s32);

extern u8 lbl_8041D508[];
extern s32 (*lbl_8055DEC8)(s32);
extern s32 lbl_8055DECC;
extern s32 lbl_8055DED0;

extern s32 RADTimerRead(void);

u32 mult64anddiv(u32 a, u32 b, u32 d);

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

#ifndef __cplusplus  /* PORT: C++ units get the C library's void* memset */
extern void memset(void *, int, unsigned int);
#endif

extern void DCFlushRange(void *, u32);

extern void AXSetVoiceState(void *vpb, s32 state);

extern f64 pow(f64, f64);

extern f64 pow(f64, f64);

extern s32 lbl_8055DEE0[];
extern s32 lbl_8055DEE8;
struct BinkIO8011;
extern void ReadKickoff(struct BinkIO8011 *);
extern void TRKGetFreeBuffer(void *);
extern s32 lbl_8055DEE0[2];  // the sample buffer allocator: alloc, free
extern s32 lbl_8055DEE8;
extern void AXFreeVoice(void *);
typedef StreamTrack StreamChannel;  // the voice side of a track

// Bink movie header fields used for the sound length.
typedef struct BinkInfo8011 {
    u32 width;
    u32 height;
    u32 frames;        // 0x08
    u32 frameNum;
    u32 lastFrameNum;
    u32 frameRate;     // 0x14
    u32 frameRateDiv;  // 0x18
} BinkInfo8011;

extern void *AXAcquireVoice(u32 prio, void *callback, u32 context);
extern void AXSetVoiceSrcType(void *vpb, u32 type);
extern void AXSetVoiceSrc(void *vpb, AXPBSRC *src);
extern void AXSetVoiceSrcRatio(void *vpb, f32 ratio);
extern void AXSetVoiceAdpcm(void *vpb, AXPBADPCM *adpcm);
s32 fn_80119994(void);
s32 fn_8011999C(void);

typedef struct StreamAllocator {
    void *(*alloc)(u32 size);
    void (*free)(void *p);
} StreamAllocator;

// RAD's Bink I/O object as the Wii library lays it out (the GameCube one has no slots at 0x18 and 0x50).
// Layout and reader logic cross-checked against the n100f decomp's GameCube Bink (ngcfile.c), used as a
// reference only; the code here is written from this game's own instructions.
// The status fields are volatile: the stream callbacks update them from interrupt context.
typedef struct BinkIO8011 {
    u32 (*readHeader)(struct BinkIO8011 *io, s32 offset, void *dest, u32 size);                // 0x00
    u32 (*readFrame)(struct BinkIO8011 *io, u32 frame, s32 offset, void *dest, u32 size);      // 0x04
    u32 (*getBufferSize)(struct BinkIO8011 *io, u32 size);                                      // 0x08
    void (*setInfo)(struct BinkIO8011 *io, void *buf, u32 size, u32 filesize, u32 simulate);   // 0x0C
    u32 (*idle)(struct BinkIO8011 *io);                                                         // 0x10
    void (*close)(struct BinkIO8011 *io);                                                       // 0x14
    void *unk18;                                                                                // 0x18
    void *bink;                                                                                 // 0x1C
    volatile u32 readError;       // 0x20
    volatile u32 doingRead;       // 0x24
    volatile u32 bytesRead;       // 0x28
    volatile u32 working;         // 0x2C
    volatile u32 totalTime;       // 0x30
    volatile u32 foregroundTime;  // 0x34
    volatile u32 idleTime;        // 0x38
    volatile u32 threadTime;      // 0x3C
    volatile u32 bufSize;         // 0x40
    volatile u32 bufHighUsed;     // 0x44
    volatile u32 curBufSize;      // 0x48
    volatile u32 curBufUsed;      // 0x4C
    volatile u32 unk50;           // 0x50
    u8 iodata[160];               // 0x54: the file state (BinkFile8011)
    void (*suspend)(struct BinkIO8011 *io);       // 0xF4
    s32 (*trySuspend)(struct BinkIO8011 *io);     // 0xF8
    void (*resume)(struct BinkIO8011 *io);        // 0xFC
    void (*idleOn)(struct BinkIO8011 *io);        // 0x100
    u8 callbacks[0x1C];           // 0x104 RAD callback registration
    u8 unk120[0x24];              // 0x120
} BinkIO8011;

// The DVD file reader's state inside BinkIO8011::iodata: a ring buffer filled ahead by async reads.
typedef struct BinkFile8011 {
    DVDFileInfo fi;               // 0x00 (io 0x54)
    volatile u32 filePos;         // 0x3C next file offset to read into the ring
    u32 skip;                     // 0x40
    u32 bufPos;                   // 0x44 file offset of the ring's head
    u8 *bufHead;                  // 0x48 oldest unread byte
    volatile u32 bufEmpty;        // 0x4C free bytes in the ring
    u8 *buf;                      // 0x50 ring start (0 = unbuffered)
    u8 *bufEnd;                   // 0x54 ring end
    u8 *bufTail;                  // 0x58 next byte to fill
    u32 dontClose;                // 0x5C
    u32 startOfs;                 // 0x60
    u32 fileSize;                 // 0x64
    u32 simulate;                 // 0x68 simulated read rate (bytes/s), 0 = off
    s32 simTime;                  // 0x6C
    volatile u32 cancelling;      // 0x70 set while CancelReadRequests cancels the pending read
    u32 wasWorking;               // 0x74 io->working when the read started
    u32 readStart;                // 0x78 timer at the read's start, then its duration
    struct BinkIO8011 *io;        // 0x7C owner, for the DVD callback
} BinkFile8011;

#define BINKFILE(io) ((BinkFile8011 *)(io)->iodata)

extern u32 radreadngc(void *, u32, s32, u32);
extern void dosimulate(void *, s32, s32);
extern void intelendian(u32 *, u32);

extern f32 lbl_80329D48[];
extern void CancelReadRequests(void *);

extern u32 BinkFileReadHeader(void*, s32, s32, u32);
extern u32 BinkFileReadFrame(BinkIO8011 *, u32, s32, void *, u32);
extern s32 fn_8011AC04(s32, s32);
extern void BinkFileSetInfo(void*, s32, s32, s32, s32);
extern s32 BinkFileIdle(DVDFileInfo*);
extern void BinkFileClose(void*);
extern s32 BinkFileBGControl(void*, s32);

// Scales float samples by `gain` into clamped 16-bit PCM.
// The 64-bit borrow of v - bound: d gets the low word of the difference and v becomes the borrow mask (0 or -1).
// RAD wrote this as inline assembly on register variables (proof: retail's scheduler never moves anything across
// the subfe, which only assembly statements cause; the mask lands in v's own register and the difference in d's).
// The C form is the same arithmetic for other compilers.
#ifdef __MWERKS__
#define BINK_BORROW(d, bound, v) asm { subfc d, bound, v ; subfe v, v, v }
#else
#define BINK_BORROW(d, bound, v) { u64 t_ = (u64)(v) - (bound); (d) = (u32)t_; (v) = (u32)(t_ >> 32); }
#endif

extern u8 lbl_80329B50[];

void pushmalloc(void **slot, u32 amount);
extern u32 lbl_80329B60[];  // critical band frequencies (Hz)

void RADCB_free_handler(void *handler);

#endif
