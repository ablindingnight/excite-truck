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
// DUMP_DATA lbl_80329B50 u8
u8 lbl_80329B50[16] = {
#include "dumpdata/lbl_80329B50.inc"
};
// DUMP_DATA lbl_80329B60 u32
u32 lbl_80329B60[26] = {
#include "dumpdata/lbl_80329B60.inc"
};
// DUMP_DATA lbl_80329BC8 f32
f32 lbl_80329BC8[96] = {
#include "dumpdata/lbl_80329BC8.inc"
};
// DUMP_DATA lbl_80329D48 f32
f32 lbl_80329D48[24] = {
#include "dumpdata/lbl_80329D48.inc"
};
u8 *lbl_80329DA8[16] = {
    (u8 *)lbl_802F65C0, (u8 *)lbl_802F65D0, (u8 *)lbl_802F65F0, (u8 *)lbl_802F6610,
    (u8 *)lbl_802F6630, (u8 *)lbl_802F6650, (u8 *)lbl_802F6670, (u8 *)lbl_802F6690,
    (u8 *)lbl_802F66D0, (u8 *)lbl_802F6710, (u8 *)lbl_802F6750, (u8 *)lbl_802F6790,
    (u8 *)lbl_802F67D0, (u8 *)lbl_802F6810, (u8 *)lbl_802F6890, (u8 *)lbl_802F6910
};


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

BOOL ARCFastOpen(ARCHandle *handle, s32 entrynum, ARCFileInfo *info) {
    u32 *nodes = (u32 *)handle->FSTStart;
    s32 offset;
    if (entrynum < 0 || (u32)entrynum >= handle->entryNum || (nodes[entrynum * 3] & 0xFF000000) != 0) return FALSE;
    info->handle = handle;
    offset = (entrynum * 0xC) + (s32)nodes;
    info->startOffset = *(u32 *)(offset + 4);
    info->length = *(u32 *)(offset + 8);
    return TRUE;
}

extern u8 lbl_802FE850[];
s32 ARCConvertPathToEntrynum(ARCHandle *handle, const char *path) {
    u32 *fst = (u32 *)handle->FSTStart;
    u32 entry = handle->currDir;
    u32 parent;
    u32 node;
    const char *name;
    const char *p;
    char *entryName;
    u8 *arc = *(u8 **)(lbl_802FE850 + 0x38);
    u8 *caseTable = *(u8 **)(arc + 0x10);
    while (*path != 0) {
        if (*path == '/') { entry = 0; path++; continue; }
        if (*path == '.' && path[1] == '.' && path[2] == '/') {
            path += 3; entry = fst[entry * 3 + 1]; continue;
        }
        if (*path == '.' && path[1] == '/') { path += 2; continue; }
        name = path;
        while (*path != 0 && *path != '/') path++;
        parent = entry;
        entry = parent + 1;
        while (entry < fst[parent * 3 + 2]) {
            node = fst[entry * 3];
            if ((node & 0xFF000000) == 0) { entry = fst[entry * 3 + 2]; continue; }
            entryName = handle->FSTStringStart + (node & 0xFFFFFF);
            p = name;
            while (*entryName != 0) {
                u8 a = *entryName++;
                u8 b = *p++;
                if (a >= 0x80 || b >= 0x80) { if (a != b) break; }
                else if (caseTable[a] != caseTable[b]) break;
            }
            if (*entryName == 0 && (*p == '/' || *p == 0)) break;
            entry++;
        }
        if (entry >= fst[parent * 3 + 2]) return -1;
        if (*path == 0) return entry;
        path++;
    }
    return entry;
}

void* ARCGetStartAddrInMem(ARCFileInfo* info) {
    return (void*)((s32)info->handle->archiveStartAddr + info->startOffset);
}

s32 fn_80116A30(void *arg0) {
    return *(s32*)((u8*)arg0 + 8);
}

s32 fn_80116A38(void) {
    return 1;
}

BOOL ARCChangeDir(ARCHandle* handle, const char* path) {
    s32 entrynum = ARCConvertPathToEntrynum(handle, path);
    u32 entryAddr = (u32)handle->FSTStart + entrynum * 0xC;
    
    if (entrynum < 0 || !(*(u32*)entryAddr & 0xFF000000)) {
        return FALSE;
    }
    
    handle->currDir = entrynum;
    return TRUE;
}

BOOL ARCOpenDir(ARCHandle* handle, const char* path, ARCDir* dir) {
    s32 entrynum = ARCConvertPathToEntrynum(handle, path);
    void* fst = handle->FSTStart;
    
    if (entrynum < 0 || !(*(u32*)((u8*)fst + entrynum * 0xC) & 0xFF000000)) {
        return FALSE;
    }
    
    dir->handle = handle;
    dir->location = entrynum + 1;
    dir->entryNum = entrynum;
    dir->next = *(u32*)((u8*)fst + entrynum * (unsigned)0xC + 8);
    return TRUE;
}

BOOL ARCReadDir(ARCDir* dir, ARCDirEntry* entry) {
    ARCHandle* handle = dir->handle;
    u32 location = dir->location;
    u32 entryAddr = (u32)handle->FSTStart + location * 0xC;
    u32 entryData = *(u32*)entryAddr;
    u8 highByte = entryData >> 24;
    
    if (location >= dir->entryNum || dir->next <= location) {
        return FALSE;
    }
    
    entry->handle = handle;
    entry->entryNum = location;
    entry->isDir = (s8)highByte < 0;
    
    return TRUE;
}

u32 fn_80116BCC(void) {
    return 1;
}

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
        return (hi << (s + 1)) | (lo >> (31 - s));
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

void fn_80119214(void *arg0, void *arg1) {
    fn_8011FFB8(arg0, arg1);
}

extern void memset(void *, int, unsigned int);
void SetStreamVolumePan(void *arg0, s32 arg1, s32 arg2, s32 arg3) {
    void *temp_r31 = (u8*)arg0 + 0x14C;
    memset(temp_r31, 0, 0x30);
    *(s16*)temp_r31 = arg2;
    *(s16*)((u8*)temp_r31 + 4) = arg3;
    *(s16*)((u8*)arg0 + 0x1AC) = arg1;
    *(s16*)((u8*)arg0 + 0x1AE) = 0;
    *(s32*)((u8*)arg0 + 0x144) = 3;
    *(s32*)((u8*)arg0 + 0x1C) |= 0x212;
}

void set_rate_volume_and_pan(void *arg0) {
    if (*(void **)((u8 *)arg0 + 0x7C) != 0) {
        SetStreamVolumePan(*(void **)((u8 *)arg0 + 0x7C), *(s32 *)((u8 *)arg0 + 0xA4), *(s32 *)((u8 *)arg0 + 0xA8),
                    *(s32 *)((u8 *)arg0 + 0x78) == 1 ? *(s32 *)((u8 *)arg0 + 0xAC) : 0);
    }
    if (*(void **)((u8 *)arg0 + 0x80) != 0) {
        SetStreamVolumePan(*(void **)((u8 *)arg0 + 0x80), *(s32 *)((u8 *)arg0 + 0xA4), 0, *(s32 *)((u8 *)arg0 + 0xAC));
    }
}

extern void DCFlushRange(void *, u32);
void clear(void *arg0, u32 arg1, u32 arg2) {
    u32 len;
    u32 n;
    len = arg2;
    if (arg2 + arg1 > *(u32 *)((u8 *)arg0 + 0x18)) {
        len = *(u32 *)((u8 *)arg0 + 0x18) - arg1;
    }
    if (*(s32 *)((u8 *)arg0 + 4) != 0) {
        arg1 >>= 1;
        n = len >> 1;
        memset(*(u8 **)((u8 *)arg0 + 8) + arg1, 0, n);
        DCFlushRange(*(u8 **)((u8 *)arg0 + 8) + arg1, n);
        memset(*(u8 **)((u8 *)arg0 + 0xC) + arg1, 0, n);
        DCFlushRange(*(u8 **)((u8 *)arg0 + 0xC) + arg1, n);
        if (len < arg2) {
            n = (arg2 - len) >> 1;
            memset(*(u8 **)((u8 *)arg0 + 8), 0, n);
            DCFlushRange(*(u8 **)((u8 *)arg0 + 8), n);
            memset(*(u8 **)((u8 *)arg0 + 0xC), 0, n);
            DCFlushRange(*(u8 **)((u8 *)arg0 + 0xC), n);
        }
    } else {
        memset(*(u8 **)((u8 *)arg0 + 8) + arg1, 0, len);
        DCFlushRange(*(u8 **)((u8 *)arg0 + 8) + arg1, len);
        if (len < arg2) {
            n = arg2 - len;
            memset(*(u8 **)((u8 *)arg0 + 8), 0, n);
            DCFlushRange(*(u8 **)((u8 *)arg0 + 8), n);
        }
    }
}

extern void AXSetVoiceState(void *vpb, s32 state);
void set_both_state(void *arg0, s32 arg1) {
    void *v;
    if ((v = *(void **)arg0) != NULL) {
        AXSetVoiceState(v, arg1);
    }
    if ((v = *(void **)((u8 *)arg0 + 4)) != NULL) {
        AXSetVoiceState(v, arg1);
    }
}

void fn_80119498(void *arg0) {
    set_both_state(arg0, 1);
    *(s32 *)((u8 *)arg0 + 0x38) = 0;
}

s32 get_pos(void* arg0) {
 return *(u16*)((u8*)arg0+0x1be) + (*(u16*)((u8*)arg0+0x1bc)<<16);
}

s32 GoodGetCurrentPosition(void *arg0) {
    s32 first;
    s32 current;
    s32 result;
    do {
        first = get_pos(*(void **)arg0);
        get_pos(*(void **)arg0);
        result = get_pos(*(void **)arg0);
    } while ((u32)(result - first) >= 0x400);
    current = result * 2 - *(s32 *)((u8 *)arg0 + 0x10);
    if (*(s32 *)((u8 *)arg0 + 4) != 0)
        current *= 2;
    return current;
}

void start_streaming(void *arg0) {
    *(s32 *)((u8 *)arg0 + 0x34) = 1;
    fn_80119498(arg0);
    *(s32 *)((u8 *)arg0 + 0x48) = GoodGetCurrentPosition(arg0);
    *(s32 *)((u8 *)arg0 + 0x50) = RADTimerRead();
}

void fn_801195A0(void *arg0) {
    set_both_state(arg0, 0);
    *(s32 *)((u8 *)arg0 + 0x38) = 1;
}

extern f64 pow(f32, f64);
extern void set_rate_volume_and_pan(void *);
extern void clear(void *, u32, u32);
extern void set_both_state(void *, s32);
extern void set_addrs(void *, s32);

// Moves buffered data from a track's ring into its voice buffers, splitting interleaved stereo into the left and
// right voices.
#pragma push
#pragma opt_dead_assignments off
void try_a_mix(StreamTrack *t) {
    u16 *src;
    u32 n;
    u16 *l;
    u16 *r;
    u16 *l0;
    u16 *r0;
    u32 i;
    u16 *dst;
    s32 level;
    if (t->available > t->minimum) {
        n = t->pending;
        if (n > t->available) {
            n = t->available;
        }
        if (n + t->writePos > t->bufSize) {
            n = t->bufSize - t->writePos;
        }
        src = (u16 *)t->readPtr;
        if ((u32)src + n > (u32)t->ringEnd) {
            n = (u32)t->ringEnd - (u32)src;
        }
        if (t->channels != 1) {
            l = l0 = (u16 *)((u8 *)t->bufL + (t->writePos >> 1));
            r = r0 = (u16 *)((u8 *)t->bufR + (t->writePos >> 1));
            for (i = n >> 2; i != 0; i--) {
                *l++ = *src++;
                *r++ = *src++;
            }
            DCFlushRange(l0, (l - l0) * sizeof(u16));
            DCFlushRange(r0, (r - r0) * sizeof(u16));
        } else {
            dst = (u16 *)((u8 *)t->bufL + t->writePos);
            memcpy(dst, src, n);
            DCFlushRange(dst, n);
            src += n / 2;
        }
        level = OSDisableInterrupts();
        t->available -= n;
        OSRestoreInterrupts(level);
        if ((u32)src >= (u32)t->ringEnd) {
            src = (u16 *)t->ringStart;
        }
        t->readPtr = (u8 *)src;
        t->writePos += n;
        if (t->writePos >= t->bufSize) {
            t->writePos -= t->bufSize;
        }
        t->pending -= n;
        if (t->ending == 0 && t->pending == 0 && t->stopping == 0) {
            start_streaming(t->voices);
        }
    }
}
#pragma pop

// Keeps a track's voices fed: tops them up from the ring, or writes silence while the voices outrun the data.
s32 Ready(StreamTrack *t) {
    u32 pos;
    u32 now;
    u32 space;
    if (t->stopping != 0 || t->playing == 0) {
        return 0;
    }
    if (t->ending == 0) {
        try_a_mix(t);
        return 0;
    }
    pos = t->lastPos;
    if (RADTimerRead() - t->lastTime <= 12) {
        goto compute;
    }
    for (;;) {
        pos = GoodGetCurrentPosition(t->voices);
        now = RADTimerRead();
        t->lastPos = pos;
        if (now - t->lastTime >= t->interval) {
            t->lastTime = now;
            goto silence;
        }
        t->lastTime = now;
    compute:
        if (pos < t->writePos) {
            space = pos + t->bufSize - t->writePos;
        } else {
            space = pos - t->writePos;
        }
        space = space < 1024 ? 0 : (space - 1024) & ~3;
        if (space < t->chunk && (t->pending == 0 || space == 0)) {
            goto done;
        }
        if (space < t->threshold) {
            break;
        }
    silence:
        t->dirty = 1;
        clear(t->voices, t->writePos, t->chunk * 2);
        t->writePos += t->chunk * 2;
        if (t->writePos >= t->bufSize) {
            t->writePos -= t->bufSize;
        }
    }
    t->pending = space;
    try_a_mix(t);
done:
    return 0;
}

s32 fn_80119994(void) { return 0; }

s32 fn_8011999C(void) { return 0; }

void set_addrs(void* arg0, s32 arg1) {
    extern void AXSetVoiceAddr(void*, s16*);
    s16 rect[8];
    u32 width;
    u32 limit;
    u32 x0;
    u32 x1;
    rect[0] = 1;
    rect[1] = 10;
    width = *(u32*)((u8*)arg0 + 0x10);
    x0 = width >> 1;
    x1 = ((width + *(u32*)((u8*)arg0 + 0x1C)) >> 1) - 1;
    if (arg1 != 0)
        limit = x0;
    else
        limit = *(u32*)(*(u32*)arg0 + 0x1BC);
    rect[2] = x0 >> 16;
    rect[3] = x0;
    rect[4] = x1 >> 16;
    rect[5] = x1;
    rect[6] = limit >> 16;
    rect[7] = limit;
    AXSetVoiceAddr(*(void**)arg0, rect);
    if (*(void**)((u8*)arg0 + 4) != 0) {
        u32 height = *(u32*)((u8*)arg0 + 0x14);
        u32 y0 = height >> 1;
        u32 y1 = ((height + *(u32*)((u8*)arg0 + 0x1C)) >> 1) - 1;
        if (arg1 != 0)
            limit = y0;
        else
            limit = *(u32*)(*(u32*)((u8*)arg0 + 4) + 0x1BC);
        rect[2] = y0 >> 16;
        rect[3] = y0;
        rect[4] = y1 >> 16;
        rect[5] = y1;
        rect[6] = limit >> 16;
        rect[7] = limit;
        AXSetVoiceAddr(*(void**)((u8*)arg0 + 4), rect);
    }
}

void Volume(void *arg0, s32 arg1) {
    s32 value = arg1;
    f32 temp;
    if (value < 0) value = 0;
    if (value > 0x7fff) value = 0x7fff;
    temp = (f32) pow((f32)value, 1.6666666f);
    *(s32 *)((u8 *)arg0 + 0xa4) = (s32)(0.000976582407f * temp);
    set_rate_volume_and_pan(arg0);
}

void Pan(void *arg0, s32 arg1) {
    s32 value = arg1;
    f32 temp_f31;
    f32 temp_f3;
    f32 temp_f1;
    if (value < 0) value = 0;
    if (value > 0x10000) value = 0x10000;
    temp_f31 = (f32)value / 65536.0f;
    temp_f3 = (f32)pow(1.0f - temp_f31, 0.3f);
    *(s32*)((u8*)arg0 + 0xa8) = (s32)(32768.0f * temp_f3);
    temp_f1 = (f32)pow(temp_f31, 0.3f);
    *(s32*)((u8*)arg0 + 0xac) = (s32)(32768.0f * temp_f1);
    set_rate_volume_and_pan(arg0);
}

extern f64 pow(f32, f64);
extern void set_rate_volume_and_pan(void *);
extern void clear(void *, u32, u32);
extern void set_both_state(void *, s32);
extern void set_addrs(void *, s32);
s32 SetOnOff(void *arg0, s32 arg1) {
    u8 *p = (u8 *)arg0;
    if (arg1 != 0 && *(s32 *)(p + 0x64) == 0) {
        *(s32 *)(p + 0xb0) = 0;
        *(s32 *)(p + 0x64) = 1;
        *(s32 *)(p + 0xb8) = 0;
        *(s32 *)(p + 0xbc) = *(s32 *)(p + 0xc0);
    } else if (arg1 == 0 && *(s32 *)(p + 0x64) != 0) {
        set_both_state(p + 0x7c, 0);
        set_addrs(p + 0x7c, 1);
        clear(p + 0x7c, 0, *(s32 *)(p + 0x94));
        *(s32 *)(p + 0x64) = 0;
    }
    return *(s32 *)(p + 0x64);
}

s32 Pause(void *arg0, s32 arg1) {
    s32 *var_r3;
    if (arg1 != 0) {
        if (*(s32 *)((u8 *)arg0 + 0xb0) != 0) {
            fn_801195A0((u8 *)arg0 + 0x7c);
        }
        var_r3 = (s32 *)((u8 *)arg0 + 0xb4);
        *(s32 *)((u8 *)arg0 + 0xb4) = 1;
    } else {
        if (*(s32 *)((u8 *)arg0 + 0xb0) == 0) {
            if (*(s32 *)((u8 *)arg0 + 0xbc) == 0) {
                start_streaming((u8 *)arg0 + 0x7c);
            }
        } else {
            fn_80119498((u8 *)arg0 + 0x7c);
        }
        var_r3 = (s32 *)((u8 *)arg0 + 0xb4);
        *(s32 *)((u8 *)arg0 + 0xb4) = 0;
    }
    return *var_r3;
}

extern s32 lbl_8055DEE0[];
extern s32 lbl_8055DEE8;
struct BinkIO8011;
extern void ReadKickoff(struct BinkIO8011 *);
extern void TRKGetFreeBuffer(void *);
extern s32 lbl_8055DEE0[2];  // the sample buffer allocator: alloc, free
extern s32 lbl_8055DEE8;
extern void AXFreeVoice(void *);
typedef StreamTrack StreamChannel;  // the voice side of a track

// Stops and releases the voices of a channel, then frees its sample buffer.
void Close(void *arg0) {
    StreamChannel *ch = arg0;
    if (ch->voices[0] != 0) {
        ch->stopping = 1;
        set_both_state(ch->voices, 0);
        if (ch->voices[0] != 0) {
            AXFreeVoice(ch->voices[0]);
        }
        if (ch->voices[1] != 0) {
            AXFreeVoice(ch->voices[1]);
        }
        ((void (*)(void *))lbl_8055DEE0[1])(ch->bufL);
        lbl_8055DEE8--;
        ch->voices[0] = 0;
    }
}

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
s32 Ready(StreamTrack *t);
s32 fn_80119994(void);
s32 fn_8011999C(void);
void Volume(void *arg0, s32 arg1);
void Pan(void *arg0, s32 arg1);
s32 Pause(void *arg0, s32 arg1);
s32 SetOnOff(void *arg0, s32 arg1);

// Bink sound open for AX: sets up a track of `chans` 16-bit voices at `freq` Hz with a 600 ms buffer, prefilled with
// up to the whole movie's sound, and installs the track callbacks. Returns 0 without an allocator or voices.
s32 Open(StreamTrack *t, u32 freq, s32 bits, s32 chans, u32 flags, BinkInfo8011 *bink) {
    u32 bps;
    u32 total;
    u32 n;
    void **voice;
    u32 *vs;
    u32 i;
    u8 *buf;

    if (lbl_8055DEE0[0] == 0) {
        return 0;
    }
    memset(t, 0, sizeof(StreamTrack));
    t->dirty = 0;
    t->rate = freq;
    t->bits = 16;
    t->channels = chans;
    t->voices[0] = AXAcquireVoice(31, 0, 0);
    if (t->voices[0] == 0) {
        return 0;
    }
    if (t->channels != 1) {
        t->voices[1] = AXAcquireVoice(31, 0, 0);
        if (t->voices[1] == 0) {
            AXFreeVoice(t->voices[0]);
            return 0;
        }
    }
    bps = t->channels * (t->rate * 2);
    t->curRate = freq;
    t->bufSize = (bps * 600 / 1000 + 0x3FF) & ~0x3FF;
    t->chunk = (bps * 50 / 1000 + 0xFF) & ~0xFF;
    t->threshold = t->bufSize * 90 / 100;
    t->interval = 0x21C;
    t->unk68 = 0;
    if (bink->frameRate != 0 && bink->frameRateDiv != 0) {
        u64 len = (u64)bps * bink->frameRateDiv;
        total = len * bink->frames / bink->frameRate;
    } else {
        total = t->chunk;
    }
    if (chans != 1) {
        vs = &t->voiceSize;
        *vs = t->bufSize >> 1;
    } else {
        vs = &t->voiceSize;
        *vs = t->bufSize;
    }
    buf = ((void *(*)(u32))lbl_8055DEE0[0])(t->bufSize);
    t->bufL = (s16 *)buf;
    t->bufR = (s16 *)(buf + *vs);
    t->physL = (u32)buf - 0x80000000;
    t->physR = t->physL + *vs;
    t->minimum = t->chunk;
    t->volume = 0x7FFF;
    t->panL = 0x67F8;
    t->panR = 0x67F8;
    set_rate_volume_and_pan(t);
    set_addrs(t->voices, 1);
    voice = t->voices;
    for (i = 0; i < t->channels; i++) {
        AXPBADPCM adpcm;
        AXPBSRC src;
        memset(&adpcm, 0, sizeof(adpcm));
        adpcm.gain = 0x800;
        src.ratioHi = 1;
        src.ratioLo = 0;
        src.currentAddressFrac = 0;
        src.last_samples[0] = 0;
        src.last_samples[1] = 0;
        src.last_samples[2] = 0;
        src.last_samples[3] = 0;
        AXSetVoiceSrcType(voice[i], 1);
        AXSetVoiceSrc(voice[i], &src);
        AXSetVoiceSrcRatio(voice[i], t->rate / 32028.666666666668);
        AXSetVoiceAdpcm(voice[i], &adpcm);
    }
    clear(t->voices, 0, t->bufSize);
    n = t->bufSize;
    t->writePos = 0;
    t->ending = 0;
    if (total < n) {
        n = total;
    }
    t->prefill = n;
    t->pending = n;
    t->stopping = 0;
    t->playing = 1;
    t->IsActive = Ready;
    t->GetBuffer = (s32 (*)(StreamTrack *, u8 **, u32 *))fn_80119994;
    t->Commit = (void (*)(StreamTrack *, u32))fn_8011999C;
    t->SetVolume = (void (*)(StreamTrack *, s32))Volume;
    t->SetPan = (void (*)(StreamTrack *, s32))Pan;
    t->SetPaused = (void (*)(StreamTrack *, s32))Pause;
    t->Run = (s32 (*)(StreamTrack *, s32))SetOnOff;
    t->Close = (void (*)(StreamTrack *))Close;
    lbl_8055DEE8++;
    return 1;
}

typedef struct StreamAllocator {
    void *(*alloc)(u32 size);
    void (*free)(void *p);
} StreamAllocator;

// Installs the sample buffer allocator (0 clears it); returns the stream decode callback.
void *BinkOpenAX(StreamAllocator *allocator) {
    if (allocator == NULL) {
        lbl_8055DEE0[0] = 0;
        lbl_8055DEE0[1] = 0;
    } else {
        *(StreamAllocator *)lbl_8055DEE0 = *allocator;
    }
    return (void *)Open;
}

/* Helper name unproven. The byte-reversed load is inline asm with the zero index in a register, as in the GameCube
 * Bink's intelendian (cross-checked against n100f, reference only): the __lwbrx intrinsic blocks the x8 unroll. */
static inline u32 Swap32(register u32 *p) {
    register u32 v;
    register u32 z = 0;
#ifdef __MWERKS__
    asm { lwbrx v, z, p }
#else
    /* a little-endian word load, whatever the host byte order */
    v = ((u8 *)&p[z])[0] | ((u8 *)&p[z])[1] << 8 | ((u8 *)&p[z])[2] << 16 | (u32)((u8 *)&p[z])[3] << 24;
#endif
    return v;
}

// intelendian: byte-swaps `size` bytes (rounded up to words) in place.
void intelendian(u32 *data, u32 size) {
    u32 count = (size + 3) >> 2;
    while (count--) {
        *data = Swap32(data);
        data++;
    }
}

// Reads `len` bytes at `offset` into `dst` through an aligned stack buffer, waiting for each chunk.
// Returns the bytes read, or 0 when the drive reports an error.
u32 radreadngc(void *arg0, u32 arg1, s32 arg2, u32 arg3) {
    u8 buf[0x400 + 31];
    u8 *aligned = (u8 *)(((u32)buf + 31) & ~31);
    u32 cap = ((u32)(buf + sizeof(buf)) - (u32)aligned) & ~31;
    u32 total = 0;
    u32 chunk;
    while (arg3 != 0) {
        chunk = (arg3 + 31) & ~31;
        if (arg3 >= cap) {
            chunk = cap;
        }
        DVDReadAsyncPrio((DVDFileInfo *)arg0, aligned, chunk, arg1, 0, 0);
        while (1) {
            switch (DVDGetDriveStatus()) {
            case 0:
                goto done;
            case -1:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
                return 0;
            }
        }
    done:
        if (chunk > arg3) {
            chunk = arg3;
        }
        memcpy((void *)arg2, aligned, chunk);
        arg2 += chunk;
        total += chunk;
        arg3 -= chunk;
        arg1 += chunk;
    }
    return total;
}

void CancelReadRequests(void *arg0) {
    s32 status;
    *(volatile s32 *)((u8 *)arg0 + 0xC4) = 1;
    DVDCancel((DVDCommandBlock *)((u8 *)arg0 + 0x54));
    do {
        status = DVDGetDriveStatus();
    } while (status == 2 || status == 1);
    *(volatile s32 *)((u8 *)arg0 + 0xC4) = 0;
    *(volatile s32 *)((u8 *)arg0 + 0x24) = 0;
}

u32 BinkFileReadHeader(void* arg0, s32 arg1, s32 arg2, u32 arg3) {
    extern u32 radreadngc(void*, u32, s32, u32);
    extern void CancelReadRequests(void*);
    u32 result, current, remaining;
    if (arg1 != -1 && *(u32*)((u8*)arg0 + 0x90) != (u32)arg1) {
        *(u32*)((u8*)arg0 + 0x90) = (u32)arg1;
    }
    result = radreadngc((u8*)arg0 + 0x54,
                         *(u32*)((u8*)arg0 + 0x90) + *(u32*)((u8*)arg0 + 0xB4),
                         arg2, arg3);
    if (result != arg3) {
        *(s32*)((u8*)arg0 + 0x20) = 1;
        CancelReadRequests(arg0);
    }
    // MATCHING-AID?: the read position (0x90) and cap (0x40) are re-read like volatile fields; probably genuine
    // async stream state (the 0xC0 counter in dosimulate is volatile too). Revisit with a real struct.
    *(volatile u32*)((u8*)arg0 + 0x90) += result;
    current = *(volatile u32*)((u8*)arg0 + 0x90);
    *(u32*)((u8*)arg0 + 0x98) = current;
    remaining = *(u32*)((u8*)arg0 + 0xB8) - current;
    *(u32*)((u8*)arg0 + 0x48) = remaining < *(volatile u32*)((u8*)arg0 + 0x40) ? remaining : *(volatile u32*)((u8*)arg0 + 0x40);
    intelendian((u32*)arg2, result);
    return result;
}

void dosimulate(void *arg0, s32 arg1, s32 arg2) {
    s32 temp_r0, temp_r31, temp_r3, temp_r3_2, temp_r4, var_r31;
    s32 *counter = (s32 *)((u8 *)arg0 + 0xC0);
    temp_r31 = mult64anddiv(arg1, 0x3E8, *(s32 *)((u8 *)arg0 + 0xBC));
    temp_r3 = RADTimerRead();
    var_r31 = temp_r3;
    *counter = *counter + (temp_r31 - (temp_r3 - arg2));
    while (*counter > 0) {
        do {
            temp_r3_2 = RADTimerRead();
            temp_r0 = *counter;
            temp_r4 = temp_r3_2 - var_r31;
        } while (temp_r4 < temp_r0);
        var_r31 = temp_r3_2;
        *counter = temp_r0 - temp_r4;
    }
}

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

