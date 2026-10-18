// Memory streams (0x8012F31C-0x8012F57C): a heap byte buffer plus a cursor that reads and writes words,
// blocks and in-place pointers. Names are guesses.
#include "game/common.h"

extern "C" void* memcpy(void* d, const void* s, size_t n);

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

class MemBuffer {
public:
    MemBuffer(s32 n) {
        size = n;
        data = new ("MemStream") u8[n];
    }
    ~MemBuffer(void) {
        delete[] data;
        size = 0;
    }

    u8* data;  // offset 0x0
    s32 size;  // offset 0x4
};

class MemStream {
public:
    MemStream(MemBuffer* b) {
        buf = b;
        pos = 0;
    }
    ~MemStream(void) {
        buf = 0;
        pos = 0x2BAD2BAD;
    }

    MemBuffer* buf;  // offset 0x0
    s32 pos;         // offset 0x4
};

ModuleTag lbl_8055E020;

extern "C" {

MemBuffer* fn_8012F31C(s32 size) { return new ("MemStream") MemBuffer(size); }

void fn_8012F384(MemBuffer* b) { delete b; }

MemStream* fn_8012F3CC(MemBuffer* b) { return new ("MemStreamPtr") MemStream(b); }

void fn_8012F418(MemStream* s) { delete s; }

void fn_8012F43C(MemStream* s) { s->pos = 0; }

void fn_8012F448(MemStream* s, s32 pos) { s->pos = pos; }

s32 fn_8012F450(MemStream* s) { return s->pos; }

void fn_8012F458(MemStream* s, u32 v) {
    memcpy(s->buf->data + s->pos, &v, sizeof(v));
    s->pos += sizeof(v);
}

void fn_8012F4AC(MemStream* s, const void* src, s32 n) {
    memcpy(s->buf->data + s->pos, src, n);
    s->pos += n;
}

void fn_8012F500(MemStream* s, u32* out) {
    memcpy(out, s->buf->data + s->pos, sizeof(*out));
    s->pos += sizeof(*out);
}

void fn_8012F550(MemStream* s, void** out, s32 n) {
    *out = s->buf->data + s->pos;
    s->pos += n;
}

}  // extern "C"
