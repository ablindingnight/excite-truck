# Find functions from reference object files inside a target DOL's .text.
# Relocated instruction fields are masked so link-time addresses don't matter.
# Usage: python sigmatch.py <main.dol> <symbols.txt> <obj_dir> <out.json>
import json, os, re, struct, sys
from collections import defaultdict

dol_path, syms_path, obj_dir, out_path = sys.argv[1:5]

# --- target text ---
d = open(dol_path, "rb").read()
offs = struct.unpack(">18I", d[0:72]); addrs = struct.unpack(">18I", d[72:144]); sizes = struct.unpack(">18I", d[144:216])
text = bytearray(); base = None
# .init is section 0, .text is section 1 (text sections 0-6)
words = {}  # addr -> word
for i in range(7):
    if sizes[i]:
        for k in range(0, sizes[i], 4):
            words[addrs[i] + k] = struct.unpack(">I", d[offs[i] + k:offs[i] + k + 4])[0]
by_word = defaultdict(list)
for a, w in words.items(): by_word[w].append(a)

# --- target function starts (from dtk analysis) ---
tfuncs = {}
for l in open(syms_path):
    m = re.match(r"(\S+) = \.(?:text|init):0x([0-9A-F]+); // type:function size:0x([0-9A-F]+)", l)
    if m: tfuncs[int(m.group(2), 16)] = (m.group(1), int(m.group(3), 16))

# --- minimal ELF reader ---
def read_elf(p):
    b = open(p, "rb").read()
    shoff, = struct.unpack(">I", b[0x20:0x24]); shentsize, shnum, shstrndx = struct.unpack(">HHH", b[0x2E:0x34])
    secs = []
    for i in range(shnum):
        nm, typ, flg, addr, off, size, link, info, align, entsz = struct.unpack(">10I", b[shoff + i * shentsize: shoff + i * shentsize + 40])
        secs.append(dict(nm=nm, typ=typ, off=off, size=size, link=link, info=info, entsz=entsz))
    shstr = secs[shstrndx]
    for s in secs:
        e = b.index(b"\0", shstr["off"] + s["nm"]); s["name"] = b[shstr["off"] + s["nm"]:e].decode()
    symtab = next((s for s in secs if s["typ"] == 2), None)
    syms = []
    if symtab:
        strs = secs[symtab["link"]]
        for k in range(0, symtab["size"], 16):
            nm, val, sz, info, oth, shn = struct.unpack(">IIIBBH", b[symtab["off"] + k:symtab["off"] + k + 16])
            e = b.index(b"\0", strs["off"] + nm)
            syms.append(dict(name=b[strs["off"] + nm:e].decode(errors="replace"), val=val, size=sz, type=info & 0xF, bind=info >> 4, shn=shn))
    relocs = defaultdict(list)  # target section index -> [(offset, type, symname)]
    for s in secs:
        if s["typ"] == 4:
            for k in range(0, s["size"], 12):
                off, info, add = struct.unpack(">IIi", b[s["off"] + k:s["off"] + k + 12])
                relocs[s["info"]].append((off, info & 0xFF, syms[info >> 8]["name"] if syms else "?"))
    return b, secs, syms, relocs

MASK = {10: 0x03FFFFFC, 11: 0x0000FFFC, 1: 0, 4: 0xFFFF0000, 5: 0xFFFF0000, 6: 0xFFFF0000, 109: 0xFFE00000, 26: 0}
results = []
for root, _, fs in os.walk(obj_dir):
    for f in fs:
        if not f.endswith(".o"): continue
        p = os.path.join(root, f)
        try: b, secs, syms, relocs = read_elf(p)
        except Exception as e: continue
        unit = os.path.relpath(p, obj_dir).replace("\\", "/")[:-2]
        for si, s in enumerate(secs):
            if s["name"] != ".text" or s["size"] == 0: continue
            data = b[s["off"]:s["off"] + s["size"]]
            rmap = {}
            for off, typ, name in relocs.get(si, []):
                rmap[off & ~3] = (MASK.get(typ, 0) if typ not in (4, 5, 6, 109) else (0xFFFF0000 if typ != 109 else 0xFFE00000), name, typ)
            for sym in syms:
                if sym["shn"] != si or sym["type"] != 2 or sym["size"] < 4: continue
                n = sym["size"] // 4
                ws = [struct.unpack(">I", data[sym["val"] + 4 * k: sym["val"] + 4 * k + 4])[0] for k in range(n)]
                ms = [0xFFFFFFFF if (sym["val"] + 4 * k) not in rmap else (~rmap[sym["val"] + 4 * k][0] & 0xFFFFFFFF) ^ 0xFFFFFFFF for k in range(n)]
                # ms[k] = bits that must match: for relocated words, keep only the non-field bits
                ms = [0xFFFFFFFF if (sym["val"] + 4 * k) not in rmap else (rmap[sym["val"] + 4 * k][0] ^ 0xFFFFFFFF if rmap[sym["val"] + 4*k][2] in (10, 11) else rmap[sym["val"] + 4*k][0]) for k in range(n)]
                anchor = next((k for k in range(n) if ms[k] == 0xFFFFFFFF), None)
                if anchor is None: continue
                hits = []
                for a in by_word.get(ws[anchor], []):
                    start = a - 4 * anchor
                    if all((words.get(start + 4 * k, -1) & ms[k]) == (ws[k] & ms[k]) for k in range(n)):
                        hits.append(start)
                calls = [rmap[o][1] for o in sorted(rmap) if sym["val"] <= o < sym["val"] + sym["size"]]
                results.append(dict(unit=unit, name=sym["name"], size=sym["size"], hits=hits, relocs=calls,
                                    target=[tfuncs.get(h, (None,))[0] for h in hits]))
json.dump(results, open(out_path, "w"), indent=0)
uniq = [r for r in results if len(r["hits"]) == 1]
print(f"functions: {len(results)}, unique hits: {len(uniq)}, multi: {sum(1 for r in results if len(r['hits'])>1)}, none: {sum(1 for r in results if not r['hits'])}")
print(f"unique-hit bytes: {sum(r['size'] for r in uniq):#x}")
