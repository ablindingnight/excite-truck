# Side-by-side view of an address range of the original DOL and a compiled object: each original
# function with its size, which compiled functions match it exactly (relocations masked), and which
# have the same size; then the compiled functions not matched anywhere in the range.
# Usage: python region.py <compiled.o> <lo hex> <hi hex>
import re, struct, sys
obj, lo, hi = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16)
b = open(obj, "rb").read()
sh, = struct.unpack(">I", b[0x20:0x24]); es, n, si = struct.unpack(">HHH", b[0x2E:0x34])
S = [struct.unpack(">10I", b[sh + i * es:sh + i * es + 40]) for i in range(n)]; so = S[si][4]
names = [b[so + s[0]:b.index(b"\0", so + s[0])].decode() for s in S]
ti = names.index(".text"); text = b[S[ti][4]:S[ti][4] + S[ti][5]]
st = [i for i, s in enumerate(S) if s[1] == 2][0]; strs = S[S[st][6]][4]
rel = set()
for s in S:
    if s[1] == 4 and s[7] == ti:
        for k in range(0, s[5], 12): rel.add(struct.unpack(">I", b[s[4] + k:s[4] + k + 4])[0] & ~3)
fns = []
for k in range(0, S[st][5], 16):
    o, v, sz, info, oth, shn = struct.unpack(">IIIBBH", b[S[st][4] + k:S[st][4] + k + 16])
    if shn == ti and (info & 15) == 2 and sz: fns.append((v, sz, b[strs + o:b.index(b"\0", strs + o)].decode()))
fns.sort()
d = open("orig/REXE01/sys/main.dol", "rb").read()
O = struct.unpack(">18I", d[0:72]); A = struct.unpack(">18I", d[72:144]); Z = struct.unpack(">18I", d[144:216])
def rd(x, n):
    for i in range(18):
        if Z[i] and A[i] <= x and x + n <= A[i] + Z[i]: return d[O[i] + x - A[i]:O[i] + x - A[i] + n]
def same(addr, size, v):
    t = rd(addr, size)
    for k in range(0, size, 4):
        x, y = struct.unpack(">I", t[k:k + 4])[0], struct.unpack(">I", text[v + k:v + k + 4])[0]
        if v + k in rel:
            if x >> 26 != y >> 26: return False
            if x >> 26 != 18 and (x >> 21) & 31 != (y >> 21) & 31: return False
        elif x != y:
            return False
    return True
syms = sorted((int(a, 16), int(z, 16), nm) for nm, a, z in
              re.findall(r"^(\S+) = \.text:0x(\w+); // type:function size:0x(\w+)", open("config/REXE01/symbols.txt").read(), re.M))
hit = set()
for a, z, nm in syms:
    if not lo <= a < hi: continue
    ex = [f for v, sz, f in fns if sz == z and same(a, z, v)]
    sz_only = [f for v, sz, f in fns if sz == z and f not in ex]
    hit.update(ex)
    print(f"{a:#x} {z:#6x} {nm[:30]:30} {'= ' + ','.join(ex) if ex else '  ' + ('size~ ' + ','.join(sz_only) if sz_only else '-')}"[:150])
print("compiled, unmatched:", " ".join(f"{f}:{sz:#x}" for v, sz, f in fns if f not in hit))
