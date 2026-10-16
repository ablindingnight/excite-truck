# Finish a unit already registered in configure.py (any library, any flags): build its object with
# the project's own rule, derive the split from it (dead-strip tolerant), apply names, verify, and
# link it if every function matches.
# Usage: python finish_any.py <unit, e.g. MSL/string.c or revolution/os/OSThread.c> [--force] [matches.json...]
import json, os, re, subprocess, sys, tempfile

unit = sys.argv[1]
force = "--force" in sys.argv
stem = unit.rsplit(".", 1)[0]
tmp = tempfile.mkdtemp()
py = [sys.executable, "-I"]
DTK = os.path.abspath("build/tools/dtk.exe")


def run(cmd, **kw):
    return subprocess.run(cmd, capture_output=True, text=True, **kw)


def build(target=None):
    run(py + ["configure.py"])
    run([DTK, "dol", "split", "config/REXE01/config.yml", "build/REXE01"])
    try:
        r = run(["ninja"] + ([target] if target else []), timeout=400)
    except subprocess.TimeoutExpired:
        run(["taskkill", "/F", "/IM", "mwldeppc.exe"])
        return False, "TIMEOUT"
    out = r.stdout + r.stderr
    return "main.dol: OK" in out, "\n".join(l for l in out.splitlines()
                                           if any(k in l for k in ("rror", "undefined", "multiply", "FAILED")))[:2000]


cfg = open("configure.py").read()
m = re.search(r'(Object|TpRevolution)\((NonMatching|Matching), "([^"]*)"', cfg[cfg.find(f'"{unit}"') - 40:cfg.find(f'"{unit}"') + len(unit) + 2]) \
    if f'"{unit}"' in cfg else None
if not m and f'"{unit.split("/", 1)[-1]}"' not in cfg:
    sys.exit(f"{unit} is not registered in configure.py")

obj = os.path.join("build", "REXE01", "src", *stem.split("/")) + ".o"
splits = open("config/REXE01/splits.txt").read()
if unit + ":" not in splits:
    # compile directly (no ninja rule exists until the unit has a split): find its library block
    pos = cfg.find(f'"{unit}"')
    tp_style = pos < 0
    if tp_style:
        pos = cfg.find(f'"{unit.split("/", 1)[-1]}"')
    lib_blk = cfg[cfg.rfind('"lib":', 0, pos):pos]
    var = re.search(r'"cflags":\s*(\w+)', lib_blk).group(1)
    ver = re.search(r'"mw_version":\s*"([^"]+)"', lib_blk).group(1)
    sd = re.search(r'"src_dir":\s*"([^"]+)"', lib_blk)
    src_dir = sd.group(1) if sd else "src"
    line = cfg[pos:cfg.find("\n", pos)]
    ex = re.search(r"extra_cflags=\[([^\]]*)\]", line)
    extra = " ".join(re.findall(r"['\"](-[^'\"]+)['\"]", ex.group(1))) if ex else ""
    src_rel = unit.split("/", 1)[1] if tp_style else unit
    root = os.path.join(tmp, "obj")
    os.makedirs(os.path.join(root, os.path.dirname(stem)), exist_ok=True)
    r = run(py + ["tools/refmatch/tpcc.py", f"{src_dir}/{src_rel}", os.path.join(root, stem + ".o"), var, ver],
            env={**os.environ, "EXTRA_CFLAGS": extra})
    if r.returncode:
        sys.exit("compile failed:\n" + r.stdout + r.stderr)
    r = run(py + ["tools/refmatch/autosplit2.py", "orig/REXE01/sys/main.dol", "config/REXE01/symbols.txt", root, "", stem])
    print(r.stderr.strip())
    if f"{stem}.c:" not in r.stdout and f"{stem}." not in r.stdout:
        sys.exit("no split produced")
    names = os.path.join(tmp, "names.json")
    os.replace("tools/refmatch/names.json", names)
    blk = r.stdout.strip().replace(f"{stem}.c:", f"{unit}:", 1)
    cand = os.path.join(tmp, "cand.txt")
    open(cand, "w", newline="\n").write(blk + "\n")
    print(blk)
    if not force:
        mj = [a for a in sys.argv[2:] if a.endswith(".json")]
        r = run(py + ["tools/refmatch/boundary_check.py", "config/REXE01/symbols.txt", "config/REXE01/splits.txt",
                      cand, os.path.join(tmp, "b.txt"), *mj])
        print(r.stdout.strip())
        if "1 bounded" not in r.stdout:
            sys.exit("boundary check failed (use --force if the neighbours belong to other files)")
    open("config/REXE01/splits.txt", "a", newline="\n").write("\n" + blk + "\n")
    subprocess.run(py + ["tools/refmatch/apply_names2.py", "config/REXE01/symbols.txt", names, cand], check=True)

ok, log = build()
rep = json.load(open("build/REXE01/report.json"))
u = next((x for x in rep["units"] if x["name"] == "main/" + stem), None)
if not u:
    sys.exit("unit not in report\n" + log)
bad = [(f["name"], round(f.get("fuzzy_match_percent", 0), 1)) for f in u["functions"] if f.get("fuzzy_match_percent", 0) < 100]
print(f"{unit}: code {u['measures'].get('fuzzy_match_percent', 0):.2f}%  DOL ok={ok}")
if bad:
    sys.exit(f"not matching yet: {bad}")
cfg = open("configure.py").read()
short = unit[len("revolution/"):] if f'TpRevolution("{unit[len("revolution/"):]}", NonMatching)' in cfg else None
if short:
    cfg = cfg.replace(f'TpRevolution("{short}", NonMatching)', f'TpRevolution("{short}", Matching)')
else:
    cfg = cfg.replace(f'Object(NonMatching, "{unit}"', f'Object(Matching, "{unit}"')
open("configure.py", "w", newline="\n").write(cfg)
r = run(py + ["tools/refmatch/xfer_names.py", "config/REXE01/symbols.txt", "build/REXE01/obj", "build/REXE01/src", stem])
print(r.stdout.strip())
ok, log = build()
print("LINKED, DOL matches" if ok else "LINK PROBLEM:\n" + log)
sys.exit(0 if ok else 1)
