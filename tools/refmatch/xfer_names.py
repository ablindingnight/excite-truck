# Pair relocations between target (dtk-split) and base (compiled) objects to learn names
# for auto-named symbols (fn_/lbl_) referenced from matched code. Updates symbols.txt.
# Usage: python xfer_names.py symbols.txt target_obj_dir base_obj_dir unit...
import re, struct, sys
syms_path, tdir, bdir = sys.argv[1:4]; units = sys.argv[4:]
def read(p):
    b = open(p, "rb").read()
    shoff, = struct.unpack(">I", b[0x20:0x24]); es, n, si = struct.unpack(">HHH", b[0x2E:0x34])
    secs = [dict(zip("nm typ flg addr off size link info align entsz".split(), struct.unpack(">10I", b[shoff+i*es:shoff+i*es+40]))) for i in range(n)]
    so = secs[si]["off"]
    for s in secs: s["name"] = b[so+s["nm"]:b.index(b"\0", so+s["nm"])].decode()
    st = next(s for s in secs if s["typ"] == 2); strs = secs[st["link"]]["off"]
    syms = []
    for k in range(0, st["size"], 16):
        nm, val, sz, info, oth, shn = struct.unpack(">IIIBBH", b[st["off"]+k:st["off"]+k+16])
        syms.append(b[strs+nm:b.index(b"\0", strs+nm)].decode(errors="replace"))
    # key relocations by (containing function, offset within it) so dead-stripped
    # functions in either object don't shift the pairing
    fsyms = []
    for k in range(0, st["size"], 16):
        nm, val, sz, info, oth, shn = struct.unpack(">IIIBBH", b[st["off"]+k:st["off"]+k+16])
        if info & 0xF in (1, 2) and sz:
            fsyms.append((shn, val, sz, syms[k // 16]))
    rel = {}
    for s in secs:
        if s["typ"] == 4:
            tsec = s["info"]
            for k in range(0, s["size"], 12):
                off, info, add = struct.unpack(">IIi", b[s["off"]+k:s["off"]+k+12])
                owner = next((n for shn, v, sz, n in fsyms if shn == tsec and v <= off < v + sz), None)
                if owner is None:
                    continue
                v = next(v for shn, v, sz, n in fsyms if shn == tsec and n == owner and v <= off < v + sz)
                rel[(owner, (off - v) & ~3, info & 0xFF)] = (syms[info >> 8], add)
    return rel
mapping = {}
for u in units:
    t = read(f"{tdir}/{u}.o"); m = read(f"{bdir}/{u}.o")
    for k, (tn, ta) in t.items():
        if k in m:
            bn, ba = m[k]
            if tn != bn and re.match(r"(fn|lbl)_[0-9A-F]{8}$", tn) and not bn.startswith("@") and ta == ba:
                if mapping.get(tn, bn) != bn: print("conflict", tn, mapping[tn], bn)
                mapping[tn] = bn
lines = open(syms_path).read().splitlines()
taken = {l.split(" = ")[0] for l in lines}
n = 0
for i, l in enumerate(lines):
    name = l.split(" = ")[0]
    if name in mapping and mapping[name] not in taken:
        lines[i] = mapping[name] + l[len(name):]
        lines[i] = re.sub(r" scope:\w+", "", lines[i]).replace("; // type:", "; // type:", 1)
        lines[i] = re.sub(r"(type:\w+(?: size:0x[0-9A-F]+)?)", r"\1 scope:global", lines[i], count=1)
        taken.add(mapping[name]); n += 1
open(syms_path, "w", newline="\n").write("\n".join(lines) + "\n")
print(f"renamed {n} symbols")
for k, v in sorted(mapping.items()): print(f"  {k} -> {v}")
