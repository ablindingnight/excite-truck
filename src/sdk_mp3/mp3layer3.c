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
    s32 c1end;
    s32 mixed = block_flags & 1;
    s16* q;
    s32 limit;
    s16 region1_end;
    s32 region;
    s32 i;
    s32 end;
    s16 region2_end;

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
            s32 r;
            s16 x;
            s32 bits = MP3_PeekBitsRing(buf, 16, &d->pos) << 16;
            s16 y;

            if (tbl == 13 || tbl == 16) {
                u32 b0;
                u32 p = d->pos;
                s32 sh = 16 - (p & 7);
                u32 b1;
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
    u32 mask;
    s32 shift;
    s32 node = 0;
    mask = 0xF;
    shift = 28;

    while (n-- > 0) {
        u16 e = t->nodes[node][(bits >> shift) & mask];
        if (e & 0x8000) {
            return e & 0x7FFF;
        }
        mask = lbl_8031C2B8[e & 0xF];
        node = (e >> 8) & 0xFF;
        e &= 0xF;
        shift -= e;
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

/* Peek up to 16 bits from the main-data ring without consuming them past n (fn_800C0DF8): reads three bytes
 * (wrapping at 0x2000) and advances *pos by n. Matches. */
u32 MP3_PeekBitsRing(u8* buf, u32 n, u32* pos) {
    u8 b0;
    u32 idx = (*pos >> 3) & 0x1FFF;
    u8 b1;
    u8 b2;
    s32 sh = 24 - (*pos & 7);

    *pos += n;
    if (idx < 0x1FFE) {
        b0 = buf[idx++];
        b1 = buf[idx++];
        b2 = buf[idx];
    } else {
        b0 = buf[idx++];
        idx &= 0x1FFF;
        b1 = buf[idx++];
        idx &= 0x1FFF;
        b2 = buf[idx];
    }
    return (u16)((((b0 << 16) | (b1 << 8) | b2) & lbl_802F3F54[sh]) >> (sh - n));
}

/* IMDCT and overlap-add for one granule of every channel (fn_800C0E7C): per subband the 18-point IMDCT, the
 * long/start/stop window stages, or for short and mixed blocks three 6-point transforms per window
 * (fn_800C1E88 plus an inline butterfly) and fn_800C3088; then the overlap pass into prev. Behaviour verified
 * identical to the original in a PPC simulation (callees run from the original). NonMatching: 649/649, same
 * opcodes. */
void MP3_IMDCT(MP3Decoder* d, s32 (*work)[36], s32* prev, s32* outbuf, u32* block_flags, MP3ChannelBuf* out) {
    s16 ch;
    s16 sb;
    u32 flags;
    s32* pa;
    s32* pb;
    s32* pc;
    s32* pc2;
    s32* p;
    s32* outbuf2;
    s32 t0, t1;
    s32 tmp0[12];
    s32 tmp1[12];
    s32 tmp2[12];

    outbuf2 = outbuf + 0x440;
    for (ch = 0; ch < d->channels; ch++) {
        flags = *block_flags;
        if (flags & 7) {
            if (flags & 1) {
                pa = work[0];
                pc2 = outbuf2;
                pc = outbuf;
                pb = prev;
                for (sb = 0; sb < d->band_count; sb++) {
                    fn_800C18A0(&out->xr[(sb << 4) + (sb << 1)], pa);
                    if (!(sb & 1)) {
                        fn_800C2590(pa, pb, pc, pc2);
                    } else {
                        fn_800C280C(pa, pb, pc, pc2);
                    }
                    fn_800C2DBC(pa + 18, pb, pb + 17);
                    pa += 36;
                    pc2++;
                    pc++;
                    pb += 18;
                }
            } else {
                pa = work[0];
                pc2 = outbuf2;
                pc = outbuf;
                pb = prev;
                for (sb = 0; sb < d->band_count; sb++) {
                    fn_800C18A0(&out->xr[(sb << 4) + (sb << 1)], pa);
                    if (*block_flags & 2) {
                        if (!(sb & 1)) {
                            fn_800C2590(pa, pb, pc, pc2);
                        } else {
                            fn_800C280C(pa, pb, pc, pc2);
                        }
                        fn_800C2F98(pa + 18, pb, pb + 17);
                    }
                    if (*block_flags & 4) {
                        if (!(sb & 1)) {
                            fn_800C2A88(pa, pb, pc, pc2);
                        } else {
                            fn_800C2C18(pa, pb, pc, pc2);
                        }
                        fn_800C2DBC(pa + 18, pb, pb + 17);
                    }
                    pa += 36;
                    pc2++;
                    pc++;
                    pb += 18;
                }
            }
        } else if (flags & 8) {
            pa = work[0];
            for (sb = 0; sb < 2; sb++) {
                fn_800C18A0(&out->xr[(sb << 4) + (sb << 1)], pa);
                fn_800C21D8(pa, pa + 17);
                pa += 36;
            }
            pa = work[d->band_count - 1];
            pa[0] = 0; pa[1] = 0; pa[2] = 0; pa[3] = 0; pa[4] = 0; pa[5] = 0;
            pa[6] = 0; pa[7] = 0; pa[8] = 0; pa[9] = 0; pa[10] = 0; pa[11] = 0;
            pa[12] = 0; pa[13] = 0; pa[14] = 0; pa[15] = 0; pa[16] = 0; pa[17] = 0;
            pa[18] = 0; pa[19] = 0; pa[20] = 0; pa[21] = 0; pa[22] = 0; pa[23] = 0;
            pa[24] = 0; pa[25] = 0; pa[26] = 0; pa[27] = 0; pa[28] = 0; pa[29] = 0;
            pa[30] = 0; pa[31] = 0; pa[32] = 0; pa[33] = 0; pa[34] = 0; pa[35] = 0;
            pa = work[2];
            for (sb = 2; sb < d->band_count - 1; sb++) {
                p = &out->sxr[0][(sb << 2) + (sb << 1)];
                fn_800C1E88(p, (s32*)lbl_802F3FB8, tmp0);
                t0 = p[0] - p[3] - p[4];
                t1 = p[1] - p[2] - p[5];
                tmp0[1] = (s32)(((s64)t0 * lbl_8031D5C8[0]) >> 20) + (s32)(((s64)t1 * lbl_8031D5C8[1]) >> 20);
                tmp0[7] = (s32)(((s64)t0 * lbl_8031D5C8[1]) >> 20) + (s32)(((s64)t1 * -lbl_8031D5C8[0]) >> 20);
                p = &out->sxr[1][(sb << 2) + (sb << 1)];
                fn_800C1E88(p, (s32*)lbl_802F3FB8, tmp1);
                t0 = p[0] - p[3] - p[4];
                t1 = p[1] - p[2] - p[5];
                tmp1[1] = (s32)(((s64)t0 * lbl_8031D5C8[0]) >> 20) + (s32)(((s64)t1 * lbl_8031D5C8[1]) >> 20);
                tmp1[7] = (s32)(((s64)t0 * lbl_8031D5C8[1]) >> 20) + (s32)(((s64)t1 * -lbl_8031D5C8[0]) >> 20);
                p = &out->sxr[2][(sb << 2) + (sb << 1)];
                fn_800C1E88(p, (s32*)lbl_802F3FB8, tmp2);
                t0 = p[0] - p[3] - p[4];
                t1 = p[1] - p[2] - p[5];
                tmp2[1] = (s32)(((s64)t0 * lbl_8031D5C8[0]) >> 20) + (s32)(((s64)t1 * lbl_8031D5C8[1]) >> 20);
                tmp2[7] = (s32)(((s64)t0 * lbl_8031D5C8[1]) >> 20) + (s32)(((s64)t1 * -lbl_8031D5C8[0]) >> 20);
                fn_800C3088(pa, tmp0, tmp1, tmp2);
                pa += 36;
            }
            pb = prev;
            pa = work[0];
            pc = outbuf;
            for (sb = 0; sb < d->band_count; sb++) {
                if (!(sb & 1)) {
                    fn_800C1F6C(pc, pa, pb);
                } else {
                    fn_800C2090(pc, pa, pb);
                }
                pc++;
                pb[0] = pa[18]; pb[1] = pa[19]; pb[2] = pa[20]; pb[3] = pa[21]; pb[4] = pa[22]; pb[5] = pa[23];
                pb[6] = pa[24]; pb[7] = pa[25]; pb[8] = pa[26]; pb[9] = pa[27]; pb[10] = pa[28]; pb[11] = pa[29];
                pb[12] = pa[30]; pb[13] = pa[31]; pb[14] = pa[32]; pb[15] = pa[33]; pb[16] = pa[34]; pb[17] = pa[35];
                pa += 36;
                pb += 18;
            }
        } else if (flags & 0x10) {
            pa = work[d->band_count - 1];
            pa[0] = 0; pa[1] = 0; pa[2] = 0; pa[3] = 0; pa[4] = 0; pa[5] = 0;
            pa[6] = 0; pa[7] = 0; pa[8] = 0; pa[9] = 0; pa[10] = 0; pa[11] = 0;
            pa[12] = 0; pa[13] = 0; pa[14] = 0; pa[15] = 0; pa[16] = 0; pa[17] = 0;
            pa[18] = 0; pa[19] = 0; pa[20] = 0; pa[21] = 0; pa[22] = 0; pa[23] = 0;
            pa[24] = 0; pa[25] = 0; pa[26] = 0; pa[27] = 0; pa[28] = 0; pa[29] = 0;
            pa[30] = 0; pa[31] = 0; pa[32] = 0; pa[33] = 0; pa[34] = 0; pa[35] = 0;
            pa = work[0];
            for (sb = 0; sb < d->band_count - 1; sb++) {
                p = &out->sxr[0][(sb << 2) + (sb << 1)];
                fn_800C1E88(p, (s32*)lbl_802F3FB8, tmp0);
                t0 = p[0] - p[3] - p[4];
                t1 = p[1] - p[2] - p[5];
                tmp0[1] = (s32)(((s64)t0 * lbl_8031D5C8[0]) >> 20) + (s32)(((s64)t1 * lbl_8031D5C8[1]) >> 20);
                tmp0[7] = (s32)(((s64)t0 * lbl_8031D5C8[1]) >> 20) + (s32)(((s64)t1 * -lbl_8031D5C8[0]) >> 20);
                p = &out->sxr[1][(sb << 2) + (sb << 1)];
                fn_800C1E88(p, (s32*)lbl_802F3FB8, tmp1);
                t0 = p[0] - p[3] - p[4];
                t1 = p[1] - p[2] - p[5];
                tmp1[1] = (s32)(((s64)t0 * lbl_8031D5C8[0]) >> 20) + (s32)(((s64)t1 * lbl_8031D5C8[1]) >> 20);
                tmp1[7] = (s32)(((s64)t0 * lbl_8031D5C8[1]) >> 20) + (s32)(((s64)t1 * -lbl_8031D5C8[0]) >> 20);
                p = &out->sxr[2][(sb << 2) + (sb << 1)];
                fn_800C1E88(p, (s32*)lbl_802F3FB8, tmp2);
                t0 = p[0] - p[3] - p[4];
                t1 = p[1] - p[2] - p[5];
                tmp2[1] = (s32)(((s64)t0 * lbl_8031D5C8[0]) >> 20) + (s32)(((s64)t1 * lbl_8031D5C8[1]) >> 20);
                tmp2[7] = (s32)(((s64)t0 * lbl_8031D5C8[1]) >> 20) + (s32)(((s64)t1 * -lbl_8031D5C8[0]) >> 20);
                fn_800C3088(pa, tmp0, tmp1, tmp2);
                pa += 36;
            }
            pb = prev;
            pa = work[0];
            pc = outbuf;
            for (sb = 0; sb < d->band_count; sb++) {
                if (!(sb & 1)) {
                    fn_800C1F6C(pc, pa, pb);
                } else {
                    fn_800C2090(pc, pa, pb);
                }
                pc++;
                pb[0] = pa[18]; pb[1] = pa[19]; pb[2] = pa[20]; pb[3] = pa[21]; pb[4] = pa[22]; pb[5] = pa[23];
                pb[6] = pa[24]; pb[7] = pa[25]; pb[8] = pa[26]; pb[9] = pa[27]; pb[10] = pa[28]; pb[11] = pa[29];
                pb[12] = pa[30]; pb[13] = pa[31]; pb[14] = pa[32]; pb[15] = pa[33]; pb[16] = pa[34]; pb[17] = pa[35];
                pa += 36;
                pb += 18;
            }
        }
        block_flags++;
        work += 32;
        out++;
        outbuf2 += 32;
        outbuf += 32;
        prev += 0x240;
    }
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
        q = in[i];
        p = (s32)(((s64)lbl_802F4018[i] * q) >> 20);
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

/* Windows two 9-sample halves in place: a[i] = Q20(a[i] * w[2i]), b[-i] = Q20(a[i] * w[2i+1]), then the same
 * on a + 18 / b + 18 with the next 18 coefficients (lbl_8031D5E0). Math verified against a simulation of the
 * original. NonMatching: 237/238 (the original keeps a + 18 / b + 18 in their own registers and walks one
 * table pointer). */
static inline void MP3_Window9(s32* a, s32* b, s32* w) {
    s32 x, c0, c1;

    x = a[0];
    c0 = *w++;
    c1 = *w++;
    a[0] = (s32)(((s64)x * c0) >> 20);
    b[0] = (s32)(((s64)x * c1) >> 20);
    x = a[1];
    c0 = *w++;
    c1 = *w++;
    a[1] = (s32)(((s64)x * c0) >> 20);
    b[-1] = (s32)(((s64)x * c1) >> 20);
    x = a[2];
    c0 = *w++;
    c1 = *w++;
    a[2] = (s32)(((s64)x * c0) >> 20);
    b[-2] = (s32)(((s64)x * c1) >> 20);
    x = a[3];
    c0 = *w++;
    c1 = *w++;
    a[3] = (s32)(((s64)x * c0) >> 20);
    b[-3] = (s32)(((s64)x * c1) >> 20);
    x = a[4];
    c0 = *w++;
    c1 = *w++;
    a[4] = (s32)(((s64)x * c0) >> 20);
    b[-4] = (s32)(((s64)x * c1) >> 20);
    x = a[5];
    c0 = *w++;
    c1 = *w++;
    a[5] = (s32)(((s64)x * c0) >> 20);
    b[-5] = (s32)(((s64)x * c1) >> 20);
    x = a[6];
    c0 = *w++;
    c1 = *w++;
    a[6] = (s32)(((s64)x * c0) >> 20);
    b[-6] = (s32)(((s64)x * c1) >> 20);
    x = a[7];
    c0 = *w++;
    c1 = *w++;
    a[7] = (s32)(((s64)x * c0) >> 20);
    b[-7] = (s32)(((s64)x * c1) >> 20);
    x = a[8];
    c0 = *w++;
    c1 = *w++;
    a[8] = (s32)(((s64)x * c0) >> 20);
    b[-8] = (s32)(((s64)x * c1) >> 20);
}

void fn_800C21D8(s32* a, s32* b) {
    MP3_Window9(a, b, lbl_8031D5E0);
    MP3_Window9(a + 18, b + 18, lbl_8031D5E0 + 18);
}

/* IMDCT overlap-add, all nine steps windowed: out1[i*64] = Q20(cur[i] * w[2i]) + prev[i],
 * out2[-i*64] = Q20(cur[i] * w[2i+1]) + prev[17-i] (table walked with *w++). Steps 5-6 compute both
 * products first, which is where the original needs r31. NonMatching: same size and opcodes. */
void fn_800C2590(s32* cur, s32* prev, s32* out1, s32* out2) {
    s32* w = lbl_8031D670;
    s32 x, c0, c1, y0, y1;

    x = cur[0];
    c0 = *w++;
    c1 = *w++;
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[0] = y0 + prev[0];
    out2[0] = y1 + prev[17];
    x = cur[1];
    c0 = *w++;
    c1 = *w++;
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[64] = y0 + prev[1];
    out2[-64] = y1 + prev[16];
    x = cur[2];
    c0 = *w++;
    c1 = *w++;
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[128] = y0 + prev[2];
    out2[-128] = y1 + prev[15];
    x = cur[3];
    c0 = *w++;
    c1 = *w++;
    out1[192] = (s32)(((s64)x * c0) >> 20) + prev[3];
    out2[-192] = (s32)(((s64)x * c1) >> 20) + prev[14];
    x = cur[4];
    c0 = *w++;
    c1 = *w++;
    out1[256] = (s32)(((s64)x * c0) >> 20) + prev[4];
    out2[-256] = (s32)(((s64)x * c1) >> 20) + prev[13];
    x = cur[5];
    c0 = *w++;
    c1 = *w++;
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[320] = y0 + prev[5];
    out2[-320] = y1 + prev[12];
    x = cur[6];
    c0 = *w++;
    c1 = *w++;
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[384] = y0 + prev[6];
    out2[-384] = y1 + prev[11];
    x = cur[7];
    c0 = *w++;
    c1 = *w++;
    out1[448] = (s32)(((s64)x * c0) >> 20) + prev[7];
    out2[-448] = (s32)(((s64)x * c1) >> 20) + prev[10];
    x = cur[8];
    c0 = *w++;
    c1 = *w++;
    out1[512] = (s32)(((s64)x * c0) >> 20) + prev[8];
    out2[-512] = (s32)(((s64)x * c1) >> 20) + prev[9];
}


/* Like fn_800C2590 with the other window table (lbl_8031D6B8) and alternating signs on prev: even steps
 * add to out1 and subtract from out2, odd steps the reverse (verified against a simulation of the
 * original). NonMatching: 159/159, same opcodes. */
void fn_800C280C(s32* cur, s32* prev, s32* out1, s32* out2) {
    s32* w = lbl_8031D6B8;
    s32 x, c0, c1, y0, y1;

    x = cur[0];
    c0 = *w++;
    c1 = *w++;
    out1[0] = (s32)(((s64)x * c0) >> 20) + prev[0];
    out2[0] = (s32)(((s64)x * c1) >> 20) - prev[17];
    x = cur[1];
    c0 = *w++;
    c1 = *w++;
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[64] = y0 - prev[1];
    out2[-64] = y1 + prev[16];
    x = cur[2];
    c0 = *w++;
    c1 = *w++;
    out1[128] = (s32)(((s64)x * c0) >> 20) + prev[2];
    out2[-128] = (s32)(((s64)x * c1) >> 20) - prev[15];
    x = cur[3];
    c0 = *w++;
    c1 = *w++;
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[192] = y0 - prev[3];
    out2[-192] = y1 + prev[14];
    x = cur[4];
    c0 = *w++;
    c1 = *w++;
    out1[256] = (s32)(((s64)x * c0) >> 20) + prev[4];
    out2[-256] = (s32)(((s64)x * c1) >> 20) - prev[13];
    x = cur[5];
    c0 = *w++;
    c1 = *w++;
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[320] = y0 - prev[5];
    out2[-320] = y1 + prev[12];
    x = cur[6];
    c0 = *w++;
    c1 = *w++;
    out1[384] = (s32)(((s64)x * c0) >> 20) + prev[6];
    out2[-384] = (s32)(((s64)x * c1) >> 20) - prev[11];
    x = cur[7];
    c0 = *w++;
    c1 = *w++;
    out1[448] = (s32)(((s64)x * c0) >> 20) - prev[7];
    out2[-448] = (s32)(((s64)x * c1) >> 20) + prev[10];
    x = cur[8];
    c0 = *w++;
    c1 = *w++;
    out1[512] = (s32)(((s64)x * c0) >> 20) + prev[8];
    out2[-512] = (s32)(((s64)x * c1) >> 20) - prev[9];
}


/* IMDCT output, first window half: copies prev[0..5] and writes prev[17-i] - cur[i] backwards from out2;
 * the last three use Q20 window coefficients. prev[17-i] is read through a pointer walking down from prev + 17,
 * and the coefficients through *w++ then w[k]. Matches; behaviour also checked in a PPC simulation. */
void fn_800C2A88(s32* cur, s32* prev, s32* out1, s32* out2) {
    s32* pp = prev + 17;
    s32* w = lbl_8031D700;
    s32 i;
    s32 x, c0, c1, y0, y1;

    for (i = 0; i < 6; i++) {
        x = -cur[i];
        out1[i * 64] = prev[i];
        out2[-i * 64] = x + *pp--;
    }
    x = cur[6];
    c0 = *w++;
    c1 = *w++;
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[384] = y0 + prev[6];
    out2[-384] = y1 + *pp--;
    x = cur[7];
    c0 = w[0];
    c1 = w[1];
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[448] = y0 + prev[7];
    out2[-448] = y1 + *pp--;
    x = cur[8];
    c0 = w[2];
    c1 = w[3];
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[512] = y0 + prev[8];
    out2[-512] = y1 + *pp--;
}

/* Second window half: like fn_800C2A88 with lbl_8031D718 and alternating signs (verified 100/100 against
 * a simulation of the original), with the same downward prev pointer and *w++ coefficients. NonMatching:
 * 105/105, same instructions; the original keeps prev + 17 as the base for prev[10]/prev[9]. */
void fn_800C2C18(s32* cur, s32* prev, s32* out1, s32* out2) {
    s32* pp = prev + 17;
    s32* w = lbl_8031D718;
    s32 x, c0, c1, y0, y1;

    x = -cur[0];
    out1[0] = prev[0];
    out2[0] = -(x + *pp--);
    x = -cur[1];
    out1[64] = -prev[1];
    out2[-64] = x + *pp--;
    x = -cur[2];
    out1[128] = prev[2];
    out2[-128] = -(x + *pp--);
    x = -cur[3];
    out1[192] = -prev[3];
    out2[-192] = x + *pp--;
    x = -cur[4];
    out1[256] = prev[4];
    out2[-256] = -(x + *pp--);
    x = -cur[5];
    out1[320] = -prev[5];
    out2[-320] = x + *pp--;
    x = cur[6];
    c0 = *w++;
    c1 = *w++;
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[384] = (y0 + prev[6]);
    out2[-384] = -(y1 + *pp--);
    x = cur[7];
    c0 = w[0];
    c1 = w[1];
    y0 = (s32)(((s64)x * c0) >> 20);
    out1[448] = -(y0 + prev[7]);
    y1 = (s32)(((s64)x * c1) >> 20);
    out2[-448] = (y1 + *pp--);
    x = cur[8];
    c0 = w[2];
    c1 = w[3];
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[512] = (y0 + prev[8]);
    out2[-512] = -(y1 + *pp--);
}

/* Nine Q20 window pairs: out1[i] = x * w[2i], out2[-i] = x * w[2i + 1] (lbl_8031D730). The table is walked
 * with *w++ (gives the original's lwzu). */
void fn_800C2DBC(s32* in, s32* out1, s32* out2) {
    s32* w = lbl_8031D730;
    s32 x, c0, c1, y0, y1;

    x = in[0];
    c0 = *w++;
    c1 = *w++;
    out1[0] = (s32)(((s64)x * c0) >> 20);
    out2[0] = (s32)(((s64)x * c1) >> 20);
    x = in[1];
    c0 = *w++;
    c1 = *w++;
    out1[1] = (s32)(((s64)x * c0) >> 20);
    out2[-1] = (s32)(((s64)x * c1) >> 20);
    x = in[2];
    c0 = *w++;
    c1 = *w++;
    out1[2] = (s32)(((s64)x * c0) >> 20);
    out2[-2] = (s32)(((s64)x * c1) >> 20);
    x = in[3];
    c0 = *w++;
    c1 = *w++;
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[3] = y0;
    out2[-3] = y1;
    x = in[4];
    c0 = *w++;
    c1 = *w++;
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[4] = y0;
    out2[-4] = y1;
    x = in[5];
    c0 = *w++;
    c1 = *w++;
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[5] = y0;
    out2[-5] = y1;
    x = in[6];
    c0 = *w++;
    c1 = *w++;
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[6] = y0;
    out2[-6] = y1;
    x = in[7];
    c0 = *w++;
    c1 = *w++;
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[7] = y0;
    out2[-7] = y1;
    x = in[8];
    c0 = *w++;
    c1 = *w++;
    y0 = (s32)(((s64)x * c0) >> 20);
    y1 = (s32)(((s64)x * c1) >> 20);
    out1[8] = y0;
    out2[-8] = y1;
}

/* IMDCT output for the first block: copies in[0..5] forward, zeroes out2 backwards, and the last three
 * are Q20 window pairs (lbl_8031D778, walked with *w++). */
void fn_800C2F98(s32* cur, s32* out1, s32* out2) {
    s32* w = lbl_8031D778;
    s32 i;
    s32 x, c0, c1;

    for (i = 0; i < 6; i++) {
        x = cur[i];
        out1[i] = x;
        out2[-i] = 0;
    }
    x = cur[6];
    c0 = *w++;
    c1 = *w++;
    out1[6] = (s32)(((s64)x * c0) >> 20);
    out2[-6] = (s32)(((s64)x * c1) >> 20);
    x = cur[7];
    c0 = *w++;
    c1 = *w++;
    out1[7] = (s32)(((s64)x * c0) >> 20);
    out2[-7] = (s32)(((s64)x * c1) >> 20);
    x = cur[8];
    c0 = *w++;
    c1 = *w++;
    out1[8] = (s32)(((s64)x * c0) >> 20);
    out2[-8] = (s32)(((s64)x * c1) >> 20);
}

/* Short-block windowing of three 6-sample windows into 36 outputs (lbl_8031D790): out[0..5] and out[30..35]
 * are zero, the middle overlaps window k's tail with window k+1's head. Math verified against a simulation of
 * the original. NonMatching: 267/267, same opcodes, scheduling differs. */
void fn_800C3088(s32* out, s32* in1, s32* in2, s32* in3) {
    s32* w = lbl_8031D790;
    s32 x, c, y;

    out[0] = 0;
    out[1] = 0;
    out[2] = 0;
    out[3] = 0;
    out[4] = 0;
    out[5] = 0;
    x = in1[0];
    c = *w++;
    out[6] = (s32)(((s64)x * c) >> 20);
    x = in1[1];
    c = *w++;
    out[7] = (s32)(((s64)x * c) >> 20);
    x = in1[2];
    c = *w++;
    out[8] = (s32)(((s64)x * c) >> 20);
    x = in1[2];
    c = *w++;
    out[9] = (s32)(((s64)x * c) >> 20);
    x = in1[1];
    c = *w++;
    out[10] = (s32)(((s64)x * c) >> 20);
    x = in1[0];
    c = *w++;
    out[11] = (s32)(((s64)x * c) >> 20);
    x = in1[6];
    c = w[0];
    y = (s32)(((s64)x * c) >> 20);
    x = in2[0];
    c = w[-6];
    out[12] = y + (s32)(((s64)x * c) >> 20);
    x = in1[7];
    c = w[1];
    y = (s32)(((s64)x * c) >> 20);
    x = in2[1];
    c = w[-5];
    out[13] = y + (s32)(((s64)x * c) >> 20);
    x = in1[8];
    c = w[2];
    y = (s32)(((s64)x * c) >> 20);
    x = in2[2];
    c = w[-4];
    out[14] = y + (s32)(((s64)x * c) >> 20);
    x = in1[8];
    c = w[3];
    y = (s32)(((s64)x * c) >> 20);
    x = in2[2];
    c = w[-3];
    out[15] = y + (s32)(((s64)x * c) >> 20);
    x = in1[7];
    c = w[4];
    y = (s32)(((s64)x * c) >> 20);
    x = in2[1];
    c = w[-2];
    out[16] = y + (s32)(((s64)x * c) >> 20);
    x = in1[6];
    c = w[5];
    y = (s32)(((s64)x * c) >> 20);
    x = in2[0];
    c = w[-1];
    out[17] = y + (s32)(((s64)x * c) >> 20);
    x = in2[6];
    c = w[0];
    y = (s32)(((s64)x * c) >> 20);
    x = in3[0];
    c = w[-6];
    out[18] = y + (s32)(((s64)x * c) >> 20);
    x = in2[7];
    c = w[1];
    y = (s32)(((s64)x * c) >> 20);
    x = in3[1];
    c = w[-5];
    out[19] = y + (s32)(((s64)x * c) >> 20);
    x = in2[8];
    c = w[2];
    y = (s32)(((s64)x * c) >> 20);
    x = in3[2];
    c = w[-4];
    out[20] = y + (s32)(((s64)x * c) >> 20);
    x = in2[8];
    c = w[3];
    y = (s32)(((s64)x * c) >> 20);
    x = in3[2];
    c = w[-3];
    out[21] = y + (s32)(((s64)x * c) >> 20);
    x = in2[7];
    c = w[4];
    out[22] = (s32)(((s64)x * c) >> 20) + (s32)(((s64)in3[1] * w[-2]) >> 20);
    x = in2[6];
    c = w[5];
    y = (s32)(((s64)x * c) >> 20);
    x = in3[0];
    c = w[-1];
    out[23] = y + (s32)(((s64)x * c) >> 20);
    out[24] = (s32)(((s64)in3[6] * w[0]) >> 20);
    out[25] = (s32)(((s64)in3[7] * w[1]) >> 20);
    out[26] = (s32)(((s64)in3[8] * w[2]) >> 20);
    out[27] = (s32)(((s64)in3[8] * w[3]) >> 20);
    out[28] = (s32)(((s64)in3[7] * w[4]) >> 20);
    out[29] = (s32)(((s64)in3[6] * w[5]) >> 20);
    out[30] = 0;
    out[31] = 0;
    out[32] = 0;
    out[33] = 0;
    out[34] = 0;
    out[35] = 0;
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

/* Polyphase synthesis: samples 1..15 and their mirrors 31..17 for one channel. 16 Q25 MACs per sample
 * over the 1024-entry ring v (positions pos + off and pos + 0x3E0 - off), clamped to 16 bits. Math verified
 * against a simulation of the original (60/60). lo/hi locals from DeepSeek; MAC order per group searched.
 * NonMatching: 251/251, one li/stw differs plus scheduling. */
void fn_800C3574(s32* v, const s32* w, s32 pos, u8* out, s32 mode) {
    s32 j;
    s64 a, b;
    s32 sa, sb;
    s32 lo, hi;

    for (j = 0; j < 15; j++) {
        a = 0;
        b = 0;
        lo = v[pos & 0x3ff];
        hi = v[(pos + 0x3e0) & 0x3ff];
        a += (s64)hi * w[8];
        a += (s64)lo * w[0];
        b += (s64)hi * -w[0];
        b += (s64)lo * w[8];
        lo = v[(pos + 0x60) & 0x3ff];
        hi = v[(pos + 0x380) & 0x3ff];
        a += (s64)hi * -w[9];
        a += (s64)lo * w[1];
        b += (s64)hi * w[1];
        b += (s64)lo * w[9];
        lo = v[(pos + 0x80) & 0x3ff];
        hi = v[(pos + 0x360) & 0x3ff];
        a += (s64)hi * w[10];
        a += (s64)lo * w[2];
        b += (s64)hi * -w[2];
        b += (s64)lo * w[10];
        lo = v[(pos + 0xe0) & 0x3ff];
        hi = v[(pos + 0x300) & 0x3ff];
        a += (s64)hi * -w[11];
        a += (s64)lo * w[3];
        b += (s64)hi * w[3];
        b += (s64)lo * w[11];
        lo = v[(pos + 0x100) & 0x3ff];
        hi = v[(pos + 0x2e0) & 0x3ff];
        a += (s64)hi * w[12];
        a += (s64)lo * w[4];
        b += (s64)hi * -w[4];
        b += (s64)lo * w[12];
        lo = v[(pos + 0x160) & 0x3ff];
        hi = v[(pos + 0x280) & 0x3ff];
        a += (s64)hi * -w[13];
        a += (s64)lo * w[5];
        b += (s64)hi * w[5];
        b += (s64)lo * w[13];
        lo = v[(pos + 0x180) & 0x3ff];
        hi = v[(pos + 0x260) & 0x3ff];
        a += (s64)hi * w[14];
        a += (s64)lo * w[6];
        b += (s64)hi * -w[6];
        b += (s64)lo * w[14];
        lo = v[(pos + 0x1e0) & 0x3ff];
        hi = v[(pos + 0x200) & 0x3ff];
        a += (s64)hi * -w[15];
        a += (s64)lo * w[7];
        b += (s64)hi * w[7];
        b += (s64)lo * w[15];
        sa = (s32)(a >> 25);
        sb = (s32)(b >> 25);
        if ((sa >> 31) != (sa >> 15)) sa = (sa >> 31) ^ 0x7FFF;
        if ((sb >> 31) != (sb >> 15)) sb = (sb >> 31) ^ 0x7FFF;
        if (mode == 0) {
            ((s16*)out)[2 * j] = sa;
            ((s16*)out)[60 - 2 * j] = sb;
        } else {
            ((s16*)out)[j] = sa;
            ((s16*)out)[30 - j] = sb;
        }
        pos++;
        w += 16;
    }
}

/* Polyphase synthesis: samples 0 and 16 (16 + 8 MACs). Verified 100/100 against a simulation of the original.
 * NonMatching: 223/223, same opcodes, scheduling differs. */
void fn_800C3960(s32* v, const s32* w, s32 pos, u8* out, s32 mask, s32 mode) {
    s64 a;
    s64 b;
    s32 sa, sb;
    s32* v2;

    v2 = v + 16;
    a = 0;
    b = 0;
    a += (s64)v[(pos + 0x0) & mask] * w[0];
    a += (s64)v[(pos + 0x60) & mask] * w[1];
    b += (s64)v2[(pos + 0x60) & mask] * w[17];
    a += (s64)v[(pos + 0x80) & mask] * w[2];
    a += (s64)v[(pos + 0xe0) & mask] * w[3];
    b += (s64)v2[(pos + 0xe0) & mask] * w[19];
    a += (s64)v[(pos + 0x100) & mask] * w[4];
    a += (s64)v[(pos + 0x160) & mask] * w[5];
    b += (s64)v2[(pos + 0x160) & mask] * w[21];
    a += (s64)v[(pos + 0x180) & mask] * w[6];
    a += (s64)v[(pos + 0x1e0) & mask] * w[7];
    b += (s64)v2[(pos + 0x1e0) & mask] * w[23];
    a += (s64)v[(pos + 0x200) & mask] * w[8];
    a += (s64)v[(pos + 0x260) & mask] * w[9];
    b += (s64)v2[(pos + 0x260) & mask] * w[25];
    a += (s64)v[(pos + 0x280) & mask] * w[10];
    a += (s64)v[(pos + 0x2e0) & mask] * w[11];
    b += (s64)v2[(pos + 0x2e0) & mask] * w[27];
    a += (s64)v[(pos + 0x300) & mask] * w[12];
    a += (s64)v[(pos + 0x360) & mask] * w[13];
    b += (s64)v2[(pos + 0x360) & mask] * w[29];
    a += (s64)v[(pos + 0x380) & mask] * w[14];
    a += (s64)v[(pos + 0x3e0) & mask] * w[15];
    b += (s64)v2[(pos + 0x3e0) & mask] * w[31];
    a >>= 25;
    b >>= 25;
    sa = (s32)a;
    sb = (s32)b;
    if ((sa >> 31) != (sa >> 15)) {
        sa = (sa >> 31) ^ 0x7FFF;
    }
    if ((sb >> 31) != (sb >> 15)) {
        sb = (sb >> 31) ^ 0x7FFF;
    }
    if (mode == 0) {
        ((s16*)out)[0] = sa;
        ((s16*)out)[32] = sb;
    } else {
        ((s16*)out)[0] = sa;
        ((s16*)out)[16] = sb;
    }
}

/* Scalefactor decode (fn_800C3DA0): MPEG-1 scalefactors with scfsi reuse from granule 0, or MPEG-2 LSF
 * scalefactors with the scalefac_compress partitions (separate intensity-stereo table for the right channel),
 * for long, short and mixed blocks; also sets the region counts, preflag and intensity scale. Rewritten from
 * the original's structure (math from ChatGPT round 24); behaviour verified against the original in a PPC
 * simulation (400 random cases, bit reader from the original). NonMatching: 871 of 879. */
void fn_800C3DA0(MP3Decoder* d, u8* ring, u32 flags, s16* sf_long, s16* sf_long0, s16 (*sf_short)[3], s16 part2_3,
                 s16 sfc, s16* scfsi, s8 gr, s8 ch, s16* bound, s16* region0, s16* region1, s16* preflag,
                 s16* is_scale) {
    s8 i;
    s8 sfb;
    s32 w;
    s8 end;
    s8 start;
    s32 len1;
    s32 len2;
    s32 c;
    s32 t;
    s16 slen0;
    s16 slen1;
    s16 slen2;
    s16 slen3;
    s8 nr0;
    s8 nr1;
    s8 nr2;
    s8 nr3;

    if (d->lsf == 0) {
        d->unkC = d->unk10;
        d->unk10 += part2_3;
        len1 = lbl_802F44C8[sfc];
        len2 = lbl_802F44D8[sfc];
        *bound = 0;
        if (flags & 7) {
            if (flags & 6) {
                *region0 = 7;
                *region1 = 36;
            }
            if (scfsi[0] == 0 || gr == 0) {
                for (i = 0; i < 6; i++) {
                    sf_long[i] = MP3_GetBitsRing(ring, len1, (u32*)&d->unkC);
                }
            }
            if (scfsi[1] == 0 || gr == 0) {
                for (i = 6; i < 11; i++) {
                    sf_long[i] = MP3_GetBitsRing(ring, len1, (u32*)&d->unkC);
                }
            }
            if (scfsi[2] == 0 || gr == 0) {
                for (i = 11; i < 16; i++) {
                    sf_long[i] = MP3_GetBitsRing(ring, len2, (u32*)&d->unkC);
                }
            }
            if (scfsi[3] == 0 || gr == 0) {
                for (i = 16; i < 21; i++) {
                    sf_long[i] = MP3_GetBitsRing(ring, len2, (u32*)&d->unkC);
                }
            }
            if (gr == 1) {
                if (scfsi[0] == 1) {
                    for (i = 0; i < 6; i++) {
                        sf_long[i] = sf_long0[i];
                    }
                }
                if (scfsi[1] == 1) {
                    for (i = 6; i < 11; i++) {
                        sf_long[i] = sf_long0[i];
                    }
                }
                if (scfsi[2] == 1) {
                    for (i = 11; i < 16; i++) {
                        sf_long[i] = sf_long0[i];
                    }
                }
                if (scfsi[3] == 1) {
                    for (i = 16; i < 21; i++) {
                        sf_long[i] = sf_long0[i];
                    }
                }
            }
        } else if (flags & 8) {
            *region0 = 7;
            *region1 = 36;
            for (i = 0; i < 8; i++) {
                sf_long[i] = MP3_GetBitsRing(ring, len1, (u32*)&d->unkC);
            }
            for (sfb = 3; sfb < 6; sfb++) {
                for (w = 0; w < 3; w++) {
                    sf_short[sfb][w] = MP3_GetBitsRing(ring, len1, (u32*)&d->unkC);
                }
            }
            for (sfb = 6; sfb < 12; sfb++) {
                for (w = 0; w < 3; w++) {
                    sf_short[sfb][w] = MP3_GetBitsRing(ring, len2, (u32*)&d->unkC);
                }
            }
        } else if (flags & 0x10) {
            *region0 = 8;
            *region1 = 36;
            for (sfb = 0; sfb < 6; sfb++) {
                for (w = 0; w < 3; w++) {
                    sf_short[sfb][w] = MP3_GetBitsRing(ring, len1, (u32*)&d->unkC);
                }
            }
            for (sfb = 6; sfb < 12; sfb++) {
                for (w = 0; w < 3; w++) {
                    sf_short[sfb][w] = MP3_GetBitsRing(ring, len2, (u32*)&d->unkC);
                }
            }
        }
    }

    if (d->lsf == 1) {
        if ((d->mode_ext != 1 && d->mode_ext != 3) || ch != 1) {
            if (sfc < 400) {
                slen0 = (sfc >> 4) / 5;
                slen1 = (sfc >> 4) % 5;
                slen2 = (sfc >> 2) & 3;
                slen3 = sfc & 3;
                *preflag = 0;
                if (flags & 0x18) {
                    nr0 = 6;
                    nr1 = 9;
                    nr2 = 9;
                    nr3 = 9;
                    if (flags & 0x10) {
                        nr0 = 9;
                    }
                } else {
                    nr0 = 6;
                    nr1 = 5;
                    nr2 = 5;
                    nr3 = 5;
                }
            }
            if (sfc >= 400 && sfc < 500) {
                c = sfc - 400;
                slen0 = (c >> 2) / 5;
                slen1 = (c >> 2) % 5;
                slen2 = c & 3;
                slen3 = 0;
                *preflag = 0;
                if (flags & 0x18) {
                    nr0 = 6;
                    nr1 = 9;
                    nr2 = 12;
                    nr3 = 6;
                    if (flags & 0x10) {
                        nr0 = 9;
                    }
                } else {
                    nr0 = 6;
                    nr1 = 5;
                    nr2 = 7;
                    nr3 = 3;
                }
            }
            if (sfc >= 500 && sfc < 512) {
                c = sfc - 500;
                slen0 = c / 3;
                slen1 = c % 3;
                slen2 = 0;
                slen3 = 0;
                *preflag = 1;
                if (flags & 0x18) {
                    nr0 = 15;
                    nr1 = 18;
                    nr2 = 0;
                    nr3 = 0;
                    if (flags & 0x10) {
                        nr0 = 18;
                    }
                } else {
                    nr0 = 11;
                    nr1 = 10;
                    nr2 = 0;
                    nr3 = 0;
                }
            }
        }
        if ((d->mode_ext == 1 || d->mode_ext == 3) && ch == 1) {
            c = sfc >> 1;
            *is_scale = sfc & 1;
            if (c < 180) {
                slen0 = c / 36;
                slen1 = (c % 36) / 6;
                slen2 = (c % 36) % 6;
                slen3 = 0;
                *preflag = 0;
                if (flags & 0x18) {
                    if (flags & 8) {
                        nr0 = 6;
                        nr1 = 15;
                        nr2 = 12;
                        nr3 = 0;
                    } else {
                        nr0 = 12;
                        nr1 = 12;
                        nr2 = 12;
                        nr3 = 0;
                    }
                } else {
                    nr0 = 7;
                    nr1 = 7;
                    nr2 = 7;
                    nr3 = 0;
                }
            }
            if (c >= 180 && c < 244) {
                t = c - 180;
                slen0 = (t >> 4) & 3;
                slen1 = (t >> 2) & 3;
                slen2 = t & 3;
                slen3 = 0;
                *preflag = 0;
                if (flags & 0x18) {
                    if (flags & 8) {
                        nr0 = 6;
                        nr1 = 12;
                        nr2 = 9;
                        nr3 = 6;
                    } else {
                        nr0 = 12;
                        nr1 = 9;
                        nr2 = 9;
                        nr3 = 6;
                    }
                } else {
                    nr0 = 6;
                    nr1 = 6;
                    nr2 = 6;
                    nr3 = 3;
                }
            }
            if (c >= 244 && c <= 255) {
                t = c - 244;
                slen0 = t / 3;
                slen1 = t % 3;
                slen2 = 0;
                slen3 = 0;
                *preflag = 0;
                if (flags & 0x18) {
                    if (flags & 8) {
                        nr0 = 6;
                        nr1 = 18;
                        nr2 = 9;
                        nr3 = 0;
                    } else {
                        nr0 = 15;
                        nr1 = 12;
                        nr2 = 9;
                        nr3 = 0;
                    }
                } else {
                    nr0 = 8;
                    nr1 = 8;
                    nr2 = 5;
                    nr3 = 0;
                }
            }
        }

        for (i = 0; i < 21; i++) {
            sf_long[i] = 0;
        }
        for (sfb = 0; sfb < 12; sfb++) {
            for (w = 0; w < 3; w++) {
                sf_short[sfb][w] = 0;
            }
        }
        d->unkC = d->unk10;
        d->unk10 += part2_3;
        *bound = 0;
        if (flags & 0x18) {
            if (flags & 8) {
                *region0 = 7;
                *region1 = 36;
                for (i = 0; i < 6; i++) {
                    *sf_long++ = MP3_GetBitsRing(ring, slen0, (u32*)&d->unkC);
                }
                t = nr0 - 6;
                end = t / 3 + 3;
                for (sfb = 3; sfb < end; sfb++) {
                    for (w = 0; w < 3; w++) {
                        sf_short[sfb][w] = MP3_GetBitsRing(ring, slen0, (u32*)&d->unkC);
                    }
                }
                t += nr1;
                start = end;
                end = t / 3 + 3;
                for (sfb = start; sfb < end; sfb++) {
                    for (w = 0; w < 3; w++) {
                        sf_short[sfb][w] = MP3_GetBitsRing(ring, slen1, (u32*)&d->unkC);
                    }
                }
                t += nr2;
                start = end;
                end = t / 3 + 3;
                for (sfb = start; sfb < end; sfb++) {
                    for (w = 0; w < 3; w++) {
                        sf_short[sfb][w] = MP3_GetBitsRing(ring, slen2, (u32*)&d->unkC);
                    }
                }
                t += nr3;
                start = end;
                end = t / 3 + 3;
                for (sfb = start; sfb < end; sfb++) {
                    for (w = 0; w < 3; w++) {
                        sf_short[sfb][w] = MP3_GetBitsRing(ring, slen3, (u32*)&d->unkC);
                    }
                }
            } else {
                *region0 = 8;
                *region1 = 36;
                end = nr0 / 3;
                for (sfb = 0; sfb < end; sfb++) {
                    for (w = 0; w < 3; w++) {
                        sf_short[sfb][w] = MP3_GetBitsRing(ring, slen0, (u32*)&d->unkC);
                    }
                }
                t = nr0 + nr1;
                start = end;
                end = t / 3;
                for (sfb = start; sfb < end; sfb++) {
                    for (w = 0; w < 3; w++) {
                        sf_short[sfb][w] = MP3_GetBitsRing(ring, slen1, (u32*)&d->unkC);
                    }
                }
                t += nr2;
                start = end;
                end = t / 3;
                for (sfb = start; sfb < end; sfb++) {
                    for (w = 0; w < 3; w++) {
                        sf_short[sfb][w] = MP3_GetBitsRing(ring, slen2, (u32*)&d->unkC);
                    }
                }
                t += nr3;
                start = end;
                end = t / 3;
                for (sfb = start; sfb < end; sfb++) {
                    for (w = 0; w < 3; w++) {
                        sf_short[sfb][w] = MP3_GetBitsRing(ring, slen3, (u32*)&d->unkC);
                    }
                }
            }
        } else {
            if (flags & 6) {
                *region0 = 7;
                *region1 = 36;
            }
            for (i = 0; i < nr0; i++) {
                sf_long[i] = MP3_GetBitsRing(ring, slen0, (u32*)&d->unkC);
            }
            for (i = nr0; i < nr0 + nr1; i++) {
                sf_long[i] = MP3_GetBitsRing(ring, slen1, (u32*)&d->unkC);
            }
            for (i = nr0 + nr1; i < nr0 + nr1 + nr2; i++) {
                sf_long[i] = MP3_GetBitsRing(ring, slen2, (u32*)&d->unkC);
            }
            for (i = nr0 + nr1 + nr2; i < nr0 + nr1 + nr2 + nr3; i++) {
                sf_long[i] = MP3_GetBitsRing(ring, slen3, (u32*)&d->unkC);
            }
        }
    }
}

/* TMCCMP3Init (fn_800C3CDC), formerly MP3_Create: API name from the game's "mp3: TMCCMP3Init fails"
 * diagnostic at the verified caller (strongly supported, not an original linker symbol). */
MP3Decoder* TMCCMP3Init(u8* header, u32 b, s32 buf, s32 size) {
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
    d->is_init = 0;
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
 * The first call per channel indexes out[k * 64] / out[k * 32] and the second out[i2 + 1] / out[i1 + n], which gives
 * the original's four counters. NonMatching: 90/90, same instructions; registers differ. */
void fn_800C4CF4(MP3Decoder* d, s32* buf, s32* vb, s16* out, s8 channels, s32 mode) {
    s32* w;
    s32 i1;
    s32 n;
    s32 i2;
    s32* p;
    s32* v;
    s8 ch;
    s32 k;

    n = 0x480;
    if (d->lsf == 1) {
        n = 0x240;
    }
    w = vb;
    i2 = 0;
    i1 = 0;
    for (k = 0; k < 18; k++) {
        if ((d->unk27C -= 0x40) < 0) {
            d->unk27C = 0x3C0;
        }
        p = buf;
        v = w;
        for (ch = 0; ch < channels; ch++) {
            fn_800C4E5C(v, p + d->unk27C);
            p += 1024;
            v += 32;
        }
        if (channels == 2) {
            if (mode == 0) {
                fn_800C34B4(buf, (u8*)&out[k * 64], d->unk27C, mode);
                fn_800C34B4(buf + 1024, (u8*)&out[i2 + 1], d->unk27C, mode);
            } else {
                fn_800C34B4(buf, (u8*)&out[k * 32], d->unk27C, mode);
                fn_800C34B4(buf + 1024, (u8*)&out[i1 + n], d->unk27C, mode);
            }
        } else {
            fn_800C34B4(buf, (u8*)&out[k * 32], d->unk27C, 1);
        }
        w += 64;
        i2 += 64;
        i1 += 32;
    }
}

/* DCT-32 for one synthesis step: reorder, the five butterfly stages (lbl_8031D7C0), then the output
 * recombination (an in-place b[x] += b[y] chain in the original; MWCC drops the dead stores). The
 * recombination below transcribes the target's dataflow; its formulas are verified against a symbolic
 * trace. Returns 0. NonMatching: 216/216, same opcodes, scheduling/registers differ. */
s32 fn_800C4E5C(s32* in, s32* out) {
    s32 b[32];
    s32 r0, r3, r4, r5, r6, r7, r8, r9;

    MP3_Reorder32((u32*)b, (u32*)in, 0);
    fn_800C51BC(b, b + 16, lbl_8031D7C0);
    fn_800C5298(b, b + 8, lbl_8031D7C0 + 16);
    fn_800C5430(b, b + 4, lbl_8031D7C0 + 24);
    fn_800C5508(b, b + 2, lbl_8031D7C0 + 28);
    fn_800C55D8(b, b + 1, lbl_8031D7C0 + 30);
    r0 = b[0];
    r0 = -r0;
    out[48] = r0;
    r4 = b[31];
    r3 = b[30];
    r5 = b[24];
    r0 = b[28];
    r4 = r4 + r3;
    r3 = b[16];
    r0 = r5 + r0;
    r0 = r4 + r0;
    r3 = r3 + r0;
    r3 = -r3;
    out[47] = r3;
    r4 = b[15];
    r3 = b[12];
    r5 = b[14];
    r4 = r4 + r3;
    r3 = b[8];
    r4 = r5 + r4;
    r3 = r3 + r4;
    r3 = -r3;
    out[46] = r3;
    r6 = b[23];
    r3 = b[20];
    r5 = b[22];
    r3 = r6 + r3;
    r5 = r5 + r3;
    r3 = r5 + r0;
    r3 = -r3;
    out[45] = r3;
    r7 = b[7];
    r3 = b[4];
    r6 = b[6];
    r3 = r7 + r3;
    r3 = r6 + r3;
    r3 = -r3;
    out[44] = r3;
    r6 = b[26];
    r3 = b[27];
    r7 = b[24];
    r3 = r6 + r3;
    r3 = r3 - r7;
    r0 = r0 + r3;
    r3 = r5 + r0;
    r3 = -r3;
    out[43] = r3;
    r3 = b[10];
    r6 = b[11];
    r3 = r4 + r3;
    r4 = r6 + r3;
    r3 = -r4;
    out[42] = r3;
    r3 = b[18];
    r6 = b[19];
    r3 = r0 + r3;
    r3 = r6 + r3;
    r3 = -r3;
    out[41] = r3;
    r6 = b[2];
    r3 = b[3];
    r3 = r6 + r3;
    r3 = -r3;
    out[40] = r3;
    r7 = b[28];
    r6 = b[29];
    r3 = b[18];
    r7 = r6 - r7;
    r6 = b[19];
    r0 = r0 + r7;
    r3 = r0 + r3;
    r3 = r6 + r3;
    r3 = -r3;
    out[39] = r3;
    r6 = b[12];
    r3 = b[13];
    r3 = r3 - r6;
    r4 = r4 + r3;
    r3 = -r4;
    out[38] = r3;
    r6 = b[20];
    r3 = b[21];
    r3 = r3 - r6;
    r5 = r5 + r3;
    r3 = r5 + r0;
    r3 = -r3;
    out[37] = r3;
    r7 = b[7];
    r3 = b[5];
    r6 = b[6];
    r3 = r7 + r3;
    r3 = r6 + r3;
    r3 = -r3;
    out[36] = r3;
    r6 = b[26];
    r3 = b[25];
    r7 = b[27];
    r3 = r3 - r6;
    r3 = r3 - r7;
    r0 = r0 + r3;
    r3 = r5 + r0;
    r3 = -r3;
    out[35] = r3;
    r8 = b[10];
    r7 = 0x0;
    r6 = b[9];
    r3 = 0x0;
    r9 = b[11];
    r6 = r6 - r8;
    r6 = r6 - r9;
    r4 = r4 + r6;
    r6 = -r4;
    out[34] = r6;
    r6 = b[17];
    r6 = r6 + r0;
    r6 = -r6;
    out[33] = r6;
    r6 = b[1];
    out[0] = r6;
    r6 = -r6;
    r9 = b[30];
    r8 = b[17];
    r0 = r0 - r9;
    r8 = r8 + r0;
    out[1] = r8;
    r8 = b[14];
    r4 = r4 - r8;
    out[2] = r4;
    r4 = b[22];
    r5 = r5 - r4;
    r4 = r5 + r0;
    out[3] = r4;
    r8 = b[5];
    r4 = b[7];
    r4 = r8 + r4;
    out[4] = r4;
    r8 = b[25];
    r4 = b[27];
    r4 = r4 - r8;
    r0 = r0 + r4;
    r4 = r5 + r0;
    out[5] = r4;
    r8 = b[15];
    r4 = b[11];
    r5 = b[13];
    r4 = r8 + r4;
    r4 = r5 + r4;
    out[6] = r4;
    r4 = b[19];
    r0 = r0 + r4;
    out[7] = r0;
    r4 = b[3];
    out[8] = r4;
    r4 = b[29];
    r0 = r0 - r4;
    out[9] = r0;
    r4 = b[11];
    r0 = b[15];
    r0 = r4 + r0;
    out[10] = r0;
    r5 = b[31];
    r0 = b[23];
    r4 = b[27];
    r0 = r5 + r0;
    r0 = r4 + r0;
    out[11] = r0;
    r0 = b[7];
    out[12] = r0;
    r4 = b[23];
    r0 = b[31];
    r0 = r4 + r0;
    out[13] = r0;
    r0 = b[15];
    out[14] = r0;
    r0 = b[31];
    out[15] = r0;
    out[16] = r7;
    out[32] = r6;
    return 0;
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

/* Q24 butterfly stage: two 16-word blocks, coefficients c[0..7] reused for both.
 * Each step forms u = d + s and t = d - s, then t * c[k]. NonMatching: 102/102, same instructions; step 6
 * takes the last step's registers (diff 18 lines). */
void fn_800C5298(s32* dst, s32* src, s32* c) {
    s32 i;
    s32 s, d, u, t;

    for (i = 0; i < 2; i++) {
        s = src[0]; d = dst[0]; u = d + s; t = d - s; dst[0] = u; src[0] = (s32)(((s64)t * c[0]) >> 24);
        s = src[1]; d = dst[1]; u = d + s; t = d - s; dst[1] = u; src[1] = (s32)(((s64)t * c[1]) >> 24);
        s = src[2]; d = dst[2]; u = d + s; t = d - s; dst[2] = u; src[2] = (s32)(((s64)t * c[2]) >> 24);
        s = src[3]; d = dst[3]; u = d + s; t = d - s; dst[3] = u; src[3] = (s32)(((s64)t * c[3]) >> 24);
        s = src[4]; d = dst[4]; u = d + s; t = d - s; dst[4] = u; src[4] = (s32)(((s64)t * c[4]) >> 24);
        s = src[5]; d = dst[5]; u = d + s; t = d - s; dst[5] = u; src[5] = (s32)(((s64)t * c[5]) >> 24);
        s = src[6]; d = dst[6]; u = d + s; t = d - s; dst[6] = u; src[6] = (s32)(((s64)t * c[6]) >> 24);
        s = src[7]; d = dst[7]; u = d + s; t = d - s; dst[7] = u; src[7] = (s32)(((s64)t * c[7]) >> 24);
        dst += 16;
        src += 16;
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

/* Mid/side stereo: left = a + b, right = a - b over the long spectrum (count0 lines), the mixed layout (36 long
 * lines plus count1 lines of each short window from line 12) or the three short windows (count1 lines each).
 * From ChatGPT's candidate B (round 23), verified against the original in a PPC simulation (40/40) and by
 * ChatGPT (792 cases). NonMatching: 661/661, identical opcode sequence, register numbers differ. */
void fn_800C57B0(s32 count0, s32 count1, u32 flags, MP3ChannelBuf* g) {
    s32 i;
    s32 *left;
    s32 *right;
    s32 a, b;

    if (flags & 7) {
        /* Full selected region of the last 576-sample spectral block. */
        left  = g[0].xr;
        right = g[1].xr;
        for (i = 0; i < count0; i++) {
            a = *left;
            b = *right;
            *left++  = a + b;
            *right++ = a - b;
        }
    } else if (flags & 8) {
        /* Mixed/special mode: 36 spectral values + three other bands,
         * each beginning 12 words into its 576-word area. */
        left  = g[0].xr;
        right = g[1].xr;
        for (i = 0; i < 36; i++) {
            a = *left;
            b = *right;
            *left++  = a + b;
            *right++ = a - b;
        }

        left  = (s32*)((u8*)g + 0x30);
        right = (s32*)((u8*)g + 0x2430);
        for (i = 0; i < count1; i++) {
            a = *left;
            b = *right;
            *left++  = a + b;
            *right++ = a - b;
        }
        left  = (s32*)((u8*)g + 0x930);
        right = (s32*)((u8*)g + 0x2D30);
        for (i = 0; i < count1; i++) {
            a = *left;
            b = *right;
            *left++  = a + b;
            *right++ = a - b;
        }
        left  = (s32*)((u8*)g + 0x1230);
        right = (s32*)((u8*)g + 0x3630);
        for (i = 0; i < count1; i++) {
            a = *left;
            b = *right;
            *left++  = a + b;
            *right++ = a - b;
        }
    } else if (flags & 16) {
        /* Three complete 576-word regions, starting at region index 0. */
        left  = (s32*)((u8*)g + 0x0000);
        right = (s32*)((u8*)g + 0x2400);
        for (i = 0; i < count1; i++) {
            a = *left;
            b = *right;
            *left++  = a + b;
            *right++ = a - b;
        }
        left  = (s32*)((u8*)g + 0x0900);
        right = (s32*)((u8*)g + 0x2D00);
        for (i = 0; i < count1; i++) {
            a = *left;
            b = *right;
            *left++  = a + b;
            *right++ = a - b;
        }
        left  = (s32*)((u8*)g + 0x1200);
        right = (s32*)((u8*)g + 0x3600);
        for (i = 0; i < count1; i++) {
            a = *left;
            b = *right;
            *left++  = a + b;
            *right++ = a - b;
        }
    }
}

