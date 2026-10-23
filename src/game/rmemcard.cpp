// Save data on NAND ("rmemcard", 0x802E86D4-0x802E98C8): reads the save file, checks the banner, asks the
// player through the NAND dialogs (no space, too many files, corrupted file, ...) and writes the banner and
// the save asynchronously (create, open, write, close) from a state machine polled by the frame callback,
// blocking with a timeout where the caller needs a result. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
void* memcpy(void* dst, const void* src, size_t n);
void fn_802E30C8(const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);  // fatal
s32 fn_802E2C20(void);                   // milliseconds
void fn_802E2E38(int a);
void fn_802E2E0C(int (*cb)(int, int), const char* name);
const char* fn_80131234(const char* key);  // localized text
void fn_80127464(void);
void fn_80127C90(void (*cb)(void), const char* name);
void fn_80127D58(void (*cb)(void));
void fn_801282EC(int a);
void fn_801282F4(void);
void fn_802E6B50(void);
void fn_802E6B94(void);
void fn_8012C4D8(void* p);
void fn_8012E81C(const char* name);
void* fn_8012C370(const char* name, u32 type, void* a, s32* size, int b, int c);
int fn_802ECC10(void);

s32 NANDInit(void);
s32 NANDCheck(u32 blocks, u32 inodes, u32* answer);
s32 NANDOpen(const char* path, void* info, u8 mode);
s32 NANDClose(void* info);
s32 NANDGetLength(void* info, u32* len);
s32 NANDRead(void* info, void* buf, u32 len);
s32 NANDDelete(const char* path);
s32 NANDCreateAsync(const char* path, u8 perm, u8 attr, void (*cb)(s32, void*), void* block);
s32 NANDOpenAsync(const char* path, void* info, u8 mode, void (*cb)(s32, void*), void* block);
s32 NANDWriteAsync(void* info, const void* buf, u32 len, void (*cb)(s32, void*), void* block);
s32 NANDCloseAsync(void* info, void (*cb)(s32, void*), void* block);
}

void* operator new[](size_t size, const char* name);
void operator delete[](void* p);

struct DialogButton {
    const char* label;  // offset 0x0
    s32 result;         // offset 0x4
};

struct Dialog {
    const char* id;       // offset 0x0
    const char* text;     // offset 0x4
    const char* lyt;      // offset 0x8
    s32 aC;               // offset 0xC
    s32 a10;              // offset 0x10
    s32 count;            // offset 0x14
    DialogButton btn[4];  // offset 0x18
};

class DialogHost {
public:
    virtual void Open(void);           // 0x8
    virtual void Close(void);          // 0xC
    virtual int Run(Dialog* d);        // 0x10
};

// One asynchronous file write.
struct Writer {
    s32 state;         // offset 0x0
    u32 len;           // offset 0x4
    const void* buf;   // offset 0x8
    char name[0x100];  // offset 0xC
    u8 file[0x8C];     // offset 0x10C
    void* user;        // offset 0x198: command block
    u8 block[0xB4];    // offset 0x19C
    u8 done;           // offset 0x250
    s32 result;        // offset 0x254
    s32 err;           // offset 0x258
};

struct SaveTask {
    s32 state;          // offset 0x0
    Writer w;           // offset 0x4
    void* banner;       // offset 0x260
    s32 bannerSize;     // offset 0x264
    const void* data;   // offset 0x268
    s32 size;           // offset 0x26C
    char name[0x100];   // offset 0x270
    u8 pad370[8];
};

const char* lbl_8034DA08[] = {
    "Saving...",
    "Do not turn off your console or press the POWER Button!",
    0,
};

