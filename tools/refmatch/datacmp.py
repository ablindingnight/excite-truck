# Compare data-section symbols between a target and base object (by name, ignoring relocated words).
import struct, sys
sys.path.insert(0, __import__("os").path.dirname(__file__))
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
        syms.append((b[strs+nm:b.index(b"\0", strs+nm)].decode(errors="replace"), val, sz, info & 0xF, shn))
    out = {}
    for name, val, sz, typ, shn in syms:
        if 0 < shn < len(secs) and typ == 1 and secs[shn]["name"] not in (".text",) and secs[shn]["typ"] == 1:
            out[(secs[shn]["name"], name)] = b[secs[shn]["off"]+val:secs[shn]["off"]+val+sz]
    return out
t, m = read(sys.argv[1]), read(sys.argv[2])
for k in sorted(set(t) | set(m)):
    a, c = t.get(k), m.get(k)
    if a != c:
        print(k, "target", None if a is None else (len(a), a[:48]), "\n      ours  ", None if c is None else (len(c), c[:48]))