// DVDReadCallback: an async read of the ring finished. Checks the length (a full 0x8000 block, or the rest
// of the file plus the alignment skip), drops the skip bytes, advances the ring and the counters, then starts
// the next read unless the reader is suspended.
static __inline u32 BinkLeft(BinkFile8011 *f) { return f->fileSize - f->filePos; }

void DVDReadCallback(s32 result, DVDFileInfo *fi) {
    BinkIO8011 *io = ((BinkFile8011 *)fi)->io;

    if (BINKFILE(io)->cancelling)
        return;

    if (result != 0x8000) {
        u32 left = BinkLeft(BINKFILE(io));
        if (result != left + BINKFILE(io)->skip) {
            io->doingRead = 0;
            io->readError = 1;
            return;
        }
    }

    if (BINKFILE(io)->skip) {
        memmove(BINKFILE(io)->bufTail, BINKFILE(io)->bufTail + BINKFILE(io)->skip, BINKFILE(io)->fileSize - BINKFILE(io)->filePos);
        result -= BINKFILE(io)->skip;
        BINKFILE(io)->skip = 0;
    }

    io->bytesRead += result;
    BINKFILE(io)->filePos += result;
    BINKFILE(io)->bufTail += result;
    if (BINKFILE(io)->bufTail >= BINKFILE(io)->bufEnd)
        BINKFILE(io)->bufTail = BINKFILE(io)->buf;
    BINKFILE(io)->bufEmpty -= result;
    io->curBufUsed += result;
    if (io->curBufUsed > io->bufHighUsed)
        io->bufHighUsed = io->curBufUsed;

    if (BINKFILE(io)->simulate)
        dosimulate(io, result, BINKFILE(io)->readStart);

    BINKFILE(io)->readStart = RADTimerRead() - BINKFILE(io)->readStart;
    io->totalTime += BINKFILE(io)->readStart;
    if (BINKFILE(io)->wasWorking || io->working)
        io->threadTime += BINKFILE(io)->readStart;
    else
        io->idleTime += BINKFILE(io)->readStart;

    io->doingRead = 0;
    if (io->unk50 == 0)
        ReadKickoff(io);
}

// ReadKickoff: flags a drive error, or (when the drive is idle and no cancel is in progress) starts the next
// async read of up to 0x8000 bytes into the ring's tail; the file's last block is padded to 32 bytes by
// starting the read earlier (`skip`). With no room or nothing left, the current size is what is buffered.
void ReadKickoff(BinkIO8011 *io) {
    s32 status = DVDGetDriveStatus();
    u32 left = BINKFILE(io)->fileSize - BINKFILE(io)->filePos;

    switch (status) {
    case -1:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 11:
        io->readError = 1;
        return;
    }

    if (BINKFILE(io)->cancelling || (status != 0 && status != 10))
        return;
    {
        if (BINKFILE(io)->bufEmpty >= 0x8000 && left) {
            BINKFILE(io)->wasWorking = io->working;
            BINKFILE(io)->readStart = RADTimerRead();
            io->doingRead = 1;
            if (left > 0x8000)
                left = 0x8000;
            else if (left & 31) {
                BINKFILE(io)->skip = -left & 31;
                left += BINKFILE(io)->skip;
            }
            DVDReadAsyncPrio(&BINKFILE(io)->fi, BINKFILE(io)->bufTail, left,
                             BINKFILE(io)->filePos - BINKFILE(io)->skip + BINKFILE(io)->startOfs, DVDReadCallback, 0);
        } else {
            io->curBufSize = io->curBufUsed;
        }
    }
}

s32 BinkFileIdle(DVDFileInfo *info) {
    s32 *p = (s32 *)info;
    s32 status;
    if (p[0x20 / 4] != 0) return 0;
    if (p[0x50 / 4] != 0) return 0;
    status = DVDGetDriveStatus();
    switch (status) {
    case 0:  /* END */
        ReadKickoff((struct BinkIO8011 *)info);  // info is the BinkIO8011 itself
        break;
    case -1: /* FATAL_ERROR */
    case 3:  /* COVER_CLOSED */
    case 4:  /* NO_DISK */
    case 5:  /* COVER_OPEN */
    case 6:  /* WRONG_DISK */
    case 7:  /* MOTOR_STOPPED */
    case 8:  /* PAUSING */
    case 9:  /* IGNORED */
    case 11: /* RETRY */
        p[0x20 / 4] = 1;
        break;
    }
    return p[0x24 / 4];
}

// 0x50 is a state flag the DVD callbacks also change: every access is volatile.
s32 BinkFileBGControl(void *arg0, s32 arg1) {
    if (arg1 & 1) {
        if (*(volatile s32 *)((char *)arg0 + 0x50) == 0)
            *(volatile s32 *)((char *)arg0 + 0x50) = 1;
        if (arg1 & 0x80000000) {
            // busy-wait until the flag is cleared elsewhere (volatile: re-read every pass)
            while (*(volatile s32 *)((char *)arg0 + 0x24) != 0) {
            }
        }
    } else if (arg1 & 2) {
        if ((u32)*(volatile s32 *)((char *)arg0 + 0x50) == 1U)
            *(volatile s32 *)((char *)arg0 + 0x50) = 0;
        if (arg1 & 0x80000000)
            BinkFileIdle(arg0);
    }
    return *(volatile s32 *)((char *)arg0 + 0x50);
}


extern u32 radreadngc(void *, u32, s32, u32);
extern void dosimulate(void *, s32, s32);
extern void intelendian(u32 *, u32);
// BinkFileReadFrame: reads `size` bytes of frame data at `offset` (-1 = the current position) into dest.
// A seek inside the buffered window just drops the skipped bytes; any other seek cancels the pending reads
// and restarts the ring there. Unbuffered files read directly (synchronously); buffered ones wait on the ring
// (ReadKickoff = ReadKickoff), copying out in at most two pieces around its end. Returns the bytes read,
// byte-swapped to little-endian words like the rest of the file.
u32 BinkFileReadFrame(BinkIO8011 *io, u32 frame, s32 offset, void *dest, u32 size) {
    u32 start, t, avail, chunk, x;
    BOOL enabled;
    u32 amt = 0;
    void *orig = dest;

    if (io->readError)
        return 0;

    start = RADTimerRead();

    if (offset != -1 && BINKFILE(io)->bufPos != offset) {
        if ((u32)offset > BINKFILE(io)->bufPos && (u32)offset <= BINKFILE(io)->filePos) {
            enabled = OSDisableInterrupts();
            x = offset - BINKFILE(io)->bufPos;
            BINKFILE(io)->bufEmpty += x;
            BINKFILE(io)->bufPos = offset;
            io->curBufUsed -= x;
            BINKFILE(io)->bufHead += x;
            if (BINKFILE(io)->bufHead > BINKFILE(io)->bufEnd)
                BINKFILE(io)->bufHead -= io->bufSize;
            OSRestoreInterrupts(enabled);
        } else {
            CancelReadRequests(io);
            enabled = OSDisableInterrupts();
            BINKFILE(io)->filePos = offset;
            BINKFILE(io)->bufPos = offset;
            BINKFILE(io)->bufEmpty = io->bufSize;
            io->curBufUsed = 0;
            BINKFILE(io)->bufHead = BINKFILE(io)->buf;
            BINKFILE(io)->bufTail = BINKFILE(io)->buf;
            OSRestoreInterrupts(enabled);
        }
    }

    if (BINKFILE(io)->buf == 0) {
        t = RADTimerRead();
        amt = radreadngc(&BINKFILE(io)->fi, BINKFILE(io)->filePos, (s32)dest, size);
        if (amt < size) {
            io->readError = 1;
            CancelReadRequests(io);
        }
        BINKFILE(io)->filePos += amt;
        BINKFILE(io)->bufPos += amt;
        io->bytesRead += amt;
        if (BINKFILE(io)->simulate)
            dosimulate(io, amt, t);
        x = RADTimerRead();
        io->totalTime += x - t;
        io->foregroundTime += x - start;
    } else {
        while (size && !io->readError) {
            ReadKickoff(io);
            avail = io->curBufUsed;
            if (avail == 0)
                continue;
            if (avail > size)
                avail = size;
            BINKFILE(io)->bufPos += avail;
            size -= avail;
            amt += avail;
            chunk = BINKFILE(io)->bufEnd - BINKFILE(io)->bufHead;
            if (chunk <= avail) {
                memcpy(dest, BINKFILE(io)->bufHead, chunk);
                dest = (u8 *)dest + chunk;
                avail -= chunk;
                BINKFILE(io)->bufHead = BINKFILE(io)->buf;
                enabled = OSDisableInterrupts();
                io->curBufUsed -= chunk;
                BINKFILE(io)->bufEmpty += chunk;
                OSRestoreInterrupts(enabled);
            }
            if (avail) {
                memcpy(dest, BINKFILE(io)->bufHead, avail);
                dest = (u8 *)dest + avail;
                BINKFILE(io)->bufHead += avail;
                enabled = OSDisableInterrupts();
                io->curBufUsed -= avail;
                BINKFILE(io)->bufEmpty += avail;
                OSRestoreInterrupts(enabled);
            }
        }
        io->foregroundTime += RADTimerRead() - start;
    }

    x = BINKFILE(io)->fileSize - BINKFILE(io)->bufPos;
    io->curBufSize = (x < io->bufSize) ? x : io->bufSize;
    if (io->curBufUsed + 0x8000 > io->curBufSize)
        io->curBufSize = io->curBufUsed;

    intelendian((u32 *)orig, amt);
    return amt;
}

s32 fn_8011AC04(s32 arg0, s32 arg1) {
    return (arg1 + 0x7FFF) & 0xFFFF8000;
}

void BinkFileSetInfo(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32* p = (s32*)arg0;
    s32 level = OSDisableInterrupts();
    arg2 &= 0xFFFF8000;
    p[0xA0/4] = arg2;
    p[0x40/4] = arg2;
    p[0xA4/4] = arg1;
    p[0x9C/4] = arg1;
    p[0xAC/4] = arg1;
    p[0xA8/4] = arg1 + arg2;
    p[0x4C/4] = 0;
    p[0xB8/4] = arg3;
    p[0xBC/4] = arg4;
    p[0xD0/4] = (s32)arg0;
    OSRestoreInterrupts(level);
}

extern f32 lbl_80329D48[];
extern void CancelReadRequests(void *);
void BinkFileClose(void *arg0) {
    CancelReadRequests(arg0);
    if (*(s32 *)((u8 *)arg0 + 0xB0) == 0) {
        DVDClose((DVDFileInfo *)((u8 *)arg0 + 0x54));
    }
}

extern u32 BinkFileReadHeader(void*, s32, s32, u32);
extern u32 BinkFileReadFrame(BinkIO8011 *, u32, s32, void *, u32);
extern s32 fn_8011AC04(s32, s32);
extern void BinkFileSetInfo(void*, s32, s32, s32, s32);
extern s32 BinkFileIdle(DVDFileInfo*);
extern void BinkFileClose(void*);
extern s32 BinkFileBGControl(void*, s32);
// BinkFileOpen: sets up `io` as a DVD file reader for `name`, or (flag 0x800000) for an already open
// DVDFileInfo passed in place of the name, whose userData is the movie's offset in that file.
s32 BinkFileOpen(BinkIO8011 *io, const char *name, u32 flags) {
    memset(io, 0, sizeof(BinkIO8011));

    if (flags & 0x800000) {
        BINKFILE(io)->fi = *(DVDFileInfo *)name;
        BINKFILE(io)->dontClose = 1;
        BINKFILE(io)->startOfs = (u32)BINKFILE(io)->fi.block.userData;
    } else {
        s32 entry = DVDConvertPathToEntrynum(name);
        if (entry == -1)
            return 0;
        if (!DVDFastOpen(entry, &BINKFILE(io)->fi))
            return 0;
    }

    io->readHeader = (void *)BinkFileReadHeader;
    io->readFrame = (void *)BinkFileReadFrame;
    io->getBufferSize = (void *)fn_8011AC04;
    io->setInfo = (void *)BinkFileSetInfo;
    io->idle = (void *)BinkFileIdle;
    io->close = (void *)BinkFileClose;
    io->unk18 = (void *)BinkFileBGControl;
    return 1;
}

f32 fxptof(s32 arg0) {
    u32 bits = (u32)arg0;
    s32 magnitude = (bits & ~0x10000000U) >> 5;
    f32 temp = (f32)magnitude * lbl_80329D48[bits & 31U];
    if (bits & 0x10000000U) {
        temp = -temp;
    }
    return temp;
}

// Scales float samples by `gain` into clamped 16-bit PCM.
static inline u32 Clamp16(u32 v, u32 lo, u32 hi) {  /* Helper name unproven */
    u64 t;
    u32 d;
    v ^= 0x80000000;
    t = (u64)v - lo;
    d = (u32)t;
    d &= ~(u32)(t >> 32);
    v = d - 32768;
    v ^= 0x80000000;
    t = (u64)v - hi;
    d = (u32)t;
    d &= (u32)(t >> 32);
    return d + 32767;
}
#pragma push  /* RAD built this routine without constant propagation or speculative unrolling (Process review) */
#pragma global_optimizer off  /* same code as lifetimes/propagation/unroll_loops off; see ExpandPlane */
#pragma ppc_unroll_speculative off
void quanttos16s(s16 *arg0, f32 *arg1, u32 arg2, f32 farg0) {
    u32 lo = 0x7FFF8000;
    s16 *out = arg0 - 1;
    u32 hi = 0x80007FFF;
    u32 i;
    u32 v;
    u32 d;
    u64 t;
    for (i = 0; i < arg2; i++) {
        v = (s32)(*arg1 * farg0);
        v ^= 0x80000000;
        t = (u64)v - lo;
        d = (u32)t;
        d &= ~(u32)(t >> 32);
        v = d - 32768;
        v ^= 0x80000000;
        t = (u64)v - hi;
        d = (u32)t;
        d &= (u32)(t >> 32);
        v = d + 32767;
        arg1++;
        *++out = v;
    }
}
#pragma pop

// Interleaves two float channels (left at arg1, right after it) into clamped 16-bit stereo PCM.
#pragma push
#pragma global_optimizer off  /* same code as lifetimes/propagation/unroll_loops off; see ExpandPlane */
#pragma ppc_unroll_speculative off
void quanttos16chans2(s16 *arg0, f32 *arg1, u32 arg2, f32 farg0) {
    u32 l;
    f32 *right = arg1 + arg2;
    u32 d;
    s16 *out = arg0 - 1;
    u32 lo = 0x7FFF8000;
    u32 hi = 0x80007FFF;
    u32 r;
    u64 t;
    u64 t2;
    u64 t3;
    u64 t4;
    u32 i;
    for (i = 0; i < arg2; i++) {
        l = (s32)(*arg1 * farg0);
        r = (s32)(farg0 * *right);
        l ^= 0x80000000;
        t = (u64)l - lo;
        d = (u32)t;
        d &= ~(u32)(t >> 32);
        l = d - 32768;
        l ^= 0x80000000;
        t2 = (u64)l - hi;
        d = (u32)t2;
        d &= (u32)(t2 >> 32);
        l = d + 32767;
        r ^= 0x80000000;
        t3 = (u64)r - lo;
        d = (u32)t3;
        d &= ~(u32)(t3 >> 32);
        r = d - 32768;
        r ^= 0x80000000;
        t4 = (u64)r - hi;
        d = (u32)t4;
        d &= (u32)(t4 >> 32);
        r = d + 32767;
        out[1] = l;
        right++;
        arg1++;
        out += 2;
        *out = r;
    }
}
#pragma pop

extern u8 lbl_80329B50[];
void read_rle_samples(f32 *arg0, s32 arg1, void *arg2, f32 *arg3, s32 arg4) {
    u32 var_r29;
    s32 var_r28;
    f32 *var_r27;
    u32 *var_r26;
    u32 var_r25;
    u32 var_r24;
    u32 *var_r7;
    f32 var_f1;
    f32 var_f29;
    f32 var_f28;
    u32 *var_r5;
    u32 *var_r5_2;
    f32 *var_r8;
    s32 temp_r0;
    int temp_r18;
    s32 temp_r4;
    s32 temp_r4_2;
    s32 temp_r5_3;
    s32 temp_r6;
    struct { f32 *b; f32 *a; } rd;  /* Struct unproven: the two float read cursors as fields of a small local */
    s32 temp_r6_2;
    s32 var_ctr;
    s32 var_r0_2;
    s32 var_r5_3;
    s32 var_r0_3;
    u32 temp_r9;
    u32 temp_r10;
    u32 temp_r5;
    u32 temp_r5_2;
    u32 var_r0;
    u32 var_r24_2;
    u32 var_r3;
    u32 var_r4_3;

    var_f28 = var_f29 = 0.0f;
    var_r26 = ((BitReader8011 *)arg2)->next;
    var_r25 = ((BitReader8011 *)arg2)->value;
    var_r24 = ((BitReader8011 *)arg2)->count;
    rd.a = arg3;
    var_r5 = (u32 *)arg4;
    var_r28 = 0;
    goto init_test;
init_body:
    var_f29 = *rd.a;
    rd.a++;
    var_r5++;
    var_r28++;
    var_f28 = -var_f29;
init_test:
    var_r0 = (u32)*var_r5 << 1;
    if (var_r0 < 2U) goto init_body;

    var_r27 = (f32 *)((u8 *)arg0 + 8);
    var_r3 = 2U;
    goto outer_test;

decode:
    if (var_r24 == 0) {
        var_r0 = *var_r26;
        var_r24 = 0x1F;
        var_r26++;
        var_r25 = var_r0 >> 1U;
    } else {
        var_r0 = var_r25;
        var_r25 = var_r25 >> 1U;
        var_r24--;
    }
    if (var_r0 & 1) {
        if (var_r24 < 4U) {
            temp_r5 = *var_r26;
            temp_r0 = 4 - var_r24;
            var_r26++;
            temp_r4 = temp_r5 << var_r24;
            var_r24 += 0x1C;
            temp_r4_2 = var_r25 | temp_r4;
            var_r25 = temp_r5 >> temp_r0;
            var_r0_2 = temp_r4_2 & 0xF;
        } else {
            var_r0_2 = var_r25 & 0xF;
            var_r25 = var_r25 >> 4U;
            var_r24 -= 4;
        }
        var_r29 = var_r3 + (lbl_80329B50[var_r0_2] * 8U);
    } else {
        var_r29 = var_r3 + 8U;
    }
    if (var_r29 > (u32)arg1) var_r29 = arg1;

    if (var_r24 < 4U) {
        temp_r5_2 = *var_r26;
        temp_r0 = 4 - var_r24;
        var_r26++;
        temp_r4 = temp_r5_2 << var_r24;
        var_r24 += 0x1C;
        temp_r4_2 = var_r25 | temp_r4;
        var_r25 = temp_r5_2 >> temp_r0;
        var_r0_3 = temp_r4_2 & 0xF;
    } else {
        var_r0_3 = var_r25 & 0xF;
        var_r25 = var_r25 >> 4U;
        var_r24 -= 4;
    }

    if (var_r0_3 == 0) {
        temp_r18 = var_r29 - var_r3;
        memset(var_r27, 0, temp_r18 * 4);
        rd.b = arg3 + var_r28;
        var_r3 = var_r29;
        var_r27 += temp_r18;
        var_r5_2 = (u32 *)arg4 + var_r28;
        while (var_r29 > (u32)(*var_r5_2 * 2)) {
            var_f29 = *rd.b;
            rd.b++;
            var_r5_2++;
            var_r28++;
            var_f28 = -var_f29;
        }
    } else {
        temp_r6 = var_r28 * 4;
        var_r7 = (u32 *)arg4 + var_r28;
        var_r8 = arg3 + var_r28;
        temp_r9 = -1U >> (0x20 - var_r0_3);
        temp_r18 = var_r29 - var_r3;
        while (var_r3 < var_r29) {
            if (var_r3 == (u32)(*var_r7 * 2)) {
                var_f29 = *var_r8;
                var_r7++;
                var_r8++;
                var_r28++;
                var_f28 = -var_f29;
            }
            if ((u32)var_r24 < var_r0_3) {
                temp_r10 = *var_r26;
                var_r26++;
                temp_r6_2 = var_r25 | (temp_r10 << var_r24);
                var_r25 = temp_r10 >> (var_r0_3 - var_r24);
                var_r5_3 = temp_r9 & temp_r6_2;
                var_r24 = var_r24 + 0x20 - var_r0_3;
            } else {
                var_r5_3 = var_r25 & temp_r9;
                var_r25 = var_r25 >> var_r0_3;
                var_r24 -= var_r0_3;
            }
            if (var_r5_3 != 0) {
                if (var_r24 == 0) {
                    var_r4_3 = *var_r26;
                    var_r24 = 0x1F;
                    var_r26++;
                    var_r25 = var_r4_3 >> 1U;
                } else {
                    var_r4_3 = var_r25;
                    var_r25 = var_r25 >> 1U;
                    var_r24--;
                }
                if (var_r4_3 & 1) var_f1 = var_f28;
                else var_f1 = var_f29;
                *var_r27 = (f32)var_r5_3 * var_f1;
                var_r27++;
            } else {
                *var_r27 = 0.0f;
                var_r27++;
            }
            var_r3++;
        }
    }
    goto outer_test;

outer_test:
    if (var_r3 < (u32)arg1) goto decode;
    ((BitReader8011 *)arg2)->next = var_r26;
    ((BitReader8011 *)arg2)->value = var_r25;
    ((BitReader8011 *)arg2)->count = var_r24;
}

s32 Unquant(s32 size, f32 gain, u32 channels, s32 flags, s32 arg3, s32 arg4, s32 output, u32 *input, u32 bands, s32 arg8) {
    f32 buffer[4096];
    f32 factors[26];
    BitReader8011 bits;
    s32 mode;
    u32 i, j, index;
    f32 *p;
    f32 *q;
    mode = flags & 1;
    bits.start = input;
    bits.next = input;
    bits.count = 0;
    bits.value = 0;
    if (mode) {
        if (bits.count < 2) {
            bits.value = *bits.next >> (2 - bits.count);
            bits.count += 30;
            bits.next++;
        } else {
            bits.value >>= 2;
            bits.count -= 2;
        }
    }
    p = buffer;
    for (i = 0; i < channels; i++) {
        {
            u32 value;
            if (bits.count < 29) {
                u32 word = *bits.next;
                value = bits.value | (word << bits.count);
                bits.value = word >> (29 - bits.count);
                index = value & 0x1fffffff;
                bits.count += 3;
                bits.next++;
            } else {
                bits.count -= 29;
                value = bits.value;
                index = value & 0x1fffffff;
                bits.value >>= 29;
            }
        }
        p[0] = fxptof(index);
        {
            u32 value;
            if (bits.count < 29) {
                u32 word = *bits.next;
                value = bits.value | (word << bits.count);
                bits.value = word >> (29 - bits.count);
                index = value & 0x1fffffff;
                bits.count += 3;
                bits.next++;
            } else {
                bits.count -= 29;
                value = bits.value;
                index = value & 0x1fffffff;
                bits.value >>= 29;
            }
        }
        p[1] = fxptof(index);
        q = factors;
        for (j = 0; j < bands; j++) {
            {
                u32 value;
                u32 have = bits.count;
                if (have < 8) {
                    u32 word = *bits.next;
                    value = bits.value | (word << have);
                    bits.value = word >> (8 - have);
                    index = value & 0xff;
                    bits.count += 24;
                    bits.next++;
                } else {
                    bits.count -= 8;
                    value = bits.value;
                    index = value & 0xff;
                    bits.value >>= 8;
                }
            }
            if (index > 95) index = 95;
            *q++ = lbl_80329BC8[index];
        }
        read_rle_samples(p, size, &bits, factors, arg8);
        if (mode) ddct(size, 1, p, (int *)arg3, (f32 *)arg4);
        else rdft(size, -1, p, (int *)arg3, (f32 *)arg4);
        p += size;
    }
    if (channels == 1) quanttos16s((s16 *)output, buffer, size, gain);
    else quanttos16chans2((s16 *)output, buffer, size, gain);
    bits.count = 0;
    return ((u32)bits.next - (u32)bits.start) & 0x1fffffff;
}


void pushmalloc(void **slot, u32 amount);
extern u32 lbl_80329B60[];  // critical band frequencies (Hz)

