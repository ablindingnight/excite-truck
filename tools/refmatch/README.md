# refmatch

Helpers for importing code from other decomp projects that share libraries with Excite Truck.

1. `build_ref.py` compiles a reference repo's sources (e.g. `src/revolution`) with a chosen MWCC.
2. `sigmatch.py` finds each compiled function in `main.dol` (relocated fields masked) and writes a JSON report.
3. `autosplit.py` turns fully matched objects into `splits.txt` entries (text + data, using r13=0x80560900 / r2=0x80568100) and writes `names.json`.
4. `apply_names2.py` writes those exact symbols into `symbols.txt`, replacing auto-generated ones in the split ranges.
5. `xfer_names.py` pairs relocations between dtk's split objects and our compiled objects to name called functions.
6. `datacmp.py` compares data symbols between two objects; `cctest.py` tries one source file on many compilers; `s2mwasm.py` converts dtk asm into MW inline-asm functions.

dtk split ends must be 4-byte aligned, so objdiff may report <100% data from pad bytes; check the linked DOL hash instead.

## Verifying the link (the DOL hash is the only real arbiter)

`config/REXE01/build.sha1` must hash `build/REXE01/main.dol`. (Until 2026-10-16 it pointed at
`orig/REXE01/sys/main.dol`, an upstream change that made every "OK" vacuous.) objdiff's per-function
100% does not prove the link matches: it can't see function order, weak-function placement,
relocation targets, or data a split doesn't cover. When the hash fails, build with
`python configure.py --map` and:

1. `mapcheck.py`: per section, where the placed address of each object first drifts from its
   split start, blaming the object placed before it (wrong size, missing data, extra functions).
2. `extend_data.py [--apply]`: grows a linked unit's data split to the size the linker actually
   placed, forward or backward (e.g. a version banner before the data), only if the bytes match.
3. `doldiff.py`: once sections line up, which units still have differing words. `wdiff.py <lo> <hi>`
   lists them with symbols.
4. `syncnames.py`: with the layout aligned, the map is ground truth for linked functions' names;
   lists symbols.txt labels that disagree (swapped look-alike functions from masked matching).

Typical causes found so far: missing `-Cpp_exceptions on` (extab), version-banner build dates,
data/bss not in a split (double-counted), look-alike functions mislabelled (e.g. `PSMTXRotRad`
vs `C_MTXRotRad`), function order under `-ipa file`, weak inline functions emitted in a different
object or position, and MWLD filling alignment padding with junk where the original had zeros
(extend the split end over the padding).

## Safety net

`python tools/install_hooks.py` installs two git hooks: **pre-commit** rebuilds and refuses the
commit unless `main.dol` matches the original's SHA-1, and **post-commit** writes a git bundle of
the whole history to `../backups` (restore with `git clone <bundle>`).
