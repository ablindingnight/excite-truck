# Dump the original asm (instructions + local labels) of an address range into one text file,
# one "=== name" header per function. Usage: python dumpfn.py <lo hex> <hi hex> <out file>
import os, re, sys
lo, hi = int(sys.argv[1], 16), int(sys.argv[2], 16)
out = []
for f in sorted(os.listdir("build/REXE01/asm")):
    if not f.endswith("_text.s"):
        continue
    cur = None
    for l in open(os.path.join("build/REXE01/asm", f)):
        m = re.match(r"\.fn (\S+),", l)
        if m:
            cur = m[1]
            continue
        m = re.match(r"/\* ([0-9A-F]{8}) [0-9A-F]+  [0-9A-F ]{11} \*/\s*(.*)", l)
        if m and cur and lo <= int(m[1], 16) < hi:
            if not out or out[-1][0] != cur:
                out.append((cur, "=== " + cur))
            out.append((cur, m[2]))
        elif l.startswith(".L_") and cur and out and out[-1][0] == cur:
            out.append((cur, l.strip()))
open(sys.argv[3], "w").write("\n".join(t for _, t in out) + "\n")
print(len(out), "lines")