// MSL's inline sqrtf: three Newton steps from the frsqrte estimate.
static inline f32 Sqrtf8011(f32 x) {
    if (x > 0.0f) {
        f64 guess = __frsqrte((f64)x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        return (f32)(guess * x);
    }
    return x;
}

// Opens a Bink audio decoder for `channels` channels at `rate` Hz; returns 0 when out of memory.
BinkAudio8011 *BinkAudioDecompressOpen(u32 rate, u32 channels, u32 flags) {
    u32 *bands;
    s32 *quant;
    void *transform;
    s16 *previous;
    s16 *output;
    u32 numBands;
    u32 frameSize;
    u32 half;
    u32 frameBytes;
    u32 nyquist;
    s32 dct;
    u32 i;
    BinkAudio8011 *a;

    if (rate >= 44100) {
        frameSize = 2048;
    } else if (rate >= 22050) {
        frameSize = 1024;
    } else {
        frameSize = 512;
    }
    frameBytes = frameSize * channels * 2;
    dct = flags & 1;
    if (!dct) {
        rate *= channels;
        frameSize *= channels;
        channels = 1;
    }
    half = frameSize >> 1;
    nyquist = (rate + 1) >> 1;
    for (numBands = 0; numBands < 25; numBands++) {
        if (lbl_80329B60[numBands] >= nyquist) {
            break;
        }
    }
    pushmalloc((void **)&bands, (numBands + 1) * 4);
    pushmalloc((void **)&quant, ((s32)Sqrtf8011(half) + 2) * 4);
    if (dct) {
        pushmalloc(&transform, frameSize * 20 / 4);
    } else {
        pushmalloc(&transform, half * 4 - 1);
    }
    pushmalloc((void **)&previous, frameBytes / 2);
    pushmalloc((void **)&output, frameBytes);
    a = popmalloc(sizeof(BinkAudio8011));
    if (a == 0) {
        return 0;
    }
    memset(a, 0, sizeof(BinkAudio8011));
    a->bands = bands;
    a->quant = quant;
    a->transform = transform;
    a->previous = previous;
    a->output = output;
    a->flags = flags;
    a->channels = channels;
    a->numBands = numBands;
    a->frameSize = frameSize;
    a->frameBytes = frameBytes;
    a->overlapBytes = frameBytes / 16;
    a->scale = 2.0f / Sqrtf8011(frameSize);
    for (i = 0; i < numBands; i++) {
        a->bands[i] = half * lbl_80329B60[i] / nyquist;
        if (a->bands[i] == 0) {
            a->bands[i] = 1;
        }
    }
    a->bands[i] = half;
    a->quant[0] = 0;
    a->first = 1;
    return a;
}

// Decodes one Bink audio frame from `src`: returns the output samples (cross-faded with the previous frame's tail)
// and their size, saves the new tail, and sets *next past the frame's data.
void BinkAudioDecompress(BinkAudio8011 *a, s16 **out, u32 *outLen, u8 *src, u8 **next) {
    s32 used = Unquant(a->frameSize, a->scale, a->channels, a->flags, (s32)a->quant, (s32)a->transform,
                           (s32)a->output, (u32 *)src, a->numBands, (s32)a->bands);
    if (a->first) {
        a->first = 0;
    } else {
        u32 i;
        u32 count = a->overlapBytes >> 1;
        for (i = 0; i < count; i++) {
            a->output[i] = (i * a->output[i] + a->previous[i] * (count - i)) / count;
        }
    }
    memcpy(a->previous, (u8 *)a->output + (a->frameBytes - a->overlapBytes), a->overlapBytes);
    if (outLen) {
        *outLen = a->frameBytes - a->overlapBytes;
    }
    if (out) {
        *out = a->output;
    }
    if (next) {
        *next = src + used;
    }
}

void fn_8011BC48(s32 arg0) {
    radfree((void *)arg0);
}

// Unlink node from the singly linked list at +0x30 of owner (next pointer at +0). Returns 1 if it was there.
s32 remove_cb_from_list(void *owner, void *node) {
    void *p;
    if (owner == NULL) return 0;
    p = *(void **)((u8 *)owner + 0x30);
    if (p == node) {
        *(void **)((u8 *)owner + 0x30) = *(void **)p;
        return 1;
    }
    while (*(void **)p != NULL) {
        if (*(void **)p == node) {
            *(void **)p = *(void **)node;
            return 1;
        }
        p = *(void **)p;
    }
    return 0;
}

void add_sorted(void **head, void *node) {
    u32 key;
    void *cur = *head;
    if (cur == 0) {
        *(void **)((u8 *)node + 4) = 0;
        *head = node;
        return;
    }
    key = *(u32 *)((u8 *)node + 8);
    if (key >= *(u32 *)((u8 *)cur + 8)) {
        *(void **)((u8 *)node + 4) = cur;
        *head = node;
        return;
    }
    while (*(void **)((u8 *)cur + 4) != 0) {
        void *next = *(void **)((u8 *)cur + 4);
        if (key >= *(u32 *)((u8 *)next + 8)) {
            *(void **)((u8 *)node + 4) = next;
            *(void **)((u8 *)cur + 4) = node;
            return;
        }
        cur = next;
    }
    *(void **)((u8 *)node + 4) = 0;
    *(void **)((u8 *)cur + 4) = node;
}

// Ticks every active request of the queue; finished ones are collected and then told so.
void call_callbacks(void *arg0) {
    void *done;
    void *n;
    *(s32 *)((u8 *)arg0 + 0x34) += 1;
    if (*(s32 *)arg0 == 1) {
        n = *(void **)((u8 *)arg0 + 0x30);
        done = 0;
        while (n != 0) {
            if (*(s32 *)arg0 != 1) {
                return;
            }
            if (*(s32 *)((u8 *)n + 0xC) == 1) {
                *(s32 *)((u8 *)n + 8) = (*(s32 (**)(void *, s32))((u8 *)n + 0x14))(n, *(s32 *)((u8 *)arg0 + 0x34));
                if (*(s32 *)((u8 *)n + 8) != 0) {
                    add_sorted(&done, n);
                }
            }
            n = *(void **)n;
        }
        for (n = done; n != 0; n = *(void **)((u8 *)n + 4)) {
            (*(void (**)(void *, s32))((u8 *)n + 0x18))(n, *(s32 *)((u8 *)arg0 + 0x34));
        }
    }
}

void RAD_callback_timer(s32 arg0) {
    s32 temp;
    s32 old;
    temp = arg0 - 8;
    if (temp != 0 && *(s32 *)(temp + 0x30) != 0) {
        old = OSEnableInterrupts();
        call_callbacks((void *)temp);
        OSRestoreInterrupts(old);
    }
}

// Creates a sound request queue for `count` entries (negative counts are taken as positive).
void *RADCB_allocate_handler(s32 count) {
    void *q = radmalloc(0x40);
    if (q != NULL) {
        memset(q, 0, 0x40);
        *(s32 *)((u8 *)q + 0x00) = 0;
        *(s32 *)((u8 *)q + 0x04) = 1;
        *(s32 *)((u8 *)q + 0x30) = 0;
        *(s32 *)((u8 *)q + 0x34) = 1;
        *(s32 *)((u8 *)q + 0x38) = 0;
        if (count < 0) {
            count = -count;
        }
        *(s32 *)((u8 *)q + 0x3C) = count;
    }
    return q;
}

// Queues request `arg1` (callbacks arg2/arg3) on `arg0` with interrupts disabled.
void RADCB_register_callback(void *arg0, void *arg1, s32 arg2, s32 arg3) {
    s32 level = OSDisableInterrupts();
    *(s32 *)((u8 *)arg1 + 0xC) = 1;
    *(s32 *)((u8 *)arg1 + 0x14) = arg2;
    *(s32 *)((u8 *)arg1 + 0x18) = arg3;
    *(s32 *)((u8 *)arg1 + 0x10) = 0;
    *(void **)arg1 = *(void **)((u8 *)arg0 + 0x30);
    *(void **)((u8 *)arg0 + 0x30) = arg1;
    *(s32 *)((u8 *)arg0 + 0x38) += 1;
    OSRestoreInterrupts(level);
}

void RADCB_free_handler(s32 arg0);
s32 RADCB_unregister_callback(s32 arg0, s32 arg1, s32 arg2) {
    s32 result = 0;
    void *owner;
    void *node;
    s32 level;
    owner = (void *)arg0;
    node = (void *)arg1;
    if (arg0 != 0 && arg1 != 0 && *(s32 *)(arg1 + 0x18) != 0 && *(s32 *)(arg1 + 0x14) != 0) {
        level = OSDisableInterrupts();
        if (remove_cb_from_list(owner, node) != 0) {
            *(s32 *)(arg0 + 0x38) -= 1;
            if ((arg2 & 1) && *(s32 *)(arg0 + 0x30) == 0) {
                RADCB_free_handler(arg0);
                result = 1;
            }
        }
        OSRestoreInterrupts(level);
    }
    return result;
}

s32 RADCB_unregister_2_callbacks(s32 arg0, s32 arg1, void *arg2, s32 arg3, s32 arg4) {
    s32 temp = RADCB_unregister_callback((s32)arg2, arg3, arg4) << 1;
    s32 result = RADCB_unregister_callback(arg0, arg1, arg4);
    return result | temp;
}

u32 RADCB_try_to_suspend_handler(void *arg0) {
    s32 oldInterrupts;
    s32 value;
    if (arg0 != NULL) {
        oldInterrupts = OSDisableInterrupts();
        if (*(s32 *)arg0 == 1)
            *(s32 *)arg0 = 2;
        value = *(s32 *)((char *)arg0 + 4);
        *(s32 *)((char *)arg0 + 4) = value + 1;
        OSRestoreInterrupts(oldInterrupts);
        value = *(s32 *)arg0;
        return (u32)((1 - value) | (value - 1)) >> 31;
    }
    return 1;
}

// Waits (yielding) until RADCB_try_to_suspend_handler reports the request finished.
void RADCB_suspend_handler(void *arg0) {
    while (RADCB_try_to_suspend_handler(arg0) == 0) {
        OSYieldThread();
    }
}

// Releases one hold on the queue; the last release starts its periodic tick (`rate` per second).
void RADCB_resume_handler(void *arg0) {
    s32 level;
    if (arg0 != 0) {
        level = OSDisableInterrupts();
        // hold count, shared with the alarm handler
        *(volatile s32 *)((u8 *)arg0 + 4) -= 1;
        if (*(volatile s32 *)((u8 *)arg0 + 4) == 0) {
            if (*(s32 *)arg0 == 0) {
                OSSetPeriodicAlarm((OSAlarm *)((u8 *)arg0 + 8), OSGetTime(),
                                   (1000 / *(u32 *)((u8 *)arg0 + 0x3C)) * (*(u32 *)0x800000F8 / 4 / 1000),
                                   (OSAlarmHandler)RAD_callback_timer);
            }
            *(s32 *)arg0 = 1;
        }
        OSRestoreInterrupts(level);
    }
}

u32 RADCB_try_to_suspend_callback(void *arg0, void *arg1) {
    if (arg0 != 0 && arg1 != 0 &&
        *(s32 *)((u8 *)arg1 + 0x18) != 0 &&
        *(s32 *)((u8 *)arg1 + 0x14) != 0) {
        s32 level = OSDisableInterrupts();
        if (*(s32 *)((u8 *)arg1 + 0xC) == 1)
            *(s32 *)((u8 *)arg1 + 0xC) = 2;
        *(s32 *)((u8 *)arg1 + 0x10) += 1;
        OSRestoreInterrupts(level);
        return *(s32 *)((u8 *)arg1 + 0xC) != 1;
    }
    return 1;
}

// Waits (yielding) until RADCB_try_to_suspend_callback reports the request finished.
void RADCB_suspend_callback(void *arg0, void *arg1) {
    if (arg0 != 0 && arg1 != 0 &&
        *(s32 *)((u8 *)arg1 + 0x18) != 0 &&
        *(s32 *)((u8 *)arg1 + 0x14) != 0) {
        while (RADCB_try_to_suspend_callback(arg0, arg1) == 0) {
            OSYieldThread();
        }
    }
}

void RADCB_resume_callback(void *arg0, void *arg1) {
    if (arg0 != 0 && arg1 != 0 &&
        *(s32 *)((u8 *)arg1 + 0x18) != 0 &&
        *(s32 *)((u8 *)arg1 + 0x14) != 0) {
        s32 level = OSDisableInterrupts();
        // pending-request count, shared with interrupt-time code
        *(volatile s32 *)((u8 *)arg1 + 0x10) -= 1;
        if (*(volatile s32 *)((u8 *)arg1 + 0x10) == 0)
            *(s32 *)((u8 *)arg1 + 0xC) = 1;
        OSRestoreInterrupts(level);
    }
}

void RADCB_free_handler(s32 arg0) {
    if (arg0 != 0) {
        RADCB_suspend_handler((void *)arg0);
        OSCancelAlarm((OSAlarm *)(arg0 + 8));
        radfree((void *)arg0);
    }
}

void OpenReadBundle(s32 arg0, void *arg1, s32 arg2, u32 arg3, s32 arg4, s32 arg5, s32 arg6) {
    *(s32 *)((u8 *)arg1 + 0x28) = 0x20 - __cntlzw((u16)(arg2 + ((arg3 >> 3) * arg5) - 1));
    *(s32 *)((u8 *)arg1 + 0) = 0;
    *(s32 *)((u8 *)arg1 + 4) = 0;
    *(s32 *)((u8 *)arg1 + 8) = arg4;
    if (arg6)
        *(s32 *)((u8 *)arg1 + 0xC) = 1 << (arg4 - 1);
    else
        *(s32 *)((u8 *)arg1 + 0xC) = 0;
    *(s32 *)((u8 *)arg1 + 0x2C) = arg0;
}

s32 simpmergesort(u32 arg0, u8 *arg1, u8 *arg2, u8 *arg3, s32 arg4) {
    s32 var_r7 = arg4;
    s32 var_r8 = arg4;
    s32 var_r9 = 0;
    u8 *out = arg1;
    u8 *left = arg2;
    u8 *right = arg3;
    u8 temp;
    do {
        var_r9++;
        if (arg0 & 1) {
            *out++ = *right++;
            var_r8--;
        } else {
            *out++ = *left++;
            var_r7--;
        }
        arg0 >>= 1;
    } while (var_r7 != 0 && var_r8 != 0);
    if (var_r7 != 0) {
        do { temp = *left; var_r7--; left++; *out = temp; out++; } while (var_r7 != 0);
    } else if (var_r8 != 0) {
        do { temp = *right; var_r8--; right++; *out = temp; out++; } while (var_r8 != 0);
    }
    return var_r9;
}

// Peeks `width` bits into v without consuming them.
#define BINK_PEEK(v, width)                                               \
    if (avail < (width)) {                                                \
        v = (value | (*next << avail)) & (0xFFFFFFFFU >> (32 - (width))); \
    } else {                                                              \
        v = value & (0xFFFFFFFFU >> (32 - (width)));                      \
    }

// Consumes n bits.
#define BINK_SKIP(n)                         \
    if (avail < (n)) {                       \
        u32 word = *next;                    \
        next++;                              \
        value = word >> ((n) - avail);       \
        avail = avail + 32 - (n);            \
    } else {                                 \
        value >>= (n);                       \
        avail -= (n);                        \
    }

// One merge pass step: merges runs a and b of `size` symbols into dst, steered by up to `width` peeked bits.
#define BINK_MERGE(dst, a, b, size, width)            \
    {                                                 \
        u32 v;                                        \
        BINK_PEEK(v, width);                          \
        v = simpmergesort(v, dst, a, b, size);          \
        BINK_SKIP(v);                                 \
    }

extern u8 *lbl_80329DA8[16];  // Bink Huffman lookup tables
extern const u8 lbl_802F6990[];

// Reads a Huffman tree: picks one of the 16 fixed code tables and reads the symbol order, either explicitly or as
// a series of merge passes over the identity order.
void ReadHuffTable(BitReader8011 *bits, u8 **lookup, u32 *maxBits, u8 *syms) {
    u32 *next;
    u32 value;
    u32 avail;
    u32 table;
    u32 passes;
    u32 bit;
    u32 i;
    u8 tmp2[16];
    u8 tmp[16];

    avail = bits->count;
    next = bits->next;
    value = bits->value;
    BINK_GETBITS(table, 4);
    *lookup = lbl_80329DA8[table];
    *maxBits = lbl_802F6990[table];
    if (table == 0) {
        for (i = 0; i < 16; i++) {
            syms[i] = i;
        }
    } else {
        BINK_GETBIT(bit);
        if (!(bit & 1)) {
            BINK_GETBITS(passes, 2);
            if (passes == 0) {
                for (i = 0; i < 16; i += 2) {
                    BINK_GETBIT(bit);
                    if (bit & 1) {
                        syms[i] = i + 1;
                        syms[i + 1] = i;
                    } else {
                        syms[i] = i;
                        syms[i + 1] = i + 1;
                    }
                }
            } else {
                for (i = 0; i < 16; i += 2) {
                    BINK_GETBIT(bit);
                    if (bit & 1) {
                        tmp[i] = i + 1;
                        tmp[i + 1] = i;
                    } else {
                        tmp[i] = i;
                        tmp[i + 1] = i + 1;
                    }
                }
                if (passes == 1) {
                    BINK_MERGE(syms, &tmp[0], &tmp[2], 2, 3);
                    BINK_MERGE(syms + 4, &tmp[4], &tmp[6], 2, 3);
                    BINK_MERGE(syms + 8, &tmp[8], &tmp[10], 2, 3);
                    BINK_MERGE(syms + 12, &tmp[12], &tmp[14], 2, 3);
                } else {
                    BINK_MERGE(&tmp2[0], &tmp[0], &tmp[2], 2, 3);
                    BINK_MERGE(&tmp2[4], &tmp[4], &tmp[6], 2, 3);
                    BINK_MERGE(&tmp2[8], &tmp[8], &tmp[10], 2, 3);
                    BINK_MERGE(&tmp2[12], &tmp[12], &tmp[14], 2, 3);
                    if (passes == 2) {
                        BINK_MERGE(syms, &tmp2[0], &tmp2[4], 4, 7);
                        BINK_MERGE(syms + 8, &tmp2[8], &tmp2[12], 4, 7);
                    } else {
                        BINK_MERGE(&tmp[0], &tmp2[0], &tmp2[4], 4, 7);
                        BINK_MERGE(&tmp[8], &tmp2[8], &tmp2[12], 4, 7);
                        BINK_MERGE(syms, &tmp[0], &tmp[8], 8, 15);
                    }
                }
            }
        } else {
            u32 unused;
            u32 count;
            u8 *p;
            u32 sym;
            BINK_GETBITS(count, 3);
            unused = 0xFFFF;
            for (i = 0; i <= count; i++) {
                u32 s;
                BINK_GETBITS(s, 4);
                syms[i] = s;
                unused &= ~(1 << s);
            }
            p = syms + count;
            sym = 0;
            do {
                if (unused & 1) {
                    *++p = sym;
                }
                sym++;
            } while (unused >>= 1);
        }
    }
    bits->next = next;
    bits->value = value;
    bits->count = avail;
}

// Reads a bundle's Huffman tree.
void StartReadHuff4Bundle(Bundle8011 *b, BitReader8011 *bits) {
    ReadHuffTable(bits, &b->lookup, &b->maxBits, b->symbols);
}

// The 16 context trees used for colors (selected by the previous high nibble).
typedef struct BinkColorTrees8011 {
    u8 symbols[16][16];  // 0x000
    u32 maxBits[16];     // 0x100
    u8 *lookup[16];      // 0x140
    s32 last;            // 0x180 previous high nibble
} BinkColorTrees8011;

// Reads the 16 color context trees, then the bundle's own tree.
void StartReadHuff8Bundle(Bundle8011 *b, BitReader8011 *bits, BinkColorTrees8011 *trees) {
    u32 i;
    for (i = 0; i < 16; i++) {
        ReadHuffTable(bits, &trees->lookup[i], &trees->maxBits[i], trees->symbols[i]);
    }
    ReadHuffTable(bits, &b->lookup, &b->maxBits, b->symbols);
    trees->last = 0;
}

extern const u8 lbl_80561208[8];
typedef struct BitWin8011 {
    u32 width;
    u32 mask;
} BitWin8011;

void CheckReadRLEHuff4Bundle(void *arg0, void *arg1) {
    u32 *bits;
    u32 fill;
    u32 value, bitCount, count, code, len, s, w;
    u8 *dst, *lookup, *table;
    u32 sym, runlen, j;
    BitWin8011 win;
    if (*(u32 *)arg0 != *(u32 *)((u8 *)arg0 + 4)) return;
    bitCount = *(u32 *)((u8 *)arg1 + 8);
    win.width = *(u32 *)((u8 *)arg0 + 0x28);
    bits = *(u32 **)((u8 *)arg1 + 4);
    value = *(u32 *)arg1;
    if (bitCount < win.width) {
        u32 next = *bits++;
        count = (value | (next << bitCount)) & (0xFFFFFFFFU >> (32 - win.width));
        value = next >> (win.width - bitCount);
        bitCount = bitCount + 32 - win.width;
    } else {
        count = value & (0xFFFFFFFFU >> (32 - win.width));
        value >>= win.width;
        bitCount -= win.width;
    }
    if (count != 0) {
        dst = *(u8 **)((u8 *)arg0 + 0x2C);
        *(u8 **)arg0 = dst;
        *(u8 **)((u8 *)arg0 + 4) = dst + count;
        if (bitCount == 0) { w = *bits++; bitCount = 31; value = w >> 1; }
        else { w = value; value >>= 1; bitCount--; }
        if ((w & 1) == 0) {
            dst = *(u8 **)((u8 *)arg0 + 0x2C);
            fill = 0;
            table = (u8 *)arg0 + 0x10;
            lookup = *(u8 **)((u8 *)arg0 + 0x24);
            win.width = *(u32 *)((u8 *)arg0 + 0x20);
            win.mask = 0xFFFFFFFFU >> (32 - win.width);
            while (count != 0) {
                u8 rl;
                u32 next;
                u32 w;
                if (bitCount >= win.width) {
                    code = value & win.mask;
                    s = lookup[code];
                    code = s & 15;
                    len = s >> 4;
                    sym = table[code];
                    value >>= len;
                    bitCount -= len;
                } else {
                    next = *bits;
                    code = (value | (next << bitCount)) & win.mask;
                    len = lookup[code];
                    sym = table[len & 15];
                    len >>= 4;
                    if (bitCount >= len) { value >>= len; bitCount -= len; }
                    else { value = next >> (len - bitCount); bits++; bitCount = bitCount + 32 - len; }
                }
                if (sym >= 12) {
                    rl = lbl_80561208[sym - 12];
                    w = fill | (fill << 8);
                    count -= rl;
                    w |= w << 16;
                    /* Run fill as the target has it: a byte loop over the whole run when dst is unaligned (the word and
                     * tail loops after it never run), whole words when aligned (runs are multiples of 4 bytes). */
                    if ((u32)dst & 3) {
                        do {
                            *dst++ = w;
                        } while (--rl);
                        while (rl >= 4) {
                            *(u32 *)dst = w;
                            dst += 4;
                            rl -= 4;
                        }
                        for (; rl != 0; rl--) {
                            *dst++ = w;
                        }
                    } else {
                        do {
                            rl = rl - 4;
                            *(u32 *)dst = w;
                            dst += 4;
                        } while (rl);
                    }
                } else {
                    *dst++ = sym;
                    fill = sym;
                    count--;
                }
            }
        } else {
            if (bitCount < 4) {
                u32 next = *bits++;
                code = (value | (next << bitCount)) & 15;
                value = next >> (4 - bitCount);
                bitCount += 28;
            } else {
                code = value & 15; value >>= 4; bitCount -= 4;
            }
            memset(*(void **)((u8 *)arg0 + 0x2C), code, count);
        }
    } else {
        dst = *(u8 **)((u8 *)arg0 + 0x2C);
        *(u8 **)((u8 *)arg0 + 4) = dst; *(u8 **)arg0 = dst + 4;
    }
    *(u32 **)((u8 *)arg1 + 4) = bits;
    *(u32 *)arg1 = value; *(u32 *)((u8 *)arg1 + 8) = bitCount;
}

// Refills the colors bundle (newer format): each color is a high nibble from the context tree picked by the
// previous high nibble and a low nibble from the bundle's tree, stored sign-magnitude around 128. A set first bit
// means one color followed by a run of copies.
void CheckReadHuff8Bundle(Bundle8011 *b, BitReader8011 *bits, BinkColorTrees8011 *trees) {
    u32 *next;
    u32 value;
    u32 count;
    s32 len;
    u32 width;
    u32 e;
    u32 bit;
    u32 last;
    u32 lo;
    u32 word;
    u32 maxBits;
    u8 *dst;
    u8 *lookup;
    u8 *symbols;
    u32 ones;

    if (b->cur != b->end) return;
    count = bits->count;
    width = b->countBits;
    next = bits->next;
    value = bits->value;
    if (count < width) {
        u32 word = *next;
        len = (value | (word << count)) & (0xFFFFFFFFU >> (32 - width));
        value = word >> (width - count);
        count = count + 32 - width;
        next++;
    } else {
        len = value & (0xFFFFFFFFU >> (32 - width));
        value >>= width;
        count -= width;
    }
    if (len != 0) {
        dst = b->data;
        maxBits = b->maxBits;
        symbols = b->symbols;
        b->cur = dst;
        lookup = b->lookup;
        b->end = dst + len;
        last = trees->last;
        if (count == 0) {
            bit = *next;
            value = bit >> 1;
            count = 31;
            next++;
        } else {
            bit = value;
            value >>= 1;
            count--;
        }
        if (bit & 1) {
            len = -20 - len;
        }
        ones = 0xFFFFFFFFU;
        do {
            u32 w;
            u32 c;
            u32 v;
            len--;
            w = trees->maxBits[last];
            if (count >= w) {
                e = trees->lookup[last][value & (ones >> (32 - w))];
                last = trees->symbols[last][e & 15];
                e >>= 4;
                value >>= e;
                count -= e;
            } else {
                u32 word = *next;
                e = trees->lookup[last][(value | (word << count)) & (ones >> (32 - w))];
                last = trees->symbols[last][e & 15];
                e >>= 4;
                if (count >= e) {
                    value >>= e;
                    count -= e;
                } else {
                    value = word >> (e - count);
                    next++;
                    count = count + 32 - e;
                }
            }
            if (count >= maxBits) {
                e = lookup[value & (ones >> (32 - maxBits))];
                lo = symbols[e & 15];
                e >>= 4;
                value >>= e;
                count -= e;
            } else {
                word = *next;
                e = lookup[(ones >> (32 - maxBits)) & (value | (word << count))];
                lo = symbols[e & 15];
                e >>= 4;
                if (count >= e) {
                    value >>= e;
                    count -= e;
                } else {
                    value = word >> (e - count);
                    next++;
                    count = count + 32 - e;
                }
            }
            c = lo | (last << 4);
            v = c & 0x7F;
            if (c & 0x80) {
                v = (u8)-v;
            }
            *dst++ = v + 128;
        } while (len > 0);
        if (len < -22) {
            memset(b->data, b->data[0], -(len + 21));
        }
        trees->last = last;
    } else {
        b->end = b->data;
        b->cur = b->data + 4;
    }
    bits->next = next;
    bits->value = value;
    bits->count = count;
}

// Refills the colors bundle (older format): like CheckReadHuff8Bundle, with colors stored as plain bytes.
void NewCheckReadHuff8Bundle(Bundle8011 *b, BitReader8011 *bits, BinkColorTrees8011 *trees) {
    u32 *next;
    u32 value;
    u32 count;
    s32 len;
    u32 width;
    u32 e;
    u32 bit;
    u32 last;
    u32 lo;
    u32 word;
    u32 maxBits;
    u8 *dst;
    u8 *lookup;
    u8 *symbols;
    u32 ones;

    if (b->cur != b->end) return;
    count = bits->count;
    width = b->countBits;
    next = bits->next;
    value = bits->value;
    if (count < width) {
        u32 word = *next;
        len = (value | (word << count)) & (0xFFFFFFFFU >> (32 - width));
        value = word >> (width - count);
        count = count + 32 - width;
        next++;
    } else {
        len = value & (0xFFFFFFFFU >> (32 - width));
        value >>= width;
        count -= width;
    }
    if (len != 0) {
        dst = b->data;
        maxBits = b->maxBits;
        symbols = b->symbols;
        b->cur = dst;
        lookup = b->lookup;
        b->end = dst + len;
        last = trees->last;
        if (count == 0) {
            bit = *next;
            value = bit >> 1;
            count = 31;
            next++;
        } else {
            bit = value;
            value >>= 1;
            count--;
        }
        if (bit & 1) {
            len = -20 - len;
        }
        ones = 0xFFFFFFFFU;
        do {
            u32 w;
            len--;
            w = trees->maxBits[last];
            if (count >= w) {
                e = trees->lookup[last][value & (ones >> (32 - w))];
                last = trees->symbols[last][e & 15];
                e >>= 4;
                value >>= e;
                count -= e;
            } else {
                u32 word = *next;
                e = trees->lookup[last][(value | (word << count)) & (ones >> (32 - w))];
                last = trees->symbols[last][e & 15];
                e >>= 4;
                if (count >= e) {
                    value >>= e;
                    count -= e;
                } else {
                    value = word >> (e - count);
                    next++;
                    count = count + 32 - e;
                }
            }
            if (count >= maxBits) {
                e = lookup[value & (ones >> (32 - maxBits))];
                lo = symbols[e & 15];
                e >>= 4;
                value >>= e;
                count -= e;
            } else {
                word = *next;
                e = lookup[(ones >> (32 - maxBits)) & (value | (word << count))];
                lo = symbols[e & 15];
                e >>= 4;
                if (count >= e) {
                    value >>= e;
                    count -= e;
                } else {
                    value = word >> (e - count);
                    next++;
                    count = count + 32 - e;
                }
            }
            *dst++ = lo | (last << 4);
        } while (len > 0);
        if (len < -22) {
            memset(b->data, b->data[0], -(len + 21));
        }
        trees->last = last;
    } else {
        b->end = b->data;
        b->cur = b->data + 4;
    }
    bits->next = next;
    bits->value = value;
    bits->count = count;
}


// Refills an exhausted bundle of unsigned bytes: reads the count, then either that many Huffman-coded values or one
// 4-bit value repeated.
/* Helper name unproven */

void CheckReadHuff4Bundle(Bundle8011 *b, BitReader8011 *bits) {
    u32 *next;
    u32 value;
    u32 count;
    u32 len, bit, fill, e;
    u8 *dst, *lookup, *symbols;
    BitWin8011 w;

    if (b->cur != b->end) return;
    count = bits->count;
    w.width = b->countBits;
    next = bits->next;
    value = bits->value;
    if (count < w.width) {
        u32 word = *next;
        len = (value | (word << count)) & (0xFFFFFFFFU >> (32 - w.width));
        value = word >> (w.width - count);
        count = count + 32 - w.width;
        next++;
    } else {
        len = value & (0xFFFFFFFFU >> (32 - w.width));
        value >>= w.width;
        count -= w.width;
    }
    if (len != 0) {
        b->cur = b->data;
        b->end = b->data + len;
        if (count == 0) {
            bit = *next;
            value = bit >> 1;
            count = 31;
            next++;
        } else {
            bit = value;
            value >>= 1;
            count--;
        }
        if (!(bit & 1)) {
            w.width = b->maxBits;
            w.mask = 0xFFFFFFFFU;
            w.mask >>= 32 - w.width;
            dst = b->data;
            lookup = b->lookup;
            symbols = b->symbols;
            do {
                len--;
                if (count >= w.width) {
                    e = lookup[value & w.mask];
                    *dst++ = symbols[e & 15];
                    e >>= 4;
                    value >>= e;
                    count -= e;
                } else {
                    u32 word = *next;
                    e = lookup[(value | (word << count)) & w.mask];
                    *dst++ = symbols[e & 15];
                    e >>= 4;
                    if (count >= e) {
                        value >>= e;
                        count -= e;
                    } else {
                        value = word >> (e - count);
                        next++;
                        count = count + 32 - e;
                    }
                }
            } while (len != 0);
        } else {
            if (count < 4) {
                u32 word = *next;
                next++;
                fill = (value | (word << count)) & 15;
                value = word >> (4 - count);
                count += 28;
            } else {
                fill = value & 15;
                value >>= 4;
                count -= 4;
            }
            memset(b->data, fill, len);
        }
    } else {
        b->end = b->data;
        b->cur = b->data + 4;
    }
    bits->next = next;
    bits->value = value;
    bits->count = count;
}

// Refills an exhausted bundle of nibble pairs: reads the count, then that many bytes, each two Huffman-coded
// nibbles (low first).
void CheckReadHuff4PairBundle(Bundle8011 *b, BitReader8011 *bits) {
    u32 len;
    u32 *next;
    u32 value;
    u32 count;
    u32 mask, lo, hi, e;
    u8 *dst, *lookup, *symbols;
    u32 width;

    if (b->cur != b->end) return;
    count = bits->count;
    width = b->countBits;
    next = bits->next;
    value = bits->value;
    if (count < width) {
        u32 word = *next;
        len = (value | (word << count)) & (0xFFFFFFFFU >> (32 - width));
        value = word >> (width - count);
        count = count + 32 - width;
        next++;
    } else {
        len = value & (0xFFFFFFFFU >> (32 - width));
        value >>= width;
        count -= width;
    }
    if (len != 0) {
        mask = b->maxBits;
        dst = b->data;
        b->cur = dst;
        b->end = dst + len;
        width = mask;
        mask = 0xFFFFFFFFU >> (32 - mask);
        lookup = b->lookup;
        symbols = b->symbols;
        do {
            len--;
            if (count >= width) {
                e = lookup[value & mask];
                lo = symbols[e & 15];
                e >>= 4;
                value >>= e;
                count -= e;
            } else {
                u32 word = *next;
                e = lookup[(value | (word << count)) & mask];
                lo = symbols[e & 15];
                e >>= 4;
                if (count >= e) {
                    value >>= e;
                    count -= e;
                } else {
                    value = word >> (e - count);
                    next++;
                    count = count + 32 - e;
                }
            }
            if (count >= width) {
                e = lookup[value & mask];
                hi = symbols[e & 15];
                e >>= 4;
                value >>= e;
                count -= e;
            } else {
                u32 word = *next;
                e = lookup[(value | (word << count)) & mask];
                hi = symbols[e & 15];
                e >>= 4;
                if (count >= e) {
                    value >>= e;
                    count -= e;
                } else {
                    value = word >> (e - count);
                    next++;
                    count = count + 32 - e;
                }
            }
            *dst++ = lo | (hi << 4);
        } while (len != 0);
    } else {
        b->end = b->data;
        b->cur = b->data + 4;
    }
    bits->next = next;
    bits->value = value;
    bits->count = count;
}

void CheckReadHuff4SBundle(Bundle8011 *b, BitReader8011 *bits) {
    u32 *next;
    u32 value;
    u32 count;
    u32 len;
    u32 width;
    u32 e;
    u32 bit;
    u32 maxBits;
    u8 *dst;
    u8 *lookup;
    u8 *symbols;
    u32 sym;
    u32 word;
    u32 ones;

    if (b->cur != b->end) return;
    count = bits->count;
    width = b->countBits;
    next = bits->next;
    value = bits->value;
    if (count < width) {
        u32 word = *next;
        len = (value | (word << count)) & (0xFFFFFFFFU >> (32 - width));
        value = word >> (width - count);
        count = count + 32 - width;
        next++;
    } else {
        len = value & (0xFFFFFFFFU >> (32 - width));
        value >>= width;
        count -= width;
    }
    if (len != 0) {
        b->cur = b->data;
        b->end = b->data + len;
        if (count == 0) {
            bit = *next;
            value = bit >> 1;
            count = 31;
            next++;
        } else {
            bit = value;
            value >>= 1;
            count--;
        }
        if (!(bit & 1)) {
            maxBits = b->maxBits;
            ones = 0xFFFFFFFFU;
            dst = b->data;
            symbols = b->symbols;
            lookup = b->lookup;
            do {
                len--;
                if (count >= maxBits) {
                    e = lookup[value & (ones >> (32 - maxBits))];
                    sym = symbols[e & 15];
                    e >>= 4;
                    value >>= e;
                    count -= e;
                } else {
                    word = *next;
                    e = lookup[(ones >> (32 - maxBits)) & (value | (word << count))];
                    sym = symbols[e & 15];
                    e >>= 4;
                    if (count >= e) {
                        value >>= e;
                        count -= e;
                    } else {
                        value = word >> (e - count);
                        next++;
                        count = count + 32 - e;
                    }
                }
                if (sym != 0) {
                    if (count == 0) {
                        bit = *next;
                        value = bit >> 1;
                        count = 31;
                        next++;
                    } else {
                        bit = value;
                        value >>= 1;
                        count--;
                    }
                    if (bit & 1) {
                        sym = -sym;
                    }
                }
                *dst++ = sym;
            } while (len != 0);
        } else {
            if (count < 4) {
                u32 word = *next++;
                sym = (value | (word << count)) & 15;
                value = word >> (4 - count);
                count += 28;
            } else {
                sym = value & 15;
                value >>= 4;
                count -= 4;
            }
            if (sym != 0) {
                if (count == 0) {
                    bit = *next;
                    value = bit >> 1;
                    count = 31;
                    next++;
                } else {
                    bit = value;
                    value >>= 1;
                    count--;
                }
                if (bit & 1) {
                    sym = -sym;
                }
            }
            memset(b->data, sym, len);
        }
    } else {
        dst = b->data;
        b->end = dst;
        b->cur = dst + 4;
    }
    bits->next = next;
    bits->value = value;
    bits->count = count;
}

void CheckReadDelta16Bundle(void *arg0, void *arg1) {
    s32 n;
    u32 count;
    u32 *bits;
    u32 value;
    u32 bitCount;
    s16 *dst;
    s32 val;
    u32 width, mask, code, order, tmp, tmp2;
    s32 run;

    if (*(u32 *)arg0 != *(u32 *)((u8 *)arg0 + 4)) return;
    bitCount = *(u32 *)((u8 *)arg1 + 8);
    width = *(u32 *)((u8 *)arg0 + 0x28);
    bits = *(u32 **)((u8 *)arg1 + 4);
    value = *(u32 *)arg1;
    if (bitCount < width) {
        u32 next = *bits++;
        count = (value | (next << bitCount)) & (0xFFFFFFFFU >> (32 - width));
        value = next >> (width - bitCount);
        bitCount = bitCount + 32 - width;
    } else {
        count = value & (0xFFFFFFFFU >> (32 - width));
        value >>= width;
        bitCount -= width;
    }
    if (count != 0) {
        dst = *(s16 **)((u8 *)arg0 + 0x2C);
        if (*(s32 *)((u8 *)arg0 + 0xC) != 0) {
            width = *(u32 *)((u8 *)arg0 + 8) - 1;
            if (bitCount < width) {
                u32 next = *bits++;
                val = (value | (next << bitCount)) & (0xFFFFFFFFU >> (32 - width));
                value = next >> (width - bitCount);
                bitCount = bitCount + 32 - width;
            } else {
                val = value & (0xFFFFFFFFU >> (32 - width));
                value >>= width;
                bitCount -= width;
            }
            if (val != 0) {
                if (bitCount == 0) { tmp = *bits++; bitCount = 31; value = tmp >> 1; }
                else { tmp = value; value >>= 1; bitCount--; }
                if (tmp & 1) val = -val;
            }
        } else {
            width = *(u32 *)((u8 *)arg0 + 8);
            if (bitCount < width) {
                u32 next = *bits++;
                val = (value | (next << bitCount)) & (0xFFFFFFFFU >> (32 - width));
                value = next >> (width - bitCount);
                bitCount = bitCount + 32 - width;
            } else {
                val = value & (0xFFFFFFFFU >> (32 - width));
                value >>= width;
                bitCount -= width;
            }
        }
        *dst = val;
        *(s16 **)arg0 = *(s16 **)((u8 *)arg0 + 0x2C);
        *(s16 **)((u8 *)arg0 + 4) = *(s16 **)((u8 *)arg0 + 0x2C) + count;
        count--;
        dst++;
        while (count != 0) {
            n = 8;
            if (count <= 8) n = (s32)count;
            if (bitCount < 4) {
                u32 next = *bits++;
                order = (value | (next << bitCount)) & 15;
                value = next >> (4 - bitCount);
                bitCount = bitCount + 32 - 4;
            } else {
                order = value & 15;
                value >>= 4;
                bitCount -= 4;
            }
            if (order != 0) {
                mask = 0xFFFFFFFFU >> (32 - order);
                run = n;
                while (run != 0) {
                    run--;
                    if (bitCount < order) {
                        u32 next = *bits++;
                        code = (value | (next << bitCount)) & mask;
                        value = next >> (order - bitCount);
                        bitCount = bitCount + 32 - order;
                    } else {
                        code = value & mask;
                        value >>= order;
                        bitCount -= order;
                    }
                    if (code != 0) {
                        if (bitCount == 0) { u32 w = *bits++; bitCount = 31; value = w >> 1; tmp2 = w; }
                        else { tmp2 = value; value >>= 1; bitCount--; }
                        if (tmp2 & 1) code = -code;
                    }
                    val += code;
                    *dst = val;
                    dst++;
                }
            } else {
                radmemset16((u32 *)dst, (u16)val, (n + n));
                dst = (s16 *)((u8 *)dst + (n << 1));
            }
            count -= n;
        }
    } else {
        dst = *(s16 **)((u8 *)arg0 + 0x2C);
        *(s16 **)((u8 *)arg0 + 4) = dst;
        *(s16 **)((u8 *)arg0) = dst + 2;
    }
    *(u32 **)((u8 *)arg1 + 4) = bits;
    *(u32 *)arg1 = value;
    *(u32 *)((u8 *)arg1 + 8) = bitCount;
}

s32 getbunsize(s32 arg0, u32 arg1, u32 arg2, u32 arg3) {
    u32 var_r5 = arg2;
    u32 temp;
    if (var_r5 <= 8U) var_r5 = 8;
    else if (var_r5 < 0x10U) var_r5 = 0x10;
    temp = var_r5 * arg3;
    return (((u32)(arg0 * var_r5) >> 3) + ((temp * (arg1 >> 3)) >> 3) + 3) & 0xFFFFFFFC;
}

void ExpandBundleSizes(void* arg0, u32 arg1) {
    s32* out = (s32*)arg0;
    out[0] = getbunsize(0x200, arg1, 4, 1);
    out[1] = getbunsize(0x200, arg1 >> 1, 4, 1);
    out[2] = getbunsize(0x200, arg1, 8, 0x40);
    out[3] = getbunsize(0x200, arg1, 8, 8);
    out[4] = getbunsize(0x200, arg1, 5, 1);
    out[5] = getbunsize(0x200, arg1, 5, 1);
    out[6] = getbunsize(0x200, arg1, 0xB, 1);
    out[7] = getbunsize(0x200, arg1, 0xB, 1);
    out[8] = getbunsize(0x200, arg1, 4, 0x30);
}

extern const u8 lbl_802F65C0[];
extern const u8 lbl_802F8A40[16][64];
// Retail addresses these masks from lbl_802F65C0 (same bytes as lbl_802F6990 + 0x10..), so the original
// tables were one object starting at lbl_802F65C0; the split labels only work in retail .rodata order.
#define BINK_PAT_C1 ((u32 *)((u8 *)lbl_802F65C0 + 0x3E0))         // 4-pixel masks, 1 bit per pixel
#define BINK_PAT_C0 ((u32 *)((u8 *)lbl_802F65C0 + 0x420))
#define BINK_PAT2_C1 ((u32 *)((u8 *)lbl_802F65C0 + 0x460))        // doubled 2-pixel masks
#define BINK_PAT2_C0 ((u32 *)((u8 *)lbl_802F65C0 + 0x470))

void ReadBPLossless(s16 *block, BitReader8011 *bits);
void ReadBPLossyWithMotion(u8 *dst, s32 dstStride, BitReader8011 *bits, u32 limit, u8 *src, s32 srcStride);
void fn_80125CA0(void *dst, void *stride, void *block, s32 quant);
void fn_80125CB4(void *dst, void *stride, void *block, s32 quant);
void FastmIDCT8x8WithMotion(u8 *dst, s32 stride, s16 *block, s32 q, u8 *src, s32 srcStride);

// Reads `width` bits from reader *br into v.
#define BINK_BR_GETBITS(br, v, width)                                               \
    if ((br)->count < (width)) {                                                    \
        u32 word = *(br)->next;                                                     \
        v = ((br)->value | (word << (br)->count)) & (0xFFFFFFFFU >> (32 - (width))); \
        (br)->value = word >> ((width) - (br)->count);                              \
        (br)->count = (br)->count + 32 - (width);                                   \
        (br)->next++;                                                               \
    } else {                                                                        \
        v = (br)->value & (0xFFFFFFFFU >> (32 - (width)));                          \
        (br)->value >>= (width);                                                    \
        (br)->count -= (width);                                                     \
    }

// Interleaved 8-row walk: rows alternate between the d0/s0 and d1/s1 pointer pairs.
#define BINK_ROWS8_2(ROW, d0, d1, dp, s0, s1, sp) \
    ROW(d0, s0);                                  \
    d0 = d1 + (dp);                               \
    s0 = s1 + (sp);                               \
    ROW(d1, s1);                                  \
    d1 = d0 + (dp);                               \
    s1 = s0 + (sp);                               \
    ROW(d0, s0);                                  \
    d0 = d1 + (dp);                               \
    s0 = s1 + (sp);                               \
    ROW(d1, s1);                                  \
    d1 = d0 + (dp);                               \
    s1 = s0 + (sp);                               \
    ROW(d0, s0);                                  \
    d0 = d1 + (dp);                               \
    s0 = s1 + (sp);                               \
    ROW(d1, s1);                                  \
    d1 = d0 + (dp);                               \
    s1 = s0 + (sp);                               \
    ROW(d0, s0);                                  \
    ROW(d1, s1)

// Same walk for a destination only; ROW gets the row number.
#define BINK_ROWS8_1(ROW, d0, d1, dp) \
    ROW(d0, 0);                       \
    d0 = d1 + (dp);                   \
    ROW(d1, 1);                       \
    d1 = d0 + (dp);                   \
    ROW(d0, 2);                       \
    d0 = d1 + (dp);                   \
    ROW(d1, 3);                       \
    d1 = d0 + (dp);                   \
    ROW(d0, 4);                       \
    d0 = d1 + (dp);                   \
    ROW(d1, 5);                       \
    d1 = d0 + (dp);                   \
    ROW(d0, 6);                       \
    ROW(d1, 7)

#define BINK_ROW_F64(d, s) *(f64 *)(d) = *(f64 *)(s)
#define BINK_ROW_U32(d, s)              \
    ((u32 *)(d))[0] = ((u32 *)(s))[0]; \
    ((u32 *)(d))[1] = ((u32 *)(s))[1]
#define BINK_ROW_U8(d, s) \
    (d)[0] = (s)[0];      \
    (d)[1] = (s)[1];      \
    (d)[2] = (s)[2];      \
    (d)[3] = (s)[3];      \
    (d)[4] = (s)[4];      \
    (d)[5] = (s)[5];      \
    (d)[6] = (s)[6];      \
    (d)[7] = (s)[7]

#define BINK_TMP_F64(d, k) *(f64 *)(d) = ((f64 *)tmp)[k]
#define BINK_RAW_F64(d, k) *(f64 *)(d) = ((f64 *)colors.cur)[k]
#define BINK_RAW_U32(d, k)                                  \
    ((u32 *)(d))[0] = ((u32 *)colors.cur)[(k) * 2];         \
    ((u32 *)(d))[1] = ((u32 *)colors.cur)[(k) * 2 + 1]
#define BINK_RAW_U8(d, k)                \
    (d)[0] = colors.cur[(k) * 8 + 0];    \
    (d)[1] = colors.cur[(k) * 8 + 1];    \
    (d)[2] = colors.cur[(k) * 8 + 2];    \
    (d)[3] = colors.cur[(k) * 8 + 3];    \
    (d)[4] = colors.cur[(k) * 8 + 4];    \
    (d)[5] = colors.cur[(k) * 8 + 5];    \
    (d)[6] = colors.cur[(k) * 8 + 6];    \
    (d)[7] = colors.cur[(k) * 8 + 7]
#define BINK_FILL8(d, k)     \
    ((u32 *)(d))[0] = c;     \
    ((u32 *)(d))[1] = c
#define BINK_FILL16(d)       \
    ((u32 *)(d))[0] = c;     \
    ((u32 *)(d))[1] = c;     \
    ((u32 *)(d))[2] = c;     \
    ((u32 *)(d))[3] = c
#define BINK_PAT8(d, k)                                                         \
    p = *pattern.cur++;                                                         \
    ((u32 *)(d))[0] = (c0 & masks0[p & 15]) | (c1 & masks1[p & 15]);  \
    ((u32 *)(d))[1] = (c0 & masks0[p >> 4]) | (c1 & masks1[p >> 4])
// Doubles the two pixels of a u16 into four.
#define BINK_DOUBLE2(w) (((w) << 8) | ((w) & 0xFF)) | (((w) << 16) & 0xFF000000)

#pragma push
#pragma global_optimizer off  /* Pragma unproven, strong evidence: only this keeps both arms of the readColors if/else as retail has them (each optimizer switch alone hoists one arm); it replaces the earlier lifetimes/propagation/common_subs/unroll/loop_invariants set (Bink session) */
// Decodes one Bink plane (8x8 blocks, or 16x16 scaled blocks) into dst, using prev as the previous frame.
// `step` is the block-map step per block (1 for luma, which shares map entries between block pairs, 2 for
// chroma). Stores the number of skipped blocks whose map entry was clear in *skipped and returns the end of the
// plane's data. Structure cross-checked against n100f's ExpandPlane (reference only).
u32 *ExpandPlane(u8 *dst, u8 *prev, u32 width, u32 height, s32 pitch, u32 *data, s32 keyframe, u8 *blockMap,
                 s32 blocksWide, u32 step, void **bufs, u32 flags, u32 *skipped) {
    BinkColorTrees8011 trees;
    s16 block[64];
    Bundle8011 types;
    Bundle8011 subTypes;
    Bundle8011 colors;
    Bundle8011 pattern;
    Bundle8011 xOff;
    Bundle8011 yOff;
    Bundle8011 intraDC;
    Bundle8011 interDC;
    Bundle8011 runs;
    u8 tmp[64];
    BitReader8011 bits;
    u32 *masks1;
    u32 *masks0;
    u32 *scaledMasks1;
    u32 *scaledMasks0;
    s32 x;
    u32 y;
    s32 rowSkip;
    void (*readColors)(void *b, void *bits, void *trees);
    u32 skipCount;
    u32 idx;
    s32 i;
    u8 *tables;

    tables = (u8 *)lbl_802F65C0;
    rowSkip = pitch * 8 - width;
    skipCount = 0;
    idx = 0;
    bits.start = data;
    bits.next = data;
    bits.count = 0;
    bits.value = 0;
    if (flags & 0x8000) {
        readColors = (void (*)(void *, void *, void *))CheckReadHuff8Bundle;
    } else {
        readColors = (void (*)(void *, void *, void *))NewCheckReadHuff8Bundle;
    }
    OpenReadBundle((s32)bufs[0], &types, 0x200, width, 4, 1, 0);
    OpenReadBundle((s32)bufs[1], &subTypes, 0x200, width >> 1, 4, 1, 0);
    OpenReadBundle((s32)bufs[2], &colors, 0x200, width, 8, 64, 0);
    OpenReadBundle((s32)bufs[3], &pattern, 0x200, width, 8, 8, 0);
    OpenReadBundle((s32)bufs[4], &xOff, 0x200, width, 5, 1, 1);
    OpenReadBundle((s32)bufs[5], &yOff, 0x200, width, 5, 1, 1);
    OpenReadBundle((s32)bufs[6], &intraDC, 0x200, width, 11, 1, 0);
    OpenReadBundle((s32)bufs[7], &interDC, 0x200, width, 11, 1, 1);
    OpenReadBundle((s32)bufs[8], &runs, 0x200, width, 4, 48, 0);
    StartReadHuff4Bundle(&types, &bits);
    StartReadHuff4Bundle(&subTypes, &bits);
    StartReadHuff8Bundle(&colors, &bits, &trees);
    StartReadHuff4Bundle(&pattern, &bits);
    StartReadHuff4Bundle(&xOff, &bits);
    StartReadHuff4Bundle(&yOff, &bits);
    StartReadHuff4Bundle(&runs, &bits);
    scaledMasks0 = (u32 *)(tables + 0x470);
    scaledMasks1 = (u32 *)(tables + 0x460);
    masks0 = (u32 *)(tables + 0x420);
    masks1 = (u32 *)(tables + 0x3E0);
    for (y = 0; y < height; y += 8) {
        CheckReadRLEHuff4Bundle(&types, &bits);
        CheckReadRLEHuff4Bundle(&subTypes, &bits);
        readColors(&colors, &bits, &trees);
        CheckReadHuff4PairBundle(&pattern, &bits);
        CheckReadHuff4SBundle(&xOff, &bits);
        CheckReadHuff4SBundle(&yOff, &bits);
        CheckReadDelta16Bundle(&intraDC, &bits);
        CheckReadDelta16Bundle(&interDC, &bits);
        CheckReadHuff4Bundle(&runs, &bits);
        for (x = width; x != 0; x -= 8) {
            switch (*types.cur++) {
            case 0: {  // skip: keep the previous frame's block
                u8 *s0;
                u8 *d0;
                u8 *s1;
                u8 *d1;
                s0 = prev;
                d0 = dst;
                d1 = dst + pitch;
                s1 = prev + pitch;
                BINK_ROWS8_2(BINK_ROW_F64, d0, d1, pitch, s0, s1, pitch);
                if (blockMap[idx >> 1] == 0) {
                    skipCount++;
                }
                break;
            }
            case 1:  // scaled: one 8x8 block drawn as 16x16
                if (!((y >> 3) & 1)) {
                    switch (*subTypes.cur++) {
                    case 3: {
                        u32 value = bits.value;
                        u32 *next = bits.next;
                        u32 avail = bits.count;
                        const u8 *scan;
                        u32 v;
                        u32 bit;
                        u8 *d0;
                        u8 *d1;
                        u16 *s;
                        BINK_GETBITS(v, 4);
                        scan = lbl_802F8A40[v];
                        i = 0;
                        do {
                            u32 run;
                            BINK_GETBIT(bit);
                            if (bit & 1) {
                                u8 c = *colors.cur++;
                                for (run = *runs.cur++ + 1; run != 0; run--) {
                                    tmp[scan[i++]] = c;
                                }
                            } else {
                                for (run = *runs.cur++ + 1; run != 0; run--) {
                                    tmp[scan[i++]] = *colors.cur++;
                                }
                            }
                        } while (i < 63);
                        if (i == 63) {
                            tmp[scan[63]] = *colors.cur++;
                        }
                        d0 = dst;
                        d1 = dst + pitch;
                        s = (u16 *)tmp;
                        for (i = 0; i < 8; i++) {
                            u32 w;
                            w = s[0];
                            w = BINK_DOUBLE2(w);
                            ((u32 *)d0)[0] = w;
                            ((u32 *)d1)[0] = w;
                            w = s[1];
                            w = BINK_DOUBLE2(w);
                            ((u32 *)d0)[1] = w;
                            ((u32 *)d1)[1] = w;
                            w = s[2];
                            w = BINK_DOUBLE2(w);
                            ((u32 *)d0)[2] = w;
                            ((u32 *)d1)[2] = w;
                            w = s[3];
                            w = BINK_DOUBLE2(w);
                            ((u32 *)d0)[3] = w;
                            ((u32 *)d1)[3] = w;
                            s += 4;
                            d0 = d1 + pitch;
                            d1 = d0 + pitch;
                        }
                        bits.next = next;
                        bits.value = value;
                        bits.count = avail;
                        break;
                    }
                    case 5: {
                        s32 quant;
                        block[0] = *(s16 *)intraDC.cur;
                        intraDC.cur += 2;
                        ReadBPLossless(block, &bits);
                        BINK_BR_GETBITS(&bits, quant, 4);
                        fn_80125CB4(dst, (void *)pitch, block, quant);
                        break;
                    }
                    case 6: {
                        u32 c;
                        u8 *d0;
                        u8 *d1;
                        c = *colors.cur++;
                        c |= c << 8;
                        c |= c << 16;
                        d0 = dst;
                        d1 = dst + pitch;
                        BINK_FILL16(d0);
                        for (i = 0; i < 7; i++) {
                            BINK_FILL16(d1);
                            d0 = d1 + pitch;
                            BINK_FILL16(d0);
                            d1 = d0 + pitch;
                        }
                        BINK_FILL16(d1);
                        break;
                    }
                    case 8: {
                        u32 c0;
                        u32 c1;
                        u8 *d0;
                        u8 *d1;
                        c0 = *colors.cur++;
                        c0 |= c0 << 8;
                        c0 |= c0 << 16;
                        c1 = *colors.cur++;
                        c1 |= c1 << 8;
                        c1 |= c1 << 16;
                        d0 = dst;
                        d1 = dst + pitch;
                        for (i = 0; i < 8; i++) {
                            u32 p = *pattern.cur++;
                            u32 w;
                            w = (c0 & scaledMasks0[p & 3]) | (c1 & scaledMasks1[p & 3]);
                            ((u32 *)d0)[0] = w;
                            ((u32 *)d1)[0] = w;
                            w = (c0 & scaledMasks0[(p >> 2) & 3]) | (c1 & scaledMasks1[(p >> 2) & 3]);
                            ((u32 *)d0)[1] = w;
                            ((u32 *)d1)[1] = w;
                            w = (c0 & scaledMasks0[(p >> 4) & 3]) | (c1 & scaledMasks1[(p >> 4) & 3]);
                            ((u32 *)d0)[2] = w;
                            ((u32 *)d1)[2] = w;
                            w = (c0 & scaledMasks0[p >> 6]) | (c1 & scaledMasks1[p >> 6]);
                            ((u32 *)d0)[3] = w;
                            ((u32 *)d1)[3] = w;
                            d0 = d1 + pitch;
                            d1 = d0 + pitch;
                        }
                        break;
                    }
                    case 9: {
                        u8 *d0 = dst;
                        u8 *d1 = dst + pitch;
                        s32 pitch2 = pitch + pitch;
                        if ((u32)colors.cur & 3) {
                            for (i = 0; i < 8; i++) {
                                u32 w;
                                w = colors.cur[1];
                                w = (w | (w << 8)) | ((colors.cur[0] << 24) | (colors.cur[0] << 16));
                                ((u32 *)(d0))[0] = w;
                                ((u32 *)(d1))[0] = w;
                                w = colors.cur[3];
                                w = (w | (w << 8)) | ((colors.cur[2] << 24) | (colors.cur[2] << 16));
                                ((u32 *)(d0))[1] = w;
                                ((u32 *)(d1))[1] = w;
                                w = colors.cur[5];
                                w = (w | (w << 8)) | ((colors.cur[4] << 24) | (colors.cur[4] << 16));
                                ((u32 *)(d0))[2] = w;
                                ((u32 *)(d1))[2] = w;
                                w = colors.cur[7];
                                w = (w | (w << 8)) | ((colors.cur[6] << 24) | (colors.cur[6] << 16));
                                ((u32 *)(d0))[3] = w;
                                ((u32 *)(d1))[3] = w;
                                colors.cur += 8;
                                d0 += pitch2;
                                d1 += pitch2;
                            }
                        } else {
                            for (i = 0; i < 8; i++) {
                                u32 w;
                                w = ((u16 *)colors.cur)[0];
                                w = BINK_DOUBLE2(w);
                                ((u32 *)(d0))[0] = w;
                                ((u32 *)(d1))[0] = w;
                                w = ((u16 *)colors.cur)[1];
                                w = BINK_DOUBLE2(w);
                                ((u32 *)(d0))[1] = w;
                                ((u32 *)(d1))[1] = w;
                                w = ((u16 *)colors.cur)[2];
                                w = BINK_DOUBLE2(w);
                                ((u32 *)(d0))[2] = w;
                                ((u32 *)(d1))[2] = w;
                                w = ((u16 *)colors.cur)[3];
                                w = BINK_DOUBLE2(w);
                                ((u32 *)(d0))[3] = w;
                                ((u32 *)(d1))[3] = w;
                                colors.cur += 8;
                                d0 += pitch2;
                                d1 += pitch2;
                            }
                        }
                        break;
                    }
                    }
                }
                blockMap[idx >> 1] = 1;
                idx += step;
                blockMap[idx >> 1] = 1;
                dst += 8;
                prev += 8;
                x -= 8;
                break;
            case 2: {  // motion: copy a block of the previous frame
                u8 *s0;
                u8 *d0;
                u8 *s1;
                u8 *d1;
                blockMap[idx >> 1] = 1;
                s0 = prev + pitch * *(s8 *)yOff.cur++ + *(s8 *)xOff.cur++;
                d0 = dst;
                s1 = s0 + pitch;
                d1 = dst + pitch;
                if (!(((u32)dst | (u32)s0) & 7)) {
                    BINK_ROWS8_2(BINK_ROW_F64, d0, d1, pitch, s0, s1, pitch);
                } else if (!(((u32)dst | (u32)s0) & 3)) {
                    BINK_ROWS8_2(BINK_ROW_U32, d0, d1, pitch, s0, s1, pitch);
                } else {
                    BINK_ROWS8_2(BINK_ROW_U8, d0, d1, pitch, s0, s1, pitch);
                }
                break;
            }
            case 3: {  // run: runs of one color or of literal colors along a scan order
                u32 value = bits.value;
                u32 avail = bits.count;
                u32* nextv = bits.next;
                u32 *next = nextv;
                const u8 *scan;
                u32 v;
                u32 bit;
                BINK_GETBITS(v, 4);
                blockMap[idx >> 1] = 1;
                scan = lbl_802F8A40[v];
                i = 0;
                do {
                    u32 run;
                    BINK_GETBIT(bit);
                    if (bit & 1) {
                        u8 c = *colors.cur++;
                        for (run = *runs.cur++ + 1; run != 0; run--) {
                            tmp[scan[i++]] = c;
                        }
                    } else {
                        for (run = *runs.cur++ + 1; run != 0; run--) {
                            tmp[scan[i++]] = *colors.cur++;
                        }
                    }
                } while (i < 63);
                if (i == 63) {
                    tmp[scan[63]] = *colors.cur++;
                }
                {
                    u8 *d0 = dst;
                    u8 *d1 = dst + pitch;
                    BINK_ROWS8_1(BINK_TMP_F64, d0, d1, pitch);
                }
                bits.next = next;
                bits.value = value;
                bits.count = avail;
                break;
            }
            case 4: {  // residue on top of a motion copy
                u8 *src;
                u32 n;
                blockMap[idx >> 1] = 1;
                src = prev + pitch * *(s8 *)yOff.cur++ + *(s8 *)xOff.cur++;
                BINK_BR_GETBITS(&bits, n, 7);
                ReadBPLossyWithMotion(dst, pitch, &bits, n, src, pitch);
                break;
            }
            case 5: {  // intra DCT block
                s32 quant;
                blockMap[idx >> 1] = 1;
                block[0] = *(s16 *)intraDC.cur;
                intraDC.cur += 2;
                ReadBPLossless(block, &bits);
                BINK_BR_GETBITS(&bits, quant, 4);
                fn_80125CA0(dst, (void *)pitch, block, quant);
                break;
            }
            case 6: {  // fill with one color
                u32 c;
                u8 *d0;
                u8 *d1;
                d0 = dst;
                d1 = dst + pitch;
                c = *colors.cur++;
                blockMap[idx >> 1] = 1;
                c |= c << 8;
                c |= c << 16;
                BINK_ROWS8_1(BINK_FILL8, d0, d1, pitch);
                break;
            }
            case 7: {  // inter DCT block on top of a motion copy
                u8 *src;
                s32 quant;
                blockMap[idx >> 1] = 1;
                src = prev + pitch * *(s8 *)yOff.cur++ + *(s8 *)xOff.cur++;
                block[0] = *(s16 *)interDC.cur;
                interDC.cur += 2;
                ReadBPLossless(block, &bits);
                BINK_BR_GETBITS(&bits, quant, 4);
                FastmIDCT8x8WithMotion(dst, pitch, block, quant, src, pitch);
                break;
            }
            case 8: {  // two-color pattern
                u32 c0;
                u32 c1;
                u32 p;
                u8 *d0;
                u8 *d1;
                d0 = dst;
                d1 = dst + pitch;
                c0 = *colors.cur++;
                c0 |= c0 << 8;
                c0 |= c0 << 16;
                c1 = *colors.cur++;
                c1 |= c1 << 8;
                c1 |= c1 << 16;
                blockMap[idx >> 1] = 1;
                BINK_ROWS8_1(BINK_PAT8, d0, d1, pitch);
                break;
            }
            case 9:  // raw pixels
                blockMap[idx >> 1] = 1;
                {
                    u8 *d0 = dst;
                    u8 *d1 = dst + pitch;
                    if (!(((u32)dst | (u32)colors.cur) & 7)) {
                        BINK_ROWS8_1(BINK_RAW_F64, d0, d1, pitch);
                    } else if (!(((u32)dst | (u32)colors.cur) & 3)) {
                        BINK_ROWS8_1(BINK_RAW_U32, d0, d1, pitch);
                    } else {
                        BINK_ROWS8_1(BINK_RAW_U8, d0, d1, pitch);
                    }
                }
                colors.cur += 64;
                break;
            }
            idx += step;
            dst += 8;
            prev += 8;
        }
        if (step == 1) {
            if (y & 8) {
                blockMap += blocksWide;
            }
        } else {
            blockMap += blocksWide;
        }
        idx = 0;
        dst += rowSkip;
        prev += rowSkip;
    }
    *skipped = skipCount;
    bits.count = 0;
    return bits.next;
}
#pragma pop

// Decodes a video frame into the next frame buffer (alpha, luma, then the two chroma planes); returns the number
// of skipped blocks in the luma plane (in block pairs).
u32 ExpandBink(void *video, s32 blockMap, s32 blocksWide, s32 *data, s32 keyframe, s32 bufs, s32 flags,
                s32 videoFlags) {
    s32 cur;
    BinkFrameBuffers8011 *v = video;
    u32 result;
    s32 old = v->unk14;
    u32 *end;
    if (v->frames[old ^ 1].y.buffer != 0) {
        v->unk14 = old ^ 1;
    }
    cur = v->unk14;
    result = 0;
    if (videoFlags & 0x100000) {
        if (flags & 0x100000) {
            ExpandPlane(v->frames[cur].a.buffer, v->frames[old].a.buffer, v->width, v->height,
                        v->frames[old].a.pitch, (u32 *)(data + 1), keyframe, (u8 *)blockMap, blocksWide, 1,
                        (void **)bufs, flags, &result);
        }
        data = (s32 *)((u8 *)data + *data);
    }
    result = 0;
    if (!(flags & 0x8000)) {
        data++;
    }
    end = ExpandPlane(v->frames[cur].y.buffer, v->frames[old].y.buffer, v->width, v->height, v->frames[old].y.pitch,
                      (u32 *)data, keyframe, (u8 *)blockMap, blocksWide, 1, (void **)bufs, flags, &result);
    if (!(flags & 0x8000)) {
        end = (u32 *)((u8 *)data + data[-1] - 4);
    }
    result >>= 2;
    if (!(flags & 0x20000)) {
        end = ExpandPlane(v->frames[cur].cr.buffer, v->frames[old].cr.buffer, v->cWidth, v->cHeight,
                          v->frames[old].cr.pitch, end, keyframe, (u8 *)blockMap, blocksWide, 2, (void **)bufs,
                          flags, &result);
        result = 0;
        ExpandPlane(v->frames[cur].cb.buffer, v->frames[old].cb.buffer, v->cWidth, v->cHeight,
                    v->frames[old].cb.pitch, end, keyframe, (u8 *)blockMap, blocksWide, 2, (void **)bufs, flags,
                    &result);
    }
    return result;
}

extern u32 lbl_8055DEF0;  // total size of the pushed allocations
extern u32 lbl_8055DEF4;
// pushmalloc: registers *slot for the next batched allocation (popmalloc = popmalloc). Sizes are rounded to
// 32 bytes and padded so consecutive buffers start in different cache sets; the registration arrays double
// when full.
void pushmalloc(void **slot, u32 amount) {
    if (lbl_80559950 == lbl_8055DEF4) {
        void ***slots = radmalloc(lbl_80559950 * 16);
        u32 *sizes = (u32 *)((u8 *)slots + lbl_80559950 * 8);
        memcpy(slots, lbl_80559948, lbl_80559950 * 4);
        memcpy(sizes, lbl_8055994C, lbl_80559950 * 4);
        lbl_80559950 *= 2;
        if (lbl_80559950 != 64) {
            radfree(lbl_80559948);
        }
        lbl_80559948 = slots;
        lbl_8055994C = sizes;
    }
    amount = (amount + 31) & ~31;
    amount += ((((lbl_8055DEF0 >> 5) & 31) + 33 - ((amount >> 5) & 31)) << 5) & 0x3E0;
    lbl_8055DEF0 += amount;
    lbl_8055994C[lbl_8055DEF4] = amount;
    lbl_80559948[lbl_8055DEF4++] = slot;
}

u32 fn_8011FF24(void) {
    return lbl_8055DEF0;
}

extern u32 lbl_8055DEF4;
// Allocates one block for all the pending buffers, hands each its slice and returns the block.
void *popmalloc(s32 arg0) {
    s32 size = (arg0 + 0x1F) & ~0x1F;
    u32 i;
    u8 *block = radmalloc(lbl_8055DEF0 + size);
    u8 *p;
    lbl_8055DEF0 = 0;
    if (block != 0) {
        p = block + size;
        for (i = 0; i < lbl_8055DEF4; i++) {
            *lbl_80559948[i] = p;
            p += lbl_8055994C[i];
        }
    }
    lbl_8055DEF4 = 0;
    return block;
}

void fn_8011FFB8(void *arg0, void *arg1) {
    lbl_8055DEF8 = arg0;
    lbl_8055DEFC = arg1;
}

extern void *lbl_8055DEF8;
extern void *lbl_8055DEFC;
extern u32 lbl_8055DEF4;
extern void ***lbl_80559948;
extern u32 *lbl_8055994C;
extern void *malloc(u32);
extern void free(void *);
extern void *radmalloc(s32);
void *radmalloc(s32 arg0) {
    s32 base;
    u8 offset;
    u32 mode;
    s32 result;
    if (arg0 == 0 || (u32)arg0 == 0xFFFFFFFF) return 0;
    if (lbl_8055DEF8 != 0) {
        result = ((s32 (*)(s32))lbl_8055DEF8)(arg0 + 0x40);
        if (result != 0) {
            if (result == -1) return 0;
            mode = 3;
            goto got_memory;
        }
    }
    result = (s32)malloc(arg0 + 0x40);
    if (result == 0) return 0;
    mode = 0;
got_memory:
    base = result;
    offset = 0x40 - (base & 0x1F);
    result += offset;
    *((u8 *)result - 1) = offset;
    *((u8 *)result - 2) = mode;
    if (mode == 3) *(void **)(result - 8) = lbl_8055DEFC;
    return (void *)result;
}

// Frees a block from radmalloc: the bytes before it hold the alignment offset and which allocator made it.
void radfree(void *arg0) {
    u8 *ptr = arg0;
    if (ptr == 0) return;
    if (ptr[-2] == 3) ((void (*)(void *))*(void **)(ptr - 8))(ptr - ptr[-1]);
    else free(ptr - ptr[-1]);
}

void rdft(int n, int isgn, f32 *a, int *ip, f32 *w)
{
    void makewt(int nw, int *ip, f32 *w);
    void makect(int nc, int *ip, f32 *c);
    void fn_80120CA4(int n, f32 *a, int *ip, int nw, f32 *w);
    void fn_80120E14(int n, f32 *a, int *ip, int nw, f32 *w);
    void rftfsub(int n, f32 *a, int nc, f32 *c);
    void rftbsub(int n, f32 *a, int nc, f32 *c);
    int nw, nc;
    f32 xi;
    
    nw = ip[0];
    if (n > (nw << 2)) {
        nw = n >> 2;
        makewt(nw, ip, w);
    }
    nc = ip[1];
    if (n > (nc << 2)) {
        nc = n >> 2;
        makect(nc, ip, w + nw);
    }
    if (isgn >= 0) {
        if (n > 4) {
            fn_80120CA4(n, a, ip + 2, nw, w);
            rftfsub(n, a, nc, w + nw);
        } else if (n == 4) {
            fn_80120CA4(n, a, ip + 2, nw, w);
        }
        xi = a[0] - a[1];
        a[0] += a[1];
        a[1] = xi;
    } else {
        a[1] = 0.5f * (a[0] - a[1]);
        a[0] -= a[1];
        if (n > 4) {
            rftbsub(n, a, nc, w + nw);
            fn_80120E14(n, a, ip + 2, nw, w);
        } else if (n == 4) {
            fn_80120E14(n, a, ip + 2, nw, w);
        }
    }
}

void ddct(int n, int isgn, f32 *a, int *ip, f32 *w)
{
    void makewt(int nw, int *ip, f32 *w);
    void makect(int nc, int *ip, f32 *c);
    void fn_80120CA4(int n, f32 *a, int *ip, int nw, f32 *w);
    void fn_80120E14(int n, f32 *a, int *ip, int nw, f32 *w);
    void rftfsub(int n, f32 *a, int nc, f32 *c);
    void rftbsub(int n, f32 *a, int nc, f32 *c);
    void dctsub(int n, f32 *a, int nc, f32 *c);
    int j, nw, nc;
    f32 xr;
    
    nw = ip[0];
    if (n > (nw << 2)) {
        nw = n >> 2;
        makewt(nw, ip, w);
    }
    nc = ip[1];
    if (n > nc) {
        nc = n;
        makect(nc, ip, w + nw);
    }
    if (isgn < 0) {
        xr = a[n - 1];
        for (j = n - 2; j >= 2; j -= 2) {
            a[j + 1] = a[j] - a[j - 1];
            a[j] += a[j - 1];
        }
        a[1] = a[0] - xr;
        a[0] += xr;
        if (n > 4) {
            rftbsub(n, a, nc, w + nw);
            fn_80120E14(n, a, ip + 2, nw, w);
        } else if (n == 4) {
            fn_80120E14(n, a, ip + 2, nw, w);
        }
    }
    dctsub(n, a, nc, w + nw);
    if (isgn >= 0) {
        if (n > 4) {
            fn_80120CA4(n, a, ip + 2, nw, w);
            rftfsub(n, a, nc, w + nw);
        } else if (n == 4) {
            fn_80120CA4(n, a, ip + 2, nw, w);
        }
        xr = a[0] - a[1];
        a[0] += a[1];
        for (j = 2; j < n; j += 2) {
            a[j - 1] = a[j] - a[j + 1];
            a[j] += a[j + 1];
        }
        a[n - 1] = xr;
    }
}

// MSL's inline float wrappers (math_double.h). The float build of the package calls these, and spells
// Ooura's atan(1.0) as the float constant pi/4.
inline float cosf(float x) { return cos(x); }
inline float sinf(float x) { return sin(x); }

void makewt(int nw, int *ip, f32 *w)
{
    int j, nwh, nw0, nw1;
    f32 delta, wn4r, wk1r, wk1i, wk3r, wk3i;
    
    ip[0] = nw;
    ip[1] = 1;
    if (nw > 2) {
        nwh = nw >> 1;
        delta = 0.785398185f / nwh;  // atan(1.0) / nwh
        wn4r = cosf(delta * nwh);
        w[0] = 1;
        w[1] = wn4r;
        if (nwh >= 4) {
            w[2] = 0.5f / cosf(delta * 2);
            w[3] = 0.5f / cosf(delta * 6);
        }
        for (j = 4; j < nwh; j += 4) {
            w[j] = cosf(delta * j);
            w[j + 1] = sinf(delta * j);
            w[j + 2] = cosf(3 * delta * j);
            w[j + 3] = sinf(3 * delta * j);
        }
        nw0 = 0;
        while (nwh > 2) {
            nw1 = nw0 + nwh;
            nwh >>= 1;
            w[nw1] = 1;
            w[nw1 + 1] = wn4r;
            if (nwh >= 4) {
                wk1r = w[nw0 + 4];
                wk3r = w[nw0 + 6];
                w[nw1 + 2] = 0.5f / wk1r;
                w[nw1 + 3] = 0.5f / wk3r;
            }
            for (j = 4; j < nwh; j += 4) {
                wk1r = w[nw0 + 2 * j];
                wk1i = w[nw0 + 2 * j + 1];
                wk3r = w[nw0 + 2 * j + 2];
                wk3i = w[nw0 + 2 * j + 3];
                w[nw1 + j] = wk1r;
                w[nw1 + j + 1] = wk1i;
                w[nw1 + j + 2] = wk3r;
                w[nw1 + j + 3] = wk3i;
            }
            nw0 = nw1;
        }
    }
}


void makect(int nc, int *ip, f32 *c)
{
    int j, nch;
    f32 delta;
    
    ip[1] = nc;
    if (nc > 1) {
        nch = nc >> 1;
        delta = 0.785398185f / nch;  // atan(1.0) / nch
        c[0] = cosf(delta * nch);
        c[nch] = 0.5f * c[0];
        for (j = 1; j < nch; j++) {
            c[j] = 0.5f * cosf(delta * j);
            c[nc - j] = 0.5f * sinf(delta * j);
        }
    }
}


#define CDFT_RECURSIVE_N 512  /* length of the recursive FFT mode, <= (L1 cache size) / 16 */

void fn_80120CA4(int n, f32 *a, int *ip, int nw, f32 *w)
{
    void bitrv2(int n, int *ip, f32 *a);
    void bitrv216(f32 *a);
    void bitrv208(f32 *a);
    void cftf1st(int n, f32 *a, f32 *w);
    void cftrec1(int n, f32 *a, int nw, f32 *w);
    void cftrec2(int n, f32 *a, int nw, f32 *w);
    void cftexp1(int n, f32 *a, int nw, f32 *w);
    void cftfx41(int n, f32 *a, int nw, f32 *w);
    void cftf161(f32 *a, f32 *w);
    void cftf081(f32 *a, f32 *w);
    void cftf040(f32 *a);
    void cftx020(f32 *a);
    int m;
    
    if (n > 32) {
        m = n >> 2;
        cftf1st(n, a, &w[nw - m]);
        if (n > CDFT_RECURSIVE_N) {
            cftrec1(m, a, nw, w);
            cftrec2(m, &a[m], nw, w);
            cftrec1(m, &a[2 * m], nw, w);
            cftrec1(m, &a[3 * m], nw, w);
        } else if (m > 32) {
            cftexp1(n, a, nw, w);
        } else {
            cftfx41(n, a, nw, w);
        }
        bitrv2(n, ip, a);
    } else if (n > 8) {
        if (n == 32) {
            cftf161(a, &w[nw - 8]);
            bitrv216(a);
        } else {
            cftf081(a, w);
            bitrv208(a);
        }
    } else if (n == 8) {
        cftf040(a);
    } else if (n == 4) {
        cftx020(a);
    }
}

void fn_80120E14(int n, f32 *a, int *ip, int nw, f32 *w)
{
    void bitrv2conj(int n, int *ip, f32 *a);
    void bitrv216neg(f32 *a);
    void bitrv208neg(f32 *a);
    void cftb1st(int n, f32 *a, f32 *w);
    void cftrec1(int n, f32 *a, int nw, f32 *w);
    void cftrec2(int n, f32 *a, int nw, f32 *w);
    void cftexp1(int n, f32 *a, int nw, f32 *w);
    void cftfx41(int n, f32 *a, int nw, f32 *w);
    void cftf161(f32 *a, f32 *w);
    void cftf081(f32 *a, f32 *w);
    void cftb040(f32 *a);
    void cftx020(f32 *a);
    int m;
    
    if (n > 32) {
        m = n >> 2;
        cftb1st(n, a, &w[nw - m]);
        if (n > CDFT_RECURSIVE_N) {
            cftrec1(m, a, nw, w);
            cftrec2(m, &a[m], nw, w);
            cftrec1(m, &a[2 * m], nw, w);
            cftrec1(m, &a[3 * m], nw, w);
        } else if (m > 32) {
            cftexp1(n, a, nw, w);
        } else {
            cftfx41(n, a, nw, w);
        }
        bitrv2conj(n, ip, a);
    } else if (n > 8) {
        if (n == 32) {
            cftf161(a, &w[nw - 8]);
            bitrv216neg(a);
        } else {
            cftf081(a, w);
            bitrv208neg(a);
        }
    } else if (n == 8) {
        cftb040(a);
    } else if (n == 4) {
        cftx020(a);
    }
}

void bitrv2(int n, int *ip, f32 *a) {
    int j, j1, k, k1, l, m, m2;
    f32 xr, xi, yr, yi;

    ip[0] = 0;
    l = n;
    m = 1;
    while ((m << 3) < l) {
        l >>= 1;
        for (j = 0; j < m; j++) {
            ip[m + j] = ip[j] + l;
        }
        m <<= 1;
    }
    m2 = 2 * m;
    if ((m << 3) == l) {
        for (k = 0; k < m; k++) {
            for (j = 0; j < k; j++) {
                j1 = 2 * j + ip[k];
                k1 = 2 * k + ip[j];
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += 2 * m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 -= m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += 2 * m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
            }
            j1 = 2 * k + m2 + ip[k];
            k1 = j1 + m2;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
        }
    } else {
        for (k = 1; k < m; k++) {
            for (j = 0; j < k; j++) {
                j1 = 2 * j + ip[k];
                k1 = 2 * k + ip[j];
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
            }
        }
    }
}

void bitrv2conj(int n, int *ip, f32 *a) {
    int j, j1, k, k1, l, m, m2;
    f32 xr, xi, yr, yi;

    ip[0] = 0;
    l = n;
    m = 1;
    while ((m << 3) < l) {
        l >>= 1;
        for (j = 0; j < m; j++) {
            ip[m + j] = ip[j] + l;
        }
        m <<= 1;
    }
    m2 = 2 * m;
    if ((m << 3) == l) {
        for (k = 0; k < m; k++) {
            for (j = 0; j < k; j++) {
                j1 = 2 * j + ip[k];
                k1 = 2 * k + ip[j];
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += 2 * m2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 -= m2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += 2 * m2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
            }
            k1 = 2 * k + ip[k];
            a[k1 + 1] = -a[k1 + 1];
            j1 = k1 + m2;
            k1 = j1 + m2;
            xr = a[j1];
            xi = -a[j1 + 1];
            yr = a[k1];
            yi = -a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            k1 += m2;
            a[k1 + 1] = -a[k1 + 1];
        }
    } else {
        a[1] = -a[1];
        a[m2 + 1] = -a[m2 + 1];
        for (k = 1; k < m; k++) {
            for (j = 0; j < k; j++) {
                j1 = 2 * j + ip[k];
                k1 = 2 * k + ip[j];
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += m2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
            }
            k1 = 2 * k + ip[k];
            a[k1 + 1] = -a[k1 + 1];
            a[k1 + m2 + 1] = -a[k1 + m2 + 1];
        }
    }
}

void bitrv216(f32 *a)
{
    f32 x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i, 
        x5r, x5i, x7r, x7i, x8r, x8i, x10r, x10i, 
        x11r, x11i, x12r, x12i, x13r, x13i, x14r, x14i;
    
    x1r = a[2];
    x1i = a[3];
    x2r = a[4];
    x2i = a[5];
    x3r = a[6];
    x3i = a[7];
    x4r = a[8];
    x4i = a[9];
    x5r = a[10];
    x5i = a[11];
    x7r = a[14];
    x7i = a[15];
    x8r = a[16];
    x8i = a[17];
    x10r = a[20];
    x10i = a[21];
    x11r = a[22];
    x11i = a[23];
    x12r = a[24];
    x12i = a[25];
    x13r = a[26];
    x13i = a[27];
    x14r = a[28];
    x14i = a[29];
    a[2] = x8r;
    a[3] = x8i;
    a[4] = x4r;
    a[5] = x4i;
    a[6] = x12r;
    a[7] = x12i;
    a[8] = x2r;
    a[9] = x2i;
    a[10] = x10r;
    a[11] = x10i;
    a[14] = x14r;
    a[15] = x14i;
    a[16] = x1r;
    a[17] = x1i;
    a[20] = x5r;
    a[21] = x5i;
    a[22] = x13r;
    a[23] = x13i;
    a[24] = x3r;
    a[25] = x3i;
    a[26] = x11r;
    a[27] = x11i;
    a[28] = x7r;
    a[29] = x7i;
}

void bitrv216neg(f32 *a)
{
    f32 x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i, 
        x5r, x5i, x6r, x6i, x7r, x7i, x8r, x8i, 
        x9r, x9i, x10r, x10i, x11r, x11i, x12r, x12i, 
        x13r, x13i, x14r, x14i, x15r, x15i;
    
    x1r = a[2];
    x1i = a[3];
    x2r = a[4];
    x2i = a[5];
    x3r = a[6];
    x3i = a[7];
    x4r = a[8];
    x4i = a[9];
    x5r = a[10];
    x5i = a[11];
    x6r = a[12];
    x6i = a[13];
    x7r = a[14];
    x7i = a[15];
    x8r = a[16];
    x8i = a[17];
    x9r = a[18];
    x9i = a[19];
    x10r = a[20];
    x10i = a[21];
    x11r = a[22];
    x11i = a[23];
    x12r = a[24];
    x12i = a[25];
    x13r = a[26];
    x13i = a[27];
    x14r = a[28];
    x14i = a[29];
    x15r = a[30];
    x15i = a[31];
    a[2] = x15r;
    a[3] = x15i;
    a[4] = x7r;
    a[5] = x7i;
    a[6] = x11r;
    a[7] = x11i;
    a[8] = x3r;
    a[9] = x3i;
    a[10] = x13r;
    a[11] = x13i;
    a[12] = x5r;
    a[13] = x5i;
    a[14] = x9r;
    a[15] = x9i;
    a[16] = x1r;
    a[17] = x1i;
    a[18] = x14r;
    a[19] = x14i;
    a[20] = x6r;
    a[21] = x6i;
    a[22] = x10r;
    a[23] = x10i;
    a[24] = x2r;
    a[25] = x2i;
    a[26] = x12r;
    a[27] = x12i;
    a[28] = x4r;
    a[29] = x4i;
    a[30] = x8r;
    a[31] = x8i;
}

void bitrv208(f32 *a)
{
    f32 x1r, x1i, x3r, x3i, x4r, x4i, x6r, x6i;
    
    x1r = a[2];
    x1i = a[3];
    x3r = a[6];
    x3i = a[7];
    x4r = a[8];
    x4i = a[9];
    x6r = a[12];
    x6i = a[13];
    a[2] = x4r;
    a[3] = x4i;
    a[6] = x6r;
    a[7] = x6i;
    a[8] = x1r;
    a[9] = x1i;
    a[12] = x3r;
    a[13] = x3i;
}

void bitrv208neg(f32 *a)
{
    f32 x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i, 
        x5r, x5i, x6r, x6i, x7r, x7i;
    
    x1r = a[2];
    x1i = a[3];
    x2r = a[4];
    x2i = a[5];
    x3r = a[6];
    x3i = a[7];
    x4r = a[8];
    x4i = a[9];
    x5r = a[10];
    x5i = a[11];
    x6r = a[12];
    x6i = a[13];
    x7r = a[14];
    x7i = a[15];
    a[2] = x7r;
    a[3] = x7i;
    a[4] = x3r;
    a[5] = x3i;
    a[6] = x5r;
    a[7] = x5i;
    a[8] = x1r;
    a[9] = x1i;
    a[10] = x6r;
    a[11] = x6i;
    a[12] = x2r;
    a[13] = x2i;
    a[14] = x4r;
    a[15] = x4i;
}

void cftf1st(int n, f32 *a, f32 *w)
{
    int j, j0, j1, j2, j3, k, m, mh;
    f32 wn4r, csc1, csc3, wk1r, wk1i, wk3r, wk3i, 
        wd1r, wd1i, wd3r, wd3i;
    f32 x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i;
    
    mh = n >> 3;
    m = 2 * mh;
    j1 = m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[0] + a[j2];
    x0i = a[1] + a[j2 + 1];
    x1r = a[0] - a[j2];
    x1i = a[1] - a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i - x2i;
    a[j2] = x1r - x3i;
    a[j2 + 1] = x1i + x3r;
    a[j3] = x1r + x3i;
    a[j3 + 1] = x1i - x3r;
    wn4r = w[1];
    csc1 = w[2];
    csc3 = w[3];
    wd1r = 1;
    wd1i = 0;
    wd3r = 1;
    wd3i = 0;
    k = 0;
    for (j = 2; j < mh - 2; j += 4) {
        k += 4;
        wk1r = csc1 * (wd1r + w[k]);
        wk1i = csc1 * (wd1i + w[k + 1]);
        wk3r = csc3 * (wd3r + w[k + 2]);
        wk3i = csc3 * (wd3i - w[k + 3]);
        wd1r = w[k];
        wd1i = w[k + 1];
        wd3r = w[k + 2];
        wd3i = -w[k + 3];
        j1 = j + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j] + a[j2];
        x0i = a[j + 1] + a[j2 + 1];
        x1r = a[j] - a[j2];
        x1i = a[j + 1] - a[j2 + 1];
        y0r = a[j + 2] + a[j2 + 2];
        y0i = a[j + 3] + a[j2 + 3];
        y1r = a[j + 2] - a[j2 + 2];
        y1i = a[j + 3] - a[j2 + 3];
        x2r = a[j1] + a[j3];
        x2i = a[j1 + 1] + a[j3 + 1];
        x3r = a[j1] - a[j3];
        x3i = a[j1 + 1] - a[j3 + 1];
        y2r = a[j1 + 2] + a[j3 + 2];
        y2i = a[j1 + 3] + a[j3 + 3];
        y3r = a[j1 + 2] - a[j3 + 2];
        y3i = a[j1 + 3] - a[j3 + 3];
        a[j] = x0r + x2r;
        a[j + 1] = x0i + x2i;
        a[j + 2] = y0r + y2r;
        a[j + 3] = y0i + y2i;
        a[j1] = x0r - x2r;
        a[j1 + 1] = x0i - x2i;
        a[j1 + 2] = y0r - y2r;
        a[j1 + 3] = y0i - y2i;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j2] = wk1r * x0r - wk1i * x0i;
        a[j2 + 1] = wk1r * x0i + wk1i * x0r;
        x0r = y1r - y3i;
        x0i = y1i + y3r;
        a[j2 + 2] = wd1r * x0r - wd1i * x0i;
        a[j2 + 3] = wd1r * x0i + wd1i * x0r;
        x0r = x1r + x3i;
        x0i = x1i - x3r;
        a[j3] = wk3r * x0r + wk3i * x0i;
        a[j3 + 1] = wk3r * x0i - wk3i * x0r;
        x0r = y1r + y3i;
        x0i = y1i - y3r;
        a[j3 + 2] = wd3r * x0r + wd3i * x0i;
        a[j3 + 3] = wd3r * x0i - wd3i * x0r;
        j0 = m - j;
        j1 = j0 + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j0] + a[j2];
        x0i = a[j0 + 1] + a[j2 + 1];
        x1r = a[j0] - a[j2];
        x1i = a[j0 + 1] - a[j2 + 1];
        y0r = a[j0 - 2] + a[j2 - 2];
        y0i = a[j0 - 1] + a[j2 - 1];
        y1r = a[j0 - 2] - a[j2 - 2];
        y1i = a[j0 - 1] - a[j2 - 1];
        x2r = a[j1] + a[j3];
        x2i = a[j1 + 1] + a[j3 + 1];
        x3r = a[j1] - a[j3];
        x3i = a[j1 + 1] - a[j3 + 1];
        y2r = a[j1 - 2] + a[j3 - 2];
        y2i = a[j1 - 1] + a[j3 - 1];
        y3r = a[j1 - 2] - a[j3 - 2];
        y3i = a[j1 - 1] - a[j3 - 1];
        a[j0] = x0r + x2r;
        a[j0 + 1] = x0i + x2i;
        a[j0 - 2] = y0r + y2r;
        a[j0 - 1] = y0i + y2i;
        a[j1] = x0r - x2r;
        a[j1 + 1] = x0i - x2i;
        a[j1 - 2] = y0r - y2r;
        a[j1 - 1] = y0i - y2i;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j2] = wk1i * x0r - wk1r * x0i;
        a[j2 + 1] = wk1i * x0i + wk1r * x0r;
        x0r = y1r - y3i;
        x0i = y1i + y3r;
        a[j2 - 2] = wd1i * x0r - wd1r * x0i;
        a[j2 - 1] = wd1i * x0i + wd1r * x0r;
        x0r = x1r + x3i;
        x0i = x1i - x3r;
        a[j3] = wk3i * x0r + wk3r * x0i;
        a[j3 + 1] = wk3i * x0i - wk3r * x0r;
        x0r = y1r + y3i;
        x0i = y1i - y3r;
        a[j3 - 2] = wd3i * x0r + wd3r * x0i;
        a[j3 - 1] = wd3i * x0i - wd3r * x0r;
    }
    wk1r = csc1 * (wd1r + wn4r);
    wk1i = csc1 * (wd1i + wn4r);
    wk3r = csc3 * (wd3r - wn4r);
    wk3i = csc3 * (wd3i - wn4r);
    j0 = mh;
    j1 = j0 + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j0 - 2] + a[j2 - 2];
    x0i = a[j0 - 1] + a[j2 - 1];
    x1r = a[j0 - 2] - a[j2 - 2];
    x1i = a[j0 - 1] - a[j2 - 1];
    x2r = a[j1 - 2] + a[j3 - 2];
    x2i = a[j1 - 1] + a[j3 - 1];
    x3r = a[j1 - 2] - a[j3 - 2];
    x3i = a[j1 - 1] - a[j3 - 1];
    a[j0 - 2] = x0r + x2r;
    a[j0 - 1] = x0i + x2i;
    a[j1 - 2] = x0r - x2r;
    a[j1 - 1] = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j2 - 2] = wk1r * x0r - wk1i * x0i;
    a[j2 - 1] = wk1r * x0i + wk1i * x0r;
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    a[j3 - 2] = wk3r * x0r + wk3i * x0i;
    a[j3 - 1] = wk3r * x0i - wk3i * x0r;
    x0r = a[j0] + a[j2];
    x0i = a[j0 + 1] + a[j2 + 1];
    x1r = a[j0] - a[j2];
    x1i = a[j0 + 1] - a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[j0] = x0r + x2r;
    a[j0 + 1] = x0i + x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j2] = wn4r * (x0r - x0i);
    a[j2 + 1] = wn4r * (x0i + x0r);
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    a[j3] = -wn4r * (x0r + x0i);
    a[j3 + 1] = -wn4r * (x0i - x0r);
    x0r = a[j0 + 2] + a[j2 + 2];
    x0i = a[j0 + 3] + a[j2 + 3];
    x1r = a[j0 + 2] - a[j2 + 2];
    x1i = a[j0 + 3] - a[j2 + 3];
    x2r = a[j1 + 2] + a[j3 + 2];
    x2i = a[j1 + 3] + a[j3 + 3];
    x3r = a[j1 + 2] - a[j3 + 2];
    x3i = a[j1 + 3] - a[j3 + 3];
    a[j0 + 2] = x0r + x2r;
    a[j0 + 3] = x0i + x2i;
    a[j1 + 2] = x0r - x2r;
    a[j1 + 3] = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j2 + 2] = wk1i * x0r - wk1r * x0i;
    a[j2 + 3] = wk1i * x0i + wk1r * x0r;
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    a[j3 + 2] = wk3i * x0r + wk3r * x0i;
    a[j3 + 3] = wk3i * x0i - wk3r * x0r;
}

