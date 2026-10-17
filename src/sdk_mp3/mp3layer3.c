/* Nintendo-supplied SDK MP3 (Layer III) decoder, 0x800BC538-0x800C6204 in REXE01 (the same tables ship in
 * the Wii Photo Channel). Library/module and file names unknown. It is three translation units: MWCC addresses
 * pooled tables from the start of each unit's .rodata, and the original does so from 0x802F34E8 (dequantizer)
 * and 0x802F3D60 (Huffman decoder), which puts unit boundaries at text 0x800BF048 and 0x800C0440.
 * Build: cflags_rvl_tp, GC/3.0a3, plus -use_lmw_stmw on -inline noauto (every function saves with stmw;
 * WENCGetEncodeData right after the decoder does not, so it is a separate object). */
/* Unit 3 of 3: Huffman decoding, IMDCT, synthesis, side info, decoder allocation (0x800C0440-0x800C6204). */
#include "mp3dec.h"
#include "mp3layer3_tables.inc"

static u32 lbl_8055DD90;  /* arena end */
static u32 lbl_8055DD94;  /* arena current */

static inline s32 MP3_GetSign(BitStream* bs, u8* buf) {
    u32 p = bs->pos;
    s32 avail;

    bs->pos = p + 1;
    avail = 8 - (p & 7);
    return (buf[(p >> 3) & 0x1FFF] >> (avail - 1)) & 1;
}

s32 MP3_DecodeBigValues(HuffState* d, u8* buf, s16* out, s32 big_values, u32 block_flags, s32 region0,
                        s32 region1, s16 count1_table, s32 track, s16* table_select, s16* zero_start) {
    s32 nregions;
    s16* q;
    s32 mixed = block_flags & 1;
    s32 c1end;
    s32 end;
    s16 region1_end;
    s32 limit;
    s16 region2_end;
    s32 i;
    s32 region;

    if (mixed) {
        region1_end = lbl_802F3D60[d->lsf][d->sr_index][region0];
        region2_end = lbl_802F3E20[d->lsf][d->sr_index][region0 + region1];
    } else if (block_flags & 0x10) {
        region1_end = 36;
    } else {
        region1_end = lbl_80559270[d->lsf];
    }
    limit = big_values * 2;
    i = 0;
    q = out;
    nregions = 2 + (mixed != 0);
    for (region = 0; region < nregions; region++, table_select++) {
        HuffTable* ht;
        s32 tbl = *table_select;
        s32 linbits = lbl_8031C298[tbl];

        if (tbl > 23) {
            tbl = 17;
        } else if (tbl > 15) {
            tbl = 16;
        }
        if (region == 0) {
            end = region1_end;
        }
        if (mixed) {
            if (region == 1) {
                end = region2_end;
            }
            if (region == 2) {
                end = limit;
            }
        } else if (region == 1) {
            end = limit;
        }
        if (end > limit) {
            end = limit;
        }
        ht = &lbl_8031D580[tbl];
        for (; i < end; i += 2, q += 2) {
            s16 x;
            s16 y;
            s32 r;
            s32 bits = MP3_PeekBitsRing(buf, 16, &d->pos) << 16;

            if (tbl == 13 || tbl == 16) {
                u32 b1;
                u32 b0;
                u32 p = d->pos;
                s32 sh = 16 - (p & 7);
                u32 idx = (p >> 3) & 0x1FFF;

                d->pos = p + 4;
                b0 = buf[idx++];
                idx &= 0x1FFF;
                b1 = buf[idx];
                d->pos -= 20;
                bits |= ((((b0 << 8) | b1) & lbl_802F3F54[sh]) >> (sh - 4) & 0xF) << 12;
            } else {
                d->pos -= 16;
            }
            r = MP3_HuffLookup(bits, lbl_802F3F30[tbl], ht);
            x = (r & 0xF0) >> 4;
            y = r & 0xF;
            d->pos += (s16)r >> 8;
            if (x == 15 && linbits != 0) {
                x += (u16)MP3_GetBitsRing(buf, linbits, &d->pos);
            }
            if (x && MP3_GetSign((BitStream*)d, buf)) {
                x = -x;
            }
            if (y == 15 && linbits != 0) {
                y += (u16)MP3_GetBitsRing(buf, linbits, &d->pos);
            }
            if (y && MP3_GetSign((BitStream*)d, buf)) {
                y = -y;
            }
            q[0] = x;
            q[1] = y;
            if ((s32)d->pos > (s32)d->end) {
                if (track == 1 && *zero_start == 0) {
                    *zero_start = i;
                }
                q[0] = 0;
                q[1] = 0;
            }
        }
    }
    c1end = d->count1_end;
    if (count1_table == 0) {
        MP3_DecodeQuads((BitStream*)d, c1end, buf, out, i, track, zero_start);
    } else {
        MP3_DecodeQuadsB((BitStream*)d, c1end, buf, out, i, track, zero_start);
    }
    return 0;
}

