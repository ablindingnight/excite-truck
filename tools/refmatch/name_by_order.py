# Name every function in a unit's .text range positionally from the compiled object: the k-th
# original function gets the k-th compiled function's name, after checking that the sizes line up
# (dead-stripped compiled functions are skipped when sizes allow). Use for files whose code matches
# in source order but whose look-alike functions (tiny wrappers/getters) were labelled by body.
# Usage: python name_by_order.py <unit> [--apply]
import re, struct, sys
unit = sys.argv[1]; stem = unit.rsplit(".", 1)[0]
s = open("config/REXE01/splits.txt").read()
blk = s[s.index(unit + ":\n"):]
lo, hi = [int(x, 16) for x in re.search(r"\.text\s+start:0x(\w+) end:0x(\w+)", blk).groups()]
b = open(f"build/REXE01/src/{stem}.o", "rb").read()
sh, = struct.unpack(">I", b[0x20:0x24]); es, n, si = struct.unpack(">HHH", b[0x2E:0x34])
S = [struct.unpack(">10I", b[sh + i * es:sh + i * es + 40]) for i in range(n)]; so = S[si][4]
names = [b[so + x[0]:b.index(b"\0", so + x[0])].decode() for x in S]
st = [i for i, x in enumerate(S) if x[1] == 2][0]; strs = S[S[st][6]][4]
texts = [i for i, x in enumerate(names) if x == ".text"]
comp = []
for k in range(0, S[st][5], 16):
    o, v, sz, info, oth, shn = struct.unpack(">IIIBBH", b[S[st][4] + k:S[st][4] + k + 16])
    if shn in texts and (info & 15) == 2 and sz:
        comp.append((texts.index(shn), v, sz, b[strs + o:b.index(b"\0", strs + o)].decode()))
comp.sort()
p = "config/REXE01/symbols.txt"; L = open(p).read().split("\n")
orig = []
for i, l in enumerate(L):
    m = re.match(r"^(\S+) = \.text:0x(\w+); // type:function size:0x(\w+)", l)
    if m and lo <= int(m.group(2), 16) < hi: orig.append((int(m.group(2), 16), int(m.group(3), 16), m.group(1), i))
orig.sort()
j = 0; plan = []
for a, sz, nm, i in orig:
    while j < len(comp) and comp[j][2] != sz: j += 1   # skip stripped compiled functions
    if j == len(comp): sys.exit(f"no compiled function of size {sz:#x} for {a:#x} {nm}")
    plan.append((a, nm, comp[j][3], i)); j += 1
for a, old, new, i in plan:
    if old != new: print(f"{a:#x} {old} -> {new}")
if "--apply" in sys.argv:
    for a, old, new, i in plan:
        L[i] = L[i].replace(old + " = ", new + " = ", 1)
    open(p, "w", newline="\n").write("\n".join(L)); print("applied")
