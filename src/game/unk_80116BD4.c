// Code 0x80116BD4-0x801274C0, imported verbatim from the upstream project (github.com/orangedude27/excite-truck,
// CC0; file src/auto/text_801166C8_.c), where its functions match exactly. Unit name is provisional.
// CLEANUP: automated-decompiler source with raw offsets and placeholder types; rewrite with real types
// before the PC port. Tricks are tagged MATCHING-AID.
#include <types.h>
#include <revolution/AX/AXPB.h>
extern f64 cos(f64);
extern f64 sin(f64);
extern f32 lbl_80561210;
extern f32 lbl_80561214;
extern f32 lbl_80561218;
extern f32 lbl_8056121C;
extern f32 lbl_80561220;
extern f32 lbl_80561224;
extern f64 lbl_80561228;
extern f32 lbl_80329BC8[];


extern void fn_8011AF94(f32 *, s32, void *, f32 *, s32);
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
    // 0xF0: the file reader (fn_8011ACC4 for DVD files); its state follows its functions.
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
    u32 readerCapacity;                 // 0x138
    u32 readerFilled;                   // 0x13C
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
void fn_8011B94C(BinkAudio8011 *a, s16 **out, u32 *outLen, u8 *src, u8 **next);
u32 fn_8011FC2C(void *arg0, s32 arg1, s32 arg2, s32 *arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7);
void fn_80117330(void *arg0);
void fn_80117074(StreamPlayer *p);

extern s32 lbl_8055DED4;
u32 fn_8011C1E0(s32 arg0, void *arg1);
void fn_8011C2EC(s32 arg0, void *arg1);
s8 *fn_80116EE4(s8 *dst, s16 *src, u32 bytes);
void fn_80117074(StreamPlayer *p);
void fn_80118C44(StreamPlayer *p);
void fn_80118DC0(StreamPlayer *p);
s32 fn_80118F78(StreamPlayer *p, s32 arg1);
s32 fn_801190B0(StreamPlayer *p, s32 arg1);
void fn_80118C1C(void *arg0, s32 arg1);
void fn_80118F1C(void *arg0, s32 arg1);
void fn_80117380(struct StreamPlayer *p, s32 next);
u32 fn_80116F8C(struct StreamTrack *t);
s32 fn_80116D30(void);
void fn_8011BC48(s32 arg0);
s32 fn_8011BFFC(s32 arg0, s32 arg1, void *arg2, s32 arg3, s32 arg4);
void fn_8011C274(s32 arg0, void *arg1);
void fn_80120084(void *arg0);
extern void memset(void *, int, unsigned int);

// Takuya Ooura's general purpose FFT package (fftsg.c), built with float data.
void fn_801200BC(int n, int isgn, f32 *a, int *ip, f32 *w);  // rdft
void fn_8012023C(int n, int isgn, f32 *a, int *ip, f32 *w);  // ddct
void fn_80120834(int nw, int *ip, f32 *w);                     // makewt
void fn_80120B6C(int nc, int *ip, f32 *c);                     // makect
void fn_80120CA4(int n, f32 *a, int *ip, int nw, f32 *w);    // cftfsub
void fn_80120E14(int n, f32 *a, int *ip, int nw, f32 *w);    // cftbsub
void fn_80120F84(int n, int *ip, f32 *a);                    // bitrv2
void fn_80121400(int n, int *ip, f32 *a);                    // bitrv2conj
void fn_80121848(f32 *a);                                    // bitrv216
void fn_801219B4(f32 *a);                                    // bitrv216neg
void fn_80121BB0(f32 *a);                                    // bitrv208
void fn_80121BF4(f32 *a);                                    // bitrv208neg
void fn_80121C68(int n, f32 *a, f32 *w);                     // cftf1st
void fn_80122518(int n, f32 *a, f32 *w);                     // cftb1st
void fn_80122E08(int n, f32 *a, int nw, f32 *w);             // cftrec1
void fn_80122ED8(int n, f32 *a, int nw, f32 *w);             // cftrec2
void fn_80122FA4(int n, f32 *a, int nw, f32 *w);             // cftexp1
void fn_801231B8(int n, f32 *a, int nw, f32 *w);             // cftexp2
void fn_801233F8(int n, f32 *a, f32 *w);                     // cftmdl1
void fn_801237C4(int n, f32 *a, f32 *w);                     // cftmdl2
void fn_80123CEC(int n, f32 *a, int nw, f32 *w);             // cftfx41
void fn_80123DC0(int n, f32 *a, int nw, f32 *w);             // cftfx42
void fn_80123E98(f32 *a, f32 *w);                            // cftf161
void fn_8012440C(f32 *a, f32 *w);                            // cftf162
void fn_80124AD8(f32 *a, f32 *w);                            // cftf081
void fn_80124CE8(f32 *a, f32 *w);                            // cftf082
void fn_80124F90(f32 *a);                                    // cftf040
void fn_80125014(f32 *a);                                    // cftb040
void fn_80125098(f32 *a);                                    // cftx020
void fn_801250CC(int n, f32 *a, int nc, f32 *c);             // rftfsub
void fn_801252D8(int n, f32 *a, int nc, f32 *c);             // rftbsub
void fn_801254E4(int n, f32 *a, int nc, f32 *c);             // dctsub

#define M2C_FIELD(base, type, offset) (*(type)((u8 *)(base) + (offset)))

extern void* fn_802ECC74(void*);
extern void *fn_8011FFC4(s32);
extern f32 lbl_80329D48[];
extern f64 lbl_805611D8;
extern void fn_8011A330(void *);
void fn_8011FFB8();
extern void *lbl_8055DEF8;
extern void *lbl_8055DEFC;
extern void ***lbl_80559948;
extern u32 *lbl_8055994C;
extern void fn_80127F70(void*);
extern void fn_80127F94(void*);
extern s32 fn_8011A7CC(DVDFileInfo *);
extern s32 OSDisableInterrupts(void);
void fn_80127440(void*);
extern void fn_80127398(s32);
extern void fn_802E300C(s32);
typedef struct CallbackList {
    s32 count;
    void (*fns[4])(void);
} CallbackList;
extern CallbackList lbl_8041D728[];
extern u8 lbl_802F6A40[];
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
u32 fn_80116BD4(u32 a, u32 b, u32 d) {
    u32 hi = __mulhwu(a, b);
    u32 lo = a * b;
    u32 s;
    u32 vn1, vn0, un32, un10, un1, un0, un21, q1, q0, r, p;

    if ((d & (d - 1)) == 0) {
        s = __cntlzw(d);
        return (hi << (s + 1)) | (lo >> (31 - s));
    }
    if (hi == 0) {
        return lo / d;
    }
    s = __cntlzw(d);
    d <<= s;
    un32 = (hi << s) + (lo >> (32 - s));
    un10 = lo << s;
    vn1 = d >> 16;
    vn0 = d & 0xFFFF;
    un1 = un10 >> 16;

    q1 = un32 / vn1;
    r = ((un32 - q1 * vn1) << 16) | un1;
    p = q1 * vn0;
    if (r < p) {
        r += d;
        q1--;
        if (r >= d && r < p) {
            r += d;
            q1--;
        }
    }
    un21 = r - p;

    q0 = un21 / vn1;
    r = ((un21 - q0 * vn1) << 16) | (un10 & 0xFFFF);
    p = q0 * vn0;
    if (r < p) {
        r += d;
        q0--;
        if (r >= d && r < p) {
            q0--;
        }
    }
    return (q1 << 16) | q0;
}

// Bink was built with loop unrolling (a library build setting, applied here per function until the unit's
// hand-unrolled loops are rewritten). Fills `size` bytes at dst with the 16-bit value v.
#pragma opt_unroll_loops on
void fn_80116CC4(u32 *dst, u16 v, u32 size) {
    u32 n = size >> 1;
    u32 val = v | (v << 16);
    while (n--) {
        *dst++ = val;
    }
    if (size & 1) {
        *(u16 *)dst = v;
    }
}
#pragma opt_unroll_loops reset