Dialog lbl_8034DB28[8] = {
    {"", "NAND:NOSAVING", "mc_savefile.lyt", 0, 0, 2, {{"NAND:YES", 1}, {"NAND:CANCEL", 0}}},
    {"", "NAND:CORRUPTED", 0, 0, 0, 2, {{"NAND:CONTINUE_NOLOAD", 0}, {"NAND:DELETE_FILE", 1}}},
    {"", "NAND:SYS_INSSPACE", 0, 0, 0, 2, {{"NAND:CONTINUE_NOSAVE", 0}, {"NAND:RETURN_TO_MENU", 1}}},
    {"", "NAND:SYS_INSINODE", 0, 0, 0, 2, {{"NAND:CONTINUE_NOSAVE", 0}, {"NAND:RETURN_TO_MENU", 1}}},
    {"", "NAND:WEAR_CORRUPT", 0, 0, 0, 2, {{"NAND:CONTINUE_NOSAVE", 0}, {"NAND:RETURN_TO_MENU", 1}}},
    {"", "NAND:MAXFILES", 0, 0, 0, 1, {{"NAND:CANCEL", 0}}},
    {"", "NAND:MAXBLOCKS", 0, 0, 0, 1, {{"NAND:CANCEL", 0}}},
    {"", "NAND:SAVEFILE_OK", "mc_savefile.lyt", 0, 0, 2, {{"NAND:YES", 1}, {"NAND:NO", 0}}},
};

ModuleTag lbl_80560018;
u8* lbl_80560020;       // async buffer
DialogHost* lbl_80560024;
u8 lbl_80560028;
s32 lbl_8056002C;       // dialog nesting
const char* lbl_80560030;  // file name
u8 lbl_80560034;        // saving disabled
u8 lbl_80560035;
SaveTask lbl_80507288;

// Keeps the file name for the delete dialog while a read is in progress.
struct FileNameGuard {
    FileNameGuard(const char* name) { lbl_80560030 = name; }
    ~FileNameGuard(void) { lbl_80560030 = 0; }
};

static inline void StrCopyN(char* d, const char* s, int n) {
    char* p = d;
    int c = n;
    while ((*p++ = *s++) != '\0') {
        if (--c <= 0) {
            p[-1] = '\0';
            break;
        }
    }
}

