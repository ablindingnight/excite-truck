# Reorder the top-level function definitions of a C file so that the functions the original game
# kept appear in its order (MWCC with -ipa file emits functions in definition order). Each function
# carries the comments/declarations just above it. Functions not in the list (dead-stripped in the
# game) stay immediately before the next listed function that followed them in the source.
# Lines matching --attach-prev patterns are kept with the preceding function instead.
# --protos adds forward declarations for every non-inline function (no codegen effect).
# Usage: python reorder_fns.py file.c order.txt [--attach-prev REGEX ...] [--protos]
import re, sys
path, order_path = sys.argv[1:3]
attach_prev = [sys.argv[i + 1] for i, a in enumerate(sys.argv) if a == "--attach-prev"]
order = [l.strip() for l in open(order_path) if l.strip() and not l.startswith("#")]
L = open(path, encoding="utf-8", errors="surrogateescape").read().split("\n")

SIG_START = re.compile(r"^(?!typedef\b|return\b|#|//|/\*)[A-Za-z_][\w\s\*]*?\b(\w+)\s*\(")

# function definitions: (name, first signature line, last body line, signature text)
fns = []
i = 0
while i < len(L):
    m = SIG_START.match(L[i])
    if m and ";" not in L[i]:
        # the signature may wrap over several lines: find the line with the opening brace
        k, sig = i, L[i]
        while "{" not in L[k] and ";" not in L[k] and k - i < 8 and k + 1 < len(L):
            k += 1
            sig += "\n" + L[k]
        if "{" in L[k] and ";" not in L[k] and sig.count("(") == sig.count(")") and re.search(r"\)\s*\{\s*$", sig):
            j, d = k, 0
            while True:
                d += L[j].count("{") - L[j].count("}")
                if d == 0 and "}" in L[j]:
                    break
                j += 1
            fns.append((m.group(1), i, j, sig))
            i = j + 1
            continue
    i += 1

# the header keeps everything up to the last non-comment line before the first function
k = fns[0][1]
while k > 0 and (not L[k - 1].strip() or L[k - 1].lstrip().startswith(("//", "/*", "*"))):
    k -= 1
head = L[:k]
segs = []  # [name, lines, index of signature within lines]
start = k
for name, a, b, sig in fns:
    segs.append([name, L[start:b + 1], a - start])
    start = b + 1
tail = L[start:]
# move lines that belong with the previous function (e.g. '#pragma pop')
for n in range(1, len(segs)):
    body = segs[n][1]
    moved = [l for l in body[:segs[n][2]] if any(re.search(p, l) for p in attach_prev)]
    for l in moved:
        body.remove(l)
        segs[n - 1][1].append(l)
        segs[n][2] -= 1

names = [s[0] for s in segs]
missing = [o for o in order if o not in names]
if missing:
    sys.exit(f"not defined in {path}: {missing}")
rank = {n: i for i, n in enumerate(order)}
keys, nxt = [None] * len(segs), len(order)
for idx in range(len(segs) - 1, -1, -1):
    if segs[idx][0] in rank:
        nxt = rank[segs[idx][0]]
        keys[idx] = (nxt, 1)
    else:
        keys[idx] = (nxt, 0)
new = [s for _, s in sorted(zip([(k[0], k[1], i) for i, k in enumerate(keys)], segs), key=lambda t: t[0])]
protos = []
if "--protos" in sys.argv:
    for name, a, b, sig in fns:
        if "inline" not in sig:
            protos.append(re.sub(r"\s*\{\s*$", ";", sig.rstrip()))
    if protos:
        protos = ["", "// forward declarations: functions below are ordered to match the original binary"] + protos
out = head + protos + [l for s in new for l in s[1]] + tail
open(path, "w", encoding="utf-8", errors="surrogateescape", newline="\n").write("\n".join(out))
print(f"{len(segs)} functions, {len(order)} ordered")
