# Keep candidate split blocks that don't overlap existing splits or each other.
# Usage: python filter_splits.py candidates.txt splits.txt out.txt [--exclude substr ...] [--min-text N]
import re, sys
cand_p, splits_p, out_p, *rest = sys.argv[1:]
excl = [rest[i + 1] for i, a in enumerate(rest) if a == "--exclude"]
min_text = int(rest[rest.index("--min-text") + 1], 0) if "--min-text" in rest else 0x40
blocks = [b for b in open(cand_p).read().strip().split("\n\n") if b.strip()]
existing = open(splits_p).read()
ex = [(s, int(a, 16), int(e, 16)) for s, a, e in re.findall(r"(\S+)\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)", existing)]
parsed = []
for b in blocks:
    name = b.splitlines()[0].rstrip(":")
    rng = [(s, int(a, 16), int(e, 16)) for s, a, e in re.findall(r"(\S+)\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)", b)]
    parsed.append((name, b, rng))
ov = lambda r1, r2: any(s1 == s2 and a1 < e2 and a2 < e1 for s1, a1, e1 in r1 for s2, a2, e2 in r2)
keep = []
for name, b, rng in parsed:
    t = next(r for r in rng if r[0] == ".text")
    why = None
    if any(x in name for x in excl): why = "excluded"
    elif ov(rng, ex): why = "overlaps existing"
    elif t[2] - t[1] < min_text: why = f"tiny .text {t[2] - t[1]:#x}"
    elif any(o[0] != name and ov(rng, o[2]) for o in parsed): why = "overlaps candidate"
    if why not in ("excluded", "overlaps existing"):
        print(f"{'KEEP' if not why else 'drop'} {name:40} .text {t[1]:#x}+{t[2] - t[1]:#x} {why or ''}")
    if not why: keep.append(b)
open(out_p, "w", newline="\n").write("\n\n".join(keep) + "\n")
print(len(keep), "kept")
