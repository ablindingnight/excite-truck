# Check that each candidate split's .text is bounded by functions belonging to other files.
# Usage: python boundary_check.py symbols.txt splits.txt candidates.txt out.txt matches.json...
import json, re, sys
from collections import defaultdict
syms_p, splits_p, cand_p, out_p, *mjs = sys.argv[1:]
tf = sorted(int(m.group(1), 16) for m in re.finditer(r"\.text:0x([0-9A-F]+); // type:function", open(syms_p).read()))
ex = [(int(a, 16), int(b, 16)) for a, b in re.findall(r"\.text\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)", open(splits_p).read())]
owner = defaultdict(set)
for mp in mjs:
    for x in json.load(open(mp)):
        if len(x["hits"]) == 1 and x["size"] >= 0x10:
            owner[x["hits"][0]].add(x["unit"].split("/")[-1].lower())
def claimed_elsewhere(a, me):
    if any(lo <= a < hi for lo, hi in ex): return True
    o = owner.get(a)
    return bool(o) and me not in o
keep = []
for b in open(cand_p).read().strip().split("\n\n"):
    name = b.splitlines()[0].rstrip(":")
    lo, hi = [int(x, 16) for x in re.search(r"\.text\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)", b).groups()]
    me = name.split("/")[-1][:-2].lower()
    before = max((a for a in tf if a < lo), default=None)
    after = min((a for a in tf if a >= hi), default=None)
    okb = before is None or claimed_elsewhere(before, me)
    oka = after is None or claimed_elsewhere(after, me)
    status = "ok" if okb and oka else "OPEN " + ("before " if not okb else "") + ("after" if not oka else "")
    print(f"{status:14} {name:32} {lo:#x}-{hi:#x}")
    if okb and oka: keep.append(b)
open(out_p, "w", newline="\n").write("\n\n".join(keep) + "\n")
print(len(keep), "bounded")
