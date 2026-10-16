# Compile one source with a library's flags from configure.py (default: cflags_rvl_tp / GC/3.0a3).
# Usage: python tpcc.py <source path> <out.o> [cflags_var] [mw_version]
import re, shlex, subprocess, sys
src, out = sys.argv[1:3]
var = sys.argv[3] if len(sys.argv) > 3 else "cflags_rvl_tp"
ver = sys.argv[4] if len(sys.argv) > 4 else "GC/3.0a3"
cfg = open("configure.py").read()
blk = cfg[cfg.index(var + " = ["):]
blk = blk[:blk.index("\n]")]
flags = []
for item in re.findall(r'^\s*(".*?"|\'.*?\'),?\s*(?:#.*)?$', blk, re.M):
    flags += shlex.split(eval(item))
lang = "-lang=c++" if src.endswith((".cpp", ".cp")) else "-lang=c"
import os
r = subprocess.run([os.path.abspath("build/tools/sjiswrap.exe"), os.path.abspath(f"build/compilers/{ver}/mwcceppc.exe"), *flags, lang, "-c", src, "-o", out],
                   capture_output=True, text=True)
msg = "\n".join(l for l in (r.stdout + r.stderr).splitlines() if "Usage Warning" not in l and "deprecated" not in l and "multibyte" not in l)
if msg.strip(): print(msg)
sys.exit(r.returncode)
