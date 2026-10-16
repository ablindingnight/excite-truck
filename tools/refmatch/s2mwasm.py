# Convert a dtk-generated .s file's functions into Metrowerks inline-asm C functions.
# Usage: python s2mwasm.py in.s [renames as old=new ...]
import re, sys
src = open(sys.argv[1]).read().splitlines()
ren = dict(a.split("=") for a in sys.argv[2:])
out = []; infn = False
for l in src:
    m = re.match(r"\.fn (\S+), (\w+)", l)
    if m:
        infn = True
        static = "static " if m.group(2) == "local" else ""
        out.append(f"{static}asm void {m.group(1)}(void) {{\n    nofralloc")
        continue
    if l.startswith(".endfn"):
        infn = False; out.append("}\n"); continue
    if not infn: continue
    m = re.match(r"\.sym (\S+), \w+", l)
    if m: out.append(f"    entry {m.group(1)}"); continue
    m = re.match(r"\.L_([0-9A-F]+):", l)
    if m: out.append(f"L_{m.group(1)}:"); continue
    m = re.match(r"/\* [0-9A-F]+ [0-9A-F]+ +[0-9A-F ]{11} \*/\s+(.*)", l)
    if m:
        ins = re.sub(r"\.L_", "L_", m.group(1).strip())
        for a, b in ren.items(): ins = re.sub(r"\b" + re.escape(a) + r"\b", b, ins)
        out.append("    " + re.sub(r"\s+", " ", ins, count=1))
print("\n".join(out))
