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

/* 18-point IMDCT front end: fn_800C1984 into a stack buffer, then out[0..8] = tmp[9..17] and
 * out[18..26] = -tmp[8..0]. */
void fn_800C18A0(s32* in, s32* out) {
    s32 tmp[18];

    fn_800C1984(in, tmp);
    out[0] = tmp[9];
    out[1] = tmp[10];
    out[2] = tmp[11];
    out[3] = tmp[12];
    out[4] = tmp[13];
    out[5] = tmp[14];
    out[6] = tmp[15];
    out[7] = tmp[16];
    out[8] = tmp[17];
    out[18] = -tmp[8];
    out[19] = -tmp[7];
    out[20] = -tmp[6];
    out[21] = -tmp[5];
    out[22] = -tmp[4];
    out[23] = -tmp[3];
    out[24] = -tmp[2];
    out[25] = -tmp[1];
    out[26] = -tmp[0];
}

/* 9 Q20 butterflies (in[i] with in[17 - i]) into sum/difference halves for fn_800C1A80.
 * NonMatching: only the order of the first two loads differs. */
void fn_800C1984(s32* in, s32* out) {
    s32 a[9];
    s32 b[9];
    s32 i;
    s32 p, q;

    for (i = 0; i < 9; i++) {
        p = (s32)(((s64)lbl_802F4018[i] * in[i]) >> 20);
        q = (s32)(((s64)in[17 - i] * lbl_802F4018[17 - i]) >> 20);
        a[i] = p + q;
        p = p - q;
        b[i] = (s32)(((s64)p * lbl_802F4060[i]) >> 20);
    }
    fn_800C1A80(a, b, out);
}

/* IMDCT stage: fn_800C1C1C on both halves, then the interleaved recombination (odd outputs from the
 * running differences of tmp2). From DeepSeek's draft, with out[1] computed from out[0]. */
void fn_800C1A80(s32* a, s32* b, s32* out);
void fn_800C1C1C(s32* in, s32* out);
void fn_800C1A80(s32* a, s32* b, s32* out) {
    s32 tmp1[9];
    s32 tmp2[9];

    fn_800C1C1C(a, tmp1);
    fn_800C1C1C(b, tmp2);

    out[0] = tmp1[0];
    out[1] = tmp2[0] - out[0];
    out[2] = tmp1[1] - out[1];
    tmp2[1] = tmp2[1] - tmp2[0];
    out[3] = tmp2[1] - out[2];
    out[4] = tmp1[2] - out[3];
    tmp2[2] = tmp2[2] - tmp2[1];
    out[5] = tmp2[2] - out[4];
    out[6] = tmp1[3] - out[5];
    tmp2[3] = tmp2[3] - tmp2[2];
    out[7] = tmp2[3] - out[6];
    out[8] = tmp1[4] - out[7];
    tmp2[4] = tmp2[4] - tmp2[3];
    out[9] = tmp2[4] - out[8];
    out[10] = tmp1[5] - out[9];
    tmp2[5] = tmp2[5] - tmp2[4];
    out[11] = tmp2[5] - out[10];
    out[12] = tmp1[6] - out[11];
    tmp2[6] = tmp2[6] - tmp2[5];
    out[13] = tmp2[6] - out[12];
    out[14] = tmp1[7] - out[13];
    tmp2[7] = tmp2[7] - tmp2[6];
    out[15] = tmp2[7] - out[14];
    out[16] = tmp1[8] - out[15];
    tmp2[8] = tmp2[8] - tmp2[7];
    out[17] = tmp2[8] - out[16];
}

/* 9-point IMDCT core. Formulas derived from a symbolic trace of the original and checked against a PPC
 * simulation of it on random inputs (200/200 identical). Products are 64 x 64-bit: the constants are
 * long long. NonMatching: 155/155 with the same opcodes, scheduled differently. */
#define MP3_Q20(x, k) ((s32)(((s64)(x) * (k)) >> 20))
void fn_800C1C1C(s32* in, s32* out) {
    s32 s08, s26, s35, s17, d17, d62, d80, d35, t, u, m, x4;
    s32 a, b, c, d, e, f, g, h;

    d17 = in[1] - in[7];
    s08 = in[0] + in[8];
    s26 = in[2] + in[6];
    s35 = in[3] + in[5];
    t = s26 + s08;
    d62 = in[6] - in[2];
    d80 = in[8] - in[0];
    s17 = in[1] + in[7];
    u = s35 + t;
    d35 = in[3] - in[5];
    x4 = in[4] + s17;

    a = MP3_Q20(d17, -1816186LL);
    b = MP3_Q20(s26 - s08, 1970683LL);
    c = MP3_Q20(s35 - s08, -364166LL);
    d = MP3_Q20(s35 - s26, -1606512LL);
    e = MP3_Q20(d62 + d80, -717267LL);
    f = MP3_Q20(d35 - d80, -2065296LL);
    g = MP3_Q20(d35 + d62, -1348028LL);
    h = MP3_Q20(d35 - d62 + d80, -1816186LL);

    m = in[4] * 2 - s17;
    out[0] = x4 + u;
    out[2] = d - (m + b);
    out[6] = x4 * -2 + u;
    out[8] = m + (c + d);
    out[4] = m - b - c;
    out[5] = a - e - f;
    out[7] = a + (e + g);
    out[1] = g - (a + f);
    out[3] = h;
}

