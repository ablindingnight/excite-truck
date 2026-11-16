// DVD file system (0x802E3EB0-0x802E46AC, filecd): the disc as a read-only file system and its files,
// read through an aligned bounce buffer with retries while the drive reports a problem. Names are
// guesses apart from the messages.
#include "game/common.h"
#include <revolution/DVD/dvd.h>
#include <revolution/DVD/dvdfs.h>
#include <revolution/OS/OSError.h>
#include <revolution/VI/vi.h>

// DVDCommandBlock: the SDK's.

// DVDFileInfo: the SDK's.

extern "C" {
int strncmp(const char* a, const char* b, size_t n);
char* strcpy(char* dst, const char* src);
void* memset(void* dst, int c, size_t n);
void* memcpy(void* dst, const void* src, size_t n);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void fn_80127464(void);  // idle while waiting
}

class File {
public:
    virtual s32 Read(void* dst, s32 len) = 0;
    virtual s32 Write(const void* src, s32 len) = 0;
    virtual int ReadAsync(void* dst, s32 len) = 0;
    virtual int IsDone(void) = 0;
    virtual s32 Size(void) = 0;
    virtual s32 Tell(void) = 0;
    virtual int Seek(s32 pos) = 0;
    virtual int Skip(s32 n) = 0;
    virtual void Flush(void) = 0;
    virtual void* Disk(void) = 0;
    virtual ~File(void) {}
};

class FileSystem {
public:
    virtual ~FileSystem(void) {}
    virtual File* Create(const char* name) = 0;
    virtual File* Open(const char* name) = 0;
    virtual File* OpenWrite(const char* name) = 0;
    virtual File* OpenAppend(const char* name) = 0;
    virtual void Close(File* f) = 0;
    virtual void Print(const char* s) = 0;
    virtual class FileFinder* Finder(void);
};

extern u8 lbl_804DBAA0[0x20000];  // read buffer

class FileCD;
static inline void ReportDVDStartup(const char* message = "filecd: started DVD services") {
    OSReport((const char*)message);
}

class FileDisk : public FileSystem {
public:
    FileDisk(void) {
        DVDInit();
        ok = 1;
        ReportDVDStartup();
    }
    virtual ~FileDisk(void) {}
    virtual File* Create(const char* name);
    virtual File* Open(const char* name);
    virtual File* OpenWrite(const char* name);
    virtual File* OpenAppend(const char* name);
    virtual void Close(File* f) { delete f; }
    virtual void Print(const char* s) { OSReport((const char*)("%s"), s); }
    static void* operator new(size_t size, const char* tag);
    static void operator delete(void* p);
    FileCD* OpenCD(const char* name);

    u8 ok;  // offset 0x4
};


// FileCD's in-class methods were compiled without the global optimizer: Read keeps its always-false sync
// flag in a register and its wait loop top-tested (2026-11-14). In-class bodies take the pragma state at the
// class's closing brace, so the push/pop wraps the whole class.
#pragma push
#pragma global_optimizer off
class FileCD : public File {
public:
    FileCD(FileDisk* disk, const char* name) {
        this->disk = disk;
        if (strncmp(name, ".\\", 2) == 0) {
            name += 2;
        }
        if (*name == '\\') {
            name++;
        }
        strcpy(this->name, name);
        memset(&info, 0, sizeof(info));
        pos = 0;
        size = 0;
        char path[0x100];
        strcpy(path, this->name);
        open = DVDOpen((const char*)path, (DVDFileInfo*)&info) != 0;
        if (open) {
            size = info.size;
        }
    }
    virtual inline ~FileCD(void);
    virtual int Seek(s32 p) {
        if (p >= 0 && p <= size) {
            pos = p;
            return 1;
        }
        return 0;
    }
    virtual s32 Size(void) { return size; }
    virtual void* Disk(void) { return disk; }
    /* Helper name unproven */
    void ReadSectors(s32 n, s32 aligned, s32& got, s32 retry) {
        for (;;) {
            if (retry) {
                got = DVDReadPrio((DVDFileInfo*)&info, (void*)lbl_804DBAA0, (s32)n, (s32)aligned, (s32)2);
            } else {
                got = DVDReadAsyncPrio((DVDFileInfo*)&info, (void*)lbl_804DBAA0, (s32)n, (s32)aligned, (DVDAsyncCallback)NULL, (s32)2);
                if (got > 0) {
                    for (;;) {
                        if (DVDGetCommandBlockStatus((const DVDCommandBlock*)&info.block) == 0) {
                            break;
                        }
                        fn_80127464();
                    }
                    got = n;
                }
            }
            if (got >= 0) {
                break;
            }
            s32 st = DVDGetDriveStatus();
            if (st == 0) {
                break;
            }
            OSReport((const char*)("filecd: DVD status = %d\n"), st);
            VIFlush();
            VIWaitForRetrace();
        }
    }
    virtual s32 Read(void* dst, s32 len) {
        if (len == 0) {
            return 0;
        }
        s32 aligned;
        s32 skip;
        s32 n;
        s32 got;
        s32 retry;
        s32 cnt;
        aligned = pos & ~3;
        skip = pos - aligned;
        n = (len + skip + 0x1F) & ~0x1F;
        if (n > 0x20000) {
            n = 0x20000;
        }
        retry = 0;
        ReadSectors(n, aligned, got, retry);
        cnt = got - skip;
        if (cnt > len) {
            cnt = len;
        }
        if (cnt > 0) {
            memcpy(dst, lbl_804DBAA0 + skip, cnt);
            pos += cnt;
            return cnt;
        }
        if (cnt == 0) {
            return 0;
        }
        return -1;
        }
    virtual int ReadAsync(void* dst, s32 len) {
        if (DVDReadAsyncPrio((DVDFileInfo*)&info, (void*)dst, (s32)len, (s32)pos, (DVDAsyncCallback)NULL, (s32)2) <= 0) {
            fn_802E317C("filecd: DVDRead fails!");
        }
        pos += len;
        return 1;
    }
    virtual int IsDone(void) { return DVDGetCommandBlockStatus((const DVDCommandBlock*)&info.block) == 0; }
    virtual s32 Write(const void* src, s32 len) {
        fn_802E317C("filecd: write not supported (%s)", name);
        return -1;
    }
    virtual s32 Tell(void) { return pos; }
    virtual int Skip(s32 n) { return Seek(pos + n); }
    virtual void Flush(void) {}
    static void* operator new(size_t size, const char* tag);
    static void operator delete(void* p);

