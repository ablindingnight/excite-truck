# Adopt one file from TP's Home Button library (libs/revolution/src/homebuttonLib) into the HBM_tp
# lib: locate it in the DOL, replace overlapping unlinked Strikers HBM units, add its .text split,
# name its functions, link it, transfer names, place its initialized data, and report the result.
# Leaves the unit NonMatching (and the DOL unchanged) if anything is left to fix by hand.
# Usage: python adopt_hbm.py <path under homebuttonLib, e.g. nw4hbm/lyt/lyt_group.cpp> [--range LO HI]
import os, re, subprocess, sys
rel = sys.argv[1]
src = f"libs/revolution/src/homebuttonLib/{rel}"
unit = f"revolution/homebuttonLib/{rel}"
entry = f'TpRevolution("homebuttonLib/{rel}", '
py = [sys.executable, "-I"]
def run(*a, **kw):
    return subprocess.run(list(a), capture_output=True, text=True, **kw)
def build():
    run(sys.executable, "configure.py", "--map")
    run(os.path.abspath("build/tools/dtk.exe"), "dol", "split", "config/REXE01/config.yml", "build/REXE01")
    r = run("ninja", timeout=1500)
    return r.stdout + r.stderr
def set_status(status):
    cfg = open("configure.py").read()
    cfg = re.sub(re.escape(entry) + r"(Matching|NonMatching)\)", entry + status + ")", cfg)
    open("configure.py", "w", newline="\n").write(cfg)

os.makedirs("build/adopt", exist_ok=True)
obj = "build/adopt/x.o"  # relative: the compiler wrapper splits paths containing spaces
r = run(*py, "tools/refmatch/tpcc.py", src, obj, "cflags_rvl_tp_hbm", "GC/3.0a5")
if r.returncode: sys.exit("compile failed:\n" + r.stdout[-1500:])
if "--range" in sys.argv:
    # explicit range, for when weak copies elsewhere in the DOL confuse locate.py
    k = sys.argv.index("--range")
    lo, hi = int(sys.argv[k + 1], 16), int(sys.argv[k + 2], 16)
    sp0 = open("config/REXE01/splits.txt").read()
    over = [u for u, a, b in re.findall(r"^(\S+):\n\t\.text\s+start:0x(\w+) end:0x(\w+)", sp0, re.M)
            if int(a, 16) < hi and int(b, 16) > lo and u != unit]
else:
    loc = run(*py, "tools/refmatch/locate.py", obj).stdout
    m = re.search(r"unique hits span 0x(\w+)-0x(\w+); overlapping splits: \[(.*)\]", loc)
    if not m: sys.exit("not located:\n" + loc)
    # end = last uniquely-located function's end
    hits = [(int(a, 16), int(z, 16)) for z, a in re.findall(r"\s(0x[0-9a-f]+) (0x[0-9a-f]+)\s*$", loc, re.M)]
    lo = int(m.group(1), 16)
    hi = max(a + z for a, z in hits if lo <= a <= int(m.group(2), 16))
    over = [o.strip().strip("'") for o in m.group(3).split(",") if o.strip()]
cfg = open("configure.py").read()
for o in over:
    if f'Object(Matching, "{o}"' in cfg or re.search(r'TpRevolution\("' + re.escape(o.split("/", 1)[-1]) + r'", Matching\)', cfg):
        sys.exit(f"{lo:#x}-{hi:#x} overlaps linked unit {o}; resolve by hand")
if over:
    print("replacing unlinked units:", over)
    run(*py, "tools/refmatch/unsplit.py", *over)
cfg = open("configure.py").read()
if entry not in cfg:
    cfg = cfg.replace('        "lib": "HBM_tp",', '        "lib": "HBM_tp",', 1)
    anchor = '            TpRevolution("homebuttonLib/nw4hbm/lyt/lyt_layout.cpp", Matching),'
    cfg = cfg.replace(anchor, anchor + f"\n            {entry}NonMatching),", 1)
    open("configure.py", "w", newline="\n").write(cfg)
sp = open("config/REXE01/splits.txt").read()
if unit + ":" not in sp:
    open("config/REXE01/splits.txt", "a", newline="\n").write(f"\n{unit}:\n\t.text       start:0x{lo:08X} end:0x{hi:08X}\n")
build()
print(run(*py, "tools/refmatch/extend_text.py", unit, "--apply").stdout.strip())
set_status("Matching")
out = build()
stem = unit.rsplit(".", 1)[0]
print(run(*py, "tools/refmatch/xfer_names.py", "config/REXE01/symbols.txt", "build/REXE01/obj", "build/REXE01/src", stem).stdout.strip()[-800:])
out = build()
if "main.dol: OK" not in out:
    # place data sections by named anchors (vtables etc.), name data labels, then map-based placement
    print(run(*py, "tools/refmatch/place_data.py", unit, "--truncate", "--apply").stdout.strip())
    print(run(*py, "tools/refmatch/name_data.py", unit).stdout.strip()[-400:])
    out = build()
if "main.dol: OK" not in out:
    print(run(*py, "tools/refmatch/find_missing.py", "--apply").stdout.strip())
    out = build()
if "main.dol: OK" in out:
    print(f"LINKED {unit}: DOL matches")
else:
    print("\n".join(l for l in out.splitlines() if l.startswith("#  "))[:2000])
    mc = [l for l in run(*py, "tools/refmatch/mapcheck.py").stdout.splitlines() if "auto_fn_" not in l]
    print("\n".join(mc[:8]))
    print("\n".join(run(*py, "tools/refmatch/doldiff.py").stdout.splitlines()[:8]))
    set_status("NonMatching")
    build()
    print(f"NOT LINKED {unit}: left NonMatching (see above)")
