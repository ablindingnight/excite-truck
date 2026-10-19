// Files (0x802E321C-0x802E3DF8, file.cpp): the file id table over the file systems (the disc, and
// "nand:"/"sd:" paths on their own file systems), with reads, writes, printf and directory search.
// Names are guesses apart from the file name and messages.
#include <stdarg.h>
#include "game/common.h"

extern "C" {
size_t strlen(const char* s);
int strncmp(const char* a, const char* b, size_t n);
char* strncpy(char* dst, const char* src, size_t n);
void* memset(void* dst, int c, size_t n);
int vsprintf(char* buf, const char* fmt, va_list args);
void fn_802E317C(const char* fmt, ...);  // warning print
}

class File;

class FileSystem;

class FileFinder {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual s32 FindFirst(void* a, void* b, void* c);
    virtual s32 FindNext(void* a, void* b, void* c);
    virtual void FindClose(void* a);
};

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
    virtual FileSystem* Disk(void) = 0;
    virtual ~File(void);
};

class FileSystem {
public:
    virtual ~FileSystem(void);
    virtual File* Create(const char* name) = 0;
    virtual File* Open(const char* name) = 0;
    virtual File* OpenWrite(const char* name) = 0;
    virtual File* OpenAppend(const char* name) = 0;
    virtual void Close(File* f) = 0;
    virtual void Print(const char* s) = 0;
    virtual FileFinder* Finder(void);
};

extern "C" {
FileSystem* fn_802E3F94(void);  // disc file system
FileSystem* fn_802E98C8(void);  // SD file system
}

struct FileEntry {
    char name[0x20];  // offset 0x0
    u8 used;          // offset 0x20
    File* f;          // offset 0x24
};

ModuleTag lbl_8055FF10;
FileSystem* lbl_8055FF18;  // disc
FileSystem* lbl_8055FF1C;  // writable
FileSystem* lbl_8055FF20;  // nand
FileSystem* lbl_8055FF24;  // sd
struct FileTable {
    FileEntry e[0x10];  // offset 0x0
    u8 pad[0x10];       // offset 0x280
};

FileTable lbl_804DB810;

extern "C" {

int fn_802E321C(const char* path) {
    const char* prefix = "nand:";
    return strncmp(path, prefix, strlen(prefix)) == 0;
}

int fn_802E3270(const char* path) {
    const char* prefix = "sd:";
    return strncmp(path, prefix, strlen(prefix)) == 0;
}

FileSystem* fn_802E32C4(const char* path) {
    if (path && fn_802E321C(path) && lbl_8055FF20) {
        return lbl_8055FF20;
    }
    if (path && fn_802E3270(path) && lbl_8055FF24) {
        return lbl_8055FF24;
    }
    return NULL;
}

FileSystem* fn_802E333C(const char* path) {
    FileSystem* fs = fn_802E32C4(path);
    if (fs) {
        return fs;
    }
    return lbl_8055FF18;
}

FileSystem* fn_802E336C(const char* path) {
    FileSystem* fs = fn_802E32C4(path);
    if (fs) {
        return fs;
    }
    return lbl_8055FF1C;
}

FileFinder* fn_802E339C(void) { return fn_802E333C(NULL)->Finder(); }

}  // extern "C"

FileFinder* FileSystem::Finder(void) { return NULL; }

