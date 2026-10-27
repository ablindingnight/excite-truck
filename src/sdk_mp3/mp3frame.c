/* Nintendo-supplied SDK MP3 (Layer III) decoder, 0x800BC538-0x800C6204 in REXE01 (the same tables ship in
 * the Wii Photo Channel). Library/module and file names unknown. It is four translation units: MWCC addresses
 * pooled tables from the start of each unit's .rodata, and the original does so from 0x802F3418 (frame decoder),
 * 0x802F34E8 (dequantizer) and 0x802F3D60 (Huffman decoder), which puts unit boundaries at text 0x800BDF4C,
 * 0x800BF048 and 0x800C0440.
 * Build: cflags_rvl_tp, GC/3.0a3, plus -use_lmw_stmw on -inline noauto (every function saves with stmw;
 * WENCGetEncodeData right after the decoder does not, so it is a separate object). */
/* Unit 1 of 4: intensity stereo, alias reduction, bit reading (0x800BC538-0x800BDF4C). */
#include "mp3dec.h"
#include "mp3frame_tables.inc"

/* Intensity stereo (fn_800BC538): for lines at or above the intensity bound, rebuild the right channel from the
 * left with fn_800BD710. Long, short and mixed layouts; the band-limit tables are copied into the decoder on first
 * use (short rows from twelve pooled arrays at 0x8031BFA8). Behaviour verified identical to the original in a PPC
 * simulation (60 random cases). NonMatching: 1142/1142, same opcodes. */
void MP3_IntensityStereo(MP3Decoder* d, u8* long_map, s16* long_edges, u32 flags, MP3ChannelBuf* out, s16* sf_long,
                         s16 (*sf_short)[3], s16* is_scale, s16 bound) {
    s32 b;
    s32 w;
    s32 band;
    s32 i;
    s32 lim;

    if (flags & 7) {
        i = d->unk9CC[5] == 0;
        if (i) {
            d->unk9CC[5] = 1;
            for (b = 0; b < 21; b++) {
                d->is_lb[b + 1] = long_edges[b];
            }
            d->is_lb[0] = 0;
        }
        for (i = 0; i < d->long_end; i++) {
            if (out[0].xr[i] == 0) {
                out[1].xr[i] = 0;
                continue;
            }
            for (b = 20; b >= 0; b--) {
                if (bound > d->is_lb[b]) {
                    lim = d->is_lb[b + 1];
                    break;
                }
            }
            if (i >= lim) {
                fn_800BD710(d, is_scale[1], sf_long[21 + long_map[i]], &out[0].xr[i], &out[1].xr[i]);
            }
        }
    } else if (flags & 0x10) {
        if (d->unk9CC[6] == 0) {
            d->unk9CC[6] = 1;
            if (d->lsf == 0 && d->sr_index == 0) {
                for (b = 0; b < 12; b++) {
                    d->is_sb[b] = lbl_8031BFA8[b];
                    d->is_sw[b] = lbl_8031BFC0[b];
                }
            } else if (d->lsf == 0 && d->sr_index == 1) {
                for (b = 0; b < 12; b++) {
                    d->is_sb[b] = lbl_8031BFD0[b];
                    d->is_sw[b] = lbl_8031BFE8[b];
                }
            } else if (d->lsf == 0 && d->sr_index == 2) {
                for (b = 0; b < 12; b++) {
                    d->is_sb[b] = lbl_8031BFF8[b];
                    d->is_sw[b] = lbl_8031C010[b];
                }
            } else if (d->lsf == 1 && d->sr_index == 0) {
                for (b = 0; b < 12; b++) {
                    d->is_sb[b] = lbl_8031C020[b];
                    d->is_sw[b] = lbl_8031C038[b];
                }
            } else if (d->lsf == 1 && d->sr_index == 1) {
                for (b = 0; b < 12; b++) {
                    d->is_sb[b] = lbl_8031C048[b];
                    d->is_sw[b] = lbl_8031C060[b];
                }
            } else {
                for (b = 0; b < 12; b++) {
                    d->is_sb[b] = lbl_8031C070[b];
                    d->is_sw[b] = lbl_8031C088[b];
                }
            }
        }
        for (i = 0; i < d->short_bands; i++) {
            if (out[0].xr[i] == 0) {
                out[1].xr[i] = 0;
                continue;
            }
            for (b = 0; b < 12; b++) {
                if (bound > d->is_sb[b]) {
                    lim = d->is_sw[b];
                    break;
                }
            }
            if (i >= lim) {
                for (b = 11; b >= 0; b--) {
                    if (i < d->is_sw[b]) {
                        band = 11 - b;
                        break;
                    }
                }
                for (w = 0; w < 3; w++) {
                    fn_800BD710(d, is_scale[1], sf_short[12 + band][w], &out[0].sxr[w][i], &out[1].sxr[w][i]);
                }
            }
        }
    } else {
        if (d->unk9CC[4] == 0) {
            d->unk9CC[4] = 1;
            if (d->lsf == 0) {
                for (b = 0; b < 8; b++) {
                    d->is_mb[b] = lbl_80559250[b];
                    d->is_mlim[b] = lbl_80559258[b];
                }
                if (d->sr_index == 0) {
                    for (b = 0; b < 12; b++) {
                        d->is_sb[b] = lbl_8031BFA8[b];
                        d->is_sw[b] = lbl_8031BFC0[b];
                    }
                } else if (d->sr_index == 1) {
                    for (b = 0; b < 12; b++) {
                        d->is_sb[b] = lbl_8031BFD0[b];
                        d->is_sw[b] = lbl_8031BFE8[b];
                    }
                } else {
                    for (b = 0; b < 12; b++) {
                        d->is_sb[b] = lbl_8031BFF8[b];
                        d->is_sw[b] = lbl_8031C010[b];
                    }
                }
            } else {
                for (b = 0; b < 6; b++) {
                    d->is_mb[b] = lbl_80559260[b];
                    d->is_mlim[b] = lbl_80559268[b];
                }
                if (d->sr_index == 0) {
                    for (b = 0; b < 12; b++) {
                        d->is_sb[b] = lbl_8031C020[b];
                        d->is_sw[b] = lbl_8031C038[b];
                    }
                } else if (d->sr_index == 1) {
                    for (b = 0; b < 12; b++) {
                        d->is_sb[b] = lbl_8031C048[b];
                        d->is_sw[b] = lbl_8031C060[b];
                    }
                } else {
                    for (b = 0; b < 12; b++) {
                        d->is_sb[b] = lbl_8031C070[b];
                        d->is_sw[b] = lbl_8031C088[b];
                    }
                }
            }
        }
        for (i = 0; i < 36; i++) {
            for (b = 0; b < 8; b++) {
                if (bound > d->is_mb[b]) {
                    lim = d->is_mlim[b];
                    break;
                }
            }
            if (i >= lim) {
                for (b = 0; b < 8; b++) {
                    if (i < d->is_mlim[7 - b]) {
                        band = b;
                        break;
                    }
                }
                fn_800BD710(d, is_scale[1], sf_long[21 + band], &out[0].xr[i], &out[1].xr[i]);
            }
        }
        for (i = 12; i < d->short_bands; i++) {
            for (b = 0; b < 10; b++) {
                if (bound > d->is_sb[b]) {
                    lim = d->is_sw[b];
                    break;
                }
            }
            if (i >= lim) {
                for (b = 8; b >= 0; b--) {
                    if (i < d->is_sw[b]) {
                        band = 11 - b;
                        break;
                    }
                }
                for (w = 0; w < 3; w++) {
                    fn_800BD710(d, is_scale[1], sf_short[12 + band][w], &out[0].sxr[w][i], &out[1].sxr[w][i]);
                }
            }
        }
    }
}

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