u32 MP3_HuffLookup(s32 bits, s32 n, HuffTable* t) {
    s32 node = 0;
    u32 mask = 0xF;
    s32 shift = 28;

    while (n-- > 0) {
        u16 e = t->nodes[node][(bits >> shift) & mask];
        if (e & 0x8000) {
            return e & 0x7FFF;
        }
        mask = lbl_8031C2B8[e & 0xF];
        node = (e >> 8) & 0xFF;
        shift -= e & 0xF;
    }
    return 0;
}

void MP3_DecodeQuads(BitStream* bs, s32 end, u8* buf, s16* out, s32 i, s32 track, s16* zero_start) {
    s16* q = out + i;

    for (; i < end; q += 4, i += 4) {
        u32 code;
        u32 p = bs->pos;
        s16 v;
        u32 b0;
        s16 w;
        s16 x;
        s32 sh;
        u32 b1;
        u32 idx;
        u8 e;
        s16 y;

        bs->pos = p + 6;
        idx = (p >> 3) & 0x1FFF;
        sh = 16 - (p & 7);
        b0 = buf[idx++];
        idx &= 0x1FFF;
        b1 = buf[idx];
        code = (u16)((((b0 << 8) | b1) & lbl_802F3F54[sh]) >> (sh - 6));
        e = lbl_8031C2E0[code];
        bs->pos = p + 6 - (e >> 4);
        v = (e >> 3) & 1;
        w = (e >> 2) & 1;
        x = (e >> 1) & 1;
        y = e & 1;
        if (v && MP3_GetSign(bs, buf)) {
            v = -v;
        }
        if (w && MP3_GetSign(bs, buf)) {
            w = -w;
        }
        if (x && MP3_GetSign(bs, buf)) {
            x = -x;
        }
        if (y && MP3_GetSign(bs, buf)) {
            y = -y;
        }
        q[0] = v;
        q[1] = w;
        q[2] = x;
        q[3] = y;
        if ((s32)bs->pos > (s32)bs->end) {
            if (track == 1 && *zero_start == 0) {
                *zero_start = i;
            }
            for (; i < end; i += 4) {
                out[i] = 0;
                out[i + 1] = 0;
                out[i + 2] = 0;
                out[i + 3] = 0;
            }
            return;
        }
    }
}

void MP3_DecodeQuadsB(BitStream* bs, s32 end, u8* buf, s16* out, s32 i, s32 track, s16* zero_start) {
    s16* q = out + i;

    for (; i < end; q += 4, i += 4) {
        u32 e = ~MP3_GetBitsRing(buf, 4, &bs->pos);
        s16 v = (e >> 3) & 1;
        s16 w = (e >> 2) & 1;
        s16 x = (e >> 1) & 1;
        s16 y = e & 1;

        if (v && MP3_GetSign(bs, buf)) {
            v = -v;
        }
        if (w && MP3_GetSign(bs, buf)) {
            w = -w;
        }
        if (x && MP3_GetSign(bs, buf)) {
            x = -x;
        }
        if (y && MP3_GetSign(bs, buf)) {
            y = -y;
        }
        q[0] = v;
        q[1] = w;
        q[2] = x;
        q[3] = y;
        if ((s32)bs->pos > (s32)bs->end) {
            if (track == 1 && *zero_start == 0) {
                *zero_start = i;
            }
            for (; i < end; i += 4) {
                out[i] = 0;
                out[i + 1] = 0;
                out[i + 2] = 0;
                out[i + 3] = 0;
            }
            return;
        }
    }
}

