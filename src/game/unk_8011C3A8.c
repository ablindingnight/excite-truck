// Bink, second unit: code 0x8011C3A8-0x80127398 (bundle and plane decoder, allocators, FFT, IDCT, residue readers).
// Split from game/unk_80116BD4.c on 2026-11-16: retail addresses ExpandPlane's mask tables from lbl_802F65C0, and that
// base is always the first .rodata byte of the object, so the tables start a new object (see
// handoff/BINK_SPLIT_PLAN_2026-11-16.md). Unit name is provisional.
// CLEANUP: automated-decompiler source with raw offsets and placeholder types; rewrite with real types
// before the PC port. Tricks are tagged MATCHING-AID.
#include "game/bink_internal.h"

/* .data owned by this unit (0x80329DA8-0x80329E2C): the Huffman table pointers; the switch jump tables of
   ExpandPlane follow them. */
// DUMP_DATA lbl_80329DA8 tableptr
const u8* lbl_80329DA8[16] = {
#include "dumpdata/lbl_80329DA8.inc"
};

void OpenReadBundle(s32 arg0, void *arg1, s32 arg2, u32 arg3, s32 arg4, s32 arg5, s32 arg6) {
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

s32 simpmergesort(u32 arg0, u8 *arg1, u8 *arg2, u8 *arg3, s32 arg4) {
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
            *out++ = *right++;
            var_r8--;
        } else {
            *out++ = *left++;
            var_r7--;
        }
        arg0 >>= 1;
    } while (var_r7 != 0 && var_r8 != 0);
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
        v = simpmergesort(v, dst, a, b, size);          \
        BINK_SKIP(v);                                 \
    }

extern const u8* lbl_80329DA8[16];  // Bink Huffman lookup tables
extern const u8 lbl_802F6990[];

