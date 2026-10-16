# Per reference unit: which target functions in its address span are matched (by this or another
# reference) and which are not. Skips spans already covered by splits.txt.
# Usage: python coverage.py symbols.txt splits.txt matches.json [more matches.json...]
import json, re, sys
from collections import defaultdict
syms_path, splits_path, *match_paths = sys.argv[1:]
tfuncs = {}
for l in open(syms_path):
    m = re.match(r"(\S+) = \.text:0x([0-9A-F]+); // type:function size:0x([0-9A-F]+)", l)
    if m: tfuncs[int(m.group(2), 16)] = (m.group(1), int(m.group(3), 16))
split_text = [(int(a, 16), int(b, 16)) for a, b in re.findall(r"\.text\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)", open(splits_path).read())]
covered = lambda a: any(lo <= a < hi for lo, hi in split_text)
hit_by = defaultdict(set)   # target addr -> {"tp:unit:func", ...}
units = defaultdict(list)
for mp in match_paths:
    tag = "tp" if "tp_" in mp else "ogws"
    for x in json.load(open(mp)):
        if len(x["hits"]) == 1 and x["size"] >= 0x10:
            a = x["hits"][0]
            hit_by[a].add(f"{tag}:{x['name']}")
            units[(tag, x["unit"])].append((a, x["name"], x["size"]))
rows = []
for (tag, unit), fs in units.items():
    lo = min(a for a, _, _ in fs); hi = max(a + s for a, _, s in fs)
    if covered(lo): continue
    inspan = sorted(a for a in tfuncs if lo <= a < hi)
    missing = [a for a in inspan if not hit_by.get(a)]
    rows.append((lo, hi, tag, unit, len(fs), len(inspan), missing))
for lo, hi, tag, unit, n, tot, missing in sorted(rows):
    short = unit.split("/src/")[-1] if "/src/" in unit else unit
    miss = ", ".join(f"{tfuncs[a][0]}({tfuncs[a][1]:#x})" for a in missing[:6]) + (" ..." if len(missing) > 6 else "")
    print(f"{lo:08X}-{hi:08X} {tag:4} {short:38} {n:3} ref-matched, {tot-len(missing):3}/{tot:<3} covered  {('missing: ' + miss) if missing else 'COMPLETE'}")
