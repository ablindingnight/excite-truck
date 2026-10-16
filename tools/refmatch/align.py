# Line up a reference object's functions against the target, in order, to show which functions
# match exactly, which were dead-stripped, and which target functions sit where a reference
# function "should" be but differ (candidates for hand-matching).
# Usage: python align.py <main.dol> <symbols.txt> <obj> [--from ADDR --to ADDR]
import re, struct, sys
sys.argv += []
dol, syms_path, obj = sys.argv[1:4]
rng = None
if "--from" in sys.argv:
    rng = (int(sys.argv[sys.argv.index("--from") + 1], 16), int(sys.argv[sys.argv.index("--to") + 1], 16))

sys.path.insert(0, __import__("os").path.dirname(__file__))
from autosplit2_lib import load_target, read_elf, fn_hits  # noqa: E402

words, by_word, tfuncs, tnames = load_target(dol, syms_path)
b, secs, syms, rel = read_elf(obj)
ti = next(i for i, s in enumerate(secs) if s["name"] == ".text" and s["size"])
text = b[secs[ti]["off"]:secs[ti]["off"] + secs[ti]["size"]]
rmap = {o & ~3: (t, si, a) for o, t, si, a in rel[ti]}
funcs = sorted([s for s in syms if s["shn"] == ti and s["type"] == 2 and s["size"]], key=lambda s: s["val"])
hits = [fn_hits(words, by_word, text, rmap, f) for f in funcs]
uniq = {i: h[0] for i, h in enumerate(hits) if len(h) == 1}
if rng is None:
    lo = min(uniq.values()); hi = max(uniq[i] + funcs[i]["size"] for i in uniq)
else:
    lo, hi = rng
tin = sorted(a for a in tfuncs if lo <= a < hi)
print(f"target span {lo:#x}-{hi:#x}: {len(tin)} target functions; reference has {len(funcs)}")
# walk target functions in order, consuming reference functions in order
ri = 0
used = set(uniq.values())
for a in tin:
    # reference functions that match this exact address
    m = [i for i in range(len(funcs)) if a in hits[i]]
    if m:
        i = m[0]
        skipped = [funcs[j]["name"] for j in range(ri, i)]
        for nm in skipped:
            print(f"    {'':10} {'':6}  (ref only / stripped or differs) {nm}")
        print(f"  = {a:#010x} {tfuncs[a]:#6x}  {funcs[i]['name']}")
        ri = i + 1
    else:
        nxt = funcs[ri] if ri < len(funcs) else None
        guess = f"? maybe {nxt['name']} (ref size {nxt['size']:#x})" if nxt else ""
        print(f"  ! {a:#010x} {tfuncs[a]:#6x}  {tnames[a]:28} {guess}")
for j in range(ri, len(funcs)):
    print(f"    {'':10} {'':6}  (ref only / stripped or differs) {funcs[j]['name']}")
