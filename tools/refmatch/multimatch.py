# Map a range of the original DOL against many compiled reference objects at once: for each
# original function, list which objects have byte-identical code (relocations masked, via
# region.py). Shows which functions already have a matching source and which need porting.
# Usage: python multimatch.py <lo hex> <hi hex> <obj or dir>... [--summary]
import os, re, subprocess, sys

args = [a for a in sys.argv[1:] if not a.startswith("--")]
lo, hi = args[0], args[1]
objs = []
for a in args[2:]:
    if os.path.isdir(a):
        objs += sorted(os.path.join(a, f) for f in os.listdir(a) if f.endswith(".o"))
    else:
        objs.append(a)

rows = {}  # addr -> [size, label, set(matches)]
for o in objs:
    tag = os.path.basename(os.path.dirname(o)) + "/" + os.path.basename(o)[:-2]
    out = subprocess.run([sys.executable, "-I", "tools/refmatch/region.py", o, lo, hi],
                         capture_output=True, text=True).stdout
    for l in out.splitlines():
        m = re.match(r"(0x\w+)\s+(0x\w+)\s+(\S+)\s+(=|size~|-)\s*(\S*)", l)
        if not m:
            continue
        r = rows.setdefault(int(m.group(1), 16), [int(m.group(2), 16), m.group(3), set()])
        if m.group(4) == "=":
            r[2].add(tag + ":" + re.split(r",(?=[^0-9])", m.group(5))[0])

total = matched = 0
for a in sorted(rows):
    size, label, hits = rows[a]
    total += size
    matched += size if hits else 0
    if "--summary" not in sys.argv:
        print("%08x %5x %-24s %s" % (a, size, label[:24], " ".join(sorted(hits))[:110] if hits else "-"))
print("matched %#x of %#x bytes (%d of %d functions)" % (matched, total, sum(1 for r in rows.values() if r[2]), len(rows)))
