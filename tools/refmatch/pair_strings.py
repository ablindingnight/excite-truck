# Name the weak @STRING@ copies that a unit's code really uses in the original DOL.
# MWLD resolves a weak @STRING@ (assert strings of inline/template functions) to its FIRST copy in
# link order, so the original code often points at a copy in an earlier file while our build points
# at the copy in the unit's own .data. When a unit's code matches except for these addresses, this
# lines up the original asm with our compiled object instruction by instruction, finds each lbl_
# the original references where ours references an @STRING@ symbol, and renames that lbl_ in
# symbols.txt to the @STRING@ name (scope:weak) so our references land on the same copy.
# Usage: python pair_strings.py <unit, e.g. RVL_SDK/hbm/nw4hbm/snd/snd_SoundPlayer.cpp> [--apply]
import os, re, subprocess, sys

unit = sys.argv[1]
stem = unit.rsplit(".", 1)[0]
et_asm = f"build/REXE01/asm/{stem}.s"
obj = f"build/REXE01/src/{stem}.o"
tmp = os.path.join(os.environ.get("FNDIFF_TMP", "build"), "pair_strings.s")
subprocess.run([os.path.abspath("build/tools/dtk.exe"), "elf", "disasm", obj, tmp], capture_output=True)

def fns(path):
    out, cur = {}, None
    for l in open(path, encoding="utf-8", errors="replace"):
        m = re.match(r'^\.fn "?([^",]+)"?,', l)
        if m:
            cur = m.group(1); out[cur] = []; continue
        if l.startswith(".endfn"):
            cur = None; continue
        if cur and l.startswith("/*"):
            out[cur].append(l.split("*/", 1)[1].strip())
    return out

et, ours = fns(et_asm), fns(tmp)
pairs = {}
for name, ins in et.items():
    for a, b in zip(ins, ours.get(name, [])):
        ma = re.search(r"(lbl_[0-9A-F]{8})@(ha|l|sda21)\b", a)
        mb = re.search(r'"?(@STRING@[^"@]+(?:@\d+)?)"?@(ha|l|sda21)\b', b)
        if ma and mb:
            pairs[ma.group(1)] = mb.group(1)

import struct
b = open(obj, "rb").read()
sh, = struct.unpack(">I", b[0x20:0x24]); es, n, si = struct.unpack(">HHH", b[0x2E:0x34])
S = [struct.unpack(">10I", b[sh + i * es:sh + i * es + 40]) for i in range(n)]
st = [i for i, s in enumerate(S) if s[1] == 2][0]; strs = S[S[st][6]][4]
sizes = {}
for k in range(0, S[st][5], 16):
    o, v, sz, info, oth, shn = struct.unpack(">IIIBBH", b[S[st][4] + k:S[st][4] + k + 16])
    sizes[b[strs + o:b.index(b"\0", strs + o)].decode(errors="replace")] = sz

p = "config/REXE01/symbols.txt"
lines = open(p).read().split("\n")
for i, l in enumerate(lines):
    m = re.match(r"^(lbl_[0-9A-F]{8}) = ", l)
    if m and m.group(1) in pairs:
        nm = pairs[m.group(1)]
        print(f"  {m.group(1)} -> {nm}")
        l = l.replace(m.group(1), nm, 1)
        if nm in sizes:  # the original's label may span several strings
            l = re.sub(r"size:0x\w+", f"size:0x{sizes[nm]:X}", l)
        lines[i] = re.sub(r" scope:\w+", "", l) + " scope:weak"
if "--apply" in sys.argv:
    open(p, "w", newline="\n").write("\n".join(lines))
    print("applied")
