# Like photo_scan.py, but tolerates load/store/addi displacements that differ by exactly +-8 (the ED70 -> ED68 shift).
import struct, re, sys
def load(path):
    d = open(path, 'rb').read()
    return d, struct.unpack('>18I', d[0:72]), struct.unpack('>18I', d[72:144]), struct.unpack('>18I', d[144:216])
pd, po, pa, ps = load(sys.argv[1]); ed, eo, ea, es = load("orig/REXE01/sys/main.dol")
def erb(a, n):
    for o, ad, s in zip(eo, ea, es):
        if s and ad <= a and a + n <= ad + s: return ed[o + a - ad:o + a - ad + n]
MEM = set(range(32, 56))
def eq(e, p):
    if e == p: return True
    op = e >> 26
    if op != p >> 26: return False
    ra = (e >> 16) & 31
    if op == 18: return True
    if op == 15: return (e >> 16) == (p >> 16)
    if (e >> 16) != (p >> 16): return False
    if op == 14 and ra != 1: return True
    if op in MEM and ra in (0, 2, 13): return True
    if op in MEM or op == 14:
        de = e & 0xFFFF; dp = p & 0xFFFF
        de -= 0x10000 if de & 0x8000 else 0; dp -= 0x10000 if dp & 0x8000 else 0
        return abs(de - dp) == 8
    return False
ptext = []
for i in range(7):
    if ps[i]: ptext.append((pa[i], struct.unpack('>%dI' % (ps[i] // 4), pd[po[i]:po[i] + ps[i]])))
fns = []
for l in open('config/REXE01/symbols.txt'):
    m = re.match(r'(\S+) = \.text:0x([0-9A-F]+); // type:function size:0x([0-9A-F]+)', l)
    if m and 0x800BC538 <= int(m.group(2), 16) < 0x800C6204:
        fns.append((int(m.group(2), 16), int(m.group(3), 16), m.group(1)))
fns.sort()
found = 0
for a, n, name in fns:
    pat = struct.unpack('>%dI' % (n // 4), erb(a, n)); L = len(pat)
    hits = []
    for base, ws in ptext:
        for i in range(len(ws) - L + 1):
            if eq(pat[0], ws[i]) and eq(pat[-1], ws[i + L - 1]) and all(eq(x, y) for x, y in zip(pat, ws[i:i + L])):
                hits.append(base + i * 4)
                if len(hits) > 3: break
    if len(hits) == 1: found += 1
    print("%08X %-12s %4d -> %s" % (a, name, L, ", ".join("%08X" % h for h in hits[:3]) or "-"))
print("unique: %d/%d" % (found, len(fns)))
