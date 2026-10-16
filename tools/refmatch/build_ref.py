# Compile reference SDK sources with a given MWCC version into an output dir.
# Usage: python build_ref.py <ref_repo> <compiler_dir> <out_dir> <subdir> [extra flags]
import os, subprocess, sys
from concurrent.futures import ThreadPoolExecutor
ref, cc, out, sub = sys.argv[1:5]
extra = sys.argv[5:]
flags = ["-nodefaults","-proc","gekko","-align","powerpc","-enum","int","-fp","hardware","-Cpp_exceptions","off",
         "-O4,p","-inline","auto","-pragma","cats off","-pragma","warn_notinlined off","-maxerrors","1","-nosyspath",
         "-RTTI","off","-fp_contract","on","-str","reuse","-i","include","-i","include/MSL","-i","include/MSL/internal",
         "-DBUILD_VERSION=0","-DVERSION_RSPE01_00","-ir","include/revolution/BTE","-DREVOLUTION","-DNDEBUG=1",
         "-enc","SJIS","-fp_contract","off","-ipa","file", *extra]
srcs = []
for d, _, fs in os.walk(os.path.join(ref, "src", sub)):
    for f in fs:
        if f.endswith((".c", ".cpp", ".cp")): srcs.append(os.path.relpath(os.path.join(d, f), ref))
def build(s):
    o = os.path.join(out, os.path.splitext(s)[0] + ".o")
    os.makedirs(os.path.dirname(o), exist_ok=True)
    lang = ["-lang", "c++"] if s.endswith((".cpp", ".cp")) else ["-lang", "c"]
    r = subprocess.run([os.path.join(cc, "mwcceppc.exe"), *flags, *lang, "-c", s, "-o", o], cwd=ref, capture_output=True, text=True)
    return s, r.returncode, (r.stdout + r.stderr)[-400:]
ok = bad = 0
with ThreadPoolExecutor(8) as ex:
    for s, rc, log in ex.map(build, srcs):
        if rc: bad += 1; print("FAIL", s, log.replace("\n", " | ")[-200:])
        else: ok += 1
print(f"built {ok}, failed {bad}")