    DVDFileInfo info;  // offset 0x4
    char name[0x40];   // offset 0x40
    u8 open;           // offset 0x80
    s32 size;          // offset 0x84
    s32 pos;           // offset 0x88
    FileDisk* disk;    // offset 0x8C
};
#pragma pop




struct FileNode {
    u8 used;        // offset 0x0
    u8 data[0x90];  // offset 0x1
};

ModuleTag lbl_8055FF30;
u8 lbl_8055FF38[8];   // the FileDisk
void* lbl_8055FF40;   // no node
u8 lbl_804DBAA0[0x20000];  // read buffer
FileNode lbl_804FBAA0[0x10];

void* FileDisk::operator new(size_t size, const char* tag) { return lbl_8055FF38; }

void FileDisk::operator delete(void* p) {}

void* FileCD::operator new(size_t size, const char* tag) {
    for (int i = 0; i < 0x10; i++) {
        if (!lbl_804FBAA0[i].used) {
            lbl_804FBAA0[i].used = 1;
            return lbl_804FBAA0[i].data;
        }
    }
    fn_802E317C("filecd: out of nodes (%s)", tag);
    return lbl_8055FF40;
}

void FileCD::operator delete(void* p) {
    for (int i = 0; i < 0x10; i++) {
        if (p == lbl_804FBAA0[i].data) {
            lbl_804FBAA0[i].used = 0;
            return;
        }
    }
    fn_802E317C("FileDisk: can't find node 0x%p", p);
}

extern "C" FileDisk* fn_802E3F94(void) {
    FileDisk* d = new ("fds") FileDisk;
    if (d->ok) {
        return d;
    }
    delete d;
    return NULL;
}

inline FileCD* FileDisk::OpenCD(const char* name) {
    FileCD* f = new ("fdo") FileCD(this, name);
    if (f->open) {
        return f;
    }
    delete f;
    return NULL;
}

inline File* FileDisk::Open(const char* name) { return OpenCD(name); }

#pragma push
#pragma global_optimizer off
// Default arguments let MWCC create the report strings when emitting their callers.
static inline void ReportDVDCloseError(const char* path, const char* format = "filecd: error closing file %s") {
    fn_802E30C8(format, path);
}

inline FileCD::~FileCD(void) {
    if (open && !DVDClose((DVDFileInfo*)&info)) {
        ReportDVDCloseError(name);
    }
}

#pragma pop

static inline void ReportUnsupportedCreate(const char* path, const char* format = "filecd: create not supported (%s)") {
    fn_802E30C8(format, path);
}

inline File* FileDisk::Create(const char* name) {
    ReportUnsupportedCreate(name);
    return NULL;
}

static inline void ReportUnsupportedOpenWrite(const char* path, const char* format = "filecd: open writable not supported (%s)") {
    fn_802E30C8(format, path);
}

inline File* FileDisk::OpenWrite(const char* name) {
    ReportUnsupportedOpenWrite(name);
    return NULL;
}

inline File* FileDisk::OpenAppend(const char* name) {
    FileCD* f = OpenCD(name);
    if (f) {
        f->Seek(f->Size());
    }
    return NULL;
}


// NonMatching: FileDisk's vtable needs the FileSystem default at 0x802E33D0 named.
