# Grow a unit's .text split outward from its current range: each neighbouring original function
# (from symbols.txt) is accepted if its bytes match some not-yet-used function of the compiled
# object (relocated words masked to their opcode), in any order. Reports the full extent, the
# original's function order in compiled names, and whether that order differs from the source's
# (then reorder_fns.py is needed). With --apply, rewrites the .text range in splits.txt.
# Usage: python extend_text.py <unit, e.g. RVL_SDK/kpad/KPAD.c> [--apply] [--order-out file]
import re, struct, sys
unit = sys.argv[1]
obj = "build/REXE01/src/" + unit.rsplit(".", 1)[0] + ".o"
# TP-style units are built under revolution/...
b = open(obj, "rb").read()
sh, = struct.unpack(">I", b[0x20:0x24]); es, n, si = struct.unpack(">HHH", b[0x2E:0x34])
S = [struct.unpack(">10I", b[sh + i * es:sh + i * es + 40]) for i in range(n)]; so = S[si][4]
nm = lambda o, base: b[base + o:b.index(b"\0", base + o)].decode()
names = [nm(s[0], so) for s in S]
# C++ objects put out-of-line inline/template copies in extra .text sections: read them all
texts = [i for i, x in enumerate(names) if x == ".text"]
st = [i for i, s in enumerate(S) if s[1] == 2][0]; strs = S[S[st][6]][4]
relsec = {}
for s in S:
    if s[1] == 4 and s[7] in texts:
        relsec.setdefault(s[7], set()).update(struct.unpack(">I", b[s[4] + k:s[4] + k + 4])[0] & ~3 for k in range(0, s[5], 12))
code, fns = {}, []
for k in range(0, S[st][5], 16):
    o, v, sz, info, oth, shn = struct.unpack(">IIIBBH", b[S[st][4] + k:S[st][4] + k + 16])
    if shn in texts and (info & 15) == 2 and sz:
        name = nm(o, strs)
        sec = b[S[shn][4]:S[shn][4] + S[shn][5]]
        code[name] = (sec[v:v + sz], {r - v for r in relsec.get(shn, ()) if v <= r < v + sz})
        fns.append(((texts.index(shn), v), sz, name))
fns.sort()

d = open("orig/REXE01/sys/main.dol", "rb").read()
O = struct.unpack(">18I", d[0:72]); A = struct.unpack(">18I", d[72:144]); Z = struct.unpack(">18I", d[144:216])
def rd(x, n):
    for i in range(18):
        if Z[i] and A[i] <= x and x + n <= A[i] + Z[i]: return d[O[i] + x - A[i]:O[i] + x - A[i] + n]

def same(addr, size, fname):
    t = rd(addr, size)
    if t is None: return False
    body, rel = code[fname]
    for k in range(0, size, 4):
        x, y = t[k:k + 4], body[k:k + 4]
        if k in rel:
            xw, yw = struct.unpack(">I", x)[0], struct.unpack(">I", y)[0]
            if xw >> 26 != yw >> 26: return False
            if xw >> 26 != 18:  # not a branch: destination register and SDA base must agree
                if (xw >> 21) & 31 != (yw >> 21) & 31: return False
                if (yw >> 16) & 31 == 0 and xw >> 26 not in (15,) and (xw >> 16) & 31 not in (0, 2, 13): return False
        elif x != y:
            return False
    return True

syms = sorted((int(a, 16), int(z, 16), nme) for nme, a, z in
              re.findall(r"^(\S+) = \.text:0x(\w+); // type:function size:0x(\w+)", open("config/REXE01/symbols.txt").read(), re.M))
sp = open("config/REXE01/splits.txt").read()
blk = sp[sp.index(unit + ":\n"):]
blk = blk[:blk.find("\n\n")] if "\n\n" in blk else blk
m = re.search(r"\.text\s+start:0x(\w+) end:0x(\w+)", blk)
lo, hi = int(m.group(1), 16), int(m.group(2), 16)
claims = [(int(a, 16), int(e, 16)) for a, e in re.findall(r"\.text\s+start:0x(\w+) end:0x(\w+)", sp)]
used = {}
def match_at(addr, size):
    for v, sz, fname in fns:
        if sz == size and fname not in used.values() and same(addr, size, fname):
            return fname
# claim the current range first
for a, z, _ in syms:
    if lo <= a < hi:
        f = match_at(a, z)
        if f: used[a] = f
# grow
def free(a, z):
    return not any(ca < a + z and a < ce and not (ca == lo and ce == hi) for ca, ce in claims)
idx = {a: i for i, (a, _, _) in enumerate(syms)}
i = max(i for i, (a, _, _) in enumerate(syms) if a < hi)
while i + 1 < len(syms):
    a, z, _ = syms[i + 1]
    f = match_at(a, z) if free(a, z) else None
    if not f: break
    used[a] = f; hi = a + z; i += 1
i = min(i for i, (a, _, _) in enumerate(syms) if a >= lo)
while i > 0:
    a, z, _ = syms[i - 1]
    f = match_at(a, z) if free(a, z) else None
    if not f: break
    used[a] = f; lo = a; i -= 1
order = [used[a] for a in sorted(used)]
src_order = [f for _, _, f in fns if f in order]
print(f"{unit}: .text {lo:#x}-{hi:#x} ({len(order)} of {len(fns)} compiled functions)")
print("order matches source" if order == src_order else "ORDER DIFFERS from source -> reorder_fns.py")
unmatched = [(hex(a), z, nme) for a, z, nme in syms if lo <= a < hi and a not in used]
if unmatched: print("unmatched inside range:", unmatched)
if "--order-out" in sys.argv:
    open(sys.argv[sys.argv.index("--order-out") + 1], "w").write("\n".join(order) + "\n")
if "--apply" in sys.argv and not unmatched:
    new = re.sub(r"(\.text\s+)start:0x\w+ end:0x\w+", rf"\g<1>start:0x{lo:08X} end:0x{hi:08X}", blk, count=1)
    open("config/REXE01/splits.txt", "w", newline="\n").write(sp.replace(blk, new, 1))
    # name auto-labelled functions (fn_XXXXXXXX) after the compiled function they matched
    symp = "config/REXE01/symbols.txt"
    text_syms = open(symp).read()
    existing = set(re.findall(r"^(\S+) =", text_syms, re.M))
    renamed = 0
    for a, f in used.items():
        auto = f"fn_{a:08X}"
        if re.search(rf"^{auto} = ", text_syms, re.M) and f not in existing:
            text_syms = re.sub(rf"^{auto} = ", f"{f} = ", text_syms, flags=re.M); existing.add(f); renamed += 1
    open(symp, "w", newline="\n").write(text_syms)
    print(f"applied ({renamed} functions named)")