extern "C" {

FileEntry* fn_802E33D8(int id, const char* file, int line) {
    if (id < 0 || id == -1 || id > 0x10) {
        fn_802E317C("file: bad file_id %d (%s, %d)", id, file, line);
    }
    if (!lbl_804DB810.e[id].used) {
        fn_802E317C("file: %d|%s not in use (%s, %d)", id, lbl_804DB810.e[id].name, file, line);
    }
    return &lbl_804DB810.e[id];
}

void fn_802E3550(void);

void fn_802E3498(void) {
    memset(lbl_804DB810.e, 0, sizeof(lbl_804DB810.e));
    if (!lbl_8055FF18) {
        lbl_8055FF18 = fn_802E3F94();
    }
}

void fn_802E34DC(void) {
    fn_802E3550();
    delete lbl_8055FF18;
    lbl_8055FF18 = NULL;
    if (lbl_8055FF1C) {
        delete lbl_8055FF1C;
        lbl_8055FF1C = NULL;
    }
}

void fn_802E3550(void) {
    for (int i = 0; i < 0x10; i++) {
        if (lbl_804DB810.e[i].used) {
            fn_802E317C("file: %s still in use", lbl_804DB810.e[i].name);
        }
    }
}

int fn_802E35C0(const char* name, File* f) {
    for (int i = 0; i < 0x10; i++) {
        if (!lbl_804DB810.e[i].used) {
            lbl_804DB810.e[i].used = 1;
            strncpy(lbl_804DB810.e[i].name, name, 0x20);
            lbl_804DB810.e[i].f = f;
            return i;
        }
    }
    fn_802E317C("file: too many files (%s)", name);
    return -1;
}

int fn_802E367C(const char* name) {
    File* f = fn_802E333C(name)->Create(name);
    if (f) {
        return fn_802E35C0(name, f);
    }
    return -1;
}

int fn_802E36D8(const char* name) {
    File* f = fn_802E333C(name)->Open(name);
    if (f) {
        return fn_802E35C0(name, f);
    }
    return -1;
}

void fn_802E3734(int* id) {
    FileEntry* e = fn_802E33D8(*id, "file.cpp", 228);
    File* f = e->f;
    f->Disk()->Close(f);
    e->f = NULL;
    e->used = 0;
    *id = -1;
}

s32 fn_802E37C4(int id) { return fn_802E33D8(id, "file.cpp", 293)->f->Size(); }

void fn_802E3898(int id, void* buf, s32* len);

void fn_802E3804(int id, void* buf, s32 len) {
    FileEntry* e = fn_802E33D8(id, "file.cpp", 304);
    s32 n = len;
    fn_802E3898(id, buf, &n);
    if (n != len) {
        fn_802E317C("file: can't read %d bytes from %s", len, e->name);
    }
}

void fn_802E3898(int id, void* buf, s32* len) {
    FileEntry* e = fn_802E33D8(id, "file.cpp", 322);
    s32 want = *len;
    s32 got = 0;
    while (want > 0) {
        s32 r = e->f->Read(buf, want - got);
        if (r < 0) {
            got = r;
            break;
        }
        if (r <= 0) {
            break;
        }
        got += r;
        buf = (u8*)buf + r;
    }
    *len = got;
}

int fn_802E3930(int id, void* buf, s32 len) { return fn_802E33D8(id, "file.cpp", 350)->f->ReadAsync(buf, len); }

int fn_802E3990(int id) { return fn_802E33D8(id, "file.cpp", 364)->f->IsDone(); }

int fn_802E39D0(int id, void* buf, s32 len) {
    s32 n = len;
    fn_802E3898(id, buf, &n);
    return n == len;
}

int fn_802E3A14(int id, s32 pos) { return fn_802E33D8(id, "file.cpp", 427)->f->Seek(pos); }

int fn_802E3A64(int id, const void* buf, s32 len) {
    return fn_802E33D8(id, "file.cpp", 451)->f->Write(buf, len) == len;
}

void fn_802E3AD0(int id, const char* fmt, ...) {
    char buf[0x400];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    if (id != -666) {
        fn_802E3A64(id, buf, strlen(buf));
    } else if (fn_802E333C(NULL)) {
        fn_802E333C(NULL)->Print(buf);
    }
}

s32 fn_802E3BB4(void* a, void* b, void* c) {
    FileFinder* f = fn_802E339C();
    if (f) {
        return f->FindFirst(a, b, c);
    }
    return -1;
}

s32 fn_802E3C24(void* a, void* b, void* c) {
    FileFinder* f = fn_802E339C();
    if (f) {
        return f->FindNext(a, b, c);
    }
    fn_802E317C("FileFindNext: not supported");
    return 0;
}

void fn_802E3CA4(void* a) {
    FileFinder* f = fn_802E339C();
    if (f) {
        f->FindClose(a);
    } else {
        fn_802E317C("FileFindClose: not supported");
    }
}

void fn_802E3D00(void) {}

int fn_802E3D04(const char* name) {
    FileSystem* fs = fn_802E336C(name);
    if (fs) {
        File* f = fs->Open(name);
        if (f) {
            return fn_802E35C0(name, f);
        }
    }
    return -1;
}

int fn_802E3D68(const char* name) {
    FileSystem* fs = fn_802E336C(name);
    if (fs) {
        File* f = fs->Create(name);
        if (f) {
            return fn_802E35C0(name, f);
        }
    }
    return -1;
}

void fn_802E3DCC(void) { lbl_8055FF24 = fn_802E98C8(); }

}  // extern "C"
