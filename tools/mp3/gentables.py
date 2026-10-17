# Generate C definitions for the SDK MP3 decoder's .rodata/.data/.sdata objects from the original DOL.
# Objects are static (MWCC then addresses them section-relative, like the original), defined in an order
# where every pointer target comes first. Output: src/sdk_mp3/mp3{frame,dequant,layer3}_tables.inc
import re, struct

d = open("orig/REXE01/sys/main.dol", "rb").read()
offs = struct.unpack(">18I", d[0:72]); addrs = struct.unpack(">18I", d[72:144]); sizes = struct.unpack(">18I", d[144:216])

def rb(a, n):
    for o, ad, s in zip(offs, addrs, sizes):
        if ad <= a and a + n <= ad + s:
            return d[o + a - ad:o + a - ad + n]
    raise ValueError(hex(a))

RANGES = {"rodata": (0x802F3238, 0x802F44E8), "data": (0x8031BFA8, 0x8031D840), "sdata": (0x80559250, 0x80559708),
          "sdata2": (0x80560DA0, 0x80560DB8)}
EXTRA_SPLITS = {0x8031BFA8: "data", 0x8031BFC0: "data", 0x8031BFD0: "data", 0x8031BFE8: "data", 0x8031BFF8: "data", 0x8031C010: "data", 0x8031C020: "data", 0x8031C038: "data", 0x8031C048: "data", 0x8031C060: "data", 0x8031C070: "data", 0x8031C088: "data",
                0x8031C298: "data", 0x8031D580: "data", 0x802F34D8: "rodata",
                0x802F3E20: "rodata", 0x802F3F10: "rodata", 0x802F3F30: "rodata", 0x802F3F54: "rodata",
                0x802F3258: "rodata", 0x802F3278: "rodata", 0x802F32FC: "rodata", 0x802F3320: "rodata", 0x802F33A4: "rodata",
                # dequantizer pool (0x802F34E8): pow43, exponent bases, pretab, multipliers, exponent offsets, band edges
                0x802F38E8: "rodata", 0x802F39E8: "rodata", 0x802F3A00: "rodata", 0x802F3A8C: "rodata",
                0x802F3AD4: "rodata", 0x802F3B00: "rodata", 0x802F3B2C: "rodata", 0x802F3B58: "rodata", 0x802F3B84: "rodata",
                0x802F3BB0: "rodata", 0x802F3BF8: "rodata", 0x802F3C40: "rodata", 0x802F3C88: "rodata", 0x802F3CD0: "rodata",
                0x802F3D18: "rodata"}  # objects found from code, not relocations

starts = {}
for l in open("config/REXE01/symbols.txt"):
    m = re.match(r"(\S+) = \.(rodata|data|sdata|sdata2):0x([0-9A-F]+);", l)
    if m:
        a = int(m.group(3), 16); sec = m.group(2)
        lo, hi = RANGES[sec]
        if lo <= a < hi:
            starts[a] = sec
starts.update(EXTRA_SPLITS)
objs = {}
for sec, (lo, hi) in RANGES.items():
    al = sorted(a for a, s in starts.items() if s == sec)
    for i, a in enumerate(al):
        objs[a] = (sec, (al[i + 1] if i + 1 < len(al) else hi) - a)

def name(a): return "lbl_%08X" % a

