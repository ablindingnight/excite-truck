# Shared helpers for the refmatch tools: target DOL loading, minimal ELF reading, function matching.
import re, struct
from collections import defaultdict

MASK = {10: 0xFC000003, 11: 0xFFFF0003, 109: 0xFFE00000}


def load_target(dol, syms_path):
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
    tfuncs, tnames = {}, {}
    for l in open(syms_path):
        m = re.match(r"(\S+) = \.text:0x([0-9A-F]+); // type:function size:0x([0-9A-F]+)", l)
        if m:
            a = int(m.group(2), 16)
            tfuncs[a] = int(m.group(3), 16)
            tnames[a] = m.group(1)
    return words, by_word, tfuncs, tnames


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


def fn_hits(words, by_word, text, rmap, f):
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
            out.append(start)
    return out
