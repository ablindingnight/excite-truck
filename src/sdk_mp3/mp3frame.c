/* Nintendo-supplied SDK MP3 (Layer III) decoder, 0x800BC538-0x800C6204 in REXE01 (the same tables ship in
 * the Wii Photo Channel). Library/module and file names unknown. It is three translation units: MWCC addresses
 * pooled tables from the start of each unit's .rodata, and the original does so from 0x802F34E8 (dequantizer)
 * and 0x802F3D60 (Huffman decoder), which puts unit boundaries at text 0x800BF048 and 0x800C0440.
 * Build: cflags_rvl_tp, GC/3.0a3, plus -use_lmw_stmw on -inline noauto (every function saves with stmw;
 * WENCGetEncodeData right after the decoder does not, so it is a separate object). */
/* Unit 1 of 3: header parsing, bit reading, stereo, alias reduction, frame decoding (0x800BC538-0x800BF048). */
#include "mp3dec.h"
#include "mp3frame_tables.inc"

static s32 lbl_8055DD88;
static s32 lbl_8055DD8C;

/* Intensity stereo for one value pair. MPEG-1: two Q20 ratio tables indexed by the scalefactor.
 * MPEG-2: on first use, copies one of two 33-entry ratio tables (by channel mode) into the decoder,
 * then odd scalefactors scale the right channel and copy it left, even ones scale into the left. */
void fn_800BD710(MP3Decoder* d, s32 ch, s32 sf, s32* r, s32* l) {
    s32 i;
    s32 k;

    if (d->lsf == 0) {
        *l = (s32)(((s64)*r * lbl_802F3258[sf]) >> 20);
        *r = (s32)(((s64)*r * lbl_802F3238[sf]) >> 20);
        return;
    }
    if (d->is_init == 0) {
        d->is_init = 1;
        if (ch == 1) {
            for (i = 0; i < 33; i++) {
                d->is_table[i] = lbl_802F3320[i];
            }
        } else {
            for (i = 0; i < 33; i++) {
                d->is_table[i] = lbl_802F3278[i];
            }
        }
    }
    if ((sf & 1) == 1) {
        k = d->is_table[(s16)(sf + 1)];
        *l = *r;
        *r = (s32)(((s64)*r * k) >> 20);
    } else {
        *l = (s32)(((s64)*r * d->is_table[sf]) >> 20);
    }
}

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

s32 MP3_FindSync(u32 ctx, u8* buf, s32* pos) {
    s32 word = MP3_GetBits(ctx, buf, 12, pos);

    while ((u16)word != 0xFFF) {
        word = (u16)(word << 1) | MP3_GetBits(ctx, buf, 1, pos);
        if (*pos > 0x3FFF) {
            return 0;
        }
    }
    return 1;
}

u32 MP3_GetBits(u32 unused, u8* buf, u32 n, s32* pos) {
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

u32 MP3_GetBitsRing(u8* buf, s32 n, u32* pos) {
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

s32 MP3_ParseHeader(MP3Decoder* d, u8* buf) {
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
