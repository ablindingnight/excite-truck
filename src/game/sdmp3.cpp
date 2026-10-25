// SD card MP3 player (0x802E9A3C-0x802EC4BC): scans the SD card for .mp3 files (EXCITE TRUCK folder
// first, then the root), reads ID3 tags, and decodes the chosen track on its own thread into a ring of
// PCM that two streamed voices (left/right) pull from, waiting for the voices to drain when the sample
// rate or channel count changes. Built-in tracks can also be played from 'MMP3' resources. Names are
// guesses apart from the strings.
#include "game/common.h"
#include <string.h>

extern "C" {
void OSReport(const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);  // fatal
void fn_802E30C8(const char* fmt, ...);  // warning print
int fn_801345D8(char* dst, int n, const char* fmt, ...);
int fn_802E2C20(void);  // milliseconds
int strcasecmp(const char* a, const char* b);
char* strrchr(const char* s, int c);
void OSSleepTicks(s64 ticks);
void OSInitMutex(void* m);
void OSLockMutex(void* m);
void OSUnlockMutex(void* m);
int OSTryLockMutex(void* m);
int OSCreateThread(void* t, void* (*fn)(void*), void* arg, void* stack, u32 size, s32 prio, u16 attr);
s32 OSResumeThread(void* t);
s32 VFInit(u32 config);
s32 VFSetMediaCallback(s32 drive, void (*ins)(int), void (*rem)(int));
s32 VFAttachDrive(s32 a, s32 b, s32 c, void* drive);
s32 VFUnmountDrive(char drive, s32 mode);
s32 VFDetachDrive(char drive);
void* VFOpenFile(const char* path, const char* mode);
s32 VFCloseFile(void* fp);
s32 VFGetFileInfo(void* fp, void* info);
s32 VFSeekFile(void* fp, s32 pos, s32 origin);
s32 VFReadFile(void* buf, u32 size, u32 n, void* fp);
s32 VFFindFirst(const char* pattern, s32 attr, void* fd);
s32 VFFindNext(void* fd);
void* TMCCMP3Init(u8* buf, u32 n, u8* heap, s32 size);
void MP3_Delete(void* d);
void TMCCMP3GetInfo(void* d, void* info);
s32 TMCCMP3Decode(void* d, s16* out, u8* buf, u32 n, s32 pcm, s32* used, s32 mode);
void fn_802E3DCC(void);
void* fn_802E5DD8(const char* name, s32 size, s32 a, int (*fill)(void*, int, int), int ch);
void fn_802E5FE4(void* stream);
void* fn_802E6058(void* stream);
void fn_802E6138(void* voice);
void fn_802E61BC(void* voice);
void fn_802E61D8(void* voice);
void fn_802E61E4(void* voice, f32 vol);
void fn_802E61EC(void* voice, f32 pan);
void fn_802E620C(void* voice, f32 x);
void fn_802E622C(void* voice, f32 pitch);
void fn_802E2E0C(int (*fn)(int), const char* name);
void fn_80127C90(void (*fn)(void), const char* name);
void fn_80127D58(void (*fn)(void));
void* fn_8012C370(const char* name, u32 type, u32* version, u32* size, int flags, u8* swapped);
}

void* operator new[](size_t size, const char* tag);
void operator delete[](void* p);

#define OS_TIMER_CLOCK (*(u32*)0x800000F8 / 4)
#define OSMillisecondsToTicks(ms) ((ms) * (OS_TIMER_CLOCK / 1000))

struct MP3Info {
    s32 mpeg1;        // offset 0x0
    s32 sampleRate;   // offset 0x4
    s32 channels;     // offset 0x8
    s32 frameBytes;   // offset 0xC
    s32 pcmBytes;     // offset 0x10
};

struct Track {
    char dir[0x200];   // offset 0x0
    char file[0x208];  // offset 0x200
    char name[0x208];  // offset 0x408
};

struct TagInfo {
    char title[0x20];   // offset 0x0
    char artist[0x20];  // offset 0x20
    char album[0x20];   // offset 0x40
};

struct Res {
    void* data;  // offset 0x0
    s32 size;    // offset 0x4
};

struct DriveCache {
    void* buf;   // offset 0x0
    void* buf2;  // offset 0x4
    u16 a;       // offset 0x8
    u16 b;       // offset 0xA
    u32 c;       // offset 0xC
    u32 pad10;
};

struct Drive {
    u8 pad0[4];
    DriveCache* cache;  // offset 0x4
    char letter;         // offset 0x8
    u8 flags;           // offset 0x9
    u8 padA[2];
};

struct FindData {
    u8 pad0[0x22C];
    u8 attr;              // offset 0x22C
    char shortName[0xD];  // offset 0x22D
    char longName[0x20E]; // offset 0x23A
};

