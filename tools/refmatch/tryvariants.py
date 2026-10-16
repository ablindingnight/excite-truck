# Try source variants of one function and report which compile to the target's instructions.
# The variants file holds blocks separated by lines "=== name"; each block is the complete
# replacement text for the region between MARKER_BEGIN and MARKER_END lines in the source
# ("// @@variant begin" / "// @@variant end"). The original source is restored afterwards.
# Usage: python tryvariants.py <source> <variants.txt> <target_fn> <ref_fn> [cflags_var] [mw_version]
import os, re, shutil, subprocess, sys
src, var_path, target, ref = sys.argv[1:5]
cflags = sys.argv[5] if len(sys.argv) > 5 else "cflags_rvl_tp"
ver = sys.argv[6] if len(sys.argv) > 6 else "GC/3.0a5"
base = open(src, encoding="utf-8").read()
m = re.search(r"// @@variant begin\n.*?// @@variant end\n", base, re.S)
if not m:
    sys.exit("source has no '// @@variant begin' ... '// @@variant end' region")
blocks = re.split(r"^=== (.*)$", open(var_path, encoding="utf-8").read(), flags=re.M)[1:]
tmp = os.path.abspath("build/tryvariants"); os.makedirs(tmp, exist_ok=True)
obj = os.path.join(tmp, "v.o")
try:
    for name, body in zip(blocks[0::2], blocks[1::2]):
        if os.path.exists(obj): os.remove(obj)
        open(src, "w", encoding="utf-8", newline="\n").write(
            base[:m.start()] + "// @@variant begin\n" + body.strip("\n") + "\n// @@variant end\n" + base[m.end():])
        r = subprocess.run([sys.executable, "-I", "tools/refmatch/tpcc.py", src, obj, cflags, ver], capture_output=True, text=True)
        if r.returncode or not os.path.exists(obj):
            print(f"{name}: compile error {r.stdout.strip()[:200]}"); continue
        r = subprocess.run([shutil.which("bash") or "bash", "tools/refmatch/fndiff.sh", target, obj.replace(os.sep, "/"), ref], capture_output=True, text=True,
                           env={**os.environ, "FNDIFF_TMP": "build/tryvariants"})
        lines = r.stdout.strip().splitlines()
        if os.environ.get("TV_DEBUG"): print(r.stdout, r.stderr)
        print(f"{name}: {'MATCH' if len(lines) == 1 else f'{len(lines) - 1} diff lines'}")
finally:
    open(src, "w", encoding="utf-8", newline="\n").write(base)
