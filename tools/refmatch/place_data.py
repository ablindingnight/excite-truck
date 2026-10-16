# Place a unit's data sections using named anchors: for each initialized/uninitialized data
# section of the compiled object, find symbols whose names also appear in symbols.txt; their
# address minus their offset gives the section start. Checks that anchors agree, that the range
# doesn't cross other splits, and snaps the end to the next symbol boundary. --apply writes splits.
# --truncate ends a range at the next split (for objects whose shared template strings are
# deduplicated by the linker, making the placed size smaller than the compiled one).
# Usage: python place_data.py <unit, e.g. revolution/homebuttonLib/nw4hbm/lyt/lyt_group.cpp> [--apply] [--truncate]
import re, struct, sys
unit = sys.argv[1]
obj = "build/REXE01/src/" + unit.rsplit(".", 1)[0] + ".o"
SECS = (".rodata", ".data", ".bss", ".sdata", ".sbss", ".sdata2", ".sbss2")
b = open(obj, "rb").read()
sh, = struct.unpack(">I", b[0x20:0x24]); es, n, si = struct.unpack(">HHH", b[0x2E:0x34])
S = [struct.unpack(">10I", b[sh + i * es:sh + i * es + 40]) for i in range(n)]; so = S[si][4]
names = [b[so + s[0]:b.index(b"\0", so + s[0])].decode() for s in S]
st = [i for i, s in enumerate(S) if s[1] == 2][0]; strs = S[S[st][6]][4]
syms_txt = open("config/REXE01/symbols.txt").read()
addr_of = {nm: (sec, int(a, 16)) for nm, sec, a in re.findall(r"^(\S+) = (\.\w+):0x(\w+);", syms_txt, re.M)}
bounds = sorted((int(a, 16), int(a, 16) + int(z, 16)) for a, z in re.findall(r"= \.\w+:0x(\w+); // type:\w+ size:0x(\w+)", syms_txt))
sp = open("config/REXE01/splits.txt").read()
claims = [(s, int(a, 16), int(e, 16)) for s, a, e in re.findall(r"(\S+)\s+start:0x(\w+) end:0x(\w+)", sp)]
i0 = sp.index(unit + ":\n"); j0 = sp.find("\n\n", i0); j0 = len(sp) if j0 < 0 else j0
have = {l.split()[0] for l in sp[i0:j0].splitlines()[1:]}
anchors = {}
for k in range(0, S[st][5], 16):
    o, v, sz, info, oth, shn = struct.unpack(">IIIBBH", b[S[st][4] + k:S[st][4] + k + 16])
    if not (0 < shn < n) or names[shn] not in SECS: continue
    nm = b[strs + o:b.index(b"\0", strs + o)].decode()
    if nm in addr_of and addr_of[nm][0] == names[shn] and not nm.startswith(("@", "lbl_")):
        anchors.setdefault(names[shn], set()).add(addr_of[nm][1] - v)
size = {}
for i, s in enumerate(S):
    if names[i] in SECS and s[5]: size[names[i]] = size.get(names[i], 0) + s[5]
adds = []
for sec, sz in size.items():
    if sec in have:
        continue
    starts = anchors.get(sec)
    if not starts:
        print(f"{sec:8} {sz:#x}: no named anchor"); continue
    if len(starts) > 1:
        print(f"{sec:8} {sz:#x}: anchors disagree {[hex(x) for x in starts]}"); continue
    a = starts.pop(); e = a + sz
    # snap the end to the end of the symbol containing it (covers padding the linker would junk-fill)
    for sa, se in bounds:
        if sa < e < se: e = se
    e = (e + 3) & ~3
    clash = [c for c in claims if c[0] == sec and c[1] < e and a < c[2]]
    if clash and "--truncate" in sys.argv and all(c[1] > a for c in clash):
        # shared @STRING@/weak data is deduplicated at link time: stop at the next split
        e = min(c[1] for c in clash); clash = []
    if clash:
        print(f"{sec:8} {a:#x}-{e:#x}: clashes with {[(hex(c[1]), hex(c[2])) for c in clash]}"); continue
    print(f"{sec:8} {a:#x}-{e:#x}")
    adds.append((sec, a, e))
if "--apply" in sys.argv and adds:
    order = [".text", ".ctors", ".dtors", "extab", "extabindex", ".rodata", ".data", ".bss", ".sdata", ".sbss", ".sdata2", ".sbss2"]
    L = sp[i0:j0].splitlines(); cur = {l.split()[0]: l for l in L[1:]}
    for sec, a, e in adds:
        cur[sec] = f"\t{sec:<11} start:0x{a:08X} end:0x{e:08X}"
    sp = sp[:i0] + "\n".join([L[0]] + [cur[k] for k in order if k in cur]) + sp[j0:]
    open("config/REXE01/splits.txt", "w", newline="\n").write(sp)
    print(f"applied {len(adds)}")
