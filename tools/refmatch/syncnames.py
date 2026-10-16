# Once the link layout matches the original, every function/object placed by a compiled (non-auto)
# object sits at its true address: rename symbols.txt entries at those addresses to the compiled
# names (fixes swapped look-alike labels left by relocation-masked matching).
# Usage: python syncnames.py [--apply]
import re, sys
mp = open("build/REXE01/main.elf.MAP", errors="replace").read()
sp = "config/REXE01/symbols.txt"
lines = open(sp).read().split("\n")
by_addr = {}
for i, l in enumerate(lines):
    m = re.match(r"^(\S+) = (\.\w+):0x(\w+);", l)
    if m: by_addr.setdefault((m.group(2), int(m.group(3), 16)), []).append(i)
names_in_use = {re.match(r"^(\S+) =", l).group(1) for l in lines if re.match(r"^(\S+) =", l)}
changes = []
for sec in (".text",):
    body = mp.split(f"\n{sec} section layout", 1)[1].split("section layout", 1)[0]
    for size, addr, name, obj in re.findall(r"^\s+[0-9a-f]{8} ([0-9a-f]{6}) ([0-9a-f]{8}) [0-9a-f]{8}\s+4 (\S+) \t(\S+)", body, re.M):
        if obj.startswith("auto_") or name.startswith("@") or "$" in name: continue
        idx = by_addr.get((sec, int(addr, 16)))
        if not idx: continue
        cur = re.match(r"^(\S+) =", lines[idx[0]]).group(1)
        if cur != name:
            changes.append((idx[0], cur, name, obj))
for i, cur, new, obj in changes:
    print(f"{cur:60} -> {new}  ({obj})")
if "--apply" in sys.argv:
    # two-phase so swaps don't collide
    for i, cur, new, obj in changes:
        lines[i] = re.sub(r"^\S+ =", lambda _m: f"{new} =", lines[i])
    open(sp, "w", newline="\n").write("\n".join(lines))
    print(f"applied {len(changes)}")
