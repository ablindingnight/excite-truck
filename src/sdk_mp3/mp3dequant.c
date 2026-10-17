/* Nintendo-supplied SDK MP3 (Layer III) decoder, 0x800BC538-0x800C6204 in REXE01 (the same tables ship in
 * the Wii Photo Channel). Library/module and file names unknown. It is three translation units: MWCC addresses
 * pooled tables from the start of each unit's .rodata, and the original does so from 0x802F34E8 (dequantizer)
 * and 0x802F3D60 (Huffman decoder), which puts unit boundaries at text 0x800BF048 and 0x800C0440.
 * Build: cflags_rvl_tp, GC/3.0a3, plus -use_lmw_stmw on -inline noauto (every function saves with stmw;
 * WENCGetEncodeData right after the decoder does not, so it is a separate object). */
/* Unit 2 of 3: dequantization (0x800BF048-0x800C0440). */
#include "mp3dec.h"
#include "mp3dequant_tables.inc"

/* Dequantization of one granule for every channel (fn_800BF048). On first use, copies the scalefactor-band edges for
 * the stream's MPEG version and sample rate into the decoder; then per channel picks the long, short or mixed
 * path from the block flags, builds the line-to-band map on first use, and dequantizes each line:
 * |x| < 256 via the pow43 table, larger via the 32-segment quadratic table, scaled by the scalefactor exponent.
 * Behaviour verified identical to the original in a PPC simulation (76 random granules, including the original's
 * quirks in the mixed short part: no absolute value, and a second shift). NonMatching: 1278/1278, same opcodes. */
