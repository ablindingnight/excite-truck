# Rank reference objects by how much of their code already matches unsplit parts of the DOL:
# for each function, look for an identical body (relocated words compared by opcode/register only)
# at a function start not covered by any .text split. Prints matched bytes / total per object.
# Usage: python score_refs.py <obj_root> [glob filter]
import glob, os, re, struct, sys
root = sys.argv[1]
flt = sys.argv[2] if len(sys.argv) > 2 else "**/*.o"
d = open("orig/REXE01/sys/main.dol", "rb").read()
O = struct.unpack(">18I", d[0:72]); A = struct.unpack(">18I", d[72:144]); Z = struct.unpack(">18I", d[144:216])
def rd(x, n):
    for i in range(18):
        if Z[i] and A[i] <= x and x + n <= A[i] + Z[i]: return d[O[i] + x - A[i]:O[i] + x - A[i] + n]
sp = open("config/REXE01/splits.txt").read()
claims = sorted((int(a, 16), int(e, 16)) for a, e in re.findall(r"\.text\s+start:0x(\w+) end:0x(\w+)", sp))
def claimed(x):
    return any(a <= x < e for a, e in claims)
syms = [(int(a, 16), int(z, 16)) for a, z in
        re.findall(r"= \.text:0x(\w+); // type:function size:0x(\w+)", open("config/REXE01/symbols.txt").read())]
starts = {}
for a, z in syms:
    if not claimed(a): starts.setdefault(z, []).append(a)

def funcs(path):
    b = open(path, "rb").read()
    sh, = struct.unpack(">I", b[0x20:0x24]); es, n, si = struct.unpack(">HHH", b[0x2E:0x34])
    S = [struct.unpack(">10I", b[sh + i * es:sh + i * es + 40]) for i in range(n)]; so = S[si][4]
    names = [b[so + s[0]:b.index(b"\0", so + s[0])].decode() for s in S]
    if ".text" not in names: return []
    ti = names.index(".text"); text = b[S[ti][4]:S[ti][4] + S[ti][5]]
    st = [i for i, s in enumerate(S) if s[1] == 2][0]; strs = S[S[st][6]][4]
    rel = set()
    for s in S:
        if s[1] == 4 and s[7] == ti:
            for k in range(0, s[5], 12): rel.add(struct.unpack(">I", b[s[4] + k:s[4] + k + 4])[0] & ~3)
    out = []
    for k in range(0, S[st][5], 16):
        o, v, sz, info, oth, shn = struct.unpack(">IIIBBH", b[S[st][4] + k:S[st][4] + k + 16])
        if shn == ti and (info & 15) == 2 and sz:
            out.append((b[strs + o:b.index(b"\0", strs + o)].decode(), text[v:v + sz], {r - v for r in rel if v <= r < v + sz}))
    return out

def same(t, c, rel):
    for k in range(0, len(c), 4):
        x, y = struct.unpack(">I", t[k:k + 4])[0], struct.unpack(">I", c[k:k + 4])[0]
        if k in rel:
            if x >> 26 != y >> 26: return False
            if x >> 26 != 18 and (x >> 21) & 31 != (y >> 21) & 31: return False
        elif x != y:
            return False
    return True

rows = []
for p in glob.glob(os.path.join(root, flt), recursive=True):
    fs = funcs(p)
    tot = sum(len(c) for _, c, _ in fs)
    if not tot: continue
    hit = 0
    for name, c, rel in fs:
        if len(c) < 12: continue  # tiny functions match too easily
        if any(same(rd(a, len(c)), c, rel) for a in starts.get(len(c), []) if rd(a, len(c))):
            hit += len(c)
    if hit: rows.append((hit, tot, os.path.relpath(p, root)))
for hit, tot, p in sorted(rows, reverse=True):
    print(f"{hit:7} / {tot:7} ({100 * hit / tot:5.1f}%)  {p}")
