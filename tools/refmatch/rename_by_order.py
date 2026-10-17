# Rename the target's functions in [lo, hi) to the compiled object's function names, in order
# (for a unit whose source order matches the original). Names that start with "fn_" in the object
# (extern "C" placeholders) are left alone.
# Usage: python rename_by_order.py <obj.o> <lo hex> <hi hex>
import re, subprocess, sys, os
obj, lo, hi = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16)
out = os.path.join("build", "try", "rbo.s")
subprocess.run([os.path.abspath("build/tools/dtk.exe"), "elf", "disasm", obj, out], capture_output=True)
mine = [m.strip('"') for m in re.findall(r'^\.fn ("?[^,]+"?), ', open(out).read(), re.M)]
p = "config/REXE01/symbols.txt"
L = open(p).read().split("\n")
rows = []
for i, l in enumerate(L):
    m = re.search(r"= \.text:0x([0-9A-F]+); // type:function", l)
    if m and lo <= int(m.group(1), 16) < hi:
        rows.append((int(m.group(1), 16), i))
rows.sort()
if len(rows) != len(mine):
    sys.exit("count mismatch: target %d, object %d" % (len(rows), len(mine)))
for (a, i), n in zip(rows, mine):
    if not n.startswith("fn_"):
        L[i] = n + L[i][L[i].index(" = "):]
open(p, "w").write("\n".join(L))
print("renamed", sum(1 for n in mine if not n.startswith("fn_")), "functions")
