# refmatch

Helpers for importing code from other decomp projects that share libraries with Excite Truck.

1. `build_ref.py` compiles a reference repo's sources (e.g. `src/revolution`) with a chosen MWCC.
2. `sigmatch.py` finds each compiled function in `main.dol` (relocated fields masked) and writes a JSON report.
3. `autosplit.py` turns fully matched objects into `splits.txt` entries (text + data, using r13=0x80560900 / r2=0x80568100) and writes `names.json`.
4. `apply_names2.py` writes those exact symbols into `symbols.txt`, replacing auto-generated ones in the split ranges.
5. `xfer_names.py` pairs relocations between dtk's split objects and our compiled objects to name called functions.
6. `datacmp.py` compares data symbols between two objects; `cctest.py` tries one source file on many compilers; `s2mwasm.py` converts dtk asm into MW inline-asm functions.

dtk split ends must be 4-byte aligned, so objdiff may report <100% data from pad bytes; check the linked DOL hash instead.
