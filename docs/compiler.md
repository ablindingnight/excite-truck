# Compiler findings

## SDK and runtime
- RVL_SDK version banners read `(0x4200_60422)`: mwcc 4.2 build 60422 = **GC/3.0a5**.
- The SDK is the Sep 2006 build, identical to Twilight Princess Wii rev 0 (`SDK_SEP2006`).
- Runtime.PPCEABI.H matches on GC/3.0a3 through 3.0a5.2.

## Game code (kernel.cpp and friends)
Tested by hand-decompiling a few game functions (see `scratch/` while iterating):

| Function | Address | Result |
|---|---|---|
| `main` (kernel.cpp, `OSPanic("kernel.cpp", 210, "END")`) | 0x802E2BA8 | match |
| `AppMain` (prints `"AppMain is 0x%x"`) | 0x80004E20 | match |
| startup init list (`fn_802E2B34`) | 0x802E2B34 | match |
| `p + v*t + t*(0.5f*a*t)` | 0x801A6370 | match |

Findings:
- **`-proc generic`** (or `603e`) is required; `-proc gekko` reorders `main` differently. The SDK used gekko, the game did not.
- **`-fp_contract off`**: the game uses separate `fmuls`/`fadds`, never fused multiply-add.
- GC/2.x is ruled out (wrong code size).
- GC/3.0a3, 3.0a5 and 3.0a5.2 match everything so far at both `-O4,p` and `-O4,s`.
  The Wii/1.x compilers match only at `-O4,s` (at `-O4,p` they align loops with a `nop`).
- **`-inline noauto`**: tiny non-inline helpers (app.cpp `ResolveResName`, `LoadResidentRes`) stay calls; explicit `inline`/in-class methods still inline.
- Not yet determined: exact 3.0a version, `-O4,p` vs `-O4,s`.
  Default to GC/3.0a5 (the SDK's compiler and the version in the banners) until a function disagrees.
- The project now links with the **GC/3.0a5 linker** (as Twilight Princess does). Two things were needed:
  - `mw_comment_version: 14` in `config.yml`. With the template's value (8) dtk stamps objects as
    GC 1.3.2, and every GC 3.0+ linker crashes (access violation) when reading them.
  - The GC 3.0 runtime ctor/dtor layout: `.ctors$10`, `.dtors$10`, `.dtors$15`, all three entries owned by
    `__init_cpp_exceptions.cpp` (split with dtk `rename:`). Otherwise the linker stops with
    "runtime sources ... need to be updated to latest version".
- With the old GC/1.3.2 linker, splitting OSSync hung the link; with GC/3.0a5 (and clean
  `__OSSystemCallVectorStart/End` labels) it links normally.
- Tool versions bumped to match TP where needed: dtk v1.8.0.

## MSL C library
- Most MSL files match GC/3.0a5 with the Mario Strikers Charged per-file flags (`libs/msc`).
- **printf.c is built with GC/3.0a5.2** plus `-use_lmw_stmw on -str reuse,pool,readonly`
  (16-byte struct copies use lfd/stfd only with 3.0a5.2; the Pokémon Battle Revolution decomp pins the same).
- fdlibm wrappers (`w_*.c`) are compiled with `-D_IEEE_LIBM` (plain tail calls).
- The runtime helper at 0x8002FE08 is `__cvt_dbl_usll`; GC/3.0a5 emits calls to `__cvt_dbl_ull`, so runtime.c gives it both names.

## Excite Truck-specific differences (`GAME_REXE01`)

`-DGAME_REXE01` is defined for the TP, ogws and Strikers-derived libraries. It guards differences
from those games' builds, for example the per-library build dates in the SDK version banners
(SI/GX: Sep 7 2006, DSP: Sep 20 2006), the capitalized English disc-error text in `dvdFatal.c`,
`BTA_Init` reading `__OSGetSystemTime()` before creating its alarm, and `strncasecmp` being
defined after `strcasecmp` in MSL `extras.c`.

## Build settings by library (cheat sheet)

| Code | Source | Flags (configure.py) | Compiler | Notes |
|---|---|---|---|---|
| Runtime | ogws/TP | runtime flags | GC/3.0a5 | `.ctors$10`/`.dtors$10/$15` sections |
| MSL C | Strikers | `cflags_msc` | GC/3.0a5 | per file: `-Cpp_exceptions on` when the file has extab; some need `-use_lmw_stmw on`; `printf.c` GC/3.0a5.2 + `-str reuse,pool,readonly`; fdlibm wrappers `-D_IEEE_LIBM` |
| SDK (C) | TP | `cflags_rvl_tp` | GC/3.0a3 (= 3.0a5 for C) | `-ipa file`: functions emitted in definition order, `.sbss` in reverse declaration order |
| SDK (C) | ogws | `cflags_rvl` | GC/3.0a5 | |
| SDK (C) | Strikers | `cflags_msc_sdk`, BTE `cflags_msc_bte` | GC/3.0a5 | `exi/EXIBios.c` is `-O3,p` (`cflags_msc_sdk_O3`): mwcc keeps the higher of two `-O` levels, so replace the flag, don't append it |
| HBM / nw4hbm (C++) | TP | `cflags_rvl_tp_hbm` | GC/3.0a5 | `NW4HBM_DEBUG` (ET's HBM has asserts); **no `-sym on`** (debug info moves out-of-line inline copies into separate sections) |
| HBM / nw4hbm (C++) | Strikers | `cflags_msc_hbm` | GC/3.0a5.2 | |
| Game (Monster Games) | (ours) | `cflags_game` | GC/3.0a5 | `-proc generic -fp_contract off` |

Linker: GC/3.0a5 with `mw_comment_version: 14`. MWLD keeps the first definition of a weak function
or `@STRING@` object, so the first copy in the original must carry the right name.

To find settings for a new file automatically: `python tools/refmatch/flagsearch.py <unit>` compiles
it with every compiler version x single flag change and ranks the results by matching functions/bytes,
number of `.text` sections and function order.
