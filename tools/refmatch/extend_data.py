# Grow split data/bss ranges whose compiled (linked) object section is larger than the split.
# Tries growing forward (start fixed) and backward (end fixed, e.g. a version banner string that
# precedes the rest of the data). Initialized sections are only grown when the compiled bytes
# match the DOL there (relocated words masked); .bss/.sbss are grown forward when unclaimed.
# Never crosses another split or ends inside a symbol. Prints what it would change.
# Usage: python extend_data.py [--apply]
import os, re, struct, sys
SECS = (".rodata", ".data", ".bss", ".sdata", ".sbss", ".sdata2", ".sbss2")
sp_path = "config/REXE01/splits.txt"
sp = open(sp_path).read()
cfg = open("configure.py").read()

dol = open("orig/REXE01/sys/main.dol", "rb").read()
D_OFF = struct.unpack(">18I", dol[0:72]); D_ADDR = struct.unpack(">18I", dol[72:144]); D_SIZE = struct.unpack(">18I", dol[144:216])


def dol_bytes(addr, n):
    for i in range(18):
        if D_SIZE[i] and D_ADDR[i] <= addr and addr + n <= D_ADDR[i] + D_SIZE[i]:
            return dol[D_OFF[i] + addr - D_ADDR[i]:D_OFF[i] + addr - D_ADDR[i] + n]
    return None


def obj_secs(p):
    # {section name: (size, bytes or None, set of relocated offsets)}
    b = open(p, "rb").read()
    sh, = struct.unpack(">I", b[0x20:0x24]); es, n, si = struct.unpack(">HHH", b[0x2E:0x34])
    S = [struct.unpack(">10I", b[sh + i * es:sh + i * es + 40]) for i in range(n)]; so = S[si][4]
    names = [b[so + s[0]:b.index(b"\0", so + s[0])].decode() for s in S]
    out = {}
    for i, s in enumerate(S):
        if names[i] in SECS:
            data = None if s[1] == 8 else b[s[4]:s[4] + s[5]]
            out[i] = [names[i], s[5], data, set()]
    for s in S:
        if s[1] == 4 and s[7] in out:  # SHT_RELA for a data section
            for k in range(0, s[5], 12):
                off, info, _ = struct.unpack(">IIi", b[s[4] + k:s[4] + k + 12])
                out[s[7]][3].update(range(off & ~3, (off & ~3) + 4))
    merged = {}
    for name, size, data, rel in out.values():
        if name in merged:  # rare: two sections of one name; skip byte checks
            merged[name] = (merged[name][0] + size, None, set())
        else:
            merged[name] = (size, data, rel)
    return merged


built = open("build/REXE01/main.dol", "rb").read()
B_OFF = struct.unpack(">18I", built[0:72]); B_ADDR = struct.unpack(">18I", built[72:144]); B_SIZE = struct.unpack(">18I", built[144:216])


def built_bytes(addr, n):
    for i in range(18):
        if B_SIZE[i] and B_ADDR[i] <= addr and addr + n <= B_ADDR[i] + B_SIZE[i]:
            return built[B_OFF[i] + addr - B_ADDR[i]:B_OFF[i] + addr - B_ADDR[i] + n]
    return None


# what the linker actually placed (after dead-stripping), from the map of the last link
placed = {}
mp = open("build/REXE01/main.elf.MAP", errors="replace").read()
for sec in re.findall(r"^(\S+) section layout", mp, re.M):
    body = mp.split(f"\n{sec} section layout", 1)[1].split("section layout", 1)[0]
    for size, addr, obj in re.findall(r"^\s+[0-9a-f]{8} ([0-9a-f]{6,8}) ([0-9a-f]{8}) [0-9a-f]{8}\s+\d+ " + re.escape(sec) + r" \t(\S+)", body, re.M):
        placed[(sec, obj.lower())] = (int(addr, 16), int(size, 16))


