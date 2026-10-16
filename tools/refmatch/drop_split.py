# Remove units' blocks from splits.txt only (keep their configure.py entries), e.g. to re-derive.
# Usage: python drop_split.py unit...
import re, sys
p = "config/REXE01/splits.txt"; s = open(p).read()
for u in sys.argv[1:]:
    s2 = re.sub(r"\n*^" + re.escape(u) + r":\n(?:\t[^\n]*\n?)*", "\n", s, flags=re.M)
    print(("dropped " if s2 != s else "no split for ") + u); s = s2
open(p, "w", newline="\n").write(s)
