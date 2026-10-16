# For each named reference file, find the unsplit gap of the target it lives in (between existing
# .text splits) and align both references (TP and ogws) against that gap.
# Usage: python gaps.py <tp_obj_root> <og_obj_root> name...   (name like gx/GXLight or OS/OSCache)
import os, re, subprocess, sys, glob
tp_root, og_root, *names = sys.argv[1:]
ex = sorted((int(a, 16), int(b, 16)) for a, b in re.findall(r"\.text\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)", open("config/REXE01/splits.txt").read()))
def find(root, name):
    hits = [p for p in glob.glob(os.path.join(root, "**", "*.o"), recursive=True) if p.lower().replace("\\", "/").endswith("/" + name.lower() + ".o")]
    return hits[0] if hits else None
for n in names:
    objs = [o for o in (find(tp_root, n), find(og_root, n)) if o]
    # locate via a quick align to find matched span
    r = subprocess.run([sys.executable, "-I", "tools/refmatch/align.py", "orig/REXE01/sys/main.dol", "config/REXE01/symbols.txt", objs[0]], capture_output=True, text=True)
    m = re.search(r"target span 0x([0-9a-f]+)-0x([0-9a-f]+)", r.stdout)
    if not m: print(n, "no span"); continue
    a = int(m.group(1), 16)
    lo = max([e for s, e in ex if e <= a], default=0)
    hi = min([s for s, e in ex if s > a], default=0x80300000)
    print(f"######## {n}  gap {lo:#x}-{hi:#x}")
    for o in objs:
        r = subprocess.run([sys.executable, "-I", "tools/refmatch/align.py", "orig/REXE01/sys/main.dol", "config/REXE01/symbols.txt", o, "--from", f"{lo:x}", "--to", f"{hi:x}"], capture_output=True, text=True)
        tag = "TP " if "tp_obj" in o else "OG "
        bad = [l for l in r.stdout.splitlines() if l.startswith("  !")]
        good = sum(1 for l in r.stdout.splitlines() if l.startswith("  ="))
        print(f"  {tag} matched {good}, differing {len(bad)}")
        for l in bad: print("     " + l.strip())
