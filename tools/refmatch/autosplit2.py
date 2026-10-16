# Like autosplit.py, but tolerant of linker dead-stripping: the target .text range must be exactly the
# reference object's functions in order, where any reference function may be absent from the target.
# Usage: python autosplit2.py <main.dol> <symbols.txt> <obj_root> <strip-prefix> <unit>...
# Writes splits to stdout, names to names.json next to this script, problems to stderr.
import json, os, re, struct, sys
from collections import defaultdict

R13, R2 = 0x80560900, 0x80568100
dol, syms_path, obj_root, strip = sys.argv[1:5]
units = sys.argv[5:]

d = open(dol, "rb").read()
offs = struct.unpack(">18I", d[0:72])
addrs = struct.unpack(">18I", d[72:144])
sizes = struct.unpack(">18I", d[144:216])
words = {}
for i in range(7):
    for k in range(0, sizes[i], 4):
        words[addrs[i] + k] = struct.unpack(">I", d[offs[i] + k:offs[i] + k + 4])[0]
by_word = defaultdict(list)
for a, w in words.items():
    by_word[w].append(a)


def tdata(addr, n):
    # bytes of the target image at addr (any initialized DOL section), or None
    for i in range(18):
        if sizes[i] and addrs[i] <= addr and addr + n <= addrs[i] + sizes[i]:
            return d[offs[i] + addr - addrs[i]:offs[i] + addr - addrs[i] + n]
    return None
tfuncs = {}
for l in open(syms_path):
    m = re.match(r"\S+ = \.text:0x([0-9A-F]+); // type:function size:0x([0-9A-F]+)", l)
    if m:
        tfuncs[int(m.group(1), 16)] = int(m.group(2), 16)


def read_elf(p):
    b = open(p, "rb").read()
    shoff, = struct.unpack(">I", b[0x20:0x24])
    es, n, si = struct.unpack(">HHH", b[0x2E:0x34])
    keys = "nm typ flg addr off size link info align entsz".split()
    secs = [dict(zip(keys, struct.unpack(">10I", b[shoff + i * es:shoff + i * es + 40]))) for i in range(n)]
    so = secs[si]["off"]
    for s in secs:
        s["name"] = b[so + s["nm"]:b.index(b"\0", so + s["nm"])].decode()
    st = next(s for s in secs if s["typ"] == 2)
    strs = secs[st["link"]]["off"]
    syms = []
    for k in range(0, st["size"], 16):
        nm, val, sz, info, oth, shn = struct.unpack(">IIIBBH", b[st["off"] + k:st["off"] + k + 16])
        syms.append(dict(name=b[strs + nm:b.index(b"\0", strs + nm)].decode(errors="replace"),
                         val=val, size=sz, type=info & 0xF, bind=info >> 4, shn=shn))
    rel = defaultdict(list)
    for s in secs:
        if s["typ"] == 4:
            for k in range(0, s["size"], 12):
                off, info, add = struct.unpack(">IIi", b[s["off"] + k:s["off"] + k + 12])
                rel[s["info"]].append((off, info & 0xFF, info >> 8, add))
    return b, secs, syms, rel


def sext(v):
    return v - 0x10000 if v & 0x8000 else v


MASK = {10: 0xFC000003, 11: 0xFFFF0003, 109: 0xFFE00000}


_starts = sorted(tfuncs)


def inside_known(a):
    # True if a lies strictly inside a known target function (so it cannot be a function start)
    import bisect
    i = bisect.bisect_right(_starts, a) - 1
    return i >= 0 and _starts[i] < a < _starts[i] + tfuncs[_starts[i]]


def fn_hits(text, rmap, f):
    n = f["size"] // 4
    ws = [struct.unpack(">I", text[f["val"] + 4 * k:f["val"] + 4 * k + 4])[0] for k in range(n)]
    ms = [MASK.get(rmap[f["val"] + 4 * k][0], 0xFFFF0000) if f["val"] + 4 * k in rmap else 0xFFFFFFFF
          for k in range(n)]
    anchor = next((k for k in range(n) if ms[k] == 0xFFFFFFFF), None)
    if anchor is None:
        return []
    out = []
    for a in by_word.get(ws[anchor], []):
        start = a - 4 * anchor
        if all((words.get(start + 4 * k, -1) & ms[k]) == (ws[k] & ms[k]) for k in range(n)):
            if not inside_known(start):
                out.append(start)
    return out