// Reads a Huffman tree: picks one of the 16 fixed code tables and reads the symbol order, either explicitly or as
// a series of merge passes over the identity order.
void ReadHuffTable(BitReader8011 *bits, u8 **lookup, u32 *maxBits, u8 *syms) {
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
    *lookup = (u8*)lbl_80329DA8[table];
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
void StartReadHuff4Bundle(Bundle8011 *b, BitReader8011 *bits) {
    ReadHuffTable(bits, &b->lookup, &b->maxBits, b->symbols);
}

// The 16 context trees used for colors (selected by the previous high nibble).
typedef struct BinkColorTrees8011 {
    u8 symbols[16][16];  // 0x000
    u32 maxBits[16];     // 0x100
    u8 *lookup[16];      // 0x140
    s32 last;            // 0x180 previous high nibble
} BinkColorTrees8011;

// Reads the 16 color context trees, then the bundle's own tree.
void StartReadHuff8Bundle(Bundle8011 *b, BitReader8011 *bits, BinkColorTrees8011 *trees) {
    u32 i;
    for (i = 0; i < 16; i++) {
        ReadHuffTable(bits, &trees->lookup[i], &trees->maxBits[i], trees->symbols[i]);
    }
    ReadHuffTable(bits, &b->lookup, &b->maxBits, b->symbols);
    trees->last = 0;
}

const u8 lbl_80561208[8] = { 4, 8, 12, 32 };  // run lengths of the long-run symbols 12..15 (.sdata2)
typedef struct BitWin8011 {
    u32 width;
    u32 mask;
} BitWin8011;

void CheckReadRLEHuff4Bundle(void *arg0, void *arg1) {
    u32 *bits;
    u32 fill;
    u32 value, bitCount, count, code, len, s, w;
    u8 *dst, *lookup, *table;
    u32 sym, runlen, j;
    BitWin8011 win;
    if (*(u32 *)arg0 != *(u32 *)((u8 *)arg0 + 4)) return;
    bitCount = *(u32 *)((u8 *)arg1 + 8);
    win.width = *(u32 *)((u8 *)arg0 + 0x28);
    bits = *(u32 **)((u8 *)arg1 + 4);
    value = *(u32 *)arg1;
    if (bitCount < win.width) {
        u32 next = *bits++;
        count = (value | (next << bitCount)) & (0xFFFFFFFFU >> (32 - win.width));
        value = next >> (win.width - bitCount);
        bitCount = bitCount + 32 - win.width;
    } else {
        count = value & (0xFFFFFFFFU >> (32 - win.width));
        value >>= win.width;
        bitCount -= win.width;
    }
    if (count != 0) {
        dst = *(u8 **)((u8 *)arg0 + 0x2C);
        *(u8 **)arg0 = dst;
        *(u8 **)((u8 *)arg0 + 4) = dst + count;
        if (bitCount == 0) { w = *bits++; bitCount = 31; value = w >> 1; }
        else { w = value; value >>= 1; bitCount--; }
        if ((w & 1) == 0) {
            dst = *(u8 **)((u8 *)arg0 + 0x2C);
            fill = 0;
            table = (u8 *)arg0 + 0x10;
            lookup = *(u8 **)((u8 *)arg0 + 0x24);
            win.width = *(u32 *)((u8 *)arg0 + 0x20);
            win.mask = 0xFFFFFFFFU >> (32 - win.width);
            while (count != 0) {
                u8 rl;
                u32 next;
                u32 w;
                if (bitCount >= win.width) {
                    code = value & win.mask;
                    s = lookup[code];
                    code = s & 15;
                    len = s >> 4;
                    sym = table[code];
                    value >>= len;
                    bitCount -= len;
                } else {
                    next = *bits;
                    code = (value | (next << bitCount)) & win.mask;
                    len = lookup[code];
                    sym = table[len & 15];
                    len >>= 4;
                    if (bitCount >= len) { value >>= len; bitCount -= len; }
                    else { value = next >> (len - bitCount); bits++; bitCount = bitCount + 32 - len; }
                }
                if (sym >= 12) {
                    rl = lbl_80561208[sym - 12];
                    w = fill | (fill << 8);
                    count -= rl;
                    w |= w << 16;
                    /* Run fill as the target has it: a byte loop over the whole run when dst is unaligned (the word and
                     * tail loops after it never run), whole words when aligned (runs are multiples of 4 bytes). */
                    if ((u32)dst & 3) {
                        do {
                            *dst++ = w;
                        } while (--rl);
                        while (rl >= 4) {
                            *(u32 *)dst = w;
                            dst += 4;
                            rl -= 4;
                        }
                        for (; rl != 0; rl--) {
                            *dst++ = w;
                        }
                    } else {
                        do {
                            rl = rl - 4;
                            *(u32 *)dst = w;
                            dst += 4;
                        } while (rl);
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
void CheckReadHuff8Bundle(Bundle8011 *b, BitReader8011 *bits, BinkColorTrees8011 *trees) {
    u32 *next;
    u32 value;
    u32 count;
    s32 len;
    u32 width;
    u32 e;
    u32 bit;
    u32 last;
    u32 lo;
    u32 word;
    u32 maxBits;
    u8 *dst;
    u8 *lookup;
    u8 *symbols;
    u32 ones;

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
        maxBits = b->maxBits;
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
        ones = 0xFFFFFFFFU;
        do {
            u32 w;
            u32 c;
            u32 v;
            len--;
            w = trees->maxBits[last];
            if (count >= w) {
                e = trees->lookup[last][value & (ones >> (32 - w))];
                last = trees->symbols[last][e & 15];
                e >>= 4;
                value >>= e;
                count -= e;
            } else {
                u32 word = *next;
                e = trees->lookup[last][(value | (word << count)) & (ones >> (32 - w))];
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
            if (count >= maxBits) {
                e = lookup[value & (ones >> (32 - maxBits))];
                lo = symbols[e & 15];
                e >>= 4;
                value >>= e;
                count -= e;
            } else {
                word = *next;
                e = lookup[(ones >> (32 - maxBits)) & (value | (word << count))];
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

// Refills the colors bundle (older format): like CheckReadHuff8Bundle, with colors stored as plain bytes.
void NewCheckReadHuff8Bundle(Bundle8011 *b, BitReader8011 *bits, BinkColorTrees8011 *trees) {
    u32 *next;
    u32 value;
    u32 count;
    s32 len;
    u32 width;
    u32 e;
    u32 bit;
    u32 last;
    u32 lo;
    u32 word;
    u32 maxBits;
    u8 *dst;
    u8 *lookup;
    u8 *symbols;
    u32 ones;

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
        maxBits = b->maxBits;
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
        ones = 0xFFFFFFFFU;
        do {
            u32 w;
            len--;
            w = trees->maxBits[last];
            if (count >= w) {
                e = trees->lookup[last][value & (ones >> (32 - w))];
                last = trees->symbols[last][e & 15];
                e >>= 4;
                value >>= e;
                count -= e;
            } else {
                u32 word = *next;
                e = trees->lookup[last][(value | (word << count)) & (ones >> (32 - w))];
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
            if (count >= maxBits) {
                e = lookup[value & (ones >> (32 - maxBits))];
                lo = symbols[e & 15];
                e >>= 4;
                value >>= e;
                count -= e;
            } else {
                word = *next;
                e = lookup[(ones >> (32 - maxBits)) & (value | (word << count))];
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
/* Helper name unproven */

void CheckReadHuff4Bundle(Bundle8011 *b, BitReader8011 *bits) {
    u32 *next;
    u32 value;
    u32 count;
    u32 len, bit, fill, e;
    u8 *dst, *lookup, *symbols;
    BitWin8011 w;

    if (b->cur != b->end) return;
    count = bits->count;
    w.width = b->countBits;
    next = bits->next;
    value = bits->value;
    if (count < w.width) {
        u32 word = *next;
        len = (value | (word << count)) & (0xFFFFFFFFU >> (32 - w.width));
        value = word >> (w.width - count);
        count = count + 32 - w.width;
        next++;
    } else {
        len = value & (0xFFFFFFFFU >> (32 - w.width));
        value >>= w.width;
        count -= w.width;
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
            w.width = b->maxBits;
            w.mask = 0xFFFFFFFFU;
            w.mask >>= 32 - w.width;
            dst = b->data;
            lookup = b->lookup;
            symbols = b->symbols;
            do {
                len--;
                if (count >= w.width) {
                    e = lookup[value & w.mask];
                    *dst++ = symbols[e & 15];
                    e >>= 4;
                    value >>= e;
                    count -= e;
                } else {
                    u32 word = *next;
                    e = lookup[(value | (word << count)) & w.mask];
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
void CheckReadHuff4PairBundle(Bundle8011 *b, BitReader8011 *bits) {
    u32 len;
    u32 *next;
    u32 value;
    u32 count;
    u32 mask, lo, hi, e;
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
        mask = b->maxBits;
        dst = b->data;
        b->cur = dst;
        b->end = dst + len;
        width = mask;
        mask = 0xFFFFFFFFU >> (32 - mask);
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

void CheckReadHuff4SBundle(Bundle8011 *b, BitReader8011 *bits) {
    u32 *next;
    u32 value;
    u32 count;
    u32 len;
    u32 width;
    u32 e;
    u32 bit;
    u32 maxBits;
    u8 *dst;
    u8 *lookup;
    u8 *symbols;
    u32 sym;
    u32 word;
    u32 ones;

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
            maxBits = b->maxBits;
            ones = 0xFFFFFFFFU;
            dst = b->data;
            symbols = b->symbols;
            lookup = b->lookup;
            do {
                len--;
                if (count >= maxBits) {
                    e = lookup[value & (ones >> (32 - maxBits))];
                    sym = symbols[e & 15];
                    e >>= 4;
                    value >>= e;
                    count -= e;
                } else {
                    word = *next;
                    e = lookup[(ones >> (32 - maxBits)) & (value | (word << count))];
                    sym = symbols[e & 15];
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
                if (sym != 0) {
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
                        sym = -sym;
                    }
                }
                *dst++ = sym;
            } while (len != 0);
        } else {
            if (count < 4) {
                u32 word = *next++;
                sym = (value | (word << count)) & 15;
                value = word >> (4 - count);
                count += 28;
            } else {
                sym = value & 15;
                value >>= 4;
                count -= 4;
            }
            if (sym != 0) {
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
                    sym = -sym;
                }
            }
            memset(b->data, sym, len);
        }
    } else {
        dst = b->data;
        b->end = dst;
        b->cur = dst + 4;
    }
    bits->next = next;
    bits->value = value;
    bits->count = count;
}

void CheckReadDelta16Bundle(void *arg0, void *arg1) {
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
                radmemset16((u32 *)dst, (u16)val, (n + n));
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

s32 getbunsize(s32 arg0, u32 arg1, u32 arg2, u32 arg3) {
    u32 var_r5 = arg2;
    u32 temp;
    if (var_r5 <= 8U) var_r5 = 8;
    else if (var_r5 < 0x10U) var_r5 = 0x10;
    temp = var_r5 * arg3;
    return (((u32)(arg0 * var_r5) >> 3) + ((temp * (arg1 >> 3)) >> 3) + 3) & 0xFFFFFFFC;
}

void ExpandBundleSizes(void* arg0, u32 arg1) {
    s32* out = (s32*)arg0;
    out[0] = getbunsize(0x200, arg1, 4, 1);
    out[1] = getbunsize(0x200, arg1 >> 1, 4, 1);
    out[2] = getbunsize(0x200, arg1, 8, 0x40);
    out[3] = getbunsize(0x200, arg1, 8, 8);
    out[4] = getbunsize(0x200, arg1, 5, 1);
    out[5] = getbunsize(0x200, arg1, 5, 1);
    out[6] = getbunsize(0x200, arg1, 0xB, 1);
    out[7] = getbunsize(0x200, arg1, 0xB, 1);
    out[8] = getbunsize(0x200, arg1, 4, 0x30);
}

extern const u8 lbl_802F65C0[];
extern const u8 lbl_802F8A40[16][64];
// Huffman lookup table 0: bytes (a u32 initialiser would byte-reverse on a little-endian host).
// DUMP_DATA lbl_802F65C0 u8
const u8 lbl_802F65C0[16] = {
#include "dumpdata/lbl_802F65C0.inc"
};
// DUMP_DATA lbl_802F65D0 u8
const u8 lbl_802F65D0[32] = {
#include "dumpdata/lbl_802F65D0.inc"
};
// DUMP_DATA lbl_802F65F0 u8
const u8 lbl_802F65F0[32] = {
#include "dumpdata/lbl_802F65F0.inc"
};
// DUMP_DATA lbl_802F6610 u8
const u8 lbl_802F6610[32] = {
#include "dumpdata/lbl_802F6610.inc"
};
// DUMP_DATA lbl_802F6630 u8
const u8 lbl_802F6630[32] = {
#include "dumpdata/lbl_802F6630.inc"
};
// DUMP_DATA lbl_802F6650 u8
const u8 lbl_802F6650[32] = {
#include "dumpdata/lbl_802F6650.inc"
};
// DUMP_DATA lbl_802F6670 u8
const u8 lbl_802F6670[32] = {
#include "dumpdata/lbl_802F6670.inc"
};
// DUMP_DATA lbl_802F6690 u8
const u8 lbl_802F6690[64] = {
#include "dumpdata/lbl_802F6690.inc"
};
// DUMP_DATA lbl_802F66D0 u8
const u8 lbl_802F66D0[64] = {
#include "dumpdata/lbl_802F66D0.inc"
};
// DUMP_DATA lbl_802F6710 u8
const u8 lbl_802F6710[64] = {
#include "dumpdata/lbl_802F6710.inc"
};
// DUMP_DATA lbl_802F6750 u8
const u8 lbl_802F6750[64] = {
#include "dumpdata/lbl_802F6750.inc"
};
// DUMP_DATA lbl_802F6790 u8
const u8 lbl_802F6790[64] = {
#include "dumpdata/lbl_802F6790.inc"
};
// DUMP_DATA lbl_802F67D0 u8
const u8 lbl_802F67D0[64] = {
#include "dumpdata/lbl_802F67D0.inc"
};
// DUMP_DATA lbl_802F6810 u8
const u8 lbl_802F6810[128] = {
#include "dumpdata/lbl_802F6810.inc"
};
// DUMP_DATA lbl_802F6890 u8
const u8 lbl_802F6890[128] = {
#include "dumpdata/lbl_802F6890.inc"
};
// DUMP_DATA lbl_802F6910 u8
const u8 lbl_802F6910[128] = {
#include "dumpdata/lbl_802F6910.inc"
};
// DUMP_DATA lbl_802F6990 u8
const u8 lbl_802F6990[16] = {
#include "dumpdata/lbl_802F6990.inc"
};
// Pattern masks: the pixels of a 4-bit (masks) or 2-bit (scaled masks) pattern nibble that take colour 1 / colour 0.
// They are separate arrays defined above ExpandPlane: retail addresses each from the start of this unit's .rodata.
// DUMP_DATA lbl_802F69A0 u32
const u32 lbl_802F69A0[16] = {
#include "dumpdata/lbl_802F69A0.inc"
};
// DUMP_DATA lbl_802F69E0 u32
const u32 lbl_802F69E0[16] = {
#include "dumpdata/lbl_802F69E0.inc"
};
// DUMP_DATA lbl_802F6A20 u32
const u32 lbl_802F6A20[4] = {
#include "dumpdata/lbl_802F6A20.inc"
};
// DUMP_DATA lbl_802F6A30 u32
const u32 lbl_802F6A30[4] = {
#include "dumpdata/lbl_802F6A30.inc"
};
#define bink_masks1 lbl_802F69A0   // 4-pixel masks, colour 1
#define bink_masks0 lbl_802F69E0   // 4-pixel masks, colour 0
#define bink_smasks1 lbl_802F6A20  // doubled 2-pixel masks, colour 1
#define bink_smasks0 lbl_802F6A30  // doubled 2-pixel masks, colour 0

void ReadBPLossless(s16 *block, BitReader8011 *bits);
void ReadBPLossyWithMotion(u8 *dst, s32 dstStride, BitReader8011 *bits, u32 limit, u8 *src, s32 srcStride);
void fn_80125CA0(void *dst, void *stride, void *block, s32 quant);
void fn_80125CB4(void *dst, void *stride, void *block, s32 quant);
void FastmIDCT8x8WithMotion(u8 *dst, s32 stride, s16 *block, s32 q, u8 *src, s32 srcStride);

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

// Interleaved 8-row walk: rows alternate between the d0/s0 and d1/s1 pointer pairs.
#define BINK_ROWS8_2(ROW, d0, d1, dp, s0, s1, sp) \
    ROW(d0, s0);                                  \
    d0 = d1 + (dp);                               \
    s0 = s1 + (sp);                               \
    ROW(d1, s1);                                  \
    d1 = d0 + (dp);                               \
    s1 = s0 + (sp);                               \
    ROW(d0, s0);                                  \
    d0 = d1 + (dp);                               \
    s0 = s1 + (sp);                               \
    ROW(d1, s1);                                  \
    d1 = d0 + (dp);                               \
    s1 = s0 + (sp);                               \
    ROW(d0, s0);                                  \
    d0 = d1 + (dp);                               \
    s0 = s1 + (sp);                               \
    ROW(d1, s1);                                  \
    d1 = d0 + (dp);                               \
    s1 = s0 + (sp);                               \
    ROW(d0, s0);                                  \
    ROW(d1, s1)

// Same walk for a destination only; ROW gets the row number.
#define BINK_ROWS8_1(ROW, d0, d1, dp) \
    ROW(d0, 0);                       \
    d0 = d1 + (dp);                   \
    ROW(d1, 1);                       \
    d1 = d0 + (dp);                   \
    ROW(d0, 2);                       \
    d0 = d1 + (dp);                   \
    ROW(d1, 3);                       \
    d1 = d0 + (dp);                   \
    ROW(d0, 4);                       \
    d0 = d1 + (dp);                   \
    ROW(d1, 5);                       \
    d1 = d0 + (dp);                   \
    ROW(d0, 6);                       \
    ROW(d1, 7)

#define BINK_ROW_F64(d, s) *(f64 *)(d) = *(f64 *)(s)
#define BINK_ROW_U32(d, s)              \
    ((u32 *)(d))[0] = ((u32 *)(s))[0]; \
    ((u32 *)(d))[1] = ((u32 *)(s))[1]
#define BINK_ROW_U8(d, s) \
    (d)[0] = (s)[0];      \
    (d)[1] = (s)[1];      \
    (d)[2] = (s)[2];      \
    (d)[3] = (s)[3];      \
    (d)[4] = (s)[4];      \
    (d)[5] = (s)[5];      \
    (d)[6] = (s)[6];      \
    (d)[7] = (s)[7]

#define BINK_TMP_F64(d, k) *(f64 *)(d) = ((f64 *)tmp)[k]
#define BINK_RAW_F64(d, k) *(f64 *)(d) = ((f64 *)colors.cur)[k]
#define BINK_RAW_U32(d, k)                                  \
    ((u32 *)(d))[0] = ((u32 *)colors.cur)[(k) * 2];         \
    ((u32 *)(d))[1] = ((u32 *)colors.cur)[(k) * 2 + 1]
#define BINK_RAW_U8(d, k)                \
    (d)[0] = colors.cur[(k) * 8 + 0];    \
    (d)[1] = colors.cur[(k) * 8 + 1];    \
    (d)[2] = colors.cur[(k) * 8 + 2];    \
    (d)[3] = colors.cur[(k) * 8 + 3];    \
    (d)[4] = colors.cur[(k) * 8 + 4];    \
    (d)[5] = colors.cur[(k) * 8 + 5];    \
    (d)[6] = colors.cur[(k) * 8 + 6];    \
    (d)[7] = colors.cur[(k) * 8 + 7]
#define BINK_FILL8(d, k)     \
    ((u32 *)(d))[0] = c;     \
    ((u32 *)(d))[1] = c
#define BINK_FILL16(d)       \
    ((u32 *)(d))[0] = c;     \
    ((u32 *)(d))[1] = c;     \
    ((u32 *)(d))[2] = c;     \
    ((u32 *)(d))[3] = c
#define BINK_PAT8(d, k)                                                         \
    p = *pattern.cur++;                                                         \
    ((u32 *)(d))[0] = (c0 & bink_masks0[p & 15]) | (c1 & bink_masks1[p & 15]);  \
    ((u32 *)(d))[1] = (c0 & bink_masks0[p >> 4]) | (c1 & bink_masks1[p >> 4])
// Doubles the two pixels of a u16 into four.
#define BINK_DOUBLE2(w) (((w) << 8) | ((w) & 0xFF)) | (((w) << 16) & 0xFF000000)

#pragma push
#pragma global_optimizer off  /* Proven by the pragma gate (aids audit 2026-11-16: ExpandPlane exact with it, 72.04 and 1857/1758 instructions without): only this keeps both arms of the readColors if/else as retail has them (each optimizer switch alone hoists one arm); it replaces the earlier lifetimes/propagation/common_subs/unroll/loop_invariants set (Bink session) */
// Decodes one Bink plane (8x8 blocks, or 16x16 scaled blocks) into dst, using prev as the previous frame.
// `step` is the block-map step per block (1 for luma, which shares map entries between block pairs, 2 for
// chroma). Stores the number of skipped blocks whose map entry was clear in *skipped and returns the end of the
// plane's data. Structure cross-checked against n100f's ExpandPlane (reference only).
u32 *ExpandPlane(u8 *dstIn, u8 *prevIn, u32 width, u32 height, s32 pitch, u32 *data, s32 keyframe, u8 *blockMapIn,
                 s32 blocksWide, u32 step, void **bufs, u32 flags, u32 *skipped) {
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
    s32 x;
    u32 y;
    /* Local copies unproven as text, proven as numbering: retail colours dst, blockMap and prev between y and
       skipCount (r22, r21, r20), above bufs/step/pitch, which only locals declared here do (Bink session). */
    u8 *dst;
    u8 *blockMap;
    u8 *prev;
    u32 idx;
    u32 skipCount;
    void (*readColors)(void *b, void *bits, void *trees);
    s32 rowSkip;
    s32 i;

    dst = dstIn;
    prev = prevIn;
    blockMap = blockMapIn;
    rowSkip = pitch * 8 - width;
    idx = 0;
    skipCount = 0;
    bits.start = data;
    bits.next = data;
    bits.count = 0;
    bits.value = 0;
    if (flags & 0x8000) {
        readColors = (void (*)(void *, void *, void *))CheckReadHuff8Bundle;
    } else {
        readColors = (void (*)(void *, void *, void *))NewCheckReadHuff8Bundle;
    }
    OpenReadBundle((s32)bufs[0], &types, 0x200, width, 4, 1, 0);
    OpenReadBundle((s32)bufs[1], &subTypes, 0x200, width >> 1, 4, 1, 0);
    OpenReadBundle((s32)bufs[2], &colors, 0x200, width, 8, 64, 0);
    OpenReadBundle((s32)bufs[3], &pattern, 0x200, width, 8, 8, 0);
    OpenReadBundle((s32)bufs[4], &xOff, 0x200, width, 5, 1, 1);
    OpenReadBundle((s32)bufs[5], &yOff, 0x200, width, 5, 1, 1);
    OpenReadBundle((s32)bufs[6], &intraDC, 0x200, width, 11, 1, 0);
    OpenReadBundle((s32)bufs[7], &interDC, 0x200, width, 11, 1, 1);
    OpenReadBundle((s32)bufs[8], &runs, 0x200, width, 4, 48, 0);
    StartReadHuff4Bundle(&types, &bits);
    StartReadHuff4Bundle(&subTypes, &bits);
    StartReadHuff8Bundle(&colors, &bits, &trees);
    StartReadHuff4Bundle(&pattern, &bits);
    StartReadHuff4Bundle(&xOff, &bits);
    StartReadHuff4Bundle(&yOff, &bits);
    StartReadHuff4Bundle(&runs, &bits);
    for (y = 0; y < height; y += 8) {
        CheckReadRLEHuff4Bundle(&types, &bits);
        CheckReadRLEHuff4Bundle(&subTypes, &bits);
        readColors(&colors, &bits, &trees);
        CheckReadHuff4PairBundle(&pattern, &bits);
        CheckReadHuff4SBundle(&xOff, &bits);
        CheckReadHuff4SBundle(&yOff, &bits);
        CheckReadDelta16Bundle(&intraDC, &bits);
        CheckReadDelta16Bundle(&interDC, &bits);
        CheckReadHuff4Bundle(&runs, &bits);
        for (x = width; x != 0; x -= 8) {
            switch (*types.cur++) {
            case 0: {  // skip: keep the previous frame's block
                u8 *s0;
                u8 *d0;
                u8 *d1;
                u8 *s1;
                s0 = prev;
                d0 = dst;
                d1 = dst + pitch;
                s1 = prev + pitch;
                BINK_ROWS8_2(BINK_ROW_F64, d0, d1, pitch, s0, s1, pitch);
                if (blockMap[idx >> 1] == 0) {
                    skipCount++;
                }
                break;
            }
            case 1:  // scaled: one 8x8 block drawn as 16x16
                if (!((y >> 3) & 1)) {
                    switch (*subTypes.cur++) {
                    case 3: {
                        u32 run;
                        u32 w;
                        u16 *s;
                        u8 *d1;
                        u8 *d0;
                        u32 *next = bits.next;
                        u32 value = bits.value;
                        u32 avail = bits.count;
                        u32 v;
                        BINK_GETBITS(v, 4);
                        d0 = (u8 *)lbl_802F8A40[v];
                        i = 0;
                        do {
                            BINK_GETBIT(v);
                            if (v & 1) {
                                u8 c = *colors.cur++;
                                for (run = *runs.cur++ + 1; run != 0; run--) {
                                    tmp[d0[i++]] = c;
                                }
                            } else {
                                for (run = *runs.cur++ + 1; run != 0; run--) {
                                    tmp[d0[i++]] = *colors.cur++;
                                }
                            }
                        } while (i < 63);
                        if (i == 63) {
                            tmp[d0[63]] = *colors.cur++;
                        }
                        d0 = dst;
                        d1 = dst + pitch;
                        s = (u16 *)tmp;
                        for (i = 0; i < 8; i++) {
                            w = s[0];
                            w = BINK_DOUBLE2(w);
                            ((u32 *)d0)[0] = w;
                            ((u32 *)d1)[0] = w;
                            w = s[1];
                            w = BINK_DOUBLE2(w);
                            ((u32 *)d0)[1] = w;
                            ((u32 *)d1)[1] = w;
                            w = s[2];
                            w = BINK_DOUBLE2(w);
                            ((u32 *)d0)[2] = w;
                            ((u32 *)d1)[2] = w;
                            w = s[3];
                            w = BINK_DOUBLE2(w);
                            ((u32 *)d0)[3] = w;
                            ((u32 *)d1)[3] = w;
                            s += 4;
                            d0 = d1 + pitch;
                            d1 = d0 + pitch;
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
                        ReadBPLossless(block, &bits);
                        BINK_BR_GETBITS(&bits, quant, 4);
                        fn_80125CB4(dst, (void *)pitch, block, quant);
                        break;
                    }
                    case 6: {
                        u32 c;
                        u8 *d0;
                        u8 *d1;
                        c = *colors.cur++;
                        c |= c << 8;
                        c |= c << 16;
                        d0 = dst;
                        d1 = dst + pitch;
                        BINK_FILL16(d0);
                        for (i = 0; i < 7; i++) {
                            BINK_FILL16(d1);
                            d0 = d1 + pitch;
                            BINK_FILL16(d0);
                            d1 = d0 + pitch;
                        }
                        BINK_FILL16(d1);
                        break;
                    }
                    case 8: {
                        u32 c0;
                        u32 c1;
                        u8 *d0;
                        u8 *d1;
                        c0 = *colors.cur++;
                        c0 |= c0 << 8;
                        c0 |= c0 << 16;
                        c1 = *colors.cur++;
                        c1 |= c1 << 8;
                        c1 |= c1 << 16;
                        d0 = dst;
                        d1 = dst + pitch;
                        for (i = 0; i < 8; i++) {
                            u32 p = *pattern.cur++;
                            u32 w;
                            w = (c0 & bink_smasks0[p & 3]) | (c1 & bink_smasks1[p & 3]);
                            ((u32 *)d0)[0] = w;
                            ((u32 *)d1)[0] = w;
                            w = (c0 & bink_smasks0[(p >> 2) & 3]) | (c1 & bink_smasks1[(p >> 2) & 3]);
                            ((u32 *)d0)[1] = w;
                            ((u32 *)d1)[1] = w;
                            w = (c0 & bink_smasks0[(p >> 4) & 3]) | (c1 & bink_smasks1[(p >> 4) & 3]);
                            ((u32 *)d0)[2] = w;
                            ((u32 *)d1)[2] = w;
                            w = (c0 & bink_smasks0[p >> 6]) | (c1 & bink_smasks1[p >> 6]);
                            ((u32 *)d0)[3] = w;
                            ((u32 *)d1)[3] = w;
                            d0 = d1 + pitch;
                            d1 = d0 + pitch;
                        }
                        break;
                    }
                    case 9: {
                        u32 w;
                        s32 pitch2;
                        u8 *d0;
                        u8 *d1;
                        u32 b;
                        d0 = dst;
                        d1 = dst + pitch;
                        pitch2 = pitch + pitch;
                        if ((u32)colors.cur & 3) {
                            for (i = 0; i < 8; i++) {
                                b = colors.cur[0];
                                w = colors.cur[1];
                                w = (w | (w << 8)) | ((b << 24) | (b << 16));
                                ((u32 *)(d0))[0] = w;
                                ((u32 *)(d1))[0] = w;
                                b = colors.cur[2];
                                w = colors.cur[3];
                                w = (w | (w << 8)) | ((b << 24) | (b << 16));
                                ((u32 *)(d0))[1] = w;
                                ((u32 *)(d1))[1] = w;
                                b = colors.cur[4];
                                w = colors.cur[5];
                                w = (w | (w << 8)) | ((b << 24) | (b << 16));
                                ((u32 *)(d0))[2] = w;
                                ((u32 *)(d1))[2] = w;
                                b = colors.cur[6];
                                w = colors.cur[7];
                                w = (w | (w << 8)) | ((b << 24) | (b << 16));
                                ((u32 *)(d0))[3] = w;
                                ((u32 *)(d1))[3] = w;
                                colors.cur += 8;
                                d0 += pitch2;
                                d1 += pitch2;
                            }
                        } else {
                            for (i = 0; i < 8; i++) {
                                w = ((u16 *)colors.cur)[0];
                                w = BINK_DOUBLE2(w);
                                ((u32 *)(d0))[0] = w;
                                ((u32 *)(d1))[0] = w;
                                w = ((u16 *)colors.cur)[1];
                                w = BINK_DOUBLE2(w);
                                ((u32 *)(d0))[1] = w;
                                ((u32 *)(d1))[1] = w;
                                w = ((u16 *)colors.cur)[2];
                                w = BINK_DOUBLE2(w);
                                ((u32 *)(d0))[2] = w;
                                ((u32 *)(d1))[2] = w;
                                w = ((u16 *)colors.cur)[3];
                                w = BINK_DOUBLE2(w);
                                ((u32 *)(d0))[3] = w;
                                ((u32 *)(d1))[3] = w;
                                colors.cur += 8;
                                d0 += pitch2;
                                d1 += pitch2;
                            }
                        }
                        break;
                    }
                    }
                }
                blockMap[idx >> 1] = 1;
                idx += step;
                blockMap[idx >> 1] = 1;
                dst += 8;
                prev += 8;
                x -= 8;
                break;
            case 2: {  // motion: copy a block of the previous frame
                u8 *s0;
                u8 *d0;
                u8 *d1;
                u8 *s1;
                blockMap[idx >> 1] = 1;
                s0 = prev + pitch * *(s8 *)yOff.cur++ + *(s8 *)xOff.cur++;
                d0 = dst;
                s1 = s0 + pitch;
                d1 = dst + pitch;
                if (!(((u32)dst | (u32)s0) & 7)) {
                    BINK_ROWS8_2(BINK_ROW_F64, d0, d1, pitch, s0, s1, pitch);
                } else if (!(((u32)dst | (u32)s0) & 3)) {
                    BINK_ROWS8_2(BINK_ROW_U32, d0, d1, pitch, s0, s1, pitch);
                } else {
                    BINK_ROWS8_2(BINK_ROW_U8, d0, d1, pitch, s0, s1, pitch);
                }
                break;
            }
            case 3: {  // run: runs of one color or of literal colors along a scan order
                u32 run;
                const u8 *scan;
                u32 *next = bits.next;
                u32 value = bits.value;
                u32 avail = bits.count;
                u32 v;
                BINK_GETBITS(v, 4);
                scan = lbl_802F8A40[v];
                blockMap[idx >> 1] = 1;
                i = 0;
                do {
                    BINK_GETBIT(v);
                    if (v & 1) {
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
                    BINK_ROWS8_1(BINK_TMP_F64, d0, d1, pitch);
                }
                bits.next = next;
                bits.value = value;
                bits.count = avail;
                break;
            }
            case 4: {  // residue on top of a motion copy
                u8 *src;
                u32 n;
                blockMap[idx >> 1] = 1;
                src = prev + pitch * *(s8 *)yOff.cur++ + *(s8 *)xOff.cur++;
                BINK_BR_GETBITS(&bits, n, 7);
                ReadBPLossyWithMotion(dst, pitch, &bits, n, src, pitch);
                break;
            }
            case 5: {  // intra DCT block
                s32 quant;
                blockMap[idx >> 1] = 1;
                block[0] = *(s16 *)intraDC.cur;
                intraDC.cur += 2;
                ReadBPLossless(block, &bits);
                BINK_BR_GETBITS(&bits, quant, 4);
                fn_80125CA0(dst, (void *)pitch, block, quant);
                break;
            }
            case 6: {  // fill with one color
                u32 c;
                u8 *d0;
                u8 *d1;
                d0 = dst;
                d1 = dst + pitch;
                c = *colors.cur++;
                blockMap[idx >> 1] = 1;
                c |= c << 8;
                c |= c << 16;
                BINK_ROWS8_1(BINK_FILL8, d0, d1, pitch);
                break;
            }
            case 7: {  // inter DCT block on top of a motion copy
                u8 *src;
                s32 quant;
                blockMap[idx >> 1] = 1;
                src = prev + pitch * *(s8 *)yOff.cur++ + *(s8 *)xOff.cur++;
                block[0] = *(s16 *)interDC.cur;
                interDC.cur += 2;
                ReadBPLossless(block, &bits);
                BINK_BR_GETBITS(&bits, quant, 4);
                FastmIDCT8x8WithMotion(dst, pitch, block, quant, src, pitch);
                break;
            }
            case 8: {  // two-color pattern
                u32 c0;
                u32 c1;
                u8 *d0;
                u8 *d1;
                u32 p;
                d0 = dst;
                d1 = dst + pitch;
                c0 = *colors.cur++;
                c1 = *colors.cur++;
                blockMap[idx >> 1] = 1;
                c0 |= c0 << 8;
                c0 |= c0 << 16;
                c1 |= c1 << 8;
                c1 |= c1 << 16;
                BINK_ROWS8_1(BINK_PAT8, d0, d1, pitch);
                break;
            }
            case 9:  // raw pixels
                blockMap[idx >> 1] = 1;
                {
                    u8 *d0 = dst;
                    u8 *d1 = dst + pitch;
                    if (!(((u32)dst | (u32)colors.cur) & 7)) {
                        BINK_ROWS8_1(BINK_RAW_F64, d0, d1, pitch);
                    } else if (!(((u32)dst | (u32)colors.cur) & 3)) {
                        BINK_ROWS8_1(BINK_RAW_U32, d0, d1, pitch);
                    } else {
                        BINK_ROWS8_1(BINK_RAW_U8, d0, d1, pitch);
                    }
                }
                colors.cur += 64;
                break;
            }
            idx += step;
            dst += 8;
            prev += 8;
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
#pragma pop

// Decodes a video frame into the next frame buffer (alpha, luma, then the two chroma planes); returns the number
// of skipped blocks in the luma plane (in block pairs).
u32 ExpandBink(void *video, s32 blockMap, s32 blocksWide, s32 *data, s32 keyframe, s32 bufs, s32 flags,
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
            ExpandPlane(v->frames[cur].a.buffer, v->frames[old].a.buffer, v->width, v->height,
                        v->frames[old].a.pitch, (u32 *)(data + 1), keyframe, (u8 *)blockMap, blocksWide, 1,
                        (void **)bufs, flags, &result);
        }
        data = (s32 *)((u8 *)data + *data);
    }
    result = 0;
    if (!(flags & 0x8000)) {
        data++;
    }
    end = ExpandPlane(v->frames[cur].y.buffer, v->frames[old].y.buffer, v->width, v->height, v->frames[old].y.pitch,
                      (u32 *)data, keyframe, (u8 *)blockMap, blocksWide, 1, (void **)bufs, flags, &result);
    if (!(flags & 0x8000)) {
        end = (u32 *)((u8 *)data + data[-1] - 4);
    }
    result >>= 2;
    if (!(flags & 0x20000)) {
        end = ExpandPlane(v->frames[cur].cr.buffer, v->frames[old].cr.buffer, v->cWidth, v->cHeight,
                          v->frames[old].cr.pitch, end, keyframe, (u8 *)blockMap, blocksWide, 2, (void **)bufs,
                          flags, &result);
        result = 0;
        ExpandPlane(v->frames[cur].cb.buffer, v->frames[old].cb.buffer, v->cWidth, v->cHeight,
                    v->frames[old].cb.pitch, end, keyframe, (u8 *)blockMap, blocksWide, 2, (void **)bufs, flags,
                    &result);
    }
    return result;
}

extern u32 lbl_8055DEF0;  // total size of the pushed allocations
extern u32 lbl_8055DEF4;
// pushmalloc: registers *slot for the next batched allocation (popmalloc = popmalloc). Sizes are rounded to
// 32 bytes and padded so consecutive buffers start in different cache sets; the registration arrays double
// when full.
void pushmalloc(void **slot, u32 amount) {
    if (lbl_80559950 == lbl_8055DEF4) {
        void ***slots = radmalloc(lbl_80559950 * 16);
        u32 *sizes = (u32 *)((u8 *)slots + lbl_80559950 * 8);
        memcpy(slots, lbl_80559948, lbl_80559950 * 4);
        memcpy(sizes, lbl_8055994C, lbl_80559950 * 4);
        lbl_80559950 *= 2;
        if (lbl_80559950 != 64) {
            radfree(lbl_80559948);
        }
        lbl_80559948 = slots;
        lbl_8055994C = sizes;
    }
    amount = (amount + 31) & ~31;
    amount += ((((lbl_8055DEF0 >> 5) & 31) + 33 - ((amount >> 5) & 31)) << 5) & 0x3E0;
    lbl_8055DEF0 += amount;
    lbl_8055994C[lbl_8055DEF4] = amount;
    lbl_80559948[lbl_8055DEF4++] = slot;
}

u32 fn_8011FF24(void) {
    return lbl_8055DEF0;
}

extern u32 lbl_8055DEF4;
// Allocates one block for all the pending buffers, hands each its slice and returns the block.
void *popmalloc(s32 arg0) {
    s32 size = (arg0 + 0x1F) & ~0x1F;
    u32 i;
    u8 *block = radmalloc(lbl_8055DEF0 + size);
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
extern void *radmalloc(s32);
void *radmalloc(s32 arg0) {
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

// Frees a block from radmalloc: the bytes before it hold the alignment offset and which allocator made it.
void radfree(void *arg0) {
    u8 *ptr = arg0;
    if (ptr == 0) return;
    if (ptr[-2] == 3) ((void (*)(void *))*(void **)(ptr - 8))(ptr - ptr[-1]);
    else free(ptr - ptr[-1]);
}

void rdft(int n, int isgn, f32 *a, int *ip, f32 *w)
{
    void makewt(int nw, int *ip, f32 *w);
    void makect(int nc, int *ip, f32 *c);
    void fn_80120CA4(int n, f32 *a, int *ip, int nw, f32 *w);
    void fn_80120E14(int n, f32 *a, int *ip, int nw, f32 *w);
    void rftfsub(int n, f32 *a, int nc, f32 *c);
    void rftbsub(int n, f32 *a, int nc, f32 *c);
    int nw, nc;
    f32 xi;
    
    nw = ip[0];
    if (n > (nw << 2)) {
        nw = n >> 2;
        makewt(nw, ip, w);
    }
    nc = ip[1];
    if (n > (nc << 2)) {
        nc = n >> 2;
        makect(nc, ip, w + nw);
    }
    if (isgn >= 0) {
        if (n > 4) {
            fn_80120CA4(n, a, ip + 2, nw, w);
            rftfsub(n, a, nc, w + nw);
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
            rftbsub(n, a, nc, w + nw);
            fn_80120E14(n, a, ip + 2, nw, w);
        } else if (n == 4) {
            fn_80120E14(n, a, ip + 2, nw, w);
        }
    }
}

void ddct(int n, int isgn, f32 *a, int *ip, f32 *w)
{
    void makewt(int nw, int *ip, f32 *w);
    void makect(int nc, int *ip, f32 *c);
    void fn_80120CA4(int n, f32 *a, int *ip, int nw, f32 *w);
    void fn_80120E14(int n, f32 *a, int *ip, int nw, f32 *w);
    void rftfsub(int n, f32 *a, int nc, f32 *c);
    void rftbsub(int n, f32 *a, int nc, f32 *c);
    void dctsub(int n, f32 *a, int nc, f32 *c);
    int j, nw, nc;
    f32 xr;
    
    nw = ip[0];
    if (n > (nw << 2)) {
        nw = n >> 2;
        makewt(nw, ip, w);
    }
    nc = ip[1];
    if (n > nc) {
        nc = n;
        makect(nc, ip, w + nw);
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
            rftbsub(n, a, nc, w + nw);
            fn_80120E14(n, a, ip + 2, nw, w);
        } else if (n == 4) {
            fn_80120E14(n, a, ip + 2, nw, w);
        }
    }
    dctsub(n, a, nc, w + nw);
    if (isgn >= 0) {
        if (n > 4) {
            fn_80120CA4(n, a, ip + 2, nw, w);
            rftfsub(n, a, nc, w + nw);
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

// MSL's inline float wrappers (math_double.h). The float build of the package calls these, and spells
// Ooura's atan(1.0) as the float constant pi/4.
inline float cosf(float x) { return cos(x); }
inline float sinf(float x) { return sin(x); }

void makewt(int nw, int *ip, f32 *w)
{
    int j, nwh, nw0, nw1;
    f32 delta, wn4r, wk1r, wk1i, wk3r, wk3i;
    
    ip[0] = nw;
    ip[1] = 1;
    if (nw > 2) {
        nwh = nw >> 1;
        delta = 0.785398185f / nwh;  // atan(1.0) / nwh
        wn4r = cosf(delta * nwh);
        w[0] = 1;
        w[1] = wn4r;
        if (nwh >= 4) {
            w[2] = 0.5f / cosf(delta * 2);
            w[3] = 0.5f / cosf(delta * 6);
        }
        for (j = 4; j < nwh; j += 4) {
            w[j] = cosf(delta * j);
            w[j + 1] = sinf(delta * j);
            w[j + 2] = cosf(3 * delta * j);
            w[j + 3] = sinf(3 * delta * j);
        }
        nw0 = 0;
        while (nwh > 2) {
            nw1 = nw0 + nwh;
            nwh >>= 1;
            w[nw1] = 1;
            w[nw1 + 1] = wn4r;
            if (nwh >= 4) {
                wk1r = w[nw0 + 4];
                wk3r = w[nw0 + 6];
                w[nw1 + 2] = 0.5f / wk1r;
                w[nw1 + 3] = 0.5f / wk3r;
            }
            for (j = 4; j < nwh; j += 4) {
                wk1r = w[nw0 + 2 * j];
                wk1i = w[nw0 + 2 * j + 1];
                wk3r = w[nw0 + 2 * j + 2];
                wk3i = w[nw0 + 2 * j + 3];
                w[nw1 + j] = wk1r;
                w[nw1 + j + 1] = wk1i;
                w[nw1 + j + 2] = wk3r;
                w[nw1 + j + 3] = wk3i;
            }
            nw0 = nw1;
        }
    }
}


void makect(int nc, int *ip, f32 *c)
{
    int j, nch;
    f32 delta;
    
    ip[1] = nc;
    if (nc > 1) {
        nch = nc >> 1;
        delta = 0.785398185f / nch;  // atan(1.0) / nch
        c[0] = cosf(delta * nch);
        c[nch] = 0.5f * c[0];
        for (j = 1; j < nch; j++) {
            c[j] = 0.5f * cosf(delta * j);
            c[nc - j] = 0.5f * sinf(delta * j);
        }
    }
}


#define CDFT_RECURSIVE_N 512  /* length of the recursive FFT mode, <= (L1 cache size) / 16 */

void fn_80120CA4(int n, f32 *a, int *ip, int nw, f32 *w)
{
    void bitrv2(int n, int *ip, f32 *a);
    void bitrv216(f32 *a);
    void bitrv208(f32 *a);
    void cftf1st(int n, f32 *a, f32 *w);
    void cftrec1(int n, f32 *a, int nw, f32 *w);
    void cftrec2(int n, f32 *a, int nw, f32 *w);
    void cftexp1(int n, f32 *a, int nw, f32 *w);
    void cftfx41(int n, f32 *a, int nw, f32 *w);
    void cftf161(f32 *a, f32 *w);
    void cftf081(f32 *a, f32 *w);
    void cftf040(f32 *a);
    void cftx020(f32 *a);
    int m;
    
    if (n > 32) {
        m = n >> 2;
        cftf1st(n, a, &w[nw - m]);
        if (n > CDFT_RECURSIVE_N) {
            cftrec1(m, a, nw, w);
            cftrec2(m, &a[m], nw, w);
            cftrec1(m, &a[2 * m], nw, w);
            cftrec1(m, &a[3 * m], nw, w);
        } else if (m > 32) {
            cftexp1(n, a, nw, w);
        } else {
            cftfx41(n, a, nw, w);
        }
        bitrv2(n, ip, a);
    } else if (n > 8) {
        if (n == 32) {
            cftf161(a, &w[nw - 8]);
            bitrv216(a);
        } else {
            cftf081(a, w);
            bitrv208(a);
        }
    } else if (n == 8) {
        cftf040(a);
    } else if (n == 4) {
        cftx020(a);
    }
}

void fn_80120E14(int n, f32 *a, int *ip, int nw, f32 *w)
{
    void bitrv2conj(int n, int *ip, f32 *a);
    void bitrv216neg(f32 *a);
    void bitrv208neg(f32 *a);
    void cftb1st(int n, f32 *a, f32 *w);
    void cftrec1(int n, f32 *a, int nw, f32 *w);
    void cftrec2(int n, f32 *a, int nw, f32 *w);
    void cftexp1(int n, f32 *a, int nw, f32 *w);
    void cftfx41(int n, f32 *a, int nw, f32 *w);
    void cftf161(f32 *a, f32 *w);
    void cftf081(f32 *a, f32 *w);
    void cftb040(f32 *a);
    void cftx020(f32 *a);
    int m;
    
    if (n > 32) {
        m = n >> 2;
        cftb1st(n, a, &w[nw - m]);
        if (n > CDFT_RECURSIVE_N) {
            cftrec1(m, a, nw, w);
            cftrec2(m, &a[m], nw, w);
            cftrec1(m, &a[2 * m], nw, w);
            cftrec1(m, &a[3 * m], nw, w);
        } else if (m > 32) {
            cftexp1(n, a, nw, w);
        } else {
            cftfx41(n, a, nw, w);
        }
        bitrv2conj(n, ip, a);
    } else if (n > 8) {
        if (n == 32) {
            cftf161(a, &w[nw - 8]);
            bitrv216neg(a);
        } else {
            cftf081(a, w);
            bitrv208neg(a);
        }
    } else if (n == 8) {
        cftb040(a);
    } else if (n == 4) {
        cftx020(a);
    }
}

void bitrv2(int n, int *ip, f32 *a) {
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

void bitrv2conj(int n, int *ip, f32 *a) {
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

void bitrv216(f32 *a)
{
    f32 x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i, 
        x5r, x5i, x7r, x7i, x8r, x8i, x10r, x10i, 
        x11r, x11i, x12r, x12i, x13r, x13i, x14r, x14i;
    
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
    x7r = a[14];
    x7i = a[15];
    x8r = a[16];
    x8i = a[17];
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
    a[2] = x8r;
    a[3] = x8i;
    a[4] = x4r;
    a[5] = x4i;
    a[6] = x12r;
    a[7] = x12i;
    a[8] = x2r;
    a[9] = x2i;
    a[10] = x10r;
    a[11] = x10i;
    a[14] = x14r;
    a[15] = x14i;
    a[16] = x1r;
    a[17] = x1i;
    a[20] = x5r;
    a[21] = x5i;
    a[22] = x13r;
    a[23] = x13i;
    a[24] = x3r;
    a[25] = x3i;
    a[26] = x11r;
    a[27] = x11i;
    a[28] = x7r;
    a[29] = x7i;
}

void bitrv216neg(f32 *a)
{
    f32 x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i, 
        x5r, x5i, x6r, x6i, x7r, x7i, x8r, x8i, 
        x9r, x9i, x10r, x10i, x11r, x11i, x12r, x12i, 
        x13r, x13i, x14r, x14i, x15r, x15i;
    
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

void bitrv208(f32 *a)
{
    f32 x1r, x1i, x3r, x3i, x4r, x4i, x6r, x6i;
    
    x1r = a[2];
    x1i = a[3];
    x3r = a[6];
    x3i = a[7];
    x4r = a[8];
    x4i = a[9];
    x6r = a[12];
    x6i = a[13];
    a[2] = x4r;
    a[3] = x4i;
    a[6] = x6r;
    a[7] = x6i;
    a[8] = x1r;
    a[9] = x1i;
    a[12] = x3r;
    a[13] = x3i;
}

void bitrv208neg(f32 *a)
{
    f32 x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i, 
        x5r, x5i, x6r, x6i, x7r, x7i;
    
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
    a[2] = x7r;
    a[3] = x7i;
    a[4] = x3r;
    a[5] = x3i;
    a[6] = x5r;
    a[7] = x5i;
    a[8] = x1r;
    a[9] = x1i;
    a[10] = x6r;
    a[11] = x6i;
    a[12] = x2r;
    a[13] = x2i;
    a[14] = x4r;
    a[15] = x4i;
}

void cftf1st(int n, f32 *a, f32 *w)
{
    int j, j0, j1, j2, j3, k, m, mh;
    f32 wn4r, csc1, csc3, wk1r, wk1i, wk3r, wk3i, 
        wd1r, wd1i, wd3r, wd3i;
    f32 x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i;
    
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
        wk3i = csc3 * (wd3i - w[k + 3]);
        wd1r = w[k];
        wd1i = w[k + 1];
        wd3r = w[k + 2];
        wd3i = -w[k + 3];
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

void cftb1st(int n, f32 *a, f32 *w)
{
    int j, j0, j1, j2, j3, k, m, mh;
    f32 wn4r, csc1, csc3, wk1r, wk1i, wk3r, wk3i, 
        wd1r, wd1i, wd3r, wd3i;
    f32 x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i;
    
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
        wk3i = csc3 * (wd3i - w[k + 3]);
        wd1r = w[k];
        wd1i = w[k + 1];
        wd3r = w[k + 2];
        wd3i = -w[k + 3];
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

void cftrec1(int n, f32 *a, int nw, f32 *w)
{
    void cftrec1(int n, f32 *a, int nw, f32 *w);
    void cftrec2(int n, f32 *a, int nw, f32 *w);
    void cftmdl1(int n, f32 *a, f32 *w);
    void cftexp1(int n, f32 *a, int nw, f32 *w);
    int m;
    
    m = n >> 2;
    cftmdl1(n, a, &w[nw - 2 * m]);
    if (n > CDFT_RECURSIVE_N) {
        cftrec1(m, a, nw, w);
        cftrec2(m, &a[m], nw, w);
        cftrec1(m, &a[2 * m], nw, w);
        cftrec1(m, &a[3 * m], nw, w);
    } else {
        cftexp1(n, a, nw, w);
    }
}

void cftrec2(int n, f32 *a, int nw, f32 *w)
{
    void cftrec1(int n, f32 *a, int nw, f32 *w);
    void cftrec2(int n, f32 *a, int nw, f32 *w);
    void cftmdl2(int n, f32 *a, f32 *w);
    void cftexp2(int n, f32 *a, int nw, f32 *w);
    int m;
    
    m = n >> 2;
    cftmdl2(n, a, &w[nw - n]);
    if (n > CDFT_RECURSIVE_N) {
        cftrec1(m, a, nw, w);
        cftrec2(m, &a[m], nw, w);
        cftrec1(m, &a[2 * m], nw, w);
        cftrec2(m, &a[3 * m], nw, w);
    } else {
        cftexp2(n, a, nw, w);
    }
}

void cftexp1(int n, f32 *a, int nw, f32 *w)
{
    void cftmdl1(int n, f32 *a, f32 *w);
    void cftmdl2(int n, f32 *a, f32 *w);
    void cftfx41(int n, f32 *a, int nw, f32 *w);
    void cftfx42(int n, f32 *a, int nw, f32 *w);
    int j, k, l;
    
    l = n >> 2;
    while (l > 128) {
        for (k = l; k < n; k <<= 2) {
            for (j = k - l; j < n; j += 4 * k) {
                cftmdl1(l, &a[j], &w[nw - (l >> 1)]);
                cftmdl2(l, &a[k + j], &w[nw - l]);
                cftmdl1(l, &a[2 * k + j], &w[nw - (l >> 1)]);
            }
        }
        cftmdl1(l, &a[n - l], &w[nw - (l >> 1)]);
        l >>= 2;
    }
    for (k = l; k < n; k <<= 2) {
        for (j = k - l; j < n; j += 4 * k) {
            cftmdl1(l, &a[j], &w[nw - (l >> 1)]);
            cftfx41(l, &a[j], nw, w);
            cftmdl2(l, &a[k + j], &w[nw - l]);
            cftfx42(l, &a[k + j], nw, w);
            cftmdl1(l, &a[2 * k + j], &w[nw - (l >> 1)]);
            cftfx41(l, &a[2 * k + j], nw, w);
        }
    }
    cftmdl1(l, &a[n - l], &w[nw - (l >> 1)]);
    cftfx41(l, &a[n - l], nw, w);
}

void cftexp2(int n, f32 *a, int nw, f32 *w)
{
    void cftmdl1(int n, f32 *a, f32 *w);
    void cftmdl2(int n, f32 *a, f32 *w);
    void cftfx41(int n, f32 *a, int nw, f32 *w);
    void cftfx42(int n, f32 *a, int nw, f32 *w);
    int j, k, l, m;
    
    m = n >> 1;
    l = n >> 2;
    while (l > 128) {
        for (k = l; k < m; k <<= 2) {
            for (j = k - l; j < m; j += 2 * k) {
                cftmdl1(l, &a[j], &w[nw - (l >> 1)]);
                cftmdl1(l, &a[m + j], &w[nw - (l >> 1)]);
            }
            for (j = 2 * k - l; j < m; j += 4 * k) {
                cftmdl2(l, &a[j], &w[nw - l]);
                cftmdl2(l, &a[m + j], &w[nw - l]);
            }
        }
        l >>= 2;
    }
    for (k = l; k < m; k <<= 2) {
        for (j = k - l; j < m; j += 2 * k) {
            cftmdl1(l, &a[j], &w[nw - (l >> 1)]);
            cftfx41(l, &a[j], nw, w);
            cftmdl1(l, &a[m + j], &w[nw - (l >> 1)]);
            cftfx41(l, &a[m + j], nw, w);
        }
        for (j = 2 * k - l; j < m; j += 4 * k) {
            cftmdl2(l, &a[j], &w[nw - l]);
            cftfx42(l, &a[j], nw, w);
            cftmdl2(l, &a[m + j], &w[nw - l]);
            cftfx42(l, &a[m + j], nw, w);
        }
    }
}

void cftmdl1(int n, f32 *a, f32 *w)
{
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
        wk3i = -w[k + 3];
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

void cftmdl2(int n, f32 *a, f32 *w)
{
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
        wk3i = -w[k + 3];
        kr -= 4;
        wd1i = w[kr];
        wd1r = w[kr + 1];
        wd3i = w[kr + 2];
        wd3r = -w[kr + 3];
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

void cftfx41(int n, f32 *a, int nw, f32 *w)
{
    void cftf161(f32 *a, f32 *w);
    void cftf162(f32 *a, f32 *w);
    void cftf081(f32 *a, f32 *w);
    void cftf082(f32 *a, f32 *w);
    
    if (n == 128) {
        cftf161(a, &w[nw - 8]);
        cftf162(&a[32], &w[nw - 32]);
        cftf161(&a[64], &w[nw - 8]);
        cftf161(&a[96], &w[nw - 8]);
    } else {
        cftf081(a, &w[nw - 16]);
        cftf082(&a[16], &w[nw - 16]);
        cftf081(&a[32], &w[nw - 16]);
        cftf081(&a[48], &w[nw - 16]);
    }
}

void cftfx42(int n, f32 *a, int nw, f32 *w)
{
    void cftf161(f32 *a, f32 *w);
    void cftf162(f32 *a, f32 *w);
    void cftf081(f32 *a, f32 *w);
    void cftf082(f32 *a, f32 *w);
    
    if (n == 128) {
        cftf161(a, &w[nw - 8]);
        cftf162(&a[32], &w[nw - 32]);
        cftf161(&a[64], &w[nw - 8]);
        cftf162(&a[96], &w[nw - 32]);
    } else {
        cftf081(a, &w[nw - 16]);
        cftf082(&a[16], &w[nw - 16]);
        cftf081(&a[32], &w[nw - 16]);
        cftf082(&a[48], &w[nw - 16]);
    }
}

void cftf161(f32 *a, f32 *w)
{
    f32 wn4r, wk1r, wk1i, 
        x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, 
        y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i, 
        y8r, y8i, y9r, y9i, y10r, y10i, y11r, y11i, 
        y12r, y12i, y13r, y13i, y14r, y14i, y15r, y15i;
    
    wn4r = w[1];
    wk1i = wn4r * w[2];
    wk1r = wk1i + w[2];
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

void cftf162(f32 *a, f32 *w)
{
    f32 wn4r, wk1r, wk1i, wk2r, wk2i, wk3r, wk3i, 
        x0r, x0i, x1r, x1i, x2r, x2i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, 
        y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i, 
        y8r, y8i, y9r, y9i, y10r, y10i, y11r, y11i, 
        y12r, y12i, y13r, y13i, y14r, y14i, y15r, y15i;
    
    wn4r = w[1];
    wk1r = w[4];
    wk1i = w[5];
    wk3r = w[6];
    wk3i = w[7];
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

void cftf081(f32 *a, f32 *w)
{
    f32 wn4r, x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, 
        y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i;
    
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

void cftf082(f32 *a, f32 *w)
{
    f32 wn4r, wk1r, wk1i, x0r, x0i, x1r, x1i, 
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, 
        y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i;
    
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

void cftf040(f32 *a) {
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

void cftb040(f32 *a) {
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

void cftx020(f32 *a) {
    f32 x0r, x0i;

    x0r = a[0] - a[2];
    x0i = a[1] - a[3];
    a[0] += a[2];
    a[1] += a[3];
    a[2] = x0r;
    a[3] = x0i;
}

void rftfsub(int n, f32 *a, int nc, f32 *c)
{
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

void rftbsub(int n, f32 *a, int nc, f32 *c)
{
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

void dctsub(int n, f32 *a, int nc, f32 *c) {
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
// This is the AAN fast IDCT as laid out in libjpeg's jidctfst.c (same work variables, reused; the BINK_A
// constants are its FIX_ constants at 11 bits). The plain OR chain below is what gives retail's OR tree.
void fastidct8x8(u8* dst, s32 stride, s16* block, s32* quant) {
    s32 tmp[64];
    s32* t = tmp;
    s32 i;
    s32 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    s32 tmp10, tmp11, tmp12, tmp13;
    s32 z5, z10, z11, z12, z13;

    // Pass 1: columns, from the coefficient block into the work array.
    for (i = 0; i < 8; i++) {
        if ((block[8] | block[16] | block[24] | block[32] | block[40] | block[48] | block[56]) == 0) {
            // All AC terms are zero: the column is the dequantized DC value.
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
            // Even part
            tmp0 = (block[0] * quant[0]) >> 11;
            tmp1 = (block[16] * quant[16]) >> 11;
            tmp2 = (block[32] * quant[32]) >> 11;
            tmp3 = (block[48] * quant[48]) >> 11;
            tmp10 = tmp0 + tmp2;
            tmp11 = tmp0 - tmp2;
            tmp13 = tmp1 + tmp3;
            tmp12 = BINK_MUL(BINK_A1, tmp1 - tmp3) - tmp13;
            tmp0 = tmp10 + tmp13;
            tmp3 = tmp10 - tmp13;
            tmp1 = tmp11 + tmp12;
            tmp2 = tmp11 - tmp12;

            // Odd part
            tmp4 = (block[8] * quant[8]) >> 11;
            tmp5 = (block[24] * quant[24]) >> 11;
            tmp6 = (block[40] * quant[40]) >> 11;
            tmp7 = (block[56] * quant[56]) >> 11;
            z13 = tmp6 + tmp5;
            z10 = tmp6 - tmp5;
            z11 = tmp4 + tmp7;
            z12 = tmp4 - tmp7;
            tmp7 = z11 + z13;
            tmp11 = BINK_MUL(BINK_A1, z11 - z13);
            z5 = BINK_MUL(BINK_A3, z10 + z12);
            tmp10 = BINK_MUL(BINK_A2, z12) - z5;
            tmp12 = z5 + BINK_MUL(BINK_A4, z10);
            tmp6 = tmp12 - tmp7;
            tmp5 = tmp11 - tmp6;
            tmp4 = tmp10 + tmp5;

            block++;
            quant++;
            t[0] = tmp0 + tmp7;
            t[56] = tmp0 - tmp7;
            t[8] = tmp1 + tmp6;
            t[48] = tmp1 - tmp6;
            t[16] = tmp2 + tmp5;
            t[40] = tmp2 - tmp5;
            t[32] = tmp3 + tmp4;
            t[24] = tmp3 - tmp4;
            t++;
        }
    }

    // Pass 2: rows, from the work array to pixels.
    t = tmp;
    for (i = 0; i < 8; i++) {
        // Even part
        tmp10 = t[0] + t[4];
        tmp11 = t[0] - t[4];
        tmp13 = t[2] + t[6];
        tmp12 = BINK_MUL(BINK_A1, t[2] - t[6]) - tmp13;
        tmp0 = tmp10 + tmp13;
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        // Odd part
        z13 = t[5] + t[3];
        z10 = t[5] - t[3];
        z11 = t[1] + t[7];
        z12 = t[1] - t[7];
        tmp7 = z11 + z13;
        tmp11 = BINK_MUL(BINK_A1, z11 - z13);
        z5 = BINK_MUL(BINK_A3, z10 + z12);
        tmp10 = BINK_MUL(BINK_A2, z12) - z5;
        tmp12 = z5 + BINK_MUL(BINK_A4, z10);
        tmp6 = tmp12 - tmp7;
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        dst[0] = (tmp0 + tmp7 + 0x7F) >> 8;
        dst[7] = (tmp0 - tmp7 + 0x7F) >> 8;
        dst[1] = (tmp1 + tmp6 + 0x7F) >> 8;
        dst[6] = (tmp1 - tmp6 + 0x7F) >> 8;
        dst[2] = (tmp2 + tmp5 + 0x7F) >> 8;
        dst[5] = (tmp2 - tmp5 + 0x7F) >> 8;
        dst[4] = (tmp3 + tmp4 + 0x7F) >> 8;
        dst[3] = (tmp3 - tmp4 + 0x7F) >> 8;
        t += 8;
        dst += stride;
    }
}

// Like fastidct8x8, but draws the block at double size (16x16) into dst.
void fastidct8x8d(u8 *dst, s32 stride, s16 *block, s32 *quant) {
    s32 tmp[64];
    s32 *t;
    s32 i;
    u8 *dst2;
    s32 stride2;
    s32 tmp0;
    s32 tmp1;
    s32 tmp2;
    s32 tmp3;
    s32 tmp4;
    s32 tmp5;
    s32 tmp6;
    s32 tmp7;
    s32 tmp10;
    s32 tmp11;
    s32 tmp12;
    s32 tmp13;
    s32 z5;
    s32 z10;
    s32 z11;
    s32 z12;
    s32 z13;
    dst2 = dst + stride;
    stride2 = stride * 2;
    t = tmp;

    for (i = 0; i < 8; i++) {
        if ((block[8] | block[16] | block[24] | block[32] | block[40] | block[48] | block[56]) == 0) {
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
            tmp0 = (block[0] * quant[0]) >> 11;
            tmp1 = (block[16] * quant[16]) >> 11;
            tmp2 = (block[32] * quant[32]) >> 11;
            tmp3 = (block[48] * quant[48]) >> 11;
            tmp10 = tmp0 + tmp2;
            tmp11 = tmp0 - tmp2;
            tmp13 = tmp1 + tmp3;
            tmp12 = BINK_MUL(BINK_A1, tmp1 - tmp3) - tmp13;
            tmp0 = tmp10 + tmp13;
            tmp3 = tmp10 - tmp13;
            tmp1 = tmp11 + tmp12;
            tmp2 = tmp11 - tmp12;
            tmp4 = (block[8] * quant[8]) >> 11;
            tmp5 = (block[24] * quant[24]) >> 11;
            tmp6 = (block[40] * quant[40]) >> 11;
            tmp7 = (block[56] * quant[56]) >> 11;
            z13 = tmp6 + tmp5;
            z10 = tmp6 - tmp5;
            z11 = tmp4 + tmp7;
            z12 = tmp4 - tmp7;
            tmp7 = z11 + z13;
            tmp11 = BINK_MUL(BINK_A1, z11 - z13);
            z5 = BINK_MUL(BINK_A3, z10 + z12);
            tmp10 = BINK_MUL(BINK_A2, z12) - z5;
            tmp12 = z5 + BINK_MUL(BINK_A4, z10);
            tmp6 = tmp12 - tmp7;
            tmp5 = tmp11 - tmp6;
            tmp4 = tmp10 + tmp5;
            block++;
            quant++;
            t[0] = tmp0 + tmp7;
            t[56] = tmp0 - tmp7;
            t[8] = tmp1 + tmp6;
            t[48] = tmp1 - tmp6;
            t[16] = tmp2 + tmp5;
            t[40] = tmp2 - tmp5;
            t[32] = tmp3 + tmp4;
            t[24] = tmp3 - tmp4;
            t++;
        }
    }
    // Pass 2: rows. Each output pixel is written twice across and the row twice down.
    t = tmp;
    for (i = 0; i < 8; i++) {
        u32 w0, w1, w2, w3;

        // Even part
        tmp10 = t[0] + t[4];
        tmp11 = t[0] - t[4];
        tmp13 = t[2] + t[6];
        tmp12 = BINK_MUL(BINK_A1, t[2] - t[6]) - tmp13;
        tmp0 = tmp10 + tmp13;
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        // Odd part
        z13 = t[5] + t[3];
        z10 = t[5] - t[3];
        z11 = t[1] + t[7];
        z12 = t[1] - t[7];
        tmp7 = z11 + z13;
        tmp11 = BINK_MUL(BINK_A1, z11 - z13);
        z5 = BINK_MUL(BINK_A3, z10 + z12);
        tmp10 = BINK_MUL(BINK_A2, z12) - z5;
        tmp12 = z5 + BINK_MUL(BINK_A4, z10);
        tmp6 = tmp12 - tmp7;
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        // Final output stage in libjpeg's order (pixels 0, 7, 1, 6, 2, 5, 4, 3), as in fastidct8x8: each pixel
        // goes into the word that holds its pair of the row (w0 = pixels 0 1, w1 = 2 3, w2 = 4 5, w3 = 6 7), then
        // every pixel is doubled across. The doublings must follow all eight pixels, w3 before w2 before w1.
        w0 = (u8)((tmp0 + tmp7 + 0x7F) >> 8) << 16;
        w3 = (u8)((tmp0 - tmp7 + 0x7F) >> 8);
        w0 |= (u8)((tmp1 + tmp6 + 0x7F) >> 8);
        w3 |= (u8)((tmp1 - tmp6 + 0x7F) >> 8) << 16;
        w1 = (u8)((tmp2 + tmp5 + 0x7F) >> 8) << 16;
        w2 = (u8)((tmp2 - tmp5 + 0x7F) >> 8);
        w2 |= (u8)((tmp3 + tmp4 + 0x7F) >> 8) << 16;
        w1 |= (u8)((tmp3 - tmp4 + 0x7F) >> 8);
        w3 |= w3 << 8;
        w2 |= w2 << 8;
        w1 |= w1 << 8;
        w0 |= w0 << 8;
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
    fastidct8x8(arg0, (s32)arg1, arg2, (s32 *)((u8 *)&lbl_802F6A40 + (arg3 << 8)));
}

void fn_80125CB4(void *arg0, void *arg1, void *arg2, s32 arg3) {
    fastidct8x8d(arg0, (s32)arg1, arg2, (s32 *)((u8 *)&lbl_802F6A40 + (arg3 << 8)));
}

extern const s32 lbl_802F7A40[];
// Dequantizes (inter table `q`) and inverse-transforms a block, adding the result to the motion source src.
#pragma push
#pragma global_optimizer off  /* same code as the three switches it replaces (lifetimes, propagation, common_subs off); see ExpandPlane */
/* The AAN fast IDCT in the libjpeg jidctfst.c layout: the same work variables serve both passes and are
 * assigned again (tmp10..tmp12 hold the even sums first and the odd products later). RAD built it without
 * lifetime splitting, copy propagation or common-subexpression elimination, so every variable keeps one
 * register for the whole routine (Process review, 2026-11-16). */
void FastmIDCT8x8WithMotion(u8 *dst, s32 stride, s16 *block, s32 q, u8 *src, s32 srcStride) {
    s32 tmp[64];
    s32 i;
    s32 *t;
    s32 *quant;
    s32 s1, s2, s3, s4, s5, s6, s7;
    s32 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    s32 tmp10, tmp11, tmp12, tmp13;
    s32 z5, z10, z11, z12, z13;

    quant = (s32 *)((u8 *)lbl_802F7A40 + (q << 8));
    t = tmp;

    // Pass 1: columns, from the coefficient block into the work array.
    for (i = 8; i != 0; --i) {
        s6 = block[48];
        s5 = block[40];
        s4 = block[32];
        s3 = block[24];
        s7 = block[56];
        s1 = block[8];
        s2 = block[16];
        // The cast keeps CodeWarrior from reassociating the zero-test ORs.
        if (((s7 | (s32)((u32)s6 | (u32)s5)) | ((s4 | s3) | (s1 | s2))) == 0) {
            // All AC terms are zero: the column is the dequantized DC value.
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
            s32 s0 = block[0];

            // Even part
            tmp0 = (s0 * quant[0]) >> 11;
            tmp1 = (s2 * quant[16]) >> 11;
            tmp2 = (s4 * quant[32]) >> 11;
            tmp3 = (s6 * quant[48]) >> 11;
            tmp10 = tmp0 + tmp2;
            tmp11 = tmp0 - tmp2;
            tmp13 = tmp1 + tmp3;
            tmp12 = BINK_MUL(BINK_A1, tmp1 - tmp3) - tmp13;
            tmp0 = tmp10 + tmp13;
            tmp3 = tmp10 - tmp13;
            tmp1 = tmp11 + tmp12;
            tmp2 = tmp11 - tmp12;

            // Odd part
            tmp4 = (s1 * quant[8]) >> 11;
            tmp5 = (s3 * quant[24]) >> 11;
            tmp6 = (s5 * quant[40]) >> 11;
            tmp7 = (s7 * quant[56]) >> 11;
            z13 = tmp6 + tmp5;
            z10 = tmp6 - tmp5;
            z11 = tmp4 + tmp7;
            z12 = tmp4 - tmp7;
            tmp7 = z11 + z13;
            tmp11 = BINK_MUL(BINK_A1, z11 - z13);
            z5 = BINK_MUL(BINK_A3, z10 + z12);
            tmp10 = BINK_MUL(BINK_A2, z12) - z5;
            tmp12 = z5 + BINK_MUL(BINK_A4, z10);
            tmp6 = tmp12 - tmp7;
            tmp5 = tmp11 - tmp6;
            tmp4 = tmp10 + tmp5;

            block++;
            quant++;
            t[0] = tmp0 + tmp7;
            t[56] = tmp0 - tmp7;
            t[8] = tmp1 + tmp6;
            t[48] = tmp1 - tmp6;
            t[16] = tmp2 + tmp5;
            t[40] = tmp2 - tmp5;
            t[32] = tmp3 + tmp4;
            t[24] = tmp3 - tmp4;
            t++;
        }
    }

    // Pass 2: rows, from the work array onto the motion-compensated source.
    t = tmp;
    for (i = 0; i < 8; i++) {
        // Even part
        tmp10 = t[0] + t[4];
        tmp11 = t[0] - t[4];
        tmp13 = t[2] + t[6];
        tmp12 = BINK_MUL(BINK_A1, t[2] - t[6]) - tmp13;
        tmp0 = tmp10 + tmp13;
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;

        // Odd part
        z13 = t[5] + t[3];
        z10 = t[5] - t[3];
        z11 = t[1] + t[7];
        z12 = t[1] - t[7];
        tmp7 = z11 + z13;
        tmp11 = BINK_MUL(BINK_A1, z11 - z13);
        z5 = BINK_MUL(BINK_A3, z10 + z12);
        tmp10 = BINK_MUL(BINK_A2, z12) - z5;
        tmp12 = z5 + BINK_MUL(BINK_A4, z10);
        tmp6 = tmp12 - tmp7;
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;

        dst[0] = src[0] + ((tmp0 + tmp7 + 0x7F) >> 8);
        dst[7] = src[7] + ((tmp0 - tmp7 + 0x7F) >> 8);
        dst[1] = src[1] + ((tmp1 + tmp6 + 0x7F) >> 8);
        dst[6] = src[6] + ((tmp1 - tmp6 + 0x7F) >> 8);
        dst[2] = src[2] + ((tmp2 + tmp5 + 0x7F) >> 8);
        dst[5] = src[5] + ((tmp2 - tmp5 + 0x7F) >> 8);
        dst[3] = src[3] + ((tmp3 - tmp4 + 0x7F) >> 8);
        dst[4] = src[4] + ((tmp3 + tmp4 + 0x7F) >> 8);
        src += srcStride;
        dst += stride;
        t += 8;
    }
}
#pragma pop

// Reads the AC coefficients of an 8x8 DCT block (block[0], the DC, is set by the caller) bit plane by bit plane,
// using the same coefficient list scheme as the residue reader, then stores them in natural order.
void ReadBPLossless(s16 *block, BitReader8011 *bits) {
    u32 planes;
    u32 width;
    u8 *start;
    u32 bit;
    u32 esc;
    u8 *end;
    u8 *p;
    s32 mask;
    u32 *next;
    s32 val;
    s32 i;
    u32 value;
    u32 avail;
    s32 sign;
    s32 low;
    u32 e;
    u32 v;
    u8 list[144];
    s16 tmp[64] = {0};

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
        low = sign - 1;
        p = start;
        while (p < end) {
            e = *p;
            if (e != 0) {
                BINK_GETBIT(bit);
                if (bit & 1) {
                    switch (e & 3) {
                    case 0:
                        e = e >> 2;
                        *p = ((e + 4) << 2) + 1;
                        goto group4;
                    case 1:
                        e = e >> 2;
                        *p = (e << 2) + 2;
                        *end++ = ((e + 4) << 2) + 2;
                        *end++ = ((e + 8) << 2) + 2;
                        *end++ = ((e + 12) << 2) + 2;
                        continue;
                    case 2:
                        *p = 0;
                        e = e >> 2;
                        p++;
                    group4:
                        BINK_GETESC(esc, v, width);
                        if (esc) {
                            *--start = ((e) << 2) + 3;
                        } else {
                            val = mask | (v & low);
                            val = v & sign ? -val : val;
                            tmp[e] = val;
                        }
                        BINK_GETESC(esc, v, width);
                        if (esc) {
                            *--start = ((e + 1) << 2) + 3;
                        } else {
                            val = mask | (v & low);
                            val = v & sign ? -val : val;
                            tmp[e + 1] = val;
                        }
                        BINK_GETESC(esc, v, width);
                        if (esc) {
                            *--start = ((e + 2) << 2) + 3;
                        } else {
                            val = mask | (v & low);
                            val = v & sign ? -val : val;
                            tmp[e + 2] = val;
                        }
                        BINK_GETESC(esc, v, width);
                        if (esc) {
                            *--start = ((e + 3) << 2) + 3;
                        } else {
                            val = mask | (v & low);
                            val = v & sign ? -val : val;
                            tmp[e + 3] = val;
                        }
                        continue;
                    case 3:
                        BINK_GETBITS(v, width);
                        val = mask | (v & low);
                        val = v & sign ? -val : val;
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
                        e = e >> 2;
                        *p = ((e + 4) << 2) + 1;
                        goto group4s;
                    case 1:
                        e = e >> 2;
                        *p = (e << 2) + 2;
                        *end++ = ((e + 4) << 2) + 2;
                        *end++ = ((e + 8) << 2) + 2;
                        *end++ = ((e + 12) << 2) + 2;
                        continue;
                    case 2:
                        *p = 0;
                        e = e >> 2;
                        p++;
                    group4s:
                        BINK_GETBIT(bit);
                        if (bit & 1) {
                            *--start = ((e) << 2) + 3;
                        } else {
                            BINK_GETBIT(bit);
                            tmp[e] = -(bit & 1) | 1;
                        }
                        BINK_GETBIT(bit);
                        if (bit & 1) {
                            *--start = ((e + 1) << 2) + 3;
                        } else {
                            BINK_GETBIT(bit);
                            tmp[e + 1] = -(bit & 1) | 1;
                        }
                        BINK_GETBIT(bit);
                        if (bit & 1) {
                            *--start = ((e + 2) << 2) + 3;
                        } else {
                            BINK_GETBIT(bit);
                            tmp[e + 2] = -(bit & 1) | 1;
                        }
                        BINK_GETBIT(bit);
                        if (bit & 1) {
                            *--start = ((e + 3) << 2) + 3;
                        } else {
                            BINK_GETBIT(bit);
                            tmp[e + 3] = -(bit & 1) | 1;
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
void readlossy(s8* block, BitReader8011* bits, u32 limit) {
    s32 i;
    s32 planes;
    s32 nm;
    s32 k;
    u8* start;
    u32 bit;
    u8* end;
    u8* p;
    s32 mask;
    s32 nzCount;
    u32 n = 0;
    u8* nz_p;
    u32* next;
    u32 value;
    u32 avail;
    u32 e;
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
    start[3] = (0 << 2) | 2;
    end = start + 4;
    mask = 1 << planes;
    nzCount = 0;
    for (k = planes + 1; k != 0; k--) {
        nm = -mask;
        nz_p = nz;
        for (i = 0; i < nzCount; i++) {
            if (avail == 0) {
                bit = *next;
                avail = 31;
                next++;
                value = bit >> 1;
            } else {
                bit = value;
                value >>= 1;
                avail--;
            }
            ;
            if (bit & 1) {
                block[*nz_p] += (block[*nz_p] < 0) ? nm : (mask);
                if ((n++) == limit) {
                    goto done;
                }
            }
            nz_p++;
        }

        p = start;
        while (p < end) {
            e = *p;
            if (e != 0) {
                if (avail == 0) {
                    bit = *next;
                    avail = 31;
                    next++;
                    value = bit >> 1;
                } else {
                    bit = value;
                    value >>= 1;
                    avail--;
                }
                ;
                if (bit & 1) {
                    switch (e & 3) {
                        case 0:
                            e = e >> 2;
                            *p = ((e + 4) << 2) + 1;
                            goto group4;

                        case 1:
                            e = e >> 2;
                            *p = (e << 2) + 2;
                            *end++ = ((e + 4) << 2) + 2;
                            *end++ = ((e + 8) << 2) + 2;
                            *end++ = ((e + 12) << 2) + 2;
                            continue;

                        case 2:
                            *p = 0;
                            e = e >> 2;
                            p++;
                            group4:
                        if (avail == 0) {
                            bit = *next;
                            avail = 31;
                            next++;
                            value = bit >> 1;
                        } else {
                            bit = value;
                            value >>= 1;
                            avail--;
                        }

                            ;
                            if (bit & 1) {
                            *(--start) = (e << 2) + 3;
                        } else {
                            nz[nzCount++] = e;
                            if (avail == 0) {
                                bit = *next;
                                avail = 31;
                                next++;
                                value = bit >> 1;
                            } else {
                                bit = value;
                                value >>= 1;
                                avail--;
                            }
                            ;
                            block[e] = (bit & 1) ? nm : (mask);
                            if ((n++) == limit) {
                                goto done;
                            }
                        }
                            if (avail == 0) {
                            bit = *next;
                            avail = 31;
                            next++;
                            value = bit >> 1;
                        } else {
                            bit = value;
                            value >>= 1;
                            avail--;
                        }
                            ;
                            if (bit & 1) {
                            *(--start) = ((e + 1) << 2) + 3;
                        } else {
                            nz[nzCount++] = e + 1;
                            if (avail == 0) {
                                bit = *next;
                                avail = 31;
                                next++;
                                value = bit >> 1;
                            } else {
                                bit = value;
                                value >>= 1;
                                avail--;
                            }
                            ;
                            block[e + 1] = (bit & 1) ? nm : (mask);
                            if ((n++) == limit) {
                                goto done;
                            }
                        }
                            if (avail == 0) {
                            bit = *next;
                            avail = 31;
                            next++;
                            value = bit >> 1;
                        } else {
                            bit = value;
                            value >>= 1;
                            avail--;
                        }
                            ;
                            if (bit & 1) {
                            *(--start) = ((e + 2) << 2) + 3;
                        } else {
                            nz[nzCount++] = e + 2;
                            if (avail == 0) {
                                bit = *next;
                                avail = 31;
                                next++;
                                value = bit >> 1;
                            } else {
                                bit = value;
                                value >>= 1;
                                avail--;
                            }
                            ;
                            block[e + 2] = (bit & 1) ? nm : (mask);
                            if ((n++) == limit) {
                                goto done;
                            }
                        }
                            if (avail == 0) {
                            bit = *next;
                            avail = 31;
                            next++;
                            value = bit >> 1;
                        } else {
                            bit = value;
                            value >>= 1;
                            avail--;
                        }
                            ;
                            if (bit & 1) {
                            *(--start) = ((e + 3) << 2) + 3;
                        } else {
                            nz[nzCount++] = e + 3;
                            if (avail == 0) {
                                bit = *next;
                                avail = 31;
                                next++;
                                value = bit >> 1;
                            } else {
                                bit = value;
                                value >>= 1;
                                avail--;
                            }
                            ;
                            block[e + 3] = (bit & 1) ? nm : (mask);
                            if ((n++) != limit) {
                                continue;
                            }
                            goto done;
                        }
                            continue;

                        case 3:
                            e = e >> 2;
                            nz[nzCount++] = e;
                            if (avail == 0) {
                            bit = *next;
                            avail = 31;
                            next++;
                            value = bit >> 1;
                        } else {
                            bit = value;
                            value >>= 1;
                            avail--;
                        }
                            ;
                            block[e] = (bit & 1) ? nm : (mask);
                            if ((n++) == limit) {
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
void ReadBPLossyWithMotion(u8 *dst, s32 dstStride, BitReader8011 *bits, u32 limit, u8 *src, s32 srcStride) {
    s8 block[64] = {0};
    u8 *row = dst;
    u8 *ref = src;

    readlossy(block, bits, limit);
    row[0] = block[0] + ref[0];
    row[1] = block[1] + ref[1];
    row[2] = block[4] + ref[2];
    row[3] = block[5] + ref[3];
    row[4] = block[8] + ref[4];
    row[5] = block[9] + ref[5];
    row[6] = block[12] + ref[6];
    row[7] = block[13] + ref[7];
    row += dstStride;
    ref += srcStride;
    row[0] = block[2] + ref[0];
    row[1] = block[3] + ref[1];
    row[2] = block[6] + ref[2];
    row[3] = block[7] + ref[3];
    row[4] = block[10] + ref[4];
    row[5] = block[11] + ref[5];
    row[6] = block[14] + ref[6];
    row[7] = block[15] + ref[7];
    row += dstStride;
    ref += srcStride;
    row[0] = block[24] + ref[0];
    row[1] = block[25] + ref[1];
    row[2] = block[44] + ref[2];
    row[3] = block[45] + ref[3];
    row[4] = block[16] + ref[4];
    row[5] = block[17] + ref[5];
    row[6] = block[20] + ref[6];
    row[7] = block[21] + ref[7];
    row += dstStride;
    ref += srcStride;
    row[0] = block[26] + ref[0];
    row[1] = block[27] + ref[1];
    row[2] = block[46] + ref[2];
    row[3] = block[47] + ref[3];
    row[4] = block[18] + ref[4];
    row[5] = block[19] + ref[5];
    row[6] = block[22] + ref[6];
    row[7] = block[23] + ref[7];
    row += dstStride;
    ref += srcStride;
    row[0] = block[28] + ref[0];
    row[1] = block[29] + ref[1];
    row[2] = block[32] + ref[2];
    row[3] = block[33] + ref[3];
    row[4] = block[48] + ref[4];
    row[5] = block[49] + ref[5];
    row[6] = block[52] + ref[6];
    row[7] = block[53] + ref[7];
    row += dstStride;
    ref += srcStride;
    row[0] = block[30] + ref[0];
    row[1] = block[31] + ref[1];
    row[2] = block[34] + ref[2];
    row[3] = block[35] + ref[3];
    row[4] = block[50] + ref[4];
    row[5] = block[51] + ref[5];
    row[6] = block[54] + ref[6];
    row[7] = block[55] + ref[7];
    row += dstStride;
    ref += srcStride;
    row[0] = block[36] + ref[0];
    row[1] = block[37] + ref[1];
    row[2] = block[40] + ref[2];
    row[3] = block[41] + ref[3];
    row[4] = block[56] + ref[4];
    row[5] = block[57] + ref[5];
    row[6] = block[60] + ref[6];
    row[7] = block[61] + ref[7];
    row += dstStride;
    ref += srcStride;
    row[0] = block[38] + ref[0];
    row[1] = block[39] + ref[1];
    row[2] = block[42] + ref[2];
    row[3] = block[43] + ref[3];
    row[4] = block[58] + ref[4];
    row[5] = block[59] + ref[5];
    row[6] = block[62] + ref[6];
    row[7] = block[63] + ref[7];
}

extern f64 pow(f64, f64);
extern void set_rate_volume_and_pan(void *);
extern void clear(void *, u32, u32);
extern void set_both_state(void *, s32);
extern void set_addrs(void *, s32);

/* .sdata owned by this unit (0x80559948-0x80559954), in address order. */
extern u8 lbl_8041D628[0x80];
extern u8 lbl_8041D6A8[0x80];
void ***lbl_80559948 = (void ***)lbl_8041D6A8;
u32 *lbl_8055994C = (u32 *)lbl_8041D628;
u32 lbl_80559950 = 0x20;

/* .rodata tables owned by this unit (0x802F65C0-0x802F8E40): the Huffman and mask tables are defined above ExpandPlane
   (they must be: retail addresses them from the start of this unit's .rodata); the IDCT and scan tables follow here. */
// DUMP_DATA lbl_802F6A40 s32
const s32 lbl_802F6A40[1024] = {
#include "dumpdata/lbl_802F6A40.inc"
};
// DUMP_DATA lbl_802F7A40 s32
const s32 lbl_802F7A40[1024] = {
#include "dumpdata/lbl_802F7A40.inc"
};
// DUMP_DATA lbl_802F8A40 u8
const u8 lbl_802F8A40[16][64] = {
#include "dumpdata/lbl_802F8A40.inc"
};

/* .bss owned by this unit (0x8041D628-0x8041D728), in REVERSE address order: this C unit emits globals last-defined first. */
u8 lbl_8041D6A8[0x80];
u8 lbl_8041D628[0x80];

/* .sbss owned by this unit (0x8055DEF0-0x8055DF00), in REVERSE address order: this C unit emits globals last-defined first. */
void *lbl_8055DEFC;
void *lbl_8055DEF8;
u32 lbl_8055DEF4;
u32 lbl_8055DEF0;
