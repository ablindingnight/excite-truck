# For linked objects whose placed sections (from the last link's map) have no range in splits.txt,
# find those bytes in the original DOL (pointer-looking words masked) and propose split ranges.
# .bss/.sbss can't be located by content and are only reported.
# Usage: python find_missing.py [--apply]
import os, re, struct, sys
mp = open("build/REXE01/main.elf.MAP", errors="replace").read()
sp_path = "config/REXE01/splits.txt"
sp = open(sp_path).read()

def ld(p):
    d = open(p, "rb").read()
    return d, struct.unpack(">18I", d[0:72]), struct.unpack(">18I", d[72:144]), struct.unpack(">18I", d[144:216])
O, B = ld("orig/REXE01/sys/main.dol"), ld("build/REXE01/main.dol")
def section_of(D, addr):
    d, o, a, s = D
    for i in range(18):
        if s[i] and a[i] <= addr < a[i] + s[i]: return i
def rd(D, addr, n):
    d, o, a, s = D
    i = section_of(D, addr)
    return None if i is None else d[o[i] + addr - a[i]:o[i] + addr - a[i] + n]

units = {}
for blk in sp.split("\n\n"):
    L = blk.strip().splitlines()
    if L and L[0].endswith(":") and not L[0].startswith("Sections"):
        units[os.path.basename(L[0][:-1]).rsplit(".", 1)[0].lower() + ".o"] = (L[0][:-1], {l.split()[0] for l in L[1:]})
claims = [(s, int(a, 16), int(e, 16)) for s, a, e in re.findall(r"(\S+)\s+start:0x(\w+) end:0x(\w+)", sp)]

adds = []
for sec in re.findall(r"^(\S+) section layout", mp, re.M):
    if sec in (".text", ".init", "extab", "extabindex", ".ctors", ".dtors"): continue
    body = mp.split(f"\n{sec} section layout", 1)[1].split("section layout", 1)[0]
    for size, addr, obj in re.findall(r"^\s+[0-9a-f]{8} ([0-9a-f]{6,8}) ([0-9a-f]{8}) [0-9a-f]{8}\s+\d+ " + re.escape(sec) + r" \t(\S+)", body, re.M):
        size, addr = int(size, 16), int(addr, 16)
        if obj.startswith("auto_") or not size or obj.lower() not in units: continue
        unit, have = units[obj.lower()]
        if sec in have: continue
        if sec in (".bss", ".sbss", ".sbss2"):
            print(f"{unit:45} {sec:8} {size:#x}  (uninitialized: locate by hand)"); continue
        data = rd(B, addr, size)
        i = section_of(O, addr)
        d, o, a, s = O
        hay = d[o[i]:o[i] + s[i]]
        pat = b"".join(re.escape(data[k:k + 4]) if not (0x80 <= data[k] <= 0x81) else b"...." for k in range(0, size & ~3, 4)) + re.escape(data[size & ~3:])
        hits = [a[i] + m.start() for m in re.finditer(pat, hay, re.S)]
        free = [h for h in hits if not any(c[0] == sec and c[1] < h + size and h < c[2] for c in claims)]
        if len(free) == 1:
            h = free[0]; e = (h + size + 3) & ~3
            print(f"{unit:45} {sec:8} {size:#x}  -> {h:#x}-{e:#x}")
            adds.append((unit, sec, h, e))
        else:
            print(f"{unit:45} {sec:8} {size:#x}  {len(hits)} hits ({len(free)} unclaimed): {[hex(x) for x in free[:6]]}")

if "--apply" in sys.argv:
    order = [".text", ".ctors", ".dtors", "extab", "extabindex", ".rodata", ".data", ".bss", ".sdata", ".sbss", ".sdata2", ".sbss2"]
    for unit, sec, h, e in adds:
        i = sp.index(unit + ":\n"); j = sp.find("\n\n", i); j = len(sp) if j < 0 else j
        L = sp[i:j].splitlines(); cur = {l.split()[0]: l for l in L[1:]}
        cur[sec] = f"\t{sec:<11} start:0x{h:08X} end:0x{e:08X}"
        sp = sp[:i] + "\n".join([L[0]] + [cur[k] for k in order if k in cur]) + sp[j:]
    open(sp_path, "w", newline="\n").write(sp)
    print(f"applied {len(adds)}")
