// DVD file system (0x802E3EB0-0x802E46AC, filecd): the disc as a read-only file system and its files,
// read through an aligned bounce buffer with retries while the drive reports a problem. Names are
// guesses apart from the messages.
#include "game/common.h"

struct DVDCommandBlock {
    u8 data[0x30];
};

struct DVDFileInfo {
    DVDCommandBlock block;  // offset 0x0
    u32 offset;             // offset 0x30
    u32 size;               // offset 0x34
    void* callback;         // offset 0x38
};

extern "C" {
int strncmp(const char* a, const char* b, size_t n);
char* strcpy(char* dst, const char* src);
void* memset(void* dst, int c, size_t n);
void* memcpy(void* dst, const void* src, size_t n);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void OSReport(const char* msg, ...);
void DVDInit(void);
int DVDOpen(const char* path, DVDFileInfo* info);
int DVDClose(DVDFileInfo* info);
s32 DVDReadPrio(DVDFileInfo* info, void* dst, s32 size, s32 offset, s32 prio);
int DVDReadAsyncPrio(DVDFileInfo* info, void* dst, s32 size, s32 offset, void* cb, s32 prio);
s32 DVDGetCommandBlockStatus(const DVDCommandBlock* block);
s32 DVDGetDriveStatus(void);
void VIFlush(void);
void VIWaitForRetrace(void);
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
    virtual void vf24(void);
};

extern u8 lbl_804DBAA0[0x20000];  // read buffer

class FileCD;

class FileDisk : public FileSystem {
public:
    FileDisk(void) {
        DVDInit();
        ok = 1;
        OSReport("filecd: started DVD services");
    }
    virtual ~FileDisk(void) {}
    virtual File* Create(const char* name);
    virtual File* Open(const char* name);
    virtual File* OpenWrite(const char* name);
    virtual File* OpenAppend(const char* name);
    virtual void Close(File* f) { delete f; }
    virtual void Print(const char* s) { OSReport("%s", s); }
    static void* operator new(size_t size, const char* tag);
    static void operator delete(void* p);
    FileCD* OpenCD(const char* name);

    u8 ok;  // offset 0x4
};

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
        open = DVDOpen(path, &info) != 0;
        if (open) {
            size = info.size;
        }
    }
    virtual ~FileCD(void) {
        if (open && !DVDClose(&info)) {
            fn_802E30C8("filecd: error closing file %s", name);
        }
    }
    virtual int Seek(s32 p) {
        if (p >= 0 && p <= size) {
            pos = p;
            return 1;
        }
        return 0;
    }
    virtual s32 Size(void) { return size; }
    virtual void* Disk(void) { return disk; }
    virtual s32 Read(void* dst, s32 len) {
        if (len == 0) {
            return 0;
        }
        s32 aligned = pos & ~3;
        s32 skip = pos - aligned;
        s32 n = (len + skip + 0x1F) & ~0x1F;
        if (n > 0x20000) {
            n = 0x20000;
        }
        s32 cnt = 0;
        s32 got;
        for (;;) {
            if (cnt) {
                got = DVDReadPrio(&info, lbl_804DBAA0, n, aligned, 2);
            } else {
                got = DVDReadAsyncPrio(&info, lbl_804DBAA0, n, aligned, NULL, 2);
                if (got > 0) {
                    while (DVDGetCommandBlockStatus(&info.block)) {
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
            OSReport("filecd: DVD status = %d\n", st);
            VIFlush();
            VIWaitForRetrace();
        }
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
        if (DVDReadAsyncPrio(&info, dst, len, pos, NULL, 2) <= 0) {
            fn_802E317C("filecd: DVDRead fails!");
        }
        pos += len;
        return 1;
    }
    virtual int IsDone(void) { return DVDGetCommandBlockStatus(&info.block) == 0; }
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


struct FileNode {
    u8 used;        // offset 0x0
    u8 data[0x90];  // offset 0x1
};

ModuleTag lbl_8055FF30;
u8 lbl_8055FF38[8];   // the FileDisk
void* lbl_8055FF40;   // no node
FileNode lbl_804FBAA0[0x10];
u8 lbl_804DBAA0[0x20000];  // read buffer

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

File* FileDisk::Create(const char* name) {
    fn_802E30C8("filecd: create not supported (%s)", name);
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

File* FileDisk::Open(const char* name) { return OpenCD(name); }

File* FileDisk::OpenWrite(const char* name) {
    fn_802E30C8("filecd: open writable not supported (%s)", name);
    return NULL;
}

File* FileDisk::OpenAppend(const char* name) {
    FileCD* f = OpenCD(name);
    if (f) {
        f->Seek(f->Size());
    }
    return NULL;
}


// NonMatching: FileCD::Read keeps its (always false) sync flag in a register in the target, while our
// compiler folds it away; and FileDisk's vtable needs the FileSystem default at 0x802E33D0 named.
