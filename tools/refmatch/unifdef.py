# Resolve #if/#ifdef blocks whose condition is exactly a known macro (or its negation), leaving
# all other preprocessor blocks untouched. Usage: python unifdef.py file -DNAME -UNAME ...
import re, sys
path = sys.argv[1]
defs = {a[2:]: True for a in sys.argv[2:] if a.startswith("-D")}
defs.update({a[2:]: False for a in sys.argv[2:] if a.startswith("-U")})

def cond_value(line):
    m = re.match(r"#\s*(if|elif)\s+(!?)\s*(?:defined\s*\(?\s*)?(\w+)\s*\)?\s*(//.*)?$", line.strip())
    if m and m.group(3) in defs:
        v = defs[m.group(3)]
        return (not v) if m.group(2) else v
    m = re.match(r"#\s*(ifdef|ifndef)\s+(\w+)\s*(//.*)?$", line.strip())
    if m and m.group(2) in defs:
        v = defs[m.group(2)]
        return v if m.group(1) == "ifdef" else not v
    return None

out = []
stack = []  # entries: [known, keep_current, any_taken, parent_emitting]
def emitting():
    return all(e[1] for e in stack if e[0]) if stack else True
for line in open(path, encoding="utf-8", errors="surrogateescape").read().split("\n"):
    s = line.strip()
    if re.match(r"#\s*(if|ifdef|ifndef)\b", s):
        v = cond_value(s)
        if v is None:
            stack.append([False, True, False])
            if emitting(): out.append(line)
        else:
            stack.append([True, v, v])
        continue
    if re.match(r"#\s*elif\b", s) and stack:
        top = stack[-1]
        if top[0]:
            v = cond_value(s)
            if v is None:
                sys.exit(f"elif with unknown condition in a resolved block: {s}")
            top[1] = (not top[2]) and v; top[2] = top[2] or v
            continue
    if re.match(r"#\s*else\b", s) and stack:
        top = stack[-1]
        if top[0]:
            top[1] = not top[2]; top[2] = True
            continue
    if re.match(r"#\s*endif\b", s) and stack:
        top = stack.pop()
        if top[0]:
            continue
        if emitting(): out.append(line)
        continue
    if emitting():
        out.append(line)
open(path, "w", encoding="utf-8", errors="surrogateescape", newline="\n").write("\n".join(out))
