#ifndef GAME_BYTESWAP_H
#define GAME_BYTESWAP_H

// Byte reversal of loaded file data. The game's data files are little-endian and the console is big-endian,
// so every loader reverses the bytes of each 16- and 32-bit field in place after reading. The original code
// had these helpers written out per file in slightly different ways; each way compiles differently, so each
// is kept here under its own name (same bytes moved, other source form). Use the one a file used before.
//
// PORT: on a little-endian host the file data is already in host order and NOTHING must be reversed. This
// header is the single switch: without __MWERKS__ every helper is
// empty. A big-endian port target would define GAME_FILE_DATA_NEEDS_SWAP.

#if defined(__MWERKS__) || defined(GAME_FILE_DATA_NEEDS_SWAP)

// Four bytes, through a byte pointer local.
static inline void SwapWord(void* p) {
    unsigned char* b = (unsigned char*)p;
    unsigned char t = b[0];
    b[0] = b[3];
    b[3] = t;
    t = b[1];
    b[1] = b[2];
    b[2] = t;
}

// Four bytes, byte pointer parameter.
static inline void SwapWordB(unsigned char* p) {
    unsigned char t = p[0];
    p[0] = p[3];
    p[3] = t;
    t = p[1];
    p[1] = p[2];
    p[2] = t;
}

// Four bytes, the cast written at each access.
static inline void SwapWordV(void* p) {
    unsigned char t = ((unsigned char*)p)[0];
    ((unsigned char*)p)[0] = ((unsigned char*)p)[3];
    ((unsigned char*)p)[3] = t;
    t = ((unsigned char*)p)[1];
    ((unsigned char*)p)[1] = ((unsigned char*)p)[2];
    ((unsigned char*)p)[2] = t;
}

// Four bytes, temporary declared first.
static inline void SwapWordT(void* p) {
    unsigned char t;
    unsigned char* b = (unsigned char*)p;
    t = b[0];
    b[0] = b[3];
    b[3] = t;
    t = b[1];
    b[1] = b[2];
    b[2] = t;
}

// Four bytes, pointer taken by reference.
static inline void SwapWordRef(unsigned char* const& p) {
    unsigned char t = p[0];
    p[0] = p[3];
    p[3] = t;
    t = p[1];
    p[1] = p[2];
    p[2] = t;
}

// Two bytes, through a byte pointer local.
static inline void SwapHalf(void* p) {
    unsigned char* b = (unsigned char*)p;
    unsigned char t = b[0];
    b[0] = b[1];
    b[1] = t;
}

// Two bytes, byte pointer parameter.
static inline void SwapHalfB(unsigned char* p) {
    unsigned char t = p[0];
    p[0] = p[1];
    p[1] = t;
}

// Two bytes, the cast written at each access.
static inline void SwapHalfV(void* p) {
    unsigned char t = ((unsigned char*)p)[0];
    ((unsigned char*)p)[0] = ((unsigned char*)p)[1];
    ((unsigned char*)p)[1] = t;
}

// N bytes.
static inline void SwapBytesN(unsigned char* p, int n) {
    int i;
    for (i = 0; i < n / 2; i++) {
        unsigned char t = p[i];
        p[i] = p[n - 1 - i];
        p[n - 1 - i] = t;
    }
}

// Sixteen bytes.
static inline void SwapBytes16(unsigned char* p) {
    for (int i = 0; i < 8; i++) {
        unsigned char t = p[i];
        p[i] = p[15 - i];
        p[15 - i] = t;
    }
}

#else  // PORT: file data is already in host byte order

static inline void SwapWord(void*) {}
static inline void SwapWordB(unsigned char*) {}
static inline void SwapWordV(void*) {}
static inline void SwapWordT(void*) {}
static inline void SwapWordRef(unsigned char* const&) {}
static inline void SwapHalf(void*) {}
static inline void SwapHalfB(unsigned char*) {}
static inline void SwapHalfV(void*) {}
static inline void SwapBytesN(unsigned char*, int) {}
static inline void SwapBytes16(unsigned char*) {}

#endif

#endif
