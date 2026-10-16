# Import bounded ogws split candidates (unit names like revolution/GX/GXPixel.c): copy sources into
# src/, register in configure.py's RVL_SDK lib (NonMatching), append splits, apply exact names.
# Usage: python import_og.py <ogws_repo> <candidates.txt> <names.json>
import os, shutil, subprocess, sys
og, cand_p, names_p = sys.argv[1:4]
blocks = [b for b in open(cand_p).read().strip().split("\n\n") if b.strip()]
paths = [b.splitlines()[0].rstrip(":") for b in blocks]
for p in paths:
    os.makedirs(os.path.dirname(os.path.join("src", p)), exist_ok=True)
    shutil.copy(os.path.join(og, "src", p), os.path.join("src", p))
cfg = open("configure.py").read()
anchor = '            Object(Matching, "revolution/OS/OSUtf.c"),\n'
assert anchor in cfg
cfg = cfg.replace(anchor, anchor + "".join(f'            Object(NonMatching, "{p}"),\n' for p in paths if f'"{p}"' not in cfg))
open("configure.py", "w", newline="\n").write(cfg)
frag = "\n\n".join(blocks) + "\n"
open("config/REXE01/splits.txt", "a", newline="\n").write("\n" + frag)
subprocess.run([sys.executable, "-I", "tools/refmatch/apply_names2.py", "config/REXE01/symbols.txt", names_p, cand_p], check=True)
print("imported", len(paths), ":", " ".join(paths))