void cftb1st(int n, f32 *a, f32 *w)
{
    int j, j0, j1, j2, j3, k, m, mh;
    f32 wn4r, csc1, csc3, wk1r, wk1i, wk3r, wk3i, 
        wd1r, wd1i, wd3r, wd3i;
    f32 x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i;
    
    mh = n >> 3;
    m = 2 * mh;
    j1 = m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[0] + a[j2];
    x0i = -a[1] - a[j2 + 1];
    x1r = a[0] - a[j2];
    x1i = -a[1] + a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[0] = x0r + x2r;
    a[1] = x0i - x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i + x2i;
    a[j2] = x1r + x3i;
    a[j2 + 1] = x1i + x3r;
    a[j3] = x1r - x3i;
    a[j3 + 1] = x1i - x3r;
    wn4r = w[1];
    csc1 = w[2];
    csc3 = w[3];
    wd1r = 1;
    wd1i = 0;
    wd3r = 1;
    wd3i = 0;
    k = 0;
    for (j = 2; j < mh - 2; j += 4) {
        k += 4;
        wk1r = csc1 * (wd1r + w[k]);
        wk1i = csc1 * (wd1i + w[k + 1]);
        wk3r = csc3 * (wd3r + w[k + 2]);
        wk3i = csc3 * (wd3i - w[k + 3]);
        wd1r = w[k];
        wd1i = w[k + 1];
        wd3r = w[k + 2];
        wd3i = -w[k + 3];
        j1 = j + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j] + a[j2];
        x0i = -a[j + 1] - a[j2 + 1];
        x1r = a[j] - a[j2];
        x1i = -a[j + 1] + a[j2 + 1];
        y0r = a[j + 2] + a[j2 + 2];
        y0i = -a[j + 3] - a[j2 + 3];
        y1r = a[j + 2] - a[j2 + 2];
        y1i = -a[j + 3] + a[j2 + 3];
        x2r = a[j1] + a[j3];
        x2i = a[j1 + 1] + a[j3 + 1];
        x3r = a[j1] - a[j3];
        x3i = a[j1 + 1] - a[j3 + 1];
        y2r = a[j1 + 2] + a[j3 + 2];
        y2i = a[j1 + 3] + a[j3 + 3];
        y3r = a[j1 + 2] - a[j3 + 2];
        y3i = a[j1 + 3] - a[j3 + 3];
        a[j] = x0r + x2r;
        a[j + 1] = x0i - x2i;
        a[j + 2] = y0r + y2r;
        a[j + 3] = y0i - y2i;
        a[j1] = x0r - x2r;
        a[j1 + 1] = x0i + x2i;
        a[j1 + 2] = y0r - y2r;
        a[j1 + 3] = y0i + y2i;
        x0r = x1r + x3i;
        x0i = x1i + x3r;
        a[j2] = wk1r * x0r - wk1i * x0i;
        a[j2 + 1] = wk1r * x0i + wk1i * x0r;
        x0r = y1r + y3i;
        x0i = y1i + y3r;
        a[j2 + 2] = wd1r * x0r - wd1i * x0i;
        a[j2 + 3] = wd1r * x0i + wd1i * x0r;
        x0r = x1r - x3i;
        x0i = x1i - x3r;
        a[j3] = wk3r * x0r + wk3i * x0i;
        a[j3 + 1] = wk3r * x0i - wk3i * x0r;
        x0r = y1r - y3i;
        x0i = y1i - y3r;
        a[j3 + 2] = wd3r * x0r + wd3i * x0i;
        a[j3 + 3] = wd3r * x0i - wd3i * x0r;
        j0 = m - j;
        j1 = j0 + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j0] + a[j2];
        x0i = -a[j0 + 1] - a[j2 + 1];
        x1r = a[j0] - a[j2];
        x1i = -a[j0 + 1] + a[j2 + 1];
        y0r = a[j0 - 2] + a[j2 - 2];
        y0i = -a[j0 - 1] - a[j2 - 1];
        y1r = a[j0 - 2] - a[j2 - 2];
        y1i = -a[j0 - 1] + a[j2 - 1];
        x2r = a[j1] + a[j3];
        x2i = a[j1 + 1] + a[j3 + 1];
        x3r = a[j1] - a[j3];
        x3i = a[j1 + 1] - a[j3 + 1];
        y2r = a[j1 - 2] + a[j3 - 2];
        y2i = a[j1 - 1] + a[j3 - 1];
        y3r = a[j1 - 2] - a[j3 - 2];
        y3i = a[j1 - 1] - a[j3 - 1];
        a[j0] = x0r + x2r;
        a[j0 + 1] = x0i - x2i;
        a[j0 - 2] = y0r + y2r;
        a[j0 - 1] = y0i - y2i;
        a[j1] = x0r - x2r;
        a[j1 + 1] = x0i + x2i;
        a[j1 - 2] = y0r - y2r;
        a[j1 - 1] = y0i + y2i;
        x0r = x1r + x3i;
        x0i = x1i + x3r;
        a[j2] = wk1i * x0r - wk1r * x0i;
        a[j2 + 1] = wk1i * x0i + wk1r * x0r;
        x0r = y1r + y3i;
        x0i = y1i + y3r;
        a[j2 - 2] = wd1i * x0r - wd1r * x0i;
        a[j2 - 1] = wd1i * x0i + wd1r * x0r;
        x0r = x1r - x3i;
        x0i = x1i - x3r;
        a[j3] = wk3i * x0r + wk3r * x0i;
        a[j3 + 1] = wk3i * x0i - wk3r * x0r;
        x0r = y1r - y3i;
        x0i = y1i - y3r;
        a[j3 - 2] = wd3i * x0r + wd3r * x0i;
        a[j3 - 1] = wd3i * x0i - wd3r * x0r;
    }
    wk1r = csc1 * (wd1r + wn4r);
    wk1i = csc1 * (wd1i + wn4r);
    wk3r = csc3 * (wd3r - wn4r);
    wk3i = csc3 * (wd3i - wn4r);
    j0 = mh;
    j1 = j0 + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j0 - 2] + a[j2 - 2];
    x0i = -a[j0 - 1] - a[j2 - 1];
    x1r = a[j0 - 2] - a[j2 - 2];
    x1i = -a[j0 - 1] + a[j2 - 1];
    x2r = a[j1 - 2] + a[j3 - 2];
    x2i = a[j1 - 1] + a[j3 - 1];
    x3r = a[j1 - 2] - a[j3 - 2];
    x3i = a[j1 - 1] - a[j3 - 1];
    a[j0 - 2] = x0r + x2r;
    a[j0 - 1] = x0i - x2i;
    a[j1 - 2] = x0r - x2r;
    a[j1 - 1] = x0i + x2i;
    x0r = x1r + x3i;
    x0i = x1i + x3r;
    a[j2 - 2] = wk1r * x0r - wk1i * x0i;
    a[j2 - 1] = wk1r * x0i + wk1i * x0r;
    x0r = x1r - x3i;
    x0i = x1i - x3r;
    a[j3 - 2] = wk3r * x0r + wk3i * x0i;
    a[j3 - 1] = wk3r * x0i - wk3i * x0r;
    x0r = a[j0] + a[j2];
    x0i = -a[j0 + 1] - a[j2 + 1];
    x1r = a[j0] - a[j2];
    x1i = -a[j0 + 1] + a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[j0] = x0r + x2r;
    a[j0 + 1] = x0i - x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i + x2i;
    x0r = x1r + x3i;
    x0i = x1i + x3r;
    a[j2] = wn4r * (x0r - x0i);
    a[j2 + 1] = wn4r * (x0i + x0r);
    x0r = x1r - x3i;
    x0i = x1i - x3r;
    a[j3] = -wn4r * (x0r + x0i);
    a[j3 + 1] = -wn4r * (x0i - x0r);
    x0r = a[j0 + 2] + a[j2 + 2];
    x0i = -a[j0 + 3] - a[j2 + 3];
    x1r = a[j0 + 2] - a[j2 + 2];
    x1i = -a[j0 + 3] + a[j2 + 3];
    x2r = a[j1 + 2] + a[j3 + 2];
    x2i = a[j1 + 3] + a[j3 + 3];
    x3r = a[j1 + 2] - a[j3 + 2];
    x3i = a[j1 + 3] - a[j3 + 3];
    a[j0 + 2] = x0r + x2r;
    a[j0 + 3] = x0i - x2i;
    a[j1 + 2] = x0r - x2r;
    a[j1 + 3] = x0i + x2i;
    x0r = x1r + x3i;
    x0i = x1i + x3r;
    a[j2 + 2] = wk1i * x0r - wk1r * x0i;
    a[j2 + 3] = wk1i * x0i + wk1r * x0r;
    x0r = x1r - x3i;
    x0i = x1i - x3r;
    a[j3 + 2] = wk3i * x0r + wk3r * x0i;
    a[j3 + 3] = wk3i * x0i - wk3r * x0r;
}

