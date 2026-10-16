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
- The Twilight Princess decomp found TP's game code needs GC/3.0a3 (patched for multi-char constants)
  and the **GC/3.0a5 linker** for Wii builds. Our link currently uses GC/1.3.2 because the 3.0a5 linker
  crashed with this project's settings; worth revisiting.