extern s64 OSGetTime(void);
extern s64 lbl_8055DEC0;  // time of the first call
// Milliseconds since the first call (60750 timer ticks per ms), divided without a 64-bit division.
s32 fn_80116D30(void) {
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

extern void* fn_8011FF24(void);
extern void *fn_8011FF2C(s32);
// Allocates `size` bytes for player p, counting them (plus allocator overhead) in its memory total.
void *fn_80116DDC(void *p, s32 size) {
    void *overhead = fn_8011FF24();
    s32 *fields = (s32 *)p;
    fields[0x290 / 4] += size + (s32)overhead;
    return fn_8011FF2C(size);
}

void fn_80116E2C(const u8 *arg0) {
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
s32 fn_80116E54(s32 (*arg0)(s32), s32 arg1) {
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
s8 *fn_80116EE4(s8 *dst, s16 *src, u32 bytes) {
    u32 n;
    for (n = bytes >> 1; n != 0; n--) {
        *dst++ = *src++ >> 8;
    }
    return dst;
}

#pragma push
#pragma use_lmw_stmw off
// Tops a track's buffered data up to its fill target by inserting silence before the read position; returns the
// milliseconds of silence added.
u32 fn_80116F8C(StreamTrack *t) {
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
#pragma pop
extern s32 fn_80116D30(void);
// Feeds every stream track: while a track is playing and has enough data buffered (or its deadline passed),
// copies the next block out of its ring buffer into the buffer it hands out.
// Feeds every stream track: while a track is playing and has enough data buffered (or its deadline passed),
// copies the next block out of its ring buffer into the buffer it hands out.
void fn_80117074(StreamPlayer *p) {
    u32 i;
    u8 *dst;
    u32 len;
    u32 first;

    if (p->tracks[p->numTracks - 1].IsActive != 0 && fn_8011C1E0(lbl_8055DED4, p->queue) != 0) {
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
                            dst = (u8 *)fn_80116EE4((s8 *)dst, (s16 *)p->tracks[i].readPtr, first);
                        } else {
                            memcpy(dst, p->tracks[i].readPtr, first);
                            dst += first;
                        }
                    }
                    if (p->tracks[i].eightBit) {
                        fn_80116EE4((s8 *)dst, (s16 *)p->tracks[i].ringStart, len - first);
                    } else {
                        memcpy(dst, p->tracks[i].ringStart, len - first);
                    }
                    p->tracks[i].readPtr = p->tracks[i].ringStart + (len - first);
                } else {
                    if (p->tracks[i].eightBit) {
                        fn_80116EE4((s8 *)dst, (s16 *)p->tracks[i].readPtr, len);
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
        fn_8011C2EC(lbl_8055DED4, p->queue);
    }
}

void fn_80117330(void* arg0) {
 if(*(s32*)((u8*)arg0+0x268)==0) {
  s32 value=fn_80116D30();
  s32 count=*(s32*)((u8*)arg0+0x258);
  *(s32*)((u8*)arg0+0x268)=value;
  *(s32*)((u8*)arg0+0x26c)=count-1;
  *(s32*)((u8*)arg0+0x330)=0;
 }
}

// Moves a stream player to set `next` (1-based; 0 restarts the first): feeds the tracks, notes when the first set
// starts (flagging a stall after 300000 ticks), and points the data at the set, in memory or through the reader.
void fn_80117380(StreamPlayer *p, s32 next) {
    int set;
    s32 now;
    s32 base;
    set = next;
    if (next != 0) set = next - 1;
    p->unk2F8 = 0;
    if (p->numTracks != 0) fn_80117074(p);
    p->prevSet = p->curSet;
    if (set == 0) {
        now = fn_80116D30();
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
    if (p->numTracks != 0) fn_80117074(p);
    p->curSet = set + 1;
}

u32 fn_80116BD4(u32 a, u32 b, u32 d);
u32 fn_801174B0(s32 count, u32 *values, s32 offset, s32 *outIndex, s32 *outFlag) {
    s32 i;
    u32 maxDiff = 0;
    s32 maxIndex = 0;
    s32 allOdd = 1;
    for (i = 0; i < count - offset; i++) {
        u32 *ahead = values + offset;
        u32 diff = ahead[i] - values[i];
        if (diff > maxDiff) {
            maxDiff = diff;
            maxIndex = i;
        }
    }
    if (maxDiff == 0) {
        maxDiff = fn_80116BD4(values[count] - values[0], offset, count);
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

s32 fn_80117598(s32 arg0,s32 arg1){
 s32 off=((arg0+0x340)-arg0);
 s32* p=(s32*)(arg0-off);
 s32 v=p[0x33c/4];
 return (~v) & ((v-arg1 | arg1-v)>>31);
}

// Queue callback: `arg0` is the player's queue entry; feeds the player's tracks and stores the tick.
void fn_801175C4(s32 arg0, s32 arg1) {
    // container-of: the player that owns this queue entry
    StreamPlayer *p = (StreamPlayer *)((u8 *)arg0 - ((u8 *)((StreamPlayer *)arg0)->queue - (u8 *)arg0));
    if (p->numTracks) fn_80117074(p);
    p->tick = arg1;
}

void fn_80117618(void *arg0, void *arg1) {
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

void fn_801176FC(void* arg0,s32 arg1){if(arg0)*(s32*)((u8*)arg0+0xb8)=arg1;}

s32 fn_8011ACC4(void *reader, void *name, s32 flags);
BinkAudio8011 *fn_8011B584(u32 rate, u32 channels, u32 flags);
void fn_8011FE20(void **slot, s32 size);
void fn_8011DFAC(void *arg0, u32 arg1);
void *fn_8011BE48(s32 arg0);
void fn_8011C120(void *arg0);
void fn_8011BEC8(void *arg0, void *arg1, s32 arg2, s32 arg3);
void fn_80120084(void *p);

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

// Frame buffer layout filled in by fn_80117618.
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

extern char lbl_802F6528[];  // BinkOpen error messages
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
StreamPlayer *fn_8011770C(void *name, u32 flags) {
    BinkHeader8011 hdr;
    BinkFrameBuffers8011 fb;
    StreamPlayer local;
    s32 fromMemory;
    StreamPlayer *p;
    s32 (*open)(void *reader, void *name, u32 flags);
    s32 startArg;
    u32 i;
    u32 n;
    s32 any;
    s32 f;
    void *buf;

    open = (s32 (*)(void *, void *, u32))fn_8011ACC4;
    if (!(flags & 0x4000)) {
        lbl_80559944 = 1;
        lbl_8041D608[0] = 0;
    }
    memset(&local, 0, sizeof(local));
    local.openTime = fn_80116D30();
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
                fn_80116E2C((u8 *)lbl_802F6528);
            }
            return 0;
        }
        local.readerRead(&local.readerRead, 0, &hdr, sizeof(hdr));
    }
    if ((hdr.magic != 'fKIB' && hdr.magic != 'gKIB' && hdr.magic != 'hKIB' && hdr.magic != 'iKIB') ||
        local.unk110 != 0) {
        fn_80116E2C((u8 *)lbl_802F6528 + 0x14);
    close:
        if (!(flags & 0x04000000)) {
            local.readerClose(&local.readerRead);
        }
        return 0;
    }
    if (hdr.frames == 0) {
        fn_80116E2C((u8 *)lbl_802F6528 + 0x25);
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
    fn_8011FE20((void **)&local.frameBuffer, local.frameBufferSize + 16);
    fn_8011FE20((void **)&local.histFrameStart, local.histSize * 4);
    fn_8011FE20((void **)&local.histIdle, local.histSize * 4);
    fn_8011FE20((void **)&local.histDecode, local.histSize * 4);
    fn_8011FE20((void **)&local.hist29C, local.histSize * 4);
    fn_8011FE20((void **)&local.histRead, local.histSize * 4);
    fn_8011FE20((void **)&local.hist128, local.histSize * 4);
    fn_8011FE20((void **)&local.hist12C, local.histSize * 4);
    fn_8011FE20((void **)&local.tracks, (lbl_80559944 != 0 ? lbl_80559944 : 1) * sizeof(StreamTrack));
    fn_8011FE20((void **)&local.trackIds, lbl_80559944 * 4);
    fn_8011DFAC(local.planeSizes, (local.width + 15) & ~15);
    fn_8011FE20((void **)&local.planeSizes[0], local.planeSizes[0]);
    fn_8011FE20((void **)&local.planeSizes[1], local.planeSizes[1]);
    fn_8011FE20((void **)&local.planeSizes[2], local.planeSizes[2]);
    fn_8011FE20((void **)&local.planeSizes[3], local.planeSizes[3]);
    fn_8011FE20((void **)&local.planeSizes[4], local.planeSizes[4]);
    fn_8011FE20((void **)&local.planeSizes[5], local.planeSizes[5]);
    fn_8011FE20((void **)&local.planeSizes[6], local.planeSizes[6]);
    fn_8011FE20((void **)&local.planeSizes[7], local.planeSizes[7]);
    fn_8011FE20((void **)&local.planeSizes[8], local.planeSizes[8]);
    if (!fromMemory) {
        fn_8011FE20((void **)&local.setIndex, (local.numIndex + 1) * 4);
        fn_8011FE20((void **)&local.audioMaxSize, local.numAudio * 4);
        fn_8011FE20((void **)&local.audioFlags, local.numAudio * 4);
        fn_8011FE20((void **)&local.audioIds, local.numAudio * 4);
    }
    p = fn_80116DDC(&local, sizeof(StreamPlayer));
    if (p == 0) {
    nomem:
        fn_80116E2C((u8 *)lbl_802F6528 + 0x59);
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
            fn_80116E2C((u8 *)lbl_802F6528 + 0x68);
            goto close;
        }
    }
    p->maxFrameSize = fn_801174B0(p->numSets, (u32 *)p->setIndex, p->histSize, (s32 *)&p->unkD8, &p->unk380);
    if (!(flags & 0x400)) {
        fn_80117618(p, &fb);
        any = 0;
        for (f = 0; f < fb.count; f++) {
            u32 cPitch = (fb.cWidth + 15) & ~15;
            if (fb.frames[f].y.wanted) {
                fn_8011FE20(&fb.frames[f].y.buffer, fb.width * fb.height);
                any = 1;
                fb.frames[f].y.pitch = fb.width;
            }
            if (fb.frames[f].cr.wanted) {
                fn_8011FE20(&fb.frames[f].cr.buffer, cPitch * fb.cHeight);
                fb.frames[f].cr.pitch = cPitch;
                any = 1;
            }
            if (fb.frames[f].cb.wanted) {
                fn_8011FE20(&fb.frames[f].cb.buffer, cPitch * fb.cHeight);
                fb.frames[f].cb.pitch = cPitch;
                any = 1;
            }
            if (fb.frames[f].a.wanted) {
                fn_8011FE20(&fb.frames[f].a.buffer, fb.width * fb.height);
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
            p->extra = buf = fn_80116DDC(p, sizeof(fb));
            if (buf == 0) {
            freeplayer:
                fn_80120084(p);
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
            p->memoryData = fn_80116DDC(p, n);
            if (p->memoryData == 0) {
                if (p->video != 0) {
                    fn_80120084(p->video);
                }
                goto freeplayer;
            }
            p->readerStart(&p->readerRead, 0, 0, p->fileSize + 8, startArg);
            p->readerSeek(&p->readerRead, 0, p->setIndex[0] & ~1, (s32)p->memoryData, n);
            p->readerClose(&p->readerRead);
            p->readTime = 0;
        } else {
            fn_8011FE20((void **)&p->setData, p->largestFrame);
            p->readBuffer = fn_80116DDC(p, p->readBufSize);
            if (p->readBuffer == 0) {
                p->readBufSize = 0;
            }
            p->readerStart(&p->readerRead, p->readBuffer, p->readBufSize, p->fileSize + 8, startArg);
        }
    }
    p->frameBuffer[p->frameBufferSize] = 0;
    p->curSet = -1;
    if (p->fpsNum != 0) {
        p->unk270 = fn_80116BD4(2000, p->fpsDen, p->fpsNum);
    } else {
        p->unk270 = 2000;
    }
    p->unk28C = 1;
    fn_80117380(p, 1);
    p->openTime = fn_80116D30() - p->openTime;
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
    for (i = 0; i < p->numTracks;) {
        StreamTrack *t;
        BinkAudioFlags8011 af;
        p->tracks[i].ringStart = 0;
#define AF (p->audioFlags[p->trackIds[i]])
        af = AF;
        if ((af.valid || af.dct) && lbl_8055DECC != 0) {
            u32 rate = af.rate;
            if (local.fpsNum != 0 && local.fpsDen != 0) {
                rate = (s32)((f64)(s32)local.rateDen * ((f64)(s32)rate * (f64)(s32)local.fpsNum) /
                             ((f64)(s32)local.fpsDen * (f64)(s32)local.rateNum));
            }
            if (((BinkSoundOpen8011)lbl_8055DECC)(&p->tracks[i], rate, af.is16 * 8 + 8, af.stereo + 1, p->flags, p)) {
                s32 latency;
                if (p->tracks[i].blockMask == 0) {
                    p->tracks[i].blockMask = -1;
                }
                p->tracks[i].ringSize = (p->audioMaxSize[p->trackIds[i]] + 0xFF) & ~0xFF;
                p->tracks[i].ringStart = fn_80116DDC(p, p->tracks[i].ringSize);
                t = &p->tracks[i];
                if (t->ringStart == 0) {
                    t->Close(t);
                } else {
                    latency = 750 - t->unk68;
                    if (latency < 0) {
                        latency = 0;
                    }
                    t->eightBit = AF.is16 * 8 + 8 == 8;
                    p->tracks[i].ringEnd = p->tracks[i].ringStart + p->tracks[i].ringSize;
                    p->tracks[i].writePtr = p->tracks[i].ringStart;
                    p->tracks[i].readPtr = p->tracks[i].ringStart;
                    p->tracks[i].fillTarget = fn_80116BD4(rate * (AF.stereo + 1) * 2, latency, 1000) & ~3;
                    if (p->tracks[i].fillTarget > p->tracks[i].ringSize) {
                        p->tracks[i].fillTarget = p->tracks[i].ringSize;
                    }
                    p->tracks[i].handle = (s32)fn_8011B584(AF.rate, AF.stereo + 1, !AF.valid && AF.dct);
                    if (p->tracks[i].handle == 0) {
                        p->tracks[i].Close(&p->tracks[i]);
                        p->tracks[i].ringStart = 0;
                    }
                    lbl_8055DED0++;
                    p->tracks[i].deadline = p->numSets - (p->rateNum * 3) / (p->rateDen * 4);
                    p->tracks[i].available = 0;
                }
            }
        }
#undef AF
        if (p->tracks[i].gain == 0) {
            p->tracks[i].gain = 0x10000;
        }
        if (p->tracks[i].ringStart == 0) {
            p->numTracks--;
            memcpy(&p->trackIds[i], &p->trackIds[i + 1], (p->numTracks - i) * 4);
        } else {
            i++;
        }
    }
    if (p->numTracks != 0) {
        p->running = 1;
        if (p->tracks[0].unk60 == 0) {
            if (lbl_8055DED4 == 0) {
                lbl_8055DED4 = (s32)fn_8011BE48(-13);
                if (lbl_8055DED4 != 0) {
                    fn_8011C120((void *)lbl_8055DED4);
                }
            }
            if (lbl_8055DED4 != 0) {
                fn_8011BEC8((void *)lbl_8055DED4, p->queue, (s32)fn_80117598, (s32)fn_801175C4);
            }
        }
    }
    if (p->memoryData == 0 && !(flags & 0x200000)) {
        while (p->readerFill(&p->readerRead)) {
        }
    }
    return p;
}

// Decodes the current frame of a stream player: updates the timing history, copies each audio track's decoded
// packets into its ring (dropping the oldest data on overflow), then decodes the video. Returns 1 when playback is
// done, otherwise 0.
s32 fn_801186B8(StreamPlayer *p) {
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
            fn_801190B0(p, 0);
            fn_801190B0(p, 1);
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
    p->frameStart = fn_80116D30();
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
        p->histFrameStart[old] = p->frameStart - fn_80116BD4(1000, p->rateDen, p->rateNum);
        p->unk12C = 0;
        p->unk25C = p->frameStart;
        p->unk128 = 0;
    } else if (p->numTracks != 0) {
        fn_80117074(p);
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
                fn_8011C274(lbl_8055DED4, p->queue);
            }
            while (remaining != 0) {
                t = &p->tracks[j];
                room = t->ringSize - t->available;
                fn_8011B94C((BinkAudio8011 *)t->handle, &out, &len, src, &src);
                if (len > remaining) {
                    len = remaining;
                }
                remaining -= len;
                if (len > room) {
                    over = len - room;
                    p->tracks[j].readPtr += over;
                    level = OSDisableInterrupts();
                    p->tracks[j].available -= over;
                    OSRestoreInterrupts(level);
                    t = &p->tracks[j];
                    if (t->readPtr >= t->ringEnd) {
                        t->readPtr -= t->ringSize;
                    }
                }
                level = OSDisableInterrupts();
                p->tracks[j].available += len;
                OSRestoreInterrupts(level);
                t = &p->tracks[j];
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
        fn_8011C2EC(lbl_8055DED4, p->queue);
    }
    now = fn_80116D30();
    if (p->unk28C && p->video) {
        if (p->unk2F4 == 0) {
            memset(p->frameBuffer, 0, p->frameBufferSize);
        }
        if (p->memoryData == 0 && p->unk114 == 0) {
            u32 n = p->maxFrameSize;
            if (n >= p->readerCapacity) {
                n = p->readerCapacity;
            }
            if (p->readerFilled < (n * 4 - n) >> 2) {
                p->readerFill(&p->readerRead);
            }
        }
        p->videoResult = fn_8011FC2C(p->video, (s32)p->frameBuffer, (s32)p->blocksWide, (s32 *)data,
                                     p->setIndex[p->curSet - 1] & 1, (s32)p->planeSizes, p->flags, p->videoFlags);
    }
    if (p->numTracks != 0) {
        fn_80117074(p);
    }
    p->unkB4 = -1;
    p->unk258++;
    fn_80117330(p);
    p->frameEnd = fn_80116D30();
    p->idleTime += now - p->frameStart;
    p->decodeTime += p->frameEnd - now;
    p->unk2D4 = p->curSet;
    p->prevSet = p->curSet;
    p->busy = 0;
    return 0;
}

void fn_80118C1C(void *arg0, s32 arg1) {
    s32 temp_r0 = *(s32 *)((u8 *)arg0 + 0x264);
    if (temp_r0 != 0) {
        s32 temp_r5 = *(s32 *)((u8 *)arg0 + 0x29c);
        *(s32 *)((u8 *)arg0 + 0x29c) = temp_r5 + (arg1 - temp_r0);
        *(s32 *)((u8 *)arg0 + 0x264) = 0;
    }
}

// Per-frame update of a stream player: catches up tracks whose dirty flag is set, refills buffers and steps the
// playback state.
void fn_80118C44(StreamPlayer *p) {
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
            fn_8011C274(lbl_8055DED4, p->queue);
            fn_801190B0(p, 0);
            while ((p->readerCapacity * 29) >> 5 > p->readerFilled && p->readerFill(&p->readerRead) != 0) {
            }
            p->startTime = 0;
            p->unk328 = 0;
            p->pauseTime = 0;
            fn_801190B0(p, 1);
            fn_8011C2EC(lbl_8055DED4, p->queue);
        }
        p->busy = 1;
        if (p->numTracks != 0) {
            fn_80117074(p);
        }
        fn_80118C1C(p, fn_80116D30());
        fn_80117380(p, p->curSet < p->numSets ? p->curSet + 1 : 1);
        p->busy = 0;
    }
}

// Destroys a stream player: closes its tracks, releases its file and buffers, and frees it.
// Destroys a stream player: closes its tracks, releases its file and buffers, and frees it.
void fn_80118DC0(StreamPlayer *p) {
    u32 i;
    if (p != 0) {
        fn_80118F78(p, 1);
        switch (fn_8011BFFC(0, (s32)p->request, p->numTracks != 0 ? (void *)lbl_8055DED4 : (void *)0, (s32)p->queue, 1)) {
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
                fn_80120084(p->tracks[i].ringStart);
            }
        }
        if (p->memoryData != 0) {
            if (!(p->flags & 0x04000000)) {
                fn_80120084(p->memoryData);
            }
        } else {
            p->readerClose(&p->readerRead);
            fn_80120084(p->readBuffer);
        }
        if (p->extra != 0) {
            fn_80120084(p->extra);
        }
        memset(p, 0, sizeof(StreamPlayer));
        fn_80120084(p);
    }
}

void fn_80118F1C(void *arg0, s32 arg1) {
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
s32 fn_80118F78(StreamPlayer *p, s32 arg1) {
    s32 now;
    u32 i;
    s32 resumed = 0;
    if (p == 0) {
        return 0;
    }
    now = fn_80116D30();
    fn_80118C1C(p, now);
    fn_80118F1C(p, now);
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
    fn_8011C274(lbl_8055DED4, p->queue);
    for (i = 0; i < p->numTracks; i++) {
        p->tracks[i].SetPaused(&p->tracks[i], arg1);
    }
    fn_8011C2EC(lbl_8055DED4, p->queue);
    if (p->unk258 != 0 && p->numTracks != 0) {
        fn_80117074(p);
    }
    if (resumed) {
        p->startTime = fn_80116D30() - p->pauseTime;
    }
    return p->paused;
}

// Stops (arg1 == 0) or restarts every track; when a stop empties the tracks their rings are reset.
s32 fn_801190B0(StreamPlayer *p, s32 arg1) {
    s32 any = 0;
    s32 locked;
    u32 i;
    s32 r;
    if (p != 0) {
        locked = 0;
        for (i = 0; i < p->numTracks; i++) {
            if (p->tracks[i].Run != 0) {
                if (!locked) {
                    locked = 1;
                    fn_8011C274(lbl_8055DED4, p->queue);
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
                        fn_80116F8C(&p->tracks[i]);
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
            fn_8011C2EC(lbl_8055DED4, p->queue);
        }
    }
    return any;
}

void fn_80119214(void) {
    fn_8011FFB8();
}

extern void memset(void *, int, unsigned int);
void fn_80119218(void *arg0, s32 arg1, s32 arg2, s32 arg3) {
    void *temp_r31 = (u8*)arg0 + 0x14C;
    memset(temp_r31, 0, 0x30);
    *(s16*)temp_r31 = arg2;
    *(s16*)((u8*)temp_r31 + 4) = arg3;
    *(s16*)((u8*)arg0 + 0x1AC) = arg1;
    *(s16*)((u8*)arg0 + 0x1AE) = 0;
    *(s32*)((u8*)arg0 + 0x144) = 3;
    *(s32*)((u8*)arg0 + 0x1C) |= 0x212;
}

void fn_80119288(void *arg0) {
    if (*(void **)((u8 *)arg0 + 0x7C) != 0) {
        fn_80119218(*(void **)((u8 *)arg0 + 0x7C), *(s32 *)((u8 *)arg0 + 0xA4), *(s32 *)((u8 *)arg0 + 0xA8),
                    *(s32 *)((u8 *)arg0 + 0x78) == 1 ? *(s32 *)((u8 *)arg0 + 0xAC) : 0);
    }
    if (*(void **)((u8 *)arg0 + 0x80) != 0) {
        fn_80119218(*(void **)((u8 *)arg0 + 0x80), *(s32 *)((u8 *)arg0 + 0xA4), 0, *(s32 *)((u8 *)arg0 + 0xAC));
    }
}

extern void DCFlushRange(void *, u32);
void fn_80119300(void *arg0, u32 arg1, u32 arg2) {
    u32 n;
    u32 len;
    u32 off;
    len = arg2;
    if (arg2 + arg1 > *(u32 *)((u8 *)arg0 + 0x18)) {
        len = *(u32 *)((u8 *)arg0 + 0x18) - arg1;
    }
    if (*(s32 *)((u8 *)arg0 + 4) != 0) {
        off = arg1 >> 1;
        n = len >> 1;
        memset(*(u8 **)((u8 *)arg0 + 8) + off, 0, n);
        DCFlushRange(*(u8 **)((u8 *)arg0 + 8) + off, n);
        memset(*(u8 **)((u8 *)arg0 + 0xC) + off, 0, n);
        DCFlushRange(*(u8 **)((u8 *)arg0 + 0xC) + off, n);
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

extern void AXSetVoiceState(s32);
void fn_8011943C(void *arg0, s32 arg1) {
    s32 temp_r0;
    s32 temp_r3;
    temp_r0 = *(s32 *)arg0;
    if (temp_r0 != 0) {
        ((void (*)())AXSetVoiceState)(temp_r0, arg1);
    }
    if ((temp_r3 = *(s32 *)((u8 *)arg0 + 4)) != 0) {
        ((void (*)())AXSetVoiceState)(temp_r3, arg1);
    }
}

void fn_80119498(void *arg0) {
    fn_8011943C(arg0, 1);
    *(s32 *)((u8 *)arg0 + 0x38) = 0;
}

s32 fn_801194D0(void* arg0) {
 return *(u16*)((u8*)arg0+0x1be) + (*(u16*)((u8*)arg0+0x1bc)<<16);
}

s32 fn_801194E4(void *arg0) {
    s32 first;
    s32 current;
    s32 result;
    do {
        first = fn_801194D0(*(void **)arg0);
        fn_801194D0(*(void **)arg0);
        result = fn_801194D0(*(void **)arg0);
    } while ((u32)(result - first) >= 0x400);
    current = result * 2 - *(s32 *)((u8 *)arg0 + 0x10);
    if (*(s32 *)((u8 *)arg0 + 4) != 0)
        current *= 2;
    return current;
}

void fn_80119558(void *arg0) {
    *(s32 *)((u8 *)arg0 + 0x34) = 1;
    fn_80119498(arg0);
    *(s32 *)((u8 *)arg0 + 0x48) = fn_801194E4(arg0);
    *(s32 *)((u8 *)arg0 + 0x50) = fn_80116D30();
}

void fn_801195A0(void *arg0) {
    fn_8011943C(arg0, 0);
    *(s32 *)((u8 *)arg0 + 0x38) = 1;
}

extern f64 lbl_80561198;
extern f64 lbl_805611A8;
extern f32 lbl_805611A0;
extern f64 fn_8002EFBC(f32, f64);
extern void fn_80119288(void *);
extern void fn_80119300(void *, u32, u32);
extern void fn_8011943C(void *, s32);
extern s32 fn_801199A4(void *, s32);

// Moves buffered data from a track's ring into its voice buffers, splitting interleaved stereo into the left and
// right voices.
void fn_801195D8(StreamTrack *t) {
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
            fn_80119558(t->voices);
        }
    }
}

// Keeps a track's voices fed: tops them up from the ring, or writes silence while the voices outrun the data.
s32 fn_80119824(StreamTrack *t) {
    u32 pos;
    u32 now;
    u32 space;
    if (t->stopping != 0 || t->playing == 0) {
        return 0;
    }
    if (t->ending == 0) {
        fn_801195D8(t);
        return 0;
    }
    pos = t->lastPos;
    if (fn_80116D30() - t->lastTime <= 12) {
        goto compute;
    }
    for (;;) {
        pos = fn_801194E4(t->voices);
        now = fn_80116D30();
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
        fn_80119300(t->voices, t->writePos, t->chunk * 2);
        t->writePos += t->chunk * 2;
        if (t->writePos >= t->bufSize) {
            t->writePos -= t->bufSize;
        }
    }
    t->pending = space;
    fn_801195D8(t);
done:
    return 0;
}

s32 fn_80119994(void) { return 0; }

s32 fn_8011999C(void) { return 0; }

s32 fn_801199A4(void* arg0, s32 arg1) {
    extern void fn_8003381C(void*, s16*);
    s16 rect[8];
    u32 width = *(u32*)((u8*)arg0 + 0x10);
    u32 x0 = width >> 1;
    u32 x1 = ((width + *(u32*)((u8*)arg0 + 0x1C)) >> 1) - 1;
    u32 limit;
    rect[0] = 1;
    rect[1] = 10;
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
    fn_8003381C(*(void**)arg0, rect);
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
        fn_8003381C(*(void**)((u8*)arg0 + 4), rect);
    }
    return 0;
}

void fn_80119AB0(void *arg0, s32 arg1) {
    s32 value = arg1;
    f32 temp;
    if (value < 0) value = 0;
    if (value > 0x7fff) value = 0x7fff;
    temp = (f32) fn_8002EFBC((f32)value, lbl_80561198);
    *(s32 *)((u8 *)arg0 + 0xa4) = (s32)(lbl_805611A0 * temp);
    fn_80119288(arg0);
}

extern f32 lbl_805611B0;
extern f32 lbl_805611B4;
extern f64 lbl_805611B8;
extern f32 lbl_805611C0;
void fn_80119B38(void *arg0, s32 arg1) {
    s32 value = arg1;
    f32 temp_f31;
    f32 temp_f3;
    f32 temp_f1;
    if (value < 0) value = 0;
    if (value > 0x10000) value = 0x10000;
    temp_f31 = (f32)value / lbl_805611B0;
    temp_f3 = (f32)fn_8002EFBC(lbl_805611B4 - temp_f31, lbl_805611B8);
    *(s32*)((u8*)arg0 + 0xa8) = (s32)(lbl_805611C0 * temp_f3);
    temp_f1 = (f32)fn_8002EFBC(temp_f31, lbl_805611B8);
    *(s32*)((u8*)arg0 + 0xac) = (s32)(lbl_805611C0 * temp_f1);
    fn_80119288(arg0);
}

extern f64 lbl_80561198;
extern f64 lbl_805611A8;
extern f32 lbl_805611A0;
extern f64 fn_8002EFBC(f32, f64);
extern void fn_80119288(void *);
extern void fn_80119300(void *, u32, u32);
extern void fn_8011943C(void *, s32);
extern s32 fn_801199A4(void *, s32);
s32 fn_80119C0C(void *arg0, s32 arg1) {
    s32 state = *(s32 *)((u8 *)arg0 + 0x64);
    if (arg1 != 0) {
        if (state == 0) {
            *(s32 *)((u8 *)arg0 + 0xb0) = 0;
            *(s32 *)((u8 *)arg0 + 0x64) = 1;
            *(s32 *)((u8 *)arg0 + 0xb8) = 0;
            *(s32 *)((u8 *)arg0 + 0xbc) = *(s32 *)((u8 *)arg0 + 0xc0);
        }
    } else if (state != 0) {
        fn_8011943C((u8 *)arg0 + 0x7c, 0);
        fn_801199A4((u8 *)arg0 + 0x7c, 1);
        fn_80119300((u8 *)arg0 + 0x7c, 0, *(s32 *)((u8 *)arg0 + 0x94));
        *(s32 *)((u8 *)arg0 + 0x64) = 0;
    }
    return *(s32 *)((u8 *)arg0 + 0x64);
}

s32 fn_80119CB0(void *arg0, s32 arg1) {
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
                fn_80119558((u8 *)arg0 + 0x7c);
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
extern s32 lbl_8055DEE4;
extern s32 lbl_8055DEE8;
extern void fn_8011A6A0(void *);
extern void TRKGetFreeBuffer(void *);
extern s32 lbl_8055DEE0[2];  // the sample buffer allocator: alloc, free
extern s32 lbl_8055DEE8;
extern void AXFreeVoice(void *);
typedef StreamTrack StreamChannel;  // the voice side of a track

// Stops and releases the voices of a channel, then frees its sample buffer.
void fn_80119D40(void *arg0) {
    StreamChannel *ch = arg0;
    if (ch->voices[0] != 0) {
        ch->stopping = 1;
        fn_8011943C(ch->voices, 0);
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
s32 fn_80119824(StreamTrack *t);
s32 fn_80119994(void);
s32 fn_8011999C(void);
void fn_80119AB0(void *arg0, s32 arg1);
void fn_80119B38(void *arg0, s32 arg1);
s32 fn_80119CB0(void *arg0, s32 arg1);
s32 fn_80119C0C(void *arg0, s32 arg1);

// Bink sound open for AX: sets up a track of `chans` 16-bit voices at `freq` Hz with a 600 ms buffer, prefilled with
// up to the whole movie's sound, and installs the track callbacks. Returns 0 without an allocator or voices.
s32 fn_80119DD0(StreamTrack *t, u32 freq, s32 bits, s32 chans, u32 flags, BinkInfo8011 *bink) {
    u32 bps;
    u32 total;
    u32 i;
    u32 n;
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
    t->interval = 0x21C;
    t->curRate = freq;
    t->unk68 = 0;
    t->bufSize = (bps * 600 / 1000 + 0x3FF) & ~0x3FF;
    t->chunk = (bps * 50 / 1000 + 0xFF) & ~0xFF;
    t->threshold = t->bufSize * 90 / 100;
    if (bink->frameRate != 0 && bink->frameRateDiv != 0) {
        total = (u64)bps * bink->frameRateDiv * bink->frames / bink->frameRate;
    } else {
        total = t->chunk;
    }
    if (chans != 1) {
        t->voiceSize = t->bufSize >> 1;
    } else {
        t->voiceSize = t->bufSize;
    }
    buf = ((void *(*)(u32))lbl_8055DEE0[0])(t->bufSize);
    t->bufL = (s16 *)buf;
    t->bufR = (s16 *)(buf + t->voiceSize);
    t->physL = (u32)buf - 0x80000000;
    t->physR = (u32)buf - 0x80000000 + t->voiceSize;
    t->minimum = t->chunk;
    t->volume = 0x7FFF;
    t->panL = 0x67F8;
    t->panR = 0x67F8;
    fn_80119288(t);
    fn_801199A4(t->voices, 1);
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
        AXSetVoiceSrcType(t->voices[i], 1);
        AXSetVoiceSrc(t->voices[i], &src);
        AXSetVoiceSrcRatio(t->voices[i], t->rate / 32000.0);
        AXSetVoiceAdpcm(t->voices[i], &adpcm);
    }
    fn_80119300(t->voices, 0, t->bufSize);
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
    t->IsActive = fn_80119824;
    t->GetBuffer = (s32 (*)(StreamTrack *, u8 **, u32 *))fn_80119994;
    t->Commit = (void (*)(StreamTrack *, u32))fn_8011999C;
    t->SetVolume = (void (*)(StreamTrack *, s32))fn_80119AB0;
    t->SetPan = (void (*)(StreamTrack *, s32))fn_80119B38;
    t->SetPaused = (void (*)(StreamTrack *, s32))fn_80119CB0;
    t->Run = (s32 (*)(StreamTrack *, s32))fn_80119C0C;
    t->Close = (void (*)(StreamTrack *))fn_80119D40;
    lbl_8055DEE8++;
    return 1;
}

typedef struct StreamAllocator {
    void *(*alloc)(u32 size);
    void (*free)(void *p);
} StreamAllocator;

// Installs the sample buffer allocator (0 clears it); returns the stream decode callback.
void *fn_8011A168(StreamAllocator *allocator) {
    if (allocator == NULL) {
        lbl_8055DEE0[0] = 0;
        lbl_8055DEE0[1] = 0;
    } else {
        *(StreamAllocator *)lbl_8055DEE0 = *allocator;
    }
    return (void *)fn_80119DD0;
}

#pragma unroll on
void fn_8011A1A0(u32 *data, u32 size) {
    u32 count = (size + 3) >> 2;
    while (count--) {
        *data = __lwbrx(data, 0);
        data++;
    }
}

// Reads `len` bytes at `offset` into `dst` through an aligned stack buffer, waiting for each chunk.
// Returns the bytes read, or 0 when the drive reports an error.
u32 fn_8011A240(void *arg0, u32 arg1, s32 arg2, u32 arg3) {
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

void fn_8011A330(void *arg0) {
    s32 status;
    *(s32 *)((u8 *)arg0 + 0xC4) = 1;
    DVDCancel((DVDCommandBlock *)((u8 *)arg0 + 0x54));
    do {
        status = DVDGetDriveStatus();
    } while (status == 2 || status == 1);
    *(s32 *)((u8 *)arg0 + 0xC4) = 0;
    *(s32 *)((u8 *)arg0 + 0x24) = 0;
}

u32 fn_8011A388(void* arg0, s32 arg1, s32 arg2, u32 arg3) {
    extern u32 fn_8011A240(void*, u32, s32, u32);
    extern void fn_8011A330(void*);
    u32 result, current, remaining;
    if (arg1 == -1 || *(u32*)((u8*)arg0 + 0x90) == (u32)arg1) return arg3;
    *(u32*)((u8*)arg0 + 0x90) = (u32)arg1;
    result = fn_8011A240((u8*)arg0 + 0x54,
                         *(u32*)((u8*)arg0 + 0x90) + *(u32*)((u8*)arg0 + 0xB4),
                         arg2, arg3);
    if (result != arg3) {
        *(s32*)((u8*)arg0 + 0x20) = 1;
        fn_8011A330(arg0);
    }
    *(u32*)((u8*)arg0 + 0x90) += result;
    current = *(u32*)((u8*)arg0 + 0x90);
    *(u32*)((u8*)arg0 + 0x98) = current;
    remaining = *(u32*)((u8*)arg0 + 0xB8) - current;
    if (remaining >= *(u32*)((u8*)arg0 + 0x40))
        *(u32*)((u8*)arg0 + 0x48) = *(u32*)((u8*)arg0 + 0x40);
    fn_8011A1A0((u32*)arg2, result);
    return result;
}

#pragma unroll reset
void fn_8011A45C(void *arg0, s32 arg1, s32 arg2) {
    s32 temp_r0, temp_r31, temp_r3, temp_r3_2, temp_r4, var_r31;
// MATCHING-AID?: volatile from upstream (check: hardware or trick); revisit.
    volatile s32 *counter = (volatile s32 *)((u8 *)arg0 + 0xC0);
    temp_r31 = fn_80116BD4(arg1, 0x3E8, *(s32 *)((u8 *)arg0 + 0xBC));
    temp_r3 = fn_80116D30();
    var_r31 = temp_r3;
    *counter = *counter + (temp_r31 - (temp_r3 - arg2));
    goto loop;
loop:
    temp_r3_2 = fn_80116D30();
    temp_r0 = *counter;
    temp_r4 = temp_r3_2 - var_r31;
    if (temp_r4 < temp_r0) goto loop;
    var_r31 = temp_r3_2;
    *counter = temp_r0 - temp_r4;
    if (*counter > 0) goto loop;
}

void fn_8011A4F8(u32 arg0, void *arg1) {
#define FIELD(p, o) (*(s32 *)((u8 *)(p) + (o)))
    s32 temp_r3_2, temp_r3_4, temp_r4;
    u32 temp_r3, temp_r3_3, var_r30;
    void *temp_r31;
    var_r30 = arg0;
    temp_r31 = *(void **)((u8 *)arg1 + 0x7C);
    if (FIELD(temp_r31, 0xC4) == 0) {
        if ((arg0 != 0x8000) && (arg0 != (u32)((FIELD(temp_r31, 0xB8) - FIELD(temp_r31, 0x90)) + FIELD(temp_r31, 0x94)))) {
            FIELD(temp_r31, 0x24) = 0; FIELD(temp_r31, 0x20) = 1; return;
        }
        temp_r4 = FIELD(temp_r31, 0x94);
        if (temp_r4 != 0) {
            temp_r3 = *(u32 *)((u8 *)temp_r31 + 0xAC);
            memmove((void *)temp_r3, (void *)(temp_r3 + temp_r4), FIELD(temp_r31, 0xB8) - FIELD(temp_r31, 0x90));
            temp_r3_2 = FIELD(temp_r31, 0x94); FIELD(temp_r31, 0x94) = 0; var_r30 -= temp_r3_2;
        }
        FIELD(temp_r31, 0x28) += var_r30; FIELD(temp_r31, 0x90) += var_r30;
        temp_r3_3 = *(u32 *)((u8 *)temp_r31 + 0xAC) + var_r30;
        *(u32 *)((u8 *)temp_r31 + 0xAC) = temp_r3_3;
        if (temp_r3_3 >= *(u32 *)((u8 *)temp_r31 + 0xA8)) *(u32 *)((u8 *)temp_r31 + 0xAC) = *(u32 *)((u8 *)temp_r31 + 0xA4);
        FIELD(temp_r31, 0xA0) -= var_r30;
        *(u32 *)((u8 *)temp_r31 + 0x4C) += var_r30;
        if (*(u32 *)((u8 *)temp_r31 + 0x4C) > *(u32 *)((u8 *)temp_r31 + 0x44)) *(u32 *)((u8 *)temp_r31 + 0x44) = *(u32 *)((u8 *)temp_r31 + 0x4C);
        if (FIELD(temp_r31, 0xBC) != 0) fn_8011A45C(temp_r31, var_r30, FIELD(temp_r31, 0xCC));
        temp_r3_4 = fn_80116D30() - FIELD(temp_r31, 0xCC);
        FIELD(temp_r31, 0xCC) = temp_r3_4; FIELD(temp_r31, 0x30) += temp_r3_4;
        if (FIELD(temp_r31, 0xC8) != 0 || FIELD(temp_r31, 0x2C) != 0) FIELD(temp_r31, 0x3C) += FIELD(temp_r31, 0xCC);
        else FIELD(temp_r31, 0x38) += FIELD(temp_r31, 0xCC);
        FIELD(temp_r31, 0x24) = 0;
        if (FIELD(temp_r31, 0x50) == 0) fn_8011A6A0(temp_r31);
    }
#undef FIELD
}

void fn_8011A6A0(void* arg0) {
    s32 status = DVDGetDriveStatus();
    s32 remaining = *(s32*)((u8*)arg0 + 0xB8) - *(s32*)((u8*)arg0 + 0x90);
    if (status != 10) {
        if (status < 10) {
            if (status == -1 || status >= 3)
                goto set_flag;
            goto check_read;
        }
        if (status < 12)
            goto set_flag;
        goto done;
    }
check_read:
    if (*(s32*)((u8*)arg0 + 0xC4) != 0)
        goto done;
    if (status != 0 && status != 10)
        goto done;
    if (*(u32*)((u8*)arg0 + 0xA0) >= 0x8000 && remaining != 0) {
        *(s32*)((u8*)arg0 + 0xC8) = *(s32*)((u8*)arg0 + 0x2C);
        *(s32*)((u8*)arg0 + 0xCC) = fn_80116D30();
        *(s32*)((u8*)arg0 + 0x24) = 1;
        if ((u32)remaining > 0x8000)
            remaining = 0x8000;
        else if (remaining & 0x1F) {
            s32 pad = (-remaining) & 0x1F;
            *(s32*)((u8*)arg0 + 0x94) = pad;
            remaining += pad;
        }
        DVDReadAsyncPrio((u8*)arg0 + 0x54, *(s32*)((u8*)arg0 + 0xAC), remaining,
            *(s32*)((u8*)arg0 + 0xB4) + (*(s32*)((u8*)arg0 + 0x90) - *(s32*)((u8*)arg0 + 0x94)),
            fn_8011A4F8, 0);
        goto done;
    }
    *(s32*)((u8*)arg0 + 0x48) = *(s32*)((u8*)arg0 + 0x4C);
    goto done;
set_flag:
    *(s32*)((u8*)arg0 + 0x20) = 1;
done:
    return;
}

s32 fn_8011A7CC(DVDFileInfo *info) {
    s32 *p = (s32 *)info;
    s32 status;
    if (p[0x20 / 4] != 0) return 0;
    if (p[0x50 / 4] != 0) return 0;
    status = DVDGetDriveStatus();
    if (status == 0) {
        fn_8011A6A0(info);
    } else if (status == 0xB) {
        p[0x20 / 4] = 1;
    }
    return p[0x24 / 4];
}

// 0x50 is a state flag the DVD callbacks also change: every access is volatile.
s32 fn_8011A870(void *arg0, s32 arg1) {
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
            fn_8011A7CC(arg0);
    }
    return *(volatile s32 *)((char *)arg0 + 0x50);
}

extern u32 fn_8011A240(void *, u32, s32, u32);
extern void fn_8011A45C(void *, s32, s32);
extern void fn_8011A1A0(u32 *, u32);
u32 fn_8011A8FC(void *arg0, s32 arg1, s32 arg2, void *arg3, u32 arg4) {
              s32 var_r29 = arg2;
              u32 var_r26 = arg4;
              s32 var_r25 = (s32)arg3;
              u32 var_r28 = 0;
              s32 temp_r27 = var_r25;
                s32 var_r31;
// MATCHING-AID?: volatile from upstream (check: hardware or trick); revisit.
              volatile u32 *var_r30;
              u32 temp_r0;
       u32 temp_r3;
        u32 temp_r6;
              u32 temp_r4;
              u32 temp_r5;
              u32 temp_r23;
          
              if (*(s32 *)((u8 *)arg0 + 0x20) != 0)
                  return 0;
              var_r31 = fn_80116D30();
              if (var_r29 != -1) {
                  temp_r4 = *(u32 *)((u8 *)arg0 + 0x98);
                  if (temp_r4 != (u32)var_r29) {
                      if ((u32)var_r29 > temp_r4) {
                          if ((u32)var_r29 <= *(u32 *)((u8 *)arg0 + 0x90)) {
                              BOOL interrupts = OSDisableInterrupts();
                              temp_r6 = (u32)var_r29 - *(u32 *)((u8 *)arg0 + 0x98);
                              *(u32 *)((u8 *)arg0 + 0xA0) += temp_r6;
                              temp_r4 = *(u32 *)((u8 *)arg0 + 0x9C) + temp_r6;
                              *(u32 *)((u8 *)arg0 + 0x98) = (u32)var_r29;
                              *(u32 *)((u8 *)arg0 + 0x4C) -= temp_r6;
                              *(u32 *)((u8 *)arg0 + 0x9C) = temp_r4;
                              if (temp_r4 > *(u32 *)((u8 *)arg0 + 0xA8))
                                  *(u32 *)((u8 *)arg0 + 0x9C) =
                                      temp_r4 - *(u32 *)((u8 *)arg0 + 0x40);
                              OSRestoreInterrupts(interrupts);
                          } else {
                              goto reset_seek;
                          }
                      } else {
                          goto reset_seek;
                      }
                  }
              }
              goto seek_done;
          reset_seek:
              fn_8011A330(arg0);
              {
                  BOOL interrupts = OSDisableInterrupts();
               temp_r0 = *(u32 *)((u8 *)arg0 + 0xA4);
               temp_r4 = 0;
               *(u32 *)((u8 *)arg0 + 0x90) = (u32)var_r29;
               temp_r5 = *(u32 *)((u8 *)arg0 + 0x40);
               *(u32 *)((u8 *)arg0 + 0x98) = (u32)var_r29;
               *(u32 *)((u8 *)arg0 + 0xA0) = temp_r5;
               *(u32 *)((u8 *)arg0 + 0x4C) = temp_r4;
               *(u32 *)((u8 *)arg0 + 0x9C) = temp_r0;
               *(u32 *)((u8 *)arg0 + 0xAC) = temp_r0;
                  OSRestoreInterrupts(interrupts);
              }
          seek_done:
              if (*(u32 *)((u8 *)arg0 + 0xA4) == 0) {
                  var_r29 = fn_80116D30();
                  var_r28 = fn_8011A240((u8 *)arg0 + 0x54,
                                        *(u32 *)((u8 *)arg0 + 0x90),
                                        (s32)var_r25, var_r26);
                  if (var_r28 < var_r26) {
                      *(s32 *)((u8 *)arg0 + 0x20) = 1;
                      fn_8011A330(arg0);
                  }
                  var_r30 = (u32 *)((u8 *)arg0 + 0x98);
                  *(u32 *)((u8 *)arg0 + 0x90) += var_r28;
                  *var_r30 += var_r28;
                  *(u32 *)((u8 *)arg0 + 0x28) += var_r28;
                  if (*(s32 *)((u8 *)arg0 + 0xBC) != 0)
                      fn_8011A45C(arg0, var_r28, var_r29);
                  temp_r3 = fn_80116D30();
          temp_r5 = *(u32 *)((u8 *)arg0 + 0x30);
          temp_r4 = temp_r3 - var_r29;
          temp_r0 = temp_r3 - var_r31;
          temp_r3 = temp_r5 + temp_r4;
          *(u32 *)((u8 *)arg0 + 0x30) = temp_r3;
          temp_r3 = *(u32 *)((u8 *)arg0 + 0x34);
          temp_r0 = temp_r3 + temp_r0;
          *(u32 *)((u8 *)arg0 + 0x34) = temp_r0;
              } else {
                  var_r30 = (u32 *)((u8 *)arg0 + 0x98);
                  goto loop_check;
              loop_body:
                  fn_8011A6A0(arg0);
                  var_r29 = *(s32 *)((u8 *)arg0 + 0x4C);
                  if (var_r29 != 0) {
                      if ((u32)var_r29 > var_r26)
                          var_r29 = (s32)var_r26;
                      var_r26 -= var_r29;
                      var_r28 += var_r29;
                      temp_r4 = *(u32 *)((u8 *)arg0 + 0x9C);
                      temp_r23 = *(u32 *)((u8 *)arg0 + 0xA8) - temp_r4;
                      *var_r30 += var_r29;
                      if (temp_r23 <= (u32)var_r29) {
                          memcpy((void *)var_r25, (void *)temp_r4, temp_r23);
                          var_r25 += temp_r23;
                          var_r29 -= temp_r23;
                          *(u32 *)((u8 *)arg0 + 0x9C) = *(u32 *)((u8 *)arg0 + 0xA4);
                          {
                              BOOL interrupts = OSDisableInterrupts();
                              *(u32 *)((u8 *)arg0 + 0x4C) -= temp_r23;
                              *(u32 *)((u8 *)arg0 + 0xA0) += temp_r23;
                              OSRestoreInterrupts(interrupts);
                          }
                      }
                      if (var_r29 != 0) {
                          memcpy(var_r25, (void *)*(u32 *)((u8 *)arg0 + 0x9C), var_r29);
                          var_r25 += var_r29;
                          * (u32 *)((u8 *)arg0 + 0x9C) += var_r29;
                          {
                              BOOL interrupts = OSDisableInterrupts();
                              *(u32 *)((u8 *)arg0 + 0x4C) -= var_r29;
                              *(u32 *)((u8 *)arg0 + 0xA0) += var_r29;
                              OSRestoreInterrupts(interrupts);
                          }
                      }
                  }
              loop_check:
                  if (var_r26 != 0 && *(s32 *)((u8 *)arg0 + 0x20) == 0)
                      goto loop_body;
                  *(s32 *)((u8 *)arg0 + 0x34) += fn_80116D30() - var_r31;
              }
          
              temp_r4 = *(u32 *)((u8 *)arg0 + 0xB8) - *var_r30;
             if (temp_r4 < *(u32 *)((u8 *)arg0 + 0x40)) {
                 *(u32 *)((u8 *)arg0 + 0x48) = temp_r4;
             } else {
                 *(u32 *)((u8 *)arg0 + 0x48) =
                     *(u32 *)((u8 *)arg0 + 0x40);
             }
// MATCHING-AID?: volatile from upstream (check: hardware or trick); revisit.
              if (*(volatile u32 *)((u8 *)arg0 + 0x4C) + 0x8000 >
// MATCHING-AID?: volatile from upstream (check: hardware or trick); revisit.
                   *(volatile u32 *)((u8 *)arg0 + 0x48))
                   *(u32 *)((u8 *)arg0 + 0x48) =
// MATCHING-AID?: volatile from upstream (check: hardware or trick); revisit.
                       *(volatile u32 *)((u8 *)arg0 + 0x4C);
              fn_8011A1A0((u32 *)temp_r27, var_r28);
              return var_r28;
         }

s32 fn_8011AC04(s32 arg0, s32 arg1) {
    return (arg1 + 0x7FFF) & 0xFFFF8000;
}

void fn_8011AC10(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
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
extern f64 lbl_805611D8;
extern void fn_8011A330(void *);
void fn_8011AC84(void *arg0) {
    fn_8011A330(arg0);
    if (*(s32 *)((u8 *)arg0 + 0xB0) == 0) {
        DVDClose((u8 *)arg0 + 0x54);
    }
}

extern u32 fn_8011A388(void*, s32, s32, u32);
extern u32 fn_8011A8FC(void *, s32, s32, void *, u32);
extern s32 fn_8011AC04(s32, s32);
extern void fn_8011AC10(void*, s32, s32, s32, s32);
extern s32 fn_8011A7CC(DVDFileInfo*);
extern void fn_8011AC84(void*);
extern s32 fn_8011A870(void*, s32);
s32 fn_8011ACC4(void* arg0, void* arg1, s32 arg2) {
    s32 size = *(s32*)((u8*)arg1 + 0x2C);
    memset(arg0, 0, 0x144);
    if (arg2 & 0x800000) {
        *(u32*)((u8*)arg0 + 0x54) = *(u32*)((u8*)arg1 + 0x00);
        *(u32*)((u8*)arg0 + 0x58) = *(u32*)((u8*)arg1 + 0x04);
        *(u32*)((u8*)arg0 + 0x5C) = *(u32*)((u8*)arg1 + 0x08);
        *(u32*)((u8*)arg0 + 0x60) = *(u32*)((u8*)arg1 + 0x0C);
        *(u32*)((u8*)arg0 + 0x64) = *(u32*)((u8*)arg1 + 0x10);
        *(u32*)((u8*)arg0 + 0x68) = *(u32*)((u8*)arg1 + 0x14);
        *(u32*)((u8*)arg0 + 0x6C) = *(u32*)((u8*)arg1 + 0x18);
        *(u32*)((u8*)arg0 + 0x70) = *(u32*)((u8*)arg1 + 0x1C);
        *(u32*)((u8*)arg0 + 0x74) = *(u32*)((u8*)arg1 + 0x20);
        *(u32*)((u8*)arg0 + 0x78) = *(u32*)((u8*)arg1 + 0x24);
        *(u32*)((u8*)arg0 + 0x7C) = *(u32*)((u8*)arg1 + 0x28);
        *(u32*)((u8*)arg0 + 0x80) = size;
        *(u32*)((u8*)arg0 + 0x84) = *(u32*)((u8*)arg1 + 0x30);
        *(u32*)((u8*)arg0 + 0x88) = *(u32*)((u8*)arg1 + 0x34);
        *(u32*)((u8*)arg0 + 0x8C) = *(u32*)((u8*)arg1 + 0x38);
        *(u32*)((u8*)arg0 + 0xB0) = 1;
        *(u32*)((u8*)arg0 + 0xB4) = size;
    } else {
        if (DVDConvertPathToEntrynum(arg1) == -1) return 0;
        if (DVDFastOpen((u8*)arg0 + 0x54) == 0) return 0;
    }
    ((void**)arg0)[0] = fn_8011A388;
    ((void**)arg0)[1] = fn_8011A8FC;
    ((void**)arg0)[2] = fn_8011AC04;
    ((void**)arg0)[3] = fn_8011AC10;
    ((void**)arg0)[4] = fn_8011A7CC;
    ((void**)arg0)[5] = fn_8011AC84;
    ((void**)arg0)[6] = fn_8011A870;
    return 1;
}

f32 fn_8011AE18(s32 arg0) {
    f32 temp;
    temp = (f32)(((u32)arg0 >> 5) & 0x1FFFFFF) * lbl_80329D48[((u32)arg0 * 4) & 0x7C];
    if (arg0 & 0x10000000) {
        temp = (f32)(-((f64)temp));
    }
    return temp;
}

// Scales float samples by `gain` into clamped 16-bit PCM.
void fn_8011AE68(s32 arg0, f32 *arg1, u32 arg2, f32 farg0) {
    s16 *out = (s16 *)arg0;
    u32 i;
    s32 v;
    for (i = 0; i < arg2; i++) {
        v = (s32)(*arg1++ * farg0);
        if (v < -32768) v = -32768;
        if (v > 32767) v = 32767;
        *out++ = v;
    }
}

// Interleaves two float channels (left at arg1, right after it) into clamped 16-bit stereo PCM.
void fn_8011AED8(s32 arg0, f32 *arg1, u32 arg2, f32 farg0) {
    s16 *out = (s16 *)arg0;
    f32 *right = arg1 + arg2;
    u32 i;
    s32 l;
    s32 r;
    for (i = 0; i < arg2; i++) {
        l = (s32)(*arg1++ * farg0);
        r = (s32)(farg0 * *right++);
        if (l < -32768) l = -32768;
        if (l > 32767) l = 32767;
        if (r < -32768) r = -32768;
        if (r > 32767) r = 32767;
        *out++ = l;
        *out++ = r;
    }
}

extern u8 lbl_80329B50[];
extern f32 lbl_805611E0;
void fn_8011AF94(f32 *arg0, s32 arg1, void *arg2, f32 *arg3, s32 arg4) {
    u32 var_r29;
    s32 var_r28;
    f32 *var_r27;
    u32 *var_r26;
    u32 var_r25;
    u32 var_r24;
    f32 *var_r4;
    f32 *var_r4_2;
    f32 *var_r8;
    f32 temp_f31;
    f32 var_f1;
    f32 var_f29;
    f32 var_f28;
    u32 *var_r5;
    u32 *var_r5_2;
    u32 *var_r7;
    s32 temp_r0;
    s32 temp_r18;
    s32 temp_r4;
    s32 temp_r4_2;
    s32 temp_r5_3;
    s32 temp_r6;
    s32 temp_r6_2;
    s32 var_ctr;
    s32 var_r0_2;
    s32 var_r0_3;
    s32 var_r5_3;
    u32 temp_r10;
    u32 temp_r5;
    u32 temp_r5_2;
    u32 temp_r9;
    u32 var_r0;
    u32 var_r24_2;
    u32 var_r3;
    u32 var_r4_3;

    var_f28 = lbl_805611E0;
    var_f29 = var_f28;
    var_r26 = ((BitReader8011 *)arg2)->next;
    var_r25 = ((BitReader8011 *)arg2)->value;
    var_r24 = ((BitReader8011 *)arg2)->count;
    var_r4 = arg3;
    var_r5 = (u32 *)arg4;
    var_r28 = 0;
    goto init_test;
init_body:
    var_f29 = *var_r4;
    var_r4++;
    var_r5++;
    var_r28++;
    var_f28 = -var_f29;
init_test:
    var_r0 = (u32)*var_r5 << 1;
    if (var_r0 < 2U) goto init_body;

    temp_f31 = lbl_805611E0;
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
        var_r4_2 = arg3 + var_r28;
        var_r3 = var_r29;
        var_r27 += temp_r18;
        var_r5_2 = (u32 *)arg4 + var_r28;
        while (var_r29 > (u32)(*var_r5_2 * 2)) {
            var_f29 = *var_r4_2;
            var_r4_2++;
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
        if (var_r3 < var_r29) {
            for (var_ctr = 0; var_ctr < temp_r18; var_ctr++) {
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
                    *var_r27 = temp_f31;
                    var_r27++;
                }
                var_r3++;
            }
        }
    }
    goto outer_test;

outer_test:
    if (var_r3 < (u32)arg1) goto decode;
    ((BitReader8011 *)arg2)->next = var_r26;
    ((BitReader8011 *)arg2)->value = var_r25;
    ((BitReader8011 *)arg2)->count = var_r24;
}

s32 fn_8011B2AC(s32 size, f32 gain, u32 channels, s32 flags, s32 arg3, s32 arg4, s32 output, u32 *input, u32 bands, s32 arg8) {
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
        p[0] = fn_8011AE18(index);
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
        p[1] = fn_8011AE18(index);
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
        fn_8011AF94(p, size, &bits, factors, arg8);
        if (mode) fn_8012023C(size, 1, p, (int *)arg3, (f32 *)arg4);
        else fn_801200BC(size, -1, p, (int *)arg3, (f32 *)arg4);
        p += size;
    }
    if (channels == 1) fn_8011AE68(output, buffer, size, gain);
    else fn_8011AED8(output, buffer, size, gain);
    bits.count = 0;
    return ((u32)bits.next - (u32)bits.start) & 0x1fffffff;
}


void fn_8011FE20(void **slot, s32 size);
extern u32 lbl_80329B60[25];  // critical band frequencies (Hz)

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
BinkAudio8011 *fn_8011B584(u32 rate, u32 channels, u32 flags) {
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
    fn_8011FE20((void **)&bands, (numBands + 1) * 4);
    fn_8011FE20((void **)&quant, ((s32)Sqrtf8011(half) + 2) * 4);
    if (dct) {
        fn_8011FE20(&transform, frameSize * 20 / 4);
    } else {
        fn_8011FE20(&transform, half * 4 - 1);
    }
    fn_8011FE20((void **)&previous, frameBytes / 2);
    fn_8011FE20((void **)&output, frameBytes);
    a = fn_8011FF2C(sizeof(BinkAudio8011));
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
void fn_8011B94C(BinkAudio8011 *a, s16 **out, u32 *outLen, u8 *src, u8 **next) {
    s32 used = fn_8011B2AC(a->frameSize, a->scale, a->channels, a->flags, (s32)a->quant, (s32)a->transform,
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
    fn_80120084((void *)arg0);
}

// Unlink node from the singly linked list at +0x30 of owner (next pointer at +0). Returns 1 if it was there.
s32 fn_8011BC4C(void *owner, void *node) {
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

void fn_8011BCAC(void **head, void *node) {
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
void fn_8011BD20(void *arg0) {
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
                    fn_8011BCAC(&done, n);
                }
            }
            n = *(void **)n;
        }
        for (n = done; n != 0; n = *(void **)((u8 *)n + 4)) {
            (*(void (**)(void *, s32))((u8 *)n + 0x18))(n, *(s32 *)((u8 *)arg0 + 0x34));
        }
    }
}

void fn_8011BDF0(s32 arg0) {
    s32 temp;
    s32 old;
    temp = arg0 - 8;
    if (temp != 0 && *(s32 *)(temp + 0x30) != 0) {
        old = OSEnableInterrupts();
        fn_8011BD20((void *)temp);
        OSRestoreInterrupts(old);
    }
}

// Creates a sound request queue for `count` entries (negative counts are taken as positive).
void *fn_8011BE48(s32 count) {
    void *q = fn_8011FFC4(0x40);
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
void fn_8011BEC8(void *arg0, void *arg1, s32 arg2, s32 arg3) {
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

void fn_8011C364(s32 arg0);
s32 fn_8011BF4C(s32 arg0, s32 arg1, s32 arg2) {
    s32 result = 0;
    void *owner;
    void *node;
    s32 level;
    owner = (void *)arg0;
    node = (void *)arg1;
    if (arg0 != 0 && arg1 != 0 && *(s32 *)(arg1 + 0x18) != 0 && *(s32 *)(arg1 + 0x14) != 0) {
        level = OSDisableInterrupts();
        if (fn_8011BC4C(owner, node) != 0) {
            *(s32 *)(arg0 + 0x38) -= 1;
            if ((arg2 & 1) && *(s32 *)(arg0 + 0x30) == 0) {
                fn_8011C364(arg0);
                result = 1;
            }
        }
        OSRestoreInterrupts(level);
    }
    return result;
}

s32 fn_8011BFFC(s32 arg0, s32 arg1, void *arg2, s32 arg3, s32 arg4) {
    s32 temp = fn_8011BF4C((s32)arg2, arg3, arg4) << 1;
    s32 result = fn_8011BF4C(arg0, arg1, arg4);
    return result | temp;
}

u32 fn_8011C06C(void *arg0) {
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

// Waits (yielding) until fn_8011C06C reports the request finished.
void fn_8011C0E0(void *arg0) {
    while (fn_8011C06C(arg0) == 0) {
        OSYieldThread();
    }
}

// Releases one hold on the queue; the last release starts its periodic tick (`rate` per second).
void fn_8011C120(void *arg0) {
    s32 level;
    if (arg0 != 0) {
        level = OSDisableInterrupts();
        // hold count, shared with the alarm handler
        *(volatile s32 *)((u8 *)arg0 + 4) -= 1;
        if (*(volatile s32 *)((u8 *)arg0 + 4) == 0) {
            if (*(s32 *)arg0 == 0) {
                OSSetPeriodicAlarm((OSAlarm *)((u8 *)arg0 + 8), OSGetTime(),
                                   (1000 / *(u32 *)((u8 *)arg0 + 0x3C)) * (*(u32 *)0x800000F8 / 4 / 1000),
                                   (OSAlarmHandler)fn_8011BDF0);
            }
            *(s32 *)arg0 = 1;
        }
        OSRestoreInterrupts(level);
    }
}

u32 fn_8011C1E0(s32 arg0, void *arg1) {
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

// Waits (yielding) until fn_8011C1E0 reports the request finished.
void fn_8011C274(s32 arg0, void *arg1) {
    if (arg0 != 0 && arg1 != 0 &&
        *(s32 *)((u8 *)arg1 + 0x18) != 0 &&
        *(s32 *)((u8 *)arg1 + 0x14) != 0) {
        do {
            OSYieldThread();
        } while (fn_8011C1E0(arg0, arg1) == 0);
    }
}

void fn_8011C2EC(s32 arg0, void *arg1) {
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

void fn_8011C364(s32 arg0) {
    if (arg0 != 0) {
        fn_8011C0E0((void *)arg0);
        OSCancelAlarm((OSAlarm *)(arg0 + 8));
        fn_80120084((void *)arg0);
    }
}

void fn_8011C3A8(s32 arg0, void *arg1, s32 arg2, u32 arg3, s32 arg4, s32 arg5, s32 arg6) {
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

s32 fn_8011C400(u32 arg0, u8 *arg1, u8 *arg2, u8 *arg3, s32 arg4) {
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
            temp = *right;
            var_r8--;
            right++;
        } else {
            temp = *left;
            var_r7--;
            left++;
        }
        *out = temp;
        out++;
        arg0 >>= 1;
        if (var_r7 == 0 || var_r8 == 0) break;
    } while (1);
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
        v = fn_8011C400(v, dst, a, b, size);          \
        BINK_SKIP(v);                                 \
    }

extern u8 *const lbl_80329DA8[16];  // Bink Huffman lookup tables
extern const u8 lbl_802F6990[16];   // their maximum code lengths

// Reads a Huffman tree: picks one of the 16 fixed code tables and reads the symbol order, either explicitly or as
// a series of merge passes over the identity order.
void fn_8011C4A0(BitReader8011 *bits, u8 **lookup, u32 *maxBits, u8 *syms) {
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
void fn_8011CD78(Bundle8011 *b, BitReader8011 *bits) {
    fn_8011C4A0(bits, &b->lookup, &b->maxBits, b->symbols);
}

// The 16 context trees used for colors (selected by the previous high nibble).
typedef struct BinkColorTrees8011 {
    u8 symbols[16][16];  // 0x000
    u32 maxBits[16];     // 0x100
    u8 *lookup[16];      // 0x140
    s32 last;            // 0x180 previous high nibble
} BinkColorTrees8011;

// Reads the 16 color context trees, then the bundle's own tree.
void fn_8011CD90(Bundle8011 *b, BitReader8011 *bits, BinkColorTrees8011 *trees) {
    u32 i;
    for (i = 0; i < 16; i++) {
        fn_8011C4A0(bits, &trees->lookup[i], &trees->maxBits[i], trees->symbols[i]);
    }
    fn_8011C4A0(bits, &b->lookup, &b->maxBits, b->symbols);
    trees->last = 0;
}

extern u8 lbl_80561208[];
void fn_8011CE18(void *arg0, void *arg1) {
    u32 *bits;
    u32 value, bitCount, available, mask, count, code, len, s, w;
    u8 fill;
    u8 *dst, *table, *lookup;
    u32 sym, runlen, j;
    if (*(u32 *)arg0 != *(u32 *)((u8 *)arg0 + 4)) return;
    bitCount = *(u32 *)((u8 *)arg1 + 8);
    available = *(u32 *)((u8 *)arg0 + 0x28);
    bits = *(u32 **)((u8 *)arg1 + 4);
    value = *(u32 *)arg1;
    if (bitCount < available) {
        u32 next = *bits++;
        count = (value | (next << bitCount)) & (0xFFFFFFFFU >> (32 - available));
        value = next >> (available - bitCount);
        bitCount = bitCount + 32 - available;
    } else {
        count = value & (0xFFFFFFFFU >> (32 - available));
        value >>= available;
        bitCount -= available;
    }
    if (count != 0) {
        dst = *(u8 **)((u8 *)arg0 + 0x2C);
        *(u8 **)arg0 = dst;
        *(u8 **)((u8 *)arg0 + 4) = dst + count;
        if (bitCount == 0) { w = *bits++; bitCount = 31; value = w >> 1; }
        else { w = value; value >>= 1; bitCount--; }
        if ((w & 1) == 0) {
            fill = 0;
            table = (u8 *)arg0 + 0x10;
            lookup = *(u8 **)((u8 *)arg0 + 0x24);
            available = *(u32 *)((u8 *)arg0 + 0x20);
            mask = 0xFFFFFFFFU >> (32 - available);
            while (count != 0) {
                if (bitCount >= available) {
                    code = value & mask;
                    s = lookup[code];
                    code = s & 15;
                    len = s >> 4;
                    sym = table[code];
                    value >>= len;
                    bitCount -= len;
                } else {
                    u32 next = *bits;
                    code = (value | (next << bitCount)) & mask;
                    s = lookup[code];
                    code = s & 15;
                    len = s >> 4;
                    sym = table[code];
                    if (bitCount >= len) { value >>= len; bitCount -= len; }
                    else { value = next >> (len - bitCount); bits++; bitCount = bitCount + 32 - len; }
                }
                if (sym >= 12) {
                    runlen = lbl_80561208[sym - 12];
                    count -= runlen;
                    for (j = 0; j < runlen; j++) {
                        *dst++ = fill;
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
void fn_8011D0E8(Bundle8011 *b, BitReader8011 *bits, BinkColorTrees8011 *trees) {
    u32 *next;
    u32 value;
    u32 count;
    s32 len;
    u32 width;
    u32 mask;
    u32 e;
    u32 bit;
    u32 last;
    u32 lo;
    u8 *dst;
    u8 *lookup;
    u8 *symbols;

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
        width = b->maxBits;
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
        mask = 0xFFFFFFFFU >> (32 - width);
        do {
            u32 w;
            u32 c;
            u32 v;
            len--;
            w = trees->maxBits[last];
            if (count >= w) {
                e = trees->lookup[last][value & (0xFFFFFFFFU >> (32 - w))];
                last = trees->symbols[last][e & 15];
                e >>= 4;
                value >>= e;
                count -= e;
            } else {
                u32 word = *next;
                e = trees->lookup[last][(value | (word << count)) & (0xFFFFFFFFU >> (32 - w))];
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

// Refills the colors bundle (older format): like fn_8011D0E8, with colors stored as plain bytes.
void fn_8011D378(Bundle8011 *b, BitReader8011 *bits, BinkColorTrees8011 *trees) {
    u32 *next;
    u32 value;
    u32 count;
    s32 len;
    u32 width;
    u32 mask;
    u32 e;
    u32 bit;
    u32 last;
    u32 lo;
    u8 *dst;
    u8 *lookup;
    u8 *symbols;

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
        width = b->maxBits;
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
        mask = 0xFFFFFFFFU >> (32 - width);
        do {
            u32 w;
            len--;
            w = trees->maxBits[last];
            if (count >= w) {
                e = trees->lookup[last][value & (0xFFFFFFFFU >> (32 - w))];
                last = trees->symbols[last][e & 15];
                e >>= 4;
                value >>= e;
                count -= e;
            } else {
                u32 word = *next;
                e = trees->lookup[last][(value | (word << count)) & (0xFFFFFFFFU >> (32 - w))];
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
void fn_8011D5F0(Bundle8011 *b, BitReader8011 *bits) {
    u32 *next;
    u32 value;
    u32 count;
    u32 len, bit, mask, fill, e;
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
            width = b->maxBits;
            mask = 0xFFFFFFFFU >> (32 - width);
            dst = b->data;
            lookup = b->lookup;
            symbols = b->symbols;
            do {
                len--;
                if (count >= width) {
                    e = lookup[value & mask];
                    *dst++ = symbols[e & 15];
                    e >>= 4;
                    value >>= e;
                    count -= e;
                } else {
                    u32 word = *next;
                    e = lookup[(value | (word << count)) & mask];
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
void fn_8011D7F8(Bundle8011 *b, BitReader8011 *bits) {
    u32 *next;
    u32 value;
    u32 count;
    u32 len, mask, e, lo, hi;
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
        dst = b->data;
        b->cur = dst;
        b->end = dst + len;
        width = b->maxBits;
        mask = 0xFFFFFFFFU >> (32 - width);
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

void fn_8011D9E4(void *arg0, void *arg1) {
    u32 *bits;
    u32 value, bitCount, available, mask, count, i, code, len;
    u8 *dst, *table, *lookup;
    s32 symbol;
    if (*(u32 *)arg0 != *(u32 *)((u8 *)arg0 + 4)) return;
    bitCount = *(u32 *)((u8 *)arg1 + 8);
    available = *(u32 *)((u8 *)arg0 + 0x28);
    bits = *(u32 **)((u8 *)arg1 + 4);
    value = *(u32 *)arg1;
    if (bitCount < available) {
        u32 next = *bits++;
        count = (value | (next << bitCount)) & (0xFFFFFFFFU >> (32 - available));
        value = next >> (available - bitCount);
        bitCount = bitCount + 32 - available;
    } else {
        count = value & (0xFFFFFFFFU >> (32 - available));
        value >>= available;
        bitCount -= available;
    }
    if (count != 0) {
        dst = *(u8 **)((u8 *)arg0 + 0x2C);
        *(u8 **)arg0 = dst;
        *(u8 **)((u8 *)arg0 + 4) = dst + count;
        if (bitCount == 0) { value = *bits++ >> 1; bitCount = 31; }
        else { bitCount--; value >>= 1; }
        if ((value & 1) == 0) {
            table = (u8 *)arg0 + 0x10;
            lookup = *(u8 **)((u8 *)arg0 + 0x24);
            available = *(u32 *)((u8 *)arg0 + 0x20);
            mask = 0xFFFFFFFFU >> (32 - available);
            for (i = 0; i < count; i++) {
                if (bitCount >= available) {
                    code = value & mask; symbol = lookup[code]; len = symbol >> 4;
                    symbol = table[symbol & 15]; value >>= len; bitCount -= len;
                } else {
                    u32 next = *bits;
                    code = (value | (next << bitCount)) & mask;
                    symbol = lookup[code]; len = symbol >> 4; symbol = table[symbol & 15];
                    if (bitCount >= len) { value >>= len; bitCount -= len; }
                    else { value = next >> (len - bitCount); bits++; bitCount = bitCount + 32 - len; }
                }
                if (symbol != 0) {
                    if (bitCount == 0) { value = *bits++ >> 1; bitCount = 31; }
                    else { bitCount--; value >>= 1; }
                    if (value & 1) symbol = -symbol;
                }
                *dst++ = symbol;
            }
        } else {
            if (bitCount < 4) {
                u32 next = *bits++;
                code = (value | (next << bitCount)) & 15;
                value = next >> (4 - bitCount); bitCount += 28;
            } else { code = value & 15; value >>= 4; bitCount -= 4; }
            if (code != 0) {
                if (bitCount == 0) { value = *bits++ >> 1; bitCount = 31; }
                else { bitCount--; value >>= 1; }
                if (value & 1) code = -code;
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

void fn_8011DC5C(void *arg0, void *arg1) {
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
                fn_80116CC4((u32 *)dst, (u16)val, (n + n));
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

s32 fn_8011DF68(s32 arg0, u32 arg1, u32 arg2, u32 arg3) {
    u32 var_r5 = arg2;
    u32 temp;
    if (var_r5 <= 8U) var_r5 = 8;
    else if (var_r5 < 0x10U) var_r5 = 0x10;
    temp = var_r5 * arg3;
    return (((u32)(arg0 * var_r5) >> 3) + ((temp * (arg1 >> 3)) >> 3) + 3) & 0xFFFFFFFC;
}

void fn_8011DFAC(void* arg0, u32 arg1) {
    s32* out = (s32*)arg0;
    out[0] = fn_8011DF68(0x200, arg1, 4, 1);
    out[1] = fn_8011DF68(0x200, arg1 >> 1, 4, 1);
    out[2] = fn_8011DF68(0x200, arg1, 8, 0x40);
    out[3] = fn_8011DF68(0x200, arg1, 8, 8);
    out[4] = fn_8011DF68(0x200, arg1, 5, 1);
    out[5] = fn_8011DF68(0x200, arg1, 5, 1);
    out[6] = fn_8011DF68(0x200, arg1, 0xB, 1);
    out[7] = fn_8011DF68(0x200, arg1, 0xB, 1);
    out[8] = fn_8011DF68(0x200, arg1, 4, 0x30);
}

#pragma opt_unroll_loops on
extern u32 lbl_802F65C0[];  // Bink pixel masks (pattern blocks use the tables at +0x3E0..+0x47F)
extern u8 lbl_802F8A40[16][64];  // run block scan orders
#define BINK_PAT_C1 ((u32 *)((u8 *)lbl_802F65C0 + 0x3E0))         // 4-pixel masks, 1 bit per pixel
#define BINK_PAT_C0 ((u32 *)((u8 *)lbl_802F65C0 + 0x420))
#define BINK_PAT2_C1 ((u32 *)((u8 *)lbl_802F65C0 + 0x460))        // doubled 2-pixel masks
#define BINK_PAT2_C0 ((u32 *)((u8 *)lbl_802F65C0 + 0x470))

void fn_80126018(s16 *block, BitReader8011 *bits);
void fn_80126EF0(u8 *dst, s32 dstStride, BitReader8011 *bits, u32 limit, u8 *src, s32 srcStride);
void fn_80125CA0(void *dst, void *stride, void *block, s32 quant);
void fn_80125CB4(void *dst, void *stride, void *block, s32 quant);
void fn_80125CC8(u8 *dst, s32 stride, s16 *block, s32 q, u8 *src, s32 srcStride);

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

// Decodes one Bink plane (8x8 blocks, or 16x16 scaled blocks) into dst, using prev as the previous frame.
// `step` is the block-map step per block (1 for luma, which shares map entries between block pairs, 2 for
// chroma). Stores the number of skipped blocks whose map entry was clear in *skipped and returns the end of the
// plane's data.
u32 *fn_8011E0B4(u8 *dst, u8 *prev, u32 width, u32 height, s32 pitch, u32 *data, s32 keyframe, u8 *blockMap,
                 s32 blocksWide, s32 step, void **bufs, u32 flags, u32 *skipped) {
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
    void (*readColors)(void *b, void *bits, void *trees);
    s32 rowSkip;
    u32 skipCount;
    u32 idx;
    u32 y;
    u32 x;
    u8 *src;
    s32 i;

    rowSkip = pitch * 8 - width;
    idx = 0;
    skipCount = 0;
    bits.start = data;
    bits.next = data;
    bits.count = 0;
    bits.value = 0;
    if (flags & 0x8000) {
        readColors = (void (*)(void *, void *, void *))fn_8011D0E8;
    } else {
        readColors = (void (*)(void *, void *, void *))fn_8011D378;
    }
    fn_8011C3A8((s32)bufs[0], &types, 0x200, width, 4, 1, 0);
    fn_8011C3A8((s32)bufs[1], &subTypes, 0x200, width >> 1, 4, 1, 0);
    fn_8011C3A8((s32)bufs[2], &colors, 0x200, width, 8, 64, 0);
    fn_8011C3A8((s32)bufs[3], &pattern, 0x200, width, 8, 8, 0);
    fn_8011C3A8((s32)bufs[4], &xOff, 0x200, width, 5, 1, 1);
    fn_8011C3A8((s32)bufs[5], &yOff, 0x200, width, 5, 1, 1);
    fn_8011C3A8((s32)bufs[6], &intraDC, 0x200, width, 11, 1, 0);
    fn_8011C3A8((s32)bufs[7], &interDC, 0x200, width, 11, 1, 1);
    fn_8011C3A8((s32)bufs[8], &runs, 0x200, width, 4, 48, 0);
    fn_8011CD78(&types, &bits);
    fn_8011CD78(&subTypes, &bits);
    fn_8011CD90(&colors, &bits, &trees);
    fn_8011CD78(&pattern, &bits);
    fn_8011CD78(&xOff, &bits);
    fn_8011CD78(&yOff, &bits);
    fn_8011CD78(&runs, &bits);
    for (y = 0; y < height; y += 8) {
        fn_8011CE18(&types, &bits);
        fn_8011CE18(&subTypes, &bits);
        readColors(&colors, &bits, &trees);
        fn_8011D7F8(&pattern, &bits);
        fn_8011D9E4(&xOff, &bits);
        fn_8011D9E4(&yOff, &bits);
        fn_8011DC5C(&intraDC, &bits);
        fn_8011DC5C(&interDC, &bits);
        fn_8011D5F0(&runs, &bits);
        for (x = width; x != 0; x -= 8, dst += 8, prev += 8, idx += step) {
            switch (*types.cur++) {
            case 0:  // skip: keep the previous frame's block
                for (i = 0; i < 8; i++) {
                    *(f64 *)(dst + i * pitch) = *(f64 *)(prev + i * pitch);
                }
                if (blockMap[idx >> 1] == 0) {
                    skipCount++;
                }
                break;
            case 1:  // scaled: one 8x8 block drawn as 16x16
                if (!(y & 8)) {
                    switch (*subTypes.cur++) {
                    case 3: {
                        u32 value = bits.value;
                        u32 *next = bits.next;
                        u32 avail = bits.count;
                        u8 *scan;
                        u32 v;
                        u32 bit;
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
                        {
                            u8 *d0 = dst;
                            u8 *d1 = dst + pitch;
                            u16 *s = (u16 *)tmp;
                            for (i = 0; i < 8; i++) {
                                u32 w;
                                w = s[0];
                                w = ((w & 0xFF00) << 16) | ((w & 0xFF00) << 8) | ((w & 0xFF) << 8) | (w & 0xFF);
                                ((u32 *)d0)[0] = w;
                                ((u32 *)d1)[0] = w;
                                w = s[1];
                                w = ((w & 0xFF00) << 16) | ((w & 0xFF00) << 8) | ((w & 0xFF) << 8) | (w & 0xFF);
                                ((u32 *)d0)[1] = w;
                                ((u32 *)d1)[1] = w;
                                w = s[2];
                                w = ((w & 0xFF00) << 16) | ((w & 0xFF00) << 8) | ((w & 0xFF) << 8) | (w & 0xFF);
                                ((u32 *)d0)[2] = w;
                                ((u32 *)d1)[2] = w;
                                w = s[3];
                                w = ((w & 0xFF00) << 16) | ((w & 0xFF00) << 8) | ((w & 0xFF) << 8) | (w & 0xFF);
                                ((u32 *)d0)[3] = w;
                                ((u32 *)d1)[3] = w;
                                s += 4;
                                d0 = d1 + pitch;
                                d1 = d0 + pitch;
                            }
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
                        fn_80126018(block, &bits);
                        BINK_BR_GETBITS(&bits, quant, 4);
                        fn_80125CB4(dst, (void *)pitch, block, quant);
                        break;
                    }
                    case 6: {
                        u32 c = *colors.cur++;
                        u8 *d = dst;
                        c |= c << 8;
                        c |= c << 16;
                        for (i = 0; i < 16; i++) {
                            ((u32 *)d)[0] = c;
                            ((u32 *)d)[1] = c;
                            ((u32 *)d)[2] = c;
                            ((u32 *)d)[3] = c;
                            d += pitch;
                        }
                        break;
                    }
                    case 8: {
                        u32 c0 = *colors.cur++;
                        u32 c1 = *colors.cur++;
                        u8 *d0 = dst;
                        u8 *d1 = dst + pitch;
                        c0 |= c0 << 8;
                        c0 |= c0 << 16;
                        c1 |= c1 << 8;
                        c1 |= c1 << 16;
                        for (i = 0; i < 8; i++) {
                            u32 p = *pattern.cur++;
                            u32 w;
                            w = (c0 & BINK_PAT2_C0[p & 3]) | (c1 & BINK_PAT2_C1[p & 3]);
                            ((u32 *)d0)[0] = w;
                            ((u32 *)d1)[0] = w;
                            w = (c0 & BINK_PAT2_C0[(p >> 2) & 3]) | (c1 & BINK_PAT2_C1[(p >> 2) & 3]);
                            ((u32 *)d0)[1] = w;
                            ((u32 *)d1)[1] = w;
                            w = (c0 & BINK_PAT2_C0[(p >> 4) & 3]) | (c1 & BINK_PAT2_C1[(p >> 4) & 3]);
                            ((u32 *)d0)[2] = w;
                            ((u32 *)d1)[2] = w;
                            w = (c0 & BINK_PAT2_C0[p >> 6]) | (c1 & BINK_PAT2_C1[p >> 6]);
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
                        if ((u32)colors.cur & 3) {
                            for (i = 0; i < 8; i++) {
                                u32 w;
                                w = (colors.cur[0] << 24) | (colors.cur[0] << 16) | (colors.cur[1] << 8) | colors.cur[1];
                                ((u32 *)d0)[0] = w;
                                ((u32 *)d1)[0] = w;
                                w = (colors.cur[2] << 24) | (colors.cur[2] << 16) | (colors.cur[3] << 8) | colors.cur[3];
                                ((u32 *)d0)[1] = w;
                                ((u32 *)d1)[1] = w;
                                w = (colors.cur[4] << 24) | (colors.cur[4] << 16) | (colors.cur[5] << 8) | colors.cur[5];
                                ((u32 *)d0)[2] = w;
                                ((u32 *)d1)[2] = w;
                                w = (colors.cur[6] << 24) | (colors.cur[6] << 16) | (colors.cur[7] << 8) | colors.cur[7];
                                ((u32 *)d0)[3] = w;
                                ((u32 *)d1)[3] = w;
                                d0 = d1 + pitch;
                                d1 = d0 + pitch;
                                colors.cur += 8;
                            }
                        } else {
                            for (i = 0; i < 8; i++) {
                                u32 w;
                                w = ((u16 *)colors.cur)[0];
                                w = ((w & 0xFF00) << 16) | ((w & 0xFF00) << 8) | ((w & 0xFF) << 8) | (w & 0xFF);
                                ((u32 *)d0)[0] = w;
                                ((u32 *)d1)[0] = w;
                                w = ((u16 *)colors.cur)[1];
                                w = ((w & 0xFF00) << 16) | ((w & 0xFF00) << 8) | ((w & 0xFF) << 8) | (w & 0xFF);
                                ((u32 *)d0)[1] = w;
                                ((u32 *)d1)[1] = w;
                                w = ((u16 *)colors.cur)[2];
                                w = ((w & 0xFF00) << 16) | ((w & 0xFF00) << 8) | ((w & 0xFF) << 8) | (w & 0xFF);
                                ((u32 *)d0)[2] = w;
                                ((u32 *)d1)[2] = w;
                                w = ((u16 *)colors.cur)[3];
                                w = ((w & 0xFF00) << 16) | ((w & 0xFF00) << 8) | ((w & 0xFF) << 8) | (w & 0xFF);
                                ((u32 *)d0)[3] = w;
                                ((u32 *)d1)[3] = w;
                                d0 = d1 + pitch;
                                d1 = d0 + pitch;
                                colors.cur += 8;
                            }
                        }
                        break;
                    }
                    }
                }
                blockMap[idx >> 1] = 1;
                idx += step;
                blockMap[idx >> 1] = 1;
                x -= 8;
                dst += 8;
                prev += 8;
                break;
            case 2:  // motion: copy a block of the previous frame
                blockMap[idx >> 1] = 1;
                src = prev + *(s8 *)yOff.cur++ * pitch;
                src += *(s8 *)xOff.cur++;
                if ((((u32)dst | (u32)src) & 7) == 0) {
                    u8 *d = dst;
                    for (i = 0; i < 8; i++) {
                        *(f64 *)d = *(f64 *)src;
                        d += pitch;
                        src += pitch;
                    }
                } else if ((((u32)dst | (u32)src) & 3) == 0) {
                    u8 *d = dst;
                    for (i = 0; i < 8; i++) {
                        ((u32 *)d)[0] = ((u32 *)src)[0];
                        ((u32 *)d)[1] = ((u32 *)src)[1];
                        d += pitch;
                        src += pitch;
                    }
                } else {
                    u8 *d = dst;
                    d[0] = src[0];
                    d[1] = src[1];
                    d[2] = src[2];
                    d[3] = src[3];
                    d[4] = src[4];
                    d[5] = src[5];
                    d[6] = src[6];
                    d[7] = src[7];
                    d += pitch;
                    src += pitch;
                    d[0] = src[0];
                    d[1] = src[1];
                    d[2] = src[2];
                    d[3] = src[3];
                    d[4] = src[4];
                    d[5] = src[5];
                    d[6] = src[6];
                    d[7] = src[7];
                    d += pitch;
                    src += pitch;
                    d[0] = src[0];
                    d[1] = src[1];
                    d[2] = src[2];
                    d[3] = src[3];
                    d[4] = src[4];
                    d[5] = src[5];
                    d[6] = src[6];
                    d[7] = src[7];
                    d += pitch;
                    src += pitch;
                    d[0] = src[0];
                    d[1] = src[1];
                    d[2] = src[2];
                    d[3] = src[3];
                    d[4] = src[4];
                    d[5] = src[5];
                    d[6] = src[6];
                    d[7] = src[7];
                    d += pitch;
                    src += pitch;
                    d[0] = src[0];
                    d[1] = src[1];
                    d[2] = src[2];
                    d[3] = src[3];
                    d[4] = src[4];
                    d[5] = src[5];
                    d[6] = src[6];
                    d[7] = src[7];
                    d += pitch;
                    src += pitch;
                    d[0] = src[0];
                    d[1] = src[1];
                    d[2] = src[2];
                    d[3] = src[3];
                    d[4] = src[4];
                    d[5] = src[5];
                    d[6] = src[6];
                    d[7] = src[7];
                    d += pitch;
                    src += pitch;
                    d[0] = src[0];
                    d[1] = src[1];
                    d[2] = src[2];
                    d[3] = src[3];
                    d[4] = src[4];
                    d[5] = src[5];
                    d[6] = src[6];
                    d[7] = src[7];
                    d += pitch;
                    src += pitch;
                    d[0] = src[0];
                    d[1] = src[1];
                    d[2] = src[2];
                    d[3] = src[3];
                    d[4] = src[4];
                    d[5] = src[5];
                    d[6] = src[6];
                    d[7] = src[7];
                }
                break;
            case 3: {  // run: runs of one color or of literal colors along a scan order
                u32 value = bits.value;
                u32 *next = bits.next;
                u32 avail = bits.count;
                u8 *scan;
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
                for (i = 0; i < 8; i++) {
                    *(f64 *)(dst + i * pitch) = ((f64 *)tmp)[i];
                }
                bits.next = next;
                bits.value = value;
                bits.count = avail;
                break;
            }
            case 4: {  // residue on top of a motion copy
                u32 n;
                blockMap[idx >> 1] = 1;
                src = prev + *(s8 *)yOff.cur++ * pitch;
                src += *(s8 *)xOff.cur++;
                BINK_BR_GETBITS(&bits, n, 7);
                fn_80126EF0(dst, pitch, &bits, n, src, pitch);
                break;
            }
            case 5: {  // intra DCT block
                s32 quant;
                blockMap[idx >> 1] = 1;
                block[0] = *(s16 *)intraDC.cur;
                intraDC.cur += 2;
                fn_80126018(block, &bits);
                BINK_BR_GETBITS(&bits, quant, 4);
                fn_80125CA0(dst, (void *)pitch, block, quant);
                break;
            }
            case 6: {  // fill with one color
                u32 c = *colors.cur++;
                blockMap[idx >> 1] = 1;
                c |= c << 8;
                c |= c << 16;
                for (i = 0; i < 8; i++) {
                    ((u32 *)(dst + i * pitch))[0] = c;
                    ((u32 *)(dst + i * pitch))[1] = c;
                }
                break;
            }
            case 7: {  // inter DCT block on top of a motion copy
                s32 quant;
                blockMap[idx >> 1] = 1;
                src = prev + *(s8 *)yOff.cur++ * pitch;
                src += *(s8 *)xOff.cur++;
                block[0] = *(s16 *)interDC.cur;
                interDC.cur += 2;
                fn_80126018(block, &bits);
                BINK_BR_GETBITS(&bits, quant, 4);
                fn_80125CC8(dst, pitch, block, quant, src, pitch);
                break;
            }
            case 8: {  // two-color pattern
                u32 c0 = colors.cur[0];
                u32 c1;
                colors.cur++;
                c1 = *colors.cur++;
                c0 |= c0 << 8;
                c0 |= c0 << 16;
                blockMap[idx >> 1] = 1;
                c1 |= c1 << 8;
                c1 |= c1 << 16;
                for (i = 0; i < 8; i++) {
                    u32 p = *pattern.cur++;
                    ((u32 *)(dst + i * pitch))[0] = (c0 & BINK_PAT_C0[p & 15]) | (c1 & BINK_PAT_C1[p & 15]);
                    ((u32 *)(dst + i * pitch))[1] = (c0 & BINK_PAT_C0[p >> 4]) | (c1 & BINK_PAT_C1[p >> 4]);
                }
                break;
            }
            case 9:  // raw pixels
                blockMap[idx >> 1] = 1;
                if ((((u32)dst | (u32)colors.cur) & 7) == 0) {
                    u8 *d = dst;
                    *(f64 *)d = ((f64 *)colors.cur)[0];
                    d += pitch;
                    *(f64 *)d = ((f64 *)colors.cur)[1];
                    d += pitch;
                    *(f64 *)d = ((f64 *)colors.cur)[2];
                    d += pitch;
                    *(f64 *)d = ((f64 *)colors.cur)[3];
                    d += pitch;
                    *(f64 *)d = ((f64 *)colors.cur)[4];
                    d += pitch;
                    *(f64 *)d = ((f64 *)colors.cur)[5];
                    d += pitch;
                    *(f64 *)d = ((f64 *)colors.cur)[6];
                    d += pitch;
                    *(f64 *)d = ((f64 *)colors.cur)[7];
                } else if ((((u32)dst | (u32)colors.cur) & 3) == 0) {
                    u8 *d = dst;
                    ((u32 *)d)[0] = ((u32 *)colors.cur)[0];
                    ((u32 *)d)[1] = ((u32 *)colors.cur)[1];
                    d += pitch;
                    ((u32 *)d)[0] = ((u32 *)colors.cur)[2];
                    ((u32 *)d)[1] = ((u32 *)colors.cur)[3];
                    d += pitch;
                    ((u32 *)d)[0] = ((u32 *)colors.cur)[4];
                    ((u32 *)d)[1] = ((u32 *)colors.cur)[5];
                    d += pitch;
                    ((u32 *)d)[0] = ((u32 *)colors.cur)[6];
                    ((u32 *)d)[1] = ((u32 *)colors.cur)[7];
                    d += pitch;
                    ((u32 *)d)[0] = ((u32 *)colors.cur)[8];
                    ((u32 *)d)[1] = ((u32 *)colors.cur)[9];
                    d += pitch;
                    ((u32 *)d)[0] = ((u32 *)colors.cur)[10];
                    ((u32 *)d)[1] = ((u32 *)colors.cur)[11];
                    d += pitch;
                    ((u32 *)d)[0] = ((u32 *)colors.cur)[12];
                    ((u32 *)d)[1] = ((u32 *)colors.cur)[13];
                    d += pitch;
                    ((u32 *)d)[0] = ((u32 *)colors.cur)[14];
                    ((u32 *)d)[1] = ((u32 *)colors.cur)[15];
                } else {
                    u8 *d = dst;
                    d[0] = colors.cur[0];
                    d[1] = colors.cur[1];
                    d[2] = colors.cur[2];
                    d[3] = colors.cur[3];
                    d[4] = colors.cur[4];
                    d[5] = colors.cur[5];
                    d[6] = colors.cur[6];
                    d[7] = colors.cur[7];
                    d += pitch;
                    d[0] = colors.cur[8];
                    d[1] = colors.cur[9];
                    d[2] = colors.cur[10];
                    d[3] = colors.cur[11];
                    d[4] = colors.cur[12];
                    d[5] = colors.cur[13];
                    d[6] = colors.cur[14];
                    d[7] = colors.cur[15];
                    d += pitch;
                    d[0] = colors.cur[16];
                    d[1] = colors.cur[17];
                    d[2] = colors.cur[18];
                    d[3] = colors.cur[19];
                    d[4] = colors.cur[20];
                    d[5] = colors.cur[21];
                    d[6] = colors.cur[22];
                    d[7] = colors.cur[23];
                    d += pitch;
                    d[0] = colors.cur[24];
                    d[1] = colors.cur[25];
                    d[2] = colors.cur[26];
                    d[3] = colors.cur[27];
                    d[4] = colors.cur[28];
                    d[5] = colors.cur[29];
                    d[6] = colors.cur[30];
                    d[7] = colors.cur[31];
                    d += pitch;
                    d[0] = colors.cur[32];
                    d[1] = colors.cur[33];
                    d[2] = colors.cur[34];
                    d[3] = colors.cur[35];
                    d[4] = colors.cur[36];
                    d[5] = colors.cur[37];
                    d[6] = colors.cur[38];
                    d[7] = colors.cur[39];
                    d += pitch;
                    d[0] = colors.cur[40];
                    d[1] = colors.cur[41];
                    d[2] = colors.cur[42];
                    d[3] = colors.cur[43];
                    d[4] = colors.cur[44];
                    d[5] = colors.cur[45];
                    d[6] = colors.cur[46];
                    d[7] = colors.cur[47];
                    d += pitch;
                    d[0] = colors.cur[48];
                    d[1] = colors.cur[49];
                    d[2] = colors.cur[50];
                    d[3] = colors.cur[51];
                    d[4] = colors.cur[52];
                    d[5] = colors.cur[53];
                    d[6] = colors.cur[54];
                    d[7] = colors.cur[55];
                    d += pitch;
                    d[0] = colors.cur[56];
                    d[1] = colors.cur[57];
                    d[2] = colors.cur[58];
                    d[3] = colors.cur[59];
                    d[4] = colors.cur[60];
                    d[5] = colors.cur[61];
                    d[6] = colors.cur[62];
                    d[7] = colors.cur[63];
                }
                colors.cur += 64;
                break;
            }
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
#pragma opt_unroll_loops reset

// Decodes a video frame into the next frame buffer (alpha, luma, then the two chroma planes); returns the number
// of skipped blocks in the luma plane (in block pairs).
u32 fn_8011FC2C(void *video, s32 blockMap, s32 blocksWide, s32 *data, s32 keyframe, s32 bufs, s32 flags,
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
            fn_8011E0B4(v->frames[cur].a.buffer, v->frames[old].a.buffer, v->width, v->height,
                        v->frames[old].a.pitch, (u32 *)(data + 1), keyframe, (u8 *)blockMap, blocksWide, 1,
                        (void **)bufs, flags, &result);
        }
        data = (s32 *)((u8 *)data + *data);
    }
    result = 0;
    if (!(flags & 0x8000)) {
        data++;
    }
    end = fn_8011E0B4(v->frames[cur].y.buffer, v->frames[old].y.buffer, v->width, v->height, v->frames[old].y.pitch,
                      (u32 *)data, keyframe, (u8 *)blockMap, blocksWide, 1, (void **)bufs, flags, &result);
    if (!(flags & 0x8000)) {
        end = (u32 *)((u8 *)data + data[-1] - 4);
    }
    result >>= 2;
    if (!(flags & 0x20000)) {
        end = fn_8011E0B4(v->frames[cur].cr.buffer, v->frames[old].cr.buffer, v->cWidth, v->cHeight,
                          v->frames[old].cr.pitch, end, keyframe, (u8 *)blockMap, blocksWide, 2, (void **)bufs,
                          flags, &result);
        result = 0;
        fn_8011E0B4(v->frames[cur].cb.buffer, v->frames[old].cb.buffer, v->cWidth, v->cHeight,
                    v->frames[old].cb.pitch, end, keyframe, (u8 *)blockMap, blocksWide, 2, (void **)bufs, flags,
                    &result);
    }
    return result;
}

extern void *lbl_8055DEF0;
extern void *lbl_8055DEF0;
extern u32 lbl_8055DEF4;
// Registers *slot for the next batched allocation (fn_8011FF2C). Sizes are rounded to 32 bytes and padded so
// consecutive buffers start in different cache sets; the registration arrays double when full.
void fn_8011FE20(void **slot, s32 size) {
    void ***slots;
    u32 *sizes;
    u32 padded;
    if (lbl_80559950 == lbl_8055DEF4) {
        slots = fn_8011FFC4(lbl_80559950 * 16);
        sizes = (u32 *)((u8 *)slots + lbl_80559950 * 8);
        memcpy(slots, lbl_80559948, lbl_80559950 * 4);
        memcpy(sizes, lbl_8055994C, lbl_80559950 * 4);
        lbl_80559950 *= 2;
        if (lbl_80559950 != 64) {
            fn_80120084(lbl_80559948);
        }
        lbl_80559948 = slots;
        lbl_8055994C = sizes;
    }
    padded = ((size + 31) & ~31) +
             (((((u32)lbl_8055DEF0 >> 5) & 31) + 33 - (((u32)(size + 31) >> 5) & 31)) << 5 & 0x3E0);
    lbl_8055DEF0 = (u8 *)lbl_8055DEF0 + padded;
    lbl_8055994C[lbl_8055DEF4] = padded;
    lbl_80559948[lbl_8055DEF4++] = slot;
}

void* fn_8011FF24(void) {
    return lbl_8055DEF0;
}

extern u32 lbl_8055DEF4;
// Allocates one block for all the pending buffers, hands each its slice and returns the block.
void *fn_8011FF2C(s32 arg0) {
    s32 size = (arg0 + 0x1F) & ~0x1F;
    u32 i;
    u8 *block = fn_8011FFC4((s32)lbl_8055DEF0 + size);
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
extern void *fn_8011FFC4(s32);
void *fn_8011FFC4(s32 arg0) {
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

// Frees a block from fn_8011FFC4: the bytes before it hold the alignment offset and which allocator made it.
void fn_80120084(void *arg0) {
    u8 *ptr = arg0;
    if (ptr == 0) return;
    if (ptr[-2] == 3) ((void (*)(void *))*(void **)(ptr - 8))(ptr - ptr[-1]);
    else free(ptr - ptr[-1]);
}

void fn_801200BC(int n, int isgn, f32 *a, int *ip, f32 *w) {
    int nw, nc;
    f32 xi;

    nw = ip[0];
    if (n > (nw << 2)) {
        nw = n >> 2;
        fn_80120834(nw, ip, w);
    }
    nc = ip[1];
    if (n > (nc << 2)) {
        nc = n >> 2;
        fn_80120B6C(nc, ip, w + nw);
    }
    if (isgn >= 0) {
        if (n > 4) {
            fn_80120CA4(n, a, ip + 2, nw, w);
            fn_801250CC(n, a, nc, w + nw);
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
            fn_801252D8(n, a, nc, w + nw);
            fn_80120E14(n, a, ip + 2, nw, w);
        } else if (n == 4) {
            fn_80120E14(n, a, ip + 2, nw, w);
        }
    }
}

void fn_8012023C(int n, int isgn, f32 *a, int *ip, f32 *w) {
    int j, nw, nc;
    f32 xr;

    nw = ip[0];
    if (n > (nw << 2)) {
        nw = n >> 2;
        fn_80120834(nw, ip, w);
    }
    nc = ip[1];
    if (n > nc) {
        nc = n;
        fn_80120B6C(nc, ip, w + nw);
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
            fn_801252D8(n, a, nc, w + nw);
            fn_80120E14(n, a, ip + 2, nw, w);
        } else if (n == 4) {
            fn_80120E14(n, a, ip + 2, nw, w);
        }
    }
    fn_801254E4(n, a, nc, w + nw);
    if (isgn >= 0) {
        if (n > 4) {
            fn_80120CA4(n, a, ip + 2, nw, w);
            fn_801250CC(n, a, nc, w + nw);
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

void fn_80120834(int n, int *info, f32 *table) {
    s32 i;
    s32 stage;
    s32 half;
    s32 j;
    f32 endpoint;
    f32 angle;
    f32 angle2;
    f32 stage_scale;
    f32 stage_zero;

    info[0] = n;
    info[1] = 1;
    if (n <= 2)
        return;

    half = n >> 1;
    angle = lbl_80561214 / (f32)half;
    endpoint = (f32)cos((f64)(angle * (f32)half));
    table[0] = lbl_80561218;
    table[1] = endpoint;

    if (half >= 4) {
        f32 first = (f32)cos((f64)(lbl_8056121C * angle));
        f32 second_angle = lbl_80561220 * angle;
        table[2] = lbl_80561210 / first;
        table[3] = lbl_80561210 / (f32)cos((f64)second_angle);
    }

    angle2 = lbl_80561224 * angle;
    for (i = 4; i < half; i += 4) {
        table[i + 0] = (f32)cos((f64)(angle * (f32)i));
        table[i + 1] = (f32)sin((f64)(angle * (f32)i));
        table[i + 2] = (f32)cos((f64)(angle2 * (f32)i));
        table[i + 3] = (f32)sin((f64)(angle2 * (f32)i));
    }

    stage_scale = lbl_80561210;
    stage_zero = lbl_80561218;
    i = 0;
    while (half > 2) {
        f32 *stagep;
        stage = i + half;
        half >>= 1;
        stagep = table + stage;
        stagep[0] = stage_zero;
        stagep[1] = endpoint;
        if (half >= 4) {
            f32 source_a = table[i + 4];
            f32 source_b = table[i + 6];
            stagep[2] = stage_scale / source_a;
            stagep[3] = stage_scale / source_b;
        }
        if (half > 4) {
            u8 *src = (u8 *)(table + i);
            u8 *dst = (u8 *)(stagep + 4);
            s32 offset = 0x20;
            for (j = 0; j < ((half - 1) >> 2); j++) {
                f32 t0 = *(f32 *)(src + offset);
                f32 t1 = *(f32 *)(src + offset + 4);
                f32 t2 = *(f32 *)(src + offset + 8);
                f32 t3 = *(f32 *)(src + offset + 12);
                *(f32 *)(dst + 0) = t0;
                *(f32 *)(dst + 4) = t1;
                *(f32 *)(dst + 8) = t2;
                *(f32 *)(dst + 12) = t3;
                offset += 0x20;
                dst += 0x10;
            }
        }
        i = stage;
    }
}

extern f64 cos(f64);
extern f64 sin(f64);
extern f32 lbl_80561210;
extern f32 lbl_80561214;
extern f64 lbl_80561228;
void fn_80120B6C(int arg0, int *arg1, f32 *arg2) {
    s32 i;
    s32 half;
    f32 scale;
    f32 angle;
    f32 result;

    arg1[1] = arg0;
    if (arg0 <= 1) return;
    half = arg0 >> 1;
    angle = lbl_80561214 / (f32)half;
    result = (f32)cos((f64)(angle * (f32)half));
    scale = lbl_80561210;
    arg2[0] = result;
    arg2[half] = scale * result;
    for (i = 1; i < half; i++) {
        arg2[i] = scale * (f32)cos((f64)(angle * (f32)i));
        arg2[arg0 - i] = scale * (f32)sin((f64)(angle * (f32)i));
    }
}

void fn_80120CA4(int n, f32 *a, int *ip, int nw, f32 *w) {
    int m;

    if (n > 32) {
        m = n >> 2;
        fn_80121C68(n, a, &w[nw - m]);
        if (n > 512) {
            fn_80122E08(m, a, nw, w);
            fn_80122ED8(m, &a[m], nw, w);
            fn_80122E08(m, &a[2 * m], nw, w);
            fn_80122E08(m, &a[3 * m], nw, w);
        } else if (m > 32) {
            fn_80122FA4(n, a, nw, w);
        } else {
            fn_80123CEC(n, a, nw, w);
        }
        fn_80120F84(n, ip, a);
    } else if (n > 8) {
        if (n == 32) {
            fn_80123E98(a, &w[nw - 8]);
            fn_80121848(a);
        } else {
            fn_80124AD8(a, w);
            fn_80121BB0(a);
        }
    } else if (n == 8) {
        fn_80124F90(a);
    } else if (n == 4) {
        fn_80125098(a);
    }
}

void fn_80120E14(int n, f32 *a, int *ip, int nw, f32 *w) {
    int m;

    if (n > 32) {
        m = n >> 2;
        fn_80122518(n, a, &w[nw - m]);
        if (n > 512) {
            fn_80122E08(m, a, nw, w);
            fn_80122ED8(m, &a[m], nw, w);
            fn_80122E08(m, &a[2 * m], nw, w);
            fn_80122E08(m, &a[3 * m], nw, w);
        } else if (m > 32) {
            fn_80122FA4(n, a, nw, w);
        } else {
            fn_80123CEC(n, a, nw, w);
        }
        fn_80121400(n, ip, a);
    } else if (n > 8) {
        if (n == 32) {
            fn_80123E98(a, &w[nw - 8]);
            fn_801219B4(a);
        } else {
            fn_80124AD8(a, w);
            fn_80121BF4(a);
        }
    } else if (n == 8) {
        fn_80125014(a);
    } else if (n == 4) {
        fn_80125098(a);
    }
}

void fn_80120F84(int n, int *ip, f32 *a) {
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

void fn_80121400(int n, int *ip, f32 *a) {
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

void fn_80121848(f32 *arg0) {
    f32 temp_f0;
    f32 temp_f10;
    f32 temp_f11;
    f32 temp_f1;
    f32 temp_f28;
    f32 temp_f29;
    f32 temp_f2;
    f32 temp_f3;
    f32 temp_f4;
    f32 temp_f5;
    f32 temp_f8;
    f32 temp_f9;
    temp_f0 = *(f32 *)((s8 *)arg0 + 8);
    temp_f1 = *(f32 *)((s8 *)arg0 + 0xC);
    temp_f2 = *(f32 *)((s8 *)arg0 + 0x10);
    temp_f3 = *(f32 *)((s8 *)arg0 + 0x14);
    temp_f4 = *(f32 *)((s8 *)arg0 + 0x18);
    temp_f5 = *(f32 *)((s8 *)arg0 + 0x1C);
    temp_f8 = *(f32 *)((s8 *)arg0 + 0x28);
    temp_f9 = *(f32 *)((s8 *)arg0 + 0x2C);
    temp_f10 = *(f32 *)((s8 *)arg0 + 0x38);
    temp_f11 = *(f32 *)((s8 *)arg0 + 0x3C);
    temp_f29 = *(f32 *)((s8 *)arg0 + 0x58);
    temp_f28 = *(f32 *)((s8 *)arg0 + 0x5C);
    *(f32 *)((s8 *)arg0 + 8) = *(f32 *)((s8 *)arg0 + 0x40);
    *(f32 *)((s8 *)arg0 + 0xC) = *(f32 *)((s8 *)arg0 + 0x44);
    *(f32 *)((s8 *)arg0 + 0x10) = *(f32 *)((s8 *)arg0 + 0x20);
    *(f32 *)((s8 *)arg0 + 0x14) = *(f32 *)((s8 *)arg0 + 0x24);
    *(f32 *)((s8 *)arg0 + 0x18) = *(f32 *)((s8 *)arg0 + 0x60);
    *(f32 *)((s8 *)arg0 + 0x1C) = *(f32 *)((s8 *)arg0 + 0x64);
    *(f32 *)((s8 *)arg0 + 0x20) = temp_f2;
    *(f32 *)((s8 *)arg0 + 0x24) = temp_f3;
    *(f32 *)((s8 *)arg0 + 0x28) = *(f32 *)((s8 *)arg0 + 0x50);
    *(f32 *)((s8 *)arg0 + 0x2C) = *(f32 *)((s8 *)arg0 + 0x54);
    *(f32 *)((s8 *)arg0 + 0x38) = *(f32 *)((s8 *)arg0 + 0x70);
    *(f32 *)((s8 *)arg0 + 0x3C) = *(f32 *)((s8 *)arg0 + 0x74);
    *(f32 *)((s8 *)arg0 + 0x40) = temp_f0;
    *(f32 *)((s8 *)arg0 + 0x44) = temp_f1;
    *(f32 *)((s8 *)arg0 + 0x50) = temp_f8;
    *(f32 *)((s8 *)arg0 + 0x54) = temp_f9;
    *(f32 *)((s8 *)arg0 + 0x58) = *(f32 *)((s8 *)arg0 + 0x68);
    *(f32 *)((s8 *)arg0 + 0x5C) = *(f32 *)((s8 *)arg0 + 0x6C);
    *(f32 *)((s8 *)arg0 + 0x60) = temp_f4;
    *(f32 *)((s8 *)arg0 + 0x64) = temp_f5;
    *(f32 *)((s8 *)arg0 + 0x68) = temp_f29;
    *(f32 *)((s8 *)arg0 + 0x6C) = temp_f28;
    *(f32 *)((s8 *)arg0 + 0x70) = temp_f10;
    *(f32 *)((s8 *)arg0 + 0x74) = temp_f11;
}

void fn_801219B4(f32 *a) {
    f32 x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i, x5r, x5i, x6r, x6i, x7r, x7i, x8r, x8i, x9r, x9i, x10r, x10i,
        x11r, x11i, x12r, x12i, x13r, x13i, x14r, x14i, x15r, x15i;

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

void fn_80121BB0(f32 *arg0) {
    f32 *p = (f32 *)arg0;
    f32 a, c, e, g, b, d, f, h;
    a = p[2]; c = p[3]; e = p[6]; g = p[7];
    b = p[8]; d = p[9]; f = p[12]; h = p[13];
    p[2] = b; p[3] = d; p[6] = f; p[7] = h;
    p[8] = a; p[9] = c; p[12] = e; p[13] = g;
}

void fn_80121BF4(f32 *arg0) {
    f32 temp_8, temp_C, temp_10, temp_14, temp_18, temp_1C;
    f32 temp_20, temp_24, temp_28, temp_2C, temp_30, temp_34;
    f32 temp_38, temp_3C;
    
    temp_8 = *(f32 *)((u8 *)arg0 + 8);
    temp_C = *(f32 *)((u8 *)arg0 + 0xc);
    temp_10 = *(f32 *)((u8 *)arg0 + 0x10);
    temp_14 = *(f32 *)((u8 *)arg0 + 0x14);
    temp_18 = *(f32 *)((u8 *)arg0 + 0x18);
    temp_1C = *(f32 *)((u8 *)arg0 + 0x1c);
    temp_20 = *(f32 *)((u8 *)arg0 + 0x20);
    temp_24 = *(f32 *)((u8 *)arg0 + 0x24);
    temp_28 = *(f32 *)((u8 *)arg0 + 0x28);
    temp_2C = *(f32 *)((u8 *)arg0 + 0x2c);
    temp_30 = *(f32 *)((u8 *)arg0 + 0x30);
    temp_34 = *(f32 *)((u8 *)arg0 + 0x34);
    temp_38 = *(f32 *)((u8 *)arg0 + 0x38);
    temp_3C = *(f32 *)((u8 *)arg0 + 0x3c);
    
    *(f32 *)((u8 *)arg0 + 8) = temp_38;
    *(f32 *)((u8 *)arg0 + 0xc) = temp_3C;
    *(f32 *)((u8 *)arg0 + 0x10) = temp_18;
    *(f32 *)((u8 *)arg0 + 0x14) = temp_1C;
    *(f32 *)((u8 *)arg0 + 0x18) = temp_28;
    *(f32 *)((u8 *)arg0 + 0x1c) = temp_2C;
    *(f32 *)((u8 *)arg0 + 0x20) = temp_8;
    *(f32 *)((u8 *)arg0 + 0x24) = temp_C;
    *(f32 *)((u8 *)arg0 + 0x28) = temp_30;
    *(f32 *)((u8 *)arg0 + 0x2c) = temp_34;
    *(f32 *)((u8 *)arg0 + 0x30) = temp_10;
    *(f32 *)((u8 *)arg0 + 0x34) = temp_14;
    *(f32 *)((u8 *)arg0 + 0x38) = temp_20;
    *(f32 *)((u8 *)arg0 + 0x3c) = temp_24;
}

extern f32 lbl_80561218;
extern f32 lbl_80561230;
void fn_80121C68(int n, f32 *a, f32 *w) {
    int j, j0, j1, j2, j3, k, m, mh;
    f32 wn4r, csc1, csc3, wk1r, wk1i, wk3r, wk3i, wd1r, wd1i, wd3r, wd3i;
    f32 x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i;

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
        wk3i = csc3 * (wd3i + w[k + 3]);
        wd1r = w[k];
        wd1i = w[k + 1];
        wd3r = w[k + 2];
        wd3i = w[k + 3];
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

void fn_80122518(int n, f32 *a, f32 *w) {
    int j, j0, j1, j2, j3, k, m, mh;
    f32 wn4r, csc1, csc3, wk1r, wk1i, wk3r, wk3i, wd1r, wd1i, wd3r, wd3i;
    f32 x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i;

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
        wk3i = csc3 * (wd3i + w[k + 3]);
        wd1r = w[k];
        wd1i = w[k + 1];
        wd3r = w[k + 2];
        wd3i = w[k + 3];
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

void fn_80122E08(int n, f32 *a, int nw, f32 *w) {
    int m;

    m = n >> 2;
    fn_801233F8(n, a, &w[nw - 2 * m]);
    if (n > 512) {
        fn_80122E08(m, a, nw, w);
        fn_80122ED8(m, &a[m], nw, w);
        fn_80122E08(m, &a[2 * m], nw, w);
        fn_80122E08(m, &a[3 * m], nw, w);
    } else {
        fn_80122FA4(n, a, nw, w);
    }
}

void fn_80122ED8(int n, f32 *a, int nw, f32 *w) {
    int m;

    m = n >> 2;
    fn_801237C4(n, a, &w[nw - n]);
    if (n > 512) {
        fn_80122E08(m, a, nw, w);
        fn_80122ED8(m, &a[m], nw, w);
        fn_80122E08(m, &a[2 * m], nw, w);
        fn_80122ED8(m, &a[3 * m], nw, w);
    } else {
        fn_801231B8(n, a, nw, w);
    }
}

void fn_80122FA4(int n, f32 *a, int nw, f32 *w) {
    int j, k, l;

    l = n >> 2;
    while (l > 128) {
        for (k = l; k < n; k <<= 2) {
            for (j = k - l; j < n; j += 4 * k) {
                fn_801233F8(l, &a[j], &w[nw - (l >> 1)]);
                fn_801237C4(l, &a[k + j], &w[nw - l]);
                fn_801233F8(l, &a[2 * k + j], &w[nw - (l >> 1)]);
            }
        }
        fn_801233F8(l, &a[n - l], &w[nw - (l >> 1)]);
        l >>= 2;
    }
    for (k = l; k < n; k <<= 2) {
        for (j = k - l; j < n; j += 4 * k) {
            fn_801233F8(l, &a[j], &w[nw - (l >> 1)]);
            fn_80123CEC(l, &a[j], nw, w);
            fn_801237C4(l, &a[k + j], &w[nw - l]);
            fn_80123DC0(l, &a[k + j], nw, w);
            fn_801233F8(l, &a[2 * k + j], &w[nw - (l >> 1)]);
            fn_80123CEC(l, &a[2 * k + j], nw, w);
        }
    }
    fn_801233F8(l, &a[n - l], &w[nw - (l >> 1)]);
    fn_80123CEC(l, &a[n - l], nw, w);
}

void fn_801231B8(int n, f32 *a, int nw, f32 *w) {
    int j, k, l, m;

    m = n >> 1;
    l = n >> 2;
    while (l > 128) {
        for (k = l; k < m; k <<= 2) {
            for (j = k - l; j < m; j += 2 * k) {
                fn_801233F8(l, &a[j], &w[nw - (l >> 1)]);
                fn_801233F8(l, &a[m + j], &w[nw - (l >> 1)]);
            }
            for (j = 2 * k - l; j < m; j += 4 * k) {
                fn_801237C4(l, &a[j], &w[nw - l]);
                fn_801237C4(l, &a[m + j], &w[nw - l]);
            }
        }
        l >>= 2;
    }
    for (k = l; k < m; k <<= 2) {
        for (j = k - l; j < m; j += 2 * k) {
            fn_801233F8(l, &a[j], &w[nw - (l >> 1)]);
            fn_80123CEC(l, &a[j], nw, w);
            fn_801233F8(l, &a[m + j], &w[nw - (l >> 1)]);
            fn_80123CEC(l, &a[m + j], nw, w);
        }
        for (j = 2 * k - l; j < m; j += 4 * k) {
            fn_801237C4(l, &a[j], &w[nw - l]);
            fn_80123DC0(l, &a[j], nw, w);
            fn_801237C4(l, &a[m + j], &w[nw - l]);
            fn_80123DC0(l, &a[m + j], nw, w);
        }
    }
}

void fn_801233F8(int n, f32 *a, f32 *w) {
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
        wk3i = w[k + 3];
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

void fn_801237C4(int n, f32 *a, f32 *w) {
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
        wk3i = w[k + 3];
        kr -= 4;
        wd1i = w[kr];
        wd1r = w[kr + 1];
        wd3i = w[kr + 2];
        wd3r = w[kr + 3];
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

void fn_80123CEC(int n, f32 *a, int nw, f32 *w) {
    if (n == 128) {
        fn_80123E98(a, &w[nw - 8]);
        fn_8012440C(&a[32], &w[nw - 32]);
        fn_80123E98(&a[64], &w[nw - 8]);
        fn_80123E98(&a[96], &w[nw - 8]);
    } else {
        fn_80124AD8(a, &w[nw - 16]);
        fn_80124CE8(&a[16], &w[nw - 16]);
        fn_80124AD8(&a[32], &w[nw - 16]);
        fn_80124AD8(&a[48], &w[nw - 16]);
    }
}

void fn_80123DC0(int n, f32 *a, int nw, f32 *w) {
    if (n == 128) {
        fn_80123E98(a, &w[nw - 8]);
        fn_8012440C(&a[32], &w[nw - 32]);
        fn_80123E98(&a[64], &w[nw - 8]);
        fn_8012440C(&a[96], &w[nw - 32]);
    } else {
        fn_80124AD8(a, &w[nw - 16]);
        fn_80124CE8(&a[16], &w[nw - 16]);
        fn_80124AD8(&a[32], &w[nw - 16]);
        fn_80124CE8(&a[48], &w[nw - 16]);
    }
}

void fn_80123E98(f32 *a, f32 *w) {
    f32 wn4r, wk1r, wk1i, x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, y4r, y4i,
        y5r, y5i, y6r, y6i, y7r, y7i, y8r, y8i, y9r, y9i, y10r, y10i, y11r, y11i, y12r, y12i, y13r, y13i, y14r,
        y14i, y15r, y15i;

    wn4r = w[1];
    wk1r = w[2];
    wk1i = w[3];
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

void fn_8012440C(f32 *a, f32 *w) {
    f32 wn4r, wk1r, wk1i, wk2r, wk2i, wk3r, wk3i, x0r, x0i, x1r, x1i, x2r, x2i, y0r, y0i, y1r, y1i, y2r, y2i,
        y3r, y3i, y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i, y8r, y8i, y9r, y9i, y10r, y10i, y11r, y11i, y12r, y12i,
        y13r, y13i, y14r, y14i, y15r, y15i;

    wn4r = w[1];
    wk1r = w[4];
    wk1i = w[5];
    wk3r = w[6];
    wk3i = -w[7];
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

void fn_80124AD8(f32 *a, f32 *w) {
    f32 wn4r, x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, y4r, y4i, y5r, y5i,
        y6r, y6i, y7r, y7i;

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

void fn_80124CE8(f32 *a, f32 *w) {
    f32 wn4r, wk1r, wk1i, x0r, x0i, x1r, x1i, y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, y4r, y4i, y5r, y5i, y6r,
        y6i, y7r, y7i;

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

void fn_80124F90(f32 *a) {
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

void fn_80125014(f32 *a) {
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

void fn_80125098(f32 *a) {
    f32 x0r, x0i;

    x0r = a[0] - a[2];
    x0i = a[1] - a[3];
    a[0] += a[2];
    a[1] += a[3];
    a[2] = x0r;
    a[3] = x0i;
}

void fn_801250CC(int n, f32 *a, int nc, f32 *c) {
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

void fn_801252D8(int n, f32 *a, int nc, f32 *c) {
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

void fn_801254E4(int n, f32 *a, int nc, f32 *c) {
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
void fn_80125664(u8 *dst, s32 stride, s16 *block, s32 *quant) {
    s32 tmp[64];
    s32 *t = tmp;
    s32 i;

    for (i = 0; i < 8; i++) {
        s32 s1 = block[8];
        s32 s2 = block[16];
        s32 s3 = block[24];
        s32 s4 = block[32];
        s32 s5 = block[40];
        s32 s6 = block[48];
        s32 s7 = block[56];
        if ((s1 | s2 | s3 | s4 | s5 | s6 | s7) == 0) {
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
            s32 d0 = (block[0] * quant[0]) >> 11;
            s32 d2 = (s2 * quant[16]) >> 11;
            s32 d4 = (s4 * quant[32]) >> 11;
            s32 d6 = (s6 * quant[48]) >> 11;
            s32 a0 = d0 + d4;
            s32 a1 = d0 - d4;
            s32 a2 = d2 + d6;
            s32 a3 = BINK_MUL(BINK_A1, d2 - d6) - a2;
            s32 e0 = a0 + a2;
            s32 e3 = a0 - a2;
            s32 e1 = a1 + a3;
            s32 e2 = a1 - a3;
            s32 d1 = (s1 * quant[8]) >> 11;
            s32 d3 = (s3 * quant[24]) >> 11;
            s32 d5 = (s5 * quant[40]) >> 11;
            s32 d7 = (s7 * quant[56]) >> 11;
            s32 a4 = d5 + d3;
            s32 a5 = d5 - d3;
            s32 a6 = d1 + d7;
            s32 a7 = d1 - d7;
            s32 b0 = a4 + a6;
            s32 b1 = BINK_MUL(BINK_A3, a5 + a7);
            s32 b2 = BINK_MUL(BINK_A4, a5) + b1 - b0;
            s32 b3 = BINK_MUL(BINK_A1, a6 - a4) - b2;
            s32 b4 = BINK_MUL(BINK_A2, a7) - b1 + b3;
            block++;
            quant++;
            t[0] = e0 + b0;
            t[56] = e0 - b0;
            t[8] = e1 + b2;
            t[48] = e1 - b2;
            t[16] = e2 + b3;
            t[40] = e2 - b3;
            t[32] = e3 + b4;
            t[24] = e3 - b4;
            t++;
        }
    }
    t = tmp;
    for (i = 0; i < 8; i++) {
        s32 a0 = t[0] + t[4];
        s32 a1 = t[0] - t[4];
        s32 a2 = t[2] + t[6];
        s32 a3 = BINK_MUL(BINK_A1, t[2] - t[6]) - a2;
        s32 a4 = t[5] + t[3];
        s32 a5 = t[5] - t[3];
        s32 a6 = t[1] + t[7];
        s32 a7 = t[1] - t[7];
        s32 b0 = a4 + a6;
        s32 b1 = BINK_MUL(BINK_A3, a5 + a7);
        s32 b2 = BINK_MUL(BINK_A4, a5) + b1 - b0;
        s32 b3 = BINK_MUL(BINK_A1, a6 - a4) - b2;
        s32 b4 = BINK_MUL(BINK_A2, a7) - b1 + b3;
        dst[0] = (a0 + a2 + b0 + 0x7F) >> 8;
        dst[7] = (a0 + a2 - b0 + 0x7F) >> 8;
        dst[1] = (a1 + a3 + b2 + 0x7F) >> 8;
        dst[6] = (a1 + a3 - b2 + 0x7F) >> 8;
        dst[2] = (a1 - a3 + b3 + 0x7F) >> 8;
        dst[5] = (a1 - a3 - b3 + 0x7F) >> 8;
        dst[4] = (a0 - a2 + b4 + 0x7F) >> 8;
        dst[3] = (a0 - a2 - b4 + 0x7F) >> 8;
        t += 8;
        dst += stride;
    }
}

// Like fn_80125664, but draws the block at double size (16x16) into dst.
void fn_80125960(u8 *dst, s32 stride, s16 *block, s32 *quant) {
    s32 tmp[64];
    s32 *t = tmp;
    s32 i;
    u8 *dst2 = dst + stride;
    s32 stride2 = stride * 2;

    for (i = 0; i < 8; i++) {
        s32 s1 = block[8];
        s32 s2 = block[16];
        s32 s3 = block[24];
        s32 s4 = block[32];
        s32 s5 = block[40];
        s32 s6 = block[48];
        s32 s7 = block[56];
        if ((s1 | s2 | s3 | s4 | s5 | s6 | s7) == 0) {
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
            s32 d0 = (block[0] * quant[0]) >> 11;
            s32 d2 = (s2 * quant[16]) >> 11;
            s32 d4 = (s4 * quant[32]) >> 11;
            s32 d6 = (s6 * quant[48]) >> 11;
            s32 a0 = d0 + d4;
            s32 a1 = d0 - d4;
            s32 a2 = d2 + d6;
            s32 a3 = BINK_MUL(BINK_A1, d2 - d6) - a2;
            s32 e0 = a0 + a2;
            s32 e3 = a0 - a2;
            s32 e1 = a1 + a3;
            s32 e2 = a1 - a3;
            s32 d1 = (s1 * quant[8]) >> 11;
            s32 d3 = (s3 * quant[24]) >> 11;
            s32 d5 = (s5 * quant[40]) >> 11;
            s32 d7 = (s7 * quant[56]) >> 11;
            s32 a4 = d5 + d3;
            s32 a5 = d5 - d3;
            s32 a6 = d1 + d7;
            s32 a7 = d1 - d7;
            s32 b0 = a4 + a6;
            s32 b1 = BINK_MUL(BINK_A3, a5 + a7);
            s32 b2 = BINK_MUL(BINK_A4, a5) + b1 - b0;
            s32 b3 = BINK_MUL(BINK_A1, a6 - a4) - b2;
            s32 b4 = BINK_MUL(BINK_A2, a7) - b1 + b3;
            block++;
            quant++;
            t[0] = e0 + b0;
            t[56] = e0 - b0;
            t[8] = e1 + b2;
            t[48] = e1 - b2;
            t[16] = e2 + b3;
            t[40] = e2 - b3;
            t[32] = e3 + b4;
            t[24] = e3 - b4;
            t++;
        }
    }
    t = tmp;
    for (i = 0; i < 8; i++) {
        s32 a0 = t[0] + t[4];
        s32 a1 = t[0] - t[4];
        s32 a2 = t[2] + t[6];
        s32 a3 = BINK_MUL(BINK_A1, t[2] - t[6]) - a2;
        s32 a4 = t[5] + t[3];
        s32 a5 = t[5] - t[3];
        s32 a6 = t[1] + t[7];
        s32 a7 = t[1] - t[7];
        s32 b0 = a4 + a6;
        s32 b1 = BINK_MUL(BINK_A3, a5 + a7);
        s32 b2 = BINK_MUL(BINK_A4, a5) + b1 - b0;
        s32 b3 = BINK_MUL(BINK_A1, a6 - a4) - b2;
        s32 b4 = BINK_MUL(BINK_A2, a7) - b1 + b3;
        u32 o0 = (a0 + a2 + b0 + 0x7F) >> 8;
        u32 o1 = (a1 + a3 + b2 + 0x7F) >> 8;
        u32 o2 = (a1 - a3 + b3 + 0x7F) >> 8;
        u32 o3 = (a0 - a2 - b4 + 0x7F) >> 8;
        u32 o4 = (a0 - a2 + b4 + 0x7F) >> 8;
        u32 o5 = (a1 - a3 - b3 + 0x7F) >> 8;
        u32 o6 = (a1 + a3 - b2 + 0x7F) >> 8;
        u32 o7 = (a0 + a2 - b0 + 0x7F) >> 8;
        u32 w0 = ((o0 & 0xFF) << 16) | (o1 & 0xFF);
        u32 w1 = ((o2 & 0xFF) << 16) | (o3 & 0xFF);
        u32 w2 = ((o4 & 0xFF) << 16) | (o5 & 0xFF);
        u32 w3 = ((o6 & 0xFF) << 16) | (o7 & 0xFF);
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
    fn_80125664(arg0, (s32)arg1, arg2, (s32 *)((u8 *)&lbl_802F6A40 + (arg3 << 8)));
}

void fn_80125CB4(void *arg0, void *arg1, void *arg2, s32 arg3) {
    fn_80125960(arg0, (s32)arg1, arg2, (s32 *)((u8 *)&lbl_802F6A40 + (arg3 << 8)));
}

extern u8 lbl_802F7A40[];
// Dequantizes (inter table `q`) and inverse-transforms a block, adding the result to the motion source src.
void fn_80125CC8(u8 *dst, s32 stride, s16 *block, s32 q, u8 *src, s32 srcStride) {
    s32 tmp[64];
    s32 *quant = (s32 *)((u8 *)lbl_802F7A40 + (q << 8));
    s32 *t = tmp;
    s32 i;

    for (i = 0; i < 8; i++) {
        s32 s1 = block[8];
        s32 s2 = block[16];
        s32 s3 = block[24];
        s32 s4 = block[32];
        s32 s5 = block[40];
        s32 s6 = block[48];
        s32 s7 = block[56];
        if ((s1 | s2 | s3 | s4 | s5 | s6 | s7) == 0) {
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
            s32 d0 = (block[0] * quant[0]) >> 11;
            s32 d2 = (s2 * quant[16]) >> 11;
            s32 d4 = (s4 * quant[32]) >> 11;
            s32 d6 = (s6 * quant[48]) >> 11;
            s32 a0 = d0 + d4;
            s32 a1 = d0 - d4;
            s32 a2 = d2 + d6;
            s32 a3 = BINK_MUL(BINK_A1, d2 - d6) - a2;
            s32 e0 = a0 + a2;
            s32 e3 = a0 - a2;
            s32 e1 = a1 + a3;
            s32 e2 = a1 - a3;
            s32 d1 = (s1 * quant[8]) >> 11;
            s32 d3 = (s3 * quant[24]) >> 11;
            s32 d5 = (s5 * quant[40]) >> 11;
            s32 d7 = (s7 * quant[56]) >> 11;
            s32 a4 = d5 + d3;
            s32 a5 = d5 - d3;
            s32 a6 = d1 + d7;
            s32 a7 = d1 - d7;
            s32 b0 = a4 + a6;
            s32 b1 = BINK_MUL(BINK_A3, a5 + a7);
            s32 b2 = BINK_MUL(BINK_A4, a5) + b1 - b0;
            s32 b3 = BINK_MUL(BINK_A1, a6 - a4) - b2;
            s32 b4 = BINK_MUL(BINK_A2, a7) - b1 + b3;
            block++;
            quant++;
            t[0] = e0 + b0;
            t[56] = e0 - b0;
            t[8] = e1 + b2;
            t[48] = e1 - b2;
            t[16] = e2 + b3;
            t[40] = e2 - b3;
            t[32] = e3 + b4;
            t[24] = e3 - b4;
            t++;
        }
    }
    t = tmp;
    for (i = 0; i < 8; i++) {
        s32 a0 = t[0] + t[4];
        s32 a1 = t[0] - t[4];
        s32 a2 = t[2] + t[6];
        s32 a3 = BINK_MUL(BINK_A1, t[2] - t[6]) - a2;
        s32 a4 = t[5] + t[3];
        s32 a5 = t[5] - t[3];
        s32 a6 = t[1] + t[7];
        s32 a7 = t[1] - t[7];
        s32 b0 = a4 + a6;
        s32 b1 = BINK_MUL(BINK_A3, a5 + a7);
        s32 b2 = BINK_MUL(BINK_A4, a5) + b1 - b0;
        s32 b3 = BINK_MUL(BINK_A1, a6 - a4) - b2;
        s32 b4 = BINK_MUL(BINK_A2, a7) - b1 + b3;
        dst[0] = src[0] + ((a0 + a2 + b0 + 0x7F) >> 8);
        dst[7] = src[7] + ((a0 + a2 - b0 + 0x7F) >> 8);
        dst[1] = src[1] + ((a1 + a3 + b2 + 0x7F) >> 8);
        dst[6] = src[6] + ((a1 + a3 - b2 + 0x7F) >> 8);
        dst[2] = src[2] + ((a1 - a3 + b3 + 0x7F) >> 8);
        dst[5] = src[5] + ((a1 - a3 - b3 + 0x7F) >> 8);
        dst[3] = src[3] + ((a0 - a2 - b4 + 0x7F) >> 8);
        dst[4] = src[4] + ((a0 - a2 + b4 + 0x7F) >> 8);
        src += srcStride;
        dst += stride;
        t += 8;
    }
}

// Reads the AC coefficients of an 8x8 DCT block (block[0], the DC, is set by the caller) bit plane by bit plane,
// using the same coefficient list scheme as the residue reader, then stores them in natural order.
void fn_80126018(s16 *block, BitReader8011 *bits) {
    u32 value;
    u32 *next;
    u32 avail;
    u32 bit;
    u32 v;
    u32 coef;
    u32 e;
    u32 width;
    u32 planes;
    s32 mask;
    s32 sign;
    s32 val;
    s32 i;
    u8 *p;
    u8 *start;
    u8 *end;
    u8 list[128];
    s16 tmp[64];

    {
        u32 *z = (u32 *)tmp;
        for (i = 0; i < 16; i++) {
            z[0] = 0;
            z[1] = 0;
            z += 2;
        }
    }
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
        p = start;
        while (p < end) {
            e = *p;
            if (e != 0) {
                BINK_GETBIT(bit);
                if (bit & 1) {
                    switch (e & 3) {
                    case 0:
                        coef = e >> 2;
                        *p = ((coef + 4) << 2) + 1;
                        goto group4;
                    case 1:
                        coef = e >> 2;
                        *p = (coef << 2) + 2;
                        end[0] = ((coef + 4) << 2) + 2;
                        end[1] = ((coef + 8) << 2) + 2;
                        end[2] = ((coef + 12) << 2) + 2;
                        end += 3;
                        continue;
                    case 2:
                        *p = 0;
                        coef = e >> 2;
                        p++;
                    group4:
                        BINK_GETBIT(bit);
                        if (bit & 1) {
                            *--start = ((coef) << 2) + 3;
                        } else {
                            BINK_GETBITS(v, width);
                            val = mask | (v & (sign - 1));
                            if (v & sign) {
                                val = -val;
                            }
                            tmp[coef] = val;
                        }
                        BINK_GETBIT(bit);
                        if (bit & 1) {
                            *--start = ((coef + 1) << 2) + 3;
                        } else {
                            BINK_GETBITS(v, width);
                            val = mask | (v & (sign - 1));
                            if (v & sign) {
                                val = -val;
                            }
                            tmp[coef + 1] = val;
                        }
                        BINK_GETBIT(bit);
                        if (bit & 1) {
                            *--start = ((coef + 2) << 2) + 3;
                        } else {
                            BINK_GETBITS(v, width);
                            val = mask | (v & (sign - 1));
                            if (v & sign) {
                                val = -val;
                            }
                            tmp[coef + 2] = val;
                        }
                        BINK_GETBIT(bit);
                        if (bit & 1) {
                            *--start = ((coef + 3) << 2) + 3;
                        } else {
                            BINK_GETBITS(v, width);
                            val = mask | (v & (sign - 1));
                            if (v & sign) {
                                val = -val;
                            }
                            tmp[coef + 3] = val;
                        }
                        continue;
                    case 3:
                        BINK_GETBITS(v, width);
                        val = mask | (v & (sign - 1));
                        if (v & sign) {
                            val = -val;
                        }
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
                        coef = e >> 2;
                        *p = ((coef + 4) << 2) + 1;
                        goto group4s;
                    case 1:
                        coef = e >> 2;
                        *p = (coef << 2) + 2;
                        end[0] = ((coef + 4) << 2) + 2;
                        end[1] = ((coef + 8) << 2) + 2;
                        end[2] = ((coef + 12) << 2) + 2;
                        end += 3;
                        continue;
                    case 2:
                        *p = 0;
                        coef = e >> 2;
                        p++;
                    group4s:
                        BINK_GETBIT(bit);
                        if (bit & 1) {
                            *--start = ((coef) << 2) + 3;
                        } else {
                            BINK_GETBIT(bit);
                            tmp[coef] = -(bit & 1) | 1;
                        }
                        BINK_GETBIT(bit);
                        if (bit & 1) {
                            *--start = ((coef + 1) << 2) + 3;
                        } else {
                            BINK_GETBIT(bit);
                            tmp[coef + 1] = -(bit & 1) | 1;
                        }
                        BINK_GETBIT(bit);
                        if (bit & 1) {
                            *--start = ((coef + 2) << 2) + 3;
                        } else {
                            BINK_GETBIT(bit);
                            tmp[coef + 2] = -(bit & 1) | 1;
                        }
                        BINK_GETBIT(bit);
                        if (bit & 1) {
                            *--start = ((coef + 3) << 2) + 3;
                        } else {
                            BINK_GETBIT(bit);
                            tmp[coef + 3] = -(bit & 1) | 1;
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
void fn_80126A10(s8 *block, BitReader8011 *bits, u32 limit) {
    u32 value;
    u32 *next;
    u32 avail;
    u32 n = 0;
    u32 bit;
    u32 coef;
    s32 mask;
    s32 planes;
    s32 nzCount;
    s32 i;
    u32 e;
    u8 *nz_p;
    u8 *p;
    u8 *start;
    u8 *end;
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
    start[3] = 0 << 2 | 2;
    end = start + 4;
    mask = 1 << planes;
    nzCount = 0;
    for (planes++; planes != 0; planes--) {
        nz_p = nz;
        for (i = 0; i < nzCount; i++) {
            BINK_GETBIT(bit);
            if (bit & 1) {
                block[*nz_p] += block[*nz_p] < 0 ? -mask : mask;
                if (n++ == limit) {
                    goto done;
                }
            }
            nz_p++;
        }
        p = start;
        while (p < end) {
            e = *p;
            if (e != 0) {
                BINK_GETBIT(bit);
                if (bit & 1) {
                    switch (e & 3) {
                    case 0:
                        coef = e >> 2;
                        *p = ((coef + 4) << 2) + 1;
                        goto group4;
                    case 1:
                        coef = e >> 2;
                        *p = (coef << 2) + 2;
                        end[0] = ((coef + 4) << 2) + 2;
                        end[1] = ((coef + 8) << 2) + 2;
                        end[2] = ((coef + 12) << 2) + 2;
                        end += 3;
                        continue;
                    case 2:
                        *p = 0;
                        coef = e >> 2;
                        p++;
                    group4:
                        BINK_GETBIT(bit);
                        if (bit & 1) {
                            *--start = ((coef) << 2) + 3;
                        } else {
                            nz[nzCount++] = coef;
                            BINK_GETBIT(bit);
                            block[coef] = (bit & 1) ? -mask : mask;
                            if (n++ == limit) {
                                goto done;
                            }
                        }
                        BINK_GETBIT(bit);
                        if (bit & 1) {
                            *--start = ((coef + 1) << 2) + 3;
                        } else {
                            nz[nzCount++] = coef + 1;
                            BINK_GETBIT(bit);
                            block[coef + 1] = (bit & 1) ? -mask : mask;
                            if (n++ == limit) {
                                goto done;
                            }
                        }
                        BINK_GETBIT(bit);
                        if (bit & 1) {
                            *--start = ((coef + 2) << 2) + 3;
                        } else {
                            nz[nzCount++] = coef + 2;
                            BINK_GETBIT(bit);
                            block[coef + 2] = (bit & 1) ? -mask : mask;
                            if (n++ == limit) {
                                goto done;
                            }
                        }
                        BINK_GETBIT(bit);
                        if (bit & 1) {
                            *--start = ((coef + 3) << 2) + 3;
                        } else {
                            nz[nzCount++] = coef + 3;
                            BINK_GETBIT(bit);
                            block[coef + 3] = (bit & 1) ? -mask : mask;
                            if (n++ == limit) {
                                goto done;
                            }
                        }
                        continue;
                    case 3:
                        coef = e >> 2;
                        nz[nzCount++] = coef;
                        BINK_GETBIT(bit);
                        block[coef] = (bit & 1) ? -mask : mask;
                        if (n++ == limit) {
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
void fn_80126EF0(u8 *dst, s32 dstStride, BitReader8011 *bits, u32 limit, u8 *src, s32 srcStride) {
    s8 block[64] = {0};

    fn_80126A10(block, bits, limit);
    dst[0] = block[0] + src[0];
    dst[1] = block[1] + src[1];
    dst[2] = block[4] + src[2];
    dst[3] = block[5] + src[3];
    dst[4] = block[8] + src[4];
    dst[5] = block[9] + src[5];
    dst[6] = block[12] + src[6];
    dst[7] = block[13] + src[7];
    dst += dstStride;
    src += srcStride;
    dst[0] = block[2] + src[0];
    dst[1] = block[3] + src[1];
    dst[2] = block[6] + src[2];
    dst[3] = block[7] + src[3];
    dst[4] = block[10] + src[4];
    dst[5] = block[11] + src[5];
    dst[6] = block[14] + src[6];
    dst[7] = block[15] + src[7];
    dst += dstStride;
    src += srcStride;
    dst[0] = block[24] + src[0];
    dst[1] = block[25] + src[1];
    dst[2] = block[44] + src[2];
    dst[3] = block[45] + src[3];
    dst[4] = block[16] + src[4];
    dst[5] = block[17] + src[5];
    dst[6] = block[20] + src[6];
    dst[7] = block[21] + src[7];
    dst += dstStride;
    src += srcStride;
    dst[0] = block[26] + src[0];
    dst[1] = block[27] + src[1];
    dst[2] = block[46] + src[2];
    dst[3] = block[47] + src[3];
    dst[4] = block[18] + src[4];
    dst[5] = block[19] + src[5];
    dst[6] = block[22] + src[6];
    dst[7] = block[23] + src[7];
    dst += dstStride;
    src += srcStride;
    dst[0] = block[28] + src[0];
    dst[1] = block[29] + src[1];
    dst[2] = block[32] + src[2];
    dst[3] = block[33] + src[3];
    dst[4] = block[48] + src[4];
    dst[5] = block[49] + src[5];
    dst[6] = block[52] + src[6];
    dst[7] = block[53] + src[7];
    dst += dstStride;
    src += srcStride;
    dst[0] = block[30] + src[0];
    dst[1] = block[31] + src[1];
    dst[2] = block[34] + src[2];
    dst[3] = block[35] + src[3];
    dst[4] = block[50] + src[4];
    dst[5] = block[51] + src[5];
    dst[6] = block[54] + src[6];
    dst[7] = block[55] + src[7];
    dst += dstStride;
    src += srcStride;
    dst[0] = block[36] + src[0];
    dst[1] = block[37] + src[1];
    dst[2] = block[40] + src[2];
    dst[3] = block[41] + src[3];
    dst[4] = block[56] + src[4];
    dst[5] = block[57] + src[5];
    dst[6] = block[60] + src[6];
    dst[7] = block[61] + src[7];
    dst += dstStride;
    src += srcStride;
    dst[0] = block[38] + src[0];
    dst[1] = block[39] + src[1];
    dst[2] = block[42] + src[2];
    dst[3] = block[43] + src[3];
    dst[4] = block[58] + src[4];
    dst[5] = block[59] + src[5];
    dst[6] = block[62] + src[6];
    dst[7] = block[63] + src[7];
}

extern char lbl_8055DF08;
// Calls every callback registered in list `arg0` (-1..3).
void fn_80127398(s32 arg0) {
    CallbackList *list;
    s32 i;
    if (arg0 >= -1 && arg0 < 4) {
        list = &(lbl_8041D728 + 1)[arg0];
    } else {
        list = 0;
    }
    if (list != 0) {
        for (i = 0; i < list->count; i++) {
            list->fns[i]();
        }
    }
}

char fn_8012742C(void) {
    return lbl_8055DF08;
}

void fn_80127434(void) {
    lbl_8055DF08 = 0;
}

void fn_80127440(void* alarm) {
    fn_80127F94(fn_802ECC74(alarm));
}

void fn_80127464(void *alarm) {
    fn_80127440(alarm);
}

void fn_80127468(OSAlarm *alarm, OSContext *ctx) {
#pragma unused(ctx)
    fn_80127F70(fn_802ECC74(alarm));
}

void fn_8012748C(s32 arg0) {
    fn_80127398(arg0);
    fn_802E300C(arg0);
}
extern f64 lbl_80561198;
extern f64 lbl_805611A8;
extern f32 lbl_805611A0;
extern f64 fn_8002EFBC(f32, f64);
extern void fn_80119288(void *);
extern void fn_80119300(void *, u32, u32);
extern void fn_8011943C(void *, s32);
extern s32 fn_801199A4(void *, s32);
