/* Nintendo-supplied SDK MP3 (Layer III) decoder, 0x800BC538-0x800C6204 in REXE01 (the same tables
 * ship in the Wii Photo Channel). Library/module name unknown. Work in progress: functions are being
 * decompiled out of address order and will be reordered when the unit is registered.
 * Build: cflags_rvl_tp, GC/3.0a3, plus -use_lmw_stmw on -inline noauto (every function saves with
 * stmw; WENCGetEncodeData right after it does not, so it is a separate object).
 * Matching so far: MP3_Create, MP3_GetInfo, MP3_Delete, MP3_Fill, MP3_ArenaInit/Alloc/Set. Close (register or
 * scheduling only): FindSync, GetBits, GetBitsRing, ParseHeader, HuffLookup, PeekBitsRing,
 * DecodeQuads (0x800C08A8, 176/176), DecodeQuadsB (0x800C0B68, 164/164). Drafted: DecodeBigValues
 * (0x800C0440, 256 vs 255; needs the .rodata tables defined here so MWCC pools their addresses). */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef long long s64;


typedef struct HuffTable {
    u16** nodes;
} HuffTable;

typedef struct HuffMeta {
    s16 long_bands[2][3][16];   /* 0x0 */
    s16 short_bands[2][3][20];  /* 0xC0 */
    s16 unk1B0[16];             /* 0x1B0 */
    s16 depth[18];              /* 0x1D0 */
    u32 masks[25];              /* 0x1F4: (1 << n) - 1, n = 0..24 */
} HuffMeta;

#define NULL 0
#ifndef MP3_STATIC
#define MP3_STATIC static
#endif
#include "mp3_tables.inc"

static s32 lbl_8055DD88;
static s32 lbl_8055DD8C;
static u32 lbl_8055DD90;  /* arena end */
static u32 lbl_8055DD94;  /* arena current */

typedef struct MP3Info {
    s32 mpeg1;        /* 0x0 */
    s32 sample_rate;  /* 0x4 */
    s32 channels;     /* 0x8 */
    s32 frame_bytes;  /* 0xC */
    s32 out_bytes;    /* 0x10 */
} MP3Info;

typedef struct MP3Decoder {
    s8 channels;              /* 0x0 */
    u8 pad1[3];
    s32 lsf;                  /* 0x4: MPEG-2 low sampling frequency */
    s32 unk8;                 /* 0x8 */
    s32 unkC;                 /* 0xC */
    u8 pad10[4];
    s32 unk14;                /* 0x14 */
    s32 unk18;                /* 0x18 */
    s16 sr_index;             /* 0x1C */
    u8 pad1E[0x278 - 0x1E];
    u32 unk278;               /* 0x278 */
    u16 unk27C;               /* 0x27C */
    u8 pad27E[0x9CC - 0x27E];
    s32 unk9CC[8];            /* 0x9CC */
    u8 pad9EC[0xED70 - 0x9EC];
    s32 frame_bytes;          /* 0xED70 */
    s32 out_bytes;            /* 0xED74 */
} MP3Decoder;






u32 MP3_GetBits(u32 unused, const u8* buf, u32 n, s32* pos);

/* Per-channel granule buffer (0x2400 bytes); the frame decoder passes decoder + 0xAB0. */
typedef struct MP3ChannelBuf {
    u8 pad[0x1B00];
    s32 xr[576];              /* 0x1B00: dequantized spectrum */
} MP3ChannelBuf;

/* Alias reduction between subbands (Q24 butterflies with the cs^2 / cs*ca tables). With a non-zero
 * block-type field the butterflies run on every subband boundary below sblim and the spectrum from
 * start up to sblim*18 is cleared; otherwise only the boundary between subbands 0 and 1 is processed. */
void MP3_Antialias(s32 sblim, s32 start, s8 channels, s32 mixed, MP3ChannelBuf* g) {
    s8 ch;
    s8 sb;
    s32 i;
    s32 j;
    s16 idx;
    s32* lo;
    s32* hi;
    s32 bu;
    s32 bd;

    if (mixed & 7) {
        for (ch = 0; ch < channels; ch++, g++) {
            for (sb = 1; sb < sblim; sb++) {
                idx = (sb << 4) + (sb << 1);
                hi = &g->xr[idx];
                lo = &g->xr[idx - 1];
                for (i = 0; i < 8; i++) {
                    bd = hi[i];
                    bu = *lo;
                    *lo-- = (s32)(((s64)bu * lbl_802F33C8[i]) >> 24) - (s32)(((s64)bd * lbl_802F33E8[i]) >> 24);
                    hi[i] = (s32)(((s64)bd * lbl_802F33C8[i]) >> 24) + (s32)(((s64)bu * lbl_802F33E8[i]) >> 24);
                }
            }
            for (j = start; j < sblim * 18; j++) {
                g->xr[j] = 0;
            }
        }
    } else {
        for (ch = 0; ch < channels; ch++, g++) {
            hi = &g->xr[18];
            lo = &g->xr[17];
            for (i = 0; i < 8; i++) {
                bd = hi[i];
                bu = *lo;
                *lo-- = (s32)(((s64)bu * lbl_802F33C8[i]) >> 24) - (s32)(((s64)bd * lbl_802F33E8[i]) >> 24);
                hi[i] = (s32)(((s64)bd * lbl_802F33C8[i]) >> 24) + (s32)(((s64)bu * lbl_802F33E8[i]) >> 24);
            }
        }
    }
}