extern "C" {

void fn_802E9010(void);
int fn_802E95E0(const char* what, void (*poll)(void));

void fn_802E86D4(void) {
    lbl_80560024->Open();
    lbl_80560028 = 1;
}

void fn_802E870C(void) {
    lbl_80560024->Close();
    lbl_80560028 = 0;
}

int fn_802E8744(const Dialog* d) {
    Dialog c = *d;
    c.text = fn_80131234(c.text);
    Dialog* p = &c;
    for (int i = 0; i < 4; i++) {
        if (d->btn[0].label) {
            p->btn[0].label = fn_80131234(d->btn[0].label);
        }
        d = (const Dialog*)((const u8*)d + 8);
        p = (Dialog*)((u8*)p + 8);
    }
    return lbl_80560024->Run(&c);
}

void fn_802E8838(void) { fn_80127464(); }

void fn_802E883C(void) {
    if (lbl_8056002C == 0) {
        fn_802E86D4();
    }
    lbl_8056002C++;
}

void fn_802E8874(void) {
    if (--lbl_8056002C == 0) {
        fn_802E870C();
    }
}

int fn_802E888C(int which) {
    fn_802E883C();
    int r = fn_802E8744(&lbl_8034DB28[which]);
    fn_802E8874();
    return r;
}

int fn_802E88D8(void) {
    if (fn_802E888C(0)) {
        lbl_80560034 = 1;
        return 1;
    }
    return 0;
}

void fn_802E8918(void) { fn_802E2E38(1); }

void fn_802E8920(s32 size) {
    u32 answer = 0;
    s32 r = NANDCheck((size + 0x3FFF) / 0x4000 + 4, 2, &answer);
    if (r == 0) {
        for (;;) {
            if (answer & 1) {
                fn_802E30C8("rmemcard: exceeded maximum file size in home dir");
            }
            if (answer & 2) {
                fn_802E30C8("rmemcard: exceeded maximum files/directories in home dir");
            }
            if (answer & 0xF) {
                int which = 3;
                if (answer & 5) {
                    which = 2;
                }
                if (fn_802E888C(which)) {
                    fn_802E8918();
                    return;
                }
                if (fn_802E88D8()) {
                    return;
                }
            } else {
                if (fn_802E888C(7)) {
                    return;
                }
                if (fn_802E88D8()) {
                    return;
                }
            }
        }
    } else {
        fn_802E30C8("rmemcard: NANDCheck returns %d", r);
    }
}

int fn_802E8A18(void) {
    u8 file[0x8C];
    int ok = 0;
    s32 r = NANDOpen("banner.bin", file, 1);
    if (r == 0) {
        u32 len = 0;
        if (NANDGetLength(file, &len) == 0) {
            if (len == 0xF0A0) {
                fn_802E30C8("rmemcard: test_banner_ok: banner is ok");
                ok = 1;
            } else {
                fn_802E30C8("rmemcard: test_banner_ok: banner wrong length (%d != %d)", len, 0xF0A0);
            }
        } else {
            fn_802E30C8("rmemcard: test_banner_ok: banner length cannot be read.");
        }
        NANDClose(file);
    } else {
        fn_802E30C8("rmemcard: test_banner_ok: can't open banner (%d)", r);
    }
    return ok;
}

int fn_802E8AE8(const char* name) {
    u8 file[0x8C];
    int ok = 0;
    if (NANDOpen(name, file, 1) == 0) {
        ok = 1;
        NANDClose(file);
    }
    return ok;
}

void fn_802E8B34(const char* name) {
    fn_802E30C8("rmemcard: deleting %s", name);
    s32 r = NANDDelete(name);
    if (r != 0) {
        fn_802E30C8("rmemcard: NANDDelete returned %d! (%s)", r, name);
    }
}

void fn_802E8B98(void) {
    if (lbl_80560030) {
        fn_802E8B34(lbl_80560030);
        fn_802E8B34("banner.bin");
    } else {
        fn_802E30C8("rmemcard: g_filename not set for delete file!");
    }
}

int fn_802E8BE4(void) {
    for (;;) {
        if (fn_802E888C(1)) {
            fn_802E8B98();
            return 1;
        }
        if (fn_802E88D8()) {
            return 0;
        }
    }
}

void fn_802E8C2C(void) {
    for (;;) {
        if (fn_802E888C(4)) {
            fn_802E8918();
            return;
        }
        if (fn_802E88D8()) {
            return;
        }
    }
}

int fn_802E8C6C(int phase, int mode) {
    if (phase == 1) {
        fn_802E95E0("reset hander", fn_802E9010);
    }
    return 0;
}

void fn_802E8CA8(void) {
    s32 r = NANDInit();
    if (r != 0) {
        fn_802E317C("rmemcard: NANDInit returns %d", r);
    }
    fn_801282F4();
    fn_801282EC(0x80);
    lbl_80560020 = new ("mc_async") u8[0x55730];
    fn_802E6B50();
    fn_80127C90(fn_802E9010, "memcard");
    fn_802E2E0C(fn_802E8C6C, "NAND");
}

void fn_802E8D2C(void) {
    fn_802E95E0("MemCardEnd", fn_802E8838);
    fn_80127D58(fn_802E9010);
    fn_802E6B94();
    delete[] lbl_80560020;
    lbl_80560020 = 0;
}

void fn_802E8D7C(DialogHost* h) { lbl_80560024 = h; }

int fn_802E8D84(void) { return 1; }

}  // extern "C"

bool lbl_8055D255 = true;  // first read

