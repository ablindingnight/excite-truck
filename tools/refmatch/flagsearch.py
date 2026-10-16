# Search compiler versions x flag variations for the build that best reproduces a unit.
# For each variant: compile the unit's source, then score it against the unit's .text split in
# the original DOL: functions (and bytes) whose code matches exactly (relocations masked), the
# number of .text sections (ET usually has 1), and whether matched functions appear in the same
# order. Prints a ranked table; the best line is what to put in configure.py / docs/compiler.md.
# Usage: python flagsearch.py <unit as in splits.txt> [--cflags VAR] [--quick]
#   --cflags VAR  base flag list from configure.py (default: the unit's library cflags)
#   --quick       only the current compiler with each single flag change
import os, re, shlex, struct, subprocess, sys

unit = sys.argv[1]
quick = "--quick" in sys.argv
cfg = open("configure.py").read()

# --- locate source, base flags and compiler from configure.py --------------------------------
short = unit.split("/", 1)[1] if unit.startswith("revolution/") else unit
pos = cfg.find(f'"{unit}"')
if pos < 0: pos = cfg.find(f'"{short}"')
if pos < 0: sys.exit(f"{unit} not registered in configure.py")
lib = cfg[cfg.rfind('"lib":', 0, pos):pos]
var = re.search(r'"cflags":\s*(\w+)', lib).group(1)
if "--cflags" in sys.argv: var = sys.argv[sys.argv.index("--cflags") + 1]
base_ver = re.search(r'"mw_version":\s*"([^"]+)"', lib).group(1)
sd = re.search(r'"src_dir":\s*"([^"]+)"', lib)
src_dir = sd.group(1) if sd else "src"
line = cfg[cfg.rfind("\n", 0, pos):cfg.find("\n", pos)]
src = f"{src_dir}/{short if 'TpRevolution' in line else unit}"
mv = re.search(r'mw_version="([^"]+)"', line)
if mv: base_ver = mv.group(1)
ex = re.search(r"extra_cflags=\[([^\]]*)\]", line)
extra = shlex.split(" ".join(re.findall(r"['\"](-[^'\"]+)['\"]", ex.group(1)))) if ex else []

def flags_of(name):
    blk = cfg[cfg.index(name + " = ["):]
    blk = blk[:blk.index("\n]")]
    out = []
    for l in blk.splitlines()[1:]:
        l = l.split("#", 1)[0].strip().rstrip(",")
        if not l: continue
        out += flags_of(l[1:]) if l.startswith("*") else shlex.split(eval(l, {"config": type("C", (), {"version": "REXE01"})(), "version_num": 0}))
    if name == "cflags_base": out.append("-DNDEBUG=1")
    return out
base = flags_of(var) + extra

# --- flag editing: replace an option (and its argument) rather than appending --------------
WITH_ARG = {"-sym", "-ipa", "-inline", "-Cpp_exceptions", "-fp_contract", "-str", "-use_lmw_stmw",
            "-RTTI", "-opt", "-pool", "-enc", "-proc", "-align", "-enum", "-fp", "-pragma", "-i", "-ir"}
def without(flags, opt):
    out, it = [], iter(flags)
    for f in it:
        if f == opt and opt in WITH_ARG: next(it, None); continue
        if f == opt or f.startswith(opt + ","): continue
        out.append(f)
    return out
def variant(flags, change):
    opt = change.split()[0] if change else None
    if not change: return flags
    if change.startswith("-O"):
        return [f for f in flags if not re.match(r"-O\d", f)] + [change]
    if change == "no -sym":
        return without(flags, "-sym")
    return without(flags, opt) + shlex.split(change)

CHANGES = ["", "no -sym", "-sym on", "-ipa function", "-ipa file", "-inline deferred", "-inline off",
           "-inline noauto", "-Cpp_exceptions on", "-Cpp_exceptions off", "-fp_contract on",
           "-fp_contract off", "-O4,s", "-O3,p", "-use_lmw_stmw on", "-str reuse,pool,readonly",
           "-RTTI on"]
VERSIONS = [base_ver] if quick else ["GC/3.0a3", "GC/3.0a3.2", "GC/3.0a3.3", "GC/3.0a3.4", "GC/3.0a5", "GC/3.0a5.2"]

