// Virtual memory buffer (0x802E4C14-0x802E4D84): a heap block handed out by offset, plus a bounded string
// copy. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" void fn_802E30C8(const char* fmt, ...);  // debug print

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

class VirtualMemory {
public:
    VirtualMemory(int n) {
        size = n;
        buf = new ("vmem_buf") u8[n];
        fn_802E30C8("VirtualMemory: %d K", n / 1024);
    }
    virtual ~VirtualMemory(void);
    virtual void* Get(int off);
    virtual void* Addr(int off);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual int vf1C(void);
    u8* buf;   // offset 0x4
    s32 size;  // offset 0x8
};

ModuleTag lbl_8055FF68;

extern "C" {

void fn_802E4C14(void) {}

void fn_802E4C18(void) {}

VirtualMemory* fn_802E4C1C(int n) { return new ("vmem") VirtualMemory(n); }

VirtualMemory* fn_802E4CA4(int n) { return fn_802E4C1C(n); }

void fn_802E4CA8(char* dst, const char* src, int n) {
    while ((*dst++ = *src++) != 0) {
        if (--n <= 0) {
            dst[-1] = 0;
            return;
        }
    }
}

void fn_802E4CDC(void) {}

}  // extern "C"

VirtualMemory::~VirtualMemory(void) {
    delete[] buf;
    buf = 0;
    size = 0;
}

void* VirtualMemory::Get(int off) { return Addr(off); }

void* VirtualMemory::Addr(int off) { return buf + off; }

void VirtualMemory::vf14(void) {}

void VirtualMemory::vf18(void) {}

int VirtualMemory::vf1C(void) { return 1; }