void cftrec1(int n, f32 *a, int nw, f32 *w)
{
    void cftrec1(int n, f32 *a, int nw, f32 *w);
    void cftrec2(int n, f32 *a, int nw, f32 *w);
    void cftmdl1(int n, f32 *a, f32 *w);
    void cftexp1(int n, f32 *a, int nw, f32 *w);
    int m;
    
    m = n >> 2;
    cftmdl1(n, a, &w[nw - 2 * m]);
    if (n > CDFT_RECURSIVE_N) {
        cftrec1(m, a, nw, w);
        cftrec2(m, &a[m], nw, w);
        cftrec1(m, &a[2 * m], nw, w);
        cftrec1(m, &a[3 * m], nw, w);
    } else {
        cftexp1(n, a, nw, w);
    }
}

void cftrec2(int n, f32 *a, int nw, f32 *w)
{
    void cftrec1(int n, f32 *a, int nw, f32 *w);
    void cftrec2(int n, f32 *a, int nw, f32 *w);
    void cftmdl2(int n, f32 *a, f32 *w);
    void cftexp2(int n, f32 *a, int nw, f32 *w);
    int m;
    
    m = n >> 2;
    cftmdl2(n, a, &w[nw - n]);
    if (n > CDFT_RECURSIVE_N) {
        cftrec1(m, a, nw, w);
        cftrec2(m, &a[m], nw, w);
        cftrec1(m, &a[2 * m], nw, w);
        cftrec2(m, &a[3 * m], nw, w);
    } else {
        cftexp2(n, a, nw, w);
    }
}

