# Replace auto-generated symbols inside split data ranges with the reference object's exact symbols.
# Usage: python apply_names2.py symbols.txt names.json splits_fragment.txt
import json, re, sys
syms_path, names_path, splits_path = sys.argv[1:4]
names = {int(k, 16): v for k, v in json.load(open(names_path)).items()}
ranges = [(int(a, 16), int(b, 16)) for a, b in re.findall(r"\.\w+\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)", open(splits_path).read())]
inr = lambda a: any(lo <= a < hi for lo, hi in ranges)
out = []
for l in open(syms_path).read().splitlines():
    m = re.match(r"(\S+) = (\S+):0x([0-9A-F]+);", l)
    if m and inr(int(m.group(3), 16)) and int(m.group(3), 16) in names:
        continue  # replaced below
    if m and inr(int(m.group(3), 16)) and re.match(r"(lbl_|@|gap_|fn_)", m.group(1)) and "type:object" in l:
        continue  # stale auto symbol inside a known range
    out.append(l)
taken = {l.split(" = ")[0] for l in out}
for a, (name, sec, size, kind, bind) in sorted(names.items()):
    if not inr(a): continue
    scope = {0: "local", 1: "global", 2: "weak"}[bind]
    if scope != "local" and name in taken:
        print("skip duplicate global", name); continue
    extra = " data:string" if kind == "object" and name.startswith("@") and sec in (".data", ".rodata") else ""
    out.append(f"{name} = {sec}:0x{a:08X}; // type:{kind} size:0x{size:X} scope:{scope}{extra}")
    taken.add(name)
open(syms_path, "w", newline="\n").write("\n".join(out) + "\n")