struct Signal {
    Signal(void) {
        seen = 0;
        cur = 0;
        Raise();
    }
    void Raise(void) { cur++; }
    int Poll(void) {
        s32 c = cur;
        if (c != seen) {
            seen = c;
            return 1;
        }
        return 0;
    }
    volatile s32 cur;  // offset 0x0
    volatile s32 seen;  // offset 0x4
};

static u8 lbl_80507600[0x40];
static TagInfo lbl_80507640;
static Res lbl_805076A0[3];
static Drive lbl_805076B8;
static DriveCache lbl_805076C4;
static u8 lbl_805076D8[0x140];
static u8 lbl_80507818[0x1000];
static Track lbl_80508818[100];  // scanned on the thread
static Track lbl_8052E658[100];  // the main thread's copy
static u8 lbl_805544A0[0x4000] __attribute__((aligned(32)));  // thread stack
static u8 lbl_805584A0[0x18];    // OSMutex
static u8 lbl_805584B8[0x328];   // OSThread
static u8 lbl_805587E0[0x80];    // ID3v1 tag

s32 lbl_8055D25C = -1;  // decoded sample rate
s32 lbl_8055D260 = -1;  // rate the voices play at
volatile s32 lbl_8055D264 = -1;  // rate wanted
s32 lbl_8055D268 = 2;   // decoded channels
s32 lbl_8055D26C = 2;   // channels in the PCM ring
f32 lbl_8055D270 = 1.0f;
const char* lbl_8055D274 = "EXCITE TRUCK";

ModuleTag lbl_80560058;
u8 lbl_80560060;  // reader in use
volatile u8 lbl_80560061;  // SD inserted
u8 lbl_80560062;  // thread running
u8 lbl_80560063;  // thread finished
Signal lbl_80560068;  // SD changed
Signal lbl_80560070;  // track list changed
s32 lbl_80560078;     // track requested
s32 lbl_8056007C;     // scanned track count
s32 lbl_80560080;     // track count
void* lbl_80560088[2];  // streams
void* lbl_80560090[2];  // voices
void* lbl_80560098;     // decoder
u8* lbl_8056009C;       // decoder heap
u8* lbl_805600A0;       // PCM ring
s32 lbl_805600A4;       // PCM ring size
u8* lbl_805600A8;       // MP3 read buffer
s32 lbl_805600AC;       // MP3 read buffer size
volatile s32 lbl_805600B0[2];  // read positions
volatile s32 lbl_805600B8;  // write position
s32 lbl_805600BC;       // bytes buffered
volatile s32 lbl_805600C0[2];  // underruns
u8 lbl_805600C8;        // abort requested
u8 lbl_805600C9;        // fading out
s32 lbl_805600CC;       // fade start
volatile u8 lbl_805600D0;  // enabled
volatile u8 lbl_805600D1;  // restart
u8 lbl_805600D2;        // channel drift

namespace {

struct Mp3Reader {
    virtual ~Mp3Reader(void) {}
    virtual int Seek(int pos) = 0;
    virtual int Size(void) = 0;
    virtual int Read(void* buf, int n) = 0;
    static void* operator new(size_t size) {
        lbl_80560060 = 1;
        return lbl_80507600;
    }
    static void operator delete(void* p) { lbl_80560060 = 0; }
};

struct MemReader : Mp3Reader {
    MemReader(u8* data, int size) {
        this->data = data;
        this->size = size;
    }
    virtual ~MemReader(void) {}
    virtual int Seek(int pos) {
        if (pos >= 0 && pos < size) {
            this->pos = pos;
            return 1;
        }
        return 0;
    }
    virtual int Size(void) { return size; }
    virtual int Read(void* buf, int n) {
        if (pos >= 0 && pos + n <= size) {
            memcpy(buf, data + pos, n);
            pos += n;
            return 1;
        }
        return 0;
    }
    u8* data;  // offset 0x4
    s32 pos;   // offset 0x8
    s32 size;  // offset 0xC
};

struct FileReader : Mp3Reader {
    FileReader(const char* path) {
        fp = VFOpenFile(path, "r");
        size = 0;
        if (fp) {
            u32 info[7];
            if (VFGetFileInfo(fp, info) == 0) {
                size = info[0];
            } else {
                fn_802E30C8("mp3: can't get info for %s", path);
                VFCloseFile(fp);
                fp = 0;
            }
        }
    }
    virtual ~FileReader(void);
    virtual int Seek(int pos) { return VFSeekFile(fp, pos, 0) == 0; }
    virtual int Size(void) { return size; }
    virtual int Read(void* buf, int n) { return VFReadFile(buf, n, 1, fp) == 1; }
    void* fp;  // offset 0x4
    s32 size;  // offset 0x8
};

}  // namespace

extern "C" {

Mp3Reader* fn_802E9A3C(u8* data, int size) { return new MemReader(data, size); }

Mp3Reader* fn_802E9A6C(const char* path) {
    FileReader* r = new FileReader(path);
    if (r->fp) {
        return r;
    }
    delete r;
    return 0;
}

}  // extern "C"