#undef MP3_Q20

/* 6-tap Q20 dot product over four 6-sample groups; outputs at word offsets 0, 2, 6, 8 (strides 2 and 4).
 * From DeepSeek's draft (accumulator starting at 0, ChatGPT's variant B). NonMatching: same opcodes and size,
 * but the original adds each low product word (addc) before computing its high word (mulhw). */
void fn_800C1E88(s32* coeff, s32* input, s32* output) {
    s32 n;
    s32 c0 = coeff[0];
    s32 c1 = coeff[1];
    s32 c2 = coeff[2];
    s32 c3 = coeff[3];

    for (n = 0; n < 4; n++) {
        s64 acc;

        acc = 0;
        acc += (s64)c0 * input[0];
        acc += (s64)c1 * input[1];
        acc += (s64)c2 * input[2];
        acc += (s64)c3 * input[3];
        acc += (s64)coeff[4] * input[4];
        acc += (s64)coeff[5] * input[5];

        *output = (s32)(acc >> 20);

        input += 6;
        if (n & 1) {
            output += 4;
        } else {
            output += 2;
        }
    }
}

/* Sums 18 pairs into every 64th word (stride 0x100 bytes). */
void fn_800C1F6C(s32* dst, s32* a, s32* b) {
    dst[0] = a[0] + b[0];
    dst[64] = a[1] + b[1];
    dst[128] = a[2] + b[2];
    dst[192] = a[3] + b[3];
    dst[256] = a[4] + b[4];
    dst[320] = a[5] + b[5];
    dst[384] = a[6] + b[6];
    dst[448] = a[7] + b[7];
    dst[512] = a[8] + b[8];
    dst[576] = a[9] + b[9];
    dst[640] = a[10] + b[10];
    dst[704] = a[11] + b[11];
    dst[768] = a[12] + b[12];
    dst[832] = a[13] + b[13];
    dst[896] = a[14] + b[14];
    dst[960] = a[15] + b[15];
    dst[1024] = a[16] + b[16];
    dst[1088] = a[17] + b[17];
}

/* Like fn_800C1F6C, but odd entries are negated. */
void fn_800C2090(s32* dst, s32* a, s32* b) {
    dst[0] = a[0] + b[0];
    dst[64] = -(a[1] + b[1]);
    dst[128] = a[2] + b[2];
    dst[192] = -(a[3] + b[3]);
    dst[256] = a[4] + b[4];
    dst[320] = -(a[5] + b[5]);
    dst[384] = a[6] + b[6];
    dst[448] = -(a[7] + b[7]);
    dst[512] = a[8] + b[8];
    dst[576] = -(a[9] + b[9]);
    dst[640] = a[10] + b[10];
    dst[704] = -(a[11] + b[11]);
    dst[768] = a[12] + b[12];
    dst[832] = -(a[13] + b[13]);
    dst[896] = a[14] + b[14];
    dst[960] = -(a[15] + b[15]);
    dst[1024] = a[16] + b[16];
    dst[1088] = -(a[17] + b[17]);
}

/* IMDCT output, first window half: copies prev[0..5] and writes prev[17-i] - cur[i] backwards from out2;
 * the last three use Q20 window coefficients. Math verified against a simulation of the original.
 * NonMatching: 96/100 (the original keeps both products live and spills r31). */
void fn_800C2A88(s32* cur, s32* prev, s32* out1, s32* out2) {
    s32* w = lbl_8031D700;
    s32 i;
    s32 x, c0, c1, y0, y1;

    for (i = 0; i < 6; i++) {
        x = -cur[i];
        out1[i * 64] = prev[i];
        out2[-i * 64] = x + prev[17 - i];
    }
    x = cur[6];
    c0 = w[0];
    c1 = w[1];
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[384] = y0 + prev[6];
    out2[-384] = y1 + prev[11];
    x = cur[7];
    c0 = w[2];
    c1 = w[3];
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[448] = y0 + prev[7];
    out2[-448] = y1 + prev[10];
    x = cur[8];
    c0 = w[4];
    c1 = w[5];
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[512] = y0 + prev[8];
    out2[-512] = y1 + prev[9];
}