s32 MP3_FindSync(u32 ctx, const u8* buf, s32* pos) {
    s32 word = MP3_GetBits(ctx, buf, 12, pos);

    while ((u16)word != 0xFFF) {
        word = (u16)(word << 1) | MP3_GetBits(ctx, buf, 1, pos);
        if (*pos > 0x3FFF) {
            return 0;
        }
    }
    return 1;
}

u32 MP3_GetBits(u32 unused, const u8* buf, u32 n, s32* pos) {
    u16 result = 0;
    s32 left = n;
    u32 avail;
    u32 take;

    buf += *pos >> 3;
    avail = 8 - (*pos & 7);
    while (left > 0) {
        if (avail == 0) {
            avail = 8;
            buf++;
        }
        take = left < avail ? left : avail;
        result |= ((lbl_802F3408[avail] & *buf) >> (avail - take)) << (left - take);
        avail -= take;
        left -= take;
    }
    *pos += n;
    return result;
}

u32 MP3_GetBitsRing(const u8* buf, s32 n, u32* pos) {
    u16 result = 0;
    u32 p;
    u32 idx;
    u32 avail;
    u32 take;

    if (n == 0) {
        return 0;
    }
    p = *pos;
    *pos = p + n;
    idx = (p >> 3) & 0x1FFF;
    avail = 8 - (p & 7);
    while (n > 0) {
        if (avail == 0) {
            idx = (idx + 1) & 0x1FFF;
            avail = 8;
        }
        take = n < avail ? n : avail;
        result |= ((lbl_802F3408[avail] & buf[idx]) >> (avail - take)) << (n - take);
        avail -= take;
        n -= take;
    }
    return result;
}

void MP3_GetInfo(MP3Decoder* d, MP3Info* info) {
    s32 rate = 44100;

    info->mpeg1 = d->lsf == 0;
    if (d->sr_index == 0) {
        rate = 44100;
    } else if (d->sr_index == 1) {
        rate = 48000;
    } else if (d->sr_index == 2) {
        rate = 32000;
    }
    if (info->mpeg1 == 0) {
        rate /= 2;
    }
    info->sample_rate = rate;
    info->channels = d->channels;
    info->out_bytes = d->out_bytes;
    info->frame_bytes = d->frame_bytes;
}

