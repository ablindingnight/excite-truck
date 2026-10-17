# Link a registered Object(...) unit whose .text split is already set: name its functions, mark it
# Matching, transfer names, place its data (named anchors, then map-based), point its weak
# @STRING@ references at the copies the original uses, and check the DOL. Leaves the unit
# NonMatching (DOL unchanged) if the hash doesn't match, printing what's left.
# Usage: python link_unit.py <unit as in splits.txt, e.g. RVL_SDK/hbm/nw4hbm/snd/snd_Channel.cpp>...
# Several units are linked together (for units that call into each other's ranges).
import os, re, subprocess, sys
units = sys.argv[1:]
py = [sys.executable, "-I"]
def run(*a, **kw):
    return subprocess.run(list(a), capture_output=True, text=True, **kw)
def build():
    run(sys.executable, "configure.py", "--map")
    r = run(os.path.abspath("build/tools/dtk.exe"), "dol", "split", "config/REXE01/config.yml", "build/REXE01")
    if "Caused by" in r.stdout + r.stderr:
        print((r.stdout + r.stderr)[-600:])
    r = run("ninja", timeout=1500)
    return r.stdout + r.stderr
def set_status(st):
    for unit in units:
        set_unit_status(unit, st)
def set_unit_status(unit, st):
    cfg = open("configure.py").read()
    cfg = re.sub(r'Object\((Matching|NonMatching), "' + re.escape(unit) + '"', f'Object({st}, "{unit}"', cfg)
    if unit.startswith("revolution/"):  # TP sources: TpRevolution("<path>", Matching)
        tp = re.escape(unit[len("revolution/"):])
        cfg = re.sub(r'TpRevolution\("' + tp + r'", (Matching|NonMatching)', f'TpRevolution("{unit[len("revolution/"):]}", {st}', cfg)
    open("configure.py", "w", newline="\n").write(cfg)
def ok(out):
    # ninja prints nothing about the hash when a step changed nothing, so compare the DOLs directly
    if "Link failed" in out or "FAILED" in out:
        return False
    return "IDENTICAL" in run(*py, "tools/refmatch/doldiff.py").stdout

# snapshot the config so a failed attempt leaves no renamed symbols or moved data splits behind
SNAP = {p: open(p).read() for p in ("config/REXE01/symbols.txt", "config/REXE01/splits.txt")}
build()
for unit in units:
    print(run(*py, "tools/refmatch/extend_text.py", unit, "--apply").stdout.strip())
set_status("Matching")
out = build()
for unit in units:
    stem = unit.rsplit(".", 1)[0]
    print(run(*py, "tools/refmatch/xfer_names.py", "config/REXE01/symbols.txt", "build/REXE01/obj", "build/REXE01/src", stem).stdout.strip()[-600:])
out = build()
steps = [["place_data.py", u, "--apply"] for u in units] + [["find_missing.py", "--apply"]] + [["pair_strings.py", u, "--apply"] for u in units]
for step in steps:
    if ok(out): break
    print(run(*py, "tools/refmatch/" + step[0], *step[1:]).stdout.strip()[-800:])
    out = build()
if ok(out):
    for unit in units:
        print(run(*py, "tools/refmatch/name_data.py", unit).stdout.strip()[-300:])
    out = build()
if ok(out):
    print(f"LINKED {' '.join(units)}")
else:
    print("\n".join(l for l in out.splitlines() if l.startswith("#  ") and "FORCEACTIVE" not in l and "Ignored" not in l)[:1500])
    print("\n".join([l for l in run(*py, "tools/refmatch/mapcheck.py").stdout.splitlines() if "auto_fn_" not in l][:6]))
    print("\n".join(run(*py, "tools/refmatch/doldiff.py").stdout.splitlines()[:6]))
    set_status("NonMatching")
    for p, text in SNAP.items():
        open(p, "w", newline="\n").write(text)
    out = build()
    print(f"NOT LINKED {' '.join(units)}: left NonMatching, config restored ({'DOL OK' if ok(out) else 'DOL STILL DIFFERS'})")