/* Second window half: like fn_800C2A88 with lbl_8031D718 and alternating signs (verified 100/100 against
 * a simulation of the original). NonMatching: 105/105, registers/scheduling. */
void fn_800C2C18(s32* cur, s32* prev, s32* out1, s32* out2) {
    s32* w = lbl_8031D718;
    s32 i;
    s32 x, c0, c1;

    x = -cur[0];
    out1[0] = prev[0];
    out2[0] = -(x + prev[17]);
    x = -cur[1];
    out1[64] = -prev[1];
    out2[-64] = x + prev[16];
    x = -cur[2];
    out1[128] = prev[2];
    out2[-128] = -(x + prev[15]);
    x = -cur[3];
    out1[192] = -prev[3];
    out2[-192] = x + prev[14];
    x = -cur[4];
    out1[256] = prev[4];
    out2[-256] = -(x + prev[13]);
    x = -cur[5];
    out1[320] = -prev[5];
    out2[-320] = x + prev[12];
    x = cur[6];
    c0 = w[0];
    c1 = w[1];
    out1[384] = ((s32)(((s64)x * c0) >> 20) + prev[6]);
    out2[-384] = -((s32)(((s64)x * c1) >> 20) + prev[11]);
    x = cur[7];
    c0 = w[2];
    c1 = w[3];
    out1[448] = -((s32)(((s64)x * c0) >> 20) + prev[7]);
    out2[-448] = ((s32)(((s64)x * c1) >> 20) + prev[10]);
    x = cur[8];
    c0 = w[4];
    c1 = w[5];
    out1[512] = ((s32)(((s64)x * c0) >> 20) + prev[8]);
    out2[-512] = -((s32)(((s64)x * c1) >> 20) + prev[9]);
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

/* Synthesis for one granule: 18 steps; each moves the window position (decoder +0x27C) back by 0x40
 * (wrapping to 0x3C0), runs fn_800C4E5C per channel, then fn_800C34B4 per output channel (stereo
 * interleaved when mode == 0, else planar with the second channel n samples later).
 * NonMatching: 90/90, the original keeps byte offsets as loop variables where ours keeps pointers. */
void fn_800C4CF4(MP3Decoder* d, s32* buf, s32* vb, s16* out, s8 channels, s32 mode) {
    s32 k;
    s8 ch;
    s32 n;
    s32* p;
    s32* v;
    s32 i2;
    s32 i1;

    n = 0x480;
    if (d->lsf == 1) {
        n = 0x240;
    }
    i2 = 0;
    i1 = 0;
    for (k = 0; k < 18; k++) {
        if ((d->unk27C -= 0x40) < 0) {
            d->unk27C = 0x3C0;
        }
        p = buf;
        v = vb;
        for (ch = 0; ch < channels; ch++) {
            fn_800C4E5C(v, p + d->unk27C);
            p += 1024;
            v += 32;
        }
        if (channels == 2) {
            if (mode == 0) {
                fn_800C34B4(buf, (u8*)&out[i2], d->unk27C, mode);
                fn_800C34B4(buf + 1024, (u8*)&out[i2 + 1], d->unk27C, mode);
            } else {
                fn_800C34B4(buf, (u8*)&out[i1], d->unk27C, mode);
                fn_800C34B4(buf + 1024, (u8*)&out[i1 + n], d->unk27C, mode);
            }
        } else {
            fn_800C34B4(buf, (u8*)&out[i1], d->unk27C, 1);
        }
        vb += 64;
        i2 += 64;
        i1 += 32;
    }
}

/* Q24 butterfly stage with a coefficient per word (c advances with the data, 4 words per block).
 * NonMatching: same opcodes, register pairs differ (42 diff lines; per-step forms don't change it). */
void fn_800C51BC(s32* dst, s32* src, s32* c) {
    s32 i;
    s32 s, d, u;

    for (i = 0; i < 4; i++) {
        s = src[0]; d = dst[0]; u = d + s; dst[0] = u; src[0] = (s32)(((s64)(d - s) * c[0]) >> 24);
        s = src[1]; d = dst[1]; u = d + s; dst[1] = u; src[1] = (s32)(((s64)(d - s) * c[1]) >> 24);
        s = src[2]; d = dst[2]; u = d + s; dst[2] = u; src[2] = (s32)(((s64)(d - s) * c[2]) >> 24);
        s = src[3]; d = dst[3]; u = d + s; dst[3] = u; src[3] = (s32)(((s64)(d - s) * c[3]) >> 24);
        dst += 4;
        src += 4;
        c += 4;
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
