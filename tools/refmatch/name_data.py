# For each data section a unit's split places, rename auto-labelled symbols (lbl_XXXXXXXX) in
# symbols.txt to the compiled object's symbol at the same offset (global names only, so other code
# that references them by name links). Sizes are taken from the compiled symbol.
# Usage: python name_data.py <unit, e.g. revolution/homebuttonLib/nw4hbm/lyt/lyt_material.cpp>
import re, struct, sys
unit = sys.argv[1]
obj = "build/REXE01/src/" + unit.rsplit(".", 1)[0] + ".o"
SECS = (".rodata", ".data", ".bss", ".sdata", ".sbss", ".sdata2", ".sbss2")
b = open(obj, "rb").read()
sh, = struct.unpack(">I", b[0x20:0x24]); es, n, si = struct.unpack(">HHH", b[0x2E:0x34])
S = [struct.unpack(">10I", b[sh + i * es:sh + i * es + 40]) for i in range(n)]; so = S[si][4]
names = [b[so + s[0]:b.index(b"\0", so + s[0])].decode() for s in S]
st = [i for i, s in enumerate(S) if s[1] == 2][0]; strs = S[S[st][6]][4]
sp = open("config/REXE01/splits.txt").read()
i0 = sp.index(unit + ":\n"); j0 = sp.find("\n\n", i0); j0 = len(sp) if j0 < 0 else j0
start = {s: int(a, 16) for s, a in re.findall(r"(\S+)\s+start:0x(\w+) end:0x\w+", sp[i0:j0])}
comp = {}
for k in range(0, S[st][5], 16):
    o, v, sz, info, oth, shn = struct.unpack(">IIIBBH", b[S[st][4] + k:S[st][4] + k + 16])
    if 0 < shn < n and names[shn] in start and names[shn] in SECS and (info >> 4) in (1, 2):
        comp[start[names[shn]] + v] = (b[strs + o:b.index(b"\0", strs + o)].decode(), sz)
p = "config/REXE01/symbols.txt"
lines = open(p).read().split("\n")
existing = {l.split(" = ")[0] for l in lines if " = " in l}
renamed = 0
for i, l in enumerate(lines):
    m = re.match(r"^(lbl_[0-9A-F]{8}) = (\.\w+):0x(\w+);", l)
    if not m: continue
    a = int(m.group(3), 16)
    if a in comp and comp[a][0] not in existing:
        nm, sz = comp[a]
        lines[i] = re.sub(r"^\S+ = ", f"{nm} = ", l)
        lines[i] = re.sub(r"size:0x\w+", f"size:0x{sz:X}", lines[i])
        if "scope:" not in lines[i]: lines[i] += " scope:global"
        existing.add(nm); renamed += 1
        print(f"  {m.group(1)} -> {nm}")
open(p, "w", newline="\n").write("\n".join(lines))
print(f"renamed {renamed}")