# --- original functions in the unit's .text range ---------------------------------------------
sp = open("config/REXE01/splits.txt").read()
blk = sp[sp.index(unit + ":\n"):]
m = re.search(r"\.text\s+start:0x(\w+) end:0x(\w+)", blk[:blk.find("\n\n")] if "\n\n" in blk else blk)
if not m: sys.exit("unit has no .text split")
lo, hi = int(m.group(1), 16), int(m.group(2), 16)
d = open("orig/REXE01/sys/main.dol", "rb").read()
O = struct.unpack(">18I", d[0:72]); A = struct.unpack(">18I", d[72:144]); Z = struct.unpack(">18I", d[144:216])
def rd(x, n):
    for i in range(18):
        if Z[i] and A[i] <= x and x + n <= A[i] + Z[i]: return d[O[i] + x - A[i]:O[i] + x - A[i] + n]
origs = sorted((int(a, 16), int(z, 16)) for a, z in
               re.findall(r"= \.text:0x(\w+); // type:function size:0x(\w+)", open("config/REXE01/symbols.txt").read())
               if lo <= int(a, 16) < hi)

def score(obj):
    b = open(obj, "rb").read()
    sh, = struct.unpack(">I", b[0x20:0x24]); es, n, si = struct.unpack(">HHH", b[0x2E:0x34])
    S = [struct.unpack(">10I", b[sh + i * es:sh + i * es + 40]) for i in range(n)]; so = S[si][4]
    names = [b[so + s[0]:b.index(b"\0", so + s[0])].decode() for s in S]
    texts = [i for i, x in enumerate(names) if x == ".text"]
    st = [i for i, s in enumerate(S) if s[1] == 2][0]
    rel = {}
    for s in S:
        if s[1] == 4 and s[7] in texts:
            rel.setdefault(s[7], set()).update(struct.unpack(">I", b[s[4] + k:s[4] + k + 4])[0] & ~3 for k in range(0, s[5], 12))
    fns = []
    for k in range(0, S[st][5], 16):
        o, v, sz, info, oth, shn = struct.unpack(">IIIBBH", b[S[st][4] + k:S[st][4] + k + 16])
        if shn in texts and (info & 15) == 2 and sz:
            t = b[S[shn][4]:S[shn][4] + S[shn][5]]
            fns.append(((texts.index(shn), v), t[v:v + sz], {r - v for r in rel.get(shn, ()) if v <= r < v + sz}))
    fns.sort(key=lambda f: f[0])
    def same(t, c, r):
        for k in range(0, len(c), 4):
            x, y = struct.unpack(">I", t[k:k + 4])[0], struct.unpack(">I", c[k:k + 4])[0]
            if k in r:
                if x >> 26 != y >> 26: return False
                if x >> 26 != 18 and (x >> 21) & 31 != (y >> 21) & 31: return False
            elif x != y: return False
        return True
    used, hits, nbytes = set(), [], 0
    for a, z in origs:
        t = rd(a, z)
        for idx, (key, c, r) in enumerate(fns):
            if idx not in used and len(c) == z and same(t, c, r):
                used.add(idx); hits.append(idx); nbytes += z; break
    in_order = hits == sorted(hits)
    return len(hits), nbytes, len(texts), in_order

os.makedirs("build/flagsearch", exist_ok=True)
obj = "build/flagsearch/x.o"  # relative: the compiler wrapper splits paths with spaces
lang = "-lang=c++" if src.endswith((".cpp", ".cp")) else "-lang=c"
rows = []
for ver in VERSIONS:
    for ch in CHANGES:
        if os.path.exists(obj): os.remove(obj)
        fl = variant(base, ch)
        r = subprocess.run([os.path.abspath("build/tools/sjiswrap.exe"), os.path.abspath(f"build/compilers/{ver}/mwcceppc.exe"),
                            *fl, lang, "-c", src, "-o", obj], capture_output=True, text=True)
        if r.returncode or not os.path.exists(obj):
            continue
        n_ok, nb, nsec, order = score(obj)
        rows.append((n_ok, nb, -nsec, order, ver, ch or "(base)"))
rows.sort(key=lambda r: (r[1], r[0], r[2], r[3]), reverse=True)
total = sum(z for _, z in origs)
print(f"{unit}: {len(origs)} original functions, {total:#x} bytes in {lo:#x}-{hi:#x}; base {var} @ {base_ver}")
print(f"{'match':>9} {'bytes':>13} {'sect':>4} {'order':>5}  compiler     change")
for n_ok, nb, ns, order, ver, ch in rows[:25]:
    print(f"{n_ok:4}/{len(origs):<4} {nb:#7x}/{total:#x} {-ns:4} {'yes' if order else 'no':>5}  {ver:12} {ch}")
