# Harvest structure-offset differences between the original code and a reference object.
# For every original function in a range whose reference counterpart has the same instruction
# count (region.py "=" or "size~" pairing), compare the code line by line; where two lines differ
# only in a load/store displacement (or an addi immediate), record "ours -> original". Prints the
# pairs with counts, so a changed struct layout shows up as a consistent offset mapping.
# Usage: python offsetmap.py <ref.o> <lo hex> <hi hex> [--fns]
import collections, os, re, subprocess, sys

obj, lo, hi = sys.argv[1], sys.argv[2], sys.argv[3]
show_fns = "--fns" in sys.argv
tmp = os.path.join(os.environ.get("FNDIFF_TMP", "build"), "offsetmap.s")
subprocess.run([os.path.abspath("build/tools/dtk.exe"), "elf", "disasm", obj, tmp], capture_output=True)


def fns_of(path, wanted=None):
    out, cur = {}, None
    for l in open(path, encoding="utf-8", errors="replace"):
        m = re.match(r'^\.fn "?([^",]+)"?,', l)
        if m:
            cur = m.group(1)
            out[cur] = []
            continue
        if l.startswith(".endfn"):
            cur = None
            continue
        if cur and l.startswith("/*"):
            out[cur].append(l.split("*/", 1)[1].strip())
    return out


ours = fns_of(tmp)
reg = subprocess.run([sys.executable, "-I", "tools/refmatch/region.py", obj, lo, hi], capture_output=True, text=True).stdout
pairs = []
for l in reg.splitlines():
    m = re.match(r"(0x\w+)\s+(0x\w+)\s+(\S+)\s+(=|size~)\s+(\S+)", l)
    if m:
        cands = re.split(r",(?=[^0-9])", m.group(5))
        if len(cands) == 1:
            pairs.append((m.group(3), cands[0]))

et_all = {}
for f in os.listdir("build/REXE01/asm"):
    if f.endswith(".s"):
        et_all.update(fns_of(os.path.join("build/REXE01/asm", f)))

DISP = re.compile(r"^(\w+\.?) (\w+), (-?0x[0-9a-f]+|\d+)\((r\d+)\)$")
IMM = re.compile(r"^(addi) (r\d+), (r\d+), (-?0x[0-9a-f]+)$")
counts = collections.Counter()
where = collections.defaultdict(set)
for et_name, ref_name in pairs:
    a, b = et_all.get(et_name, []), ours.get(ref_name, [])
    if len(a) != len(b) or not a:
        continue
    for x, y in zip(a, b):
        if x == y:
            continue
        mx, my = DISP.match(x) or IMM.match(x), DISP.match(y) or IMM.match(y)
        if mx and my and mx.group(1) == my.group(1):
            vx = int(mx.group(3), 16) if "0x" in mx.group(3) else int(mx.group(3))
            vy = int(my.group(3), 16) if "0x" in my.group(3) else int(my.group(3))
            key = (mx.group(1), vy, vx)
            counts[key] += 1
            where[key].add(ref_name)
for (op, ref_off, et_off), n in sorted(counts.items(), key=lambda kv: (kv[0][1], kv[0][0])):
    line = "%-6s ours %#7x -> ET %#7x  (delta %+#x) x%d" % (op, ref_off, et_off, et_off - ref_off, n)
    if show_fns:
        line += "  " + " ".join(sorted(where[(op, ref_off, et_off)]))[:120]
    print(line)
