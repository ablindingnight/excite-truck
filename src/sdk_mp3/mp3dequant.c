/* Nintendo-supplied SDK MP3 (Layer III) decoder, 0x800BC538-0x800C6204 in REXE01 (the same tables ship in
 * the Wii Photo Channel). Library/module and file names unknown. It is three translation units: MWCC addresses
 * pooled tables from the start of each unit's .rodata, and the original does so from 0x802F34E8 (dequantizer)
 * and 0x802F3D60 (Huffman decoder), which puts unit boundaries at text 0x800BF048 and 0x800C0440.
 * Build: cflags_rvl_tp, GC/3.0a3, plus -use_lmw_stmw on -inline noauto (every function saves with stmw;
 * WENCGetEncodeData right after the decoder does not, so it is a separate object). */
/* Unit 2 of 3: dequantization (0x800BF048-0x800C0440). */
#include "mp3dec.h"
#include "mp3dequant_tables.inc"
