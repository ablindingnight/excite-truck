/* Nintendo-supplied SDK MP3 decoder, unit 2 of 4: frame decoding, stream info, header parsing
 * (0x800BDF4C-0x800BF048). See mp3frame.c for the unit layout. */
#include "mp3dec.h"
#include "mp3decode_tables.inc"

static s16* lbl_8055DD88;
static s32 lbl_8055DD8C;

/* Frame decode (fn_800BDF4C): header, side info, main data, then per granule the scalefactors, Huffman
 * decode, dequantization, stereo processing, antialias, IMDCT and synthesis. Draft: 930 of 933; behaviour verified against the original in a PPC simulation
 * (200 random frames, heavy stages stubbed, bit readers from the original). */
s32 MP3_DecodeFrame(MP3Decoder* d, s16* out, u8* buf, u32 ctx, s32 unused, s32* frame_bytes, s32 mode) {
    s16 id;
    s16 prot;
    s16 bitrate;
    s16 stereo;
    s16 padding;
    s16 mdb;
    u8 ngr;
    s8 gr;
    s8 ch;
    u32 bits;
    s32 start;
    s32 frame_bits;
    s32 end;
    s32 n;
    s16 i;
    u8* p;
    s16 bound[2];
    s8 mixed[2][2];
    s8 window_switching[2][2];
    s8 block_type[2][2];
    s16 preflag[2][2];
    s16 big_values[2][2];
    s16 global_gain[2][2];
    s16 sf_scale[2][2];
    s16 part2_3[2][2];
    s16 region1[2][2];
    s16 region0[2][2];
    s16 is_scale[2][2];
    s16 sfc[2][2];
    s16 count1_table[2][2];
    u32 flags[2][2];
    s16 scfsi[2][4];
    s16 subblock_gain[2][2][3];
    s16 table_select[2][2][3];

    d->unk278 = ctx;
    d->unk8 = 0;
    lbl_8055DD88 = out;
    if ((s16)MP3_FindSync(d->unk278, buf, &d->unk8) == 0) {
        return 8;
    }
    id = MP3_GetBits(d->unk278, buf, 1, &d->unk8);
    if ((s16)MP3_GetBits(d->unk278, buf, 2, &d->unk8) != 1) {
        return 0xD;
    }
    start = d->unk8 - 15;
    if (start < 0) {
        start += d->unk278 * 8;
    }
    if (id == 0) {
        ngr = 1;
        d->lsf = 1;
    } else {
        ngr = 2;
        d->lsf = 0;
    }
    prot = MP3_GetBits(d->unk278, buf, 1, &d->unk8);
    bits = MP3_GetBits(d->unk278, buf, 16, &d->unk8);
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
    stereo = (bits >> 6) & 3;
    padding = (bits >> 9) & 1;
    if (stereo == 3) {
        d->channels = 1;
    } else {
        d->channels = 2;
    }
    d->mode_ext = (bits >> 4) & 3;
    if (d->lsf == 0) {
        frame_bits = lbl_802F3418[d->sr_index][bitrate] * 8;
    } else {
        frame_bits = lbl_802F3478[d->sr_index][bitrate] * 8;
    }
    if (padding == 1) {
        frame_bits += 8;
    }
    *frame_bytes = frame_bits / 8;
    end = start + frame_bits;
    if (prot == 0) {
        MP3_GetBits(d->unk278, buf, 16, &d->unk8);
    }
    mdb = MP3_GetBits(d->unk278, buf, (s16)(9 - d->lsf), &d->unk8);
    if (stereo == 3) {
        MP3_GetBits(d->unk278, buf, (s16)(5 - d->lsf * 4), &d->unk8);
    } else {
        MP3_GetBits(d->unk278, buf, (s16)(3 - d->lsf), &d->unk8);
    }
    if (d->lsf == 0) {
        for (ch = 0; ch < d->channels; ch++) {
            bits = MP3_GetBits(d->unk278, buf, 4, &d->unk8);
            scfsi[ch][0] = (bits >> 3) & 1;
            scfsi[ch][1] = (bits >> 2) & 1;
            scfsi[ch][2] = (bits >> 1) & 1;
            scfsi[ch][3] = bits & 1;
        }
    }

    for (gr = 0; gr < ngr; gr++) {
        for (ch = 0; ch < d->channels; ch++) {
            part2_3[gr][ch] = MP3_GetBits(d->unk278, buf, 12, &d->unk8);
            big_values[gr][ch] = MP3_GetBits(d->unk278, buf, 9, &d->unk8);
            global_gain[gr][ch] = MP3_GetBits(d->unk278, buf, 8, &d->unk8);
            if (d->lsf == 0) {
                sfc[gr][ch] = MP3_GetBits(d->unk278, buf, 4, &d->unk8);
            } else {
                sfc[gr][ch] = MP3_GetBits(d->unk278, buf, 9, &d->unk8);
            }
            window_switching[gr][ch] = MP3_GetBits(d->unk278, buf, 1, &d->unk8);
            if (window_switching[gr][ch] == 1) {
                block_type[gr][ch] = MP3_GetBits(d->unk278, buf, 2, &d->unk8);
                mixed[gr][ch] = MP3_GetBits(d->unk278, buf, 1, &d->unk8);
                bits = MP3_GetBits(d->unk278, buf, 10, &d->unk8);
                table_select[gr][ch][0] = (bits >> 5) & 0x1F;
                table_select[gr][ch][1] = bits & 0x1F;
                bits = MP3_GetBits(d->unk278, buf, 9, &d->unk8);
                subblock_gain[gr][ch][0] = (bits >> 6) & 7;
                subblock_gain[gr][ch][1] = (bits >> 3) & 7;
                subblock_gain[gr][ch][2] = bits & 7;
                if (block_type[gr][ch] == 0) {
                    flags[gr][ch] = 1;
                } else if (block_type[gr][ch] == 1) {
                    flags[gr][ch] = 2;
                } else if (block_type[gr][ch] == 3) {
                    flags[gr][ch] = 4;
                } else if (mixed[gr][ch] != 0) {
                    flags[gr][ch] = 8;
                } else {
                    flags[gr][ch] = 0x10;
                }
            } else {
                block_type[gr][ch] = 0;
                mixed[gr][ch] = 0;
                bits = MP3_GetBits(d->unk278, buf, 15, &d->unk8);
                table_select[gr][ch][0] = (bits >> 10) & 0x1F;
                table_select[gr][ch][1] = (bits >> 5) & 0x1F;
                table_select[gr][ch][2] = bits & 0x1F;
                bits = MP3_GetBits(d->unk278, buf, 7, &d->unk8);
                region0[gr][ch] = (bits >> 3) & 0xF;
                region1[gr][ch] = bits & 7;
                flags[gr][ch] = 1;
            }
            if (d->lsf == 0) {
                preflag[gr][ch] = MP3_GetBits(d->unk278, buf, 1, &d->unk8);
            }
            sf_scale[gr][ch] = MP3_GetBits(d->unk278, buf, 1, &d->unk8);
            count1_table[gr][ch] = MP3_GetBits(d->unk278, buf, 1, &d->unk8);
        }
    }

    if (d->unk8 < 0) {
        n = (end >> 3) - ((d->unk8 >> 3) + d->unk278);
    } else {
        n = (end >> 3) - (d->unk8 >> 3);
    }
    d->unk18 = d->unk14 >> 3;
    p = buf + (d->unk8 >> 3);
    while (n != 0) {
        d->ring[d->unk18 & 0x1FFF] = *p++;
        d->unk18++;
        n--;
    }
    d->unk14 = (d->unk18 & 0x1FFF) << 3;
    if (d->lsf == 0) {
        d->unk10 = d->unk14 - mdb * 8 - (frame_bits - ((const s16(*)[2])lbl_80560DA0)[d->channels - 1][prot]);
    } else {
        d->unk10 = d->unk14 - mdb * 8 - (frame_bits - ((const s16(*)[2])lbl_80560DA8)[d->channels - 1][prot]);
    }
    d->count1_end = ((const s16(*)[3])lbl_802F34D8)[d->lsf][d->sr_index];
    if (d->unk10 < 0 && d->unkC == 0 && 0x200000 - d->unk10 - d->unkC > 0x200000) {
        for (gr = 0; gr < ngr; gr++) {
            for (i = 0; i < 0x480; i++) {
                *lbl_8055DD88++ = 0;
            }
        }
        return 0;
    }

    for (gr = 0; gr < ngr; gr++) {
        for (ch = 0; ch < d->channels; ch++) {
            fn_800C3DA0(d, d->ring, flags[gr][ch], d->sf_long[gr][ch], d->sf_long[0][ch], d->sf_short[gr][ch][0],
                        part2_3[gr][ch], sfc[gr][ch], scfsi[ch], gr, ch, &bound[gr], &region0[gr][ch],
                        &region1[gr][ch], &preflag[gr][ch], &is_scale[gr][ch]);
            MP3_DecodeBigValues((HuffState*)d, d->ring, d->huff_out[gr][ch], big_values[gr][ch], flags[gr][ch],
                                region0[gr][ch], region1[gr][ch], count1_table[gr][ch], ch, table_select[gr][ch],
                                &bound[gr]);
        }
    }
    for (gr = 0; gr < ngr; gr++) {
        MP3_Dequantize(d, d->long_map, d->short_map, d->mixed_map, d->long_edges, d->short_edges, flags[gr],
                       d->gr_out, d->huff_out[gr][0], global_gain[gr], subblock_gain[gr][0], sf_scale[gr],
                       d->sf_long[gr][0], d->sf_short[gr][0], preflag[gr]);
        if (d->mode_ext & 2) {
            fn_800C57B0(d->long_end, d->short_bands, flags[gr][0], d->gr_out);
        }
        if (d->mode_ext & 1) {
            MP3_IntensityStereo(d, d->long_map, d->long_edges, flags[gr][1], d->gr_out, d->sf_long[gr][0],
                                d->sf_short[gr][0], is_scale[gr], bound[gr]);
        }
        if (flags[gr][0] & 0xF) {
            MP3_Antialias(d->band_count, d->long_end, d->channels, flags[gr][0], d->gr_out);
        }
        MP3_IMDCT(d, d->imdct_work[0], d->imdct_prev[0], d->imdct_out, flags[gr], d->gr_out);
        if (mode == 0) {
            fn_800C4CF4(d, d->synth_buf, d->imdct_out, lbl_8055DD88, d->channels, mode);
            lbl_8055DD88 += d->channels * 0x240;
        } else {
            fn_800C4CF4(d, d->synth_buf, d->imdct_out, lbl_8055DD88, d->channels, mode);
            lbl_8055DD88 += 0x240;
        }
    }
    return 0;
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