FileReader::~FileReader(void) {
    if (fp) {
        VFCloseFile(fp);
        fp = 0;
    }
}

extern "C" {

void fn_802E9BCC(Mp3Reader* r) { delete r; }

const char* fn_802E9BEC(int err) {
    switch (err) {
    case 0:
        return "NO_ERR";
    case 8:
        return "SYNC_ERR";
    case 13:
        return "LAYER_ERR";
    case 14:
        return "BITRATE_ERR";
    case 15:
        return "SFREQ_ERR";
    case 17:
        return "BLOCKTYPE_ERR";
    case 18:
        return "TABLE_SEL_ERR";
    case 19:
        return "LENGTH_ERR";
    case -10:
        return "BYTES_DISC_ERR";
    }
    return "???";
}

void fn_802E9C68(int drive) {
    OSReport(" mp3: SD card inserted (%c)\n", drive);
    lbl_80560061 = 1;
    lbl_80560068.Raise();
}

void fn_802E9CAC(int drive) {
    OSReport(" mp3: SD card removed (%c)\n", drive);
    lbl_80560061 = 0;
    lbl_80560068.Raise();
}

void fn_802E9CF0(void) {
    DriveCache* c = &lbl_805076C4;
    c->buf = lbl_805076D8;
    c->buf2 = lbl_80507818;
    c->a = 4;
    c->b = 4;
    c->c = 1;
    lbl_805076B8.cache = c;
    if (VFInit(0x20000)) {
        fn_802E317C("FAInit fails");
    }
    if (VFSetMediaCallback(0, fn_802E9C68, fn_802E9CAC)) {
        fn_802E317C("FARegistCB fails");
    }
    if (VFAttachDrive(0, 0, 0, &lbl_805076B8)) {
        fn_802E317C("FAAttach fails");
    }
    lbl_80560061 = (lbl_805076B8.flags >> 4) & 1;
}

void fn_802E9DDC(void) {
    if (VFUnmountDrive(lbl_805076B8.letter, 1)) {
        fn_802E30C8("FAUnmount fails");
    }
    if (VFDetachDrive(lbl_805076B8.letter)) {
        fn_802E317C("FADetach fails");
    }
}

void fn_802E9E54(char* dst, const char* src, int n) {
    while (n > 1 && *src) {
        if ((u8)*src < 0x80) {
            *dst++ = *src++;
            n--;
        } else {
            char c = *++src;
            n--;
            *dst++ = '?';
            if (c) {
                src++;
            }
        }
    }
    *dst = 0;
}

static inline f32 Ramp(f32 x, f32 a, f32 len) {
    f32 t = (x - a) / len;
    t = 1.0f < t ? 1.0f : t;
    return t > 0.0f ? t : 0.0f;
}

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

void fn_802E9EB8(void) {
    if (lbl_805600D2) {
        fn_802E30C8("mp3: trying to restart after channel drift");
        for (int i = 0; i < 2; i++) {
            fn_802E61BC(lbl_80560090[i]);
        }
        lbl_805600D2 = 0;
    }
    if (OSTryLockMutex(lbl_805584A0)) {
        if (lbl_80560070.Poll()) {
            lbl_80560080 = lbl_8056007C;
            for (int i = 0; i < lbl_80560080; i++) {
                lbl_8052E658[i] = lbl_80508818[i];
            }
            fn_802E30C8("mp3: %d tracks", lbl_80560080);
            for (int i = 0; i < lbl_80560080; i++) {
                fn_802E30C8("%d: \"%s\"", i, lbl_8052E658[i].name);
            }
        }
        OSUnlockMutex(lbl_805584A0);
    }
    if ((lbl_805600D1 || lbl_8055D264 != lbl_8055D260) && (lbl_805600BC >= 0x23FE0 || !lbl_805600D1)) {
        fn_802E30C8("mp3: starting at W%d (R%d, R%d)", lbl_805600B8, lbl_805600B0[0], lbl_805600B0[1]);
        if (lbl_805600B0[0] != lbl_805600B0[1]) {
            fn_802E30C8("mp3: ==== TWO TRACKS OUT OF SYNC! =====");
            s32 r = lbl_805600B0[0];
            for (int i = 0; i < 2; i++) {
                fn_802E61D8(lbl_80560090[i]);
                lbl_805600B0[i] = r;
                lbl_805600D2 = 1;
            }
        }
        lbl_805600D1 = 0;
        for (int i = 0; i < 2; i++) {
            fn_802E622C(lbl_80560090[i], (f32)lbl_8055D264 / 44100.0f);
        }
        lbl_8055D260 = lbl_8055D264;
    }
    f32 fade;
    if (lbl_805600C9) {
        fade = Lerp(1.0f, 0.0f, Ramp(0.001f * (f32)(fn_802E2C20() - lbl_805600CC), 0.0f, 0.25f));
    } else {
        fade = 1.0f;
    }
    for (int i = 0; i < 2; i++) {
        fn_802E61E4(lbl_80560090[i], (lbl_805600D0 && !lbl_805600D1) ? lbl_8055D270 * fade : 0.0f);
        fn_802E620C(lbl_80560090[i], 0.4f);
    }
}

void fn_802EA5D4(const char* dir, int depth) {
    char pat[0x100];
    char sub[0x200];
    FindData fd;
    fn_801345D8(pat, 0x100, "%s/*", dir);
    if (VFFindFirst(pat, 0x71, &fd) == 0) {
        do {
            if (lbl_8056007C >= 100) {
                OSReport(" mp3: too many tracks\n");
                break;
            }
            char* name = fd.shortName;
            if (fd.longName[0]) {
                name = fd.longName;
            }
            if ((fd.attr & 0x10) && depth > 0) {
                if (name[0] && strcmp(name, ".") && strcmp(name, "..")) {
                    if (strlen(dir) + strlen(fd.shortName) + 1 < 0x200) {
                        fn_801345D8(sub, 0x200, "%s/%s", dir, fd.shortName);
                        fn_802EA5D4(sub, depth - 1);
                    } else {
                        fn_802E30C8("mp3: sub path would be too long");
                    }
                }
            } else {
                char* ext = strrchr(name, '.');
                if (ext && !strcasecmp(ext, ".mp3")) {
                    Track* t = &lbl_80508818[lbl_8056007C];
                    if (strlen(fd.shortName) < 0x208 && strlen(dir) < 0x200) {
                        strncpy(t->file, fd.shortName, 0x207);
                        t->file[0x207] = 0;
                        fn_802E9E54(t->name, name, 0x208);
                        strncpy(t->dir, dir, 0x1FF);
                        t->dir[0x1FF] = 0;
                        lbl_8056007C++;
                    } else {
                        fn_802E30C8("mp3: filename too long");
                    }
                }
            }
        } while (VFFindNext(&fd) == 0);
    }
}

void fn_802EA7E0(void) {
    lbl_8056007C = 0;
    if (lbl_80560061) {
        fn_802EA5D4(lbl_8055D274, 0);
        if (lbl_8056007C == 0) {
            fn_802EA5D4("", 8);
        }
    }
}

Mp3Reader* fn_802EA834(int idx, s32* size) {
    int k;
    Mp3Reader* r = 0;
    if (idx <= -2) {
        k = -2 - idx;
        if (k >= 0 && k < 3 && lbl_805076A0[k].data) {
            r = fn_802E9A3C((u8*)lbl_805076A0[k].data, lbl_805076A0[k].size);
            if (r) {
                strncpy(lbl_80507640.title, "", 0x1F);
                lbl_80507640.title[0x1F] = 0;
            }
        }
    } else {
        char path[0x40A];
        char* file = lbl_80508818[idx].file;
        fn_801345D8(path, 0x40A, "%s/%s", lbl_80508818[idx].dir, file);
        fn_802E30C8("+OpenSd(%s)...", path);
        r = fn_802E9A6C(path);
        fn_802E30C8("-OpenSD(%s): %p", path, r);
        if (r) {
            fn_802E30C8("mp3: file %s is %d bytes", path, r->Size());
            strncpy(lbl_80507640.title, file, 0x1F);
            lbl_80507640.title[0x1F] = 0;
        } else {
            fn_802E30C8("mp3: can't open %s from SD", path);
        }
    }
    if (r) {
        *size = r->Size();
    } else {
        fn_802E30C8("mp3: can't open track %d", idx);
        if (lbl_80560062) {
            OSSleepTicks(OSMillisecondsToTicks(1000));
        }
    }
    return r;
}

void fn_802EAA08(void) {
    if (lbl_805600B8 >= lbl_805600A4) {
        memcpy(lbl_805600A0, lbl_805600A0 + lbl_805600A4, lbl_805600B8 - lbl_805600A4);
        lbl_805600B8 -= lbl_805600A4;
    }
}

static inline bool ChannelsOk(const MP3Info* info) { return info->channels == 1 || info->channels == 2; }
static inline bool InRange(s32 v, s32 lo, s32 hi) { return v >= lo && v <= hi; }
static inline bool RangeOk(const MP3Info* info) { return InRange(info->sampleRate, 8000, 60000); }
static inline bool FormatOk(const MP3Info* info) { return ChannelsOk(info) && RangeOk(info); }
static inline bool PcmOk(const MP3Info* info) { return info->pcmBytes != 0 && (u32)info->pcmBytes <= 0x1200; }

int fn_802EAA58(MP3Info* info, int strict) {
    if (strict) {
        switch (info->sampleRate) {
        case 11025:
        case 22050:
        case 44100:
            break;
        default:
            return 0;
        }
    }
    int ok = 0;
    if (FormatOk(info) && PcmOk(info)) {
        ok = 1;
    }
    return ok;
}

static inline int SyncAt(const u8* b) { return b[0] == 0xFF && (b[1] & 0xE0) == 0xE0; }

void* fn_802EAB28(void* arg) {
    OSReport("kerngc/mp3:thread begins!\n");
    Mp3Reader* r = 0;
    s32 size = 0;
    volatile u8 decoded = 0;
    int pos = 0;
    int errs = 0;
    MP3Info info;
    s32 used;
    while (lbl_80560062) {
        if (decoded) {
            OSSleepTicks(OSMillisecondsToTicks(5));
            decoded = 0;
        } else {
            OSSleepTicks(OSMillisecondsToTicks(50));
        }
        OSLockMutex(lbl_805584A0);
        if (lbl_80560068.Poll()) {
            if (lbl_80560098) {
                MP3_Delete(lbl_80560098);
                lbl_80560098 = 0;
            }
            if (r) {
                fn_802E9BCC(r);
                r = 0;
            }
            fn_802EA7E0();
            lbl_80560070.Raise();
            lbl_805600C9 = 0;
        }
        if (!r && lbl_80560078 != -1 && lbl_80560078 < lbl_8056007C) {
            pos = 0;
            size = 0;
            r = fn_802EA834(lbl_80560078, &size);
            errs = 0;
            if (r && r->Seek(r->Size() - 0x80) && r->Read(lbl_805587E0, 0x80) &&
                strncmp((char*)lbl_805587E0, "TAG", 3) == 0) {
                strncpy(lbl_80507640.title, (char*)lbl_805587E0 + 3, 0x1E);
                lbl_80507640.title[0x1E] = 0;
                strncpy(lbl_80507640.artist, (char*)lbl_805587E0 + 0x21, 0x1E);
                lbl_80507640.artist[0x1E] = 0;
                strncpy(lbl_80507640.album, (char*)lbl_805587E0 + 0x3F, 0x1E);
                lbl_80507640.album[0x1E] = 0;
            }
            lbl_80560078 = -1;
        }
        if (lbl_805600C8) {
            if (!lbl_805600C9) {
                lbl_805600CC = fn_802E2C20();
                lbl_805600C9 = 1;
            }
            if (r) {
                pos = size;
            }
            fn_802E30C8("mp3: aborting current track");
            if (!r) {
                lbl_805600C8 = 0;
                fn_802E30C8("mp3: completed aborting current track (trivial)");
            }
        }
        if (r) {
            int left = size - pos;
            if (left < 0x780) {
                fn_802E30C8("mp3: perhaps not enough bytes left?");
            }
            int n = lbl_805600AC < left ? lbl_805600AC : left;
            int done = 1;
            if (pos >= size) {
                fn_802E30C8("mp3: at end of file");
                done = 1;
            } else if (!r->Seek(pos)) {
                fn_802E30C8("mp3: FAFseek fails");
            } else if (!r->Read(lbl_805600A8, n)) {
                fn_802E30C8("mp3: FAFread fails");
            } else {
                done = 0;
                int off = 0;
                int hdr = 0;
                if (!lbl_80560098 && strncmp((char*)lbl_805600A8, "ID3", 3) == 0) {
                    u8* b = lbl_805600A8;
                    int tagSize = (b[6] << 21) + (b[7] << 14) + (b[8] << 7) + b[9] + 10;
                    u16 ver = (b[3] << 8) | b[4];
                    if (tagSize < n) {
                        int p = 10;
                        if (ver >= 0x200 && ver <= 0x2FF) {
                            while (p + 6 < n) {
                                if (!lbl_805600A8[p]) {
                                    break;
                                }
                                char* f = (char*)lbl_805600A8 + p;
                                int fsz = ((u8)f[3] << 14) + ((u8)f[4] << 7) + (u8)f[5] + 6;
                                if (f[0] == 'T') {
                                    char* txt = f + 7;
                                    if (!strncmp(f, "TT2", 3)) {
                                        strncpy(lbl_80507640.title, txt, 0x1F);
                                        lbl_80507640.title[0x1F] = 0;
                                    } else if (!strncmp(f, "TP1", 3)) {
                                        strncpy(lbl_80507640.artist, txt, 0x1F);
                                        lbl_80507640.artist[0x1F] = 0;
                                    } else if (!strncmp(f, "TAL", 3)) {
                                        strncpy(lbl_80507640.album, txt, 0x1F);
                                        lbl_80507640.album[0x1F] = 0;
                                    }
                                }
                                p += fsz;
                                if (!fsz) {
                                    break;
                                }
                            }
                        } else if (ver >= 0x300 && ver <= 0x3FF) {
                            while (p + 10 < n) {
                                if (!lbl_805600A8[p]) {
                                    break;
                                }
                                char* f = (char*)lbl_805600A8 + p;
                                int len = ((u8)f[4] << 21) + ((u8)f[5] << 14) + ((u8)f[6] << 7) + (u8)f[7];
                                int tl = len - 1;
                                int fsz = len + 10;
                                if (f[0] == 'T' && tl > 0) {
                                    char* txt = f + 11;
                                    if (!strncmp(f, "TIT2", 4)) {
                                        int k = tl + 1 < 0x20 ? tl + 1 : 0x20;
                                        strncpy(lbl_80507640.title, txt, k - 1);
                                        lbl_80507640.title[k - 1] = 0;
                                    }
                                    if (!strncmp(f, "TPE1", 4)) {
                                        int k = tl + 1 < 0x20 ? tl + 1 : 0x20;
                                        strncpy(lbl_80507640.artist, txt, k - 1);
                                        lbl_80507640.artist[k - 1] = 0;
                                    }
                                    if (!strncmp(f, "TALB", 4)) {
                                        int k = tl + 1 < 0x20 ? tl + 1 : 0x20;
                                        strncpy(lbl_80507640.album, txt, k - 1);
                                        lbl_80507640.album[k - 1] = 0;
                                    }
                                }
                                p += fsz;
                                if (!fsz) {
                                    break;
                                }
                            }
                        } else {
                            fn_802E30C8("mp3: unhandled tag ver %04x", ver);
                        }
                        off = tagSize;
                    } else {
                        fn_802E30C8("mp3: header is too big for us! (%d >= %d)", tagSize, n);
                    }
                    pos += tagSize;
                    hdr = 1;
                }
                if (!hdr) {
                    int found = 0;
                    for (int i = 0; i < (n > 0x400 ? 0x400 : n); i++) {
                        if (SyncAt(lbl_805600A8 + i)) {
                            if (i) {
                                fn_802E30C8("mp3: found header frame sync at %d", i);
                            }
                            off = i;
                            pos += i;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        fn_802E30C8("mp3: couldn't find sync");
                    }
                }
                if (!lbl_80560098) {
                    if (!hdr) {
                        lbl_80560098 = TMCCMP3Init(lbl_805600A8 + off, n - off, lbl_8056009C, 0x10000 - 0x1280);
                        if (!lbl_80560098) {
                            fn_802E30C8("mp3: TMCCMP3Init fails");
                            done = 1;
                        }
                    }
                    if (lbl_80560098) {
                        memset(&info, 0, sizeof(info));
                        TMCCMP3GetInfo(lbl_80560098, &info);
                        fn_802E30C8("mpegVersion = %d", info.mpeg1);
                        fn_802E30C8("sampleRate = %d", info.sampleRate);
                        fn_802E30C8("channelCount = %d", info.channels);
                        fn_802E30C8("frameSize = %d", info.frameBytes);
                        fn_802E30C8("pcmSize = %d", info.pcmBytes);
                        fn_802E30C8("sizeof pcmSize = %d", sizeof(info.pcmBytes));
                        if (!fn_802EAA58(&info, errs == 0)) {
                            MP3_Delete(lbl_80560098);
                            errs++;
                            lbl_80560098 = 0;
                            if (errs > 16) {
                                fn_802E30C8("mp3: info is invalid--aborting track");
                                done = 1;
                                OSSleepTicks(OSMillisecondsToTicks(100));
                            } else {
                                fn_802E30C8("mp3: header invalid, skipping forward to retry (%d)", errs);
                                pos += 0x780;
                                OSSleepTicks(OSMillisecondsToTicks(30));
                            }
                        }
                    }
                }
                if (lbl_80560098) {
                    lbl_8055D25C = info.sampleRate;
                    lbl_8055D268 = info.channels;
                    s32 pcm = info.pcmBytes;
                    if (lbl_8055D25C != lbl_8055D260 || lbl_8055D268 != lbl_8055D26C || lbl_805600C9) {
                        fn_802E30C8("mp3: waiting for channel to starve");
                        while (lbl_805600C0[1] < 4 && !lbl_805600D1 && lbl_805600D0) {
                            OSSleepTicks(OSMillisecondsToTicks(5));
                        }
                        lbl_8055D26C = lbl_8055D268;
                        lbl_8055D264 = lbl_8055D25C;
                        fn_802E30C8("mp3: waiting for sample rate change");
                        while (lbl_8055D260 != lbl_8055D264 && !lbl_805600D1 && lbl_805600D0) {
                            OSSleepTicks(OSMillisecondsToTicks(5));
                        }
                        fn_802E30C8("mp3: completed sample rate change");
                        OSSleepTicks(OSMillisecondsToTicks(50));
                        lbl_805600C9 = 0;
                    }
                    used = 0;
                    int need = pcm * 2;
                    s32 rp = lbl_805600B0[0];
                    while (off + 1 <= n) {
                        int space = lbl_805600B8 < rp ? rp - lbl_805600B8 : lbl_805600A4 - (lbl_805600B8 - rp);
                        if (space < need) {
                            break;
                        }
                        int skip = -1;
                        for (int i = 0; i < (n > 0x400 ? 0x400 : n); i++) {
                            if (SyncAt(lbl_805600A8 + off + i)) {
                                if (i) {
                                    u8* b = lbl_805600A8 + off;
                                    fn_802E30C8("mp3: found inner frame sync at %d (%x,%x,%x,%x)", i, b[0], b[1], b[2],
                                                b[3]);
                                }
                                skip = i;
                                off += i;
                                pos += i;
                                break;
                            }
                        }
                        if (skip < 0) {
                            fn_802E30C8("mp3: couldn't find sync");
                        }
                        if (skip > 0) {
                            continue;
                        }
                        s32 rc = TMCCMP3Decode(lbl_80560098, (s16*)(lbl_805600A0 + lbl_805600B8), lbl_805600A8 + off,
                                               0x780, info.pcmBytes, &used, 0);
                        if (rc == 0) {
                            lbl_805600B8 += pcm;
                            pos += used;
                            off += used;
                        } else {
                            errs++;
                            fn_802E30C8("mp3: TMCCMP3Decode returns %d (%s) c=%d", rc, fn_802E9BEC(rc), used);
                            if (!used || errs > 30) {
                                done = 1;
                                break;
                            }
                            pos += used;
                            off += used;
                            lbl_805600B8 += pcm;
                        }
                        decoded = 1;
                        fn_802EAA08();
                    }
                    if (lbl_805600B8 > rp) {
                        lbl_805600BC = lbl_805600B8 - rp;
                    } else {
                        lbl_805600BC = lbl_805600A4 - (rp - lbl_805600B8);
                    }
                } else {
                    lbl_805600BC = 0;
                }
            }
            if (pos >= size) {
                fn_802E30C8("mp3: done with current file!");
                done = 1;
            }
            if (done) {
                fn_802E30C8("mp3: closing file");
                if (lbl_80560098) {
                    MP3_Delete(lbl_80560098);
                    lbl_80560098 = 0;
                }
                if (r) {
                    fn_802E9BCC(r);
                    fn_802E30C8("mp3: did Close");
                    r = 0;
                }
                memset(&lbl_80507640, 0, sizeof(lbl_80507640));
            }
            if (lbl_805600C8 && !r && !lbl_80560098) {
                fn_802E30C8("mp3: completed aborting current track");
                lbl_805600C8 = 0;
            }
        }
        OSUnlockMutex(lbl_805584A0);
    }
    if (r) {
        fn_802E9BCC(r);
    }
    lbl_80560063 = 1;
    return 0;
}

int fn_802EB7D4(void* dst, int n, int ch) {
    s32 nch = lbl_8055D26C;
    if (lbl_805600B8 == lbl_805600B0[ch]) {
        memset(dst, 0, n);
        lbl_805600C0[ch]++;
    } else if (lbl_805600B0[ch] < lbl_805600B8) {
        int want = n * nch;
        int avail = want < lbl_805600B8 - lbl_805600B0[ch] ? want : lbl_805600B8 - lbl_805600B0[ch];
        if (avail < want && lbl_805600C0[ch]) {
            memset(dst, 0, n);
            lbl_805600C0[ch]++;
            return n;
        }
        int cnt = avail / (nch * 2);
        lbl_805600C0[ch] = 0;
        u16* out = (u16*)dst;
        u16* src = (u16*)(lbl_805600A0 + lbl_805600B0[ch]);
        if (nch == 2) {
            for (int i = 0; i < cnt; i++) {
                out[i] = src[i * 2 + ch];
            }
        } else {
            for (int i = 0; i < cnt; i++) {
                out[i] = src[i];
            }
        }
        lbl_805600B0[ch] += avail;
        if (lbl_805600B0[ch] >= lbl_805600A4) {
            lbl_805600B0[ch] = 0;
        }
    } else {
        int want = n * nch;
        int toEnd = lbl_805600A4 - lbl_805600B0[ch];
        int total = lbl_805600A4 - (lbl_805600B0[ch] - lbl_805600B8);
        int avail = want < toEnd ? want : toEnd;
        if (total < want && lbl_805600C0[ch]) {
            memset(dst, 0, n);
            lbl_805600C0[ch]++;
            return n;
        }
        int cnt = avail / (nch * 2);
        int got = avail / nch;
        lbl_805600C0[ch] = 0;
        u16* out = (u16*)dst;
        u16* src = (u16*)(lbl_805600A0 + lbl_805600B0[ch]);
        if (nch == 2) {
            for (int i = 0; i < cnt; i++) {
                out[i] = src[i * 2 + ch];
            }
        } else {
            for (int i = 0; i < cnt; i++) {
                out[i] = src[i];
            }
        }
        lbl_805600B0[ch] += avail;
        if (lbl_805600B0[ch] >= lbl_805600A4) {
            lbl_805600B0[ch] = 0;
            if (n > got) {
                fn_802EB7D4((u8*)dst + got, n - got, ch);
            }
        }
    }
    return n;
}

void fn_802EBD40(void) {
    lbl_80560062 = 0;
    fn_802E30C8("mp3: waiting for thread to end");
    int t = 0;
    while (!lbl_80560063) {
        OSSleepTicks(OSMillisecondsToTicks(5));
        t += 5;
        if (t > 1000) {
            fn_802E30C8("mp3: timed out waiting for thread to end");
            break;
        }
    }
    fn_802E30C8("mp3: thread ended");
}

int fn_802EBE00(int ev) {
    if (ev == 1) {
        fn_802EBD40();
    }
    return 0;
}

void fn_802EBE2C(void) {
    lbl_8055D260 = -1;
    lbl_8055D264 = 44100;
    lbl_80560078 = -1;
    lbl_8055D25C = lbl_8055D264;
    lbl_8056009C = new ("mp3_heap") u8[0x10000 - 0x1280];
    lbl_805600A4 = 0x47FC0;
    fn_802E30C8("mp3: pcm_buf_size = %dK", lbl_805600A4 / 1024);
    lbl_805600A0 = new ("pcm_buf") u8[(lbl_805600A4 + 0x1200) * 2 - (lbl_805600A4 + 0x1200)];
    lbl_805600AC = 0x1E000;
    lbl_805600A8 = new ("mp3_buf") u8[lbl_805600AC * 2 - lbl_805600AC];
    lbl_805600B8 = 0x2400;
    for (int i = 0; i < 2; i++) {
        lbl_805600B0[i] = 0;
        lbl_80560090[i] = fn_802E6058(lbl_80560088[i] = fn_802E5DD8("mp3", 0x7FF8, 0, fn_802EB7D4, i));
        fn_802E61EC(lbl_80560090[i], i == 0 ? -1.0f : 1.0f);
    }
    for (int i = 0; i < 2; i++) {
        fn_802E61E4(lbl_80560090[i], 0.0f);
        fn_802E622C(lbl_80560090[i], 0.0f);
        fn_802E61BC(lbl_80560090[i]);
    }
    lbl_805600D0 = 0;
    OSInitMutex(lbl_805584A0);
    fn_802E9CF0();
    fn_802E3DCC();
    lbl_80560062 = 1;
    lbl_80560063 = 0;
    if (!OSCreateThread(lbl_805584B8, fn_802EAB28, 0, lbl_805544A0 + 0x3FE0, 0x3FE0, 14, 1)) {
        fn_802E317C("mp3: can't create thread");
    }
    fn_802E2E0C(fn_802EBE00, "mp3");
    OSResumeThread(lbl_805584B8);
    fn_80127C90(fn_802E9EB8, "mp3");
}

void fn_802EC038(void) {
    for (int i = 0; i < 2; i++) {
        if (lbl_80560090[i]) {
            fn_802E61D8(lbl_80560090[i]);
            fn_802E6138(lbl_80560090[i]);
        }
        lbl_80560090[i] = 0;
        fn_802E5FE4(lbl_80560088[i]);
        lbl_80560088[i] = 0;
    }
    fn_80127D58(fn_802E9EB8);
    fn_802EBD40();
    fn_802E9DDC();
    delete[] lbl_8056009C;
    lbl_8056009C = 0;
    delete[] lbl_805600A0;
    lbl_805600A0 = 0;
    delete[] lbl_805600A8;
    lbl_805600A8 = 0;
}

s32 fn_802EC0FC(void) { return lbl_80560080; }

const char* fn_802EC104(int i) { return lbl_8052E658[i].name; }

void fn_802EC11C(s32 i) { lbl_80560078 = i; }

s32 fn_802EC124(void) { return lbl_80560078; }

void fn_802EC12C(void) { lbl_80560078 = -1; }

void fn_802EC138(void) {
    if (!lbl_805600D0) {
        lbl_805600D1 = 1;
    }
    lbl_805600D0 = 1;
}

void fn_802EC158(void) {
    lbl_805600D0 = 0;
    lbl_805600D1 = 0;
    for (int i = 0; i < 2; i++) {
        fn_802E622C(lbl_80560090[i], 0.0f);
        fn_802E61E4(lbl_80560090[i], 0.0f);
    }
}

void fn_802EC1C0(f32 vol) {
    lbl_8055D270 = vol;
    if (lbl_805600D0) {
        fn_802EC138();
    }
}

void fn_802EC1D8(void) { lbl_805600C8 = 1; }

void fn_802EC1E4(void) {
    fn_802EC1D8();
    fn_802E30C8("mp3: blocking until track aborted");
    int start = fn_802E2C20();
    while (lbl_805600C8) {
        if (fn_802E2C20() - start > 3000) {
            fn_802E30C8("mp3: timed out!");
            break;
        }
        OSSleepTicks(OSMillisecondsToTicks(5));
    }
}

void fn_802EC294(int slot, const char* name) {
    Res* res = &lbl_805076A0[slot];
    u32 ver = 0;
    u32 size = 0;
    void* p = fn_8012C370(name, 'MMP3', &ver, &size, 0, 0);
    if (p) {
        res->data = p;
        res->size = size;
    } else {
        fn_802E30C8("mp3: can't open resource %s", name);
    }
}

}  // extern "C"