s32 MP3_ParseHeader(MP3Decoder* d, const u8* buf) {
    s32 pos = 0;
    s16 id;
    u16 bits;
    s16 bitrate;
    s32 size;

    if ((s16)MP3_FindSync(0x400, buf, &pos) == 0) {
        return 8;
    }
    id = MP3_GetBits(d->unk278, buf, 1, &pos);
    if ((s16)MP3_GetBits(d->unk278, buf, 2, &pos) != 1) {
        return 0xD;
    }
    if (id == 0) {
        d->lsf = 1;
    } else {
        d->lsf = 0;
    }
    MP3_GetBits(d->unk278, buf, 1, &pos);
    bits = MP3_GetBits(d->unk278, buf, 16, &pos);
    bitrate = (bits >> 12) & 0xF;
    if (bitrate == 0) {
        return 0xE;
    }
    if (bitrate == 0xF) {
        return 0xE;
    }
    d->sr_index = (bits >> 10) & 3;
    if (d->sr_index == 3) {
        return 0xF;
    }
    if ((s16)((bits >> 6) & 3) == 3) {
        d->channels = 1;
    } else {
        d->channels = 2;
    }
    if (d->lsf == 0) {
        size = lbl_802F3418[d->sr_index][bitrate] * 8;
    } else {
        size = lbl_802F3478[d->sr_index][bitrate] * 8;
    }
    d->frame_bytes = (size + 8) / 8;
    if ((size + 8) & 7) {
        d->frame_bytes++;
    }
    if (d->lsf == 0) {
        d->out_bytes = d->channels * 0x900;
    } else {
        d->out_bytes = d->channels * 0x480;
    }
    return 0;
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

void MP3_ArenaInit(s32 base, s32 size);
void* MP3_ArenaAlloc(u32 size);

MP3Decoder* MP3_Create(const u8* header, u32 b, s32 buf, s32 size) {
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

void MP3_ArenaSet(u32 p);

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



u32 MP3_HuffLookup(s32 bits, s32 n, const HuffTable* t) {
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

u32 MP3_PeekBitsRing(const u8* buf, u32 n, u32* pos) {
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
    return (u16)((((b0 << 16) | (b1 << 8) | b2) & lbl_802F3D60.masks[sh]) >> (sh - n));
}


typedef struct BitStream {
    u8 pad0[0xC];
    u32 pos;   /* 0xC */
    u32 end;   /* 0x10 */
} BitStream;

static inline s32 MP3_GetSign(BitStream* bs, const u8* buf) {
    u32 p = bs->pos;
    s32 avail;

    bs->pos = p + 1;
    avail = 8 - (p & 7);
    return (buf[(p >> 3) & 0x1FFF] >> (avail - 1)) & 1;
}

void MP3_DecodeQuads(BitStream* bs, s32 end, const u8* buf, s16* out, s32 i, s32 track, s16* zero_start) {
    s16* q = out + i;

    for (; i < end; i += 4, q += 4) {
        u32 p = bs->pos;
        u32 idx;
        s32 sh;
        u32 code;
        u8 e;
        s16 v;
        s16 w;
        s16 x;
        s16 y;

        bs->pos = p + 6;
        idx = (p >> 3) & 0x1FFF;
        sh = 16 - (p & 7);
        code = (u16)((((buf[idx] << 8) | buf[(idx + 1) & 0x1FFF]) & lbl_802F3D60.masks[sh]) >> (sh - 6));
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
        if (bs->pos > bs->end) {
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

void MP3_DecodeQuadsB(BitStream* bs, s32 end, const u8* buf, s16* out, s32 i, s32 track, s16* zero_start) {
    s16* q = out + i;

    for (; i < end; i += 4, q += 4) {
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
        if (bs->pos > bs->end) {
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



void MP3_DecodeQuads(BitStream* bs, s32 end, const u8* buf, s16* out, s32 i, s32 track, s16* zero_start);
void MP3_DecodeQuadsB(BitStream* bs, s32 end, const u8* buf, s16* out, s32 i, s32 track, s16* zero_start);

typedef struct HuffState {
    u8 pad0[0x4];
    s32 lsf;      /* 0x4 */
    u8 pad8[0x4];
    u32 pos;      /* 0xC */
    u32 end;      /* 0x10 */
    u8 pad14[0x8];
    s16 sr_index; /* 0x1C */
    u8 pad1E[0x288 - 0x1E];
    s16 count1_end; /* 0x288 */
} HuffState;

s32 MP3_DecodeBigValues(HuffState* d, const u8* buf, s16* out, s32 big_values, u32 block_flags, s32 region0,
                        s32 region1, s16 count1_table, s32 track, s16* table_select, s16* zero_start) {
    s32 mixed = block_flags & 1;
    s16 region1_end;
    s16 region2_end;
    s32 limit;
    s32 nregions;
    s32 region;
    s32 i;
    s32 end;
    s16* q = out;

    if (mixed) {
        region1_end = lbl_802F3D60.long_bands[d->lsf][d->sr_index][region0];
        region2_end = lbl_802F3D60.short_bands[d->lsf][d->sr_index][region0 + region1];
    } else if (block_flags & 0x10) {
        region1_end = 36;
    } else {
        region1_end = lbl_80559270[d->lsf];
    }
    limit = big_values * 2;
    i = 0;
    nregions = 2 + (mixed != 0);
    for (region = 0; region < nregions; region++, table_select++) {
        s32 tbl = *table_select;
        s32 linbits = lbl_8031C298[tbl];
        const HuffTable* ht;

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
            s32 bits = MP3_PeekBitsRing(buf, 16, &d->pos) << 16;
            u32 r;
            s16 x;
            s16 y;

            if (tbl == 13 || tbl == 16) {
                u32 p = d->pos;
                u32 idx = (p >> 3) & 0x1FFF;
                s32 sh = 16 - (p & 7);
                u32 b0;
                u32 b1;

                d->pos = p + 4;
                b0 = buf[idx];
                idx = (idx + 1) & 0x1FFF;
                b1 = buf[idx];
                d->pos -= 20;
                bits |= ((((b0 << 8) | b1) & lbl_802F3D60.masks[sh]) >> (sh - 4) & 0xF) << 12;
            } else {
                d->pos -= 16;
            }
            r = MP3_HuffLookup(bits, lbl_802F3D60.depth[tbl], ht);
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
            if (d->pos > d->end) {
                if (track == 1 && *zero_start == 0) {
                    *zero_start = i;
                }
                q[0] = 0;
                q[1] = 0;
            }
        }
    }
    if (count1_table == 0) {
        MP3_DecodeQuads((BitStream*)d, d->count1_end, buf, out, i, track, zero_start);
    } else {
        MP3_DecodeQuadsB((BitStream*)d, d->count1_end, buf, out, i, track, zero_start);
    }
    return 0;
}
