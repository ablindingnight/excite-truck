# Import bounded TP split candidates: copy sources (+ private headers), register in configure.py
# (as NonMatching), append splits with the "revolution/" prefix, and apply exact symbol names.
# Usage: python import_tp.py <tp_repo> <candidates.txt> <names.json>
import os, re, shutil, subprocess, sys
tp, cand_p, names_p = sys.argv[1:4]
blocks = [b for b in open(cand_p).read().strip().split("\n\n") if b.strip()]
paths = [b.splitlines()[0].rstrip(":") for b in blocks]
for p in paths:
    src = os.path.join(tp, "libs/revolution/src", p)
    dst = os.path.join("libs/revolution/src", p)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copy(src, dst)
    for h in os.listdir(os.path.dirname(src)):
        if h.endswith(".h") and not os.path.exists(os.path.join(os.path.dirname(dst), h)):
            shutil.copy(os.path.join(os.path.dirname(src), h), os.path.dirname(dst))
cfg = open("configure.py").read()
anchor = '            TpRevolution("ipc/ipcProfile.c", Matching),\n'
assert anchor in cfg
new = "".join(f'            TpRevolution("{p}", NonMatching),\n' for p in paths if f'"{p}"' not in cfg)
cfg = cfg.replace(anchor, anchor + new)
open("configure.py", "w", newline="\n").write(cfg)
frag = "\n\n".join("revolution/" + b for b in blocks) + "\n"
open("config/REXE01/splits.txt", "a", newline="\n").write("\n" + frag)
open(cand_p + ".prefixed", "w", newline="\n").write(frag)
subprocess.run([sys.executable, "-I", "tools/refmatch/apply_names2.py", "config/REXE01/symbols.txt", names_p, cand_p + ".prefixed"], check=True)
print("imported", len(paths), ":", " ".join(paths))