u32 MP3_PeekBitsRing(u8* buf, u32 n, u32* pos) {
    u32 idx = (*pos >> 3) & 0x1FFF;
    s32 sh = 24 - (*pos & 7);
    u32 b0;
    u32 b1;
    u32 b2;

    *pos += n;
    if (idx < 0x1FFE) {
        b0 = buf[idx];
        b1 = buf[++idx];
        b2 = buf[++idx];
    } else {
        b0 = buf[idx];
        idx = (idx + 1) & 0x1FFF;
        b1 = buf[idx];
        idx = (idx + 1) & 0x1FFF;
        b2 = buf[idx];
    }
    return (u16)((((b0 << 16) | (b1 << 8) | b2) & lbl_802F3F54[sh]) >> (sh - n));
}

/* IMDCT stage dispatcher: window stage, then the butterfly stage on win + 1 and src advanced by 4 bytes
 * (mode 0) or 2 bytes. win/src are byte-addressed in the original (addi 1 / 4 / 2). */
void fn_800C34B4(s32* dst, u8* src, s32 win, s32 mode) {
    if (mode == 0) {
        fn_800C3960(dst, lbl_802F4088, win, src, 0x3ff, mode);
        fn_800C3574(dst, lbl_802F4108, win + 1, src + 4, mode);
    } else {
        fn_800C3960(dst, lbl_802F4088, win, src, 0x3ff, mode);
        fn_800C3574(dst, lbl_802F4108, win + 1, src + 2, mode);
    }
}

MP3Decoder* MP3_Create(u8* header, u32 b, s32 buf, s32 size) {
    MP3Decoder* d;

    MP3_ArenaInit(buf, size);
    d = MP3_ArenaAlloc(sizeof(MP3Decoder));
    MP3_Fill((u32*)d, 0, sizeof(MP3Decoder));
    d->lsf = 0;
    d->unk8 = 0;
    d->unkC = 0;
    d->unk14 = 0;
    d->unk18 = 0;
    d->unk9CC[0] = 0;
    d->unk9CC[1] = 0;
    d->unk9CC[2] = 0;
    d->unk9CC[3] = 0;
    d->unk9CC[4] = 0;
    d->unk9CC[5] = 0;
    d->unk9CC[6] = 0;
    d->unk9CC[7] = 0;
    d->unk27C = 0x400;
    d->unk278 = b;
    MP3_ParseHeader(d, header);
    return d;
}

/* Frees a decoder: the arena is a bump allocator, so resetting it to the decoder's address releases it. */
void MP3_Delete(u32 p) {
    MP3_ArenaSet(p);
}

void MP3_ArenaInit(s32 base, s32 size) {
    lbl_8055DD94 = base + (4 - base % 4);
    lbl_8055DD90 = base + size - 4;
}

void* MP3_ArenaAlloc(u32 size) {
    u32 old = lbl_8055DD94;

    lbl_8055DD94 = old + (size & ~3) + ((size & 3) != 0) * 4;
    if (lbl_8055DD94 > lbl_8055DD90) {
        lbl_8055DD94 = old;
        return 0;
    }
    return (void*)old;
}

void MP3_ArenaSet(u32 p) {
    lbl_8055DD94 = p;
}

void MP3_Fill(u32* p, u32 value, u32 size) {
    u32 n16 = size >> 4;
    u32 n4 = (size >> 2) & 3;

    while (n16-- != 0) {
        p[0] = value;
        p[1] = value;
        p[2] = value;
        p[3] = value;
        p += 4;
    }
    while (n4-- != 0) {
        *p++ = value;
    }
}

/* Q24 butterfly stage: words 0-3 of four 8-word blocks, coefficients c[0..3].
 * The last step keeps d - s in its own variable, the others reuse s: that is what reproduces the
 * original's register pairs (same in fn_800C5430/5508/55D8). */