def linked(unit):
    # Matching objects (lib Object(...) or TpRevolution(...)) are the only ones the linker uses
    for name in (unit, unit.split("/", 1)[-1]):
        i = cfg.find(f'"{name}"')
        if i >= 0 and re.search(r"\bMatching\b", cfg[cfg.rfind("\n", 0, i):cfg.find("\n", i)]):
            return True
    return False


syms = sorted((int(a, 16), int(z, 16)) for a, z in
              re.findall(r"= \.\w+:0x(\w+); // type:\w+ size:0x(\w+)", open("config/REXE01/symbols.txt").read()))


def mid_symbol(x):
    return any(a < x < a + z for a, z in syms)


def bytes_match(addr, data, rel):
    t = dol_bytes(addr, len(data))
    return t is not None and all(t[k] == data[k] for k in range(len(data)) if k not in rel)


claims = [(s, int(a, 16), int(e, 16)) for s, a, e in re.findall(r"(\S+)\s+start:0x(\w+) end:0x(\w+)", sp)]
changes = []
for blk in sp.split("\n\n"):
    L = blk.strip().splitlines()
    if not L or not L[0].endswith(":") or L[0].startswith("Sections"): continue
    unit = L[0][:-1]
    if not linked(unit): continue
    o = "build/REXE01/src/" + unit.rsplit(".", 1)[0] + ".o"
    if not os.path.exists(o): continue
    have = obj_secs(o)
    for s, a, e in re.findall(r"(\S+)\s+start:0x(\w+) end:0x(\w+)", blk):
        if s not in have: continue
        a, e = int(a, 16), int(e, 16)
        pa = placed.get((s, os.path.basename(o).lower()))
        if not pa: continue
        size = pa[1]
        data = None if have[s][1] is None and s.endswith("bss") or s in (".bss", ".sbss", ".sbss2") else built_bytes(pa[0], size)
        rel = {k + j for k in range(0, size & ~3, 4) for j in range(4) if data and 0x80 <= data[k] <= 0x81}
        if a + size <= e: continue
        cands = [(a, (a + size + 3) & ~3)]
        if data is not None:
            for al in (4, 8, 16, 32):
                ns = (e - size) & ~(al - 1)
                if (ns, e) not in cands: cands.append((ns, e))
        verdict = []
        for ns, ne in cands:
            clash = [c for c in claims if c[0] == s and c[1] < ne and c[2] > ns and not (c[1] == a and c[2] == e)]
            why = ("clash " + ",".join(f"{c[1]:#x}-{c[2]:#x}" for c in clash)) if clash else \
                  "mid-symbol" if mid_symbol(ns) or mid_symbol(ne) else \
                  "bytes differ" if data is not None and not bytes_match(ns, data, rel) else "ok"
            verdict.append((ns, ne, why))
        good = [v for v in verdict if v[2] == "ok"]
        if good:
            ns, ne, _ = good[0]
            print(f"{unit:50} {s:8} {a:#x}-{e:#x} -> {ns:#x}-{ne:#x} ok")
            changes.append((unit, s, a, e, ns, ne))
        else:
            print(f"{unit:50} {s:8} {a:#x}-{e:#x} compiled {size:#x}: " + "; ".join(f"{v[0]:#x}-{v[1]:#x} {v[2]}" for v in verdict))
if "--apply" in sys.argv:
    for unit, s, a, e, ns, ne in changes:
        i = sp.index(unit + ":\n"); j = sp.find("\n\n", i); j = len(sp) if j < 0 else j
        blk = re.sub(rf"(\t{re.escape(s)}\s+)start:0x{a:08X} end:0x{e:08X}", rf"\g<1>start:0x{ns:08X} end:0x{ne:08X}", sp[i:j])
        sp = sp[:i] + blk + sp[j:]
    open(sp_path, "w", newline="\n").write(sp)
    print(f"applied {len(changes)}")
