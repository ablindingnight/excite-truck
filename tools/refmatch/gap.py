# Show the unsplit .text gap around an address range (between the neighbouring units' splits) and
# run region.py for a compiled object over that whole gap, to find a file's exact extent.
# Usage: python gap.py <compiled.o> <lo hex> <hi hex>
import re, subprocess, sys
obj, lo, hi = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16)
s = open("config/REXE01/splits.txt").read()
units = sorted((int(a, 16), int(b, 16), u) for u, a, b in
               re.findall(r"^(\S+):\n\t\.text\s+start:0x(\w+) end:0x(\w+)", s, re.M))
prev = max((b for a, b, u in units if b <= lo), default=0)
nxt = min((a for a, b, u in units if a >= hi), default=0x80300000)
inside = [u for a, b, u in units if a < hi and b > lo]
print(f"gap {prev:#x}-{nxt:#x}; units inside {lo:#x}-{hi:#x}: {inside}")
for a, b, u in units:
    if b == prev or a == nxt: print(f"  neighbour {u} {a:#x}-{b:#x}")
print(subprocess.run([sys.executable, "-I", "tools/refmatch/region.py", obj, f"{prev:x}", f"{nxt:x}"],
                     capture_output=True, text=True).stdout)
