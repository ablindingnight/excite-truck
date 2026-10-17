# Register a game unit: add its splits (before MSL/alloc.c, i.e. in the low game range, or before a
# given anchor unit), add Object(NonMatching, ...) to the "game" lib in configure.py, rename
# symbols, and optionally shrink symbols that run past a split end (alignment padding).
# Usage: python add_unit.py game/x.cpp --sec .text:0x8..:0x8.. [--sec .sbss:..:..] \
#            [--ren OLD=NEW]... [--size SYM=0x4]... [--before UNIT]
# NEW may contain a literal backslash (e.g. __sinit_\x_cpp); pass it single-quoted from bash.
import argparse, re

ap = argparse.ArgumentParser()
ap.add_argument("unit")
ap.add_argument("--sec", action="append", default=[])
ap.add_argument("--ren", action="append", default=[])
ap.add_argument("--size", action="append", default=[])
ap.add_argument("--before", default="MSL/alloc.c")
a = ap.parse_args()

p = "config/REXE01/splits.txt"
s = open(p).read()
if "\n%s:\n" % a.unit in s:
    raise SystemExit("unit already in splits.txt")
lines = []
for sec in a.sec:
    name, lo, hi = sec.split(":")
    lines.append("\t%-11s start:%s end:%s" % (name, lo.upper().replace("0X", "0x"), hi.upper().replace("0X", "0x")))
anchor = "\n%s:\n" % a.before
assert anchor in s, a.before
s = s.replace(anchor, "\n%s:\n%s\n%s" % (a.unit, "\n".join(lines), anchor), 1)
open(p, "w").write(s)

p = "configure.py"
s = open(p).read()
m = re.search(r'"lib": "game",.*?"objects": \[\n(.*?)        \],', s, re.S)
assert m
objs = m.group(1)
s = s[:m.end(1)] + '            Object(NonMatching, "%s"),\n' % a.unit + s[m.end(1):]
open(p, "w").write(s)

p = "config/REXE01/symbols.txt"
L = open(p).read().split("\n")
ren = dict(r.split("=", 1) for r in a.ren)
size = dict(r.split("=", 1) for r in a.size)
done = set()
for i, l in enumerate(L):
    k = l.split(" = ")[0]
    if k in ren:
        L[i] = ren[k] + l[len(k):]
        done.add(k)
    if k in size:
        L[i] = re.sub(r"size:0x[0-9A-Fa-f]+", "size:" + size[k], L[i])
        done.add(k)
open(p, "w").write("\n".join(L))
missing = (set(ren) | set(size)) - done
print("registered", a.unit, "; missing symbols:", sorted(missing) if missing else "none")
