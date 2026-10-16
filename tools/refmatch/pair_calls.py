# List call targets that differ between a unit's original asm and our compiled object
# (instruction-aligned, so only useful when the unit's functions already match): each line is
# "<original target> <-> <our target>". With --apply, rename original fn_XXXXXXXX labels to our names.
# Usage: python pair_calls.py <unit as in splits.txt> [--apply]
import os, re, subprocess, sys
unit = sys.argv[1]; stem = unit.rsplit(".", 1)[0]
tmp = os.path.join(os.environ.get("FNDIFF_TMP", "build"), "pair_calls.s")
subprocess.run([os.path.abspath("build/tools/dtk.exe"), "elf", "disasm", f"build/REXE01/src/{stem}.o", tmp], capture_output=True)
def fns(path):
    out, cur = {}, None
    for l in open(path, encoding="utf-8", errors="replace"):
        m = re.match(r'^\.fn "?([^",]+)"?,', l)
        if m: cur = m.group(1); out[cur] = []; continue
        if l.startswith(".endfn"): cur = None; continue
        if cur and l.startswith("/*"): out[cur].append(l.split("*/", 1)[1].strip())
    return out
et, ours = fns(f"build/REXE01/asm/{stem}.s"), fns(tmp)
pairs = {}
for n, ins in et.items():
    for a, b in zip(ins, ours.get(n, [])):
        if re.match(r"(bl|b) ", a) and re.match(r"(bl|b) ", b):
            ta, tb = a.split()[1].strip('"'), b.split(None, 1)[1].strip().strip('"')
            if ta != tb and not tb.startswith("."): pairs[ta] = tb
for a, b in sorted(pairs.items()): print(a, "<->", b)
if "--apply" in sys.argv:
    p = "config/REXE01/symbols.txt"; s = open(p).read()
    for a, b in pairs.items():
        if re.match(r"fn_[0-9A-F]{8}$", a):
            s = re.sub(r"^" + a + r" = ", b + " = ", s, flags=re.M)
    open(p, "w", newline="\n").write(s); print("applied (fn_ labels only)")
