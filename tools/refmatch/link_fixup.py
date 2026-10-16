# Repeatedly build; when the link fails, demote (Matching -> NonMatching) the units responsible:
#  - objects named in "multiply-defined ... in X.o" / "Referenced from ... in X.o" (non-auto)
#  - units whose split contains the address of an undefined symbol (lbl_XXXXXXXX / name_XXXXXXXX)
# Only units whose path contains the given filter are demoted.
# Usage: python link_fixup.py <unit-path-filter, e.g. RVL_SDK/hbm> [max_iterations]
import re, subprocess, sys
flt = sys.argv[1]; maxit = int(sys.argv[2]) if len(sys.argv) > 2 else 10
for it in range(maxit):
    subprocess.run([sys.executable, "-I", "configure.py"], capture_output=True)
    try:
        r = subprocess.run(["ninja"], capture_output=True, text=True, timeout=900)
        log = r.stdout + r.stderr
    except subprocess.TimeoutExpired:
        subprocess.run(["taskkill", "/F", "/IM", "mwldeppc.exe"], capture_output=True); print("timeout"); sys.exit(1)
    if "main.dol: OK" in log:
        print(f"iteration {it}: LINKED, DOL OK"); sys.exit(0)
    if "Linker Error" not in log:
        print(f"iteration {it}: failed without linker errors:\n" + "\n".join(l for l in log.splitlines() if "FAILED" in l or "rror" in l)[:1500]); sys.exit(1)
    splits = []
    for blk in open("config/REXE01/splits.txt").read().split("\n\n"):
        name = blk.strip().splitlines()[0].rstrip(":") if blk.strip() else ""
        for a, b in re.findall(r"start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)", blk):
            splits.append((int(a, 16), int(b, 16), name))
    bad = set()
    for b in log.split("### "):
        if "Linker Error" not in b:
            continue
        flat = " ".join(l.strip("# ").strip() for l in b.splitlines())
        for m in re.finditer(r"in\s+(\S+?)\.o\b", flat):
            if not m.group(1).startswith("auto_"):
                bad.add(("obj", m.group(1)))
        m = re.search(r"undefined: '([^']*?)_?([0-9A-F]{8})'", flat)
        if m:
            a = int(m.group(2), 16)
            for lo, hi, name in splits:
                if lo <= a < hi:
                    bad.add(("unit", name))
    cfg = open("configure.py").read(); n = 0
    for kind, x in sorted(bad):
        if kind == "unit":
            pats = [x]
        else:
            pats = [u for u in re.findall(r'Object\(Matching, "([^"]+)"', cfg) if u.rsplit("/", 1)[-1].rsplit(".", 1)[0] == x]
        for u in pats:
            if flt in u and f'Object(Matching, "{u}"' in cfg:
                cfg = cfg.replace(f'Object(Matching, "{u}"', f'Object(NonMatching, "{u}"'); n += 1
                print(f"iteration {it}: demote {u}")
    open("configure.py", "w", newline="\n").write(cfg)
    if n == 0:
        print(f"iteration {it}: could not attribute errors:\n" + "\n".join(l for l in log.splitlines() if "undefined" in l or "multiply" in l)[:1500]); sys.exit(1)
print("gave up"); sys.exit(1)