void fn_800C5430(s32* dst, s32* src, s32* c) {
    s32 m0 = c[0];
    s32 m1 = c[1];
    s32 m2 = c[2];
    s32 m3 = c[3];
    s32 i;
    s32 s, d, t;

    for (i = 0; i < 4; i++) {
        s = src[0]; d = dst[0]; dst[0] = d + s; s = d - s; src[0] = (s32)(((s64)s * m0) >> 24);
        s = src[1]; d = dst[1]; dst[1] = d + s; s = d - s; src[1] = (s32)(((s64)s * m1) >> 24);
        s = src[2]; d = dst[2]; dst[2] = d + s; s = d - s; src[2] = (s32)(((s64)s * m2) >> 24);
        s = src[3]; d = dst[3]; t = d - s; dst[3] = d + s; src[3] = (s32)(((s64)t * m3) >> 24);
        dst += 8;
        src += 8;
    }
}

/* Q24 butterfly stage: words 0,1,4,5 of four 8-word blocks, coefficients c[0], c[1].
 * The last step keeps d - s in its own variable, the others reuse s: that is what reproduces the
 * original's register pairs (same in fn_800C5430/5508/55D8). */
void fn_800C5508(s32* dst, s32* src, s32* c) {
    s32 m0 = c[0];
    s32 m1 = c[1];
    s32 i;
    s32 s, d, t;

    for (i = 0; i < 4; i++) {
        s = src[0]; d = dst[0]; dst[0] = d + s; s = d - s; src[0] = (s32)(((s64)s * m0) >> 24);
        s = src[1]; d = dst[1]; dst[1] = d + s; s = d - s; src[1] = (s32)(((s64)s * m1) >> 24);
        s = src[4]; d = dst[4]; dst[4] = d + s; s = d - s; src[4] = (s32)(((s64)s * m0) >> 24);
        s = src[5]; d = dst[5]; t = d - s; dst[5] = d + s; src[5] = (s32)(((s64)t * m1) >> 24);
        dst += 8;
        src += 8;
    }
}

/* Q24 butterfly stage: even words of four 8-word blocks, coefficient *c.
 * The last step keeps d - s in its own variable, the others reuse s: that is what reproduces the
 * original's register pairs (same in fn_800C5430/5508/55D8). */
void fn_800C55D8(s32* dst, s32* src, s32* c) {
    s32 m = *c;
    s32 i;
    s32 s, d, t;

    for (i = 0; i < 4; i++) {
        s = src[0]; d = dst[0]; dst[0] = d + s; s = d - s; src[0] = (s32)(((s64)s * m) >> 24);
        s = src[2]; d = dst[2]; dst[2] = d + s; s = d - s; src[2] = (s32)(((s64)s * m) >> 24);
        s = src[4]; d = dst[4]; dst[4] = d + s; s = d - s; src[4] = (s32)(((s64)s * m) >> 24);
        s = src[6]; d = dst[6]; t = d - s; dst[6] = d + s; src[6] = (s32)(((s64)t * m) >> 24);
        dst += 8;
        src += 8;
    }
}

/* Reorders 16 words (and, unless mode == 1, the next 16) into the synthesis input order. src is not const:
 * the original reloads after every store, as when the compiler cannot rule out aliasing. */
void MP3_Reorder32(u32* dst, u32* src, s32 mode) {
    dst[ 0] = src[ 0];
    dst[ 1] = src[ 1];
    dst[ 2] = src[ 3];
    dst[ 3] = src[ 2];
    dst[ 4] = src[ 7];
    dst[ 5] = src[ 6];
    dst[ 6] = src[ 4];
    dst[ 7] = src[ 5];
    dst[ 8] = src[15];
    dst[ 9] = src[14];
    dst[10] = src[12];
    dst[11] = src[13];
    dst[12] = src[ 8];
    dst[13] = src[ 9];
    dst[14] = src[11];
    dst[15] = src[10];
    if (mode == 1) {
        return;
    }
    dst[16] = src[31];
    dst[17] = src[30];
    dst[18] = src[28];
    dst[19] = src[29];
    dst[20] = src[24];
    dst[21] = src[25];
    dst[22] = src[27];
    dst[23] = src[26];
    dst[24] = src[16];
    dst[25] = src[17];
    dst[26] = src[19];
    dst[27] = src[18];
    dst[28] = src[23];
    dst[29] = src[22];
    dst[30] = src[20];
    dst[31] = src[21];
}
