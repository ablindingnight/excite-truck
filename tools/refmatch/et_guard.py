# Wrap exact snippets inside one function of a source file in "#ifndef GAME_REXE01" (code Excite
# Truck's older SDK does not have), or swap them for ET-only replacements.
# Usage: python et_guard.py <file> <function signature prefix> <spec.txt>
#   spec.txt holds blocks separated by lines "=== drop" (block is removed for ET) or
#   "=== replace" followed by the old text, a line "--- with", then the ET text.
import sys

path, sig, spec = sys.argv[1], sys.argv[2], open(sys.argv[3]).read()
s = open(path).read()
i = s.index(sig)
end = s.index("\n}\n", i) + 3
body = s[i:end]
for blk in spec.split("=== ")[1:]:
    kind, text = blk.split("\n", 1)
    kind = kind.strip()
    if kind == "drop":
        assert text in body, text
        body = body.replace(text, "#ifndef GAME_REXE01\n" + text + "#endif\n", 1)
    elif kind == "replace":
        old, new = text.split("--- with\n", 1)
        assert old in body, old
        body = body.replace(old, "#ifdef GAME_REXE01\n" + new + "#else\n" + old + "#endif\n", 1)
    else:
        sys.exit("unknown block kind " + kind)
s = s[:i] + body + s[end:]
open(path, "w", newline="\n").write(s)