extern "C" {

int fn_802E8D8C(const char* name, void* dst, s32 size) {
    u8 file[0x8C];
    FileNameGuard guard(name);
    if (!fn_802E95E0("MemCardReadFile", fn_802E8838)) {
        return 0;
    }
    const bool first = lbl_8055D255;
    lbl_8055D255 = false;
    int banner = fn_802E8A18();
    int have = fn_802E8AE8(name);
    if (!banner && !have) {
        fn_802E30C8("rmemcard: new user, missing both banner and memcard");
        if (first) {
            fn_802E8920(size);
        }
        return 0;
    }
    if (!banner || !have) {
        fn_802E30C8("rmemcard: new user, missing either banner or memcard");
        if (fn_802E8BE4()) {
            fn_802E8920(size);
        }
        return 0;
    }
    s32 r = NANDOpen(name, file, 1);
    if (r != 0) {
        fn_802E30C8("rmemcard: NANDOpen (R) returns %d", r);
        if (first) {
            fn_802E8920(size);
        }
        return 0;
    }
    s32 got;
    if (lbl_80560020) {
        if (size > 0x55730) {
            fn_802E317C("memcard: too big for ASYNC_MEMCARD_BYTES (%d > %d)", size, 0x55730);
        }
        got = NANDRead(file, lbl_80560020, size);
        if (got == size) {
            memcpy(dst, lbl_80560020, size);
        }
    } else {
        got = NANDRead(file, dst, size);
    }
    if (got != size) {
        fn_802E30C8("rmemcard: NANDRead returns %d", got);
        NANDClose(file);
        if (got == -4) {
            fn_802E8C2C();
        } else if (fn_802E8BE4()) {
            fn_802E8920(size);
        }
        return 0;
    }
    r = NANDClose(file);
    if (r != 0) {
        fn_802E30C8("rmemcard: NANDClose returns %d", r);
        return 0;
    }
    return 1;
}

void fn_802E9254(Writer* w);
void fn_802E91D8(Writer* w, const char* name, const void* buf, u32 len);
int fn_802E91BC(Writer* w);
int fn_802E91AC(Writer* w);
void fn_802E9118(SaveTask* t);
void fn_802E9160(SaveTask* t);

inline void fn_802E9010(void) {
    fn_802E9254(&lbl_80507288.w);
    SaveTask* t = &lbl_80507288;
    switch (lbl_80507288.state) {
    case 2:
        fn_802E91D8(&t->w, "banner.bin", t->banner, t->bannerSize);
        lbl_80507288.state = 3;
        break;
    case 3:
        if (fn_802E91BC(&t->w)) {
            if (!fn_802E91AC(&t->w)) {
                fn_802E91D8(&t->w, t->name, t->data, t->size);
                lbl_80507288.state = 4;
            } else {
                fn_802E9160(t);
            }
        }
        break;
    case 4:
        if (fn_802E91BC(&t->w)) {
            if (!fn_802E91AC(&t->w)) {
                fn_802E9118(t);
            } else {
                fn_802E9160(t);
            }
        }
        break;
    }
}

inline void fn_802E9118(SaveTask* t) {
    fn_8012C4D8(t->banner);
    t->banner = 0;
    t->bannerSize = 0;
    t->data = 0;
    t->size = 0;
    t->state = 0;
}

inline void fn_802E9160(SaveTask* t) {
    fn_8012C4D8(t->banner);
    t->banner = 0;
    t->bannerSize = 0;
    t->data = 0;
    t->size = 0;
    t->state = 1;
}

inline int fn_802E91AC(Writer* w) { return w->state == 0; }

inline int fn_802E91BC(Writer* w) { return w->state < 2; }

inline void fn_802E91D8(Writer* w, const char* name, const void* buf, u32 len) {
    w->buf = buf;
    w->len = len;
    StrCopyN(w->name, name, 0x100);
    w->user = w;
    w->err = 0;
    w->done = 0;
    w->result = 0;
    w->state = 2;
    fn_802E30C8("rmemcard: Starting to write %db to %s", len, name);
}

inline void fn_802E95CC(s32 result, void* block);

// Starts one NAND call; a refused call ends the write.
#define NAND_START(call, name, next, closeOnFail)      {                                                         w->done = 0;                                          w->result = 0;                                        s32 r = call;                                         if (r != 0) {                                             fn_802E30C8("rmemcard: %s failed (%d)", name, r);             w->state = 0;                                         w->err = r;                                           if (closeOnFail) {                                        NANDClose(w->file);                               }                                                 } else {                                                  w->state = next;                                  }                                                 }

inline void fn_802E9254(Writer* w) {
    s32 res;
    switch (w->state) {
    case 2:
        NAND_START(NANDCreateAsync(w->name, 0x30, 0, fn_802E95CC, &w->user), "NANDCreateAsync", 3, 0);
        break;
    case 3:
        if (!w->done) {
            break;
        }
        res = w->result;
        if (res == 0 || res == -6) {
            NAND_START(NANDOpenAsync(w->name, w->file, 2, fn_802E95CC, &w->user), "NANDOpenAsync", 4, 0);
        } else {
            if (res == -11) {
                lbl_80560034 = 1;
                fn_802E888C(5);
            }
            res = w->result;
            fn_802E30C8("rmemcard: %s failed (%d)", "NANDCreateAsync", res);
            w->err = res;
            w->state = 0;
        }
        break;
    case 4:
        if (!w->done) {
            break;
        }
        res = w->result;
        if (res == 0) {
            NAND_START(NANDWriteAsync(w->file, w->buf, w->len, fn_802E95CC, &w->user), "NANDWriteAsync", 5, 1);
        } else {
            fn_802E30C8("rmemcard: %s failed (%d)", "NANDOpenAsync", res);
            w->err = res;
            w->state = 0;
        }
        break;
    case 5:
        if (!w->done) {
            break;
        }
        res = w->result;
        if (res == (s32)w->len) {
            NAND_START(NANDCloseAsync(w->file, fn_802E95CC, &w->user), "NANDCloseAsync", 6, 1);
        } else {
            fn_802E30C8("rmemcard: %s failed (%d)", "NANDWriteAsync", res);
            w->err = res;
            w->state = 0;
            NANDClose(w->file);
        }
        break;
    case 6:
        if (!w->done) {
            break;
        }
        res = w->result;
        if (res == 0) {
            w->state = 1;
            fn_802E30C8("rmemcard: Wrote %db to %s", w->len, w->name);
        } else {
            fn_802E30C8("rmemcard: %s failed (%d)", "NANDCloseAsync", res);
            w->err = res;
            w->state = 0;
            NANDClose(w->file);
        }
        break;
    }
}

inline void fn_802E95CC(s32 result, void* block) {
    Writer* w = *(Writer**)block;
    w->result = result;
    w->done = 1;
}

int fn_802E95E0(const char* what, void (*poll)(void)) {
    s32 start = fn_802E2C20();
    while (lbl_80507288.state >= 2) {
        s32 now = fn_802E2C20();
        if (now < start || now > (start + 4000)) {
            fn_802E30C8("rmemcard: (%s) timeout while blocking (%d ms)", what, now - start);
            return 0;
        }
        poll();
    }
    return 1;
}

inline void* LoadBanner(s32* size) {
    u8 tmp[8];
    fn_8012E81C("mcbanner.res");
    return fn_8012C370("out\\banner.bin", 'RVBN', tmp, size, 0, 0);
}

int fn_802E967C(const char* name, const void* data, s32 size) {
    if (!fn_802E95E0("MemCardWriteFile", fn_802E8838)) {
        fn_802E30C8("rmemcard: block_for_ready fails on writefile!");
        return 0;
    }
    if (lbl_80560034) {
        fn_802E30C8("rmemcard: saving has been disabled");
        return 0;
    }
    memcpy(lbl_80560020, data, size);
    u8* buf = lbl_80560020;
    lbl_80507288.state = 2;
    SaveTask* t = &lbl_80507288;
    t->banner = LoadBanner(&t->bannerSize);
    t->data = buf;
    t->size = size;
    StrCopyN(t->name, name, 0x100);
    return 1;
}

void fn_802E97B4(void) {
    if (lbl_80560035 || fn_802ECC10()) {
        return;
    }
    s32 state = lbl_80507288.state;
    lbl_80560035 = 1;
    if (state == 1) {
        switch (lbl_80507288.w.err) {
        case 0:
            break;
        case -9:
            fn_802E888C(6);
            break;
        case -11:
            fn_802E888C(5);
            break;
        default:
            fn_802E30C8("rmemcard: unhandled async memcard error %d", lbl_80507288.w.err);
            break;
        }
    }
    lbl_80560035 = 0;
    lbl_80507288.w.err = 0;
}

void fn_802E986C(void) {}

void fn_802E9870(void) {}

void fn_802E9874(void) {}

void fn_802E9878(void) {}

void fn_802E987C(void) {}

void fn_802E9880(void) {}

void fn_802E9884(void) {}

void fn_802E9888(void) {}

}  // extern "C"

struct SaveTaskInit {
    SaveTaskInit(void) {
        lbl_80507288.state = 0;
        lbl_80507288.w.state = 1;
    }
};

static SaveTaskInit sSaveInit;
