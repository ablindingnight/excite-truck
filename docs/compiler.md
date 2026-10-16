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
- Not yet determined: exact 3.0a version, `-O4,p` vs `-O4,s`, `-inline auto` vs `noauto`.
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