def words(a, n): return struct.unpack(">%dI" % (n // 4), rb(a, n - n % 4))

def is_ptr_obj(a):
    sec, n = objs[a]
    if sec in ("rodata", "sdata2") or n % 4: return False
    ws = words(a, n)
    return any(w in objs for w in ws) and all(w in objs or w == 0 for w in ws)

# Pointer depth: node arrays are u16; arrays of node pointers are u16*; arrays of those are tables.
def kind(a):
    if not is_ptr_obj(a): return "plain"
    tg = [w for w in words(a, objs[a][1]) if w]
    return "table" if any(is_ptr_obj(t) for t in tg) else "ptrs"

# Element types for non-pointer objects.
TYPES = {
    0x802F3408: ("u8", 1), 0x802F3418: ("s16", 2), 0x802F3478: ("s16", 2), 
    0x8031C2E0: ("u8", 1), 0x8031C298: ("s8", 1), 0x80559250: ("u8", 1), 0x80559258: ("u8", 1),
    0x80559260: ("u8", 1), 0x80559268: ("u8", 1), 0x80560DA0: ("s16", 2), 0x80560DA8: ("s16", 2),
    0x80560DB0: ("s16", 2), 0x802F3F54: ("u32", 4), 0x8031C2B8: ("u32", 4),
}
# Objects declared with a specific C shape: address -> (declaration with %s for the name, elem type, size)
SHAPES = {
    0x802F3418: ("MP3_STATIC const s16 %s[3][16]", "s16", 2),
    0x802F3478: ("MP3_STATIC const s16 %s[3][16]", "s16", 2),
    0x802F3D60: ("MP3_STATIC const s16 %s[2][3][16]", "s16", 2),  # long-block band boundaries
    0x802F3E20: ("MP3_STATIC const s16 %s[2][3][20]", "s16", 2),  # short-block band boundaries
    0x802F3F10: ("const s16 %s[16]", "s16", 2),
    0x802F32FC: ("const s32 %s[9]", "s32", 4),  # unreferenced so far, so not static (MWCC would drop it)
    0x802F33A4: ("const s32 %s[9]", "s32", 4),  # unreferenced so far, so not static  # 4-bit popcount; unreferenced, so not static (MWCC would drop it)
    0x802F3F30: ("MP3_STATIC const s16 %s[18]", "s16", 2),  # Huffman lookup depth per table
    0x802F38E8: ("MP3_STATIC const u8 %s[256]", "u8", 1),  # dequant: exponent base by global gain
    0x8031BFA8: ("MP3_STATIC s16 %s[12]", "s16", 2),  # intensity stereo short-band edges
    0x8031BFC0: ("MP3_STATIC u8 %s[16]", "u8", 1),  # intensity stereo short-band line limits
    0x8031BFD0: ("MP3_STATIC s16 %s[12]", "s16", 2),  # intensity stereo short-band edges
    0x8031BFE8: ("MP3_STATIC u8 %s[16]", "u8", 1),  # intensity stereo short-band line limits
    0x8031BFF8: ("MP3_STATIC s16 %s[12]", "s16", 2),  # intensity stereo short-band edges
    0x8031C010: ("MP3_STATIC u8 %s[16]", "u8", 1),  # intensity stereo short-band line limits
    0x8031C020: ("MP3_STATIC s16 %s[12]", "s16", 2),  # intensity stereo short-band edges
    0x8031C038: ("MP3_STATIC u8 %s[16]", "u8", 1),  # intensity stereo short-band line limits
    0x8031C048: ("MP3_STATIC s16 %s[12]", "s16", 2),  # intensity stereo short-band edges
    0x8031C060: ("MP3_STATIC u8 %s[16]", "u8", 1),  # intensity stereo short-band line limits
    0x8031C070: ("MP3_STATIC s16 %s[12]", "s16", 2),  # intensity stereo short-band edges
    0x8031C088: ("MP3_STATIC u8 %s[16]", "u8", 1),  # intensity stereo short-band line limits
    0x8031C098: ("MP3_STATIC s32 %s[32][4]", "s32", 4),  # dequant: large values, x^(4/3) per 256-wide segment
    0x802F39E8: ("MP3_STATIC const s8 %s[22]", "s8", 1),  # dequant: pretab
    0x802F44C8: ("MP3_STATIC const s8 %s[16]", "s8", 1),  # MPEG-1 slen1 by scalefac_compress
    0x802F44D8: ("MP3_STATIC const s8 %s[16]", "s8", 1),  # MPEG-1 slen2 by scalefac_compress
    0x802F3A00: ("MP3_STATIC const s16 %s[2][35]", "s16", 2),  # dequant: multiplier by scalefac_scale
    0x802F3A8C: ("MP3_STATIC const u8 %s[2][35]", "u8", 1),  # dequant: exponent offset by scalefac_scale
    0x802F3AD4: ("MP3_STATIC const s16 %s[21]", "s16", 2),  # long-block band edges
    0x802F3B00: ("MP3_STATIC const s16 %s[21]", "s16", 2),  # long-block band edges
    0x802F3B2C: ("MP3_STATIC const s16 %s[21]", "s16", 2),  # long-block band edges
    0x802F3B58: ("MP3_STATIC const s16 %s[21]", "s16", 2),  # long-block band edges
    0x802F3B84: ("MP3_STATIC const s16 %s[21]", "s16", 2),  # long-block band edges
    0x802F3BB0: ("MP3_STATIC const s16 %s[36]", "s16", 2),  # short-block band edges
    0x802F3BF8: ("MP3_STATIC const s16 %s[36]", "s16", 2),  # short-block band edges
    0x802F3C40: ("MP3_STATIC const s16 %s[36]", "s16", 2),  # short-block band edges
    0x802F3C88: ("MP3_STATIC const s16 %s[36]", "s16", 2),  # short-block band edges
    0x802F3CD0: ("MP3_STATIC const s16 %s[36]", "s16", 2),  # short-block band edges
    0x802F3D18: ("MP3_STATIC const s16 %s[36]", "s16", 2),  # short-block band edges
}
NODE_LO, NODE_HI = 0x8031C320, 0x8031D478

def elem(a):
    if a in TYPES: return TYPES[a]
    sec, n = objs[a]
    if (NODE_LO <= a < NODE_HI or (sec == "sdata" and a >= 0x80559278)) and n % 2 == 0: return ("u16", 2)
    if n % 4 == 0: return ("s32", 4)
    if n % 2 == 0: return ("s16", 2)
    return ("u8", 1)

def fmt_vals(a, n, t, sz):
    raw = rb(a, n)
    if sz == 1: vals = list(raw)
    elif sz == 2: vals = list(struct.unpack(">%dH" % (n // 2), raw))
    else: vals = list(struct.unpack(">%dI" % (n // 4), raw))
    out = []
    for v in vals:
        if t.startswith("s"):
            bits = sz * 8
            if v >= 1 << (bits - 1): v -= 1 << bits
            out.append(str(v) if v > -1000 and v < 1000 else ("-0x%X" % -v if v < 0 else "0x%X" % v))
        else:
            out.append("0x%X" % v)
    return out

def emit(a):
    sec, n = objs[a]
    const = "const " if sec in ("rodata", "sdata2") else ""
    k = kind(a)
    if a == 0x8031D580:
        ws = words(a, n)
        return "MP3_STATIC HuffTable %s[%d] = {%s};\n" % (name(a), len(ws), ", ".join("{%s}" % (name(w) if w else "NULL") for w in ws))
    if a in SHAPES:
        decl, t, sz = SHAPES[a]
        dims = [int(x) for x in re.findall(r"\[(\d+)\]", decl)]
        cnt = 1
        for x in dims: cnt *= x
        vals = fmt_vals(a, min(n, cnt * sz), t, sz)
        lines = ["    " + ", ".join(vals[i:i + 12]) + "," for i in range(0, len(vals), 12)]
        return (decl % name(a)) + " = {\n%s\n};\n" % "\n".join(lines)
    if k != "plain":
        ws = words(a, n)
        ty = "u16**" if k == "table" else "u16*"
        vals = [name(w) if w else "NULL" for w in ws]
        return "MP3_STATIC %s %s[%d] = {%s};\n" % (ty, name(a), len(ws), ", ".join(vals))
    t, sz = elem(a)
    vals = fmt_vals(a, n, t, sz)
    lines = []
    for i in range(0, len(vals), 12):
        lines.append("    " + ", ".join(vals[i:i + 12]) + ",")
    return "MP3_STATIC %s%s %s[%d] = {\n%s\n};\n" % (const, t, name(a), len(vals), "\n".join(lines))

# The decoder is three translation units (MWCC addresses pooled tables from the start of each unit's
# .rodata, which puts unit boundaries at 0x802F34E8 and 0x802F3D60). Each unit gets its own table file.
UNITS = [
    ("src/sdk_mp3/mp3frame_tables.inc", {"rodata": (0x802F3238, 0x802F34E8), "data": (0x8031BFA8, 0x8031C098),
                                         "sdata": (0x80559250, 0x80559270), "sdata2": (0x80560DA0, 0x80560DB0)}),
    ("src/sdk_mp3/mp3dequant_tables.inc", {"rodata": (0x802F34E8, 0x802F3D60), "data": (0x8031C098, 0x8031C298),
                                           "sdata2": (0x80560DB0, 0x80560DB8)}),
    ("src/sdk_mp3/mp3layer3_tables.inc", {"rodata": (0x802F3D60, 0x802F44E8), "data": (0x8031C298, 0x8031D840),
                                          "sdata": (0x80559270, 0x80559708)}),
]
def deps(a):
    if kind(a) == "plain": return []
    return [w for w in words(a, objs[a][1]) if w]
for path, rng in UNITS:
    def mine(a, secs):
        sec = objs[a][0]
        return sec in secs and sec in rng and rng[sec][0] <= a < rng[sec][1]
    out = ["/* Generated by tools/mp3/gentables.py from REXE01 main.dol. */\n"]
    for a in sorted(a for a in objs if mine(a, ("rodata", "sdata2"))):
        out.append(emit(a))
    # .data and .sdata interleaved so pointer targets are defined first, each section kept in address order.
    data = sorted(a for a in objs if mine(a, ("data",))); sd = sorted(a for a in objs if mine(a, ("sdata",)))
    done = set(); i = j = 0
    while i < len(data) or j < len(sd):
        if i < len(data) and all(x in done for x in deps(data[i])):
            out.append(emit(data[i])); done.add(data[i]); i += 1
        elif j < len(sd) and all(x in done for x in deps(sd[j])):
            out.append(emit(sd[j])); done.add(sd[j]); j += 1
        else:
            raise SystemExit("cannot order at %s / %s" % (hex(data[i]) if i < len(data) else "-", hex(sd[j]) if j < len(sd) else "-"))
    open(path, "w", newline="\n").write("".join(out))
    print(path, "objects:", len(out) - 1)
print("objects:", len(objs), " pointer tables:", sum(1 for a in objs if kind(a) != "plain"))
