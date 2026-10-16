# Locate a compiled object's functions in the original DOL: for each function, list the original
# function starts (from symbols.txt) whose bytes match (relocations compared by opcode/register),
# then summarize the address span of the unique hits and which existing splits overlap it.
# Usage: python locate.py <compiled.o> [min function size, default 0x10]
import re, struct, sys
obj = sys.argv[1]
minsz = int(sys.argv[2], 16) if len(sys.argv) > 2 else 0x10
b = open(obj, "rb").read()
sh, = struct.unpack(">I", b[0x20:0x24]); es, n, si = struct.unpack(">HHH", b[0x2E:0x34])
S = [struct.unpack(">10I", b[sh + i * es:sh + i * es + 40]) for i in range(n)]; so = S[si][4]
names = [b[so + s[0]:b.index(b"\0", so + s[0])].decode() for s in S]
texts = [i for i, nm in enumerate(names) if nm == ".text"]
st = [i for i, s in enumerate(S) if s[1] == 2][0]; strs = S[S[st][6]][4]
rel = {}
for s in S:
    if s[1] == 4 and s[7] in texts:
        rel.setdefault(s[7], set()).update(struct.unpack(">I", b[s[4] + k:s[4] + k + 4])[0] & ~3 for k in range(0, s[5], 12))
fns = []
for k in range(0, S[st][5], 16):
    o, v, sz, info, oth, shn = struct.unpack(">IIIBBH", b[S[st][4] + k:S[st][4] + k + 16])
    if shn in texts and (info & 15) == 2 and sz >= minsz:
        t = b[S[shn][4]:S[shn][4] + S[shn][5]]
        fns.append((b[strs + o:b.index(b"\0", strs + o)].decode(), t[v:v + sz], {r - v for r in rel.get(shn, ()) if v <= r < v + sz}))
d = open("orig/REXE01/sys/main.dol", "rb").read()
O = struct.unpack(">18I", d[0:72]); A = struct.unpack(">18I", d[72:144]); Z = struct.unpack(">18I", d[144:216])
def rd(x, n):
    for i in range(18):
        if Z[i] and A[i] <= x and x + n <= A[i] + Z[i]: return d[O[i] + x - A[i]:O[i] + x - A[i] + n]
def same(t, c, r):
    if t is None: return False
    for k in range(0, len(c), 4):
        x, y = struct.unpack(">I", t[k:k + 4])[0], struct.unpack(">I", c[k:k + 4])[0]
        if k in r:
            if x >> 26 != y >> 26: return False
            if x >> 26 != 18 and (x >> 21) & 31 != (y >> 21) & 31: return False
        elif x != y:
            return False
    return True
syms = {}
for a, z in re.findall(r"= \.text:0x(\w+); // type:function size:0x(\w+)", open("config/REXE01/symbols.txt").read()):
    syms.setdefault(int(z, 16), []).append(int(a, 16))
uniq = []
for name, c, r in fns:
    hits = [a for a in syms.get(len(c), []) if same(rd(a, len(c)), c, r)]
    print(f"{name[:60]:60} {len(c):#6x} {' '.join(hex(h) for h in hits[:4])}{' ...' if len(hits) > 4 else ''}")
    if len(hits) == 1: uniq.append(hits[0])
if uniq:
    lo, hi = min(uniq), max(uniq)
    sp = open("config/REXE01/splits.txt").read()
    over = []
    for blk in sp.split("\n\n"):
        L = blk.strip().splitlines()
        m = re.search(r"\.text\s+start:0x(\w+) end:0x(\w+)", blk)
        if L and m and int(m.group(1), 16) <= hi and int(m.group(2), 16) > lo: over.append(L[0][:-1])
    print(f"unique hits span {lo:#x}-{hi:#x}; overlapping splits: {over}")
