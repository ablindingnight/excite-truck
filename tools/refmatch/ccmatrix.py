# Compile a scratch source with many compilers/flag sets and report which reproduce target functions.
# Usage: python ccmatrix.py <src> <func>=<target_addr_hex> [<func>=<addr> ...]
import itertools, os, subprocess, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from autosplit2_lib import load_target, read_elf, fn_hits

src = sys.argv[1]
wants = [(a.split("=")[0], int(a.split("=")[1], 16)) for a in sys.argv[2:]]
words, by_word, tfuncs, tnames = load_target("orig/REXE01/sys/main.dol", "config/REXE01/symbols.txt")
COMPILERS = ["GC/2.6", "GC/2.7", "GC/3.0a3", "GC/3.0a5", "GC/3.0a5.2", "Wii/1.0RC1", "Wii/1.0a", "Wii/1.0", "Wii/1.1", "Wii/1.3"]
OPTS = [["-O4,p"], ["-O4,s"], ["-O3"], ["-O2"]]
INLINE = [["-inline", "auto"], ["-inline", "noauto"]]
PROC = os.environ.get("PROC", "gekko")
base = ["-nodefaults", "-proc", PROC, "-align", "powerpc", "-enum", "int", "-fp", "hardware", "-Cpp_exceptions", "off",
        "-RTTI", "off", "-fp_contract", os.environ.get("FPC", "on"), "-str", "reuse", "-maxerrors", "1", "-nosyspath",
        "-i", "include", "-i", "include/MSL", "-i", "include/MSL/internal", "-lang=c++"]
tmp = tempfile.mkdtemp()
results = []
for cc, opt, inl in itertools.product(COMPILERS, OPTS, INLINE):
    out = os.path.join(tmp, "t.o")
    if os.path.exists(out): os.remove(out)
    r = subprocess.run([os.path.abspath(f"build/compilers/{cc}/mwcceppc.exe"), *base, *opt, *inl, "-c", src, "-o", out],
                       capture_output=True, text=True)
    if r.returncode:
        results.append((cc, opt, inl, "compile error: " + (r.stdout + r.stderr).strip().splitlines()[-1][:80])); continue
    b, secs, syms, rel = read_elf(out)
    ti = next(i for i, s in enumerate(secs) if s["name"] == ".text")
    text = b[secs[ti]["off"]:secs[ti]["off"] + secs[ti]["size"]]
    rmap = {o & ~3: (t, si, a) for o, t, si, a in rel[ti]}
    res = []
    for fn, addr in wants:
        f = next((s for s in syms if s["shn"] == ti and s["type"] == 2 and (s["name"] == fn or s["name"].startswith(fn + "__"))), None)
        if f is None: res.append(f"{fn}:missing"); continue
        ok = addr in fn_hits(words, by_word, text, rmap, f)
        if ok:
            res.append(f"{fn}:MATCH"); continue
        n = f["size"] // 4
        bad = 0
        for k in range(max(n, tfuncs.get(addr, 0) // 4)):
            o = f["val"] + 4 * k
            if k >= n or o + 4 > len(text): bad += 1; continue
            w = int.from_bytes(text[o:o + 4], "big")
            m = {10: 0xFC000003, 11: 0xFFFF0003, 109: 0xFFE00000}.get(rmap[o][0], 0xFFFF0000) if o in rmap else 0xFFFFFFFF
            if (words.get(addr + 4 * k, -1) & m) != (w & m): bad += 1
        res.append(f"{fn}:{bad}diff(size {f['size']:#x}/{tfuncs.get(addr, 0):#x})")
    results.append((cc, opt, inl, "  ".join(res)))
only = os.environ.get("BEST")
if only:
    def score(r):
        import re
        return sum(0 if "MATCH" in x else int(re.match(r".*?:(\d+)diff", x).group(1)) if "diff" in x else 999 for x in r[3].split("  "))
    results.sort(key=score); results = results[:int(only)]
for cc, opt, inl, r in results:
    print(f"{cc:11} {' '.join(opt):6} {' '.join(inl):13} {r}")
