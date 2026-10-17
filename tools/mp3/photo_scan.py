# For each ET decoder function, look for an instruction-identical function in Photo Channel photo_4.dol,
# ignoring relocated immediates (lis/addi/sda21 loads+stores) and branch displacements.
import struct, re, sys
def load(path):
    d = open(path, 'rb').read()
    offs = struct.unpack('>18I', d[0:72]); addrs = struct.unpack('>18I', d[72:144]); sizes = struct.unpack('>18I', d[144:216])
    return d, offs, addrs, sizes
pd, po, pa, ps = load(sys.argv[1])
ed, eo, ea, es = load("orig/REXE01/sys/main.dol")
def erb(a, n):
    for o, ad, s in zip(eo, ea, es):
        if s and ad <= a and a + n <= ad + s: return ed[o + a - ad:o + a - ad + n]
MEMOPS = set(range(32, 56))
def mask(w):
    op = w >> 26; ra = (w >> 16) & 31
    if op == 18: return w & 0xFC000003
    if op == 15: return w & 0xFFFF0000
    if op == 14 and ra not in (1,): return w & 0xFFFF0000
    if op in MEMOPS and ra in (0, 2, 13) or (op in MEMOPS and ra not in (1,) and False): return w & 0xFFFF0000
    if op in MEMOPS: return w  # stack/struct offsets must match
    return w
ptext = []
for i in range(7):  # text sections
    if ps[i]: ptext.append((pa[i], [mask(x) for x in struct.unpack('>%dI' % (ps[i] // 4), pd[po[i]:po[i] + ps[i]])]))
fns = []
for l in open('config/REXE01/symbols.txt'):
    m = re.match(r'(\S+) = \.text:0x([0-9A-F]+); // type:function size:0x([0-9A-F]+)', l)
    if m:
        a = int(m.group(2), 16)
        if 0x800BC538 <= a < 0x800C6204: fns.append((a, int(m.group(3), 16), m.group(1)))
fns.sort()
found = 0
for a, n, name in fns:
    pat = [mask(x) for x in struct.unpack('>%dI' % (n // 4), erb(a, n))]
    hits = []
    for base, ws in ptext:
        first = pat[0]; L = len(pat)
        i = 0
        while True:
            try: i = ws.index(first, i)
            except ValueError: break
            if ws[i:i + L] == pat: hits.append(base + i * 4)
            i += 1
    if len(hits) == 1: found += 1
    print("%08X %-12s %4d insns -> %s" % (a, name, n // 4, ", ".join("%08X" % h for h in hits[:3]) if hits else "-"))
print("unique matches: %d/%d" % (found, len(fns)))