void cftexp1(int n, f32 *a, int nw, f32 *w)
{
    void cftmdl1(int n, f32 *a, f32 *w);
    void cftmdl2(int n, f32 *a, f32 *w);
    void cftfx41(int n, f32 *a, int nw, f32 *w);
    void cftfx42(int n, f32 *a, int nw, f32 *w);
    int j, k, l;
    
    l = n >> 2;
    while (l > 128) {
        for (k = l; k < n; k <<= 2) {
            for (j = k - l; j < n; j += 4 * k) {
                cftmdl1(l, &a[j], &w[nw - (l >> 1)]);
                cftmdl2(l, &a[k + j], &w[nw - l]);
                cftmdl1(l, &a[2 * k + j], &w[nw - (l >> 1)]);
            }
        }
        cftmdl1(l, &a[n - l], &w[nw - (l >> 1)]);
        l >>= 2;
    }
    for (k = l; k < n; k <<= 2) {
        for (j = k - l; j < n; j += 4 * k) {
            cftmdl1(l, &a[j], &w[nw - (l >> 1)]);
            cftfx41(l, &a[j], nw, w);
            cftmdl2(l, &a[k + j], &w[nw - l]);
            cftfx42(l, &a[k + j], nw, w);
            cftmdl1(l, &a[2 * k + j], &w[nw - (l >> 1)]);
            cftfx41(l, &a[2 * k + j], nw, w);
        }
    }
    cftmdl1(l, &a[n - l], &w[nw - (l >> 1)]);
    cftfx41(l, &a[n - l], nw, w);
}

void cftexp2(int n, f32 *a, int nw, f32 *w)
{
    void cftmdl1(int n, f32 *a, f32 *w);
    void cftmdl2(int n, f32 *a, f32 *w);
    void cftfx41(int n, f32 *a, int nw, f32 *w);
    void cftfx42(int n, f32 *a, int nw, f32 *w);
    int j, k, l, m;
    
    m = n >> 1;
    l = n >> 2;
    while (l > 128) {
        for (k = l; k < m; k <<= 2) {
            for (j = k - l; j < m; j += 2 * k) {
                cftmdl1(l, &a[j], &w[nw - (l >> 1)]);
                cftmdl1(l, &a[m + j], &w[nw - (l >> 1)]);
            }
            for (j = 2 * k - l; j < m; j += 4 * k) {
                cftmdl2(l, &a[j], &w[nw - l]);
                cftmdl2(l, &a[m + j], &w[nw - l]);
            }
        }
        l >>= 2;
    }
    for (k = l; k < m; k <<= 2) {
        for (j = k - l; j < m; j += 2 * k) {
            cftmdl1(l, &a[j], &w[nw - (l >> 1)]);
            cftfx41(l, &a[j], nw, w);
            cftmdl1(l, &a[m + j], &w[nw - (l >> 1)]);
            cftfx41(l, &a[m + j], nw, w);
        }
        for (j = 2 * k - l; j < m; j += 4 * k) {
            cftmdl2(l, &a[j], &w[nw - l]);
            cftfx42(l, &a[j], nw, w);
            cftmdl2(l, &a[m + j], &w[nw - l]);
            cftfx42(l, &a[m + j], nw, w);
        }
    }
}

void cftmdl1(int n, f32 *a, f32 *w)
{
    int j, j0, j1, j2, j3, k, m, mh;
    f32 wn4r, wk1r, wk1i, wk3r, wk3i;
    f32 x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    
    mh = n >> 3;
    m = 2 * mh;
    j1 = m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[0] + a[j2];
    x0i = a[1] + a[j2 + 1];
    x1r = a[0] - a[j2];
    x1i = a[1] - a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i - x2i;
    a[j2] = x1r - x3i;
    a[j2 + 1] = x1i + x3r;
    a[j3] = x1r + x3i;
    a[j3 + 1] = x1i - x3r;
    wn4r = w[1];
    k = 0;
    for (j = 2; j < mh; j += 2) {
        k += 4;
        wk1r = w[k];
        wk1i = w[k + 1];
        wk3r = w[k + 2];
        wk3i = -w[k + 3];
        j1 = j + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j] + a[j2];
        x0i = a[j + 1] + a[j2 + 1];
        x1r = a[j] - a[j2];
        x1i = a[j + 1] - a[j2 + 1];
        x2r = a[j1] + a[j3];
        x2i = a[j1 + 1] + a[j3 + 1];
        x3r = a[j1] - a[j3];
        x3i = a[j1 + 1] - a[j3 + 1];
        a[j] = x0r + x2r;
        a[j + 1] = x0i + x2i;
        a[j1] = x0r - x2r;
        a[j1 + 1] = x0i - x2i;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j2] = wk1r * x0r - wk1i * x0i;
        a[j2 + 1] = wk1r * x0i + wk1i * x0r;
        x0r = x1r + x3i;
        x0i = x1i - x3r;
        a[j3] = wk3r * x0r + wk3i * x0i;
        a[j3 + 1] = wk3r * x0i - wk3i * x0r;
        j0 = m - j;
        j1 = j0 + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j0] + a[j2];
        x0i = a[j0 + 1] + a[j2 + 1];
        x1r = a[j0] - a[j2];
        x1i = a[j0 + 1] - a[j2 + 1];
        x2r = a[j1] + a[j3];
        x2i = a[j1 + 1] + a[j3 + 1];
        x3r = a[j1] - a[j3];
        x3i = a[j1 + 1] - a[j3 + 1];
        a[j0] = x0r + x2r;
        a[j0 + 1] = x0i + x2i;
        a[j1] = x0r - x2r;
        a[j1 + 1] = x0i - x2i;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j2] = wk1i * x0r - wk1r * x0i;
        a[j2 + 1] = wk1i * x0i + wk1r * x0r;
        x0r = x1r + x3i;
        x0i = x1i - x3r;
        a[j3] = wk3i * x0r + wk3r * x0i;
        a[j3 + 1] = wk3i * x0i - wk3r * x0r;
    }
    j0 = mh;
    j1 = j0 + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j0] + a[j2];
    x0i = a[j0 + 1] + a[j2 + 1];
    x1r = a[j0] - a[j2];
    x1i = a[j0 + 1] - a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[j0] = x0r + x2r;
    a[j0 + 1] = x0i + x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j2] = wn4r * (x0r - x0i);
    a[j2 + 1] = wn4r * (x0i + x0r);
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    a[j3] = -wn4r * (x0r + x0i);
    a[j3 + 1] = -wn4r * (x0i - x0r);
}

void cftmdl2(int n, f32 *a, f32 *w)
{
    int j, j0, j1, j2, j3, k, kr, m, mh;
    f32 wn4r, wk1r, wk1i, wk3r, wk3i, wd1r, wd1i, wd3r, wd3i;
    f32 x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, y0r, y0i, y2r, y2i;
    
    mh = n >> 3;
    m = 2 * mh;
    wn4r = w[1];
    j1 = m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[0] - a[j2 + 1];
    x0i = a[1] + a[j2];
    x1r = a[0] + a[j2 + 1];
    x1i = a[1] - a[j2];
    x2r = a[j1] - a[j3 + 1];
    x2i = a[j1 + 1] + a[j3];
    x3r = a[j1] + a[j3 + 1];
    x3i = a[j1 + 1] - a[j3];
    y0r = wn4r * (x2r - x2i);
    y0i = wn4r * (x2i + x2r);
    a[0] = x0r + y0r;
    a[1] = x0i + y0i;
    a[j1] = x0r - y0r;
    a[j1 + 1] = x0i - y0i;
    y0r = wn4r * (x3r - x3i);
    y0i = wn4r * (x3i + x3r);
    a[j2] = x1r - y0i;
    a[j2 + 1] = x1i + y0r;
    a[j3] = x1r + y0i;
    a[j3 + 1] = x1i - y0r;
    k = 0;
    kr = 2 * m;
    for (j = 2; j < mh; j += 2) {
        k += 4;
        wk1r = w[k];
        wk1i = w[k + 1];
        wk3r = w[k + 2];
        wk3i = -w[k + 3];
        kr -= 4;
        wd1i = w[kr];
        wd1r = w[kr + 1];
        wd3i = w[kr + 2];
        wd3r = -w[kr + 3];
        j1 = j + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j] - a[j2 + 1];
        x0i = a[j + 1] + a[j2];
        x1r = a[j] + a[j2 + 1];
        x1i = a[j + 1] - a[j2];
        x2r = a[j1] - a[j3 + 1];
        x2i = a[j1 + 1] + a[j3];
        x3r = a[j1] + a[j3 + 1];
        x3i = a[j1 + 1] - a[j3];
        y0r = wk1r * x0r - wk1i * x0i;
        y0i = wk1r * x0i + wk1i * x0r;
        y2r = wd1r * x2r - wd1i * x2i;
        y2i = wd1r * x2i + wd1i * x2r;
        a[j] = y0r + y2r;
        a[j + 1] = y0i + y2i;
        a[j1] = y0r - y2r;
        a[j1 + 1] = y0i - y2i;
        y0r = wk3r * x1r + wk3i * x1i;
        y0i = wk3r * x1i - wk3i * x1r;
        y2r = wd3r * x3r + wd3i * x3i;
        y2i = wd3r * x3i - wd3i * x3r;
        a[j2] = y0r + y2r;
        a[j2 + 1] = y0i + y2i;
        a[j3] = y0r - y2r;
        a[j3 + 1] = y0i - y2i;
        j0 = m - j;
        j1 = j0 + m;
        j2 = j1 + m;
        j3 = j2 + m;
        x0r = a[j0] - a[j2 + 1];
        x0i = a[j0 + 1] + a[j2];
        x1r = a[j0] + a[j2 + 1];
        x1i = a[j0 + 1] - a[j2];
        x2r = a[j1] - a[j3 + 1];
        x2i = a[j1 + 1] + a[j3];
        x3r = a[j1] + a[j3 + 1];
        x3i = a[j1 + 1] - a[j3];
        y0r = wd1i * x0r - wd1r * x0i;
        y0i = wd1i * x0i + wd1r * x0r;
        y2r = wk1i * x2r - wk1r * x2i;
        y2i = wk1i * x2i + wk1r * x2r;
        a[j0] = y0r + y2r;
        a[j0 + 1] = y0i + y2i;
        a[j1] = y0r - y2r;
        a[j1 + 1] = y0i - y2i;
        y0r = wd3i * x1r + wd3r * x1i;
        y0i = wd3i * x1i - wd3r * x1r;
        y2r = wk3i * x3r + wk3r * x3i;
        y2i = wk3i * x3i - wk3r * x3r;
        a[j2] = y0r + y2r;
        a[j2 + 1] = y0i + y2i;
        a[j3] = y0r - y2r;
        a[j3 + 1] = y0i - y2i;
    }
    wk1r = w[m];
    wk1i = w[m + 1];
    j0 = mh;
    j1 = j0 + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j0] - a[j2 + 1];
    x0i = a[j0 + 1] + a[j2];
    x1r = a[j0] + a[j2 + 1];
    x1i = a[j0 + 1] - a[j2];
    x2r = a[j1] - a[j3 + 1];
    x2i = a[j1 + 1] + a[j3];
    x3r = a[j1] + a[j3 + 1];
    x3i = a[j1 + 1] - a[j3];
    y0r = wk1r * x0r - wk1i * x0i;
    y0i = wk1r * x0i + wk1i * x0r;
    y2r = wk1i * x2r - wk1r * x2i;
    y2i = wk1i * x2i + wk1r * x2r;
    a[j0] = y0r + y2r;
    a[j0 + 1] = y0i + y2i;
    a[j1] = y0r - y2r;
    a[j1 + 1] = y0i - y2i;
    y0r = wk1i * x1r - wk1r * x1i;
    y0i = wk1i * x1i + wk1r * x1r;
    y2r = wk1r * x3r - wk1i * x3i;
    y2i = wk1r * x3i + wk1i * x3r;
    a[j2] = y0r - y2r;
    a[j2 + 1] = y0i - y2i;
    a[j3] = y0r + y2r;
    a[j3 + 1] = y0i + y2i;
}

void cftfx41(int n, f32 *a, int nw, f32 *w)
{
    void cftf161(f32 *a, f32 *w);
    void cftf162(f32 *a, f32 *w);
    void cftf081(f32 *a, f32 *w);
    void cftf082(f32 *a, f32 *w);
    
    if (n == 128) {
        cftf161(a, &w[nw - 8]);
        cftf162(&a[32], &w[nw - 32]);
        cftf161(&a[64], &w[nw - 8]);
        cftf161(&a[96], &w[nw - 8]);
    } else {
        cftf081(a, &w[nw - 16]);
        cftf082(&a[16], &w[nw - 16]);
        cftf081(&a[32], &w[nw - 16]);
        cftf081(&a[48], &w[nw - 16]);
    }
}

void cftfx42(int n, f32 *a, int nw, f32 *w)
{
    void cftf161(f32 *a, f32 *w);
    void cftf162(f32 *a, f32 *w);
    void cftf081(f32 *a, f32 *w);
    void cftf082(f32 *a, f32 *w);
    
    if (n == 128) {
        cftf161(a, &w[nw - 8]);
        cftf162(&a[32], &w[nw - 32]);
        cftf161(&a[64], &w[nw - 8]);
        cftf162(&a[96], &w[nw - 32]);
    } else {
        cftf081(a, &w[nw - 16]);
        cftf082(&a[16], &w[nw - 16]);
        cftf081(&a[32], &w[nw - 16]);
        cftf082(&a[48], &w[nw - 16]);
    }
}