/* Find the next frame sync word (fn_800BDD7C): twelve 1 bits after at least four 0 bits. Matches. */
s32 MP3_FindSync(u32 ctx, u8* buf, s32* pos) {
    s32 word = MP3_GetBits(ctx, buf, 12, pos);

    while ((u16)word != 0xFFF) {
        word = (word << 1) & 0xFFFF;
        word |= MP3_GetBits(ctx, buf, 1, pos);
        if (*pos > 0x3FFF) {
            return 0;
        }
    }
    return 1;
}

u32 MP3_GetBits(u32 unused, u8* buf, s16 n, s32* pos) {
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

/* Read n bits from the main-data ring buffer (fn_800BDEB4). Matches (in-place shift of the masked byte from
 * DeepSeek, round 52). */
u32 MP3_GetBitsRing(u8* buf, s32 n, u32* pos) {
    u16 result = 0;
    u32 p;
    u32 idx;
    u32 avail;
    u32 take;
    s32 val;

    if (n == 0) {
        return 0;
    }
    p = *pos;
    idx = (p >> 3) & 0x1FFF;
    avail = 8 - (p & 7);
    *pos = p + n;
    while (n > 0) {
        if (avail == 0) {
            idx++;
            idx &= 0x1FFF;
            avail = 8;
        }
        take = n < avail ? n : avail;
        val = lbl_802F3408[avail] & buf[idx];
        val >>= avail - take;
        result |= val << (n - take);
        avail -= take;
        n -= take;
    }
    return result;
}
