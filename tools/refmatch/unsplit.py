# Remove units' split blocks from splits.txt and their entries from configure.py.
# Usage: python unsplit.py unit...
import re, sys
p = "config/REXE01/splits.txt"; s = open(p).read()
for u in sys.argv[1:]:
    s = re.sub(r"\n*^" + re.escape(u) + r":\n(?:\t[^\n]*\n?)*", "\n", s, flags=re.M)
open(p, "w", newline="\n").write(re.sub(r"\n{3,}", "\n\n", s).rstrip("\n") + "\n")
p = "configure.py"; c = open(p).read()
for u in sys.argv[1:]:
    c = re.sub(r'^\s*Object\((?:Non)?Matching, "' + re.escape(u) + r'"[^\n]*\n', "", c, flags=re.M)
open(p, "w", newline="\n").write(c)
print("removed", sys.argv[1:])