void cftf161(f32 *a, f32 *w)
{
    f32 wn4r, wk1r, wk1i, 
        x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, 
        y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i, 
        y8r, y8i, y9r, y9i, y10r, y10i, y11r, y11i, 
        y12r, y12i, y13r, y13i, y14r, y14i, y15r, y15i;
    
    wn4r = w[1];
    wk1i = wn4r * w[2];
    wk1r = wk1i + w[2];
    x0r = a[0] + a[16];
    x0i = a[1] + a[17];
    x1r = a[0] - a[16];
    x1i = a[1] - a[17];
    x2r = a[8] + a[24];
    x2i = a[9] + a[25];
    x3r = a[8] - a[24];
    x3i = a[9] - a[25];
    y0r = x0r + x2r;
    y0i = x0i + x2i;
    y4r = x0r - x2r;
    y4i = x0i - x2i;
    y8r = x1r - x3i;
    y8i = x1i + x3r;
    y12r = x1r + x3i;
    y12i = x1i - x3r;
    x0r = a[2] + a[18];
    x0i = a[3] + a[19];
    x1r = a[2] - a[18];
    x1i = a[3] - a[19];
    x2r = a[10] + a[26];
    x2i = a[11] + a[27];
    x3r = a[10] - a[26];
    x3i = a[11] - a[27];
    y1r = x0r + x2r;
    y1i = x0i + x2i;
    y5r = x0r - x2r;
    y5i = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    y9r = wk1r * x0r - wk1i * x0i;
    y9i = wk1r * x0i + wk1i * x0r;
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    y13r = wk1i * x0r - wk1r * x0i;
    y13i = wk1i * x0i + wk1r * x0r;
    x0r = a[4] + a[20];
    x0i = a[5] + a[21];
    x1r = a[4] - a[20];
    x1i = a[5] - a[21];
    x2r = a[12] + a[28];
    x2i = a[13] + a[29];
    x3r = a[12] - a[28];
    x3i = a[13] - a[29];
    y2r = x0r + x2r;
    y2i = x0i + x2i;
    y6r = x0r - x2r;
    y6i = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    y10r = wn4r * (x0r - x0i);
    y10i = wn4r * (x0i + x0r);
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    y14r = wn4r * (x0r + x0i);
    y14i = wn4r * (x0i - x0r);
    x0r = a[6] + a[22];
    x0i = a[7] + a[23];
    x1r = a[6] - a[22];
    x1i = a[7] - a[23];
    x2r = a[14] + a[30];
    x2i = a[15] + a[31];
    x3r = a[14] - a[30];
    x3i = a[15] - a[31];
    y3r = x0r + x2r;
    y3i = x0i + x2i;
    y7r = x0r - x2r;
    y7i = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    y11r = wk1i * x0r - wk1r * x0i;
    y11i = wk1i * x0i + wk1r * x0r;
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    y15r = wk1r * x0r - wk1i * x0i;
    y15i = wk1r * x0i + wk1i * x0r;
    x0r = y12r - y14r;
    x0i = y12i - y14i;
    x1r = y12r + y14r;
    x1i = y12i + y14i;
    x2r = y13r - y15r;
    x2i = y13i - y15i;
    x3r = y13r + y15r;
    x3i = y13i + y15i;
    a[24] = x0r + x2r;
    a[25] = x0i + x2i;
    a[26] = x0r - x2r;
    a[27] = x0i - x2i;
    a[28] = x1r - x3i;
    a[29] = x1i + x3r;
    a[30] = x1r + x3i;
    a[31] = x1i - x3r;
    x0r = y8r + y10r;
    x0i = y8i + y10i;
    x1r = y8r - y10r;
    x1i = y8i - y10i;
    x2r = y9r + y11r;
    x2i = y9i + y11i;
    x3r = y9r - y11r;
    x3i = y9i - y11i;
    a[16] = x0r + x2r;
    a[17] = x0i + x2i;
    a[18] = x0r - x2r;
    a[19] = x0i - x2i;
    a[20] = x1r - x3i;
    a[21] = x1i + x3r;
    a[22] = x1r + x3i;
    a[23] = x1i - x3r;
    x0r = y5r - y7i;
    x0i = y5i + y7r;
    x2r = wn4r * (x0r - x0i);
    x2i = wn4r * (x0i + x0r);
    x0r = y5r + y7i;
    x0i = y5i - y7r;
    x3r = wn4r * (x0r - x0i);
    x3i = wn4r * (x0i + x0r);
    x0r = y4r - y6i;
    x0i = y4i + y6r;
    x1r = y4r + y6i;
    x1i = y4i - y6r;
    a[8] = x0r + x2r;
    a[9] = x0i + x2i;
    a[10] = x0r - x2r;
    a[11] = x0i - x2i;
    a[12] = x1r - x3i;
    a[13] = x1i + x3r;
    a[14] = x1r + x3i;
    a[15] = x1i - x3r;
    x0r = y0r + y2r;
    x0i = y0i + y2i;
    x1r = y0r - y2r;
    x1i = y0i - y2i;
    x2r = y1r + y3r;
    x2i = y1i + y3i;
    x3r = y1r - y3r;
    x3i = y1i - y3i;
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[2] = x0r - x2r;
    a[3] = x0i - x2i;
    a[4] = x1r - x3i;
    a[5] = x1i + x3r;
    a[6] = x1r + x3i;
    a[7] = x1i - x3r;
}

void cftf162(f32 *a, f32 *w)
{
    f32 wn4r, wk1r, wk1i, wk2r, wk2i, wk3r, wk3i, 
        x0r, x0i, x1r, x1i, x2r, x2i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, 
        y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i, 
        y8r, y8i, y9r, y9i, y10r, y10i, y11r, y11i, 
        y12r, y12i, y13r, y13i, y14r, y14i, y15r, y15i;
    
    wn4r = w[1];
    wk1r = w[4];
    wk1i = w[5];
    wk3r = w[6];
    wk3i = w[7];
    wk2r = w[8];
    wk2i = w[9];
    x1r = a[0] - a[17];
    x1i = a[1] + a[16];
    x0r = a[8] - a[25];
    x0i = a[9] + a[24];
    x2r = wn4r * (x0r - x0i);
    x2i = wn4r * (x0i + x0r);
    y0r = x1r + x2r;
    y0i = x1i + x2i;
    y4r = x1r - x2r;
    y4i = x1i - x2i;
    x1r = a[0] + a[17];
    x1i = a[1] - a[16];
    x0r = a[8] + a[25];
    x0i = a[9] - a[24];
    x2r = wn4r * (x0r - x0i);
    x2i = wn4r * (x0i + x0r);
    y8r = x1r - x2i;
    y8i = x1i + x2r;
    y12r = x1r + x2i;
    y12i = x1i - x2r;
    x0r = a[2] - a[19];
    x0i = a[3] + a[18];
    x1r = wk1r * x0r - wk1i * x0i;
    x1i = wk1r * x0i + wk1i * x0r;
    x0r = a[10] - a[27];
    x0i = a[11] + a[26];
    x2r = wk3i * x0r - wk3r * x0i;
    x2i = wk3i * x0i + wk3r * x0r;
    y1r = x1r + x2r;
    y1i = x1i + x2i;
    y5r = x1r - x2r;
    y5i = x1i - x2i;
    x0r = a[2] + a[19];
    x0i = a[3] - a[18];
    x1r = wk3r * x0r - wk3i * x0i;
    x1i = wk3r * x0i + wk3i * x0r;
    x0r = a[10] + a[27];
    x0i = a[11] - a[26];
    x2r = wk1r * x0r + wk1i * x0i;
    x2i = wk1r * x0i - wk1i * x0r;
    y9r = x1r - x2r;
    y9i = x1i - x2i;
    y13r = x1r + x2r;
    y13i = x1i + x2i;
    x0r = a[4] - a[21];
    x0i = a[5] + a[20];
    x1r = wk2r * x0r - wk2i * x0i;
    x1i = wk2r * x0i + wk2i * x0r;
    x0r = a[12] - a[29];
    x0i = a[13] + a[28];
    x2r = wk2i * x0r - wk2r * x0i;
    x2i = wk2i * x0i + wk2r * x0r;
    y2r = x1r + x2r;
    y2i = x1i + x2i;
    y6r = x1r - x2r;
    y6i = x1i - x2i;
    x0r = a[4] + a[21];
    x0i = a[5] - a[20];
    x1r = wk2i * x0r - wk2r * x0i;
    x1i = wk2i * x0i + wk2r * x0r;
    x0r = a[12] + a[29];
    x0i = a[13] - a[28];
    x2r = wk2r * x0r - wk2i * x0i;
    x2i = wk2r * x0i + wk2i * x0r;
    y10r = x1r - x2r;
    y10i = x1i - x2i;
    y14r = x1r + x2r;
    y14i = x1i + x2i;
    x0r = a[6] - a[23];
    x0i = a[7] + a[22];
    x1r = wk3r * x0r - wk3i * x0i;
    x1i = wk3r * x0i + wk3i * x0r;
    x0r = a[14] - a[31];
    x0i = a[15] + a[30];
    x2r = wk1i * x0r - wk1r * x0i;
    x2i = wk1i * x0i + wk1r * x0r;
    y3r = x1r + x2r;
    y3i = x1i + x2i;
    y7r = x1r - x2r;
    y7i = x1i - x2i;
    x0r = a[6] + a[23];
    x0i = a[7] - a[22];
    x1r = wk1i * x0r + wk1r * x0i;
    x1i = wk1i * x0i - wk1r * x0r;
    x0r = a[14] + a[31];
    x0i = a[15] - a[30];
    x2r = wk3i * x0r - wk3r * x0i;
    x2i = wk3i * x0i + wk3r * x0r;
    y11r = x1r + x2r;
    y11i = x1i + x2i;
    y15r = x1r - x2r;
    y15i = x1i - x2i;
    x1r = y0r + y2r;
    x1i = y0i + y2i;
    x2r = y1r + y3r;
    x2i = y1i + y3i;
    a[0] = x1r + x2r;
    a[1] = x1i + x2i;
    a[2] = x1r - x2r;
    a[3] = x1i - x2i;
    x1r = y0r - y2r;
    x1i = y0i - y2i;
    x2r = y1r - y3r;
    x2i = y1i - y3i;
    a[4] = x1r - x2i;
    a[5] = x1i + x2r;
    a[6] = x1r + x2i;
    a[7] = x1i - x2r;
    x1r = y4r - y6i;
    x1i = y4i + y6r;
    x0r = y5r - y7i;
    x0i = y5i + y7r;
    x2r = wn4r * (x0r - x0i);
    x2i = wn4r * (x0i + x0r);
    a[8] = x1r + x2r;
    a[9] = x1i + x2i;
    a[10] = x1r - x2r;
    a[11] = x1i - x2i;
    x1r = y4r + y6i;
    x1i = y4i - y6r;
    x0r = y5r + y7i;
    x0i = y5i - y7r;
    x2r = wn4r * (x0r - x0i);
    x2i = wn4r * (x0i + x0r);
    a[12] = x1r - x2i;
    a[13] = x1i + x2r;
    a[14] = x1r + x2i;
    a[15] = x1i - x2r;
    x1r = y8r + y10r;
    x1i = y8i + y10i;
    x2r = y9r - y11r;
    x2i = y9i - y11i;
    a[16] = x1r + x2r;
    a[17] = x1i + x2i;
    a[18] = x1r - x2r;
    a[19] = x1i - x2i;
    x1r = y8r - y10r;
    x1i = y8i - y10i;
    x2r = y9r + y11r;
    x2i = y9i + y11i;
    a[20] = x1r - x2i;
    a[21] = x1i + x2r;
    a[22] = x1r + x2i;
    a[23] = x1i - x2r;
    x1r = y12r - y14i;
    x1i = y12i + y14r;
    x0r = y13r + y15i;
    x0i = y13i - y15r;
    x2r = wn4r * (x0r - x0i);
    x2i = wn4r * (x0i + x0r);
    a[24] = x1r + x2r;
    a[25] = x1i + x2i;
    a[26] = x1r - x2r;
    a[27] = x1i - x2i;
    x1r = y12r + y14i;
    x1i = y12i - y14r;
    x0r = y13r - y15i;
    x0i = y13i + y15r;
    x2r = wn4r * (x0r - x0i);
    x2i = wn4r * (x0i + x0r);
    a[28] = x1r - x2i;
    a[29] = x1i + x2r;
    a[30] = x1r + x2i;
    a[31] = x1i - x2r;
}

void cftf081(f32 *a, f32 *w)
{
    f32 wn4r, x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, 
        y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i;
    
    wn4r = w[1];
    x0r = a[0] + a[8];
    x0i = a[1] + a[9];
    x1r = a[0] - a[8];
    x1i = a[1] - a[9];
    x2r = a[4] + a[12];
    x2i = a[5] + a[13];
    x3r = a[4] - a[12];
    x3i = a[5] - a[13];
    y0r = x0r + x2r;
    y0i = x0i + x2i;
    y2r = x0r - x2r;
    y2i = x0i - x2i;
    y1r = x1r - x3i;
    y1i = x1i + x3r;
    y3r = x1r + x3i;
    y3i = x1i - x3r;
    x0r = a[2] + a[10];
    x0i = a[3] + a[11];
    x1r = a[2] - a[10];
    x1i = a[3] - a[11];
    x2r = a[6] + a[14];
    x2i = a[7] + a[15];
    x3r = a[6] - a[14];
    x3i = a[7] - a[15];
    y4r = x0r + x2r;
    y4i = x0i + x2i;
    y6r = x0r - x2r;
    y6i = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    x2r = x1r + x3i;
    x2i = x1i - x3r;
    y5r = wn4r * (x0r - x0i);
    y5i = wn4r * (x0r + x0i);
    y7r = wn4r * (x2r - x2i);
    y7i = wn4r * (x2r + x2i);
    a[8] = y1r + y5r;
    a[9] = y1i + y5i;
    a[10] = y1r - y5r;
    a[11] = y1i - y5i;
    a[12] = y3r - y7i;
    a[13] = y3i + y7r;
    a[14] = y3r + y7i;
    a[15] = y3i - y7r;
    a[0] = y0r + y4r;
    a[1] = y0i + y4i;
    a[2] = y0r - y4r;
    a[3] = y0i - y4i;
    a[4] = y2r - y6i;
    a[5] = y2i + y6r;
    a[6] = y2r + y6i;
    a[7] = y2i - y6r;
}

void cftf082(f32 *a, f32 *w)
{
    f32 wn4r, wk1r, wk1i, x0r, x0i, x1r, x1i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, 
        y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i;
    
    wn4r = w[1];
    wk1r = w[4];
    wk1i = w[5];
    y0r = a[0] - a[9];
    y0i = a[1] + a[8];
    y1r = a[0] + a[9];
    y1i = a[1] - a[8];
    x0r = a[4] - a[13];
    x0i = a[5] + a[12];
    y2r = wn4r * (x0r - x0i);
    y2i = wn4r * (x0i + x0r);
    x0r = a[4] + a[13];
    x0i = a[5] - a[12];
    y3r = wn4r * (x0r - x0i);
    y3i = wn4r * (x0i + x0r);
    x0r = a[2] - a[11];
    x0i = a[3] + a[10];
    y4r = wk1r * x0r - wk1i * x0i;
    y4i = wk1r * x0i + wk1i * x0r;
    x0r = a[2] + a[11];
    x0i = a[3] - a[10];
    y5r = wk1i * x0r - wk1r * x0i;
    y5i = wk1i * x0i + wk1r * x0r;
    x0r = a[6] - a[15];
    x0i = a[7] + a[14];
    y6r = wk1i * x0r - wk1r * x0i;
    y6i = wk1i * x0i + wk1r * x0r;
    x0r = a[6] + a[15];
    x0i = a[7] - a[14];
    y7r = wk1r * x0r - wk1i * x0i;
    y7i = wk1r * x0i + wk1i * x0r;
    x0r = y0r + y2r;
    x0i = y0i + y2i;
    x1r = y4r + y6r;
    x1i = y4i + y6i;
    a[0] = x0r + x1r;
    a[1] = x0i + x1i;
    a[2] = x0r - x1r;
    a[3] = x0i - x1i;
    x0r = y0r - y2r;
    x0i = y0i - y2i;
    x1r = y4r - y6r;
    x1i = y4i - y6i;
    a[4] = x0r - x1i;
    a[5] = x0i + x1r;
    a[6] = x0r + x1i;
    a[7] = x0i - x1r;
    x0r = y1r - y3i;
    x0i = y1i + y3r;
    x1r = y5r - y7r;
    x1i = y5i - y7i;
    a[8] = x0r + x1r;
    a[9] = x0i + x1i;
    a[10] = x0r - x1r;
    a[11] = x0i - x1i;
    x0r = y1r + y3i;
    x0i = y1i - y3r;
    x1r = y5r + y7r;
    x1i = y5i + y7i;
    a[12] = x0r - x1i;
    a[13] = x0i + x1r;
    a[14] = x0r + x1i;
    a[15] = x0i - x1r;
}

void cftf040(f32 *a) {
    f32 x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

    x0r = a[0] + a[4];
    x0i = a[1] + a[5];
    x1r = a[0] - a[4];
    x1i = a[1] - a[5];
    x2r = a[2] + a[6];
    x2i = a[3] + a[7];
    x3r = a[2] - a[6];
    x3i = a[3] - a[7];
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[4] = x0r - x2r;
    a[5] = x0i - x2i;
    a[2] = x1r - x3i;
    a[3] = x1i + x3r;
    a[6] = x1r + x3i;
    a[7] = x1i - x3r;
}

void cftb040(f32 *a) {
    f32 x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

    x0r = a[0] + a[4];
    x0i = a[1] + a[5];
    x1r = a[0] - a[4];
    x1i = a[1] - a[5];
    x2r = a[2] + a[6];
    x2i = a[3] + a[7];
    x3r = a[2] - a[6];
    x3i = a[3] - a[7];
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[4] = x0r - x2r;
    a[5] = x0i - x2i;
    a[2] = x1r + x3i;
    a[3] = x1i - x3r;
    a[6] = x1r - x3i;
    a[7] = x1i + x3r;
}

void cftx020(f32 *a) {
    f32 x0r, x0i;

    x0r = a[0] - a[2];
    x0i = a[1] - a[3];
    a[0] += a[2];
    a[1] += a[3];
    a[2] = x0r;
    a[3] = x0i;
}

void rftfsub(int n, f32 *a, int nc, f32 *c)
{
    int j, k, kk, ks, m;
    f32 wkr, wki, xr, xi, yr, yi;
    
    m = n >> 1;
    ks = 2 * nc / m;
    kk = 0;
    for (j = 2; j < m; j += 2) {
        k = n - j;
        kk += ks;
        wkr = 0.5f - c[nc - kk];
        wki = c[kk];
        xr = a[j] - a[k];
        xi = a[j + 1] + a[k + 1];
        yr = wkr * xr - wki * xi;
        yi = wkr * xi + wki * xr;
        a[j] -= yr;
        a[j + 1] -= yi;
        a[k] += yr;
        a[k + 1] -= yi;
    }
}

void rftbsub(int n, f32 *a, int nc, f32 *c)
{
    int j, k, kk, ks, m;
    f32 wkr, wki, xr, xi, yr, yi;
    
    m = n >> 1;
    ks = 2 * nc / m;
    kk = 0;
    for (j = 2; j < m; j += 2) {
        k = n - j;
        kk += ks;
        wkr = 0.5f - c[nc - kk];
        wki = c[kk];
        xr = a[j] - a[k];
        xi = a[j + 1] + a[k + 1];
        yr = wkr * xr + wki * xi;
        yi = wkr * xi - wki * xr;
        a[j] -= yr;
        a[j + 1] -= yi;
        a[k] += yr;
        a[k + 1] -= yi;
    }
}

void dctsub(int n, f32 *a, int nc, f32 *c) {
    int j, k, kk, ks, m;
    f32 wkr, wki, xr;

    m = n >> 1;
    ks = nc / n;
    kk = 0;
    for (j = 1; j < m; j++) {
        k = n - j;
        kk += ks;
        wkr = c[kk] - c[nc - kk];
        wki = c[kk] + c[nc - kk];
        xr = wki * a[j] - wkr * a[k];
        a[j] = wkr * a[j] + wki * a[k];
        a[k] = xr;
    }
    a[m] *= c[0];
}

#define BINK_A1 2896   // 1/sqrt(2) << 12
#define BINK_A2 2217
#define BINK_A3 3784
#define BINK_A4 -5352
#define BINK_MUL(x, y) (((x) * (y)) >> 11)

// Dequantizes and inverse-transforms an 8x8 block of coefficients (columns, then rows) into 8-bit pixels at dst.
// This is the AAN fast IDCT as laid out in libjpeg's jidctfst.c (same work variables, reused; the BINK_A
// constants are its FIX_ constants at 11 bits). The plain OR chain below is what gives retail's OR tree.
void fastidct8x8(u8* dst, s32 stride, s16* block, s32* quant) {
    s32 tmp[64];
    s32* t = tmp;
    s32 i;
    s32 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    s32 tmp10, tmp11, tmp12, tmp13;
    s32 z5, z10, z11, z12, z13;

    // Pass 1: columns, from the coefficient block into the work array.
    for (i = 0; i < 8; i++) {
        if ((block[8] | block[16] | block[24] | block[32] | block[40] | block[48] | block[56]) == 0) {
            // All AC terms are zero: the column is the dequantized DC value.
            s32 dc = (block[0] * quant[0]) >> 11;
            block++;
            quant++;
            t[0] = dc;
            t[8] = dc;
            t[16] = dc;
            t[24] = dc;
            t[32] = dc;
            t[40] = dc;
            t[48] = dc;
            t[56] = dc;
            t++;
        } else {
            // Even part
            tmp0 = (block[0] * quant[0]) >> 11;
            tmp1 = (block[16] * quant[16]) >> 11;
            tmp2 = (block[32] * quant[32]) >> 11;
            tmp3 = (block[48] * quant[48]) >> 11;
            tmp10 = tmp0 + tmp2;
            tmp11 = tmp0 - tmp2;
            tmp13 = tmp1 + tmp3;
            tmp12 = BINK_MUL(BINK_A1, tmp1 - tmp3) - tmp13;
            tmp0 = tmp10 + tmp13;
            tmp3 = tmp10 - tmp13;
            tmp1 = tmp11 + tmp12;
            tmp2 = tmp11 - tmp12;

            // Odd part
            tmp4 = (block[8] * quant[8]) >> 11;
            tmp5 = (block[24] * quant[24]) >> 11;
            tmp6 = (block[40] * quant[40]) >> 11;
            tmp7 = (block[56] * quant[56]) >> 11;
            z13 = tmp6 + tmp5;
            z10 = tmp6 - tmp5;
            z11 = tmp4 + tmp7;
            z12 = tmp4 - tmp7;
            tmp7 = z11 + z13;
            tmp11 = BINK_MUL(BINK_A1, z11 - z13);
            z5 = BINK_MUL(BINK_A3, z10 + z12);
            tmp10 = BINK_MUL(BINK_A2, z12) - z5;
            tmp12 = z5 + BINK_MUL(BINK_A4, z10);
            tmp6 = tmp12 - tmp7;
            tmp5 = tmp11 - tmp6;
            tmp4 = tmp10 + tmp5;

            block++;
            quant++;
            t[0] = tmp0 + tmp7;
            t[56] = tmp0 - tmp7;
            t[8] = tmp1 + tmp6;
            t[48] = tmp1 - tmp6;
            t[16] = tmp2 + tmp5;
            t[40] = tmp2 - tmp5;
            t[32] = tmp3 + tmp4;
            t[24] = tmp3 - tmp4;
            t++;
        }
    }

    // Pass 2: rows, from the work array to pixels.
    t = tmp;
    for (i = 0; i < 8; i++) {
        // Even part
        tmp10 = t[0] + t[4];
        tmp11 = t[0] - t[4];
        tmp13 = t[2] + t[6];
        tmp12 = BINK_MUL(BINK_A1, t[2] - t[6]) - tmp13;
        tmp0 = tmp10 + tmp13;
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        // Odd part
        z13 = t[5] + t[3];
        z10 = t[5] - t[3];
        z11 = t[1] + t[7];
        z12 = t[1] - t[7];
        tmp7 = z11 + z13;
        tmp11 = BINK_MUL(BINK_A1, z11 - z13);
        z5 = BINK_MUL(BINK_A3, z10 + z12);
        tmp10 = BINK_MUL(BINK_A2, z12) - z5;
        tmp12 = z5 + BINK_MUL(BINK_A4, z10);
        tmp6 = tmp12 - tmp7;
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        dst[0] = (tmp0 + tmp7 + 0x7F) >> 8;
        dst[7] = (tmp0 - tmp7 + 0x7F) >> 8;
        dst[1] = (tmp1 + tmp6 + 0x7F) >> 8;
        dst[6] = (tmp1 - tmp6 + 0x7F) >> 8;
        dst[2] = (tmp2 + tmp5 + 0x7F) >> 8;
        dst[5] = (tmp2 - tmp5 + 0x7F) >> 8;
        dst[4] = (tmp3 + tmp4 + 0x7F) >> 8;
        dst[3] = (tmp3 - tmp4 + 0x7F) >> 8;
        t += 8;
        dst += stride;
    }
}

// Like fastidct8x8, but draws the block at double size (16x16) into dst.
void fastidct8x8d(u8 *dst, s32 stride, s16 *block, s32 *quant) {
    s32 tmp[64];
    s32 *t;
    s32 i;
    u8 *dst2;
    s32 stride2;
    s32 tmp0;
    s32 tmp1;
    s32 tmp2;
    s32 tmp3;
    s32 tmp4;
    s32 tmp5;
    s32 tmp6;
    s32 tmp7;
    s32 tmp10;
    s32 tmp11;
    s32 tmp12;
    s32 tmp13;
    s32 z5;
    s32 z10;
    s32 z11;
    s32 z12;
    s32 z13;
    dst2 = dst + stride;
    stride2 = stride * 2;
    t = tmp;

    for (i = 0; i < 8; i++) {
        if ((block[8] | block[16] | block[24] | block[32] | block[40] | block[48] | block[56]) == 0) {
            s32 dc = (block[0] * quant[0]) >> 11;
            block++;
            quant++;
            t[0] = dc;
            t[8] = dc;
            t[16] = dc;
            t[24] = dc;
            t[32] = dc;
            t[40] = dc;
            t[48] = dc;
            t[56] = dc;
            t++;
        } else {
            tmp0 = (block[0] * quant[0]) >> 11;
            tmp1 = (block[16] * quant[16]) >> 11;
            tmp2 = (block[32] * quant[32]) >> 11;
            tmp3 = (block[48] * quant[48]) >> 11;
            tmp10 = tmp0 + tmp2;
            tmp11 = tmp0 - tmp2;
            tmp13 = tmp1 + tmp3;
            tmp12 = BINK_MUL(BINK_A1, tmp1 - tmp3) - tmp13;
            tmp0 = tmp10 + tmp13;
            tmp3 = tmp10 - tmp13;
            tmp1 = tmp11 + tmp12;
            tmp2 = tmp11 - tmp12;
            tmp4 = (block[8] * quant[8]) >> 11;
            tmp5 = (block[24] * quant[24]) >> 11;
            tmp6 = (block[40] * quant[40]) >> 11;
            tmp7 = (block[56] * quant[56]) >> 11;
            z13 = tmp6 + tmp5;
            z10 = tmp6 - tmp5;
            z11 = tmp4 + tmp7;
            z12 = tmp4 - tmp7;
            tmp7 = z11 + z13;
            tmp11 = BINK_MUL(BINK_A1, z11 - z13);
            z5 = BINK_MUL(BINK_A3, z10 + z12);
            tmp10 = BINK_MUL(BINK_A2, z12) - z5;
            tmp12 = z5 + BINK_MUL(BINK_A4, z10);
            tmp6 = tmp12 - tmp7;
            tmp5 = tmp11 - tmp6;
            tmp4 = tmp10 + tmp5;
            block++;
            quant++;
            t[0] = tmp0 + tmp7;
            t[56] = tmp0 - tmp7;
            t[8] = tmp1 + tmp6;
            t[48] = tmp1 - tmp6;
            t[16] = tmp2 + tmp5;
            t[40] = tmp2 - tmp5;
            t[32] = tmp3 + tmp4;
            t[24] = tmp3 - tmp4;
            t++;
        }
    }
    // Pass 2: rows. Each output pixel is written twice across and the row twice down.
    t = tmp;
    for (i = 0; i < 8; i++) {
        u32 o0, o1, o2, o3, o4;
        u32 o5, o6, o7;
        u32 w0, w1, w2, w3;

        // Even part
        tmp10 = t[0] + t[4];
        tmp11 = t[0] - t[4];
        tmp13 = t[2] + t[6];
        tmp12 = BINK_MUL(BINK_A1, t[2] - t[6]) - tmp13;
        tmp0 = tmp10 + tmp13;
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        // Odd part
        z13 = t[5] + t[3];
        z10 = t[5] - t[3];
        z11 = t[1] + t[7];
        z12 = t[1] - t[7];
        tmp7 = z11 + z13;
        tmp11 = BINK_MUL(BINK_A1, z11 - z13);
        z5 = BINK_MUL(BINK_A3, z10 + z12);
        tmp10 = BINK_MUL(BINK_A2, z12) - z5;
        tmp12 = z5 + BINK_MUL(BINK_A4, z10);
        tmp6 = tmp12 - tmp7;
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        // Pixel packing: not the original statements yet (85.72). Retail computes the shift for pixels 0 and 2,
        // computes-and-drops it for 1, 3, 4, 6 and has none for 5 and 7, so o5..o7 are kept unshifted here
        // (see handoff/FACTS.md 2026-11-15).
        o0 = (tmp0 + tmp7 + 0x7F) >> 8;
        o7 = tmp0 - tmp7 + 0x7F;
        o1 = (tmp1 + tmp6 + 0x7F) >> 8;
        o6 = tmp1 - tmp6 + 0x7F;
        o2 = (tmp2 + tmp5 + 0x7F) >> 8;
        o5 = tmp2 - tmp5 + 0x7F;
        o4 = (tmp3 + tmp4 + 0x7F) >> 8;
        o3 = (tmp3 - tmp4 + 0x7F) >> 8;
        w0 = ((o0 & 0xFF) << 16) | (o1 & 0xFF);
        w1 = ((o2 & 0xFF) << 16) | (o3 & 0xFF);
        w2 = ((o5 >> 8) & 0xFF) | ((o4 & 0xFF) << 16);
        w3 = ((o7 >> 8) & 0xFF) | (((o6 >> 8) & 0xFF) << 16);
        w0 |= w0 << 8;
        w1 |= w1 << 8;
        w2 |= w2 << 8;
        w3 |= w3 << 8;
        ((u32 *)dst)[0] = w0;
        ((u32 *)dst)[1] = w1;
        ((u32 *)dst)[2] = w2;
        ((u32 *)dst)[3] = w3;
        dst += stride2;
        ((u32 *)dst2)[0] = w0;
        ((u32 *)dst2)[1] = w1;
        ((u32 *)dst2)[2] = w2;
        ((u32 *)dst2)[3] = w3;
        dst2 += stride2;
        t += 8;
    }
}

