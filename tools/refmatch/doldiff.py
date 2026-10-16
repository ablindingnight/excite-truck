# Compare the built DOL with the original and attribute differing bytes to split units.
# Usage: python doldiff.py [built.dol] [orig.dol]
import re, struct, sys
from collections import defaultdict
a = open(sys.argv[1] if len(sys.argv) > 1 else "build/REXE01/main.dol", "rb").read()
b = open(sys.argv[2] if len(sys.argv) > 2 else "orig/REXE01/sys/main.dol", "rb").read()
def secs(d):
    o = struct.unpack(">18I", d[0:72]); ad = struct.unpack(">18I", d[72:144]); sz = struct.unpack(">18I", d[144:216])
    return [(o[i], ad[i], sz[i]) for i in range(18) if sz[i]]
sa, sb = secs(a), secs(b)
if [(x[1], x[2]) for x in sa] != [(x[1], x[2]) for x in sb]:
    print("section layout differs:", [(hex(x[1]), hex(x[2])) for x in sa], [(hex(x[1]), hex(x[2])) for x in sb])
splits = open("config/REXE01/splits.txt").read()
rng = []
for blk in splits.split("\n\n"):
    lines = blk.strip().splitlines()
    if not lines or not lines[0].endswith(":") or lines[0].startswith("Sections"): continue
    for s, st, en in re.findall(r"(\S+)\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)", blk):
        rng.append((int(st, 16), int(en, 16), lines[0][:-1], s))
rng.sort()
def owner(x):
    for st, en, u, s in rng:
        if st <= x < en: return f"{u} {s}"
    return "(unsplit)"
diff = defaultdict(list)
for (oa, ad, sz), (ob, _, szb) in zip(sa, sb):
    n = min(sz, szb); x = a[oa:oa + n]; y = b[ob:ob + n]
    if x == y: continue
    for k in range(0, n, 4):
        if x[k:k + 4] != y[k:k + 4]: diff[owner(ad + k)].append(ad + k)
for u, l in sorted(diff.items(), key=lambda t: t[1][0]):
    print(f"{u:60} {len(l):5} words first {l[0]:#x}")
print("IDENTICAL" if not diff and a == b else f"{len(diff)} differing units")
