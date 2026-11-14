# Port audit (64-bit PC readiness) - generated 2026-11-13

Counts of source patterns that assume the Wii's 32-bit, big-endian layout. They compile identically for matching
but must become real types/symbols before a 64-bit port (see docs/porting.md sections 1, 2 and 4). Counts are
pattern matches, so treat them as a guide.

## Raw-offset field macros (F32/S32/U32/S16/U16/U8/S8/PTR(p, 0xNN)): 4850 uses in 20 files

- `src/game/carbody.cpp`: 1298
- `src/game/aicar.cpp`: 884
- `src/game/sphcoll.cpp`: 700
- `src/game/playcar.cpp`: 403
- `src/game/cardmg.cpp`: 243
- `src/game/obstacles.cpp`: 206
- `src/game/cammgr.cpp`: 184
- `src/game/garage.cpp`: 163
- `src/game/chalpath.cpp`: 145
- `src/game/multievt.cpp`: 139
- `src/game/camstate.cpp`: 121
- `src/game/physcam.cpp`: 111
- `src/game/evpath.cpp`: 58
- `src/game/achview.cpp`: 46
- `src/game/mroll.cpp`: 46

## Byte-offset pointer arithmetic ((u8*)p + 0xNN): 257 uses in 14 files

- `src/game/unk_80116BD4.c`: 156
- `src/game/deity.cpp`: 37
- `src/game/unk_8020DE74.cpp`: 16
- `src/game/carbody.cpp`: 9
- `src/game/uiapi.cpp`: 9
- `src/game/unk_80262F8C.c`: 6
- `src/game/sphcoll.cpp`: 5
- `src/game/cardmg.cpp`: 4
- `src/game/panelvol.cpp`: 4
- `src/game/unk_80215E68.c`: 4
- `src/game/drsurf.cpp`: 3
- `src/game/garage.cpp`: 2
- `src/game/gx.cpp`: 1
- `src/game/uianime.cpp`: 1

## reinterpret_cast to int: 10 uses in 1 files

- `src/game/widgets.cpp`: 10

## Pointer casts of fixed labels ((T*)lbl_..): 210 uses in 47 files

- `src/game/evpath.cpp`: 37
- `src/game/unk_80116BD4.c`: 31
- `src/game/tutorial.cpp`: 11
- `src/game/sdmp3.cpp`: 10
- `src/game/gx.cpp`: 8
- `src/game/physcam.cpp`: 7
- `src/game/unk_8020CF68.c`: 7
- `src/game/unk_8020F3B4.c`: 7
- `src/game/homebtn.cpp`: 6
- `src/game/unk_802E4D84.cpp`: 6
- `src/game/cammgr.cpp`: 5
- `src/game/carbody.cpp`: 5
- `src/game/obstacles.cpp`: 5
- `src/game/chalpath.cpp`: 4
- `src/game/namedcam.cpp`: 4

## Priorities

1. **carbody, aicar, sphcoll, playcar** hold most raw-offset accesses: recovering their structs (CarBody, AICar,
   collision volumes, PlayerCar) removes most of the port risk in one place. Keep each field's `// offset 0xNN`.
2. **Bink (unk_80116BD4)**: byte-offset arithmetic and `u32` pointer fields in the decoder state need real structs
   with pointer-sized fields.
3. **widgets / evpath**: pointers carried in `int`/`s32` (window context, bar handles) are known 64-bit bugs.
4. Byte-swapping loaders (sphcoll Swap32, path, drtex Fix): keep them, but mark them as big-endian data readers.
