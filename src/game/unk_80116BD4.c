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
    u8 pad20[0x2C - 0x20];
    u8 *ringStart;   // 0x2C
    u8 *ringEnd;     // 0x30
    u8 *writePtr;    // 0x34
    u8 *readPtr;     // 0x38
    s32 handle;      // 0x3C
    u32 available;   // 0x40
    s32 eightBit;    // 0x44  (16-bit data delivered as 8-bit)
    s32 deadline;    // 0x48
    u8 pad4C[0x54 - 0x4C];
    u32 minimum;     // 0x54
    s32 blockMask;   // 0x58
    s32 dirty;       // 0x5C  set when the voices ran dry (silence was written)
    u8 pad60[0x64 - 0x60];
    s32 playing;     // 0x64
    s32 unk68;       // 0x68
    u8 pad6C[0x70 - 0x6C];
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
typedef struct StreamPlayer {
    u8 pad0[0x8];
    u32 numSets;                        // 0x08
    s32 curSet;                         // 0x0C
    s32 prevSet;                        // 0x10
    u8 pad14[0x20 - 0x14];
    u32 flags;                          // 0x20
    u8 pad24[0xDC - 0x24];
    s32 paused;                         // 0xDC
    u8 padE0[0xE4 - 0xE0];
    s32 setData;                        // 0xE4  start of the current set's data
    void *memoryData;                   // 0xE8  set when playing from memory
    s32 *setIndex;                      // 0xEC  file offset of each set (low bit is a flag)
    u8 reader[4];                       // 0xF0  file reader state
    s32 (*readerSeek)(void *reader, s32 set, s32 offset, s32 dst, s32 size);  // 0xF4
    u8 readerF8[0x100 - 0xF8];
    s32 (*readerFill)(void *reader);    // 0x100
    void (*readerClose)(void *reader);  // 0x104
    u8 pad108[0x11C - 0x108];
    volatile s32 busy;                  // 0x11C  also read by the decode callbacks
    u8 pad120[0x138 - 0x120];
    u32 readerCapacity;                 // 0x138
    u32 readerFilled;                   // 0x13C
    u8 pad140[0x1F4 - 0x140];
    u8 request[0x234 - 0x1F4];          // 0x1F4
    void *readBuffer;                   // 0x234
    u8 pad238[0x258 - 0x238];
    s32 unk258;                         // 0x258
    u8 pad25C[0x268 - 0x25C];
    s32 startTime;                      // 0x268
    u8 pad26C[0x288 - 0x26C];
    s32 running;                        // 0x288
    u8 pad28C[0x2D4 - 0x28C];
    u32 unk2D4;                         // 0x2D4
    s32 firstTime;                      // 0x2D8  time the first set last started
    s32 stalled;                        // 0x2DC
    u8 pad2E0[0x2E8 - 0x2E0];
    int numTracks;                      // 0x2E8
    s32 resyncs;                        // 0x2EC
    StreamTrack *tracks;                // 0x2F0
    u8 pad2F4[0x2F8 - 0x2F4];
    s32 unk2F8;                         // 0x2F8
    u8 pad2FC[0x328 - 0x2FC];
    s32 unk328;                         // 0x328
    u8 pad32C[0x330 - 0x32C];
    s32 pauseTime;                      // 0x330
    u8 pad334[0x33C - 0x334];
    s32 tick;                           // 0x33C
    u8 queue[0x384 - 0x340];            // 0x340  request queue entry
    void *extra;                        // 0x384
} StreamPlayer;

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
u32 fn_80116F8C(void *arg0);
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

void fn_80116CC4(s32* arg0, u16 arg1, u32 arg2) {
    s32* r3;
    u32 r6;
    s32 r7;
    s32 val;
    s32* p;
    u32 count;
    
    r3 = arg0;
    r6 = arg2 >> 1;
    r7 = arg1;
    
    val = (s32)(arg1 & 0xFFFF);
    val = val | (val << 16);
    
    if (r6 != 0) {
        count = r6 >> 3;
        if (count != 0) {
            p = r3;
            do {
                p[0] = val;
                p[1] = val;
                p[2] = val;
                p[3] = val;
                p[4] = val;
                p[5] = val;
                p[6] = val;
                p[7] = val;
                p += 8;
                count--;
            } while (count != 0);
            r6 &= 7;
        }
        if (r6 != 0) {
            do {
                *r3 = val;
                r3++;
                r6--;
            } while (r6 != 0);
        }
    }
    
    if (arg2 & 1) {
        *(s16*)r3 = arg1;
    }
}

extern s64 OSGetTime(void);
extern s32 lbl_8055DEC0;
extern u32 lbl_8055DEC4;
s32 fn_80116D30(void) {
    s64 time = OSGetTime();
    u32 time_high;
    s32 time_low;
    s32 saved_low;
    u32 saved_high;
    s64 delta;
    
    time_high = (u32)(time >> 32);
    time_low = (s32)time;
    
    saved_low = lbl_8055DEC0;
    saved_high = lbl_8055DEC4;
    
    if (saved_low == 0 && saved_high == 0) {
        lbl_8055DEC4 = time_high;
        lbl_8055DEC0 = time_low;
        saved_high = time_high;
        saved_low = time_low;
    }
    
    delta = time - ((s32)saved_high << 32 | saved_low);
    
    return (s32)(delta * 70811LL / 2317483787LL);
}

