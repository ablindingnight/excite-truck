# For each section, compare where the linker placed each object (from the .MAP) with its split
# start in splits.txt, and report objects whose placed size differs from their split size.
# Only prints where the placement delta changes, blaming the object placed just before.
# Usage: python mapcheck.py [map] [splits]
import os, re, sys
mp = open(sys.argv[1] if len(sys.argv) > 1 else "build/REXE01/main.elf.MAP", errors="replace").read()
sp = open(sys.argv[2] if len(sys.argv) > 2 else "config/REXE01/splits.txt").read()
want = {}
for blk in sp.split("\n\n"):
    L = blk.strip().splitlines()
    if not L or not L[0].endswith(":") or L[0].startswith("Sections"): continue
    base = os.path.basename(L[0][:-1]).rsplit(".", 1)[0] + ".o"
    for s, a, e in re.findall(r"(\S+)\s+start:0x(\w+) end:0x(\w+)", blk):
        want.setdefault((s, base.lower()), []).append((int(a, 16), int(e, 16)))
for sec in re.findall(r"^(\S+) section layout", mp, re.M):
    body = mp.split(f"\n{sec} section layout", 1)[1].split("section layout", 1)[0]
    rows = re.findall(r"^\s+[0-9a-f]{8} ([0-9a-f]{6,8}) ([0-9a-f]{8}) [0-9a-f]{8}\s+\d+ " + re.escape(sec) + r" \t(\S+)", body, re.M)
    delta = 0
    for size, addr, obj in rows:
        size, addr = int(size, 16), int(addr, 16)
        w = want.get((sec, obj.lower()))
        m = re.match(r"auto_\w+?_([0-9A-F]{8})_", obj)
        if w:
            exp_a, exp_e = w[0]
        elif m:
            exp_a = int(m.group(1), 16); exp_e = None
        else:
            continue
        if addr - exp_a != delta:
            print(f"{sec:10} {obj:40} placed {addr:#x}+{size:#x}  split {exp_a:#x}" + (f"+{exp_e-exp_a:#x}" if exp_e else "")
                  + f"  delta {delta:+#x} -> {addr - exp_a:+#x}  (blame previous: {prev})")
            delta = addr - exp_a
        prev = f"{obj} +{size:#x}" + (f" want {exp_e-exp_a:#x}" if exp_e else "")
