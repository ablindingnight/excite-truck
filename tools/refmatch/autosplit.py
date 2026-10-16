# Propose dtk splits + symbol names for reference objects whose functions all match the target DOL.
# Usage: python autosplit.py <main.dol> <symbols.txt> <obj_root> <unit-prefix-to-strip> <unit>... 
import json, re, struct, sys
from collections import defaultdict
sys.path.insert(0, __import__("os").path.dirname(__file__))
R13, R2 = 0x80560900, 0x80568100
dol, syms_path, obj_root, strip = sys.argv[1:5]
units = sys.argv[5:]

d = open(dol, "rb").read()
offs = struct.unpack(">18I", d[0:72]); addrs = struct.unpack(">18I", d[72:144]); sizes = struct.unpack(">18I", d[144:216])
words = {}
for i in range(7):
    for k in range(0, sizes[i], 4):
        words[addrs[i] + k] = struct.unpack(">I", d[offs[i] + k:offs[i] + k + 4])[0]
by_word = defaultdict(list)
for a, w in words.items(): by_word[w].append(a)

def read_elf(p):
    b = open(p, "rb").read()
    shoff, = struct.unpack(">I", b[0x20:0x24]); shentsize, shnum, shstrndx = struct.unpack(">HHH", b[0x2E:0x34])
    secs = []
    for i in range(shnum):
        nm, typ, flg, addr, off, size, link, info, align, entsz = struct.unpack(">10I", b[shoff + i * shentsize: shoff + i * shentsize + 40])
        secs.append(dict(nm=nm, typ=typ, off=off, size=size, link=link, info=info, align=align))
    so = secs[shstrndx]["off"]
    for s in secs: s["name"] = b[so + s["nm"]:b.index(b"\0", so + s["nm"])].decode()
    st = next(s for s in secs if s["typ"] == 2); strs = secs[st["link"]]["off"]
    syms = []
    for k in range(0, st["size"], 16):
        nm, val, sz, info, oth, shn = struct.unpack(">IIIBBH", b[st["off"] + k:st["off"] + k + 16])
        syms.append(dict(name=b[strs + nm:b.index(b"\0", strs + nm)].decode(errors="replace"), val=val, size=sz, type=info & 0xF, bind=info >> 4, shn=shn))
    rel = defaultdict(list)
    for s in secs:
        if s["typ"] == 4:
            for k in range(0, s["size"], 12):
                off, info, add = struct.unpack(">IIi", b[s["off"] + k:s["off"] + k + 12])
                rel[s["info"]].append((off, info & 0xFF, info >> 8, add))
    return b, secs, syms, rel

def sext(v): return v - 0x10000 if v & 0x8000 else v
out_splits, out_names, report = [], {}, []
for unit in units:
    b, secs, syms, rel = read_elf(f"{obj_root}/{unit}.o")
    ti = next((i for i, s in enumerate(secs) if s["name"] == ".text" and s["size"]), None)
    if ti is None:
        report.append(f"{unit}: no .text; skipping"); continue
    text = b[secs[ti]["off"]:secs[ti]["off"] + secs[ti]["size"]]
    rmap = {o & ~3: (t, si, a) for o, t, si, a in rel[ti]}
    funcs = sorted([s for s in syms if s["shn"] == ti and s["type"] == 2], key=lambda s: s["val"])
    # place .text by anchoring on the first uniquely matching function
    def masks(lo, n):
        ms = []
        for k in range(n):
            o = lo + 4 * k
            if o in rmap:
                t = rmap[o][0]
                ms.append({10: 0xFC000003, 11: 0xFFFF0003, 109: 0xFFE00000}.get(t, 0xFFFF0000))
            else: ms.append(0xFFFFFFFF)
        return ms
    n = len(text) // 4
    ws = [struct.unpack(">I", text[4 * k:4 * k + 4])[0] for k in range(n)]
    ms = masks(0, n)
    anchor = next((k for k in range(n) if ms[k] == 0xFFFFFFFF), None)
    if anchor is None:
        report.append(f"{unit}: no anchor; skipping"); continue
    starts = [a - 4 * anchor for a in by_word[ws[anchor]] if all((words.get(a - 4 * anchor + 4 * k, -1) & ms[k]) == (ws[k] & ms[k]) for k in range(n))]
    if len(starts) != 1:
        report.append(f"{unit}: whole .text matched at {len(starts)} places; skipping"); continue
    tbase = starts[0]
    split = {".text": (tbase, tbase + len(text))}
    for f in funcs:
        if f["size"]: out_names[tbase + f["val"]] = (f["name"], ".text", f["size"], "function", f["bind"])
    # data: resolve each data-symbol reference to a target address
    secaddr = {}
    for o, (t, si, add) in sorted(rmap.items()):
        sym = syms[si]; sec = secs[sym["shn"]]["name"] if 0 < sym["shn"] < len(secs) else None
        if sec in (None, ".text") or sym["shn"] == 0: continue
        w = words[tbase + o]
        if t == 109:
            ra = (w >> 16) & 0x1F
            tgt = (R13 if ra == 13 else R2 if ra == 2 else 0) + sext(w & 0xFFFF)
        elif t == 6:
            lo = next(((oo, tt) for oo, (tt, ss, aa) in rmap.items() if ss == si and aa == add and tt == 4 and oo > o), None)
            if not lo: continue
            tgt = ((w & 0xFFFF) << 16) + sext(words[tbase + lo[0]] & 0xFFFF)
        else: continue
        base = tgt - add - sym["val"] if sym["type"] != 3 else tgt - add
        if sym["type"] == 3: base = tgt - add  # section symbol: addend is offset in section
        secaddr.setdefault(sec, set()).add(base)
    ok = True
    for sec, bases in secaddr.items():
        if len(bases) != 1:
            report.append(f"{unit}: {sec} resolves to {[hex(x) for x in bases]}"); ok = False; continue
        si = next(i for i, s in enumerate(secs) if s["name"] == sec)
        base = bases.pop(); split[sec] = (base, base + secs[si]["size"])
        for s in syms:
            if s["shn"] == si and s["type"] == 1 and s["size"]:
                out_names[base + s["val"]] = (s["name"], sec, s["size"], "object", s["bind"])
    # sections we have but couldn't place (e.g. .ctors/.dtors, unreferenced data)
    for s in secs:
        if s["typ"] in (1, 8) and s["size"] and s["name"] not in split and s["name"] not in (".comment",) and not s["name"].startswith((".debug", ".line", ".mwcats", "extab")):
            report.append(f"{unit}: unplaced section {s['name']} size {s['size']:#x}")
    out_splits.append((unit, split))
order = [".text", ".ctors", ".dtors", ".rodata", ".data", ".bss", ".sdata", ".sbss", ".sdata2", ".sbss2"]
for unit, sp in sorted(out_splits, key=lambda x: x[1][".text"][0]):
    print(f"{unit[len(strip):] if unit.startswith(strip) else unit}.c:")
    for sec in sorted(sp, key=order.index):
        print(f"\t{sec:<11} start:{sp[sec][0]:#010X} end:{sp[sec][1]:#010X}".replace("0X", "0x"))
    print()
json.dump({f"{k:08X}": v for k, v in out_names.items()}, open(__import__("os").path.join(__import__("os").path.dirname(__file__), "names.json"), "w"))
for r in report: print("#", r, file=sys.stderr)