extern void* fn_8011FF24(void);
extern void *fn_8011FF2C(s32);
void fn_80116DDC(void* arg0, s32 arg1) {
    void* result = fn_8011FF24();
    s32* ptr = (s32*)arg0;
    s32 current = ptr[0x290 / 4];
    s32 new_val = arg1 + (s32)result + current;
    ptr[0x290 / 4] = new_val;
    fn_8011FF2C(arg1);
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
u32 fn_80116F8C(void *arg0) {
    s32 temp_r28;
    s32 temp_r30;
    u32 temp_r0;
    u32 temp_r29;
    u32 temp_r4;
    u32 temp_r6;
    u32 temp_r9;
    u32 *data = (u32 *)arg0;

    temp_r4 = data[0x4C / 4];
    temp_r0 = data[0x40 / 4];
    if (temp_r0 < temp_r4) {
        temp_r30 = temp_r4 - temp_r0;
        temp_r6 = data[0x38 / 4] - temp_r30;
        temp_r9 = data[0x2C / 4];
        data[0x38 / 4] = temp_r6;
        temp_r29 = (temp_r30 * 0x3E8) / ((data[0x78 / 4] * (data[0x70 / 4] * data[0x74 / 4])) >> 3);
        if (temp_r6 < temp_r9) {
            temp_r28 = temp_r9 - temp_r6;
            data[0x38 / 4] = temp_r6 + data[0x28 / 4];
            __memclr((void *)temp_r9, temp_r30 - temp_r28);
            __memclr((void *)data[0x38 / 4], temp_r28);
        } else {
            __memclr((void *)temp_r6, temp_r30);
        }
        data[0x40 / 4] += temp_r30;
        return temp_r29;
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
        p->readerSeek(p->reader, set, a, p->setData, b - a);
    }
    if (p->numTracks != 0) fn_80117074(p);
    p->curSet = set + 1;
}

u32 fn_80116BD4(u32 a, u32 b, u32 d);
u32 fn_801174B0(s32 count, u32 *values, s32 offset, s32 *outIndex, s32 *outFlag) {
    s32 i;
    u32 maxDiff;
    s32 maxIndex;
    s32 allOdd;

    maxDiff = 0;
    maxIndex = 0;
    for (i = 0; i < count - offset; i++) {
        u32 diff = values[offset + i] - values[i];
        if (diff > maxDiff) {
            maxDiff = diff;
            maxIndex = i;
        }
    }
    if (maxDiff == 0) {
        maxDiff = fn_80116BD4(values[count] - values[0], offset, count);
    }
    allOdd = 1;
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
            while ((p->readerCapacity * 29) >> 5 > p->readerFilled && p->readerFill(p->reader) != 0) {
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
            p->readerClose(p->reader);
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
    u32 groups;
    if (count) {
        groups = count >> 3;
        if (groups) {
            do {
                *data = __lwbrx(data, 0); data++;
                *data = __lwbrx(data, 0); data++;
                *data = __lwbrx(data, 0); data++;
                *data = __lwbrx(data, 0); data++;
                *data = __lwbrx(data, 0); data++;
                *data = __lwbrx(data, 0); data++;
                *data = __lwbrx(data, 0); data++;
                *data = __lwbrx(data, 0); data++;
            } while (--groups);
            count &= 7;
        }
        if (count) {
            do {
                *data = __lwbrx(data, 0); data++;
            } while (--count);
        }
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
    s32 temp_r4;
    s32* p = (s32*)arg0;
    OSDisableInterrupts();
    arg2 &= 0xFFFF8000;
    temp_r4 = arg1 + arg2;
    p[0xA0/4] = arg2;
    p[0x40/4] = arg2;
    p[0xA4/4] = arg1;
    p[0x9C/4] = arg1;
    p[0xAC/4] = arg1;
    p[0xA8/4] = temp_r4;
    p[0x4C/4] = 0;
    p[0xB8/4] = arg3;
    p[0xBC/4] = arg4;
    p[0xD0/4] = (s32)arg0;
    OSRestoreInterrupts(temp_r4);
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

s32 fn_8011B2AC(s32 size, u32 channels, s32 flags, s32 arg3, s32 arg4, s32 output, u32 *input, u32 bands, f32 gain, s32 arg8) {
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

void fn_8011B94C(void *arg0, s32 *arg1, s32 *arg2, s32 arg3, s32 *arg4, s32 arg5, u32 *arg6, u32 arg7, f32 arg8, s32 arg9) {
    u8 *p = (u8 *)arg0;
    u32 count = *(u32 *)(p + 0xC) >> 1;
    u32 i;
    s32 result = fn_8011B2AC(*(s32 *)p, *(u32 *)(p + 0x10), *(s32 *)(p + 0x30), *(s32 *)(p + 0x24), *(s32 *)(p + 0x28), *(s32 *)(p + 0x14), arg6, *(u32 *)(p + 0x1C), *(f32 *)(p + 4), *(u32 *)(p + 0x20));
    if (*(s32 *)(p + 0x18)) *(s32 *)(p + 0x18) = 0;
    else for (i = 0; i < count; i++) {
        s16 *a = *(s16 **)(p + 0x14);
        s16 *b = *(s16 **)(p + 0x2C);
        a[i] = (s16)(((u32)i * a[i] + (u32)(count - i) * b[i]) / count);
    }
    memcpy(*(void **)(p + 0x2C), (u8 *)*(void **)(p + 0x14) + (*(u32 *)(p + 8) - *(u32 *)(p + 0xC)), *(u32 *)(p + 0xC));
    if (arg2) *arg2 = *(u32 *)(p + 8) - *(u32 *)(p + 0xC);
    if (arg1) *arg1 = (s32)*(void **)(p + 0x14);
    if (arg4) *arg4 = arg6[0] + result;
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

void *fn_8011BE48(s32 arg0) {
    void *ptr;
    s32 sign;
    s32 value;
    ptr = fn_8011FFC4(0x40);
    if (ptr != NULL) {
        memset(ptr, 0, 0x40);
        sign = arg0 >> 31;
        *(s32 *)((u8 *)ptr + 0x00) = 0;
        value = (sign ^ arg0) - sign;
        *(s32 *)((u8 *)ptr + 0x04) = 1;
        *(s32 *)((u8 *)ptr + 0x30) = 0;
        *(s32 *)((u8 *)ptr + 0x34) = 1;
        *(s32 *)((u8 *)ptr + 0x38) = 0;
        *(s32 *)((u8 *)ptr + 0x3C) = value;
    }
    return ptr;
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

extern void fn_8011C4A0(s32, s32, s32, s32);
void fn_8011CD78(s32 arg0, s32 arg1) {
    fn_8011C4A0(arg1, arg0 + 0x24, arg0 + 0x20, arg0 + 0x10);
}

void fn_8011CD90(s32 arg0, s32 arg1, s32 arg2) {
    s32 a = arg2;
    s32 b = arg2 + 0x100;
    s32 c = arg2 + 0x140;
    u32 i;
    for (i = 0; i < 0x10; i++) {
        fn_8011C4A0(arg1, c, b, a);
        a += 0x10;
        b += 4;
        c += 4;
    }
    fn_8011C4A0(arg1, arg0 + 0x24, arg0 + 0x20, arg0 + 0x10);
    *(s32 *)(arg2 + 0x180) = 0;
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

void fn_8011D0E8(void *arg0, void *arg1, void *arg2) {
 u32 *p=(u32*)M2C_FIELD(arg1,u32*,4), bits=M2C_FIELD(arg1,u32*,0), n=M2C_FIELD(arg1,u32*,8), width=M2C_FIELD(arg0,u32*,0x28), out;
 s32 left, tree=M2C_FIELD(arg2,s32*,0x180);
 u32 code, mask, value;
 u8 *dst;
 if(M2C_FIELD(arg0,u8**,0)!=M2C_FIELD(arg0,u8**,4)) return;
 if(n<width){u32 x=*p++; out=(-1U>>(32-width))&(bits|(x<<n)); bits=x>>(width-n); n=n+32-width;} else {out=bits&(-1U>>(32-width)); bits>>=width; n-=width;}
 if(out){
  dst=M2C_FIELD(arg0,u8**,0x2C); M2C_FIELD(arg0,u8**,0)=dst; M2C_FIELD(arg0,u8**,4)=dst+out;
  if(n==0){bits=*p++>>1;n=31;} else {bits>>=1;n--;}
  if(bits&1) out=-20-out;
  mask=-1U>>(32-M2C_FIELD(arg0,u32*,0x20));
  do {
   s32 rem;
   out--;
   {u8 *tab=(u8*)M2C_FIELD(arg2,u32*,tree*4+0x140); u32 w=M2C_FIELD(arg2,u32*,tree*4+0x100);
    if(n>=w){code=tab[bits&(-1U>>(32-w))];}
    else {u32 x=*p, idx=(bits|(x<<n))&(-1U>>(32-w)); code=tab[idx];}
    tree=*(u8*)((u8*)arg2+tree*16+(code&15)); rem=code>>4;
    if(n<rem){u32 x=*p++; bits=x>>(rem-n); n=n+32-rem;} else {bits>>=rem;n-=rem;}
   }
   {u8 *table=(u8*)M2C_FIELD(arg0,u8**,0x24); u32 ix;
    if(n>=M2C_FIELD(arg0,u32*,0x20)){u8 c=table[bits&mask]; value=*(u8*)((u8*)arg0+0x10+(c&15)); rem=c>>4;}
    else {u32 x=*p, ix2=(bits|(x<<n))&mask; u8 c=table[ix2]; value=*(u8*)((u8*)arg0+0x10+(c&15)); rem=c>>4;}
    if(n<rem){u32 x=*p++;bits=x>>(rem-n);n=n+32-rem;}else{bits>>=rem;n-=rem;}
   }
   {u32 c=value|(tree<<4); u32 v=c&127; if(c&128)v=(u8)(-v); *dst++=v+128;}
  } while(out>0);
  if(out < -22) memset(M2C_FIELD(arg0,u8**,0x2C),*(u8*)M2C_FIELD(arg0,u8**,0x2C),-(out+21));
  M2C_FIELD(arg2,s32*,0x180)=tree;
 } else {dst=M2C_FIELD(arg0,u8**,0x2C);M2C_FIELD(arg0,u8**,4)=dst;M2C_FIELD(arg0,u8**,0)=dst+4;}
 M2C_FIELD(arg1,u32**,4)=p;M2C_FIELD(arg1,u32*,0)=bits;M2C_FIELD(arg1,u32*,8)=n;
}

void fn_8011D378(void *arg0, void *arg1, void *arg2) {
u32 *src; u32 bits, index, remain, node, tmp; u8 *out; u32 endbits, mask;
if (*(u32*)arg0 != *(u32*)((u8*)arg0+4)) return;
remain=*(u32*)((u8*)arg1+8); index=*(u32*)((u8*)arg0+0x28); src=*(u32**)((u8*)arg1+4); bits=*(u32*)arg1;
if(remain<index){tmp=*src++; bits=(bits|(tmp<<remain))&(0xFFFFFFFFU>>(32-index)); remain=remain+32-index; bits=tmp>>(index-*(u32*)((u8*)arg1+8));}else{remain-=index;bits&=0xFFFFFFFFU>>(32-index);bits>>= index;}
if(bits){out=*(u8**)((u8*)arg0+0x2c);endbits=*(u32*)((u8*)arg0+0x20);*(u8**)arg0=out;*(u8**)((u8*)arg0+4)=out+bits;node=*(u32*)((u8*)arg2+0x180);
if(remain==0){tmp=*src++;remain=31;bits=tmp>>1;}else{tmp=bits;bits>>=1;remain--;}
if(tmp&1)bits=-20-bits; mask=0xFFFFFFFFU>>(32-endbits);
do{bits--;{u32 n=*(u32*)((u8*)arg2+node*4+0x100);u8 v;if(remain>=n){v=*(*(u8**)((u8*)arg2+node*4+0x140)+(bits&(0xFFFFFFFFU>>(32-n))));node=*((u8*)arg2+node*16+(v&15));tmp=v>>4;bits>>=tmp;remain-=tmp;}else{u32 w=*src;v=*(*(u8**)((u8*)arg2+node*4+0x140)+((bits|(w<<remain))&(0xFFFFFFFFU>>(32-n))));node=*((u8*)arg2+node*16+(v&15));tmp=v>>4;if(remain>=tmp){bits>>=tmp;remain-=tmp;}else{bits=w>>(tmp-remain);src++;remain=remain+32-tmp;}}}
{u8 v;if(remain>=endbits){v=*((u8*)arg0+0x24+(bits&mask));tmp=*(u8*)((u8*)arg0+0x10+(v&15));bits>>=v>>4;remain-=v>>4;}else{u32 w=*src;v=*((u8*)arg0+0x24+((bits|(w<<remain))&mask));tmp=*(u8*)((u8*)arg0+0x10+(v&15));if(remain>=v>>4){bits>>=v>>4;remain-=v>>4;}else{bits=w>>((v>>4)-remain);src++;remain=remain+32-(v>>4);}}}
*out++=(u8)(tmp|(node<<4));
}while(bits>0);
if(bits < -22) memset(*(u8**)((u8*)arg0+0x2c), **(u8**)((u8*)arg0+0x2c), -bits-21);
*(u32*)((u8*)arg2+0x180)=node;
}else{out=*(u8**)((u8*)arg0+0x2c);*(u8**)((u8*)arg0+4)=out;*(u8**)arg0=out+4;}
*(u32*)((u8*)arg1+4)=(u32)src;*(u32*)arg1=bits;*(u32*)((u8*)arg1+8)=remain;
}

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
                fn_80116CC4((s32 *)dst, (u16)val, (n + n));
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

extern s32* fn_8011E0B4(s32, s32, s32, s32, s32, s32*, s32, s32);
u32 fn_8011FC2C(void* arg0, s32 arg1, s32 arg2, s32* arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7) {
    u32 result = 0;
    s32* ptr = arg3;
    s32 old = *(s32*)((u8*)arg0 + 0x14);
    s32 next = old ^ 1;
    s32 cur;
    s32 flag;
    s32* ret;
    s32 tmp;
    if (*(s32*)((u8*)arg0 + next * 0x30 + 0x1C))
        *(s32*)((u8*)arg0 + 0x14) = next;
    cur = *(s32*)((u8*)arg0 + 0x14);
    if ((arg7 & 0x100000) && (arg6 & 0x100000)) {
        tmp = 1;
        fn_8011E0B4(*(s32*)((u8*)arg0 + cur * 0x30 + 0x40), *(s32*)((u8*)arg0 + old * 0x30 + 0x40),
            *(s32*)((u8*)arg0 + 4), *(s32*)((u8*)arg0 + 8), *(s32*)((u8*)arg0 + old * 0x30 + 0x44),
            ptr + 1, arg4, arg1);
        ptr = (s32*)((u8*)ptr + *ptr);
    }
    flag = arg6 & 0x8000;
    result = 0;
    if (flag == 0) ptr++;
    ret = fn_8011E0B4(*(s32*)((u8*)arg0 + cur * 0x30 + 0x1C), *(s32*)((u8*)arg0 + old * 0x30 + 0x1C),
        *(s32*)((u8*)arg0 + 4), *(s32*)((u8*)arg0 + 8), *(s32*)((u8*)arg0 + old * 0x30 + 0x20),
        ptr, arg4, arg1);
    if (flag == 0) ret = (s32*)((u8*)ptr + *(s32*)((u8*)ptr - 4) - 4);
    result >>= 2;
    if (!(arg6 & 0x20000)) {
        tmp = 2;
        ret = fn_8011E0B4(*(s32*)((u8*)arg0 + cur * 0x30 + 0x28), *(s32*)((u8*)arg0 + old * 0x30 + 0x28),
            *(s32*)((u8*)arg0 + 0xC), *(s32*)((u8*)arg0 + 0x10), *(s32*)((u8*)arg0 + old * 0x30 + 0x2C),
            ret, arg4, arg1);
        result = 0;
        fn_8011E0B4(*(s32*)((u8*)arg0 + cur * 0x30 + 0x34), *(s32*)((u8*)arg0 + old * 0x30 + 0x34),
            *(s32*)((u8*)arg0 + 0xC), *(s32*)((u8*)arg0 + 0x10), *(s32*)((u8*)arg0 + old * 0x30 + 0x38),
            ret, arg4, arg1);
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

void fn_80125960(void *arg0, s32 arg1, void *arg2, void *arg3) {
    s32 sp8[64];
    s32 *var_r12;
    s32 *var_r4;
    s16 temp_r0, temp_r10, temp_r25, temp_r4, temp_r4_2, temp_r7, temp_r8, temp_r9;
    s16 temp_r23;
    s32 temp_r0_2, temp_r0_3, temp_r0_4, temp_r0_5, temp_r0_6, temp_r0_7;
    s32 temp_r10_2, temp_r10_3, temp_r10_4, temp_r10_5, temp_r10_6;
    s32 temp_r11;
    s32 temp_r11_2, temp_r11_3, temp_r11_4, temp_r11_5;
    s32 temp_r12, temp_r12_2, temp_r12_3, temp_r12_4, temp_r12_5;
    s32 temp_r23_2, temp_r23_3, temp_r23_4, temp_r23_5, temp_r23_6, temp_r23_7, temp_r23_8, temp_r23_9, temp_r23_10, temp_r23_11;
    s32 temp_r24;
    s32 temp_r24_2, temp_r24_3, temp_r24_4, temp_r24_5, temp_r24_6, temp_r24_7, temp_r24_8;
    s32 temp_r25_2, temp_r25_3, temp_r25_4, temp_r25_5, temp_r25_6;
    s32 temp_r26, temp_r26_2, temp_r26_3, temp_r27, temp_r28, temp_r29; s32 temp_r30;
    s32 temp_r4_3, temp_r4_4, temp_r5, temp_r5_2, temp_r6;
    s32 temp_r7_2, temp_r7_3, temp_r7_4, temp_r7_5;
    s32 temp_r8_2, temp_r8_3, temp_r8_4, temp_r8_5;
    s32 temp_r9_2, temp_r9_3, temp_r9_4, temp_r9_5, temp_r9_6, temp_r9_7, temp_r9_8;
    s32 var_ctr, var_ctr_2;
    u32 temp_r11_6;
    u8 *var_r31;
    u8 *var_r3;
    s16 *var_r5;
    s32 *var_r6;
    var_r3 = (u8 *)arg0;
    var_r5 = (s16 *)arg2;
    var_r6 = (s32 *)arg3;
    var_r31 = var_r3 + arg1;
    temp_r30 = arg1 * 2;
    var_r12 = sp8;
    for (var_ctr = 8; var_ctr != 0; var_ctr--) {
        temp_r7 = var_r5[8];
        temp_r8 = var_r5[24];
        temp_r23 = var_r5[16];
        temp_r0 = var_r5[32];
        temp_r4 = var_r5[48];
        temp_r9 = var_r5[40];
        temp_r24 = temp_r8 | temp_r7;
        temp_r4 = var_r5[48];
        temp_r9 = var_r5[40];
        temp_r11 = temp_r0 | temp_r23;
        temp_r10 = var_r5[56];
        temp_r24 = temp_r4 | temp_r24;
        temp_r11 = temp_r9 | temp_r11;
        temp_r24 = temp_r10 | temp_r24;
        if ((temp_r24 | temp_r11) == 0) {
            temp_r4_2 = var_r5[0];
            var_r5++;
            temp_r0_2 = var_r6[0];
            var_r6++;
            temp_r0_3 = (s32)(temp_r4_2 * temp_r0_2) >> 11;
            var_r12[0] = temp_r0_3;
            var_r12[8] = temp_r0_3;
            var_r12[16] = temp_r0_3;
            var_r12[24] = temp_r0_3;
            var_r12[32] = temp_r0_3;
            var_r12[40] = temp_r0_3;
            var_r12[48] = temp_r0_3;
            var_r12[56] = temp_r0_3;
            var_r12++;
        } else {
            temp_r25 = var_r5[0];
            var_r5++;
            temp_r25_2 = var_r6[32];
            temp_r26 = var_r6[8];
            temp_r27 = var_r6[24];
            temp_r11 = var_r6[48];
            temp_r28 = var_r6[40];
            temp_r24 = temp_r23 * var_r6[16];
            temp_r23_2 = temp_r25 * var_r6[0] >> 11;
            temp_r29 = var_r6[56];
            var_r6++;
            temp_r24_2 = temp_r24 >> 11;
            temp_r25_3 = temp_r0 * temp_r25_2 >> 11;
            temp_r11_2 = temp_r23_2 + temp_r25_3;
            temp_r23_3 = temp_r23_2 - temp_r25_3;
            temp_r0_4 = temp_r4 * temp_r11 >> 11;
            temp_r25_4 = temp_r24_2 + temp_r0_4;
            temp_r0_5 = temp_r11_2 + temp_r25_4;
            temp_r11_3 = temp_r11_2 - temp_r25_4;
            temp_r24_3 = ((temp_r24_2 - temp_r0_4) * 0xB50 >> 11) - temp_r25_4;
            temp_r4_3 = temp_r23_3 + temp_r24_3;
            temp_r7_2 = temp_r23_3 - temp_r24_3;
            temp_r23_4 = temp_r7 * temp_r26 >> 11;
            temp_r9_2 = temp_r8 * temp_r27 >> 11;
            temp_r24_4 = temp_r9 * temp_r28 >> 11;
            temp_r25_5 = temp_r24_4 - temp_r9_2;
            temp_r24_5 = temp_r24_4 + temp_r9_2;
            temp_r9_3 = temp_r10 * temp_r29 >> 11;
            temp_r8_2 = temp_r23_4 - temp_r9_3;
            temp_r10_2 = temp_r23_4 + temp_r9_3;
            temp_r23_5 = temp_r10_2 + temp_r24_5;
            var_r12[0] = temp_r0_5 + temp_r23_5;
            temp_r24_6 = (temp_r25_5 + temp_r8_2) * 0xEC8 >> 11;
            var_r12[56] = temp_r0_5 - temp_r23_5;
            temp_r23_6 = temp_r24_6 + (temp_r25_5 * -0x14E8 >> 11) - temp_r23_5;
            var_r12[8] = temp_r4_3 + temp_r23_6;
            var_r12[48] = temp_r4_3 - temp_r23_6;
            temp_r9_4 = ((temp_r10_2 - temp_r24_5) * 0xB50 >> 11) - temp_r23_6;
            var_r12[16] = temp_r7_2 + temp_r9_4;
            var_r12[40] = temp_r7_2 - temp_r9_4;
            temp_r4_4 = ((temp_r8_2 * 0x8A9) >> 11) - temp_r24_6;
            temp_r4_4 += temp_r9_4;
            var_r12[32] = temp_r11_3 + temp_r4_4;
            var_r12[24] = temp_r11_3 - temp_r4_4;
            var_r12++;
        }
    }
    var_r4 = sp8;
    for (var_ctr_2 = 8; var_ctr_2 != 0; var_ctr_2--) {
        temp_r9_5 = M2C_FIELD(var_r4, s32 *, 0x18);
        temp_r11_4 = M2C_FIELD(var_r4, s32 *, 8);
        temp_r5 = M2C_FIELD(var_r4, s32 *, 0xC);
        temp_r7_3 = M2C_FIELD(var_r4, s32 *, 0x14);
        temp_r12 = M2C_FIELD(var_r4, s32 *, 0x1C);
        temp_r23_7 = M2C_FIELD(var_r4, s32 *, 4);
        temp_r0_6 = temp_r7_3 - temp_r5;
        temp_r26_2 = temp_r7_3 + temp_r5;
        temp_r8_3 = M2C_FIELD(var_r4, s32 *, 0x10);
        temp_r5_2 = temp_r23_7 - temp_r12;
        temp_r10_3 = M2C_FIELD(var_r4, s32 *, 0);
        temp_r24_7 = temp_r11_4 + temp_r9_5;
        temp_r12_2 = temp_r23_7 + temp_r12;
        temp_r23_8 = temp_r10_3 + temp_r8_3;
        temp_r11_5 = temp_r10_3 - temp_r8_3;
        temp_r25_6 = ((temp_r11_4 - temp_r9_5) * 0xB50 >> 11) - temp_r24_7;
        temp_r7_4 = temp_r23_8 + temp_r24_7;
        temp_r26_3 = temp_r12_2 + temp_r26_2;
        temp_r6 = (temp_r0_6 + temp_r5_2) * 0xEC8 >> 11;
        temp_r12_3 = temp_r11_5 + temp_r25_6;
        temp_r0_7 = temp_r11_5 - temp_r25_6;
        temp_r8_4 = temp_r7_4 + temp_r26_3 + 0x7F;
        temp_r11_6 = (temp_r7_4 - temp_r26_3) + 0x7F;
        temp_r7_5 = (temp_r12_2 - temp_r26_2) * 0xB50;
        temp_r23_9 = temp_r23_8 - temp_r24_7;
        temp_r9_6 = temp_r6 + (temp_r0_6 * -0x14E8 >> 11);
        var_r4 += 8;
        temp_r10_4 = temp_r9_6 - temp_r26_3;
        temp_r24_8 = (temp_r7_5 >> 11) - temp_r10_4;
        temp_r8_5 = ((temp_r5_2 * 0x8A9 >> 11) - temp_r6) + temp_r24_8;
        temp_r12_4 = ((u8)((u32)(temp_r12_3 + temp_r10_4 + 0x7F) >> 8) & ~0xFF0000) | (((temp_r8_4 >> 8) << 16) & 0xFF0000);
        temp_r12_5 = temp_r12_4 | (temp_r12_4 << 8);
        temp_r23_10 = ((((temp_r12_3 - temp_r10_4) + 0x7F) << 8) & 0xFF0000 & ~0xFF) | ((temp_r11_6 >> 8) & 0xFF);
        temp_r9_7 = ((u8)((u32)((temp_r23_9 - temp_r8_5) + 0x7F) >> 8) & ~0xFF0000) | ((((s32)(temp_r0_7 + temp_r24_8 + 0x7F) >> 8) << 16) & 0xFF0000);
        M2C_FIELD(var_r3, s32 *, 0) = temp_r12_5;
        temp_r9_8 = temp_r9_7 | (temp_r9_7 << 8);
        temp_r10_5 = (((temp_r23_9 + temp_r8_5 + 0x7F) << 8) & 0xFF0000 & ~0xFF) | (((u32)(temp_r0_7 - temp_r24_8 + 0x7F) >> 8) & 0xFF);
        M2C_FIELD(var_r3, s32 *, 4) = temp_r9_8;
        temp_r23_11 = temp_r23_10 | (temp_r23_10 << 8);
        temp_r10_6 = temp_r10_5 | (temp_r10_5 << 8);
        M2C_FIELD(var_r3, s32 *, 8) = temp_r10_6;
        M2C_FIELD(var_r3, s32 *, 0xC) = temp_r23_11;
        var_r3 += temp_r30;
        M2C_FIELD(var_r31, s32 *, 0) = temp_r12_5;
        M2C_FIELD(var_r31, s32 *, 4) = temp_r9_8;
        M2C_FIELD(var_r31, s32 *, 8) = temp_r10_6;
        M2C_FIELD(var_r31, s32 *, 0xC) = temp_r23_11;
        var_r31 += temp_r30;
    }
}

extern void fn_80125664(void *p0, void *p1, void *p2, void *p3);
void fn_80125CA0(void *arg0, void *arg1, void *arg2, s32 arg3) {
    fn_80125664(arg0, arg1, arg2, (u8 *)&lbl_802F6A40 + (arg3 << 8));
}

extern void fn_80125960(void *p0, s32 p1, void *p2, void *p3);
void fn_80125CB4(void *arg0, void *arg1, void *arg2, s32 arg3) {
    fn_80125960(arg0, (s32)arg1, arg2, (u8 *)&lbl_802F6A40 + (arg3 << 8));
}

extern u8 lbl_802F7A40[];
void fn_80125CC8(void *arg0, s32 arg1, void *arg2, s32 arg3, void *arg4, s32 arg5) {
    s32 sp8[64];
    u8 *var_r6;
    u8 *var_r6_2;
    s16 temp_r0;
    s16 temp_r10;
    s16 temp_r10_2;
    s16 temp_r11;
    s16 temp_r12;
    s16 temp_r22;
    s16 temp_r26;
    s16 temp_r30;
    s16 temp_r31;
    s32 temp_r0_10;
    s32 temp_r0_2;
    s32 temp_r0_3;
    s32 temp_r0_4;
    s32 temp_r0_5;
    s32 temp_r0_6;
    s32 temp_r0_7;
    s32 temp_r0_8;
    s32 temp_r0_9;
    s32 temp_r10_10;
    s32 temp_r10_3;
    s32 temp_r10_4;
    s32 temp_r10_5;
    s32 temp_r10_6;
    s32 temp_r10_7;
    s32 temp_r10_8;
    s32 temp_r10_9;
    s32 temp_r11_2;
    s32 temp_r11_3;
    s32 temp_r11_4;
    s32 temp_r12_2;
    s32 temp_r12_3;
    s32 temp_r20;
    s32 temp_r20_2;
    s32 temp_r20_3;
    s32 temp_r20_4;
    s32 temp_r20_5;
    s32 temp_r20_6;
    s32 temp_r21;
    s32 temp_r21_2;
    s32 temp_r22_2;
    s32 temp_r22_3;
    s32 temp_r23;
    s32 temp_r23_2;
    s32 temp_r23_3;
    s32 temp_r24;
    s32 temp_r24_2;
    s32 temp_r24_3;
    s32 temp_r25;
    s32 temp_r25_2;
    s32 temp_r25_3;
    s32 temp_r25_4;
    s32 temp_r26_2;
    s32 temp_r26_3;
    s32 temp_r26_4;
    s32 temp_r26_5;
    s32 temp_r27;
    s32 temp_r27_2;
    s32 temp_r28;
    s32 temp_r28_2;
    s32 temp_r28_3;
    s32 temp_r29;
    s32 temp_r29_2;
    s32 temp_r29_3;
    s32 temp_r30_2;
    s32 temp_r30_3;
    s32 temp_r31_2;
    s32 temp_r31_3;
    s32 temp_r31_4;
    s32 temp_r5;
    s32 temp_r5_2;
    s32 temp_r9;
    s32 temp_r9_2;
    s32 var_ctr;
    s32 var_ctr_2;
    u8 temp_r5_3;
    u8 *var_r3;
    u8 *var_r5;
    u8 *var_r7;
    u8 *var_r9;

    var_r3 = arg0;
    var_r5 = arg2;
    var_r7 = arg4;
    var_r9 = lbl_802F7A40 + (arg3 << 8);
    var_r6 = (u8 *)&sp8;
    for (var_ctr = 0; var_ctr < 8; var_ctr++) {
        temp_r31 = M2C_FIELD(var_r5, s16 *, 0x60);
        temp_r12 = M2C_FIELD(var_r5, s16 *, 0x50);
        temp_r11 = M2C_FIELD(var_r5, s16 *, 0x40);
        temp_r10 = M2C_FIELD(var_r5, s16 *, 0x30);
        temp_r30 = M2C_FIELD(var_r5, s16 *, 0x70);
        temp_r0 = M2C_FIELD(var_r5, s16 *, 0x10);
        temp_r26 = M2C_FIELD(var_r5, s16 *, 0x20);
        if ((temp_r30 | (temp_r31 | temp_r12) | (temp_r11 | temp_r10 | (temp_r0 | temp_r26))) == 0) {
            temp_r10_2 = M2C_FIELD(var_r5, s16 *, 0);
            var_r5 += 2;
            temp_r0_2 = M2C_FIELD(var_r9, s32 *, 0);
            var_r9 += 4;
            temp_r0_3 = (s32) (temp_r10_2 * temp_r0_2) >> 0xB;
            M2C_FIELD(var_r6, s32 *, 0) = temp_r0_3;
            M2C_FIELD(var_r6, s32 *, 0x20) = temp_r0_3;
            M2C_FIELD(var_r6, s32 *, 0x40) = temp_r0_3;
            M2C_FIELD(var_r6, s32 *, 0x60) = temp_r0_3;
            M2C_FIELD(var_r6, s32 *, 0x80) = temp_r0_3;
            M2C_FIELD(var_r6, s32 *, 0xA0) = temp_r0_3;
            M2C_FIELD(var_r6, s32 *, 0xC0) = temp_r0_3;
            M2C_FIELD(var_r6, s32 *, 0xE0) = temp_r0_3;
            var_r6 += 4;
        } else {
            temp_r22 = M2C_FIELD(var_r5, s16 *, 0);
            var_r5 += 2;
            temp_r25 = M2C_FIELD(var_r9, s32 *, 0x80);
            temp_r24 = M2C_FIELD(var_r9, s32 *, 0xC0);
            temp_r20 = M2C_FIELD(var_r9, s32 *, 0x20);
            temp_r23 = M2C_FIELD(var_r9, s32 *, 0x60);
            temp_r22_2 = M2C_FIELD(var_r9, s32 *, 0xA0);
            temp_r26_2 = temp_r26 * M2C_FIELD(var_r9, s32 *, 0x40);
            temp_r29 = (s32) (temp_r22 * M2C_FIELD(var_r9, s32 *, 0)) >> 0xB;
            temp_r21 = M2C_FIELD(var_r9, s32 *, 0xE0);
            var_r9 += 4;
            temp_r11_2 = temp_r26_2 >> 0xB;
            temp_r28 = (s32) (temp_r11 * temp_r25) >> 0xB;
            temp_r27 = temp_r29 + temp_r28;
            temp_r26_3 = temp_r29 - temp_r28;
            temp_r31_2 = (s32) (temp_r31 * temp_r24) >> 0xB;
            temp_r24_2 = temp_r11_2 + temp_r31_2;
            temp_r29_2 = temp_r27 + temp_r24_2;
            temp_r31_3 = temp_r27 - temp_r24_2;
            temp_r25_2 = ((s32) ((temp_r11_2 - temp_r31_2) * 0xB50) >> 0xB) - temp_r24_2;
            temp_r11_3 = temp_r26_3 + temp_r25_2;
            temp_r28_2 = temp_r26_3 - temp_r25_2;
            temp_r0_4 = (s32) (temp_r0 * temp_r20) >> 0xB;
            temp_r10_3 = (s32) (temp_r10 * temp_r23) >> 0xB;
            temp_r20_2 = (s32) (temp_r12 * temp_r22_2) >> 0xB;
            temp_r12_2 = temp_r20_2 - temp_r10_3;
            temp_r23_2 = temp_r20_2 + temp_r10_3;
            temp_r10_4 = (s32) (temp_r30 * temp_r21) >> 0xB;
            temp_r30_2 = temp_r0_4 - temp_r10_4;
            temp_r0_5 = temp_r0_4 + temp_r10_4;
            temp_r10_5 = temp_r0_5 + temp_r23_2;
            M2C_FIELD(var_r6, s32 *, 0) = (s32) (temp_r29_2 + temp_r10_5);
            M2C_FIELD(var_r6, s32 *, 0xE0) = (s32) (temp_r29_2 - temp_r10_5);
            temp_r20_3 = (s32) ((temp_r12_2 + temp_r30_2) * 0xEC8) >> 0xB;
            temp_r20_4 = (temp_r20_3 + ((s32) (temp_r12_2 * -0x14E8) >> 0xB)) - temp_r10_5;
            temp_r10_6 = ((s32) ((temp_r0_5 - temp_r23_2) * 0xB50) >> 0xB) - temp_r20_4;
            M2C_FIELD(var_r6, s32 *, 0x20) = (s32) (temp_r11_3 + temp_r20_4);
            M2C_FIELD(var_r6, s32 *, 0xC0) = (s32) (temp_r11_3 - temp_r20_4);
            temp_r0_6 = (((s32) (temp_r30_2 * 0x8A9) >> 0xB) - temp_r20_3) + temp_r10_6;
            M2C_FIELD(var_r6, s32 *, 0x40) = (s32) (temp_r28_2 + temp_r10_6);
            M2C_FIELD(var_r6, s32 *, 0xA0) = (s32) (temp_r28_2 - temp_r10_6);
            M2C_FIELD(var_r6, s32 *, 0x80) = (s32) (temp_r31_3 + temp_r0_6);
            M2C_FIELD(var_r6, s32 *, 0x60) = (s32) (temp_r31_3 - temp_r0_6);
            var_r6 += 4;
        }
    }
    var_r6_2 = (u8 *)&sp8;
    for (var_ctr_2 = 0; var_ctr_2 < 8; var_ctr_2++) {
        temp_r22_3 = M2C_FIELD(var_r6_2, s32 *, 8);
        temp_r21_2 = M2C_FIELD(var_r6_2, s32 *, 0x18);
        temp_r10_7 = M2C_FIELD(var_r6_2, s32 *, 0x14);
        temp_r0_7 = M2C_FIELD(var_r6_2, s32 *, 0xC);
        temp_r9 = M2C_FIELD(var_r6_2, s32 *, 4);
        temp_r5 = M2C_FIELD(var_r6_2, s32 *, 0x1C);
        temp_r12_3 = temp_r10_7 - temp_r0_7;
        temp_r23_3 = temp_r10_7 + temp_r0_7;
        temp_r25_3 = M2C_FIELD(var_r6_2, s32 *, 0);
        temp_r10_8 = M2C_FIELD(var_r6_2, s32 *, 0x10);
        temp_r30_3 = temp_r9 - temp_r5;
        temp_r0_8 = temp_r9 + temp_r5;
        temp_r24_3 = temp_r22_3 + temp_r21_2;
        temp_r27_2 = temp_r25_3 + temp_r10_8;
        temp_r26_4 = temp_r25_3 - temp_r10_8;
        temp_r25_4 = ((s32) ((temp_r22_3 - temp_r21_2) * 0xB50) >> 0xB) - temp_r24_3;
        temp_r10_9 = temp_r0_8 + temp_r23_3;
        temp_r29_3 = temp_r27_2 + temp_r24_3;
        temp_r11_4 = temp_r26_4 + temp_r25_4;
        temp_r28_3 = temp_r26_4 - temp_r25_4;
        temp_r0_9 = (temp_r29_3 - temp_r10_9) + 0x7F;
        temp_r26_5 = (s32) ((temp_r0_8 - temp_r23_3) * 0xB50) >> 0xB;
        temp_r31_4 = temp_r27_2 - temp_r24_3;
        temp_r5_2 = temp_r30_3 * 0x8A9;
        temp_r20_5 = (s32) ((temp_r12_3 + temp_r30_3) * 0xEC8) >> 0xB;
        temp_r9_2 = temp_r29_3 + temp_r10_9 + 0x7F;
        var_r6_2 += 0x20;
        M2C_FIELD(var_r3, s8 *, 0) = (s8) (M2C_FIELD(var_r7, u8 *, 0) + (temp_r9_2 >> 8));
        temp_r20_6 = (temp_r20_5 + ((s32) (temp_r12_3 * -0x14E8) >> 0xB)) - temp_r10_9;
        temp_r10_10 = temp_r26_5 - temp_r20_6;
        M2C_FIELD(var_r3, s8 *, 7) = (s8) (M2C_FIELD(var_r7, u8 *, 7) + (temp_r0_9 >> 8));
        temp_r0_10 = ((temp_r5_2 >> 0xB) - temp_r20_5) + temp_r10_10;
        M2C_FIELD(var_r3, s8 *, 1) = (s8) (M2C_FIELD(var_r7, u8 *, 1) + ((s32) (temp_r11_4 + temp_r20_6 + 0x7F) >> 8));
        M2C_FIELD(var_r3, s8 *, 6) = (s8) (M2C_FIELD(var_r7, u8 *, 6) + ((s32) ((temp_r11_4 - temp_r20_6) + 0x7F) >> 8));
        M2C_FIELD(var_r3, s8 *, 2) = (s8) (M2C_FIELD(var_r7, u8 *, 2) + ((s32) (temp_r28_3 + temp_r10_10 + 0x7F) >> 8));
        M2C_FIELD(var_r3, s8 *, 5) = (s8) (M2C_FIELD(var_r7, u8 *, 5) + ((s32) ((temp_r28_3 - temp_r10_10) + 0x7F) >> 8));
        M2C_FIELD(var_r3, s8 *, 3) = (s8) (M2C_FIELD(var_r7, u8 *, 3) + ((s32) ((temp_r31_4 - temp_r0_10) + 0x7F) >> 8));
        temp_r5_3 = M2C_FIELD(var_r7, u8 *, 4);
        var_r7 += arg5;
        M2C_FIELD(var_r3, s8 *, 4) = (s8) (temp_r5_3 + ((s32) (temp_r31_4 + temp_r0_10 + 0x7F) >> 8));
        var_r3 += arg1;
    }
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