out_splits, out_names, report = [], {}, []
for unit in units:
    try:
        b, secs, syms, rel = read_elf(f"{obj_root}/{unit}.o")
    except Exception as e:
        report.append(f"{unit}: read error {e}")
        continue
    ti = next((i for i, s in enumerate(secs) if s["name"] == ".text" and s["size"]), None)
    if ti is None:
        continue
    text = b[secs[ti]["off"]:secs[ti]["off"] + secs[ti]["size"]]
    rmap = {o & ~3: (t, si, a) for o, t, si, a in rel[ti]}
    funcs = sorted([s for s in syms if s["shn"] == ti and s["type"] == 2 and s["size"]], key=lambda s: s["val"])
    hits = [set(fn_hits(text, rmap, f)) for f in funcs]
    uniq = [(f["size"], i) for i, f in enumerate(funcs) if len(hits[i]) == 1]
    if not uniq:
        report.append(f"{unit}: no unique function")
        continue
    _, ai = max(uniq)
    placed = {ai: next(iter(hits[ai]))}
    # grow a contiguous cluster from the anchor: any unplaced reference function whose match sits
    # directly before or after the cluster joins it (the compiler may emit functions in a different
    # order than the target, e.g. with -ipa file); unmatched reference functions are dead-stripped
    lo_c, hi_c = placed[ai], placed[ai] + funcs[ai]["size"]
    grew = True
    while grew:
        grew = False
        for j in range(len(funcs)):
            if j in placed:
                continue
            if hi_c in hits[j]:
                placed[j] = hi_c
                hi_c += funcs[j]["size"]
                grew = True
            elif lo_c - funcs[j]["size"] in hits[j]:
                placed[j] = lo_c - funcs[j]["size"]
                lo_c = placed[j]
                grew = True
    lo = min(placed.values())
    hi = max(placed[i] + funcs[i]["size"] for i in placed)
    pset = set(placed.values())
    stray = [a for a in sorted(tfuncs) if lo <= a < hi and a not in pset]
    gaps, prev = [], lo
    for i in sorted(placed, key=lambda i: placed[i]):
        if placed[i] != prev:
            gaps.append((prev, placed[i]))
        prev = placed[i] + funcs[i]["size"]
    if stray or gaps:
        report.append(f"{unit}: {len(placed)}/{len(funcs)} placed, stray {[hex(a) for a in stray[:5]]}, "
                      f"gaps {[(hex(x), hex(y)) for x, y in gaps[:3]]}")
        continue
    # absorb zero padding before the next function
    nxt = min((a for a in tfuncs if a >= hi), default=hi)
    if nxt - hi < 0x20 and all(words.get(x, 0) == 0 for x in range(hi, nxt, 4)):
        hi = nxt
    split = {".text": (lo, hi)}
    for i in placed:
        f = funcs[i]
        out_names[placed[i]] = (f["name"], ".text", f["size"], "function", f["bind"])
    secaddr = defaultdict(set)
    for i in placed:
        f = funcs[i]
        base = placed[i] - f["val"]
        for o in range(f["val"], f["val"] + f["size"], 4):
            if o not in rmap:
                continue
            t, si, add = rmap[o]
            sym = syms[si]
            if not (0 < sym["shn"] < len(secs)) or sym["shn"] == ti:
                continue
            sec = secs[sym["shn"]]["name"]
            w = words[base + o]
            if t == 109:
                ra = (w >> 16) & 0x1F
                tgt = (R13 if ra == 13 else R2 if ra == 2 else 0) + sext(w & 0xFFFF)
            elif t == 6:
                lo_o = next((oo for oo, (tt, ss, aa) in rmap.items() if ss == si and aa == add and tt == 4 and oo > o), None)
                if lo_o is None:
                    continue
                tgt = ((w & 0xFFFF) << 16) + sext(words[base + lo_o] & 0xFFFF)
            else:
                continue
            if sym["type"] == 3 or sym["size"] == 0:
                # section/marker symbol + addend: attribute to the sized object containing that offset
                off_in = (0 if sym["type"] == 3 else sym["val"]) + add
                k = next((k for k, s in enumerate(syms) if s["shn"] == sym["shn"] and s["type"] == 1 and s["size"]
                          and s["val"] <= off_in < s["val"] + s["size"]), None)
                if k is None:
                    continue
                a = tgt - (off_in - syms[k]["val"])
            else:
                k, a = si, tgt - add
            secaddr[k].add(a)
    bad = [syms[k]["name"] for k, v in secaddr.items() if len(v) != 1]
    if bad:
        report.append(f"{unit}: conflicting addresses for {bad[:4]}")
        continue
    symaddr = {k: next(iter(v)) for k, v in secaddr.items()}
    # .ctors/.dtors: find the entry pointing at our placed function
    fnaddr = {funcs[i]["name"]: placed[i] for i in placed}
    for shn, s in enumerate(secs):
        if s["name"] in (".ctors", ".dtors") and s["size"]:
            for off, t, rsi, add in rel[shn]:
                tgt = fnaddr.get(syms[rsi]["name"])
                if tgt is None:
                    continue
                lo_c, hi_c = (0x802ED060, 0x802ED640) if s["name"] == ".ctors" else (0x802ED640, 0x802ED680)
                where = [x for x in range(lo_c, hi_c, 4) if tdata(x, 4) == struct.pack(">I", tgt)]
                if len(where) == 1:
                    split[s["name"]] = (where[0] - off, where[0] - off + s["size"])
    # grow placements to unreferenced neighbours in initialized sections by comparing bytes
    for shn, s in enumerate(secs):
        if s["typ"] != 1 or s["name"] in (".text", ".ctors", ".dtors") or not s["size"] or s["name"].startswith((".debug", ".line", ".mwcats", ".comment", "extab")):
            continue
        objs = sorted([k for k, y in enumerate(syms) if y["shn"] == shn and y["type"] == 1 and y["size"]], key=lambda k: syms[k]["val"])
        relmask = {o & ~3 for o, *_ in rel[shn]}
        sb = b[s["off"]:s["off"] + s["size"]]

        def fits(k, at):
            y = syms[k]
            got = tdata(at, y["size"])
            if got is None:
                return False
            want = sb[y["val"]:y["val"] + y["size"]]
            checked = 0
            for o in range(0, y["size"], 4):
                if (y["val"] + o) & ~3 in relmask:
                    continue
                if got[o:o + 4] != want[o:o + 4]:
                    return False
                if any(want[o:o + 4]):
                    checked += 1
            # require real evidence: some non-zero, non-relocated bytes must agree
            return checked >= 1 and checked * 2 >= (y["size"] + 3) // 4

        changed = True
        while changed:
            changed = False
            for idx, k in enumerate(objs):
                if k in symaddr:
                    continue
                prev = next((objs[j] for j in range(idx - 1, -1, -1) if objs[j] in symaddr), None)
                nxt = next((objs[j] for j in range(idx + 1, len(objs)) if objs[j] in symaddr), None)
                cands = []
                if prev is not None:
                    cur = symaddr[prev] + syms[prev]["size"]
                    cands += [(cur + al - 1) & ~(al - 1) for al in (1, 2, 4, 8, 16, 32)]
                if nxt is not None:
                    end = symaddr[nxt]
                    cands += [(end - syms[k]["size"]) & ~(al - 1) for al in (1, 2, 4, 8, 16, 32)]
                for c in dict.fromkeys(cands):
                    if (prev is None or c >= symaddr[prev] + syms[prev]["size"]) and \
                       (nxt is None or c + syms[k]["size"] <= symaddr[nxt]) and fits(k, c):
                        symaddr[k] = c
                        changed = True
                        break
    # order sanity + split ranges per section
    for shn, s in enumerate(secs):
        ks = sorted([k for k in symaddr if syms[k]["shn"] == shn], key=lambda k: syms[k]["val"])
        if not ks:
            continue
        ads = [symaddr[k] for k in ks]
        if ads != sorted(ads):
            bad.append(s["name"] + ": " + ", ".join(f"{syms[k]['name']}@{symaddr[k]:#x}" for k in ks))
            continue
        lo_d = ads[0]
        hi_d = max(symaddr[k] + syms[k]["size"] for k in ks)
        split[s["name"]] = (lo_d, (hi_d + 3) & ~3)
        for k in ks:
            y = syms[k]
            out_names[symaddr[k]] = (y["name"], s["name"], y["size"], "object", y["bind"])
    if bad:
        report.append(f"{unit}: data order differs in {bad}")
        continue
    stripped = len(funcs) - len(placed)
    report.append(f"{unit}: OK" + (f", {stripped} dead-stripped function(s)" if stripped else ""))
    out_splits.append((unit, split))

order = [".text", ".ctors", ".dtors", ".rodata", ".data", ".bss", ".sdata", ".sbss", ".sdata2", ".sbss2"]
for unit, sp in sorted(out_splits, key=lambda x: x[1][".text"][0]):
    name = unit[len(strip):] if unit.startswith(strip) else unit
    print(f"{name}.c:")
    for sec in sorted(sp, key=lambda s: order.index(s) if s in order else 99):
        print(f"\t{sec:<11} start:0x{sp[sec][0]:08X} end:0x{sp[sec][1]:08X}")
    print()
json.dump({f"{k:08X}": v for k, v in out_names.items()},
          open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "names.json"), "w"))
for r in report:
    print("#", r, file=sys.stderr)
