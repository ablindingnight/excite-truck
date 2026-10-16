# Compile one source with a library's flags from configure.py (default: cflags_rvl_tp / GC/3.0a3).
# Usage: python tpcc.py <source path> <out.o> [cflags_var] [mw_version]
import os, re, shlex, subprocess, sys

src, out = sys.argv[1:3]
var = sys.argv[3] if len(sys.argv) > 3 else "cflags_rvl_tp"
ver = sys.argv[4] if len(sys.argv) > 4 else "GC/3.0a3"
cfg = open("configure.py").read()
ENV = {"config": type("C", (), {"version": "REXE01"})(), "version_num": 0}


def flags_of(name):
    blk = cfg[cfg.index(name + " = ["):]
    blk = blk[:blk.index("\n]")]
    out = []
    for line in blk.splitlines()[1:]:
        line = line.split("#", 1)[0].strip().rstrip(",")
        if not line:
            continue
        if line.startswith("*"):
            out += flags_of(line[1:])
        else:
            out += shlex.split(eval(line, ENV))
    # mirror configure.py: debug flags are appended to cflags_base after its definition
    if name == "cflags_base":
        out.append("-DNDEBUG=1")
    return out


flags = flags_of(var)
lang = "-lang=c++" if src.endswith((".cpp", ".cp")) else "-lang=c"
r = subprocess.run([os.path.abspath("build/tools/sjiswrap.exe"), os.path.abspath(f"build/compilers/{ver}/mwcceppc.exe"),
                    *flags, lang, "-c", src, "-o", out], capture_output=True, text=True)
msg = "\n".join(l for l in (r.stdout + r.stderr).splitlines()
                if "Usage Warning" not in l and "deprecated" not in l and "multibyte" not in l)
if msg.strip():
    print(msg)
sys.exit(r.returncode)