void fn_80125CA0(void *arg0, void *arg1, void *arg2, s32 arg3) {
    fastidct8x8(arg0, (s32)arg1, arg2, (s32 *)((u8 *)&lbl_802F6A40 + (arg3 << 8)));
}

void fn_80125CB4(void *arg0, void *arg1, void *arg2, s32 arg3) {
    fastidct8x8d(arg0, (s32)arg1, arg2, (s32 *)((u8 *)&lbl_802F6A40 + (arg3 << 8)));
}

extern const s32 lbl_802F7A40[];
// Dequantizes (inter table `q`) and inverse-transforms a block, adding the result to the motion source src.
#pragma push
#pragma global_optimizer off  /* same code as the three switches it replaces (lifetimes, propagation, common_subs off); see ExpandPlane */
/* The AAN fast IDCT in the libjpeg jidctfst.c layout: the same work variables serve both passes and are
 * assigned again (tmp10..tmp12 hold the even sums first and the odd products later). RAD built it without
 * lifetime splitting, copy propagation or common-subexpression elimination, so every variable keeps one
 * register for the whole routine (Process review, 2026-11-16). */
void FastmIDCT8x8WithMotion(u8 *dst, s32 stride, s16 *block, s32 q, u8 *src, s32 srcStride) {
    s32 tmp[64];
    s32 i;
    s32 *t;
    s32 *quant;
    s32 s1, s2, s3, s4, s5, s6, s7;
    s32 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    s32 tmp10, tmp11, tmp12, tmp13;
    s32 z5, z10, z11, z12, z13;

    quant = (s32 *)((u8 *)lbl_802F7A40 + (q << 8));
    t = tmp;

    // Pass 1: columns, from the coefficient block into the work array.
    for (i = 8; i != 0; --i) {
        s6 = block[48];
        s5 = block[40];
        s4 = block[32];
        s3 = block[24];
        s7 = block[56];
        s1 = block[8];
        s2 = block[16];
        // The cast keeps CodeWarrior from reassociating the zero-test ORs.
        if (((s7 | (s32)((u32)s6 | (u32)s5)) | ((s4 | s3) | (s1 | s2))) == 0) {
            // All AC terms are zero: the column is the dequantized DC value.
            s32 dc = (block[0] * quant[0]) >> 11;
            block++;
            quant++;
            t[0] = dc;
            t[8] = dc;
            t[16] = dc;
            t[24] = dc;
            t[32] = dc;
            t[40] = dc;
            t[48] = dc;
            t[56] = dc;
            t++;
        } else {
            s32 s0 = block[0];

            // Even part
            tmp0 = (s0 * quant[0]) >> 11;
            tmp1 = (s2 * quant[16]) >> 11;
            tmp2 = (s4 * quant[32]) >> 11;
            tmp3 = (s6 * quant[48]) >> 11;
            tmp10 = tmp0 + tmp2;
            tmp11 = tmp0 - tmp2;
            tmp13 = tmp1 + tmp3;
            tmp12 = BINK_MUL(BINK_A1, tmp1 - tmp3) - tmp13;
            tmp0 = tmp10 + tmp13;
            tmp3 = tmp10 - tmp13;
            tmp1 = tmp11 + tmp12;
            tmp2 = tmp11 - tmp12;

            // Odd part
            tmp4 = (s1 * quant[8]) >> 11;
            tmp5 = (s3 * quant[24]) >> 11;
            tmp6 = (s5 * quant[40]) >> 11;
            tmp7 = (s7 * quant[56]) >> 11;
            z13 = tmp6 + tmp5;
            z10 = tmp6 - tmp5;
            z11 = tmp4 + tmp7;
            z12 = tmp4 - tmp7;
            tmp7 = z11 + z13;
            tmp11 = BINK_MUL(BINK_A1, z11 - z13);
            z5 = BINK_MUL(BINK_A3, z10 + z12);
            tmp10 = BINK_MUL(BINK_A2, z12) - z5;
            tmp12 = z5 + BINK_MUL(BINK_A4, z10);
            tmp6 = tmp12 - tmp7;
            tmp5 = tmp11 - tmp6;
            tmp4 = tmp10 + tmp5;

            block++;
            quant++;
            t[0] = tmp0 + tmp7;
            t[56] = tmp0 - tmp7;
            t[8] = tmp1 + tmp6;
            t[48] = tmp1 - tmp6;
            t[16] = tmp2 + tmp5;
            t[40] = tmp2 - tmp5;
            t[32] = tmp3 + tmp4;
            t[24] = tmp3 - tmp4;
            t++;
        }
    }

    // Pass 2: rows, from the work array onto the motion-compensated source.
    t = tmp;
    for (i = 0; i < 8; i++) {
        // Even part
        tmp10 = t[0] + t[4];
        tmp11 = t[0] - t[4];
        tmp13 = t[2] + t[6];
        tmp12 = BINK_MUL(BINK_A1, t[2] - t[6]) - tmp13;
        tmp0 = tmp10 + tmp13;
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        // Odd part
        z13 = t[5] + t[3];
        z10 = t[5] - t[3];
        z11 = t[1] + t[7];
        z12 = t[1] - t[7];
        tmp7 = z11 + z13;
        tmp11 = BINK_MUL(BINK_A1, z11 - z13);
        z5 = BINK_MUL(BINK_A3, z10 + z12);
        tmp10 = BINK_MUL(BINK_A2, z12) - z5;
        tmp12 = z5 + BINK_MUL(BINK_A4, z10);
        tmp6 = tmp12 - tmp7;
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        dst[0] = src[0] + ((tmp0 + tmp7 + 0x7F) >> 8);
        dst[7] = src[7] + ((tmp0 - tmp7 + 0x7F) >> 8);
        dst[1] = src[1] + ((tmp1 + tmp6 + 0x7F) >> 8);
        dst[6] = src[6] + ((tmp1 - tmp6 + 0x7F) >> 8);
        dst[2] = src[2] + ((tmp2 + tmp5 + 0x7F) >> 8);
        dst[5] = src[5] + ((tmp2 - tmp5 + 0x7F) >> 8);
        dst[3] = src[3] + ((tmp3 - tmp4 + 0x7F) >> 8);
        dst[4] = src[4] + ((tmp3 + tmp4 + 0x7F) >> 8);
        src += srcStride;
        dst += stride;
        t += 8;
    }
}
#pragma pop

// Reads the AC coefficients of an 8x8 DCT block (block[0], the DC, is set by the caller) bit plane by bit plane,
// using the same coefficient list scheme as the residue reader, then stores them in natural order.
void ReadBPLossless(s16 *block, BitReader8011 *bits) {
    u32 planes;
    u32 width;
    u8 *start;
    u32 bit;
    u32 esc;
    u8 *end;
    u8 *p;
    s32 mask;
    u32 *next;
    s32 val;
    s32 i;
    u32 value;
    u32 avail;
    s32 sign;
    s32 low;
    u32 e;
    u32 v;
    u8 list[144];
    s16 tmp[64] = {0};

    avail = bits->count;
    next = bits->next;
    value = bits->value;
    BINK_GETBITS(planes, 4);
    start = &list[68];
    start[0] = (4 << 2) + 0;
    start[1] = (24 << 2) + 0;
    start[2] = (44 << 2) + 0;
    start[3] = (1 << 2) + 3;
    start[4] = (2 << 2) + 3;
    start[5] = (3 << 2) + 3;
    end = start + 6;
    mask = 1 << (planes - 1);
    for (width = planes; width > 1; width--) {
        sign = 1 << (width - 1);
        low = sign - 1;
        p = start;
        while (p < end) {
            e = *p;
            if (e != 0) {
                BINK_GETBIT(bit);
                if (bit & 1) {
                    switch (e & 3) {
                    case 0:
                        e = e >> 2;
                        *p = ((e + 4) << 2) + 1;
                        goto group4;
                    case 1:
                        e = e >> 2;
                        *p = (e << 2) + 2;
                        *end++ = ((e + 4) << 2) + 2;
                        *end++ = ((e + 8) << 2) + 2;
                        *end++ = ((e + 12) << 2) + 2;
                        continue;
                    case 2:
                        *p = 0;
                        e = e >> 2;
                        p++;
                    group4:
                        BINK_GETESC(esc, v, width);
                        if (esc) {
                            *--start = ((e) << 2) + 3;
                        } else {
                            val = mask | (v & low);
                            val = v & sign ? -val : val;
                            tmp[e] = val;
                        }
                        BINK_GETESC(esc, v, width);
                        if (esc) {
                            *--start = ((e + 1) << 2) + 3;
                        } else {
                            val = mask | (v & low);
                            val = v & sign ? -val : val;
                            tmp[e + 1] = val;
                        }
                        BINK_GETESC(esc, v, width);
                        if (esc) {
                            *--start = ((e + 2) << 2) + 3;
                        } else {
                            val = mask | (v & low);
                            val = v & sign ? -val : val;
                            tmp[e + 2] = val;
                        }
                        BINK_GETESC(esc, v, width);
                        if (esc) {
                            *--start = ((e + 3) << 2) + 3;
                        } else {
                            val = mask | (v & low);
                            val = v & sign ? -val : val;
                            tmp[e + 3] = val;
                        }
                        continue;
                    case 3:
                        BINK_GETBITS(v, width);
                        val = mask | (v & low);
                        val = v & sign ? -val : val;
                        *p = 0;
                        tmp[e >> 2] = val;
                        break;
                    }
                }
            }
            p++;
        }
        mask >>= 1;
    }
    if (planes != 0) {
        p = start;
        while (p < end) {
            e = *p;
            if (e != 0) {
                BINK_GETBIT(bit);
                if (bit & 1) {
                    switch (e & 3) {
                    case 0:
                        e = e >> 2;
                        *p = ((e + 4) << 2) + 1;
                        goto group4s;
                    case 1:
                        e = e >> 2;
                        *p = (e << 2) + 2;
                        *end++ = ((e + 4) << 2) + 2;
                        *end++ = ((e + 8) << 2) + 2;
                        *end++ = ((e + 12) << 2) + 2;
                        continue;
                    case 2:
                        *p = 0;
                        e = e >> 2;
                        p++;
                    group4s:
                        BINK_GETBIT(bit);
                        if (bit & 1) {
                            *--start = ((e) << 2) + 3;
                        } else {
                            BINK_GETBIT(bit);
                            tmp[e] = -(bit & 1) | 1;
                        }
                        BINK_GETBIT(bit);
                        if (bit & 1) {
                            *--start = ((e + 1) << 2) + 3;
                        } else {
                            BINK_GETBIT(bit);
                            tmp[e + 1] = -(bit & 1) | 1;
                        }
                        BINK_GETBIT(bit);
                        if (bit & 1) {
                            *--start = ((e + 2) << 2) + 3;
                        } else {
                            BINK_GETBIT(bit);
                            tmp[e + 2] = -(bit & 1) | 1;
                        }
                        BINK_GETBIT(bit);
                        if (bit & 1) {
                            *--start = ((e + 3) << 2) + 3;
                        } else {
                            BINK_GETBIT(bit);
                            tmp[e + 3] = -(bit & 1) | 1;
                        }
                        continue;
                    case 3:
                        BINK_GETBIT(bit);
                        *p = 0;
                        tmp[e >> 2] = -(bit & 1) | 1;
                        break;
                    }
                }
            }
            p++;
        }
    }
    bits->next = next;
    bits->value = value;
    bits->count = avail;

    block[1] = tmp[1];
    ((u32 *)block)[1] = ((u32 *)tmp)[2];
    ((u32 *)block)[2] = ((u32 *)tmp)[4];
    ((u32 *)block)[3] = ((u32 *)tmp)[6];
    ((u32 *)block)[4] = ((u32 *)tmp)[1];
    ((u32 *)block)[5] = ((u32 *)tmp)[3];
    ((u32 *)block)[6] = ((u32 *)tmp)[5];
    ((u32 *)block)[7] = ((u32 *)tmp)[7];
    ((u32 *)block)[8] = ((u32 *)tmp)[12];
    ((u32 *)block)[9] = ((u32 *)tmp)[22];
    ((u32 *)block)[10] = ((u32 *)tmp)[8];
    ((u32 *)block)[11] = ((u32 *)tmp)[10];
    ((u32 *)block)[12] = ((u32 *)tmp)[13];
    ((u32 *)block)[13] = ((u32 *)tmp)[23];
    ((u32 *)block)[14] = ((u32 *)tmp)[9];
    ((u32 *)block)[15] = ((u32 *)tmp)[11];
    ((u32 *)block)[16] = ((u32 *)tmp)[14];
    ((u32 *)block)[17] = ((u32 *)tmp)[16];
    ((u32 *)block)[18] = ((u32 *)tmp)[24];
    ((u32 *)block)[19] = ((u32 *)tmp)[26];
    ((u32 *)block)[20] = ((u32 *)tmp)[15];
    ((u32 *)block)[21] = ((u32 *)tmp)[17];
    ((u32 *)block)[22] = ((u32 *)tmp)[25];
    ((u32 *)block)[23] = ((u32 *)tmp)[27];
    ((u32 *)block)[24] = ((u32 *)tmp)[18];
    ((u32 *)block)[25] = ((u32 *)tmp)[20];
    ((u32 *)block)[26] = ((u32 *)tmp)[28];
    ((u32 *)block)[27] = ((u32 *)tmp)[30];
    ((u32 *)block)[28] = ((u32 *)tmp)[19];
    ((u32 *)block)[29] = ((u32 *)tmp)[21];
    ((u32 *)block)[30] = ((u32 *)tmp)[29];
    ((u32 *)block)[31] = ((u32 *)tmp)[31];
}

// Reads the residue of an 8x8 block into `block` (signed bytes), bit plane by bit plane, stopping after
// `limit` + 1 values. Each list entry is coefficient << 2 | mode: 0 = group of 16 untouched, 1 = group of 16 to
// split, 2 = group of 4, 3 = single coefficient.
void readlossy(s8* block, BitReader8011* bits, u32 limit) {
    s32 i;
    s32 planes;
    s32 nm;
    s32 k;
    u8* start;
    u32 bit;
    u8* end;
    u8* p;
    s32 mask;
    s32 nzCount;
    u32 n = 0;
    u8* nz_p;
    u32* next;
    u32 value;
    u32 avail;
    u32 e;
    u8 nz[64];
    u8 list[128];
    avail = bits->count;
    next = bits->next;
    value = bits->value;
    if (avail < 3) {
        u32 word = *next;
        next++;
        planes = (value | (word << avail)) & 7;
        value = word >> (3 - avail);
        avail += 29;
    } else {
        planes = value & 7;
        value >>= 3;
        avail -= 3;
    }
    start = &list[68];
    start[0] = 4 << 2;
    start[1] = 24 << 2;
    start[2] = 44 << 2;
    start[3] = (0 << 2) | 2;
    end = start + 4;
    mask = 1 << planes;
    nzCount = 0;
    for (k = planes + 1; k != 0; k--) {
        nm = -mask;
        nz_p = nz;
        for (i = 0; i < nzCount; i++) {
            if (avail == 0) {
                bit = *next;
                avail = 31;
                next++;
                value = bit >> 1;
            } else {
                bit = value;
                value >>= 1;
                avail--;
            }
            ;
            if (bit & 1) {
                block[*nz_p] += (block[*nz_p] < 0) ? nm : (mask);
                if ((n++) == limit) {
                    goto done;
                }
            }
            nz_p++;
        }

        p = start;
        while (p < end) {
            e = *p;
            if (e != 0) {
                if (avail == 0) {
                    bit = *next;
                    avail = 31;
                    next++;
                    value = bit >> 1;
                } else {
                    bit = value;
                    value >>= 1;
                    avail--;
                }
                ;
                if (bit & 1) {
                    switch (e & 3) {
                        case 0:
                            e = e >> 2;
                            *p = ((e + 4) << 2) + 1;
                            goto group4;

                        case 1:
                            e = e >> 2;
                            *p = (e << 2) + 2;
                            *end++ = ((e + 4) << 2) + 2;
                            *end++ = ((e + 8) << 2) + 2;
                            *end++ = ((e + 12) << 2) + 2;
                            continue;

                        case 2:
                            *p = 0;
                            e = e >> 2;
                            p++;
                            group4:
                        if (avail == 0) {
                            bit = *next;
                            avail = 31;
                            next++;
                            value = bit >> 1;
                        } else {
                            bit = value;
                            value >>= 1;
                            avail--;
                        }

                            ;
                            if (bit & 1) {
                            *(--start) = (e << 2) + 3;
                        } else {
                            nz[nzCount++] = e;
                            if (avail == 0) {
                                bit = *next;
                                avail = 31;
                                next++;
                                value = bit >> 1;
                            } else {
                                bit = value;
                                value >>= 1;
                                avail--;
                            }
                            ;
                            block[e] = (bit & 1) ? nm : (mask);
                            if ((n++) == limit) {
                                goto done;
                            }
                        }
                            if (avail == 0) {
                            bit = *next;
                            avail = 31;
                            next++;
                            value = bit >> 1;
                        } else {
                            bit = value;
                            value >>= 1;
                            avail--;
                        }
                            ;
                            if (bit & 1) {
                            *(--start) = ((e + 1) << 2) + 3;
                        } else {
                            nz[nzCount++] = e + 1;
                            if (avail == 0) {
                                bit = *next;
                                avail = 31;
                                next++;
                                value = bit >> 1;
                            } else {
                                bit = value;
                                value >>= 1;
                                avail--;
                            }
                            ;
                            block[e + 1] = (bit & 1) ? nm : (mask);
                            if ((n++) == limit) {
                                goto done;
                            }
                        }
                            if (avail == 0) {
                            bit = *next;
                            avail = 31;
                            next++;
                            value = bit >> 1;
                        } else {
                            bit = value;
                            value >>= 1;
                            avail--;
                        }
                            ;
                            if (bit & 1) {
                            *(--start) = ((e + 2) << 2) + 3;
                        } else {
                            nz[nzCount++] = e + 2;
                            if (avail == 0) {
                                bit = *next;
                                avail = 31;
                                next++;
                                value = bit >> 1;
                            } else {
                                bit = value;
                                value >>= 1;
                                avail--;
                            }
                            ;
                            block[e + 2] = (bit & 1) ? nm : (mask);
                            if ((n++) == limit) {
                                goto done;
                            }
                        }
                            if (avail == 0) {
                            bit = *next;
                            avail = 31;
                            next++;
                            value = bit >> 1;
                        } else {
                            bit = value;
                            value >>= 1;
                            avail--;
                        }
                            ;
                            if (bit & 1) {
                            *(--start) = ((e + 3) << 2) + 3;
                        } else {
                            nz[nzCount++] = e + 3;
                            if (avail == 0) {
                                bit = *next;
                                avail = 31;
                                next++;
                                value = bit >> 1;
                            } else {
                                bit = value;
                                value >>= 1;
                                avail--;
                            }
                            ;
                            block[e + 3] = (bit & 1) ? nm : (mask);
                            if ((n++) != limit) {
                                continue;
                            }
                            goto done;
                        }
                            continue;

                        case 3:
                            e = e >> 2;
                            nz[nzCount++] = e;
                            if (avail == 0) {
                            bit = *next;
                            avail = 31;
                            next++;
                            value = bit >> 1;
                        } else {
                            bit = value;
                            value >>= 1;
                            avail--;
                        }
                            ;
                            block[e] = (bit & 1) ? nm : (mask);
                            if ((n++) == limit) {
                            goto done;
                        }
                            *p = 0;
                            break;

                    }

                }
            }
            p++;
        }

        mask >>= 1;
    }

    done:
    bits->next = next;

    bits->value = value;
    bits->count = avail;
}

// Adds a residue read from the bitstream to an 8x8 block: dst = src + residue (the residue is stored in Bink's
// coefficient order, 2x2 groups within 4x4 quadrants).
void ReadBPLossyWithMotion(u8 *dst, s32 dstStride, BitReader8011 *bits, u32 limit, u8 *src, s32 srcStride) {
    s8 block[64] = {0};
    u8 *row = dst;
    u8 *ref = src;

    readlossy(block, bits, limit);
    row[0] = block[0] + ref[0];
    row[1] = block[1] + ref[1];
    row[2] = block[4] + ref[2];
    row[3] = block[5] + ref[3];
    row[4] = block[8] + ref[4];
    row[5] = block[9] + ref[5];
    row[6] = block[12] + ref[6];
    row[7] = block[13] + ref[7];
    row += dstStride;
    ref += srcStride;
    row[0] = block[2] + ref[0];
    row[1] = block[3] + ref[1];
    row[2] = block[6] + ref[2];
    row[3] = block[7] + ref[3];
    row[4] = block[10] + ref[4];
    row[5] = block[11] + ref[5];
    row[6] = block[14] + ref[6];
    row[7] = block[15] + ref[7];
    row += dstStride;
    ref += srcStride;
    row[0] = block[24] + ref[0];
    row[1] = block[25] + ref[1];
    row[2] = block[44] + ref[2];
    row[3] = block[45] + ref[3];
    row[4] = block[16] + ref[4];
    row[5] = block[17] + ref[5];
    row[6] = block[20] + ref[6];
    row[7] = block[21] + ref[7];
    row += dstStride;
    ref += srcStride;
    row[0] = block[26] + ref[0];
    row[1] = block[27] + ref[1];
    row[2] = block[46] + ref[2];
    row[3] = block[47] + ref[3];
    row[4] = block[18] + ref[4];
    row[5] = block[19] + ref[5];
    row[6] = block[22] + ref[6];
    row[7] = block[23] + ref[7];
    row += dstStride;
    ref += srcStride;
    row[0] = block[28] + ref[0];
    row[1] = block[29] + ref[1];
    row[2] = block[32] + ref[2];
    row[3] = block[33] + ref[3];
    row[4] = block[48] + ref[4];
    row[5] = block[49] + ref[5];
    row[6] = block[52] + ref[6];
    row[7] = block[53] + ref[7];
    row += dstStride;
    ref += srcStride;
    row[0] = block[30] + ref[0];
    row[1] = block[31] + ref[1];
    row[2] = block[34] + ref[2];
    row[3] = block[35] + ref[3];
    row[4] = block[50] + ref[4];
    row[5] = block[51] + ref[5];
    row[6] = block[54] + ref[6];
    row[7] = block[55] + ref[7];
    row += dstStride;
    ref += srcStride;
    row[0] = block[36] + ref[0];
    row[1] = block[37] + ref[1];
    row[2] = block[40] + ref[2];
    row[3] = block[41] + ref[3];
    row[4] = block[56] + ref[4];
    row[5] = block[57] + ref[5];
    row[6] = block[60] + ref[6];
    row[7] = block[61] + ref[7];
    row += dstStride;
    ref += srcStride;
    row[0] = block[38] + ref[0];
    row[1] = block[39] + ref[1];
    row[2] = block[42] + ref[2];
    row[3] = block[43] + ref[3];
    row[4] = block[58] + ref[4];
    row[5] = block[59] + ref[5];
    row[6] = block[62] + ref[6];
    row[7] = block[63] + ref[7];
}

extern f64 pow(f32, f64);
extern void set_rate_volume_and_pan(void *);
extern void clear(void *, u32, u32);
extern void set_both_state(void *, s32);
extern void set_addrs(void *, s32);

/* .sdata owned by this unit (0x80559938-0x80559954), in address order. */
extern u8 lbl_8041D628[0x80];
extern u8 lbl_8041D6A8[0x80];
s32 lbl_80559938 = -1;
s32 lbl_8055993C = -1;
s32 lbl_80559940 = -1;
s32 lbl_80559944 = 1;
void ***lbl_80559948 = (void ***)lbl_8041D6A8;
u32 *lbl_8055994C = (u32 *)lbl_8041D628;
u32 lbl_80559950 = 0x20;

/* .rodata tables owned by this unit (0x802F6528-0x802F8E40), in address order. */
// DUMP_DATA lbl_802F6528 char
const char lbl_802F6528[152] = {
#include "dumpdata/lbl_802F6528.inc"
};
// Huffman lookup table 0: bytes (a u32 initialiser would byte-reverse on a little-endian host).
// DUMP_DATA lbl_802F65C0 u8
const u8 lbl_802F65C0[16] = {
#include "dumpdata/lbl_802F65C0.inc"
};
// DUMP_DATA lbl_802F65D0 u8
const u8 lbl_802F65D0[32] = {
#include "dumpdata/lbl_802F65D0.inc"
};
// DUMP_DATA lbl_802F65F0 u8
const u8 lbl_802F65F0[32] = {
#include "dumpdata/lbl_802F65F0.inc"
};
// DUMP_DATA lbl_802F6610 u8
const u8 lbl_802F6610[32] = {
#include "dumpdata/lbl_802F6610.inc"
};
// DUMP_DATA lbl_802F6630 u8
const u8 lbl_802F6630[32] = {
#include "dumpdata/lbl_802F6630.inc"
};
// DUMP_DATA lbl_802F6650 u8
const u8 lbl_802F6650[32] = {
#include "dumpdata/lbl_802F6650.inc"
};
// DUMP_DATA lbl_802F6670 u8
const u8 lbl_802F6670[32] = {
#include "dumpdata/lbl_802F6670.inc"
};
// DUMP_DATA lbl_802F6690 u8
const u8 lbl_802F6690[64] = {
#include "dumpdata/lbl_802F6690.inc"
};
// DUMP_DATA lbl_802F66D0 u8
const u8 lbl_802F66D0[64] = {
#include "dumpdata/lbl_802F66D0.inc"
};
// DUMP_DATA lbl_802F6710 u8
const u8 lbl_802F6710[64] = {
#include "dumpdata/lbl_802F6710.inc"
};
// DUMP_DATA lbl_802F6750 u8
const u8 lbl_802F6750[64] = {
#include "dumpdata/lbl_802F6750.inc"
};
// DUMP_DATA lbl_802F6790 u8
const u8 lbl_802F6790[64] = {
#include "dumpdata/lbl_802F6790.inc"
};
// DUMP_DATA lbl_802F67D0 u8
const u8 lbl_802F67D0[64] = {
#include "dumpdata/lbl_802F67D0.inc"
};
// DUMP_DATA lbl_802F6810 u8
const u8 lbl_802F6810[128] = {
#include "dumpdata/lbl_802F6810.inc"
};
// DUMP_DATA lbl_802F6890 u8
const u8 lbl_802F6890[128] = {
#include "dumpdata/lbl_802F6890.inc"
};
// DUMP_DATA lbl_802F6910 u8
const u8 lbl_802F6910[128] = {
#include "dumpdata/lbl_802F6910.inc"
};
// DUMP_DATA lbl_802F6990 u8
const u8 lbl_802F6990[176] = {
#include "dumpdata/lbl_802F6990.inc"
};
// DUMP_DATA lbl_802F6A40 s32
const s32 lbl_802F6A40[1024] = {
#include "dumpdata/lbl_802F6A40.inc"
};
// DUMP_DATA lbl_802F7A40 s32
const s32 lbl_802F7A40[1024] = {
#include "dumpdata/lbl_802F7A40.inc"
};
// DUMP_DATA lbl_802F8A40 u8
const u8 lbl_802F8A40[16][64] = {
#include "dumpdata/lbl_802F8A40.inc"
};

/* .bss owned by this unit (0x8041D508-0x8041D728), in REVERSE address order: this C unit emits globals last-defined first. */
u8 lbl_8041D6A8[0x80];
u8 lbl_8041D628[0x80];
s32 lbl_8041D608[8];
u8 lbl_8041D508[0x100];

/* .sbss owned by this unit (0x8055DEC0-0x8055DF00), in REVERSE address order: this C unit emits globals last-defined first. */
void *lbl_8055DEFC;
void *lbl_8055DEF8;
u32 lbl_8055DEF4;
u32 lbl_8055DEF0;
s32 lbl_8055DEEC;  /* unreferenced (a gap in the target) */
s32 lbl_8055DEE8;
s32 lbl_8055DEE0[2];
s32 lbl_8055DEDC;
s32 (*lbl_8055DED8)(void *reader, void *name, u32 flags);
void *lbl_8055DED4;
s32 lbl_8055DED0;
s32 lbl_8055DECC;
s32 (*lbl_8055DEC8)(s32);
s64 lbl_8055DEC0;
