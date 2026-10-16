# Per SDK file: which target functions match TP's version, ogws's version, or neither.
# File extent = cluster of target functions matched (by either reference) under the same file name,
# bridging across unmatched functions only when both sides belong to the file.
# Usage: python worksheet.py symbols.txt splits.txt tp_matches.json ogws_matches.json
import json, re, sys
from collections import defaultdict
syms_p, splits_p, tp_p, og_p = sys.argv[1:5]
tf = {}
for l in open(syms_p):
    m = re.match(r"(\S+) = \.text:0x([0-9A-F]+); // type:function size:0x([0-9A-F]+)", l)
    if m: tf[int(m.group(2), 16)] = (m.group(1), int(m.group(3), 16))
addrs = sorted(tf)
ex = [(int(a, 16), int(b, 16)) for a, b in re.findall(r"\.text\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)", open(splits_p).read())]
split = lambda a: any(lo <= a < hi for lo, hi in ex)
by = defaultdict(dict)  # addr -> {"tp": name, "og": name} with file key
fileof = defaultdict(set)
for tag, p in (("tp", tp_p), ("og", og_p)):
    for x in json.load(open(p)):
        if len(x["hits"]) == 1 and x["size"] >= 8:
            key = x["unit"].split("/")[-1].lower()
            by[x["hits"][0]][tag] = x["name"]
            fileof[x["hits"][0]].add(key)
files = defaultdict(list)
for a, ks in fileof.items():
    if not split(a):
        for k in ks: files[k].append(a)
rows = []
for k, hs in files.items():
    hs = sorted(a for a in hs if a in tf)
    if not hs: continue
    # walk from first to last hit, include unmatched gaps between this file's hits
    idx = [addrs.index(a) for a in hs]
    lo, hi = min(idx), max(idx)
    seq = addrs[lo:hi + 1]
    if any(split(a) for a in seq): continue
    status = []
    for a in seq:
        d = by.get(a, {})
        mine = k in fileof.get(a, set())
        if not mine and d: status.append("X")       # belongs to another file: extent is wrong
        elif "tp" in d and "og" in d: status.append("B")
        elif "tp" in d: status.append("T")
        elif "og" in d: status.append("O")
        else: status.append("-")
    if "X" in status: continue
    rows.append((seq[0], k, "".join(status), [(tf[a][0], tf[a][1]) for a, s in zip(seq, status) if s == "-"]))
for a, k, st, miss in sorted(rows):
    print(f"{a:08X} {k:22} {len(st):3} funcs  B={st.count('B'):<3} T={st.count('T'):<3} O={st.count('O'):<3} none={st.count('-'):<3} {st}")
    if miss: print("          unmatched:", ", ".join(f"{n}({s:#x})" for n, s in miss[:8]) + (" ..." if len(miss) > 8 else ""))

if len(sys.argv) > 5:
    want = sys.argv[5].lower()
    hs = sorted(a for a in files.get(want, []) if a in tf)
    lo, hi = addrs.index(hs[0]), addrs.index(hs[-1])
    for a in addrs[max(0, lo - 2):hi + 3]:
        d = by.get(a, {})
        tag = "split" if split(a) else ("T " + d["tp"] if "tp" in d else "O " + d["og"] if "og" in d else "-")
        print(f"  {a:08X} {tf[a][1]:#6x} {tf[a][0]:28} {tag}  {'' if want in fileof.get(a, set()) or not d else '(other file: ' + ','.join(fileof[a]) + ')'}")