void MP3_Dequantize(MP3Decoder* d, u8* long_map, u8* short_map, u8* mixed_map, s16* long_edges, s16* short_edges,
                    u32* block_flags, MP3ChannelBuf* out, s16* in, s16* global_gain, s16* subblock_gain,
                    s16* sf_scale, s16* sf_long, s16 (*sf_short)[3], s16* preflag) {
    const s32* c;
    s16 idx;
    u32 e0;
    s32 k;
    u32 flags;
    const s16* lt;
    u16 e;
    s32 t;
    s32 i;
    s16 row;
    s32 cnt[3];
    s16 x;
    s32 b;
    s16 m;
    s16 gi;
    s32 p;
    s32 ch;
    const s16* st;
    s16 v;
    s32 w;
    s16 g;
    s32 r;

    if (d->unk9CC[3] == 0) {
        d->unk9CC[3] = 1;
        if (d->lsf == 0 && d->sr_index == 0) {
            d->band_count = 0x18;
            lt = lbl_802F3AD4;
            st = lbl_802F3BB0;
        } else if (d->lsf == 0 && d->sr_index == 1) {
            d->band_count = 0x16;
            lt = lbl_802F3B00;
            st = lbl_802F3BF8;
        } else if (d->lsf == 0 && d->sr_index == 2) {
            d->band_count = 0x1F;
            lt = lbl_802F3B2C;
            st = lbl_802F3C40;
        } else if (d->lsf == 1 && d->sr_index == 0) {
            d->band_count = 0x1D;
            lt = lbl_802F3B84;
            st = lbl_802F3C88;
        } else if (d->lsf == 1 && d->sr_index == 1) {
            d->band_count = 0x1E;
            lt = lbl_802F3B58;
            st = lbl_802F3CD0;
        } else {
            d->band_count = 0x1D;
            lt = lbl_802F3B84;
            st = lbl_802F3D18;
        }
        for (i = 0; i < 21; i++) {
            long_edges[i] = lt[i];
        }
        for (i = 0; i < 36; i++) {
            short_edges[i] = st[i];
        }
        d->long_end = long_edges[20];
        d->short_end = short_edges[35];
        d->short_bands = short_edges[35] / 3;
    }

    for (ch = 0; ch < d->channels; ch++) {
        row = *sf_scale;
        if (d->mode_ext & 2) {
            gi = *global_gain - 2;
        } else {
            gi = *global_gain;
        }
        if (gi < 0) {
            gi = 0;
        }
        e0 = lbl_802F38E8[gi];
        flags = *block_flags;
        g = lbl_80560DB0[gi & 3];

        if (flags & 7) {
            /* long blocks */
            if (d->unk9CC[1] == 0) {
                d->unk9CC[1] = 1;
                for (i = 0; i < d->long_end; i++) {
                    for (b = 0; b < 21; b++) {
                        if (i < long_edges[b]) {
                            break;
                        }
                    }
                    long_map[i] = b;
                }
            }
            if (e0 > 0x33) {
                for (i = 0; i < d->long_end; i++) {
                    out->xr[i] = 0;
                }
            } else {
                for (i = 0; i < d->long_end; i++) {
                    v = in[i];
                    if (v == 0) {
                        out->xr[i] = 0;
                        continue;
                    }
                    b = long_map[i];
                    idx = lbl_802F39E8[b] * *preflag + sf_long[b];
                    e = e0 + lbl_802F3A8C[row][idx];
                    m = lbl_802F3A00[row][idx];
                    x = v;
                    if (v < 0) {
                        x = -v;
                    }
                    if (x > 0x1FFF) {
                        x = 0x1FFF;
                    }
                    if ((x & 0x1F00) == 0) {
                        p = lbl_802F34E8[x];
                        if (m == 1) {
                            r = (s32)(((s64)p * g) >> 20);
                            if (e > 20) {
                                r >>= e - 20;
                            } else {
                                r <<= 20 - e;
                            }
                        } else {
                            t = (g * m) >> 4;
                            if (e > 20) {
                                r = (s32)(((s64)t * p) >> 24);
                                e -= 8;
                                if (e < 20) {
                                    r <<= 20 - e;
                                } else {
                                    r >>= e - 20;
                                }
                            } else {
                                r = (s32)(((s64)t * p) >> 20);
                                e -= 4;
                                if (e < 20) {
                                    r <<= 20 - e;
                                } else {
                                    r >>= e - 20;
                                }
                            }
                        }
                    } else {
                        c = lbl_8031C098[x >> 8];
                        p = ((s32)(((s64)(x * x) * c[0]) >> 20) + (s32)(((s64)c[1] * x) >> 20) + c[2] >> 4) + c[3];
                        if (m == 1) {
                            r = p * g;
                            if (e > 20) {
                                r >>= e - 20;
                            } else {
                                r <<= 20 - e;
                            }
                        } else {
                            t = g * m;
                            e -= 8;
                            r = (s32)(((s64)t * p) >> 8);
                            if (e < 20) {
                                r <<= 20 - e;
                            } else {
                                r >>= e - 20;
                            }
                        }
                    }
                    if (v < 0) {
                        out->xr[i] = -r;
                    } else {
                        out->xr[i] = r;
                    }
                }
                for (; i < 576; i++) {
                    out->xr[i] = 0;
                }
            }
        } else if (flags & 0x10) {
            /* short blocks */
            cnt[0] = -1;
            cnt[1] = -1;
            cnt[2] = -1;
            if (d->unk9CC[2] == 0) {
                d->unk9CC[2] = 1;
                for (i = 0; i < d->short_end; i++) {
                    for (b = 0; b < 36; b++) {
                        if (i < short_edges[b]) {
                            break;
                        }
                    }
                    short_map[i] = (b / 3) * 16 + b % 3;
                }
            }
            for (i = 0; i < d->short_end; i++) {
                v = in[i];
                x = v;
                if (v > 0x1FFF) {
                    x = 0x1FFF;
                }
                w = short_map[i] & 0xF;
                k = ++cnt[w];
                if (e0 > 0x33 || x == 0) {
                    out->sxr[w][k] = 0;
                    continue;
                }
                b = short_map[i] >> 4;
                idx = sf_short[b][w];
                e = lbl_802F3A8C[row][idx] + ((s16)(subblock_gain[w] * 2) + e0);
                m = lbl_802F3A00[row][idx];
                if (x < 0) {
                    x = -x;
                }
                if (x > 0x1FFF) {
                    x = 0x1FFF;
                }
                if ((x & 0x1F00) == 0) {
                    p = lbl_802F34E8[x];
                    if (m == 1) {
                        r = (s32)(((s64)p * g) >> 20);
                        if (e > 20) {
                            r >>= e - 20;
                        } else {
                            r <<= 20 - e;
                        }
                    } else {
                        t = (g * m) >> 4;
                        if (e > 20) {
                            r = (s32)(((s64)t * p) >> 24);
                            e -= 8;
                            if (e < 20) {
                                r <<= 20 - e;
                            } else {
                                r >>= e - 20;
                            }
                        } else {
                            r = (s32)(((s64)t * p) >> 20);
                            e -= 4;
                            if (e < 20) {
                                r <<= 20 - e;
                            } else {
                                r >>= e - 20;
                            }
                        }
                    }
                } else {
                    c = lbl_8031C098[x >> 8];
                    p = ((s32)(((s64)(x * x) * c[0]) >> 20) + (s32)(((s64)c[1] * x) >> 20) + c[2] >> 4) + c[3];
                    if (m == 1) {
                        r = p * g;
                        if (e > 20) {
                            r >>= e - 20;
                        } else {
                            r <<= 20 - e;
                        }
                    } else {
                        t = g * m;
                        e -= 8;
                        r = (s32)(((s64)t * p) >> 8);
                        if (e < 20) {
                            r <<= 20 - e;
                        } else {
                            r >>= e - 20;
                        }
                    }
                }
                if (v < 0) {
                    out->sxr[w][k] = -r;
                } else {
                    out->sxr[w][k] = r;
                }
            }
            for (i /= 3; i < 192; i++) {
                out->sxr[0][i] = 0;
                out->sxr[1][i] = 0;
                out->sxr[2][i] = 0;
            }
        } else {
            /* mixed blocks: 36 long-block lines, then short-block lines */
            cnt[0] = 11;
            cnt[1] = 11;
            cnt[2] = 11;
            if (d->unk9CC[0] == 0) {
                d->unk9CC[0] = 1;
                for (i = 0; i < 36; i++) {
                    for (b = 0; b < 21; b++) {
                        if (i < long_edges[b]) {
                            break;
                        }
                    }
                    mixed_map[i] = b;
                }
                for (i = 36; i < d->short_end; i++) {
                    for (b = 0; b < 36; b++) {
                        if (i < short_edges[b]) {
                            break;
                        }
                    }
                    mixed_map[i] = (b / 3) * 16 + b % 3;
                }
            }
            for (i = 0; i < 36; i++) {
                v = in[i];
                if (e0 > 0x33 || v == 0) {
                    out->xr[i] = 0;
                    continue;
                }
                b = mixed_map[i];
                idx = lbl_802F39E8[b] * *preflag + sf_long[b];
                e = e0 + lbl_802F3A8C[row][idx];
                m = lbl_802F3A00[row][idx];
                x = v;
                if (v < 0) {
                    x = -v;
                }
                if (x > 0x1FFF) {
                    x = 0x1FFF;
                }
                if (e > 0x33) {
                    out->xr[i] = 0;
                    continue;
                }
                if (x > 0x1FFF) {
                    x = 0x1FFF;
                }
                if ((x & 0x1F00) == 0) {
                    p = lbl_802F34E8[x];
                    if (m == 1) {
                        r = (s32)(((s64)p * g) >> 20);
                        if (e > 20) {
                            r >>= e - 20;
                        } else {
                            r <<= 20 - e;
                        }
                    } else {
                        t = (g * m) >> 4;
                        if (e > 20) {
                            r = (s32)(((s64)t * p) >> 24);
                            e -= 8;
                            if (e < 20) {
                                r <<= 20 - e;
                            } else {
                                r >>= e - 20;
                            }
                        } else {
                            r = (s32)(((s64)t * p) >> 20);
                            e -= 4;
                            if (e < 20) {
                                r <<= 20 - e;
                            } else {
                                r >>= e - 20;
                            }
                        }
                    }
                } else {
                    c = lbl_8031C098[x >> 8];
                    p = ((s32)(((s64)(x * x) * c[0]) >> 20) + (s32)(((s64)c[1] * x) >> 20) + c[2] >> 4) + c[3];
                    if (m == 1) {
                        r = p * g;
                        if (e > 20) {
                            r >>= e - 20;
                        } else {
                            r <<= 20 - e;
                        }
                    } else {
                        t = g * m;
                        e -= 8;
                        r = (s32)(((s64)t * p) >> 8);
                        if (e < 20) {
                            r <<= 20 - e;
                        } else {
                            r >>= e - 20;
                        }
                    }
                }
                if (v < 0) {
                    out->xr[i] = -r;
                } else {
                    out->xr[i] = r;
                }
            }
            for (i = 36; i < d->short_end; i++) {
                v = in[i];
                w = mixed_map[i] & 0xF;
                k = ++cnt[w];
                if (e0 > 0x33 || v == 0) {
                    out->sxr[w][k] = 0;
                    continue;
                }
                idx = sf_short[short_map[i] >> 4][w];
                e = lbl_802F3A8C[row][idx] + ((s16)(subblock_gain[w] * 2) + e0);
                m = lbl_802F3A00[row][idx];
                if (e > 0x33) {
                    out->sxr[w][k] = 0;
                    continue;
                }
                /* NB: no absolute value in this part of the original */
                x = v;
                if (v > 0x1FFF) {
                    x = 0x1FFF;
                }
                if ((x & 0x1F00) == 0) {
                    p = lbl_802F34E8[x];
                    if (m == 1) {
                        r = (s32)(((s64)p * g) >> 20);
                        if (e > 20) {
                            r >>= e - 20;
                        } else {
                            r <<= 20 - e;
                        }
                    } else {
                        t = (g * m) >> 4;
                        if (e > 20) {
                            e -= 8;
                            r = (s32)(((s64)t * p) >> 24);
                            if (e < 20) {
                                r <<= 20 - e;
                            } else {
                                r >>= e - 20;
                            }
                        } else {
                            r = (s32)(((s64)t * p) >> 20);
                            e -= 4;
                            if (e < 20) {
                                r <<= 20 - e;
                            } else {
                                r >>= e - 20;
                            }
                            r <<= 20 - e; /* NB: the original shifts a second time here */
                        }
                    }
                } else {
                    c = lbl_8031C098[x >> 8];
                    p = ((s32)(((s64)(x * x) * c[0]) >> 20) + (s32)(((s64)c[1] * x) >> 20) + c[2] >> 4) + c[3];
                    if (m == 1) {
                        r = p * g;
                        if (e > 20) {
                            r >>= e - 20;
                        } else {
                            r <<= 20 - e;
                        }
                    } else {
                        t = g * m;
                        e -= 8;
                        r = (s32)(((s64)t * p) >> 8);
                        if (e < 20) {
                            r <<= 20 - e;
                        } else {
                            r >>= e - 20;
                        }
                    }
                }
                if (v < 0) {
                    out->sxr[w][k] = -r;
                } else {
                    out->sxr[w][k] = r;
                }
            }
            for (i /= 3; i < 192; i++) {
                out->sxr[0][i] = 0;
                out->sxr[1][i] = 0;
                out->sxr[2][i] = 0;
            }
        }
        block_flags++;
        out++;
        sf_scale++;
        global_gain++;
        in += 600;
        sf_long += 21;
        preflag++;
        sf_short += 12;
        subblock_gain += 3;
    }
}
