# Print differing words between the built and original DOL in an address range, with the symbol
# each falls in. Usage: python wdiff.py <start> <end> [max]
import re, struct, sys
def ld(p):
    d = open(p, "rb").read()
    return d, struct.unpack(">18I", d[0:72]), struct.unpack(">18I", d[72:144]), struct.unpack(">18I", d[144:216])
def w(D, x):
    d, o, a, s = D
    for i in range(18):
        if s[i] and a[i] <= x < a[i] + s[i]:
            return struct.unpack(">I", d[o[i] + x - a[i]:o[i] + x - a[i] + 4])[0]
O = ld("orig/REXE01/sys/main.dol"); B = ld("build/REXE01/main.dol")
lo, hi = int(sys.argv[1], 16), int(sys.argv[2], 16); mx = int(sys.argv[3]) if len(sys.argv) > 3 else 30
syms = sorted((int(a, 16), int(z, 16), n) for n, a, z in re.findall(r"^(\S+) = \.\w+:0x(\w+); // type:\w+ size:0x(\w+)", open("config/REXE01/symbols.txt").read(), re.M))
def sym(x):
    best = None
    for a, z, n in syms:
        if a <= x < a + max(z, 1): best = f"{n}+{x - a:#x}"
    return best or "?"
n = 0
for x in range(lo, hi, 4):
    a, b = w(O, x), w(B, x)
    if a != b:
        print(f"{x:#x} orig {a:08x} built {b:08x}  {sym(x)}"); n += 1
        if n >= mx: break
