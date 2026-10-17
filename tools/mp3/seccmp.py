# Compare a compiled object's data sections with the original DOL address ranges, ignoring relocated words.
# Usage: python tools/mp3/seccmp.py <obj> .rodata:0x802F3238 .data:0x8031BFA8 ...
import struct, sys
b = open(sys.argv[1], "rb").read()
shoff, = struct.unpack(">I", b[0x20:0x24]); es, n, si = struct.unpack(">HHH", b[0x2E:0x34])
secs = [dict(zip("nm typ flg addr off size link info align entsz".split(), struct.unpack(">10I", b[shoff + i * es:shoff + i * es + 40]))) for i in range(n)]
so = secs[si]["off"]
for s in secs: s["name"] = b[so + s["nm"]:b.index(b"\0", so + s["nm"])].decode()
relocated = {}
for s in secs:
    if s["typ"] == 4:  # RELA
        tgt = secs[s["info"]]["name"]
        for k in range(0, s["size"], 12):
            off, = struct.unpack(">I", b[s["off"] + k:s["off"] + k + 4])
            relocated.setdefault(tgt, set()).add(off & ~3)
d = open("orig/REXE01/sys/main.dol", "rb").read()
offs = struct.unpack(">18I", d[0:72]); addrs = struct.unpack(">18I", d[72:144]); sizes = struct.unpack(">18I", d[144:216])
def rb(a, n):
    for o, ad, sz in zip(offs, addrs, sizes):
        if sz and ad <= a and a + n <= ad + sz: return d[o + a - ad:o + a - ad + n]
for arg in sys.argv[2:]:
    name, addr = arg.split(":"); addr = int(addr, 16)
    s = next(x for x in secs if x["name"] == name)
    mine = b[s["off"]:s["off"] + s["size"]] if s["typ"] != 8 else bytes(s["size"])
    orig = rb(addr, s["size"])
    rel = relocated.get(name, set())
    bad = [i for i in range(0, s["size"], 4) if i not in rel and mine[i:i + 4] != orig[i:i + 4]]
    print("%-8s size 0x%X  relocated words %d  differing words %d %s" % (name, s["size"], len(rel), len(bad), [hex(addr + i) for i in bad[:5]]))
