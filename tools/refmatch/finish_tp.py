# Finish a TP-derived SDK file after hand fixes in libs/revolution/src/<path>:
# compile it, derive its split (dead-strip tolerant), check boundaries, register it, build,
# and if every function matches, link it (Matching), transfer names and verify the DOL hash.
# Usage: python finish_tp.py <path under libs/revolution/src, e.g. gx/GXInit.c> [--force]
#   --force: skip the boundary check (use when the neighbours are known to belong elsewhere)
import json, os, re, subprocess, sys, tempfile

path = sys.argv[1]
force = "--force" in sys.argv
unit = "revolution/" + path
stem = path[:-2] if path.endswith(".c") else path.rsplit(".", 1)[0]
tmp = tempfile.mkdtemp()
py = [sys.executable, "-I"]


def run(cmd, **kw):
    return subprocess.run(cmd, capture_output=True, text=True, **kw)


def build():
    subprocess.run(py + ["configure.py"], capture_output=True)
    subprocess.run([os.path.abspath("build/tools/dtk.exe"), "dol", "split", "config/REXE01/config.yml", "build/REXE01"], capture_output=True)
    try:
        r = run(["ninja"], timeout=400)
        out = r.stdout + r.stderr
    except subprocess.TimeoutExpired:
        subprocess.run(["taskkill", "/F", "/IM", "mwldeppc.exe"], capture_output=True)
        return False, "TIMEOUT"
    return "main.dol: OK" in out, "\n".join(l for l in out.splitlines() if "rror" in l or "undefined" in l or "multiply" in l or "FAILED" in l)[:2000]


def unit_report():
    r = json.load(open("build/REXE01/report.json"))
    for u in r["units"]:
        if u["name"] == "main/" + unit[:-2] or u["name"] == "main/" + unit.rsplit(".", 1)[0]:
            return u
    return None


# 1. compile our fixed source
objdir = os.path.join(tmp, "obj", os.path.dirname(stem))
os.makedirs(objdir, exist_ok=True)
obj = os.path.join(tmp, "obj", stem + ".o")
r = run(py + ["tools/refmatch/tpcc.py", "libs/revolution/src/" + path, obj])
if r.returncode:
    sys.exit("compile failed:\n" + r.stdout + r.stderr)

cfg = open("configure.py").read()
already = f'TpRevolution("{path}"' in cfg
splits = open("config/REXE01/splits.txt").read()
if unit + ":" not in splits:
    # 2. split
    r = run(py + ["tools/refmatch/autosplit2.py", "orig/REXE01/sys/main.dol", "config/REXE01/symbols.txt",
                  os.path.join(tmp, "obj"), "", stem])
    print(r.stderr.strip())
    if f"{stem}.c:" not in r.stdout:
        sys.exit("no split produced")
    names = os.path.join(tmp, "names.json")
    os.replace("tools/refmatch/names.json", names)
    cand = os.path.join(tmp, "cand.txt")
    open(cand, "w", newline="\n").write("revolution/" + r.stdout.strip() + "\n")
    print(open(cand).read())
    # 3. boundaries
    if not force:
        mj = [m for m in sys.argv[2:] if m.endswith(".json")]
        r = run(py + ["tools/refmatch/boundary_check.py", "config/REXE01/symbols.txt", "config/REXE01/splits.txt",
                      cand, os.path.join(tmp, "b.txt"), *mj])
        print(r.stdout.strip())
        if "1 bounded" not in r.stdout:
            sys.exit("boundary check failed (use --force if the neighbours belong to other files)")
    # 4. register + split + names
    if not already:
        anchor = '            TpRevolution("ipc/ipcProfile.c", Matching),\n'
        cfg = cfg.replace(anchor, anchor + f'            TpRevolution("{path}", NonMatching),\n')
        open("configure.py", "w", newline="\n").write(cfg)
    open("config/REXE01/splits.txt", "a", newline="\n").write("\n" + open(cand).read())
    subprocess.run(py + ["tools/refmatch/apply_names2.py", "config/REXE01/symbols.txt", names, cand], check=True)

ok, log = build()
u = unit_report()
if not u:
    sys.exit("unit not in report\n" + log)
bad = [(f["name"], round(f.get("fuzzy_match_percent", 0), 1)) for f in u["functions"] if f.get("fuzzy_match_percent", 0) < 100]
print(f"{unit}: code {u['measures'].get('fuzzy_match_percent', 0):.2f}%  DOL ok={ok}")
if bad:
    sys.exit(f"not matching yet: {bad}")
# 5. link it
cfg = open("configure.py").read().replace(f'TpRevolution("{path}", NonMatching)', f'TpRevolution("{path}", Matching)')
open("configure.py", "w", newline="\n").write(cfg)
r = run(py + ["tools/refmatch/xfer_names.py", "config/REXE01/symbols.txt", "build/REXE01/obj", "build/REXE01/src", unit.rsplit(".", 1)[0]])
print(r.stdout.strip())
ok, log = build()
print("LINKED, DOL matches" if ok else "LINK PROBLEM:\n" + log)
sys.exit(0 if ok else 1)
